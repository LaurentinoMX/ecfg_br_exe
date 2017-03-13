#ifndef _AB_EUOPIPRT_HPP_
#define _AB_EUOPIPRT_HPP_

#define INCL_SPLERRORS
#define INCL_SPL
#define INCL_DOSERRORS           //ERROR_MORE_DATA
#define INCL_SPLDOSPRINT
#define INCL_PM
#include <os2.h>

#include <fstream.h>
#include <stdio.h>
#include <istring.hpp>
#include "euoutil.hpp"

class IPrinter;
class ResMessage;

class EUOPPrint : public ResMessage
{
 char *Buf;
//IPrinter  *prtr;
 IString   strQueue
         , strDriver
         , strFont
         , strErr;
  unsigned cLines
         , cPoints
         , cChars;
  PRINTDLG  pd;       //edw

  Boolean  fISpool(char *,char *,char *, Boolean);
  IPrinter * getPrinter();
  Boolean  setFont(IPrinter *);

 public:

//  Boolean   ispool(char * szBuf,char * JobName);
  Boolean   fSpool(char * szFilename,char * szJobName,
                   char * szPageBreak = 0,Boolean fDelete = true);
  Boolean  spool(char * szBuf,char * JobName,char * szPageBreak = 0, Boolean fDelete = true);
  IString   printerQueue() { return strQueue; }
  IString   font() { return strFont; }
  IString   lastErrorText() { return strErr;}
  unsigned  lineCount();
  unsigned  charCount();
  unsigned  points() { return cPoints; }
  Boolean   setCounts();
  EUOPPrint();
  ~EUOPPrint();
  Boolean init();
};
#pragma pack()
#endif

