/* =======================================================================
 * FILE NAME         AF9RECPF.HPP
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
 * =======================================================================
 * Copyright (c) International Business Machines Corp. 2002
 * All rights reserved
 * =======================================================================
 */
 
#ifdef BR_PROFILE
#ifndef AF9RECPF_HPP
#define AF9RECPF_HPP

#include <icmdhdr.hpp>                           // ICommandHandler
#include <ikeyhdr.hpp>                           // IKeyboardHandler
#include <iframe.hpp>                            // IFrameWindow
#include "af9pfdlg.hpp"                          // ProfileDialog

class ICheckBox;
class IRadioButton;
class Profile;

class ReconcileProfile : public IFrameWindow, public ICommandHandler,
                         public IKeyboardHandler
   {
   public:
   ReconcileProfile(ProfileDialog* pOwner);
   ~ReconcileProfile();
   void hideButtons();

   protected:
   Boolean command(ICommandEvent& cmdEvt);
   Boolean virtualKeyPress(IKeyboardEvent& keyEvt);

   private:
   ProfileDialog* pMain;                         // Main window
   Profile* pProfile;                            // User profile
   ICheckBox*    pcbMismatches;                  // Show mismatches (for S/390)
   ICheckBox*    pcbAutoRec;                     // Automatically reconcile differences
   ICheckBox*    pcbProcRPO;                     // Process RPOs
   IRadioButton* prbTileHorizontal;              // Tile reconciliation windows horizontally
   IRadioButton* prbTileVertical;                // Tile reconciliation windows vertically
   IRadioButton* prbViewExpanded;                // View reconciliation data in expanded mode
   IRadioButton* prbViewCollapsed;               // View reconciliation data in collapsed mode

   void buildControls();                         // Create control objects
   void loadDefaults();                          // Initialize control values
   };
#endif
#endif
