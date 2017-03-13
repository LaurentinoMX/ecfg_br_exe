//=======================================================================
// FILE NAME        AF9MAIN.CPP
//
// CLASS NAME       MainWindow
//
// DESCRIPTION      This class is the main frame window of the Apply
//                  Orders CPF.
//=======================================================================
// Copyright (c) International Business Machines Corp. 1996
// All rights reserved
//=======================================================================
//  48163 - Forrest Walker  e-Config needed to have the registry keys reside
//                          HKEY_CURRENT_USER instead of HKEY_LOCAL_MACHINE
//                          so that user on NT would not have write access
//                          error if they do not have Admin. rights on machine
// D54593 - E. Wilson       (09/21/00)Fixed logic so the ProductDetail is
//                          correctly displayed when the S390 Family is
//                          selected in the application's preferences.
// D65574 - HECC            (11/14/01)The logic to display the sort order
//                          in the VIew option of BaseRet menu was changed so
//                          in case that the flag pWindow->bLock is ON (wich
//                          mean that BaseRet is locked by the UB9HOST1 interface)
//                          the options IDMI_ViewSortName and IDMI_ViewSortDate
//                          be disabled instead of the complete View option Menu
//                          IDMI_ViewSort.
// F62721 - E. Wilson       Omitted event handling for the following menuItems:
//                          "Change retrieval path" submenu
//                          "View" menu
//                          All submenus in "View" menu
//                          All submenus in "Options" menu
//                          Added event handling for the "Preferences" menuItem
//                          in the "Options" menu.  This menuItem will invoke
//                          the Base Retrieval Profile DLL (BF9PROFL.DLL) which
//                          now contains the logic for managing all preferences
//                          in Base Retrieval.  All future preference logic should
//                          go into BF9PROFL.DLL instead of in BF9BRET.EXE
//                          Also added call to method, changeLocationOfKeys,
//                          to copy the registry keys to a new root location:
//                          SOFTWARE\\IBM\\econfig_profile\\BaseRetrieval
// D72623 - E. Wilson       Fixed logic error in regenerateSerialNbrs() which was
//                          causing valid serial numbers in the MRD report to be
//                          incorrectly regenerated and the same machine to show up
//                          twice in the base system.  Also, fixed the following
//                          problems in openHost():
//                             Intermediate files created during reconciliation are
//                             not deleted (added cleanTempFiles method)
// D72481 - E. Wilson       The user's reconciled CFR is not being loaded when the
//                          AAS report has a filetype is .ADM.  This causes various
//                          error conditions to occur
// F72273 - C. Herrera      Added logic to check if the actual configuration is
//                          iSeries or not and in case validation be true,
//                          necessary logic to load the new iSeries SW features table
//                          is added.
// D73492 - V. Bevill       Changed the select files dialog to the default modal setting.
//
// ICA68115 - C. Herrera    No helpbutton and Windows explorer style
//                          in the 'Save file' and 'Save As' FileDialogs
// 52646  - Z. Obregon      program doesn't delete .RET file. the if statment was changed
//                          to check if the lRC value is 0 then exist and can be deleted.
// 79548  - C. Herrera      a) Adds the Product Family profile value to the parameter
//                             passed to the methods that load the tables quickfix.tbl,
//                             nocomp.tbl and iseriesw.tbl, to correctly load them
//                             from the directory where these tables are stored.
//
//                          b) Adds the call to ChangePKFilePath. This routine adds
//                             the product family variable to the path where the file
//                             feature.$$f is located to load the one that corresponds
//                             to the Product Family profile preference.
//
// 81710  - C. Herrera      Changed logic to read the Product Knowledge files from the
//                          model directory indicated in the Parsed parameters from eConfig
//                          in case Base Retrieval be called from here.  If Base Retrieval
//                          was started as stand alone, the Product Knowledge files will
//                          be loaded from the model that the profile indicates.
//
// 81952  - C. Herrera      Parsed parameter from eConfig was incorrectly read.
//                          The selected configuration model from eConfig is in the
//                          1st position, but logic tried to read it in the 2nd position
// D73383 - C. Herrera      Added necessary logic to update the pref flags after the
//                          profile structure has been loaded.
// D81872 - C. Herrera      Added logic to change the path of the Host interface
//                          executable from a long to short path name.
//                          (c:\Program Files\eConfig\cfgviews\ub9host3.exe will be
//                           converted to: c:\Progra~1\eConfig\cfgviews\ub9host3.exe)
// D84922 - V. Bevill       Allow BR to continue processing even if 128 is received from
//                          _spawnl as a return code.  Need to investigate this further when
//                          time allows.
// D80040 - P. Sánchez      New function added in the profile and invoked in here to move --
//    10/07/2003            the RM2 Registry Entries located in the wrong path: ------------
//                          "HKEY_CURRENT_USER\\Software\\IBM\\_econfig_profile\\_BF9RM2".--
//                          to the right one: ----------------------------------------------
//                          "HKEY_CURRENT_USER\\Software\\IBM\\econfig_profile\\BF9RM2".----
// D85278 - C. Herrera      Condition that allows RC = 128 will be disabled as Ub9host4.exe
//                          is now returning correctly a RC = 0 for a successfull execution.
// D87694 - C. Herrera      Added logic to convert long path names to short path names.
//                          This is used to correctly load filenames which contains spaces
//                          in them and avoid execution errors.
//                          (e.g. c:\Program Files\eConfig will be converted to: c:\Progra~1\eConfig)
// F88290 - C. Herrera      Added condition to display the "Reconcile the retrieved data?" window
//                          only in case that the retrieved configuration be other than an S390.
//                          Otherwise, avoid display it and automatically reconcile configuration.
//
// F86752 - Z. Obregon      MRPD+ Convergence (Reconciliation mods), handle 06 records, data reconciled
//      March-2004          Inventory Services
// F94021 - Z. Obregon      store the mrd file name in the variable this->strMRD    is commented becauseof 95887
//      15/10/2004
// ICD22680351 - C. Herrera Added logic to write the .RET file with the lastest changes to the
//                          Assigned/Unassigned products in the Systems Window
//
// F100160 - C. Herrera     Add the necessary logic to display the error messages and stop the process
//      07/26/2005          if the input configuration belongs to Blue Horizon or xSeries family.
// F104628 - E. Reyes       Modify the load of the nocomp.tbl file to considerate the mismatch
//      01/30/2006          type information.
// D106661 - E. Reyes       Add logic to validate the memory allocated for a pointer
//      03/14/2006          into the ConvertToShortPathName function.
// ICE27674825  05/15/2006  The ConvertToShortPathName function was modified to back out the fix for
//                          D106661, the function was reestablished as it was before.
// D106661A - E. Reyes      The logic for the output path for the ConvertToShortPath function
//      05/17/2006          is changed due to the ShortPath sometimes is bigger than input path
//                          (e.g. c:\econfig_test\5.1.39\2066.cfr is converted to
//                                c:\econf_1~1\51B19D~1.39\2066.CFR)
//========================================================================================================
#pragma comment ( copyright , "(C) Copyright IBM Corporation 1999. All Rights Reserved." )

#define INCL_WINCOUNTRY                 // WinQueryCp
#define INCL_WINDIALOGS                 // WinAlarm
#define INCL_WINSTDFILE                 // File dialog
#define READBUF_SIZE1 10240             //53915a - MAMT 02/01/2001
#include <os2.h>

#include <process.h>                    // _spawnlp
#include <stdio.h>                      // tmpnam
#include <stdlib.h>                     // getenv, memcpy, memcmp
#include <iostream.h>                   // lineFrom         //edw 12/4/97
#include <fstream.h>                    // ifstream         //edw 12/4/97
#include <string.h>                     // strcpy
#include <sys\types.h>                  // stat
#include <sys\stat.h>                   // stat
#include <direct.h>                     // stat

#include <iaccel.hpp>                   // IAccelerator
#include <iapp.hpp>                     // IApplication
#include <icnrcol.hpp>                  // IContainerColumn
#include <icnrctl.hpp>                  // IContainerControl
#include <icnrhdr.hpp>                  // ICnrHandler
#include <icnrmhdr.hpp>                 // ICnrMenuHandler
#include <icombobx.hpp>                 // IComboBox
#include <ientryfd.hpp>                 // IEntryField
#include <ifiledlg.hpp>                 // IFileDialog
#include <ifilehdr.hpp>                 // IFileDialogHandler
#include <ifont.hpp>                    // IFont
#include <ifontdlg.hpp>                 // IFontDialog
#include <iframe.hpp>                   // IFrameWindow
#include <iframhdr.hpp>                 // IFrameHandler
#include <ihelp.hpp>                    // IHelpWindow
#include <imenubar.hpp>                 // IMenuBar
#include <imsgbox.hpp>                  // IMessageBox
#include <ipopmenu.hpp>                 // IPopUpMenu
#include <iprofile.hpp>                 // IProfile
#include <irect.hpp>                    // IRectangle
#include <ireslib.hpp>                  // IResourceLibrary/IResourceId
#include <isizehdr.hpp>                 // IResizeHandler
#include <istattxt.hpp>                 // IStaticText
#include <isubmenu.hpp>                 // ISubMenu
#include <isysmenu.hpp>                 // ISystemMenu
#include <ithread.hpp>                  // IThread
#include <ititle.hpp>                   // ITitle

#include <locale.h>                     // setlocale, localeconv

#include "af9main.hpp"                  // MainWindow
#include "rtrnret.hpp"                  // Return Retrieval window 53363 - ZOR
#include "af9profl.hpp"                 // Profile
#include "afcbkend.hpp"                 // BackEnd
#include "af9ordrs.hpp"                 // OrdersNotebook
#include "af9systm.hpp"                 // SystemsNotebook
#include "af9detl.hpp"                  // DetailWindow
#include "af9name.hpp"                  // FileNameDialog
#include "af9ption.hpp"                 // OptionsDialog
#include "af9local.hpp"                 // LocaleDialog
#include "af9apply.h"                   // Resource ids
#include "af9rtsrc.hpp"                 // RetrSource            //edw 09/02/98
#include "af9recon.hpp"                 // ReconcileData         //edw 06/22/98
#include "afcv2031.hpp"                 // Convert2031           //edw 12/17/97
#include "ub9host.h"                    // Host interface tags
#include "mainw.hpp"                    // RM/2 main window      //jt,5/29/97
#include "mainw.h"                      // RM/2 resource ids     //jt,5/29/97
#include "prf.hpp"                      // RM/2 profile          //jt,5/29/97
#include "prfw4.hpp"                    // RM/2 profile          //jt,5/29/97
//#include "prfw4.h"                      // RM/2 profile        //jt,5/29/97
#include "bridge.h"                     // common data variable public to RM/2  //jt,7/3/97
#include "bf9profl.h"                   // BF9PROFL.DLL APIs     // F62721 EDW 01/21/2002

// MSN 1/27/97
#define THREAD_STACK_SIZE               50000

const char * RegKey_BaseRet_Old = "SOFTWARE\\BASERETRIEVAL\\BASERET";              //edw06/11/99 //62721 EDW 01/21/2002
const char * RegKey_BaseRet     = "SOFTWARE\\IBM\\econfig_profile\\BaseRetrieval\\BaseRet";      //62721 EDW 01/21/2002
Boolean UpdateBIR(IString FileName);
IString CFRDummy = "C:\\CFRtemp.cfr";
Boolean is1704=false;

//=======================================================================
// FUNCTION NAME    main
//
// DESCRIPTION      Declare an instance of the MainWindow class and start
//                  the message-processing loop.
//=======================================================================

ULONG main(SHORT argc, PCHAR argv[])
   {
   ULONG ulReturnCode = false;
   try
      {

      // Set Presentation Manager Control Resource Library
     if(access((IString("cppwor3u") + ".dll"),0)==0)
        IApplication::current().setResourceLibrary("cppwor3u");

      // Save any command line arguments.

      IApplication::current().setArgs(argc, argv);

      // Create the main window object.

      MainWindow mainWin(IDW_AFOAPPLY);

      // Enter the message processing loop.

      IApplication::current().run();
      ulReturnCode = mainWin.ulReturnCode;
      }
   catch(IException& exc)
      {
      IMessageBox msg(IWindow::desktopWindow());
      msg.show(exc);
      }
   return ulReturnCode;
   }

IString MainWindow::strExecPath;

//=======================================================================
// FUNCTION NAME    MainWindow
//
// DESCRIPTION      Construct an instance of the MainWindow class.
//=======================================================================

MainWindow :: MainWindow(ULONG ulWindow)
            : IFrameWindow("", IResourceId(ulWindow))
   {

   setId(IDW_AFOAPPLY);

   IHelpWindow::setDefaultStyle( IHelpWindow::defaultStyle()
                               | IHelpWindow::ipfCompatible ); // MSN

   // Initialize global variables.

   ptxProgress = new IStaticText(999, this, this, IRectangle(),
                                 IStaticText::defaultStyle() |
                                 IStaticText::vertCenter);
   ptxProgress->hide();
   pFile = (CfReportFileStats*)NULL;
   pOrders = (OrdersNotebook*)NULL;
   pSystems = (SystemsNotebook*)NULL;
   pDetail = (DetailWindow*)NULL;
   pFileName = (FileNameDialog*)NULL;
   pRec = (ReconcileData*)NULL;        //47698 - flw
   pOptions = (OptionsDialog*)NULL;
   pRetSrc = (RetrSource*)NULL;
   pFeature54Table = new ISortedSet<IString>; //53915-MAMT-09/01/2000
   bLock = false;
   bMinimized = false;
   ulReturnCode = false;
   bValid = false;
   bConfig = false;//jt-7/25/97
   bOnlyRename = false; // 49608 - MAMT

   bHostUsed = false;//R15170

   // Change location of registry keys to new location, if
   // they have not already been moved (F62721 EDW 01/21/2002)

   Profile::changeLocationOfKeys();

   //Set application execution path attribute

   IString strAppName = IApplication::current().argv(0);
   ULONG ulOffset = strAppName.lastIndexOf(STR_PathDelimiter);
   if ( ulOffset )
      strAppName = strAppName.subString( ulOffset + 1 );
   ulOffset = strAppName.lastIndexOf(".");
   if ( !ulOffset )
      strAppName = strAppName.insert( STR_ExtensionEXE, strAppName.length() );

   char strE[100], strE1[100];
   LPTSTR *strE2 = (LPSTR*)strE1;
   SearchPath(NULL, strAppName, NULL, 99, strE, strE2);
   strExecPath = strE;

   ulOffset = strExecPath.lastIndexOf(STR_PathDelimiter);
   if ( ulOffset )
      strExecPath.remove(ulOffset+1);
   else
      strExecPath = "";

   if ( IApplication::current().argc() > 1 )
      {
      IString strArg = IApplication::current().argv(1);
      bConfig = true;  //edw02/16/99-argv(1) should always hold the model name
                       //unless invoked from ISM.  In which case argv(1) = SBOS2.
      /*bConfig = ( ( strArg == STR_ConfigApp ) ||
                  ( strArg == STR_S390 )      ||
                  ( strArg == STR_AS400 )     ||
                  ( strArg == STR_RS6000 ) );
      */

     //IString strFamily = strArg.stripTrailing(CHR_Underscore);     //47701 - flw
     IString strFamily = strArg.subString( strArg.lastIndexOf('_') );
      if ( bConfig )
         {
         //Set product model attribute

         strProductModel =  strArg.upperCase();

         // Reset RetFile profile keys

         /*  F62721  EDW 01/21/2002 begin
         IProfile::userProfile().addOrReplaceElementWithKey(STR_ConfigKey,
           STR_Blank2, STR_ConfigApp);
         */
         HKEY sboKey = 0;
         IString sboKeyName = STR_RegKey_eConfig + IString(STR_ConfigApp) +
                              IString(STR_PathDelimiter) + IString(STR_ConfigKey);
         RegCreateKeyEx(HKEY_CURRENT_USER, sboKeyName, 0, 0, 0,
                        KEY_ALL_ACCESS | KEY_WRITE, 0, &sboKey, 0);
         RegSetValueEx(sboKey, "Data", 0, REG_SZ, (CONST BYTE*)STR_Blank2,
                       IString(STR_Blank2).length() + 1);
         if (sboKey)
            RegCloseKey(sboKey);
         /*  F62721  EDW 01/21/2002 end */

         //edw06/11/99 replaced with block below
         //edw11/08/99 Portable configs still read this version of the "Retfile" registry key
         //IProfile profile(STR_Profile_INI);

         /*  F62721  EDW 01/21/2002 begin
         IString sProfileName = IProfile::userProfile().name() + STR_PathDelimiter + STR_Profile_INI;     //48163 - flw
         IProfile profile(sProfileName);
         if ( profile.containsKeyName(STR_ConfigKey, STR_Profile_App) )
            profile.addOrReplaceElementWithKey(STR_ConfigKey,
               STR_Blank1, STR_Profile_App);
         */
         HKEY brKey = 0;
         sboKey = 0;
         RegCreateKeyEx(HKEY_CURRENT_USER, REGPATH, 0, 0, 0,
                        KEY_ALL_ACCESS | KEY_WRITE, 0, &brKey, 0);
         if ( RegOpenKeyEx(brKey, STR_ConfigKey,
                           0, KEY_ALL_ACCESS | KEY_WRITE, &sboKey) == ERROR_SUCCESS )
            {
            RegSetValueEx(sboKey, "Data", 0, REG_SZ, (CONST BYTE*)STR_Blank1,
                          IString(STR_Blank1).length() + 1);
            }
         if (brKey)
            RegCloseKey(brKey);
         if (sboKey)
            RegCloseKey(sboKey);
         /*  F62721  EDW 01/21/2002 end */
                                                   //edw06/11/99 made "Retfile" a "StringValue" in the "BaseRet" key instead of
         //edw06/11/99 a subkey of the "BaseRet" key REQ by eConfig bridge code team.
         HKEY key;
         //if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, RegKey_BaseRet, 0, KEY_WRITE, &key) == ERROR_SUCCESS)
         if ( RegOpenKeyEx(HKEY_CURRENT_USER, RegKey_BaseRet, 0, KEY_WRITE, &key) == ERROR_SUCCESS)        //48163 - flw
            {
            RegSetValueEx(key, STR_ConfigKey, 0, REG_SZ, (CONST BYTE*)STR_Blank2, 256);
            RegCloseKey (key);
            }
         }
     }

   // Instantiate the user profile.

   bProfile = false;
   pProfile = new Profile();

   // Add handlers for command, frame, and resize messages.

   ICommandHandler::handleEventsFor(this);
   IFrameHandler::handleEventsFor(this);
   IResizeHandler::handleEventsFor(this);

   // Throw remaining startup processing into a secondary thread to
   // free the input queue.

   //postEvent(IWindow::command, UM_StartUp1, (ULONG)true);
   postEvent(IWindow::command, UM_StartUp1);
   }

//=======================================================================
// FUNCTION NAME    command
//
// DESCRIPTION      Handle menu commands.
//=======================================================================

Boolean MainWindow :: command(ICommandEvent& cmdEvt)
   {
   IThread thread;
   thread.setStackSize(THREAD_STACK_SIZE);

   LONG lCommand = cmdEvt.commandId();
   switch( lCommand )
      {

      case IDMI_ConfigRetrieve:
         if ( !confirmFileRetrieve() )
            break;
         strMismatches = "";      //D51647 - edw 09/06/00
         strUserChoices = "";     //D51647 - edw 09/06/00

//   if ( pProfile->strRetApp == STR_LocalFiles )  old logic 82420 MAMT 11/10/2000
         if ( IString(pProfile->strRetApp).upperCase() == IString(STR_LocalFiles).upperCase() ) // new logic 82420
            {
            usPath = enumFileConfig;
            selectDirectory(strBrowsePath);
            }
         else
            thread.start(new IThreadMemberFn<MainWindow>(*this, openHost));
         break;

#ifdef BR_OBSOLETE_MENU   /* F62721 EDW 01/21/2002 */
      case IDMI_ConfigBrowse:
         if ( !confirmFileSave() )
            break;
         //edw01/27/98-next 14 lines
         if (pOrders)
            {
            pOrders->clearNotebook();
            pOrders->hide();
            pOrders->removeFromWindowList();
            }
         if (pSystems)
            {
            pSystems->clearNotebook();
            pSystems->hide();
            pSystems->removeFromWindowList();
            }
         if (pFile)
            pFile = (CfReportFileStats*)NULL;

         usPath = enumPathBrowse;
         selectDirectory(strBrowsePath);
         break;
#endif
      //edw 12/2/97-added following case
      case IDMI_ConfigISMProceed:
         strGenerateName = pFile->name();
         postEvent(IWindow::command, UM_ReturnRetrieval);
         //openFileName(enumFileGenerate);
         break;

      //D50578 edw10/05/2000-added following case
      case IDMI_ConfigExit:
         postEvent(IWindow::systemCommand, ISystemMenu::idClose);
         break;

      case IDMI_SelectedOpen:
      case IDMI_PopupOpen:
         {
         //52012 begin new logic MAMT 08/10/2000
         IContainerControl::ObjectCursor crs(*pcnrFiles);
         for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
            {
            CfReportFileStats* pObject = (CfReportFileStats*)crs.current();
            if ( IString(pObject->name()).upperCase() ==  STR_RetrievalFile )
               {
               pcnrFiles->removeObject(pObject);
               IThread::current().sleep(500);//jt
               delete pObject;
               break;
               }
            }
         pcnrFiles->refresh();
         //52012 end new logic MAMT 08/10/2000
         pSelectedFile = (CfReportFileStats*)pcnrFiles->cursoredObject();
         if ( !pSelectedFile->usable() )
            {
            IMessageBox::Style style = IMessageBox::cancelButton |
                                       IMessageBox::warningIcon |
                                       IMessageBox::moveable |
                                       IMessageBox::systemModal ;
            IMessageBox msg(this);
            msg.setTitle(IResourceId(IDS_ErrorUnusable_Title));
            msg.show(IResourceId(pSelectedFile->uErrorCode),
                     style, IDH_ErrorUnusable);
            break;
            }
         Boolean bNew = ( pFile != pSelectedFile );
         if ( bNew )
            if ( !confirmFileSave() )
               break;
         bLock = true;
         setPointer(true);
         if ( pFile )
         pFile->setIcon(NULL);
         pFile = pSelectedFile;
         pFile->setIcon(IResourceId(IDI_ApplyOrders));
         pcnrFiles->refresh(true);     // added MSN 2/13/97 to change the icon
                                       // for the selected file
         if ( !pOrders )
            {
            pOrders = new OrdersNotebook(this);
            pOrders->moveTo(pProfile->position[WINDOWORDERS]);
            pOrders->sizeTo(pProfile->size[WINDOWORDERS]);
            pOrders->addToWindowList();
            }
         if ( !pSystems )
            {
            pSystems = new SystemsNotebook(this);
            pSystems->moveTo(pProfile->position[WINDOWSYSTEMS]);
            pSystems->sizeTo(pProfile->size[WINDOWSYSTEMS]);
            pSystems->addToWindowList();
            }

         if ( bNew )
            {
            if ( pDetail )
               { // MSN 2/26/97
               pDetail->sendEvent(IWindow::systemCommand, ISystemMenu::idClose);
               delete pDetail;   //edw10/20/98
               pDetail = NULL;   //edw10/20/98
               }
            pOrders->clearNotebook();
            pSystems->clearNotebook();
            pSystems->buildNotebook(pFile->Systems()+1);
            }
         strSaveName = strGenerateName = pFile->name();
         thread.setVariable(STR_Prefix, IString(bNew));
         thread.start(new IThreadMemberFn<MainWindow>(*this, openNotebooks));
         break;
         }

      case IDMI_SelectedOpen1:  //52012 MAMT 08/01/2000
         {
         IContainerControl::ObjectCursor crs(*pcnrFiles);
         for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
            {
            CfReportFileStats* pObject = (CfReportFileStats*)crs.current();
            if ( IString(pObject->name()).upperCase() ==  strSaveName )
               {
               pcnrFiles->removeObject(pObject);
               IThread::current().sleep(500);//jt
               delete pObject;
               break;
               }
            }
         pcnrFiles->refresh();

         IString tempfileN1 = strBrowsePath + STR_PathDelimiter + strSaveName +
                              STR_ExtensionBIR;
         IString tempfileN2 = strBrowsePath + STR_PathDelimiter + strSaveName +
                              STR_ExtensionBIR;
         remove(tempfileN1);
         remove(tempfileN2);

         CfReportFileStats* pNew = new CfReportFileStats(STR_RetrievalFile);
         pcnrFiles->addObject(pNew);
         rename(tempfileB, tempfileN1);

         rename(oldfileB, tempfileB);
         rename(oldfileR, tempfileR);

         pNew->sCfrFullFileName = tempfileN1;
         IThread::current().sleep(500);//jt
         sortFiles();

         for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
            {
            CfReportFileStats* pObject = (CfReportFileStats*)crs.current();
            if ( IString(pObject->name()).upperCase() ==  STR_RetrievalFile )
               {
               pcnrFiles->removeObject(pObject);
               IThread::current().sleep(500);//jt
               delete pObject;
               break;
               }
            }

         pcnrFiles->refresh();

         pSelectedFile->setName(strSaveName);
         pSelectedFile->refresh();
         if ( !pSelectedFile->usable() )
            {
            IMessageBox::Style style = IMessageBox::cancelButton |
                                       IMessageBox::warningIcon |
                                       IMessageBox::moveable;
            IMessageBox msg(this);
            msg.setTitle(IResourceId(IDS_ErrorUnusable_Title));
            msg.show(IResourceId(pSelectedFile->uErrorCode),
                     style, IDH_ErrorUnusable);
            break;
            }
         Boolean bNew = ( pFile != pSelectedFile );
         if ( bNew )
            if ( !confirmFileSave() )
               break;
         bLock = true;
         setPointer(true);
         if ( pFile )
         pFile->setIcon(NULL);
         pFile = pSelectedFile;
         pFile->setIcon(IResourceId(IDI_ApplyOrders));
         pcnrFiles->refresh(true);     // added MSN 2/13/97 to change the icon
                                       // for the selected file
         if ( !pOrders )
            {
            pOrders = new OrdersNotebook(this);
            pOrders->moveTo(pProfile->position[WINDOWORDERS]);
            pOrders->sizeTo(pProfile->size[WINDOWORDERS]);
            pOrders->addToWindowList();
            }
         if ( !pSystems )
            {
            pSystems = new SystemsNotebook(this);
            pSystems->moveTo(pProfile->position[WINDOWSYSTEMS]);
            pSystems->sizeTo(pProfile->size[WINDOWSYSTEMS]);
            pSystems->addToWindowList();
            }

         if ( bNew )
            {
            if ( pDetail )
               { // MSN 2/26/97
               pDetail->sendEvent(IWindow::systemCommand, ISystemMenu::idClose);
               delete pDetail;   //edw10/20/98
               pDetail = NULL;   //edw10/20/98
               }
            pOrders->clearNotebook();
            pSystems->clearNotebook();
            pSystems->buildNotebook(pFile->Systems()+1);
            }
         strSaveName = strGenerateName = pFile->name();
         thread.setVariable(STR_Prefix, IString(bNew));
         thread.start(new IThreadMemberFn<MainWindow>(*this, openNotebooks));
         break;
         }

      case IDMI_CloseFile:  //52012 MAMT 08/01/2000
         {
/*         pBackEnd->CloseFile(); */
         pFile = (CfReportFileStats*)NULL;

         remove(tempfileB);
         remove(tempfileR);
         rename(oldfileB, tempfileB);
         rename(oldfileR, tempfileR);
         // erase the last file created (the same name that the previous created in pcnrfiles
         IContainerControl::ObjectCursor crs(*pcnrFiles);
         for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
            {
            CfReportFileStats* pObject = (CfReportFileStats*)crs.current();
            if ( IString(pObject->name()).upperCase() ==  strSaveName )
               {
               pcnrFiles->removeObject(pObject);
               IThread::current().sleep(500);//jt
               delete pObject;
               break;
               }
            }
         pcnrFiles->refresh();
         // To assign the current values (pcnrfiles) to pselectedfile
         pSelectedFile = (CfReportFileStats*)pcnrFiles->cursoredObject();
         if ( !pSelectedFile->usable() )
            {
            IMessageBox::Style style = IMessageBox::cancelButton |
                                       IMessageBox::warningIcon |
                                       IMessageBox::moveable;
            IMessageBox msg(this);
            msg.setTitle(IResourceId(IDS_ErrorUnusable_Title));
            msg.show(IResourceId(pSelectedFile->uErrorCode),
                     style, IDH_ErrorUnusable);
            break;
            }
         Boolean bNew = ( pFile != pSelectedFile );
         if ( bNew )
            if ( !confirmFileSave() )
               break;
         bLock = true;
         setPointer(true);
         if ( pFile )
         pFile->setIcon(NULL);
         pFile = pSelectedFile;
         pFile->setIcon(IResourceId(IDI_ApplyOrders));
         pcnrFiles->refresh(true);     // added MSN 2/13/97 to change the icon
                                       // for the selected file
         if ( !pOrders )
            {
            pOrders = new OrdersNotebook(this);
            pOrders->moveTo(pProfile->position[WINDOWORDERS]);
            pOrders->sizeTo(pProfile->size[WINDOWORDERS]);
            pOrders->addToWindowList();
            }
         if ( !pSystems )
            {
            pSystems = new SystemsNotebook(this);
            pSystems->moveTo(pProfile->position[WINDOWSYSTEMS]);
            pSystems->sizeTo(pProfile->size[WINDOWSYSTEMS]);
            pSystems->addToWindowList();
            }

         if ( bNew )
            {
            if ( pDetail )
               { // MSN 2/26/97
               pDetail->sendEvent(IWindow::systemCommand, ISystemMenu::idClose);
               delete pDetail;   //edw10/20/98
               pDetail = NULL;   //edw10/20/98
               }
            pOrders->clearNotebook();
            pSystems->clearNotebook();
            pSystems->buildNotebook(pFile->Systems()+1);
            }
         strSaveName = strGenerateName = pFile->name();
         thread.setVariable(STR_Prefix, IString(bNew));
         thread.start(new IThreadMemberFn<MainWindow>(*this, openNotebooks));

         break;
         }

      case IDMI_SelectedSave:
      case IDMI_PopupSave:
         //strSaveName = pFile->name();
         strSaveName = pFile->name();
         strGenerateName = pFile->name();   //47705 - flw
         postEvent(IWindow::command, UM_SaveFile);
         postEvent(IWindow::command, UM_GenerateFile);  //47705
         break;

      case IDMI_SelectedSaveAs:
      case IDMI_PopupSaveAs:
         strSaveName = pFile->name();
         strGenerateName = pFile->name();   //47705 - flw
         openFileName(enumFileSave);
         break;

      case IDMI_SelectedGenerate:
      case IDMI_PopupGenerate:
         strGenerateName = pFile->name();
         openFileName(enumFileGenerate);
         break;

      case IDMI_SelectedRename:
      case IDMI_PopupRename:
         {
         pSelectedFile = (CfReportFileStats*)pcnrFiles->cursoredObject();
         strSaveName = pSelectedFile->name();
         strGenerateName = pSelectedFile->name();
         openFileName(enumFileRename);
         bOnlyRename = true;
         break;
         }

      case IDMI_SelectedDelete:
      case IDMI_PopupDelete:
         pSelectedFile = (CfReportFileStats*)pcnrFiles->cursoredObject();
         strSaveName = pSelectedFile->name();
         deleteFile();
         break;
#ifdef BR_OBSOLETE_MENU   /* F62721 EDW 01/21/2002 */
//IC01552
    case IDMI_ViewSortNameA:

         pMenu->checkItem(IDMI_ViewSortNameA);
         pMenu->uncheckItem(IDMI_ViewSortNameD);
         pMenu->uncheckItem(IDMI_ViewSortDateA);
         pMenu->uncheckItem(IDMI_ViewSortDateD);

         pMenu->checkItem(IDMI_ViewSortName);
         pMenu->uncheckItem(IDMI_ViewSortDate);

//         pWindow->sSort = MainWindow::enumSortName;
         pProfile->bSort = true;
         this->bSort = true;
         pProfile->bSascending = true;
         this->bSascending = true;

         sSort = 1;
         sortFiles();
        break;

    case IDMI_ViewSortNameD:
        pMenu->checkItem(IDMI_ViewSortName);
        pMenu->uncheckItem(IDMI_ViewSortDate);

        pMenu->checkItem(IDMI_ViewSortNameD);
        pMenu->uncheckItem(IDMI_ViewSortNameA);
        pMenu->uncheckItem(IDMI_ViewSortDateA);
        pMenu->uncheckItem(IDMI_ViewSortDateD);

//         pWindow->sSort = MainWindow::enumSortDate;
         pProfile->bSort = true;
         this->bSort = true;
         pProfile->bSascending = false;
         this->bSascending = false;

         sSort = -1;
         sortFiles();
       break;

    case IDMI_ViewSortDateA:

         pMenu->checkItem(IDMI_ViewSortDate);
         pMenu->uncheckItem(IDMI_ViewSortName);

         pMenu->checkItem(IDMI_ViewSortDateA);
         pMenu->uncheckItem(IDMI_ViewSortDateD);
         pMenu->uncheckItem(IDMI_ViewSortNameA);
         pMenu->uncheckItem(IDMI_ViewSortNameD);
//         pWindow->sSort = MainWindow::enumSortName;
         pProfile->bSort = false;
         this->bSort = false;
         pProfile->bSascending = true;
         this->bSascending = true;
         sSort = 2;
         sortFiles();
        break;

    case IDMI_ViewSortDateD:

        pMenu->uncheckItem(IDMI_ViewSortName);
        pMenu->checkItem(IDMI_ViewSortDate);

        pMenu->checkItem(IDMI_ViewSortDateD);
        pMenu->uncheckItem(IDMI_ViewSortDateA);
        pMenu->uncheckItem(IDMI_ViewSortNameA);
        pMenu->uncheckItem(IDMI_ViewSortNameD);
//         pWindow->sSort = MainWindow::enumSortDate;
         pProfile->bSort = false;
         this->bSort = false;
         pProfile->bSascending = false;
         this->bSascending = false;
         sSort = -2;
         sortFiles();
       break;


/*      case IDMI_ViewSortA:
         sSort = abs(sSort);
         sortFiles();
         break;

      case IDMI_ViewSortD:
         sSort = -abs(sSort);
         sortFiles();
         break;*/
#endif
      case IDMI_OptionsGenerate:
         usPath = enumPathGenerate;
         selectDirectory(strGeneratePath);
         break;

#ifdef BR_OBSOLETE_MENU   /* F62721 EDW 01/21/2002 */
      case IDMI_OptionsLocale:
         openOptions();
         break;

      case IDMI_OptionsRetrieval:
         openRetrievalOptions();
         break;
#endif
      /* F62721 EDW 01/21/2002 begin */
      case IDMI_OptionsPrefs:
         openPrefsOptions();
         break;
      /* F62721 EDW 01/21/2002 end */
      case IDMI_HelpUsing:
         pHelp->show(IHelpWindow::using);
         break;

      case IDMI_HelpProduct:
         openProductInfo();
         break;

      case UM_StartUp1:
         thread.start(new IThreadMemberFn<MainWindow>(*this, startUp1));
         break;

      case UM_StartUp2:
         startUp2();
         break;

      case UM_SwitchLocale:
         switchLocale();
         thread.start(new IThreadMemberFn<MainWindow>(*this, reformat));
         break;

      case UM_SaveFile:
         thread.start(new IThreadMemberFn<MainWindow>(*this, saveFile));
         break;

      case UM_GenerateFile:
         thread.start(new IThreadMemberFn<MainWindow>(*this, generateFile));
         break;

      case UM_RenameFile:
         renameFile();
         break;
      //edw 12/5/97-added this case to name files downloaded from host
      case UM_NameFile:
         {
         pSelectedFile = (CfReportFileStats*)pcnrFiles->cursoredObject();
         strSaveName = pSelectedFile->name();
         strGenerateName = pSelectedFile->name();  //47705 - flw
         openFileName(enumFileName);
         break;
         }

      // Begin new logic 52012  MAMT - 28/07/2000
      case UM_NameFile1:
         {
         pSelectedFile = (CfReportFileStats*)pcnrFiles->cursoredObject();
         strSaveName = pSelectedFile->name();
         strGenerateName = pSelectedFile->name();  //47705 - flw
         openFileName(enumFileName1);
         break;
         }
      // End new logic 52012 MAMT - 28/07/2000

      case UM_ReturnRetrieval:
         returnRetrievalFile();
         break;

#ifdef BR_OBSOLETE_MENU   /* F62721 EDW 01/21/2002 */
      case IDMI_OptionsFastPathOn:
         {
         pMenu->checkItem(IDMI_OptionsFastPathOn);
         pMenu->uncheckItem(IDMI_OptionsFastPathOff);
         pProfile->bFastPath = true;
         break;
         }

      case IDMI_OptionsFastPathOff:
         {
         pMenu->checkItem(IDMI_OptionsFastPathOff);
         pMenu->uncheckItem(IDMI_OptionsFastPathOn);
         pProfile->bFastPath = false;
         break;
         }
     //ZOR 48548 280600
      case IDMI_Options_ExpandedOn:
         {
         pMenu->checkItem(IDMI_Options_ExpandedOn);
         pMenu->uncheckItem(IDMI_Options_ExpandedOff);
         pProfile->bExpanded = true;
         this->bExpanded = true;
         break;
         }

      case IDMI_Options_ExpandedOff:
         {
         pMenu->checkItem(IDMI_Options_ExpandedOff);
         pMenu->uncheckItem(IDMI_Options_ExpandedOn);
         pProfile->bExpanded = false;
         this->bExpanded = false;
         break;
         }
      //ZOR 48548 280600
     //47971 begin new logic MAMT 21/08/2000
      case IDMI_Options_TileHor:
         {
         pMenu->checkItem(IDMI_Options_TileHor);
         pMenu->uncheckItem(IDMI_Options_TileVer);
         pProfile->bTile = true;
         this->bTile = true;
         break;
         }

      case IDMI_Options_TileVer:
         {
         pMenu->checkItem(IDMI_Options_TileVer);
         pMenu->uncheckItem(IDMI_Options_TileHor);
         pProfile->bTile = false;
         this->bTile = false;
         break;
         }
#endif
     //47971 end logic MAMT 21/08/2000
      default:
         return false ;
      }

   return true;
   }

//=======================================================================
// FUNCTION NAME    systemCommand
//
// DESCRIPTION      Handle system commands.
//=======================================================================

Boolean MainWindow :: systemCommand(ICommandEvent& cmdEvt)
   {
   if ( cmdEvt.commandId() != ISystemMenu::idClose )
      return false;
   if ( bLock )
      {
      //WinAlarm(HWND_DESKTOP, WA_WARNING);
      MessageBeep(MB_ICONEXCLAMATION);
      return true;
      }
   if ( !confirmFileSave() )
      return true;
   if ( bProfile )
      {
      IString strProfile;
      pProfile->save(strProfile);
      }

   if ( pDetail )
      {
      pDetail->sendEvent(IWindow::systemCommand, ISystemMenu::idClose);//jt-5/12/97
      //pDetail->close();
      delete pDetail;
      }

   if ( pSystems )
      {
      pSystems->clearNotebook();
      //pSystems->close();
      pSystems->sendEvent(IWindow::systemCommand, ISystemMenu::idClose);//jt
      delete pSystems;
      }
   if ( pOrders )
      {
      pOrders->clearNotebook();
      pOrders->sendEvent(IWindow::systemCommand, ISystemMenu::idClose);//jt
      //pOrders->close();
      delete pOrders;
      }

   //edw hide();
   if (bConfig)                                        //edw 1/9/97
      {
      HANDLE hSemaphore;
      hSemaphore = OpenSemaphore(SEMAPHORE_MODIFY_STATE, FALSE, "bf9bret");
      ReleaseSemaphore(hSemaphore, 1, NULL);
      }
   return false;
   }

//=======================================================================
// FUNCTION NAME    enter
//
// DESCRIPTION      Handle container enter events.
//=======================================================================

Boolean MainWindow :: enter(ICnrEnterEvent& enterEvt)
   {
   if ( bLock )
      {
      //WinAlarm(HWND_DESKTOP, WA_WARNING);
      MessageBeep(MB_ICONEXCLAMATION);
      return false;
      }
   CfReportFileStats* pObject = (CfReportFileStats*)enterEvt.object();
   if ( !pObject )
      {
      //WinAlarm(HWND_DESKTOP, WA_WARNING);
      MessageBeep(MB_ICONEXCLAMATION);
      return false;
      }
   postEvent(IWindow::command, IDMI_SelectedOpen);
   return false;
   }

//=======================================================================
// FUNCTION NAME    help
//
// DESCRIPTION      Handle container help events.
//=======================================================================

Boolean MainWindow :: help(ICnrHelpEvent& helpEvt)
   {
   pHelp->show(IResourceId(IDW_AFOAPPLY));
   return true;
   }

//=======================================================================
// FUNCTION NAME    makePopupMenu
//
// DESCRIPTION      Handle object select events.
//=======================================================================

Boolean MainWindow :: makePopUpMenu(IMenuEvent& menuEvt)
   {
   if ( bLock )
      {
      //WinAlarm(HWND_DESKTOP, WA_WARNING);
      MessageBeep(MB_ICONEXCLAMATION);
      return false;
      }
   CfReportFileStats* pObject = (CfReportFileStats*)popupMenuObject();
   if ( !pObject )
      {
      //WinAlarm(HWND_DESKTOP, WA_WARNING);
      MessageBeep(MB_ICONEXCLAMATION);
      return false;
      }
   IPoint pt = menuEvt.mousePosition();
   if ( pcnrFiles->objectUnderPoint(pt) != pObject )
      pt = pcnrFiles->detailsObjectRectangle(pObject).center();
   IPopUpMenu* pPopUp = new IPopUpMenu(IDM_Popup, menuEvt.window());
   pPopUp->deleteItem(IDMI_PopupApply);
   pPopUp->deleteItem(IDMI_PopupAssign);
   pPopUp->deleteItem(IDMI_PopupRemove);
   pPopUp->deleteItem(IDMI_PopupDetail);
   if ( pObject != pFile )
      {
      pPopUp->deleteItem(IDMI_PopupSave);
      pPopUp->deleteItem(IDMI_PopupSaveAs);
      //pPopUp->deleteItem(IDMI_PopupGenerate);      //47705
      }
   else
      if ( pFileName )
         if ( pFileName->isVisible() )
            {
            pPopUp->deleteItem(IDMI_PopupSaveAs);
            //pPopUp->deleteItem(IDMI_PopupGenerate);     47705
            pPopUp->deleteItem(IDMI_PopupRename);
            pPopUp->deleteItem(IDMI_PopupDelete);
            }
   pPopUp->setAutoDeleteObject();
   pPopUp->show(pt);
   pcnrFiles->setCursor(pObject);
   return false;
   }

//=======================================================================
// FUNCTION NAME    validate
//
// DESCRIPTION     validate the filename entered on the file dialog
//=======================================================================
Boolean MainWindow :: validate(IFileDialogEvent& fdEvt)
   {
   bValid = false;
   IString strFilename = fdEvt.fileName();
   switch ( usPath )
      {
      case enumFileConfig:
         {
         Boolean bValidate = true;
         WIN32_FIND_DATA fileData;
         HANDLE handle = FindFirstFile( (LPCTSTR)strFilename, &fileData );
         if ( handle == INVALID_HANDLE_VALUE )
            bValidate = false;
         FindClose( handle );

         IString strExt = strFilename.subString( strFilename.lastIndexOf('.') );
         strExt = strExt.upperCase();
         if ( (strExt != STR_ExtensionCFR && strExt!= STR_ExtensionADM ) || !bValidate )  //42619
            {
            fdEvt.setResult(false);
            return true;
            }
         else
            {
            bValid = true;
            IThread thread;
            thread.setStackSize(THREAD_STACK_SIZE);
            strLocalRetrFile = strFilename;
            thread.start(new IThreadMemberFn<MainWindow>(*this, openHost));
            }
         break;
         }
      }
   return false;
   }

//=======================================================================
// FUNCTION NAME    modelessResults
//
// DESCRIPTION      Handle close of modeless file dialog.
//=======================================================================

Boolean MainWindow :: modelessResults(IFileDialog* endingDialog)
   {
   bLock = false;
   if ( usPath == enumFileConfig )
      {
      if ( bValid )
         {
         bLock = true;
         bValid = false;
         }
      else
         {
         if( pProfile->bFastPath && pProfile->bEconfig )
           postEvent(IWindow::systemCommand, ISystemMenu::idClose);  //58370
         }
      return false;
      }

   // endingDialog->setAutoDeleteObject(true); //MSN-del
   if ( !isVisible() )
      return false;
   IString strPath = endingDialog->fileName();
   // strPath.change(STR_DummyFile, "");//MSN-del

   if ( endingDialog->pressedOK() )
      switch ( usPath )
         {
         case enumPathBrowse:
            {
            strPath.remove(strPath.length() - IString(STR_DummyFile).length() + 1);//MSN-add
            pProfile->strBrowsePath = strBrowsePath = strPath;
            pProfile->strGeneratePath = strGeneratePath = pProfile->strBrowsePath;  //47705 - flw
            ptxPath->setText(STR_PathLabelText + strBrowsePath+" ");   //edw01/20/98-Add Label
            IThread thread;
            thread.setStackSize(THREAD_STACK_SIZE);
            thread.start(new IThreadMemberFn<MainWindow>(*this, listFiles));
            break;
            }
        /* case enumPathGenerate:
            {
            strPath.remove(strPath.length() - IString(STR_DummyFile).length() + 1);//MSN-add
            pProfile->strGeneratePath = strGeneratePath = strPath;
            break;
            }   */
         }

   return false;
   }

//=======================================================================
// FUNCTION NAME    windowResize
//
// DESCRIPTION      Handle window resize events.
//=======================================================================

Boolean MainWindow :: windowResize(IResizeEvent& sizeEvt)
   {
   unsigned long ulNewCount, ulMESCount;              //edw 12/2/97
   if ( isVisible() && !isMinimized() )
      {
      pProfile->position[WINDOWMAIN] = position();
      pProfile->size[WINDOWMAIN] = size();
      }
   //edw 12/2/97-Only show Orders Notebook if it is not empty
   if ( pOrders )
      {
      ulNewCount = pOrders->pcnrOrders[ORDERSNEW]->objectCount();
      ulMESCount = pOrders->pcnrOrders[ORDERSMES]->objectCount();
      }
   if ( isMinimized() )
      {
      if ( pOrders )
         if ( ulNewCount || ulMESCount )              //edw 12/2/97
            if ( pOrders->isVisible() )               //D54593
               pOrders->minimize();
      if ( pSystems )
         if ( pSystems->isVisible() )                 //D54593
            pSystems->minimize();
      if ( pDetail )
         if ( pDetail->pcnrDetail->objectCount() )    //edw07/01/1998
            pDetail->minimize();
      bMinimized = true;
      }
   else
      {
      if ( bMinimized )
         {
         if ( pOrders )
            if ( ulNewCount || ulMESCount )           //edw 12/2/97
               {
               if ( pOrders->isMinimized() )          //D54593
                  {
                  pOrders->restore();
                  pOrders->setFocus();
                  }
               }
         if ( pSystems )
            {
            if ( pSystems->isMinimized() )            //D54593
               {
               pSystems->restore();
               pSystems->setFocus();
               }
            }
         if ( pDetail )
            if ( pDetail->pcnrDetail->objectCount() ) //edw07/01/1998
               {
               pDetail->restore();
               pDetail->setFocus();
               }
         setFocus();
         }
      bMinimized = false;
      }
   return false;
   }

//=======================================================================
// FUNCTION NAME    activated
//
// DESCRIPTION      Handle frame activation events.
//=======================================================================

Boolean MainWindow :: activated(IFrameEvent& frmEvt)
   {
   activeWindow = this;
   return false;
   }

//=======================================================================
// FUNCTION NAME    closed
//
// DESCRIPTION      Handle frame close events.
//=======================================================================

Boolean MainWindow :: closed(IFrameEvent& frmEvt)
   {
   return false;
   }

//=======================================================================
// FUNCTION NAME    getSensedDate(IString strMRD)
//
// DESCRIPTION      Retrieved the sensed date from the MRDB file.
//=======================================================================
//edw-12/04/97
IString MainWindow :: getSensedDate(IString strMRD)
   {
   IString sInputRec;

   ifstream ifs(strMRD);
   if ( !ifs )
      return STR_NoSensedDateRetrieved;

   while ( !ifs.eof() )
      {
      sInputRec = IString::lineFrom(ifs,'\n');
      if ( sInputRec.subString(1, 2) != "53" )
         continue;
      else
         {
         ifs.close();
         return sInputRec.subString(40, 8);
         }
      }
   ifs.close();
   return STR_NoSensedDateRetrieved;
   }

//=======================================================================
// FUNCTION NAME    hasVPDTransData(IString strCFR)
//
// DESCRIPTION      Retrieved AAS data contains VPDTran data
//=======================================================================
//edw05/29/1998
Boolean MainWindow :: hasVPDTransData(IString strCFR)
   {
   IString sInputRec;
   Boolean b54, bVPD = false;
   IString strValidHWRecs = STR_HWRecTypes;
   ifstream ifs(strCFR);
   if ( !ifs )
      {
      IMessageBox::Style style = IMessageBox::cancelButton |
                                 IMessageBox::warningIcon |
                                 IMessageBox::moveable |
                                 IMessageBox::systemModal;
      IMessageBox msg (this);
      msg.setTitle(IResourceId(IDS_ErrorBackEnd_Title));
      msg.show(IResourceId(IDS_ErrorFileRead_Text), style, IDH_ErrorBackEnd);
      return false;
      }

   while ( !ifs.eof() )
      {
      sInputRec = IString::lineFrom(ifs,'\n');
      if ( sInputRec.subString(1, 2) == STR_07RecType )
         {
         if ( sInputRec.subString(6, 7) == STR_VPDTRAN )
            bVPD = true;
         }

      //F100160 Begin Logic: Set the type of configuration flags, used to determine if the configuration
      //                     belongs to Blue Horizon or xSeries products family. (Positions 13-20 of '00' record)

      if ( sInputRec.subString(1, 2) == STR_00RecType )
         {
          if ( (sInputRec.subString(13,8).strip() == STR_BlueHorizon1) ||
               (sInputRec.subString(13,8).strip() == STR_BlueHorizon2)   )
             bIsBlueHor = true;

          if (sInputRec.subString(13,8).strip() == STR_XSeriesConf)
             bIsXseries = true;
         }
      //F100160 End Logic: HECC - 07/26/2005


      //Set the strFirstHWRecType variable.  Will use to determine the ISM product group later.
      if ( bFirstHWRec )
         if ( strValidHWRecs.indexOf( sInputRec.subString(1, 2) ) )
            {
            //IString strTypeMod = sInputRec.subString(3, 4) + STR_Hyphen +
            //                                          sInputRec.subString(8, 3);   cmt'd 44690
            //edw02/11/99if ( myCpuList.IsCpu( strTypeMod ) )
            //if ( pBackEnd->getCpuList().IsCpu( strTypeMod ) )
            //   {
               strFirstHWRecType = sInputRec.subString(3, 4);
               bFirstHWRec = false;
            // }
            }

      if ( sInputRec.subString(1, 2) != STR_54RecType )
         continue;
      else
         b54 = true;
      }
   if ( b54 && bVPD )
      {
      ifs.close();
      return true;
      }
   ifs.close();
   return false;
   }

//=======================================================================
// FUNCTION NAME    hasStackedMRDB(IString strMRD)
//
// DESCRIPTION      Multiple MRDB reports are in retrieved data
//=======================================================================
//edw05/29/1998
Boolean MainWindow :: hasStackedMRDB(IString strMRD)
   {
   IString sInputRec;
   Boolean bFirst00 = false;
   IString strValidHWRecs = STR_HWRecTypes;

   ifstream ifs(strMRD);
   if ( !ifs )
      {
      IMessageBox::Style style = IMessageBox::cancelButton |
                                 IMessageBox::warningIcon |
                                 IMessageBox::moveable |
						IMessageBox::systemModal;
      IMessageBox msg (this);
      msg.setTitle(IResourceId(IDS_ErrorBackEnd_Title));
      msg.show(IResourceId(IDS_ErrorFileRead_Text), style, IDH_ErrorBackEnd);
      return false;
      }
   while ( !ifs.eof() )
      {
      sInputRec = IString::lineFrom(ifs,'\n');

      //Set the strFirstHWRec variable.  Will use to determine the ISM product group later.
      if ( bFirstHWRec )
         if ( strValidHWRecs.indexOf( sInputRec.subString(1, 2) ) )
            {
            //IString strTypeMod = sInputRec.subString(3, 4) + STR_Hyphen +              cmt'd 44690
            //                                          sInputRec.subString(24, 3);
            //edw02/11/99if ( myCpuList.IsCpu( strTypeMod ) )
            //if ( pBackEnd->getCpuList().IsCpu( strTypeMod ) )
            //   {
               strFirstHWRecType = sInputRec.subString(3, 4);
               bFirstHWRec = false;
            //   }
            }

      if ( sInputRec.subString(1, 2) != STR_00RecType )
         continue;
      else
         if ( !bFirst00 )
            bFirst00 = true;
         else
            {
            ifs.close();
            return true;
            }
      }
   ifs.close();
   return false;
   }

//=======================================================================
// FUNCTION NAME    regenerateSerialNbrs(IString strMRD)
//
// DESCRIPTION      Generate unique SN's for products in MRDB w/ SN's
//                  of zero.
//=======================================================================
//edw12/15/1998
Boolean MainWindow :: regenerateSerialNbrs( IString strMRD )
   {
   ULONG ulSequenceNbr = 1;
   IString sRecType, sInputRec;
   IString strTempFile = strMRD;
   IString strValidTypes = STR_AllProductRecTypes;
   IMessageBox msg (this);
   ifstream ifs(strMRD);
   ofstream ofs;
   strTempFile.remove(strTempFile.lastIndexOf('.') + 1);
   strTempFile += IString(STR_Prefix);
   ofs.open( strTempFile );
   Boolean bFileChanged = false;       // D72623 EDW 07/01/2002

   if ( !ifs )
      {
      IMessageBox::Style style = IMessageBox::cancelButton |
                                 IMessageBox::warningIcon |
                                 IMessageBox::moveable |
					IMessageBox::systemModal;
      msg.setTitle(IResourceId(IDS_ErrorBackEnd_Title));
      msg.show(IResourceId(IDS_ErrorFileRead_Text), style, IDH_ErrorBackEnd);
      return false;
      }
   if ( !ofs )
      {
      IMessageBox::Style style = IMessageBox::cancelButton |
                                 IMessageBox::warningIcon |
                                 IMessageBox::moveable |
					IMessageBox::systemModal;
      msg.setTitle(IResourceId(IDS_ErrorBackEnd_Title));
      msg.show(IResourceId(IDS_ErrorFileWrite_Text), style, IDH_ErrorBackEnd);
      return false;
      }
   while ( !ifs.eof() )
      {
      sInputRec = IString::lineFrom(ifs,'\n');
      sRecType = sInputRec.subString(1, 2);

      if ( strValidTypes.indexOf(sRecType ) == 0 )
         ofs << sInputRec << endl;
      else
         {
         ofs << sInputRec << endl;
         sInputRec = IString::lineFrom(ifs,'\n');
         sRecType = sInputRec.subString(1, 2);
         if ( sRecType == STR_54RecType )
            {
            IString strSerialNbr = sInputRec.subString( 28, 9 );
            strSerialNbr = strSerialNbr.strip();                                          // D72623 EDW 07/01/2002
            if ( (strSerialNbr.isDigits() && !strSerialNbr.asInt()) ||   //SN equal zero  // D72623 EDW 07/01/2002
                 (strSerialNbr.length() == 0) )                          //SN is blank    // D72623 EDW 07/01/2002
               {
               IString strSeqNbr = ulSequenceNbr;
               strSeqNbr = strSeqNbr.rightJustify( 3, '0' );
               strSeqNbr = STR_Id + strSeqNbr;
               strSeqNbr = strSeqNbr.rightJustify( 9, '0' );
               sInputRec.overlayWith( strSeqNbr, 28 );
               ulSequenceNbr++;
               bFileChanged = true;
               }
            }
         ofs << sInputRec << endl;
         }
      }
   ifs.close();
   ofs.close();
   if (bFileChanged)                   // D72623 EDW 07/01/2002
      {                                // D72623 EDW 07/01/2002
      remove( strMRD );
      rename( strTempFile, strMRD );
      }                                // D72623 EDW 07/01/2002
   else                                // D72623 EDW 07/01/2002
      remove (strTempFile);            // D72623 EDW 07/01/2002
   return true;
   }

//=======================================================================
// FUNCTION NAME    resequenceInstIds(IString strMRD)
//
// DESCRIPTION      Resequence inst ids of stacked MRDB reports
//=======================================================================
//edw06/05/1998
Boolean MainWindow :: resequenceInstIds(IString strMRD)
   {
   Boolean bSkip;
   IString sRecType, sInputRec, strSysId;
   IString strTempFile = strMRD;
   IString strValidTypes = STR_54AndSubTypes;
   IString strIds = STR_VPDTranIds;
   int iCnt = 0;
   IMessageBox msg (this);
   ifstream ifs(strMRD);
   ofstream ofs;
   strTempFile.remove(strTempFile.lastIndexOf('.') + 1);
   strTempFile += IString(STR_Prefix);
   ofs.open( strTempFile );

   if ( !ifs )
      {
      IMessageBox::Style style = IMessageBox::cancelButton |
                                 IMessageBox::warningIcon |
                                 IMessageBox::moveable |
						IMessageBox::systemModal;
      msg.setTitle(IResourceId(IDS_ErrorBackEnd_Title));
      msg.show(IResourceId(IDS_ErrorFileRead_Text), style, IDH_ErrorBackEnd);
      return false;
      }
   if ( !ofs )
      {
      IMessageBox::Style style = IMessageBox::cancelButton |
                                 IMessageBox::warningIcon |
                                 IMessageBox::moveable |
					IMessageBox::systemModal;
      msg.setTitle(IResourceId(IDS_ErrorBackEnd_Title));
      msg.show(IResourceId(IDS_ErrorFileWrite_Text), style, IDH_ErrorBackEnd);
      return false;
      }
   while ( !ifs.eof() )
      {
      sInputRec = IString::lineFrom(ifs,'\n');
      sRecType = sInputRec.subString(1, 2);
      if ( sRecType == STR_00RecType )
         {
         iCnt++;
         strSysId = strIds.word( iCnt );
         }
      if ( strValidTypes.indexOf(sRecType ) == 0 )
         ofs << sInputRec << endl;
      else
         {
         if ( !( sInputRec.subString( 3, 1) == STR_H ) )
            {
            sInputRec.insert( strSysId, 8 );
            sInputRec.remove( 3, 1 );
            }
         if ( sRecType == STR_55RecType )
            {
            if ( !( sInputRec.subString( 14, 1) == STR_H ) )
               if ( !( sInputRec.subString( 14, 6 ).isWhiteSpace() ) )
                  {
                  sInputRec.insert( strSysId, 19 );
                  sInputRec.remove( 14, 1 );
                  }
            }
         if ( sRecType == STR_56RecType )
            {
            int iStart = 15;
            while ( !( sInputRec.subString( iStart, 30 ).isWhiteSpace() ) )
               {
               if ( !( sInputRec.subString( iStart, 1) == STR_H ) )
                  if ( !( sInputRec.subString( iStart, 6 ).isWhiteSpace() ) )
                     {
                     sInputRec.insert( strSysId, iStart+5 );
                     sInputRec.remove( iStart, 1 );
                     }
               iStart = iStart + 30;
               }
            //replace if below with while above since last 4 fields of 56 can repeat
            //if ( !( sInputRec.subString( 15, 6 ).isWhiteSpace() ) )
            //   {
            //   sInputRec.insert( strSysId, 20 );
            //   sInputRec.remove( 15, 1 );
            //   }
            }
         ofs << sInputRec << endl;
         }
      }
   ifs.close();
   ofs.close();
   remove( strMRD );
   rename( strTempFile, strMRD );

   return true;
   }


//=======================================================================
// FUNCTION NAME    createMRDBFile(strCFR)
//
// DESCRIPTION      Move VPDTran data to a V20-like CFReport
//=======================================================================
//edw05/29/1998
IString MainWindow :: createMRDBFile(IString strCFR)
   {
   IString sInputRec, strOutFile, strLast08, strRecord00;
   Boolean bFirst54 = false;
   int iCnt = 0;
   IMessageBox msg (this);
   ifstream ifs(strCFR);
   ofstream ofs;
   IString strHeader=STR_V20Header;
   IString strVPDAidName=STR_VPDAidSystemName;
   IString strRecord53=STR_V20Rec53Header;
   IString strRec08Qtys=STR_Rec08Qtys;
   IString sBlank = STR_Blank1;

   strOutFile=strCFR;
   strOutFile.remove(strOutFile.lastIndexOf('.') + 1);
   strOutFile += IString(STR_TypeMRD);
   ofs.open( strOutFile );
   if ( !ofs )
      {
      IMessageBox::Style style = IMessageBox::cancelButton |
                                 IMessageBox::warningIcon |
                                 IMessageBox::moveable |
						IMessageBox::systemModal;
      msg.setTitle(IResourceId(IDS_ErrorBackEnd_Title));
      msg.show(IResourceId(IDS_ErrorFileWrite_Text), style, IDH_ErrorBackEnd);
      return STR_DummyRecType;
      }
   if ( !ifs )
      {
      IMessageBox::Style style = IMessageBox::cancelButton |
                                 IMessageBox::warningIcon |
                                 IMessageBox::moveable |
						IMessageBox::systemModal;
      msg.setTitle(IResourceId(IDS_ErrorBackEnd_Title));
      msg.show(IResourceId(IDS_ErrorFileRead_Text), style, IDH_ErrorBackEnd);
      return STR_DummyRecType;
      }

   while ( !ifs.eof() )
      {
      sInputRec = IString::lineFrom(ifs,'\n');
      if ( sInputRec.subString(1, 2) == STR_00RecType )
         {
         strRecord00 = strHeader+sInputRec.subString(9, 4)+sInputRec.subString(7, 2);
         strRecord00 += strVPDAidName;
         ofs << strRecord00 << endl;
         //ofs << strRecord53 << sBlank.rightJustify(37) << sInputRec.subString(5, 8) << endl;
         ofs << strRecord53 << endl;
         iCnt += 2;
         }
      else
      if ( ( sInputRec.subString(1, 2) == STR_08RecType ) ||
           ( sInputRec.subString(1, 2) == STR_25RecType ) )
         {
         IString strMach = sInputRec.subString(3, 4);
         IString strMod = sInputRec.subString(8, 3);
         IString strModQty = sInputRec.subString(11, 5);
         IString strRest = sInputRec.subString(16);
         int iFeat = strRest.length() / 12;
         strLast08 = STR_08RecType + strMach + strRec08Qtys + strMod.strip().rightJustify(7) +
                                             strModQty.strip().rightJustify(8);
         for (int i=0; i < iFeat; i++)
            {
            IString strFeat = strRest.subString(1, 7);
            IString strFeatQty = strRest.subString(8, 5);
            strLast08 = strLast08 + strFeat.strip().rightJustify(7) +
                                             strFeatQty.strip().rightJustify(8);
            strRest = strRest.subString(13);
            }
         bFirst54 = true;
         }
      else
      if ( sInputRec.subString(1, 2) == STR_98RecType )
         {
         iCnt += 1;
         IString strCnt = iCnt;
         ofs << STR_98RecType << strCnt.rightJustify(5) << endl;
         }
      else
      if ( sInputRec.subString(1, 2) == STR_54RecType )
         if ( bFirst54 )
            {
            bFirst54 = false;
            ofs << strLast08 << endl;
            ofs << sInputRec << endl;
            iCnt += 2;
            }
         else
            {
            ofs << sInputRec << endl;
            iCnt += 1;
            }
      else
      if ( ( sInputRec.subString(1, 2) == STR_55RecType ) ||
           ( sInputRec.subString(1, 2) == STR_56RecType ) )
         {
         ofs << sInputRec << endl;
         iCnt += 1;
         }
      else
         continue;
      }
   ifs.close();
   ofs.close();
   return strOutFile;
   }

//=======================================================================
// FUNCTION NAME    loadNoCompareTable
//
// DESCRIPTION      Opens the S/390 No Compare Table
//=======================================================================
//edw07/07/1998
USHORT MainWindow :: loadNoCompareTable( IString strExecPath )
   {
   IString strFilename = strExecPath + STR_NoCompTable;
   fstream noCompStream;
   fstream& ifs = noCompStream;
   IString sInputRec, sOutputRec;

   int iError = BackEnd::OpenFile( strFilename, BackEnd::READ, ifs );
   if ( iError )
      {
      if ( iError == IDS_ErrorInputFileOpen_Text )
         iError = IDS_ErrorLoadTableInput_Text;
      if ( iError == IDS_ErrorOutputFileOpen_Text )
         iError = IDS_ErrorLoadTableOutput_Text;
      return iError;
      }

   while ( !ifs.eof() )
      {
      sInputRec = IString::lineFrom(ifs,'\n');
      // begin new logic 49222 - MAMT - 06/27/00
/*      if ( sInputRec.subString(1, 4).isDigits() &&
           sInputRec.subString(9, 4).isDigits() )  old code 55982  */
      if ( sInputRec.subString(1, 4).isDigits())   //55982 MAMT 31/10/00
         {
//         sOutputRec = sInputRec.subString(1, 12); old code 55982
         //sOutputRec = sInputRec.subString(1, 16);  //55982 MAMT 31/10/00
         //F104628. Begin logic - RERC 01/30/06
         sOutputRec = sInputRec.subString(19,1);
         if (sOutputRec == STR_Blank1)
            sOutputRec = sInputRec.subString(1, 18)+STR_UNACCEPTED;
         else
            sOutputRec = sInputRec.subString(1,19);
         noCompTableSet.add(sOutputRec);
         }
         //F104628. Finish logic - RERC 01/30/06

     // finish new logic 49222 - MAMT - 06/27/00
/*      if ( sInputRec.subString(1, 8).isDigits() )
         {
         sOutputRec = sInputRec.subString(1, 8);
         noCompTableSet.add(sOutputRec);
         } old logic 49222 - MAMT*/
      }

   ifs.close();
   return iError;
   }

//=======================================================================
// FUNCTION NAME    loadQuickFixTable
//
// DESCRIPTION      Opens the S/390 Quick fix table 49222 - MAMT 06/27/00
//=======================================================================
USHORT MainWindow :: loadQuickFixTable( IString strExecPath )
   {
   IString strFilename = strExecPath + STR_QuickFixTable;
   fstream quickFixStream;
   fstream& ifs = quickFixStream;
   IString sInputRec, sOutputRec;

   int iError = BackEnd::OpenFile( strFilename, BackEnd::READ, ifs );
   if ( iError )
      {
      //* 79548 Begin new logic: Wrong message, doesn't explain the right table name
      //* that caused the error.  Message was changed to display the right name.
      //* As this file is never opened for output, the error message explaining such
      //* error will never appear, so, it is not needed here.
      //* if ( iError == IDS_ErrorInputFileOpen_Text )
      //*    iError = IDS_ErrorLoadTableInput_Text;
      //* if ( iError == IDS_ErrorOutputFileOpen_Text )
      //*    iError = IDS_ErrorLoadTableOutput_Text;

      if ( iError == IDS_ErrorInputFileOpen_Text )
         iError = IDS_ErrorLoadQuickInput_Text;

      //* 79548 Ends new logic:   HECC - 04/28/2003

      return iError;
      }

   while ( !ifs.eof() )
      {
      sInputRec = IString::lineFrom(ifs,'\n');
/*      if ( sInputRec.subString(1, 4).isDigits() &&
           sInputRec.subString(9, 4).isDigits() &&
           sInputRec.subString(23, 4).isDigits() ) old code 55982  */
      if ( sInputRec.subString(1, 4).isDigits())  //55982 MAMT 31/10/00
         {
//         sOutputRec = sInputRec.subString(1, 26); old code 55982
         sOutputRec = sInputRec.subString(1, 34); //55982 MAMT 31/10/00
         QuickFixTableSet.add(sOutputRec);
         }
      }

   ifs.close();
   return iError;
   }

//=======================================================================
// FUNCTION NAME    loadISeriesSWFeatureTable
//
// DESCRIPTION      Loads the SW features table for iSeries or AS400
//                  configurations.
//=======================================================================
//hecc 27/06/2002 F72273
USHORT MainWindow :: loadISeriesSWFeatureTable( IString strExecPath )
   {
   IString strFilename = strExecPath + STR_ISeriesSWTable;
   fstream iSerieSWStream;
   fstream& ifs = iSerieSWStream;

   IString sInputRec, sOutputRec;

   int iError = BackEnd::OpenFile( strFilename, BackEnd::READ, ifs );
   if ( iError )
      {
      if ( iError == IDS_ErrorInputFileOpen_Text )
         iError = IDS_ErrorLoadSWTableInput_Text;
      return iError;
      }

   while ( !ifs.eof() )
      {
      sInputRec = IString::lineFrom(ifs,'\n');
      if ( sInputRec.subString(1, 4).isDigits())
         {
         sOutputRec = sInputRec.subString(1, 16);
         iSeriesSWTableSet.add(sOutputRec);
         }
      }

   ifs.close();
   return iError;
   }

//=======================================================================
// FUNCTION NAME    openNotebooks
//
// DESCRIPTION      Open the Orders and Systems notebooks.
//=======================================================================

void MainWindow :: openNotebooks()
   {
   /*--JT
   IHelpWindow localHelp(HELPTABLE_AFOAPPLY, this);
   try
      {
      IString strMRI = pProfile->strMRI;
      strMRI.insert(STR_PathMRI);
      strMRI.insert(strExecPath);
      localHelp.addLibraries(strMRI+STR_ExtensionHelp);
      }
      catch(IException& exc)
      {
      }
   localHelp.setTitle(IResourceId(IDS_HelpTitle));

   */
   IString strArg = IApplication::current().argv(1);
   IString strFamily = strArg.remove( strArg.indexOf('_') );

   Boolean bNew = IThread::current().variable(STR_Prefix).asInt();
   if ( bNew )
      {
      if ( pDetail )
         {
         //pDetail->postEvent(IWindow::systemCommand, ISystemMenu::idClose);
         pDetail->sendEvent(IWindow::systemCommand, ISystemMenu::idClose);  //edw10/20/98
         delete pDetail;   //edw10/20/98
         pDetail = NULL;   //edw10/20/98
         }

      USHORT usError = pBackEnd->LoadCFREPORT(false);
      if ( usError )
         {
         IMessageBox::Style style = IMessageBox::cancelButton |
                                    IMessageBox::warningIcon |
                                    IMessageBox::moveable |
                                    IMessageBox::systemModal;
         IMessageBox msg(this);
         msg.setTitle(IResourceId(IDS_ErrorBackEnd_Title));
         msg.show(IResourceId(usError), style, IDH_ErrorBackEnd);
         pFile->setIcon(NULL);
         pFile = (CfReportFileStats*)NULL;
         //edw pOrders->postEvent(IWindow::systemCommand, ISystemMenu::idClose);
         //edw pSystems->postEvent(IWindow::systemCommand, ISystemMenu::idClose);
         //edw - next 12 lines
         if (pOrders)
            {
            pOrders->clearNotebook();
            pOrders->hide();
            pOrders->removeFromWindowList();
            }
         if (pSystems)
            {
            pSystems->clearNotebook();
            pSystems->hide();
            pSystems->removeFromWindowList();
            }
         setPointer(false);
         bLock = false;
         pHelp->setAssociatedWindow(this);
         if (pProfile->bEconfig && pProfile->bFastPath)
            {
            bLock = false;           //Usability
            close();
            }
         return;
         }
      pFile->setChanges(0);
      ITitle titleOrders(pOrders, pFile->name(), LOADSTRING(IDS_OrdersTitle));
      ITitle titleSystems(pSystems, pFile->name(), LOADSTRING(IDS_SystemsTitle));
      if ( pDetail )
         ITitle titleDetail(pDetail, pFile->name(), LOADSTRING(IDS_DetailTitle));
      }

   if( pProfile->bFastPath && pProfile->bEconfig );
   else if ( pProfile->bFastPath && !pProfile->bEconfig || !pProfile->bFastPath)
      {
      if ( pSystems->isMinimized() )
         pSystems->restore();
      if ( (pProfile->bEconfig && strFamily == STR_S390)  ||
                 (!pProfile->bEconfig && pProfile->strProdFamily == STR_S390) )
         {
         pSystems->hide();
         pSystems->initializeNotebook();      //47705
         /* D54593
         pSystems->sendEvent(IWindow::command, IDMI_EditSelectAll);
         pSystems->sendEvent(IWindow::command, IDMI_SelectedDetail);
         */

         /* D54593 - show the ProductDetail of all items in the pSystem object */
         pSystems->openAllDetail();
         }
      else
         {
         pSystems->show();
         pSystems->initializeNotebook();
         pSystems->setFocus();
         }
      }

   //edw 12/2/97 - Don't show Orders Notebook if it is empty
   unsigned long ulNewCount = pOrders->pcnrOrders[ORDERSNEW]->objectCount();
   unsigned long ulMESCount = pOrders->pcnrOrders[ORDERSMES]->objectCount();
   if (ulNewCount || ulMESCount)
      {
      if( pProfile->bFastPath && pProfile->bEconfig );
      else if ( pProfile->bFastPath && !pProfile->bEconfig || !pProfile->bFastPath)
         {
         if ( pOrders->isMinimized() )
            pOrders->restore();
         if ( (pProfile->bEconfig && strFamily == STR_S390) ||
                     (!pProfile->bEconfig && pProfile->strProdFamily == STR_S390) )
            {
            pOrders->hide();
            pOrders->initializeNotebook();      //47701
            /* D54593
             * In this case, we should only be showing the pSystem
             * Product Detail Window
            pOrders->sendEvent(IWindow::command, IDMI_EditSelectAll);
            pOrders->sendEvent(IWindow::command, IDMI_SelectedDetail);
            */
            }
         else
            {
            pOrders->show();
            pOrders->initializeNotebook();
            pOrders->setFocus();
            }
         }
      }
   else
      {
      pOrders->hide();
      pOrders->removeFromWindowList();
      }
   setPointer(false);
   bLock = false;
   //if( pProfile->bFastPath && pProfile->bEconfig )  //47705 - flw
    postEvent(IWindow::command, UM_GenerateFile );
   //pHelp->setAssociatedWindow(this);jt-del
   }

//=======================================================================
// FUNCTION NAME    openFileName
//
// DESCRIPTION      Open the file name dialog.
//=======================================================================

void MainWindow :: openFileName(USHORT usOption)
   {
   if ( !pFileName )
      {
      pFileName = new FileNameDialog(this);
      pFileName->moveTo(position());
      }
      pFileName->loadDefaults(usOption);
      //  pFileName->show();   erg 46463
      if ( pFileName->isMinimized() )
       pFileName->restore();
     pFileName->addToWindowList();
   }

//=======================================================================
// FUNCTION NAME    openHost
//
// DESCRIPTION      Open the VM Host window for Retrieval.
//=======================================================================

  void MainWindow :: openHost()
  {
   struct stat fstat;      // 52014 MAMT - 28/07/2000
   bLock = true;
   IMessageBox msg(this);
   IMessageBox::Response response;
   IString strModel;
   IString sOutputRec54;            //53915 MAMT 02/02/2001
   //IString sMsg, sText;

   IString strTempFile = tmpnam(NULL);
   FILE* pTempFile = fopen(strTempFile, "w");

   //Initializations
   bFirstHWRec = true;
   strFirstHWRecType = "";         //51080 - MAMT 06/06/2000
   bIsBlueHor = false;             //F100160 - HECC: 07/26/2005
   bIsXseries = false;             //F100160 - HECC: 07/26/2005

   bS390 = false;
   bMultSysVPDTran = false;
   bMultSysStackedMRDB = false;
   bResolve = false;
   bUseMRDB = false;
   bUseAAS = false;
   bInvRet = false;    //1570

   IString strConfig = "";    //* F81710: To store the configuration model family

   //*79548 Begins New logic: Checks if Base Retrieval was started from eConfig or
   //* stand alone to correctly set the flags bAS4 or bS39 that shows wich model the
   //* retrieved configuration belongs to.
   bAS4 = false;

   //* F81710 Begin new logic: If bConfig flag is true, Base Retrieval was started from
   //* eConfig.  Logic will check the selected model in the parsed parameter.
   if (bConfig)
      {
      //* D81952 The parsed parameter is incorrectly being read in Base Retrieval.
      //* Data that contains the selected model configuration is 1 instead of 2
      //* IString strArg = IApplication::current().argv(2);
      IString strArg = IApplication::current().argv(1);

      //* Checks if the parsed parameter is for the iSeries, pSeries
      //* or zSeries server model, (as Product knowledge files have meaning
      //* only for the server models) and checks wich model is being retrieved.
      if ( (strArg == STR_AS400_Econfig)  ||
           (strArg == STR_RS6000_Econfig) ||
           (strArg == STR_S390_Econfig)   ||
           (strArg == STR_S390IGF_Econfig)  )
         {
          ULONG ulOffset = strArg.indexOf('_');
          strArg.remove(ulOffset);

          //* Sets strConfig, (creates the feature.$$F file path) with the parsed parameter
          //* from eConfig
          strConfig = strArg;

          if ( strArg == STR_AS400 )
             bAS4 = true;
          if ( strArg == STR_S390 )
             bS390 = true;
          if ( strArg == STR_RS6000 )
             strConfig = STR_RS6K;
          }
      }
   //* If bConfig flag is false, Base Retrieval was started as stand alone and logic will
   //* set the bAS4 and bS39 from the Product Family profile preference.
   else
   //* F81710 Ends new logic:  HECC - 06/03/2003
      {
      //* Sets strConfig, with the Product Family profile preference.
      strConfig = pProfile->strProdFamily;


      if ( pProfile->strProdFamily == STR_AS400 )
         bAS4 = true;
      if ( pProfile->strProdFamily == STR_S390 )
         bS390 = true;
      if ( pProfile->strProdFamily == STR_RS6000 )
         strConfig = STR_RS6K;
      }
   //*79548 Ends new logic. HECC: 04/10/2003

   fprintf(pTempFile, "%s\n", STR_TagLocale);
   fprintf(pTempFile, "%s\n", pProfile->strLocale);

   fprintf(pTempFile, "%s\n", STR_TagDirectory);
   fprintf(pTempFile, "%s\n", strBrowsePath);

   fprintf(pTempFile, "%s\n", STR_TagFunction);

   if ( bConfig )
      {
      //if running from ISM, get product model from directory structure
      if ( strProductModel == STR_ConfigApp )
         strModel = getProductModel();
      //if running from eConfig, get product model from eConfig
      else
         strModel = strProductModel;
      }

   //edw 09/29/1998 - pass host interface HWflag instead of model
   if ( pProfile->bHardware && pProfile->bSoftware )
      fprintf(pTempFile, "%s %s %d%d%s \n", STR_TagRetrieve, STR_HWSW, pProfile->bFastPath, pProfile->bMRPD, pProfile->strBrowser);
   else if ( !pProfile->bHardware && !pProfile->bSoftware )
      fprintf(pTempFile, "%s %s %d%d%s \n", STR_TagRetrieve, STR_HWSW, pProfile->bFastPath, pProfile->bMRPD, pProfile->strBrowser);
   else if ( pProfile->bHardware )
      fprintf(pTempFile, "%s %s %d%d%s \n", STR_TagRetrieve, STR_HW, pProfile->bFastPath, pProfile->bMRPD, pProfile->strBrowser);
   else
      fprintf(pTempFile, "%s %s %d%d%s \n", STR_TagRetrieve, STR_SW, pProfile->bFastPath, pProfile->bMRPD, pProfile->strBrowser);

   fclose(pTempFile);

   //invoke user selected retrieval application
   boolean bLocalFiles = false;
//   if ( pProfile->strRetApp == STR_LocalFiles )  old logic 82420 MAMT 11/10/2000
   if ( IString(pProfile->strRetApp).upperCase() == IString(STR_LocalFiles).upperCase() ) // new logic 82420
      bLocalFiles = true;

   IString strHost;
   if ( bLocalFiles )
      {
      //usPath = enumFileConfig;
      //selectDirectory(strBrowsePath);
      strHost = strLocalRetrFile;
      bHostUsed=true;//R1570

      }
   else
      {
      strHost = strExecPath + pProfile->strRetApp;
      bInvRet=true; //1570
      }



   //* D87694 Begin new logic: Adds the call to the ConvertToShortPathName() function as it is more
   //* usefull for all path names convertions from long to short. This way, next code is disabled.

   //* D81872 Begin new logic: Changes a log path name of the interface host
   //* executable to a short path name.
   //* char *shortPath = (char*) malloc( strHost.length() + 1 );
   //* if( shortPath != NULL )
   //*   {
   //*   if( GetShortPathName(strHost, shortPath, strHost.length()+1) )
   //*      strHost = IString( shortPath );
   //*
   //*   free( shortPath );
   //*   }

   IString strShortName = ConvertToShortPathName(strHost);
   strHost = strShortName;

   //* D81872 Ends New logic:  HECC - 06/26/2003
   //* D87694 Ends New logic:  HECC - 02/10/2004



   LONG lRC = 0;
   if ( !bLocalFiles )
      {
      IString strPara = IString("\"")+ strHost +IString("\"");
      lRC = _spawnl(P_WAIT, strHost, strPara, strTempFile, NULL);
      }
   //* D84922 Begin: allow Error_wait_no_children
   //* D85278 Begin: Condition that allows RC = 128 (Error_wait_no_children)
   //* will be disabled as Ub9host4.exe is now returning correctly a RC = 0
   //* for a successfull execution.
   //* if ( !lRC || ( lRC == ERROR_WAIT_NO_CHILDREN ) )
   if ( !lRC )
   //* D84922 End:  VBB - 09/11/2003
   //* D85278 End:  HECC - 11/05/2003
      {
      //edw02/11/99-Load CPU lookup table
      USHORT usErrorOpen = pBackEnd->LoadCpuList();
      if ( usErrorOpen )
         {
         //* F79548 Begind New logic:  In case that the CPULOOK.tbl file couldn't
         //* be opened, display an error message showing the directory and the
         //* file that was not found.
         if (usErrorOpen == IDS_ErrorOpenCpuList_Text)
             usErrorOpen = IDS_ErrorDirCpuList_Text;

         //* F81710:  Begin new logic:  Change the error text displayed to the user. It
         //* will explain now that the CPULOOK.TBL file couldn't be found in any model
         //* directory.
         //* IString strText = LOADSTRING(usErrorOpen);
         //* IString strConfig = pProfile->strProdFamily;
         //* if (strConfig == STR_RS6000) strConfig = STR_RS6K;

         //* strText.change(STR_Substitute0, strExecPath +
         //*                                          strConfig +
         //*                                          STR_PathDelimiter +
         //*                                          STR_CpuTable );
         //*          strText.change(STR_Substitute1, strExecPath +
         //*                                          STR_CpuTable );
         //* 79548 Ends new Logic:    HECC - 04/10/2003
         //* F81710:  End new logic.  HECC - 06/03/2003

         //* F79548 Changes style from "Cancel" buton to a Yes/No choice for user
         //* HECC - 05/14/2003
         IMessageBox::Style style = //* IMessageBox::cancelButton |
                                    IMessageBox::yesNoButton |
                                    IMessageBox::warningIcon |
                                    IMessageBox::moveable |
                                    IMessageBox::systemModal;
         IMessageBox msg(this);
         msg.setTitle(IResourceId(IDS_ErrorBackEnd_Title));

         //* 79548 Begins new logic: The way to show the error message was
         //* changed so it can explain the exact directory and file not found
         //* It also includes the 'reply' variable to know the choice of the user
         //* Yes/No continue process.
         //* msg.show(IResourceId(usErrorOpen), style, IDH_ErrorBackEnd);

         //* F81710: Change again the format of the ID of the error message that must
         //* be displayed.
         //* IMessageBox::Response reply = msg.show(strText, style, IDH_ErrorBackEnd);
         IMessageBox::Response reply = msg.show(IResourceId(usErrorOpen), style,
                                                IDH_ErrorBackEnd);

         //* 79548 Ends new Logic:   HECC - 04/28/2003

         //* F79548 Begin New Logic: When doing a SSD lookup, none of the models will
         //* be selected in the profile, and the the CPULOOK.TBL file will not be
         //* found in any directory. In this case, if user selects "Yes" button,
         //* process, will continue, if user selects "No" button, application will end.
         if (reply == IMessageBox::no)
            {
             close();
             remove( strTempFile ); //edw05/19/99
             return;
            }
         //* F79548 End New Logic.
         }

      IString strCFR;
      IString strMRD, strNewMRD;
      IString strDSC;
      IString strBII;
      IString strCFT, strMRT;  //53915 MAMT 02/02/2001

      if ( bLocalFiles )
         {


         strCFR = strHost;
         strHost = strHost.remove( strHost.lastIndexOf( '.' ) );
         IString strTemp = strHost + IString(STR_ExtensionMRD);


         //53915 MAMT 02/02/2001 begin new logic
         strCFT = strHost + IString(STR_ExtensionCFT);
         strMRT = strHost + IString(STR_ExtensionMRT);
         remove(strCFT);
         remove(strMRT);
         rename (strCFR, strCFT);
         rename (strTemp, strMRT);
         CopyFile (strCFT, strCFR, FALSE);
         CopyFile (strMRT, strTemp, FALSE);
         //53915 MAMT 02/02/2001 end new logic
         WIN32_FIND_DATA fileData;
         HANDLE handle = FindFirstFile( (LPCTSTR)strTemp, &fileData );
         if ( handle != INVALID_HANDLE_VALUE )
            strMRD = strTemp;
         else
            {
            strTemp = strHost + IString(STR_ExtensionMRR);
            handle = FindFirstFile( (LPCTSTR)strTemp, &fileData ); //42619 - flw
            if ( handle != INVALID_HANDLE_VALUE )                  //Handle *.MRR file types
               strMRD = strTemp;
            }

         strMRDMulti=strMRD; // R1570

         strTemp = strHost + IString(STR_ExtensionDSC);
         handle = FindFirstFile( (LPCTSTR)strTemp, &fileData );
         if ( handle != INVALID_HANDLE_VALUE )
            strDSC = strTemp;
         FindClose( handle );
         }
      else
         {

         pTempFile = fopen(strTempFile, "r");
         char szFile[512];
         Boolean bFiles = false;
         while ( fgets(szFile, sizeof(szFile), pTempFile) )
            {
            IString strFile = szFile;
            strFile.remove(strFile.length());
            if ( strFile == STR_TagFiles )
               {
               bFiles = true;
               continue;
               }
         //   if ( strFile == STR_TagReturn )
         //      break;

            if ( strFile == STR_TagReturn )
               //break;
               continue;
            if ( strFile.subString(1, 2) == "RC" )
               {
                  int i = strFile.subString(7, 1 ).asInt();
                  if( i != 2 )
                     pProfile->bMRPD = i;
                  switch ( strFile.subString(4, 1 ).asInt() )
                  {
                     case 1:
                     case 2:
                        remove( strTempFile );
                        bLock = false;
                        if( pProfile->bFastPath && pProfile->bEconfig)// Usability
                           postEvent(IWindow::systemCommand, ISystemMenu::idClose);  //58370
                        else
                           {                                                     // D72623 EDW 07/01/2002
                           cleanTempFiles(strBII, strCFR, strMRT, strCFT);       // D72623 EDW 07/01/2002
                           return;
                           }                                                     // D72623 EDW 07/01/2002
                  }
               break;
               }

            if ( bFiles )
               {
               IString strType = strFile.word(1);
               strFile = strFile.words(2);
               if ( strType == STR_TypeCFREPORT )
                  strCFR = strFile;
               else if ( strType == STR_TypeCFR )
                  strCFR = strFile;
               else if ( strType == STR_TypeMRD )
                  strMRD = strFile;
               else if ( strType == STR_TypeMRR )  //42619 - flw
                  strMRD = strFile;
               else if ( strType == STR_TypeDSC )
                  strDSC = strFile;
               else
                  remove(strFile);
               }
            }
         fclose(pTempFile);
         }




      //this->strMRD=strMRD; // ZOR 10/15/2004 F94021
      IString strDateTxt;
      if ( strMRD.length() )
         {
            IString strDate;
            strDate=getSensedDate(strMRD);
            if ( !strDate.isDigits() )
               strDateTxt = STR_NoSensedDateRetrieved;
            else
               strDateTxt = strDate.subString(5,2)+
                              STR_Hyphen+strDate.subString(7,2)+
                              STR_Hyphen+strDate.subString(1,4);
         }


      if ( strCFR.length() || strMRD.length() )
         {
         USHORT usError; //edw02/11/99

         //edw05/29/1998-Determine if multiple systems were retrieved
         if ( strCFR.length() )
            bMultSysVPDTran = hasVPDTransData(strCFR);
         if ( strMRD.length() )
            {
            bMultSysStackedMRDB = hasStackedMRDB(strMRD);

            //edw12/15/98-Generate unique SN's for products in MRDB w/ SN's of zero
            Boolean bDone = regenerateSerialNbrs( strMRD );
            if ( !bDone )
               {
               remove( strTempFile ); //edw05/19/99
               bLock = false;
               if( pProfile->bFastPath && pProfile->bEconfig)// Usability
                  postEvent(IWindow::systemCommand, ISystemMenu::idClose);  //58370
               else
                  {                                                     // D72623 EDW 07/01/2002
                  cleanTempFiles(strBII, strCFR, strMRT, strCFT);       // D72623 EDW 07/01/2002
                  return;           //End the retrieval
                  }                                                     // D72623 EDW 07/01/2002
               }
            }

         //*F100160 Begin logic:  Determine whether the input CFR belongs to Blue Horizon configurator or xSeries
         //*                      product family. The process will then be stopped.

         if ( bIsBlueHor || bIsXseries)
            {

             IMessageBox::Style style = IMessageBox::okButton |
                                        IMessageBox::errorIcon |
                                        IMessageBox::moveable |
                                        IMessageBox::applicationModal ;
             IMessageBox msg(this);

             if (bIsBlueHor)
                {
                 msg.setTitle(IResourceId(IDS_CFReportNotAccepted_Title));
                 msg.show(IResourceId(IDS_BlueHorizonNotAccepted_Text),style);
                }

             if (bIsXseries)
                {
                 msg.setTitle(IResourceId(IDS_CFReportNotAccepted_Title));
                 msg.show(IResourceId(IDS_XseriesProductsNotAccepted_Text),style);
                }

             remove( strTempFile );
             bLock = false;
             cleanTempFiles(strBII, strCFR, strMRT, strCFT);
             return;

            }
         //*F100160 End logic:  HECC - 07/26/2005

         //edw05/29/1998-Create MRDB file from VPDTran records in CFReport
         if ( bMultSysVPDTran )
            {
            if ( strMRD.length() )
               bMultSysVPDTran = false;
            else
               bMultSysVPDTran = true;
            if ( bMultSysVPDTran )
               {
               strNewMRD = createMRDBFile(strCFR);
               if ( strNewMRD == STR_DummyRecType )
                  {
                  remove( strTempFile ); //edw05/19/99
                  bLock = false;
                  if( pProfile->bFastPath && pProfile->bEconfig)// Usability
                      postEvent(IWindow::systemCommand, ISystemMenu::idClose);  //58370
                  else
                     {                                                     // D72623 EDW 07/01/2002
                     cleanTempFiles(strBII, strCFR, strMRT, strCFT);       // D72623 EDW 07/01/2002
                     return;        //End the retrieval
                     }                                                     // D72623 EDW 07/01/2002
                  }
               }
            }

         if ( strNewMRD.length() )
            strMRD = strNewMRD;
         //else                                                     //edw test only
         //   strMRD=strBrowsePath+STR_PathDelimiter+"$rt2$.mrd";   //edw test only
         if ( bMultSysStackedMRDB )
            {
            Boolean bInst = resequenceInstIds( strMRD );
            if ( !bInst )
               {
               remove( strTempFile ); //edw05/19/99
               bLock = false;
               if( pProfile->bFastPath && pProfile->bEconfig)// Usability
                  postEvent(IWindow::systemCommand, ISystemMenu::idClose);  //58370
               else
                  {                                                     // D72623 EDW 07/01/2002
                  cleanTempFiles(strBII, strCFR, strMRT, strCFT);       // D72623 EDW 07/01/2002
                  return;           //End the retrieval
                  }                                                     // D72623 EDW 07/01/2002
               }
            }

         /* Begin New logic: Determine if the retrieved data is from the AS400 */
         /* product group   F72273  HECC: 06/03/2002 */

         /* F81710 Begin new logic: Lines not needed, as this flag was already set */
         /* before with the parsed parameter from eConfig or the model preference  */
         /* from the profile.                                                      */
         /* if ( pProfile->strProdFamily == STR_AS400 ) */
         /*    bAS4 = true;  */
         /* else             */
         /*    bAS4 = false; */
         /* F81710 End new logic:   HECC: 06/03/2003 */

         /* End New logic:  F72273  HECC: 06/27/2002 */

         // edw06/29/1998-Determine if retrieved data is from the S/390 product group
         if ( strFirstHWRecType.length() )
            {
            //IString strProdGrp = myCpuList.getISMProductModel( strFirstHWRecType );
            IString strProdGrp = pBackEnd->getCpuList().getISMProductModel( strFirstHWRecType );

            if ( strProdGrp == STR_S390 )
               bS390 = true;
            else
               bS390 = false;
            }
         else
            {
            if ( strModel.length() )
               {
               if( strModel == STR_S390 )
                  bS390 = true;
               else
                  bS390 = false;
               }
            else
               bS390 = false;
            }


         /* F81710 Begin new logic: If retrieved data is from the S/390 product group change */
         /* also the value of strConfig used to build the path of the feature.$$ file  */
         if (bS390)
            strConfig = STR_S390;
         /* F81710 End new logic:   HECC - 06/04/2003 */

         // Both AAS and MRDB retrieved from host
         if ( strCFR.length() && strMRD.length() )
            {
            //edw06/22/1998

            //* F88290 Begin logic: Added condition to display the "Reconcile the retrieved data?" window
            //* only in case that the retrieved configuration be other than an S390.  Otherwise, avoid
            //* display it and automatically reconcile configuration.
            if (!bS390)
               {
               // F86752 It reads the .CFR to check if the 06 record has the ALL label
               // if it has then it won't show the "Reconcile the retrieved data?" window
               // because all the data is already reconciled by Inventory Services.
               IString buf;     //F86752 Req 1373  starting
               int ISflag = 1;
               char * ReadBuff1 = new char[READBUF_SIZE1];
               FILE  *fall;
               if (fall = fopen(strCFR,"r"))      // F86752 Req 1373
                {
                 while (fgets(ReadBuff1,READBUF_SIZE1,fall))
                  {
                   buf = ReadBuff1;
                   if (buf.subString(1, 2) == "06" && buf.subString(13,2)=="FL" && buf.subString(15,3)=="ALL")
                      {
                      ISflag=0;
                      bResolve = true;
                      break;
                      }
                  }
                 }
                fclose(fall);
                if(ISflag)    //F86752 Req 1373
                 {
                  ReconcileData *pRec;
                  pRec = new ReconcileData( this, strDateTxt);
                  IPoint mainPos = this->position();
                  mainPos.setX( mainPos.x() + 20 );
                  mainPos.setY( mainPos.y() + 20 );
                  pRec->moveTo( mainPos );
                  pRec->addToWindowList();
                  pRec->setFocus();
                  pRec->showModally();
                 } // F86752 req 1373    end
               }
            else
               bResolve = true;
            //* F88290 End logic:  HECC - 02/26/2004

            }

         // Only AAS retrieved from host (no MRDB data)
         else if ( strCFR.length() )
            {
            msg.setTitle(IResourceId(IDS_NoMRDBRetrieved_Title));
            if ( bS390 )
               {
               //if ( pProfile->bMRPD )          // 64340 ZOR 08/29/01
               //   {
               //When bS390 flag is true, the IDS_NoMRDBS390_Text message
               //should always be displayed
               IMessageBox::Style style = IMessageBox::okButton |
                                          IMessageBox::errorIcon |
                                          IMessageBox::moveable |
                                          IMessageBox::systemModal;
               response = msg.show(IResourceId(IDS_NoMRDBS390_Text), style,
                                          IDH_NoMRDBRetrieved);
               //   }
               }

             else
               // bUseAAS = true;   51080 - MAMT 06/06/2000
               // This logic should not have been modified with 51080 above
               // as it is working as designed
               // 64340 ZOR 08/29/01
               {
               if( pProfile->bMRPD )
                  {
                  IMessageBox::Style style = IMessageBox::okCancelButton |
                                          IMessageBox::informationIcon |
                                          IMessageBox::moveable |
                                          IMessageBox::systemModal;
                  response = msg.show(IResourceId(IDS_NoMRDBRetrieved_Text), style,
                                          IDH_NoMRDBRetrieved);
                  if ( response == IMessageBox::ok )
                     bUseAAS = true;
                  }
               else
                  bUseAAS = true;
               }
            }

         // Only MRDB retrieved from host (no AAS data)
         else
            {
            IMessageBox::Style style = IMessageBox::okCancelButton |
                                       IMessageBox::informationIcon |
                                       IMessageBox::moveable |
                                       IMessageBox::systemModal;
            msg.setTitle(IResourceId(IDS_NoAASRetrieved_Title));
            response = msg.show(IResourceId(IDS_NoAASRetrieved_Text),
                                       style, IDH_NoAASRetrieved);
            if ( response == IMessageBox::ok )
               bUseMRDB = true;
            }

         if ( bResolve )
            {
            //edw07/07/1998-Load No Compare Table for S/390 users
            if ( bS390 )
               {

               //* 79548 Begins new logic:  Passes the Product Family profile variable
               //* to the method that loads the NOCOMP.TBL file to correctly load it
               //* from the directory where it is stored.
               //* usError = loadNoCompareTable( strExecPath );
               usError = loadNoCompareTable(strExecPath + STR_S390 + STR_PathDelimiter);
               //* 79548 Ends new Logic:    HECC - 04/10/2003

               if ( usError )
                  {
                  //* 79548 Begins new logic:  When an error occurs, verify if it was
                  //* caused because non-existing directory or non-existing file.
                  IString strText = LOADSTRING(usError);
                  Boolean bPathExist = validProdFamilyPath(strExecPath + STR_S390 +
                                                           STR_PathDelimiter);
                  //* If the path doesn't exist, display it to the user
                  if (!bPathExist)
                     {
                      strText = LOADSTRING(IDS_ErrorOpenDir_Text);
                      strText.change(STR_Substitute0, STR_S390);
                     }
                  else
                     //* F81710: If path exists, display the path where the file was not found
                     strText.change(STR_Substitute0, strExecPath + STR_S390 +
                                                      STR_PathDelimiter);

                  //* 79548 Ends new Logic:   HECC - 04/28/2003

                  //* F81710 Changes style from "Cancel" buton to a Yes/No choice for user
                  //* HECC - 06/03/2003
                  IMessageBox::Style style = //* IMessageBox::cancelButton |
                                             IMessageBox::yesNoButton |
                                             IMessageBox::warningIcon |
                                             IMessageBox::moveable |
                                             IMessageBox::systemModal;

                  msg.setTitle(IResourceId(IDS_ErrorBackEnd_Title));

                  //* 79548 Begins new logic: The way to show the error message was
                  //* changed so it can explain if directory or file was not found
                  //* msg.show(IResourceId(usError), style, IDH_ErrorBackEnd);

                  //* F81710 Begin Logic: The 'reply' variable was included to save the
                  //* choice of the user if he Yes/No wishes to continue process.
                  //* msg.show(strText, style, IDH_ErrorBackEnd);
                  IMessageBox::Response reply = msg.show(strText,style,IDH_ErrorBackEnd);
                  //* F81710 Ends new Logic:   HECC - 06/03/2003

                  //* 79548 Ends new Logic:   HECC - 04/28/2003

                  //* F81710 Begin New Logic: If user selects "No" button, application will end.
                  //* If user selects "Yes", an explanation of what the logic will do is displayed
                  //* and continue execution.
                  if (reply == IMessageBox::no)
                     {
                      remove( strTempFile ); //edw05/19/99
                      bLock = false;
                      close();
                      cleanTempFiles(strBII, strCFR, strMRT, strCFT);       // D72623 EDW 07/01/2002
                      return;
                     }
                  //* F81710 End New Logic. HECC 06/03/2003

                  }
               // begin new logic 49222 - MAMT - 06/27/00 Load Quick Fix table


               //* 79548 Begins new logic:  Passes the Product Family profile variable
               //* to the method that loads the QUICKFIX.TBL file to correctly load it
               //* from the directory where it is stored.
               //* usError = loadQuickFixTable( strExecPath );
               usError = loadQuickFixTable( strExecPath + STR_S390 + STR_PathDelimiter );
               //* 79548 Ends new Logic:    HECC - 04/10/2003

               if ( usError )
                  {

                  //* 79548 Begins new logic:  When an error occurs, verify if it was
                  //* caused because non-existing directory or non-existing file.
                  IString strText = LOADSTRING(usError);
                  Boolean bPathExist = validProdFamilyPath(strExecPath + STR_S390 +
                                                           STR_PathDelimiter);
                  //* If the path doesn't exist, display it to the user
                  if (!bPathExist)
                     {
                      strText = LOADSTRING(IDS_ErrorOpenDir_Text);
                      strText.change(STR_Substitute0, STR_S390);
                     }
                  else
                     //* F81710: If path exists, display the path where the file was not found
                     strText.change(STR_Substitute0, strExecPath + STR_S390 +
                                                     STR_PathDelimiter);

                  //* 79548 Ends new Logic:   HECC - 04/28/2003

                  //* F81710 Changes style from "Cancel" buton to a Yes/No choice for user
                  //* HECC - 06/03/2003
                  IMessageBox::Style style = //* IMessageBox::cancelButton |
                                             IMessageBox::yesNoButton |
                                             IMessageBox::warningIcon |
                                             IMessageBox::moveable |
                                             IMessageBox::systemModal;

                  msg.setTitle(IResourceId(IDS_ErrorBackEnd_Title));

                  //* 79548 Begins new logic: The way to show the error message was
                  //* changed so it can explain if directory or file was not found
                  //* msg.show(IResourceId(usError), style, IDH_ErrorBackEnd);

                  //* F81710 Begin Logic: The 'reply' variable was included to save the
                  //* choice of the user if he Yes/No wishes to continue process.
                  //* msg.show(strText, style, IDH_ErrorBackEnd);
                  IMessageBox::Response reply = msg.show(strText,style,IDH_ErrorBackEnd);
                  //* F81710 Ends new Logic:   HECC - 06/03/2003

                  //* 79548 Ends new Logic:   HECC - 04/28/2003
                  //* F81710 Begin New Logic: If user selects "No" button, application will end.
                  //* If user selects "Yes", an explanation of what the logic will do is displayed
                  //* and continue execution.
                  if (reply == IMessageBox::no)
                     {
                      remove( strTempFile ); //edw05/19/99
                      bLock = false;
                      close();
                      cleanTempFiles(strBII, strCFR, strMRT, strCFT);       // D72623 EDW 07/01/2002
                      return;
                     }
                  //* F81710 End New Logic. HECC 06/03/2003
                  }
               // finish new logic 49222 - MAMT - 06/27/00
               // Create the Feature table to be loaded in the system.cpp to eliminate
               // the 54, 55 and 56 records from the RET file  53915 - MAMT 08/31/2000
               sOutputRec54 = "*TYP12MODELFEATURE";
               pFeature54Table->add(sOutputRec54);
               }

               // Begin:  Load iSeries SW Feature Table for iSeries or AS400
               // configurations.  HECC 06/27/2002  F72273
               if ( bAS4 )
                  {

                  //* 79548 Begins new logic:  Passes the Product Family profile variable
                  //* to the method that loads the ISERIESW.TBL file to correctly load it
                  //* from the directory where it is stored.
                  //* usError = loadISeriesSWFeatureTable( strExecPath );
                  usError = loadISeriesSWFeatureTable( strExecPath + STR_AS400 +
                                                       STR_PathDelimiter         );
                  //* 79548 Ends new Logic:    HECC - 04/10/2003

                  if ( usError )
                     {

                     //* 79548 Begins new logic:  When an error occurs, verify if it was
                     //* caused because non-existing directory or non-existing file.
                     IString strText = LOADSTRING(usError);
                     Boolean bPathExist = validProdFamilyPath(strExecPath + STR_AS400 +
                                                              STR_PathDelimiter);
                     //* If the path doesn't exist, display it to the user
                     if (!bPathExist)
                        {
                         strText = LOADSTRING(IDS_ErrorOpenDir_Text);
                         strText.change(STR_Substitute0, STR_AS400);
                        }
                     else
                        //* F81710: If path exists, display the path where the file was not found
                        strText.change(STR_Substitute0, strExecPath + STR_AS400 +
                                                        STR_PathDelimiter);

                     //* 79548 Ends new Logic:   HECC - 04/28/2003

                     //* F81710 Changes style from "Cancel" buton to a Yes/No choice
                     //* HECC - 06/03/2003
                     IMessageBox::Style style = //* IMessageBox::cancelButton |
                                                IMessageBox::yesNoButton |
                                                IMessageBox::warningIcon |
                                                IMessageBox::moveable |
                                                IMessageBox::systemModal;

                     msg.setTitle(IResourceId(IDS_ErrorBackEnd_Title));

                     //* 79548 Begins new logic: The way to show the error message was
                     //* changed so it can explain if directory or file was not found
                     //* msg.show(IResourceId(usError), style, IDH_ErrorBackEnd);

                     //* F81710 Begin Logic: The 'reply' variable was included to save the
                     //* choice of the user if he Yes/No wishes to continue process.
                     //* msg.show(strText, style, IDH_ErrorBackEnd);
                     IMessageBox::Response reply = msg.show(strText,style,IDH_ErrorBackEnd);
                     //* F81710 Ends new Logic:   HECC - 06/03/2003

                     //* 79548 Ends new Logic:   HECC - 04/28/2003

                     //* F81710 Begin New Logic: If user selects "No" button, application will end.
                     //* If user selects "Yes", an explanation of what the logic will do is displayed
                     //* and continue execution.
                     if (reply == IMessageBox::no)
                        {
                         remove( strTempFile ); //edw05/19/99
                         bLock = false;
                         close();
                         cleanTempFiles(strBII, strCFR, strMRT, strCFT);  //* F81710  HECC 06/03/2003
                         return;
                        }
                     //* F81710 End New Logic. HECC 06/03/2003

                     }
                  }
               // End HECC 06/27/2002  F72273

               bUseAAS = true;
               // Instantiate the RM/2 object and replace the .CFR file from
               // the Retrieve CPF with the .CFR file produced in RM/2.  If no
               // .CFR file is produced in RM/2, use the .CFR file from Retrieve

               // Instantiate the RM/2 user profile
               EUOPProfile * pUserProfile = new EUOPProfile();
               if (!pUserProfile->ReadProfile())
                  {
                  pUserProfile->WriteProfile();     // just writing will get you  no file
                  delete pUserProfile;              // writing and reading will get you a zero-length 's' file
                  pUserProfile = new EUOPProfile(); // recreating the object gets you 's' file with a length
                  pUserProfile->ReadProfile();
                  }

               // PSC D80040 10/07/2003 start -----------------
               // Execute new function to move the RM2 Registry
               // Entries located in the wrong path.-----------
               // This re-located is executed after the IPro---
               // file is instantiated, the RM2 Registry En----
               // try path is passed as a parameter to the ----
               // IProfile constructor which records the wrong-
               // Registry Entry Path.
               Profile::changeLocationOfRM2Keys();
               // PSC D80040 10/07/2003 end -------------------

               //* 79548 Begin New Logic:  Adds the Product Family profile preference
               //* to the path of the file feature.$$f that has been retrieved from
               //* the registry to load it from the directory that is specified in
               //* this user preference.

               //* F81710 Begin New logic: Next two lines of code are commented because
               //* the strConfig variable is being set before
               //* IString strConfig = pProfile->strProdFamily;
               //* if (strConfig == STR_RS6000) strConfig = STR_RS6K;
               //* F81710 End new logic:  HECC- 06/04/2003

               pUserProfile->ChangePKFilePath(strExecPath, strConfig);
               //* 79548 Ends new Logic:    HECC - 04/10/2003

               // Set DLL from which RM/2 resources will be loaded
               //IApplication::current().setResourceLibrary ("RM2OOR3U");




    //Begin 1704

  IString strRET;

  if ( ( strCFR.length() && strMRD.length() )&&( bS390==true))
    {

    //manipulate the BIR file to get the MRD serial number
    UpdateBIR(strCFR);

     if(is1704==true)
     {strCFR= CFRDummy;
     }


                // Rename *.CFReport file to fit RM/2 naming convention
               strBII = strCFR;
               strBII.remove(strBII.lastIndexOf('.') + 1);
               strBII += IString(STR_ExtensionBII);
               remove(strBII);
               rename(strCFR, strBII);
             //  IString strRET = strCFR;                     // D72481 EDW 07/01/2002
               strRET = strCFR;                             //1704
               strRET.remove(strRET.lastIndexOf('.'));      // D72481 EDW 07/01/2002
               strRET += IString(STR_ExtensionRET);         // D72481 EDW 07/01/2002
               remove(strRET);                              // D72481 EDW 07/01/2002


    } //end if

    else
    {

               // Rename *.CFReport file to fit RM/2 naming convention
               strBII = strCFR;
               strBII.remove(strBII.lastIndexOf('.') + 1);
               strBII += IString(STR_ExtensionBII);
               remove(strBII);
               rename(strCFR, strBII);
             //  IString strRET = strCFR;                     // D72481 EDW 07/01/2002
               strRET = strCFR;                             //1704
               strRET.remove(strRET.lastIndexOf('.'));      // D72481 EDW 07/01/2002
               strRET += IString(STR_ExtensionRET);         // D72481 EDW 07/01/2002
               remove(strRET);                              // D72481 EDW 07/01/2002
     } // END 1704






               // Instantiate the RM/2 Main Window
               // XWindow * pRM2mainw = new XWindow(WND_MAIN,strBII,strMRD,strDSC,helpWindow(),pUserProfile);
               try{
               XWindow  pRM2main( WND_MAIN,strBII,strMRD,strDSC,this,helpWindow(),pUserProfile );
               pRM2main.pFeature54Table = pFeature54Table; //53915-MAMT-09/01/2000
               pRM2main.showModally();
               }
               catch(IException& exc)
               {
                  IString strError = (IString)exc.text();
                  IMessageBox m(this);
                  m.show( strError, IMessageBox::information );
                  remove( strTempFile ); //edw05/19/99
                  close();
                  cleanTempFiles(strBII, strCFR, strMRT, strCFT);       // D72623 EDW 07/01/2002
                  return;
               }
               //jtest-11-21-97 delete pRM2main;
               IThread::current().sleep(1500); //jt
               delete pUserProfile;
               if (!CopyFile (strRET, strCFR, false))       // D72481 EDW 07/01/2002 - if a reconciled report was created, use it
                  CopyFile( strBII, strCFR, false);         // D72481 EDW 07/01/2002 - otherwise, use the original CFR file from AAS

               // See if RM2 generated a *.CFReport File (successfully reconciled)
               //if( rename(strBII, strCFR) != 0 )
               //   bUseMRDB = false;                //Use new reconcile *.cfr file
               //else
               //   bResolve = false;                //Use original *.cfr or *.mrd file
            } // end b-s390

         // If exists a discrepancy between AAS and MRDB 53915a - MAMT 02/01/2001
         pBackEnd->Feature54TableSet1 = *pFeature54Table;
         if (pBackEnd->Feature54TableSet1.numberOfElements() >  1 )
            {
             IMessageBox::Style style = IMessageBox::yesNoButton |
                                        IMessageBox::defButton2 |
                                        IMessageBox::warningIcon |
                                        IMessageBox::moveable |
                                        IMessageBox::systemModal ;
             IMessageBox msgrepdata(this);
                        msgrepdata.setTitle(IResourceId(IDS_PlacementData_Title));
             response = msgrepdata.show(IResourceId(IDS_PlacementData_Text), style,
                                        IDH_PlacementData);

             switch( response ) {
                case IMessageBox::yes:
                     if ( bLocalFiles )
                       {
                        remove(strCFT);
                        remove(strMRT);
                        IString sPlacementData  = "05         10   Reconciliation used AAS data, but placement data exists\n";
                        IString sPlacementData1 = "05         10       for affected f/c. User Chose to continue\n";
                        IString strRET;
                        IString buf;
                        IString vtotalstr;
                        char * ReadBuff1;
                        int vtotal;
                        Boolean b05rec = false;
                        ReadBuff1 = new char[READBUF_SIZE1];
                        strBII.remove(strBII.lastIndexOf('.') + 1);
                        strRET = strBII + IString(STR_TypeRET);
                        FILE  *f;
                        FILE  *f1;
                        if (f1 = fopen(strCFT,"w"))
                           {
                           if (f = fopen(strRET,"r"))
                             {
                              while (fgets(ReadBuff1,READBUF_SIZE1,f))
                                {
                                  buf = ReadBuff1;
                                  if (buf.subString(1, 2).asInt() == 5)
                                   {
                                    fputs(buf,f1);
                                    fputs(sPlacementData,f1);
                                    fputs(sPlacementData1,f1);
                                    b05rec = true;
                                   }
                                  else
                                   {
                                    if (buf.subString(1, 2).asInt() == 98)
                                      {
                                       if (b05rec)
                                         {
                                          int vtotal = buf.subString(3, 5).asInt();
                                          vtotal++;
                                          vtotal++;
                                          vtotalstr = IString(vtotal);
                                          while (vtotalstr.length() < 5)
                                              vtotalstr = " " + vtotalstr;
                                          buf = buf.subString(1, 2) + vtotalstr;
                                          fputs(buf,f1);
                                          b05rec = false;
                                         }
                                      }
                                    else
                                      fputs(buf,f1);
                                   }
                                } // end-while
                             } // fopen f
                          } // fopen f1
                        fclose(f);
                        fclose(f1);
                        remove(strRET);
                        rename(strCFT, strRET);
                       }
                     sOutputRec54 = "PLACEMENTDATA";
                     pBackEnd->Feature54TableSet1.add(sOutputRec54);
                break;
                case IMessageBox::no:
                     IString strCFREPORT, strRET, strMSG;
                     remove(strBII);
                     remove(strCFR);
                     strBII.remove(strBII.lastIndexOf('.') + 1);
                     strCFREPORT = strBII + IString(STR_TypeCFREPORT);
                     strRET = strBII + IString(STR_TypeRET);
                     strMSG = strBII + IString(STR_TypeMSG);
                     remove(strCFREPORT);
                     remove(strRET);
                     remove(strMSG);
                     remove(strMRD);
                     bLock = false;
                     if ( bLocalFiles )
                        {
                         rename(strCFT, strCFR);
                         rename(strMRT, strMRD);
                        }
                     cleanTempFiles(strBII, strCFR, strMRT, strCFT);       // D72623 EDW 07/01/2002
                     return;
                break;
                } // switch
            } // if pbackend

         // User wants to end current retrieval session

         if ( !bUseMRDB && !bUseAAS && !bResolve )
            {
            remove( strTempFile ); //edw05/19/99
            bLock = false;
            if( pProfile->bFastPath && pProfile->bEconfig)// Usability
               postEvent(IWindow::systemCommand, ISystemMenu::idClose);  //58370
            else
               {                                                     // D72623 EDW 07/01/2002
               cleanTempFiles(strBII, strCFR, strMRT, strCFT);       // D72623 EDW 07/01/2002
               return;
               }                                                     // D72623 EDW 07/01/2002
            }

         // User wants to use MRDB as the base (BaseRet invoked by ISM).
         else if ( ( bUseMRDB ) && (bConfig) )        //edw12/16/97
            {

            //* D87694 Begin new logic: Adds the call to the ConvertToShortPathName() function
            //* for convertion from long to short path of the .RET and .MRD filenames.

            IString strShortName = ConvertToShortPathName(strGeneratePath);
            strGeneratePath = strShortName;
            strShortName = ConvertToShortPathName(strMRD);
            strMRD = strShortName;

            //* D87694 Ends New logic:  HECC - 02/10/2004

            strGenerateName = STR_RetrievalFile;
            IString strRET = strGeneratePath + STR_PathDelimiter +
                             strGenerateName + STR_ExtensionRET;

            IString strCommand = STR_CopyCommand;
            strCommand = strCommand + STR_Blank1 + strMRD + STR_Blank1 + strRET;
            system(strCommand);
            //if (bConfig) //jt-07/25/97              //edw12/16/97
            postEvent(IWindow::command, UM_ReturnRetrieval);
            }

         // User wants to use MRDB (BasRet standalone) or AAS as base.
         else
            {
            if ( bUseMRDB )                           //edw12/16/97
               {
               IString strCNV=strMRD;
               strCNV.remove( strCNV.lastIndexOf('.') + 1 );
               strCNV += IString( STR_ExtensionCNV );
               Convert2031 *pConvert = new Convert2031();
               ULONG iCode = pConvert->convertCFReport( strMRD );
               delete pConvert;
               if ( !iCode )
                  {
                  remove(strCFR);
                  strCFR = strMRD;
                  strCFR.remove(strCFR.lastIndexOf('.') );
                  strCFR += IString( STR_ExtensionCFR );
                  rename(strCNV, strCFR);
                  }
               else
                  {
                  IMessageBox::Style style = IMessageBox::cancelButton |
                              IMessageBox::warningIcon |
                              IMessageBox::moveable |
                              IMessageBox::systemModal;
                  IMessageBox msgcvnt(this);
                  msgcvnt.setTitle(IResourceId(IDS_ConvertError_Title));
                  msgcvnt.show(IResourceId(iCode), style,
                                           IDH_ConvertError);
                  remove( strTempFile ); //edw05/19/99
                  bLock = false;
                  if( pProfile->bFastPath && pProfile->bEconfig)// Usability
                    postEvent(IWindow::systemCommand, ISystemMenu::idClose);  //58370
                  else
                     {                                                     // D72623 EDW 07/01/2002
                     cleanTempFiles(strBII, strCFR, strMRT, strCFT);       // D72623 EDW 07/01/2002
                     return;
                     }                                                     // D72623 EDW 07/01/2002
                  }
               }

            IString strBIR = strBrowsePath + STR_PathDelimiter +
                             STR_RetrievalFile + STR_ExtensionBIR;
            remove(strBIR);
            IContainerControl::ObjectCursor crs(*pcnrFiles);
            for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
               {
               CfReportFileStats* pObject = (CfReportFileStats*)crs.current();
               if ( IString(pObject->name()).upperCase() ==  STR_RetrievalFile )
                  {
                  pcnrFiles->removeObject(pObject);
                  IThread::current().sleep(500);//jt
                  delete pObject;
                  break;
                  }
               }

            //edw-copy cfrfile to birfile instead of replacing cfrfile with birfile
            //if (rename(strCFR, strBIR) != 0)
            //   {
            //   rename(strBII, strBIR);
            //   }
            if( !CopyFile(strCFR, strBIR, FALSE) )
               CopyFile(strBII, strBIR, FALSE);

            CfReportFileStats* pNew = new CfReportFileStats(STR_RetrievalFile);
            pNew->sCfrFullFileName = strBIR;
            pBackEnd->ParseCFREPORT(*pNew);
            pNew->setName(pBackEnd->strTempName);
            /*start IC65167 fix ZOR 072600*/
            if (pBackEnd->strTempName.subString(6,5).strip()=="")
               {
                 IMessageBox::Style style = IMessageBox::cancelButton |
                                            IMessageBox::warningIcon |
                                            IMessageBox::moveable |
                                            IMessageBox::systemModal;
                                            IMessageBox msg (this);
                 msg.setTitle(IResourceId(IDS_ErrorBackEnd_Title));
                 msg.show(IResourceId(IDS_ErrorNoSerialNo_Text), style, IDH_ErrorBackEnd);
               }
            /*end of IC65167 fix ZOR 0702600*/
            IString tempfile = strBrowsePath + STR_PathDelimiter + pBackEnd->strTempName + STR_ExtensionBIR;
            // Begin new logic 52014 MAMT - 28/07/2000
            oldfileB = "";
            oldfileR = "";
            tempfileR= "";
            if ( !stat(tempfile , &fstat) )
               {
                IMessageBox msg(this);
                IMessageBox::Style style = IMessageBox::yesNoButton |
                                           IMessageBox::defButton2 |
                                           IMessageBox::warningIcon |
                                           IMessageBox::moveable |
                                           IMessageBox::systemModal;
                msg.setTitle(IResourceId(IDS_ReplaceFile_Title));
                IString strText = LOADSTRING(IDS_ReplaceFile_Text);
                strText.change(STR_Substitute0, tempfile);
                response = msg.show(strText, style,IDH_ReplaceFile);
                if ( response == IMessageBox::no )
                   {
                   // user doesn't want to replace the file (rename)
                   pSelectedFile = (CfReportFileStats*)pcnrFiles->cursoredObject();
                   strSaveName = pSelectedFile->name();
                   if ( pSelectedFile == pFile )
                      {
                       if ( pOrders )
                          {
                          pOrders->clearNotebook();
                          pOrders->hide();                      //edw01/23/98
                          pOrders->removeFromWindowList();      //edw01/23/98
                          }
                       if ( pSystems )
                          {
                          pSystems->clearNotebook();
                          pSystems->hide();                      //edw01/23/98
                          pSystems->removeFromWindowList();      //edw01/23/98
                          }
                       if ( pDetail )
                          pDetail->postEvent(IWindow::systemCommand, ISystemMenu::idClose);
                       Boolean bDeleted1 = !pBackEnd->CloseFile();
                      }
                   tempfile = strBrowsePath + STR_PathDelimiter + pBackEnd->strTempName +
                                              STR_ExtensionBIR;
                   tempfileB= tempfile;
                   tempfileR= strBrowsePath + STR_PathDelimiter + pBackEnd->strTempName +
                                              STR_ExtensionRET;
                   oldfileB = strBrowsePath + STR_PathDelimiter + pBackEnd->strTempName +
                                              STR_ExtensionBYZ;
                   oldfileR = strBrowsePath + STR_PathDelimiter + pBackEnd->strTempName +
                                              STR_ExtensionCYZ;
                   rename(tempfile, oldfileB);
                   rename(tempfileR, oldfileR);
                   }
                else
                   {
                   // user wants to replace the file
                   pSelectedFile = (CfReportFileStats*)pcnrFiles->cursoredObject();
                   strSaveName = pSelectedFile->name();
                   if (strSaveName.upperCase() == pBackEnd->strTempName.upperCase())
                      deleteFile1();
                   else
                      {
                      strSaveName = pBackEnd->strTempName;
                      deleteFile1();
                      strSaveName = pSelectedFile->name();
                      }
                   }
               }
            // End new logic 52014 MAMT - 28/07/2000

            if (rename(strBIR, tempfile) != 0);
               rename(strBII, tempfile);
            pcnrFiles->addObject(pNew);
            pNew->sCfrFullFileName = tempfile;
            IThread::current().sleep(500);//jt
            sortFiles();
            //edw01/23/98-Remove if clause so retrieved file is alwayed renamed and opened
            //edwif ( !pFile )
             //edw   {
              pcnrFiles->setCursor(pNew);
               pcnrFiles->scrollToObject(pNew);
               //edw 12/5/97 Will open after the user names the file
               //postEvent(IWindow::command, IDMI_SelectedOpen);
            // old logic 52012 postEvent(IWindow::command, UM_NameFile);   //edw 12/5/97

            // Begin new logic 52012 MAMT - 28/07/2000
            remove(strBIR);
            int l_tempf = oldfileB.length();
            // verify if exists the file
            if (oldfileB.subString(l_tempf - 2,3) == "BYZ" )
               postEvent(IWindow::command, UM_NameFile1);
            else
               {
               postEvent(IWindow::command, UM_NameFile);
               }
            // End new logic 52012 MAMT - 28/07/2000
            }
         }
        else
         {
         /* D51693
            If running from eConfig in fastpath mode and no configuration
            files were returned from the retrieval, exit base retrieval and
            return control to eConfig
         */
         remove( strTempFile );
         bLock = false;
         if( pProfile->bFastPath && pProfile->bEconfig)
            postEvent(IWindow::systemCommand, ISystemMenu::idClose);  //58370
         else
            {                                                     // D72623 EDW 07/01/2002
            cleanTempFiles(strBII, strCFR, strMRT, strCFT);       // D72623 EDW 07/01/2002
            return;
            }                                                     // D72623 EDW 07/01/2002
         }
         cleanTempFiles(strBII, strCFR, strMRT, strCFT);       // D72623 EDW 07/01/2002

     }

   else  // _spawnl() failed
      if ( lRC == -1 )
         {
         IMessageBox::Style style = IMessageBox::retryCancelButton |
                                    IMessageBox::warningIcon |
                                    IMessageBox::moveable |
                                    IMessageBox::systemModal;
         msg.setTitle(IResourceId(IDS_ErrorRetrieve_Title));
         response = msg.show(IResourceId(IDS_ErrorRetrieve_Text),
                             style, IDH_ErrorRetrieve);
         if ( response == IMessageBox::retry )
            postEvent(IWindow::command, IDMI_ConfigRetrieve);
         else
            {
            remove( strTempFile );
            bLock = false;
            if( pProfile->bFastPath && pProfile->bEconfig)// Usability
               postEvent(IWindow::systemCommand, ISystemMenu::idClose);  //58370
            else
               return;
            }
         }
      else
         {
         remove( strTempFile );
         bLock = false;
         if( pProfile->bFastPath && pProfile->bEconfig)// Usability
            postEvent(IWindow::systemCommand, ISystemMenu::idClose);  //58370
         else
            return;
         }
   remove( strTempFile ); //edw05/19/99
   bLock = false;
   }
//=======================================================================
// FUNCTION NAME    validProdFamilyPath
//
// DESCRIPTION      Verifies if the product family directory exists
//
// RETURN VALUE     Returns true if path is valid.
//                  Returns false if path is invalid.
//
// HECC 04/28/2003  F79548
//=======================================================================
Boolean MainWindow :: validProdFamilyPath(IString strPath)
{
   DWORD rc = GetFileAttributes(strPath);

   if ( ( rc == 0xFFFFFFFF ) || ( ( rc & FILE_ATTRIBUTE_DIRECTORY ) != FILE_ATTRIBUTE_DIRECTORY ) )
      {
      return false;
      }
   return true;
}

//=======================================================================
// FUNCTION NAME    cleanTempFiles
//
// DESCRIPTION      Remove all intermediate files created when processing
//                  the .MRD and .CFR input files retrieved and loaded
//                  into BaseRetrieval in the openHost method.
//                  D72623 EDW 07/01/2002
//=======================================================================
void MainWindow :: cleanTempFiles(IString strBII, IString strCFR,
                                  IString strMRT, IString strCFT)
   {
   struct stat fstat;
   if (stat(strBII, &fstat) == 0)
      {
      remove(strCFR);
      rename(strBII, strCFR);
      }
   remove(strMRT);
   remove(strCFT);
   }

//=======================================================================
// FUNCTION NAME    openDetail
//
// DESCRIPTION      Open the product detail window.
//=======================================================================

void MainWindow :: openDetail(Product* pProduct, Boolean bLastObject)
   {
   if ( !pDetail )
      {
      pDetail = new DetailWindow(this);
      pDetail->moveTo(pProfile->position[WINDOWDETAIL]);
      pDetail->sizeTo(pProfile->size[WINDOWDETAIL]);
      }
   pDetail->add(pProduct, (DetailObject*)NULL, bLastObject); // EDS-chnaged 2/21
   pDetail->show();
   if ( pDetail->isMinimized() )
      pDetail->restore();
   pDetail->addToWindowList();
   pDetail->setFocus();
   }

//=======================================================================
// FUNCTION NAME    openDetail
//
// DESCRIPTION      Open the MES detail window.
//=======================================================================

void MainWindow :: openDetail(Mes* pMES, Boolean bLastObject)
   {
   if ( !pDetail )
      {
      pDetail = new DetailWindow(this);
      pDetail->moveTo(pProfile->position[WINDOWDETAIL]);
      pDetail->sizeTo(pProfile->size[WINDOWDETAIL]);
      }
   pDetail->add(pMES, (DetailObject*)NULL, bLastObject); // EDS-chg 2-22
   pDetail->show();
   if ( pDetail->isMinimized() )
      pDetail->restore();
   pDetail->addToWindowList();
   pDetail->setFocus();
   }

//=======================================================================
// FUNCTION NAME    reloadDetail
//
// DESCRIPTION      Refesh the product detail window.
//=======================================================================

void MainWindow :: reloadDetail(Product* pProduct)
   {
   pDetail->reload(pProduct);
   }

//=======================================================================
// FUNCTION NAME    openOptions
//
// DESCRIPTION      Open the Options dialog.
//=======================================================================

void MainWindow :: openOptions()
   {
#ifdef BR_OBSOLETE_MENU   /* F62721 EDW 01/21/2002 */
   if ( !pOptions )
      {
      pOptions = new OptionsDialog(this);
      pOptions->moveTo(position());
      }
   pOptions->show();
   if ( pOptions->isMinimized() )
      pOptions->restore();
   pOptions->addToWindowList();
   pOptions->setFocus();
#endif
   }

//=======================================================================
// FUNCTION NAME    openRetrievalOptions
//
// DESCRIPTION      Open the Options dialog.
//=======================================================================

void MainWindow :: openRetrievalOptions()
   {
#ifdef BR_OBSOLETE_MENU   /* F62721 EDW 01/21/2002 */
   if ( !pRetSrc )
      {
      pRetSrc = new RetrSource(this);
      pRetSrc->moveTo(position());
      }
   pRetSrc->show();
   if ( pRetSrc->isMinimized() )
      pRetSrc->restore();
   pRetSrc->addToWindowList();
   pRetSrc->setFocus();
#endif
   }

//=======================================================================
// FUNCTION NAME    openPrefsOptions
//
// DESCRIPTION      Open the Preference dialog by invoking the
//                  EditBaseRetrievalProfile API of the BF9PROFL.DLL
//                  F62721 EDW 01/21/2002
//=======================================================================

void MainWindow :: openPrefsOptions()
   {
   /* If a file is currently opened in main window,
    * do a little clean-up first
    */

   if ( !confirmFileSave() )
      return;

   if (pOrders)
      {
      pOrders->clearNotebook();
      pOrders->hide();
      pOrders->removeFromWindowList();
      }
   if (pSystems)
      {
      pSystems->clearNotebook();
      pSystems->hide();
      pSystems->removeFromWindowList();
      }
   if ( pDetail )
      pDetail->postEvent(IWindow::systemCommand, ISystemMenu::idClose);
   if (pFile)
      {
      pFile = (CfReportFileStats*)NULL;
      listFiles();
      }

   /* Invoke the profile API to display preference dialog */

   int iRc = EditBaseRetrievalProfile(this->handle());
   if ( iRc == 0 )    /* ok button clicked */
      {
      loadProfile();

      /* Set local copies of pref flags */

      bSort = pProfile->bSort;
      bSascending = pProfile->bSascending;
      bExpanded = pProfile->bExpanded;
      bTile = pProfile->bTile;

      /* Refresh the main window view with the new preferences */

      if ( pProfile->bRefreshList )
         {
         ptxPath->setText(STR_PathLabelText + strBrowsePath+" ");
         listFiles();
         }
      sortFiles();

      /* Load a new locale resource if language pref was changed */

      if ( pProfile->bRefreshLocale )
         {
         postEvent(IWindow::command, UM_SwitchLocale);
         }
      }
   }
//=======================================================================
// FUNCTION NAME    openProductInfo
//
// DESCRIPTION      Open the product information dialog.
//=======================================================================

void MainWindow :: openProductInfo()
   {
   IMessageBox::Style style = IMessageBox::okButton |
                              IMessageBox::informationIcon |
                              IMessageBox::moveable |
                              IMessageBox::systemModal ;   //ZOR 060900 def 51688
   IMessageBox msg(this);
   msg.setTitle(IResourceId(IDS_Product_Title));
   IString strText = LOADSTRING(IDS_Product_Text);
   strText.change(STR_Substitute0, STR_Version);
   msg.show(strText, style);
   }

//=======================================================================
// FUNCTION NAME    getProductModel
//
// DESCRIPTION      Determines the ISM Model that is running by querying
//                  the application's current working directory.
//=======================================================================
//edw06/18/1998
IString MainWindow :: getProductModel()
   {
   IString strModel;
   char strT1[100];
   IString strBlank = STR_Blank1;

   LPSTR strT2 = (LPSTR) strT1;
   GetCurrentDirectory( 100 , strT2 );
   strModel = strT2;

   ULONG ulOffset = strModel.lastIndexOf(STR_PathDelimiter);
   if ( ulOffset )
      {
      strModel = strModel.subString( ulOffset+1 );
      strModel = strModel.upperCase();
      //if ( strModel == STR_S390 || strModel == STR_AS400 || strModel == STR_RS6000 )
      if ( strModel.length() )     //edw02/16/99
         return( strModel );
      else
         return( strBlank );
      }
   else
      return( strBlank );

   }

//=======================================================================
// FUNCTION NAME    findResources
//
// DESCRIPTION      Confirm that a resource DLL and help file are
//                  available.
//=======================================================================

Boolean MainWindow :: findResources()
   {
   struct stat fstat;

   // Find the interface MRI resource DLL specified in the profile, and
   // verify that the matching locale for the current code page is
   // available.

   LONG lRC = 1;
   while ( lRC )
      {
      IString strMRI = STR_PathMRI;
      strMRI.insert(strExecPath);
      strMRI += pProfile->strMRI;
      strMRI += STR_ExtensionDLL;
      IString strLocale = pProfile->strLocale;
      lRC = stat(strMRI, &fstat);
      if ( !lRC )
         if ( setlocale(LC_ALL, strLocale) == NULL )
            setlocale(LC_ALL, STR_DefaultLocale);

      // If the DLL is missing, open a minimal dialog to select a locale.

      if ( lRC )
         {
         LocaleDialog* pDialog = new LocaleDialog(this);
         ULONG ulResult = pDialog->showModally();
         delete pDialog;
         if ( !ulResult )
            return false;
         }
      }

   return true;
   }

//=======================================================================
// FUNCTION NAME    loadResources
//
// DESCRIPTION      Construct a new set of resources, loaded from the
//                  current resource DLL.
//=======================================================================

Boolean MainWindow :: loadResources()
   {

   if ( setlocale(LC_ALL, pProfile->strLocale))
      setlocale(LC_ALL, STR_DefaultLocale);
   IString strMRI = pProfile->strMRI;

   try
      {

      // Attach the MRI DLL.

      strMRI.insert(STR_PathMRI);
      strMRI.insert(strExecPath);
      IApplication::current().setUserResourceLibrary(strMRI+STR_ExtensionDLL);

      // Attach the appropriate icon.

      setIcon(IResourceId(IDI_ApplyOrders));

      // Create the menu bar.  Disable some menu options until a CFREPORT
      // file is loaded.

      pMenu = new IMenuBar(IDW_AFOAPPLY, this);

      // Attach an accelerator table to the menu.

      pAccelerator = new IAccelerator(IResourceId(ACCELTABLE_AFOAPPLY));

      // Set the title for the current language.

      ITitle title(this, LOADSTRING(IDS_MainTitle));
      }
   catch(IException& exc)
      {
      IMessageBox::Style style = IMessageBox::cancelButton |
                                 IMessageBox::warningIcon |
                                 IMessageBox::moveable |
					IMessageBox::systemModal;
      IMessageBox msg(this);
      msg.setTitle(STR_ErrorMRITitle);
      IString sMsg = STR_ErrorMRIText;
      sMsg.change(STR_Substitute0, strMRI);
      msg.show(sMsg, style);
      return false;
      }

   // Append the help file extension and create a help window.

   pHelp = new IHelpWindow(HELPTABLE_AFOAPPLY, this);

   try
      {
      pHelp->addLibraries(IString("\"")+strMRI+STR_ExtensionHelp+IString("\""));
      pHelp->addLibraries("hmhelp.hlp"); // MSN added for testing 2/14/97
      strHLP = strMRI;                   //JT pass to bridge to RM/2 7/3/97
      }
   catch(IException& exc)
      {
      IMessageBox::Style style = IMessageBox::okButton |
                                 IMessageBox::warningIcon |
                                 IMessageBox::moveable |
					IMessageBox::systemModal;
      IMessageBox msg(this);
      msg.setTitle(IResourceId(IDS_NoHelp_Title));
      msg.show(IResourceId(IDS_NoHelp_Text), style);
      }
   pHelp->setTitle(IResourceId(IDS_HelpTitle));
   return true;
   }

//=======================================================================
// FUNCTION NAME    listFiles
//
// DESCRIPTION      List the files in the current browse directory.
//=======================================================================

void MainWindow :: listFiles()
   {
   bLock = true;
   setPointer(true);
   pcnrFiles->setRefreshOff();
   if ( pcnrFiles->objectCount() )
      pcnrFiles->deleteAllObjects();
   USHORT usError = pBackEnd->ListFiles();
   setPointer(false);
   bLock = false;
   if ( usError )
      {
      //edw05/19/99 SR6952 - Informational box instead of warning box if no
      //                     previously retrieved files are found
      if ( usError == IDS_ErrorGettingFileList_Text )
         {
         if( !pProfile->bFastPath )     //usability
            {
            IMessageBox::Style style = IMessageBox::okButton |
                                    IMessageBox::informationIcon |
                                    IMessageBox::moveable |
						IMessageBox::systemModal;
            IMessageBox msg(this);
            msg.setTitle(IResourceId(IDS_NoFilesFound_Title));
            msg.show(IResourceId(usError), style, IDH_ErrorBackEnd);
            }
         }
      else
         {
         IMessageBox::Style style = IMessageBox::cancelButton |
                                    IMessageBox::warningIcon |
                                    IMessageBox::moveable |
						IMessageBox::systemModal;
         IMessageBox msg(this);
         msg.setTitle(IResourceId(IDS_ErrorBackEnd_Title));
         msg.show(IResourceId(usError), style, IDH_ErrorBackEnd);
         }
      }
   if ( pcnrFiles->objectCount() )
      {
      sortFiles();
      pcnrFiles->setSelected(pcnrFiles->objectAt(0));
      pcnrFiles->setCursor(pcnrFiles->objectAt(0));
      }
   pcnrFiles->setRefreshOn();
   pcnrFiles->refresh(true);
   // pHelp->setAssociatedWindow(this); deleted MSN 1/21/97

    if( pProfile->bFastPath && pProfile->bEconfig )     //usability   erg 17/03/2000
      postEvent(IWindow::command, IDMI_ConfigRetrieve);

 }

//=======================================================================
// FUNCTION NAME    sortFiles
//
// DESCRIPTION      Sort the file list by name or by date.
//=======================================================================

class NameSort : public IContainerControl::CompareFn
   {
   public:
   Boolean bAscending;
   int isEqual(IContainerObject* pObj1, IContainerObject* pObj2,
               IContainerControl* pCnr) const
      {
      CfReportFileStats* pFile1 = (CfReportFileStats*)pObj1;
      CfReportFileStats* pFile2 = (CfReportFileStats*)pObj2;
      if ( IString(pFile1->name()).upperCase() < IString(pFile2->name()).upperCase() )
         {
         if ( bAscending )
            return -1;
         else
            return 1;
         }
      else
         {
         if ( bAscending )
            return 1;
         else
            return -1;
         }
      }
   };
class DateSort : public IContainerControl::CompareFn
   {
   public:
   Boolean bAscending;
   int isEqual(IContainerObject* pObj1, IContainerObject* pObj2,
               IContainerControl* pCnr) const
      {
      CfReportFileStats* pFile1 = (CfReportFileStats*)pObj1;
      CfReportFileStats* pFile2 = (CfReportFileStats*)pObj2;
      if ( !pFile1->usable() )
         return 1;
      if ( !pFile2->usable() )
         return -1;
      if ( pFile1->date() < pFile2->date() )
         {
         if ( bAscending )
            return -1;
         else
            return 1;
         }
      else
         {
         if ( bAscending )
            return 1;
         else
            return -1;
         }
      }
   };

void MainWindow :: sortFiles()
   {
   NameSort sortName;
   DateSort sortDate;
   bAscending = ( sSort > 0 );
   USHORT usSort = abs(sSort);

//   pMenu->uncheckItem(IDMI_ViewSortName);   IC01552
//   pMenu->uncheckItem(IDMI_ViewSortDate);

   pcnrFiles->setRefreshOff();
   switch ( usSort )
      {
        case 1:
//      case enumSortName:                    //IC01552
         sortName.bAscending = bAscending;
         pcnrFiles->sort(sortName);
//         pMenu->checkItem(IDMI_ViewSortName);
         break;
        case 2:
//      case enumSortDate:                 //IC01552
         sortDate.bAscending = bAscending;
         pcnrFiles->sort(sortDate);
//         pMenu->checkItem(IDMI_ViewSortDate);
         break;
      }
   pcnrFiles->setRefreshOn();
   pcnrFiles->refresh(true);

   pProfile->sSortFiles = sSort;
   }

//=======================================================================
// FUNCTION NAME    saveFile
//
// DESCRIPTION      Write a new copy of the file.
//=======================================================================

void MainWindow :: saveFile()
   {
   /*--JT
   IHelpWindow localHelp(HELPTABLE_AFOAPPLY, this);
   try
      {
      IString strMRI = pProfile->strMRI;
      strMRI.insert(STR_PathMRI);
      strMRI.insert(strExecPath);
      localHelp.addLibraries(strMRI+STR_ExtensionHelp);
      }
      catch(IException& exc)
      {
      }
   localHelp.setTitle(IResourceId(IDS_HelpTitle));
   */
   bLock = true;
   setPointer(true);
   IString strOldName = pFile->name();
   USHORT usError = pBackEnd->SaveCFREPORT();
   setPointer(false);
   bLock = false;
   if ( usError )
      {
      IMessageBox::Style style = IMessageBox::cancelButton |
                                 IMessageBox::warningIcon |
                                 IMessageBox::moveable |
                                 IMessageBox::systemModal;
      IMessageBox msg(this);
      msg.setTitle(IResourceId(IDS_ErrorBackEnd_Title));
      msg.show(IResourceId(usError), style, IDH_ErrorBackEnd);
      }
   else
      {
      if ( strSaveName != strOldName )
         {
         sortFiles();
         if ( pOrders )
            {
            ITitle title(pOrders, pFile->name(), LOADSTRING(IDS_OrdersTitle));
            }
         if ( pSystems )
            {
            ITitle title(pSystems, pFile->name(), LOADSTRING(IDS_SystemsTitle));
            }
         if ( pDetail )
            {
            ITitle title(pDetail, pFile->name(), LOADSTRING(IDS_DetailTitle));
            }
         }
      }
   pFile->setChanges(0);
   pcnrFiles->refresh();
   //pHelp->setAssociatedWindow(this); JT-DEL
   }

//=======================================================================
// FUNCTION NAME    generateFile
//
// DESCRIPTION      Write a new output file.
//=======================================================================

void MainWindow :: generateFile()
   {
   /*--JT
   IHelpWindow localHelp(HELPTABLE_AFOAPPLY, this);
   try
      {
      IString strMRI = pProfile->strMRI;
      strMRI.insert(STR_PathMRI);
      strMRI.insert(strExecPath);
      localHelp.addLibraries(strMRI+STR_ExtensionHelp);
      }
      catch(IException& exc)
      {
      }
   localHelp.setTitle(IResourceId(IDS_HelpTitle));
   */
   bLock = true;
   setPointer(true);
   USHORT usError = pBackEnd->GenerateCFREPORT();
   setPointer(false);
   bLock = false;
   if ( usError )
      {
      IMessageBox::Style style = IMessageBox::cancelButton |
                                 IMessageBox::warningIcon |
                                 IMessageBox::moveable |
                                 IMessageBox::systemModal;
      IMessageBox msg(this);
      msg.setTitle(IResourceId(IDS_ErrorBackEnd_Title));
      msg.show(IResourceId(usError), style, IDH_ErrorBackEnd);
      }
   //pHelp->setAssociatedWindow(this); JT-del
   if ( bConfig && pProfile->bFastPath)
      postEvent(IWindow::command, UM_ReturnRetrieval);
   }

//=======================================================================
// FUNCTION NAME    renameFile
//
// DESCRIPTION      Give a BIR file a new name.
//=======================================================================

void MainWindow :: renameFile()
   {
   struct stat fstat;
   Boolean bRenamed = false;
   remove(strBrowsePath + STR_PathDelimiter + strSaveName + STR_ExtensionBIR);

   // 49608 - MAMT
   if ( !bOnlyRename ){
   	//Remove alredy proviously created CFReport  47705 - flw
      remove(strBrowsePath + STR_PathDelimiter + strGenerateName + STR_ExtensionRET);
   } else {
      //Remove the new file to create 49608 - MAMT
      remove(strBrowsePath + STR_PathDelimiter + strSaveName + STR_ExtensionRET);
   }
   // End 49608 - MAMT

   IContainerControl::ObjectCursor crs(*pcnrFiles);
   for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
      {
      CfReportFileStats* pObject = (CfReportFileStats*)crs.current();
      if ( IString(pObject->name()).upperCase() == IString(strSaveName).upperCase() )
         {
         pcnrFiles->removeObject(pObject);
         delete pObject;
         break;
         }
      }
   if ( pSelectedFile == pFile )
      {
     // Rename the BIR file and RET file when its exist
     //ZOR IC66972  062600
      bRenamed = !pBackEnd->RenameFile();
      IString strOld = strBrowsePath + STR_PathDelimiter +
                       pSelectedFile->name() + STR_ExtensionRET;
      IString strNew = strBrowsePath + STR_PathDelimiter + strSaveName + STR_ExtensionRET;
      LONG lRC = 0;
      lRC = !stat(strOld, &fstat);
        if ( lRC )
          rename(strOld, strNew);
        else
           {
                 IMessageBox msg(this);
                 IMessageBox::Style style = IMessageBox::cancelButton |
                                            IMessageBox::warningIcon |
                                            IMessageBox::moveable |
                                            IMessageBox::systemModal;
                 msg.setTitle(IResourceId(IDS_ErrorRename_Title));
                IString strText = LOADSTRING(IDS_ErrorRenameRET_Text);
                strText.change(STR_Substitute0, IString(pSelectedFile->name()));
                msg.show(strText, style, IDH_ErrorRename);
            }
      }
     // ZOR IC66972 062600
   else
      {
      IString strOld = strBrowsePath + STR_PathDelimiter +
                       pSelectedFile->name() + STR_ExtensionBIR;
      // added path and delimiter to strNew 2/10/97 MSN
      IString strNew = strBrowsePath + STR_PathDelimiter + strSaveName + STR_ExtensionBIR;
      bRenamed = !rename(strOld, strNew);
      strOld = strBrowsePath + STR_PathDelimiter +
                       pSelectedFile->name() + STR_ExtensionRET;
      strNew = strBrowsePath + STR_PathDelimiter + strSaveName + STR_ExtensionRET;
      //Verify existance of OLD .RET file - flw
      LONG lRC = 0;
      // The previous comment was bad because returns 0 when the file exists 49608 - MAMT
     lRC = !stat(strOld, &fstat);
        if ( lRC )
          rename(strOld, strNew);
        else
           {
                 IMessageBox msg(this);
                 IMessageBox::Style style = IMessageBox::cancelButton |
                                            IMessageBox::warningIcon |
                                            IMessageBox::moveable |
                                            IMessageBox::systemModal;
                 msg.setTitle(IResourceId(IDS_ErrorRename_Title));
                IString strText = LOADSTRING(IDS_ErrorRenameRET_Text);
                strText.change(STR_Substitute0, IString(pSelectedFile->name()));
                msg.show(strText, style, IDH_ErrorRename);
            }
      }
     // ZOR IC66972 062600
   if ( bRenamed )
      {
      pSelectedFile->setName(strSaveName);
      pSelectedFile->refresh();
      strGenerateName = strSaveName;
      // bOnlyRename is used to rename only 49608 - MAMT
      if ( !bOnlyRename ){
         postEvent(IWindow::command, UM_GenerateFile);
         if ( pSelectedFile == pFile ) {
            if ( pOrders )
               {
               ITitle title(pOrders, pFile->name(), LOADSTRING(IDS_OrdersTitle));
               }
            if ( pSystems )
               {
               ITitle title(pSystems, pFile->name(), LOADSTRING(IDS_SystemsTitle));
               }
            if ( pDetail )
               {
               ITitle title(pDetail, pFile->name(), LOADSTRING(IDS_DetailTitle));
               }
         }
      }
      bOnlyRename = false;      // End 49608 - MAMT
      if ( abs(sSort) == enumSortName )
         sortFiles();
      pcnrFiles->refresh();//jt-add-4/14/97
      }
   else
      {
      IMessageBox msg(this);
      IMessageBox::Style style = IMessageBox::cancelButton |
                                 IMessageBox::warningIcon |
                                 IMessageBox::moveable |
					IMessageBox::systemModal;
      msg.setTitle(IResourceId(IDS_ErrorRename_Title));
      IString strText = LOADSTRING(IDS_ErrorRename_Text);
      strText.change(STR_Substitute0, pSelectedFile->name());
      msg.show(strText, style, IDH_ErrorRename);
      }
   }

//=======================================================================
// FUNCTION NAME    deleteFile
//
// DESCRIPTION      Delete a BIR file.
//=======================================================================

void MainWindow :: deleteFile()
   {
   struct stat fstat;

   IMessageBox msg(this);
   IMessageBox::Style style = IMessageBox::yesNoButton |
                              IMessageBox::warningIcon |
                              IMessageBox::moveable |
					IMessageBox::systemModal;
   msg.setTitle(IResourceId(IDS_DeleteFile_Title));
   IString strText = LOADSTRING(IDS_DeleteFile_Text);
   strText.change(STR_Substitute0, strSaveName);
   IMessageBox::Response response = msg.show(strText, style, IDH_DeleteFile);
   if ( response != IMessageBox::yes )
      return;
   bLock = true;
   Boolean bDeleted;
   if ( pSelectedFile == pFile )
      {
      if ( pOrders )
         {
         //pOrders->sendEvent(IWindow::systemCommand, ISystemMenu::idClose);
         pOrders->clearNotebook();
         pOrders->hide();                      //edw01/23/98
         pOrders->removeFromWindowList();      //edw01/23/98
         }
      if ( pSystems )
         {
         //pSystems->sendEvent(IWindow::systemCommand, ISystemMenu::idClose);
         pSystems->clearNotebook();
         pSystems->hide();                      //edw01/23/98
         pSystems->removeFromWindowList();      //edw01/23/98
         }
      if ( pDetail )
         pDetail->postEvent(IWindow::systemCommand, ISystemMenu::idClose);
      bDeleted = !pBackEnd->CloseFile();
      if ( bDeleted )
         pFile = (CfReportFileStats*)NULL;
      }
   if ( bDeleted )
      {
        bDeleted = !remove(strBrowsePath + STR_PathDelimiter +
                         strSaveName + STR_ExtensionBIR);
      //Delete previously created CFReport   47705 - flw
      // old code LONG lRC = 0;
      LONG lRC = -1;       // ZOR 042803  52646
      IString delfile = strBrowsePath + STR_PathDelimiter + strSaveName + STR_ExtensionRET;
      lRC = stat(delfile, &fstat);
        // old code if ( lRC )
        if ( lRC == 0 )      // ZOR  042803  52646
        { remove(delfile);
        // 139136 Begin:
        IString delfileMRD = strBrowsePath + STR_PathDelimiter + strSaveName + STR_ExtensionMRD;
        remove(delfileMRD);
        // 139136 END:
        }
       }

   if ( bDeleted )
      {
      delete pSelectedFile;
      pSelectedFile = (CfReportFileStats*)NULL;
      }
   else
      {
      IMessageBox msg(this);
      IMessageBox::Style style = IMessageBox::cancelButton |
                                 IMessageBox::warningIcon |
                                 IMessageBox::moveable |
						IMessageBox::systemModal;
      msg.setTitle(IResourceId(IDS_ErrorDelete_Title));
      IString strText = LOADSTRING(IDS_ErrorDelete_Text);
      strText.change(STR_Substitute0, strSaveName);
      msg.show(strText, style, IDH_ErrorDelete);
      }
   bLock = false;
   }

//=======================================================================
// FUNCTION NAME    deleteFile1                   52012 MAMT - 28/07/2000	
//
// DESCRIPTION      Delete a file that will be opened again.
//=======================================================================

void MainWindow :: deleteFile1()
   {
   struct stat fstat;

   bLock = true;
   Boolean bDeleted;
   if ( pFile )
   pFile->setIcon(NULL);

   if ( pSelectedFile == pFile )
      {
      if ( pOrders )
         {
         //pOrders->sendEvent(IWindow::systemCommand, ISystemMenu::idClose);
         pOrders->clearNotebook();
         pOrders->hide();                      //edw01/23/98
         pOrders->removeFromWindowList();      //edw01/23/98
         }
      if ( pSystems )
         {
         //pSystems->sendEvent(IWindow::systemCommand, ISystemMenu::idClose);
         pSystems->clearNotebook();
         pSystems->hide();                      //edw01/23/98
         pSystems->removeFromWindowList();      //edw01/23/98
         }
      if ( pDetail )
         pDetail->postEvent(IWindow::systemCommand, ISystemMenu::idClose);
      bDeleted = !(pBackEnd->CloseFile());
      pFile = (CfReportFileStats*)NULL;
      }
      // eliminate the old file to create later the new file with the same name
      IContainerControl::ObjectCursor crs(*pcnrFiles);
      for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
          {
          CfReportFileStats* pObject = (CfReportFileStats*)crs.current();
          if ( IString(pObject->name()).upperCase() ==  strSaveName )
             {
             //pFile = pObject;
             //pFile.setSelected();
             pcnrFiles->removeObject(pObject);
             IThread::current().sleep(500);//jt
             delete pObject;
             break;
             }
          }
   pcnrFiles->refresh();

   remove(strBrowsePath + STR_PathDelimiter + strSaveName + STR_ExtensionBIR);

   bLock = false;
   }


//=======================================================================
// FUNCTION NAME    loadProfile
//
// DESCRIPTION      Read profile settings from the INI file.
//=======================================================================

void MainWindow :: loadProfile()
   {
   IString strProfile;
   pProfile->load(strProfile);
   strBrowsePath = pProfile->strBrowsePath;
   strGeneratePath = pProfile->strGeneratePath;
   sSort = pProfile->sSortFiles;
   if ( findResources() )
      bProfile = true;
   }

//=======================================================================
// FUNCTION NAME    selectDirectory
//
// DESCRIPTION      Open a hobbled file dialog to select an input path.
//=======================================================================

void MainWindow :: selectDirectory(IString& str)
   {
   bLock = true;

   IFileDialog::Settings settings;

   switch ( usPath )
      {
      case enumFileConfig:
         {
         IString strFilename = str + STR_PathDelimiter;
         strFilename += STR_WildCard;
         strFilename += STR_ExtensionCFR ;
         strFilename += STR_PathDelimeter;
         strFilename += STR_WildCard ;
         strFilename += STR_ExtensionADM ;  //42619 flw
         settings.setOpenDialog();
         settings.setTitle( IResourceId(IDS_SelectFileTitle) );
         settings.setFileName( strFilename );
         break;
         }
      default:
         {
         settings.setDialogTemplate( IDD_DirectoryDialog ); //EDS-add 1/20/97
         settings.setSaveAsDialog();
         settings.setTitle( IResourceId(IDS_SelectFileTitle) );
         settings.setFileName(str + STR_DummyFile);
         break;
         }
      }

//   IFileDialog::Style fileStyle = IFileDialog::modeless |
//                                  IFileDialog::helpButton;
   IFileDialog::Style fileStyle = IFileDialog::helpButton |  //VBB D73492 08/13/02
                                  IFileDialog::explorer;     //ICA68115  HECC: 08/27/2002

   IFileDialog* pFileDlg = new IFileDialog(this, this,
                                           (IFileDialogHandler*)this,
                                           fileStyle, settings);
   //In changing to modeless dialog, modelessResults must be processed to
   //re-enable the menu options.
   modelessResults(pFileDlg);                                //VBB D73492 08/13/02

   /*  EDS-del 1/20/97
   IStaticText txFile(DID_FILENAME_TXT, pFileDlg);
   IEntryField efFile(DID_FILENAME_ED, pFileDlg);
   IStaticText txFilter(DID_FILTER_TXT, pFileDlg);
   IComboBox cbFilter(DID_FILTER_CB, pFileDlg);
   txFile.hide();
   efFile.hide();
   txFilter.hide();
   cbFilter.hide();

   IComboBox cbDrive(DID_DRIVE_CB, pFileDlg);
   cbDrive.setFocus();
   ULONG ulHeight = pFileDlg->size().height() -
                    txFile.size().height() -
                    efFile.size().height();
   pFileDlg->sizeTo(ISize(pFileDlg->size().width(), ulHeight));
*/
   pHelp->setAssociatedWindow(pFileDlg);
   delete pFileDlg;
   }

//=======================================================================
// FUNCTION NAME    confirmFileSave
//
// DESCRIPTION      Confirm that the unsaved file is to be discarded
//                  when opening a new file or closing the main window.
//
// Note: This function is called to save only the .BIR file including
//       the modifications made to the configuration. This function must
//       also save the .RET file to deliver the configuration with all
//       its changes to eConfig.

//=======================================================================

Boolean MainWindow :: confirmFileSave()
   {
   if ( !pFile )
      return true;
   if ( !pFile->changes() )
      {
      pBackEnd->CloseFile();
      return true;
      }
   IMessageBox msg(this);
   IMessageBox::Style style = IMessageBox::yesNoCancelButton |
                              IMessageBox::defButton3 |
                              IMessageBox::warningIcon |
                              IMessageBox::moveable |
                              IMessageBox::systemModal;
   msg.setTitle(IResourceId(IDS_SaveFile_Title));
   IMessageBox::Response response = msg.show(IResourceId(IDS_SaveFile_Text),
                                             style, IDH_SaveFile);
   if ( response == IMessageBox::yes )
      {
      saveFile();

      //* ICD22680351:  Recreates the lastest changes to the Assigned/Unassigned
      //* products in the Systems Window to the .RET file
      generateFile();
      //* ICD22680351:  HECC - 02/25/2005
      }
   if ( response != IMessageBox::cancel )
      pBackEnd->CloseFile();
   return ( response != IMessageBox::cancel );
   }

//=======================================================================
// FUNCTION NAME    confirmFileRetrieve
//
// DESCRIPTION      Confirm that the open LASTRET file is to be discarded
//                  when retrieving a new one.
//=======================================================================

Boolean MainWindow :: confirmFileRetrieve()
   {
   if ( !pFile )
      return true;
   if ( IString(pFile->name()).upperCase() != STR_RetrievalFile )
      return true;

   if ( !pProfile->bFastPath)
      {
       IMessageBox msg(this);
       IMessageBox::Style style = IMessageBox::okCancelButton |
                              IMessageBox::defButton2 |
                              IMessageBox::warningIcon |
                              IMessageBox::moveable |
					IMessageBox::systemModal;
       msg.setTitle(IResourceId(IDS_RetrieveFile_Title));
       IMessageBox::Response response = msg.show(IResourceId(IDS_RetrieveFile_Text),
                                             style, IDH_RetrieveFile);
       if ( response == IMessageBox::ok )
         {
         //pOrders->sendEvent(IWindow::systemCommand, ISystemMenu::idClose);
         pOrders->clearNotebook();
         pOrders->hide();                      //edw01/23/98
         pOrders->removeFromWindowList();      //edw01/23/98
         //pSystems->sendEvent(IWindow::systemCommand, ISystemMenu::idClose);
         pSystems->clearNotebook();
         pSystems->hide();                      //edw01/23/98
         pSystems->removeFromWindowList();      //edw01/23/98
         if ( pDetail )
           pDetail->postEvent(IWindow::systemCommand, ISystemMenu::idClose);
         pBackEnd->CloseFile();
         remove(strBrowsePath + STR_PathDelimiter +
                    STR_RetrievalFile + STR_ExtensionBIR);
         delete pFile;
         pFile = (CfReportFileStats*)NULL;
         strSaveName = strGenerateName = "";
         }
       return ( response == IMessageBox::ok );
     }
     else if (pProfile->bFastPath)
        {
         pOrders->clearNotebook();
         pOrders->hide();                      //edw01/23/98
         pOrders->removeFromWindowList();      //edw01/23/98
         //pSystems->sendEvent(IWindow::systemCommand, ISystemMenu::idClose);
         pSystems->clearNotebook();
         pSystems->hide();                      //edw01/23/98
         pSystems->removeFromWindowList();      //edw01/23/98
         if ( pDetail )
           pDetail->postEvent(IWindow::systemCommand, ISystemMenu::idClose);
         pBackEnd->CloseFile();
         remove(strBrowsePath + STR_PathDelimiter +
                    STR_RetrievalFile + STR_ExtensionBIR);
         delete pFile;
         pFile = (CfReportFileStats*)NULL;
         strSaveName = strGenerateName = "";
        }
      return true;
  }

//=======================================================================
// FUNCTION NAME    createColumn
//
// DESCRIPTION      Create a new container column.
//=======================================================================

void MainWindow :: createColumn(IContainerColumn** ppColumn, ULONG ulOffset,
                                IContainerColumn::HorizontalAlignment align)
   {
   IContainerColumn* pColumn = new IContainerColumn(ulOffset);
   pColumn->showSeparators();
   pColumn->justifyHeading(IContainerColumn::bottom);
   pColumn->justifyData(IContainerColumn::centeredVertically, align);
   *ppColumn = pColumn;
   }

//=======================================================================
// FUNCTION NAME    setHeadingText
//
// DESCRIPTION      Build a two-line column heading.
//=======================================================================

void MainWindow :: setHeadingText(IContainerColumn* pColumn, ULONG ulString)
   {
   IString line1 = LOADSTRING(ulString);
   IString line2 = LOADSTRING(ulString+1);
   if ( line1 == STR_Blank1 )
      pColumn->setHeadingText(line2);
   else
      pColumn->setHeadingText(line1 + "\n" + line2);
   }

//=======================================================================
// FUNCTION NAME    helpWindow
//
// DESCRIPTION      Return a pointer to the help window.
//=======================================================================

IHelpWindow* MainWindow :: helpWindow()
   {
   return pHelp;
   }

//=======================================================================
// FUNCTION NAME    switchLocale
//
// DESCRIPTION      Switch to new locale DLLs.
//=======================================================================

void MainWindow :: switchLocale()
   {
   delete pHelp;
   delete pMenu;
   delete pAccelerator;
   IApplication::current().setUserResourceLibrary(0);
   if ( !loadResources() )
      {
      close();
      return;
      }
   }

//=======================================================================
// FUNCTION NAME    returnRetrievalFile
//
// DESCRIPTION      If called by the configurator, return as soon as a
//                  .RET file is generated.
//=======================================================================

void MainWindow :: returnRetrievalFile()
   {
//   IMessageBox::Style style = IMessageBox::okButton |
//                              IMessageBox::informationIcon |
//                              IMessageBox::moveable |
//                              IMessageBox::systemModal ;          //ZOR 060900 d51688
//   IMessageBox msg(this);
//   msg.setTitle(IResourceId(IDS_ReturnRetrieval_Title));
//   msg.show(IResourceId(IDS_ReturnRetrieval_Text), style, IDH_ReturnRetrieval);
   //ZOR 53363 - New Code
     ReturnRetrieval *pDialog;
     pDialog = new ReturnRetrieval(this);
     IPoint mainPos = this->position();
     mainPos.setX( mainPos.x() + 200 );      //ZOR F47969
     mainPos.setY( mainPos.y() + 150 );      //ZOR F47969
     pDialog->moveTo( mainPos );
     pDialog->addToWindowList();
     pDialog->setFocus();
     pDialog->showModally();
     /*  F62721  EDW 01/21/2002 begin
     if ( pDialog->bContinue ) {
        IProfile::userProfile().addOrReplaceElementWithKey
        (STR_ConfigKey,
        strGeneratePath + STR_PathDelimiter + strGenerateName + STR_ExtensionRET,
        STR_ConfigApp);
        */
     if ( pDialog->bContinue ) {
        HKEY sboKey = 0;
        IString sboKeyName = STR_RegKey_eConfig + IString(STR_ConfigApp) +
                             IString(STR_PathDelimiter) + IString(STR_ConfigKey);
        RegCreateKeyEx(HKEY_CURRENT_USER, sboKeyName, 0, 0, 0,
                       KEY_ALL_ACCESS | KEY_WRITE, 0, &sboKey, 0);
        IString strData = strGeneratePath + STR_PathDelimiter +
                          strGenerateName + STR_ExtensionRET;
        RegSetValueEx(sboKey, "Data", 0, REG_SZ, (CONST BYTE*)strData,
                      strData.length() + 1);
        if (sboKey)
           RegCloseKey(sboKey);
        /*  F62721  EDW 01/21/2002 end */

        //edw06/11/99 replace with block below
        //edw11/08/99 Portable configs still read this version of the "Retfile" registry key
        //IProfile profile(STR_Profile_INI);

        /*  F62721  EDW 01/21/2002 begin
        IString sProfileName = IProfile::userProfile().name() + STR_PathDelimiter + STR_Profile_INI;   //48163 - flw
        IProfile profile(sProfileName);
        profile.addOrReplaceElementWithKey(STR_ConfigKey,
           strGeneratePath + STR_PathDelimiter + strGenerateName + STR_ExtensionRET,
           STR_Profile_App);
        */
        HKEY brKey = 0;
        sboKey = 0;
        RegCreateKeyEx(HKEY_CURRENT_USER, REGPATH, 0, 0, 0,
                       KEY_ALL_ACCESS | KEY_WRITE, 0, &brKey, 0);
        RegCreateKeyEx(brKey, STR_ConfigKey, 0, 0, 0,
               KEY_ALL_ACCESS | KEY_WRITE, 0, &sboKey, 0);
        RegSetValueEx(sboKey, "Data", 0, REG_SZ, (CONST BYTE*)strData,
                      strData.length() + 1);
        if (brKey)
           RegCloseKey(brKey);
        if (sboKey)
           RegCloseKey(sboKey);
        /*  F62721  EDW 01/21/2002 end */

        //edw06/11/99 made "Retfile" a "StringValue" in the "BaseRet" key instead of
        //edw06/11/99 a subkey of the "BaseRet" key REQ by eConfig bridge code team.
        CHAR Data[256];
        HKEY key;
        strcpy( Data, strGeneratePath + STR_PathDelimiter + strGenerateName + STR_ExtensionRET );
        //if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                       //RegKey_BaseRet, 0, KEY_WRITE, &key) == ERROR_SUCCESS )      //48163 - flw
        if ( RegOpenKeyEx(HKEY_CURRENT_USER,
                        RegKey_BaseRet, 0, KEY_WRITE, &key) == ERROR_SUCCESS )
           {
           RegSetValueEx(key, STR_ConfigKey, 0, REG_SZ, (CONST BYTE*)Data, 256);
           RegCloseKey (key);
           }

        ulReturnCode = true;
        postEvent(IWindow::systemCommand, ISystemMenu::idClose);
        }
   delete pDialog; //53363 - ZOR finish new code
  }
//=======================================================================
// FUNCTION NAME    reformat
//
// DESCRIPTION      Reformat dates for new locale.
//=======================================================================

void MainWindow :: reformat()
   {
   if ( pOrders )
      pOrders->postEvent(IWindow::command, UM_SwitchLocale);
   if ( pSystems )
      pSystems->postEvent(IWindow::command, UM_SwitchLocale);
   if ( pDetail )
      pDetail->postEvent(IWindow::command, UM_SwitchLocale);
   if ( pFileName )
      {
      pFileName->close();
      pFileName = (FileNameDialog*)NULL;
      }
   if ( pOptions )
      {
      pOptions->close();
      pOptions = (OptionsDialog*)NULL;
      }
   pcnrFiles->setRefreshOff();
   setHeadingText(pcolName, IDS_Name1);
   setHeadingText(pcolDate, IDS_DateRetrieval1);
   setHeadingText(pcolSystems, IDS_Systems1);
   setHeadingText(pcolProducts, IDS_Products1);
   setHeadingText(pcolOrders, IDS_Orders1);
   IContainerControl::ObjectCursor crs(*pcnrFiles);
   for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
      {
      CfReportFileStats* pObject = (CfReportFileStats*)crs.current();
      pObject->format();
      }
/*   switch ( abs(sSort) )
      {
      case enumSortName:
         pMenu->checkItem(IDMI_ViewSortName);
         break;
      case enumSortDate:
         pMenu->checkItem(IDMI_ViewSortDate);
         break;
      }*/

#ifdef BR_OBSOLETE_MENU   /* F62721 EDW 01/21/2002 */
//IC01552
    if( pProfile->bSort )
       {
         pMenu->checkItem(IDMI_ViewSortName);
         pMenu->uncheckItem(IDMI_ViewSortDate);
         bSort = pProfile->bSort;
         if (pProfile->bSascending)
            {
             pMenu->checkItem(IDMI_ViewSortNameA);
             pMenu->uncheckItem(IDMI_ViewSortNameD);
             pMenu->uncheckItem(IDMI_ViewSortDateA);
             pMenu->uncheckItem(IDMI_ViewSortDateD);
             pProfile->bSascending = true;
            }
         else
            {
             pMenu->checkItem(IDMI_ViewSortNameD);
             pMenu->uncheckItem(IDMI_ViewSortNameA);
             pMenu->uncheckItem(IDMI_ViewSortDateA);
             pMenu->uncheckItem(IDMI_ViewSortDateD);
             pProfile->bSascending = false;
            }
       }
    else
       {
        pMenu->uncheckItem(IDMI_ViewSortName);
        pMenu->checkItem(IDMI_ViewSortDate);
        bSort = pProfile->bSort;
        if (pProfile->bSascending)
            {
             pMenu->checkItem(IDMI_ViewSortDateA);
             pMenu->uncheckItem(IDMI_ViewSortNameD);
             pMenu->uncheckItem(IDMI_ViewSortNameA);
             pMenu->uncheckItem(IDMI_ViewSortDateD);
             pProfile->bSascending = true;
            }
         else
            {
             pMenu->checkItem(IDMI_ViewSortNameD);
             pMenu->uncheckItem(IDMI_ViewSortNameA);
             pMenu->uncheckItem(IDMI_ViewSortDateA);
             pMenu->uncheckItem(IDMI_ViewSortDateD);
             pProfile->bSascending = false;
            }
      }
#endif

   pcnrFiles->setRefreshOn();
   pcnrFiles->refresh();
   }

//=======================================================================
// FUNCTION NAME    setPointer
//
// DESCRIPTION      Display the wait pointer during time-consuming tasks.
//=======================================================================

void MainWindow :: setPointer(Boolean bBusy)
   {
   if ( bBusy )
      {
      IThread thread;
      thread.setStackSize(THREAD_STACK_SIZE);
      thread.start(new IThreadMemberFn<MainWindow>(*this, showProgress));
      setMousePointer(ISystemPointerHandle(ISystemPointerHandle::wait));
      if ( pOrders )
         if ( !( pOrders->isMinimized() ) )        //edw 12/5/97
            pOrders->setMousePointer(ISystemPointerHandle(ISystemPointerHandle::wait));
      if ( pSystems )
         if ( !( pSystems->isMinimized() ) )       //edw 12/5/97
            pSystems->setMousePointer(ISystemPointerHandle(ISystemPointerHandle::wait));
      if ( pDetail )
         if ( !( pDetail->isMinimized() ) )        //edw 12/5/97
            pDetail->setMousePointer(ISystemPointerHandle(ISystemPointerHandle::wait));
      }
   else
      {
      setMousePointer(NULL);
      if ( pOrders )
         pOrders->setMousePointer(NULL);
      if ( pSystems )
         pSystems->setMousePointer(NULL);
      if ( pDetail )
         pDetail->setMousePointer(NULL);
      }
   }

//=======================================================================
// FUNCTION NAME    startUp1
//
// DESCRIPTION      Start initial processing in separate thread to free
//                  input queue.
//=======================================================================

void MainWindow :: startUp1()
   {

   // Load the default profile settings.

   loadProfile();

   //* D73383 Begins new logic:  Update the pref flags after the profile structure
   //* has been loaded.
   bSort = pProfile->bSort;
   bSascending = pProfile->bSascending;
   bExpanded = pProfile->bExpanded;
   bTile = pProfile->bTile;
   //* D73383 End new logic:  HECC - 06/10/2003

   if ( bProfile )
      {
      // edw-Display wait cursor while resources are loading
      moveTo(pProfile->position[WINDOWMAIN]);
      sizeTo(pProfile->size[WINDOWMAIN]);
      if (!pProfile->bFastPath)
       {
        show();
        setFocus();
       }
      show();  //51688 - ZOR
      HCURSOR curWait = LoadCursor(NULL, IDC_WAIT);
      HCURSOR curNormal = SetCursor( curWait );

      postEvent(IWindow::command, UM_StartUp2, (ULONG)false);
      }
   else
      postEvent(IWindow::systemCommand, ISystemMenu::idClose);
   }

//=======================================================================
// FUNCTION NAME    startUp2
//
// DESCRIPTION      Finish initial processing in main thread.
//=======================================================================

void MainWindow :: startUp2()
   {
    // Load resources from the MRI DLL.
      if ( !loadResources() )
      {
      close();
      return;
      }

   // Create backend data manager.

   pBackEnd = new BackEnd(this);

   /*edw02/11/99-Will load the CPU table in openHost() instead of at startup
                 to improve performance.
   USHORT usError = pBackEnd->LoadCpuList();
   if ( usError )
      {
      IMessageBox::Style style = IMessageBox::cancelButton |
                                 IMessageBox::warningIcon |
                                 IMessageBox::moveable |
					IMessageBox::systemModal;
      IMessageBox msg(this);
      msg.setTitle(IResourceId(IDS_ErrorBackEnd_Title));
      msg.show(IResourceId(usError), style, IDH_ErrorBackEnd);
      close();
      return;
      }*/

   // Create client container.

   pcnrFiles = new IContainerControl(IDC_CN_Files, this, this, IRectangle(),
                        IContainerControl::defaultStyle() | IContainerControl::pmCompatible);//jt-chg
   pcolOpen = new IContainerColumn(IContainerColumn::isIcon);
   pcolOpen->hideSeparators();
   createColumn(&pcolName, CfReportFileStats::offsetName(),
                IContainerColumn::left);
   createColumn(&pcolDate, CfReportFileStats::offsetDate(),
                IContainerColumn::centered);
   createColumn(&pcolSystems, CfReportFileStats::offsetSystems(),
                IContainerColumn::right);
   createColumn(&pcolProducts, CfReportFileStats::offsetProducts(),
                IContainerColumn::right);
   createColumn(&pcolOrders, CfReportFileStats::offsetOrders(),
                IContainerColumn::right);
   pcnrFiles->addColumn(pcolOpen);
   IFont font(pcnrFiles);
   pcnrFiles->setIconSize(ISize(font.maxCharHeight(),font.maxCharHeight()));
   pcnrFiles->addColumn(pcolName);
   pcnrFiles->addColumn(pcolDate);
   pcnrFiles->addColumn(pcolSystems);
   pcnrFiles->addColumn(pcolProducts);
   pcnrFiles->addColumn(pcolOrders);
   pcolOrders->hideSeparators(IContainerColumn::verticalSeparator);

   setHeadingText(pcolName, IDS_Name1);
   setHeadingText(pcolDate, IDS_DateRetrieval1);
   setHeadingText(pcolSystems, IDS_Systems1);
   setHeadingText(pcolProducts, IDS_Products1);
   setHeadingText(pcolOrders, IDS_Orders1);

   setClient(pcnrFiles);
   pcnrFiles->showDetailsView();

   // Add handlers for container and container menu events.

   ICnrHandler::handleEventsFor(pcnrFiles);
   ICnrMenuHandler::handleEventsFor(pcnrFiles);
   menuHandler.pWindow = this;
   menuHandler.handleEventsFor(this);

   // Create a static text to show the browse path.

   ptxPath = new IStaticText(IDC_TX_Path, this, this);
   ptxPath->setAlignment(IStaticText::centerRight);

// EDS -del 1-22  ptxPath->setColor(IStaticText::background, pMenu->color(IMenu::background));
   ptxPath->setBackgroundColor( pMenu->backgroundColor()); /* EDS-added 1-22 */

   ptxPath->setText(STR_PathLabelText + strBrowsePath + " ");    //edw 12/2/97-add label
   addExtension(ptxPath, IFrameWindow::aboveClient, thickLine);

   // Set the size for the restored window.

   moveTo(pProfile->position[WINDOWMAIN]);
   sizeTo(pProfile->size[WINDOWMAIN]);

#ifdef BR_OBSOLETE_MENU   /* F62721 EDW 01/21/2002 */
//IC01552
    if( pProfile->bSort )
       {
         pMenu->checkItem(IDMI_ViewSortName);
         pMenu->uncheckItem(IDMI_ViewSortDate);
         bSort = pProfile->bSort;
         if (pProfile->bSascending)
            {
             pMenu->checkItem(IDMI_ViewSortNameA);
             pMenu->uncheckItem(IDMI_ViewSortNameD);
             pMenu->uncheckItem(IDMI_ViewSortDateA);
             pMenu->uncheckItem(IDMI_ViewSortDateD);
             pProfile->bSascending = true;
            }
         else
            {
             pMenu->checkItem(IDMI_ViewSortNameD);
             pMenu->uncheckItem(IDMI_ViewSortNameA);
             pMenu->uncheckItem(IDMI_ViewSortDateA);
             pMenu->uncheckItem(IDMI_ViewSortDateD);
             pProfile->bSascending = false;
            }
       }
    else
       {
        pMenu->uncheckItem(IDMI_ViewSortName);
        pMenu->checkItem(IDMI_ViewSortDate);
        bSort = pProfile->bSort;
        if (pProfile->bSascending)
            {
             pMenu->checkItem(IDMI_ViewSortDateA);
             pMenu->uncheckItem(IDMI_ViewSortNameD);
             pMenu->uncheckItem(IDMI_ViewSortNameA);
             pMenu->uncheckItem(IDMI_ViewSortDateD);
             pProfile->bSascending = true;
            }
         else
            {
             pMenu->checkItem(IDMI_ViewSortDateD);
             pMenu->uncheckItem(IDMI_ViewSortNameA);
             pMenu->uncheckItem(IDMI_ViewSortDateA);
             pMenu->uncheckItem(IDMI_ViewSortNameD);
             pProfile->bSascending = false;
            }

      }
#endif
#ifdef BR_OBSOLETE_MENU   /* F62721 EDW 01/21/2002 */
  // Usabilty
  if( pProfile->bFastPath )
     {
     pMenu->checkItem(IDMI_OptionsFastPathOn);
     pMenu->uncheckItem(IDMI_OptionsFastPathOff);
     }
   else
     {
     pMenu->checkItem(IDMI_OptionsFastPathOff);
     pMenu->uncheckItem(IDMI_OptionsFastPathOn);
     }
   //ZOR 48548
  if( pProfile->bExpanded )
     {
     pMenu->checkItem(IDMI_Options_ExpandedOn);
     pMenu->uncheckItem(IDMI_Options_ExpandedOff);
     bExpanded = pProfile->bExpanded;
     }
   else
     {
     pMenu->checkItem(IDMI_Options_ExpandedOff);
     pMenu->uncheckItem(IDMI_Options_ExpandedOn);
     bExpanded = pProfile->bExpanded;
     }
  // ZOR 48548
  //47971 begin new logic MAMT 21/08/2000
  if( pProfile->bTile )
     {
     pMenu->checkItem(IDMI_Options_TileHor);
     pMenu->uncheckItem(IDMI_Options_TileVer);
     bTile = pProfile->bTile;
     }
   else
     {
     pMenu->checkItem(IDMI_Options_TileVer);
     pMenu->uncheckItem(IDMI_Options_TileHor);
     bTile = pProfile->bTile;
     }
  //47971 end new logic MAMT 21/08/2000
#endif
   // Make this window visible and give it the keyboard focus.
    if( !pProfile->bFastPath )  //usability
       {
        show();
        setFocus();
       }
     else
      {
      if ( !pProfile->bEconfig && pProfile->bFastPath)
        {
        show();
        setFocus();
        }

      }

   // Load the initial list of files.
   IThread thread;
   thread.setStackSize(THREAD_STACK_SIZE);
   thread.start(new IThreadMemberFn<MainWindow>(*this, listFiles));

   }

//=======================================================================
// FUNCTION NAME    menuShowing
//
// DESCRIPTION      Handle submenu events.
//=======================================================================

Boolean MainWindow::MenuHandler :: menuShowing(IMenuEvent& menuEvt,
                                               ISubmenu& subMenu)
   {
   switch ( subMenu.id() )
      {

      // Disable the Config options inappropriate to this state.


      case IDM_Config:
         //edw 12/3/97-Remove "Proceed with Upgrade" submenu option if not running from ISM
         if ( !pWindow->bConfig )
            subMenu.deleteItem(IDMI_ConfigISMProceed);
         if ( pWindow->bLock )
            {
            subMenu.disableItem(IDMI_ConfigRetrieve);
#ifdef BR_OBSOLETE_MENU   /* F62721 EDW 01/21/2002 */
            subMenu.disableItem(IDMI_ConfigBrowse);
#endif
            if ( pWindow->bConfig )
               subMenu.disableItem(IDMI_ConfigISMProceed);   //edw 12/3/97
            break;
            }
         //edw 12/3/97
         if ( pWindow->bConfig )
            {
            if ( !pWindow->pcnrFiles->objectCount() )
               subMenu.disableItem(IDMI_ConfigISMProceed);
            if ( !pWindow->pFile )
               subMenu.disableItem(IDMI_ConfigISMProceed);
            else
               if ( !pWindow->pcnrFiles->isSelected(pWindow->pFile) )
                  subMenu.disableItem(IDMI_ConfigISMProceed);
            if ( pWindow->pFileName )
               if ( pWindow->pFileName->isVisible() )
                  subMenu.disableItem(IDMI_ConfigISMProceed);
            }
         break;

      // Disable the Selected options inappropriate to this state.

      case IDM_Selected:
         if ( pWindow->bLock )
            {
            subMenu.disableItem(IDMI_SelectedOpen);
            subMenu.disableItem(IDMI_SelectedSave);
            subMenu.disableItem(IDMI_SelectedSaveAs);
            //subMenu.disableItem(IDMI_SelectedGenerate);  //47705 - flw
            subMenu.disableItem(IDMI_SelectedRename);
            subMenu.disableItem(IDMI_SelectedDelete);
            break;
            }
         if ( !pWindow->pcnrFiles->objectCount() )
            {
            subMenu.disableItem(IDMI_SelectedOpen);
            subMenu.disableItem(IDMI_SelectedSave);
            subMenu.disableItem(IDMI_SelectedSaveAs);
            //subMenu.disableItem(IDMI_SelectedGenerate);   //47705 - flw
            subMenu.disableItem(IDMI_SelectedRename);
            subMenu.disableItem(IDMI_SelectedDelete);
            }
         if ( !pWindow->pFile )
            {
            subMenu.disableItem(IDMI_SelectedSave);
            subMenu.disableItem(IDMI_SelectedSaveAs);
            //subMenu.disableItem(IDMI_SelectedGenerate);
            }
         else
            if ( !pWindow->pcnrFiles->isSelected(pWindow->pFile) )
               {
               subMenu.disableItem(IDMI_SelectedSave);
               subMenu.disableItem(IDMI_SelectedSaveAs);
               //subMenu.disableItem(IDMI_SelectedGenerate);
               }
         if ( pWindow->pFileName )
            if ( pWindow->pFileName->isVisible() )
               {
               subMenu.disableItem(IDMI_SelectedSaveAs);
               //subMenu.disableItem(IDMI_SelectedGenerate);
               subMenu.disableItem(IDMI_SelectedRename);
               subMenu.disableItem(IDMI_SelectedDelete);
               }
         break;

      // Disable the View options inappropriate to this state.

      case IDM_View:
         if ( pWindow->bLock )

         //* D65574 Begin New Logic: (HECC) 11/14/2001
            {
         //*   subMenu.disableItem(IDMI_ViewSort);
#ifdef BR_OBSOLETE_MENU   /* F62721 EDW 01/21/2002 */
            subMenu.disableItem(IDMI_ViewSortName);
            subMenu.disableItem(IDMI_ViewSortDate);
#endif
            }
         //* D65574 End New Logic: (HECC) 11/14/2001

         break;

      // Note the impending sort sequence.

/*      case IDMI_ViewSortName:     //IC01552
         if ( pWindow->bAscending )
            subMenu.checkItem(IDMI_ViewSortA);
         else
            subMenu.checkItem(IDMI_ViewSortD);
         pWindow->sSort = MainWindow::enumSortName;
         break;

      case IDMI_ViewSortDate:
         if ( pWindow->bAscending )
            subMenu.checkItem(IDMI_ViewSortA);
         else
            subMenu.checkItem(IDMI_ViewSortD);
         pWindow->sSort = MainWindow::enumSortDate;
         break;*/

      // Disable the Options options inappropriate to this state.

      case IDM_Options:
         if ( pWindow->bLock )
            {
            //51081 - MAMT 06/01/2000
            //subMenu.disableItem(IDMI_OptionsGenerate);

#ifdef BR_OBSOLETE_MENU   /* F62721 EDW 01/21/2002 */
            subMenu.disableItem(IDMI_OptionsLocale);
            subMenu.disableItem(IDMI_OptionsRetrieval);
            subMenu.disableItem(IDMI_OptionsFastPath);
            subMenu.disableItem(IDMI_OptionsView);  //47971 - MAMT 10/12/2000
            subMenu.disableItem(IDMI_OptionsTile);  //47971 - MAMT 10/12/2000
#endif
            }
         break;
      }


   // Disable Close from the System menu.

   if ( subMenu.id() == ISystemMenu::idPulldown )
      if ( pWindow->bLock )
         subMenu.disableItem(ISystemMenu::idClose);

   return true;
   }

//=======================================================================
// FUNCTION NAME    showProgress
//
// DESCRIPTION      Show that a background task is in progress.
//=======================================================================

void MainWindow :: showProgress()
   {
   IString str0 = LOADSTRING(IDS_Progress0);
   IString str1 = LOADSTRING(IDS_Progress1);
   IString str2 = LOADSTRING(IDS_Progress2);
   IString str3 = LOADSTRING(IDS_Progress3);
   IFont font(this);
   ULONG ulWidth = font.textWidth(str3) + 20;
   ULONG ulHeight = font.maxCharHeight() + 10;
   ptxProgress->sizeTo(ISize(ulWidth,ulHeight));
   ptxProgress->moveTo(IPoint(size().width()/2-ptxProgress->size().width()/2,
                              size().height()/2-ptxProgress->size().height()/2));
   ptxProgress->setBackgroundColor(pMenu->backgroundColor());
   ptxProgress->show();
   while ( bLock )
      {
      ptxProgress->setText(str0);
      IThread::current().sleep(500);
      ptxProgress->setText(str1);
      IThread::current().sleep(500);
      ptxProgress->setText(str2);
      IThread::current().sleep(500);
      ptxProgress->setText(str3);
      IThread::current().sleep(500);
      }
   ptxProgress->hide();
   pcnrFiles->refresh(true);
   }

//edw12/29/1999
//=======================================================================
// FUNCTION NAME    addMismatch
//
// DESCRIPTION      Add given mismatch entry to strMismatches.
//=======================================================================

void MainWindow :: addMismatch( IString s )
   {
   s.change( "|", " " );
   strMismatches = strMismatches + s + "|";
   }

//edw12/29/1999
//=======================================================================
// FUNCTION NAME    addUserChoice
//
// DESCRIPTION      Add given user decision entry to strUserChoices.
//=======================================================================

void MainWindow :: addUserChoice( IString s )
   {
   s.change( "|", " " );
   strUserChoices = strUserChoices + s + "|";
   }

//=======================================================================
// 87694:  New Function.
// FUNCTION NAME    ConvertToShortPath
//
// DESCRIPTION      Converts long path names to short path names
//                  (eg, "c:\program files\econfig" to
//                       "c:\progra~1\econfig")
//                  Returns short path if successful; otherwise,
//                  returns the path name as is
//                  HECC: 02/10/2004
// MODIFY:
// D106661   RERC   03/14/2006   Modify the logic to validate the pointer
//                               of the path.
// D106661A  RERC   05/17/2006   Create a new logic for the function
//=======================================================================

//IString MainWindow :: ConvertToShortPathName( IString strPath )
//{
//   IString strReturn = strPath;

//   char *shortPath = (char*) malloc( strPath.length() + 1 );
//   /* BEGIN D106661  Modify the logic to validate the pointer
//   if( shortPath == NULL )
//      return strReturn;

//   if( GetShortPathName(strPath, shortPath, strPath.length()+1) )
//      strReturn = IString( shortPath );*/
//   if (!shortPath )
//       if( GetShortPathName(strPath, shortPath, strPath.length()+1) )
//          strReturn = IString( shortPath );
//   /*  END D106661 */

//   free( shortPath );
//   return strReturn;
//}
// D106661A Begin RERC 05/17/2006
//          The lines of the code are comented and replaced by the
//          code below.
/*IString MainWindow :: ConvertToShortPathName( IString strPath )
{
   IString strReturn = strPath;

   char *shortPath = (char*) malloc( strPath.length() + 1 );
   if( shortPath == NULL )
      return strReturn;

   if( GetShortPathName(strPath, shortPath, strPath.length()+1) )
      strReturn = IString( shortPath );

   free( shortPath );
   return strReturn;
}*/

IString MainWindow :: ConvertToShortPathName( IString strPath )
{
   IString strReturn = strPath;
   int isizePath = strPath.length()+ 1;
   int isizePathFunction = 0;
   char* shortPath = NULL;

   shortPath = (char*) malloc( isizePath );

   if( shortPath != NULL ) //Validate the allocate of memory
   {
     strcpy(shortPath,strPath);

     isizePathFunction =  GetShortPathName(strPath, shortPath, isizePath );

     //If the size of the input path is lower than the size of the path converted
     //then return the shorPath value
     if (isizePath < (isizePathFunction + 1 ))
       {
         strReturn = IString( shortPath );
       }
     else
       {
         //The length of the shortPath is bigger than the input path
         //reallocate enough memory.
         shortPath = (char*) realloc(shortPath, isizePathFunction + 1);
         if (shortPath != NULL)
         {
             isizePath = GetShortPathName(strPath, shortPath, isizePathFunction + 1 );
             if( isizePath != 0  )
                 strReturn = IString( shortPath );
         }
       }
     }
   free( shortPath );
   return strReturn;
}
//D10661A End.
//=================================================================================================



//=======================================================================
// FXXXXXX:   New Function.
// FUNCTION NAME   UpdateBIR(char *FileName)
//
// DESCRIPTION     put the serial numbers found in the MRD file and update the CFR with the correct ones
//                 feature XXXXXXX  REQ1704
//                 ELRR: 31/07/2014
//=======================================================================
///////////////////////////////////////////////////////////////////
Boolean  UpdateBIR(IString FileName)
{
char buf[5000];    // Array to hold lines
char *MRDLine[5000];
char *CFRLine[5000];
char *MRDSerials[5000];
int Count=0;int FinalCount=0;
int i=0,j=0;
int Count2=0;
IString is, ix;
int pos1=0;int pos2=0;int ipad;
IString MRDFile, CFRFile;
IString CFRSerial;
IString MRDSerialTemp;
IString CFRSerialTemp;
IString MRDSerial;



FileName.remove(FileName.lastIndexOf('.') + 1);
MRDFile=FileName;
CFRFile=FileName;
MRDFile+= IString("MRD");
CFRFile+= IString("CFR");



// Open the MRD file to get the serial numbers to compare in the CFR
//
FILE * pFile;
  pFile = fopen (MRDFile,"r");
  if (pFile==NULL)
  {
    MessageBox(0,"Error opening file",MRDFile,0);
   return FALSE;
  }



// Read All rows of Text file into Array HRET[]
//
while( fgets(buf,1000,pFile) )
 {
 MRDLine[Count] =  (char *) malloc(1500);
 strcpy(MRDLine[Count],buf);
 Count++;
 }
fclose(pFile);



// -------------------------------------------------------------------------------


for(i=0;i<Count;i++)
{
ix = MRDLine[i];

if ( !(ix.indexOf("08")== 1)) continue;
 ix = MRDLine[i+1];

  if ( !(ix.indexOf("54")== 1)) continue;
    {
      ix = ix.subString( 28, 9 );
      ix = ix.strip();
      MRDSerials[j] =  (char *) malloc(500);
      strcpy(MRDSerials[j],ix);
      j++;

     }


} // end for MRD extract
//So far we have the MRD serials to compare with the serials in the CFR



  pFile = fopen (CFRFile,"r");
  if (pFile==NULL)
  {
    MessageBox(0,"Error opening file",CFRFile,0);
   return FALSE;
  }


Count=0;

// Read All rows of CFR file into Array CFRLine[]
//
while( fgets(buf,1000,pFile) )
 {
 CFRLine[Count] =  (char *) malloc(1500);
 strcpy(CFRLine[Count],buf);
 Count++;
 }
fclose(pFile);



// we check if the MRD serial number is the same as the CFR serial number if not replace
for(i=0;i<Count;i++)
{  // for CFR
ix = CFRLine[i];


if ( !(ix.indexOf("95")== 1)) continue;
 { //if95

  CFRSerial = ix.subString( 20, 9 );
  CFRSerial = CFRSerial.strip();
  CFRSerialTemp= ix.subString( 22, 9 );
  CFRSerialTemp= CFRSerialTemp.strip();

 for(Count2=0;Count2<j;Count2++)
  {

   MRDSerialTemp= MRDSerials[Count2];
   MRDSerialTemp= MRDSerialTemp.subString(3,9);
   MRDSerialTemp= MRDSerialTemp.strip();


      if (MRDSerialTemp==CFRSerialTemp) // checamos serial menos 2 primeros digitos
      {


         if (!(CFRSerial == MRDSerials[Count2])) //checamos serial completos del CFR y MRD
         {


          pos1=ix.indexOf(CFRSerial);
          is= ix.subString(1,pos1-1);
          ix = ix.subString(pos1+9);
          ix= is + MRDSerials[Count2] + ix;

         // Store the modified string into array HRET[]
          strcpy(CFRLine[i],ix);

          is1704=true;

         }


      }//if
  } //for2
 } //if95
} //for 1






// -------------------------------------------------------------------------------



//open the ret file now for writing
  pFile = fopen (CFRDummy,"w");
  if (pFile==NULL)
  {
    MessageBox(0,"Error opening file","",0);
   return FALSE;
  }


// Write updated array into the same file
//
for (i=0;i<Count;i++)
{
fputs(CFRLine[i],pFile);
FinalCount++;
}

fclose(pFile);



/*

// Free locally allocated array in this function
for(i=0;i<Count;i++)
{
free(CFRLine[i]);
}
Count=0;
*/

return TRUE;



} // UpdateRemote()



