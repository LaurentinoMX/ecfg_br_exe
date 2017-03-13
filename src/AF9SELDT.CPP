//=======================================================================
// FILE NAME        AF9SELDT.CPP
//
// CLASS NAME       SelectDateDialog
//
// DESCRIPTION      This class is the locale dialog of the Apply Orders
//                  CPF.
//=======================================================================
// Copyright (c) International Business Machines Corp. 1996
// All rights reserved
//=======================================================================

#define INCL_WINMSG
#include <os2.h>

#include <icmdhdr.hpp>                  // ICommandHandler
#include <iframe.hpp>                   // IFrameWindow
//#include <ihelp.hpp>                    // IHelpWindow
#include <ikeyhdr.hpp>                  // IKeyboardHandler
#include <ilistbox.hpp>                 // IListBox
#include <ireslib.hpp>                  // IResourceLibrary/IResourceId
#include <ispinhdr.hpp>                 // ISpinHandler
#include <ispinnum.hpp>                 // ISpinButton, EDS-chg
#include <istring.hpp>                  // IString
#include <isysmenu.hpp>                 // ISystemMenu

#include "af9seldt.hpp"                 // SelectDateDialog
#include "af9ordrs.hpp"                 // OrdersNotebook
#include "af9profl.hpp"                 // Profile
#include "af9apply.h"                   // Resource ids

//=======================================================================
// FUNCTION NAME    SelectDateDialog
//
// DESCRIPTION      Construct an instance of the SelectDateDialog class.
//=======================================================================

SelectDateDialog :: SelectDateDialog(OrdersNotebook* pOwner)
                  : IFrameWindow(IResourceId(IDD_SelectDate), NULL, pOwner)
   {

   // Save parameters;
   pNotebook = pOwner;
   pProfile = pNotebook->pProfile;

   // Attach the help window.
   pNotebook->pMain->helpWindow()->setAssociatedWindow(this);

   // Add handlers for command, keyboard, and spinbutton events.
   ICommandHandler::handleEventsFor(this);
   IKeyboardHandler::handleEventsFor(this);
   ISpinHandler::handleEventsFor(this);

   // Create control objects and load with initial values.
   buildControls();
   loadDefaults();

   // Set the dialog to self-destruct when closed.
   setAutoDeleteObject();
   }

//=======================================================================
// FUNCTION NAME    command
//
// DESCRIPTION      Handle command events.
//=======================================================================

Boolean SelectDateDialog :: command(ICommandEvent& event)
   {

   switch ( event.commandId() )
      {

      // User selected OK.

      case IDC_PB_OK:
         {
         hide();
         pProfile->dateSelect = IDate(psbDay->value(),
                                      (IDate::Month)psbMonth->value(),
                                      psbYear->value());
         pNotebook->postEvent(IWindow::command, UM_SelectDate);
         removeFromWindowList();
         break;
         }

      // User selected Cancel.

      case IDC_PB_Cancel:
         hide();
         removeFromWindowList();
         break;

      default:
         return false;
      }
   return true;
   }

//=======================================================================
// FUNCTION NAME    systemCommand
//
// DESCRIPTION      Handle system commands.
//=======================================================================

Boolean SelectDateDialog :: systemCommand(ICommandEvent& cmdEvt)
   {
   if ( cmdEvt.commandId() != ISystemMenu::idClose )
      return false;
   postEvent(IWindow::command, IDC_PB_Cancel);
   return true;
   }

//=======================================================================
// FUNCTION NAME    virtualKeyPress
//
// DESCRIPTION      Handle the Escape key.
//=======================================================================

Boolean SelectDateDialog :: virtualKeyPress(IKeyboardEvent& event)
   {
   if ( event.virtualKey() == IKeyboardEvent::esc )
      {
      postEvent(IWindow::command, IDC_PB_Cancel);
      return true;
      }
   return false;
   }

//=======================================================================
// FUNCTION NAME    spinEnded
//
// DESCRIPTION      Handle spinbutton events.
//=======================================================================

Boolean SelectDateDialog :: spinEnded(IControlEvent& event)
   {
   psbDay->setRange(IRange(1, IDate::daysInMonth((IDate::Month)psbMonth->value(), psbYear->value())));
   return false;
   }

//=======================================================================
// FUNCTION NAME    buildControls
//
// DESCRIPTION      Instantiate the various dialog controls.
//=======================================================================

void SelectDateDialog :: buildControls()
   {
   INumericSpinButton::Style style = //INumericSpinButton::pmCompatible
                                      INumericSpinButton::classDefaultStyle
                                     | IControl::tabStop;

   psbYear = new INumericSpinButton( IDC_SB_SelectDate_Year, this, this, IRectangle( 30, 70, 100, 94), style);
   psbMonth = new INumericSpinButton(IDC_SB_SelectDate_Month, this, this, IRectangle( 142, 70, 212, 94 ), style);
   psbDay = new INumericSpinButton(IDC_SB_SelectDate_Day, this, this, IRectangle( 262, 70, 332, 94), style);
   psbYear->disableDataUpdate();
   psbMonth->disableDataUpdate();
   psbDay->disableDataUpdate();

   //EDS--added  Set help button styles 2/13/97
   ppbHelp = new IPushButton(IDC_PB_Help, this);
   ppbHelp->enableHelp();
   ppbHelp->disableMouseClickFocus();
   }

//=======================================================================
// FUNCTION NAME    loadDefaults
//
// DESCRIPTION      Initialize the dialog control values.
//=======================================================================

void SelectDateDialog :: loadDefaults()
   {
   IDate today;
   psbYear->setRange(IRange(today.year()-1, today.year()+5));
   psbMonth->setRange(IRange(1, 12));
   psbDay->setRange(IRange(1, 31));
   psbYear->spinTo(today.year());
   psbMonth->spinTo((LONG)today.monthOfYear());
   psbDay->spinTo(today.dayOfMonth());
   }

//=======================================================================
// FUNCTION NAME    ~SelectDateDialog
//
// DESCRIPTION      Delete any objects created by the constructor.
//=======================================================================

SelectDateDialog :: ~SelectDateDialog()
   {
   delete psbYear;
   delete psbMonth;
   delete psbDay;
   delete ppbHelp;
   }
