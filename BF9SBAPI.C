/*
=======================================================================
 FILE NAME        BF9SBAPI.C

 DESCRIPTION
=======================================================================
 Copyright (c) International Business Machines Corp. 1996
 All rights reserved
*/

#define INCL_WINSHELLDATA
#include <os2.h>

#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winreg.h>                     // Registry functions

/*
=======================================================================
FUNCTION RetrieveAPI
=======================================================================
*/


//APIRET _Pascal RETRIEVEAPI(HWND hwnd, LONG* retrID,
//                          char* retrFile, LONG retrFileLen)
extern APIRET __stdcall RETRIEVEAPI(HWND hwnd, LONG* retrID,
                       char* retrFile, LONG retrFileLen)
   {
   LONG lRC;
   char szFile[512];
   DWORD DataSize = 512;
   HKEY hkResult;
   char strValue[8]="RetFile";
   LONG szLen;
   STARTUPINFO          sui;
   PROCESS_INFORMATION  pi;

   *retrID = 0;
    sui.cb               = sizeof (STARTUPINFO);
    sui.lpReserved       = 0;
    sui.lpDesktop        = NULL;
    sui.lpTitle          = NULL;
    sui.dwX              = 0;
    sui.dwY              = 0;
    sui.dwXSize          = 0;
    sui.dwYSize          = 0;
    sui.dwXCountChars    = 0;
    sui.dwYCountChars    = 0;
    sui.dwFillAttribute  = 0;
    sui.dwFlags          = 0;
    sui.wShowWindow      = 0;
    sui.cbReserved2      = 0;
    sui.lpReserved2      = 0;

   //edw CreateProcess (NULL, "BF9BRET Software\\VB and VBA Program Settings\\SBO\\Settings",
   //edw                NULL, NULL, FALSE, 0, NULL, NULL, &sui, &pi );
   CreateProcess (NULL, "BF9BRET SBOS2",                                     //edw 08-12-97
                  NULL, NULL, FALSE, 0, NULL, NULL, &sui, &pi );             //edw 08-12-97

   //lRC = system("BF9BRET SBOS2");
   //if ( lRC < 1 )
   //   return 1;
   memset(szFile, 0, sizeof(szFile));

   //lRC = PrfQueryProfileString(HINI_USERPROFILE, "SBOS2", "RetFile",
   //                            NULL, szFile, sizeof(szFile));

   lRC = RegOpenKeyEx( HKEY_CURRENT_USER,  // handle of open key
                       "Software\\VB and VBA Program Settings\\SBO\\Settings",// address of name of subkey to open
                       0,                  // reserved
                       KEY_READ,           // security access mask
                       &hkResult);         // address of handle of open key
   if (lRC != ERROR_SUCCESS )
      return 1;

   lRC = RegQueryValueEx(hkResult,        // handle of key to query
                         strValue,        // address of name of value to query
                         NULL,            // reserved
                         NULL,            // address of buffer for value type
                         (LPBYTE)&szFile, // address of data buffer
                         (LPDWORD)&DataSize);  // address of data buffer size
   if (lRC != ERROR_SUCCESS)
      return 1;

   RegCloseKey(hkResult);


   // if (!lRC)
   szLen = (LONG)strlen(szFile);
   if ( !szLen)
      return 1;

   //PrfWriteProfileString(HINI_USERPROFILE, "SBOS2", "RetFile", NULL);
   lRC = RegOpenKeyEx(HKEY_CURRENT_USER,  // handle of open key
                      "Software\\VB and VBA Program Settings\\SBO\\Settings",// address of name of subkey to open
                      0,                  // reserved
                      KEY_WRITE,          // security access mask
                      &hkResult);         // address of handle of open key
   if (lRC != ERROR_SUCCESS )
      return 1;

   memset(szFile, 0, sizeof(szFile));
   DataSize = 1;
   lRC = RegSetValueEx(hkResult,            // handle of key to query
                       strValue,            // address of name of value to query
                       0,                   // reserved
                       REG_SZ,              // address of buffer for value type
                       (LPBYTE)szFile,      // address of value data
                       DataSize);           // size of value data
   if(lRC != ERROR_SUCCESS)
     return 1;

   RegCloseKey(hkResult);

   if ( szLen > retrFileLen )
      return 2;
   *retrID = 1;
   strcpy(retrFile, szFile);
   return 0;
   }
