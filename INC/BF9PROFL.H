#ifndef BF9PROFL_H
#define BF9PROFL_H

#define STR_DLLNAME      "bf9profl.dll"

/* =======================================================================
 * FILE NAME         BF9PROFL.H
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

extern "C"
{
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

int __stdcall EditBaseRetrievalProfile( HWND handle );

}
#endif
