#ifndef _AB_MAINWNDX_HPP_
#define _AB_MAINWNDX_HPP_

#define INCL_IFRAMEWINDOW
#define INCL_IFRAMEHANDLER
#define INCL_ISTATICTEXT
#define INCL_IHELPWINDOW
#define INCL_IHELPHANDLER
#define INCL_IMULTICELLCANVAS
#define INCL_ICOMMANDHANDLER
#define INCL_IMESSAGEBOX
#define INCL_ISTRING
#define INCL_IEVENT
#define INCL_IPUSHBUTTON
#define INCL_IVIEWPORT
#define INCL_IMENUBAR
#define INCL_ISYSTEMMENU
#include "uicl.hpp"

#include <iseq.h>
#include <isrtset.h>              //edw07/01/1998

#include "basew.hpp"
#include "myxcphdr.hpp"
#include "ww.hpp"
#include "euopres.hpp"
#include "euopiprt.hpp"
#include "minmxhdr.hpp"
#include "af9main.hpp"

class EUOPProfile;
class MinSecTimer;
class MMWnd;
class EUOPResolve;
class SWDWndw;
class RPOsWndw;
class PPrvW;
class EUOPAboutDlg;
//class BkLogWnd;
class RMBkLogW;
class XPLNWndw;
class RMDiscResoW;
class EUOPProfileWndw;
//47973 - MAMT
class SpecWindow;

#define MSG_TO_WW_FSPOOL             WM_USER+42
#define MSG_FROM_WW_FSPOOL           WM_USER+43


#pragma pack(4)

class Kid
{
  public:
  IFrameWindow * pW;
  Boolean   fMinned;
    Kid(IFrameWindow * w, Boolean fMin = false) : pW(w), fMinned(fMin) {}
    Kid(Kid const & other) { *this = other;}
    ~Kid(){}
    Kid & operator=(Kid const & other) { if (this != &other)
                                           memcpy(this,&other,sizeof(*this));
                                         return *this;
                                       }
};

class Kids :  public ISequence<Kid>
{
  public:
    void  Hide() { Cursor crsr(*this);
                   forCursor(crsr)
                     elementAt(crsr).pW->hide();
                 }

    void  Close() { Cursor crsr(*this);
                    forCursor(crsr)
                      elementAt(crsr).pW->close();
                  }

    void  Minimize()
           { Cursor crsr(*this);
             forCursor(crsr)
               if ( elementAt(crsr).fMinned = elementAt(crsr).pW->isVisible())
                 elementAt(crsr).pW->hide();
           }

    void  ReShow() { Cursor crsr(*this);
                     forCursor(crsr)
                       if ( elementAt(crsr).fMinned)
                         { elementAt(crsr).pW->show();
                           elementAt(crsr).fMinned = false;
                         }
                   }
};

class XWindow   : public IFrameWindow
                 ,public BaseWnd
                 ,public IFrameHandler
                 ,public WorkWindow
                 ,public ResMessage
                 ,public MyWMHelpHandler
                 ,public MyMinMaxHandler
{
  friend class RMDiscResoW;                            //edw12/29/1999
  public:
    XWindow(unsigned long, IString strAAS, IString strMRD, IString strDSC, MainWindow* pOwner, IHelpWindow *pHW = 0, EUOPProfile * p = NULL);
    ~XWindow();
    void      WWShowDetails(Boolean fMRDB = true)
            , BlankDetails()
            , WWRe_StartTimer(const char * = 0)
            , WWStopTimer(const char * = 0)
            , WWHideTimer()
            , WWShowTimer()
            , WWSetTimerText(const char *)
            , HideTimer()
            , ShowTimer()
            , Re_StartTimer(const char * szText)
            , ShowDetails(Boolean fMRDB = true)
            , changedRPOList()
            , SetResolvedState(Boolean f = true);
     Boolean  printSystem();
     Boolean  convertFormattedPrintFile(char * filename, Boolean fToDisplay);
     Boolean  postEventToWW ( unsigned long           eventId,
                              const IEventParameter1 &parm1 = 0,
                              const IEventParameter2 &parm2 = 0 ) const;
    IString   currentSystem() { return IString(Res.currentSystem());}
    IString   printerQueue();
    void      setHostPID(unsigned long id) { ulHostPID = id;}
    ISortedSet<IString>* pNoCompTable;                 //edw07/01/1998
    ISortedSet<IString>* pQuickFixTable;               //49222 - MAMT - 06/27/2000
    ISortedSet<IString>* pFeature54Table;              //53915 - MAMT - 08/31/2000

    ISortedSet<IString>* pISeriesSWTable;              //F72273  HECC:  06/27/2002

    static Boolean bShowS390Results;                   //edw10/19/1999
    int iUseOldMRDBUserChoice;                         //edw12/29/1999
    int iContMismatchesUserChoice;                     //edw12/29/1999
    int iUseAASMRDBUserChoice;                         //edw12/29/1999
    Boolean bExpan;                                    //ZOR 48548
    Boolean bTil;                                      //47971 - MAMT - 21/08/2000

    IMessageBox::Response
              WWHandleMessage(char * szMsg,
                              unsigned long HelpId = 0,
                              IMessageBox::Style style = IMessageBox::okButton
                                                         |IMessageBox::informationIcon
                                                         |IMessageBox::applicationModal
                                                         |IMessageBox::moveable
                                                         |IMessageBox::systemModal);  //ZOR 060900 add system modal defect 51688
    IMessageBox::Response
              WWHandleErrorMsg(char * szMsg,
                               unsigned long HelpId = 0,
                              IMessageBox::Style style = IMessageBox::okButton
                                                         |IMessageBox::errorIcon
                                                         |IMessageBox::applicationModal
                                                         |IMessageBox::moveable
                                                         |IMessageBox::systemModal); //ZOR 060900 add system modal defect 51688

    unsigned long    WWShowMismatch(char * szB1,char * szB2, char * szTxt);
    unsigned long    WWShowMismatchS390(char * szTxt); //53915 MAMT

    IString          getAASFile()   {return AASFile;} //* F73686  HECC: 10/15/2002
    MainWindow *     getMainWindow() {return pMain;}  //* F73686  HECC: 10/15/2002

  protected:
    Boolean              userMessage(IEvent&)
                ,        WorkWindowMessage(IEvent&)
                ,        command(ICommandEvent& cmdEvent)
                ,        closed(IFrameEvent& evt)
                ,        PressedEsc_F3(IKeyboardEvent &)
                ,        keysHelpId(IEvent&)
                ,        controlSelect(IHelpNotifyEvent&)
                ,        Check_SetupHelp(Boolean fShowNoHelp = true)
                ,        wmhelpMessage(IEvent &)
                ,        Minimizing(ISWP *)
                ,        Restoring (ISWP *)
                ,        systemCommand(ICommandEvent& evt);
  private:
    MainWindow*           pMain;                  // BaseRet main window
    Boolean               bHost;                  // Host dialog is open, edw
    Boolean               bRPOForwarding;         // Forward RPOs edw
    Boolean               bUpLoad;                // UpLoad RPOs/CFR to host edw
    Boolean               bSetLockingFlag;        // Write a locking flag to an O5 Record in CFReport edw
    void                  openHost();             // Open the host connection dialog edw
    void                  openProductInfo();      // Open Product Info dialog edw
    IString               strCFRFile;             // CFR File to be xfered to host edw
    IString               strRPOFile;             // RPO File to be xfered to host edw
    IViewPort             VP;
    IMultiCellCanvas      MCC
                        , MCCE;
    MinSecTimer         * pTimer;
    IStaticText           TextMain
                        , Label1
                        , Label2
                        , Label3
                        , Label4
                        , Label5
                        , Text1
                        , Text2
                        , Text3
                        , Text4
                        , Text5
                        , TextTimer;
    MyExceptionHandler    MyXHndlr;
    Boolean               fFirst
                      ,   fFirstDiscWShowing
                      ,   fFirstRPOWShowing
                      ,   fFirstSWDWShowing
                      ,   fPrintAvailable
                      ,   fViewDisabled
                      ,   fForcePKFileRead
                      ,   fMyProfile
                      ,   fResolved;
    EUOPProfile         * pProfile;
    EUOPResolve           Res;
    unsigned long         HelpIdMB;
    IMessageBox::Response respMB;
    IMessageBox::Style    styleMB;
    IMenuBar              menubar;
    ISystemMenu           sysMenu;
    IString               strMB
                      ,   strExecPath
                      ,   strTimerText
                      ,   strSecurity
                      ,   AASFile
                      ,   ErrTitle
                      ,   MRDFile
                      ,   SaveFile
                      ,   MRDDescFile
                      ,   PrinterQueue
                      ,   PrinterName
                      ,   strReshowPKFile
                      ,   PrinterDriver ;
    EUOPPrint             prt;
    IResourceLibrary     & ResLib;
    unsigned              qtyWrites;
    unsigned              idFDHelpPanel;
    void                  Able(Boolean)
                      ,   CreateExtension(void)
                      ,   CreateClient(void)
                      ,   Center(void)
                      ,   SetInitialState(void);
//  Boolean               GetPrinterInfo();
    Boolean               showWindows();
    void                  BuildWindow();
    void                  SetMenu(int);
    Boolean               WorkWindowPostBack( unsigned long id
                                             ,unsigned long p1 = 0
                                             ,unsigned long p2 = 0);
    Boolean               testPrint();
    Boolean               WakeUp();
    char *                BufK;
    MMWnd               * pMMWnd;
//jt    EUOPAboutDlg   *      pAbtDlg
//jt                  ,*      pAbtDlgT2;
    SWDWndw        *       pSWDW;
    RPOsWndw       *      pRPOW;
//    BkLogWnd       *      pBLW;
    RMBkLogW       *      pBLW;
    PPrvW          *      pPPrvW;
    XPLNWndw       *      pXW;
    RMDiscResoW    *      pDiscW
                  ,*      pResoW;
    EUOPProfileWndw *     pProfW;
    Kids                  kids;
    Boolean               DoRPOForwarding();
    Boolean               UploadCFReport(IString fileName);
    Boolean               GetNewData();
    void                  adjust2Profile();
    Boolean               closeRUSure();
    Boolean               checkAlreadyStarted(Boolean fSwitch = true);
    Boolean               switch2ThisApp();
    void                  focusTheKids();
    void                  ableTheKids(Boolean);
    unsigned long         ulHostPID;
};


#pragma pack()

#endif
