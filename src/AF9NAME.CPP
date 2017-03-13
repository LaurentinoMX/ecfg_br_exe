//=======================================================================
// FILE NAME        AF9NAME.CPP
//
// CLASS NAME       FileNameDialog
//
// DESCRIPTION      This class is the file name entry dialog of
//                  the Apply Orders CPF.
//=======================================================================
// Copyright (c) International Business Machines Corp. 1996
// All rights reserved
//=======================================================================

#define INCL_WINMSG                     // WM_USER
#include <os2.h>

#include <stdio.h>                      // fopen
#include <sys\types.h>                  // stat
#include <sys\stat.h>                   // stat

#include <icmdhdr.hpp>                  // ICommandHandler
#include <iedithdr.hpp>                 // IEditHandler
#include <ientryfd.hpp>                 // IEntryField
#include <iframe.hpp>                   // IFrameWindow
#include <ihelp.hpp>                    // IHelpWindow
#include <ikeyhdr.hpp>                  // IKeyboardHandler
#include <imsgbox.hpp>                  // IMessageBox
#include <ipushbut.hpp>                 // IPushButton
#include <ireslib.hpp>                  // IResourceLibrary/IResourceId
#include <istring.hpp>                  // IString
#include <isysmenu.hpp>                 // ISystemMenu
#include <ititle.hpp>                   // ITitle
#include "af9profl.hpp"
#include "af9name.hpp"                  // FileNameDialog
#include "af9main.hpp"                  // MainWindow
#include "afcbkend.hpp"                 // BackEnd
#include "af9apply.h"                   // Resource ids

//=======================================================================
// FUNCTION NAME    FileNameDialog
//
// DESCRIPTION      Construct an instance of the FileNameDialog class.
//=======================================================================

FileNameDialog :: FileNameDialog(MainWindow* pOwner)
                : IFrameWindow(IResourceId(IDD_FileName), NULL, pOwner)
   {

   // Save parameters;

   pMain = pOwner;

   // Attach the help window.

   pMain->helpWindow()->setAssociatedWindow(this);

   // Create control objects.

   buildControls();

   // Add handlers for command, keyboard, and edit messages.

   ICommandHandler::handleEventsFor(this);
   IKeyboardHandler::handleEventsFor(this);
   IEditHandler::handleEventsFor(pefName);

   // Set the dialog to self-destruct when closed.

   setAutoDeleteObject();
   }

//=======================================================================
// FUNCTION NAME    command
//
// DESCRIPTION      Handle command messages.
//=======================================================================

Boolean FileNameDialog :: command(ICommandEvent& cmdEvt)
   {

   switch ( cmdEvt.commandId() )
      {

      // User selected OK.

      case IDC_PB_OK:
         {
         if ( !confirmReplace() )
            break;
         if ( !confirmValid() )
            break;
         hide();
         removeFromWindowList();
         switch ( usMode )
            {
            case MainWindow::enumFileSave:
               pMain->strSaveName = pefName->text();
               pMain->strGenerateName = pefName->text();   //47705
               pMain->postEvent(IWindow::command, UM_SaveFile);
               pMain->postEvent(IWindow::command, UM_GenerateFile);
               break;
            case MainWindow::enumFileGenerate:
               pMain->strGenerateName = pefName->text();
               pMain->postEvent(IWindow::command, UM_GenerateFile);
               break;
            case MainWindow::enumFileName:                    //edw 12/5/97
               pMain->strSaveName = pefName->text();
               pMain->strGenerateName = pefName->text();      //47705
               pMain->postEvent(IWindow::command, UM_RenameFile);
               if (!pMain->pProfile->bFastPath)         //38935 flw 10/1/99
                  pMain->postEvent(IWindow::command, IDMI_SelectedOpen);
               break;

            // Begin new logic 52012 MAMT - 28/07/2000               
            case MainWindow::enumFileName1: 
               {                    
               pMain->strSaveName = pefName->text();
//               pMain->strGenerateName = pefName->text();      //47705
//               pMain->postEvent(IWindow::command, UM_RenameFile);
               pMain->postEvent(IWindow::command, IDMI_SelectedOpen1);
               break;
            // End new logic 52012 MAMT - 28/07/2000               
               } 
            case MainWindow::enumFileRename:
               pMain->strSaveName = pefName->text();
               //pMain->strGenerateName = pefName->text();     //47705
               pMain->postEvent(IWindow::command, UM_RenameFile);
               //pMain->postEvent(IWindow::command, UM_GenerateFile);
               break;
            }
         pMain->bLock = false;
         pMain->pcnrFiles->enable();
         pMain->setFocus();
         break;
         }

      // User selected Cancel.

      case IDC_PB_Cancel:
         hide();
         removeFromWindowList();
         pMain->bLock = false;
         pMain->pcnrFiles->enable();
         pMain->setFocus();
         if ( usMode == MainWindow::enumFileName )                   //edw01/20/98
            pMain->postEvent(IWindow::command, IDMI_SelectedOpen);   //edw01/20/98
         if ( usMode == MainWindow::enumFileName1 )                  //52012 MAMT 15/08/2000
            pMain->postEvent(IWindow::command, IDMI_CloseFile);      //52012 MAMT 15/08/2000
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

Boolean FileNameDialog :: systemCommand(ICommandEvent& cmdEvt)
   {

   // Hide this window instead of closing it.

   if ( cmdEvt.commandId() == ISystemMenu::idClose )
      {
      postEvent(IWindow::command, IDC_PB_Cancel);
      return true;
      }
   return false;
   }

//=======================================================================
// FUNCTION NAME    virtualKeyPress
//
// DESCRIPTION      Handle the Escape key.
//=======================================================================

Boolean FileNameDialog :: virtualKeyPress(IKeyboardEvent& keyEvt)
   {
   if ( keyEvt.virtualKey() == IKeyboardEvent::esc )
      {
      postEvent(IWindow::command, IDC_PB_Cancel);
      return true;
      }
   return false;
   }

//=======================================================================
// FUNCTION NAME    edit
//
// DESCRIPTION      Handle entry field edit events.
//=======================================================================

Boolean FileNameDialog :: edit(IControlEvent& ctlEvt)
   {
   Boolean bOK = true;
   // if ( pefName->isEmpty() ) // chaged MSN 2/13/97 to check for blanks
   if ( pefName->text().strip() == "" )
      bOK = false;
   else
      if ( (usMode == MainWindow::enumFileRename) || (usMode == MainWindow::enumFileName) )
         if ( IString(pefName->text()).upperCase() == IString(pMain->strSaveName).upperCase() )
            bOK = false;
   if ( !bOK )
      {
      ppbOK->disable();
      ppbCancel->enableTabStop();
      }
   else
      {
      ppbOK->enable();
      ppbCancel->disableTabStop();
      }
   return false;
   }

//=======================================================================
// FUNCTION NAME    confirmReplace
//
// DESCRIPTION      Avoid writing over an existing file.
//=======================================================================

Boolean FileNameDialog :: confirmReplace()
   {
   struct stat fstat;
   IString strName;

   switch ( usMode )
      {
      case MainWindow::enumFileSave:
      case MainWindow::enumFileName:                          //edw 12/5/97
      case MainWindow::enumFileName1:                         //49222 MAMT 08/01/2000
      case MainWindow::enumFileRename:
         strName = pMain->strBrowsePath + STR_PathDelimiter +
                   pefName->text() + STR_ExtensionBIR;
         break;
      case MainWindow::enumFileGenerate:
         strName = pMain->strGeneratePath + STR_PathDelimiter +
                   pefName->text() + STR_ExtensionRET;
         break;
      }
   if ( stat(strName, &fstat) )
      return true;
   IMessageBox msg(this);
   IMessageBox::Style style = IMessageBox::yesNoButton |
                              IMessageBox::defButton2 |
                              IMessageBox::warningIcon |
                              IMessageBox::moveable |
					IMessageBox::systemModal;
   msg.setTitle(IResourceId(IDS_ReplaceFile_Title));
   IString strText = LOADSTRING(IDS_ReplaceFile_Text);
   strText.change(STR_Substitute0, strName);
   IMessageBox::Response response = msg.show(strText, style,
                                             IDH_ReplaceFile);
   return ( response == IMessageBox::yes );
   }

//=======================================================================
// FUNCTION NAME    confirmValid
//
// DESCRIPTION      Guarantee a legitimate new file name.
//=======================================================================

Boolean FileNameDialog :: confirmValid()
   {
   IString strName;
   Boolean bValid = true;

   switch ( usMode )
      {
      case MainWindow::enumFileName:                          //edw 12/5/97
      case MainWindow::enumFileName1:                         //49222 MAMT 08/01/2000
      case MainWindow::enumFileRename:
         strName = pMain->strBrowsePath + STR_PathDelimiter +
                   pefName->text() + STR_ExtensionBIR;
         if ( pMain->pFile )
            if ( IString(pefName->text()).upperCase() == IString(pMain->pFile->name()).upperCase() )
               bValid = false;
         break;
      case MainWindow::enumFileSave:
         strName = pMain->strBrowsePath + STR_PathDelimiter +
                   pefName->text() + STR_ExtensionBIR;
         if ( IString(pefName->text()).upperCase() == IString(pMain->pFile->name()).upperCase() )
            return true;
         break;
      case MainWindow::enumFileGenerate:
         strName = pMain->strGeneratePath + STR_PathDelimiter +
                   pefName->text() + STR_ExtensionRET;
         break;
      }
   if ( bValid )
      {
      FILE* pFile = fopen(strName, "w");
      if ( pFile )
         {
         fclose(pFile);
         remove(strName);
         }
      else
         bValid = false;
      }
   if ( bValid )
      return true;
   IMessageBox msg(this);
   IMessageBox::Style style = IMessageBox::cancelButton |
                              IMessageBox::warningIcon |
                              IMessageBox::moveable |
					IMessageBox::systemModal;
   msg.setTitle(IResourceId(IDS_ErrorWriteFile_Title));
   IString strText = LOADSTRING(IDS_ErrorWriteFile_Text);
   strText.change(STR_Substitute0, strName);
   msg.show(strText, style, IDH_ErrorWriteFile);
   return false;
   }

//=======================================================================
// FUNCTION NAME    buildControls
//
// DESCRIPTION      Instantiate the various dialog controls.
//=======================================================================

void FileNameDialog :: buildControls()
   {
   pefName = new IEntryField(IDC_EF_FileName_Name, this);
   pefName->setLimit(15);
   ppbOK = new IPushButton(IDC_PB_OK, this);
   ppbCancel = new IPushButton(IDC_PB_Cancel, this);

   //MSN--added  Set help button styles 2/5/97
   ppbHelp = new IPushButton(IDC_PB_Help, this);
   ppbHelp->enableHelp();
   ppbHelp->disableMouseClickFocus();
   }

//=======================================================================
// FUNCTION NAME    loadDefaults
//
// DESCRIPTION      Initialize the dialog control values.
//=======================================================================

void FileNameDialog :: loadDefaults(USHORT usMode)
   {
   Boolean bSave = false;
   this->usMode = usMode;
   switch ( usMode )
      {
      case MainWindow::enumFileSave:
         {
         ITitle title(this, LOADSTRING(IDS_FileNameSave));
         pefName->setText(pMain->strSaveName);
         ppbOK->enable();
         ppbCancel->disableTabStop();
         break;
         }
      case MainWindow::enumFileGenerate:
         {
         ITitle title(this, LOADSTRING(IDS_FileNameGenerate));
         pefName->setText(pMain->strGenerateName);
         ppbOK->enable();
         ppbCancel->disableTabStop();
         if (pMain->pProfile->bFastPath)
            ppbOK->click();
         break;
         }
      //edw 12/5/97
      case MainWindow::enumFileName:
         {
         ITitle title(this, LOADSTRING(IDS_FileNameName));
         pefName->setText(pMain->strSaveName);
         ppbOK->disable();
         ppbCancel->enableTabStop();
         //if (pMain->pProfile->bFastPath && pMain->pProfile->bEconfig)   38935 flw    erg comtd
         // ppbCancel->click();                        erg com

         //if (pMain->pProfile->bFastPath && pMain->pProfile->bEconfig)   // erg
          //{
         pMain->pFileName->hide();                                          // erg
         ppbCancel->click();                              // erg
          //}
         /*else if (pMain->pProfile->bFastPath && !pMain->pProfile->bEconfig)   // erg
           pMain->pFileName->show();   // erg
         else if (!pMain->pProfile->bFastPath && !pMain->pProfile->bEconfig)
           pMain->pFileName->show();
         else if (!pMain->pProfile->bFastPath && pMain->pProfile->bEconfig)
            pMain->pFileName->show();
           */    //All recommented for 47705 - flw
         bSave = true;
         break;
         }
      case MainWindow::enumFileName1:  // Begin new logic 52012 MAMT - 28/07/2000
         {
         ITitle title(this, LOADSTRING(IDS_FileNameRename));
         pefName->setText(pMain->strSaveName);
         ppbOK->disable();
         ppbCancel->enableTabStop();
         break;
         }                             // End new logic 52012 MAMT - 28/07/2000
      case MainWindow::enumFileRename:
         {
         ITitle title(this, LOADSTRING(IDS_FileNameRename));
         pefName->setText(pMain->strSaveName);
         ppbOK->disable();
         ppbCancel->enableTabStop();
         break;
         }
      }
   pefName->setFocus();
   pefName->selectRange();

  // if (pMain->pProfile->bFastPath)       //38935 flw     erg 46463
  //     pefName->hide();                                  erg 46463

   // Disable the files container to ensure that the selected file
   // does not change.

   pMain->bLock = true;
   pMain->pcnrFiles->disable();
   if (!bSave)
      pMain->pFileName->show(); // erg  46483
   }

//=======================================================================
// FUNCTION NAME    ~FileNameDialog
//
// DESCRIPTION      Delete any objects created by the constructor.
//=======================================================================

FileNameDialog :: ~FileNameDialog()
   {
   delete pefName;
   delete ppbOK;
   delete ppbCancel;
   delete ppbHelp;
   }