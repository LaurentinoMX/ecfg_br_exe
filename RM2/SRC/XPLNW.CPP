 /*--------------------------------------------------------------------------*\
 |    Filename - XPLNW.CPP                                                    |
 |      contains source for the following EUOP class implementation:          |
 |      XPLNWndw         - Explanation Window                                 |
 |                                                                            |
 |      Window has the following features or peculiarities:                   |
 |         Minimum Size  - The window can NOT be sized smaller by the         |
 |                         user than the width occupied by the                |
 |                         pushbuttons and a height required to show the      |
 |                         number of lines given as a parameter               |
 |                         (defaulting to 10 with a minimum of 3).            |
 |                                                                            |
 |         Data Reentrant- The same instance of the window can be given       |
 |                         different Explanation without having to re-create  |
 |                         it. For multiple appearances of Lists, the window  |
 |                         has been designed to be 'shown' and 'hid' and      |
 |                         given data via the 'FillMLE' method.               |
 |                                                                            |
 |         Initial State - Invisible                                          |
 |                         To be positioned by the caller.                    |
 |                         The initial and minimum height of the              |
 |                            window is dependend on the                      |
 |                            'NumberofLines' parmeter which defaults         |
 |                            to 10 while anything less than 3 is ignored.    |
 |                                                                            |
 |         Interface     - There is only one response to the window:          |
 |                           DID_CANCEL - User canceled                       |
 |                                                                            |
 |                           If the window was instantiated with a            |
 |                           non-zero 'ClosingMsg' parameter:                 |
 |                             That message will be sent with the             |
 |                             aforementioned response as                     |
 |                             'parameter1'                                   |
 |                                                                            |
 |                           If the window was instantiated modally,          |
 |                           it will me dismissed with the reponse.           |
 |                                                                            |
 |                           If no 'ClosingMsg' was given and the             |
 |                           window was not shown modally, it will            |
 |                           itself.                                          |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |          PM and Standard Library includes           |
        \*---------------------------------------------------*/
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

        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/
#define INCL_IAPPLICATION
#define INCL_IFONT
#define INCL_ISTRING
#include "uicl.hpp"

        /*---------------------------------------------------*\
        |               EUOP-specific includes                |
        \*---------------------------------------------------*/
#include "xpln2l.hpp"                           // XPLNList
#include "prf.hpp"                               // EUOPProfile
#include "xplnw.h"                               // this window's Ids
#include "xplnw.hpp"                             // this window's include
#include "euoutil.hpp"                           // utilities

#include "euopids.h"

        /*---------------------------------------------------*\
        |                        Ctor                         |
        \*---------------------------------------------------*/
XPLNWndw :: XPLNWndw( unsigned long ulId
                     ,XPLN2List * pList
                     ,IHelpWindow *pHW
                     ,IWindow *  Parent
                     ,unsigned long ClosingMsg
                     ,long NumberOfLines
                    )
                : IFrameWindow( IFrameWindow::titleBar
                                | IFrameWindow::systemMenu
                                | IFrameWindow::sizingBorder
                                | IFrameWindow::maximizeButton
                                | IFrameWindow::hideButton
//                              | IFrameWindow::windowList
                               ,ulId
                              )
                 ,BaseWnd(this,Parent,ClosingMsg,pHW)
                 ,rReslib(IApplication::current().userResourceLibrary())
                 ,XPLNCntl(XPLNWND_XPLNCNTL,this,this)
                 ,pXPLNList(pList)
{
  setClient(&XPLNCntl);
  CreatePBs();                                   // create pushbuttons
                   /*------------------------------------------*\
                   | Use the Explain Control Font's character   |
                   | height, and the number of requested lines  |
                   | to set its minimum size.                   |
                   \*------------------------------------------*/
  if (!pHW)                                 // disable Help Button
     pPBHelp->disable();                         // if there's none
  long lLineHeight = IFont(&XPLNCntl).maxSize().height();
//jt  NumberOfLines =
//jt        ALMIN(ALMAX(NumberOfLines,XPLNWND_MIN_NMBR_LINES)
//jt              ,(WinQuerySysValue(HWND_DESKTOP,SV_CYSCREEN) * 0.7)/lLineHeight);
                                                 // allow for the GroupBox
    NumberOfLines = 10; //jt-test
//  XPLNCntl.setMinimumSize( ISize( pSCPBs->minimumSize().width()
//                                 ,lLineHeight * (NumberOfLines + 3) - 6 ));
  XPLNCntl.setMinimumSize( ISize( pSCPBs->minimumSize().width()
                                 ,lLineHeight * NumberOfLines ));

  XPLNCntl.FillMLE(pXPLNList);                   // fill the control
//sizeTo(minimumSize());
  sizeTo(ISize((IWindow::desktopWindow()->size().width()*5)/10,
               (IWindow::desktopWindow()->size().height()*5)/10));

  MyTrackHandler::StartIfSizable(this);          // enforce minimum size
}

          /*-------------------------------------------------*\
          |      Dtor - free obtained storage resources       |
          \*-------------------------------------------------*/
XPLNWndw :: ~XPLNWndw()
{
}

          /*-----------------------------------------------------------------*\
          |   IWindow & setFocus()                                            |
          |       Method to override the inherited meothod to                 |
          |       set the focus to the Explain Control.                       |
          \*-----------------------------------------------------------------*/
IWindow & XPLNWndw :: setFocus()
{
 postEvent(XPLNWND_UM_SETFOCUS);
 return *this;
}

          /*-----------------------------------------------------------------*\
          |   unsigned long  showModally()                                    |
          |       Method to override the inherited method to                  |
          |       set the focus to the ListBox.                               |
          |       Returns: the inherited method's return value.               |
          \*-----------------------------------------------------------------*/
unsigned long XPLNWndw :: showModally()
{
 XPLNWndw::setFocus();
 return IFrameWindow::showModally();
}

          /*-----------------------------------------------------------------*\
          |    ISize calcMinimumSize() const                                  |
          |       Method, called by 'IFrameWindow', returns the minimum       |
          |       size for the window.                                        |
          |       Method is overridden since IFrameWindow's method            |
          |       does not poperly take into account the Info Area and        |
          |       PushButton Extension.                                       |
          |       Returns: ISize : minimum size.                              |
          \*-----------------------------------------------------------------*/
ISize XPLNWndw :: calcMinimumSize() const
{
/*jt  return ISize( pSCPBs->minimumSize().width()
               + WinQuerySysValue(HWND_DESKTOP,SV_CXSIZEBORDER) * 2
              ,XPLNCntl.minimumSize().height()
               + pSCPBs->minimumSize().height()
               + WinQuerySysValue(HWND_DESKTOP,SV_CYSIZEBORDER) * 3
               + WinQuerySysValue(HWND_DESKTOP,SV_CYTITLEBAR)
             );  jt*/
  return ISize( pSCPBs->minimumSize().width() + 10,
                XPLNCntl.minimumSize().height()
               + pSCPBs->minimumSize().height()+ 10);
}

          /*-----------------------------------------------------------------*\
          |    Boolean  SetMinTrackSize(IPoint & ipoint)                      |
          |       Method called by 'MyTrackHandler' returning                 |
          |       the previously established minimum size.                    |
          |       Returns: true to indicate 'message handled'                 |
          \*-----------------------------------------------------------------*/
Boolean XPLNWndw :: SetMinTrackSize(IPoint & ipoint)
{
 ipoint = minimumSize();
 return true;
}

          /*-----------------------------------------------------------------*\
          |    void CreatePBs()                                               |
          |       Method builds the Pushbutton extension:                     |
          |         1. Create the pushbuttons and add them to a SetCanvas     |
          |         2. Calculate and set the canvas' minimum size             |
          |         3. Add the extension and make it visible                  |
          |       Return: nothing                                             |
          \*-----------------------------------------------------------------*/
void XPLNWndw :: CreatePBs()
{
  unsigned short usPBCount = 2;
  pSCPBs = new ISetCanvas(XPLNWND_PB_SCV,this, this);
  pSCPBs->setAutoDeleteObject();
  pSCPBs->setDeckCount(usPBCount).setDeckOrientation(ISetCanvas::vertical);
  pSCPBs->setPackType(ISetCanvas::expanded).
          setAlignment(ISetCanvas::bottomCenter);

  pPBCancel =  new IPushButton(DID_CANCEL,pSCPBs,pSCPBs, IRectangle(),
                               IPushButton::classDefaultStyle |
                               IPushButton::defaultButton);
  pPBCancel->setAutoDeleteObject();
  pPBCancel->setText(IResourceId(ID_STR_ACC_OK)).enableGroup().enableTabStop();

  pPBHelp = new IPushButton(XPLNWND_PB_HELP,pSCPBs,pSCPBs);
  pPBHelp->setAutoDeleteObject();
//pPBHelp = new IPushButton(XPLNWND_PB_HELP,pSCPBs,pSCPBs, IRectangle(),
//                          IPushButton::classDefaultStyle |
//                          IButton::noPointerFocus | IPushButton::help);
  pPBHelp->setText(IResourceId(ID_STR_HELP));

  pSCPBs->setMinimumSize( ISize(pPBCancel->minimumSize().width() * (usPBCount+1)
                                 + pSCPBs->pad().width() * (usPBCount-1)
                                 + pSCPBs->margin().width() * 2
                               , pPBCancel->minimumSize().height()
                                 + pSCPBs->margin().width() * 2) );

  addExtension(pSCPBs,belowClient,(unsigned long)pSCPBs->minimumSize().height());
  pSCPBs->show();
}

          /*-----------------------------------------------------------------*\
          |    void  Fill(XPLNList *pList)                                    |
          |       Fill the Explain Control with the contents of the           |
          |       given List of Expalanations.                                |
          |       Returns: nothing                                            |
          \*-----------------------------------------------------------------*/
void  XPLNWndw :: Fill(XPLN2List *pList)
{
 XPLNCntl.FillMLE(pList);
}

          /*-----------------------------------------------------------------*\
          |    void  Fill(XPLNList *pList)                                    |
          |       Append the currently displayed Explanation List             |
          |       with the given-one.                                         |
          |       Returns: nothing                                            |
          \*-----------------------------------------------------------------*/
void  XPLNWndw :: Add2(XPLN2List *pList)
{
 XPLNCntl.FillMLE(pList,false);
}

Boolean XPLNWndw :: PressedEnter(IKeyboardEvent & evt)
{
 if ( evt.window() == pPBCancel    ||
      evt.window() == pPBHelp ) {
   evt.handle().sendEvent(BM_CLICK);
   return true;
   }
 return false;
}

          /*-----------------------------------------------------------------*\
          |    Boolean command(ICommandEvent & evt)                           |
          |       Method, called by ICommandHandler, processes                |
          |       the following cases:                                        |
          |                                                                   |
          |          DID_CANCEL:                                              |
          |                ( the Cancel PushButton)                           |
          |             Report the user's intend to our parent.               |
          |                                                                   |
          |          RPOSWND_PB_HELP:                                         |
          |                ( the Help PushButton)                             |
          |             Tell the Help Window to show our Help Panel.          |
          |                                                                   |
          |       Returns: false/true : message (not)/handled                 |
          \*-----------------------------------------------------------------*/
Boolean XPLNWndw :: command(ICommandEvent & cmdEvent)
{
 switch (cmdEvent.commandId()) {
   case DID_CANCEL:
     NotifyMyClosing(DID_CANCEL);
     return true;
   case XPLNWND_PB_HELP:
     helpWindow()->show(IHelpWindow::general);
     return true;
   }
 return false;
}

          /*-----------------------------------------------------------------*\
          |    Boolean  userMessage(IEvent& evt)                              |
          |       Method, called by IUserHandler, processes                   |
          |       the following User Messages ( those > WM_USER):             |
          |                                                                   |
          |          XPLNWND_UM_SETFOCUS:                                     |
          |             Set the focus to the Explain Control.                 |
          |                                                                   |
          |       Returns: false/true : message (not)/handled                 |
          \*-----------------------------------------------------------------*/
Boolean XPLNWndw :: userMessage(IEvent& evt)
{
 switch(evt.eventId()) {
   case XPLNWND_UM_SETFOCUS:
     XPLNCntl.setFocus();
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
 XPLN2List *pXList = new XPLN2List;
 pXList->add( XPLN2("Mach1","Mod1","Ser1","Feat1","This is the text for the longer passed-in Explanation 234567890"));
 pXList->add( XPLN2("Mach2","Mod2","Ser2","Feat2","This is the text for the passed-in Explanation 12345"));
 pXList->add( XPLN2("Mach3","Mod3","Ser3","Feat3","This is like really long winded passed-in text. It talks about this and that and all that stuff. it just won't shut up. You know what I'm saying?"));
 XPLNWndw mainWindow (WND_XPLN,pXList,pProfile,NULL,0,100);
 XPLN2List *pXList2 = new XPLN2List;
 pXList2->add( XPLN2("Mach1","Mod1","Ser1","Feat4","Here's an addition."));
 mainWindow.Add2(pXList2);
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
