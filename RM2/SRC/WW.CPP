//jt extern "C"
//jt  {
   #define INCL_PM
   #include <os2.h>
   #include <stdio.h>
//jt  }

#include "ww.hpp"

Boolean IWorkUserHandler:: dispatchHandlerEvent(IEvent& evt)
{ return (evt.eventId() >= WM_USER)? WorkUserMessage(evt) : false; }


WorkWindow::WorkWindow()
           :fBusy(true)
           ,WorkThread(0)
           ,pOW(0)
{}

Boolean WorkWindow::StartUp(IWindowHandle hwnd,unsigned long ReadyMessage)
{
 ihwndPa = hwnd;
 ReadyMsg = ReadyMessage;
 if (!WorkThread) {
   IThreadMemberFn<WorkWindow> *workFn =
          new IThreadMemberFn<WorkWindow>(*this,WorkWindow::StartWorkThread);
  if (workFn && (WorkThread = new IThread())) {
    WorkThread->setStackSize(65536);
    WorkThread->start(workFn);
    return true;
    }
  }
 return false;
}

WorkWindow::~WorkWindow()
{
// WinAlarm(HWND_DESKTOP, WA_NOTE);
// WorkThread->stopProcessingMsgs();
// delete pOW;
// IThread::current().waitFor(*WorkThread);
 if (pOW) delete pOW;
 if (WorkThread) delete WorkThread;
}

// Don't call this-one
// Doesn't always work. When called from the main window, it sometimes
// hangs, like there's a deadlock.
// Changed the scheme by posting a WM_QUIT to the object window and
// deleting the latter in the desctructor
void WorkWindow::CloseDown()
{
// WinAlarm(HWND_DESKTOP, WA_NOTE);

 WorkThread->stopProcessingMsgs();
 delete pOW;
 pOW = 0;
 WorkThread->stop();

// IThread::current().waitFor(*WorkThread);
// WorkThread = 0;
}

void WorkWindow :: StartWorkThread(void)
{
 if (!pOW) {
   pOW = new IObjectWindow();
 // object windows are tagged as primary windows by ICLUI which also does
 // not quit the message queue (IApplication.current().run()) until
 // all primary windows are closed.
 // setting the parent supposedly removes this 'primary tag'
// pOW->setParent(IWindow::objectWindow());
   IWorkUserHandler::handleEventsFor((IWindow *)pOW);
   pOW->handle().postEvent(WW_UM_READY);
   IThread::current().processMsgs();
   }
}

Boolean WorkWindow :: WorkUserMessage(IEvent& evt)
{
 Boolean fHandled = true;
 fBusy = true;
 switch(evt.eventId()) {
   case WW_UM_READY: break;
   default: fHandled = WorkWindowMessage(evt);
   }
 fBusy = false;
 ihwndPa.postEvent(ReadyMsg); //always tell parent we're available
  //jt WinAlarm(HWND_DESKTOP, WA_WARNING);
 MessageBeep(MB_ICONEXCLAMATION);  //jt
 return fHandled;
}


IEventResult WorkWindow:: sendEvent ( unsigned long eventId,
                                      const IEventParameter1 &parm1,
                                      const IEventParameter2 &parm2) const
{
 return fBusy? IEventResult(0) : pOW->sendEvent(eventId,parm1,parm2);
}

IEventResult WorkWindow:: sendEvent ( const IEvent& eventId) const
{
 return fBusy? IEventResult(0) : pOW->sendEvent(eventId);
}

Boolean WorkWindow:: postEvent ( unsigned long eventId,
                                 const IEventParameter1 &parm1,
                                 const IEventParameter2 &parm2) const
{
 if (!fBusy)
   pOW->postEvent(eventId,parm1,parm2);
 return !fBusy;
}

Boolean WorkWindow:: postEvent ( const IEvent& eventId) const
{
 if (!fBusy)
   pOW->postEvent(eventId);
 return !fBusy;
}

#if 0
IEventResult WorkWindow:: sendEvent ( EventType  eventType,
                                      const IEventParameter1 &parm1,
                                      const IEventParameter2 &parm2) const
{
 return fBusy? IEventResult(0) : pOW->sendEvent(eventType,parm1,parm2);
}
#endif

