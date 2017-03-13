#ifndef __TRILAPI_H
#define __TRILAPI_H

/***************************************************************************
	$Workfile:   trilapi.h  $
	$Revision:   1.2  $
		$Date:   01 Dec 1995 13:55:46  $
	  $Author:   bhala  $
	Contains:    Definitions for Trilogy APIs
	Last Modified: $Modtime:   01 Dec 1995 12:56:56  $

  *************************************************************************
	Copyright (C) 1991,1992,1993 Trilogy Development Group, Inc.
	Trilogy Development Group, Inc., Confidential and Proprietary.
	All rights reserved.  This software is subject to copyright protection
	under the laws of the United States and other countries.
  *************************************************************************
	Comments:

***************************************************************************/

/* Using _far _pascal linkage for the engine API for windows */
/* Use _pascal linkage for OS/2 */

#if defined(__MSDOS__) || defined(_MSDOS)
#define FARPASCAL _far _pascal
#elif defined(__OS2__)
#	if defined(__IBMCPP__) || defined(__IBMC__)
#		define FARPASCAL _Pascal
#	else
/* this has to be defined with two underscores because */
/* xvt defines pascal and _pascal as null. */
#		define FARPASCAL __pascal
#	endif
//*******************changed by rd*****************************
#elif defined( __WINDOWS__ )
#define FARPASCAL _System
//*** end of change********************************************
#elif defined(_NT_)
#define FARPASCAL __stdcall
#else
#define FARPASCAL
#endif


#endif

