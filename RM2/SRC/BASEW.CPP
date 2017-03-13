 /*--------------------------------------------------------------------------*\
 |    Filename - BASEPWND.CPP                                                 |
 |      contains source for the following EUOP class implementations:         |
 |      BasePopWnd    - A depository of data and methods common to the        |
 |                      small Pop-up Windows.                                 |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |          PM and Standard Library includes           |
        \*---------------------------------------------------*/
//JT extern "C"
//JT  {
   #define INCL_PM
   #include <os2.h>

   #include <stdio.h>
//JT  }

        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/

#define INCL_ITITLE
#define INCL_ISYSTEMMENU
extern IString strHLP;         // data variable pass to RM/2 from af9main  jt,7/3/97

#include "uicl.hpp"
        /*---------------------------------------------------*\
        |               EUOP-specific includes                |
        \*---------------------------------------------------*/
#include "basew.hpp"                          // this class' include

BaseWnd :: BaseWnd( IFrameWindow * frame
                   ,IWindow * ClosingMsgOwner
                   ,unsigned long ulClosingMsg
                   ,IHelpWindow *pHW
                  )
               : Me(frame)
                ,MsgOwner(ClosingMsgOwner)
                ,ClosingMsg(ulClosingMsg)
                ,ptrHdr(ISystemPointerHandle(ISystemPointerHandle::arrow))
{
   IHelpWindow::setDefaultStyle( IHelpWindow::defaultStyle()
                               | IHelpWindow::ipfCompatible ); //JT

 ICommandHandler::handleEventsFor(Me);
 IKeyboardHandler::handleEventsFor(Me);
 MyUserMsgHandler::handleEventsFor(Me);
 ptrHdr.handleEventsFor(Me);
 ptrHdr.disable();
 SetupHelp(pHW);
}

Boolean BaseWnd :: disableNow()
{
//jt if (!WinQueryCapture(HWND_DESKTOP)) {
//jt   WinSetPointer(HWND_DESKTOP, ptrHdr.pointerHandle());
   Able(false);
   return true;
//   }
// return false;
}

void BaseWnd :: Able(Boolean fEn)
{
//jtest return; //jt
//jtest ptrHdr.enable(fEn? false:true);
 Me->enable(fEn);
}

void BaseWnd :: SetTitle(const char * txt)
{
 if (txt) ITitle(Me).setObjectText(txt);
}

          /*-----------------------------------------------------------------*\
          |    void PlaceWnd(XPlacement,YPlacement)                           |
          |       Method positions the window in anyone of the 9              |
          |       locations made possible by the incoming X and Y positions   |
          |       (if the parent's size exceeds that of our own)              |
          |                                                                   |
          |       If the window is wider than the parent, the latter's        |
          |       X position is used, while if the parent has vertical        |
          |       shortcomings, the window is centered on its parent.         |
          |       Returns: nothing                                            |
          \*-----------------------------------------------------------------*/
void BaseWnd :: PlaceWnd(XPlacement xPos, YPlacement yPos, IWindow * pParent)
{
 if (!pParent)
   pParent = IWindow::desktopWindow();
 ISize mysize(Me->size()),parentsize;
 IRectangle rectl;
 rectl = (pParent->isFrameWindow()? MyClientRect((IFrameWindow *)pParent)
                                  : pParent->rect());
 parentsize = rectl.size();
 if (!parentsize.width() || !parentsize.height())
   parentsize = IWindow::desktopWindow()->size();
 IPoint ipoint(rectl.bottomLeft());
 if (parentsize.width() > mysize.width())        // only if we fit
    ipoint.setX(ipoint.x() + (xPos==left ? 0 :
                              xPos == xcenter ?
                                (parentsize.width()-mysize.width())/2 :
                                 parentsize.width()-mysize.width()));
 if (parentsize.height() > mysize.height())      // only if we fit
    ipoint.setY(ipoint.y() + (yPos==bottom ? 0 :
                              yPos == ycenter ?
                                (parentsize.height()-mysize.height())/2 :
                                 parentsize.height()-mysize.height()));
 else ipoint.setY(ALMAX(0,ipoint.y() - mysize.height() + parentsize.height()));
 Me->moveTo(ipoint);
}

void BaseWnd :: Center()
{
 Me->moveTo(Me->rect().centeredAt(IWindow::desktopWindow()->rect().center()).bottomLeft());
}

void BaseWnd :: NotifyMyClosing(unsigned long parm1,unsigned long parm2)
{
// WinAlarm(HWND_DESKTOP,WA_ERROR);
 if (pHLPWndw && pHLPWndw->isVisible())
   pHLPWndw->hide();
 Me->removeFromWindowList();                             //edw 12/9/97
 if (ClosingMsg)
   if (MsgOwner)
      MsgOwner->handle().sendEvent(ClosingMsg,parm1,parm2);
   else Me->owner()->handle().sendEvent(ClosingMsg,parm1,parm2);
 if (Me->isModal())
    Me->dismiss(parm1);
 else if (!ClosingMsg)
          Me->close();
}

void BaseWnd :: AbleHelpHandler(Boolean fEn)
{
 if (pHLPWndw)
   hlpHandler.enable(fEn);
}

void BaseWnd :: SetupHelp(IHelpWindow * pHW)
{
 if ( !pHW) {
   IHelpWindow::setDefaultStyle( IHelpWindow::defaultStyle()
                               | IHelpWindow::ipfCompatible ); // JT
   pHLPWndw = new IHelpWindow(HELPTABLE_AFOAPPLY, Me);
   //pHLPWndw->addLibraries(strHLP+STR_ExtensionHelp);//jt, strHLP defined in bridge.h
   pHLPWndw->addLibraries( IString("\"")+strHLP+STR_ExtensionHelp+IString("\"") ); //edwtest
   pHLPWndw->addLibraries("hmhelp.hlp");
   }
 else {
   pHLPWndw = pHW;
   pHLPWndw->setAssociatedWindow(Me);
   }
}

Boolean BaseWnd :: systemCommand(ICommandEvent& cmdEvt)
   {

   // Hide this window instead of closing it.
   //jt switch(cmdevt.parameter1()) {
     //case SC_HIDE:
     //case SC_CLOSE:
       //NotifyMyClosing(DID_CANCEL);
       //return true;
     //}
   if ( cmdEvt.commandId() == ISystemMenu::idClose )
      {
      NotifyMyClosing(DID_CANCEL);
      return true;
      }
   return false;
   }


Boolean BaseWnd :: PressedEsc_F3(IKeyboardEvent & evt)
{
 NotifyMyClosing(DID_CANCEL);
 return true;
}

Boolean BaseWnd :: virtualKeyPress(IKeyboardEvent & evt)
{
 switch(evt.virtualKey()) {
   case IKeyboardEvent::enter:
   case IKeyboardEvent::newLine:
     return PressedEnter(evt);
   case IKeyboardEvent::esc:
   case IKeyboardEvent::f3:
     return PressedEsc_F3(evt);
   case IKeyboardEvent::tab:
     return PressedTab(evt);
   }
 return false;
}

//jt void BaseWnd :: AbleSystemClose(Boolean fEn)
//{ AbleSystemMenuItem(SC_CLOSE,fEn); }


void BaseWnd :: AbleSystemMenuItem(unsigned long Id,Boolean fEn)
{
 ISystemMenu systemMenu(Me);
 (fEn ? systemMenu.enableItem(Id) : systemMenu.disableItem(Id));
}
