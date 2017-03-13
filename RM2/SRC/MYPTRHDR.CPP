 /*--------------------------------------------------------------------------*\
 |    Filename - MYPTRHDR.CPP                                                 |
 |      contains source for the following class implementations:              |
 |                                                                            |
 |      MyHitTestHandler - WM_HITTEST messages dispatcher.                    |
 |                                                                            |
 |      MyPointerHandler - Shows SPTR_WAIT or the given pointer               |
 |                         over the handled disabled window,                  |
 |                         optionally activating the top                      |
 |                         level-one in the application.                      |
 |                                                                            |
 |      Based on the PM fact that for EVERY pointer                           |
 |            related event, a window gets this message to find               |
 |            out if the message is deliverable (e.g. whether                 |
 |            a disabled window can process pointing device events)           |
 |                                                                            |
 |      Only for PM window classes with the CS_HITTEST style set.             |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                     PM Includes                     |
        \*---------------------------------------------------*/
//JT extern "C"
//JT  {
   #define INCL_DOSRESOURCES
   #define INCL_PM
   #include <os2.h>
//JT  }

        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/
#include <iwindow.hpp>
#include <ireslib.hpp>
#include <iexcbase.hpp>

        /*---------------------------------------------------*\
        |               EUOP-specific includes                |
        \*---------------------------------------------------*/
#include "myptrhdr.hpp"                          // this class's include


 /*--------------------------------------------------------------------------*\
 |    MyHitTestHandler Class                                                  |
 |      Ctor, Dtor and methods in-lined in MYPTRHDR.HPP                       |
 \*--------------------------------------------------------------------------*/

       /*--------------------------------------------------------------------*\
       |    Boolean dispatchHandlerEvent(IEvent&)                             |
       |      Method to catch and route WM_HITTEST messages.                  |
       |      Returns: false/true : (un)successful                            |
       \*--------------------------------------------------------------------*/

Boolean MyHitTestHandler :: dispatchHandlerEvent(IEvent& evt)
{ /*JT return ((evt.eventId() == WM_HITTEST)? testHit(evt): false); */}


 /*--------------------------------------------------------------------------*\
 |    MyPointerHandler Class                                                  |
 |      Dtor and methods in-lined in MYPTRHDR.HPP                             |
 \*--------------------------------------------------------------------------*/

       /*--------------------------------------------------------------------*\
       |    Ctor                                                              |
       |      Optional parameters:                                            |
       |          ptr      : pointer of your choice                           |
       |                     default( ISystemPointerHandle::wait )            |
       |          f        : Allow button1-click to activate                  |
       |                     the top leval window of the application          |
       |                     when disabled                                    |
       \*--------------------------------------------------------------------*/

MyPointerHandler:: MyPointerHandler( const IPointerHandle& ptr,Boolean f)
                 : MyHitTestHandler()
                 , hndlBusyPtr(ptr)
                 , peeking(false)
                 , But1Activate(f)
{}

       /*--------------------------------------------------------------------*\
       |    Boolean testHit(IEvent& evt)                                      |
       |       Method, called by 'MyHitTestHandler' to handle                 |
       |       the pointer for our disabled windows,                          |
       |       optionally activating the top level-one in our app             |
       |       as follows:                                                    |
       |                                                                      |
       |                  Ensure we're only handling diabled windows and      |
       |                  that we're not doing what we're doing:              |
       |                                                                      |
       |                    If we're set to activate on button 1,             |
       |                      activate the top level window in our app,       |
       |                      while raspberry-ing other mouse clicks.         |
       |                    Set our busy pointer, but be a nice guy,          |
       |                      and don't mess with somebody else's             |
       |                      captured pointer.                               |
       |                    Remind ourself that we stopped doing what         |
       |                    we're doing and throw the message away.           |
       |                                                                      |
       |                " For the message created as result of the peeking:   |
       |                  I had been returning HT_NORMAL which worked fine    |
       |                  except when over a disabled child control, in       |
       |                  which the mouse appearance would flicker and button |
       |                  clicks would case panel activation.                 |
       |                  -10 is not an HT_ value and this currently solves   |
       |                  the problem ( I guess that returning something      |
       |                  other that an HT_ value means not to do any of      |
       |                  the HT_* functions. "                               |
       |                  11-91: I == Craig A. Swearingen  (SEARING at AUSVM1)
       \*--------------------------------------------------------------------*/

Boolean MyPointerHandler :: testHit(IEvent& evt)
{
/*JT if (evt.window())
   {
     HWND hwnd = evt.window()->handle();
     if (WinIsWindowEnabled(hwnd)) return false;
     if (!peeking)
       { QMSG qmsg;
                                                     // WinPeekMsg'll create a
         peeking = true;                             // WM_HITTEST
         if (WinPeekMsg(hwnd, &qmsg, NULL, 0, 0, PM_NOREMOVE))
           switch(qmsg.msg)
             { case WM_BUTTON1DOWN:
                if (But1Activate)
                  WinSetActiveWindow(HWND_DESKTOP,
                                     (HWND)WinSendMsg(hwnd,WM_QUERYFOCUSCHAIN,
                                   MPFROMSHORT(QFC_SELECTACTIVE),MPFROMHWND(hwnd)));
               case WM_BUTTON2DOWN:
               case WM_BUTTON3DOWN:
                WinAlarm(HWND_DESKTOP, WA_NOTE);
             }
         if (!WinQueryCapture(HWND_DESKTOP))
           WinSetPointer(HWND_DESKTOP, hndlBusyPtr);
         peeking = false;
         evt.setResult(MRFROMSHORT(HT_DISCARD));
       }
     else evt.setResult(MRFROMSHORT(-10));
   }
jt*/ return true;
}


       /*--------------------------------------------------------------------*\
       |    Boolean SetPointer(IPointerHandle)                                |
       |    Boolean SetPointer(unsigned long)                                 |
       |      Methods to set the pointer that will be shown over              |
       |      the handled disabled window.                                    |
       |    Returns: false/true: (un)successful                               |
       \*--------------------------------------------------------------------*/
Boolean MyPointerHandler :: SetPointer(IPointerHandle hndl)
{
 if (hndl != IPointerHandle())
   { hndlBusyPtr = hndl;
     return true;
   }
 return false;
}
Boolean MyPointerHandler :: SetPointer(unsigned long Id)
{
 IPointerHandle ptrhndl;
 try { ptrhndl=IApplication::current().userResourceLibrary().loadPointer(Id); }
 catch (IException& exc)
     { return false; }
 hndlBusyPtr = ptrhndl;
 return true;
}

       /*--------------------------------------------------------------------*\
       |    unsigned TrySetPointer(unsigned long)                             |
       |      Method to set the pointer that will be shown over               |
       |      the handled disabled window ensuring the resource               |
       |      can be loaded without throwing an exception.                    |
       |   Returns: unsigned : DosGetResource's rc:                           |
       |                 0 : OK                                               |
       |                 6 : invalid resource module                          |
       |                87 : invalid parm (unknown id)                        |
       \*--------------------------------------------------------------------*/

unsigned MyPointerHandler :: TrySetPointer(unsigned long Id)
{
/*JT PVOID pOffset;
 APIRET rc=DosGetResource(IApplication::current().userResourceLibrary().handle()
                          ,1,Id,&pOffset);
 if (!rc)
   this->SetPointer(Id);
 return (unsigned)rc; jt*/
}

