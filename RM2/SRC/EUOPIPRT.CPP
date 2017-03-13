
#include <isynonym.hpp> // Definition of Boolean etc
#include <string.h>
#include <stddef.h>
#include <ithread.hpp>
#include "pcol6.hpp"
#include "euopiprt.hpp"
#include "euopres.hpp"
#include <ifont.hpp>
#include "iprinter.hpp"

#include "mainw.h"

//char *pPB_Stub  = "\x0D\x0A>>Pagebreak<<\x0D\x0A\x0D\x0A";
//char *pPB_Stub2  = ">>Pagebreak<<";
char *pPB       = "\x0D\x0C";

char *pIBMNULL  = "IBMNULL";

#define MAX_BUFF  256
#define REPORT_MARGIN  8      //jt

EUOPPrint::EUOPPrint() : cLines(0), cChars(0), cPoints(10), strFont("Courier")
{
 Buf = new char[MAX_BUFF+1];
}

EUOPPrint::~EUOPPrint()
{
 if (Buf)
   delete[] Buf;
}

IPrinter * EUOPPrint :: getPrinter()
{
 IPrinter * printer;
 try {
   printer = new IPrinter();
   }
 catch( IInvalidParameter& exception ) {
   strErr = exception.text();
   delete printer;
   printer = 0;
   }
 return printer;
}


Boolean EUOPPrint :: init()
{
 IPrinter * printer = getPrinter();
 if (printer) {
   if (printer->numberOfQueues()) {
     //edw--os2 get printer name and driver
     //edw strQueue = printer->queueName();
     //edw strDriver = printer->queueDriverName();

     strFont = (strDriver == pIBMNULL)?
                            IString("System Monospaced") : IString("Courier");
     }
   delete printer;
   return TRUE;
   }
 return FALSE;
}

Boolean EUOPPrint::setFont(IPrinter *p)
{
 IFont fnt(strFont,cPoints,false,true);
 try {
   p->setFont(&fnt);
   }
 catch( IInvalidParameter& exception ) {
   strErr = exception.text();
   return false;
   }
 return true;
}

Boolean EUOPPrint :: setCounts()
{
 IPrinter * pPrtr = getPrinter();
 Boolean fOK = (pPrtr != 0);
 if (fOK) {
   //edw--os2
   //edw FONTMETRICS * fm;
   //edw fm = (FONTMETRICS *)new char[sizeof(*fm)];

   //edw--win95
   TEXTMETRIC * tm;
   tm = (TEXTMETRIC *)new char[sizeof(*tm)];

   pPrtr->startJob(IString("Temp Job"));
    // if I set the font to System MonoSpaced on the IBMNULL the
    // available area width gets larger!!!!!!!!!
    // A Default iprinter shows a Courier-10 with an available size
    // of 960x792 allowing for 80 chars
    // which I expect from System MonoSpaced
    // If I set it to System Mono, it shows a Courier-10 with a larger
    // available width of 1152.
    // If I don't,  it retains the 960 pels I expect
   if ( strDriver != pIBMNULL)
      fOK = setFont(pPrtr);
   if (fOK) {
                     //  IFont does a 'startusing' which fails -
                    //   we're only interested in measurements
//            cChars = pPrtr->availableArea().x()/IFont(pPrtr->HPS()).avgCharWidth();
     //edw--os2
     //edw GpiQueryFontMetrics(pPrtr->HPS(),sizeof(*fm),fm);
     //edw--win95
     GetTextMetrics( pPrtr->HDC(), tm);

     cChars = pPrtr->availableArea().x()/tm->tmAveCharWidth;
     cLines = (pPrtr->availableArea().y()/pPrtr->leading()) - 3;
     }
   pPrtr->cancelJob();
   delete (char *)tm;
   delete pPrtr;
   }
 if (!fOK)
   cLines = cChars = 0;
 return fOK;
return 0;
}

unsigned  EUOPPrint:: lineCount()
{
 if (!cLines)
   setCounts();
 return cLines;
}

unsigned  EUOPPrint:: charCount()
{
 if (!cChars)
   setCounts();
 return cChars;
}

Boolean EUOPPrint :: fSpool(char * szFileName,char * JobName,char * szPageBreak,
                            Boolean fDeleteF)
{
// if (getenv("RM2TESTIPRINT"))
//   return fISpool(szFileName,JobName,fDeleteF);
 Boolean fOK = false;
/*jt if ( strDriver == pIBMNULL)  //edwtest if ( strDriver != pIBMNULL)
   fOK = fISpool(szFileName,JobName,szPageBreak,fDeleteF);
 else {
   FILE *f;
   char *pMem = 0;
   unsigned len;
   if (szFileName && (f =  fopen(szFileName,"rb"))) {
     if (len = _filelength(_fileno(f)))
       if (pMem = new char[len+2])
         if (fread(pMem,len,1,f) == 1)
           fOK = spool(pMem,JobName,szPageBreak);
     fclose(f);
     (fOK && fDeleteF && remove(szFileName));
     if (pMem)
       delete[] pMem;
     }
   else strErr = IString( "Unable to open file '") + szFileName + IString("'");
   }  */
 fOK = spool(szFileName, JobName, szPageBreak, fDeleteF);
 if (fOK)
   strErr = IString();
 return fOK;
}


Boolean EUOPPrint :: spool(char * szFileName, char * JobName, char * szPageBreak, Boolean fDeleteF)
{
   //edw add for win95 to setup a printdialog
   memset ((void *) &pd, 0, sizeof(PRINTDLG));
   pd.lStructSize = sizeof(PRINTDLG);
   pd.hwndOwner   = NULL;
   pd.Flags       = PD_RETURNDC;
   pd.hInstance   = NULL;
   pd.hDevNames   = NULL;
   if ( !PrintDlg(&pd))
      {
      strErr = "";
      return 0;
      }
   HDC hdcPrinter = pd.hDC;

   //edw--win95 get printer name and driver
   LPDEVNAMES lpDevNames = (LPDEVNAMES)GlobalLock(pd.hDevNames);
   if ( lpDevNames == NULL )                                     //edw04/07/1999
      {                                                          //edw04/07/1999
      strErr = getString(MAINW_MSG_SPOOL_ERR);                   //edw04/07/1999
      strErr = strErr.words( 1, 4 );                             //edw04/07/1999
      return 0;                                                  //edw04/07/1999
      }                                                          //edw04/07/1999
   strQueue  = (char*) lpDevNames + lpDevNames->wDeviceOffset;   //printername
   strDriver = (char*) lpDevNames + lpDevNames->wDriverOffset;
   GlobalUnlock(pd.hDevNames);

   //win95 printer characteristics
   TEXTMETRIC tm;
   GetTextMetrics( hdcPrinter, &tm);
   ULONG ulCharHeight = tm.tmHeight + tm.tmExternalLeading;

   ULONG ulPageWidth = GetDeviceCaps( hdcPrinter, HORZRES);
   ULONG ulPelsPerMil = ulPageWidth / GetDeviceCaps( hdcPrinter, HORZSIZE);
   ULONG ulHorzMargin = ulPelsPerMil * REPORT_MARGIN;
   ULONG ulPageHeight = GetDeviceCaps( hdcPrinter, VERTRES);
   ulPelsPerMil = ulPageHeight / GetDeviceCaps( hdcPrinter, VERTSIZE);
   ULONG ulVertMargin = ulPelsPerMil * REPORT_MARGIN;
   long  lLine = ulVertMargin;

   //edw--win95 prepare document information prior to printing
   DOCINFO di;
   di.cbSize = sizeof(DOCINFO);
   di.lpszDocName = JobName;
   di.lpszOutput = (LPTSTR) NULL;
   di.lpszDatatype = (LPTSTR) NULL;
   di.fwType = 0;
   //-end of printer preparation-------------

 Boolean fOK = true;
 FILE *f;
 IString str;
 if (!szPageBreak)
   szPageBreak = pPB;
 if (szFileName && (f =  fopen(szFileName,"r"))) {
   if ( fOK = StartDoc( hdcPrinter, &di) ) {

     while (fgets(Buf, MAX_BUFF, f)) {

       if (lLine == ulVertMargin) {  // inform printer
         StartPage(hdcPrinter);
         }

       if (!(strcmp(mystrrearstrip(Buf),szPageBreak))) {
         EndPage(hdcPrinter);        // dump buffer
         lLine = ulVertMargin;       // reset y coordinate
         }
       else  {
         fOK = TextOut(
               hdcPrinter,          // handle of device context            ---win95
               ulHorzMargin,        // x-coordinate of starting position
               lLine,               // y-coordinate of starting position
               Buf,                 // address of string
               strlen(Buf) );       // number of characters in string
         lLine += ulCharHeight;     //edwtest
         }
       } //while

     if (fOK) {
       EndPage(hdcPrinter);
       fOK = EndDoc(hdcPrinter);
       }

     } //StartDoc
   fclose(f);
   (fOK && fDeleteF && remove(szFileName));
   } //open

 if (!fOK) {
   unsigned long splerr = GetLastError();
   IGUIErrorInfo errInfo("SplQmWrite");
   strErr = getString(MAINW_MSG_SPOOL_ERR,(char *)strQueue,
                      (char *)IString(splerr), errInfo.text());
   }
 return fOK;
}

Boolean EUOPPrint :: fISpool(char * szFileName,char * JobName,char * szPageBreak,
                             Boolean fDeleteF)
{
 IPrinter * pPrtr = getPrinter();
 if (!pPrtr)
   return false;
 Boolean fOK = true;
 FILE *f;
 IString str;
 if (!szPageBreak)
   szPageBreak = pPB;
 if (szFileName && (f =  fopen(szFileName,"r"))) {
   pPrtr->startJob( JobName);
   if (fOK = setFont(pPrtr)) {
     IPoint pntTop(0,pPrtr->availableArea().y() - (pPrtr->leading()*2));
//       IPoint pntTopOfForm = prtr->currentPosition();
     IPoint pntPrt(pntTop);
     while (fgets(Buf,256,f))
//       if (!(strcmp(mystrrearstrip(Buf),pPB_Stub2))) {
       if (!(strcmp(mystrrearstrip(Buf),szPageBreak))) {
         pPrtr->newPage();
         pntPrt = pntTop;
         }
       else pPrtr->print(IString(Buf),pntPrt);
     }
   pPrtr->endJob();
   fclose(f);
   (fOK && fDeleteF && remove(szFileName));
   }
 else fOK = false, strErr = IString( "Unable to open file '") + szFileName + IString("'");
 delete pPrtr;
 return fOK;
}

///////////////////////////////////////
#ifdef UNITTEST

void main(void)
{
}

#endif
#if 0
Boolean EUOPPrint :: init()
{
 Boolean fOK=true;
 if (!prtr)
   { try
      {
        prtr = new IPrinter();
      }
     catch( IInvalidParameter& exception )
       {
         strErr = exception.text();
         return false;
       }
     PrinterQueue = prtr->queueName();
     FONTMETRICS * fm;
     fm = (FONTMETRICS *)new char[sizeof(*fm)];
     if ( strDriver != pIBMNULL)
       {
         IFont fnt("Courier",10,false,true);
         prtr->startJob(IString("Temp Job"));
         try
          {
            prtr->setFont(&fnt);
            GpiQueryFontMetrics(prtr->HPS(),sizeof(*fm),fm);
            cChars = prtr->availableArea().x()/fm->lAveCharWidth;
         // IFont does a 'startusing' which fails - we're only interested in
         // measurements
//            cChars = prtr->availableArea().x()/IFont(prtr->HPS()).avgCharWidth();
          }
         catch( IInvalidParameter& exception )
           {
             strErr = exception.text();
             fOK = false;
           }
         prtr->endJob();
       }
     else { GpiQueryFontMetrics(prtr->HPS(),sizeof(*fm),fm);
            cChars = prtr->availableArea().x()/fm->lAveCharWidth;
          }
     delete (char *)fm;
     if (fOK)
        cLines = (prtr->availableArea().y()/prtr->leading())-1;
     else cChars = 0;
   }
 return fOK;
}
#endif
