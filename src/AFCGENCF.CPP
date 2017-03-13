#include "AFCBKEND.HPP"
#include "AFCCFRST.HPP"
#include "AFCFEAT.HPP"
#include "AFCPRODS.HPP"

#include "AFCMCROS.H"

#include "AF9MAIN.HPP"
#include "AF9ORDRS.HPP"
#include "AF9SYSTM.HPP"
#include "AF9APPLY.H"


#include <ICNRCTL.HPP>

#define READBUF_SIZE1 10240   //  ZOR 09032004 93732

/////////////////////////////////////////R1570
char *mrd1[4000];int MRDCount1=0;
char *mrd2[4000];int MRDCount2=0;
char *MRD[4000];int MRDCount=0;

char **Multi[1000];int MSystems =0;int CurrentSystem =0;
int MultiCount[1000];



char *ret[4000];int RETCount=0;
//139136 Begin
Boolean bS390=false;
IString strArg="";
IString MRDname;
//139136 End

Boolean ReadMRD(char *FileName,char *FileNameRET);
Boolean CreateRETArray();
Boolean AppendRETRecords(char *FileName);
Boolean UpdateRemote(char *FileName);
///////////////////////////////////////erick
int GenSystem( ostream&, IString&, IString );  // 94021
int SeekWrite( istream&, ostream&, IString, unsigned long );
//IString get53record ( IString strMRD ); // 94021      commented because of 95887
////////////////////////////////////////////////////////////////////////////////
// Function: BackEnd::GenerateCFREPORT()
//    Creates a new CFREPORT from the current objects in memory.  Only systems
//    that have products assigned are included.  New orders (record types 08
//    and 47) are changed to installed (record types 25 and 49) in the output
//    file.
//
// Overall logic:
//    1. Re-seek the input file to the beginning.
//    2. Obtain the output file's name and open it.
//    3. Read, update, and write the CFREPORT file header (00) record.
//    4. Read and copy the security (03) record to the output file.
//    5. Loop through all of the assigned (system 1 thru n, ignoring system 0)
//       system containers
//       a. For each system that has at least one product assigned
//          (ignore those systems with no assigned products):
//          1) Create a System (07) and a CFREPORT Section (01) record with
//             the text from the notebook tab in the description field and
//             write them to the output file.
//          2) For  each product in the system container:
//             a) If the product has an applied MES:
//                1> Create a new product record and write it to the output
//                   file.
//                2> Seek to the associated product Id record (95) in the input
//                   file, then read, update, and write it to the output file.
//                3> Loop through the updated product's feature set.  For each
//                   feature, seek to its record in the input file, then read
//                   and copy it to the output file.
//             b) If the product does not have an applied MES:
//                1> Seek to the product record in the input file, read it,
//                   change its record type to installed if it is an initial
//                   order product, and write it to the output file.
//                2> Seek to the associated product Id record (95) in the input
//                   file and copy it to the output file.
//                3> If  the product has associated features, continue copying
//                   all of its feature records to the output file.
//    6. If no product records were written to the output file (no systems had
//       assigned products), close and delete the output file, returning an
//       error code.
//    7. Otherwise, create and write the CFREPORT trailer record (98) to the
//       output file, then close the file and return.
//
//******************************************************************************
// Revision Key:
//   VDT --- Von D. Tucker           (VNRTC370 at RHQVM22)
//   EDW --- Erica D. Wilson         (EDWILSON@US.IBM.COM)
//
// Revision Notes:
//   Rev #   Flag   Who     Date                    Description
//   1.00           VDT   09/30/96   Initial creation
//           edw1   EDW   05/05/98   Added logic to process 54, 55, and 56 record types
//           edw2   EDW   01/11/2000 Added logic to write comment records that detail
//                                   Base Retrieval's reconciliation results
//           93732  ZOR   09/03/2004 added logic to check 05 records after the .RET file
//			             is created, if they are grather that 76 chars then cut it.
//           94021 ZOR    10/20/2004  add code in GenSystem() to add the 53 record under 07 record,
//                                    also a new method to get53record from mrd file  commented because of 95887
//           D95887 ZOR   10/25/2004  inlcude 53 record in the RET file right after the 07 record
//           95670 HECC   10/29/2004 Writes new logic to write RT 05 correctly:if is longer
//                                   than 76 chars, cut it into two 05 records, otherwise,
//                                   write it as is, in the RET file, and count correctly
//                                   the written records.
//           96389 HECC   11/18/2004 Fixes the correctly copy of the records quantity
//                                   when it is 6 characters long.
//           D95997 ZOR   19/11/2004 Include non INV 06 records in the RET file
//           96549 HECC   11/29/2004 Disable fix for defect 96389
//           D106441 RERC 02/21/2006 Code modified  to write RT05 putting the text of comments
//                                   in two lines beginning in the same column.
//
//           Req1578 ELRR 24/09/2010 Adding new READMRD and CreatRETArray functions to support
//           F136652                 the new Z series machine for Base Retrieval.
//
//           D136691 ELRR 28/09/2010 Adding a Filter to Impact only Zseries with the new Req 1578 code.
//
//           D139136 ELRR 01/07/2011 An association was established between MRD files and the BIR files for Zseries with Multiple Systems
//                                   in order to create correct RET files when base Retrieval is called from econfig
//
//
//           D147293          ELRR  29/04/2013 Added logic to copy the 05 record types in the .ret file
//                            the Printing Type on the 05 record ( position 12) could be come in as 0 ( zero) but needs to be '1' for e-config to let them flow through.
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int BackEnd::GenerateCFREPORT()
{

   IBoolean bHdrProc    = False;          /* Set when all headers processed*/
   IString  sOrderTypes = STR_OrderRecTypes;
   IString         sInputCfrRec, sOutputFileName, sProductIdRec, sProductRec,
                   sRecCnt, sRecType, sTrailerRecord, s06Rec,MRDInutputFileName;      // 95997 add s06Rec var
   long int        iError, iFeatRecs, iRecCnt = 3, iSysCnt;
   unsigned long   uProdCnt, uRecOffset, uSystemId;
   ISortedSet<IString> rec54InstSet;                                 /*edw1*/
   IBoolean        bWriteInfoRecs = True;                            /*edw2*/
   IBoolean        bfeatread = True; // ZOR 95997 flag to know if there is no 06 records

//   Re-seek the input file to the beginning and open the output file
   fstream& sInputCfr = pMainWindow->pFile->CfrFileStream;
   sInputCfr.seekg( 0, ios::beg );
   if ( sInputCfr.fail() )
   {
     sInputCfr.close();
     int iError = OpenFile( pMainWindow->pFile->sCfrFullFileName, BackEnd::READ, sInputCfr );
     if ( iError )
       return iError;
   }
   sOutputFileName = pMainWindow->strGeneratePath + STR_PathDelimiter
                   + pMainWindow->strGenerateName + STR_ExtensionRET;

   ofstream sOutputCfr( sOutputFileName );
   if ( sOutputCfr.fail() )
     return IDS_ErrorOutputFileOpen_Text;

   while ( ! bHdrProc )
   {                                /* Copy the header and security records*/
     sInputCfrRec = IString::lineFrom( sInputCfr );
     if ( sInputCfr.fail() )
       if ( sInputCfr.eof() )
         CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorUnexpectedEof_Text )
       else
         CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileRead_Text )
     sRecType = sInputCfrRec.subString(1,2);
     if ( sRecType == STR_AaRecType )               /* Ignore the AA record*/
       continue;
     else
       if ( sRecType == STR_00RecType )
       {                               /* Update and copy the header record*/
         pMainWindow->pFile->UpdateHeader( sInputCfrRec );
         if ( WRITELINE(sOutputCfr, sInputCfrRec) )
           CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileWrite_Text )
       }                                  /* End copying  the header record*/
       else
         if ( sRecType == STR_03RecType )
         {                                      /* Copy the security record*/
           bHdrProc = True;
           if ( WRITELINE(sOutputCfr, sInputCfrRec) )
             CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileWrite_Text )
         }                               /* End copying the security record*/
         else
           CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorInvalidCfreport_Text )
   }                         /* End copying the header and security records*/


//  Second, loop through all of the assigned system objects and write out all
//  of the products assigned to each.
   IContainerControl **pcnrSystems = pMainWindow->pSystems->pcnrProducts;
   for ( USHORT usSysId = 1;
         usSysId < pMainWindow->pSystems->usSystems;
         usSysId ++ )
   {                     /* Loop through all of the assigned system objects*/
     if ( pcnrSystems[usSysId]->objectCount() == 0 )
       continue;                  /* Ignore those with no products assigned*/
   //if ( GenSystem(sOutputCfr, pMainWindow->pSystems->systemName(usSysId),pMainWindow->strMRD) )    // 94021
   //    CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileWrite_Text )
   //  iRecCnt = iRecCnt + 2;         /* Count the system and section records*/

    //* 95887
     sInputCfr.seekg( uOffsetTo53Recs );  // seek the 53 record position
     IString sRecord53 = IString::lineFrom( sInputCfr );     // read the 53 record

     //* 95887
     if ( GenSystem(sOutputCfr, pMainWindow->pSystems->systemName(usSysId), sRecord53) )
       CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileWrite_Text )

     //* 95887
     iRecCnt = iRecCnt + 3;         /* Count the system and section records*/

     /*edw2-------------------write out informational 05 records, if exists*/
     IBoolean b05 = True;
     IString sRecord05,modifier,sRecord05mod;//147293
     sInputCfr.seekg( uOffsetToInfoRecs );
     if ( sInputCfr.fail() )
       return IDS_ErrorInputFileSeek_Text;
     while ( b05 && bWriteInfoRecs )
     {
        sRecord05 = IString::lineFrom( sInputCfr );
        if ( sInputCfr.fail() )
           if ( sInputCfr.eof() )
              CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorUnexpectedEof_Text )
           else
              CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileRead_Text )
        if ( sRecord05.subString(1, 2) == STR_05RecType )
        {
           //* D95670:  Begin logic:  Calls to the function that writes correclty all RT 05
           //*                        using a unique function
           //* if ( WRITELINE(sOutputCfr, sRecord05) )
           //*    CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileWrite_Text )
           //* iRecCnt++;



          //start 147293 ELRR we set the value of the record 05 pos 12 to 1.
          modifier='1';
          sRecord05mod= sRecord05.subString(13,90);
          sRecord05=sRecord05.subString(1,11)+modifier+sRecord05mod;
          //End   147293 ELRR



           Write05Recs(sOutputCfr, sRecord05, iRecCnt);
          //* D95670:  End logic:  HECC - 10/29/2004
           continue;
        }
        else
           b05 = False;
     }
     bWriteInfoRecs = False;

     if ( !rec54InstSet.isEmpty() )                                  /*edw1*/
        rec54InstSet.removeAll();                                    /*edw1*/

     IContainerControl *pcnrProducts = pcnrSystems[usSysId];
     IContainerControl::ObjectCursor pcrs(*pcnrProducts);
     forCursor ( pcrs )
     {                           /* Loop through all of the product objects*/
       Product* pProduct = (Product*)pcrs.current();
       if ( pProduct->HasAppliedMes() )         /* If it has applied MES(s)*/
       {                /* Create new product record based on applied MES's*/
         sProductRec = pProduct->CreateProductRecord();
         if ( WRITELINE(sOutputCfr, sProductRec) )
           CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileWrite_Text )
         iRecCnt ++;                            /* Count the product record*/
         sInputCfr.seekg( pProduct->uOffsetToProductIdRecord );
         if ( sInputCfr.fail() )
           return IDS_ErrorInputFileSeek_Text;
         sProductIdRec = IString::lineFrom( sInputCfr );
         if ( sInputCfr.fail() )
           if ( sInputCfr.eof() )
             CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorUnexpectedEof_Text )
           else
             CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileRead_Text )

         //  Next, create the updated product Id record
         pProduct->UpdateProdIdRec( sProductIdRec );
         if ( WRITELINE(sOutputCfr, sProductIdRec) )
           CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileWrite_Text )
         iRecCnt ++;                            /* Count the 95 type record*/

         //  Last, copy all of the feature records
         FeatureCollection& FeatureCnr = pProduct->FeatureSet();
         FeatureCollection::Cursor FeatCursor(FeatureCnr);
         forCursor( FeatCursor )
         {                   /* Loop through all of this product's features*/
           Feature& wkFeature = FeatureCnr.elementAt( FeatCursor );
           iError = SeekWrite(sInputCfr, sOutputCfr, STR_96RecType,
                              wkFeature.uOffset );
           if ( iError )
             CLOSE_OUTPUT_CFR( sOutputCfr, iError )
           iRecCnt++;                          /* Count each feature record*/
         }                   /* End looping through this product's features*/

         // edw1-Very last, copy all the 54 records for the product block
         sInputCfr.seekg( pProduct->ulRecord54Offset );
         if ( sInputCfr.fail() )
           return IDS_ErrorInputFileSeek_Text;
         IBoolean b54 = True;
         IString sRecord54, sRec54Type, s54InstId;
         while( b54 )
            {
            sRecord54 = IString::lineFrom( sInputCfr );

            if ( sInputCfr.fail() )
              if ( sInputCfr.eof() )
                CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorUnexpectedEof_Text )
              else
                CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileRead_Text )
            sRec54Type = sRecord54.subString(1, 2);
            if( sRec54Type == STR_54RecType )
               {
               if( WRITELINE(sOutputCfr, sRecord54) )
                  CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileWrite_Text )
               s54InstId = sRecord54.subString( 3,6 );
               try
                  {
                  rec54InstSet.add( s54InstId );
                  }
               catch ( IOutOfMemory )
                  {
                  CLOSE_INPUT_CFR( sInputCfr, IDS_ErrorOutOfMem_Text )
                  }
               iRecCnt++;                       /* Count the 54 type record*/
               }
            else
               b54 = False;
            }
       }
       else                         /* Else it does not have applied MES(s)*/
       {                     /* Copy all of its records from the input file*/
         //  First, copy the product record
         sRecType = pProduct->sOriginalRecordType;
         if ( sOrderTypes.indexOf(sRecType) )           /* If initial order*/
           if ( sRecType == STR_08RecType )                  /* If hardware*/
             sRecType = STR_25RecType;          /* Id as installed hardware*/
           else
             sRecType = STR_49RecType;          /* Id as installed software*/
         iError = SeekWrite(sInputCfr, sOutputCfr, sRecType,
                            pProduct->uOffsetToProductRecord );
         if ( iError )
           CLOSE_OUTPUT_CFR( sOutputCfr, iError )
         iRecCnt ++;                            /* Count the product record*/

         //  Next, copy and update the product Id record
         sInputCfr.seekg( pProduct->uOffsetToProductIdRecord );
         if ( sInputCfr.fail() )
           return IDS_ErrorInputFileSeek_Text;
         sProductIdRec = IString::lineFrom( sInputCfr );
         if ( sInputCfr.fail() )
           if ( sInputCfr.eof() )
             CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorUnexpectedEof_Text )
           else
             CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileRead_Text )
         pProduct->UpdateProdIdRec( sProductIdRec );
         if ( WRITELINE(sOutputCfr, sProductIdRec) )
           CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileWrite_Text )
         iRecCnt ++;                            /* Count the 95 type record*/
         // ZOR 95997  start
         s06Rec = IString::lineFrom( sInputCfr );  //  read next line after 95 record
         if ( sInputCfr.fail() )
           if ( sInputCfr.eof() )
             CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorUnexpectedEof_Text )
           else
             CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileRead_Text )
          if (s06Rec.subString(1,2)==STR_06RecType)   // if the next record is 06 then include it
            {
             if ( WRITELINE(sOutputCfr, s06Rec) )
               CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileWrite_Text )
             iRecCnt ++;
            }
          else
            {
             bfeatread = False;                         // if the next record is not an 06 then bfeatread=False
             sInputCfrRec = s06Rec ;                    // and copy s06Rec to sInputCfrRec for the next loop
            }
         // ZOR 95997 end
         //  Last, copy all of the feature records
         iFeatRecs = pProduct->uCountFeatures;
         for ( int x = 1; x <= iFeatRecs; x++ )
         {           /* Copy all of its feature records from the input file*/
           if (bfeatread)           //95997   if last record was an 06 record
              sInputCfrRec = IString::lineFrom( sInputCfr );
           if ( sInputCfr.fail() )
             if ( sInputCfr.eof() )
               CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorUnexpectedEof_Text )
             else
               CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileRead_Text )

           if ( WRITELINE(sOutputCfr, sInputCfrRec) )
             CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileWrite_Text )
           bfeatread = True;        //95997  set bfeatread true to read the next lines ( features )
         }    /* End copying all of its feature records from the input file*/
         iRecCnt = iRecCnt + iFeatRecs;        /* Count the feature records*/

         // edw1-Very last, copy all the 54 records for the product block
         sInputCfr.seekg( pProduct->ulRecord54Offset );
         if ( sInputCfr.fail() )
           return IDS_ErrorInputFileSeek_Text;
         IBoolean b54 = True;
         IString sRecord54, sRec54Type, s54InstId;
         while( b54 )
            {
            sRecord54 = IString::lineFrom( sInputCfr );
            if ( sInputCfr.fail() )
              if ( sInputCfr.eof() )
                CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorUnexpectedEof_Text )
              else
                CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileRead_Text )
            sRec54Type = sRecord54.subString(1, 2);
            if( sRec54Type == STR_54RecType )
               {
               if( WRITELINE(sOutputCfr, sRecord54) )
                  CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileWrite_Text )
               s54InstId = sRecord54.subString( 3,6 );
               try
                  {
                  rec54InstSet.add( s54InstId );
                  }
               catch ( IOutOfMemory )
                  {
                  CLOSE_INPUT_CFR( sInputCfr, IDS_ErrorOutOfMem_Text )
                  }
               iRecCnt++;                       /* Count the 54 type record*/
               }
            else
               b54 = False;
            }
       }              /* End copying all of its records from the input file*/
     }                    /* End looping through the of the product objects*/

   //edw1-Write out 55 and 56 records for system block
   ISortedSet<IString>::Cursor rec54crs( rec54InstSet );
   ISortedSet<IString>::Cursor rec55crs( placementRecSet );
   ISortedSet<IString>::Cursor rec56crs( connectionRecSet );
   IString s54Inst, s55Inst, s56Inst, s55rec, s56rec;
   forCursor( rec54crs )                     /*edw1-Write placement records*/
      {
      s54Inst = rec54InstSet.elementAt( rec54crs );
      forCursor( rec55crs )
         {
         s55rec = placementRecSet.elementAt( rec55crs );
         s55Inst=s55rec.subString( 3,6 );
         if ( s54Inst == s55Inst )
            {
            if( WRITELINE(sOutputCfr, s55rec) )
                  CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileWrite_Text )
            iRecCnt++;
            break;
            }
         }
      }

   forCursor( rec54crs )                    /*edw1-Write connection records*/
      {
      s54Inst = rec54InstSet.elementAt( rec54crs );
      forCursor( rec56crs )
         {
         s56rec = connectionRecSet.elementAt( rec56crs );
         s56Inst=s56rec.subString( 3,6 );
         if ( s54Inst == s56Inst )
            {
            if( WRITELINE(sOutputCfr, s56rec) )
                  CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileWrite_Text )
            iRecCnt++;
            //break;   edw1-per request of S390 group; allow multiple 56's for a 54.
            }
         }
      }
   }              /* End looping through all of the assigned system objects*/

   //  If no products were written, close and erase the output file
   if ( iRecCnt < 6 )
   {                                           /* Eliminate the output file*/
     sOutputCfr.close();
     remove( (char *) sOutputFileName );
     return IDS_ErrorNoProdsToGen_Text;
   }                                    /* End eliminating the output  file*/

   sRecCnt = IString(iRecCnt);

   //* D96389: Begin logic:  Fixes the correctly copy of the records quantity
   //*         when it is 6 characters long.
   //* sTrailerRecord = STR_98RecType + sRecCnt.rightJustify( 5 );

   //  96549   Disable fix for defect 96389    HECC  11/29/2004
   // sTrailerRecord = STR_98RecType + sRecCnt.rightJustify( 6 );
   sTrailerRecord = STR_98RecType + sRecCnt.rightJustify( 5 );

   //* D96389: End logic   HECC - 11/18/2004

   if ( WRITELINE(sOutputCfr, sTrailerRecord) )
     return IDS_ErrorFileWrite_Text;
   sOutputCfr.close();



  // Begin D136691 Req 1578 filter to only impact Zseries

//139136 Begin

// We check if base retrieval was launched from econfig, if it was, then store the model name selected into variable strproductModel
// so we know with which brand we will be working
  if (pMainWindow->bConfig)
      {
      strArg = pMainWindow->strProductModel;
      ULONG ulOffset = strArg.indexOf('_');
      strArg.remove(ulOffset);

         // if the Model selected by the user is S390(Z series) we make the bS390 variable as true.
      if(strArg=="S390")
         {
                bS390=true;
         }
      }




  if(bS390==true)
{

  // If the Model selected by the user profile is Zseries and if bHostUsed and bInvret variables are diferent than true,
  // the user select to retieve a file listed in the Base retrieval main Window,
  if(pMainWindow->bHostUsed != true)
  if(pMainWindow->bInvRet!= true)
     {

      MRDname=pMainWindow->strBrowsePath + STR_PathDelimiter + pMainWindow->strGenerateName + ".MRD";// we look for the MRD asosiated with the BIR file.

       FILE* fp = fopen(MRDname, "r");
       if (fp) {
       // file exists
       fclose(fp);
       ReadMRD(MRDname,sOutputFileName);
       UpdateRemote(sOutputFileName);

       } else {
       // file doesn't exist
       MessageBox(0,"The selected products brand require an MRDB file, and it is not present, Please try again entering to 'Retreive from Host' menu and select an Inventory Services Host Interface or Local File Retreival","No MRD file associated with the selected file",0);

       }

     }
}
//139136 End





// we check if the user will use a Z series (S390) model and the check if the retrieve will be with a local file or and IS retrieval.
  if(pMainWindow->bS390==true)
  {
  //* REQ 1578: Begin logic ELRR- 24/09/2010
  if(pMainWindow->bHostUsed == true)
  {
  ReadMRD(pMainWindow->strMRDMulti,sOutputFileName);

//DEFECT 136853 BEGIN
  UpdateRemote(sOutputFileName);
  //DEFECT 136853 END
//139136 Begin
  //After the Local retrieve was made we make a copy of the MRD retrieved if the Model is a Zseries with the same name of the BIR generated.
  CopyFile(pMainWindow->strMRDMulti,pMainWindow->strBrowsePath + STR_PathDelimiter + pMainWindow->strGenerateName + ".MRD",false);
//139136 End
  pMainWindow->bHostUsed = false;
  }


 // Function Call for the IS Files Retrieval
  if(pMainWindow->bInvRet== true)
  {
  ReadMRD(pMainWindow->strBrowsePath + "\\invret.mrd",sOutputFileName);
  //DEFECT 136853 BEGIN
  UpdateRemote(sOutputFileName);
  //DEFECT 136853 END

//139136 Begin
  //After the IS retrieve was made we make a copy of the MRD retrieved if the Model is a Zseries with the same name of the BIR generated.
  CopyFile(pMainWindow->strBrowsePath + STR_PathDelimiter + "invret.mrd",pMainWindow->strBrowsePath + STR_PathDelimiter + pMainWindow->strGenerateName + ".MRD",false);
//139136 End

  pMainWindow->bInvRet = false;
  }
  //* REQ 1578: END logic ELRR- 24/09/2010
  } // End  D136691


   //* D95670:  Begin logic:  This code is now obsolete. Please check the function Write05Recs
   //* Check05reclen(sOutputFileName); // ZOR 09032004 93732
   //* D95670:  End new logic:   HECC - 10/29/2004
   return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  The GenSystem function starts a new system by creating the 07 and 01
//  CFREPORT records and adding them to the output file.  The notebook
//  tab name is used as the description on both records
//int GenSystem( ostream& sOutStream, IString& sSystemName )
//{
//   IString  sSystemRecord, sSectionRecord;
//int GenSystem( ostream& sOutStream, IString& sSystemName, IString& strMRD )     // 94021
//  D95887 also add 53 record between 07 and 01
int GenSystem( ostream& sOutStream, IString& sSystemName, IString sRecord53 )      //95887
{
   IString  sSystemRecord, sSectionRecord;
   sSystemRecord = STR_07RecType + IString::copy( STR_Blank1, 83 );
   sSystemRecord.overlayWith( IString(sSystemName.subString( 1, 40 )), 6);
   sSectionRecord = STR_01RecContent + IString::copy( STR_Blank1, 82 );
   sSectionRecord.overlayWith( IString(sSystemName.subString( 1, 40 )), 4);
 // ZOR 94021  get53record from mrd file to add it in the RET file under 07 record every system
 //  sDateRecord=get53record(strMRD);

   if ( BackEnd::WRITELINE(sOutStream, sSystemRecord) )
     return IDS_ErrorFileWrite_Text;
 //  if ( BackEnd::WRITELINE(sOutStream, sDateRecord) )  // ZOR 94021
 //    return IDS_ErrorFileWrite_Text;
 //* 95887  add 53 record in RET file
   if ( BackEnd::WRITELINE(sOutStream, sRecord53) )
     return IDS_ErrorFileWrite_Text;
 //* 95887    end
   if ( BackEnd::WRITELINE(sOutStream, sSectionRecord) )
     return IDS_ErrorFileWrite_Text;
   return NO_ERROR;

}


//  The SeekWrite function seeks an offset within the input file, reads
//  the record at that location, overlays bytes 1 and 2 with the record
//  type specified in sRecType, and writes the record to the output file.
int SeekWrite( istream& sIstream, ostream& sOstream,
               IString  sRecType, unsigned long uOffset )
{
   /* Seek to the record in the input file  */
   sIstream.seekg( uOffset );
   if ( sIstream.fail() )
     return IDS_ErrorInputFileSeek_Text;
   /* Read the record from the input file   */
   IString sIOrecord = IString::lineFrom( sIstream );
   if ( sIstream.fail() )
     if ( sIstream.eof() )
       return IDS_ErrorUnexpectedEof_Text;
     else
       return IDS_ErrorFileRead_Text;
   /* Write the record to the output file   */
   if ( BackEnd::WRITELINE(sOstream, sIOrecord.overlayWith( sRecType, 1 )) )
     return IDS_ErrorFileWrite_Text;
   return NO_ERROR;
}
//=======================================================================
// 93732:  New Function.
// FUNCTION NAME    Check05reclen
//
// DESCRIPTION      Check the 05rec length to know if is greater than 76 chars
//                  it means the description is less than 57 chars
//                  if it is greater then cut the line and add a new line with the
//                  rest of the description
//                  ZOR: 09/03/2004
// 95670: This function is now obsolete, please see Write05Recs
//        10/29/2004  -  HECC
//=======================================================================
void BackEnd :: Check05reclen( IString strRET )
{
  IString buf;
  char * ReadBuff1 = new char[READBUF_SIZE1];
  IString strnew;
  int imaxreclen=74;
  int ichrtocut=False;
  IBoolean islonger=False;
  IBoolean iswrapped=False;
  int inewreclen;
  FILE  *fret;
  FILE  *fdum;
  IString strDummy = strRET;
  //create a dummy file to copy the RET file and reformat the 05 records
  strDummy.remove(strDummy.lastIndexOf('.')+1);
  strDummy+= "DUM";
  rename(strRET,strDummy);
  //open dummy file for reading
  if (fdum= fopen(strDummy,"r"))
   {
      //create a new RET file for writing
      if(fret= fopen(strRET,"w"))
       {
          // read every record in the dummy file
          while (fgets(ReadBuff1,READBUF_SIZE1,fdum))
           {
              buf = ReadBuff1;
              islonger=False;
              // if the type of record is 05 and is greater than 73 chars
              if (buf.subString(1, 2) == "05" && buf.length()>=imaxreclen)
               {
                  ichrtocut=imaxreclen-1;
                  iswrapped=False;
                  //check every char in the record from the end to back to find a blank char
                  while(buf.subString(ichrtocut,1)!=" ")
                   {
                      ichrtocut--;
                      inewreclen=buf.length()-ichrtocut;
                      //if the records is a header "***" or "----" then cut it with 73 chars
                      if(buf.subString(ichrtocut,1)=="*" || buf.subString(ichrtocut,1)=="-")
                       {
                           iswrapped=False;
                           break;
                       }
                      else
                       {
                           //else strnew will include the words of the 05 record greater of position 73
                           strnew = buf.subString(ichrtocut,inewreclen)+"\n";
                           iswrapped=True;
                       }
                   }

                  islonger=True;
                  // move the remainder of the description over 37 spaces so that it is aligned with the previous line
                  strnew = buf.subString(1,16)+"                                     "+strnew;
                  // cut the original 05 record until the strnew record starts
                  buf = buf.subString(1,ichrtocut)+"\n";
                  // write original 05 record in RET file
                  fputs(buf,fret);
                  // if the new 05 record was created then write it in RET file
                  if(iswrapped)
                  fputs(strnew,fret);
              }
             if(!islonger)
             fputs(buf,fret);
          }
       }
   }
  fclose(fret);
  fclose(fdum);
  remove(strDummy);
  return ;
}

//=======================================================================
// 95670:   New Function.
// FUNCTION NAME    Write05Recs
//
// DESCRIPTION      Check the 05rec length to know if is greater than 76 chars
//                  it means the description is less than 57 chars
//                  if it is greater then cut the line and add a new line with the
//                  rest of the description
// Note:            This function subsitutes the previous version of this function
//                  (see Check05reclen). This was made to improve the functionality
//                  the actual version processes only RT 05 instead of the old
//                  version that reads all the records in the .RET file.
//                  HECC: 10/29/2004
//=======================================================================
void BackEnd :: Write05Recs( ofstream& sOutputCfr, IString strRT05, long int& iRecCnt )
{
  IString strNew;
  int iMaxReclen = 74;
  int iChrToCut  = 0;
  IBoolean bIsWrapped;
  int iNewReclen;

  iChrToCut = iMaxReclen-1;
  bIsWrapped = False;

  if (strRT05.length() >= 73)
     {
     //Check every char in the input record from last to first to find a blank char
     while(strRT05.subString(iChrToCut,1)!=" ")
          {
           iChrToCut--;
           iNewReclen = strRT05.length() - iChrToCut;

           //Cuts the record "***" or "----" to 73 chars
           if (strRT05.subString(iChrToCut,1)=="*") //147293
              {
               bIsWrapped = False;
               break;
              }
           else
              {
               //else:  Creates the second RT 05, strNew, which will contain the chars beyond position 73
               //strNew = strRT05.subString(iChrToCut,iNewReclen+1);
               strNew = strRT05.subString(iChrToCut,iNewReclen+1).stripLeading(); //D106441
               bIsWrapped = True;
              }
          }
     //D106441 RERC  02/21/2006  if the text begins with a number it means that the
     //                          text corresponds a feature, is not then write the text
     //BEGIN                     below of the previous line.
     if (!(strRT05.subString(LENGTH_START_TEXT,1).isDigits()))
     {
       // Adds a string of spaces so that it is aligned with the previous text line
       strNew = strRT05.subString(1,LENGTH_TO_ALIGN)+ strNew;
     }
     else
     {
       // Adds a string of 37 spaces so that it is aligned with the previous line
       //strNew = strRT05.subString(1,16) + "                                     " +
       //       strNew;
       strNew = strRT05.subString(1,LENGTH_TO_ALIGN) + STR_SpacesAlign + strNew;
     }
     //D1046441 RERC 02/21/2006  END.

     // Cut the first section of the RT 05 and writes it to the .RET output file
     strRT05 = strRT05.subString(1,iChrToCut);
     WRITELINE(sOutputCfr, strRT05);
     iRecCnt++;

     // Writes the second section of the RT 05 if it was created
     if (bIsWrapped)
        {
         WRITELINE(sOutputCfr, strNew);
         iRecCnt++;
        }
     }
  else
     {
      WRITELINE(sOutputCfr, strRT05);
      iRecCnt++;
     }

  return;

}

//=======================================================================
// 94021:  New Function.
// FUNCTION NAME    get53record
//
// DESCRIPTION     get 53 record from MRD file
//                  ZOR: 10/15/2004
//=======================================================================
/*IString get53record(IString strMRD)
{
  IString bufmrd, buf, str53;
  char * ReadBuff1 = new char[READBUF_SIZE1];
  FILE  *fMRD;

  //open MRD file for reading
  if(fMRD = fopen(strMRD,"r"))
    {
     while (fgets(ReadBuff1,READBUF_SIZE1,fMRD))
       {
        bufmrd = ReadBuff1;
        if (bufmrd.subString(1, 2) == STR_53RecType )
           {
           str53 = bufmrd.subString(0,bufmrd.length()-1);
           break;
           }
       }
     }

  fclose(fMRD);
  return str53;
}   */




//=======================================================================
// REQ 1578:  New Function.
// FUNCTION NAME    READMRD()
//
// DESCRIPTION     get the 53,08 and stored in an array to create the 01,
//                 07,25 and 95 after, also get the 54,55 and 56 records
//                 and stored in an array too to include them in the .RET
//                 File
//                 ELRR: 09/24/2010
//=======================================================================
///////////////////////////////////////////////////////////////////
Boolean ReadMRD(char *FileName,char *RETFile)
{
char buf[3000];

int Count=0;int Count2=0;
int i=0;
int Found = 0;
IString is, ix;
int indexMain,indexSub;


int MultiStart[1000];
int MultiEnd[1000];
char **MRDm;

int indexMainm[1000];
int indexSubm[1000];
int Foundm =0;
int iRow;



// Open the MRD file
//
FILE * pFile;
  pFile = fopen (FileName,"r");
  if (pFile==NULL)
  {
   // MessageBox(0,FileName,"Error opening file",0);
   return FALSE;
}



// Read All rows from the Text file into Array MRD[]
//
while( fgets(buf,3000,pFile) )
 {
 MRD[Count] =  (char *) malloc(3000);
 strcpy(MRD[Count],buf);
 Count++;
 }
fclose(pFile);


// count systems in the MRD file 28/07/2010 erick

for(i=0;i<Count;i++)
{
 ix=MRD[i];
 if(ix.subString(1,2)=="00")
  {
  MultiStart[MSystems] = i;
  MSystems++;
  }

 if(ix.subString(1,2)=="98")
  {
  MultiEnd[MSystems-1] = i;
 }
}


//End Count systesms in the MRD 28/07/2010 Erick


// Reset all counters for multi systems
for(i=0;i<MSystems;i++) MultiCount[i]=0;


int m;
int r;
int rows;

for (m=0;m<MSystems;m++)
{

rows = MultiEnd[m] - MultiStart[m] + 1;

// Create an array of (char *) with the number of rows
Multi[m] = (char **) malloc(rows*sizeof(char **));
i=0;
for(r=MultiStart[m];r<=MultiEnd[m];r++)
{
Multi[m][i] = (char *) malloc(strlen(MRD[r])+100);
strcpy(Multi[m][i], MRD[r]);
i++;
MultiCount[m]++;
}

}




// MRD[] Array was dumped into a bidimensional array, Multi[][]
// Free the memory reserved in heap
for (i=0;i<Count;i++)
{
free(MRD[i]);
}



for (m=0;m<MSystems;m++)
{

ix = "System";
ix = ix + (IString) m;
ix = ix + (IString) ".CFR";

FILE *pFileD;
  pFileD = fopen (ix,"w");
  if (pFileD==NULL)
  {
    MessageBox(0,ix,"Error opening file",0);
   return FALSE;
  }

// Take all records disregarding the information about the main system and subsystem
//
for (i=0;i<MultiCount[m];i++)
{
fputs(Multi[m][i],pFileD);
}
fclose(pFileD);


} // loop mSystems








// BEGIN OF MAIN LOOP THAT LOOKS INTO DIFFERENT CFR's                      /////////////////////////////////////////////////////////////////////////
for (m=0;m<MSystems;m++)
{

// Take data of current system
MRDm = Multi[m];
Count = MultiCount[m];
Found = 0;

indexMain=-1;
indexSub=-1;


// Look for the 1st. System inside the MRD data
for (i=0;i<Count;i++)
{
ix = MRDm[i];
   while (ix.indexOf("53") == 1)
   {
   // We need at least two more records
   if ( (i+2) >=Count ) break;
   // Now check that the next record is 08 with the same product
   ix = MRDm[i+1];
   if ( ix.indexOf("08") == 1 )
    {
     // Ok, now the 3rd. row must be a "54" record
     ix = MRDm[i+2];
     if ( ix.indexOf("54") == 1 )
      {
       Found = 1;
       break;
      }

    }
   break;
   }

if ( Found == 1 ) break;
} //end for


 if ( Found == 0 ) continue; // Go for the next system

Foundm = 1;     // Signal that at least 1 system was found
indexMain = i;  // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


Found = 0;


// Look for the 2nd. System inside the MRD data
for (i=0;i<Count;i++)
{
ix = MRDm[i];
   while (ix.indexOf("53  2458") == 1)
   {
   // We need at least two more records
   if ( (i+2) >=Count ) break;
   // Now check that the next record is 08 with the same product
   ix = MRDm[i+1];
   if ( ix.indexOf("082458") == 1 )
    {
     // Ok, now the 3rd. row must be a "54" record
     ix = MRDm[i+2];
     if ( ix.indexOf("54") == 1 )
      {
       Found = 1;

       break;
      }

    }
   break;
   }

if ( Found == 1 ) break;
} //end for




if ( Found == 1 ) indexSub = i;




// UP TO THIS POINT, WE HAD FOUND A NEW SYSTEM (2458) IN THE MRD FILE







// INSERT ALL ROWS FROM THE 1st. SYSTEM INTO THE GLOBAL ARRAY mrd1[]

// Take note of where the set of "New System" rows begin
iRow = indexMain;

MRDCount1 =0;

// Copy the data from the MRD belonging to the 1st. System into the "New Sytem" array, "mrd1[]"
for (i=iRow+2;i<Count;i++)
{
ix = MRDm[i];
// The record MUST be 54,55 or 56
if ( !(ix.indexOf("54") == 1 || ix.indexOf("55") == 1 || ix.indexOf("56") == 1) ) break;
mrd1[MRDCount1] =  (char *) malloc( strlen( MRDm[i] )+100 );
strcpy(mrd1[MRDCount1],MRDm[i]);
MRDCount1++;
}






// INSERT ALL ROWS FROM THE 2nd. SYSTEM INTO THE GLOBAL ARRAY mrd2[]

// Take note of where the set of "New System" rows begin
iRow = indexSub;

ix = iRow;



if (iRow>=0)   // If that verifys that there is a subsystem
{

 MRDCount2 =0;

// Copy into new array "mrd[]" The first 2 rows starting from the point the 2nd. System was found
//
mrd2[MRDCount2] =  (char *) malloc( strlen(MRDm[iRow])+100 );
strcpy(mrd2[MRDCount2],MRDm[iRow]);
MRDCount2++;

mrd2[MRDCount2] = (char *) malloc( strlen(MRDm[iRow+1])+100 );
strcpy(mrd2[MRDCount2],MRDm[iRow+1]);
MRDCount2++;



// Copy the rest of the data from the MRD file into the "New Sytem" array, "mrd2[]"
for (i=iRow+2;i<Count;i++)
{
ix = MRDm[i];
// The record MUST be 54,55 or 56
if ( !(ix.indexOf("54") == 1 || ix.indexOf("55") == 1 || ix.indexOf("56") == 1) ) break;
mrd2[MRDCount2] =  (char *) malloc(strlen(MRDm[i])+100);
strcpy(mrd2[MRDCount2],MRDm[i]);
MRDCount2++;
}

} // End  if that verifys that there is a subsystem





FILE *pFileD;
  ix = "mrdArray1_";
  ix = ix + (IString) m;
  ix = ix + ".cfr";
  pFileD = fopen (ix,"w");
  if (pFileD==NULL)
  {
    MessageBox(0,ix,"Error opening file",0);
   return FALSE;
 }

// Take all records disregarding the information about the main system and subsystem
//
for (i=0;i<MRDCount1;i++)
{
fputs(mrd1[i],pFileD);
}
fclose(pFileD);
//
  ix = "mrdArray2_";
  ix = ix + (IString) m;
  ix = ix + ".cfr";
  pFileD = fopen (ix,"w");
  if (pFileD==NULL)
  {
    MessageBox(0,ix,"Error opening file",0);
   return FALSE;
 }
// Take all records disregarding the information about the main system and subsystem
//
for (i=0;i<MRDCount2;i++)
{
fputs(mrd2[i],pFileD);
}
fclose(pFileD);



CurrentSystem = m; // Tell the other functions the current system number, via global variable
CreateRETArray();
AppendRETRecords(RETFile);




} // END OF MAIN LOOP THAT LOOKS INTO DIFFERENT CFR's                      /////////////////////////////////////////////////////////////////////////




MSystems=0;

return TRUE;

}   // ReadMRD()







//=======================================================================
// REQ 1578:  New Function.
// FUNCTION NAME   CreateRETArray()
//
// DESCRIPTION     We take the previous registers and add them to the
//                 correct systems, and them create the RET file.
//                 ELRR: 09/24/2010
//=======================================================================
///////////////////////////////////////////////////////////////////
Boolean CreateRETArray()
{

int i;
IString ix,iy;


// If there are no records update, don't do anything
if (MRDCount1 == 0 ) return FALSE;



// Copy all system1 "as is" into ret[] array
for(i=0;i<MRDCount1;i++)
{
ret[RETCount] =  (char *) malloc( strlen(mrd1[i])+100 );
strcpy(ret[RETCount],mrd1[i]);
RETCount++;
}



//If MRDCount2 is 0 das mean there is not subsystem in the 00/98 records

if (MRDCount2>0)   // Begin of the MRDCount2 validation                          ////////////////////////////////////////////////////////////////////////////////
{
strcat(ret[RETCount-1],"07   CPU 2 \n");  // Add Record 07


// At least 4 more records are required
//
for(i=0;i<=3;i++)
{
ret[RETCount+i] =  (char *) malloc(1500);
}



// Add Record 53 & 01
strcpy(ret[RETCount+0],mrd2[0]);        // Copy record 53 from MRD 2nd. System
strcpy(ret[RETCount+1],"012CPU 2\n");    // Record 01



// Add Record 25
strcpy(ret[RETCount+2],"252458 ");
ix = mrd2[1];ix = ix.subString(24,3);
iy = ix;
strcat(ret[RETCount+2],ix);
strcat(ret[RETCount+2],"    1\n");



// Add Record 95
strcpy(ret[RETCount+3],"952458");
strcat(ret[RETCount+3],iy);



// 10 Spaces filler
strcat(ret[RETCount+3],"          ");

// Append serial number
ix = mrd2[2];
ix = ix.subString(28,9);
strcat(ret[RETCount+3],ix);
strcat(ret[RETCount+3],"\n");


// Add to pointer the inserted 5 records
RETCount+=4;



// Copy the rest of the records from the MRD extract (Records 54,55 and 56)
for(i=2;i<MRDCount2;i++)
{
ret[RETCount] =  (char *) malloc(strlen(mrd2[i])+100);
strcpy(ret[RETCount],mrd2[i]);
RETCount++;
}

}// End of the MRDCount2 validation                          ////////////////////////////////////////////////////////////////////////////////





ix = "RETArray_";
ix = ix + (IString) CurrentSystem;
ix = ix + ".cfr";

FILE *pFile;
  pFile = fopen (ix,"w");
  if (pFile==NULL)
  {
    MessageBox(0,"Error opening file",ix,0);
   return FALSE;
 }


// Take all records disregarding the information about the main system and subsystem
//


for (i=0;i<RETCount;i++)
{
fputs(ret[i],pFile);
}
fclose(pFile);
//
 //End RETARRAY.cfr



return TRUE;
}   // CreateRETArray()




























Boolean AppendRETRecords(char *FileName)
{
char buf[5000];
char *HRET[5000];
int Count=0;int FinalCount=0;
int i=0;
int Found = 0;
IString is, ix;
int pos1=0;int pos2=0;int ipad;




// If there is no array with which update the RET file, return
if (RETCount == 0) return FALSE;

// Open the file
//
FILE * pFile;
  pFile = fopen (FileName,"r");
  if (pFile==NULL)
  {
    MessageBox(0,"Error opening file",FileName,0);
   return FALSE;
}




// Read All rows of Text file into Array HRET[]
//
while( fgets(buf,1000,pFile) )
 {
 HRET[Count] =  (char *) malloc(1500);
 strcpy(HRET[Count],buf);
 Count++;
 }
fclose(pFile);




IString Serial = ret[0];
Serial = Serial.subString(28,9);




// Look into the HRET array -the RET file- where the Serial begins
for(i=0;i<Count;i++)
{
ix = HRET[i];
if (ix.indexOf("54") == 1 && ix.subString(28,9) == Serial ) break;
}

if (i>=Count) return FALSE;


// Determine pos1 and pos2, which will be rows ommited from the current RET, that is, those will NOT be reinserted
// and in their place, the ret[] array will inserted
pos1 = i;



for(i=pos1+1;i<Count;i++)
{
ix = HRET[i];
if ( ix.indexOf("54")== 1 || ix.indexOf("55")== 1 || ix.indexOf("56")== 1 ) {pos2=i;continue;}
break;
}





//open the ret file now for writing
  pFile = fopen (FileName,"w");
  if (pFile==NULL)
  {
    MessageBox(0,"Error opening file","",0);
   return FALSE;
 }



// Take all records disregarding the information about the main system and subsystem
//
for (i=0;i<pos1;i++)
{
fputs(HRET[i],pFile);
FinalCount++;
}



// Insert processed MRD records
//
for (i=0;i<RETCount;i++)
{
fputs(ret[i],pFile);
FinalCount++;
}




// Insert all records after the serial number was found
//
for (i=pos2+1;i<Count;i++)
{
ix = HRET[i];
// Do not store 98 and 99 records. (Record 54 must have appeared before reaching Record 98 or 99)
if (ix.indexOf("98") == 1  || ix.indexOf("99") == 1 ) break;
fputs(HRET[i],pFile);
FinalCount++;
}



// Create Record 98
//
FinalCount++;//.Record 98 counts too
ix = FinalCount;
ipad = 5 - ix.length();
is = "98";
for(i=0;i<ipad;i++) is = is + " ";
is = is + ix;

// Write record 98
fputs(is,pFile);


// Close RET file
fclose(pFile);




// ------------------------------------------------------------------------------------------------------------------------------



// Free locally allocated array in this function
for(i=0;i<Count;i++)
{
free(HRET[i]);
}
Count=0;



// Free globally allocated array ret[]
for(i=0;i<RETCount;i++)
{
free(ret[i]);
}
RETCount=0;




// Free globally allocated array mrd1[]
for(i=0;i<MRDCount1;i++)
{
free(mrd1[i]);
}
MRDCount1=0;



// Free globally allocated array mrd2[]
for(i=0;i<MRDCount2;i++)
{
free(mrd2[i]);
}
MRDCount2=0;


return TRUE;


}   // AppendRETRecords()





//=======================================================================
// DEFECT 136853:   New Function.
// FUNCTION NAME   UpdateRemote(char *FileName)
//
// DESCRIPTION     Update those records with wrong description for
//                 defect 136853
//                 ELRR: 11/05/2010
//=======================================================================
///////////////////////////////////////////////////////////////////
Boolean  UpdateRemote(char *FileName)
{
char buf[5000];    // Array to hold lines
char *HRET[5000];
int Count=0;int FinalCount=0;
int i=0;
int Found = 0;
IString is, ix;
int pos1=0;int pos2=0;int ipad;


// Open the file
//
FILE * pFile;
  pFile = fopen (FileName,"r");
  if (pFile==NULL)
  {
    MessageBox(0,"Error opening file",FileName,0);
   return FALSE;
}



// Read All rows of Text file into Array HRET[]
//
while( fgets(buf,1000,pFile) )
 {
 HRET[Count] =  (char *) malloc(1500);
 strcpy(HRET[Count],buf);
 Count++;
 }
fclose(pFile);



// -------------------------------------------------------------------------------

// Make the appropriate changes to HRET[]





for(i=0;i<Count;i++)
{
ix = HRET[i];

if ( !(ix.indexOf("54")== 1 || ix.indexOf("55")== 1 || ix.indexOf("56")== 1 )) continue;
if ( !(ix.indexOf("EMOTEA") ||  ix.indexOf("EMOTEB")) ) continue;


Found = 1;

// Take starting position
pos1 = ix.indexOf("EMOTEA");if (pos1==0) pos1 = ix.indexOf("EMOTEB");

is = ix.subString(1,pos1-1);
ix = ix.subString(pos1+6);
ix = is + (IString) "REMOTE" + ix;

// Store the modified string into array HRET[]
strcpy(HRET[i],ix);

}

// -------------------------------------------------------------------------------




// If there were no changes made, then it does not make sense to update the file

while(Found)
{

//open the ret file now for writing
  pFile = fopen (FileName,"w");
  if (pFile==NULL)
  {
    MessageBox(0,"Error opening file","",0);
   return FALSE;
  }



// Write updated array into the same file
//
for (i=0;i<Count;i++)
{
fputs(HRET[i],pFile);
FinalCount++;
}
fclose(pFile);

break;
}



// Free locally allocated array in this function
for(i=0;i<Count;i++)
{
free(HRET[i]);
}
Count=0;


return TRUE;



} // UpdateRemote()









