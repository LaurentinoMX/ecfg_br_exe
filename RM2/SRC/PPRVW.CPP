//jt extern "C"
//jt   {
    #define INCL_PM
    #define INCL_DOSNLS
    #define INCL_WIN
    #define INCL_WINSHELLDATA
    #include <os2.h>

    #include <stdio.h>
    #include <stdlib.h>
//jt    }

#define INCL_IAPPLICATION
#define INCL_ICOLOR
#define INCL_IFONT
#define INCL_IMENUEVENT
#define INCL_IPOPUPMENU
#define INCL_ISETCANVAS
#define INCL_ITITLE
#include <ifiledlg.hpp>                     //jt
#include "uicl.hpp"

#include "prf.hpp"
#include "pprvw.hpp"
#include "euophlp.h"
#include "pprvw.h"
#include "mainw.h"
#include "euoutil.hpp"
#include "prf.hpp"
#include "ialctls.hpp"

#include "euopids.h"

#define  PPRVW_MIN_NMBR_LINES  4
#define  PPRVW_UM_SETFOCUS   WM_USER + 1

//JT FILEDLG               FD;

PPrvCtl::PPrvCtl( unsigned long ulId
                , IWindow * Parent
                , IWindow * Owner
                , unsigned width
                , unsigned lines
                , IString FontName
                , unsigned points
                )
  : IMultiCellCanvas(ulId,Parent,Owner)
    ,pParent(Parent)
    ,pOwner(Owner)
    ,gb(PPRFW_ID_GB,this,this)
    ,MLE( PPRFW_ID_MLE,this,this, IRectangle(),
                  IMultiLineEdit::border
                | IMultiLineEdit::verticalScroll
                | IMultiLineEdit::horizontalScroll
                | IMultiLineEdit::readOnly
                | IMultiLineEdit::ignoreTab
//              | IEntryField::autoScroll
                | IControl::tabStop
                | IWindow::visible
         )

{

//jt memset(&FD,0,sizeof(FILEDLG));
//jt FD.cbSize = sizeof(FILEDLG);
//jt FD.fl = FDS_CENTER  | FDS_SAVEAS_DIALOG | FDS_HELPBUTTON;
// MLE.setColor(IMultiLineEdit::background,IColor(IColor::cyan));
 gb.setText("XXXXXXXX");
 IFont font(FontName.length()? (char *)FontName : "System Monospaced",points);
 MLE.setFont(font);
 long lH = font.maxSize().height()
     ,lW = font.maxSize().width();
 lines = ALMAX(lines,PPRVW_MIN_NMBR_LINES);
 lH *= lines;
 //jt lH += WinQuerySysValue(HWND_DESKTOP,SV_CYHSCROLL);
 lH = ALMIN(IWindow::desktopWindow()->size().height() * 0.85,lH);
 lW *= width;
 lW = ALMIN(IWindow::desktopWindow()->size().width() * 0.85,lW);
// MLE.setMinimumSize(ISize(lW,lH));
 ArrangeControls();
 setMinimumSize(ISize(lW + (IMultiCellCanvas::defaultCell().width() * 4),
                      lH + (IMultiCellCanvas::defaultCell().height() * 6) ));

}

void PPrvCtl :: ArrangeControls()
{
 addToCell(&gb     ,2, 2, 3, 4);
 addToCell(&MLE   ,3, 4      );
 setColumnWidth(3,0,true);
 setRowHeight(4,0,true);
 setRowHeight(6,IMultiCellCanvas::defaultCell().height());
 setColumnWidth(5,IMultiCellCanvas::defaultCell().width()/2);
}

void PPrvCtl :: Fill(char *Buffer)
{
 MLE.disableUpdate()
    .removeAll().setText(Buffer).setTop(0)
    .enableUpdate();
}

Boolean PPrvCtl :: import(char * FileName)
{
 MLE.disableUpdate().removeAll();
 Boolean fOK = MLE.importFromFile(FileName);
 MLE.setTop(0).enableUpdate();
 return fOK;
}

///////////////////////////////////////////////////////

PPrvW :: PPrvW( unsigned long ulId
         ,char * B
         ,IWindow *  Parent
         ,IHelpWindow * pHW
         ,EUOPProfile *pProf
         ,unsigned long ClosingMsg
         ,Boolean  fPrintAvailable
         ,unsigned width
         ,unsigned lines
         ,IString font
         ,unsigned points
        )
                : IFrameWindow( IFrameWindow::titleBar
                                | IFrameWindow::systemMenu
                                | IFrameWindow::sizingBorder
//                              | IFrameWindow::maximizeButton
//                              | IFrameWindow::hideButton
                               ,ulId )
                 ,BaseWnd(this,Parent,ClosingMsg,pHW)
                 ,rReslib(IApplication::current().userResourceLibrary())
                 ,MLE(PPRFW_ID_MLE_CNTL,this,this,width,lines,font,points)
                 ,MCCE(PPRVW_ID_MCCE,this,this)
                 ,TextTimer(PPRVW_ID_TEXT_TIMER,&MCCE,&MCCE)
                 ,fCanPrint(fPrintAvailable)
                 ,MyXHndlr(this,this)
                 ,pProfile(pProf)
                 ,pParent(Parent)
{
 setOwner(Parent);
 setClient(&MLE);
 if (fMyProfile = (pProfile == NULL )) {          // get profile if not given
   pProfile = new EUOPProfile(true);
   pProfile->SetResources(IApplication::current());
   pProfile->ReadProfile();
   }
 createExtension();
 if (!pHW)
   pPBHelp->disable();
 if (B) Fill(B);
 ISize isize(MLE.minimumSize().width(),
             MLE.minimumSize().height() + MCCE.minimumSize().height());
 isize.setWidth(ALMIN(IWindow::desktopWindow()->size().width() * 0.9,isize.width()));
 isize.setHeight(ALMIN(IWindow::desktopWindow()->size().height() * 0.8,isize.height()));
// sizeTo(frameRectFor(IRectangle(IPoint(50,50),isize)).size());
 sizeTo(frameRectFor(IRectangle(IPoint(),isize)).size());
 ICommandHandler::handleEventsFor(&MLE.mle());
 IMenuHandler::handleEventsFor(&MLE);
 IMenuHandler::handleEventsFor(&MLE.mle());
 MyWMHelpHandler::handleEventsFor(this);
}

PPrvW :: ~PPrvW()
{
 //edw pTimer->StopCounting();
 if (fMyProfile && pProfile)  delete pProfile;
}

void PPrvW :: createExtension(void)
{
 pPBOK = new IPushButton(DID_OK,&MCCE,&MCCE);
 pPBOK->setAutoDeleteObject();
 pPBOK->enableDefault().setText(IResourceId(ID_STR_ACC_OK))
       .enableTabStop().enableGroup();
 pPBHelp = new IPushButton(PPRVW_ID_PB_HELP,&MCCE,&MCCE);
 pPBHelp->setAutoDeleteObject();
 pPBHelp->disableMouseClickFocus().setText(IResourceId(ID_STR_HELP));

//jt pTimer = new MinSecTimer(PPRVW_ID_TIMER,&MCCE,&MCCE, IRectangle(),
//jt                          MinSecTimer::defaultStyle() & ~MinSecTimer::visible);
//jt pTimer->setAutoDeleteObject();
 TextTimer.setText("Formatting and spooling");
 TextTimer.setLimit(12).setAlignment(IStaticText::centerLeft).hide();

 MCCE.addToCell(pPBOK  ,2, 2 )
     .addToCell(pPBHelp,4, 2 )
//jt     .addToCell(pTimer, 6, 2 )
     .addToCell(&TextTimer,8, 2 );
 MCCE.setColumnWidth(1,IMultiCellCanvas::defaultCell().width())
     .setColumnWidth(3,IMultiCellCanvas::defaultCell().width())
     .setColumnWidth(5,IMultiCellCanvas::defaultCell().width(),true)
     .setColumnWidth(7,IMultiCellCanvas::defaultCell().width())
     .setColumnWidth(9,IMultiCellCanvas::defaultCell().width())
//     .setRowHeight(1,IMultiCellCanvas::defaultCell().height())
     .setRowHeight(1,3)
     .setRowHeight(3,2);
 MCCE.show();
 addExtension(&MCCE,belowClient,(unsigned long)MCCE.minimumSize().height());
}

void PPrvW :: hideTimer()
{
//jt pTimer->StopCounting();
//jt pTimer->hide();
 TextTimer.hide();
}

void PPrvW :: showTimer()
{
//edw pTimer->Reset();
//edw pTimer->show();
 TextTimer.show();
//edw pTimer->StartCountingUp();
}

IWindow & PPrvW :: setFocus()
{
 postEvent(PPRVW_UM_SETFOCUS);
 return *this;
}

unsigned long PPrvW :: showModally()
{
 PPrvW::setFocus();
 return IFrameWindow::showModally();
}

void  PPrvW :: Fill(char * Buf)
{
  MLE.Fill(Buf);
  ImportFile = IString();
}

Boolean PPrvW :: import(char * FileName)
{
 Boolean fOK = MLE.import(FileName);
 if (fOK) {
   ImportFile = IString(FileName);
   MLE.setGroupBoxText((char *)((XWindow *)pParent)->currentSystem());
   }
 else ImportFile = IString();
 return fOK;
}


Boolean PPrvW :: makePopUpMenu(IMenuEvent& menuEvent)
{
 IPopUpMenu * popUp = new IPopUpMenu(PPRVW_POPUP, menuEvent.window());
//  popUp->setFont(IFont(&MCCControls));
 if (!MLE.mle().hasSelectedText()) {
   popUp->disableItem(PPRVW_ID_POP_COPY);
   popUp->disableItem(PPRVW_ID_POP_EXPORT_SEL);
   }
 if (!helpWindow())
   popUp->disableItem(PPRVW_ID_POP_HELP);
 if (!fCanPrint || (ImportFile == IString()))
   popUp->disableItem(PPRVW_ID_POP_PRINT);
 popUp->setAutoDeleteObject();
              /*-----------------------------------------------*\
              | The message might have been generated as a      |
              | result of a mouseclick or a keystroke           |
              | combination, in which case the mouse might be   |
              | outside the container and it would              |
              | inappropriate to pop the menu at the mouse      |
              | position. In stead, the bottom-left position of |
              | the container is used. Reminder: the mouse      |
              | position and the menu position are in relation  |
              | to the Container!                               |
              \*-----------------------------------------------*/
 IPoint pt(menuEvent.mousePosition());
 IRectangle rct(MLE.rect());
 if (pt.x() >= 0 && pt.x() <= rct.width() &&
     pt.y() >= 0 && pt.y() <= rct.height())
   popUp->show(pt);
 else  popUp->show(IPoint());
 return true;
}


Boolean  PPrvW::wmhelpMessage(IEvent & evt)
{
 if (evt.parameter1().asUnsignedLong() == DID_HELP_PB) {
   helpWindow()->show(IResourceId(IDH_PNL_FD_EXPORT));
   return true;
   }
 return false;
}

Boolean PPrvW :: PressedEnter(IKeyboardEvent & evt)
{
 if ( evt.window() == pPBOK     ||
      evt.window() == pPBHelp ) {
   evt.handle().sendEvent(BM_CLICK);
   return true;
   }
 MessageBeep(MB_ICONEXCLAMATION);
 return false;
}

Boolean PPrvW :: userMessage(IEvent& evt)
{
 switch(evt.eventId()) {
   case PPRVW_UM_SETFOCUS:
     MLE.setFocus();
     return true;
   case MSG_TO_WW_FSPOOL:
     if ((unsigned long)evt.parameter1())
       MyXHndlr.HandleMsgResponse(
                (char *)getString(MAINW_MSG_SPOOL_JOB, (char *)ImportFile,
                                  (char *)((XWindow *)pParent)->printerQueue())
               ,(char *)(ITitle(this).objectText()));
     Able();
     setFocus();
     hideTimer();
     return true;
   }
 return false;
}

Boolean PPrvW :: command(ICommandEvent & evt)
{
 switch (evt.commandId()) {
   case DID_OK:
   case PPRVW_ID_POP_CANCEL:
     NotifyMyClosing(DID_CANCEL);
     return true;
   case PPRVW_ID_POP_COPY:
     MLE.mle().copy();
     return true;
   case PPRVW_ID_POP_EXPORT_SEL:
   case PPRVW_ID_POP_EXPORT: {
//jt     IString title(IApplication::current().userResourceLibrary().loadString( WND_PPRVW ));
//jt     FD.pszTitle = (PSZ)(char *)title;
//jt     WinFileDlg(HWND_DESKTOP,handle(),&FD);
//jt     MakeFileSpec(FD.szFullFile,
//jt                  (char *)pProfile->GetStringKey(EUOPProfile::WorkDir),
//jt                  (char *)((XWindow *)pParent)->currentSystem(),"EXP");
     IString strExecPath = EUOPProfile::strExecPath;

     IString str = MakeFileSpecStr( (char *)strExecPath,
                      (char *)((XWindow *)pParent)->currentSystem(), "EXP");

     IFileDialog::Settings settings;
     //settings.setDialogTemplate( IDD_DirectoryDialog );
     settings.setSaveAsDialog();
     settings.setTitle( LOADSTRING( WND_PPRVW));
     settings.setFileName( str);
     IFileDialog::Style fileStyle = IFileDialog::defaultStyle() |
                                    IFileDialog::helpButton;

     IFileDialog fd((IWindow*)desktopWindow(), (IWindow*)this, settings, fileStyle);

     if (fd.pressedOK())
       if (evt.commandId() == PPRVW_ID_POP_EXPORT)
         MLE.mle().exportToFile( str); //jt FD.szFullFile);
       else MLE.mle().exportSelectedTextToFile( str); //jt FD.szFullFile );
     MessageBeep(MB_ICONEXCLAMATION);
     return true;
     }

   case PPRVW_ID_PB_HELP:
   case PPRVW_ID_POP_HELP:
     helpWindow()->show(IHelpWindow::general);
     break;
   case PPRVW_ID_POP_PRINT:
     showTimer();
     ((XWindow *)pParent)->postEventToWW( (unsigned long)MSG_TO_WW_FSPOOL
                                         ,(unsigned long)(char *)ImportFile
                                         ,(unsigned long)this);
     Able(false);
     return true;
   }
 return false;
}

        /*---------------------------------------------------*\
        |                     Test Driver                     |
        \*---------------------------------------------------*/
#ifdef DRIVER
//Boolean LoadResources(ICurrentApplication &,EUOPProfile *);
int main(int argc, char **argv)
{
 IApplication::current().setArgs(argc, argv);
 EUOPProfile * pProfile = new EUOPProfile();
 if (!(pProfile->SetResources(IApplication::current())))
   exit(3) ;
 PPrvW mainWindow (WND_PPRVW,0,IWindow::desktopWindow());
 mainWindow.import("D:\\CONFIG.SYS");
 mainWindow.Center();
 mainWindow.show();
 IApplication::current().run();
}
#endif

        /*---------------------------------------------------*\
        |             Unused - non compiled code              |
        \*---------------------------------------------------*/
#if 0
// no exception is thrown when help file isn't there
Boolean XPLNWndw :: SetupHelp()
{
 if (fMyProfile = (pProfile==NULL))
    pProfile = new EUOPProfile();
 IString hlpName = pProfile->CurrentHelpFile().size()?
                     pProfile->CurrentHelpFile() : pProfile->DefaultHelpFile();
 try { HLPWndw.addLibraries((char *)hlpName);
        HLPWndw.setTitle(XPLNWND_STR_HELPTITLE);
        IHelpHandler::handleEventsFor(this);
     }
 catch (IException& exc)
     { MyXHndlr.HandleException(exc);
       return false;
     }
 return true;
}
#endif
