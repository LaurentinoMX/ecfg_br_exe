//=======================================================================
// FILE NAME        AF9SELDT.HPP
//
// CLASS NAME       SelectDateDialog
//
// DESCRIPTION      This class is the date selection dialog of the
//                  Apply Orders CPF.
//=======================================================================
// Copyright (c) International Business Machines Corp. 1996
// All rights reserved
//=======================================================================

#ifndef AF9SELDT_HPP
#define AF9SELDT_HPP

#include <icmdhdr.hpp>                  // ICommandHandler
#include <iframe.hpp>                   // IFrameWindow
#include <ikeyhdr.hpp>                  // IKeyboardHandler
#include <ispinhdr.hpp>                 // ISpinHandler
#include <ispinnum.hpp>                 // INumericSpinButton -MSN
#include <ihelp.hpp>                    // IHelpWindow

#include "af9ordrs.hpp"                 // OrdersNotebook

class ICheckBox;
class IListBox;
class ISpinButton;
class IStaticText;

class Profile;

class SelectDateDialog : public IFrameWindow, public ICommandHandler,
                         public IKeyboardHandler, public ISpinHandler
   {
   public:
   SelectDateDialog(OrdersNotebook* pOwner);
   ~SelectDateDialog();

   protected:
   Boolean command(ICommandEvent& event);
   Boolean systemCommand(ICommandEvent& event);
   Boolean virtualKeyPress(IKeyboardEvent& event);
   Boolean spinEnded(IControlEvent& event);

   private:
   OrdersNotebook* pNotebook;           // Orders notebook
   Profile* pProfile;                   // User profile
   INumericSpinButton* psbYear;         // Year spinbutton
   INumericSpinButton* psbMonth;        // Month spinbutton
   INumericSpinButton* psbDay;          // Day spinbutton, EDS-chg
   void buildControls();                // Create control objects
   void loadDefaults();                 // Initialize control values
   IPushButton* ppbHelp;                // Help push button
   };
#endif
