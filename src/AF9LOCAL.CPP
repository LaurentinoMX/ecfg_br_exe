//=======================================================================
// FILE NAME        AF9LOCAL.CPP
//
// CLASS NAME       LocaleDialog
//
// DESCRIPTION      This class is the locale dialog of the Apply Orders
//                  CPF.
//=======================================================================
// Copyright (c) International Business Machines Corp. 1996
// All rights reserved
//=======================================================================

#define INCL_DOSFILEMGR                 // DosFindFirst
#include <os2.h>

#include <time.h>                       // time

#include <icmdhdr.hpp>                  // ICommandHandler
#include <iframe.hpp>                   // IFrameWindow
#include <ikeyhdr.hpp>                  // IKeyboardHandler
#include <ilistbox.hpp>                 // IListBox
#include <imsgbox.hpp>                  // IMessageBox
#include <ireslib.hpp>                  // IResourceLibrary/IResourceId
#include <istring.hpp>                  // IString
#include <isysmenu.hpp>                 // ISystemMenu

#include <locale.h>                     // setlocale

#include "af9local.hpp"                 // LocaleDialog
#include "af9main.hpp"                  // MainWindow
#include "af9profl.hpp"                 // Profile
#include "af9apply.h"                   // Resource ids

//=======================================================================
// FUNCTION NAME    LocaleDialog
//
// DESCRIPTION      Construct an instance of the LocaleDialog class.
//=======================================================================

LocaleDialog :: LocaleDialog(MainWindow* pOwner)
              : IFrameWindow(IResourceId(IDD_Options), NULL, pOwner)
   {

   // Save parameters;

   pMain = pOwner;
   pProfile = pOwner->pProfile;

   // Add handlers for command, select, and keyboard messages.

   ICommandHandler::handleEventsFor(this);
   IKeyboardHandler::handleEventsFor(this);

   // Center dialog on desktop.

   ISize dialogSize = size();
   ISize desktopSize = IWindow::desktopWindow()->size();
   IPoint pt(desktopSize.width() - dialogSize.width()/2,
             desktopSize.height() - dialogSize.height()/2);
   moveTo(pt);

   // Create control objects and load with initial values.

   buildControls();
   loadDefaults();
   }

//=======================================================================
// FUNCTION NAME    command
//
// DESCRIPTION      Handle command messages.
//=======================================================================

Boolean LocaleDialog :: command(ICommandEvent& cmdEvt)
   {

   switch ( cmdEvt.commandId() )
      {

      // User selected OK.

      case IDC_PB_OK:
         {
         hide();
         IString strLocale = astrLocale[plbLocale->selection()];
         IString strMRI = IString(strLocale).insert(STR_Prefix);
         pProfile->strLocale = strLocale;
         pProfile->strMRI = strMRI;
         dismiss(1);
         break;
         }

      // User selected Cancel.

      case IDC_PB_Cancel:
         dismiss(0);
         break;

      default:
         return false;
      }
   return true;
   }

//=======================================================================
// FUNCTION NAME    virtualKeyPress
//
// DESCRIPTION      Handle the Escape key.
//=======================================================================

Boolean LocaleDialog :: virtualKeyPress(IKeyboardEvent& keyEvt)
    {
    if ( keyEvt.virtualKey() == IKeyboardEvent::esc )
        {
        postEvent(IWindow::command, IDC_PB_Cancel);
        return true;
        }
    return false;
    }

//=======================================================================
// FUNCTION NAME    buildControls
//
// DESCRIPTION      Instantiate the various dialog controls.
//=======================================================================

void LocaleDialog :: buildControls()
   {
   plbLocale = new IListBox(IDC_LB_Options_Locale, this);
   }

//=======================================================================
// FUNCTION NAME    loadDefaults
//
// DESCRIPTION      Initialize the dialog control values.
//=======================================================================

void LocaleDialog :: loadDefaults()
   {

   // Load the list of available locales.

   USHORT usLocale = 0;
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
            IString sList = "%0 - %1";
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
   else
      {
      IMessageBox::Style style = IMessageBox::cancelButton |
                                 IMessageBox::errorIcon |
                                 IMessageBox::moveable |
					IMessageBox::systemModal;
      IMessageBox msgBox(this);
      msgBox.setTitle(STR_ErrorLocaleTitle);
      msgBox.show(STR_ErrorLocaleText, style);
      postEvent(IWindow::command, IDC_PB_Cancel);
      }
   }

//=======================================================================
// FUNCTION NAME    ~LocaleDialog
//
// DESCRIPTION      Delete any objects created by the constructor.
//=======================================================================

LocaleDialog :: ~LocaleDialog()
   {
   delete plbLocale;
   }
