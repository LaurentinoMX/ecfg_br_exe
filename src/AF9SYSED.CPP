//=======================================================================
// FILE NAME        AF9SYSED.CPP
//
// CLASS NAME       SystemEdit
//
// DESCRIPTION      This class is the new/move/rename system dialog of
//                  the Apply Orders CPF.
//=======================================================================
// Copyright (c) International Business Machines Corp. 1996
// All rights reserved
//=======================================================================

#include <icmdhdr.hpp>                  // ICommandHandler
#include <iedithdr.hpp>                 // IEditHandler
#include <ientryfd.hpp>                 // IEntryField
#include <iframe.hpp>                   // IFrameWindow
#include <ihelp.hpp>                    // IHelpWindow
#include <ikeyhdr.hpp>                  // IKeyboardHandler
#include <ilistbox.hpp>                 // IListBox
#include <ipushbut.hpp>                 // IPushButton
#include <iradiobt.hpp>                 // IRadioButton
#include <ireslib.hpp>                  // IResourceLibrary/IResourceId
#include <istring.hpp>                  // IString
#include <isysmenu.hpp>                 // ISystemMenu
#include <ititle.hpp>                   // ITitle

#include "af9sysed.hpp"                 // SystemEdit
#include "af9systm.hpp"                 // SystemsNotebook
#include "af9apply.h"                   // Resource ids

//=======================================================================
// FUNCTION NAME    SystemEdit
//
// DESCRIPTION      Construct an instance of the SystemEdit class.
//=======================================================================

SystemEdit :: SystemEdit(SystemsNotebook* pOwner)
            : IFrameWindow(IResourceId(IDD_SystemEdit), NULL, pOwner)
   {

   // Save parameters;

   pNotebook = pOwner;

   // Attach the help window.

   pNotebook->pMain->helpWindow()->setAssociatedWindow(this);

   // Add handlers for command, keyboard, and edit messages.

   ICommandHandler::handleEventsFor(this);
   IKeyboardHandler::handleEventsFor(this);
   IEditHandler::handleEventsFor(this);

   // Create control objects.

   buildControls();

   // Set the dialog to self-destruct when closed.

   setAutoDeleteObject();
   }

//=======================================================================
// FUNCTION NAME    command
//
// DESCRIPTION      Handle command messages.
//=======================================================================

Boolean SystemEdit :: command(ICommandEvent& cmdEvt)
   {

   switch ( cmdEvt.commandId() )
      {

      // User selected OK.

      case IDC_PB_OK:
         {
         hide();
         USHORT usNewPage = 1;
         if ( plbPlacement->count() )
            if ( prbBefore->isSelected() )
               usNewPage = plbPlacement->selection() + 1;
            else
               usNewPage = plbPlacement->selection() + 2;
         IString strText = pefName->text();
         switch ( usMode )
            {
            case enumNew:
               pNotebook->addPage(usNewPage, strText);
               break;
            case enumSettings:
               pNotebook->setPage(usPage, usNewPage, strText);
               break;
            }
         pNotebook->bLock = false;
         pNotebook->setFocus();
         break;
         }

      // User selected Cancel.

      case IDC_PB_Cancel:
         hide();
         removeFromWindowList();
         pNotebook->bLock = false;
         pNotebook->setFocus();
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

Boolean SystemEdit :: systemCommand(ICommandEvent& cmdEvt)
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

Boolean SystemEdit :: virtualKeyPress(IKeyboardEvent& keyEvt)
   {
   if ( keyEvt.virtualKey() == IKeyboardEvent::esc )
      {
      postEvent(IWindow::systemCommand, ISystemMenu::idClose);
      return true;
      }
   return false;
   }

//=======================================================================
// FUNCTION NAME    edit
//
// DESCRIPTION      Handle entry field edit events.
//=======================================================================

Boolean SystemEdit :: edit(IControlEvent& ctlEvt)
   {
   if ( pefName->isEmpty() )
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
// FUNCTION NAME    buildControls
//
// DESCRIPTION      Instantiate the various dialog controls.
//=======================================================================

void SystemEdit :: buildControls()
   {
   pefName = new IEntryField(IDC_EF_SystemEdit_Name, this);
   pefName->setLimit(40);
   prbBefore = new IRadioButton(IDC_RB_SystemEdit_Before, this);
   prbAfter = new IRadioButton(IDC_RB_SystemEdit_After, this);
   plbPlacement = new IListBox(IDC_LB_SystemEdit_Placement, this);
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

void SystemEdit :: loadDefaults(USHORT usMode)
   {
   pNotebook->bLock = true;
   usPage = pNotebook->pageNumber();
   this->usMode = usMode;
   if ( usMode == enumNew )
      {
      ITitle title(this, LOADSTRING(IDS_SystemEditNew));
      pefName->setText("");
      }
   else
      ITitle title(this, LOADSTRING(IDS_SystemEditSettings));
   if ( plbPlacement->count() )
      plbPlacement->removeAll();
   for ( USHORT usIndex = 1; usIndex < pNotebook->usSystems; usIndex++ )
      if ( (usMode == enumNew) || (usIndex != usPage) )
         plbPlacement->addAsLast(pNotebook->systemName(usIndex));
      else
         pefName->setText(pNotebook->systemName(usIndex));
   if ( plbPlacement->count() )
      {
      prbBefore->enable();
      prbAfter->enable();
      plbPlacement->enable();
      if ( usMode == enumSettings )
         if ( usPage > 1 )
            {
            plbPlacement->select(usPage-2);
            prbAfter->select();
            }
         else
            {
            plbPlacement->select(0);
            prbBefore->select();
            }
      else
         {
         plbPlacement->select(plbPlacement->count()-1);
         prbAfter->select();
         }
      }
   else
      {
      prbBefore->disable();
      prbAfter->disable();
      prbBefore->deselect();
      prbAfter->deselect();
      plbPlacement->disable();
      }
   pefName->setFocus();
   }

//=======================================================================
// FUNCTION NAME    ~SystemEdit
//
// DESCRIPTION      Delete any objects created by the constructor.
//=======================================================================

SystemEdit :: ~SystemEdit()
   {
   delete pefName;
   delete prbBefore;
   delete prbAfter;
   delete plbPlacement;
   delete ppbOK;
   delete ppbCancel;
   delete ppbHelp;
   }
