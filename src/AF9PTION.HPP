//=======================================================================
// FILE NAME        AF9PTION.HPP
//
// CLASS NAME       OptionsDialog
//
// DESCRIPTION      This class is the options dialog of the Apply
//                  Orders CPF.
// Modification Log
//-----------------------------------------------------------------------
// E. Wilson   01/21/2002   F62721 Added support for use of this class by
//                          the Base Retrieval profile DLL (BF9PROFL.DLL)
//=======================================================================
// Copyright (c) International Business Machines Corp. 1996
// All rights reserved
//=======================================================================

#ifndef AF9PTION_HPP
#define AF9PTION_HPP

#include <icmdhdr.hpp>                  // ICommandHandler
#include <iframe.hpp>                   // IFrameWindow
#include <ikeyhdr.hpp>                  // IKeyboardHandler
#include <iselhdr.hpp>                  // ISelectHandler
#include <istring.hpp>                  // IString

#ifndef BR_PROFILE    /* F62721 EDW 01/21/2002 */
  #include "af9main.hpp"                // MainWindow
#else
  #include "af9pfdlg.hpp"               // ProfileDialog
#endif  

class IListBox;
class IStaticText;
class IPushButton;

class Profile;

class OptionsDialog : public IFrameWindow, public ICommandHandler,
                      public ISelectHandler, public IKeyboardHandler
   {
   public:
#ifndef BR_PROFILE    /* F62721 EDW 01/21/2002 */
   OptionsDialog(MainWindow* pOwner);
#else
   OptionsDialog(ProfileDialog* pOwner);
#endif
   ~OptionsDialog();
   void hideButtons();/* F62721 EDW 01/21/2002 */

   protected:
   Boolean command(ICommandEvent& cmdEvt);
   Boolean systemCommand(ICommandEvent& cmdEvt);
   Boolean enter(IControlEvent& ctlEvt);
   Boolean selected(IControlEvent& ctlEvt);
   Boolean virtualKeyPress(IKeyboardEvent& keyEvt);

   private:
   USHORT usLocale;                     // Default selection
   IString sRestoreLocale;              // Current interface locale
   IString astrLocale[100];             // Array of available locales
#ifndef BR_PROFILE    /* F62721 EDW 01/21/2002 */
   MainWindow* pMain;                   // Main window
#else
   ProfileDialog* pMain;                // Main window
#endif
   Profile* pProfile;                   // User profile
   IListBox* plbLocale;                 // Locale listbox
   IStaticText* ptxDate;                // Date sample text
   IStaticText* ptxTime;                // Time sample text
   void buildControls();                // Create control objects
   void loadDefaults();                 // Initialize control values
   IPushButton* ppbHelp;                 // Help push button
   };
#endif
