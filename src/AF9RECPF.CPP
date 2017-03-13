/* =======================================================================
 * FILE NAME         AF9RECPF.CPP
 *
 * CLASS NAME        ReconcileProfile
 * -----------------------------------------------------------------------
 * DESCRIPTION       This class is the reconciliation profile dialog of the
 *                   Base Retrieval CPF ProfileDialog class.
 *
 *
 * MODIFICATION LOG:
 * -----------------------------------------------------------------------
 * Date        Programmer        Description
 * =======================================================================
 * 2002-01-21  EDW               Initial implementation (F62721).
 *                               Designed to work as a child dialog of the
 *                               ProfileDialog class in BF9PROFL.DLL as
 *                               Base Retrieval will now use this DLL to
 *                               manage its preferences.
 * 2002-03-08  DCG     68569     Added code to catch TAB and SHIFT+TAB
 *                               key presses to return focus to the
 *                               ProfileDialog window
 * 2003-10-21  HECC    79847     Adds necessary logic to catch the F1 key to start
 *                               the Help windows displaying process
 * =======================================================================
 * Copyright (c) International Business Machines Corp. 2002
 * All rights reserved
 * =======================================================================
 */
#ifdef BR_PROFILE
#include <icmdhdr.hpp>                           // ICommandHandler
#include <ikeyhdr.hpp>                           // IKeyboardHandler
#include <iframe.hpp>                            // IFrameWindow
#include <iradiobt.hpp>                          // IRadioButton
#include <icheckbx.hpp>                          // ICheckBox

#include <ihelp.hpp>                             // IHelpWindow  D79847:  HECC - 10/21/2003

#include "af9recpf.hpp"                          // ReconcileProfile
#include "af9pfdlg.hpp"                          // ProfileDialog
#include "af9profl.hpp"                          // Profile
#include "af9apply.h"                            // Resource ids

/* =======================================================================
 * FUNCTION NAME     ReconcileProfile
 * -----------------------------------------------------------------------
 * DESCRIPTION       Construct an instance of the ReconcileProfile class
 * =======================================================================
 */
ReconcileProfile :: ReconcileProfile(ProfileDialog* pOwner)
                  : IFrameWindow(IResourceId(IDD_RecProfile), NULL,
                                 pOwner->getProfileNotebook())
   {

   // Save parameters

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

/* =======================================================================
 * FUNCTION NAME     buildControls
 * -----------------------------------------------------------------------
 * DESCRIPTION       Instantiate the various dialog controls.
 * =======================================================================
 */
void ReconcileProfile :: buildControls()
   {
   pcbProcRPO = new ICheckBox(IDC_CB_RecProfile_ProcRPOs, this);
   pcbMismatches = new ICheckBox(IDC_CB_RecProfile_Mismatches, this);
   pcbAutoRec = new ICheckBox(IDC_CB_RecProfile_AutoRec, this);
   prbTileHorizontal = new IRadioButton(IDC_RB_RecProfile_Horizontal, this);
   prbTileVertical = new IRadioButton(IDC_RB_RecProfile_Vertical, this);
   prbViewExpanded = new IRadioButton(IDC_RB_RecProfile_Expanded, this);
   prbViewCollapsed = new IRadioButton(IDC_RB_RecProfile_Collapsed, this);
   }

/* =======================================================================
 * FUNCTION NAME     command
 * -----------------------------------------------------------------------
 * DESCRIPTION       Handle command messages.
 * =======================================================================
 */
Boolean ReconcileProfile :: command(ICommandEvent& cmdEvt)
   {

   switch ( cmdEvt.commandId() )
      {

      // User selected OK.

      case IDC_PB_OK:
         {
         if ( prbViewExpanded->isSelected() )
            pMain->pProfile->bExpanded = true;
         else
            pMain->pProfile->bExpanded = false;

         if ( prbTileHorizontal->isSelected() )
               pMain->pProfile->bTile = true;
         else
               pMain->pProfile->bTile = false;

         if ( pcbMismatches->isSelected() )
            pMain->pProfile->bShowS390Mismatches = true;
         else
            pMain->pProfile->bShowS390Mismatches = false;

         if ( pcbAutoRec->isSelected() )
               pMain->pProfile->bAutoRec = true;
         else
               pMain->pProfile->bAutoRec = false;

         if ( pcbProcRPO->isSelected() )
               pMain->pProfile->bProcRPO = true;
         else
               pMain->pProfile->bProcRPO = false;
         break;
         }

      // User selected Cancel.

      case IDC_PB_Cancel:
         break;

      default:
         return false;
      }
   return true;
   }

/* =======================================================================
 * FUNCTION NAME     virtualKeyPress
 * -----------------------------------------------------------------------
 * DESCRIPTION       Handle the Escape key.
 * =======================================================================
 */
Boolean ReconcileProfile :: virtualKeyPress(IKeyboardEvent& keyEvt)
    {
#ifndef BR_PROFILE
    if ( keyEvt.virtualKey() == IKeyboardEvent::esc )
        {
        postEvent(IWindow::command, IDC_PB_Cancel);
        return true;
        }
#endif

// DCG 03/07/2002 68569 Begin
#ifdef BR_PROFILE
    IKeyboardEvent::VirtualKey keyPressed = keyEvt.virtualKey();

    if ( ( prbViewExpanded->hasFocus() || prbViewCollapsed->hasFocus() ) && keyPressed == IKeyboardEvent::backTab )
       {
       pMain->postEvent(IWindow::command, IDS_ProfileDialog_TabSetFocus);
       return true;
       }
    if ( pcbAutoRec->hasFocus() && keyPressed == IKeyboardEvent::tab )
       {
       pMain->postEvent(IWindow::command, IDS_ProfileDialog_OKSetFocus);
       return true;
       }

   // 79847 Begin: Checks if the 'F1' key was pressed to display the help windows of the
   // Reconciliation tab of the Base Retrieval profile preferences.
   if ( keyPressed == IKeyboardEvent::f1 )
      {
      pMain->helpWindow()->show(IResourceId(IDD_RecProfile));
      return true;
      }
   //*79847 End:   HECC - 10/21/2003

#endif
// DCG 03/07/2002 68569 End

    return false;
    }

/* =======================================================================
 * FUNCTION NAME     loadDefaults
 * -----------------------------------------------------------------------
 * DESCRIPTION       Initialize the dialog control values.
 * =======================================================================
 */
void ReconcileProfile :: loadDefaults()
   {
      prbTileHorizontal->enableGroup();
      prbViewExpanded->enableGroup();

      if (pMain->pProfile->bExpanded )
         prbViewExpanded->select();
      else
         prbViewCollapsed->select();

      if (pMain->pProfile->bTile )
         prbTileHorizontal->select();
      else
         prbTileVertical->select();

      if ( pMain->pProfile->bShowS390Mismatches )
         pcbMismatches->select();

      if ( pMain->pProfile->bAutoRec )
         pcbAutoRec->select();

      /* EDW 02/05/2002 begin - disabled this button
      if ( pMain->pProfile->bProcRPO )
         pcbProcRPO->select();
      */
      pcbProcRPO->deselect();
      pcbProcRPO->hide();
      /* EDW 02/05/2002 end */
   }

/* =======================================================================
 * FUNCTION NAME     hideButtons
 * -----------------------------------------------------------------------
 * DESCRIPTION       Hide the action buttons for this dialog.
 * =======================================================================
 */
void ReconcileProfile :: hideButtons()
   {
   IPushButton* ppbOK = new IPushButton(IDC_PB_OK, this);
   IPushButton* ppbCancel = new IPushButton(IDC_PB_Cancel, this);
   IPushButton* ppbHelp = new IPushButton(IDC_PB_Help, this);

   ppbOK->hide();
   ppbCancel->hide();
   ppbHelp->hide();

   delete ppbOK;
   delete ppbCancel;
   delete ppbHelp;
   }

/* =======================================================================
 * FUNCTION NAME     ~ReconcileProfile
 * -----------------------------------------------------------------------
 * DESCRIPTION       Delete any objects created by the constructor.
 * =======================================================================
 */
ReconcileProfile :: ~ReconcileProfile()
   {
   delete pcbProcRPO;
   delete pcbMismatches;
   delete pcbAutoRec;
   delete prbTileHorizontal;
   delete prbTileVertical;
   delete prbViewExpanded;
   delete prbViewCollapsed;
   }
#endif
