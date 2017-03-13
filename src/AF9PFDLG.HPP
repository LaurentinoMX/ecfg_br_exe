/* =======================================================================
 * FILE NAME         AF9PFDLG.HPP
 *
 * CLASS NAME        ProfileDialog
 * -----------------------------------------------------------------------
 * DESCRIPTION       Defines the Base Retrieval dialogbox for editing the
 *                   application preferences.
 *
 *
 * MODIFICATION LOG:
 * -----------------------------------------------------------------------
 * Date        Programmer        Description
 * =======================================================================
 * 2002-01-21  EDW               Initial implementation (F62721)
 * 2002-02-26  DCG    68568      Added prototype of function
 *                               IPageHandler::select() to handle all
 *                               IPageSelectEvent events on the INotebook
 *                               and verify at page change if the retrieval
 *                               path keyed by the user exists.
 * 2002-03-08  DCG    68569      Added prototype of function
 *                               IKeyboardHandler::virtualKeyPress() to
 *                               handle keyboard events.
 * 2003-03-06  HECC   70162      Added prototype of function
 *                               ConvertToShortPathName to convert long path
 *                               names to short path names.
 *
 * =======================================================================
 * Copyright (c) International Business Machines Corp. 2002
 * All rights reserved
 * =======================================================================
 */
#ifdef  BR_PROFILE
#ifndef AF9PFDLG_HPP
#define AF9PFDLG_HPP

#include <os2.h>                                 // Windows definitions
#include <iframe.hpp>                            // IFrameWindow
#include <inotebk.hpp>                           // INoteBook
#include <istattxt.hpp>                          // IStaticText
#include <imcelcv.hpp>                           // IMultiCellCanvas
#include <ipagehdr.hpp>                          // IPageHandler
#include <icmdhdr.hpp>                           // ICommandHandler
#include <ikeyhdr.hpp>                           // IKeyboardHandler                               // DCG 03/07/2002 68569

class IPushButton;
class Profile;
class RetrSource;
class ReconcileProfile;
class OptionsDialog;
class IHelpWindow;
class INotebook;
class IStaticText;
class IMultiCellCanvas;

class ProfileDialog: public IFrameWindow, public ICommandHandler,
                     public IPageHandler,
                     public IKeyboardHandler                                                       // DCG 03/07/2002 68569
{
   public:
      ProfileDialog(ULONG ulWindow,
                    IWindow* pOwner);            // Constructor
      ~ProfileDialog();                          // Destructor
      IHelpWindow* helpWindow();                 // Return pointer to help window
      INotebook* getProfileNotebook();           // Return pointer to client notebook
      IBoolean static loadResources();           // Loads country specific resources
      int getReturnCode();                       // Returns code that indicates how the
                                                 // dialog was closed
      IString static strExecPath;                // Path to executable file
      Profile* pProfile;                         // User profile object

      //* 70162 Begin new logic: Returns the short path name from a given
      //* long path name.
      IString static ConvertToShortPathName( IString strPath );
      //* 70162 Ends new logic:    HECC:  03/06/2003

   protected:
      Boolean command(ICommandEvent& cmdEvt);
      Boolean systemCommand(ICommandEvent& cmdEvt);
      Boolean select(IPageSelectEvent& pgsEvt);                                                    // DCG 02/25/2002 68568
      Boolean virtualKeyPress(IKeyboardEvent& kbEvt);                                              // DCG 03/07/2002 68569

   private:
      RetrSource* pRetSrc;                       // Retrieval source dialog
      ReconcileProfile* pRecPrfl;                // Reconcile profile dialog
      OptionsDialog* pLocale;                    // Locale dialog
      INotebook* pNotebook;                      // Client window notebook
      IStaticText* ptxExtension;                 // Blank text extension for this dialog
      IMultiCellCanvas* pmccButtonExtension;     // Canvas for action buttons
      IPushButton*  ppbOK;                       // OK pushbutton
      IPushButton*  ppbCancel;                   // Cancel pushbutton
      IPushButton*  ppbHelp;                     // Help pushbutton
      IHelpWindow* pHelp;                        // Help window
      IString static strMRI;                     // Path to MRI resource file
      void buildGUI();                           // Constructs the user interface
      void adjustTabWidth();                     // Make all notebook tabs widths the same
      IBoolean createHelpWindow();               // Create a help window
      int iRC;                                    // Code the indicates how the dialog was closed
};

#endif
#endif
