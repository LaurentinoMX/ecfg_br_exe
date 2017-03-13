/****************************** Module Header ******************************\
*
* Module Name: PMWIN.H
*
* Port OS/2 Presentation Manager Window Manager TO Windows 95
*
\***************************************************************************/
#ifndef __pmwin0__H
#define __pmwin0__H

#include "af9apply.h"

#define DID_OK                IDC_PB_OK
typedef VOID *MPARAM;    /* mp    */
typedef MPARAM *PMPARAM;   /* pmp   */
#define SHORT2FROMMP(mp)           ((USHORT)((ULONG)mp >> 16))
#define MPFROM2SHORT(s1, s2)       ((MPARAM)MAKELONG(s1, s2))


#endif  // __pmwin0__H

