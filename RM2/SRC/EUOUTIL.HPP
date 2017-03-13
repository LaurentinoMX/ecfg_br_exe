#ifndef _AB_EUOUTIL_HPP_
#define _AB_EUOUTIL_HPP_

#include <fstream.h>
#include <io.h>
#include <stdio.h>

#define INCL_ICOLOR
#define INCL_IDATE
#define INCL_IENTRYFIELD
#define INCL_IFRAMEWINDOW
#define INCL_IHANDLER
#define INCL_IMESSAGETEXT
#define INCL_ISPINBUTTON
#define INCL_ISTRING
#include "uicl.hpp"

#define ALMAX(a,b)        (((a) > (b)) ? (a) : (b))
#define ALMIN(a,b)        (((a) < (b)) ? (a) : (b))
#define ISDIGIT(c)   ((c) >= '0' && (c) <= '9')
#define DIGIT(c)     unsigned ((c) - '0')

//unsigned long fCheckSum(const char * file);
Boolean cfrValiDateYYYYMMDDFields( const char * p, unsigned *pM
                                  ,unsigned *pD ,unsigned *pY);
Boolean cfrDateYYYYMMDD(const char * p,IDate & date);
int     cfrInt(const char * Buf, unsigned BufLen);
int     cfrUnsigned(const char * Buf, unsigned BufLen);
Boolean writableFile(char * name);
Boolean readableFile(char * name);
const char * findFileName(const char *szPath);
char *  mystrncpy(char * Target,const char * Source , unsigned len);
unsigned myantou(const char *source, unsigned n);
int     myWildCardSortCompare(const char * sz1,const char * sz2);
char *  mystrstrip(char *);
char *  MakeFileName(char * Target,const char *Name,const char *Ext);
char *  MakeFileSpec(char * Target,const char * Path,const char *Name,const char *Ext);
IString MakeFileSpecStr(const char * Path,const char *Name, const char * Ext);
char *  SetFileExtension(char * File ,const char *Ext);
char *  myrearstrstripchar(char * str,char c);
char *  mystrrearstrip(char *str);
char *  stringreplace(char *String, char *From, char *To, int   MaxLen);
char *  stringpaddchar(char *string,char c, int iTill);
char *  CFRPaddCopySZ(char * Target,const char *szSource,unsigned TargetLen,char cPadd = ' ');
char *  CFRCopySZPadd(char * Target,const char *szSource,unsigned TargetLen,char cPadd = ' ');
int     compSerialNo(const char *, const char *);
char * ensureLF(char *str);
int    CFRecType(const char *pc);
void   setCFRecType(char *pc, unsigned nmbr);
void   FileNameOnly(const char *File,IString & );
Boolean  fWriteBIString(FILE *,const IString &, unsigned * pBytesAccumulator);
Boolean  fReadBIString(FILE *,IString &);
void     setSPBBackgroundColor(ISpinButton &,const IColor &);
IRectangle MyClientRect(IFrameWindow *);
IString PathNameOnly(const char *FileName);

#pragma pack(4)
class Tellem
{
 ofstream * pOut;
 Boolean    fEmptyStream;
 IString    StreamFile;
 IString    MsgFile;
 IString    strErr;
 IString    strMsg;
 unsigned   ErrNo;

  void      reset();
public:

  Tellem() : pOut(0),ErrNo(0),fEmptyStream(true){}
  ~Tellem() { reset();}
  Boolean    set(const char * msgFile = 0,const char * OutStream = 0);
  unsigned     errorNumber() { return ErrNo;}
  const char * errorText() { return (const char *)strErr;}
  Boolean    say ( Boolean fError
                  ,unsigned long messageId
                  ,const char*   textInsert1 = 0
                  ,const char*   textInsert2 = 0
                  ,const char*   textInsert3 = 0
                  ,const char*   textInsert4 = 0
                  ,const char*   textInsert5 = 0
                  ,const char*   textInsert6 = 0
                  ,const char*   textInsert7 = 0
                  ,const char*   textInsert8 = 0
                  ,const char*   textInsert9 = 0 );
  const char *    message (  unsigned long messageId
                            ,const char*   textInsert1 = 0
                            ,const char*   textInsert2 = 0
                            ,const char*   textInsert3 = 0
                            ,const char*   textInsert4 = 0
                            ,const char*   textInsert5 = 0
                            ,const char*   textInsert6 = 0
                            ,const char*   textInsert7 = 0
                            ,const char*   textInsert8 = 0
                            ,const char*   textInsert9 = 0 );
  Boolean    say (const IString &,unsigned ErrorId = 0);
  Boolean    say (const char *, unsigned ErrorId = 0);
};

//#if 0
class ResMessage
{
   IString str;
   char * Buf;
 public:
   ResMessage() { Buf = new char[1024];}
   ~ResMessage() { delete[] Buf;}
   IString & getString( unsigned long Id
                       ,const char*   textInsert1 = 0
                       ,const char*   textInsert2 = 0
                       ,const char*   textInsert3 = 0
                       ,const char*   textInsert4 = 0
                       ,const char*   textInsert5 = 0
                       ,const char*   textInsert6 = 0
                       ,const char*   textInsert7 = 0
                       ,const char*   textInsert8 = 0
                       ,const char*   textInsert9 = 0 );
};

class MyWMHelpHandler : public IHandler
{
  public:
    MyWMHelpHandler() : IHandler() {}
   ~MyWMHelpHandler(){}
    Boolean           dispatchHandlerEvent(IEvent&);
  protected:
    virtual Boolean   wmhelpMessage(IEvent &) { return false;}
} ;

//#endif
#pragma pack()

#endif






