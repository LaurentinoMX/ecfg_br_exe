//=======================================================================
// FILE NAME        AF9PROFL.CPP
//
// CLASS NAME       Profile
//
// DESCRIPTION      This class stores the user profile for the Order
//                  Apply CPF.
// Modification Log
//-----------------------------------------------------------------------
// E. Wilson   03/15/99    Use "\econfig\userdata" as the default
//                         browse and generate directory if it exists
// E. Wilson   10/18/1999  Add processing for bShowS390Mismatches profile
//                         entry
// E. Wilson   10/18/1999  Add setupKeyData method
//
// F.Walker    12/17/1999  44549  Added code so the if the user does not have
//                                Personal Communications installed on their
//                                machines to supress error message and not
//                                give them the option of doing VM retrieval.
//
// F.Walker     03/13/2000  48163 e-Config needed to have the registry keys reside
//                          HKEY_CURRENT_USER instead of HKEY_LOCAL_MACHINE
//                          so that user on NT would not have write access
//                          error if they do not have Admin. rights on machine
//
// F.Walker    03/13/2000   F47698  Profile for the Reconciliation
//
// F.Walker    03/13/2000   F47700  Profile for the Process Rpos
//
// I. A. Khan  01/16/2002   D67437  Look for Netscape 6 path if Netscape 4
//                                  path is not found in registry
//
// E. Wilson   01/21/2002   F62721 Added support for use of this class by
//                                 the Base Retrieval profile DLL (BF9PROFL.DLL)
//                                 Added functions for moving the location of
//                                 the registry keys
// Z. Obregon  03/26/2001   D69791 For PCOMM 5.5 the "HKEY_LOCAL_MACHINE\\SOFTWARE\\
//                                 Microsoft\\Windows\\CurrentVersion\\App Paths
//                                 \\PCSWS.EXE\\default"is inclomplete, this fix adds
//                                 the word pcsws.exe at the end to find the program.
//                                 and shows it in the preference window.
// C Herrera   10/02/2002   D73565 Begin Logic:  In case that the Browse Path and the
//                                 Generate Path end with a '/' character, this must
//                                 be removed as it causes problems when the file path
//                                 is tested if exists or not: is evaluated as not existing
// P.Sánchez   10/07/2003   D80040 New function added to move the RM2 Registry Entries ---------
//                                 located in the wrong path: -RM2 Registry Entry-  ------------
//                                 HKEY_CURRENT_USER\\Software\\IBM\\_econfig_profile\\_BF9RM2".
//                                 to the right one: -------------------------------------------
//                                 HKEY_CURRENT_USER\\Software\\IBM\\econfig_profile\\BF9RM2".--
// P.Sanchez   12/04/2003   69271  The Retrieval Directory in the Profile could not be saved ---
//                                 if this contained the Root Directory (C: or C:\).------------
//                                 The stat() function was returning -1 if the Retrieval Dir ---
//                                 was C: or C:\ which mean not exist(Just happened in Win 98).-
//                                 The evaluation process was commented due it was not necessary
// 05/11/2004 Z. Obregon 91005   IS is not maintaining MR data checkbox when via BaseRetrieval
//                               -- when zseries is selected the mrpd flag will be true, this flag is passed to IS
// 06/06/2006 E. Reyes   106765    Added logic into "load" function to write ub9host4.exe as
//                       Req1436   default in the registry if the user geography is "US".
//
//
// 12/12/2008 ELRR       F127433   Disable the US geography condition to allow the Inventory Services
//                       Req1515   host interface (ub9host4.exe) option to be set as default for all geographies
//                                 if the selected option is VM host.
//
// 30/12/2009 ELRR       F133122   Several changes were made to determine if the user has installed
//                       Req1547   Firefox browser on his machine and to know what browser is using to make
//                                 the retrieve from Inventory Services
//
// 25/06/2014  GTL       Req1702   Several changes were made to determine if the user has installed Chrome 
//								   on his machine and to define which browser will be used to retrieve 
//								   from Inventory Services.
//
//
//=======================================================================
// Copyright (c) International Business Machines Corp. 1996
// All rights reserved
//=======================================================================

#include <direct.h>                     // getcwd
#include <stdlib.h>                     // memset
#include <string.h>                     // str*
#include <sys\types.h>                  // stat
#include <sys\stat.h>                   // stat
#include <fstream.h>

#include <ipoint.hpp>                   // IPoint
#include <iprofile.hpp>                 // IProfile
#include <istring.hpp>                  // IString

#include <locale.h>                     // setlocale
#include "af9rtsrc.hpp"                 // F106765 RERC 06/05/2006
#include "af9profl.hpp"                 // Profile
#ifndef BR_PROFILE  /* F62721 EDW 01/21/2002 */
  #include "af9main.hpp"                // MainWindow
  #include "af9ordrs.hpp"               // OrdersNotebook
#else
  #include "af9pfdlg.hpp"               // ProfileDialog
#endif

typedef struct
   {
   LONG lLeft[WINDOWCOUNT];
   LONG lBottom[WINDOWCOUNT];
   LONG lWidth[WINDOWCOUNT];
   LONG lHeight[WINDOWCOUNT];
   SHORT sSortFiles;
   SHORT sSortOrders;
  // Boolean bHardware;
  // Boolean bSoftware;
   } SAVESTR;

//=======================================================================
// FUNCTION NAME
//
// DESCRIPTION      Construct an instance of the Profile class.
//=======================================================================

Profile :: Profile()
   {

   // Set default values.

   strLocale = "EN_US";
   strMRI = strLocale;
   strMRI.insert(STR_Prefix);
   char szPath[256];
   _getcwd(szPath, sizeof(szPath));
   strBrowsePath = szPath;
   strGeneratePath = strBrowsePath;
   strRetApp = STR_LocalFiles;
   position[WINDOWMAIN].setX(50);
   position[WINDOWMAIN].setY(50);
   size[WINDOWMAIN].setWidth(400);
   size[WINDOWMAIN].setHeight(300);
   position[WINDOWORDERS].setX(100);
   position[WINDOWORDERS].setY(100);
   size[WINDOWORDERS].setWidth(600);
   size[WINDOWORDERS].setHeight(400);
   position[WINDOWSYSTEMS].setX(200);
   position[WINDOWSYSTEMS].setY(150);
   size[WINDOWSYSTEMS].setWidth(600);        //edw 12/17/97 changed so notebooks are tiled
   size[WINDOWSYSTEMS].setHeight(400);
   position[WINDOWDETAIL].setX(50);
   position[WINDOWDETAIL].setY(50);
   size[WINDOWDETAIL].setWidth(500);
   size[WINDOWDETAIL].setHeight(500);
#ifndef BR_PROFILE  /* F62721 EDW 01/21/2002 */
   sSortFiles = -MainWindow::enumSortDate;
   sSortOrders = OrdersNotebook::enumSortProduct;
#else
   sSortFiles = -2;
   sSortOrders = 1;
#endif
   bHardware = true;
   bSoftware = true;
   bShowS390Mismatches = true;
   bFastPath = false;                         // Usability 38935
   bExpanded = true;                          // ZOR 48548
   bSort = true;                              // ZOR IC01552
   bSascending = true;                        // ZOR IC01552
   bTile = true;                              // 47971 MAMT 21/08/2000
   bMRPD = false;                              // Usability 38935 ZOR 91005 was changed to false as default
   strBrowser = STR_Blank1;                   // Usability 38935
   bIESupported = false;                      // Usability 38935


// GTL Req1702 Begin: The variable bChromeSupported is set as false to validate if the user has Chrome installed.
   bChromeSupported = false;
// GTL Req1702 END.
   
// ELRR-F133122 Begin: The variable bFirefoxSupported is set as false to validate if the user has Firefox installed.
   bFirefoxSupported = false;
// bNetscapeSupported = false;                // Usability 38935
// ELRR-F133122 END.

   bPassportSupported = false;                // IC42054 - flw
   bEconfig = false;                          // Usability 38935
   bPComSupported = false;                    // 44549 - flw
   bAutoRec = false;                          // 47698 - flw
   bProcRPO = false;                          // 47700 - flw
   strProdFamily = STR_Blank1;                // 47701 - flw
   bRefreshList = false;                      // F62721 EDW 01/21/2002
   bRefreshLocale = false;                    // F62721 EDW 01/21/2002
   strRetSources = STR_LocalFiles;            // F106765 RERC 05/30/2006
   pRetrSource = (RetrSource*)NULL;           // F106765 RERC 05/30/2006
   }

//=======================================================================
// FUNCTION NAME    load
//
// DESCRIPTION      Load settings from a file.
//=======================================================================

Boolean Profile :: load(IString& strName)
   {
   SAVESTR savestr;

#ifndef BR_PROFILE  /* F62721 EDW 01/21/2002 */
   IString strExecPath = MainWindow::strExecPath;
#else
   IString strExecPath = ProfileDialog::strExecPath;
#endif

   //IProfile profile(STR_Profile_INI);
   /*  F62721  EDW 01/21/2002 begin
   IString sProfileName = IProfile::userProfile().name() + STR_PathDelimiter + STR_Profile_INI;    //48163
   IProfile profile(sProfileName);
   */
   HKEY brKey = 0, verKey = 0, saveKey = 0;
   DWORD dataSize = MAX_KEY_PATH, dwType = REG_SZ;
   char  data [MAX_KEY_PATH];
   RegCreateKeyEx(HKEY_CURRENT_USER, REGPATH, 0, 0, 0,
                     KEY_ALL_ACCESS | KEY_WRITE, 0, &brKey, 0);
   /*  F62721  EDW 01/21/2002 end */
   char input[256], default_data[256]; //flw Usability

   //edw-Add profile version key if it does not exist and delete saved
   //    profile information
   /*  F62721  EDW 01/21/2002 begin
   if ( !profile.containsKeyName(STR_ProfileVersion_Key, STR_Profile_App) )
      {
      if ( profile.containsKeyName(STR_Profile_Key, STR_Profile_App) )
          profile.deleteElementWithKey( STR_Profile_Key, STR_Profile_App );
      profile.addOrReplaceElementWithKey(STR_ProfileVersion_Key, STR_ProfileVersion, STR_Profile_App);
      }
   */
   if ( RegOpenKeyEx(brKey, STR_ProfileVersion_Key,
                     0, KEY_ALL_ACCESS | KEY_WRITE, &verKey) != ERROR_SUCCESS )
      {
      if ( RegOpenKeyEx(brKey, STR_Profile_Key,
                        0, KEY_ALL_ACCESS | KEY_WRITE, &saveKey) == ERROR_SUCCESS )
         {
         RegDeleteValue(saveKey, (LPCTSTR)"0");
         RegDeleteValue(saveKey, (LPCTSTR)"DataType");
         }
      RegCreateKeyEx(brKey, STR_ProfileVersion_Key, 0, 0, 0,
                     KEY_ALL_ACCESS | KEY_WRITE, 0, &verKey, 0);
      RegSetValueEx(verKey, "Data", 0, REG_SZ, (CONST BYTE*)STR_ProfileVersion,
                    IString(STR_ProfileVersion).length() + 1);
      }
   /*  F62721  EDW 01/21/2002 end */

   //edw-Update profile version key and delete saved profile information if
   //    the profile version has changed
   else
      {
      /*  F62721  EDW 01/21/2002 begin
      IString strProfileVersion = profile.elementWithKey(STR_ProfileVersion_Key, STR_Profile_App);
      if( strProfileVersion != STR_ProfileVersion )
         {
         if ( profile.containsKeyName(STR_Profile_Key, STR_Profile_App) )
            profile.deleteElementWithKey( STR_Profile_Key, STR_Profile_App );
         profile.addOrReplaceElementWithKey(STR_ProfileVersion_Key, STR_ProfileVersion, STR_Profile_App);
         }
      }
      */
      RegQueryValueEx(verKey, "Data", 0, &dwType, (LPBYTE)data, &dataSize);
      IString strProfileVersion = (IString)data;
      if( strProfileVersion != STR_ProfileVersion )
         {
         if ( RegOpenKeyEx(brKey, STR_Profile_Key,
                           0, KEY_ALL_ACCESS | KEY_WRITE, &saveKey) == ERROR_SUCCESS )
            {
            RegDeleteValue(saveKey, (LPCTSTR)"0");
            RegDeleteValue(saveKey, (LPCTSTR)"DataType");
            }
         RegSetValueEx(verKey, "Data", 0, REG_SZ, (CONST BYTE*)STR_ProfileVersion,
                       IString(STR_ProfileVersion).length() + 1);
         }
      }
      /*  F62721  EDW 01/21/2002 end */

   /*  F62721  EDW 01/21/2002 begin
   if ( !profile.containsKeyName(STR_Profile_Key, STR_Profile_App) )
   */
   if ( RegOpenKeyEx(brKey, STR_Profile_Key,
                     0, KEY_ALL_ACCESS | KEY_WRITE, &saveKey) != ERROR_SUCCESS )
   /*  F62721  EDW 01/21/2002 end */
      {
      //edw03/15/99-Added block below
      IString strDefaultDir = strExecPath.stripTrailing( STR_PathDelimiter);;
      strDefaultDir.remove( strDefaultDir.lastIndexOf(STR_PathDelimiter) + 1 );
      strDefaultDir = strDefaultDir + "UserData";
      WIN32_FIND_DATA fileData;
      HANDLE handle = FindFirstFile( (LPCTSTR)strDefaultDir, &fileData );
      if ( handle == INVALID_HANDLE_VALUE )
         {
         //Use execution path
         strBrowsePath = strExecPath.stripTrailing( STR_PathDelimiter);
         strGeneratePath = strBrowsePath;
         }
      else
         {
         //Use '\econfig\userdata' if it exists
         strBrowsePath = strDefaultDir;
         strGeneratePath = strBrowsePath;
         }
      FindClose( handle );

      //strBrowsePath = strExecPath.stripTrailing( STR_PathDelimiter);   //jt3/17/98
      //strGeneratePath = strExecPath.stripTrailing( STR_PathDelimiter); //jt3/17/98
      save(strName);
      }
   /*  F62721  EDW 01/21/2002 begin
   IString strSave = profile.elementWithKey(STR_Profile_Key, STR_Profile_App);
   memcpy(&savestr, (char*)strSave, sizeof(SAVESTR));
   */
   dataSize = MAX_KEY_PATH;
   memset(data, 0, sizeof(data));
   dwType = REG_BINARY;
   brKey = saveKey = 0;

   RegCreateKeyEx(HKEY_CURRENT_USER, REGPATH, 0, 0, 0,
                  KEY_ALL_ACCESS | KEY_WRITE, 0, &brKey, 0);
   RegCreateKeyEx(brKey, STR_Profile_Key, 0, 0, 0,
                  KEY_ALL_ACCESS | KEY_WRITE, 0, &saveKey, 0);
   int iRC = RegQueryValueEx(saveKey, "0", 0, &dwType, (LPBYTE)data, &dataSize);

   if (brKey)
      RegCloseKey(brKey);
   if (verKey)
      RegCloseKey(verKey);
   if (saveKey)
      RegCloseKey(saveKey);

   memcpy(&savestr, (char*)data, sizeof(SAVESTR));
   /*  F62721  EDW 01/21/2002 end */

   for ( USHORT usIndex = 0; usIndex < WINDOWCOUNT; usIndex++ )
      {
      position[usIndex].setX(savestr.lLeft[usIndex]);
      position[usIndex].setY(savestr.lBottom[usIndex]);
      size[usIndex].setWidth(savestr.lWidth[usIndex]);
      size[usIndex].setHeight(savestr.lHeight[usIndex]);
      }

   //Read showS390 registry key value
   strcpy(input, STR_Key_ShowS390);
   strcpy(default_data, "1");
   setupKeyData(input, default_data);
    if ( !strcmp( default_data, "1") )
       bShowS390Mismatches = true;
     else
       bShowS390Mismatches = false;

   IString eConfig = IApplication::current().argv(2);
   eConfig = eConfig.upperCase();
   if (eConfig == ECONFIG)
       bEconfig = true;


   strcpy(input, STR_Profile_FastPath);
   strcpy(default_data, STR_FastPath_Off);
   setupKeyData(input, default_data);    // Get fastpath value from registry
    if ( !strcmp( default_data, STR_FastPath_Off) )
       bFastPath = false;
     else
       bFastPath = true;

// F62721 EDW 01/21/2002
   strcpy(input, STR_Key_RefreshList);
   strcpy(default_data, "0");
   setupKeyData(input, default_data);    // Get RefreshList value from registry
   if ( !strcmp( default_data, "0") )
      bRefreshList = false;
   else
      bRefreshList = true;
// F62721 EDW 01/21/2002

// F62721 EDW 01/21/2002
   strcpy(input, STR_Key_RefreshLocale);
   strcpy(default_data, "0");
   setupKeyData(input, default_data);    // Get RefreshLocale value from registry
   if ( !strcmp( default_data, "0") )
      bRefreshLocale = false;
   else
      bRefreshLocale = true;
// F62721 EDW 01/21/2002
//===================================================================================================================
// F106765 RERC 05/29/2006 BEGIN. Logic to write as default "ub9host4.exe" in the "RetSource" (Retrieval Source)
//         of the "\HKEY_CURRENT_USER\Software\IBM\econfig_profile\BaseRetrieval\BaseRet" registry
//         if the user geography is "US".

// F127433 ELRR: Begin logic: Set the 'Inventory services host interface' as default for all geographies when
//                                 the 'VM Host' option is selected.
// if (pRetrSource->getUserGeography() == STR_US)
// {
     HKEY hKey;
     TCHAR szRetSources[1024];
     DWORD dwBufLen = 1024;

     if ( RegOpenKeyEx(HKEY_CURRENT_USER, REGPATH, 0, KEY_READ | KEY_WRITE , &hKey) == ERROR_SUCCESS)
       if (RegQueryValueEx(hKey, STR_Profile_RetSource,
                        NULL, NULL, (LPBYTE)szRetSources, &dwBufLen) == ERROR_SUCCESS)
          if (IString(szRetSources).upperCase() == STR_VMHost || IString(szRetSources)== STR_Blank1)
          {
              strRetSources = IString(STR_IServices);
              RegSetValueEx(hKey, STR_Profile_RetSource, 0, REG_SZ, (CONST BYTE*)strRetSources, strRetSources.length()+1);
          }
     RegCloseKey (hKey);
// }
//F127433 End Logic:ELRR 11/12/08

   // F106765 RERC 05/29/2006 END.
//===================================================================================================================
// ZOR 48548 280600
   strcpy(input, STR_Profile_Expanded);
   strcpy(default_data, STR_Expanded_On);
   setupKeyData(input, default_data);    // Get Expanded value from registry
    if ( !strcmp( default_data, STR_Expanded_Off) )
       bExpanded = false;
     else
       bExpanded = true;
// ZOR 48548 280600

// ZOR IC01552 02202001
   strcpy(input, STR_ViewSort);
   strcpy(default_data, STR_ViewSortName);
   setupKeyData(input, default_data);    // Get Sort value from registry
    if ( !strcmp( default_data, STR_ViewSortDate) )
       bSort = false;
     else
       bSort = true;
// ZOR  IC01552 02202001
// ZOR IC01552 02202001
   strcpy(input, STR_ViewSortAscending);
   strcpy(default_data, STR_ViewSortAscend);
   setupKeyData(input, default_data);    // Get Sort value from registry
    if ( !strcmp( default_data, STR_ViewSortDescend) )
       bSascending = false;
     else
       bSascending = true;
// ZOR  IC01552 02202001


//47971 begin new logic MAMT 21/08/2000
   strcpy(input, STR_Profile_Tile);
   strcpy(default_data, STR_Tile_Hor);
   setupKeyData(input, default_data);    // Get Tile value from registry
    if ( !strcmp( default_data, STR_Tile_Ver) )
       bTile = false;
     else
       bTile = true;
//47971 end new logic MAMT 21/08/2000

   strcpy(input, STR_Profile_MRPD);
   strcpy(default_data, STR_MRPD_Yes);
   setupKeyData(input, default_data);    // Get MRPD preference from registry
    if ( !strcmp( default_data, STR_MRPD_Yes) )
       bMRPD = true;
     else
       bMRPD = false;

   //*  57146  HECC:  07/24/2001
   //*  Set to zero as this value is not used any more
 //  bMRPD = false;      //ZOR 91005 was commented because this could change when zseries is selected

   strcpy(input, STR_Profile_Browser);

// ELRR-F133122 Begin:
// strcpy(default_data, STR_Profile_Netscape);
   strcpy(default_data, STR_Profile_Firefox);
// ELRR-F133122 End.

   setupKeyData(input, default_data);    // Get browser preference from registry

	// GTL Req1702 BEGIN:
	if ( !strcmp( default_data, STR_Profile_Chrome ) )
        strBrowser = STR_Profile_Chrome;		
	// GTL Req1702 END
	
	 // ELRR-F133122 Begin:
//  if ( !strcmp( default_data, STR_Profile_Netscape ) )
//      strBrowser = STR_Profile_Netscape;
     else if ( !strcmp( default_data, STR_Profile_Firefox ) )
		strBrowser = STR_Profile_Firefox;
	// ELRR-F133122 End.
	
	 else
        strBrowser = STR_Profile_IE;
   //Detrmine what browsers are available on the desktop
    BrowserSupported();
	
//GTL Req1702 BEGIN:
  // ELRR-F133122 Begin: Determine if browser in registry is actually supported
    // if (!bFirefoxSupported && strBrowser == STR_Profile_Firefox) 
	 if (!bChromeSupported && strBrowser == STR_Profile_Chrome)
        strBrowser = STR_Profile_IE;
      else if (!bIESupported && strBrowser == STR_Profile_IE)
	    strBrowser = STR_Profile_Firefox;
		
//  if (!bNetscapeSupported && strBrowser == STR_Profile_Netscape)
	  else if (!bFirefoxSupported && strBrowser == STR_Profile_Firefox)
	    strBrowser = STR_Profile_Chrome;
// ELRR-F133122 End.

	  else if (!bIESupported && !bChromeSupported)
        strBrowser = STR_Profile_NoBrowser;	  
// GTL Req1702 END

   strcpy(input, STR_Profile_RetProd_Types);
   strcpy(default_data, STR_RetProd_Both);
   setupKeyData(input, default_data);    // Get product type(s) preferences from registry
   if ( !strcmp( default_data, STR_RetProd_Both ) )
        {
         bHardware = true;
         bSoftware = true;
         }
      else
        {
         if ( !strcmp( default_data, STR_RetProd_SW ) )
            {
             bHardware = false;
             bSoftware = true;
             }
            else
               {
               if ( !strcmp( default_data, STR_RetProd_HW ) )
                 {
                  bHardware = true;
                  bSoftware = false;
                 }
           }
     }

   //Test to see if PCOM is installed on the users machine
   PCOMMSupported();

   strcpy(input, STR_Profile_RetSource);
   strcpy(default_data, STR_LocalFiles);
   setupKeyData(input, default_data);    //Get retrieval source from  preference from registry

   if ((!bPComSupported && bPassportSupported) && (strcmpi(default_data, STR_VMHost) == 0))
      strRetApp = default_data;
    else if (!bPComSupported && (strcmpi(default_data, STR_VMHost) == 0))
      strRetApp = STR_LocalFiles;
    else
      //strcpy(strRetApp, default_data);  //Retrival App is eqaul to registry value
      strRetApp = default_data;

   strcpy(input, STR_Profile_AutoRec);
   strcpy(default_data, STR_AutoRec_Off);
   setupKeyData(input, default_data);    // Get Auto Reconcile preference from registry
    if ( !strcmp( default_data, STR_AutoRec_On) )
       bAutoRec = true;                                       //47698 - flw
     else
       bAutoRec = false;

   strcpy(input, STR_Profile_ProcRPO);
   strcpy(default_data, STR_ProcRPO_Off);
   setupKeyData(input, default_data);    // Get Process RPO preference from registry
    if ( !strcmp( default_data, STR_ProcRPO_On) )
       bProcRPO = true;                                       //47700 - flw
     else
       bProcRPO = false;

  strcpy(input, STR_Profile_Family);
  strcpy(default_data, STR_S390);       // Get Product Family from registry
  setupKeyData(input, default_data);                          //47701 - flw
  strProdFamily = default_data;

   sSortFiles = savestr.sSortFiles;
   sSortOrders = savestr.sSortOrders;
   //bHardware = savestr.bHardware;   // Commented out for 38935 - Usability
   //bSoftware = savestr.bSoftware;   // flw 9/15/99

   /*  F62721  EDW 01/21/2002 begin
   strSave.remove(1, sizeof(SAVESTR));
   */
   char* pstrSave;
   pstrSave = data + sizeof(SAVESTR);
   IString strSave;
   strSave = pstrSave;
   /*  F62721  EDW 01/21/2002 end */
   ULONG ulIndex = 1;
   ULONG ulOffset = strSave.indexOf(STR_ProfileDelimiter, ulIndex);
   strLocale = strSave.subString(ulIndex, ulOffset-ulIndex);
   strMRI = strLocale;
   strMRI.insert(STR_Prefix);
   ulIndex = ulOffset+1;
   ulOffset = strSave.indexOf(STR_ProfileDelimiter, ulIndex);
   strBrowsePath = strSave.subString(ulIndex, ulOffset-ulIndex);
   ulIndex = ulOffset+1;
   ulOffset = strSave.indexOf(STR_ProfileDelimiter, ulIndex);
   strGeneratePath = strSave.subString(ulIndex, ulOffset-ulIndex);

   //* D73565  Begin Logic:  In case that the Browse Path and the Generate Path
   //* end with a '/' character, this must be removed as it causes problems when
   //* the file path is tested if exists or not:  is evaluated as not existing
   //* HECC: 10/02/2002
   strBrowsePath.stripTrailing( STR_PathDelimiter);
   strGeneratePath.stripTrailing( STR_PathDelimiter);
   //* D73565  Ends Logic   HECC: 10/02/2002


   //Commented for Usability 38935 - flw
   //ulIndex = ulOffset+1;
   //ulOffset = strSave.indexOf(STR_ProfileDelimiter, ulIndex);
   //strRetApp = strSave.subString(ulIndex, ulOffset-ulIndex);

   // PSC 69271 12/04/2003 start
   // ---------Un-necessary evaluation------------
   // At this point the Retrieval Path was previ--
   // ously evaluated, it means the Directory al--
   // ready exist.--------------------------------

   //struct stat fstat;
   //if ( stat( strBrowsePath, &fstat) )
   //strBrowsePath = strExecPath.stripTrailing( STR_PathDelimiter);   //jt3/17/98
   //if ( stat( strGeneratePath, &fstat) )
   //strGeneratePath = strExecPath.stripTrailing( STR_PathDelimiter);   //jt3/17/98
   // PSC 69271 12/04/2003 end

   return true;
   }

//=======================================================================
// FUNCTION NAME    save
//
// DESCRIPTION      Save settings to a file.
//=======================================================================

Boolean Profile :: save(IString& strName)
   {
   SAVESTR savestr;

   DWORD DataSize = 256;
   HKEY key;
   char  Data[256];

   memset(&savestr, 0, sizeof(SAVESTR));
   for ( USHORT usIndex = 0; usIndex < WINDOWCOUNT; usIndex++ )
      {
      savestr.lLeft[usIndex] = position[usIndex].x();
      savestr.lBottom[usIndex] = position[usIndex].y();
      savestr.lWidth[usIndex] = size[usIndex].width();
      savestr.lHeight[usIndex] = size[usIndex].height();
      }
   savestr.sSortFiles = sSortFiles;
   savestr.sSortOrders = sSortOrders;
   //savestr.bHardware = bHardware;
   //savestr.bSoftware = bSoftware;
   IString strSave(&savestr, sizeof(SAVESTR));
   strSave += strLocale + STR_ProfileDelimiter;
   strSave += strBrowsePath + STR_ProfileDelimiter;
   strSave += strGeneratePath + STR_ProfileDelimiter;
   //strSave += strRetApp + STR_ProfileDelimiter;

   //Save showS390 registry key value
   //if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)
   if (RegOpenKeyEx (HKEY_CURRENT_USER, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)     //48163 - flw
      {
      if (bShowS390Mismatches)
         RegSetValueEx(key, STR_Key_ShowS390, 0, REG_SZ, (CONST BYTE*)"1", 2);
      else
         RegSetValueEx(key, STR_Key_ShowS390, 0, REG_SZ, (CONST BYTE*)"0", 2);
      }
   RegCloseKey (key);

   //Save HW/SW flags
   //if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)   //48163 - flw
   if ( RegOpenKeyEx(HKEY_CURRENT_USER, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)
     {
      if (bSoftware && bHardware)
         RegSetValueEx(key, STR_Profile_RetProd_Types, 0, REG_SZ, (CONST BYTE*)STR_RetProd_Both, IString(STR_RetProd_Both).length()+1);
        if (bHardware && !bSoftware)
          RegSetValueEx(key, STR_Profile_RetProd_Types, 0, REG_SZ, (CONST BYTE*)STR_RetProd_HW, IString(STR_RetProd_HW).length()+1);
         if (!bHardware && bSoftware)
            RegSetValueEx(key, STR_Profile_RetProd_Types, 0, REG_SZ, (CONST BYTE*)STR_RetProd_SW, IString(STR_RetProd_SW).length()+1);
     RegCloseKey (key);
      }


  //Usability  Save Retrieval source
  //if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)
  if ( RegOpenKeyEx(HKEY_CURRENT_USER, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)      //48163 - flw
      {
       RegSetValueEx(key, STR_Profile_RetSource, 0, REG_SZ, (CONST BYTE*)strRetApp, strRetApp.length()+1);
       }
   RegCloseKey (key);

  //Save FastPath value to register
  //if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)
  if (RegOpenKeyEx (HKEY_CURRENT_USER, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)      //48163 - flw
     {
       if (bFastPath)
         RegSetValueEx(key, STR_Profile_FastPath, 0, REG_SZ, (CONST BYTE*)STR_FastPath_On, IString(STR_FastPath_On).length()+1);
        else
          RegSetValueEx(key, STR_Profile_FastPath, 0, REG_SZ, (CONST BYTE*)STR_FastPath_Off, IString(STR_FastPath_Off).length()+1);
      }
   RegCloseKey (key);
  //ZOR 48548
  //Save Expand_Collapse value to register
  if (RegOpenKeyEx (HKEY_CURRENT_USER, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)      //48163 - flw
     {
       if (bExpanded)
         RegSetValueEx(key, STR_Profile_Expanded, 0, REG_SZ, (CONST BYTE*)STR_Expanded_On, IString(STR_Expanded_On).length()+1);
        else
          RegSetValueEx(key, STR_Profile_Expanded, 0, REG_SZ, (CONST BYTE*)STR_Expanded_Off, IString(STR_Expanded_Off).length()+1);
      }
   RegCloseKey (key);

  //ZOR IC01552
  //Save Sort by Name or Date value to register
  if (RegOpenKeyEx (HKEY_CURRENT_USER, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)      //48163 - flw
     {
       if (bSort)
         RegSetValueEx(key, STR_ViewSort, 0, REG_SZ, (CONST BYTE*)STR_ViewSortName, IString(STR_ViewSortName).length()+1);
        else
          RegSetValueEx(key, STR_ViewSort, 0, REG_SZ, (CONST BYTE*)STR_ViewSortDate, IString(STR_ViewSortDate).length()+1);
      }
   RegCloseKey (key);
  // End of code IC01552
  //ZOR IC01552
  //Save Ascending or Descending value to register
  if (RegOpenKeyEx (HKEY_CURRENT_USER, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)      //48163 - flw
     {
       if (bSascending)
         RegSetValueEx(key, STR_ViewSortAscending, 0, REG_SZ, (CONST BYTE*)STR_ViewSortName, IString(STR_ViewSortAscend).length()+1);
        else
          RegSetValueEx(key, STR_ViewSortAscending, 0, REG_SZ, (CONST BYTE*)STR_ViewSortDate, IString(STR_ViewSortDescend).length()+1);
      }
   RegCloseKey (key);
  // End of code IC01552

  //47971 begin new logic MAMT 21/08/2000
  //Save Tile_Hor_Ver value to register
  if (RegOpenKeyEx (HKEY_CURRENT_USER, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)      //48163 - flw
     {
       if (bTile)
         RegSetValueEx(key, STR_Profile_Tile, 0, REG_SZ, (CONST BYTE*)STR_Tile_Hor, DataSize);
        else
          RegSetValueEx(key, STR_Profile_Tile, 0, REG_SZ, (CONST BYTE*)STR_Tile_Ver, DataSize);
      }
   RegCloseKey (key);
//47971 end new logic MAMT 21/08/2000

  //Save MRPD value to register
  //if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)    //48163 - flw
  if (RegOpenKeyEx (HKEY_CURRENT_USER, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)
     {
       if (bMRPD)
         RegSetValueEx(key, STR_Profile_MRPD, 0, REG_SZ, (CONST BYTE*)STR_MRPD_Yes, IString(STR_MRPD_Yes).length()+1);
        else
          RegSetValueEx(key, STR_Profile_MRPD, 0, REG_SZ, (CONST BYTE*)STR_MRPD_No, IString(STR_MRPD_No).length()+1);
      }
   RegCloseKey (key);

  //Save Browser value to register
  //if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)    //48163 - flw
   if (RegOpenKeyEx (HKEY_CURRENT_USER, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)
         {

// GTL Req1702 BEGIN:		 
		if (strBrowser == STR_Profile_Chrome )
		// ELRR-F133122 Begin: Change made to allow firefox registry key to be included instead of Netscape.
//      RegSetValueEx(key, STR_Profile_Browser, 0, REG_SZ, (CONST BYTE*)STR_Profile_Netscape, IString(STR_Profile_Netscape).length()+1);
        // RegSetValueEx(key, STR_Profile_Browser, 0, REG_SZ, (CONST BYTE*)STR_Profile_Firefox, IString(STR_Profile_Firefox).length()+1);
		RegSetValueEx(key, STR_Profile_Browser, 0, REG_SZ, (CONST BYTE*)STR_Profile_Chrome, IString(STR_Profile_Chrome).length()+1);

        else if (strBrowser == STR_Profile_Firefox )
		RegSetValueEx(key, STR_Profile_Browser, 0, REG_SZ, (CONST BYTE*)STR_Profile_Firefox, IString(STR_Profile_Firefox).length()+1);
// ELRR-F133122 End.		
//GTL Req1702 END:
        else
		RegSetValueEx(key, STR_Profile_Browser, 0, REG_SZ, (CONST BYTE*)STR_Profile_IE, IString(STR_Profile_IE).length()+1);
      }
   RegCloseKey (key);
   
  //Save Auto Reconcile value to register
  if (RegOpenKeyEx (HKEY_CURRENT_USER, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)
     {
       if (bAutoRec)                                                                                      //47698 - flw
         RegSetValueEx(key, STR_Profile_AutoRec, 0, REG_SZ, (CONST BYTE*)STR_AutoRec_On, IString(STR_AutoRec_On).length()+1);
        else
          RegSetValueEx(key, STR_Profile_AutoRec, 0, REG_SZ, (CONST BYTE*)STR_AutoRec_Off, IString(STR_AutoRec_Off).length()+1);
      }
   RegCloseKey (key);

   //Save Process RPO value to register
  if (RegOpenKeyEx (HKEY_CURRENT_USER, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)
     {
       if (bProcRPO)                                                                                      //47700 - flw
         RegSetValueEx(key, STR_Profile_ProcRPO, 0, REG_SZ, (CONST BYTE*)STR_ProcRPO_On, IString(STR_ProcRPO_On).length()+1);
        else
          RegSetValueEx(key, STR_Profile_ProcRPO, 0, REG_SZ, (CONST BYTE*)STR_ProcRPO_Off, IString(STR_ProcRPO_Off).length()+1);
      }
   RegCloseKey (key);

  //Save Product Family to register
  if (RegOpenKeyEx (HKEY_CURRENT_USER, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)    //47701 - flw
     {
      RegSetValueEx(key, STR_Profile_Family, 0, REG_SZ, (CONST BYTE*)strProdFamily, strProdFamily.length()+1);
      }
   RegCloseKey (key);

   //Save bRefreshList registry key value - F62721 EDW 01/21/2002
   if (RegOpenKeyEx (HKEY_CURRENT_USER, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)
      {
      if (bRefreshList)
         RegSetValueEx(key, STR_Key_RefreshList, 0, REG_SZ, (CONST BYTE*)"1", 2);
      else
         RegSetValueEx(key, STR_Key_RefreshList, 0, REG_SZ, (CONST BYTE*)"0", 2);
      }
   RegCloseKey (key);

   //Save bRefreshLocale registry key value - F62721 EDW 01/21/2002
   if (RegOpenKeyEx (HKEY_CURRENT_USER, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)
      {
      if (bRefreshLocale)
         RegSetValueEx(key, STR_Key_RefreshLocale, 0, REG_SZ, (CONST BYTE*)"1", 2);
      else
         RegSetValueEx(key, STR_Key_RefreshLocale, 0, REG_SZ, (CONST BYTE*)"0", 2);
      }
   RegCloseKey (key);

#ifndef BR_PROFILE  /* F62721 EDW 01/21/2002 */
   IString strExecPath = MainWindow::strExecPath;
#else
   IString strExecPath = ProfileDialog::strExecPath;
#endif

   //IProfile profile(STR_Profile_INI);
   /*  F62721  EDW 01/21/2002 begin
   IString sProfileName = IProfile::userProfile().name() + STR_PathDelimiter + STR_Profile_INI;     //48163 - flw
   IProfile profile(sProfileName);
   profile.addOrReplaceElementWithKey(STR_Profile_Key, strSave, STR_Profile_App);
   */
   HKEY brKey = 0, saveKey = 0;
   RegCreateKeyEx(HKEY_CURRENT_USER, REGPATH, 0, 0, 0,
                  KEY_ALL_ACCESS | KEY_WRITE, 0, &brKey, 0);
   RegCreateKeyEx(brKey, STR_Profile_Key, 0, 0, 0,
                  KEY_ALL_ACCESS | KEY_WRITE, 0, &saveKey, 0);
   int iRC = RegSetValueEx(saveKey, "0", 0, REG_BINARY, (CONST BYTE*)strSave,
                 sizeof(SAVESTR) + strLocale.length() + strBrowsePath.length()
                 + strGeneratePath.length() + 8);
   if (brKey)
      RegCloseKey(brKey);
   if (saveKey)
      RegCloseKey(saveKey);
   /*  F62721  EDW 01/21/2002 end */
   return true;
   }


//**********************************************************************
//  BrowserSupported searches for the supported browsers in the
//  system registry database.
//  Usability - 38935
//  Function rewritten completely - D67437 (iak)
//  If lower versions of Netscape not found, check for Netscape6 - D67437
//**********************************************************************

void Profile :: BrowserSupported()
{
    const int SIZE_FILENAME  = 256;
    DWORD ValueType, DataSize = 256;
    typedef char BUFFER_FILE[SIZE_FILENAME];
    BUFFER_FILE  tempBuf;
    struct stat status;
    char  Data[256];
    HKEY key;

    // First see whether Netscape browser exists on user's machine

    /*Logic: First see whether registry key for Netscape 4.7x and lower versions
    contains the filename of the browser's exe. If such a key is found, retrieve the
    filename and make sure that file exists. If yes, then Netscape browser supported.
    If no registry key for Netscape 4.7x, then check for a key for Netscape 6 and
    repeat above procedure. If Netscape 6 registry is also not found then Netscape
    is not supported.*/
	
//GTL Req1702 BEGIN: Added variable for Chrome instead of Firefox
//    ELRR-F133122 Begin: Variable for Firefox is used intead of Netscape.
//    bNetscapeSupported = false;
	  bChromeSupported = false;
//  if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, RegKey_Netscape, 0, KEY_READ, &key) == ERROR_SUCCESS)
    // if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, RegKey_Firefox, 0, KEY_READ, &key) == ERROR_SUCCESS)
	if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, RegKey_Chrome, 0, KEY_READ, &key) == ERROR_SUCCESS)

       { // Chrome filename registry key found

       if (RegQueryValueEx (key, "", NULL, &ValueType, (LPBYTE)Data, &DataSize) == ERROR_SUCCESS)
          { // Chrome filename retrieved from registry
          strcpy(tempBuf, Data);
          if (stat((char *)tempBuf, &status) != -1)
             {
             // bFirefoxSupported = true;
			 bChromeSupported = true;
      //    ELRR-F133122 End.
	  //GTL Req1702 END
             }
          }
       RegCloseKey(key);
       }
	   
/*  ELRR-F133122 Begin: Unable this part due Netscape 6.0 version will be no longer used.
    else if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, RegKey_Netscape6, 0, KEY_READ, &key) == ERROR_SUCCESS)
       { // Netscape 6 filename's registry key found
       if (RegQueryValueEx (key, "", NULL, &ValueType, (LPBYTE)Data, &DataSize) == ERROR_SUCCESS)
          { // Netscape filename retrieved from registry
          strcpy(tempBuf, Data);
          if (stat((char *)tempBuf, &status) != -1)
             { // exe for Netscape 6 exists
             bNetscapeSupported = true;
             }
          }
       RegCloseKey(key);
       }
    ELRR-F133122 END. */
    else
	//GTL Req1702 BEGIN:
//     bNetscapeSupported = false;   // No registry keys containing filenames of Netscape exe exist
       // bFirefoxSupported = false;   // No registry keys containing filenames of Firefox exe exist
		bChromeSupported = false;
		//GTL Req1702 END.

    // Now check whether IE browser is installed on user's machine

    // Logic: Open registry key containing filename of IE exe. Retrieve filename. Verify file
    // exists on user's machine. If yes, then IE supported, otherwise IE not supported.

// GTL Req1702 BEGIN:	
   bFirefoxSupported = false;
   DataSize = 256; //reinitializing value after it is changed in above registry calls

    if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, RegKey_Firefox, 0, KEY_READ, &key) == ERROR_SUCCESS)
       { // Firefox filename registy key found
       if (RegQueryValueEx (key, "", NULL, &ValueType, (LPBYTE)Data, &DataSize) == ERROR_SUCCESS)
          { // firefox exe filename retrieved from registry
          strcpy(tempBuf, Data);
          if (stat((char *)tempBuf, &status) != -1)
             { // firefox for IE exists
             bFirefoxSupported = true;
             }
          }
        RegCloseKey(key);
        }
    else
       bFirefoxSupported = false;
//GTL Req1702 END
	   
    bIESupported = false;
    DataSize = 256; //reinitializing value after it is changed in above registry calls

    if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, RegKey_IExplorer, 0, KEY_READ, &key) == ERROR_SUCCESS)
       { // IE filename registy key found
       if (RegQueryValueEx (key, "", NULL, &ValueType, (LPBYTE)Data, &DataSize) == ERROR_SUCCESS)
          { // IE exe filename retrieved from registry
          strcpy(tempBuf, Data);
          if (stat((char *)tempBuf, &status) != -1)
             { // exe for IE exists
             bIESupported = true;
             }
          }
        RegCloseKey(key);
        }
    else
       bIESupported = false;  // No registry key containing filename of IE exe exists

  }
  
  

//**********************************************************************
//  PCOMMSupported searches for the IBM Personal Communications
//  in the system registry to determine if the user has the application
//  installed, if not the user will not be given the oppurtunity to
//  retrieve from VM. flw - 44549
//***********************************************************************

void Profile :: PCOMMSupported()
{
    const int SIZE_FILENAME  = 256;
    DWORD ValueType, DataSize = 256;
    DWORD ValueType1, DataSize1 = 256;
    typedef char BUFFER_FILE[SIZE_FILENAME];
    BUFFER_FILE  tempBuf = "";
    struct stat status;
    char  Data[256];
    char Data1[256];
    HKEY key;
    /* Begin new logic:  HECC: IC00242  07/18/2001 */

    /* char  *p, *newData;  */

    char *p;
    char *newData = NULL;

    /* End new logic:    HECC: IC00242  07/18/2001 */

    long retval, RetVal, iDataNeeded, ipath, idefault;           // ZOR 69791
    ULONG ulppcom = 0;                         // ZOR 69791
    ULONG uldpcom = 0;                         // ZOR 69791
    IString sShortData = "";        // ZOR 69791
    if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, RegKey_PCOMM, 0, KEY_READ, &key) == ERROR_SUCCESS)
      {
       // starting fix 69791
         idefault = RegQueryValueEx (key, "", NULL, &ValueType, (LPBYTE)Data, &DataSize);
         if( idefault == ERROR_SUCCESS)
           {
             sShortData = Data;
             sShortData = sShortData.strip().upperCase();
             uldpcom = sShortData.indexOf(RegKey_PCOMM_short);
             if (uldpcom == 0)
                {
                 ipath = RegQueryValueEx (key, RegKey_PCOMM_path, NULL, &ValueType1, (LPBYTE)Data1, &DataSize1);
                 if (ipath == ERROR_SUCCESS)
                   {
                   sShortData = Data1;
                   sShortData = sShortData.strip().upperCase();
                   ulppcom = sShortData.indexOf(RegKey_PCOMM_short);
                   ULONG ulsemicol = sShortData.indexOf(";");
                   if (ulsemicol != 0)
                     sShortData.remove(ulsemicol);
                   sShortData = sShortData.strip();
                   }
                }
              else
                 {
                  ULONG ulsemicol = sShortData.indexOf(";");
                  if (ulsemicol != 0)
                    sShortData.remove(ulsemicol);
                  sShortData = sShortData.strip();
                 }
            }
            strcpy(tempBuf, sShortData);
         // ending fix 69791
            int zstatus = stat((char *)tempBuf, &status);
            if (stat((char *)tempBuf, &status) != -1)
                {
                 bPComSupported = true;
                 RegCloseKey(key);
                }
       }
 /*   if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, RegKey_PCOMM, 0, KEY_READ, &key) == ERROR_SUCCESS)
       {
          if (RegQueryValueEx (key, "", NULL, &ValueType, (LPBYTE)Data, &DataSize) == ERROR_SUCCESS)
           {
            strcpy(tempBuf, Data);
             if (stat((char *)tempBuf, &status) != -1)
               {
                bPComSupported = true;
                RegCloseKey(key);
                }
            }
        }
  */
     if (RegOpenKeyEx (HKEY_CURRENT_USER, RegKey_Passport, 0, KEY_READ, &key) == ERROR_SUCCESS)
          {
           retval = RegQueryValueEx (key, STR_Passport_Key, NULL, &ValueType, (LPBYTE)Data, &DataSize);
             if (retval == ERROR_SUCCESS)            //IC42054 - flw
                {
                 bPassportSupported = true;
                 RegCloseKey(key);
                 }
                  else
                     {
                      if (retval == ERROR_MORE_DATA)  /* reissue RegQuery call */
                         {
                           iDataNeeded = DataSize;

                           /* Begin new logic:  HECC: IC00242  07/18/2001 */
                           /* Note:  As no returned data is required from RegQueryValueEx, next */
                           /* instruction is no longer required.   In  declaration  of  newData */
                           /* pointer, it's being set as NULL                                   */

                           /* newData = new char(iDataNeeded ); */

                           /* End new logic:    HECC: IC00242  07/18/2001 */

                           RetVal = RegQueryValueEx(key, STR_Passport_Key, NULL, &ValueType, (LPBYTE)newData, (LPDWORD) &iDataNeeded);
                           if (RetVal == ERROR_SUCCESS)
                             {
                              bPassportSupported = true;
                              RegCloseKey(key);
                             }
                        //   else
                        //     { //51747 - MAMT 09/06/00
                        //     if (RegOpenKeyEx (HKEY_CURRENT_USER, RegKey_Passport2000, 0, KEY_READ, &key) == ERROR_SUCCESS)
                        //        {
                        //         retval = RegQueryValueEx (key, STR_Passport_Key, NULL, &ValueType, (LPBYTE)Data, &DataSize);
                        //         if (retval == ERROR_SUCCESS)
                        //            {
                        //             bPassportSupported = true;
                        //             RegCloseKey(key);
                        //             }
                        //         else
                        //           {
                        //           if (retval == ERROR_MORE_DATA)  /* reissue RegQuery call */
                        //             {
                        //              iDataNeeded = DataSize;
                        //              newData = new char(iDataNeeded );
                        //              RetVal = RegQueryValueEx(key, STR_Passport_Key, NULL, &ValueType, (LPBYTE)newData, (LPDWORD) &iDataNeeded);
                        //              if (RetVal == ERROR_SUCCESS)
                        //               {
                        //                 bPassportSupported = true;
                        //                 RegCloseKey(key);
                        //                }
                        //             }
                        //           }
                        //        }
                        //     }  //51747 - MAMT 09/06/00

                         }
                     }
          }


// ZOR 51747a
   if (RegOpenKeyEx (HKEY_CURRENT_USER, RegKey_Passport2000, 0, KEY_READ, &key) == ERROR_SUCCESS)
       {
         retval = RegQueryValueEx (key, STR_Passport_Key, NULL, &ValueType, (LPBYTE)Data, &DataSize);
         if (retval == ERROR_SUCCESS)
            {
             bPassportSupported = true;
             RegCloseKey(key);
            }
          else
            {
             if (retval == ERROR_MORE_DATA)  /* reissue RegQuery call */
                {
                 iDataNeeded = DataSize;

                 /* Begin new logic:  HECC: IC00242  07/18/2001 */
                 /* Note:  As no returned data is required from RegQueryValueEx, next */
                 /* instruction is no longer required.   In  declaration  of  newData */
                 /* pointer, it's being set as NULL                                   */

                 /* newData = new char(iDataNeeded ); */

                 /* End new logic:    HECC: IC00242  07/18/2001 */

                 RetVal = RegQueryValueEx(key, STR_Passport_Key, NULL, &ValueType, (LPBYTE)newData, (LPDWORD) &iDataNeeded);
                 if (RetVal == ERROR_SUCCESS)
                    {
                     bPassportSupported = true;
                     RegCloseKey(key);
                    }
                 }
            }
        }
}
//=======================================================================
//  FUNCTION NAME setupKeyData
//
//  DESCRIPTION   searches for a given key ( input ) in the registry.  If
//                the key doesn't exist, it is created and assigned the
//                given default value ( default_data ).  If the key exists,
//                its value is retrieved and written to 'default_data'.
// PROGRAMMER     James Hsieh
//=======================================================================
boolean Profile :: setupKeyData (char * input, char *default_data)
   {
   IString QryData;
   DWORD ValueType, DataSize = 1024;
   char buf[35];
   char  *Data = (char *)malloc(1024);
   char  *p, *newData;
   long retval, RetVal, iDataNeeded;
   HKEY key;

   boolean rc = true;

   //if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, REGPATH, 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS)     //48163 - flw
   if (RegOpenKeyEx (HKEY_CURRENT_USER, REGPATH, 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS)
      {
      retval = RegQueryValueEx(key, input, NULL, &ValueType, (LPBYTE)Data, &DataSize);

      p = _ltoa(retval, buf, 10);

      if (retval == ERROR_SUCCESS)
         strcpy(default_data, Data);
      else
         {
         if (retval == ERROR_FILE_NOT_FOUND)  /* Set registry to default_data value */
            {
            RegSetValueEx(key, input , 0, REG_SZ, (CONST BYTE*)default_data, strlen(default_data)+1);
            RegCloseKey (key);
            }
         else
            {
            if (retval == ERROR_MORE_DATA)  /* reissue RegQuery call */
               {
               iDataNeeded = DataSize;
               newData = new char(iDataNeeded*2 );
               RetVal = RegQueryValueEx(key, input /* STR_Profile_FastPath */, NULL, &ValueType, (LPBYTE)newData, (LPDWORD) &iDataNeeded);
               if (RetVal == ERROR_SUCCESS)
                  {
                  strcpy(default_data, newData);
                  }
               else
                  {
                  rc = false;
                  strcpy(default_data, "Query Error");
                  }
               delete newData;
               }
            }
         }
      RegCloseKey (key);
      }
   else
      {
         //cout << "Unable to open key" << endl;
         rc = false;
      }

   free(Data);
   return rc;

   }

//=======================================================================
// FUNCTION NAME    changeLocationOfKeys
//
// DESCRIPTION      Move registry keys to new location:
//                  SOFTWARE\\IBM\\econfig_profile\\BaseRetrieval
//                  F62721 EDW 01/21/2002
//=======================================================================

void Profile :: changeLocationOfKeys()
   {
   HKEY  key = 0;
   HKEY  newKey = 0;
   DWORD dwDisposition;
   DWORD retCode = ERROR_SUCCESS;
   DWORD dwType = REG_SZ;
   DWORD dataSize = MAX_KEY_PATH;
   char  data [MAX_KEY_PATH];

   data[0] = '\0';

   /* Define Base Retrieval root keys */

   IString strOldRootKey = REGPATH_OLD;
   IString strNewRootKey = REGPATH;

   /* Traverse up one level to get the root Base Retrieval key */

   int iIdx = strOldRootKey.lastIndexOf(STR_PathDelimiter);
   strOldRootKey = strOldRootKey.subString(1, iIdx-1);

   iIdx = strNewRootKey.lastIndexOf(STR_PathDelimiter);
   strNewRootKey = strNewRootKey.subString(1, iIdx-1);

   /* Define RM2 root keys */

   IString strOldRM2RootKey = STR_RegKey_RM2_OLD;
   IString strNewRM2RootKey = STR_RegKey_RM2;


   /* Copy all Base Retrieval keys to new location */

   if ( RegOpenKeyEx(HKEY_CURRENT_USER, strOldRootKey,
                     0, KEY_ALL_ACCESS | KEY_WRITE, &key) == ERROR_SUCCESS )
      {
      retCode = RegQueryValueEx(key, STR_KEY_DELETED, 0,
                              &dwType, (LPBYTE)data, &dataSize);
      if (retCode != ERROR_SUCCESS)
         {
         RegCreateKeyEx(HKEY_CURRENT_USER, strNewRootKey, 0, 0, 0,
                     KEY_ALL_ACCESS | KEY_WRITE, 0, &newKey, &dwDisposition);
         if ( copyRegKeys(key, newKey) == ERROR_SUCCESS )
            {
            markRegKeysAsDeleted(strOldRootKey);
            }
         }
      }
   if (key)
      RegCloseKey(key);
   if(newKey)
      RegCloseKey(newKey);

    key = newKey = 0;

   /* Copy all RM2 keys to new location */

   if ( RegOpenKeyEx(HKEY_CURRENT_USER, strOldRM2RootKey,
                     0, KEY_ALL_ACCESS | KEY_WRITE, &key) == ERROR_SUCCESS )
      {
      retCode = RegQueryValueEx(key, STR_KEY_DELETED, 0,
                              &dwType, (LPBYTE)data, &dataSize);
      if (retCode != ERROR_SUCCESS)
         {
         RegCreateKeyEx(HKEY_CURRENT_USER, strNewRM2RootKey, 0, 0, 0,
                     KEY_ALL_ACCESS | KEY_WRITE, 0, &newKey, &dwDisposition);
         if ( copyRegKeys(key, newKey) == ERROR_SUCCESS )
            {
            markRegKeysAsDeleted(strOldRM2RootKey);
            }
         }
      }
   if (key)
      RegCloseKey(key);
   if(newKey)
      RegCloseKey(newKey);
   }

//=======================================================================
// FUNCTION NAME    changeLocationOfRM2Keys                             =
//                                                                      =
// DESCRIPTION      Move the RM2 Registry Keys to new location.         =
//                  From:                                               =
//                  Software\\IBM/_econfig_profile/_BF9RM2              =
//                  To:                                                 =
//                  Software\\IBM\\econfig_profile\\BF9RM2              =
//                  D80040 PSC 10/07/2003                               =
//=======================================================================

void Profile :: changeLocationOfRM2Keys()
   {
   /*          Define local variables              */
   HKEY  key = 0;
   HKEY  newKey = 0;
   DWORD dwDisposition;
   DWORD retCode = ERROR_SUCCESS;
   DWORD dwType = REG_SZ;
   DWORD dataSize = MAX_KEY_PATH;
   char  data [MAX_KEY_PATH];

   data[0] = '\0';


   /*          Define RM2 root keys                */

   /* Assign the erroneous RM2 Registry Entry Path */
   IString strOldRM2RootKey = REGPATH_RM2_OLD;
   /* Assign the right RM2 Registry Entry Path     */
   IString strNewRM2RootKey = REGPATH_RM2_NEW;
   /* Assign the complete RM2 Registry Entry Path  */
   IString strDelRM2RootKey = REGPATH_RM2_DEL;


   /*   Copy all RM2 keys to new location          */

   /*   Open the the erroneous RM2 Registry Entry where the Keys are located    */
   /*   At this time it should be created.                                      */
   if ( RegOpenKeyEx(HKEY_CURRENT_USER, strOldRM2RootKey,
                     0, KEY_ALL_ACCESS | KEY_WRITE, &key) == ERROR_SUCCESS )
      {
      retCode = RegQueryValueEx(key, STR_KEY_DELETED, 0,
                              &dwType, (LPBYTE)data, &dataSize);
      if (retCode != ERROR_SUCCESS)
         {
         /*   The Right RM2 Registry Entry is created    */
         RegCreateKeyEx(HKEY_CURRENT_USER, strNewRM2RootKey, 0, 0, 0,
                     KEY_ALL_ACCESS | KEY_WRITE, 0, &newKey, &dwDisposition);
         /*   All Registry Keys are copied to new location   */
         if ( copyRegKeys(key, newKey) == ERROR_SUCCESS )
            {
            /*   Mark the Registry Entry as DELETED     */
            markRegKeysAsDeleted(strOldRM2RootKey);
            }

         /* Each Key is deleted before the RM2 Root Registry Entry */
         /* It is important to remove first all Entry keys before  */
         /* the RM2 Root Registry Entry                            */
         RegDeleteKey(HKEY_CURRENT_USER, strDelRM2RootKey + REGPATH_RM2_DEL_DSPLYALL);
         RegDeleteKey(HKEY_CURRENT_USER, strDelRM2RootKey + REGPATH_RM2_DEL_DSPLYDSCRP);
         RegDeleteKey(HKEY_CURRENT_USER, strDelRM2RootKey + REGPATH_RM2_DEL_PKFILE);
         RegDeleteKey(HKEY_CURRENT_USER, strDelRM2RootKey + REGPATH_RM2_DEL_PRCSSAPPLYRSLVDFEAT);
         RegDeleteKey(HKEY_CURRENT_USER, strDelRM2RootKey + REGPATH_RM2_DEL_PRCSSDSPLYEDITWND );
         RegDeleteKey(HKEY_CURRENT_USER, strDelRM2RootKey + REGPATH_RM2_DEL_PRCSSGENRPO);
         RegDeleteKey(HKEY_CURRENT_USER, strDelRM2RootKey + REGPATH_RM2_DEL_PRCSSGENRPOAUTO);
         RegDeleteKey(HKEY_CURRENT_USER, strDelRM2RootKey + REGPATH_RM2_DEL_PRCSSGENRPOLIST );
         RegDeleteKey(HKEY_CURRENT_USER, strDelRM2RootKey + REGPATH_RM2_DEL_USERNAME);
         RegDeleteKey(HKEY_CURRENT_USER, strDelRM2RootKey + REGPATH_RM2_DEL_USERTIELINE);
         RegDeleteKey(HKEY_CURRENT_USER, strDelRM2RootKey + REGPATH_RM2_DEL_USERVMID );
         RegDeleteKey(HKEY_CURRENT_USER, strDelRM2RootKey + REGPATH_RM2_DEL_USERVMNMODE);
         RegDeleteKey(HKEY_CURRENT_USER, strDelRM2RootKey + REGPATH_RM2_DEL_VIEWDSCRPFEATINFO);
         RegDeleteKey(HKEY_CURRENT_USER, strDelRM2RootKey + REGPATH_RM2_DEL_VIEWEDITWNDINFO);
         RegDeleteKey(HKEY_CURRENT_USER, strDelRM2RootKey + REGPATH_RM2_DEL_VIEWGENRPO);
         RegDeleteKey(HKEY_CURRENT_USER, strDelRM2RootKey + REGPATH_RM2_DEL_VIEWGENRPOINFO);
         RegDeleteKey(HKEY_CURRENT_USER, strDelRM2RootKey + REGPATH_RM2_DEL_VIEWPRFLWNDINFO);
         RegDeleteKey(HKEY_CURRENT_USER, strDelRM2RootKey + REGPATH_RM2_DEL_VIEWRSLVDFEAT);
         RegDeleteKey(HKEY_CURRENT_USER, strDelRM2RootKey + REGPATH_RM2_DEL_VIEWRSLVDFEATINFO);
         RegDeleteKey(HKEY_CURRENT_USER, strDelRM2RootKey + REGPATH_RM2_DEL_WORKDIR);

         // The RM2 Root Registry Entry is deleted
         RegDeleteKey(HKEY_CURRENT_USER, strDelRM2RootKey);
         RegDeleteKey(HKEY_CURRENT_USER, strOldRM2RootKey);


         }
      }

   /* If keys still open, the Registry Entry is closed            */
   if (key)
      RegCloseKey(key);
   if(newKey)
      RegCloseKey(newKey);


   }


//=======================================================================
// FUNCTION NAME    markRegKeysAsDeleted
//
// DESCRIPTION      Mark an old set of keys as 'deleted'.
//                  F62721 EDW 01/21/2002
//=======================================================================
int Profile :: markRegKeysAsDeleted(IString oldKey)
{
   HKEY     key;
   DWORD    retCode;

   if ( ( retCode = RegOpenKeyEx(HKEY_CURRENT_USER, oldKey,
                     0, KEY_ALL_ACCESS | KEY_WRITE, &key) ) == ERROR_SUCCESS )
   {
      retCode = RegSetValueEx(key, STR_KEY_DELETED, 0,
                              REG_SZ, (CONST BYTE*)STR_VALUE_DELETED,
                              strlen(STR_VALUE_DELETED) + 1);
   }
   RegCloseKey(key);
   return retCode;
}

//=======================================================================
// FUNCTION NAME    copyRegKeys
//
// DESCRIPTION      Copy the registry keys to a new location.
//                  F62721 EDW 01/21/2002
//=======================================================================
int Profile :: copyRegKeys(HKEY hKey, HKEY hNewKey)
{
   DWORD    i, j;
   DWORD    retCode, retValue;
   DWORD    dwDisposition;

   /* Enumerate all the subkeys and create new
      versions of the subkeys in location, hNewKey
    */

   char     achKey[MAX_KEY_PATH];              /* subkey name being queried     */
   DWORD    cchKey = MAX_KEY_PATH;             /* length of subkey string       */
   FILETIME ftLastWriteTime;                   /* last write time               */
   HKEY     nextKey;                           /* next subkey to be queried     */
   HKEY     nextNewKey;                        /* next new subkey to create     */
   int      iRC = ERROR_SUCCESS;

   for ( i = 0, retCode = ERROR_SUCCESS;
       retCode == ERROR_SUCCESS; i++ )
   {
      achKey[0] = '\0';
      cchKey = MAX_VALUE_NAME;

      retCode = RegEnumKeyEx(hKey, i, achKey, &cchKey,
                             0, 0, 0, &ftLastWriteTime);

      if ( retCode == ERROR_SUCCESS )
      {
         /* Recursively create the all new subkeys */

         if ( ( retCode = RegOpenKeyEx( hKey, achKey, 0,
                                      KEY_ALL_ACCESS | KEY_WRITE,
                                      &nextKey) ) == ERROR_SUCCESS )
         {
            retCode = RegCreateKeyEx( hNewKey, achKey, 0, 0, 0,
                                      KEY_ALL_ACCESS | KEY_WRITE,
                                      0, &nextNewKey, &dwDisposition);

            retCode = copyRegKeys(nextKey, nextNewKey);
         }
      }
   }


   /* Enumerate the key values and copy them
      to the new key just created
    */

   char     achValue[MAX_VALUE_NAME];          /* name of the value             */
   DWORD    cchValue = MAX_VALUE_NAME;         /* length of value               */
   DWORD    cchDataType = MAX_VALUE_NAME;      /* type code for the value       */
   char     achData[MAX_VALUE_NAME];           /* data for the value            */
   DWORD    cchData = MAX_VALUE_NAME;          /* length of data value          */

   for ( j = 0, retValue = ERROR_SUCCESS;
       retValue ==  ERROR_SUCCESS; j++ )
   {
      cchValue = MAX_VALUE_NAME;
      cchData = MAX_VALUE_NAME;
      cchDataType = MAX_VALUE_NAME;
      achValue[0] = '\0';
      achData[0] = '\0';
      retValue = RegEnumValue(hKey, j, achValue,
                              &cchValue,
                              NULL,
                              &cchDataType,
                              (LPBYTE)achData,
                              &cchData);

      if ( retValue == ERROR_SUCCESS )
      {
         /* copy key value to new location */

         retValue = RegSetValueEx(hNewKey, achValue, 0, cchDataType,
                                  (CONST BYTE*)achData, cchData + 1);
      }
   }
   RegCloseKey(hKey);
   RegCloseKey(hNewKey);

   if (retValue == ERROR_NO_MORE_ITEMS)
      iRC = ERROR_SUCCESS;
   else
      iRC = retValue;

   return iRC;
}

//=======================================================================
// FUNCTION NAME    ~Profile
//
// DESCRIPTION      Delete any objects created by the constructor.
//=======================================================================

Profile :: ~Profile()
   {
     delete pRetrSource;   //F106765 RERC 06/06/2006
   }
