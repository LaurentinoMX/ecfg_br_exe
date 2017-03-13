/* =======================================================================
 * FILE NAME         AF9PFAPI.CPP
 * -----------------------------------------------------------------------
 * DESCRIPTION       Defines the external API(s) that are exported to 
 *                   BF9PROFL.DLL clients
 *  
 *
 * MODIFICATION LOG:
 * -----------------------------------------------------------------------
 * Date        Programmer        Description
 * =======================================================================
 * 2002-01-21  EDW               Initial implementation (F62721)
 *
 * =======================================================================
 * Copyright (c) International Business Machines Corp. 2002
 * All rights reserved
 * =======================================================================
 */
#ifdef BR_PROFILE
#include <iapp.hpp>                    /* IApplication                  */
#include <istring.hpp>                 /* IString                       */
#include <imsgbox.hpp>                 /* IMessagebox                   */
#include "af9pfdlg.hpp"                /* ProfileDialog                 */
#include "bf9profl.h"                  /* Exported function definitions */

/* ----------------------------------------------------------------------- 
 * local function prototypes
 * -----------------------------------------------------------------------
 */
IString setExecutionPath();


/* =======================================================================
 * FUNCTION NAME     EditBaseRetrievalProfile
 * -----------------------------------------------------------------------
 * DESCRIPTION       Creates an instance of the ProfileDialog class which 
 *                   contains the functionality which allows a client to 
 *                   externally managing the Base Retrieval CPF preferences 
 *                   in standalone mode.  This same API is also used  
 *                   inside Base Retrieval for preference management 
 * -----------------------------------------------------------------------
 * EXPORTED TO       BF9PROFL.DLL
 *
 * PARAMETER(S)      handle   reference to the Win32 window handle which 
 *                            will be used as the parent for the 
 *                            ProfileDialog 
 *
 * RETURN VALUE(S)   0        profile saved (OK button clicked)
 *                   1        profile not saved (Cancel button clicked)
 *                  -1        application resources could not be loaded
 *                  -2, -3    other application errors (messagebox displayed)
 * =======================================================================
 */

int __stdcall EditBaseRetrievalProfile( HWND handle )
{
   IWindow* winParent = (IWindow*)NULL;
   ProfileDialog* dialog = (ProfileDialog*)NULL;
   boolean bDeleteParentWinHandle = false;
   int iRC = 0;

   try
   {
      /* Set program's execution path */

      IString strPath = setExecutionPath();
      ProfileDialog::strExecPath = strPath;

      /* Load application resources */

      if ( !ProfileDialog::loadResources() )
      {
         return -1;
      }

      /* Set the parent window for the ProfileDialog */
      
      if ( ( ( IWindowHandle )handle ).isValid() )
      {
         winParent = (IFrameWindow*)IWindow::windowWithHandle( IWindowHandle( handle ) );
         if (winParent == NULL)
         {     
            winParent = new IFrameWindow( IWindowHandle( handle ) );
            bDeleteParentWinHandle = true;
         }
      }
      else
      {
         winParent = IWindow::desktopWindow();
      }

      /* Set global attributes for ProfileDialog */

      INotebook::setDefaultStyle(INotebook::roundedTabs |
                                 INotebook::backPagesBottomRight |
                                 INotebook::solidBinding |
                                 INotebook::statusTextLeft |
                                 INotebook::tabTextCenter |
                                 INotebook::majorTabsTop |
                                 IWindow::visible);

      ICoordinateSystem::setApplicationOrientation(ICoordinateSystem::originUpperLeft);


      /* Create and display a ProfileDialog object */

      dialog = new ProfileDialog( IDW_ProfileDialog, winParent );
      dialog->moveTo( IPoint(winParent->position().x() + 30, 
                             winParent->position().y() + 50));
      dialog->showModally();
      iRC = dialog->getReturnCode();

      /* The setFocus Windows API does not work with eConfig's SCPortfolio.
         Brute force is required to bring it back without flicker.
       */

      BringWindowToTop( winParent );

      /* Deallocate memory */

      delete dialog;
      if ( bDeleteParentWinHandle )
         delete winParent;
   }
   catch ( IException& exc )
   {
      IString strError = (IString)exc.text();
      IMessageBox m(winParent);
      m.show( strError, IMessageBox::information );
      return -2;
   }
   return iRC;
}

/* =======================================================================
 * FUNCTION NAME     setExecutionPath
 * -----------------------------------------------------------------------
 * DESCRIPTION       Returns the program's execution path 
 * =======================================================================
 */
IString setExecutionPath()
{
   IString strExecPath = "";

   char strE[256], strE1[256];
   LPTSTR *strE2 = (LPSTR*)strE1;
   SearchPath(NULL, STR_DLLNAME, NULL, 255, strE, strE2);
   strExecPath = strE;
   ULONG ulOffset = strExecPath.lastIndexOf(STR_PathDelimiter);
   if (ulOffset)
      strExecPath.remove(ulOffset+1);
   else
      strExecPath = "";
   return strExecPath;
}
#endif
