//=======================================================================
// FILE NAME        AF9RTSRC.CPP
//
// CLASS NAME       RetrSource
//
// DESCRIPTION      This class is the "Retrieval source" dialog of the
//                  the Base Retrieval CPF.  It is used to capture the
//                  retrieval application the user wants to use to retrieve
//                  configuration files.
//
// Programmer:      Erica Wilson  09/02/1998
// Change Log:
// Date        Programmer        Description
// =======================================================================
// 10/18/1999  E. Wilson         Add processing for pcbMismatches checkbox
//                               Note:  This dialog will now serve as the
//                               Base Retrieval preferences dialog and will
//                               store other profile data.
// 12/17/1999 F. Walker   44549  Added code so the if the user does not have
//                               Personal Communications installed on their
//                               machines to supress error message and not
//                               give them the option of doing VM retrieval.
// 07/25/2001 C. Herrera  57146  Lines to handle checkbox option for MRPD
//                               will be commented as the option Retrieve
//                               machine reported data is no longer needed.
// 01/21/2002 E. Wilson  F62721  Added support for use of this class by
//                               the Base Retrieval profile DLL (BF9PROFL.DLL)
// 02/26/2002 D. Cruz    68568   Added definition of functions
//                               validRetrievalPath() and setFocusRetrievalPath().
//                               Allowed the user to key in the retrieval path
//                               in the pefRetrPath entry field.
// 03/08/2002 D. Cruz    68569   Added code to catch TAB and SHIFT+TAB
//                               key presses to return focus to the
//                               ProfileDialog window. Added select calls
//                               to radiobuttons prbSortByNameA & prbSortByDateA,
//                               so that when prbSortByName & prbSortByDate
//                               loose focus, the former controls regain
//                               focus correctly and TABBING through the
//                               window controls works correctly.
// 02/28/2003 C. Herrera 70162   Added logic to convert a long path name to a
//                               short path name, in the parameter that Base
//                               Retrieval passes to the host files.
//                               This is necessary to avoid problems in the
//                               handling of this paramter by the host files
// 08/19/2003 C. Herrera 79483   Added logic to set the default Retrieval path directory
//                               to <execution path>\UserData as the default when
//                               the Browse button is pressed and the path specified
//                               by the user doesn't exist.
// 28/07/2003 C. Herrera 79751   Added logic to handle the two browser options:
//                               a) ub9host3.exe will now be handled as "Installed
//                               Inventory"
//                               b) ub9host4.exe new option,  will be handled as
//                               "Inventory Services"
// 09/16/2003 C. Herrera 79483A. The function stripTrailing() is not correctly processed
//                               so the call to it must be coded in a second instruction.
// 10/21/2003 C. Herrera 79847   Adds necessary logic to catch the F1 key to start
//                               the Help windows displaying process
// 03/10/2004 C. Herrera 87612   Add the new SQUADRON string identifier to the Product
//                               Family list
//
// 05/11/2004 Z. Obregon 91005   IS is not maintaining MR data checkbox when via BaseRetrieval
//                               -- when zseries is selected the mrpd flag will be true, this flag is passed to IS
//
// 09/21/2004 C. Herrera 94556   A general problem of consistency in the "/" and "\" characters
//                               within the cpf profiles was fixed.
// 09/02/2005 E. Reyes   97759   The strings "//" or "/\" or "\/" or "\\\" follow of
//                               the drive are invalid as input in the retrieval path
//                               in the pefRetrPath entry field.
// 24/02/2005 E. Reyes   98105   Profile Files Path accepting invalid characters
// 29/05/2006 C. Herrera 108502  Added logic to disable the "VM Host" (Ub9host1.exe) and
//                               "Inventory Services" (Ub9host4.exe) options for EMEA
//                               geography users.
// 29/05/2006 E. Reyes D106765   Req 1436.  Modify the buildcontrols() function to
//                               not display "VM host" interface when user geo is "US"
//
// 12/12/2008 ELRR      F127433  Added logic to Remove the 'VM Host'(ub9host1.exe) from
//                               the retrieval tab of Base Retrieval Preferences Window.
//
//
//08/12/2009  ELRR      F133122  Several modification were made to allow Base Retrieval controls handle Firefox Browser.
//                      Req1547
//
//
//09/05/2011  ELRR       F140394  This Requirement is a request for Base Retrieval to include EMEA in its operation for Inventory Services.
//                      Req1597
//
//25/06/2014  GTL       Req1702  Several changes were made to support Google Chrome on Base Retrieval.
// =======================================================================
// Copyright (c) International Business Machines Corp. 1998
// All rights reserved
//========================================================================
#include <process.h>                    // _spawnlp
#include <icmdhdr.hpp>                  // ICommandHandler
#include <iframe.hpp>                   // IFrameWindow
#include <ihelp.hpp>                    // IHelpWindow
#include <ikeyhdr.hpp>                  // IKeyboardHandler
#include <ipushbut.hpp>                 // IPushButton
#include <icheckbx.hpp>                 // ICheckBox
#include <iradiobt.hpp>                 // IRadioButton
#include <ientryfd.hpp>                 // IEntryField        /* F62721 EDW 01/21/2002 */
#include <ifiledlg.hpp>                 // IFileDialog        /* F62721 EDW 01/21/2002 */
#include <ifilehdr.hpp>                 // IFileDialogHandler /* F62721 EDW 01/21/2002 */
#include <ilistbox.hpp>                 // IListBox
#include <isysmenu.hpp>                 // ISystemMenu
#include <iselhdr.hpp>                  // ISelectHandler
#include "af9rtsrc.hpp"                 // Retrieval source
#ifndef BR_PROFILE    /* F62721 EDW 01/21/2002 */
  #include "af9main.hpp"                // MainWindow
#else
  #include "af9pfdlg.hpp"                // ProfileDialog
#endif
#include "af9profl.hpp"                 // Profile
#include "ub9host.h"                    // Host interface tags
#include "af9apply.h"                   // Resource ids
#include <sys\types.h>                  // stat
#include <sys\stat.h>                   // stat
#include <ctype.h>                      // character classification - 44549

//=======================================================================
// FUNCTION NAME    RetrSource
//
// DESCRIPTION      Construct an instance of the RetrSource class.
//=======================================================================

#ifndef BR_PROFILE    /* F62721 EDW 01/21/2002 */
RetrSource :: RetrSource(MainWindow* pOwner )
            : IFrameWindow(IResourceId(IDD_RetrSource), NULL, pOwner)
#else
RetrSource :: RetrSource(ProfileDialog* pOwner )
            : IFrameWindow(IResourceId(IDD_RetrSource),
                           pOwner->getProfileNotebook(),
                           pOwner->getProfileNotebook())
#endif
   {
   // Save parameters;

   pMain = pOwner;

   // Initial variables

   sRetFamItem = -1;  //D68816 02/15/2002

   // Attach the help window.

   pOwner->helpWindow()->setAssociatedWindow(this);

   // Add handlers for command and keyboard messages.

   ICommandHandler::handleEventsFor(this);
   IKeyboardHandler::handleEventsFor(this);
   ISelectHandler::handleEventsFor(this);

   // Create control objects.

   buildControls();

   // Set listbox auto horizontal scroll - Only needed for W95
   setHorzScrollWidth();

   // Set the dialog to self-destruct when closed.

   setAutoDeleteObject();

   }

//=======================================================================
// FUNCTION NAME    command
//
// DESCRIPTION      Handle command messages.
//=======================================================================

Boolean RetrSource :: command(ICommandEvent& cmdEvt)
   {

    switch ( cmdEvt.commandId() )
      {
      // User selected OK.

      case IDC_PB_OK:
         {
          pMain->pProfile->strRetApp = appSet.elementWithKey( strAppName ).getExeName();
          try  //D68491 EDW 02/05/2002 - added try/catch because this logic fails sometimes
             {
             if (!pMain->pProfile->bEconfig && sRetFamItem != -1) //54727 MAMT 18/10/2000 //D68816 02/15/2002
                {
                //54792 begin new logic MAMT 09/14/2000
                if ((IString)plbProdFamily->itemText ( sRetFamItem ) == STR_S390N)
                   pMain->pProfile->strProdFamily = STR_S390;
                else
                   if ((IString)plbProdFamily->itemText ( sRetFamItem ) == STR_AS400N)
                      pMain->pProfile->strProdFamily = STR_AS400;
                   else
                      //* F87612 Begin Logic: Add the new SQUADRON identifier to the profile structure
                      if ((IString)plbProdFamily->itemText ( sRetFamItem ) == STR_SQUADN)
                         pMain->pProfile->strProdFamily = STR_SQUAD;
                      else
                      //* F87612 End new logic:  HECC - 03/10/2004
                      pMain->pProfile->strProdFamily = STR_RS6000;
                //54792 end new logic MAMT 09/14/2000
                }
             }
          catch(IException& exc)
             { // If this fails, it means nothing is selected in the
             } // listbox, so ignore this request to update profile
//old code 54035 pMain->pProfile->strProdFamily = plbProdFamily->itemText ( sRetFamItem );

         if ( pcbHW->isSelected() )
               pMain->pProfile->bHardware = true;
         else
               pMain->pProfile->bHardware = false;

         if ( pcbSW->isSelected() )
               pMain->pProfile->bSoftware = true;
         else
               pMain->pProfile->bSoftware = false;
#ifndef BR_PROFILE    /* F62721 EDW 01/21/2002 */
         if ( pcbMismatches->isSelected() )
            pMain->pProfile->bShowS390Mismatches = true;
         else
            pMain->pProfile->bShowS390Mismatches = false;
#endif
         //* 57146 Begin new logic.  HECC:  07/24/2001

         //* if ( pcbMRPD->isSelected() )                //Usability
         //*       pMain->pProfile->bMRPD = true;
         //* else
         //*       pMain->pProfile->bMRPD = false;

         //* 57146 End new logic.  HECC:  07/24/2001

         if ( pcbIE->isSelected() )                    //Usability
            {
             pMain->pProfile->strBrowser = STR_Profile_IE;
             // Begin Code d51744  (HECC)
             HKEY key;
             if (RegOpenKeyEx (HKEY_CURRENT_USER, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)
                {
                 RegSetValueEx(key, STR_Profile_Browser, 0, REG_SZ, (CONST BYTE*)STR_Profile_IE, IString(STR_Profile_IE).length()+1);
                }
             RegCloseKey (key);
             // End Code d51744  (HECC)
            }
  
  // GTL Req1702 BEGIN:
			else if ( pcbNet->isSelected() )
            {
			 pMain->pProfile->strBrowser = STR_Profile_Chrome;
             HKEY key;
             if (RegOpenKeyEx (HKEY_CURRENT_USER, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)
                {
				RegSetValueEx(key, STR_Profile_Browser, 0, REG_SZ, (CONST BYTE*)STR_Profile_Chrome, IString(STR_Profile_Chrome).length()+1);
                }
             RegCloseKey (key);
            }
			
			else
	//GTL Req1702 END
	
             {
			 //ELRR-F133122 Begin:
               pMain->pProfile->strBrowser = STR_Profile_Firefox;
			   //ELRR-F133122 End.
			   
			  // Begin Code d51744  (HECC)
              HKEY key;
			  
              if (RegOpenKeyEx (HKEY_CURRENT_USER, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)
                 {
				 //ELRR-F133122 Begin:
	 //              RegSetValueEx(key, STR_Profile_Browser, 0, REG_SZ, (CONST BYTE*)STR_Profile_Netscape, IString(STR_Profile_Netscape).length()+1);
                  RegSetValueEx(key, STR_Profile_Browser, 0, REG_SZ, (CONST BYTE*)STR_Profile_Firefox, IString(STR_Profile_Firefox).length()+1);
				  //ELRR-F133122 End:
				  // End Code d51744  (HECC)
                 }
              RegCloseKey (key);
             }
			
#ifndef BR_PROFILE    /* F62721 EDW 01/21/2002 */
         if ( pcbAutoRec->isSelected() )
               pMain->pProfile->bAutoRec = true;          //47698
         else
               pMain->pProfile->bAutoRec = false;

         if ( pcbProcRPO->isSelected() )
               pMain->pProfile->bProcRPO = true;          //47700
         else
               pMain->pProfile->bProcRPO = false;

         //close();
         hide();
         removeFromWindowList();
         pMain->bLock = false;
         pMain->setFocus();
#else
         if ( pcbFastPath->isSelected() )
            pMain->pProfile->bFastPath = true;
         else
            pMain->pProfile->bFastPath = false;

         if ( prbSortByName->isSelected() )
            {
            pMain->pProfile->bSort = true;
            if ( prbSortByNameA->isSelected() )
               {
               pMain->pProfile->bSascending = true;
               pMain->pProfile->sSortFiles = 1;
               }
            else
               {
               pMain->pProfile->bSascending = false;
               pMain->pProfile->sSortFiles = -1;
               }
            }
         else
            {
            pMain->pProfile->bSort = false;
            if ( prbSortByDateA->isSelected() )
               {
               pMain->pProfile->bSascending = true;
               pMain->pProfile->sSortFiles = 2;
               }
            else
               {
               pMain->pProfile->bSascending = false;
               pMain->pProfile->sSortFiles = -2;
               }
            }

         IString strNewPath = pefRetrPath->text().strip();
         if ( strNewPath != pMain->pProfile->strBrowsePath.strip() )
            pMain->pProfile->bRefreshList = true;
         else
            pMain->pProfile->bRefreshList = false;
         pMain->pProfile->strBrowsePath = strNewPath;
         pMain->pProfile->strGeneratePath = strNewPath;   /* D69031 EDW 02/21/2002 */
#endif
         break;
         }
#ifdef BR_PROFILE    /* F62721 EDW 01/21/2002 */
      // User selected Browse button

      case IDC_PB_RetrSource_Browse:
         {
         IString strPath = pefRetrPath->text();
         selectDirectory( strPath );
         break;
         }
#endif
      // User selected Cancel.

      case IDC_PB_Cancel:
         // Reset listbox to value saved in user's profile
         plbInterfaces->select( sRetAppItem );
         try //D68546 EDW 01/21/2002 - added try/catch because this logic fails sometimes
            {
            // Reset listbox to value saved in user's profile    47701 - flw
            if (sRetFamItem != -1)  //D68816 02/15/2002
               plbProdFamily->select ( sRetFamItem );
            }
         catch(IException& exc)
            { // If this fails, it means nothing was previously selected
            } // in the listbox, so ignore this request to reset listbox
         // Reset checkbox to value saved in user's profile
         if ( pMain->pProfile->bHardware )
             pcbHW->select();
         else
             pcbHW->deselect();

         if ( pMain->pProfile->bSoftware )
             pcbSW->select();
         else
             pcbSW->deselect();

         //* 57146 Begin new logic.  HECC:  07/24/2001

         //* if ( pMain->pProfile->bMRPD )
         //*     pcbMRPD->select();
         //* else
         //*     pcbMRPD->deselect();

         //* 57146 End new logic.  HECC:  07/24/2001

//GTL Req1702 BEGIN
		 if ( pMain->pProfile->strBrowser == STR_Profile_Chrome )
             pcbNet->select();
//GTL Req1702 END

			          
		
		 //ELRR-F133122 Begin:
//       if ( pMain->pProfile->strBrowser == STR_Profile_Netscape )
		else if ( pMain->pProfile->strBrowser == STR_Profile_Firefox )
             pcbFir->select();	 
		//ELRR-F133122  End.	 
		 else 
             pcbIE->select();

#ifndef BR_PROFILE    /* F62721 EDW 01/21/2002 */
         if ( pMain->pProfile->bShowS390Mismatches )
            pcbMismatches->select();
         else
            pcbMismatches->deselect();

         if ( pMain->pProfile->bAutoRec )
            pcbAutoRec->select();          //47698
         else
            pcbAutoRec->deselect();

         if ( pMain->pProfile->bProcRPO )
            pcbProcRPO->select();          //47700
         else
            pcbProcRPO->deselect();

         hide();
         removeFromWindowList();
         pMain->bLock = false;
         pMain->setFocus();
#else
         if ( pMain->pProfile->bFastPath )
            pcbFastPath->select();
         else
            pcbFastPath->deselect();

         if ( pMain->pProfile->bSort )
            {
            prbSortByName->select();
            if ( pMain->pProfile->bSascending )
               prbSortByNameA->select();
            else
               prbSortByNameD->select();
            }
         else
            {
            prbSortByDate->select();
            if ( pMain->pProfile->bSascending )
               prbSortByDateA->select();
            else
               prbSortByDateD->select();
            }

         pefRetrPath->setText(pMain->pProfile->strBrowsePath);
#endif
         break;

      default:
         return false;
      }
   return true;
   }

//=======================================================================
// FUNCTION NAME    systemCommand
//
// DESCRIPTION      Handle system command events.
//=======================================================================

Boolean RetrSource :: systemCommand(ICommandEvent& cmdEvt)
   {
#ifndef BR_PROFILE    /* F62721 EDW 01/21/2002 */
   // Hide this window instead of closing it.

   if ( cmdEvt.commandId() == ISystemMenu::idClose )
      {
      postEvent(IWindow::command, IDC_PB_Cancel);
      return true;
      }
#endif
   return false;
   }

//=======================================================================
// FUNCTION NAME    virtualKeyPress
//
// DESCRIPTION      Handle the Escape key.
//=======================================================================

Boolean RetrSource :: virtualKeyPress(IKeyboardEvent& keyEvt)
   {
#ifndef BR_PROFILE    /* F62721 EDW 01/21/2002 */
   if ( keyEvt.virtualKey() == IKeyboardEvent::esc )
      {
      postEvent(IWindow::command, IDC_PB_Cancel);
      return true;
      }
#endif

// DCG 03/07/2002 68569 Begin
#ifdef BR_PROFILE
   IKeyboardEvent::VirtualKey keyPressed = keyEvt.virtualKey();

   if ( plbInterfaces->hasFocus() && keyPressed == IKeyboardEvent::backTab )
      {
      pMain->postEvent(IWindow::command, IDS_ProfileDialog_TabSetFocus);
      return true;
      }
   if ( pcbFastPath->hasFocus() && keyPressed == IKeyboardEvent::tab )
      {
      pMain->postEvent(IWindow::command, IDS_ProfileDialog_OKSetFocus);
      return true;
      }

   // 79847 Begin: Checks if the 'F1' key was pressed to display the help windows of the
   // Retrieval tab of the Base Retrieval profile preferences.
   if ( keyPressed == IKeyboardEvent::f1 )
      {
      pMain->helpWindow()->show(IResourceId(IDD_RetrSource));
      return true;
      }
   //*79847 End:   HECC - 10/21/2003

#endif
// DCG 03/07/2002 68569 End

   return false;
   }

//=======================================================================
// FUNCTION NAME    buildControls
//
// DESCRIPTION      Instantiate the various dialog controls.
//=======================================================================

void RetrSource :: buildControls()
   {
   plbInterfaces = new IListBox(IDC_LB_RetrSource_Interfaces, this);
   plbProdFamily = new IListBox(IDC_LB_RetSource_ProdFam, this);
   pcbHW = new ICheckBox(IDC_CB_RetrSource_HW, this);
   pcbSW = new ICheckBox(IDC_CB_RetrSource_SW, this);
#ifndef BR_PROFILE    /* F62721 EDW 01/21/2002 */
   pcbMismatches = new ICheckBox(IDC_CB_RetrSource_Mismatches, this);
#endif
   //* 57146 Begin new logic.  HECC:  07/24/2001

   //* pcbMRPD = new ICheckBox(IDC_CB_MRPD, this);  // Usability

   //* 57146 End new logic.  HECC:  07/24/2001

   pcbIE = new IRadioButton(IDC_RB_IExplorer, this);   // Usability

// ELRR-F133122 Begin:
   pcbNet = new IRadioButton(IDC_RB_Netscape, this);   // Usability
// ELRR-F133122 END.

//GTL Req1702 BEGIN:
   pcbFir = new IRadioButton(IDC_RB_Firefox, this);   // Usability
//GTL Req1702 END.

#ifndef BR_PROFILE    /* F62721 EDW 01/21/2002 */
   pcbAutoRec = new ICheckBox(IDC_CB_RetrSource_AutoRec, this);    //F47698 - flw
   pcbProcRPO = new ICheckBox(IDC_CB_RetrSource_ProcRPO, this);    //F47700 - flw
#else
   pcbFastPath = new ICheckBox(IDC_CB_RetrSource_FastPath, this);
   prbSortByName = new IRadioButton(IDC_RB_RetrSource_SortName, this);
   prbSortByDate = new IRadioButton(IDC_RB_RetrSource_SortDate, this);
   prbSortByNameA = new IRadioButton(IDC_RB_RetrSource_SortNameA, this);
   prbSortByNameD = new IRadioButton(IDC_RB_RetrSource_SortNameD, this);
   prbSortByDateA = new IRadioButton(IDC_RB_RetrSource_SortDateA, this);
   prbSortByDateD = new IRadioButton(IDC_RB_RetrSource_SortDateD, this);
   ppbBrowseRetrPath = new IPushButton(IDC_PB_RetrSource_Browse, this);
   pefRetrPath = new IEntryField(IDC_EF_RetrSource_RetrPath, this);
#endif

#ifndef BR_PROFILE    /* F62721 EDW 01/21/2002 */
   static int iFileCnt( 0 );
   static int iFamCnt( 0 );
#else
   int iFileCnt( 0 );
   int iFamCnt( 0 );
#endif

   bBrowseDir = false;   //* 79483.   HECC - 08/19/2003

   Boolean bVMHost = false;
   char strAppName[256], strVMName[256]; //flw 44549

   //* F127433: Begin logic: Instead of search for all ub9host*.exe interfaces, the logic will
   //* search only for the ub9host4.exe (Inventory Services Host Interface)
   //LPCTSTR strUBXHost = (LPCTSTR)( pMain->strExecPath + STR_HostInterfaces );
     LPCTSTR strUBXHost = (LPCTSTR)( pMain->strExecPath + STR_IServices );
   //* F127433: End Logic:ELRR 12/11/08


   WIN32_FIND_DATA fileData;

   //Find first occurence of a host interface file (ub9hostX.exe) in the install dir.
   HANDLE handle = FindFirstFile( strUBXHost, &fileData );


// F140394 Begin Req1597:ELRR: 09/05/2011 EMEA will be avalible to use Inventory Services(UB9HOST4) from Base Retrieval, we will remove the F108502 code.
   //* F108502: Begin logic: Avoid to load the Ub9hostX interface programs for EMEA's users
//   if(getUserGeography() != STR_EMEA)
   //* F108502: End logic: HECC - 05/29/2006 Avoid to load the Ub9hostX interface programs for EMEA's users
//     {
// F140394 END: Req 1597: ELRR  09/05/2011

     while ( handle != INVALID_HANDLE_VALUE )
        {
        strcpy(strAppName,fileData.cFileName);   //flw - 44549
        strcpy(strVMName,STR_VMHost);
        if ( strcmpi( strAppName,strVMName) )
           bVMHost = false;
         else
           bVMHost = true;
        if (!((getUserGeography() == STR_US) && bVMHost)) //F106765  RERC 05/29/2006
          if ((!bVMHost) || (pMain->pProfile->bPComSupported  && bVMHost)
             || (!bVMHost) || (pMain->pProfile->bPassportSupported && bVMHost))   //flw - 44549
             {
            //Get name of host interface file that was found
            IString strAppName = getHostInterfaceName( fileData.cFileName );

           //Add name of host interface application to listbox.  Also, store the
           //host interface name and corresponding executable filename to a KeySet object.
           if ( strAppName != STR_NoDescription )
             {
             plbInterfaces->add( iFileCnt, strAppName );
             if ( pMain->pProfile->strRetApp.upperCase() == ((IString)fileData.cFileName).upperCase())
                {
                plbInterfaces->select( iFileCnt );
                sRetAppItem = iFileCnt;
                }
             appSet.add( RetrApps( strAppName, fileData.cFileName ) );
             iFileCnt++;
             }
           //If no host interface name, add the name of the host interface file to listbox
           else
             {
             IString strGenericName = (IString)fileData.cFileName+STR_NoDescription;
             plbInterfaces->add( iFileCnt, strGenericName );
             if ( pMain->pProfile->strRetApp.upperCase() == ((IString)fileData.cFileName).upperCase())
                {
                plbInterfaces->select( iFileCnt );
                sRetAppItem = iFileCnt;
                }
             appSet.add( RetrApps( strGenericName, fileData.cFileName ) );
             iFileCnt++;
             }
           }
          //Find all other occurences of host interface files (ub9hostX.exe) in the install dir.
          if ( !FindNextFile( handle, &fileData ) )
             break;
           }

   //F140394 Begin Req 1597 ELRR: 09/05/2011 Remove the remaining code of the F108502 implementation.
//      }
   //* F108502 Begin logic: Else condition means that the Local Files option will be selected
   //* by default for all the EMEA's users.
//   else
//      pMain->pProfile->strRetApp = (IString)STR_LocalFiles;
   //* F108502 End logic:  HECC - 05/29/2006

   //F140394 END Req 1597 ELRR: 09/05/2011

   //Add "Local files" choice as last item in listbox and KeySet object
   plbInterfaces->add(iFileCnt, STR_LocalFiles);
   if ( (IString)pMain->pProfile->strRetApp.upperCase() == ((IString)STR_LocalFiles).upperCase())
         {
         plbInterfaces->select( iFileCnt );
         sRetAppItem = iFileCnt;
         }
   appSet.add( RetrApps( STR_LocalFiles, STR_LocalFiles ) );

   //Close file find resources
   FindClose( handle );

    //Add Product Families   - 47701
   plbProdFamily->add(iFamCnt, STR_S390N);  //54035 MAMT 09/14/2000 handle the new var
   if ( (IString)pMain->pProfile->strProdFamily.upperCase() == (IString)STR_S390)
         {
         plbProdFamily->select( iFamCnt );
         sRetFamItem = iFamCnt;
         }
   iFamCnt++;

   plbProdFamily->add(iFamCnt,STR_AS400N);  //54035 MAMT 09/14/2000 handle the new var
   if ( (IString)pMain->pProfile->strProdFamily.upperCase() == (IString)STR_AS400)
         {
         plbProdFamily->select( iFamCnt );
         sRetFamItem = iFamCnt;
         }
   iFamCnt++;

   plbProdFamily->add(iFamCnt, STR_RS6000N); //54035 MAMT 09/14/2000 handle the new var
   if ( (IString)pMain->pProfile->strProdFamily.upperCase() == (IString)STR_RS6000)
         {
         plbProdFamily->select( iFamCnt );
         sRetFamItem = iFamCnt;
         }
   iFamCnt++;

   //* F87612 Begin Logic: Add the new SQUADRON identifier to the Product Family list

   plbProdFamily->add(iFamCnt, STR_SQUADN);
   if ( (IString)pMain->pProfile->strProdFamily.upperCase() == (IString)STR_SQUAD)
         {
         plbProdFamily->select( iFamCnt );
         sRetFamItem = iFamCnt;
         }
   iFamCnt++;

   //* F87612 End new logic:  HECC - 03/10/2004

   if ( pMain->pProfile->bHardware )
      pcbHW->select();

   if ( pMain->pProfile->bSoftware )
      pcbSW->select();

   //* 57146 Begin new logic.  HECC:  07/24/2001

   //* if ( pMain->pProfile->bMRPD )    //38935 flw
   //*    pcbMRPD->select();

   //* 57146 End new logic.  HECC:  07/24/2001

   if (!pMain->pProfile->bIESupported)
      pcbIE->disable();
   else
      {
      if ( pMain->pProfile->strBrowser == STR_Profile_IE )
         pcbIE->select();
      }
	  
	  
	  //GTL Req1702 BEGIN

   if (!pMain->pProfile->bChromeSupported)   
       pcbNet->disable();
   else
      {
		 if ( pMain->pProfile->strBrowser == STR_Profile_Chrome ) //GTL Req1702 END:
            pcbNet->select();
      }
	  
	  //GTL Junio 5 se agrega para firefox begin
// ELRR-F133122 Begin:Check if Firefox is intalled if not we disable the option.
// if (!pMain->pProfile->bNetscapeSupported)        //38935 flw
   if (!pMain->pProfile->bFirefoxSupported)   //ELRR-F133122 End:
	pcbFir->disable();
   else
      {
	  
	  //GTL Req1702 BEGIN
//       ELRR-F133122 Begin:
//       if ( pMain->pProfile->strBrowser == STR_Profile_Netscape )
         // if ( pMain->pProfile->strBrowser == STR_Profile_Firefox ) //ELRR-F133122 END:
		 if ( pMain->pProfile->strBrowser == STR_Profile_Firefox ) 
            pcbFir->select();
      }
	  // GTL Req1702 END

#ifndef BR_PROFILE    /* F62721 EDW 01/21/2002 */
   //Select and disable checkbox if know the product model is S390
   if ( pMain->strProductModel == STR_S390 )
      {
      pMain->pProfile->bHardware = true;
      pMain->pProfile->bSoftware = false;
      pcbHW->select();
      pcbHW->disable();
      pcbSW->deselect();
      pcbSW->disable();
      }
#else
   if ( pMain->pProfile->strProdFamily.upperCase() == STR_S390 )
      {
      pMain->pProfile->bHardware = true;
      pMain->pProfile->bSoftware = false;
      pcbHW->select();
      pcbHW->disable();
      pcbSW->deselect();
      pcbSW->disable();
      }
#endif
   /*edw
   else
      {
      pMain->pProfile->bShowS390Mismatches = false;
      pcbMismatches->deselect();
      pcbMismatches->disable();
      } */

#ifndef BR_PROFILE    /* F62721 EDW 01/21/2002 */
   if ( pMain->pProfile->bShowS390Mismatches )
      pcbMismatches->select();

   if ( pMain->pProfile->bAutoRec )             //47698 - flw
      pcbAutoRec->select();

   if ( pMain->pProfile->bProcRPO )             //47698 - flw
      pcbProcRPO->select();
#else
//   pefRetrPath->disableDataUpdate();    // DCG 68568 02/21/2002
   pefRetrPath->enableAutoScroll();
   prbSortByName->enableGroup();
   prbSortByNameA->enableGroup();
   prbSortByDateA->enableGroup();

   if ( pMain->pProfile->bFastPath )
      pcbFastPath->select();

   if ( pMain->pProfile->bSort )
      {
      prbSortByName->select();
      prbSortByDateA->disable();
      prbSortByDateD->disable();
      if ( pMain->pProfile->bSascending )
         prbSortByNameA->select();
      else
         prbSortByNameD->select();
      }
   else
      {
      prbSortByDate->select();
      prbSortByNameA->disable();
      prbSortByNameD->disable();
      if ( pMain->pProfile->bSascending )
         prbSortByDateA->select();
      else
         prbSortByDateD->select();
      }
   pefRetrPath->setText(pMain->pProfile->strBrowsePath);
#endif
   if ( pMain->pProfile->bEconfig )
      //plbProdFamily->disable();           //47701 - flw
      plbProdFamily->hide();                //48932 - flw

   ppbOK = new IPushButton(IDC_PB_OK, this);
   ppbCancel = new IPushButton(IDC_PB_Cancel, this);
   ppbHelp = new IPushButton(IDC_PB_Help, this);

   ppbHelp->enableHelp();
   ppbHelp->disableMouseClickFocus();
   }


//=======================================================================
// FUNCTION NAME    getHostInterfaceName
//
// DESCRIPTION      Calls specified host interface application and queries
//                  it for it's descriptive name.
//=======================================================================

IString RetrSource :: getHostInterfaceName( IString strHost )
   {
   IString strTempFile = tmpnam(NULL);
   FILE* pTempFile = fopen(strTempFile, "w");

   fprintf(pTempFile, "%s\n", STR_TagLocale);
   fprintf(pTempFile, "%s\n", pMain->pProfile->strLocale);

   fprintf(pTempFile, "%s\n", STR_TagGetName);
   fclose(pTempFile);

   strHost = pMain->strExecPath + strHost;

   //* 70162  Begin New logic: Converts the long path name of the parameter
   //* strHost, wich is the path name of both host files, Ub9host1.exe
   //* and Ub9host3.exe. This is necessary because, when it contains
   //* spaces, the logic of both host files evaluates it as more than one
   //* parameter. Example: The path name:  c:\Program Files\econfig represents
   //* two parameters because of the space that this name contains

   #ifdef BR_PROFILE
      IString strShortName = ProfileDialog::ConvertToShortPathName(strHost);
      strHost = strShortName;
   #endif

   //* 70162  Ends New logic.  HECC: 02/28/2003

   IString strPara = IString("\"")+ strHost +IString("\"");
   LONG lRC = _spawnl(P_WAIT, strHost, strPara, strTempFile, NULL);
   if ( !lRC )
      {
      pTempFile = fopen(strTempFile, "r");
      char szFile[512];
      Boolean bNameString = false;
      while ( fgets(szFile, sizeof(szFile), pTempFile) )
         {
         IString strName = szFile;
         strName.remove(strName.length());
         if ( strName == STR_TagName )
            {
            bNameString = true;
            continue;
            }
         if ( strName == STR_TagReturn )
            break;
         if ( bNameString )
            {
            fclose(pTempFile);
            remove( strTempFile ); //edw05/19/99
            return strName;
            }
         }
      fclose(pTempFile);
      }
   remove( strTempFile ); //edw05/19/99
   return IString( STR_NoDescription );
   }

//=======================================================================
// FUNCTION NAME    selected
//
// DESCRIPTION      Handle selection of dialog components
//=======================================================================

Boolean RetrSource :: selected(IControlEvent& ctlEvt)
   {
   LONG lControl = ctlEvt.controlId();

   switch( lControl )
      {
      case IDC_LB_RetrSource_Interfaces:
         {
         IListBox::Cursor lbCursor( *plbInterfaces );
         for ( lbCursor.setToFirst(); lbCursor.isValid(); lbCursor.setToNext() )
            if ( plbInterfaces->isSelected( lbCursor.asIndex() ) )
               strAppName = plbInterfaces->elementAt( lbCursor );

         //* F79751:  Begin new logic: Next changes are needed to handle the two browser options:
         //* a) ub9host3.exe will now be handled as "Installed Inventory"
         //* b) ub9host4.exe will be handled as "Inventory Services"
         //* if (strAppName != "Browser host")    //38935 flw
         if ( (strAppName != "Installed Inventory Host Interface") &&
              (strAppName != "Inventory Services Host Interface")    )
         //* F79751:  End new logic:  HECC - 28/07/2003

            {
            pcbIE->disable();
            pcbNet->disable();
			//GTL Req1702 BEGIN:
			pcbFir->disable();
			//GTL Req1702 END.
            }
         else
            {
			//GTL Req1702 BEGIN:
            // if (pMain->pProfile->bFirefoxSupported)   //ELRR-F133122 End:
			if (pMain->pProfile->bChromeSupported)   
               pcbNet->enable();
            else
				pcbNet->disable();
			
			//          ELRR-F133122 Begin:
			//          if (pMain->pProfile->bNetscapeSupported)  //38935 flw
			if (pMain->pProfile->bFirefoxSupported)   
               pcbFir->enable();
            else
               pcbFir->disable();
			   //GTL Req1702 END.

            if (pMain->pProfile->bIESupported)
               pcbIE->enable();
            else
               pcbIE->disable();
            }
         break;
         }
      case IDC_CB_RetrSource_HW:
      case IDC_CB_RetrSource_SW:
         {
         if ( !pcbHW->isSelected() && !pcbSW->isSelected() )
            {
            pcbHW->select();
            pcbSW->select();
            }
         break;      /* F62721 EDW 01/21/2002 */
         }
      case IDC_LB_RetSource_ProdFam:
         {
         IListBox::Cursor lbCursor( *plbProdFamily );
         for ( lbCursor.setToFirst(); lbCursor.isValid(); lbCursor.setToNext() )
            if ( plbProdFamily->isSelected( lbCursor.asIndex() ) )
               sRetFamItem = lbCursor.asIndex();
         if ( plbProdFamily->itemText( sRetFamItem ) == STR_S390N )
            {
            pcbHW->select();
            pcbHW->disable();
            pcbSW->deselect();
            pcbSW->disable();
            pMain->pProfile->bMRPD = true; //ZOR 91005 is zseries and s390
            }
         else
            {
            pcbHW->enable();
            pcbSW->enable();
            pMain->pProfile->bMRPD = false; //ZOR  91005 is not zseries
            }
         break;      /* F62721 EDW 01/21/2002 */
         }
#ifdef BR_PROFILE    /* F62721 EDW 01/21/2002 */
      case IDC_RB_RetrSource_SortName:
         {
         prbSortByNameA->enable();
         prbSortByNameD->enable();
         prbSortByDateA->deselect();
         prbSortByDateD->deselect();
         prbSortByDateA->disable();
         prbSortByDateD->disable();
         prbSortByNameA->select();             // DCG 02/27/2002 68569
         break;
         }
      case IDC_RB_RetrSource_SortDate:
         {
         prbSortByDateA->enable();
         prbSortByDateD->enable();
         prbSortByNameA->deselect();
         prbSortByNameD->deselect();
         prbSortByNameA->disable();
         prbSortByNameD->disable();
         prbSortByDateA->select();             // DCG 02/27/2002 68569
         break;
         }
#endif
      default:
         return false;
      }
   return false;
   }

//=======================================================================
// FUNCTION NAME    validRetrievalPath
//
// DESCRIPTION      Verifies if retrieval path keyed in by user exists
//
// RETURN VALUE     Returns true if path is valid.
//                  Returns false if path is invalid.
//
// DCG 02/22/2002 68568
// RECR 02/24/2005  D98105. The code from release 5.1.16 was reordered to
//                  change the logic and guarantee the correct validation
//                  of the path typed for the user.
//=======================================================================

//Code from RELEASE 5.1.16
/*#ifdef BR_PROFILE
IBoolean RetrSource :: validRetrievalPath()
{
// D94556 Begin logic:  If the lenght of the input character is 1, then validate it
// to check if it is the "\\"  character to change it to the default C:\ directory
    if ( pefRetrPath->text().strip().length() == 1)
     oneCharManage();

    IString strTempDirectory = pefRetrPath->text().strip();
    IString strTempPath = pefRetrPath->text().strip().upperCase();

   //DWORD rc = GetFileAttributes(strTempPath); //D94556

   //if ( ( ( rc == 0xFFFFFFFF ) || ( ( rc & FILE_ATTRIBUTE_DIRECTORY ) != FILE_ATTRIBUTE_DIRECTORY ) ) ||
   //     ( strTempPath == STR_Slash ) )    //D94556 The string "/" must also be invalid as input

   // D97759. In case strTempPath is the Root Directory but without the Path delimiter,
   //         concatenate it to strTempPath just for validation.
   if ( strTempPath == STR_RootDirectory )
       strTempPath.change(STR_RootDirectory_WOPD, STR_RootDirectory);

   // D97759. Path delimiter at the end of the specification is deleted just for
   // validation to allow showing and storing the path delimiter at the end
   // of the path specification.
   else
   {
      if ((strTempPath.lastIndexOf(STR_PathDelimiter) == strTempPath.length())
             && ( strTempPath.length() > MIN_DIR_LEN ))
        strTempPath=strTempPath.subString(1,strTempPath.length()-1);
   }
   // D97759.  Determines if the specified path is a directory or not
   if (!isValidDirectory(strTempPath) )
//End of Code from release 5.1.16
*/

// Begin code for D98105
#ifdef BR_PROFILE
IBoolean RetrSource :: validRetrievalPath()
{

   //* D94556 Begin logic:  If the lenght of the input character is 1, then validate it
   //* to check if it is the "\\"  character to change it to the default C:\ directory
   if ( pefRetrPath->text().strip().length() == 1)
      {
       oneCharManage();
      }
        IString strTempDirectory = pefRetrPath->text().strip();
        IString strTempPath = pefRetrPath->text().strip().upperCase();


        // D97759. In case strTempPath is the Root Directory but without the Path delimiter,
        //         concatenate it to strTempPath just for validation.
        if ( strTempPath == STR_RootDirectory )
              strTempPath.change(STR_RootDirectory_WOPD, STR_RootDirectory);
        else
        {
           // D97759. Path delimiter at the end of the specification is deleted just for
           // validation to allow showing and storing the path delimiter at the end
           // of the path specification.

                if ((strTempPath.lastIndexOf(STR_PathDelimiter) == strTempPath.length())
                && ( strTempPath.length() > MIN_DIR_LEN ))
                        strTempPath=strTempPath.subString(1,strTempPath.length()-1);
        }

            DWORD rc = GetFileAttributes(strTempPath); //D94556
            if ( ( ( rc == 0xFFFFFFFF ) || ( ( rc & FILE_ATTRIBUTE_DIRECTORY ) != FILE_ATTRIBUTE_DIRECTORY ) ) ||
                // ( strTempPath == STR_Slash ) )    //D94556 The string "/" must also be invalid as input

                // D97759.  Determines if the specified path is a directory or not
                (!isValidDirectory(strTempPath) ))
            {
                //* 79483.  Begin new logic:  If the function validRetrievalPath() was called from
                //* selectDirectory(IString& str), we don't need to display the error message in case that
                //* the specified directory by user doesn't exist because this call is only needed to check
                //* the existence of user's path to change it for the default: <econfig's path>\UserData
                if (!bBrowseDir)  //* 79483
                {
                IMessageBox msg(pMain);
                IMessageBox::Style styleError = IMessageBox::okButton |
                                         IMessageBox::errorIcon |
                                         IMessageBox::moveable;
                msg.setTitle(LOADSTRING(IDS_ErrorRetrievalPath_Title));
                IString sMsg = LOADSTRING(IDS_ErrorRetrievalPath_Text);
                sMsg.change(STR_Substitute0, strTempDirectory);  //RECR
                msg.show(sMsg, styleError);
                }
                //* 79483.  End new logic:  HECC 08/19/2003
                return false;
             }
 return true;
}
#endif

//=======================================================================
// FUNCTION NAME    oneCharManage
//
// DESCRIPTION      Manage the only one char path case. If this only char
//                  is \ then it is changed to c:
//
// D94556           HECC: 09/21/2004
//=======================================================================

#ifdef BR_PROFILE
void RetrSource :: oneCharManage()
{

 IString strTempPath = pefRetrPath->text().strip();

 if (strTempPath == STR_PathDelimiter)
    {
     // The slash is changed to drive C root dir
     // An informational message is displayed
     IString sMsg = LOADSTRING(IDS_ChangedRetrievalPath_Text);

     // Assign the output file name that is changing.
     sMsg.change(STR_Substitute0, strTempPath);

     // Changes the input to the default C: directory
     strTempPath = IString(STR_RootDirectory);
     pefRetrPath->setText(strTempPath);

     sMsg.change(STR_Substitute1, strTempPath);

     //Calls the function that will display the message
     displayMessage(sMsg, STR_Warning, IDS_ChangedRetrievalPath_Title);
    }
}
#endif

//=======================================================================
// FUNCTION NAME    isValidDirectory
//
// DESCRIPTION      Determines if the specified path is a directory or not
//
// D97759           RERC: 02/09/2005
//=======================================================================
Boolean RetrSource :: isValidDirectory(IString sDirectory)
{
     char pszdir[260];

     // Evaluate the presence of the invalid path delimiter first.--
     if ( sDirectory.lastIndexOf(STR_Slash) > 0 )
        return false;

     // Evaluate the presence of the invalid path ThreeBackSlash
     if ( sDirectory.lastIndexOf(STR_ThreeBackSlash) != 0)
        return false;

     strcpy(pszdir,sDirectory);

     struct stat fileStat;
     if (0 == stat(pszdir, &fileStat))
        {
            if (fileStat.st_mode & S_IFDIR)
               return  true;
        }
     return false;
}
// END D97759


//=======================================================================
// FUNCTION NAME    displayMessage
//
// DESCRIPTION      Show the requested messages to the user.
//
// D94556            HECC 09/21/2004
//=======================================================================

#ifdef BR_PROFILE
void RetrSource :: displayMessage(IString sMsg, char cType, int iIDS)
  {

   IMessageBox msg( pMain );
   IMessageBox::Style style = IMessageBox::moveable;

   if (cType == STR_Error)
       style = IMessageBox::errorIcon ;
   else
      if (cType == STR_Warning)
          style = IMessageBox::warningIcon;

   msg.setTitle(IResourceId(iIDS));
   msg.show(sMsg, style);

  }
#endif

//=======================================================================
// FUNCTION NAME    setFocusRetrievalPath
//
// DESCRIPTION      Sets the focus over the retrieval path entry field
//
// DCG 02/25/2002 68568
//=======================================================================
#ifdef BR_PROFILE
void RetrSource :: setFocusRetrievalPath()
{
   pefRetrPath->setFocus();
   pefRetrPath->selectRange();
}
#endif

//=======================================================================
// FUNCTION NAME     setHorzScrollWidth
//
// DESCRIPTION       Sets the horizontal width for the Host interface
//                   application Listbox.  Only needed for Win 95.
//                   Logic from WWP WFCOLUMN.CPP   iak 6/17/98
//=======================================================================
void RetrSource :: setHorzScrollWidth()
   {
   // get plbInterfaces item's max String Length
   ULONG iMaxStringLen=0;

   IListBox::Cursor cursor(*plbInterfaces, IListBox::Cursor::allItems );
   for (cursor.setToFirst(); cursor.isValid(); cursor.setToNext())
      {
      IString sTemp = plbInterfaces->elementAt(cursor);
      sTemp = IString::stripBlanks(sTemp);

      if (iMaxStringLen < sTemp.length() )
         iMaxStringLen=sTemp.length()+2;
      }

   // Get pixel value from max width

   IFont lbFont = plbInterfaces->font();
   ULONG iWidth = iMaxStringLen * lbFont.avgCharWidth();

   // Send message to listbox to set horizontal Scroll Width
   HWND dlgHandle = handle();
   SendDlgItemMessage(
     dlgHandle, IDC_LB_RetrSource_Interfaces, LB_SETHORIZONTALEXTENT,
     (WPARAM)  iWidth,
     (LPARAM)  0
     );

   }

//=======================================================================
// FUNCTION NAME    hideButtons
//
// DESCRIPTION      Hide the action buttons for this dialog.
//                  F62721 EDW 01/21/2002
//=======================================================================

void RetrSource :: hideButtons()
   {
   ppbOK->hide();
   ppbCancel->hide();
   ppbHelp->hide();
   }

//=======================================================================
// FUNCTION NAME    selectDirectory
//
// DESCRIPTION      Open a hobbled file dialog to select a retrieval path
//=======================================================================
#ifdef BR_PROFILE     /* F62721 EDW 01/21/2002 */
void RetrSource :: selectDirectory(IString& str)
   {

   //* 79483.  Begin new logic:  When users press the Browse button to select the Retrieval path,
   //* the added logic will check if the actual directory in the Retrieval path field exists or not.
   //* If it doesn't exist, logic will set the default directory in the Retrieval Path field
   //* to: <econfig's path>\UserData
   bBrowseDir = true;
   if (!validRetrievalPath())
      {

      //* 79483A. Begin new logic: Not always is correctly processed the stripTrailing() function, so
      //* it will be executed separately.
      //* IString strTempPath = pMain->strExecPath.stripTrailing(STR_PathDelimiter);
      IString strTempPath = pMain->strExecPath;
      strTempPath.stripTrailing(STR_PathDelimiter);
      //* 79483A.  End new logic:   HECC - 09/16/2003

      str = strTempPath;
      str.remove( str.lastIndexOf(STR_PathDelimiter) + 1 );
      str = str + "UserData";
      }
   bBrowseDir = false;
   //* 79483 End new logic: HECC - 08/19/2003

   IFileDialog::Settings settings;

   settings.setDialogTemplate( IDD_DirectoryDialog );
   settings.setSaveAsDialog();
   settings.setTitle( IResourceId(IDS_SelectFileTitle) );
   settings.setFileName(str + STR_DummyFile);

   IFileDialog::Style fileStyle = IFileDialog::helpButton;

   IFileDialog* pFileDlg = new IFileDialog(pMain, pMain,
                                           (IFileDialogHandler*)this,
                                           fileStyle, settings);
   pMain->helpWindow()->setAssociatedWindow(pFileDlg);

   IString strPath = pFileDlg->fileName();
   if ( pFileDlg->pressedOK() )
      {
      strPath.remove(strPath.length() - IString(STR_DummyFile).length() + 1);
      pefRetrPath->setText(strPath);
      }

   delete pFileDlg;
   }
#endif

//=======================================================================
// FUNCTION NAME    ~RetrSource
//
// DESCRIPTION      Delete any objects created by the constructor.
//=======================================================================

RetrSource :: ~RetrSource()
   {
   delete plbInterfaces;
   delete pcbHW;
   delete pcbSW;
#ifndef BR_PROFILE    /* F62721 EDW 01/21/2002 */
   delete pcbMismatches;
#endif
   //* delete pcbMRPD;  57146  HECC:  07/24/2001

   delete pcbIE;
   delete pcbNet;
   
   //GTL Req1702 BEGIN:
   delete pcbFir;
   //GTL Req1702 END.
   
   delete ppbOK;
   delete ppbCancel;
   delete ppbHelp;
#ifndef BR_PROFILE    /* F62721 EDW 01/21/2002 */
   delete pcbAutoRec;        //F47698
   delete pcbProcRPO;
#else
   delete pcbFastPath;
   delete prbSortByName;
   delete prbSortByDate;
   delete prbSortByNameA;
   delete prbSortByNameD;
   delete prbSortByDateA;
   delete prbSortByDateD;
   delete ppbBrowseRetrPath;
   delete pefRetrPath;
#endif
   }
//=======================================================================
// FUNCTION NAME    getUserGeography()
//
// DESCRIPTION      Gets get data from "SOFTWARE\\IBM\\econfig\\main"
//                  registry, to evaluate User's geography
//                  REQ 1452.  F108502   HECC: 05/29/2006
// RETURNS          Returns the user_geography(string) from the registry
//                  or a blank if the data is empty.
//
//=======================================================================

IString RetrSource :: getUserGeography()
{
  #define  MY_BUFSIZE 1024

  HKEY hKey;
  TCHAR szUserGeo[MY_BUFSIZE];
  DWORD dwBufLen = MY_BUFSIZE;
  IString sRetValue = STR_Blank2;

  if ( RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGPATH_MAIN,
                        0, KEY_READ, &hKey) == ERROR_SUCCESS )
   if (RegQueryValueEx(hKey, STR_USERGEO,
                        NULL, NULL, (LPBYTE)szUserGeo, &dwBufLen) == ERROR_SUCCESS)
       sRetValue = IString(szUserGeo);
   else
       sRetValue = STR_Blank2;

  RegCloseKey(hKey);
  return sRetValue;
}
