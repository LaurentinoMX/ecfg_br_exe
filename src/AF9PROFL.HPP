//=======================================================================
// FILE NAME        AF9PROFL.HPP
//
// CLASS NAME       Profile
//
// DESCRIPTION      This class stores the user profile for the Order
//                  Apply CPF.
// CHANGE LOG
// Date        Programmer        Description
// ======================================================================
// 10/18/1999  E. Wilson         Add bShowS390Mismatches instance variable
// 10/18/1999  E. Wilson         Add setupKeyData method prototype
// E. Wilson   01/21/2002        F62721 Added support for use of this class by
//                               the Base Retrieval profile DLL (BF9PROFL.DLL)
//                               Added functions for moving the location of
//                               the registry keys
// 10/07/2003  P. Sanchez        New prototype function added in the profile -
//      D80040                   to move the RM2 Registry Entries located in -
//                               the wrong path.
// 06/06/2006  E. Reyes          Added pointer to "Retrieval source" class
//
// 12/12/2009  ELRR   F133122    Added Boolean variable bFirefoxSupported and removed
//                    Req1547    variable bNetscapeSupported
//
//  25/06/2014  GTL   Req1702    Added Boolean variable bChromeSupported.
//=======================================================================
// Copyright (c) International Business Machines Corp. 1996
// All rights reserved
//=======================================================================

#ifndef AF9PROFL_HPP
#define AF9PROFL_HPP

#define INCL_NOBASEAPI
#define INCL_NOPMAPI
#include <os2.h>

#include <idate.hpp>                    // IDate
#include <ipoint.hpp>                   // IPoint
#include <istring.hpp>                  // IString

#ifndef BR_PROFILE  /* F62721 EDW 01/21/2002 */
  #include "af9main.hpp"                // MainWindow
#endif
#include "af9apply.h"                   // Resource ids

class Profile
   {
   public:
   Profile();
   ~Profile();
   IPoint position[WINDOWCOUNT];        // Default positions
   ISize size[WINDOWCOUNT];             // Default sizes
   IString strMRI;                      // Name of MRI DLL
   IString strLocale;                   // Name of locale DLL
   IString strBrowsePath;               // Path for browsing files
   IString strGeneratePath;             // Path for generating files
   IString strRetApp;                   // Name of host interface application for retrieving files
   IDate dateSelect;                    // Order select date
   SHORT sSortFiles;                    // Files sort option
   SHORT sSortOrders;                   // Orders sort option
   Boolean bHardware;                   // Retrieve hardware
   Boolean bSoftware;                   // Retrieve software
   Boolean bShowS390Mismatches;         // Show S390 reconciliation mismatches
   Boolean load(IString& strName);      // Load settings
   Boolean save(IString& strName);      // Save settings
   void BrowserSupported();             // Usability Determine if Browser(s) Supported
   void PCOMMSupported();               // Determine if PCOm supported
   Boolean bFastPath;                   // Usability  Fast Path flag
   Boolean bSort;                       // ZOR IC01552
   Boolean bSascending;                 // ZOR IC01552
   Boolean bExpanded;                   // ZOR 48548
   Boolean bTile;                       // 47971 MAMT 21/08/2000
   Boolean bMRPD;                       // Usability  MRPD flag
   Boolean bAutoRec;                    // Auto Reconcile flag  47698
   Boolean bProcRPO;                    // Process RPO flag 47700
   IString strBrowser;                  // Usability  Browser preference
   IString strProdFamily;                  // Product Family - 47701
   IString strRetSources;               // F106765 RERC 05/30/2006  Interface reference
   Boolean bIESupported;                // Usability number of browsers the user desktop supports
   Boolean bFirefoxSupported;           // Usability number of browsers the user desktop supports

   //GTL Req1702 BEGIN
   Boolean bChromeSupported;           // Usability number of browsers the user desktop supports
   //GTL Req1702 END
   
// Boolean bNetscapeSupported;          // Usability number of browsers the user desktop supports
   Boolean bPassportSupported;          // IC42054 - Is Passport loaded on  user desktop
   Boolean bEconfig;                    // Usability - 38935 - BR called from E-Config
   Boolean bPComSupported;              // 44549 - flw
   Boolean bRefreshList;                // Refresh previously retrieved data list    F62721 EDW 01/21/2002
   Boolean bRefreshLocale;              // Load newly selected language resource     F62721 EDW 01/21/2002
   boolean setupKeyData(char*, char*);  // Add key values to the registry
   int static copyRegKeys(HKEY hKey,
                   HKEY hNewKey);       // Copy the registry keys to a new location  F62721 EDW 01/21/2002
   int static markRegKeysAsDeleted
                    (IString oldKey);   // Mark an old set of keys as 'deleted'      F62721 EDW 01/21/2002

   void static changeLocationOfKeys();  // Move registry keys to new location        F62721 EDW 01/21/2002
   // D80040 PSC 10/07/2003 start
   // New prototype function needed to move the RM2 Registry Entries located in the wrong path.
   // This new function will allow to re-locate the RM2 Registry Entries where it must be.-----
   void static changeLocationOfRM2Keys();
   // D80040 PSC 10/07/2003 end
   RetrSource* pRetrSource;             // F106765 RERC 05/29/2006
    };
#endif
