//=======================================================================
// FILE NAME        AF9PTION.CPP
//
// CLASS NAME       OptionsDialog
//
// DESCRIPTION      This class is the options dialog of the Apply
//                  Orders CPF.
// Modification Log
//-----------------------------------------------------------------------
// E. Wilson   01/21/2002   F62721 Added support for use of this class by
//                          the Base Retrieval profile DLL (BF9PROFL.DLL)
// D. Cruz     03/08/2002   68569  Added code to catch TAB and SHIFT+TAB
//                          key presses to return focus to the
//                          ProfileDialog window
// C. Herrera  10/21/2003   D79847 Added necessary logic to catch the F1
//                          key to start the Help windows displaying process
//
//=======================================================================
// Copyright (c) International Business Machines Corp. 1996
// All rights reserved
//=======================================================================

#define INCL_DOSFILEMGR                 // DosFindFirst
#include <os2.h>

#include <icmdhdr.hpp>                  // ICommandHandler
#include <idate.hpp>                    // IDate
#include <iframe.hpp>                   // IFrameWindow
#include <ihelp.hpp>                    // IHelpWindow
#include <ikeyhdr.hpp>                  // IKeyboardHandler
#include <ilistbox.hpp>                 // IListBox
#include <ireslib.hpp>                  // IResourceLibrary/IResourceId
#include <iselhdr.hpp>                  // ISelectHandler
#include <istattxt.hpp>                 // IStaticText
#include <istring.hpp>                  // IString
#include <isysmenu.hpp>                 // ISystemMenu
#include <itime.hpp>                    // ITime
#include <ititle.hpp>                   // ITitle

#include <locale.h>                     // setlocale

#include "af9ption.hpp"                 // OptionsDialog
#ifndef BR_PROFILE  /* F62721 EDW 01/21/2002 */
  #include "af9main.hpp"                // MainWindow
#else
  #include "af9pfdlg.hpp"               // ProfileDialog
#endif
#include "af9profl.hpp"                 // Profile
#include "af9apply.h"                   // Resource ids

//=======================================================================
// FUNCTION NAME    OptionsDialog
//
// DESCRIPTION      Construct an instance of the OptionsDialog class.
//=======================================================================
#ifndef BR_PROFILE  /* F62721 EDW 01/21/2002 */
OptionsDialog :: OptionsDialog(MainWindow* pOwner)
               : IFrameWindow(IResourceId(IDD_Options), NULL, pOwner)
#else
OptionsDialog :: OptionsDialog(ProfileDialog* pOwner)
               : IFrameWindow(IResourceId(IDD_Options), NULL, pOwner->getProfileNotebook())
#endif
   {

   // Save parameters;

   pMain = pOwner;
   pProfile = pOwner->pProfile;
   pOwner->helpWindow()->setAssociatedWindow(this);
   sRestoreLocale = pProfile->strLocale;
#ifndef BR_PROFILE  /* F62721 EDW 01/21/2002 */
   pMain->bLock = true;
#endif

   setId(IDD_Options); //EDS-added 2/5/97

   // Add handlers for command, select, and keyboard messages.

   ICommandHandler::handleEventsFor(this);
   ISelectHandler::handleEventsFor(this);
   IKeyboardHandler::handleEventsFor(this);

   // Create control objects and load with initial values.

   buildControls();
   loadDefaults();

   // Set the dialog to self-destruct when closed.

   setAutoDeleteObject();
   }

//=======================================================================
// FUNCTION NAME    command
//
// DESCRIPTION      Handle command messages.
//=======================================================================

Boolean OptionsDialog :: command(ICommandEvent& cmdEvt)
   {

   switch ( cmdEvt.commandId() )
      {

      // User selected OK.

      case IDC_PB_OK:
         {
#ifndef BR_PROFILE  /* F62721 EDW 01/21/2002 */
         hide();
         removeFromWindowList();
#endif
         IString strLocale = astrLocale[plbLocale->selection()];
         IString strMRI = IString(strLocale).insert(STR_Prefix);
         if ( strLocale != pProfile->strLocale )
            {
            pProfile->strLocale = strLocale;
            pProfile->strMRI = strMRI;
#ifndef BR_PROFILE  /* F62721 EDW 01/21/2002 */
            pMain->postEvent(IWindow::command, UM_SwitchLocale);
#endif
         /* F62721 EDW 01/21/2002 */
            pProfile->bRefreshLocale = true;
            }
         else
            {
            pProfile->bRefreshLocale = false;
            }
         /* F62721 EDW 01/21/2002 */
#ifndef BR_PROFILE  /* F62721 EDW 01/21/2002 */
         pMain->bLock = false;
         pMain->setFocus();
#endif
         break;
         }

      // User selected Cancel.

      case IDC_PB_Cancel:
#ifndef BR_PROFILE  /* F62721 EDW 01/21/2002 */
         hide();
         removeFromWindowList();
#endif
         if ( plbLocale->count() )
            plbLocale->select(usLocale);

#ifndef BR_PROFILE  /* F62721 EDW 01/21/2002 */
         pMain->bLock = false;
         pMain->setFocus();
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

Boolean OptionsDialog :: systemCommand(ICommandEvent& cmdEvt)
   {
#ifndef BR_PROFILE  /* F62721 EDW 01/21/2002 */
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
// FUNCTION NAME    enter
//
// DESCRIPTION      Handle listbox Enter events just like Select events.
//=======================================================================

Boolean OptionsDialog :: enter(IControlEvent& ctlEvt)
   {
   return OptionsDialog::selected(ctlEvt);
   }

//=======================================================================
// FUNCTION NAME    selected
//
// DESCRIPTION      Handle listbox Select events.
//=======================================================================

Boolean OptionsDialog :: selected(IControlEvent& ctlEvt)
   {
   switch ( ctlEvt.controlId() )
      {
      case IDC_LB_Options_Locale:
         {
         char szFormat[FORMAT_LENGTH];
#ifndef BR_PROFILE  /* F62721 EDW 01/21/2002 */
         IString strLocale = astrLocale[plbLocale->selection()];
         if ( setlocale(LC_ALL, strLocale) == NULL)
            setlocale(LC_ALL, STR_DefaultLocale);
#endif
         ptxDate->setText(IDate::today().asString(STR_FormatDate));
         ptxTime->setText(ITime::now().asString(STR_FormatTime));

#ifndef BR_PROFILE  /* F62721 EDW 01/21/2002 */
         if ( setlocale(LC_ALL, sRestoreLocale) == NULL)
            setlocale(LC_ALL, STR_DefaultLocale);
         break;
#endif
         }
      }
   return true;
   }

//=======================================================================
// FUNCTION NAME    virtualKeyPress
//
// DESCRIPTION      Handle the Escape key.
//=======================================================================

Boolean OptionsDialog :: virtualKeyPress(IKeyboardEvent& keyEvt)
    {
#ifndef BR_PROFILE  /* F62721 EDW 01/21/2002 */
    if ( keyEvt.virtualKey() == IKeyboardEvent::esc )
        {
        postEvent(IWindow::systemCommand, ISystemMenu::idClose);
        return true;
        }
#endif

// DCG 03/07/2002 68569 Begin
#ifdef BR_PROFILE
    IKeyboardEvent::VirtualKey keyPressed = keyEvt.virtualKey();

    if ( plbLocale->hasFocus() && keyPressed == IKeyboardEvent::backTab )
       {
       pMain->postEvent(IWindow::command, IDS_ProfileDialog_TabSetFocus);
       return true;
       }
    if ( plbLocale->hasFocus() && keyPressed == IKeyboardEvent::tab )
       {
       pMain->postEvent(IWindow::command, IDS_ProfileDialog_OKSetFocus);
       return true;
       }

   // 79847 Begin: Checks if the 'F1' key was pressed to display the help windows of the
   // Language tab of the Base Retrieval profile preferences.
   if ( keyPressed == IKeyboardEvent::f1 )
      {
      pMain->helpWindow()->show(IResourceId(IDD_Options));
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

void OptionsDialog :: buildControls()
   {
   plbLocale = new IListBox(IDC_LB_Options_Locale, this);
   ptxDate = new IStaticText(IDC_TX_Options_Date, this);
   ptxTime = new IStaticText(IDC_TX_Options_Time, this);

   //EDS--added  Set help button styles 2/5/97
   ppbHelp = new IPushButton(IDC_PB_Help, this);
   ppbHelp->enableHelp();
   ppbHelp->disableMouseClickFocus();
   }

//=======================================================================
// FUNCTION NAME    loadDefaults
//
// DESCRIPTION      Initialize the dialog control values.
//=======================================================================

void OptionsDialog :: loadDefaults()
   {

   // Load the list of available locales.

   usLocale = 0;
   USHORT usIndex = 0;

   // Find the next available MRI DLL.

   IString strMRI = STR_PathMRI;
   strMRI.insert(pMain->strExecPath);
   strMRI += STR_Prefix;
   strMRI += STR_WildCard;
   strMRI += STR_ExtensionDLL;

   FILEFINDBUF3 findBuffer;
   BOOL fRet = TRUE;
   HANDLE hFindFile = FindFirstFile( strMRI, &findBuffer);

   while ( hFindFile != INVALID_HANDLE_VALUE &&
           fRet == TRUE )
         {

         // Strip the path prefix and the DLL extension from the MRI
         // DLL name.

         strMRI = findBuffer.cFileName;
         strMRI.upperCase();
         strMRI.remove(strMRI.lastIndexOf(STR_ExtensionDLL));
         if ( strMRI.length() > DLL_LENGTH )
             strMRI.remove(1, strMRI.length()-DLL_LENGTH);

         // Verify that the matching locale for the current code page is
         // available.

         IString strLocale = IString(strMRI).change(STR_Prefix, "");

         // If there is a matching locale file, add this locale to the listbox.

            astrLocale[usIndex] = strLocale;
            if ( strMRI == pProfile->strMRI )
               usLocale = usIndex;
            strMRI.insert(STR_PathMRI);
            strMRI.insert(pMain->strExecPath);
            IDynamicLinkLibrary dll(strMRI+STR_ExtensionDLL);
            IString sList = LOADSTRING(IDS_LocaleList);
            sList.change(STR_Substitute0, dll.loadString(IDS_Country).strip());
            sList.change(STR_Substitute1, dll.loadString(IDS_Language).strip());
            plbLocale->addAsLast(sList);
            usIndex++;

         // Look for the next MRI DLL in the chain.

         fRet = FindNextFile(hFindFile, &findBuffer);
         }
   FindClose(hFindFile);

   if ( plbLocale->count() )
      plbLocale->select(usLocale);

   }

//=======================================================================
// FUNCTION NAME    hideButtons
//
// DESCRIPTION      Hide the action buttons for this dialog.
//                  F62721 EDW 01/21/2002
//=======================================================================

void OptionsDialog :: hideButtons()
   {
   IPushButton* ppbOK = new IPushButton(IDC_PB_OK, this);
   IPushButton* ppbCancel = new IPushButton(IDC_PB_Cancel, this);

   ppbOK->hide();
   ppbCancel->hide();
   ppbHelp->hide();

   delete ppbOK;
   delete ppbCancel;
   }

//=======================================================================
// FUNCTION NAME    ~OptionsDialog
//
// DESCRIPTION      Delete any objects created by the constructor.
//=======================================================================

OptionsDialog :: ~OptionsDialog()
   {
   delete plbLocale;
   delete ptxDate;
   delete ptxTime;
   delete ppbHelp;
   }
