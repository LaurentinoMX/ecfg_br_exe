/*---------------------------------------------------------
* This file was created for Jack Tsai on 05/28/97.
* VisualAge for C++ Version 3.5, (C) Copyright IBM Corp. 1996
*---------------------------------------------------------*/

//#pragma library("w95ckcfr.lib")


#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stddef.h>

#define  PSZ                  LPSTR
#define  WAPIRET              unsigned long
#define  WAPIENTRY            EXPENTRY

WAPIRET WAPIENTRY cfrcksum(PSZ pszFileName, PSZ pszCookie);


void main(int argc, char* argv??(??))
{
// DosExit(EXIT_PROCESS, argc > 1? cfrcksum(argv??(1??), "82151") : 0);
  if ( argc > 1)
     {
     //unsigned long ulChecksum = cfrcksum(argv??(1??), "82151");
     //exit( ulChecksum );
     exit(0);
     }
  exit(0);
}
