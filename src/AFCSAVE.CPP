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

typedef IElemPointer<System>  SystemPtr;
typedef IHeap<SystemPtr>      SystemCnr;

int ReOpenInput( IString, int, fstream& );
int UpdtAbRec( ostream&, unsigned long, IString );

////////////////////////////////////////////////////////////////////////////////
// Function: BackEnd::SaveCFREPORT()
//    Copies the current contents of the input file excluding the AC records
//    to the output file.  It then re-writes each of the AB records, adds an
//    AC record for each system object, and finally re-writes the AA record.
//    The input file is then closed.  It this is a normal save, the current
//    input file is deleted and the output file is renamed to the input file
//    name.  If its a "save as", the input file is retained and the new
//    output file is opened as input.
//
// Overall logic:
//    1. Get the current file's path and name.
//    2. Create a new CfrStats object.
//    3. If this is a normal save, generate a unique file name for the output
//       file.  Otherwise copy the current object's contents into the new
//       CfrStats object created in step 2 and use the new file name for the
//       output file.
//    4. Re-seek the input file to the beginning and open the output file.
//    5. Copy the entire input file up to (and including) the 98 trailer
//       record, to the output file.
//    6. Examine each MES object in the MES container and update its
//       corresponding AB record in the output file.
//    7. Examine each unapplied Order object in the New Orders container and
//       update its corresponding AB record in the output file.
//    8. For each system in the system containers:
//       a. Update the AB record in the output file for each product in the
//          system.
//       b. Seek to the end of the output file and write an AC record.
//    9. Update the CfrStats object with the new statistics, then re-seek the
//       beginning of the file and re-write the AA record.
//   10. Close the output file.
//   11. If a normal save was requested:
//       a. Close the input file and rename it to a unique temporary name.
//       b. Rename the output file to the input file's original name.
//       c. Delete the old input file (using its temporary file name).
//       d. Open the new input file
//       Otherwise it must be a "save as" request
//       a. Close the input file.
//       b. Open the output file as an input file
//       c. Change the name in the current CfrStats object to reflect the new
//          file name.
//       d. Add the new CfrStats object created in step 2 to represent the
//          old input file.
//
//******************************************************************************
// Revision Key:
//   VDT --- Von D. Tucker           (VNRTC370 at RHQVM22)
//
// Revision Notes:
//   Rev #   Flag   Who     Date                    Description
//   1.00           VDT   09/30/96   Initial creation
//
////////////////////////////////////////////////////////////////////////////////

int BackEnd::SaveCFREPORT()
{

   IBoolean bAllRead    = False;              /* Set when EOF on input file*/
   IString sLastRecType = STR_DummyRecType;
   unsigned int
     uAppliedMesCnt = 0,
     uAppliedNewCnt = 0,
     uProductCnt    = 0,
     uRemovedMesCnt = 0,
     uRemovedNewCnt = 0,
     uUnappliedCnt  = 0,
     uUnassignedCnt = 0;
   int            iError;
   IString        sAArecord, sABrecord, sACrecord, sFileName, sFullFileName,
                  sInputCfrRec, sOutputFileName;
   unsigned int   uSystemId;
   unsigned long  uRecOffset;
   Mes*           pMes;
   Product*       pProduct;

   sFileName      = pMainWindow->pFile->sCfrFileName;
   sFullFileName  = pMainWindow->pFile->sCfrFullFileName;
   // Create new CfReportFileStats in case we need it
   CfReportFileStats *pNewCfrStats = new CfReportFileStats( sFileName );

   if ( sFileName == pMainWindow->strSaveName )
     sOutputFileName = GetTempFileName( sFullFileName );
   else
   {
     sOutputFileName = sFullFileName;
     sOutputFileName.change( sFileName, pMainWindow->strSaveName );
     pMainWindow->pFile->CreateAaRecord( sAArecord );
     pNewCfrStats->sCfrFullFileName = sFullFileName;
     pNewCfrStats->ParseAaRecord( sAArecord );
   }

//   Re-seek the input file to the beginning and open the output file
   fstream& sInputCfr = pMainWindow->pFile->CfrFileStream;
   sInputCfr.seekg( 0, ios::beg );
   if ( sInputCfr.fail() )
   {
     sInputCfr.close();
     int iError = OpenFile ( sFullFileName, BackEnd::READ, sInputCfr );
     if ( iError )
       return iError;
   }
   ofstream sOutputCfr( sOutputFileName );
   if ( sOutputCfr.fail() )
     return IDS_ErrorOutputFileOpen_Text;

//   First, copy the current input file (up to and including the 98 trailer
//   record) to a new, temporary file
   while ( ! bAllRead )
   {                                   /* For each record in the input file*/
     sInputCfrRec = IString::lineFrom( sInputCfr );
     if ( sInputCfr.fail() )
       if ( sInputCfr.eof() )
         CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorUnexpectedEof_Text )
       else
         CLOSE_INPUT_OUTPUT_CFRS( sInputCfr, sOutputCfr, IDS_ErrorFileRead_Text,
                                             sFullFileName )
     if ( sInputCfrRec.subString(1,2) == STR_AaRecType )
       unsigned long uAAilen = sizeof(sInputCfrRec);
     if ( WRITELINE(sOutputCfr, sInputCfrRec) )
       CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileWrite_Text )
     if ( sInputCfrRec.subString(1,2) == STR_98RecType )
       bAllRead = True;
   }                           /* End copying each record in the input file*/

//  Second, update the AB record for each MES
   IContainerControl *pcnrMes = pMainWindow->pOrders->pcnrOrders[ORDERSMES];
   IContainerControl::ObjectCursor mcrs(*pcnrMes);
   forCursor ( mcrs )
   {                                        /* Loop through all MES objects*/
     Mes* pMes = (Mes*)mcrs.current();
     if ( pMes->applied() )
       uAppliedMesCnt ++;
     else
     {
       uRemovedMesCnt ++;
       uUnappliedCnt ++;
     }
     pMes->CreateAbRecord( sABrecord );
     uRecOffset = pMes->uOffsetToProductRecord - pMes->uLengthAbRec;
     // Seek the wkProduct's AB record and overwrite it
     iError = UpdtAbRec( sOutputCfr, uRecOffset, sABrecord );
     if ( iError )
       CLOSE_OUTPUT_CFR( sOutputCfr, iError )
   }                                 /* End looping through all MES objects*/


//  Third, update the AB record for each unapplied order in the container
   IContainerControl *pcnrOrders = pMainWindow->pOrders->pcnrOrders[ORDERSNEW];
   IContainerControl::ObjectCursor ocrs(*pcnrOrders);
   forCursor ( ocrs )
   {
//   ProductObject* pProduct = (ProductObject*)ocrs.current();
     Product* pProduct = (Product*)ocrs.current();
     if ( pProduct->applied() )
       uAppliedNewCnt ++;
     else
     {
       uRemovedNewCnt ++;
       uUnappliedCnt ++;
       pProduct->CreateAbRecord( sABrecord );
       uRecOffset = pProduct->uOffsetToProductRecord - pProduct->uLengthAbRec;
       // Seek the Product's AB record and overwrite it
       iError = UpdtAbRec( sOutputCfr, uRecOffset, sABrecord );
       if ( iError )
         CLOSE_OUTPUT_CFR( sOutputCfr, iError )
     }
   }                          /* End looping through all new order products*/


//  Fourth, loop through all of the system objects and update the AB record
//  for each product assigned to the system.  Then update the AC record and
//  write it at the end of the file.
   SystemCnr mySystemSet;
   System mySystemT(0);
   IContainerControl **pcnrSystems = pMainWindow->pSystems->pcnrProducts;
   for ( USHORT usSysId = 0;
         usSysId < pMainWindow->pSystems->usSystems;
         usSysId ++ )
   {                             /* Write AC records for each system object*/
     uProductCnt = 0;
     IContainerControl *pcnrProducts = pcnrSystems[usSysId];
     IContainerControl::ObjectCursor pcrs(*pcnrProducts);
     forCursor ( pcrs )
     {
       Product* pProduct = (Product*)pcrs.current();
       if ( usSysId == 0 )
         uUnassignedCnt ++;
       uProductCnt ++;
       pProduct->uCurrentSystemId = usSysId;
       pProduct->CreateAbRecord( sABrecord );
       uRecOffset = pProduct->uOffsetToProductRecord - pProduct->uLengthAbRec;
       // Seek the Product's AB record and overwrite it
       iError = UpdtAbRec( sOutputCfr, uRecOffset, sABrecord );
       if ( iError )
         CLOSE_OUTPUT_CFR( sOutputCfr, iError )
     }                           /* End looping through the product objects*/
     sOutputCfr.seekp( 0, ios::end );
     if ( sOutputCfr.fail() )
       CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorOutputFileSeek_Text )
     mySystemT.uSystemId = usSysId;
     mySystemT.uSysProdCount = uProductCnt;
     if ( usSysId > 0 )
       mySystemT.sSystemName = pMainWindow->pSystems->systemName(usSysId);
     else
       mySystemT.sSystemName = LOADSTRING(IDS_Unassigned);
     sACrecord = mySystemT.CreateAcRecord( );
     if ( WRITELINE(sOutputCfr, sACrecord) )
       CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileWrite_Text )
   }                              /* End looping through all system objects*/


//  Fifth, update the CfrStats, seek back to the beginning of the file and
//  write out the real AA record
   pMainWindow->pFile->setCountSystems( pMainWindow->pSystems->usSystems - 1 );
   pMainWindow->pFile->setCountUnapplied( uUnappliedCnt );
   pMainWindow->pFile->setCountUnassigned( uUnassignedCnt );
   pMainWindow->pFile->uCountAppliedNew = uAppliedNewCnt;
   pMainWindow->pFile->uCountRemovedNew = uRemovedNewCnt;
   pMainWindow->pFile->uCountAppliedMes = uAppliedMesCnt;
   pMainWindow->pFile->uCountRemovedMes = uRemovedMesCnt;
   pMainWindow->pFile->CreateAaRecord(sAArecord);
   unsigned long uAAolen = sizeof(sAArecord);
   sOutputCfr.seekp( 0, ios::beg );
   if ( sOutputCfr.fail() )
     CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorOutputFileSeek_Text )
   if ( WRITELINE(sOutputCfr, sAArecord) )
     CLOSE_OUTPUT_CFR( sOutputCfr, IDS_ErrorFileWrite_Text )

//  Sixth, close the input and output files, rename the output file to the
//  name specified by the user, then open that file as the new input file.
   sOutputCfr.close();
   if ( sFileName == pMainWindow->strSaveName )       /* If using same name*/
   {                                      /* Perform normal save processing*/
     IString sTempFileName = GetTempFileName( sFullFileName );
     sInputCfr.close();

     //  Rename the input file to a temp file name
     if ( rename( (char *) sFullFileName, (char *) sTempFileName ) != 0 )
     {                          /* Perform rename input file error recovery*/
       remove( (char *) sOutputFileName );
       return ReOpenInput( sFullFileName, IDS_ErrorFileRename_Text, sInputCfr );
     }                              /* End rename input file error recovery*/

     //  Rename the output file to the original input file name
     if ( rename( (char *) sOutputFileName, (char *) sFullFileName) != 0 )
     {                         /* Perform rename output file error recovery*/
       remove( (char *) sOutputFileName );
       rename( (char *) sTempFileName, (char *) sFullFileName );
       return ReOpenInput( sFullFileName, IDS_ErrorFileRename_Text, sInputCfr );
     }                             /* End rename output file error recovery*/
     else                           /* Both files were successfully renamed*/
     {                            /* Delete old input file and open new one*/
       remove( (char *) sTempFileName );
       return ReOpenInput( sFullFileName, NO_ERROR, sInputCfr );
     }                        /* End delete old input file and open new one*/
   }                                          /* End normal save processing*/
   else                                /* Else user has given us a new name*/
   {                                        /* Perform "save as" processing*/
     //  Close the current input file and open the output file as input.
     //  Change the file names in the current CfrStats object to the new
     //  names so the currently selected object becomes the new one, and
     //  add the previously created copy of the original CfrStats object
     //  to the end of the filelist container.
     sInputCfr.close();
     if ( ReOpenInput( sOutputFileName, NO_ERROR, sInputCfr ) )
       return IDS_ErrorInputFileOpen_Text;
     pMainWindow->pFile->sCfrFileName = pMainWindow->strSaveName;
     pMainWindow->pFile->sCfrFullFileName = sOutputFileName;
     pMainWindow->pcnrFiles->addObject( pNewCfrStats );
   }                                            /* End "save as" processing*/
   return 0;
}


//  Re-open the input file
int ReOpenInput( IString sOpenFileName, int iRetCode, fstream& CfrStream )
{
   int iError = BackEnd::OpenFile( sOpenFileName, BackEnd::READ, CfrStream );
   if ( iError )
     return iError;
   else
     return iRetCode;
}


//  Recreate the product's AB record and update it in the output file
int UpdtAbRec( ostream& myStream, unsigned long ulOffset, IString sAbRec )
{
   myStream.seekp( ulOffset );
   if ( myStream.fail() )
     return IDS_ErrorOutputFileSeek_Text;
   if ( BackEnd::WRITELINE(myStream, sAbRec) )
     return IDS_ErrorFileWrite_Text;
   return NO_ERROR;
}
