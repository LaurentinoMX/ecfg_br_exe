 /*--------------------------------------------------------------------------*\
 |    Filename - MYTRKHDR.CPP                                                 |
 |      contains source for the following class implementations:              |
 |                                                                            |
 |      MyTrackHandler  - Restrict a frame's sizing capabilities              |
 |                        via the following virtual methods:                  |
 |                           SetMinTrackSize(IPoint&)                         |
 |                           SetMaxTrackSize(IPoint&)                         |
 |                           SetTrackBoundary(IRectangle&)                    |
 |                           SetMaximizedRectangle(IRectangle&)               |
 |                                                                            |
 |      Note : Handler can only be started for sizeable                       |
 |             frame windows via the                                          |
 |             'StartIfSizable' method                                        |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                     PM Includes                     |
        \*---------------------------------------------------*/
//jt extern "C"
//jt  {
   #define INCL_PM
   #include <os2.h>
//jt  }

        /*---------------------------------------------------*\
        |               Class-specific includes               |
        \*---------------------------------------------------*/
#include "mytrkhdr.hpp"                          // this class's include

       /*--------------------------------------------------------------------*\
       |     Boolean StartIfSizable(IFrameWindow *)                           |
       |       Use PM te determine if the given frame window                  |
       |       is sizable and, if so, start handling events                   |
       |       for it.                                                        |
       |     Returns false/true : handling (not) started                      |
       \*--------------------------------------------------------------------*/
Boolean MyTrackHandler :: StartIfSizable(IFrameWindow * pFW)
{
//jt ULONG ul = WinQueryWindowULong(pFW->handle(), QWL_STYLE);
//jt if ((ul & FS_SIZEBORDER) || (ul & FCF_SIZEBORDER))   {
    IHandler::handleEventsFor(pFW);
     return true;
//jt   }
//jt return false;
}

       /*--------------------------------------------------------------------*\
       |      Boolean dispatchHandlerEvent(IEvent&)                           |
       |        Method catches the following messages:                        |
       |                                                                      |
       |        WM_QUERYTRACKINFO:                                            |
       |           After the system sends the WM_TRACKFRAME to the frame      |
       |           as triggered by the user starting to size the window,      |
       |           the frame sends itself the message we're catching          |
       |           here to specify its:                                       |
       |                1. maximum size                                       |
       |                2. minimum sizew                                      |
       |                3. boundary                                           |
       |           via our virtual functions.                                 |
       |           Note: PM is called first to have the frame fill out        |
       |                 the info structure for its purposes since we're      |
       |                 meddling with its message flow.                      |
       |           Returns true : continue sizing or moving                   |
       |                                                                      |
       |       WM_MINMAXFRAME:                                                |
       |                                                                      |
       |          Route the maximizing event to our virtual                   |
       |          method and alter the position structure                     |
       |          if being returned 'true'.                                   |
       |          Returns false : have the system perform the                 |
       |                          actions as per the SWP structure            |
       |                          ( i.e. maximize)                            |
       \*--------------------------------------------------------------------*/
Boolean MyTrackHandler :: dispatchHandlerEvent(IEvent& evt)
{
/*jt
 switch (evt.eventId())
   { case WM_QUERYTRACKINFO:
       { IPoint ipoint;
         IRectangle irect;
         defaultProcedure(evt);
         TRACKINFO *pTI = (TRACKINFO *)(char *)evt.parameter2();
         if (SetMinTrackSize(ipoint))
             pTI->ptlMinTrackSize = ipoint.asPOINTL();
         if (SetMaxTrackSize(ipoint))
             pTI->ptlMaxTrackSize = ipoint.asPOINTL();
         if (SetTrackBoundary(irect))
             pTI->rclBoundary = irect.asRECTL();
         return true;
       }
     case WM_MINMAXFRAME:
       { ISWP * piswp = (ISWP *)(PSWP)(char *)evt.parameter1();
         if (piswp->flags() & SWP_MAXIMIZE)
           { IRectangle irct(piswp->position(),piswp->size());
             if (SetMaximizedRectangle(irct))
               { piswp->setPosition(irct.bottomLeft());
                 piswp->setSize(irct.size());
               }
           }
       }
#if 0
     case WM_MINMAXFRAME:
       { PSWP pswp = (PSWP)(char *)evt.parameter1();
         if (pswp->fl & SWP_MAXIMIZE)
           { IRectangle irct(pswp->x,pswp->y,pswp->x+pswp->cx,pswp->y+pswp->cy);
             if (SetMaximizedRectangle(irct))
               { pswp->x  = irct.left();
                 pswp->y  = irct.bottom();
                 pswp->cx = irct.width();
                 pswp->cy = irct.height();
               }
           }
       }
#endif
   }
jt*/
 return false;
}

        /*---------------------------------------------------*\
        |              Unused non-compiled code               |
        \*---------------------------------------------------*/

#if 0
class MyMinimumTrackSizeHandler : public IHandler
{
  protected:
    virtual Boolean SetMinimumTrackSize(POINTL * pptl) { return false;}
  public:
      MyMinimumTrackSizeHandler ( ) {;}
      Boolean dispatchHandlerEvent(IEvent& evt)
           { if (evt.eventId() == WM_QUERYTRACKINFO)
               { POINTL ptl;
                 if (SetMinimumTrackSize(&ptl))
                   { defaultProcedure(evt);
                     TRACKINFO *pTI = (TRACKINFO *)(char *)evt.parameter2();
                     pTI->ptlMinTrackSize = ptl;
                     return true;
                   }
               }
             return false;
           }
}

Boolean MyTrackHandler :: dispatchHandlerEvent(IEvent& evt)
{
 if (evt.eventId() == WM_QUERYTRACKINFO)
   { IPoint ipoint;
     IRectangle irect;
     defaultProcedure(evt);
     TRACKINFO *pTI = (TRACKINFO *)(char *)evt.parameter2();
     if (SetMinTrackSize(ipoint))
         pTI->ptlMinTrackSize = ipoint.asPOINTL();
     if (SetMaxTrackSize(ipoint))
         pTI->ptlMaxTrackSize = ipoint.asPOINTL();
     if (SetTrackBoundary(irect))
         pTI->rclBoundary = irect.asRECTL();
     return true;
   }
 if (evt.eventId() == WM_MINMAXFRAME)
    WinAlarm(HWND_DESKTOP, WA_NOTE);
 return false;
}

#endif









