//=======================================================================
// FILE NAME        AF9RTSRC.HPP
//
// CLASS NAME       RetrSource
//
// DESCRIPTION      This class is the "Retrieval source" dialog of the
//                  the Base Retrieval CPF.  It is used to capture the
//                  retrieval application the user wants to use to retrieve
//                  configuration files.
//
// Programmer:      Erica Wilson  09/02/1998
// Change Log:
// Date        Programmer        Description
// ======================================================================
// 10/18/1999  E. Wilson         Add pcbMismatches instance variable
//
// 12/17/1999  F. Walker         Added code so the if the user does not have
//                               Personal Communications installed on their
//                               machines to supress error message and not
//                               give them the option of doing VM retrieval.
// 01/21/2002  E. Wilson  F62721 Added support for use of this class by
//                               the Base Retrieval profile DLL (BF9PROFL.DLL)
// 02/26/2002  D. Cruz    68568  Added prototypes for two new functions: One,
//                               validRetrievalPath(), validates that the retrieval
//                               path keyed by the user exists and the other one,
//                               setFocusRetrievalPath(), is needed to set focus
//                               on the pefRetrPath member for the user to see
//                               where the error should be corrected.
// 08/19/2003  C. Herrera  79483 Added flag bBrowseDir to show that the function
//                               validRetrievalPath() was called from:
//                               selectDirectory(IString& str) and no error messages
//                               be displayed when the specified Retrieval Path by
//                               user doesn't exist
// 09/21/2004  C. Herrera  94556 Added function definitions to fix a general problem
//                               of consistency in the "/" and "\" characters.
// 02/09/2005  E. Reyes   D97759 Added prototype for new function: IsValidDirectory,
//                               validates the directory typed for the user is correct
// 29/05/2006 C. Herrera 108502  Added logic to disable the "VM Host" (Ub9host1.exe) and
//                               "Inventory Services" (Ub9host4.exe) options for EMEA
//                               geography users.
// 06/06/2006  E. Reyes  106765  Move the function getUserGeogaphy to public scope
//
// 25/06/2014  GTL    Req1702   Chrome Radiobutton was added.
//=======================================================================
// Copyright (c) International Business Machines Corp. 1998
// All rights reserved
//=======================================================================


#ifndef AF9RTSRC_HPP
#define AF9RTSRC_HPP

#include <icmdhdr.hpp>                  // ICommandHandler
#include <ikeyhdr.hpp>                  // IKeyboardHandler
#include <iselhdr.hpp>                  // ISelectHandler
#include <ifilehdr.hpp>                 // IFileDialogHandler /* F62721 EDW 01/21/2002 */
#include <iframe.hpp>                   // IFrameWindow
#include <ikeyset.h>                    // IKeySet
#ifndef BR_PROFILE    /* F62721 EDW 01/21/2002 */
  #include "af9main.hpp"                // MainWindow
#else
  #include "af9pfdlg.hpp"               // ProfileDialog
#endif

#define STR_Error   'E'                 // D94556:  HECC - 09/21/2004
#define STR_Warning 'W'                 // D94556:  HECC - 09/21/2004

class IPushButton;
class ICheckBox;
class IListBox;
class IRadioButton;
class IEntryField;                      /* F62721 EDW 01/21/2002 */
class ProfileDialog;                    /* F62721 EDW 01/21/2002 */

class RetrApps
   {
   public:
   RetrApps( IString strA, IString strE) : strAppName(strA), strAppExe( strE ){}

   // required for KeySet key() function below
   IString const& getAppName() const { return strAppName; }
   IString const& getExeName() const { return strAppExe; }
   private:
   IString strAppName;
   IString strAppExe;

   };

class RetrSource : public IFrameWindow, public ICommandHandler,
                      public IKeyboardHandler, public ISelectHandler,
                      public IFileDialogHandler    /* F62721 EDW 01/21/2002 */
   {
   public:

#ifndef BR_PROFILE    /* F62721 EDW 01/21/2002 */
   RetrSource( MainWindow* pOwner );
#else
   RetrSource( ProfileDialog* pOwner );
#endif
   ~RetrSource();
   void hideButtons();/* F62721 EDW 01/21/2002 */

// DCG 02/25/2002 68568 Begin
#ifdef BR_PROFILE
   IBoolean validRetrievalPath();                        // Verify the retrieval path keyed in by the user
   void setFocusRetrievalPath();                         // Set Focus on the retrieval path entry field
#endif
// DCG 02/25/2002 68568 End

   //* D94556  Begin logic:
   void oneCharManage();                                     // Validates and changes to default C directory
   void displayMessage(IString sMsg, char cType, int iIDS);  // Displays messages box to the user
   //* D94556  Ends new logic:  HECC - 09/21/2004

   Boolean bBrowseDir; //* 79483
   //F108502 Begin new logic
   IString getUserGeography();                           // Get user's geography from registry
   //F108502 End new logic      HECC - 05/29/2006
   protected:
   Boolean command(ICommandEvent& cmdEvt);
   Boolean systemCommand(ICommandEvent& cmdEvt);
   Boolean virtualKeyPress(IKeyboardEvent& keyEvt);
   Boolean selected(IControlEvent& ctlEvt);

   private:
#ifndef BR_PROFILE    /* F62721 EDW 01/21/2002 */
   MainWindow*   pMain;                                  // Parent window
#else
   ProfileDialog* pMain;                                 // Parent window
#endif
   Boolean       isValidDirectory(IString sDirectory);   // D97759 RECR - 02/09/2005
   IString       strAppName;                             // Name of host interface application for retrieving files
   SHORT         sRetAppItem;                            // listbox index of user's host interface application selection
   SHORT         sRetFamItem;                            // list box index for Product Family
   IListBox*     plbInterfaces;                          // List of host interface applications
   IListBox*     plbProdFamily;                          // List of Product family group
   ICheckBox*    pcbHW;                                  // Retrieve HW checkbox
   ICheckBox*    pcbSW;                                  // Retrieve SW checkbox
   ICheckBox*    pcbMRPD;                                // FastPath - Usability
   IRadioButton* pcbIE;                                  // FastPath - Usability
   IRadioButton* pcbNet;                                 // FastPath - Usability
   //GTL Req1702 BEGIN: Radio button for firefox is added
   IRadioButton* pcbFir;								 // FastPath - Usability
   //GTL END
#ifndef BR_PROFILE    /* F62721 EDW 01/21/2002 */
   ICheckBox*    pcbMismatches;                          // Show S390 mismatches
   ICheckBox*    pcbAutoRec;                             // F47698 - flw
   ICheckBox*    pcbProcRPO;                             // F47700 - flw
#else
   ICheckBox*    pcbFastPath;                            // Run in fastpath mode
   IRadioButton* prbSortByName;                          // Sort retrieved data by name
   IRadioButton* prbSortByDate;                          // Sort retrieved data by date
   IRadioButton* prbSortByNameA;                         // Sort retrieved data by name ascending
   IRadioButton* prbSortByNameD;                         // Sort retrieved data by name descending
   IRadioButton* prbSortByDateA;                         // Sort retrieved data by date ascending
   IRadioButton* prbSortByDateD;                         // Sort retrieved data by date descending
   IPushButton*  ppbBrowseRetrPath;                      // Browse retrieval path pushbutton
   IEntryField*  pefRetrPath;                            // Retrieval path entry field
#endif
   IKeySet<RetrApps, IString> appSet;                    // Set of retrieval applications choices
   IPushButton*  ppbOK;                                  // OK pushbutton
   IPushButton*  ppbCancel;                              // Cancel pushbutton
   IPushButton*  ppbHelp;                                // Help pushbutton
   void buildControls();                                 // Create control objects
   //F106765 Req1436 06/06/2006  Move the function getUserGeogaphy to public scope
   //F108502 Begin new logic
   //IString getUserGeography();                           // Get user's geography from registry
   //F108502 End new logic      HECC - 05/29/2006

   IString getHostInterfaceName( IString strHost );      // Get host interface application name
   void setHorzScrollWidth();                            // Set listbox auto horizontal scroll
#ifdef BR_PROFILE     /* F62721 EDW 01/21/2002 */
   void selectDirectory(IString& str);                   // Select a default retrieval path
#endif
   };

// -----------------------------------------------------
// required by KeySet
inline IString const&      key( RetrApps const& aApp )   // Key access
{   return aApp.getAppName();  }
#endif
