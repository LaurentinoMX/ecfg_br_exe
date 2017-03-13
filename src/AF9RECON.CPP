//=======================================================================
// FILE NAME        AF9RECON.CPP
//
// CLASS NAME       ReconcileData
//
// DESCRIPTION      This class is the "Data retrieved from multiple sources"
//                  dialog of the Base Retrieval CPF.
//
// Programmer:      Erica Wilson  06/22/1998
//=======================================================================
// Copyright (c) International Business Machines Corp. 1998
// All rights reserved
//=======================================================================

#include <icmdhdr.hpp>                  // ICommandHandler
#include <iframe.hpp>                   // IFrameWindow
#include <ihelp.hpp>                    // IHelpWindow
#include <ikeyhdr.hpp>                  // IKeyboardHandler
#include <istring.hpp>                  // IString
#include <istattxt.hpp>                 // IStaticText
#include <ipushbut.hpp>                 // IPushButton
#include <iradiobt.hpp>                 // IRadioButton
#include <isysmenu.hpp>                 // ISystemMenu

#include "af9profl.hpp"                 // Profile         //47698
#include "af9recon.hpp"                 // ReconcileData
#include "af9main.hpp"                  // MainWindow
#include "af9apply.h"                   // Resource ids

//=======================================================================
// FUNCTION NAME    ReconcileData
//
// DESCRIPTION      Construct an instance of the ReconcileData class.
//=======================================================================

ReconcileData :: ReconcileData(MainWindow* pOwner, IString strDate )
            : IFrameWindow(IResourceId(IDD_ReconcileData), NULL, pOwner)
   {

   // Save parameters;

   pMain = pOwner;
   strSensedDate = strDate;

   // Attach the help window.

   pOwner->helpWindow()->setAssociatedWindow(this);

   // Add handlers for command and keyboard messages.

   ICommandHandler::handleEventsFor(this);
   IKeyboardHandler::handleEventsFor(this);

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

Boolean ReconcileData :: command(ICommandEvent& cmdEvt)
   {

   switch ( cmdEvt.commandId() )
      {

      // User selected OK.

      case IDC_PB_OK:
         {
         if ( prbResolve->isSelected() )
            {
            pMain->bResolve = true;
            pMain->bUseMRDB = false;
            pMain->bUseAAS = false;
            pMain->addUserChoice( (IString)STR_REC05_ReconcileData );    //*hecc: 58629  04/04/2001 
            pMain->addUserChoice( (IString)STR_REC05_ReconcileData2 );   //*hecc: 58629  04/04/2001 
            }
         else if( prbMRDB->isSelected() )
            {
            pMain->bResolve = false;
            pMain->bUseMRDB = true;
            pMain->bUseAAS = false;
            pMain->addUserChoice( (IString)STR_REC05_ReconcileUseMRDB1 ); //edw12/29/1999
            }
         else if ( prbAAS->isSelected() )
            {
            pMain->bResolve = false;
            pMain->bUseMRDB = false;
            pMain->bUseAAS =  true;
            pMain->addUserChoice( (IString)STR_REC05_ReconcileUseAAS1 );  //edw12/29/1999
            }
         close();
         //pMain->bLock = false;
         //pMain->setFocus();
         break;
         }

      // User selected Cancel.

      case IDC_PB_Cancel:
         pMain->bResolve = false;
         pMain->bUseMRDB = false;
         pMain->bUseAAS = false;
         removeFromWindowList();
         close();
         //pMain->bLock = false;
         //pMain->setFocus();
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

Boolean ReconcileData :: systemCommand(ICommandEvent& cmdEvt)
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

Boolean ReconcileData :: virtualKeyPress(IKeyboardEvent& keyEvt)
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

void ReconcileData :: buildControls()
   {
   pstrReconcileText = new IStaticText( IDS_ReconcileData_Text, this );
   IString strRecText = LOADSTRING( IDS_ReconcileData_Text );
   IString strDateText;
   if ( !pMain->bMultSysVPDTran )
      {
      strDateText = LOADSTRING( IDS_LastSensed );
      strDateText.change(STR_Substitute0, strSensedDate);
      }
   else
      strDateText = STR_Blank1;
   strRecText.change(STR_Substitute0, strDateText);
   pstrReconcileText->setText( strRecText );

   prbResolve = new IRadioButton(IDC_RB_ReconcileData_Resolve, this);
   prbResolve->select();

   prbMRDB = new IRadioButton(IDC_RB_ReconcileData_MRDB, this);
   if ( pMain->bMultSysVPDTran || pMain->bMultSysStackedMRDB )
      prbMRDB->disable();

   prbAAS = new IRadioButton(IDC_RB_ReconcileData_AAS, this);
   if ( pMain->bS390 )
      prbAAS->disable();

   ppbOK = new IPushButton(IDC_PB_OK, this);
   ppbCancel = new IPushButton(IDC_PB_Cancel, this);

   ppbHelp = new IPushButton(IDC_PB_Help, this);
   ppbHelp->enableHelp();
   ppbHelp->disableMouseClickFocus();

   if ( pMain->pProfile->bAutoRec )
      {
       prbResolve->select();
       ppbOK->click();                //47698 - flw
      }
   }

//=======================================================================
// FUNCTION NAME    ~ReconcileData
//
// DESCRIPTION      Delete any objects created by the constructor.
//=======================================================================

ReconcileData :: ~ReconcileData()
   {
   delete prbResolve;
   delete prbMRDB;
   delete prbAAS;
   delete ppbOK;
   delete ppbCancel;
   delete ppbHelp;
   }
