//=======================================================================
// FILE NAME        AF9RECON.HPP
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

#ifndef AF9RECON_HPP
#define AF9RECON_HPP

#include <icmdhdr.hpp>                  // ICommandHandler
#include <iframe.hpp>                   // IFrameWindow
#include <ikeyhdr.hpp>                  // IKeyboardHandler
#include <istring.hpp>                  // IString
#include "af9main.hpp"                  // MainWindow

class IPushButton;
class IRadioButton;
class IStaticText;


class ReconcileData : public IFrameWindow, public ICommandHandler,
                      public IKeyboardHandler
   {
   public:
   ReconcileData(MainWindow* pOwner, IString strDate);
   ~ReconcileData();

   protected:
   Boolean command(ICommandEvent& cmdEvt);
   Boolean systemCommand(ICommandEvent& cmdEvt);
   Boolean virtualKeyPress(IKeyboardEvent& keyEvt);

   private:
   MainWindow* pMain;                   // Parent window
   IString strSensedDate;               // MRDB data last sensed date
   IStaticText* pstrReconcileText;      // ReconcileData dialog text
   IRadioButton* prbResolve;            // Reconcile radiobutton
   IRadioButton* prbMRDB;               // Use MRDB as base radiobutton
   IRadioButton* prbAAS;                // Use AAS as base radiobutton
   IPushButton* ppbOK;                  // OK pushbutton
   IPushButton* ppbCancel;              // Cancel pushbutton
   IPushButton* ppbHelp;                // Help pushbutton
   void buildControls();                // Create control objects
   };
#endif
