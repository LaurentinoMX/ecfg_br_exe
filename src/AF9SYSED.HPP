//=======================================================================
// FILE NAME        AF9SYSED.HPP
//
// CLASS NAME       SystemEdit
//
// DESCRIPTION      This class is the new/move/rename system dialog of
//                  the Apply Orders CPF.
//=======================================================================
// Copyright (c) International Business Machines Corp. 1996
// All rights reserved
//=======================================================================

#ifndef AF9SYSED_HPP
#define AF9SYSED_HPP

#include <icmdhdr.hpp>                  // ICommandHandler
#include <iedithdr.hpp>                 // IEditHandler
#include <iframe.hpp>                   // IFrameWindow
#include <ikeyhdr.hpp>                  // IKeyboardHandler
#include <istring.hpp>                  // IString

#include "af9systm.hpp"                 // SystemsNotebook

class IEntryField;
class IListBox;
class IPushButton;
class IRadioButton;

class Profile;

class SystemEdit : public IFrameWindow, public ICommandHandler,
                   public IKeyboardHandler
   {
   public:
   enum { enumNew=1, enumSettings };
   SystemEdit(SystemsNotebook* pOwner);
   ~SystemEdit();
   void loadDefaults(USHORT usMode);    // Initialize control values

   protected:
   Boolean command(ICommandEvent& cmdEvt);
   Boolean systemCommand(ICommandEvent& cmdEvt);
   Boolean virtualKeyPress(IKeyboardEvent& keyEvt);
   Boolean edit(IControlEvent& ctlEvt);

   private:
   USHORT usMode;                       // New, rename, or move mode
   USHORT usPage;                       // Original index of notebook page
   SystemsNotebook* pNotebook;          // Parent window
   IEntryField* pefName;                // Name entry field
   IRadioButton* prbBefore;             // Placement before radiobutton
   IRadioButton* prbAfter;              // Placement after radiobutton
   IListBox* plbPlacement;              // Placement listbox
   IPushButton* ppbOK;                  // OK pushbutton
   IPushButton* ppbCancel;              // Cancel pushbutton
   IPushButton* ppbHelp;                // Help pushbutton   added MAN 2/10/97
   void buildControls();                // Create control objects
   };
#endif
