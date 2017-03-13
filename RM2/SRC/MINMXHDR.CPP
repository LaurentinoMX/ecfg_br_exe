#define INCL_WIN
#include <os2.h>

#include "minmxhdr.hpp"


Boolean MyMinMaxHandler :: dispatchHandlerEvent(IEvent& evt)
{
/*jt if (evt.eventId() == WM_MINMAXFRAME)
    { ISWP * piswp = (ISWP *)(PSWP)(char *)evt.parameter1();
      return (piswp->flags() & SWP_MAXIMIZE)? Maximizing(piswp) :
                   ((piswp->flags() & SWP_MINIMIZE)?
                         Minimizing(piswp) : Restoring(piswp));
    }
*/return false;
}

Boolean MyDlgMinMaxHandler :: Maximizing(ISWP * piswp)
{
/*jt  IWindow wnd(piswp->windowHandle());
 IWindow::ChildCursor crsr(wnd);
 for (crsr.setToFirst(); crsr.isValid(); crsr.setToNext())
   WinShowWindow(wnd.childAt(crsr),true);
jt*/ return false;
}

Boolean MyDlgMinMaxHandler :: Minimizing(ISWP * piswp)
{
/*jt  IWindow wnd(piswp->windowHandle());
 IWindow::ChildCursor crsr(wnd);
 for (crsr.setToFirst(); crsr.isValid(); crsr.setToNext())
   WinShowWindow(wnd.childAt(crsr),false);
jt*/ return false;
}


