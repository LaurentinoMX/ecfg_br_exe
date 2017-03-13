/* =======================================================================
 * FILE NAME         AF9PFDLG.CPP
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
 * 2002-02-21  EDW               Remove min/max buttons from dialog (D68571)
 * 2002-02-26  DCG    68568      Added definition of function IPageHandler::select().
 *                               Also added code to support validation of
 *                               the retrieval path keyed in by the user.
 * 2002-03-08  DCG    68569      Added definition of function
 *                               IKeyboardHandler::virtualKeyPress() to
 *                               keep focus over the notebook's tabs when
 *                               the user is moving through them with the
 *                               cursor keys. Added definition of commands
 *                               IDS_ProfileDialog_TabSetFocus &
 *                               IDS_ProfileDialog_OKSetFocus to set focus
 *                               over the window of this class.
 * 2002-05-28  MACD   71400      Catch a RegisterClass exception
 * 2003-01-24  HECC   73363      Added logic to process the model argument
 *                               passed by eConfig, modifying the Base Retrieval
 *                               profile variable that contains the product
 *                               Family the configuration belongs to.
 * 2003-02-26  HECC   70162      Added logic to convert long path names
 *                               to short path names. This is used  to correctly
 *                               load the Help libraries at the time to call
 *                               the function: "addLibraries"
 *                               (e.g. c:\Program Files\eConfig will be converted
 *                                 to: c:\Progra~1\eConfig)
 * 2003-10-21  HECC   79847      Adds necessary logic to catch the F1 key to start
 *                               the Help windows displaying process
 * 2003-12-03  HECC   86659      Deleted lines that causes the crash of the OF
 *                               application when Alt+F4 is pressed.
 *
 * 2006-17-05  RERC  106661A     The logic for the output path for the ConvertToShortPath
 *                               function is changed due to the ShortPath sometimes
 *                               is bigger than input path
 *                               (e.g. c:\econfig_test\5.1.39\2066.cfr is converted to
 *                                    c:\econf_1~1\51B19D~1.39\2066.CFR)
 * =======================================================================
 * Copyright (c) International Business Machines Corp. 2002
 * All rights reserved
 * =======================================================================
 */
#ifdef BR_PROFILE
#include <icmdhdr.hpp>                           // ICommandHandler
#include <iframe.hpp>                            // IFrameWindow
#include <ihelp.hpp>                             // IHelpWindow
#include <ipushbut.hpp>                          // IPushButton
#include <istattxt.hpp>                          // IStaticText
#include <imcelcv.hpp>                           // IMultiCellCanvas
#include <imsgbox.hpp>                           // IMessageBox
#include <isysmenu.hpp>                          // ISystemMenu
#include <ikeyhdr.hpp>                           // IKeyboardHandler         // DCG 03/07/2002 68569
#include "af9pfdlg.hpp"                          // ProfileDialog
#include "af9profl.hpp"                          // Profile
#include "af9rtsrc.hpp"                          // RetrSource
#include "af9recpf.hpp"                          // ReconcileProfile
#include "af9ption.hpp"                          // OptionsDialog
#include "af9apply.h"                            // Resource ids

IString ProfileDialog::strExecPath = "";
IString ProfileDialog::strMRI = "";


/* =======================================================================
 * FUNCTION NAME     ProfileDialog
 * -----------------------------------------------------------------------
 * DESCRIPTION       Construct an instance of the ProfileDialog class
 * =======================================================================
 */
ProfileDialog :: ProfileDialog(ULONG ulWindow, IWindow* pOwner)
               : IFrameWindow(IResourceId(ulWindow), NULL, pOwner,
                              IRectangle(480, 460),        /* D68571 EDW 2/21/02 */
                              IFrameWindow::titleBar|      /* D68571 EDW 2/21/02 */
                              IFrameWindow::systemMenu |   /* D68571 EDW 2/21/02 */
                              IFrameWindow::sizingBorder)  /* D68571 EDW 2/21/02 */
   {
   setId(IDW_ProfileDialog);

   /* Initialize instance variables */

   pProfile = new Profile();
   pRetSrc = (RetrSource*)NULL;
   pRecPrfl = (ReconcileProfile*)NULL;
   pLocale = (OptionsDialog*)NULL;
   pHelp = (IHelpWindow*)NULL;
   pNotebook = (INotebook*)NULL;
   ptxExtension = (IStaticText*)NULL;
   pmccButtonExtension = (IMultiCellCanvas*)NULL;
   ppbOK = (IPushButton*)NULL;
   ppbCancel = (IPushButton*)NULL;
   ppbHelp = (IPushButton*)NULL;
   iRC = -3;

   /* Change location of registry keys to new location, if
    * they have not already been moved
    */

   Profile::changeLocationOfKeys();


   /* Construct the help window */

   createHelpWindow();

   /* Load the default profile settings */

   IString strProfile;
   pProfile->load(strProfile);

   //* 73363 Begin New Logic: Added logic to process the model argument
   //* passed by eConfig, modifying the Base Retrieval profile variable
   //* that contains the product Family the configuration belongs to.
   //* HECC: 24/Ene/2003

   if ( IApplication::current().argc() > 1 )
      {

      IString strModelFamily = IApplication::current().argv(1);

      if ( strModelFamily.indexOf(STR_AS400) )
         pProfile->strProdFamily = STR_AS400;
      else if ( strModelFamily.indexOf(STR_RS6000) )
              pProfile->strProdFamily = STR_RS6000;
           else if ( strModelFamily.indexOf(STR_S390) )
                   pProfile->strProdFamily = STR_S390;
      }
   //* 73363 End New Logic.

   /* Create and build application controls */

   buildGUI();

   /* Add handlers for command and keyboard events */

   ICommandHandler::handleEventsFor(this);
   }

/* =======================================================================
 * FUNCTION NAME     buildGUI
 * -----------------------------------------------------------------------
 * DESCRIPTION       Instantiate the various application controls
 * =======================================================================
 */
void ProfileDialog :: buildGUI()
   {
   /* Create the client notebook */

   /* Begin New Logic  D71400   MACD 05/29/2002 */
   /* pNotebook = new INotebook(IDC_ProfileDialog_NB_Profile, this, this); */

   try
     {
     pNotebook = new INotebook(IDC_ProfileDialog_NB_Profile, this, this);

     }
     catch(IAccessError& exc)             // Catch an IAccessError exception
          {                             // RegisterClass exception could be a problem
          if (exc.isRecoverable())      // because the notebook is already registered
             {                          // First checking if is a recoverable exception
              HINSTANCE hInst = GetModuleHandle(0);          // then the notebook will be unregistered.
              if ( UnregisterClass( "IOC Page Clipping Window" , hInst) )   //After that try again.
                 {
                 pNotebook = new INotebook(IDC_ProfileDialog_NB_Profile, this, this);
                 }
             }
          else                          // If it is an unrecoverable exception, then
             return;                    // return to the API.
          }

   /* End New Logic  D71400   MACD 05/29/2002 */

   /* Set notebook properties */

   pNotebook->enableTabStop();

   IPageHandler::handleEventsFor(pNotebook);
   IKeyboardHandler::handleEventsFor(pNotebook);     // DCG 03/07/2002 68569

   /* Constructor profile dialogs */

   pRetSrc = new RetrSource(this);
   pRecPrfl = new ReconcileProfile(this);
   pLocale = new OptionsDialog(this);
   pRetSrc->hideButtons();
   pRecPrfl->hideButtons();
   pLocale->hideButtons();

   /* Add profile dialogs to notebook */

   INotebook::PageSettings::Attribute attributeMajor =
       INotebook::PageSettings::majorTab |
       INotebook::PageSettings::autoPageSize |
       INotebook::PageSettings::statusTextOn;
   INotebook::PageSettings pgMajor = INotebook::PageSettings(attributeMajor);

   pgMajor.setTabText(IResourceId(IDS_ProfileDialog_Retrieval));
   pNotebook->addFirstPage(pgMajor, pRetSrc);
   pgMajor.setTabText(IResourceId(IDS_ProfileDialog_Reconcile));
   pNotebook->addLastPage(pgMajor, pRecPrfl);
   pgMajor.setTabText(IResourceId(IDS_ProfileDialog_Language));
   pNotebook->addLastPage(pgMajor, pLocale);
   pNotebook->turnToPage(pNotebook->firstPage());
   adjustTabWidth();

   /* Create a blank static text frame extension.
      Then add static text extension to frame
    */

   ptxExtension = new IStaticText(IDC_TX_Path, this, this);
   //addExtension(ptxExtension, IFrameWindow::aboveClient, thickLine);  /* D68571 EDW 2/21/02 */

   /* Create canvas frame extension and action buttons.
      Then add buttons to canvas and canvas to frame.
    */

   pmccButtonExtension = new IMultiCellCanvas(IDC_ProfileDialog_MCC_Button, this, this);
   ppbOK =     new IPushButton(IDC_PB_OK, pmccButtonExtension,
                               pmccButtonExtension);
   ppbCancel = new IPushButton(IDC_PB_Cancel, pmccButtonExtension,
                               pmccButtonExtension);
   ppbHelp =   new IPushButton(IDC_PB_Help, pmccButtonExtension,
                               pmccButtonExtension);
   ppbOK->setText(IResourceId(IDS_ProfileDialog_ButtonOK));
   ppbCancel->setText(IResourceId(IDS_ProfileDialog_ButtonCancel));
   ppbHelp->setText(IResourceId(IDS_ProfileDialog_ButtonHelp));
   pmccButtonExtension->addToCell(ppbOK, 2, 2);
   pmccButtonExtension->addToCell(ppbCancel, 4, 2);
   pmccButtonExtension->addToCell(ppbHelp, 6, 2);
   pmccButtonExtension->setBackgroundColor(pRetSrc->backgroundColor());
   pmccButtonExtension->setRowHeight(3, IMultiCellCanvas::defaultCell().height());
   addExtension(pmccButtonExtension, IFrameWindow::belowClient, thickLine);

   /* Size frame and add notebook to it's client window */

   //sizeTo(ISize(500, 500));  /* D68571 EDW 2/21/02 (size in constructor) */
   setClient(pNotebook);
   ppbHelp->enableHelp();
   }

/* =======================================================================
 * FUNCTION NAME     systemCommand
 * -----------------------------------------------------------------------
 * DESCRIPTION       Handle system close events
 * =======================================================================
 */
Boolean ProfileDialog :: systemCommand(ICommandEvent& cmdEvt)
   {
   if ( cmdEvt.commandId() == ISystemMenu::idClose )
      {
      postEvent(IWindow::command, IDC_PB_Cancel);
      return true;
      }
   return false;
   }

/* =======================================================================
 * FUNCTION NAME     select
 * -----------------------------------------------------------------------
 * DESCRIPTION       Handle notebook page selection events
 * =======================================================================
 * DCG 02/25/2002 68568
 */
Boolean ProfileDialog :: select(IPageSelectEvent& pgsEvt)
   {
      IPageHandle pghPrevSelected = pgsEvt.previousSelectedPageHandle();
      IPageHandle pghSelected = pgsEvt.pageHandle();

      // Verification ( pghSelected != pghPrevSelected ) helps prevent cycling
      // in an infinite series of pNotebook->turnToPage() and this select() calls
      // because method pNotebook->turnToPage() raises an IPageHandler::select() method.
      if ( pghSelected != pghPrevSelected && pghPrevSelected == pNotebook->firstPage() )
         {
         if ( !pRetSrc->validRetrievalPath() )
            {
            // Retrieval path keyed by user not valid
            postEvent(IWindow::command, IDC_ProfileDialog_SelFirstPage);
            return true;
            }
         }

      return false;
   }

/* =======================================================================
 * FUNCTION NAME     virtualKeyPress
 * -----------------------------------------------------------------------
 * DESCRIPTION       Handle virtual key presses
 * =======================================================================
 * DCG 03/07/2002 68569
 */
Boolean ProfileDialog :: virtualKeyPress(IKeyboardEvent& kbEvt)
{
   IKeyboardEvent::VirtualKey keyPressed = kbEvt.virtualKey();

   switch ( keyPressed )
      {
      case IKeyboardEvent::left:
         if ( pNotebook->firstPage() != pNotebook->topPage() )
            pNotebook->turnToPage(pNotebook->previousPage(pNotebook->topPage()));
         else
            pNotebook->turnToPage(pNotebook->lastPage());
         pNotebook->setFocus();
         break;
      case IKeyboardEvent::right:
         if ( pNotebook->lastPage() != pNotebook->topPage() )
            pNotebook->turnToPage(pNotebook->nextPage(pNotebook->topPage()));
         else
            pNotebook->turnToPage(pNotebook->firstPage() );
         pNotebook->setFocus();
         break;
      case IKeyboardEvent::backTab:
         if ( pNotebook->hasFocus() )
            ppbOK->setFocus();
         break;

      // 79847 Begin: Checks if the 'F1' key was pressed to display the Base Retrieval
      // preferences help windows.
      case IKeyboardEvent::f1:
         pHelp->show(IResourceId(IDW_ProfileDialog));
         break;
      //*79847 End:   HECC - 10/21/2003

      default:
         return false;
      }

   return true;

}

/* =======================================================================
 * FUNCTION NAME     command
 * -----------------------------------------------------------------------
 * DESCRIPTION       Handle application events
 * =======================================================================
 */
Boolean ProfileDialog :: command(ICommandEvent& cmdEvt)
   {
   switch ( cmdEvt.commandId() )
      {
      /* User selected 'ok' button */

      case IDC_PB_OK:
         {
         // DCG 02/25/2002 68568 Begin
         if ( !pRetSrc->validRetrievalPath() )
            {
            // Retrieval path keyed by user not valid
            postEvent(IWindow::command, IDC_ProfileDialog_SelFirstPage);
            break;
            }
         // DCG 02/25/2002 68568 End
         pRetSrc->sendEvent(IWindow::command, IDC_PB_OK);
         pRecPrfl->sendEvent(IWindow::command, IDC_PB_OK);
         pLocale->sendEvent(IWindow::command, IDC_PB_OK);
         IString strProfile;
         pProfile->save(strProfile);
         iRC = 0;
         close();
         break;
         }

      /* User selected 'cancel' button */

      case IDC_PB_Cancel:
         {
         //* 86659 Begins: Lines not necessary as they are automatically
         //* closed when the notebook is closed.
         //* pRetSrc->sendEvent(IWindow::command, IDC_PB_Cancel);
         //* pRecPrfl->sendEvent(IWindow::command, IDC_PB_Cancel);
         //* pLocale->sendEvent(IWindow::command, IDC_PB_Cancel);
         //* 86659 Ends:   HECC - 12/03/2003
         iRC = 1;
         close();
         break;
         }

      // DCG 02/25/2002 68568 Begin
      /* Select first page as retrieval path keyed by user is wrong */
      case IDC_ProfileDialog_SelFirstPage:
         {
         pNotebook->turnToPage(pNotebook->firstPage());
         pRetSrc->setFocusRetrievalPath();
         break;
         }
      // DCG 02/25/2002 68568 End

      // DCG 03/07/2002 68569 Begin
      /* This command is posted by the windows that are inside the */
      /* pNotebook's pages in order to reset focus to the pNotebook's tabs */
      case IDS_ProfileDialog_TabSetFocus:
         {
         pNotebook->setFocus();
         break;
         }

      /* This command is posted by the windows that are inside the */
      /* pNotebook's pages in order to reset focus to the OK button */
      case IDS_ProfileDialog_OKSetFocus:
         {
         ppbOK->setFocus();
         break;
         }
      // DCG 03/07/2002 68569 End

      default:
         return false;
      }
   return true;
   }

/* =======================================================================
 * FUNCTION NAME     loadResources
 * -----------------------------------------------------------------------
 * DESCRIPTION       Construct a new set of resources, loaded from the
 *                   current resource DLL.
 * =======================================================================
 */
IBoolean ProfileDialog :: loadResources()
   {
   IString strPath = ProfileDialog::strExecPath;

   /* Get locale name
    *
    * It has not yet been determined how we will get the locale,
    * so it is hard-coded to "en_us" for now
    */

   strMRI = "bf9en_us";

   try
      {
      /* Attach the MRI DLL */

      strMRI.insert(STR_PathMRI);
      strMRI.insert(strPath);
      IApplication::current().setUserResourceLibrary(strMRI+STR_ExtensionDLL);
      }
   catch(IException& exc)
      {
      IString strError = (IString)exc.text();
      IMessageBox m(IWindow::desktopWindow());
      m.show( strError + "\n" + strMRI + STR_ExtensionDLL, IMessageBox::errorIcon );
      return false;
      }
   return true;
   }

/* =======================================================================
 * FUNCTION NAME     createHelpWindow
 * -----------------------------------------------------------------------
 * DESCRIPTION       Construct a help window.
 * =======================================================================
 */
IBoolean ProfileDialog :: createHelpWindow()
   {
   IFUNCTRACE_DEVELOP();

   IHelpWindow::setDefaultStyle( IHelpWindow::defaultStyle()
                               | IHelpWindow::ipfCompatible );

   pHelp = new IHelpWindow(HELPTABLE_AFOAPPLY, this);

   //70162 Begin new logic: Returns the short path name, from a given long
   //path name.  The passed parameter is the exact path where the .HLP Base
   //Base Retrieval file is
   IString strShortName = ConvertToShortPathName(strMRI+STR_ExtensionHelp);
   //70162 Ends new logic

   try
      {
      //70162 Begin new logic: The IString strShortName variable will contain
      //the short path name where the .HLP file of Base Retrieval is, and it
      //will be used now as parameter to add the help libraries in the profile
      //window.  HECC: 02/26/2003
      /* pHelp->addLibraries(strMRI+STR_ExtensionHelp); */
      pHelp->addLibraries(strShortName);
      //70162 Ends new logic.
      }
   catch(IException& exc)
      {
      IString strError = (IString)exc.text();
      IMessageBox m(this);
      m.show( strError + "\n" + strMRI + STR_ExtensionHelp,
              IMessageBox::errorIcon );
      }
   pHelp->setTitle(IResourceId(IDS_HelpTitle));
   return true;
   }

//=======================================================================
// 70162:  New Function.
// FUNCTION NAME    ConvertToShortPath
//
// DESCRIPTION      Converts long path names to short path names
//                  (eg, "c:\program files\econfig" to
//                       "c:\progra~1\econfig")
//                  Returns short path if successful; otherwise,
//                  returns the path name as is
//                  HECC: 02/26/2003
// MODIFIED:
// D106661A  RERC   05/17/2006   Create a new logic for the function
//=======================================================================
// D106661A Begin RERC 05/17/2006
//          The lines of the code are comented and replaced by the code below.
/*
IString ProfileDialog :: ConvertToShortPathName( IString strPath )
{
   IString strReturn = strPath;


   char *shortPath = (char*) malloc( strPath.length() + 1 );
   if( shortPath == NULL )
      return strReturn;

   if( GetShortPathName(strPath, shortPath, strPath.length()+1) )
      strReturn = IString( shortPath );

   free( shortPath );
   return strReturn;
}*/
IString ProfileDialog :: ConvertToShortPathName( IString strPath )
{
   IString strReturn = strPath;
   int isizePath = strPath.length()+ 1;
   int isizePathFunction = 0;
   char* shortPath = NULL;

   shortPath = (char*) malloc( isizePath );

   if( shortPath != NULL ) //Validate the allocate of memory
   {
     strcpy(shortPath,strPath);

     isizePathFunction =  GetShortPathName(strPath, shortPath, isizePath );

     //If the size of the input path is lower than the size of the path converted
     //then return the shorPath value
     if (isizePath < (isizePathFunction + 1 ))
       {
         strReturn = IString( shortPath );
       }
     else
       {
         //The length of the shortPath is bigger than the input path
         //reallocate enough memory.
         shortPath = (char*) realloc(shortPath, isizePathFunction + 1);
         if (shortPath != NULL)
         {
             isizePath = GetShortPathName(strPath, shortPath, isizePathFunction + 1 );
             if( isizePath != 0  )
                 strReturn = IString( shortPath );
         }
       }
     }
   free( shortPath );
   return strReturn;
}
//D10661A End.
/* =======================================================================
 * FUNCTION NAME     adjustTabWidth
 * -----------------------------------------------------------------------
 * DESCRIPTION       Adjust the notebook tab width to fit the longest
 *                   description.
 * =======================================================================
 */
void ProfileDialog :: adjustTabWidth()
   {
   if ( !pNotebook->totalPages() )
      return;

   IFont iFont(pNotebook);
   ULONG ulWidth = 0;

   INotebook::Cursor crs(*pNotebook);
   for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
      {
      IString strText = pNotebook->pageSettings(crs).tabText();
      ULONG ulTest = iFont.textWidth(strText);
      if ( ulTest > ulWidth )
         ulWidth = ulTest;
      }

   ulWidth = ulWidth * 6 / 5;
   if ( ulWidth < 50 )
      ulWidth = 50;
   else if ( size().width() )
      {
      ULONG ulMax = size().width() / 3;
      if ( ulWidth > ulMax )
         ulWidth = ulMax;
      }
   ULONG ulHeight = iFont.maxCharHeight() * 3 / 2;
   pNotebook->setMajorTabSize(ISize(ulWidth, ulHeight));
   }

/* =======================================================================
 * FUNCTION NAME     getHelpWindow
 * -----------------------------------------------------------------------
 * DESCRIPTION       Return a pointer to the help window.
 * =======================================================================
 */
IHelpWindow* ProfileDialog :: helpWindow()
   {
   return pHelp;
   }

/* =======================================================================
 * FUNCTION NAME     getProfileNotebook
 * -----------------------------------------------------------------------
 * DESCRIPTION       eturn pointer to client notebook.
 * =======================================================================
 */
INotebook* ProfileDialog :: getProfileNotebook()
   {
   return pNotebook;
   }

/* =======================================================================
 * FUNCTION NAME     getReturnCode
 * -----------------------------------------------------------------------
 * DESCRIPTION       Return a value that indicates how the
 *                   dialog was closed.
 *                   0 = ok button, 1 = cancel button, -3 = abnormally
 * =======================================================================
 */
int ProfileDialog :: getReturnCode()
   {
   return iRC;
   }

/* =======================================================================
 * FUNCTION NAME     ~ProfileDialog
 * -----------------------------------------------------------------------
 * DESCRIPTION       Delete any objects created this class.
 * =======================================================================
 */
ProfileDialog::~ProfileDialog()
{
   delete   pHelp;
   delete   ppbHelp;
   delete   ppbCancel;
   delete   ppbOK;
   delete   pmccButtonExtension;
   delete   ptxExtension;
   delete   pRecPrfl;
   delete   pNotebook;
   delete   pProfile;
   /* delete   pLocale; this object is set to auto delete itself */
   /* delete   pRetSrc; this object is set to auto delete itself */
}
#endif
