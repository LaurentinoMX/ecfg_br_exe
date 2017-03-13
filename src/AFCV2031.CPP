//=======================================================================
//FILE NAME        AFCV2031.CPP
//
//CLASS NAME       CONVERT2031
//
//DESCRIPTION      Reads a Version 20 CFREPORT file and creates
//                 a Version 31 equivalent file.
//=======================================================================
//
//Modifications:
//
//Date     Programmer  Rev    Description of Change
//----     ----------  -----  ---------------------
//12/15/97 E. Wilson   1.0    Creation of AFCV2031.CPP
//
//
//
//=======================================================================

#include <stdio.h>
#include <stdlib.h>
#include <fstream.h>
#include <iostream.h>
#include <string.h>
#include "afcv2031.hpp"
#include "afcv2031.h"
#include "af9apply.h"

//=======================================================================
// FUNCTION NAME    Convert2031
//
// DESCRIPTION      Constructs a Convert2031 object
//=======================================================================

Convert2031 :: Convert2031()
   {
   iProdCnt = 1;
   iLineCnt = 1;
   bRec03 = false;
   }

//=======================================================================
// FUNCTION NAME    convertCFReport
//
// DESCRIPTION      Converts a V20 CFReport to a V31 CFReport
//=======================================================================

int Convert2031 :: convertCFReport( IString strInFileName )
   {
   IString strRecord;
   IString strRecType, strSecDesc;
   IString strValid = STR_Valid20RecTypes;
   IString strInitialTypes = STR_InitialOrderRecTypes;
   IString strInstalledTypes = STR_InstalledRecTypes;
   IString strProposedTypes = STR_ProposedRecTypes;
   IString strOrderTypes = STR_V20OrderRecTypes;
   IString strZero = STR_Zero;
   IString strOutFileName;
   IString sBlank=STR_Blank;
   int iPrevSecCode = 0;
   int iPrevSecCnt = 0;
   int iPrevSecPos = 0;
   int iSecCode, iRC;
   Boolean bCmtFlag = false;
   Boolean bBuiltRec7 = false;
   IString sSecurity, sSpaceBefore, sSpaceAfter;

   ifs.open( strInFileName );
   if ( ifs.fail() )
      return IDS_ErrorConvertInFileOpen_Text;

   strOutFileName=strInFileName;
   strOutFileName.remove(strOutFileName.lastIndexOf('.') + 1);
   strOutFileName += IString(STR_OutputExt);
   ofs.open( strOutFileName );
   if ( ofs.fail() )
      return IDS_ErrorConvertOutFileOpen_Text;

   iRC = allocateArrays( strInFileName );

   //Reset the input stream pointer
   ifs.close();
   ifs.open( strInFileName );
   if ( ifs.fail() )
      return IDS_ErrorConvertInFileOpen_Text;

   while ( !ifs.eof() )
      {
      strRecord = IString::lineFrom( ifs, '\n' );
      strRecType = strRecord.subString(1, 2);
      //If record type is invalid, skip and read next record
      if ( strValid.indexOf( strRecType ) == 0 )
         {
            continue;
         }
      if ( ( !bBuiltRec7 ) && ( strRecType.asInt() > 3 ) )
         {
         if ( !bRec03 )
               createSecurityRec();
         bBuiltRec7=true;
         ofs << STR_V31Rec07 << endl;
         ofs << STR_V31Rec05a << endl;
         ofs << STR_V31Rec05b << endl;
         bCmtFlag = true;
         iLineCnt += 3;
         }

      //Process valid record type
      switch ( strRecType.asInt() )
         {
         case 0:
            iRC=processRec00(strRecord);
            if ( iRC )
               return iRC;
            break;
         case 3:
            sSecurity = strRecord.subString(3,1);
            if (sSecurity == STR_Two)
               strRecord.change(sSecurity, "1", 1, 1);
            ofs << strRecord << endl;
            iLineCnt++;
            bRec03=true;
            break;
         case 4:
            if ( !bRec03 )
               createSecurityRec();
            sSpaceBefore = strRecord.subString(10,1);
            sSpaceAfter  = strRecord.subString(11,1);
            if ( sSpaceBefore.asInt() > 0 )
               bCmtFlag = true;
            ofs << STR_V31Rec05 << sBlank.leftJustify(9) << bCmtFlag << strZero.leftJustify(4)
                                       << strRecord.subString(14,60) << endl;
            if ( sSpaceAfter.asInt() > 0 )
               bCmtFlag = true;
            else
               bCmtFlag = false;
            iLineCnt++;
            break;
         case 5:
            if ( !bRec03 )
               createSecurityRec();
            sSpaceBefore = strRecord.subString(7,1);
            sSpaceAfter  = strRecord.subString(8,1);
            if ( sSpaceBefore.asInt() > 0 )
               bCmtFlag = true;
            ofs << STR_V31Rec05 << sBlank.leftJustify(9) << bCmtFlag << strZero.leftJustify(4)
                                               << strRecord.subString(15,60) << endl;
            if ( sSpaceAfter.asInt() > 0 )
               bCmtFlag = true;
            else
               bCmtFlag = false;
            iLineCnt++;
            break;
         //case 52:
         //case 57:
         //   break;
         case 8:
         case 25:
         case 26:
            iRC=processHWRec(strRecord);
            if ( iRC )
               return iRC;
            break;
         case 17:
            iRC=processRec17(strRecord);
            if ( iRC )
               return iRC;
            break;
         case 19:
            iRC=processRec19(strRecord);
            if ( iRC )
               return iRC;
            break;
         case 35:
         case 36:
            iRC=processSWRec(strRecord);
            if ( iRC )
               return iRC;
            break;
         case 37:
            iRC=processRec37(strRecord);
            if ( iRC )
               return iRC;
            break;
         //case 53:
         //   break;
         //case 54:
         //   break;
         //case 55:
         //   break;
         //case 56:
         //   break;
         //case 98:
         //   break;
         default:
            continue;
         }
      }
   writeOutputFile();
   IString sLineCnt = iLineCnt;
   ofs << STR_V31Rec98 << sLineCnt.rightJustify(5) << endl;
   return 0;
   }

//=======================================================================
// FUNCTION NAME    createSecurityRec
//
// DESCRIPTION      Creates and writes a default security record if
//                  one is not included in the input file
//=======================================================================
void Convert2031 :: createSecurityRec()
   {
   int iSecurity = 0;
   ofs << STR_V31Rec03 << iSecurity << endl;
   iLineCnt++;
   bRec03=true;
   }

//=======================================================================
// FUNCTION NAME    allocateArrays
//
// DESCRIPTION      Determines the sizes of the arrays and allocate
//                  memory for them.
//=======================================================================
int Convert2031 :: allocateArrays( IString strInFileName )
   {
   IString strRecord, strRecType;
   IString strValid = STR_Valid20RecTypes;
   IString sMachQty, sMachAddQty, sMachNote, sMachAct, sToMach, sResidue;

   int  i, iFeatCnt;

   // Initialize counters
   ulHwInitial = ulHwInstalled = ulHwProposed = ulHwOrder = 0;
   ulSwInitial = ulSwInstalled = ulSwProposed = ulSwOrder = 0;

   // Process all records to determine array sizes
   while ( !ifs.eof() )
      {
      strRecord = IString::lineFrom( ifs, '\n' );
      strRecType = strRecord.subString(1, 2);
      //If record type is invalid, skip and read next record
      if ( strValid.indexOf( strRecType ) == 0 )
            continue;

      //Process valid record type
      switch ( strRecType.asInt() )
         {
         case 0:
         case 3:
         case 4:
         case 5:
            continue;
            break;
         case 8:
         case 25:
         case 26:
            // Begin parsing string to find start of feature fields
            sMachQty = strRecord.subString(7,8);
            sMachAddQty = strRecord.subString(15,1);
            sMachNote = strRecord.subString(16,1);
            sResidue = strRecord.subString(20);

            //Adjust record (Model fields) based on logic for records of this type
            //See Rules of Usage for 08,25, and 26 records.
            if (sMachQty.asInt() != 0)
               sResidue.insert("      01", 7);
            if (sMachAddQty.asInt() == 0)
               sResidue.insert("      00", 7);
            if (sMachNote.asInt() == 0)
               sResidue.insert(" ", 24);

            // Start of feature fields
            sResidue = sResidue.subString(25);

            // Parse string to determine the number of feature lines to
            // allocate in the array for this product
            iFeatCnt=0;
            while ( !sResidue.subString(1,7).isWhiteSpace() )
               {
               if (sMachQty.asInt() != 0)
                  sResidue.insert("      01", 7);
               if (sMachAddQty.asInt() == 0)
                  sResidue.insert("      00", 7);
               if (sMachNote.asInt() == 0)
                  sResidue.insert(" ", 24);
               sResidue = sResidue.subString(25);
               iFeatCnt++;
               }
            // Allocate space for 8, 25, or 26 record, corresp. 95 record
            // and the features for this record
            switch ( strRecType.asInt() )
               {
               case 8:
                  ulHwInitial = ulHwInitial + iFeatCnt + 2;
                  break;
               case 25:
                  ulHwInstalled = ulHwInstalled+ iFeatCnt + 2;
                  break;
               case 26:
                  ulHwProposed = ulHwProposed + iFeatCnt + 2;
                  break;
               default:
                  break;
               }
            break;
         case 17:
            //Start of feature fields
            sResidue =  strRecord.subString(17);

            // Parse string to determine the number of feature lines to
            // allocate in the array for this product
            iFeatCnt=0;
            while ( !sResidue.subString(1,4).isWhiteSpace() )
               {
               sResidue = sResidue.subString(9);
               iFeatCnt++;
               }
            // Allocate space for record 17, corresp. 95 record
            // and the features for this record
            ulSwInitial = ulSwInitial + iFeatCnt + 2;
            break;
         case 19:
            sToMach =  strRecord.subString(13,7);
            sMachAct = strRecord.subString(24,1);
            sResidue =   strRecord.subString(72);

            iFeatCnt=0;
            while ( !sResidue.subString(1,7).isWhiteSpace() )
               {
               sResidue = sResidue.subString(49);
               iFeatCnt++;
               }
            // If the action code is 1, this is a new order so
            // allocate space for it in the Initial HW order array
            if (sMachAct == STR_One)
               {
               ulHwInitial = ulHwInitial + iFeatCnt + 2;
               if ( !sToMach.isWhiteSpace() )
                  ulHwInitial = ulHwInitial + 1;
               }
            else
               {
               ulHwOrder = ulHwOrder + iFeatCnt + 2;
               if ( !sToMach.isWhiteSpace() )
                  ulHwOrder = ulHwOrder + 1;
               }
            break;
         case 35:
         case 36:
            //Start of feature fields
            sResidue =  strRecord.subString(14);

            // Parse string to determine the number of feature lines to
            // allocate in the array for this product
            iFeatCnt=0;
            while ( !sResidue.subString(1,4).isWhiteSpace() )
               {
               sResidue = sResidue.subString(9);
               iFeatCnt++;
               }
            // Allocate space for 35 or 36 record, corresp. 95 record
            // and the features for this record
            if ( strRecType.asInt() == 35 )
               ulSwProposed = ulSwProposed + iFeatCnt + 2;
            else
               ulSwInstalled = ulSwInstalled + iFeatCnt + 2;
            break;
         case 37:
            sMachAct =   strRecord.subString(14,1);
            sResidue =   strRecord.subString(58);

            iFeatCnt=0;
            while ( !sResidue.subString(1,4).isWhiteSpace() )
               {
               sResidue = sResidue.subString(41);
               iFeatCnt++;
               }
            // If the action code is 1, this is a new order so
            // allocate space for it in the Initial SW order array
            if ( sMachAct == STR_One )
               ulSwInitial = ulSwInitial + iFeatCnt + 2;
            else
               ulSwOrder = ulSwOrder + iFeatCnt + 2;
            break;
         default:
            continue;
         }
      }
   //Allocate space for the arrays
   pHwInitial = new IString[ulHwInitial];
   pHwInstalled = new IString[ulHwInstalled];
   pHwProposed = new IString[ulHwProposed];
   pHwOrder = new IString[ulHwOrder];
   pSwInitial = new IString[ulSwInitial];
   pSwInstalled = new IString[ulSwInstalled];
   pSwProposed = new IString[ulSwProposed];
   pSwOrder = new IString[ulSwOrder];
   //Reset array counters
   ulHwInitial = ulHwInstalled = ulHwProposed = ulHwOrder = 0;
   ulSwInitial = ulSwInstalled = ulSwProposed = ulSwOrder = 0;
   return 0;
   }

//=======================================================================
// FUNCTION NAME    writeArrays
//
// DESCRIPTION      Writes data to the appropriate array.  Data is
//                  written to the arrays first, so the we can write
//                  the sections of the V31 CFReport in the correct order
//=======================================================================
void Convert2031 :: writeArrays( IString sProd, IString sProdInfo1,
                                    IString *pFeatures, int iFeatCnt, IString sProdInfo2 )
   {
   int i, iRecType;
   iRecType = ( sProd.subString(1,2).asInt() );
   switch ( iRecType )
      {
      case 8:
         pHwInitial[ulHwInitial]=sProd;
         ulHwInitial++;
         pHwInitial[ulHwInitial]=sProdInfo1;
         ulHwInitial++;
         if ( !sProdInfo2.isWhiteSpace() )
            {
            pHwInitial[ulHwInitial]=sProdInfo2;
            ulHwInitial++;
            }
         for ( i=0; i < iFeatCnt; i++ )
            {
            pHwInitial[ulHwInitial]=pFeatures[i];
            ulHwInitial++;
            }
         break;
      case 47:
         pSwInitial[ulSwInitial]=sProd;
         ulSwInitial++;
         pSwInitial[ulSwInitial]=sProdInfo1;
         ulSwInitial++;
         for ( i=0; i < iFeatCnt; i++ )
            {
            pSwInitial[ulSwInitial]=pFeatures[i];
            ulSwInitial++;
            }
         break;
      case 19:
         pHwOrder[ulHwOrder]=sProd;
         ulHwOrder++;
         pHwOrder[ulHwOrder]=sProdInfo1;
         ulHwOrder++;
         if ( !sProdInfo2.isWhiteSpace() )
            {
            pHwOrder[ulHwOrder]=sProdInfo2;
            ulHwOrder++;
            }
         for ( i=0; i < iFeatCnt; i++ )
            {
            pHwOrder[ulHwOrder]=pFeatures[i];
            ulHwOrder++;
            }
         break;
      case 25:
         pHwInstalled[ulHwInstalled]=sProd;
         ulHwInstalled++;
         pHwInstalled[ulHwInstalled]=sProdInfo1;
         ulHwInstalled++;
         for ( i=0; i < iFeatCnt; i++ )
            {
            pHwInstalled[ulHwInstalled]=pFeatures[i];
            ulHwInstalled++;
            }
         break;
      case 26:
         pHwProposed[ulHwProposed]=sProd;
         ulHwProposed++;
         pHwProposed[ulHwProposed]=sProdInfo1;
         ulHwProposed++;
         for ( i=0; i < iFeatCnt; i++ )
            {
            pHwProposed[ulHwProposed]=pFeatures[i];
            ulHwProposed++;
            }
         break;
      case 48:
         pSwProposed[ulSwProposed]=sProd;
         ulSwProposed++;
         pSwProposed[ulSwProposed]=sProdInfo1;
         ulSwProposed++;
         for ( i=0; i < iFeatCnt; i++ )
            {
            pSwProposed[ulSwProposed]=pFeatures[i];
            ulSwProposed++;
            }
         break;
      case 49:
         pSwInstalled[ulSwInstalled]=sProd;
         ulSwInstalled++;
         pSwInstalled[ulSwInstalled]=sProdInfo1;
         ulSwInstalled++;
         for ( i=0; i < iFeatCnt; i++ )
            {
            pSwInstalled[ulSwInstalled]=pFeatures[i];
            ulSwInstalled++;
            }
         break;
      case 50:
         pSwOrder[ulSwOrder]=sProd;
         ulSwOrder++;
         pSwOrder[ulSwOrder]=sProdInfo1;
         ulSwOrder++;
         for ( i=0; i < iFeatCnt; i++ )
            {
            pSwOrder[ulSwOrder]=pFeatures[i];
            ulSwOrder++;
            }
         break;
      default:
         break;
      }
   }

//=======================================================================
// FUNCTION NAME    writeOutputFile
//
// DESCRIPTION      Writes sections in arrays to ouput file in the correct
//                  V31 order
//=======================================================================
void Convert2031 :: writeOutputFile()
   {
   int i, iSecCode;
   IString strSecDesc;

   iSecCode = 2;
   strSecDesc = STR_SectionBase;
   ofs << STR_V31Rec01 << iSecCode << strSecDesc << endl;
   iLineCnt++;
   if ( ulHwInitial )
      for ( i=0; i < ulHwInitial; i++ )
         {
         ofs << pHwInitial[i] << endl;
         iLineCnt++;
         }
   if ( ulSwInitial )
      for ( i=0; i < ulSwInitial; i++ )
         {
         ofs << pSwInitial[i] << endl;
         iLineCnt++;
         }

   if ( ulHwInstalled )
      for ( i=0; i < ulHwInstalled; i++ )
         {
         ofs << pHwInstalled[i] << endl;
         iLineCnt++;
         }
   if ( ulSwInstalled )
      for ( i=0; i < ulSwInstalled; i++ )
         {
         ofs << pSwInstalled[i] << endl;
         iLineCnt++;
         }

   if ( ulHwProposed )
      for ( i=0; i < ulHwProposed; i++ )
         {
         ofs << pHwProposed[i] << endl;
         iLineCnt++;
         }
   if ( ulSwProposed )
      for ( i=0; i < ulSwProposed; i++ )
         {
         ofs << pSwProposed[i] << endl;
         iLineCnt++;
         }

   if ( ulHwOrder )
      for ( i=0; i < ulHwOrder; i++ )
         {
         ofs << pHwOrder[i] << endl;
         iLineCnt++;
         }
   if ( ulSwOrder )
      for ( i=0; i < ulSwOrder; i++ )
         {
         ofs << pSwOrder[i] << endl;
         iLineCnt++;
         }

   //De-allocate memory
   delete [] pHwInitial;
   delete [] pSwInitial;
   delete [] pHwInstalled;
   delete [] pSwInstalled;
   delete [] pHwProposed;
   delete [] pSwProposed;
   delete [] pHwOrder;
   delete [] pSwOrder;
   }

//=======================================================================
// FUNCTION NAME    makeDesc
//
// DESCRIPTION      Construct a generic description field for
//                  V31 feature and product records
//=======================================================================
IString Convert2031 :: makeDesc(IString sDesc, IString sCnt)
   {
   sDesc.change(STR_Substitute0, sCnt);
   if ( sDesc.indexOf( STR_Substitute1 ) != 0 )
      sDesc.change(STR_Substitute1, sCnt);
   return sDesc;
   }

//=======================================================================
// FUNCTION NAME    makeProdInfoRec
//
// DESCRIPTION      Construct and print a Product Information Record
//                  (V31 Record 95).
//=======================================================================
IString Convert2031 :: makeProdInfoRec(IString sType, IString sModel, IString sSerial,
                                                  IString sProdCnt)
   {
   IString sBlank=STR_Blank;
   IString sDesc=makeDesc(STR_ProdDesc, sProdCnt);
   IString sOutRec = STR_V31Rec95+
                     sType.leftJustify(4)+
                     sModel.rightJustify(3)+
                     sBlank.leftJustify(10)+
                     sSerial+
                     sBlank.leftJustify(9)+
                     sBlank.leftJustify(9)+
                     sBlank.leftJustify(2)+
                     STR_PriceFlag+
                     sBlank.leftJustify(1)+
                     sBlank.leftJustify(4)+
                     sBlank.leftJustify(7)+
                     "0"+
                     sBlank.leftJustify(4)+
                     sBlank.leftJustify(9)+
                     sBlank.leftJustify(7)+
                     STR_CradDate+
                     STR_InstDate+
                     sBlank.leftJustify(1)+
                     sBlank.leftJustify(6)+
                     "A"+
                     sBlank.leftJustify(1)+
                     sBlank.leftJustify(35)+
                     sDesc;
   //ofs << sOutRec << endl;
   iProdCnt++;
   return sOutRec;
   }

//=======================================================================
// FUNCTION NAME    processRec00
//
// DESCRIPTION      Process a V20 Header Record.
//=======================================================================
int Convert2031 :: processRec00( IString strRecord )
   {
   IString sBlank=STR_Blank;
   IString sVersion = STR_ConvertVersion;
   if (strRecord.subString(3,2) != STR_Twenty)
      return IDS_ErrorFileType_Text;  //??
   IString sMonth = strRecord.subString(5,2);
   IString sDay = strRecord.subString(7,2);
   IString sYear = strRecord.subString(9,2);
   if(sYear.asInt() > 50)
      sYear=STR_Nineteen+sYear;
   else
      sYear=STR_Twenty+sYear;
   IString strV31Date=sYear+sMonth+sDay;
   IString strRec00 = STR_V31Rec00+
                      sVersion+
                      strV31Date+
                      strRecord.subString(11,8)+
                      STR_V31SysLevel+
                      STR_Country1+
                      STR_CountryCode1+
                      sBlank.leftJustify(2)+
                      strRecord.subString(85,1)+
                      "1"+
                      sBlank.leftJustify(1)+
                      strRecord.subString(39,40)+
                      sBlank.leftJustify(61)+
                      STR_Language1+
                      sBlank.leftJustify(8)+
                      sBlank.leftJustify(5)+
                      sBlank.leftJustify(8)+
                      strRecord.subString(80,1)+
                      strRecord.subString(81,1)+
                      strRecord.subString(114,80);
   ofs << strRec00 << endl;
   iLineCnt++;
   return 0;
   }
//=======================================================================
// FUNCTION NAME    processHWRec
//
// DESCRIPTION      Process a V20 HW Record (Records 08,25 and 26).
//=======================================================================
int Convert2031 :: processHWRec( IString strRecord )
   {
   int i, iFeatCnt;
   IString sBlank=STR_Blank;
   IString sFeat, sFeatQty, sFeatAddQty, sFeatNote, sFeatTotQty;
   IString sMachine = strRecord.subString(3,4);
   IString sMachQty = strRecord.subString(7,8);
   IString sMachAddQty = strRecord.subString(15,1);
   IString sMachNote = strRecord.subString(16,1);
   IString sResidue = strRecord.subString(20);

   //Adjust record based on logic for records of this type
   //See Rules of Usage for 08,25, and 26 records.
   if (sMachQty.asInt() != 0)
      sResidue.insert("      01", 7);
   if (sMachAddQty.asInt() == 0)
      sResidue.insert("      00", 7);
   if (sMachNote.asInt() == 0)
      sResidue.insert(" ", 24);

   IString sModel = sResidue.subString(1,7);
   IString sModelQty = sResidue.subString(8,8);
   IString sModelAddQty = sResidue.subString(16,8);
   IString sModelNote = sResidue.subString(24,1);
   sResidue = sResidue.subString(25);
   IString sModTotQty = sModelQty.asInt()+sModelAddQty.asInt();

   IString strHWRec = strRecord.subString(1,2)+
                      sMachine+
                      " "+
                      sModel.subString(5,3)+
                      sModTotQty.rightJustify(5);

   //Calculate maximum size of feature array
   iFeatCnt=0;
   i=sResidue.length() / 7;
   //Declare and allocate feature array
   IString *pFeatures;
   pFeatures = new IString[i];
   if (!pFeatures)
      return IDS_ErrorAllocatingMemory;  //??
   while ( !sResidue.subString(1,7).isWhiteSpace() )
      {
      if (sMachQty.asInt() != 0)
         sResidue.insert("      01", 7);
      if (sMachAddQty.asInt() == 0)
         sResidue.insert("      00", 7);
      if (sMachNote.asInt() == 0)
         sResidue.insert(" ", 24);
      sFeat = sResidue.subString(1,7);
      sFeatQty = sResidue.subString(8,8);
      sFeatAddQty = sResidue.subString(16,8);
      sFeatNote = sResidue.subString(24,1);
      sResidue = sResidue.subString(25);
      sFeatTotQty = sFeatQty.asInt()+sFeatAddQty.asInt();
      //Add to output record
      strHWRec = strHWRec+
                 sFeat.strip().leftJustify(7)+
                 sFeatTotQty.rightJustify(5);
      //Build feature information record
      pFeatures[iFeatCnt]=STR_V31Rec96+
                   sFeat.strip().leftJustify(7)+
                   sBlank.leftJustify(3)+
                   sBlank.leftJustify(8)+
                   sBlank.leftJustify(4)+
                   sBlank.leftJustify(4)+
                   STR_PriceFlag+
                   sBlank.leftJustify(21)+
                   makeDesc(STR_FeatDesc, iFeatCnt+1);
      iFeatCnt++;
      }
   //Create the product information record
   IString sInfoRec = makeProdInfoRec( sMachine, sModel, STR_Serial, iProdCnt );
   //Write HW record, prod. info record, and feature records to output arrays
   writeArrays( strHWRec, sInfoRec, pFeatures, iFeatCnt );
   delete [] pFeatures;
   return 0;
   }

//=======================================================================
// FUNCTION NAME    processRec19
//
// DESCRIPTION      Process a V20 Record 19 (HW MES).
//=======================================================================
int Convert2031 :: processRec19( IString strRecord )
   {
   int i, iFeatCnt;
   IString sBlank=STR_Blank;
   IString sInfoRec2=" ";
   IString strProdAction = STR_ProdAction;
   IString sOutRec;
   IString sFeat, sFeatQty, sFeatAct, sFeatRest;
   IString sMachine =   strRecord.subString(3,4);
   IString sModel =     strRecord.subString(10,3);
   IString sToMach =    strRecord.subString(13,4);
   IString sToModel =   strRecord.subString(17,3);
   IString sMachQty =   strRecord.subString(20,4);
   IString sMachAct =   strRecord.subString(24,1);
   IString sSerial =    strRecord.subString(28,9);
   IString sResidue =   strRecord.subString(72);
   sSerial.strip().rightJustify(9);
   if ( (sToMach.isWhiteSpace()) && (sModel.isWhiteSpace()) )
      {
      sModel = sToModel;
      sToModel = sBlank.leftJustify(3);
      }

   //Translate old product actions to new code
   if ( strProdAction.indexOf( sMachAct ) != 0 )
      sMachAct = STR_NewProdAction;

   if (sMachAct == STR_One)
      {
      sOutRec =  STR_V31Rec08+
                 sMachine+
                 " "+
                 sModel+
                 sMachQty.rightJustify(5);
      sSerial=sBlank.leftJustify(9);
      }
   else
      sOutRec = STR_V31Rec19+
                sMachine+
                sModel+
                sToMach+
                sToModel+
                sMachAct;

   //Calculate maximum size of feature array
   iFeatCnt=0;
   i=( ( sResidue.length() / 48 ) + 1 );
   //Declare and allocate feature array
   IString *pFeatures;
   pFeatures = new IString[i];
   if (!pFeatures)
      return IDS_ErrorAllocatingMemory;  //??
   while ( !sResidue.subString(1,7).isWhiteSpace() )
      {
      sFeat = sResidue.subString(1,7);
      if( sFeat.subString(1, 3) == "000" )
         sFeat.remove(1, 3);                      /* edw01/10/2000 */
      sFeatQty = sResidue.subString(8,4);
      sFeatQty.stripLeading("0");                 /* edw01/10/2000 */
      sFeatAct = sResidue.subString(12,1);
      sFeatRest = sResidue.subString(13,36);
      sResidue = sResidue.subString(49);
      //Translate old feature actions to new codes
      if (sFeatAct == STR_A)
         sFeatAct = STR_One;
      if (sFeatAct == STR_B)
         sFeatAct = STR_Two;
      if (sMachAct == STR_One)
         sFeatAct = sBlank;

      //Add to output record
      if (sMachAct == STR_One)                    /* edw01/10/2000 */
         sOutRec = sOutRec+
                sFeat.strip().leftJustify(7)+
                sFeatQty.rightJustify(5);
      else
         sOutRec = sOutRec+
                sFeat.strip().leftJustify(7)+
                sFeatQty.rightJustify(5)+
                sFeatAct;

      //Build feature information record
      pFeatures[iFeatCnt]=STR_V31Rec96+
                   sFeat.strip().leftJustify(7)+
                   sBlank.leftJustify(3)+
                   sBlank.leftJustify(8)+
                   sBlank.leftJustify(4)+
                   sBlank.leftJustify(4)+
                   STR_PriceFlag+
                   sBlank.leftJustify(21)+
                   makeDesc(STR_FeatDesc, iFeatCnt+1);
      iFeatCnt++;
      }
   //Create the product information record
   IString sInfoRec1 = makeProdInfoRec( sMachine, sModel, sSerial, iProdCnt );

   if ( !sToMach.isWhiteSpace() || !sToModel.isWhiteSpace() )
      {
      IString sProdCntPlus = iProdCnt;
      sProdCntPlus += STR_Plus;
      sInfoRec2 = makeProdInfoRec( sToMach, sToModel, sSerial, sProdCntPlus );
      }
   //Write HW record, prod. info record, and feature records to output arrays
   writeArrays( sOutRec, sInfoRec1, pFeatures, iFeatCnt, sInfoRec2 );
   delete [] pFeatures;
   return 0;
   }

//=======================================================================
// FUNCTION NAME    processRec17
//
// DESCRIPTION      Process a V20 Record 17 (Initial SW Order).
//=======================================================================
int Convert2031 :: processRec17( IString strRecord )
   {
   int i, iFeatCnt;
   IString sBlank=STR_Blank;
   IString sOutRec;
   IString sFeat, sFeatQty;
   IString sProgType = strRecord.subString(3,4);
   IString sModel =    strRecord.subString(10,3);
   IString sQty =      strRecord.subString(13,4);
   IString sResidue =  strRecord.subString(17);

   sOutRec = STR_V31Rec47+
                sProgType+
                sModel.rightJustify(3)+
                sBlank.leftJustify(1)+
                sQty;

   //Calculate maximum size of feature array
   iFeatCnt=0;
   i=( ( sResidue.length() / 8) + 1 );
   //Declare and allocate feature array
   IString *pFeatures;
   pFeatures = new IString[i];
   if (!pFeatures)
      return IDS_ErrorAllocatingMemory;  //??
   while ( !sResidue.subString(1,4).isWhiteSpace() )
      {
      sFeat = sResidue.subString(1,4);
      sFeatQty = sResidue.subString(5,4);
      sResidue = sResidue.subString(9);

      //Add to output record
      sOutRec = sOutRec+
                sFeat.strip().leftJustify(7)+
                sFeatQty.strip().rightJustify(5);

      //Build feature information record
      pFeatures[iFeatCnt]=STR_V31Rec96+
                   sFeat.strip().leftJustify(7)+
                   sBlank.leftJustify(3)+
                   STR_SWVRM+
                   sBlank.leftJustify(4)+
                   STR_RelMod+
                   STR_PriceFlag+
                   sBlank.leftJustify(21)+
                   makeDesc(STR_FeatDesc, iFeatCnt+1);
      iFeatCnt++;
      }
   //Create the product information record
   IString sInfoRec = makeProdInfoRec( sProgType, sModel, STR_Serial, iProdCnt );
   //Write SW record, prod. info record, and feature records to output arrays
   writeArrays( sOutRec, sInfoRec, pFeatures, iFeatCnt );
   delete [] pFeatures;
   return 0;
   }

//=======================================================================
// FUNCTION NAME    processSWRec
//
// DESCRIPTION      Process V20 Records 35 and 36 (SW Orders-Inst and Pro).
//=======================================================================
int Convert2031 :: processSWRec( IString strRecord )
   {
   int i, iFeatCnt;
   IString sBlank=STR_Blank;
   IString sOutRec;
   IString sFeat, sFeatQty;
   IString sProgType = strRecord.subString(3,4);
   IString sModel =    strRecord.subString(7,3);
   IString sQty =      strRecord.subString(10,4);
   IString sResidue =  strRecord.subString(14);

   if ( strRecord.subString(1,2) == STR_V20Rec35 )
      sOutRec = STR_V31Rec48+
                sProgType+
                sModel.rightJustify(3)+
                sBlank.leftJustify(1)+
                sQty;
   else
      sOutRec = STR_V31Rec49+
                sProgType+
                sModel.rightJustify(3)+
                sBlank.leftJustify(1)+
                sQty;

   //Calculate maximum size of feature array
   iFeatCnt=0;
   i=( ( sResidue.length() / 8 ) + 1 );
   //Declare and allocate feature array
   IString *pFeatures;
   pFeatures = new IString[i];
   if (!pFeatures)
      return IDS_ErrorAllocatingMemory;  //??
   while ( !sResidue.subString(1,4).isWhiteSpace() )
      {
      sFeat = sResidue.subString(1,4);
      sFeatQty = sResidue.subString(5,4);
      sResidue = sResidue.subString(9);

      //Add to output record
      sOutRec = sOutRec+
                sFeat.strip().leftJustify(7)+
                sFeatQty.strip().rightJustify(5);

      //Build feature information record
      pFeatures[iFeatCnt]=STR_V31Rec96+
                   sFeat.strip().leftJustify(7)+
                   sBlank.leftJustify(3)+
                   STR_SWVRM+
                   sBlank.leftJustify(4)+
                   STR_RelMod+
                   STR_PriceFlag+
                   sBlank.leftJustify(21)+
                   makeDesc(STR_FeatDesc, iFeatCnt+1);
      iFeatCnt++;
      }
   //Create the product information record
   IString sInfoRec = makeProdInfoRec( sProgType, sModel, STR_Serial, iProdCnt );

   //Write SW record, prod. info record, and feature records to output arrays
   writeArrays( sOutRec, sInfoRec, pFeatures, iFeatCnt );
   delete [] pFeatures;
   return 0;
   }

//=======================================================================
// FUNCTION NAME    processRec37
//
// DESCRIPTION      Process V20 Records 37 (SW MES).
//=======================================================================
int Convert2031 :: processRec37( IString strRecord )
   {
   int i, iFeatCnt;
   IString sBlank=STR_Blank;
   IString strProdAction = STR_ProdAction;
   IString sOutRec;
   IString sFeat, sFeatQty, sFeatAct, sFeatRest;
   IString sProgType =  strRecord.subString(3,4);
   IString sModel =     strRecord.subString(7,3);
   IString sQty =       strRecord.subString(10,4);
   IString sMachAct =   strRecord.subString(14,1);
   IString sSerial =    strRecord.subString(18,9);
   IString sResidue =   strRecord.subString(58);

   sSerial.strip().rightJustify(9);

   if ( sMachAct == STR_One )
      {
      sOutRec =  STR_V31Rec47+
                 sProgType+
                 sModel+
                 sQty.rightJustify(5);
      sSerial=sBlank.leftJustify(9);
      }
   else
      sOutRec = STR_V31Rec50+
                sProgType+
                sModel+
                sMachAct;

   //Calculate maximum size of feature array
   iFeatCnt=0;
   i=( ( sResidue.length() / 40 ) + 1 );
   //Declare and allocate feature array
   IString *pFeatures;
   pFeatures = new IString[i];
   if (!pFeatures)
      return IDS_ErrorAllocatingMemory;  //??
   while ( !sResidue.subString(1,4).isWhiteSpace() )
      {
      sFeat = sResidue.subString(1,4);
      sFeatQty = sResidue.subString(5,4);
      sFeatQty.stripLeading("0");                 /* edw01/10/2000 */
      sFeatAct = sResidue.subString(9,1);
      sFeatRest = sResidue.subString(10,31);
      sResidue = sResidue.subString(41);

      //Translate old feature actions to new codes
      if (sFeatAct == STR_A)
         sFeatAct = STR_One;
      if (sFeatAct == STR_B)
         sFeatAct = STR_Two;
      if (sMachAct == STR_One)
         sFeatAct = sBlank;

      //Add to output record
      if (sMachAct == STR_One)                    /* edw01/10/2000 */
         sOutRec = sOutRec+
                sFeat.strip().leftJustify(7)+
                sFeatQty.strip().rightJustify(5);
      else
         sOutRec = sOutRec+
                sFeat.strip().leftJustify(7)+
                sFeatQty.strip().rightJustify(5)+
                sFeatAct;

      //Build feature information record
      pFeatures[iFeatCnt]=STR_V31Rec96+
                   sFeat.strip().leftJustify(7)+
                   sBlank.leftJustify(3)+
                   STR_SWVRM+
                   sBlank.leftJustify(4)+
                   STR_RelMod+
                   STR_PriceFlag+
                   sBlank.leftJustify(21)+
                   makeDesc(STR_FeatDesc, iFeatCnt+1);
      iFeatCnt++;
      }
   //Create the product information record
   IString sInfoRec = makeProdInfoRec( sProgType, sModel, sSerial, iProdCnt );

   //Write SW record, prod. info record, and feature records to output arrays
   writeArrays( sOutRec, sInfoRec, pFeatures, iFeatCnt );
   delete [] pFeatures;
   return 0;
   }

//=======================================================================
// FUNCTION NAME    ~Convert2031
//
// DESCRIPTION      Closes input and output files
//=======================================================================

Convert2031 :: ~Convert2031()
   {
   ifs.close();
   ofs.close();
   }


//Test Driver
/*void main(int argc, char* argv[])
{
int iCode;
IString sInputFile=" ";
Convert2031 *pConvert;

switch( argc )
   {
   case 1:
      break;
   case 2:
      sInputFile = argv[1];
      break;
   default:
      break;
   }

pConvert=new Convert2031();
if ( !sInputFile.isWhiteSpace() )
   {
   iCode=pConvert->convertCFReport( sInputFile );
   if ( iCode == 0 )
      cout << "Converted successfully" << endl;
   else
      cout << "Error while converting:  "<< iCode << endl;
   }
else
   {
   cout << "Usage: convert <inputfile>" << endl;
   }
} */




