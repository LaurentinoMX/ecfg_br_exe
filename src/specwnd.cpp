//=======================================================================
// FILE NAME        SPECWND.CPP
//
// CLASS NAME       Special Window for Messages
//
// DESCRIPTION      
//
// Programmer:      Miguel Angel Martínez Tlahuel 47973
//=======================================================================
// Copyright (c) International Business Machines Corp. 1998
// All rights reserved
//=======================================================================

#include <icmdhdr.hpp>                  // ICommandHandler
#include <iframe.hpp>                   // IFrameWindow
#include <ihelp.hpp>                    // IHelpWindow
#include <ikeyhdr.hpp>                  // IKeyboardHandler
#include <ipushbut.hpp>                 // IPushButton
#include <isysmenu.hpp>                 // ISystemMenu

#include "specwnd.hpp"			    // Special window
#include "mainw.hpp"			    // XWindow
#include "af9apply.h"			    // Resource Id´s

//=======================================================================
// FUNCTION NAME    SpecWindow
//
// DESCRIPTION      Construct an instance of the SpecWindow class.
//=======================================================================
SpecWindow :: SpecWindow(XWindow* pOwner)
            : IFrameWindow(IResourceId(IDD_SpecWindow), NULL)
{
   bContinue=false;
   pXWinOwn=pOwner;
   
   pOwner->helpWindow()->setAssociatedWindow(this);
   
   ICommandHandler::handleEventsFor(this);
   IKeyboardHandler::handleEventsFor(this);
   
   buildControls();
//   setAutoDeleteObject();
}

//=======================================================================
// FUNCTION NAME    command
//
// DESCRIPTION      Handle command messages.
//=======================================================================

Boolean SpecWindow::command(ICommandEvent& cmdEvt) {
   switch ( cmdEvt.commandId() ) {
      case IDC_PB_OK:
         bContinue=true;
         close();
         break;
      case IDC_PB_Cancel:
         bContinue=false;
         close();
         break;
      default:
         bContinue=false;
         close();
         return false;
   }
   return true;
}

//=======================================================================
// FUNCTION NAME    systemCommand
//
// DESCRIPTION      Handle system command events.
//=======================================================================

Boolean SpecWindow :: systemCommand(ICommandEvent& cmdEvt) {
   if ( cmdEvt.commandId() == ISystemMenu::idClose ) {
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

Boolean SpecWindow :: virtualKeyPress(IKeyboardEvent& keyEvt) {
   if ( keyEvt.virtualKey() == IKeyboardEvent::esc ) {
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

void SpecWindow::buildControls() {
   
  // pstrMsg = new IStaticText( IDC_SpecWindow_Text, this);
  // IString strMsg = LOADSTRING( IDS_SpecWindow_Text);
  // pstrMsg->setText(strMsg);
   
   ppbOk = new IPushButton(IDC_PB_OK, this);
   ppbCancel = new IPushButton(IDC_PB_Cancel, this);      
   ppbHelp = new IPushButton(IDC_PB_Help, this);
   ppbHelp->enableHelp();
   ppbHelp->disableMouseClickFocus();
}

//=======================================================================
// FUNCTION NAME    ~SpecWindow
//
// DESCRIPTION      Delete any objects created by the constructor (destructor)
//=======================================================================

SpecWindow::~SpecWindow() {
   delete ppbOk;
   delete ppbCancel;
   delete ppbHelp;
}