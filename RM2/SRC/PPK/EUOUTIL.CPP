#define INCL_WINWINDOWMGR
#define INCL_WIN
#include <os2.h>

#include <isynonym.hpp> // Definition of Boolean etc
#include <stdio.h>
#include <string.h>

#define INCL_IAPPLICATION
#define INCL_IRESOURCELIBRARY
#include "uicl.hpp"

#include "euoutil.hpp"
#define LOADSTRING(string)              IApplication::current().userResourceLibrary().loadString(string)//jt,5/28/97

#define ISCRLF(c)    ((c) == '\n' || (c) == '\r')
/* edw-11/17/98
#if 0
//extern "C"
//  {
   extern unsigned long _System cfrcksum(char*, char*);
// extern unsigned long cfrcksum(char*, char*);
   #pragma import(cfrcksum,,"OS2CKCFR",0)
//  }

unsigned long fCheckSum(const char * file)
{ return cfrcksum((char *)file, "82151"); }
#endif
*/
Boolean cfrValiDateYYYYMMDDFields( const char * p, unsigned *pM
                                  ,unsigned *pD ,unsigned *pY)
{
 if (p                  &&
     ISDIGIT(*p)        &&
     ISDIGIT(*(p+1))    &&
     ISDIGIT(*(p+2))    &&
     ISDIGIT(*(p+3))    &&
     ISDIGIT(*(p+4))    &&
     ISDIGIT(*(p+5))    &&
     ISDIGIT(*(p+6))    &&
     ISDIGIT(*(p+7))) {
   *pY = (DIGIT(p[0]) * 1000) +
         (DIGIT(p[1]) * 100)  +
         (DIGIT(p[2]) * 10)   +
         DIGIT(p[3]);
   *pM = (DIGIT(p[4]) * 10) + DIGIT(p[5]);
   *pD = (DIGIT(p[6]) * 10) + DIGIT(p[7]);
   return IDate::isValid(IDate::Month(*pM),*pD,*pY);
   }
 return false;
}

Boolean cfrDateYYYYMMDD(const char * p,IDate & date)
{
 unsigned m,d,y;
 if (cfrValiDateYYYYMMDDFields(p,&m,&d,&y)) {
   date = IDate(IDate::Month(m),d,y);
   return true;
   }
 return false;
}

int cfrInt(const char * Buf, unsigned BufLen)
{
 return IString::stripBlanks(IString(Buf,BufLen,'\0')).asInt();
}

int cfrUnsigned(const char * Buf, unsigned BufLen)
{
 return IString::stripBlanks(IString(Buf,BufLen,'\0')).asUnsigned();
}

const char * findFileName(const char *szPath)
{
 if (!szPath) return szPath;
 const char *sz;
 for (sz = szPath ; *sz ; sz++){}
 for ( ; sz >= szPath && *sz != '\\' && *sz != ':' ; sz--){}
 return ++sz;
}

IString PathNameOnly(const char *FileName)
{
 IString str(FileName,(unsigned)(findFileName(FileName)-FileName));
 str.stripTrailing('\\');
// IString str(FileName);
// str = str.remove(str.lastIndexOf('\\'));
 return str;
}

char * mystrncpy(char * Target,const char * Source , unsigned len)
{
 if (Target)
   if (Source && len) {
     strncpy(Target,Source,len);
     Target[len] = '\0';
     }
   else *Target = '\0';
 return Target;
}

char *  mystrstrip(char *str)
{
 if (str && *str)
   strcpy(str,&str[strspn(str," ")]);
 if (str && *str) {
   for (char * pc = str + strlen(str);
        pc > str && ( *(pc-1) == ' ' || ISCRLF(*(pc-1))) ;pc--){}
   *pc = 0 ;
   }
 return str;
}

char *  mystrrearstrip(char *str)
{
 if (str && *str) {
   for (char * pc = str + strlen(str);
        pc > str && ( *(pc-1) == ' ' || ISCRLF(*(pc-1))) ;pc--){}
   *pc = 0 ;
   }
 return str;
}

char *  myrearstrstripchar(char * str,char c)
{
 if (str && *str) {
   for (char * pc = str + strlen(str);
        pc > str && ( *(pc-1) == c) ;pc--){}
   *pc = 0 ;
   }
 return str;
}

char * MakeFileName(char * Target,const char *Name,const char *Ext)
{
 char *p;
// mystrncpy(Target,Name,8);
 strcpy(Target,Name);
 if (!(p = strchr(Target,'.'))) {
   p = Target + strlen(Target);
   *p = '.';
   }
 mystrncpy(++p,Ext,3);
 return Target;
}

char * SetFileExtension(char * File ,const char *Ext)
{
 char *p = (char *)findFileName(File);
 if (p) {
   char *p1 = strchr(p,'.');
   if (!p1) {
     p1 = p + strlen(p);
     *p1 = '.';
     }
   mystrncpy(++p1,Ext,3);
   }
 return File;
}

char * MakeFileSpec(char * Target,const char * Path,const char *Name,
                    const char * Ext)
{
 char *p = Target;
 if (Path) {
   strcpy(Target,Path);
   if ((p = Target + strlen(Target) - 1) >= Target)
     if (*p != '\\')
       *(++p) = '\\';
   p++;
   }
 MakeFileName(p,Name,Ext);
 return Target;
}

IString MakeFileSpecStr(const char * Path,const char *Name, const char * Ext)
{
 IString str(Path), strName(Name), strExt(Ext);
 if (strName.length()) {
   if (str.length() && (str[str.length()] != '\\')) // IString's [] is not zero based
     str += "\\";
   str += strName;
   if (strExt.length()) {
     if (str[str.length()] != '.')
       str += ".";
     str += strExt;
     }
   }
 return str;
}

void FileNameOnly(const char *File,IString & str)
{
 IString temp(findFileName(File));
 char *p = strchr((char *)temp,'.');
 if (p) *p = '\0';
 str = temp;
}

int myWildCardSortCompare(const char * sz1,const char * sz2)
{
 IString str1(sz1), str2(sz2);
 str1.translate(IString("?*"),IString("{}")); //alter collating sequence to
 str2.translate(IString("?*"),IString("{}")); // after chars and ? before *
 return strcmp((char *)str1,(char *)str2);
}

char *stringreplace(char *String, char *From, char *To, int   Max)
{
 char *Found;
 int      LenFrom, LenTo, LenDiff, CurLen, QtyReps=0;
 LenFrom = strlen(From);
 LenTo = strlen(To);
 LenDiff = LenTo - LenFrom;
 CurLen = strlen(String);
 Found = String;
 if (LenDiff > 0) {
   while(Found=strstr(Found,From)) {
     Found=&Found[LenFrom];
     QtyReps++;
     }
   if ((CurLen + (QtyReps * LenDiff)) > Max)
     return(NULL);
   else Found=String;
   }
 while(Found=strstr(Found,From)) {
   if(LenDiff)
     memmove(Found+LenTo, Found+LenFrom, strlen(Found+LenFrom)+1);
   strncpy(Found,To,LenTo);
   Found=&Found[LenTo];
   }
 return(String);
}

char *stringpaddchar(char *string,char c, int iTill)
{
 char *s;
 if (string && (0 < (iTill -=strlen(string))) ) {
   s=string+strlen(string);
   while (iTill--)
     *s++ = c;
   *s = '\0';
   }
 return string;
}

char * CFRPaddCopySZ(char * Target,const char *szSource,unsigned TargetLen,
                     char cPadd)
{
 char  *pc = Target;
 for (int n = TargetLen - strlen(szSource) ; n > 0 ; n--)
   *pc++ = cPadd;
 strncpy (pc,szSource,ALMIN(strlen(szSource),TargetLen));
 return Target;
}

char * CFRCopySZPadd(char * Target,const char *szSource,unsigned TargetLen,
                     char cPadd)
{
 if (szSource && Target && TargetLen) {
   strncpy (Target,szSource,TargetLen);
   int len = strlen(szSource);
   int n = TargetLen - len;
   for (char *pc = Target + len ; n > 0 ; n--)
     *pc++ = cPadd;
   }
 return Target;
}

int compSerialNo(const char * Ser1, const char * Ser2)
{
 Ser1 += ALMAX((int)strlen(Ser1) - 5,0);
 Ser2 += ALMAX((int)strlen(Ser2) - 5,0);
 return strcmp(Ser1,Ser2);
}

char * ensureLF(char *str)
{
 char *pc = str;
 if (pc) {
   mystrrearstrip(pc);
   if (*pc)
     pc = pc + strlen(pc) - 1;
   if ( *pc != '\n')
     strcat(pc,"\n");
   }
 return str;
}

unsigned myantou(const char *source, unsigned n)
{
 for ( ; (*source == ' ') && n ; source ++, n--);
 for (unsigned i = 0 ;source && ISDIGIT(*source) && n ; source++ , n--)
   i = (i*10) + DIGIT(*source);
 return i;
}

int CFRecType(const char *pc)
{
 if (pc && ISDIGIT(*pc) && ISDIGIT(pc[1]))
   return (((*pc - '0') * 10) + (pc[1] - '0'));
 else return -1;
}

void setCFRecType(char *pc, unsigned nmbr)
{
 *pc   = '0' + (char)((nmbr%100)/10);
 pc[1] = '0' + (char)(nmbr%10);
}

Boolean  fWriteBIString(FILE *f,const IString & str, unsigned * pu)
{
 Boolean fOK = false;
 unsigned u = 0, len = str.length()+1;
 if (fwrite(&len,sizeof(len),1,f) == 1) {
   u = sizeof(len);
   if ( fOK = (fwrite((char *)str,len,1,f) == 1) )
     u += len;
   }
 if (*pu) *pu += u;
 return fOK;
}

Boolean  fReadBIString(FILE *f,IString & istring)
{
 unsigned len;
 char * Buff;
 Boolean fOK = (fread(&len,sizeof(len),1,f) == 1);
 if (fOK)
   if (len) {
     if (fOK = ((Buff = new char[len+1]) != 0)) {
       if (fOK = (fread(Buff,len,1,f) == 1))
         istring = IString(Buff);
       delete[] Buff;
       }
     }
   else istring = IString("");
 return fOK;
}

////////////////////////////////////////////////

Boolean Tellem::set(const char * msgFile,const char * OutStream)
{
 if (msgFile)
   MsgFile = msgFile;
 reset();
 if (OutStream && strlen(OutStream)) {
   StreamFile = OutStream;

   //* ICC20079393: Begin Logic, added condition to let the failbit return code be valid, as this return code
   //* permits a successful execution of the application.
   if (pOut = new ofstream(OutStream))
      {
       return ( (pOut->fail() == 0 )  ||
                (pOut->fail() == ios::failbit) );

      }
   //* ICC20079393: End Logic, HECC - 10/13/2004

   }
 return true;
}

void Tellem::reset()
{
 if (pOut) {
   pOut->close();
   if (fEmptyStream)
     remove((char *)StreamFile);
   delete pOut;
   pOut = 0;
   }
 fEmptyStream = true;
}

Boolean Tellem::say ( Boolean fError
                     ,unsigned long Id
                     ,const char* p1
                     ,const char* p2
                     ,const char* p3
                     ,const char* p4
                     ,const char* p5
                     ,const char* p6
                     ,const char* p7
                     ,const char* p8
                     ,const char* p9)
{
 message(Id,p1,p2,p3,p4,p5,p6,p7,p8,p9);
 //IMessageText msgt(Id,(char *)MsgFile,p1,p2,p3,p4,p5,p6,p7,p8,p9);    //jt
 return say(strMsg,fError? Id : 0);                                     //jt
}

const char * Tellem::message  (  unsigned long Id
                                ,const char* p1
                                ,const char* p2
                                ,const char* p3
                                ,const char* p4
                                ,const char* p5
                                ,const char* p6
                                ,const char* p7
                                ,const char* p8
                                ,const char* p9)
{
 strMsg = LOADSTRING(Id);  //edw
 strMsg.change("%1", p1);
 strMsg.change("%2", p2);
 strMsg.change("%3", p3);
 strMsg.change("%4", p4);
 strMsg.change("%5", p5);
 strMsg.change("%6", p6);
 strMsg.change("%7", p7);
 strMsg.change("%8", p8);
 strMsg.change("%9", p9);

 //edwtest strMsg = IString((const char *)IMessageText( Id,(char *)MsgFile,
 //edwtest                                             p1,p2,p3,p4,p5,p6,p7,p8,p9));
 return (const char *)strMsg;
}

Boolean Tellem::say (const IString &str,unsigned ErrorId)
{
 return say((char *)str,ErrorId);
}

Boolean Tellem::say(const char * p, unsigned ErrorId)
{
 if (ErrorId) {
   ErrNo = ErrorId;
   strErr = IString(p);
   }
 if (pOut) {
   IString str(p);
   char * pc, * pc1;
   for (pc = (char *)str ; pc1 = strstr(pc,"\\n");pc = pc1 + 2) {
     *pc1 = '\0';
     *pOut << pc << endl;
     }
   *pOut << pc << endl;
   fEmptyStream = false;
   }
 return false;
}
///////////////////////////////////////

IString & ResMessage::getString( unsigned long Id
                                ,const char*   p1
                                ,const char*   p2
                                ,const char*   p3
                                ,const char*   p4
                                ,const char*   p5
                                ,const char*   p6
                                ,const char*   p7
                                ,const char*   p8
                                ,const char*   p9 )
{
  //if (WinLoadString( NULL,
  //              (HMODULE)IApplication::current().userResourceLibrary().handle(),
  //               Id,1024,Buf)) {
  //  str = IString(Buf);
  //  if (strstr(Buf,"%s")) {
  str = LOADSTRING(Id); //jt,5/28/97
  if (str.size()) {
     if (str.includes("%s")) {
      sprintf(Buf,(char *)str,p1,p2,p3,p4,p5,p6,p7,p8,p9);
      str = IString(Buf);
      }
    }
  else str = IString("Resource String not found : ") + IString(Id);
  return str;
}

//////////////////////////////////////////////////

Boolean MyWMHelpHandler::dispatchHandlerEvent(IEvent& evt)
{ return ((evt.eventId() == WM_HELP))? wmhelpMessage(evt) : false;}

////////////////////////////////////////////////////

Boolean writableFile(char * file)
{
 FILE *f = fopen (file,"wb");
 if (f) {
   fclose(f);
   remove(file);
   return true;
   }
 return false;
}

Boolean readableFile(char * file)
{
 FILE *f = fopen (file,"rb");
 if (f) {
   fclose(f);
   return true;
   }
 return false;
}

void setSPBBackgroundColor(INumericSpinButton & SPB,const IColor & color)
{
/*jt
 IWindow::ChildCursor crsr(SPB);
 for (crsr.setToFirst() ; crsr.isValid() ;crsr.setToNext())
   if (WinQueryWindowUShort(SPB.childAt(crsr),QWS_ID) == SPB.id() ) {
     IEntryField EF(SPB.childAt(crsr));
     EF.setColor(IEntryField::background,color);
     break;
     }
*///jt
}
#define           TESTVAL  10 //jt,test 5/28/97
IRectangle MyClientRect(IFrameWindow * pW)
{
 IRectangle irect;
 if (pW) {
   irect = pW->rect();
   irect.shrinkBy(IPair(pW->borderWidth(),pW->borderHeight()));
   irect=IRectangle( irect.bottomLeft()
                    ,ISize( irect.width()
                           ,irect.height() - TESTVAL
//                                  WinQuerySysValue(HWND_DESKTOP,SV_CYTITLEBAR)
                          )
                   );
   }
 return irect;
}
