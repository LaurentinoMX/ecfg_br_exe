// 3-22-95 [DJL] Added ulHostPID to XWindow::XWindow()
// 3-22-95 [DJL] Modified XWindow::closed() to kill the host process if it is started
// 7-01-02 [EDW] D72623 Removed logic in  MSG_TO_WW_WRITE case that makes two unused, unnecessary
//               copies of the reconciled .CFR file.  Now we will give the reconciled
//               file an extension of .RET and remove the creation of the .CFR and
//               .CFREPORT copies as they are unneccessary and now not used by the
//               MainWindow::openHost method.
// 7-08-02 [HECC] F72273 Added logic to create and pass as parameters to the
//                reconciliation logic, the flag that indicates if the actual
//                configuration is iSeries or not and the pointer to the
//                iSeries SW features table.
// 7-18-02 [HECC] Received Date at IBM and Sensed Date are passed as parameters to the
//                logic where both, the Matches and Mismatches windows are populated
//                to display them in the header of both files.
// 11-04-03 [HECC] Disabled the condition that checks if there was any change in the
//                 location of the $$F path file to always store this new value in
//                 the registry
// 08-13-03 [HECC] Changed the location where the Date received at IBM data is being taken from
//                 the array pSD->FileDate[] from the position '1' to the position '0' because
//                 this is where this value (written from the AAS file) is stored.
// 06-01-04 [HECC] ICC18033550 Section of code disabled to avoid the call to parameter1(). It only
//                 returns a non-null pointer if Motif (only for AIX boxes) is used. So, it will
//                 never apply to BaseRet.
// 02-21-06 [RERC] D106441. Text for .RET file that was on two lines was converted to one line.
//
// 01-20-10 [ELRR] D132894. Rearrangement of the sentence pDiscW-> recordMismatches ();
//                 This is to give priority to the previous code and
//                 recordMismatches function can gather the data to the profile of the user to display
//                 the view data collapsed or expanded depending on configuration.

// 04/23/10 [ELRR] D134211. Issue a Sleep command to give some time to the GUI thread to process pending GUI messages.
//

#define INCL_WINSWITCHLIST
#define INCL_DOSSEMAPHORES                     // get os2's semaphore stuff
#define INCL_DOSPROCESS                        // dossleep
#define INCL_SPL
#define INCL_DOSERRORS                         // ERROR_MORE_DATA
#define INCL_SPLDOSPRINT
#define INCL_DOS                               // dosscanenv
#define INCL_GPI
#define INCL_GPIPRIMITIVES
#define INCL_PM
#define INCL_WIN
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>                           // _spawnlp
#include <windows.h>
#define INCL_IAPPLICATION
#define INCL_ICOLOR
#define INCL_ITITLE
#define INCL_IMENUEVENT
#define INCL_IFILEDIALOG
#define INCL_ISYSTEMMENU
#include "uicl.hpp"

#include "euophlp.h"
//#include "abtw.hpp"                          //jt replaced with ordapply aboutbox
//#include "abtw.h"
#include "prf.hpp"
#include "prfw4.h"
#include "prfw4.hpp"
#include "mainw.hpp"
#include "ialctls.hpp"
#include "euomsg.h"
#include "euogmain.hpp"
#include "euogmain.h"
#include "swdw.hpp"
#include "pprvw.hpp"
#include "pprvw.h"
#include "rpo2wc.hpp"
#include "rpo2wc.h"
#include "rpoinfo.h"
#include "rpoinfo.hpp"
#include "rpodata.hpp"
#include "bklogw.hpp"
#include "bklogw.h"
#include "mmw2.hpp"
#include "mmw2.h"
#include "xplnw.hpp"
#include "xplnw.h"
//#include "hostsesw.hpp"                       //jt replaced with ub9host
#include "pcol6.hpp"
#include "alvfile.hpp"
#include "pkfeatl.hpp"
#include "af9compl.hpp"                         //edw06/24/1998
#include "af9main.hpp"                          //edw06/24/1998
#include "mainw.h"
#include "ub9host.h"                            //edw
#include "af9apply.h"                           //edw
#include "af9profl.hpp"                         //edw10/15/1999
#include "specwnd.hpp"                          //47973 - MAMT

IHelpWindow *pGHW = 0;
// EUOPProfile * pUserProfile = NULL;
char *pPPRVW_TEMP_FILE  = "PPRVWPPP.PPP";
//edw
//char *pPRINT_TEMP_FILE = "RM2PPPPP.PPP";
char *pPRINT_TEMP_FILE = "pPRINT_TEMP_FILE";

//******************************************************************************


#define MSG_BLW_CLOSING                 WM_USER+1
#define MSG_DISCW_CLOSING               WM_USER+2
#define MSG_FROM_MYSELF_START           WM_USER+3
#define MSG_FROM_WW_DO_ERRMSGBOX        WM_USER+4
#define MSG_FROM_WW_DO_MISMATCH         WM_USER+5
#define MSG_FROM_WW_DO_MSGBOX           WM_USER+6
#define MSG_FROM_WW_FORMAT_PPRVW_FILE   WM_USER+7
#define MSG_FROM_WW_HIDE_TIMER          WM_USER+8
#define MSG_FROM_WW_PRINT               WM_USER+9
#define MSG_FROM_WW_REBUILD             WM_USER+10
#define MSG_FROM_WW_RESOLVE             WM_USER+11
#define MSG_FROM_WW_RE_START_TIMER      WM_USER+12
#define MSG_FROM_WW_SAVE                WM_USER+13
#define MSG_FROM_WW_SET_TIMER_TEXT      WM_USER+14
#define MSG_FROM_WW_SHOW_DETAILS        WM_USER+15
#define MSG_FROM_WW_SHOW_TIMER          WM_USER+16
#define MSG_FROM_WW_STOP_TIMER          WM_USER+17
#define MSG_FROM_WW_T_PRT               WM_USER+18
#define MSG_FROM_WW_WRITE               WM_USER+19
#define MSG_PPRVW_CLOSING               WM_USER+20
#define MSG_RESOW_CLOSING               WM_USER+21
#define MSG_RPOW_CLOSING                WM_USER+22
#define MSG_SWDW_CLOSING                WM_USER+23
#define MSG_TO_WW_CLOSE_ABTDLG          WM_USER+24
#define MSG_TO_WW_DO_ABTDLG             WM_USER+25
#define MSG_TO_WW_FORMAT_PPRVW_FILE     WM_USER+26
#define MSG_TO_WW_PRINT                 WM_USER+27
#define MSG_TO_WW_REBUILD               WM_USER+28
#define MSG_TO_WW_RESOLVE               WM_USER+29
#define MSG_TO_WW_SAVE                  WM_USER+30
#define MSG_TO_WW_T_PRT                 WM_USER+31
#define MSG_TO_WW_WRITE                 WM_USER+32

#define MSG_TO_WW_START_HOST_CPF  WM_USER+70
#define MSG_FROM_WW_HOST_CPF_FAIL  WM_USER+71
#define MSG_FROM_WW_SET_FOCUS_TO_MAIN WM_USER+72

//#define SHOWMESSAGE(sz) MyXHndlr.ShowError("EUOP API Tester",sz,__FILE__,__LINE__)
#define SHOWMESSAGE(sz) { strMB = IString(sz);                            \
                          IFrameWindow::sendEvent(MSG_FROM_WW_DO_MSGBOX); \
                        }
#define MWMS_INITIAL        1         // main window menu state
#define MWMS_NEW_SYS        2
#define MWMS_SYS_RESOLVED   3

Boolean XWindow::bShowS390Results;

// int main() logic removed 03-24-97

XWindow::XWindow(unsigned long ulId, IString strAAS, IString strMRD, IString strDSC, MainWindow* pOwner, IHelpWindow *pHW, EUOPProfile *pProf )
      : IFrameWindow(
//                     IWindow::noStyle |
                      (IFrameWindow::titleBar
                       | IFrameWindow::systemMenu
                       | IFrameWindow::menuBar
                       | IFrameWindow::sizingBorder
                       | IFrameWindow::minimizedIcon
                       | IFrameWindow::minimizeButton
                       | IFrameWindow::maximizeButton)
//                     & ~IWindow::clipChildren
//                       & ~IWindow::clipToParent
                      ,ulId )
       ,BaseWnd(this)
       ,ResLib(IApplication::current().userResourceLibrary())
       ,VP(WND_MAIN_VP, this, this )
       ,MCC(WND_MAIN_MCC,&VP,&VP)
       ,MCCE(WND_MAIN_MCCE,this,this)
       ,TextMain(WND_MAIN_TEXT,&MCC,&MCC)
       ,Label1(WND_MAIN_LABEL1,&MCC,&MCC)
       ,Label2(WND_MAIN_LABEL2,&MCC,&MCC)
       ,Label3(WND_MAIN_LABEL3,&MCC,&MCC)
       ,Label4(WND_MAIN_LABEL4,&MCC,&MCC)
       ,Label5(WND_MAIN_LABEL5,&MCC,&MCC)
       ,Text1(WND_MAIN_TEXT1,&MCC,&MCC)
       ,Text2(WND_MAIN_TEXT2,&MCC,&MCC)
       ,Text3(WND_MAIN_TEXT3,&MCC,&MCC)
       ,Text4(WND_MAIN_TEXT4,&MCC,&MCC)
       ,Text5(WND_MAIN_TEXT5,&MCC,&MCC)
       ,TextTimer(WND_MAIN_PB_HIDE,&MCCE,&MCCE)
       ,MyXHndlr(this,this)
       ,Res(this)
       ,fFirst(true)
       ,fFirstDiscWShowing(true)
       ,fFirstRPOWShowing(true)
       ,fFirstSWDWShowing(true)
       ,respMB(IMessageBox::unknown)
       ,styleMB(  IMessageBox::okButton
                | IMessageBox::informationIcon
                | IMessageBox::applicationModal
                | IMessageBox::moveable
                | IMessageBox::systemModal)
      //jt ,pAbtDlgT2(0) --- replaced by ordapply
      //jt ,pAbtDlg(0)   --- replaced by ordapply
       ,pSWDW(0)
       ,pPPrvW(0)
       ,pMMWnd(0)
       ,pRPOW(0)
       ,pBLW(0)
       ,menubar(ulId,this)
       ,sysMenu(this)
       ,fViewDisabled(true)
       ,fPrintAvailable(false)
      //JTEST ,ulHostPID(0)
       ,fForcePKFileRead(false)
       ,bExpan(pOwner->bExpanded)  //ZOR 48548
       ,bTil(pOwner->bTile)  // 47971 MAMT 21/08/2000
{
//   char strE[100], strE1[100];
//   LPTSTR *strE2 = (LPSTR*)strE1;
//   SearchPath(NULL, "BF9BRET.EXE", NULL, 99, strE, strE2);
//   strExecPath = strE;
//   strExecPath = strExecPath.remove(strExecPath.lastIndexOf( STR_PathDelimiter)+1);

 BufK = new char[1024];
 setExceptionFunction(&MyXHndlr);
//  if (fMyProfile = ((pProfile = pProf) == NULL )) {
//   pProfile = new EUOPProfile();
//   pProfile->SetResources(IApplication::current());
//   }
 pProfile = pProf;
 ErrTitle = ResLib.loadString(MAINWND_STR_ERR);

 AASFile = strAAS;
 MRDFile = strMRD;
 MRDDescFile = strDSC;
 pMain = pOwner;
 bSetLockingFlag = false;
 if( pMain->bS390 )
    bShowS390Results = pMain->pProfile->bShowS390Mismatches;
 else
    bShowS390Results = false;
 iUseOldMRDBUserChoice = 0;      //edw12/29/1999
 iContMismatchesUserChoice = 0;  //edw12/29/1999
 iUseAASMRDBUserChoice = 0;      //edw12/29/1999

 // Set application execution path attribute
 strExecPath = pMain->strExecPath;

 // Initial NoCompare Table pointer
 pNoCompTable = new ISortedSet<IString>( pMain->noCompTableSet );
 // Initial QuickFix Table pointer 49222 - MAMT - 27/06/2000
 pQuickFixTable = new ISortedSet<IString>( pMain->QuickFixTableSet );

 // Initial iSeriesSW Table pointer  F72273 - HECC - 06/27/2002
 pISeriesSWTable = new ISortedSet<IString>( pMain->iSeriesSWTableSet );

 WorkWindow::StartUp(handle());
}

Boolean  XWindow::wmhelpMessage(IEvent & evt)
{
   helpWindow()->show(IResourceId(IDW_AFOAPPLY));
   //helpWindow()->show(IResourceId(idFDHelpPanel));
   return true;
}

Boolean  XWindow::controlSelect(IHelpNotifyEvent& evt)
{
 //jt WinAlarm(HWND_DESKTOP, WA_NOTE);
 MessageBeep(MB_ICONEXCLAMATION);     //jt
 return false;
}

void XWindow::BuildWindow()
{
 setClient(&VP);
 CreateClient();
 CreateExtension();
 IFrameHandler::handleEventsFor(this);
//jt MyWMHelpHandler::handleEventsFor(this);
 MyMinMaxHandler::handleEventsFor(this);
 addToWindowList();
}

Boolean XWindow::Minimizing(ISWP * psw)
{
 kids.Minimize();
 return false;
}

Boolean XWindow::Restoring(ISWP * psw)
{
 if (isMinimized())
    kids.ReShow();
 return false;
}

Boolean XWindow::closeRUSure()
{
  return ( IMessageBox::yes == WWHandleMessage(
                               (char *)ResLib.loadString(MAINWND_STR_R_U_SURE),
                               0, IMessageBox::yesNoButton
                                  |IMessageBox::queryIcon
                                  |IMessageBox::defButton2
                                  |IMessageBox::applicationModal
                                  |IMessageBox::moveable
					    |IMessageBox::systemModal));
}

Boolean XWindow :: PressedEsc_F3(IKeyboardEvent & evt)
{
 if (closeRUSure())
   close();
 return true;
}

XWindow::~XWindow()
{
 delete[] BufK;
 delete pNoCompTable;
 delete pQuickFixTable;             //49222 - MAMT - 27/06/00
 delete pISeriesSWTable;            //72273 - HECC - 06/27/2002

// WorkWindow::CloseDown();
// WinAlarm(HWND_DESKTOP, WA_NOTE);
}

void XWindow :: CreateExtension(void)
{
//jt pTimer = new MinSecTimer(WND_MAIN_TIMER_CTL,&MCCE,&MCCE, IRectangle(),
//jt                          MinSecTimer::defaultStyle() & ~MinSecTimer::visible);
//jt pTimer->setAutoDeleteObject();
 TextTimer.setLimit(35).setAlignment(IStaticText::centerLeft);
// IFont tempFont("Helvetica",10);
// TextTimer.setFont(tempFont);
//jt MCCE.addToCell(pTimer    ,2, 2 )
//jt     .addToCell(&TextTimer,4, 2 );
 MCCE.addToCell(&TextTimer,4, 2 );
 MCCE.setColumnWidth(1,IMultiCellCanvas::defaultCell().width())
     .setColumnWidth(3,2*IMultiCellCanvas::defaultCell().width())
     .setColumnWidth(4,IMultiCellCanvas::defaultCell().width())
     .setRowHeight(1,IMultiCellCanvas::defaultCell().height())
     .setRowHeight(3,IMultiCellCanvas::defaultCell().height());
 addExtension(&MCCE,belowClient,(unsigned long)MCCE.minimumSize().height());
}

void XWindow :: CreateClient(void)
{
 TextMain.setLimit(50);
 Text1.setLimit(35);
 Label1.setText(MAINWND_STR_LABEL1);
 Label2.setText(MAINWND_STR_LABEL2);
 Label3.setText(MAINWND_STR_LABEL3);
 Label4.setText(MAINWND_STR_LABEL4);
 Label5.setText(MAINWND_STR_LABEL5);

 MCC.addToCell(&TextMain , 1  , 1, 4, 1)
    .addToCell(&Label1   , 2  , 3      )
    .addToCell(&Label2   , 2  , 5      )
    .addToCell(&Label3   , 2  , 7      )
    .addToCell(&Label4   , 2  , 9      )
    .addToCell(&Label5   , 2  ,11      )
    .addToCell(&Text1    , 4  , 3      )
    .addToCell(&Text2    , 4  , 5      )
    .addToCell(&Text3    , 4  , 7      )
    .addToCell(&Text4    , 4  , 9      )
    .addToCell(&Text5    , 4  ,11      );
 MCC.setRowHeight(   2,IMultiCellCanvas::defaultCell().height())
    .setRowHeight(   4,IMultiCellCanvas::defaultCell().height())
    .setRowHeight(   6,IMultiCellCanvas::defaultCell().height())
    .setRowHeight(   8,IMultiCellCanvas::defaultCell().height())
    .setRowHeight(  10,IMultiCellCanvas::defaultCell().height())
    .setColumnWidth( 3,IMultiCellCanvas::defaultCell().width());
}

#define SEM_ERR_DUPLICATE_NAME 285
Boolean XWindow :: Check_SetupHelp(Boolean fShowNoHelp)
{
 IString hlpName = pProfile->CurrentHelpFile().size()?
                     pProfile->CurrentHelpFile() : pProfile->DefaultHelpFile();
 char * TempBuf = new char[257];
 _searchenv((char *)hlpName,"HELP",TempBuf);
 if (*TempBuf) {
   pGHW = new IHelpWindow(EUOP_HLP_TABLE,this);
//     pHLPWndw->setAutoDeleteObject();      // nogood
   pGHW->addLibraries(TempBuf);
   pGHW->setTitle(MAINWND_STR_HELPTITLE);
   SetupHelp(pGHW);
   delete TempBuf;
   }
 else {
   pGHW = 0;
   IString str("Help is unavailable! (Unable to locate file '");
   str += (hlpName + IString("')"));
   IException exc = IException((char *)str);
   exc.addLocation(IEXCEPTION_LOCATION());
   menubar.disableItem(SC_HELPINDEX).disableItem(SC_HELPEXTENDED);
   delete TempBuf;
   if (fShowNoHelp)
     return (MyXHndlr.HandleExceptionResponse(exc, IMessageBox::okCancelButton
                                                 | IMessageBox::informationIcon
                                                 | IMessageBox::applicationModal
                                                 | IMessageBox::moveable
								 | IMessageBox::systemModal
                                             ) == IMessageBox::ok);
   else MyXHndlr.LogException(exc);
   }
 return true;
}

Boolean XWindow :: keysHelpId(IEvent& evt)
{
 evt.setResult(IDH_PNL_KEYS);
  return true;
}

void XWindow::SetInitialState(void)
{
 VP.setMinimumSize(MCC.minimumSize());
 VP.sizeTo(VP.minimumSize());
//moveSizeToClient(VP.rect());
 sizeTo(ISize(ALMIN(frameRectFor(VP.rect()).size().width()+40,
                    IWindow::desktopWindow()->size().width() * 0.9),
              ALMIN(frameRectFor(VP.rect()).size().height()+50,          //edw-6/23/97
                    IWindow::desktopWindow()->size().width() * 0.8)));
//jt PlaceWnd(left,top,IWindow::desktopWindow());
 PlaceWnd(left,bottom,IWindow::desktopWindow());//jtest
 SetMenu(MWMS_INITIAL);
 if (pMain->bS390 && bShowS390Results)  //Hide Status window if S390 and ShowS390Mismatches
   hide();              // selected in profile flw - 44907
}

void XWindow :: Able(Boolean fEn)
{
 if (fEn) {
   if ( !fViewDisabled)
     menubar.enableItem(WND_MAIN_MI_VIEW);
   menubar.enableItem(WND_MAIN_MI_FILE)
          .enableItem(WND_MAIN_MI_HELP);
   }
 else  {
   fViewDisabled = !menubar.isItemEnabled(WND_MAIN_MI_VIEW); //jt
   menubar.disableItem(WND_MAIN_MI_FILE)
          .disableItem(WND_MAIN_MI_VIEW)
          .disableItem(WND_MAIN_MI_HELP);
   }
}

void XWindow :: focusTheKids()
{
 if (pDiscW->isVisible())
    pDiscW->setFocus();
 else if (pResoW->isVisible())
        pResoW->setFocus();
      else setFocus();
}

void XWindow :: ableTheKids(Boolean fEn)
{
 if (pSWDW->isVisible())
  pSWDW->Able(fEn);
 if (pRPOW->isVisible())
  pRPOW->Able(fEn);
 pResoW->IWindow::enable(fEn);
 pDiscW->IWindow::enable(fEn);
}

Boolean XWindow :: userMessage(IEvent& evt)
{
 switch(evt.eventId()) {
   case MSG_FROM_MYSELF_START:
     WorkWindow::StartUp(handle());
     return true;
   case MSG_PPRVW_CLOSING:
     setFocus();
     return true;
   case MSG_BLW_CLOSING:
     pBLW->hide();
     focusTheKids();
     return true;
   case MSG_SWDW_CLOSING:
     pSWDW->hide();
     focusTheKids();
     return true;
   case MSG_RPOW_CLOSING:
     pRPOW->hide();
     changedRPOList();
     focusTheKids();
     return true;
   case MSG_RESOW_CLOSING:
     if (pResoW)
       pResoW->hide();
     setFocus();
     return true;
   case MSG_DISCW_CLOSING:
     if (pDiscW)
       pDiscW->hide();
     setFocus();
     return true;
   case WW_UM_READY:
     if (fFirst) {
       fFirst = false;
//edw
//       WorkWindow::sendEvent((unsigned long)MSG_TO_WW_DO_ABTDLG);
//          WorkWindow::postEvent((unsigned long)MSG_TO_WW_DO_ABTDLG);
       pProfW = new EUOPProfileWndw(WND_PRF,pProfile,this, helpWindow());
       kids.add(Kid((IFrameWindow *)pProfW));
       pProfW->setOwner(this);
       pProfW->Center();

       pXW = new XPLNWndw( WND_XPLN,&Res.explanations(), helpWindow());
       kids.add(Kid((IFrameWindow *)pXW));


       pResoW = new RMDiscResoW( WND_RESO,"",pProfile,this, pMain->bS390,
                                 helpWindow(), pXW, MSG_RESOW_CLOSING );
       kids.add(Kid((IFrameWindow *)pResoW));
       pDiscW = new RMDiscResoW( WND_DISC,"",pProfile,this, pMain->bS390,
                                 helpWindow(), pXW, MSG_DISCW_CLOSING, pResoW );
       kids.add(Kid((IFrameWindow *)pResoW));
       //53915 MAMT
       pMMWnd = new MMWnd(WND_MM,IString("Text"),IString("B1"),IString("B2"), pMain->bS390,
                                          helpWindow());
       pMMWnd->Center();
       pMMWnd->setOwner(this);

       pSWDW = new SWDWndw(WND_SWD,&Res.softwareDiscrepancyList(),helpWindow(),
                           this,MSG_SWDW_CLOSING);
       kids.add(Kid((IFrameWindow *)pSWDW));

       fPrintAvailable = prt.init();

       pRPOW = new RPOsWndw(WND_RPO,&Res.rpos(),pProfile,
                            this,helpWindow(),MSG_RPOW_CLOSING);
       kids.add(Kid((IFrameWindow *)pRPOW));

       pBLW = new RMBkLogW(WND_BKLOG,this,helpWindow(),MSG_BLW_CLOSING);
       pBLW->Center();

       kids.add(Kid((IFrameWindow *)pBLW));

//jt       pAbtDlg = new EUOPAboutDlg(DLG_ABOUT,this);
//jt       kids.add(Kid((IFrameWindow *)pAbtDlg));
//jt       pAbtDlg->Center();

       BuildWindow();
       SetInitialState();
       IFrameWindow::postEvent(IWindow::command,WND_MAIN_ID_RESOLVE);
       }
     return true;
   case MSG_FROM_WW_DO_MISMATCH:
     pMMWnd->showModally();
     evt.setResult(pMMWnd->result());
     setFocus();
     return true;
   case MSG_FROM_WW_DO_MSGBOX:
     respMB = MyXHndlr.HandleMsgResponse( strMB,
                                    (char *)ResLib.loadString(MAINWND_STR_RM2),
                                     HelpIdMB,styleMB);
     setFocus();
     return true;
   case MSG_FROM_WW_DO_ERRMSGBOX:
     respMB = MyXHndlr.HandleErrMsgResponse(strMB, (char *)ErrTitle, HelpIdMB,
                                            styleMB);
     setFocus();
     return true;
   case MSG_FROM_WW_RE_START_TIMER:
     Re_StartTimer(strTimerText.length()? (char *)strTimerText : 0);
     return true;
   case MSG_FROM_WW_SHOW_DETAILS:
     ShowDetails((Boolean)(unsigned long)evt.parameter1());
     return true;
   case MSG_FROM_WW_HIDE_TIMER:
     HideTimer();
     return true;
   case MSG_FROM_WW_SHOW_TIMER:
     ShowTimer();
     return true;
   case MSG_FROM_WW_SET_TIMER_TEXT:
     TextTimer.setText(strTimerText);
     return true;
   case MSG_FROM_WW_STOP_TIMER:
     return true; //jtest
     if (strTimerText.length())
       TextTimer.setText(strTimerText);
     pTimer->StopCounting();
     return true;
   case MSG_FROM_WW_RESOLVE://edw 05/07/1998
     kids.Hide();
     SaveFile = IString();
   //* ICC18033550; Begin logic: Code disabled because the call to parameter1() only returns a
   //* non-null pointer if Motif (only for AIX boxes) is used, which will never apply to BaseRet.
   /*if (!(unsigned long)evt.parameter1()) {
       if (Res.lastErrorNumber())
         MyXHndlr.HandleErrMsgResponse((char *)Res.lastErrorText(),
                                       (char *)ErrTitle);
       SetMenu(MWMS_INITIAL);
       BlankDetails();
       WakeUp();
       setFocus();

       //edw1pMain->bResolve = false;
       //edw1pMain->bUseMRDB = false;
       //edw1pMain->bUseAAS = false;
       //edw1close();
       IFrameWindow::postEvent(IWindow::command, WND_MAIN_MI_EXIT);  //edw1
       }
     else { */
   //* ICC18033550; End logic: HECC - 06/01/2004
       {
       SetMenu(MWMS_NEW_SYS);
       //jtest pRPOW->FillList(&Res.rpos());
       //jtest if (Res.rpos().numberOfElements())
         //jtest pRPOW->Sort();
       pSWDW->FillList(&Res.softwareDiscrepancyList());
       if (Res.softwareDiscrepancyList().numberOfElements())
         pSWDW->Sort();
       pBLW->load(&Res.backlogList(), pProfile);
       WakeUp();
       showWindows();
       IString str(Res.currentPKFile());

       //* 79548 Begins New logic: Disables the condition that checks if there was
       //  any change in the location of the $$F path file to always store this new
       //  value in the registry.  This is necessary because changes to the $$F file
       //  path are only applied to the profile structure, but we don't have a way
       //  to directly verify if this value is the same as the value in the profile.
       //* if (str != pProfile->GetStringKey( EUOPProfile::ProductKnowledgeFile ))
       //*  pProfile->SetStringKey(EUOPProfile::ProductKnowledgeFile,str);
       pProfile->SetStringKey(EUOPProfile::ProductKnowledgeFile,str);
       //* 79548 Ends new logic:  HECC - 04/14/2003

       fForcePKFileRead = false;

       if ( pMain->bS390 && bShowS390Results )
          {
             if( !pDiscW->numberOfObjects() )
                {
                   IFrameWindow::postEvent(IWindow::command, WND_MAIN_MI_EXIT);
                }
             else
                {
// D132894 ELRR Begin: This line was moved after the function pDialog->showModally();  because the window Mismatches lost priority to update the view data.
              // pDiscW->recordMismatches();         //edw01/03/2000
// D132894 ELRR End:
                //47973 - MAMT begin new code
                /*
                IMessageBox::Response resp =
                   WWHandleMessage( ResLib.loadString( IDS_ReconcileResults_Text )
                                ,IDH_ReconcileResults
                                ,IMessageBox::yesNoButton
                                 |IMessageBox::queryIcon
                                 |IMessageBox::defButton2
                                 |IMessageBox::moveable
                                 |IMessageBox::systemModal );      //ZOR 060900 defect 51688
                switch( resp ) {
                   case IMessageBox::yes:
                       iContMismatchesUserChoice = 2;                               //edw12/29/1999
                       pDiscW->autoResolve(true);
                       IFrameWindow::postEvent(IWindow::command, WND_MAIN_MI_EXIT);
                       break;
                   case IMessageBox::no:
                       iContMismatchesUserChoice = 1;                               //edw12/29/1999
                       SetMenu(MWMS_INITIAL);
                       BlankDetails();
                       WakeUp();
                       setFocus();
                       pMain->bResolve = false;
                       pMain->bUseMRDB = false;
                       pMain->bUseAAS = false;
                       close();
                       break;
                   } */

                SpecWindow *pDialog;
                pDialog = new SpecWindow(this);
                IPoint mainPos = this->position();
                mainPos.setX( mainPos.x() + 200 );      //ZOR F47969
                mainPos.setY( mainPos.y() + 360 );      //ZOR F47969
                pDialog->moveTo( mainPos );
                pDialog->addToWindowList();
                pDialog->setFocus();
                pDialog->showModally();
                // ELRR D132894 Begin:
                pDiscW->recordMismatches();
                // ELRR D132894 End.
                if ( pDialog->bContinue ) {
                   iContMismatchesUserChoice = 2;                               //edw12/29/1999
                   pDiscW->autoResolve(true);
                   IFrameWindow::postEvent(IWindow::command, WND_MAIN_MI_EXIT);
                } else {
                   iContMismatchesUserChoice = 1;                               //edw12/29/1999
                   SetMenu(MWMS_INITIAL);
                   BlankDetails();
                   WakeUp();
                   setFocus();
                   pMain->bResolve = false;
                   pMain->bUseMRDB = false;
                   pMain->bUseAAS = false;
                   close();
                }
                delete pDialog; //47973 - MAMT finish new code
                // ELRR D134211 Begin:  Issue a Sleep command to give some time to the GUI thread to process pending GUI messages.
                   Sleep(200);
                // ELRR D134211 End.

                }
          }
       else
          {
          //edw07/01/1998-End reconiciation if system resolved
          if ( !pDiscW->numberOfObjects() && pMain->bS390 )
             {
             if ( Res.rpos().numberOfElements() )                                   //fw09/28/99
                IFrameWindow::postEvent(IWindow::command, WND_MAIN_MI_RPOW);        //fw09/28/99
             IFrameWindow::postEvent(IWindow::command, WND_MAIN_MI_EXIT);
             }
          //edw07/01/1998
          if ( pDiscW->numberOfObjects() && pMain->bS390 )
             {
             pDiscW->recordMismatches();                                            //edw01/03/2000
             IMessageBox::Response resp =
                WWHandleMessage( ResLib.loadString( IDS_MismatchWarning_Text )
                                ,IDH_MismatchWarning
                                ,IMessageBox::yesNoButton
                                 |IMessageBox::queryIcon
                                 |IMessageBox::defButton2
                                 |IMessageBox::moveable
                                 |IMessageBox::systemModal);
              switch( resp ) {
                 case IMessageBox::yes:
                    iContMismatchesUserChoice = 2;                                  //edw12/29/1999
                    bSetLockingFlag = true;
                    pDiscW->autoResolve();
                    if ( Res.rpos().numberOfElements() )                            //fw09/28/99
                       IFrameWindow::postEvent(IWindow::command, WND_MAIN_MI_RPOW); //fw09/28/99
                    IFrameWindow::postEvent(IWindow::command, WND_MAIN_MI_EXIT);
                    break;
                 case IMessageBox::no:
                    iContMismatchesUserChoice = 1;                                  //edw12/29/1999
                    SetMenu(MWMS_INITIAL);
                    BlankDetails();
                    WakeUp();
                    setFocus();
                    pMain->bResolve = false;
                    pMain->bUseMRDB = false;
                    pMain->bUseAAS = false;
                    close();
                    break;
                 }
              }
          }
       }
     return true;
   case MSG_FROM_WW_REBUILD:
     kids.Hide();
     SaveFile = IString();
     if (!(unsigned long)evt.parameter1()) {
       if (Res.lastErrorNumber())
         MyXHndlr.HandleErrMsgResponse((char *)Res.lastErrorText(),
                                       (char *)ErrTitle);
       SetMenu(MWMS_INITIAL);
       BlankDetails();
       WakeUp();
       setFocus();
       }
     else {
       SetMenu(MWMS_NEW_SYS);
       //jtest pRPOW->FillList(&Res.rpos());
       //jtest if (Res.rpos().numberOfElements())
         //jtest pRPOW->Sort();
       pSWDW->FillList(&Res.softwareDiscrepancyList());
       if (Res.softwareDiscrepancyList().numberOfElements())
         pSWDW->Sort();
       pBLW->load(&Res.backlogList(), pProfile);
       WakeUp();
       showWindows();
       IString str(Res.currentPKFile());
       if (str != pProfile->GetStringKey( EUOPProfile::ProductKnowledgeFile ))
         pProfile->SetStringKey(EUOPProfile::ProductKnowledgeFile,str);
       fForcePKFileRead = false;
       }
     return true;
   case MSG_FROM_WW_SAVE:
     if (!(unsigned long)evt.parameter1()) {
       MyXHndlr.HandleErrMsgResponse((char *)Res.lastErrorText(),
                                     (char *)ErrTitle);
       SaveFile = IString();
       }
     return WakeUp();
   case MSG_FROM_WW_FORMAT_PPRVW_FILE:
     if (!(unsigned long)evt.parameter1()) {
       MyXHndlr.HandleErrMsgResponse("Trouble formatting Print Preview File",
                                     (char *)ErrTitle);
       HideTimer();
       return WakeUp();
       }
     convertFormattedPrintFile((char *)(unsigned long)evt.parameter2(),true);
     if (fPrintAvailable)
       pPPrvW = new PPrvW(WND_PPRVW,0,this,helpWindow(),pProfile,
                          0,fPrintAvailable ,/*edwtest prt.charCount()*/ 78 ,20, prt.font(),
                          prt.points() );
     else pPPrvW = new PPrvW(WND_PPRVW,0,this,helpWindow(),pProfile,
                             0,fPrintAvailable);
     pPPrvW->import((char *)(unsigned long)evt.parameter2());
     pPPrvW->Center();
     HideTimer();
     WakeUp();
     ableTheKids(false);
     pPPrvW->showModally();
     ableTheKids(true);
     delete pPPrvW, pPPrvW = 0;
     remove((char *)(unsigned long)evt.parameter2()); // formatted file
     return true;
   case MSG_FROM_WW_PRINT:
   case MSG_FROM_WW_T_PRT:
     HideTimer();
     return WakeUp();
   case MSG_FROM_WW_WRITE:
     if (!bUpLoad)
        {
        close();
        return WakeUp();
        }
   case MSG_FROM_WW_HOST_CPF_FAIL:
     Able(True);
     setFocus();
     return True;
   case MSG_FROM_WW_SET_FOCUS_TO_MAIN:
     setFocus();
     return True;
   }
 return false;
}

Boolean XWindow :: WorkWindowPostBack( unsigned long id, unsigned long p1
                                      ,unsigned long p2)
{
 IFrameWindow::postEvent(id,p1,p2);
 return true;
}

Boolean XWindow :: postEventToWW ( unsigned long  eventId,
                                   const IEventParameter1 &parm1,
                                   const IEventParameter2 &parm2) const
{
 return WorkWindow::postEvent(eventId,parm1,parm2);
}

IMessageBox::Response XWindow :: WWHandleMessage(char * szMsg,
                                                 unsigned long HelpId,
                                                 IMessageBox::Style style)
{
 strMB = IString(szMsg);
 styleMB = style;
 HelpIdMB = HelpId;
 // TR141: remove F1 from message box which causes an error message from
 // ICLUI stating it can't find help panel for focused control
 if (!HelpId)
   AbleHelpHandler(false);
 IFrameWindow::sendEvent(MSG_FROM_WW_DO_MSGBOX);
 if (!HelpId)
   AbleHelpHandler();
 return respMB;
}

IMessageBox::Response XWindow :: WWHandleErrorMsg(char * szMsg,
                                                  unsigned long HelpId,
                                                  IMessageBox::Style style)
{
 strMB = IString(szMsg);
 styleMB = style;
 HelpIdMB = HelpId;
 IFrameWindow::sendEvent(MSG_FROM_WW_DO_ERRMSGBOX);
 return respMB;
}

Boolean XWindow :: convertFormattedPrintFile(char * filename, Boolean fToDisplay)
{
 IString strPB(PageHdr::strPageBreak)
        ,strDPB(PageHdr::displayPageBreak(78 /*edw03/29/99fPrintAvailable? prt.charCount():78*/));
 VFile vf;
 Boolean fOK = vf.openVF(filename);
 if (fOK) {
   fOK = fToDisplay?
    vf.replaceAll((char *)strPB,strPB.length(),(char *)strDPB,strDPB.length()) :
    vf.replaceAll((char *)strDPB,strDPB.length(),(char *)strPB,strPB.length());
   vf.closeVF();
   }
 return fOK;
}

unsigned long XWindow :: WWShowMismatch(char * szB1,char * szB2, char * szTxt)
{
 pMMWnd->SetText(IString(szTxt));
 pMMWnd->SetRB1Text(IString(szB1));
 pMMWnd->SetRB2Text(IString(szB2));
 return (unsigned long)(IFrameWindow::sendEvent(MSG_FROM_WW_DO_MISMATCH));
}

//53915 MAMT
unsigned long XWindow :: WWShowMismatchS390(char * szTxt)
{
 pMMWnd->SetText(IString(szTxt));
 return (unsigned long)(IFrameWindow::sendEvent(MSG_FROM_WW_DO_MISMATCH));
}

void XWindow :: SetResolvedState(Boolean f)
{
 SetMenu(f? MWMS_SYS_RESOLVED : MWMS_NEW_SYS);
}

void XWindow :: changedRPOList()
{
 if (!pProfile->GetBooleanKey(EUOPProfile::PrcssGenRPO) ||
     !Res.rpos().numberOfElements())
    menubar.disableItem(WND_MAIN_MI_RPOW);
 else menubar.enableItem(WND_MAIN_MI_RPOW);
 pRPOW->FillList(&Res.rpos());
}

void XWindow :: SetMenu(int state)
{
 switch(state) {
   case MWMS_INITIAL:
     menubar.disableItem(WND_MAIN_MI_PRINT)
            .disableItem(WND_MAIN_MI_PPRVW)
            .disableItem(WND_MAIN_MI_VIEW);
     fResolved = FALSE;
     break;
   case MWMS_NEW_SYS:
     menubar.disableItem(WND_MAIN_MI_PRINT)
            .disableItem(WND_MAIN_MI_PPRVW)
            .enableItem(WND_MAIN_MI_VIEW);
     if (!Res.softwareDiscrepancyList().numberOfElements())
       menubar.disableItem(WND_MAIN_MI_SW);
     else menubar.enableItem(WND_MAIN_MI_SW);
     if (!Res.backlogList().numberOfElements())
       menubar.disableItem(WND_MAIN_MI_BLW);
     else menubar.enableItem(WND_MAIN_MI_BLW);
     adjust2Profile();
     changedRPOList();
     fResolved = FALSE;
     break;
   case MWMS_SYS_RESOLVED:
     adjust2Profile();
     if (fPrintAvailable)
       menubar.enableItem(WND_MAIN_MI_PRINT);
     menubar.enableItem(WND_MAIN_MI_PPRVW);
//            .enableItem(WND_MAIN_MI_GENCFR);
     changedRPOList();
     fResolved = TRUE;
     //edw
     WWShowTimer();
     WWRe_StartTimer(STR_EndReconciliation);
   }
}

void XWindow::adjust2Profile()
{
 if (!pProfile->GetBooleanKey(EUOPProfile::PrcssGenRPO)) {
   menubar.disableItem(WND_MAIN_MI_RPOW);
   if (pRPOW->isVisible())
     pRPOW->hide();
   }
 else
    if ( Res.rpos().numberOfElements() )        //edw01/28/98
       menubar.enableItem(WND_MAIN_MI_RPOW);
}

void XWindow :: WWSetTimerText(const char *T)
{
 strTimerText = IString(T);
 IFrameWindow::sendEvent(MSG_FROM_WW_SET_TIMER_TEXT);
}

void XWindow :: WWRe_StartTimer(const char *T)
{
 strTimerText = IString(T);
 IFrameWindow::sendEvent(MSG_FROM_WW_RE_START_TIMER);
}

void XWindow :: WWHideTimer()
{
 IFrameWindow::sendEvent(MSG_FROM_WW_HIDE_TIMER);
}

void XWindow :: WWShowTimer()
{
 IFrameWindow::sendEvent(MSG_FROM_WW_SHOW_TIMER);
}

void XWindow :: WWStopTimer(const char *T)
{
 strTimerText = IString(T);
 IFrameWindow::sendEvent(MSG_FROM_WW_STOP_TIMER);
}

void XWindow :: WWShowDetails(Boolean fMRDB)
{
 IFrameWindow::sendEvent(MSG_FROM_WW_SHOW_DETAILS,(unsigned long)fMRDB);
}

Boolean XWindow :: WorkWindowMessage(IEvent& evt)
{
 switch(evt.eventId()) {
   /*jt
   case MSG_TO_WW_CLOSE_ABTDLG:
     if (pAbtDlgT2) {
       pAbtDlgT2->close();
       delete pAbtDlgT2;
       pAbtDlgT2 = 0;
       }
     return true;
   case MSG_TO_WW_DO_ABTDLG:
     pAbtDlgT2 = new EUOPAboutDlg(DLG_ABOUT,IWindow::desktopWindow());
     pAbtDlgT2->Center();
     pAbtDlgT2->show();
     return true;
   */
   case MSG_TO_WW_RESOLVE:
     return WorkWindowPostBack((unsigned long)evt.parameter1(),
                (unsigned long)Res.resolveX((char *)AASFile,(char *)MRDFile,
                                            (char *)MRDDescFile,
                                            (char *)(pProfile->MessageFile()),
                                            pNoCompTable,
                                            pQuickFixTable,   //49222-MAMT-06/27/2000
                                            pFeature54Table,  //53915-MAMT-09/01/2000
                                            pISeriesSWTable,  //F72273 - HECC 06/28/2002
               (char *)pProfile->GetStringKey(EUOPProfile::ProductKnowledgeFile),
                                            fForcePKFileRead,
                                            pMain->bS390,
                                            pMain->bAS4 ));   //Include bAS4 flag to reconcile iSeries SW.  HECC 72273 06/27/2002
   case MSG_TO_WW_REBUILD:
     return WorkWindowPostBack((unsigned long)evt.parameter1(),
           (unsigned long)Res.reBuild((char *)(unsigned long)evt.parameter2()));
   case MSG_TO_WW_SAVE:
     return WorkWindowPostBack((unsigned long)evt.parameter1(),
             (unsigned long)Res.save((char *)(unsigned long)evt.parameter2()));
   case MSG_TO_WW_T_PRT:
     return WorkWindowPostBack((unsigned long)evt.parameter1(),
                               (unsigned long)testPrint());
   case MSG_TO_WW_FSPOOL: {    // from the print-preview window -> print my file
//jt     DosSleep(1000);
     convertFormattedPrintFile((char *)(unsigned long)evt.parameter1(),false);
     Boolean fOK = prt.fSpool((char *)(unsigned long)evt.parameter1(),
                              (char *)(unsigned long)evt.parameter1(),
                              (char *)PageHdr::strPageBreak, false);
     ((IWindow *)(unsigned long)evt.parameter2())->postEvent(
                                             evt.eventId(),(unsigned long)fOK);
     return true;
     }
   case MSG_TO_WW_FORMAT_PPRVW_FILE: {
     Boolean fOK = false;
     FILE * f = fopen((char *)(unsigned long)evt.parameter2(),"wb");
     if (f) {
       IString dateFormat(pProfile->strftimeNLSFormattingString('y'));
       //jtest fPrintAvailable = 0;
       fOK = fPrintAvailable? Res.Format(//prt.charCount(),prt.lineCount(),0, //jtest
                                         78, 50, 0,
                                         //Columns::Solid,f,   //jtest
                                         Columns::Basic,f,
                                         (const char *)dateFormat)
                                   : Res.Format(78,50,0,Columns::Solid,f,
                                                (const char *)dateFormat);
       fclose(f);
       }
     return WorkWindowPostBack((unsigned long)evt.parameter1(),
                               (unsigned long)fOK,evt.parameter2());
     }
   case MSG_TO_WW_PRINT:
     return WorkWindowPostBack((unsigned long)evt.parameter1(),
                               (unsigned long)printSystem());



   //edw MSG_TO_WW_WRITE modified 03-20-97
   case MSG_TO_WW_WRITE: {
     Boolean fOK = true, fAppendRPOS = false;
     bUpLoad = false;
     bRPOForwarding = false;
     strRPOFile = "";
     IThread thread;
     thread.setStackSize(50000);
     boolean bVMHost;

     /*edw10/19/1999 - AEFORMS has been sunsetted, therefore we are disabling the
                       RPO forwarding/uploading logic until a new solutions for forwarding
                       RPOs to the install inventory system is developed */

     /*edw10/19/1999 - Disable RPO upload function*/
     //edw09/22/1998
     //Find first occurence of a host interface file (ub9host1.exe) in the install dir.

     WIN32_FIND_DATA fileData;
     IString strVMHost = strExecPath + STR_VMHost;
     HANDLE handle = FindFirstFile( (LPCTSTR)strVMHost, &fileData );
     if ( handle != INVALID_HANDLE_VALUE )
        bVMHost = true;
     else
        bVMHost = false;

    if (pMain->pProfile->bProcRPO)
      {
      //bVMHost = false;   //edw10/19/99 - Disable RPO upload function     cmt'd for 44583 - erg 11/22/99
       if (pProfile->GetBooleanKey(EUOPProfile::PrcssGenRPO) &&
          Res.rpos().numberOfElements() > 0 && bVMHost) {
          IMessageBox::Response resp =
                 WWHandleMessage( ResLib.loadString( IDS_ForwardRPO_Text )
                                 ,IDH_PNL_UPL_MSGBOX
                                 ,IMessageBox::yesNoButton
                                  |IMessageBox::queryIcon
                                  |IMessageBox::defButton2
                                  |IMessageBox::moveable
                                  |IMessageBox::systemModal );    //ZOR 060900 def 51688
        switch(resp) {
           case IMessageBox::yes:
             {
              fOK = true;
              bUpLoad = false; // true;
              //fOK = DoRPOForwarding();           cmt'd for 44583

                  IMessageBox::Response fwdRPO_resp =
                  WWHandleMessage ( ResLib.loadString( IDS_MessageRPO_Text )
                                   ,IDH_PNL_MES_RPO
                                   ,IMessageBox::okButton
                                    |IMessageBox::informationIcon
                                    |IMessageBox::moveable
                                    |IMessageBox::systemModal );      //ZOR 060900 def 51688

                 break;
               }
           default: fAppendRPOS = false; // append logic neutralized 5-11-95
           }
        }
     }
     else
        {
        //53915-MAMT-09/01/2000
        if (pFeature54Table->numberOfElements() == 1 )
        {
        IMessageBox::Response resp =
                WWHandleMessage( ResLib.loadString( IDS_ReconcileComplete_Text )
                                ,IDH_ReconcileComplete
                                ,IMessageBox::okButton
                                 |IMessageBox::informationIcon
                                 |IMessageBox::moveable
                                 |IMessageBox::systemModal );  //ZOR 060900 defect 51688
        }
        fOK = true;
        bUpLoad = false;      //47700 - flw
        }

     if (!fOK)
       return WorkWindowPostBack((unsigned long)evt.parameter1(),false);
     IString strCFR = AASFile;
//     IString strCFR2 = AASFile;                       // D72623 EDW 07/01/2002
//     IString strRET = AASFile;                        // D72623 EDW 07/01/2002
     strCFR.remove(strCFR.lastIndexOf('.') + 1);
//     strCFR += IString("CFR");                        // D72623 EDW 07/01/2002
     strCFR += IString("RET");                          // D72623 EDW 07/01/2002
     if (!(fOK =  Res.writeReport(strCFR, fAppendRPOS, bSetLockingFlag)))
         WWHandleErrorMsg((char *)Res.lastErrorText());
     if (fOK)
        {
//        strRET.remove(strRET.lastIndexOf('.') + 1);    // D72623 EDW 07/01/2002
//        strCFR2 = strRET;                              // D72623 EDW 07/01/2002
//        strRET += IString("RET");                      // D72623 EDW 07/01/2002
//        strCFR2 += IString("CFREPORT");                // D72623 EDW 07/01/2002
//jt        DosCopy(strCFR, strRET, DCPY_EXISTING);
//jt        DosCopy(strCFR, strCFR2, DCPY_EXISTING);
//        CopyFile(strCFR, strRET, FALSE);  //jt         // D72623 EDW 07/01/2002
//        CopyFile(strCFR, strCFR2, FALSE); //jt         // D72623 EDW 07/01/2002
        }
/*edw
     if (fOK && IMessageBox::yes ==
                 WWHandleMessage("Do you want to upload the CFReport file?",0,
                                  IMessageBox::yesNoButton
                                 |IMessageBox::queryIcon
                                 |IMessageBox::defButton2
                                 |IMessageBox::applicationModal
                                 |IMessageBox::moveable
					   |IMessageBox::systemModal))
           {
//           fOK = UploadCFReport(IString(strCFR));
           strCFRFile = strCFR;
           bUpLoad = true;
           fOK = true;
           }*/

     if ( bUpLoad )
        {
        WWShowTimer();
        WWRe_StartTimer("Uploading RPOs ");//edw and/or CFReport");
        thread.start(new IThreadMemberFn<XWindow>(*this, openHost));
        HideTimer();
        }


     //close(); edw
     return WorkWindowPostBack((unsigned long)evt.parameter1(),fOK);
     }

   case MSG_TO_WW_START_HOST_CPF:
//jt     if (!GetNewData())
       WorkWindowPostBack((unsigned long)MSG_FROM_WW_HOST_CPF_FAIL);
   }
 return false;
}

void XWindow :: HideTimer()
{
 //jtest pTimer->StopCounting();
 //jtest pTimer->hide();
 TextTimer.hide();
}

void XWindow :: ShowTimer()
{
 //jtest pTimer->Reset();
 //jtest pTimer->show();
 TextTimer.setText(" ");
 TextTimer.show();
}

void XWindow :: Re_StartTimer(const char * szText)
{
 //jtest pTimer->Reset();
 if (szText)
   TextTimer.setText((char *)szText);
 //jtest if (!pTimer->isVisible())
 //jtest  pTimer->show();
 if (!TextTimer.isVisible())
   TextTimer.show();
 setFocus();
 //jtest pTimer->StartCountingUp();
}

Boolean XWindow :: systemCommand(ICommandEvent& evt)
{
 switch(evt.parameter1()) {
   case SC_CLOSE:
   return false;
   }
 return false;
}

Boolean XWindow :: closed(IFrameEvent& evt)
{
  // Be sure to kill the Host CPF process if it exists

  //jt  if (ulHostPID != 0UL)
  //jt    DosKillProcess(DKP_PROCESS,(PID)ulHostPID);

 //edw12/29/1999 - Write out user descisions
 static boolean bDone = false;
 if( !bDone )
 {
    if( iUseOldMRDBUserChoice == 1 )
       {
        pMain->addUserChoice( (IString)STR_REC05_MRDBQuit );
        //D106441 RERC 02/21/2006 Next line was commented because the text was added to the line above
        //pMain->addUserChoice( (IString)STR_REC05_MRDBQuit2 );   //ZOR IC65198
       }
    else if( iUseOldMRDBUserChoice == 2 )
       {
        pMain->addUserChoice( (IString)STR_REC05_MRDBContinue );
        pMain->addUserChoice( (IString)STR_REC05_MRDBContinue2 );  //ZOR IC65198
       }
    if( iContMismatchesUserChoice == 1 )
        pMain->addUserChoice( (IString)STR_REC05_MismatchesQuit );
    else if( iContMismatchesUserChoice == 2 )
       {
       pMain->addUserChoice( (IString)STR_REC05_MismatchesContinue );
       //D106441 RERC 02/21/2006 Next line was commented because the text was added to the line above
       //pMain->addUserChoice( (IString)STR_REC05_MismatchesContinue2 );  //ZOR IC65198
       }
    if( iUseAASMRDBUserChoice == 1 )
       {
       pMain->addUserChoice( (IString)STR_REC05_ReconcileUseMRDB2 );
       pMain->addUserChoice( (IString)STR_REC05_ReconcileUseMRDB23 );  //ZOR IC65198
       }
    else if( iUseAASMRDBUserChoice == 2 )
      {
       pMain->addUserChoice( (IString)STR_REC05_ReconcileUseAAS2 );
       pMain->addUserChoice( (IString)STR_REC05_ReconcileUseAAS23 );   //ZOR IC65198
      }
    bDone = true;
 }

 WorkWindow::postEvent((unsigned long)WM_QUIT);
 MessageBeep(MB_ICONEXCLAMATION);     //jt
 //jtest pTimer->StopCounting();
 kids.Close();
 delete pDiscW;
 delete pResoW;
 delete pProfW;
 delete pBLW;
 delete pSWDW;
 delete pRPOW;
 delete pPPrvW;
 delete pXW;
 delete pMMWnd;
 return IFrameHandler::closed(evt);
}

IString XWindow::printerQueue()
{ return prt.printerQueue(); }

Boolean XWindow :: command(ICommandEvent & cmdEvent)
{
 switch (cmdEvent.commandId()) {
   case  WND_MAIN_MI_T_STUB_RESOLVED:
     SetResolvedState();
     return true;
   case WND_MAIN_MI_T_MMW: {
     IString str;
     pMMWnd->showModally();
     switch(pMMWnd->result()) {
       case 0: str = "You canceled."; break;
       case 1: str = "You picked button 1."; break;
       case 2: str = "You picked button 2.";
       }
     MyXHndlr.HandleMsgResponse((char *)str,"Test MMW");
     setFocus();
     return true;
     }
   case WND_MAIN_MI_T_PRT:
     WorkWindow::postEvent( (unsigned long)MSG_TO_WW_T_PRT
                           ,(unsigned long)MSG_FROM_WW_T_PRT);
     Able(false);
     return true;
   case WND_MAIN_MI_PPRVW:
   case WND_MAIN_MI_T_PPRVW:
     //edw04/07/99-if preview window exists, do not create another one
     if ( pPPrvW )
        {
        if (!pPPrvW->isVisible())
           {
           pPPrvW->show();
           pPPrvW->addToWindowList();
           }
        pPPrvW->setFocus();
        return true;
        }

     Re_StartTimer("Formatting");
     Able(false);
     WorkWindow::postEvent( (unsigned long)MSG_TO_WW_FORMAT_PPRVW_FILE
                           ,(unsigned long)MSG_FROM_WW_FORMAT_PPRVW_FILE
                           ,(unsigned long)pPPRVW_TEMP_FILE);
     return true;
   case WND_MAIN_MI_T_PK: cout << Res.pk() << endl; return true;
   case WND_MAIN_MI_T_MRL: cout << Res.MRDescList() << endl; return true;
   case WND_MAIN_MI_T_SML: cout << Res.softwareDiscrepancyList() << endl; return true;
   case WND_MAIN_MI_T_SD: Res.SysDataOut(cout); return true;
   case WND_MAIN_MI_T_RPOL: cout << Res.rpos() << endl; return true;
   case WND_MAIN_MI_T_XPLNL: cout << Res.explanations() << endl; return true;
   case WND_MAIN_MI_T_FL: cout << Res.featlist() << endl; return true;
   case WND_MAIN_MI_T_ML: cout << Res.machList() << endl; return true;
   case WND_MAIN_MI_T_BLL: cout << Res.backlogList() << endl; return true;
   case WND_MAIN_MI_T_L10: cout << Res.subsystemRecordList() << endl; return true;

   case WND_MAIN_MI_SW:
     if (fFirstSWDWShowing) {
       pSWDW->show();    // needed for next call
       pSWDW->addToWindowList();                                 //edw-12/5/97
       pSWDW->SizeToMaximumTextWidth();
       pSWDW->Center();
       fFirstSWDWShowing= false;
       }
     if (pSWDW->isMinimized())                                   //edw-12/5/97
       pSWDW->restore();                                         //edw-12/5/97
     if (!pSWDW->isVisible())                                    //edw-12/5/97
        {
        pSWDW->show();
        pSWDW->addToWindowList();                                //edw-12/5/97
        }
     pSWDW->setFocus();
     return true;
   case WND_MAIN_MI_BLW:
     if (!pBLW->isVisible())
        {
        pBLW->show();
        pBLW->addToWindowList();                                 //edw-12/5/97
        }
     pBLW->setFocus();
     return true;
   case WND_MAIN_MI_DISCW:
     if (pDiscW->isMinimized())
       pDiscW->restore();
     if (!pDiscW->isVisible())
        {
        pDiscW->show();
        pDiscW->addToWindowList();                               //edw-12/5/97
        }
     pDiscW->setFocus();
     return true;
   case WND_MAIN_MI_RESW:
     if (pResoW->isMinimized())
       pResoW->restore();
     if (!pResoW->isVisible())
        {
        pResoW->show();
        pResoW->addToWindowList();                               //edw-12/5/97
        }
     pResoW->setFocus();
     return true;
  case WND_MAIN_MI_EXIT:
    //edw
     if ( fResolved )
        {
        WorkWindow::postEvent( (unsigned long)MSG_TO_WW_WRITE
                      ,(unsigned long)MSG_FROM_WW_WRITE);
        Able(false);
        return true;
        }
     else
        {
        CompleteReconcile *pComplRec;
        pComplRec = new CompleteReconcile( this, pMain );
        IPoint mainPos = this->position();
        mainPos.setX( mainPos.x() + 20 );
        mainPos.setY( mainPos.y() + 20 );
        pComplRec->moveTo( mainPos );
        if ( pDiscW )
           pDiscW->hide();
        if ( pResoW )
           pResoW->hide();
        pComplRec->addToWindowList();
        pComplRec->setFocus();
        pComplRec->showModally();

        if ( pComplRec->bContinue )
           {
           //edw1
           if ( !pDiscW->numberOfObjects() && !pResoW->numberOfObjects() )
              IFrameWindow::postEvent(IWindow::command,WND_MAIN_ID_RESOLVE);
           else
              {
              if ( pDiscW )
                 pDiscW->show();
              if ( pResoW )
                 pResoW->show();
              }
           delete pComplRec;
           return true;
           }
        else
           {
           delete pComplRec;
           close();
           }
        //edw06/24/1998-Replaced message with new dialog above
        //IMessageBox mesg(this);
        //IMessageBox::Style style = IMessageBox::yesNoButton |
        //                   IMessageBox::queryIcon |
        //                   IMessageBox::moveable;
        //mesg.setTitle(IResourceId(MAINW_MSG_NOTRESOLVED_TITLE));
        //IMessageBox::Response resp = mesg.show(IResourceId(MAINW_MSG_NOTRESOLVED), style, IDH_MAINW_MSG_NOTRESOLVED);
        //if (resp == IMessageBox::no)
        //   return true;
        //else
        //   close();
        return true;
        }
   case WND_MAIN_MI_ABOUT:
     //pAbtDlg->showModally();
     openProductInfo();
     return true;
   case WND_MAIN_MI_RPOW:
     if (fFirstRPOWShowing) {
       pRPOW->show();    // needed for next call
       pRPOW->addToWindowList();                                 //edw-12/5/97
       pRPOW->SizeToMaximumTextWidth();
       pRPOW->Center();
       fFirstRPOWShowing= false;
       }
     pRPOW->show().setFocus();
     pRPOW->addToWindowList();                                   //edw-12/5/97
     return true;
   case WND_MAIN_MI_PRINT:
     Re_StartTimer("Formatting and spooling");
     WorkWindow::postEvent( (unsigned long)MSG_TO_WW_PRINT
                           ,(unsigned long)MSG_FROM_WW_PRINT);
     Able(false);
     return true;
   case WND_MAIN_MI_PROFILE:
     ableTheKids(false);
     pProfW->showModally();
     if (pProfW->result() != DID_CANCEL) {
       pResoW->updated(pProfile);
       pDiscW->updated(pProfile);
       }
     ableTheKids(true);
     adjust2Profile();
     return true;
   case WND_MAIN_MI_NEW:
     SaveFile = IString();
     WorkWindow::postEvent((unsigned long)MSG_TO_WW_START_HOST_CPF);
     Able(false);
     return true;
   case WND_MAIN_MI_PKFILE: {
     int rc, msg;
     IString strCurrentF;
     if (!strReshowPKFile.length()) {         // no reshow string
       strCurrentF = pProfile->GetStringKey(EUOPProfile::ProductKnowledgeFile);
       if (strCurrentF.length() && !readableFile((char *)strCurrentF))
         pProfile->SetStringKey(EUOPProfile::ProductKnowledgeFile,
                                strCurrentF = IString());
       if (!strCurrentF.length())
         strCurrentF = MakeFileSpecStr(0,"*","$$F");
       }
     else strCurrentF = strReshowPKFile;
     IFileDialog::Settings fsettings;
     fsettings.setDialogTemplate( ID_DLG_PKFILE );//jt

     fsettings.setTitle(ResLib.loadString( MAINWND_STR_SET_PKFILE_TITLE ));
     fsettings.setFileName(strCurrentF);
     idFDHelpPanel = IDH_PNL_PKFILEWND;
     IFileDialog fd((IWindow*)desktopWindow(), (IWindow*)this, fsettings,
                     IFileDialog::defaultStyle() | IFileDialog::helpButton);

     switch (fd.buttonPressedId()) {
       case DID_OK_PB: {
         IString strPickedF(fd.fileName());
         if ((strCurrentF != strPickedF) || (strReshowPKFile != IString())) {
           if (strPickedF.length() &&
               (rc=ProductKnowledge::check4CompiledPKFile((char *)strPickedF))) {
             switch (rc) {
               case RBS_ERR_NO_FILE:      msg = MAINW_MSG_ERR_NO_FILE;     break;
               case RBS_ERR_NO_EUOP_FILE: msg = MAINW_MSG_ERR_NO_PKFILE;   break;
               case RBS_ERR_DAMAGED_FILE: msg = MAINW_MSG_ERR_DMGD_PKFILE; break;
               }
             WWHandleMessage((char *)getString(msg, (char *)strPickedF), 0,
                              IMessageBox::okButton |
                              IMessageBox::errorIcon |
                              IMessageBox::applicationModal |
					IMessageBox::systemModal);

             strReshowPKFile = strPickedF;
             IFrameWindow::postEvent(IWindow::command,WND_MAIN_MI_PKFILE);
             return true;
             }
           pProfile->SetStringKey(EUOPProfile::ProductKnowledgeFile,strPickedF);
           fForcePKFileRead = true;
           }
         break;
         }
       case ID_DLG_PKFILE_PK_DFLT:{
         Boolean fFound = false;
         IString strDfltDir = strExecPath;

         if (strDfltDir != IString()) {
           IString strDfltPKFile(MakeFileSpecStr((char *)strDfltDir,
                                                 "FEATURE","$$F"));
           int rc=ProductKnowledge::check4CompiledPKFile((char *)strDfltPKFile);
           if (rc) {
             if (rc != RBS_ERR_NO_FILE)
               remove((char *)strDfltPKFile);
             fFound = (readableFile((char *)MakeFileSpecStr((char *)strDfltDir,
                                                      "FEATURE","TXT")) &&
                       readableFile((char *)MakeFileSpecStr((char *)strDfltDir,
                                                         "FEATURE","TXT")));
             }
           else fFound = true;
           }
         if (fFound) {
           strReshowPKFile = MakeFileSpecStr((char *)strDfltDir,"FEATURE","$$F");
           IFrameWindow::postEvent(IWindow::command,WND_MAIN_MI_PKFILE);
           return true;
           }
         else WWHandleMessage((char *)getString(MAINW_MSG_ERR_NO_PKFILE_IN_DIR,
                                                (char *)strDfltDir),
                              0,
                              IMessageBox::okButton |
                              IMessageBox::errorIcon |
                              IMessageBox::applicationModal |
					IMessageBox::systemModal);
         break;
         }
       }
     strReshowPKFile = IString();
     return true;
     }
   //edw 03-20-97
   case WND_MAIN_ID_RESOLVE: {
     //MRDDescFile = IString(SetFileExtension((char *)strTemp,"DSC"));
     WorkWindow::postEvent((unsigned long)MSG_TO_WW_RESOLVE,
                           (unsigned long)MSG_FROM_WW_RESOLVE);
     Able(false);
     return true;
     }

   case WND_MAIN_PB_RESOLVE: {
     IFileDialog::Settings fsettings;
     fsettings.setTitle(ResLib.loadString( MAINWND_STR_GET_AAS_NAME_TITLE ));
     fsettings.setFileName( MakeFileSpecStr( (char *)strExecPath, "*","BII"));//jt

     idFDHelpPanel = IDH_PNL_FD_RESOLVE;
     IFileDialog fd((IWindow*)desktopWindow(), (IWindow*)this, fsettings,
                     IFileDialog::defaultStyle() | IFileDialog::helpButton);
     if (fd.pressedOK()) {
       AASFile = fd.fileName();
       MRDFile = IString(SetFileExtension((char *)fd.fileName(),"MRD"));
       MRDDescFile=IString(SetFileExtension((char *)fd.fileName(),"DSC"));
       WorkWindow::postEvent((unsigned long)MSG_TO_WW_RESOLVE,
                             (unsigned long)MSG_FROM_WW_RESOLVE);
       Able(false);
       }
     return true;
     }
   case WND_MAIN_MI_GENCFR:
     WorkWindow::postEvent( (unsigned long)MSG_TO_WW_WRITE
                           ,(unsigned long)MSG_FROM_WW_WRITE);
     Able(false);
    return true;
   }
 return false;
}


Boolean XWindow::WakeUp()
{
 Able(true);
 return true;
}

void XWindow::ShowDetails(Boolean fMRDB )
{
 TextMain.setText(Res.title(fMRDB));
 Text1.setText(Res.name(fMRDB));
 Text2.setText(Res.number(fMRDB));
 if (fMRDB)  //edw
    Text3.setText(Res.fileDate(fMRDB).asString(IDate::yyyy));  //edw-uncommented
// IDate date(Res.fileDate(fMRDB));
// Text3.setText(pUserProfile->FormatNLSDateString(date.monthOfYear(),
//                                                 date.dayOfMonth(),
//                                                 date.year()));
//edw IString strDateFrmt(pProfile->strftimeNLSFormattingString());
//edw Text3.setText(Res.fileDate(fMRDB).asString((const char *)strDateFrmt));
 if (fMRDB)
//edw   Text4.setText(Res.senseDate().asString((const char *)strDateFrmt));
  Text4.setText(Res.senseDate().asString(IDate::yyyy));       //edw-uncommented
//   Text4.setText(Res.senseDate().
//         asString((const char *)(pUserProfile->strftimeNLSFormattingString())));
// Text5.setText(Res.security()==0? "Unclassified":
//            (Res.security()==1? "Internal Use Only" : "Confidential"));

 strSecurity = ResLib.loadString(Res.security()==0? MAINWND_STR_UNC :
                    (Res.security()==1? MAINWND_STR_UNC : MAINWND_STR_CONF));
 Text5.setText(strSecurity);
}

void XWindow::BlankDetails()
{
 TextMain.setText("");
 Text1.setText("");
 Text2.setText("");
 Text3.setText("");
 Text4.setText("");
 Text5.setText("");
 HideTimer();  //edw
}

Boolean XWindow::showWindows(void)
{
 //* F73252 Begin New logic. The receive and sensed dates must be displayed in both,
 //* the Matches & Mismatches windows, so these variables are first assigned and then
 //* passed as parameters to the class where these two dialogs are being populated.
 //* HECC: 07/18/2002
 IString strSensedDate(Res.senseDate().asString(IDate::yyyy));

 //* ICB15040873 Begin new logic: The date that must be returned as "Date received at IBM must be
 //* the position '0' because it is being filled from the AAS file.
 //* IString strReceiveDate(Res.fileDate(true).asString(IDate::yyyy));
 IString strReceiveDate(Res.fileDate(false).asString(IDate::yyyy));
 //* ICB15040873 End new logic:  HECC - 08/13/2003

 //* pResoW->load(&(Res.featlist()),&(Res.rpos()),
 //*                &(Res.explanations()),&Res, &(Res.pk()) );
 //* pDiscW->load(&(Res.featlist()),&(Res.rpos()),
 //*                &(Res.explanations()),&Res, &(Res.pk()) );

 pResoW->load(&(Res.featlist()),&(Res.rpos()),
                &(Res.explanations()),&Res, &(Res.pk()), strSensedDate, strReceiveDate);
 pDiscW->load(&(Res.featlist()),&(Res.rpos()),
                &(Res.explanations()),&Res, &(Res.pk()), strSensedDate, strReceiveDate);
 //* F73252 End new logic.

 //jt ITitle(pDiscW).setObjectText((char *)ResLib.loadString(MAINWND_STR_DISC));
 ITitle(pDiscW).setObjectText(  IResourceId(MAINWND_STR_DISC));
 ITitle(pResoW).setObjectText((char *)ResLib.loadString(MAINWND_STR_RESO));
 if (fFirstDiscWShowing){
   ISize sz(desktopWindow()->size().scaleBy(0.8,0.45));
   IPoint pt(rect().left()+borderWidth(),
             rect().bottom() + ITitle(this).size().height() * 3.0);    //jt-6/23/97
             //+ menubar.size().height() + borderHeight() + sz.height()
   if (pMain->bS390 && bShowS390Results )
       ISize sz(desktopWindow()->size().scaleBy(0.8,0.40));
   pDiscW->moveSizeTo(IRectangle(pt, sz));
   if (pMain->bS390 && bShowS390Results )
      pDiscW->moveTo(IPoint(75,30) );
   pResoW->moveSizeTo(IRectangle( pt + IPoint(0,pDiscW->size().height()) ,sz));

 // begin new logic 47971 MAMT 09/26/2000
 if (bTil)
   { // tile horizontal
//     ISize sz1(desktopWindow()->size());
     ISize sz1 = pMain->size();
     int verticalHeight = sz1.height() / 2;
     int verticalWidth  = sz1.width();
     IRectangle myRect = IRectangle(IPoint(0,(0*verticalHeight)),IPoint(verticalWidth,((0+1)*verticalHeight)));
     pDiscW->moveSizeTo(myRect);
     IRectangle myRect1 = IRectangle(IPoint(0,(1*verticalHeight)),IPoint(verticalWidth,((1+1)*verticalHeight)));
     pResoW->moveSizeTo(myRect1);
   }
 else
   { // tile vertical
//     ISize sz1(desktopWindow()->size());
     ISize sz1 = pMain->size();
     int verticalHeight = sz1.height();
     int verticalWidth  = sz1.width() / 2;
     IRectangle myRect  = IRectangle(IPoint((0*verticalWidth),0),IPoint(((0+1)*verticalWidth),verticalHeight));
     pDiscW->moveSizeTo(myRect);
     IRectangle myRect1 = IRectangle(IPoint((1*verticalWidth),0),IPoint(((1+1)*verticalWidth),verticalHeight));
     pResoW->moveSizeTo(myRect1);
   }

 // end new logic 47971 MAMT 09/26/2000

   pBLW->sizeTo(sz);
   fFirstDiscWShowing = false;
   }
 if ( pDiscW->numberOfObjects() )                                //edw07/01/1998
    {
    if ( pProfile->GetBooleanKey(EUOPProfile::ViewRslvdFeat) )
       {
       if( !pMain->bS390 || (pMain->bS390 && bShowS390Results ) )
          {
          pResoW->show().setFocus();
          pResoW->addToWindowList();                             //edw-12/5/97
          if (pMain->bS390 && bShowS390Results)
           pResoW->hide();    //44907
          }
       }

    pDiscW->show().setFocus();
    pDiscW->addToWindowList();                                   //edw-12/5/97

    if ( Res.softwareDiscrepancyList().numberOfElements() )
       {
       if( !pMain->bS390 || (pMain->bS390 && bShowS390Results ) )
          {
          pSWDW->show().setFocus();
          pSWDW->addToWindowList();                              //edw-12/5/97
          }
       }
    }
 addToWindowList();
// else pDiscW->setFocus();
 return true;
}


Boolean XWindow :: printSystem()
{
 Boolean fOK = false;
 IString JobName(MakeFileName(BufK,findFileName((char *)AASFile),"PRT"));
 FILE * f = fopen( pPRINT_TEMP_FILE , "wb");
 if (f) {
     Res.Format(78,50,0,Columns::Basic,f);//jtest
/*jtest   if (fPrintAvailable)
     Res.Format(prt.charCount(),prt.lineCount(),0,Columns::Solid,f);
   else Res.Format(78,50,0,Columns::Solid,f);*/
   fclose(f);
   if (fOK = prt.fSpool(pPRINT_TEMP_FILE,(char *)JobName,
                        (char *)PageHdr::strPageBreak))
     WWHandleMessage((char *)getString(MAINW_MSG_SPOOL_JOB,
                                       (char *)JobName,
                                       (char *)prt.printerQueue()));
   else
      if ( prt.lastErrorText().length())
        WWHandleMessage((char *)prt.lastErrorText());
   }
 return fOK;
}

////////////////////////////////////////////////////////////////////////

Boolean XWindow :: testPrint()
{
 char * Buf;
 FILE * f;
 IString JobName(MakeFileName(BufK,findFileName((char *)AASFile),"PRT"));
// if (prt.Format(Res,78,55,&Buf))
 if (Res.Format(78,55,&Buf,Columns::Solid))
   {
     if (f = fopen("EUOPPRT.TSB","wb"))
     {
       fputs(Buf,f);
       fclose(f);
     }
   }
 f = fopen("EUOPPRT.TSF","wb");
  if (f)
     {
//       prt.Format(Res,78,50,0,f);
       Res.Format(78,50,0,Columns::Solid,f);
       fclose(f);
     }
 prt.spool(Buf,(char *)JobName);
 WWHandleMessage((char *)Res.message(EUO_MSG_SPOOLED,(char *)JobName));

 f = fopen( "EUOPPRT.$$$" ,"wb");
  if (f)
     {
//       prt.Format(Res,78,50,0,f);
       Res.Format(78,50,0,Columns::Solid,f);
       fclose(f);
     }

 prt.fSpool("EUOPPRT.$$$","EUOPPRT.SPL",(char *)PageHdr::strPageBreak);
 WWHandleMessage((char *)Res.message(EUO_MSG_SPOOLED,"EUOPPRT.SPL"));


 return true;
}


/*************************************************************************/
/*                                                                       */
/*  RPO Uploading stuff                                                  */
/*                                                                       */
/*************************************************************************/

Boolean  XWindow :: DoRPOForwarding()
{
  RPOForwardingData forwardData;
  IString error;
  IString rc;

  //jt  RpoinfoFrame frm( desktopWindow(), this, forwardData, pProfile);
  RpoinfoFrame frm( this, forwardData, pProfile);

  // Set focus to the dialog's frame and show it as a modal dialog
  frm.setFocus();
  frm.showModally();

  // use rpoWindow.result() to determine how the panel was
  // dismissed.
  if (frm.result() != IDC_PB_OK)
    return false;

  WWShowTimer();
  WWRe_StartTimer(STR_CreateRPOReport_Text);
  bRPOForwarding = TRUE;
  rc = uploadRPOs(Res.rpos(),forwardData);
  if (rc == RPO_UPLOAD_HLLAPI_ERROR)
     {
     error = ResLib.loadString(ERROR_UPLOAD_RPO_HLLAPI_ERR);
     WWHandleErrorMsg(error,0,IMessageBox::okButton |
                               IMessageBox::errorIcon |
                               IMessageBox::applicationModal |
					IMessageBox::systemModal);
     HideTimer();
     return false;
     }
  else if (rc == RPO_UPLOAD_FILE_ERROR)
     {
     error = ResLib.loadString(ERROR_RPO_FILE_ERROR   );
     WWHandleErrorMsg(error,0,IMessageBox::okButton |
                               IMessageBox::errorIcon |
                               IMessageBox::applicationModal |
					IMessageBox::systemModal );
     HideTimer();
     return false;
     }
  else
     {
     strRPOFile = rc;
//     thread.start(new IThreadMemberFn<XWindow>(*this, openHost));
//     bRPOForwarding = FALSE;
     HideTimer();
     }
 return True;
}


Boolean  XWindow :: UploadCFReport(IString fileName)
{
//  IThread thread;
//  thread.setStackSize(50000);

//  WWShowTimer();
//  WWRe_StartTimer("Uploading CFReport");
//  bRPOForwarding = FALSE;
  strCFRFile = fileName;
//  thread.start(new IThreadMemberFn<XWindow>(*this, openHost));
//  HideTimer();
  return True;
}

//=======================================================================
// FUNCTION NAME    openProductInfo
//
// DESCRIPTION      Open the product information dialog.
//=======================================================================

void XWindow :: openProductInfo()
   {
   IMessageBox::Style style = IMessageBox::okButton |
                              IMessageBox::informationIcon |
                              IMessageBox::moveable |
					IMessageBox::systemModal;
   IMessageBox msg(this);
   msg.setTitle(IResourceId(IDS_Product_Title));
   IString strText = LOADSTRING(IDS_Product_Text);
   strText.change(STR_Substitute0, STR_Version);
   msg.show(strText, style);
   }

//=======================================================================
// FUNCTION NAME    openHost
//
// DESCRIPTION      Open the dialog to establish a connection to the host.
//=======================================================================

void XWindow :: openHost()
   {
   bHost = true;
   IString strTempFile = tmpnam(NULL);
//edw1/7/98   IString strAssetMgmt = STR_AssetManage;
   FILE* pTempFile = fopen(strTempFile, "w");

   fprintf(pTempFile, "%s\n", STR_TagLocale);
   IString strLocale = STR_ApplyLocale;
   fprintf(pTempFile, "%s\n", strLocale);

   if ( strRPOFile != "" )
      {
      fprintf(pTempFile, "%s\n", STR_TagRename);        //edw1/14/98
      fprintf(pTempFile, "%s\n", STR_ExtToExec);        //edw1/14/98
      fprintf(pTempFile, "%s\n", STR_TagFiles);
      fprintf(pTempFile, "%s\n", strRPOFile);
      fprintf(pTempFile, "%s\n", STR_TagUpload);
      fprintf(pTempFile, "%s\n", STR_TagExecute);       //edw1/14/98
      fprintf(pTempFile, "%s\n", STR_RPOExecCommand);   //edw1/14/98
//edw1/7/98      fprintf(pTempFile, "%s\n", STR_TagSendTo);
//edw1/7/98      fprintf(pTempFile, "%s\n", strAssetMgmt);
//edw1/7/98      fprintf(pTempFile, "%s %s\n", STR_TagSend);
      }

   fclose(pTempFile);

   IString strHost = strExecPath + STR_VMHost;          //edw09/22/98
   //edw
   menubar.disableItem(WND_MAIN_MI_VIEW);
   menubar.disableItem(WND_MAIN_MI_FILE);
   sysMenu.disableItem(ISystemMenu::idClose);
   LONG lRC = _spawnl(P_WAIT, strHost, strHost, strTempFile, NULL);
   if ( !lRC )
      {
      pTempFile = fopen(strTempFile, "r");
      fclose(pTempFile);
      //edw
      close();
      }
   else
      if ( lRC == -1 )
         {
         IMessageBox msg(this);
         IMessageBox::Style style = IMessageBox::retryCancelButton |     //jt
                                    IMessageBox::warningIcon |
                                    IMessageBox::moveable |
						IMessageBox::systemModal;
         msg.setTitle(IResourceId(IDS_ErrorRetrieve_Title));
         IMessageBox::Response response = msg.show(IResourceId(IDS_ErrorRetrieve_Text),
                             style, IDH_ErrorRetrieve);
         IThread thread;
         if ( response == IMessageBox::retry )//edw--07/22/97
           thread.start(new IThreadMemberFn<XWindow>(*this, openHost));
         }
   remove(strTempFile);
   if ( bRPOForwarding )
      remove(strRPOFile);
   bHost = false;
   menubar.enableItem(WND_MAIN_MI_VIEW);
   menubar.enableItem(WND_MAIN_MI_FILE);
   sysMenu.enableItem(ISystemMenu::idClose);
   }

/* //jt--replaced by ub9host.exe
Boolean  XWindow :: GetNewData()
{
  RESULTCODES resultCodes;
  CHAR        objBuf[256] = "";
  IString     pgm("RM2SHELL.EXE");
  APIRET      rc = 0;
  IString     args;

  // Get the path to start the EUOPSHEL Host Retrieval CPF
  args = IApplication::current().argv(0);
  args.remove(args.lastIndexOf('\\'));

  args += IString('\\') + pgm + IString("\0\0\0");
//  fprintf(stderr,"DosExecPgm Args: %s\n",args); // was for debugging
  resultCodes.codeTerminate = INT_MAX;
  resultCodes.codeResult = INT_MAX;

  rc = DosExecPgm(objBuf,100,EXEC_ASYNCRESULT,args,NULL,&resultCodes,pgm);
  if (rc !=0)
    {
    IString str("An error occurred starting the Host Retrieval CPF. Return code: ");
    str += IString(rc);
    str += IString('\n') + pgm;
    WWHandleErrorMsg(str,0,
                     IMessageBox::okButton |
                     IMessageBox::errorIcon |
                     IMessageBox::applicationModal |
			IMessageBox::systemModal);
    return False;
    }
  ulHostPID = resultCodes.codeTerminate; // the Process ID is stored in the first
                                         // unsigned long
  rc = DosWaitChild(DCWA_PROCESS,DCWW_WAIT,&resultCodes,&ulHostPID,ulHostPID);
  ulHostPID = 0UL;
  if (rc !=0)
    {
    IString str("An error occurred waiting on the Host Retrieval CPF. Return code: ");
    str += IString(rc);
    str += IString('\n') + pgm;
    WWHandleErrorMsg(str,0,
                     IMessageBox::okButton |
                     IMessageBox::errorIcon |
                     IMessageBox::applicationModal |
			  IMessageBox::systemModal);
    return False;
    }
  if (resultCodes.codeResult != 0)
    return False;

  WorkWindowPostBack(MSG_FROM_WW_SET_FOCUS_TO_MAIN);

  // Copy the $RM2$.* files to RM2.*  and delete the $RM2$.* files
  IString workDir = pProfile->GetStringKey(EUOPProfile::WorkDir);

  if (rc = DosCopy(workDir + "\\$RM2$.BII",workDir + "\\RM2.BII",DCPY_EXISTING) == NO_ERROR)
    if (rc = DosCopy(workDir + "\\$RM2$.MRD",workDir + "\\RM2.MRD",DCPY_EXISTING) == NO_ERROR)
      if (rc = DosCopy(workDir + "\\$RM2$.DSC",workDir + "\\RM2.DSC",DCPY_EXISTING) == NO_ERROR)
        if (rc = remove(workDir + "\\$RM2$.BII") == NO_ERROR)
          if (rc = DosDelete(workDir + "\\$RM2$.MRD") == NO_ERROR)
            if (rc = DosDelete(workDir + "\\$RM2$.DSC") == NO_ERROR)
              rc=0;

  if (rc !=0)
    {
    IString str("An error occurred renaming the downloaded files. Return code: ");
    str += IString(rc);
    WWHandleErrorMsg(str,0,
                     IMessageBox::okButton |
                     IMessageBox::errorIcon |
                     IMessageBox::applicationModal |
				IMessageBox::systemModal);
    return False;
    }

  AASFile     = workDir + "\\RM2.BII";
  MRDFile     = workDir + "\\RM2.MRD";
  MRDDescFile = workDir + "\\RM2.DSC";

  return WorkWindowPostBack(MSG_FROM_WW_RESOLVE,
            (unsigned long)Res.resolveX((char *)AASFile,(char *)MRDFile,
                                       (char *)MRDDescFile,
                                       (char *)(pProfile->MessageFile()),
           (char *)pProfile->GetStringKey(EUOPProfile::ProductKnowledgeFile)
                                       ));
}
*/
