#include "AFCBKEND.HPP"
#include "AFCCFRST.HPP"
#include "AFCPRODS.HPP"
#include "AFCMCROS.H"
#include "AFCFEAT.HPP"

#include "AF9MAIN.HPP"
#include "AF9ORDRS.HPP"
#include "AF9SYSTM.HPP"
#include "AF9APPLY.H"

typedef IElemPointer<Mes> MesPtr;
typedef IElemPointer<Product> ProductPtr;

Boolean  F05BIR(char *FileName) ;
char *BIR[4000];


////////////////////////////////////////////////////////////////////////////////
// Function: BackEnd::LoadCFREPORT()
//    Parses, partially validates, and loads the content of a pre-processed
//    and formatted CFREPORT into the appropriate containers for display and
//    manipulation by Order/Apply.  This function is called by AFxMAIN.
//
// Overall logic:
//    1. For an initial load, open the specified input file.  For a re-load,           enerated
//       just seek to the beginning of the file.
//    2. Read each line in the input file
//       a. If the record type is AA, update the CfrStats.
//       b. If the record type is 00, 03, 07, or 01 validate it's position in
//          the file, but otherwise ignore it.
//       c. If the record type is 96 and a product or an MES record was just
//          processed that set the feature count and that number of feature
//          records have not yet been read, load the feature into the last
//       a. If the record type is AB, save it for loading with the product or
//          Mes.
//       d. If the record type is 08, 25, 47, or 49, read the next record and
//          ensure it is a type 95 record, then create a product object.  From
//          the product object, create an AB control record and write it to
//          the output file, then copy the product record and its associated
//          95 record to the output file.
//       e. If the record type is 19 or 50, read the next record and ensure
//          it is a type 95 record.  If it is a hardware upgrade MES, read
//          the next record and ensure it is a type 95 record also.  Create
//          an MES object, and from it create an AB control record.  Write
//          the AB record to the output file and copy the MES record and its
//          associated 95 record(s) to the output file.
//       f. When all records have been read and processed, create two system
//          objects, then loop through all of the product objects, creating
//          additional system objects as required.  On completion:
//          1. If there are still only two system objects, do nothing.
//          2. If there are three system objects, copy the contents of system
//             object 3 to 2 and delete system object 3.  Then loop through
//             all of the product objects and assign them all to system 2.
//          3. If there are more than three system objects, delete system
//             object 2 and loop through all of the higher system objects
//             decrementing their identifiers by 1.  Then loop through
//             all of the product objects and reassign those assigned to
//             system 2 to system 1 and decrementing all others by 1.
//       g. If action 2 or 3 above was taken, loop through the product
//          objects and recreate and rewrite their associated AB records.
//       h. Loop through the system objects, creating and writing AC control
//          records for each to the output file.
//       i. Create the real AA control record and over write it in the file.
//       j. Close both files, erase the original input file, and rename the
//          output file to the input file's name.
//       a. If the record type is AC, update the CfrStats.
//    3. When the entire CFREPORT has been loaded, loop through all of the
//       MES's in the MES container and set the MES group information.
//
//******************************************************************************
// Revision Key:
//   VDT --- Von D. Tucker           (VNRTC370 at RHQVM22)
//   EDW --- Erica D. Wilson         (EDWILSON@US.IBM.COM)
//
// Revision Notes:
//   Rev #   Flag   Who     Date                    Description
//   1.00           VDT   09/30/96   Initial creation
//
//           edw1   EDW   05/05/98   Added logic to process 54, 55 and 56 record types
//   1.50    edw02  EDW   06/05/1998 Allow processing of multiple systems
//   D95887         ZOR   10/25/2004 load 53 record right after the 07 record
//   D96220         HECC  11/10/2004 Modify logic to validate the case when the RT 53
//                                   are not present in the configuration.
//   D96389         HECC  11/18/2004 Fixes the correctly copy of the records quantity
//                                   when it is 6 characters long.
//  D95997          ZOR   19/11/2004 include non INV 06 records in the RET file
//  D96549          HECC  11/29/2004 Disable fix for defect 96389
//
////////////////////////////////////////////////////////////////////////////////

int BackEnd::LoadCFREPORT(IBoolean bReLoad)
{

   IBoolean
     bAbValid      = False,                   /* Set when an AB record is processed*/
     bAllRead      = False,                           /* Set when EOF on input file*/
     bHdrProc      = False,                       /* Set when all headers processed*/
     bMesLast      = False,                  /* Set when an Mes record has features*/
     bMesUpgrade   = False,                             /* Set if MES is an upgrade*/
     bTrlProc      = False,                    /* Set when trailer record processed*/
     bFirst54      = False;  /* edw1-Set when 1st 54 record processed for a product*/

   IString         sABrecord, sInputCfrRec, sLastProdRec, sProductIdRec1,
                   sProductIdRec, sRecCnt, sSysName, sRecType;
   IString
     sLastRecType = STR_DummyRecType,
     sMesTypes    = STR_MesRecTypes,
     sOrderTypes  = STR_OrderRecTypes,
     sCtlTypes    = STR_CtlRecTypes,
     sProdTypes   = STR_ProductRecTypes,
     sValidTypes  = STR_ValidRecTypes;


   long int
     iFeatCnt = 0,
     iGroupId = 0,
     iRecCnt  = 0;
   long int        iConvertError, iTotalMes;
   unsigned long   uFeatOffset, uRecOffset, uIdRecOffset, uSystemId, uSystems,
                   ulRec54Offset;                                    /*edw1*/
   uOffsetToInfoRecs = 0;                                   /*edw01/03/2000*/

   MesPtr          pMes;
   ProductPtr      pProduct, pLastProduct;




   System mySystem(0);
   uSystems = pMainWindow->pFile->Systems();

//search for 05 records in the BIR file
   F05BIR(pMainWindow->pFile->sCfrFullFileName); //erick tst

   fstream& sInputCfr = pMainWindow->pFile->CfrFileStream;

//  Open or re-seek the input file based on the input parameter
   if (bReLoad == True)
   {
     sInputCfr.seekg( 0, ios::beg );
     if ( sInputCfr.fail() )
     {
       sInputCfr.close();
       int iError = OpenFile ( pMainWindow->pFile->sCfrFullFileName, BackEnd::READ, sInputCfr );
       if ( iError )
         return iError;
     }
   }
   else
   {
     //  If this file was last processed by the previous version of this
     //  application, convert it prior to loading with this version.
     if ( pMainWindow->pFile->sAppVersion == STR_SupportedAppVersion )
     {
       CfReportFileStats* pCfrStats = pMainWindow->pFile;
       iConvertError = ConvertPreviousCFREPORT( *pCfrStats );
       if ( iConvertError )
         return iConvertError;
     }

     int iError = OpenFile ( pMainWindow->pFile->sCfrFullFileName, BackEnd::READ, sInputCfr );
     if ( iError )
        return iError;
   }

 if ( !placementRecSet.isEmpty() ) placementRecSet.removeAll();
 if ( !connectionRecSet.isEmpty() ) connectionRecSet.removeAll();

//  Read all of the records in the input file and process them.  Only minimal
//  checking and validation need to be done on the file since it was originally
//  validated by AFCPARSE.
   while ( ! bAllRead )
   {                                   /* For each record in the input file*/
     sInputCfrRec = IString::lineFrom( sInputCfr );
     if ( sInputCfr.fail() )
       if ( sInputCfr.eof() )
       {
         bAllRead = True;
         continue;
       }
       else
         CLOSE_INPUT_CFR( sInputCfr, IDS_ErrorFileRead_Text)

     sRecType = sInputCfrRec.subString(1,2);
     if ( sValidTypes.indexOf(sRecType) == 0 )
       CLOSE_INPUT_CFR( sInputCfr, IDS_ErrorUnknownCfrRec_Text )

     //  Count all non-control records to ensure record count wasn't changed
     if ( sCtlTypes.indexOf(sRecType) == 0 )
       iRecCnt ++;

     //  First, process and validate the order of the CFREPORT header records.
     //  bHdrProc will remain false until all header records have been read.
     if ( bHdrProc == False )
     {                                        /* Process the header records*/
       if ( sRecType == STR_AaRecType && sLastRecType == STR_DummyRecType )
       {
         sLastRecType = sRecType;
         pMainWindow->pFile->ParseAaRecord( sInputCfrRec );
       }
       else                      /* Process the CFreport header (00) record*/
         if ( sRecType == STR_00RecType && sLastRecType == STR_AaRecType )
           sLastRecType = sRecType;
         else
           if ( sRecType == STR_03RecType && sLastRecType == STR_00RecType )
             sLastRecType = sRecType;
           else
             if ( sRecType == STR_07RecType && sLastRecType == STR_03RecType )
              {
               sLastRecType = sRecType;
               uOffsetTo53Recs = sInputCfr.tellg();   //95887     load the 53 record position
              }

            /**147293 Begin***/
           else
             if ( ( sRecType == STR_05RecType && sLastRecType == STR_07RecType ) ||
                  ( sRecType == STR_05RecType && sLastRecType == STR_05RecType ) ||
                  ( sRecType == STR_05RecType && sLastRecType == STR_53RecType )  )
                {

                 }

            /**147293 end**/


              else
               if ( sRecType == STR_53RecType && sLastRecType == STR_07RecType )    // ZOR 95887
                 sLastRecType = sRecType;
               else
                 if ( ( sRecType == STR_01RecType && sLastRecType == STR_53RecType ) ||  // ZOR 95887
                      ( sRecType == STR_01RecType && sLastRecType == STR_07RecType ) ||
                      ( sRecType == STR_01RecType && sLastRecType == STR_05RecType )   ) // D96220:  HECC - 11/10/2004
                 {
                 bHdrProc = True;
                 uOffsetToInfoRecs = sInputCfr.tellg();     /*edw01/03/2000*/
                 }
               else
                 CLOSE_INPUT_CFR( sInputCfr, IDS_ErrorInvalidCfreport_Text )
       continue;
     }                                   /* End of header record processing*/

     //  Now that the header records have been successfully processed, read
     //  and process all of the CFREPORT body records (products and features).
     //  Feature records are checked first because they are more numerous.
     //  bTrlProc will remain false until a type 98 record is read.
     if ( bTrlProc == False )
     {                                 /* Process the CFreport body records*/
       if ( sRecType == STR_54RecType)  /* edw1-If 54, set 54 record offset*/
          if ( bFirst54 )
             {
             if ( bMesLast )
                pMes->setRec54Offset( ulRec54Offset );
             else
                pProduct->setRec54Offset( ulRec54Offset );
             bFirst54 = False;
             }
          else
             continue;
                                         /* edw1-If it's a placement record*/
       else if ( sRecType == STR_55RecType )
          try
          {
             placementRecSet.add(sInputCfrRec);
          }
          catch ( IOutOfMemory )
          {
             CLOSE_INPUT_CFR( sInputCfr, IDS_ErrorOutOfMem_Text )
          }
                                         /* edw1-If it's a connection record*/
       else if ( sRecType == STR_56RecType )
          try
          {
             connectionRecSet.add(sInputCfrRec);
          }
          catch ( IOutOfMemory )
          {
             CLOSE_INPUT_CFR( sInputCfr, IDS_ErrorOutOfMem_Text )
          }
                             /* edw02 - Allow processing of multiple systems*/
       else if ( sRecType == STR_06RecType )     // ZOR    95997  do not load these records in the product windows
           continue;

       else if ( sRecType == STR_07RecType )
          continue;

       else if ( sRecType == STR_53RecType )   // ZOR 95887
          continue;
                             /* edw02 - Allow processing of multiple systems*/
       else if ( sRecType == STR_01RecType )
          continue;

       else if ( sRecType == STR_05RecType )               /* edw01/03/2000 */
          continue;

       else if ( sRecType == STR_96RecType )     /* If it's a feature record*/
         if ( iFeatCnt > 0 )
         {                                /* Process a valid feature records*/
           iFeatCnt --;
           bAbValid = False;
           if ( bMesLast )
             pMes->AddFeatureDescription( sInputCfrRec, uFeatOffset );
           else
             pProduct->AddFeatureDescription( sInputCfrRec, uFeatOffset );
           uFeatOffset = sInputCfr.tellg();
           ulRec54Offset = uFeatOffset;                             /* edw1 */
         }                          /* End processing a valid feature record*/
         else
           CLOSE_INPUT_CFR( sInputCfr, IDS_ErrorInvalidCfreport_Text )

       else                                /* Else it's NOT a feature record*/

         if ( sRecType == STR_AbRecType )/* If it's a product control record*/
         {                             /* Process the product control record*/
           bAbValid = True;
           sABrecord = sInputCfrRec;
           uRecOffset = sInputCfr.tellg();
         }                      /* End processing the product control record*/

         else                      /* Else it's NOT a product control record*/

           if ( bAbValid && iFeatCnt == 0 && sProdTypes.indexOf(sRecType) > 0 )
           //  If it's a product record that was preceded by an AB record and
           //  there are no outstanding feature records left to be processed
           //  from the previous product or Mes record then
           {         /* Process the product (08, 25, 47, or 49) record types*/
             bAbValid = bMesLast = False;
             uIdRecOffset = sInputCfr.tellg();
             sProductIdRec = IString::lineFrom( sInputCfr );
             if ( sInputCfr.fail() )
               CLOSE_INPUT_CFR( sInputCfr, IDS_ErrorFileRead_Text )
             if ( sInputCfr.eof() )
               CLOSE_INPUT_CFR( sInputCfr, IDS_ErrorInvalidCfreport_Text )
             if ( sProductIdRec.subString(1,2) != STR_95RecType )
               CLOSE_INPUT_CFR( sInputCfr, IDS_ErrorInvalidCfreport_Text )
             sLastProdRec = sInputCfrRec;
             iRecCnt ++;

             //  Create a product object and initialize it
             pProduct=new Product;
             pProduct->ParseAbRecord( sABrecord );
             pProduct->ParseLoad( 'L', sInputCfrRec, sProductIdRec,
                                      uRecOffset, uIdRecOffset,  myCpuList );
             iFeatCnt = pProduct->myFeatureSet.numberOfElements();
             uSystemId = pProduct->uCurrentSystemId;

             if ( uSystemId > uSystems || uSystemId < 0 )
               CLOSE_INPUT_CFR( sInputCfr, IDS_ErrorInvalidCfreport_Text )
             uFeatOffset = sInputCfr.tellg();
             ulRec54Offset = uFeatOffset;                            /*edw1*/
             bFirst54 = True;                                        /*edw1*/
             if ( sOrderTypes.indexOf(sRecType) )
             // If an initial order record, add it to the order container
             // and if it has been applied, also add it to the appropriate
             // system container based on it's SystemId value.  Otherwise
             // order, just add it to the appropriate system container.
             {
               pMainWindow->pOrders->pcnrOrders[ORDERSNEW]->addObject(pProduct);
               if ( pProduct->applied() )
                 pMainWindow->pSystems->pcnrProducts[uSystemId]->addObject(pProduct);
             }
             else
               pMainWindow->pSystems->pcnrProducts[uSystemId]->addObject(pProduct);
           }                            /* End of product record processing*/

           else                     /* Else it's not a valid product record*/

             if ( bAbValid && iFeatCnt == 0 && sMesTypes.indexOf(sRecType) )
             //  If it's an Mes record that was preceded by an AB record and
             //  there are no outstanding feature records left to be processed
             //  from the previous product or Mes record then
             {                   /* Process the MES (19 or 50) record types*/
               bAbValid = bMesUpgrade = False;
               uIdRecOffset = sInputCfr.tellg();
               sProductIdRec = IString::lineFrom( sInputCfr );
               if ( sInputCfr.fail() )
                 CLOSE_INPUT_CFR( sInputCfr, IDS_ErrorFileRead_Text )
               if ( sInputCfr.eof() )
                 CLOSE_INPUT_CFR( sInputCfr, IDS_ErrorInvalidCfreport_Text )
               if ( sProductIdRec.subString(1,2) != STR_95RecType )
                 CLOSE_INPUT_CFR( sInputCfr, IDS_ErrorInvalidCfreport_Text )
               iRecCnt ++;
               if ( sRecType == STR_19RecType )
               {                                     /* It's a hardware MES*/
                 if ( ! sInputCfrRec.subString(10,7).isWhiteSpace() )
                 {                                   /* It's an upgrade MES*/
                   sProductIdRec1 = sProductIdRec;
                   uIdRecOffset = sInputCfr.tellg();
                   sProductIdRec = IString::lineFrom( sInputCfr );
                   if ( sInputCfr.fail() )
                     CLOSE_INPUT_CFR( sInputCfr, IDS_ErrorFileRead_Text )
                   if ( sInputCfr.eof() )
                     CLOSE_INPUT_CFR( sInputCfr, IDS_ErrorInvalidCfreport_Text )
                   if ( sProductIdRec.subString(1,2) != STR_95RecType )
                     CLOSE_INPUT_CFR( sInputCfr, IDS_ErrorInvalidCfreport_Text )
                   else
                     bMesUpgrade = True;
                   iRecCnt ++;
                 }                            /* End upgrade MES processing*/
               }                             /* End hardware MES processing*/

               //   Create an MES object and initialize it
               pMes=new Mes;
               pMes->ParseAbRecord( sABrecord );
               pMes->ParseLoad( 'L', sInputCfrRec, sProductIdRec,
                                       uRecOffset, uIdRecOffset,  myCpuList );

               pMes->pBaseProduct = pProduct;
               pProduct->AssociateMes( pMes );
               uFeatOffset = sInputCfr.tellg();
               ulRec54Offset = uFeatOffset;                          /*edw1*/
               bFirst54 = True;                                      /*edw1*/
               iFeatCnt = pMes->myFeatureSet.numberOfElements();
               pMes->uOffsetBaseProductRec = pProduct->uOffsetToProductRecord;
               bMesLast = True;

               // Add to the MES container
               pMainWindow->pOrders->pcnrOrders[ORDERSMES]->addObject(pMes);
             }                              /* End of MES record processing*/

             else                            /* Else it's not an Mes record*/

               if ( ! bAbValid && sRecType == STR_98RecType &&
                    iFeatCnt == 0 )
               //  If it's a trailer record that was NOT preceded by an AB
               //  record and there are no outstanding feature records left
               //  to be processed from the previous product or Mes record then
               {                     /* Process the CFreport trailer record*/

                 //* D96389: Begin logic:  Fixes the correctly copy of the records quantity
                 //*         when it is 6 characters long.
                 //* sRecCnt = sInputCfrRec.subString(3,5);

                 //* D96549 Disable fix for defect 96389   HECC   11/29/2004
                 //* sRecCnt = sInputCfrRec.subString(3,6);
                 sRecCnt = sInputCfrRec.subString(3,5);

                 //* D96389: End logic   HECC - 11/18/2004

                 if ( iRecCnt == sRecCnt.asInt() )
                   bTrlProc = True;
                 else
                   CLOSE_INPUT_CFR( sInputCfr, IDS_ErrorInvalidCfreport_Text )
               }               /* End of CFreport trailer record processing*/

               else                  /* Else it's an unexpected record type*/
                 CLOSE_INPUT_CFR( sInputCfr, IDS_ErrorInvalidCfreport_Text )
       continue;
     }                            /* End of CFreport body record processing*/

     //  Now that all of the CFREPORT body records (including the trailer
     //  record) have been successfully processed, read and process all of
     //  the AC control records.
     if ( bTrlProc )
       if ( sRecType == STR_AcRecType )
       {                                   /* Process the AC control record*/
         mySystem.ParseAcRecord( sInputCfrRec );
         if ( mySystem.uSystemId > 0 )
           //  Update the system name on the notebook tab
           pMainWindow->pSystems->setPage(mySystem.uSystemId,
                                          mySystem.uSystemId,
                                          mySystem.sSystemName);
       }                             /* End of AC control record processing*/

       else                          /* Else it's an unexpected record type*/
         CLOSE_INPUT_CFR( sInputCfr, IDS_ErrorInvalidCfreport_Text )

   }                                           /* End input file processing*/


   //  Now that the entire CFREPORT has been loaded, loop through the MES
   //  container and set the group information for those MESs that are
   //  grouped.
   IContainerControl *pcnrMes = pMainWindow->pOrders->pcnrOrders[ORDERSMES];
   IContainerControl::ObjectCursor mcrs(*pcnrMes);
   forCursor ( mcrs )
   {                                        /* Loop through all MES objects*/
     Mes* pMes = (Mes*)mcrs.current();
     if ( pMes->pBaseProduct == pLastProduct )
     {
       pMes->sMesGroupSequence = STR_GroupMES;
       pMes->sMesGroupSequence.change( STR_Substitute0, IString(iGroupId));
       pMes->sMesGroupSequence.change( STR_Substitute1, IString(pMes->uSequence) );
       pMes->sMesGroupSequence.change( STR_Substitute2, IString(iTotalMes));
     }
     else
     {
       pLastProduct = pMes->pBaseProduct;
       iTotalMes = pLastProduct->myMesList.numberOfElements();
       if ( iTotalMes > 1 )
       {
         iGroupId ++;
         pMes->sMesGroupSequence = STR_GroupMES;
         pMes->sMesGroupSequence.change( STR_Substitute0, IString(iGroupId));
         pMes->sMesGroupSequence.change( STR_Substitute1, IString(pMes->uSequence) );
         pMes->sMesGroupSequence.change( STR_Substitute2, IString(iTotalMes));
       }
       else
         pMes->sMesGroupSequence = STR_Blank1;
     }
   }
   return 0;
}



//=======================================================================
// DEFECT 136853:   New Function.
// FUNCTION NAME   UpdateBIR(char *FileName)
//
// DESCRIPTION     Rearrange the 05 records in the BIR to acomplish
//                 the validation logic.
//
//
//                 ELRR: 02/06/2013
//=======================================================================
///////////////////////////////////////////////////////////////////
Boolean  F05BIR(char *FileName)
{

char buf[3000];
char *BIR05off[3000];
char *BIR05on[3000];

int Count=0;int Count2=0;
int i=0,j=0,rec05=0;
int TotalRecs = 0;
IString is, ix;
int BIRfree05lenght=0;
int First1=0;



// Open the BIR file
//
FILE * pFile;
  pFile = fopen (FileName,"r");
  if (pFile==NULL)
  {
//  MessageBox(0,FileName,"Error opening file",0);
   return FALSE;
  }


// Read All rows from the Text file into Array BIR[] original BIR
//
while( fgets(buf,3000,pFile) )
 {
 BIR[Count] =  (char *) malloc(3000);
 strcpy(BIR[Count],buf);
 Count++;
 }
fclose(pFile);

ix=Count;


// Create BIR witout 05 records
j=0;

for(i=0;i<Count;i++)
{
 ix=BIR[i];
 if(ix.subString(1,2)!="05")
  {
   BIR05off[j]=(char *) malloc(3000);
   strcpy(BIR05off[j],ix);
   j++;
  }

 if(ix.subString(1,2)=="05")
 { BIR05on[rec05]=(char *) malloc(3000);
   strcpy(BIR05on[rec05],ix);
   rec05++;
 }

 }//end For


TotalRecs= Count+rec05;



  FILE * pBIRfree05;
  pBIRfree05 = fopen ("C:/econfig/UserData/test.txt","w");
  if (pBIRfree05==NULL)
  {
//    MessageBox(0,"test.txt","Error opening file",0);
   return FALSE;
  }


// we will search for the 01 ans append all 05 ecordas after that
for (i=0;i<TotalRecs;i++)
{
ix=BIR05off[i];


// si no es 01 se agrega
if(ix.subString(1,2)!="01")
{
fputs(BIR05off[i],pBIRfree05);
}
// si es 01 se busca si es el primero para agregar los 05 y sino se agrega
  else
  {
   if(First1==0)
   {
    fputs(BIR05off[i],pBIRfree05); // se agrega el registro 01
    for (j=0;j<=rec05;j++)
     {
      fputs(BIR05on[j],pBIRfree05);
      First1=1;
      }
 //ciclo para meter 05 y
   }
   else
   fputs(BIR05off[i],pBIRfree05);

  } // end else


} // end for i

fclose(pBIRfree05);

remove( FileName );
rename( "C:/econfig/UserData/test.txt", FileName );



return TRUE;



} // UpdateBIR()



