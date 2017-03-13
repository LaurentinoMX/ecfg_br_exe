     /********************************************************************
      *
      *   Source File:  OS2WIN.h
      *   Author:       jack tsai
      *   Module:       OS/2 to Windows 95
      *   Program Desc: matching names
      *   Date:         09/09/96
      *   Language:     VisualAge for C++ for Windows 95
      *
      ********************************************************************/

#ifndef __OS2WIN__H
#define __OS2WIN__H


#include <windows.h>

typedef HANDLE                HDIR;
typedef unsigned long         APIRET;
typedef ULONG                 HEV;

#define FILEFINDBUF3          WIN32_FIND_DATA
#define PFILEFINDBUF3         LPWIN32_FIND_DATA

#define DosFindFirst          FindFirstFile
#define DosFindClose          FindClose
#define DosCopy               CopyFile
#define DosDeleteDirectory    RemoveDirectory

#define DID_FILE_DIALOG       FILEOPENORD
#define DID_FILENAME_TXT      stc3
#define DID_FILENAME_ED       edt1
#define DID_FILTER_TXT        stc2
#define DID_FILES_LB          lst1
#define DID_DIRECTORY_LB      lst2
#define DID_FILTER_CB         cmb1
#define DID_DRIVE_CB          cmb2
#define DID_OK_PB             IDOK
#define DID_CANCEL_PB         IDCANCEL
#define DID_CANCEL            IDC_PB_Cancel
#define DID_HELP_PB           IDC_PB_Help


#include <ibase.hpp>
#include <iapp.hpp>
#include <ifont.hpp>
#include <istring.hpp>
#include <imsgbox.hpp>
#include <icoordsy.hpp>
#include <ipushbut.hpp>
#include <ievtdata.hpp>


#include "pmwin9.h"

#endif //__OS2WIN__H

