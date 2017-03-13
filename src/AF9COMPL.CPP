//=======================================================================
// FILE NAME        AF9COMPL.CPP
//
// CLASS NAME       CompleteReconcile
//
// DESCRIPTION      This class is the "Reconciliation not complete"
//                  dialog of the Base Retrieval CPF which is displayed
//                  when the user selects the "End reconciliation" option
//                  without completely resolving the system.
//
// Programmer:      Erica Wilson  06/24/1998
//=======================================================================
// Copyright (c) International Business Machines Corp. 1998
// All rights reserved
//=======================================================================

#include <icmdhdr.hpp>                  // ICommandHandler
#include <iframe.hpp>                   // IFrameWindow
#include <ihelp.hpp>                    // IHelpWindow
#include <ikeyhdr.hpp>                  // IKeyboardHandler
#include <ipushbut.hpp>                 // IPushButton
#include <iradiobt.hpp>                 // IRadioButton
#include <isysmenu.hpp>                 // ISystemMenu

#include "af9compl.hpp"                 // CompleteReconcile
#include "af9main.hpp"                  // MainWindow
#include "mainw.hpp"                    // XWindow
#include "af9apply.h"                   // Resource ids

//=======================================================================
// FUNCTION NAME    CompleteReconcile
//
// DESCRIPTION      Construct an instance of the CompleteReconcile class.
//=======================================================================

CompleteReconcile :: CompleteReconcile( IFrameWindow* pParent, MainWindow* pOwner )
                   : IFrameWindow( IResourceId(IDD_ReconcileNotCompl), NULL, pParent )
   {

   // Save parameters;

   pMain = pOwner;
   this->pParent = pParent;

   // Initiate flag(s)

   bContinue = false;

   // Attach the help window.

   pOwner->helpWindow()->setAssociatedWindow(this);

   // Add handlers for command and keyboard messages.

   ICommandHandler::handleEventsFor(this);
   IKeyboardHandler::handleEventsFor(this);

   // Create control objects.

   buildControls();

   // Set the dialog to self-destruct when closed.

   // setAutoDeleteObject();
   }

//=======================================================================
// FUNCTION NAME    command
//
// DESCRIPTION      Handle command messages.
//=======================================================================

Boolean CompleteReconcile :: command(ICommandEvent& cmdEvt)
   {

   switch ( cmdEvt.commandId() )
      {

      // User selected OK.

      case IDC_PB_OK:
         {
         if (vPlacement_s390)   //53915 MAMT 28/02/2001 begin new logic
           {
            pMain->bResolve = false;
            pMain->bUseMRDB = false;
            pMain->bUseAAS = false;
            vPlacement_s390 = false;
           }
         else                   //53915 MAMT 28/02/2001 end new logic 
           {                       
            if ( prbContinue->isSelected() )
               bContinue = true;

            else if( prbMRDB->isSelected() )
               {
               pMain->bResolve = false;
               pMain->bUseMRDB = true;
               pMain->bUseAAS = false;
               XWindow* p = (XWindow*)pParent;
               p->iUseAASMRDBUserChoice = 1;       //edw12/29/1999
               }
            else if ( prbAAS->isSelected() )
               {
               pMain->bResolve = false;
               pMain->bUseMRDB = false;
               pMain->bUseAAS =  true;
               XWindow* p = (XWindow*)pParent;
               p->iUseAASMRDBUserChoice = 2;       //edw12/29/1999
               }
            else if ( prbQuit->isSelected() )
               {
               pMain->bResolve = false;
               pMain->bUseMRDB = false;
               pMain->bUseAAS = false;
               }
           } //53915 MAMT 28/02/2001
         close();
         //pMain->bLock = false;
         //pMain->setFocus();
         break;
         }

      // User selected Cancel.

      case IDC_PB_Cancel:
         bContinue = true;
         removeFromWindowList();
         close();
         //pMain->bLock = false;
         //pMain->setFocus();
         break;

      default:
         return false;
      }
   return true;
   }

//=======================================================================
// FUNCTION NAME    systemCommand
//
// DESCRIPTION      Handle system command events.
//=======================================================================

Boolean CompleteReconcile :: systemCommand(ICommandEvent& cmdEvt)
   {

   // Hide this window instead of closing it.

   if ( cmdEvt.commandId() == ISystemMenu::idClose )
      {
      postEvent(IWindow::command, IDC_PB_Cancel);
      return true;
      }
   return false;
   }

//=======================================================================
// FUNCTION NAME    virtualKeyPress
//
// DESCRIPTION      Handle the Escape key.
//=======================================================================

Boolean CompleteReconcile :: virtualKeyPress(IKeyboardEvent& keyEvt)
   {
   if ( keyEvt.virtualKey() == IKeyboardEvent::esc )
      {
      postEvent(IWindow::command, IDC_PB_Cancel);
      return true;
      }
   return false;
   }


//=======================================================================
// FUNCTION NAME    buildControls
//
// DESCRIPTION      Instantiate the various dialog controls.
//=======================================================================

void CompleteReconcile :: buildControls()
   {
   prbContinue = new IRadioButton(IDC_RB_ReconcileNotCompl_Contd, this);
   prbContinue->select();

   prbMRDB = new IRadioButton(IDC_RB_ReconcileNotCompl_MRDB, this);
   if ( pMain->bMultSysVPDTran || pMain->bMultSysStackedMRDB )
      prbMRDB->disable();

   prbAAS = new IRadioButton(IDC_RB_ReconcileNotCompl_AAS, this);
   if ( pMain->bS390 )
      prbAAS->disable();

   prbQuit = new IRadioButton(IDC_RB_ReconcileNotCompl_Quit, this);

   ppbOK = new IPushButton(IDC_PB_OK, this);
   ppbCancel = new IPushButton(IDC_PB_Cancel, this);

   ppbHelp = new IPushButton(IDC_PB_Help, this);
   ppbHelp->enableHelp();
   ppbHelp->disableMouseClickFocus();

   // MAMT 53915 28/02/2001 begin new code
   char input[256], default_data[256]; //flw Usability
   strcpy(input, STR_Placement_s390);
   strcpy(default_data, STR_Placement_Ns390);
   setupKeyData1(input, default_data);    // Get placement value value from registry
   if ( !strcmp( default_data, STR_Placement_Ys390) )
     {
       ppbOK->click();
       vPlacement_s390 = true;
       HKEY key;                                                                                                                
       if (RegOpenKeyEx (HKEY_CURRENT_USER, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)      
        {
         RegSetValueEx(key, STR_Placement_s390, 0, REG_SZ, (CONST BYTE*)STR_Placement_Ns390, IString(STR_Placement_Ns390).length()+1);
        }
       RegCloseKey (key);
     }
   else 
       vPlacement_s390 = false;
   
   // MAMT 53915 28/02/2001 end new code

   }


//=======================================================================
//  FUNCTION NAME setupKeyData1 53915 
//
//  DESCRIPTION   searches for a given key ( input ) in the registry.  If
//                the key doesn't exist, it is created and assigned the
//                given default value ( default_data ).  If the key exists,
//                its value is retrieved and written to 'default_data'.
// PROGRAMMER     James Hsieh
//=======================================================================
boolean CompleteReconcile :: setupKeyData1 (char * input, char *default_data)
   {
   IString QryData;
   DWORD ValueType, DataSize = 1024;
   char buf[35];
   char  *Data = (char *)malloc(1024);
   char  *p, *newData;
   long retval, RetVal, iDataNeeded;
   HKEY key;

   boolean rc = true;

   if (RegOpenKeyEx (HKEY_CURRENT_USER, REGPATH, 0, KEY_ALL_ACCESS, &key) == ERROR_SUCCESS)
      {
      retval = RegQueryValueEx(key, input, NULL, &ValueType, (LPBYTE)Data, &DataSize);

      p = _ltoa(retval, buf, 10);

      if (retval == ERROR_SUCCESS)
         strcpy(default_data, Data);
      else
         {
         if (retval == ERROR_FILE_NOT_FOUND)  /* Set registry to default_data value */
            {
            RegSetValueEx(key, input , 0, REG_SZ, (CONST BYTE*)default_data, strlen(default_data)+1);
            RegCloseKey (key);
            }
         else
            {
            if (retval == ERROR_MORE_DATA)  /* reissue RegQuery call */
               {
               iDataNeeded = DataSize;
               newData = new char(iDataNeeded*2 );
               RetVal = RegQueryValueEx(key, input /* STR_Profile_FastPath */, NULL, &ValueType, (LPBYTE)newData, (LPDWORD) &iDataNeeded);
               if (RetVal == ERROR_SUCCESS)
                  {
                  strcpy(default_data, newData);
                  }
               else
                  {
                  rc = false;
                  strcpy(default_data, "Query Error");
                  }
               delete newData;
               }
            }
         }
      RegCloseKey (key);
      }
   else
      {
         //cout << "Unable to open key" << endl;
         rc = false;
      }

   free(Data);
   return rc;

   }


//=======================================================================
// FUNCTION NAME    ~CompleteReconcile
//
// DESCRIPTION      Delete any objects created by the constructor.
//=======================================================================

CompleteReconcile :: ~CompleteReconcile()
   {
   delete prbContinue;
   delete prbMRDB;
   delete prbAAS;
   delete prbQuit;
   delete ppbOK;
   delete ppbCancel;
   delete ppbHelp;
   }
