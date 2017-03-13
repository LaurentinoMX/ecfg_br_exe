//=======================================================================
// FILE NAME        AF9COMPL.HPP
//
// CLASS NAME       CompleteReconcile
//
// DESCRIPTION      This class is the "Reconciliation not complete"
//                  dialog of the Base Retrieval CPF which is displayed
//                  when the user selects the "End reconciliation" option
//                  without completely resolving the system.
//
// Programmer:      Erica Wilson  06/24/1998
//=======================================================================
// Copyright (c) International Business Machines Corp. 1998
// All rights reserved
//=======================================================================

#ifndef AF9COMPL_HPP
#define AF9COMPL_HPP

#include <icmdhdr.hpp>                  // ICommandHandler
#include <iframe.hpp>                   // IFrameWindow
#include <ikeyhdr.hpp>                  // IKeyboardHandler
#include "af9main.hpp"                  // MainWindow

class IPushButton;
class IRadioButton;


class CompleteReconcile : public IFrameWindow, public ICommandHandler,
                          public IKeyboardHandler
   {
   public:
   CompleteReconcile( IFrameWindow* pParent, MainWindow* pOwner );
   ~CompleteReconcile();
   Boolean bContinue;                   // Continue the reconciliation process
   Boolean vPlacement_s390;             //53915 MAMT 28/02/2001
   boolean setupKeyData1(char*, char*); // Add key values to the registry 53915

   protected:
   Boolean command(ICommandEvent& cmdEvt);
   Boolean systemCommand(ICommandEvent& cmdEvt);
   Boolean virtualKeyPress(IKeyboardEvent& keyEvt);

   private:
   MainWindow* pMain;                   // Main BaseRet window
   IFrameWindow* pParent;               // Parent window
   IRadioButton* prbContinue;           // Continue reconciliation radiobutton
   IRadioButton* prbMRDB;               // Quit resolving and use MRDB as base radiobutton
   IRadioButton* prbAAS;                // Quit resolving and use AAS as base radiobutton
   IRadioButton* prbQuit;               // Quit resolving and end current session
   IPushButton* ppbOK;                  // OK pushbutton
   IPushButton* ppbCancel;              // Cancel pushbutton
   IPushButton* ppbHelp;                // Help pushbutton
   void buildControls();                // Create control objects
   };
#endif
