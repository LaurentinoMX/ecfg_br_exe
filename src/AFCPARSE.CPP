#include "AFCCFRST.HPP"
#include "AFCPRODS.HPP"
#include "AFCBKEND.HPP"
#include "AFCMCROS.H"

#include "AFCINCLS.H"
#include "AF9APPLY.H"
#include <iseq.h>

#define STR_Rec05_Template  "05         10   %0"                  /* edw02 */

typedef ISequence  <Product *> ProductCnr;
typedef IKeySortedSet<System, unsigned int> SystemCnr;

int WriteReconciliationComments( MainWindow*, ostream& );         /* edw02 */
int UpdtAbRec( ostream&, Product *);
long ProductSort( Product* const&, Product* const& );
void AssignProducts( SystemCnr&     mySystemSet,
                     ProductCnr&    myProductSeq,
                     unsigned int   uCpuCount,
                     IString        );

////////////////////////////////////////////////////////////////////////////////
// Function: BackEnd::ParseCFREPORT()
//    Parses and validates the content of both new (no AA record) and old
//    (back level AA record) CFREPORTs.  It strips out and ignores any old
//    control records (AA, AB, and AC) present in the file and inserts new
//    control records.  This function is called by BackEnd::ListFiles().
//
// Overall logic:
//    1. Open the specified input file and an output file with a generated
//       temporary name in the same directory.
//    2. Write a dummy AA record as the first record in the output file.
//    3. Read each line in the input file
//       A. If the record type is AA, AB, or AC, ignore it
//       B. If the record type is 00, 03, 07, 01, or 98, copy to output file
//       C. If the record type is 96 and a product or an MES object was just
//          processed (see d and e), copy it to the output file.
//       D. If the record type is 08, 25, 47, or 49, read the next record and
//          ensure it is a type 95 record, then create a product object.  From
//          the product object, create an AB control record and write it to
//          the output file, then copy the product record and its associated
//          95 record to the output file.
//       E. If the record type is 19 or 50, read the next record and ensure
//          it is a type 95 record.  If it is a hardware upgrade MES, read
//          the next record and ensure it is a type 95 record also.  Create
//          an MES object, and from it create an AB control record.  Write
//          the AB record to the output file and copy the MES record and its
//          associated 95 record(s) to the output file.
//    4. When all records have been read and processed, create two system
//       objects.
//       A. If there are any CPUs or MES upgrades, loop through all of the
//          product objects, updating the AB control records for each product
//          that has been upgraded and creating system objects for each CPU.
//       B. If there are more than 2 system objects on completion of step a.
//          1. Loop through all of the products assigning software products
//             to the existing system objects based on the software product's
//             designated CPU type and serial number.
//          2. Loop through all of the products assigning unassigned products
//             to the existing system objects based on the product's system
//             number.
//          3. If there are still unassigned products:
//             a. Loop through all of the products assigning unassigned
//                software products to new system objects based on the
//                software product's designated CPU type and serial number.
//             b. Loop through all of the products assigning unassigned
//                products to new system objects based on the product's
//                system number.
//       C. If there are only two system objects, do nothing.
//       D. If there are three system objects, copy the contents of system
//          object 3 to object 2 and delete system object 3.  Then loop
//          through all of the product objects and re-assign those assigned
//          to system 3 to system 2.
//       E. If there are more than three system objects, delete system
//          object 2 and loop through all of the higher system objects
//          decrementing their identifiers by 1.  Then loop through all of
//          the product objects and reassign those assigned to system 2 to
//          system 1 and decrement all higher assignments by 1.
//       F. If any products were re-assigned, loop through all of the product
//          objects and recreate and rewrite their associated AB records.
//    5. Seek to the end of the output file, then loop through the system
//       objects, creating and writing AC control records for each to the
//       output file.
//    6. Create the real AA control record and over write it in the file.
//    7  Close both files, erase the original input file, and rename the
//       output file to the input file's name.
//
//******************************************************************************
// Revision Key:
//   VDT --- Von D. Tucker           (VNRTC370 at RHQVM22)
//
// Revision Notes:
//   Rev #   Flag   Who     Date                    Description
//   1.00           VDT   09/30/96   Initial creation
//   1.50    edw01  EDW   06/05/1998 Modified parsing to allow for multiple systems
//           edw02  EDW   01/03/2000 Added logic to create/write comment records that
//                                   detail Base Retrieval's reconciliation results
//           hecc   HECC  02/12/2002 Logic to verify that a RT 54 follow a RT 08/25
//                                   was disabled.  A RT 54 can follow any product
//                                   record (hw or sw)  RT 08/25/47/49.
//           hecc   HECC  10/18/2002 Added new logic to ensure that the Type-Serial#
//                                   is being assigend of the CPU's data and in case
//                                   there's not a CPU in the RET file, assign them
//                                   from the first product machine read from the
//                                   RET file to generate the RET's filename.
//         95887    ZOR   10/25/2004 add 53 record after 07 record
//         96220    HECC  11/10/2004 Modify logic to validate the case when the RT 53
//                                   are not present in the configuration.
//         96389    HECC  11/18/2004 Fixes the correctly copy of the records quantity
//                                   when it is 6 characters long.
//         95997    ZOR   19/11/2004 Include non INV 06 records in the BIR file and RET file
//         96549    HECC  11/29/2004 Disable fix for defect 96389
//
//
//         147293          ELRR  29/04/2013 Added logic to copy the 05 record types in the .ret file
//
//
////////////////////////////////////////////////////////////////////////////////

int BackEnd::ParseCFREPORT(CfReportFileStats& myCfrStats)
{

   IBoolean
     bAllRead      = False,                   /* Set when EOF on input file*/
     bCpuType      = False,                    /* Set when product is a CPU*/
     bHasCpu       = False,                /* Set when a CPU is encountered*/
     bHasSwCpu     = False,             /* Set when Sw w/CPU is encountered*/
     bHasUpgrade   = False,           /* Set when an upgrade is encountered*/
     bHdrProc      = False,               /* Set when all headers processed*/
     bTrlProc      = False,            /* Set when trailer record processed*/
     bCanAssign    = False,             /* Set if can be assigned to system*/
     bMesUpgrade   = False,                     /* Set if MES is an upgrade*/
     bNeedAbUpdt   = False,                   /* Set when AB needs updating*/
     bNeedNewSys   = False,              /* Set when a new system is needed*/
     bProcessAsNew = False,                /* Set when AA rec is back level*/
     b54RecValid   = False,                /* Set when a 54 record is valid*/
     b54SubValid   = False,             /* Set when 55/56 records are valid*/
     bRecComments  = False;     /*Reconcilation comment records added-edw02*/
     bfind95Rec    = true;
   IString     sAArecord, sABrecord, sCpuText, sCurrTypSer,
               sOrigTypSer, sInputCfrRec, sInputFileName, sLastProdRec,
               sProductIdRec1, sProductIdRec, sRecCnt, sRecType,
               sTrailerRecord, sInstanceId; // 53915 MAMT 09/01/2000 added sInstanceId
   IString
     sLastRecType = STR_DummyRecType,
     sMesTypes    = STR_MesRecTypes,
     sOrderTypes  = STR_OrderRecTypes,
     sCtlTypes    = STR_CtlRecTypes,
     sProdTypes   = STR_ProductRecTypes,
     s54SubTypes  = STR_54SubTypes,
     sValidTypes  = STR_ValidRecIds,
     // 53915a - MAMT 02/01/2001 begin new logic
     sPlacementD  = "PLACEMENTDATA",
//     sInputPlacementData = "05         10   Reconciliation used AAS data, but placement data exists for affected f/c. User Chose to continue";
     sInputPlacementData  = "05         10   Reconciliation used AAS data, but placement data exists",
     sInputPlacementData1 = "05         10       for affected f/c. User Chose to continue";
     // 53915a - MAMT 02/01/2001 end new logic
   long int
     iRecCmtCnt   = 0, /* edw02 */
     iError       = 0,
     iFeatCnt     = 0,
     iAssigned    = 0,
     iRecCnt      = 0,
     iCmtCnt      = 0,
     k            = 0, //53915 MAMT 09/08/2000
     K            = 0, //53915 MAMT 09/08/2000
     placementCnt = 0, //53915a MAMT 02/01/2001
     iSysCnt      = 2;
     strTempName  = "";

   unsigned int     uCntUnassigned = 0, uSystemId = 1;
   unsigned long    uRecOffset;
   Mes              *pMes;
   Product          *pProduct;


// Processes the file as a new file even if it has been processed
// by a previous version of the application


//   Open both the input and output file
   sInputFileName = myCfrStats.sCfrFullFileName;

   OPEN_INPUT_OUTPUT_CFRS( sInputCfr, sOutputCfr, sInputFileName )

//   Create a sequence for the product objects
   ProductCnr ProductSeq;

//   Write out a dummy AA record as a place holder
   myCfrStats.CreateAaRecord(sAArecord);
   if ( WRITELINE(sOutputCfr, sAArecord) )
     return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                   sInputFileName, sOutputFileName );

//  First read the entire input file, parsing and validating the input records
//  Copy the 00, 03, 07, and 01 header input records to the output file
//  Create product objects from the 08, 25, 47, and 49 input record types
//  Create MES objects from the 19 and 50 input record types and associate them
//  with their parent products, updating the products for upgrades.  For each
//  product and MES record read, write an AB control record to the output file
//  and then copy the product or MES record along with its associated 95 and 96
//  records to the output file.  When the 98 trailer record is read, validate
//  it and copy it to the output file, then continue reading records until EOF.

   while ( ! bAllRead )
   {                                   /* For each record in the input file*/
     sInputCfrRec = IString::lineFrom( sInputCfr );
     //if ( sInputCfr.fail() )
     if ( (sInputCfr.fail()) && (sInputCfrRec.subString(1,2) != "98") )//jt, 5/29/97
       if ( sInputCfr.eof() )
       {
         bAllRead = True;
         /* start IC65167 fix ZOR 072600*/
         if (bfind95Rec)
            {
             strTempName = sProductIdRec.subString(3,4) + "-" + sProductIdRec.subString(20,9);
             strTempName = IString::stripBlanks(strTempName);
             bfind95Rec = false;
            }
         /* end of IC65167 fix ZOR 072600*/
         continue;
       }
       else
         return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileRead_Text,
                                       sInputFileName, sOutputFileName );
     sRecType = sInputCfrRec.subString(1,2);


     // edw 03-19-97--Temporary fix to handle/ignore record types Order Apply don't support
//     if ( (sRecType == "54") || (sRecType == "55") || (sRecType == "56") )
//       continue;

     if ( sValidTypes.indexOf(sRecType ) == 0 )
     {
       iRecCnt ++;
       iCmtCnt ++;
       continue;
     }


     IString s98rec;
     if (sRecType == "98")
        s98rec = sInputCfrRec;

     if ( sValidTypes.indexOf(sRecType) == 0 )
       return Close_Cfrs_And_Return( sInputCfr, sOutputCfr,
                                     IDS_ErrorUnknownCfrRec_Text,
                                     sInputFileName, sOutputFileName );
     //  Ignore all control records and count all other records
     if ( sCtlTypes.indexOf(sRecType) > 0 )
       continue;
     else
       iRecCnt ++;

     //  Ignore all comment records and count all other records
     if ( sRecType == STR_05RecType )
     {
             /* Begin 147293 */
        if( sInputCfrRec!="05         10   This CFReport is generated by Base Retrieval")
           {  if ( WRITELINE(sOutputCfr, sInputCfrRec) )
               return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                             sInputFileName, sOutputFileName );
             iRecCmtCnt ++; }


             if (sInputCfrRec=="05         10   This CFReport is generated by Base Retrieval")
                 iCmtCnt ++;
              /* End 147293 */
       continue;
     }

     //  First, process and validate the order of the CFREPORT header records.
     //  bHdrProc will remain false until this is complete.
     if ( bHdrProc == False )
     {                                        /* Process the header records*/
       if ( sRecType == STR_00RecType && sLastRecType == STR_DummyRecType )
       {                         /* Process the CFreport header (00) record*/
         // Determine if the input file is valid/supported
         int iFileSuptRc = myCfrStats.IsFileSupported( sInputCfrRec, bProcessAsNew );
         if ( iFileSuptRc )
           return iFileSuptRc;
         if ( WRITELINE(sOutputCfr, sInputCfrRec) )
           return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                         sInputFileName, sOutputFileName );
       }                           /* End CFreport header record processing*/
       else
         if ( sRecType == STR_03RecType && sLastRecType == STR_00RecType )
         {                     /* Process the CFreport security (03) record*/
           if ( WRITELINE(sOutputCfr, sInputCfrRec) )
             return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                           sInputFileName, sOutputFileName );
         }                       /* End CFreport security record processing*/
         else
           if ( sRecType == STR_07RecType && sLastRecType == STR_03RecType )
           {                     /* Process the CFreport system (07) record*/
             if ( WRITELINE(sOutputCfr, sInputCfrRec) )
               return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                             sInputFileName, sOutputFileName );
           }                    /* End of CFreport system record processing*/
           else
            if ( sRecType == STR_53RecType && sLastRecType == STR_07RecType )      // ZOR 95887
             {                     /* Process the CFreport system (53) record*/
             if ( WRITELINE(sOutputCfr, sInputCfrRec) )
               return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                             sInputFileName, sOutputFileName );
             }                    /* End of CFreport system record processing*/

   /****Begin 147293******/
           else
            if ( sRecType == STR_05RecType && sLastRecType == STR_07RecType )
             {                     /* Process the CFreport comments (05) records */
             if ( WRITELINE(sOutputCfr, sInputCfrRec) )
               return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                             sInputFileName, sOutputFileName );
             }

   /****END 147293****/
            else
             if ( ( sRecType == STR_01RecType && sLastRecType == STR_53RecType ) ||   // ZOR 95887
                  ( sRecType == STR_01RecType && sLastRecType == STR_05RecType ) ||  // 147293
                  ( sRecType == STR_01RecType && sLastRecType == STR_07RecType )   )  // D96220:  HECC - 11/10/2004
              {                  /* Process the CFreport section (01) record*/
               bHdrProc = True;
               if ( WRITELINE(sOutputCfr, sInputCfrRec) )
                 return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                               sInputFileName, sOutputFileName );
               //  edw02-Next, add any reconcilation comments that have been captured
               if( !bRecComments )
               {
                 // 53915a - MAMT 02/01/2001 begin new logic
                 if (Feature54TableSet1.numberOfElements() >  1)
//                     Feature54TableSet1.elementAtPosition(1) == sPlacementD)
                    {
                     for ( k =  1, K = Feature54TableSet1.numberOfElements(); k <= K ; k++)
                      {
                       if (sPlacementD == Feature54TableSet1.elementAtPosition(k))
                         {
                          Feature54TableSet1.removeAtPosition(k);
                          k = K;
                          if ( WRITELINE(sOutputCfr, sInputPlacementData) )
                            return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                                          sInputPlacementData, sOutputFileName );
                          placementCnt++;
                          if ( WRITELINE(sOutputCfr, sInputPlacementData1) )
                            return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                                          sInputPlacementData1, sOutputFileName );
                          placementCnt++;
                         }
                      }
                    }
                 // 53915a - MAMT 02/01/2001 end new logic

                 iRecCmtCnt = WriteReconciliationComments( pMainWindow, sOutputCfr );
                 iRecCmtCnt = iRecCmtCnt + placementCnt; // 53915a - MAMT 02/01/2001
                 if( iRecCmtCnt == IDS_ErrorFileWrite_Text )
                    CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileWrite_Text )
                 //iRecCnt = iRecCnt + iRc;
                 //iCmtCnt = iCmtCnt + iRc;
                 bRecComments = True;
               }
             }                 /* End of CFreport section record processing*/
             else
               return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorInvalidCfreport_Text,
                                             sInputFileName, sOutputFileName );
       sLastRecType = sRecType;
       continue;
     }                                   /* End of header record processing*/

     //  Now that the header records have been successfully processed,
     //  process all of the CFREPORT body records (products and features).
     //  Features are checked first because they are more numerous.
     //  bTrlProc will remain false until a type 98 record is read.
     if ( bHdrProc && bTrlProc == False )
     {                                 /* Process the CFreport body records*/
       if ( sRecType == STR_96RecType )          /* Process featrue records*/
         if ( iFeatCnt > 0 )
         {
           iFeatCnt --;
           if ( WRITELINE(sOutputCfr, sInputCfrRec) )
             return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                           sInputFileName, sOutputFileName );
           else
             continue;
         }
         else
           return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorInvalidCfreport_Text,
                                         sInputFileName, sOutputFileName );

       else
         if ( sProdTypes.indexOf(sRecType) > 0 && iFeatCnt == 0 )
         {          /* Process the product (08, 25, 47, or 49) record types*/
                    /* F86750: RT 38 is required to be processed also, HECC - 01/15/2004 */

           b54RecValid = True;   //* 67958  HECC: 02/12/2002

           sProductIdRec = IString::lineFrom( sInputCfr );
           if ( sInputCfr.fail() )
             return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileRead_Text,
                                           sInputFileName, sOutputFileName );
           if ( sInputCfr.eof() )
             return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorInvalidCfreport_Text,
                                           sInputFileName, sOutputFileName );
           if ( sProductIdRec.subString(1,2) != STR_95RecType )
             return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorInvalidCfreport_Text,
                                           sInputFileName, sOutputFileName );
           if (bfind95Rec)
              //Get product info to name the file upon reconciliation 47705 - flw
              if (sProductIdRec.subString(1,2) == (IString)STR_95RecType)
                {
                IString stempname = sProductIdRec.subString(63,13).strip();  //ZOR IC65167 072600
                if ( stempname != "")                          //ZOR IC65167 072600
                   {
                     strTempName = sProductIdRec.subString(63,4) + "-" + sProductIdRec.subString(67,9);    //ZOR IC65167 072700
                     strTempName = IString::stripBlanks(strTempName);                                      //ZOR IC65167 072700
//ZOR 65167                 strTempName = sProductIdRec.subString(3,4) + "-" + sProductIdRec.subString(20,9);
//ZOR 65167                 strTempName = IString::stripBlanks(strTempName);
                     //strTempName = strTempName.stripBlanks();
                     bfind95Rec = false;
                   }
                //* D73493 Begin new logic: Stores the Type-Serial# of the first
                //* machine found in the generated .RET file.   HECC:10/18/2002
                else
                   {
                   IString strTempType = sProductIdRec.subString(3,4).strip();
                   IString strTempMod = sProductIdRec.subString(7,3).strip();
                   IString strTempSerial = sProductIdRec.subString(20,9).strip();

                   if ( (strTempType != "")   &&
                        (strTempMod != "")    &&
                        ( strTempSerial != "") )
                        {
                        strTempName = sProductIdRec.subString(3,4) +
                                      STR_Hyphen +
                                      sProductIdRec.subString(20,9);
                        strTempName = IString::stripBlanks(strTempName);
                        bfind95Rec = false;
                        }
                   }
                //* D73493 Ends new logic
                }
           iRecCnt ++;
           sLastProdRec = sInputCfrRec;

           //* Begin New logic:  67958    HECC  02/12/2002
           //* if ( sRecType == STR_25RecType )      /* Process featrue records*/
           //*    b54RecValid = True;
           //* else                                                        //edw1
           //*    if ( sRecType == STR_08RecType )                         //edw1
           //*       b54RecValid = True;                                   //edw1
           //*    else
           //*       b54RecValid = False;
           //* End New logic:  67958    HECC  02/12/2002

           //  Create a product object and write the product records
           pProduct=new Product;
           uRecOffset = sOutputCfr.tellp() + pProduct->uLengthAbRec;
           pProduct->ParseLoad( 'P', sInputCfrRec, sProductIdRec,
                                       uRecOffset, uRecOffset,     myCpuList );

           if (pProduct->category() == Product::CPU )
           {
             myCfrStats.uCountCpu++;                   // added by GARGJ
             bHasCpu = True;
           }
           else
             if ( pProduct->category() == Product::SW &&
                 (pProduct->sSwDesignatedCpuTypeSerial).size() > 0 )
               bHasSwCpu = True;
           iFeatCnt = pProduct->uCountFeatures;
           pProduct->uCurrentSystemId = 1;
           pProduct->CreateAbRecord( sABrecord );
           if ( WRITELINE(sOutputCfr, sABrecord) )
             return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                           sInputFileName, sOutputFileName );
           if ( WRITELINE(sOutputCfr, sInputCfrRec) )
             return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                           sInputFileName, sOutputFileName );
           if ( WRITELINE(sOutputCfr, sProductIdRec) )
             return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                           sInputFileName, sOutputFileName );
           sLastRecType = sRecType;
           ProductSeq.add(pProduct);
           if ( sOrderTypes.indexOf(sRecType) )
           {                  /* Increment initial order (08 & 47) counters*/
             myCfrStats.uCountNew ++;
             myCfrStats.uCountOrders ++;
             myCfrStats.uCountAppliedNew ++;
           }                        /* End initial order counter processing*/
           else
             myCfrStats.uCountInstalled ++;
         }                              /* End of product record processing*/

         else
           if ( sMesTypes.indexOf(sRecType) && iFeatCnt == 0 &&
                sProdTypes.indexOf(sLastRecType) )
           {                     /* Process the MES (19 or 50) record types*/
             bMesUpgrade = False;
             sProductIdRec = IString::lineFrom( sInputCfr );
             if ( sInputCfr.fail() )
               return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileRead_Text,
                                             sInputFileName, sOutputFileName );
             if ( sInputCfr.eof() )
               return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorInvalidCfreport_Text,
                                             sInputFileName, sOutputFileName );
             if ( sProductIdRec.subString(1,2) != STR_95RecType )
               return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorInvalidCfreport_Text,
                                             sInputFileName, sOutputFileName );
             iRecCnt ++;
             if ( sRecType == STR_19RecType )
             {                                       /* It's a hardware MES*/
               if ( sInputCfrRec.subString(10,7) !=
                    IString::copy( STR_Blank1, 7 ) )
               {                                     /* It's an upgrade MES*/
                 sProductIdRec1 = sProductIdRec;
                 sProductIdRec = IString::lineFrom( sInputCfr );
                 if ( sInputCfr.fail() )
                   return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileRead_Text,
                                                 sInputFileName, sOutputFileName );
                 if ( sInputCfr.eof() )
                   return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorInvalidCfreport_Text,
                                                 sInputFileName, sOutputFileName );
                 if ( sProductIdRec.subString(1,2) != STR_95RecType )
                   return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorInvalidCfreport_Text,
                                                 sInputFileName, sOutputFileName );
                 iRecCnt ++;
                 bHasUpgrade = bMesUpgrade = True;
               }                              /* End upgrade MES processing*/
             }                               /* End hardware MES processing*/

             //   Create an MES object and write the MES records
             pMes=new Mes;

             uRecOffset = sOutputCfr.tellp() + pMes->uLengthAbRec;
             pMes->ParseLoad( 'P', sInputCfrRec, sProductIdRec,
                                     uRecOffset, uRecOffset,     myCpuList );

             // Check if the MES matches the base product by comparing
             // the type, model, and order/serial number
             //edwtest
             //if ( ! pProduct->DoesMesMatch(*pMes) )
             //  return Close_Cfrs_And_Return( sInputCfr, sOutputCfr,
             //                           IDS_ErrorMesNotMatchProd_Text,
             //                           sInputFileName, sOutputFileName );

             iFeatCnt = pMes->uCountFeatures;
             pMes->uOffsetBaseProductRec = pProduct->uOffsetToProductRecord;
             pProduct->AssociateMes(pMes );
             pMes->CreateAbRecord( sABrecord );

             // Update the product's AB record for an upgrade MES
             if ( bMesUpgrade )
             {                            /* Update the product's AB record*/
               uRecOffset = uRecOffset - pMes->uLengthAbRec;    /* Save pos*/
               iError = UpdtAbRec(sOutputCfr, pProduct);
               if ( iError )
                 return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                               sInputFileName, sOutputFileName );
               sOutputCfr.seekp( uRecOffset );               /* Restore pos*/
               if ( sOutputCfr.fail() )
                 return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorOutputFileSeek_Text,
                                               sInputFileName, sOutputFileName );
             }                      /* End updating the product's AB record*/

             // Write out the MES AB, product, and 95 record(s)
             if ( WRITELINE(sOutputCfr, sABrecord) )
               return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                             sInputFileName, sOutputFileName );
             if ( WRITELINE(sOutputCfr, sInputCfrRec) )
               return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                             sInputFileName, sOutputFileName );
             if ( bMesUpgrade )         /* If there are two type 95 records*/
               if ( WRITELINE(sOutputCfr, sProductIdRec1) )    /* write 1st*/
                 return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                               sInputFileName, sOutputFileName );
             if ( WRITELINE(sOutputCfr, sProductIdRec) )
               return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                             sInputFileName, sOutputFileName );
             myCfrStats.uCountMes ++;
             myCfrStats.uCountOrders ++;
             myCfrStats.uCountAppliedMes ++;
           }                                /* End of MES record processing*/

           else
             if ( sRecType == STR_98RecType && iFeatCnt == 0 )
             {                       /* Process the CFreport trailer record*/

               //* D96389: Begin logic:  Fixes the correctly copy of the records quantity
               //*         when it is 6 characters long.
               //* sRecCnt = sInputCfrRec.subString(3,5);

               //  D96549  Disable fix for defect 96389    HECC 11/29/2004
               // sRecCnt = sInputCfrRec.subString(3,6);
               sRecCnt = sInputCfrRec.subString(3,5);

               //* D96389: End logic   HECC - 11/18/2004

               if ( iRecCnt == sRecCnt.asInt() )
                 bTrlProc = True;
               else
                 return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorInvalidCfreport_Text,
                                               sInputFileName, sOutputFileName );

               //edw02 sRecCnt = IString(iRecCnt - iCmtCnt);
               sRecCnt = IString(iRecCnt - iCmtCnt + iRecCmtCnt); /* edw02 */

               //* D96389: Quantities of 6 characters long.   HECC - 11/18/2004
               //* sTrailerRecord = STR_98RecType + sRecCnt.rightJustify( 5 );

               //  D96549  Disable fix for defect 96389    HECC 11/29/2004
               // sTrailerRecord = STR_98RecType + sRecCnt.rightJustify( 6 );
               sTrailerRecord = STR_98RecType + sRecCnt.rightJustify( 5 );

               if ( WRITELINE(sOutputCfr, sTrailerRecord) )
                 return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                               sInputFileName, sOutputFileName );
             }                 /* End of CFreport trailer record processing*/
           else
             if ( sRecType == STR_06RecType )                  // ZOR  95997
              {                 /* ZOR Process Addl. system (06) records */
              if ( WRITELINE(sOutputCfr, sInputCfrRec) )
                 return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                             sInputFileName, sOutputFileName );
              }

           else
              if ( sRecType == STR_07RecType )
              {                 /* edw01 Process Addl. system (07) records */
              if ( WRITELINE(sOutputCfr, sInputCfrRec) )
                 return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                             sInputFileName, sOutputFileName );
              }
           else
             if ( sRecType == STR_53RecType )                  // ZOR 95887
              {                 /* ZOR Process Addl. system (53) records */
              if ( WRITELINE(sOutputCfr, sInputCfrRec) )
                 return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                             sInputFileName, sOutputFileName );
              }
           else
              if ( sRecType == STR_01RecType )
              {                 /* edw01 Process Addl. section (01) records*/
              if ( WRITELINE(sOutputCfr, sInputCfrRec) )
                 return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                               sInputFileName, sOutputFileName );
              }

           else
              if ( b54RecValid && sRecType == STR_54RecType )
              {                                 /* Process the 54 record(s)*/

                // begin new logic 53915 - MAMT - 09/01/2000
                sInstanceId = " ";
                IString Record54 = sInputCfrRec.subString(10,4).strip() +
                                   sInputCfrRec.subString(14,7).strip() +
                                   sInputCfrRec.subString(21,7).strip();
//                                   sInputCfrRec.subString(28,9).strip();

                //* Note:  At this point, the table Feature54TableSet1 is not
                //* loaded yet, so, the number of elements in it will be 0
                //* HECC:  67958     02/12/2002
                //* if (Feature54TableSet1.numberOfElements() ==  1  ||  //* HECC:  67958     02/12/2002
                if (Feature54TableSet1.numberOfElements() ==  0  ||      //* HECC:  67958     02/12/2002
                    sInputCfrRec.subString(10,4).strip()  ==  "" ||
                    sInputCfrRec.subString(14,7).strip()  ==  "" ||
                    sInputCfrRec.subString(21,7).strip()  ==  ""  )
//                    sInputCfrRec.subString(28,9).strip()  ==  "" )
                    {
                     if ( WRITELINE(sOutputCfr, sInputCfrRec) )
                        return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                               sInputFileName, sOutputFileName );
                    }
                else
                  for ( k =  1, K = Feature54TableSet1.numberOfElements(); k <= K ; k++)
                     {
                      if (Record54 == Feature54TableSet1.elementAtPosition(k))
                        {
                         k = K;
                         iCmtCnt ++;  // to decrement the value because 1 line will be deleted
                         sInstanceId = sInputCfrRec.subString(3,6).strip();
                         Feature54TableSet2.add(sInstanceId);
                        }
                      else
                        {
                         if (k == K)
                           if ( WRITELINE(sOutputCfr, sInputCfrRec) )
                              return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                                     sInputFileName, sOutputFileName );
                        }
                     }

                //   end new logic 53915 - MAMT - 09/01/2000
                /* begin old logic 53915 - MAMT - 09/01/2000
                if ( WRITELINE(sOutputCfr, sInputCfrRec) )
                  return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                          sInputFileName, sOutputFileName );
                   end old logic 53915 - MAMT - 09/01/2000  */
                b54SubValid = True;
              }
              else
                if ( b54SubValid && s54SubTypes.indexOf(sRecType) )
                {                            /* Process the 55/56 record(s)*/

                // begin new logic 53915 - MAMT - 09/01/2000
                if (Feature54TableSet2.numberOfElements() ==  0  ||
                    sInputCfrRec.subString(3,6).strip()   ==  "" )
                    {
                     if ( WRITELINE(sOutputCfr, sInputCfrRec) )
                        return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
	                                               sInputFileName, sOutputFileName );
                    }
                else
                  for ( k =  1, K = Feature54TableSet2.numberOfElements(); k <= K ; k++)
                     {
                      if (sInputCfrRec.subString(3,6).strip() == Feature54TableSet2.elementAtPosition(k))
                        {
                         Feature54TableSet2.removeAtPosition(k);
                         iCmtCnt ++;
                         k = K;
                        }
                      else
                        {
                         if (k == K)
                           if ( WRITELINE(sOutputCfr, sInputCfrRec) )
                              return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                                     sInputFileName, sOutputFileName );
                        }
                     }

                   // end new logic 53915 - MAMT - 09/01/2000
                   /* begin old logic 53915 - MAMT - 09/01/2000
                   if (sInstanceId != sInputCfrRec.subString(3,6).strip())
                    if ( WRITELINE(sOutputCfr, sInputCfrRec) )
                       return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                              sInputFileName, sOutputFileName );
                   end old logic 53915 - MAMT - 09/01/2000  */
                }
                else                            /* Else it's an unexpected record type*/
                   return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorInvalidCfreport_Text,
                                     sInputFileName, sOutputFileName );
     }                            /* End of CFreport body record processing*/

     else                            /* Else it's an unexpected record type*/
       return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorInvalidCfreport_Text,
                                     sInputFileName, sOutputFileName );

   }                                           /* End input file processing*/


//-------------------------- Added by GARGJ ----------------

   SystemCnr  mySystemSet;
   AssignProducts( mySystemSet, ProductSeq, myCfrStats.uCountCpu, sInputFileName );

   // Update AB records of all the Products
   ProductCnr::Cursor ProdCursor( ProductSeq );
   forCursor( ProdCursor )
   {
      pProduct = ProductSeq.elementAt( ProdCursor );
      iError = UpdtAbRec(sOutputCfr, pProduct);

      //* D73493 Begin new logic: Verifies if there's a CPU in the products of
      //* the .RET file and if so, overwrites the assigned machine Type-Serial#
      //* with the CPU's data to generate the .RET file    HECC: 10/18/2002
      if (pProduct->eProdType == AbstractProduct::CPU)
         {
          strTempName = pProduct->sCurrentTypeModel.subString(1,4) +
                        STR_Hyphen +
                        pProduct->sOrderSerial;
         }
      //* End New logic:   D73493  HECC:  10/18/2002

      if ( iError )
         return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text, sInputFileName, sOutputFileName );
      pProduct->DestroyAllMes();
      delete pProduct;
   }

   // Seek EOF and write an AC record for each System
   sOutputCfr.seekp( 0, ios::end );
   if ( sOutputCfr.fail() )
     return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorOutputFileSeek_Text,
                                   sInputFileName, sOutputFileName );
   SystemCnr::Cursor SysCursor( mySystemSet );
   forCursor( SysCursor )
   {
      System& wkSystem = mySystemSet.elementAt( SysCursor );
      IString sAcRecord = wkSystem.CreateAcRecord();
      if ( WRITELINE(sOutputCfr, sAcRecord) )
         return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text, sInputFileName, sOutputFileName );
   }

//------------------

   myCfrStats.setCountSystems( mySystemSet.numberOfElements() - 1 );

   // Seek beginning of the file and write correct AA record
   sOutputCfr.seekp( 0 );
   if ( sOutputCfr.fail() )
     return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorOutputFileSeek_Text,
                                   sInputFileName, sOutputFileName );
   myCfrStats.CreateAaRecord(sAArecord);
   if ( WRITELINE(sOutputCfr, sAArecord) )
     return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, IDS_ErrorFileWrite_Text,
                                   sInputFileName, sOutputFileName );

   return Close_Cfrs_And_Return( sInputCfr, sOutputCfr, NO_ERROR, sInputFileName, sOutputFileName );
}


//  Recreate the product's AB record and update it in the output file
int UpdtAbRec( ostream& myStream, Product * pProdRec )
{
   IString        sAbRec;
   unsigned long  uOffset;
   pProdRec->CreateAbRecord( sAbRec );
   uOffset = pProdRec->uOffsetToProductRecord - pProdRec->uLengthAbRec;
   myStream.seekp( uOffset );
   if ( myStream.fail() )
     return IDS_ErrorOutputFileSeek_Text;
   if ( BackEnd::WRITELINE(myStream, sAbRec) )
     return IDS_ErrorFileWrite_Text;
   return NO_ERROR;
}


// This function is called by ISequence<Product *> to the sort two
// elements.  Following function uses TypeModel and Serial/order number
// to find the ordering.
long ProductSort( Product* const& pProd1, Product* const& pProd2 )
{
   if ( pProd1->typeModel() > pProd2->typeModel() )
     return 1;
   if ( pProd1->typeModel() < pProd2->typeModel() )
     return -1;
   if ( pProd1->order() > pProd2->order() )
     return 1;
   if ( pProd1->order() < pProd2->order() )
     return -1;
   return 0;
}


void PrintProds( ofstream& temp, ProductCnr& myProductSeq );
//-------------------------------------------------
void AssignProducts( SystemCnr&     mySystemSet,
                     ProductCnr&    myProductSeq,
                     unsigned int   uCpuCount,
                     IString        sFileName )
{
  // Create a system for unassigned Products
  System aSystem( 0 );
  aSystem.sSystemName = LOADSTRING( IDS_Unassigned );
  aSystem.bHasCpu = False;
  mySystemSet.add( aSystem );

  unsigned int uSystemId = 1;
  Product*     pProduct;

  ProductCnr::Cursor ProdCursor( myProductSeq );
  // If there are no CPUs in the CFREPORT, assign everything to Group 1
  if ( uCpuCount == 0 )
  {
     // Create System
     System aSystem( uSystemId );
     aSystem.sSystemName = LOADSTRING( IDS_MiscTab );
     aSystem.sSystemName.change( STR_Substitute0, uSystemId );
     aSystem.uSysProdCount = myProductSeq.numberOfElements();
     mySystemSet.add( aSystem );

     // Update the SystemId for each Product
     forCursor( ProdCursor )
     {
         pProduct = myProductSeq.elementAt( ProdCursor );
         pProduct->uCurrentSystemId = uSystemId;
     }
     return;
  }

  myProductSeq.sort( ProductSort );
  int iUnprocessed = 0;          // Number of Products remaining to be assigned

//=========================
//ofstream temp( sFileName + ".out");
//PrintProds( temp, myProductSeq);
//========================

  // Assign all the CPUs to Systems.  If HW does not have a System Type-Number, or
  // an Installed SW does not have Designated CPU Type-Serial, assign the Product to
  // System 0 ("Unassigned products")
  forCursor( ProdCursor )
  {
      pProduct = myProductSeq.elementAt( ProdCursor );
      if ( pProduct->category() == Product::CPU )
      {
         IString sCpuTypeSerial =   pProduct->sCurrentTypeModel.subString(1,4)
                                    + STR_Hyphen
                                    + pProduct->sOrderSerial;
         IString sOrigCpuTypeSerial = pProduct->sOriginalTypeModel.subString(1,4)
                                      + STR_Hyphen
                                      + pProduct->sOrderSerial;
         System aSystem( uSystemId,
                         sCpuTypeSerial,
                         sOrigCpuTypeSerial,
                         pProduct->systemTypeNumber(),
                         pProduct->customerNumber() );
         aSystem.uSysProdCount = 1;
         aSystem.bHasCpu = True;
         aSystem.sSystemName = LOADSTRING( IDS_SystemTab );
         aSystem.sSystemName.change( STR_Substitute0, IString(uSystemId) );
         mySystemSet.add( aSystem );
         pProduct->uCurrentSystemId = uSystemId;
         uSystemId++;
      }
      //* F86750 Begin logic:  Check also for the services product category  HECC - 01/15/2004
      else if (  (pProduct->category() == Product::HW && (pProduct->systemTypeNumber()).size() == 0)
              || (pProduct->category() == Product::SV && (pProduct->systemTypeNumber()).size() == 0) //* F86750
              || (pProduct->category() == Product::SW && (pProduct->sSwDesignatedCpuTypeSerial).size() == 0 &&
                  pProduct->sCurrentStatus == IString("INSTALLED"))
              )
      {
         System& aSystem = mySystemSet.elementWithKey( 0 );
         aSystem.uSysProdCount++;
         pProduct->uCurrentSystemId = 0;
      }
      else  // This Product is yet to be assigned
      {
         pProduct->uCurrentSystemId = UINT_MAX;
         iUnprocessed++;
      }
  }

  SystemCnr::Cursor SysCursor( mySystemSet );

  // Assign all the HW and SW to their CPUs
  forCursor( ProdCursor )
  {
      pProduct = myProductSeq.elementAt( ProdCursor );
      // Ignore assigned products
      if ( pProduct->uCurrentSystemId != UINT_MAX )
         continue;

      IBoolean bAssign = False;
      forCursor( SysCursor )
      {
         System& aSystem = mySystemSet.elementAt( SysCursor );
         // Go to next System if the customer number is different or
         // this System does not have a CPU
         // ZOR 66606A it just compare if it has CPU, the customerNumber is ignored
         //if ( pProduct->customerNumber() != aSystem.sCustomerNumber ||  //ZOR  66606A
           if ( aSystem.bHasCpu == False )
            continue;

         switch( pProduct->category() )
         {
            // Associate SW using DesignatedCpuTypeSerial.  If it is Initial Order
            // software, then it may not have a DesignatedCpuTypeSerial.  In that case,
            // use SystemTypeNumber to assign it.  If Installed SW does not have
            // a DesignatedCpuTypeSerial, the code in the loop which assigns CPUs to individual systems
            // assigns such SW to system 0 ("Unassigned products").
            case Product::SW:
                 if ( pProduct->sSwDesignatedCpuTypeSerial == aSystem.sOrigCpuTypeSerial
                      || pProduct->sSwDesignatedCpuTypeSerial == aSystem.sCpuTypeSerial )
                    bAssign = True;
                 else if ( (pProduct->sSwDesignatedCpuTypeSerial).size() == 0 &&
                            pProduct->systemTypeNumber() == aSystem.sSystemTypeNumber )
                    bAssign = True;
                 break;

            // Associate HW using SystemTypeNumber

            //* F86750 Begin logic: Used to associate the SV products using SystemTypeNumber HECC - 01/15/2004
            case Product::SV:    //* F86750
            case Product::HW:
                 if ( pProduct->systemTypeNumber() == aSystem.sSystemTypeNumber )
                    bAssign = True;
                 break;
         }

         if ( bAssign )
         {
            pProduct->uCurrentSystemId = aSystem.uSystemId;
            aSystem.uSysProdCount++;
            iUnprocessed--;
            break;
         }
      } // end..for each System
  } // end..for each Product


//=============================================
//   PrintProds( temp, myProductSeq);
//=============================================

  if ( iUnprocessed == 0 )  // Everything has been assigned
     return;

  // At this point the remaining Products could have the following conditions:
  //  (i) HW has unmatched System Type-Num and/or unmatched customer number
  // (ii) SW has unmatched CpuSerial/CustomerNumber/System Type-Num or all of these
  // The following code creates Systems for the remaining HW and SW Products.
  // The HW Products are grouped by SystemTypeNumber and customer number.
  // SW Products are grouped by Designated CpuTypeSerial and customer number.

  // The remaining HW and SW Products are added at the end of the
  // mySystemSet.  Therefore, the loop to search for a System
  // can start after the Systems with CPUs.
  unsigned int uCountSystems = mySystemSet.numberOfElements();

  forCursor( ProdCursor )
  {
      pProduct = myProductSeq.elementAt( ProdCursor );
      // Ignore assigned products
      if ( pProduct->uCurrentSystemId != UINT_MAX )
         continue;

      mySystemSet.locateElementWithKey( uCountSystems, SysCursor );
      IBoolean  bAssign = False;
      IBoolean  bCreateSys = True;
      for  ( ; SysCursor.isValid(); SysCursor.setToNext() )
      {
         System& aSystem = mySystemSet.elementAt( SysCursor );
         // 66606A the customer number won't be considerated
         // if ( pProduct->customerNumber() != aSystem.sCustomerNumber ) // ZOR 66606A
         //    continue;  ZOR 6606A

         switch ( pProduct->category() )
         {
            case Product::SW:
                ( pProduct->sSwDesignatedCpuTypeSerial == aSystem.sCpuTypeSerial ) ?
                bAssign = True : bAssign = False;
                break;
            case Product::HW:
                ( pProduct->systemTypeNumber() == aSystem.sSystemTypeNumber ) ?
                bAssign = True : bAssign = False;
                break;
         }
         if ( bAssign )
         {
            pProduct->uCurrentSystemId = aSystem.uSystemId;
            aSystem.uSysProdCount++;
            iUnprocessed--;
            bCreateSys = False;
            break;
         }
      } // end..for each System

      if ( bCreateSys )
      {
          IString sCpuTypeSerial, sSystemTypeNum;
          ( pProduct->category() == Product::SW ) ?
                  sCpuTypeSerial = pProduct->sSwDesignatedCpuTypeSerial :
                  sSystemTypeNum = pProduct->systemTypeNumber();

          pProduct->uCurrentSystemId = uSystemId;
          System aSystem( uSystemId,
                          sCpuTypeSerial,
                          sCpuTypeSerial,
                          sSystemTypeNum,
                          pProduct->customerNumber() );
          aSystem.uSysProdCount++;
          aSystem.sSystemName = LOADSTRING( IDS_MiscTab );
          aSystem.sSystemName.change( STR_Substitute0, uSystemId );
          mySystemSet.add( aSystem );
          uSystemId++;
      }
   } // for .. each Product

//---------------------------
//   PrintProds( temp, myProductSeq);
//---------------------------

}

void PrintProds( ofstream& temp, ProductCnr& myProductSeq )
{
IString sTemp=\
"                                      Cat   Sys";
  BackEnd::WRITELINE(temp, sTemp);
  sTemp = "===============================================================";
  BackEnd::WRITELINE(temp, sTemp);
  ProductCnr::Cursor ProdCursor( myProductSeq );
  forCursor( ProdCursor )
  {
      Product* pProduct = myProductSeq.elementAt( ProdCursor );
      IString  sSerOrd = pProduct->order();
      sSerOrd.rightJustify(9, ' ');
      IString scat;
      if ( pProduct->category() == Product::SW )
         scat = "SW ";
      else if ( pProduct->category() == Product::HW )
         scat = "HW ";
      else scat = "CPU";
      sTemp = pProduct->typeModel() + "   " + sSerOrd + "    "
                       + pProduct->systemTypeNumber() + "    "
                       + scat + "     "
                       + IString(pProduct->uCurrentSystemId) + "   "
                       + pProduct->sSwDesignatedCpuTypeSerial + "   "
                       + pProduct->customerNumber();
      BackEnd::WRITELINE(temp, sTemp);
  }
}

//  edw02
//  The WriteReconciliationComments function writes to the output file ( as 05
//  comment records ) any informational text that was captured during the Base
//  Retrieval reconciliation process.  The function returns the number of
//  comment records that were created and written to the output file.
int WriteReconciliationComments( MainWindow* pMainWindow, ostream& sOstream )
{
   // Only create/write new comments if it's a S390 configuration
   if( !pMainWindow->bS390 )
      return 0;

   IString strUserChoices = pMainWindow->getUserChoices();
   IString strMismatches = pMainWindow->getMismatches();
   IString strTemp, strCommentRec;
   int iAddCommentCnt = 0;

   while( strUserChoices.length() )
   {
      strTemp = strUserChoices.subString( 1, strUserChoices.indexOf("|") - 1 );
      if( strTemp.length() )
      {
         strCommentRec = STR_Rec05_Template;
         strCommentRec.change( STR_Substitute0, strTemp );
         if ( BackEnd::WRITELINE(sOstream, strCommentRec) )
            return IDS_ErrorFileWrite_Text;
         iAddCommentCnt ++;
         strUserChoices.remove( 1, strTemp.length() + 1 );
      }
      else
         break;
   }
   while( strMismatches.length() )
   {
      strTemp = strMismatches.subString( 1, strMismatches.indexOf("|") - 1 );
      if( strTemp.length() )
      {
         strCommentRec = STR_Rec05_Template;
         strCommentRec.change( STR_Substitute0, strTemp );
         if ( BackEnd::WRITELINE(sOstream, strCommentRec) )
            return IDS_ErrorFileWrite_Text;
         iAddCommentCnt ++;
         strMismatches.remove( 1, strTemp.length() + 1 );
      }
      else
         break;
   }
   return iAddCommentCnt;
}
