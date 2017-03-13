//=======================================================================
// FILE NAME        AF9LOCAL.HPP
//
// CLASS NAME       LocaleDialog
//
// DESCRIPTION      This class is the locale dialog of the Apply Orders
//                  CPF.
//=======================================================================
// Copyright (c) International Business Machines Corp. 1996
// All rights reserved
//=======================================================================

#ifndef AF9LOCAL_HPP
#define AF9LOCAL_HPP

#include <icmdhdr.hpp>                  // ICommandHandler
#include <iframe.hpp>                   // IFrameWindow
#include <ikeyhdr.hpp>                  // IKeyboardHandler
#include <istring.hpp>                  // IString

#include "af9main.hpp"                  // MainWindow

class ICheckBox;
class IListBox;
class IStaticText;

class Profile;

class LocaleDialog : public IFrameWindow, public ICommandHandler,
                     public IKeyboardHandler
   {
   public:
   LocaleDialog(MainWindow* pOwner);
   ~LocaleDialog();

   protected:
   Boolean command(ICommandEvent& cmdEvt);
   Boolean virtualKeyPress(IKeyboardEvent& keyEvt);

   private:
   IString astrLocale[100];             // Array of available locales
   MainWindow* pMain;                   // Main window
   Profile* pProfile;                   // User profile
   IListBox* plbLocale;                 // Locale listbox
   void buildControls();                // Create control objects
   void loadDefaults();                 // Initialize control values
   };
#endif
