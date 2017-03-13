#include "AF9APPLY.H"
#include "AFCINCLS.H"
#include "AFCPRODS.HPP"
#include "AFCBKEND.HPP"

#define FILE_EOF    987654
#define READLINE(FSTREAM, ISTRING, ERROR)                       \
   ISTRING = IString::lineFrom( FSTREAM );                      \
   if ( FSTREAM.eof() )                                         \
      ERROR = FILE_EOF;                                         \
   else if ( FSTREAM.fail() )                                   \
      ERROR = IDS_ErrorFileRead_Text;                           \
   else                                                         \
      ERROR = NO_ERROR;

//======================================================================
// Function: BackEnd::ConvertPreviousCFREPORT()
//    Processes CFREPORTs (V31) which has been processed by previous version
//    of this application.  Changes the format of control records (AA, AB, AC, ..)
//    to the new format.  This function is called by BackEnd::LoadCFREPORT().
//
// Overall logic:
//    1. Open input file to be converted and an output file with a temporary name
//    2. Read each line in the input file
//          if record type is 00, 03, 07, 01, 96, or 98, copy it
// D95887 add 53 record type as a valid type
// D95997 add 06 record type as a valid type
//======================================================================

int BackEnd::ConvertPreviousCFREPORT( CfReportFileStats& myCfrStats )
{
   // Open the file to be converted and a temporary output file
   ifstream       InputOldCfr( myCfrStats.sCfrFullFileName );
   if ( InputOldCfr.fail() )
      return IDS_ErrorInputFileOpen_Text;
   IString        sOutputFileName = GetTempFileName( myCfrStats.sCfrFullFileName );
   ofstream       OutputNewCfr( sOutputFileName );
   if ( OutputNewCfr.fail() )
      return IDS_ErrorOutputFileOpen_Text;

   // Few local variables
   int            i, iError = NO_ERROR;
   IString        sInputRec, sOutputRec;
   IString        sValidCfrRecs  = IString("00 01 03 06 07 53 08 25 19 47 49 50 95 96 98");        // 95887  include 53 - 95997 include 06
   IString        sValidCtrlRecs = IString(STR_CtlRecTypes);
   Product        *plastProduct = NULL;   // Stores the pointer last Product (not Mes) so that it can be
                                          // used to write the Product::sCurrentTypeModel and
                                          // Mes::uOffsetBaseProductRec in the AB records
   enum { PRODUCT, MES } eLastProductBlock;   //Stores the type of last block processed

   while (1)
   {
      READLINE( InputOldCfr, sInputRec, iError )
      if ( iError == IDS_ErrorFileRead_Text )
         return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, iError, myCfrStats.sCfrFullFileName, sOutputFileName );
      else if ( iError == FILE_EOF )
         return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, NO_ERROR, myCfrStats.sCfrFullFileName, sOutputFileName );

      // Determine if the record type is valid
      IString     sRecType = sInputRec.subString( 1,2 );
      if ( !sValidCfrRecs.indexOf(sRecType) && !sValidCtrlRecs.indexOf(sRecType) )
         return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, IDS_ErrorUnknownCfrRec_Text, myCfrStats.sCfrFullFileName, sOutputFileName );

      // If AA record, create a new AA record and write it
      if ( sRecType == IString(STR_AaRecType) )
      {
         myCfrStats.ParseAaRecord( sInputRec );
         myCfrStats.CreateAaRecord( sOutputRec );
         if ( WRITELINE(OutputNewCfr, sOutputRec) != NO_ERROR )
            return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, IDS_ErrorFileWrite_Text, myCfrStats.sCfrFullFileName, sOutputFileName );
         continue;
      }

      IBoolean    bUpdateAbRecord = False;

      // Copy records 00, 03, 07, 06, 53, 01, 96, and 98 records from input to output file
      if( sRecType == IString(STR_00RecType) || sRecType == IString(STR_03RecType) || sRecType == IString(STR_07RecType) || sRecType == IString(STR_06RecType) ||sRecType == IString(STR_53RecType)  //95887 -95997
           || sRecType == IString(STR_01RecType) || sRecType == IString(STR_96RecType) || sRecType == IString(STR_98RecType))
      {
         if( WRITELINE(OutputNewCfr, sInputRec) != NO_ERROR )
            return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, IDS_ErrorFileWrite_Text, myCfrStats.sCfrFullFileName, sOutputFileName );

         // if this is last record (98) and last block was Mes, need to update the base product AB record
         if ( sRecType == IString(STR_98RecType) && eLastProductBlock == MES )
         {
            plastProduct->UpdateDetails();
            IString         sTempAbRecord;
            plastProduct->CreateAbRecord( sTempAbRecord );
            // Save the current pointer in output file
            streampos outputPos = OutputNewCfr.tellp();
            // Seek the lastProduct's AB record and overwrite it
            OutputNewCfr.seekp( plastProduct->uOffsetToProductRecord - plastProduct->uLengthAbRec );
            if ( OutputNewCfr.fail() )
               return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, IDS_ErrorOutputFileSeek_Text, myCfrStats.sCfrFullFileName, sOutputFileName );
            if ( WRITELINE(OutputNewCfr, sTempAbRecord) != NO_ERROR )
               return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, IDS_ErrorFileWrite_Text, myCfrStats.sCfrFullFileName, sOutputFileName );
            // Restore the pointer
            OutputNewCfr.seekp ( outputPos );
            if ( OutputNewCfr.fail() )
               return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, IDS_ErrorOutputFileSeek_Text, myCfrStats.sCfrFullFileName, sOutputFileName );
         }
      }

      // If this is an AB, process Product block as follows:
      //    1. Read Product and ProductID records
      //    2. Create an instance of Product
      //    3. Convert old AB record to new format
      //    4. write AB, Product, and ProductID records
      if ( sRecType == IString(STR_AbRecType) )
      {
         // Read Product and ProductID records into an array
         // Array layout:
         //    [0] = Will store the new AB record to be written to the output file
         //    [1] = Product Record
         //    [2] = Product ID Record
         //    [3] = 2nd ProductID record if this is an Upgrade
         IString    sProductRecs[4];

         for( i=1; i<=2; i++ )
         {
            READLINE( InputOldCfr, sProductRecs[i], iError )
            if ( iError == IDS_ErrorFileRead_Text )
               return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, iError, myCfrStats.sCfrFullFileName, sOutputFileName );
            else if ( iError == FILE_EOF )
               return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, IDS_ErrorInvalidCfreport_Text, myCfrStats.sCfrFullFileName, sOutputFileName );
            sRecType = sProductRecs[i].subString(1,2);
            if ( !sValidCfrRecs.indexOf(sRecType) )
               return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, IDS_ErrorUnknownCfrRec_Text, myCfrStats.sCfrFullFileName, sOutputFileName );
         }

         unsigned       uCountRecToWrite = 3;  // No. of records to write for this Product/Mes including AB record
                                               // If this is an Upgrade, it will be set it to 4
         // Extract InitialStatus from the AB record
         IString       sInitStatus;
         sInputRec >> 29 >> sInitStatus;
         sInitStatus.strip();

         if ( sInitStatus == IString("MES") )
         {
            eLastProductBlock = MES;
            // Check if the MES is an Upgrade. (it will be an upgrade if the next record is 95)
            if ( sProductRecs[1].subString(1,2) == IString(STR_19RecType) )
            {
               streampos inputPos = InputOldCfr.tellg();
               READLINE( InputOldCfr, sProductRecs[3], iError )
               if ( iError == IDS_ErrorFileRead_Text )
                  return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, IDS_ErrorFileRead_Text, myCfrStats.sCfrFullFileName, sOutputFileName );
               else if ( iError == FILE_EOF )
                       return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, IDS_ErrorInvalidCfreport_Text, myCfrStats.sCfrFullFileName, sOutputFileName );
               if ( sProductRecs[3].subString(1,2) == IString(STR_95RecType) )
                  uCountRecToWrite = 4;
               else
                  InputOldCfr.seekg( inputPos );
            }

            // Convert AB record and parse Product records
            Mes  *pMes = new Mes;
            AbstractProduct::ConvertAbRecord( sInputRec, sProductRecs[0] );
            pMes->ParseAbRecord( sProductRecs[0] );
            if ( uCountRecToWrite == 3 )
               pMes->ParseLoad( 'C', sProductRecs[1], sProductRecs[2], OutputNewCfr.tellp()+pMes->uLengthAbRec, 0, myCpuList );
            else
               pMes->ParseLoad( 'C', sProductRecs[1], sProductRecs[3], OutputNewCfr.tellp()+pMes->uLengthAbRec, 0, myCpuList );

            // if the Mes matches the last product in type, model, and orderserialnumber,
            // associate this Mes to the base product
            if ( plastProduct->DoesMesMatch(*pMes) )
            {
               pMes->uOffsetBaseProductRec = plastProduct->uOffsetToProductRecord;
               plastProduct->AssociateMes( pMes );
               pMes->CreateAbRecord( sProductRecs[0] );
            }
            else
               return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, IDS_ErrorMesNotMatchProd_Text, myCfrStats.sCfrFullFileName, sOutputFileName );
         } // end .. if MES

         if ( sInitStatus == IString("INSTALLED") || sInitStatus == IString("INITIAL") )
         {
            // If the last block was Mes, rewrite the AB record for the lastProduct.
            if ( eLastProductBlock == MES )
            {
              plastProduct->UpdateDetails();
              IString         sTempAbRecord;
              plastProduct->CreateAbRecord( sTempAbRecord );
              // Save the current pointer in output file
              streampos outputPos = OutputNewCfr.tellp();
              // Seek the lastProduct's AB record and overwrite it
              OutputNewCfr.seekp( plastProduct->uOffsetToProductRecord - plastProduct->uLengthAbRec );
              if ( OutputNewCfr.fail() )
                 return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, IDS_ErrorOutputFileSeek_Text, myCfrStats.sCfrFullFileName, sOutputFileName );
              if ( WRITELINE(OutputNewCfr, sTempAbRecord) != NO_ERROR )
                 return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, IDS_ErrorFileWrite_Text, myCfrStats.sCfrFullFileName, sOutputFileName );
              // Restore the pointer
              OutputNewCfr.seekp ( outputPos );
              if ( OutputNewCfr.fail() )
                 return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, IDS_ErrorOutputFileSeek_Text, myCfrStats.sCfrFullFileName, sOutputFileName );
            }

            eLastProductBlock = PRODUCT;
            // Create AB record
            Product  *pthisProduct = new Product;
            Product::ConvertAbRecord( sInputRec, sProductRecs[0] );
            pthisProduct->ParseAbRecord( sProductRecs[0] );
            pthisProduct->ParseLoad( 'C', sProductRecs[1], sProductRecs[2],
                                   OutputNewCfr.tellp()+pthisProduct->uLengthAbRec, 0, myCpuList );
            pthisProduct->CreateAbRecord( sProductRecs[0] );

            // Save the pointer to this Product
            if (plastProduct != NULL )  // When processing the 1st product, this pointer will be NULL
            {
               plastProduct->DestroyAllMes();
               delete plastProduct;
            }
            plastProduct = pthisProduct;
         } // end .. if INSTALLED or INITIAL

         // Write AB, Product, and ProductID records
         for( i=0; i<uCountRecToWrite; i++ )
         {
             if( WRITELINE(OutputNewCfr, sProductRecs[i]) != NO_ERROR )
                return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, IDS_ErrorFileWrite_Text, myCfrStats.sCfrFullFileName, sOutputFileName );
         }
      } // end .. if AB

      // If this is AC record
      //    (1)  Read each AC record and assign it a system ID (ist AC record = 0, 2nd Ac record = 1, ...)
      //    (2)  Convert to the new format
      //    (3)  Write the new AC record
      if ( sRecType == IString(STR_AcRecType) )
      {
         unsigned int  uSystemId = 0;
         while(1)
         {
             // Create and write the AC record
             IString       sNewAcRecord = System::ConvertAcRecord( sInputRec, uSystemId );
             if( WRITELINE(OutputNewCfr, sNewAcRecord) != NO_ERROR )
                  return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, IDS_ErrorFileWrite_Text, myCfrStats.sCfrFullFileName, sOutputFileName );

             // Read the next AC record
             READLINE( InputOldCfr, sInputRec, iError )
             if( iError == IDS_ErrorFileRead_Text )
                return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, iError, myCfrStats.sCfrFullFileName, sOutputFileName );
             else if( iError == FILE_EOF )
                return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, NO_ERROR, myCfrStats.sCfrFullFileName, sOutputFileName );

             if( sInputRec.subString(1,2) != IString(STR_AcRecType) )
                return Close_Cfrs_And_Return( InputOldCfr, OutputNewCfr, IDS_ErrorUnexpectedRecFnd_Text, myCfrStats.sCfrFullFileName, sOutputFileName );

             // Increment SystemID
             uSystemId++;
         }
      } // end .. if AC record

  } // end .. while(1)
  plastProduct->DestroyAllMes();
  delete plastProduct;

} // end .. ConvertPreviousCFREPORT()

#undef READLINE
#undef FILE_EOF

#ifdef GARGJ_TEST

main(int argc, char *argv[], char *envp[])
{
   IString sFileName = argv[1];
   CfReportFileStats  myCfr( sFileName );
   myCfr.sCfrFullFileName = sFileName;
   BackEnd BE(NULL);
   BE.LoadCpuList();
   int iError = BE.ConvertPreviousCFREPORT(myCfr);
   cout << iError << endl;
}

#endif
