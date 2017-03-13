//=======================================================================
// FILE NAME        AF9NAME.HPP
//
// CLASS NAME       FileNameDialog
//
// DESCRIPTION      This class is the file name entry dialog of
//                  the Apply Orders CPF.
//=======================================================================
// Copyright (c) International Business Machines Corp. 1996
// All rights reserved
//=======================================================================

#ifndef AF9NAME_HPP
#define AF9NAME_HPP

#include <icmdhdr.hpp>                  // ICommandHandler
#include <iedithdr.hpp>                 // IEditHandler
#include <iframe.hpp>                   // IFrameWindow
#include <ikeyhdr.hpp>                  // IKeyboardHandler
#include <istring.hpp>                  // IString

#include "af9main.hpp"                  // MainWindow

class IEntryField;
class IPushButton;

class Profile;

class FileNameDialog : public IFrameWindow, public ICommandHandler,
                       public IKeyboardHandler, public IEditHandler
   {
   public:
   FileNameDialog(MainWindow* pOwner);
   ~FileNameDialog();
   void loadDefaults(USHORT usMode);    // Initialize control values

   protected:
   Boolean command(ICommandEvent& cmdEvt);
   Boolean systemCommand(ICommandEvent& cmdEvt);
   Boolean virtualKeyPress(IKeyboardEvent& keyEvt);
   Boolean edit(IControlEvent& ctlEvt);

   private:
   USHORT usMode;                       // Save, generate, or rename mode
   MainWindow* pMain;                   // Parent window
   IEntryField* pefName;                // Name entry field
   IPushButton* ppbOK;                  // OK pushbutton
   IPushButton* ppbHelp;                // Help pushbutton   added MSN 2/10/97
   IPushButton* ppbCancel;              // Cancel pushbutton
   Boolean confirmReplace();            // Ensure not overwriting file
   Boolean confirmValid();              // Ensure legitimate file name
   void buildControls();                // Create control objects
   };
#endif
