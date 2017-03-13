 /*--------------------------------------------------------------------------*\
 |    Filename - MYFCSHDR.CPP                                                 |
 |      contains source for the following EUOP class implementations:         |
 |                                                                            |
 |      MyFocusHandler  -  Catches WM_FOCUSCHANGE messages and                |
 |                         makes them available via its virtual               |
 |                         methods:                                           |
 |                           gettingFocus(IEvent&)                            |
 |                           losingFocus(IEvent&)                             |
 |                         Note: attach to individual controls.               |
 |                                                                            |
 |      MyWindowFocusHandler - Based on 'MyFocusHandler':                     |
 |                             hides 'gettingFocus', replacing it             |
 |                             with                                           |
 |                                   LostFocus(IEvent&)                       |
 |                             which is only called when a control            |
 |                             loses focus which is not gained by any         |
 |                             other controls it handles.                     |
 |                                                                            |
 |      MyEFSetSelectHandler - Sets the contents of the handled               |
 |                             EntryField to 'selected' when the              |
 |                             control gains focus.                           |
 |                         Note: attachable only to IEntryFields.             |
 |                                                                            |
 |      MyActiveAppHandler   -  Maintains the active status of an             |
 |                              application, routing a change of              |
 |                              that status to the virtual methods:           |
 |                                 gainingActivation()                        |
 |                                 losingActivation()                         |
 |                         Notes:  attachable only to IFrameWindows           |
 |                                 (preferably the top level window           |
 |                                  since the messaging travels up the        |
 |                                  owner chain)                              |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |          PM and Standard Libarary Includes          |
        \*---------------------------------------------------*/
//jt extern "C"
//jt   {
   #define INCL_PM
   #include <os2.h>

   #include <string.h>
//jt   }

        /*---------------------------------------------------*\
        |               EUOP-specific includes                |
        \*---------------------------------------------------*/
#include "myfcshdr.hpp"                          // this class' include

  /*-------------------------------------------------------------------------*\
  |    MyFocusHandler  Class                                                  |
  |      Ctor, Dtor and additional methods in-lined in  MYFCSHDR.HPP          |
  \*-------------------------------------------------------------------------*/

      /*---------------------------------------------------------------------*\
      |     Boolean dispatchHandlerEvent(IEvent&)                             |
      |       Method overloads that of IHandler and directs                   |
      |           focus messages to the available methods.                    |
      |       Returns false/true : message (not)handled -                     |
      |                            always false for our methods               |
      \*---------------------------------------------------------------------*/

Boolean MyFocusHandler :: dispatchHandlerEvent(IEvent& evt)
{
//jt if (evt.eventId() == WM_FOCUSCHANGE)
 if (evt.eventId() == WM_SETFOCUS)
   return ((evt.parameter2().number1())? gettingFocus(evt) : losingFocus(evt));
 return false;
}

  /*-------------------------------------------------------------------------*\
  |    MyEFSetSelectHandler Class                                             |
  |      Ctor and  Dtor in-lined in in MYFCSHDR.HPP                           |
  \*-------------------------------------------------------------------------*/

          /*-----------------------------------------------------------------*\
          |    IHandler & handleEventsFor(IEntryField *)                      |
          |    IHandler & stopHandlingEventsFor(IEntryField * )               |
          |       Methods to limit this class to IEntryField usage only       |
          \*-----------------------------------------------------------------*/
IHandler & MyEFSetSelectHandler :: handleEventsFor(IEntryField * pEF)
{ return MyFocusHandler::handleEventsFor(pEF);}

IHandler & MyEFSetSelectHandler :: stopHandlingEventsFor(IEntryField * pEF)
{ return MyFocusHandler::stopHandlingEventsFor(pEF);}

          /*-----------------------------------------------------------------*\
          |    Boolean gettingFocus(IEvent&)                                  |
          |       Method, called by MyFocusHandler, sets the                  |
          |       EntryField's content to 'selected'                          |
          |       Returns : false - message not handled                       |
          |                 (let the system process the focusing              |
          \*-----------------------------------------------------------------*/
Boolean MyEFSetSelectHandler :: gettingFocus(IEvent& evt)
{
 ((IEntryField *)evt.window())->selectRange();
 return false;
}


  /*-------------------------------------------------------------------------*\
  |                        MyWindowFocusHandler Class                         |
  \*-------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                        Ctor                         |
        \*---------------------------------------------------*/
MyWindowFocusHandler :: MyWindowFocusHandler(unsigned Count)
                      : MyFocusHandler()
                       ,uCount(Count)
{
 ppWindows = GetNWindowPtrs(uCount);
}

        /*---------------------------------------------------*\
        |                        Dtor                         |
        \*---------------------------------------------------*/
MyWindowFocusHandler ::  ~MyWindowFocusHandler()
{
//StopAll();
 delete ppWindows;
}

        /*-------------------------------------------------------------------*\
        |     IWindow ** GetNWindowPtrs(unsigned u)                           |
        |        Return pointer to an array of a given number                 |
        |        of empty Window pointers or null if unsuccessful             |
        \*-------------------------------------------------------------------*/
IWindow ** MyWindowFocusHandler :: GetNWindowPtrs(unsigned u)
{
 if (ppTemp = (IWindow **)(new char[u * sizeof(*ppTemp)]))
    memset(ppTemp,0,u * sizeof(*ppTemp));
 return ppTemp;
}

        /*-------------------------------------------------------------------*\
        |     Boolean PtrArrayIndex(unsigned *)                               |
        |        Store the array index of an available Window                 |
        |        pointer at the given address, reallocating                   |
        |        if needed. (Available slots are empty.)                      |
        \*-------------------------------------------------------------------*/
Boolean MyWindowFocusHandler :: PtrArrayIndex(unsigned * pu)
{
 unsigned u;
 for (u = 0; u < uCount && ppWindows[u]; u++);
 if (u == uCount)
   { uCount += ARRAY_INCREMENT;
     if (ppTemp = GetNWindowPtrs(uCount))
        { memcpy(ppTemp,ppWindows,u * sizeof(*ppTemp));
          delete ppWindows;
          ppWindows = ppTemp;
        }
     else return false;
   }
 *pu = u;
 return true;
}

        /*-------------------------------------------------------------------*\
        |     IWindow * WindowPtr(IWindowHandle ihwnd)                        |
        |        Return the pointer to the Window with the given              |
        |        handle or null if not found.                                 |
        \*-------------------------------------------------------------------*/
IWindow * MyWindowFocusHandler :: WindowPtr(IWindowHandle ihwnd)
{
 unsigned u;
 for (u = 0 ; u < uCount ; u++)
   if (ppWindows[u] && (ppWindows[u])->handle() == ihwnd)
      break;
 return (u < uCount)? ppWindows[u] : 0;
}

        /*-------------------------------------------------------------------*\
        |     Boolean WindowIndex(IWindowHandle ihwnd,unsigned *)             |
        |        Store the array index in the given address                   |
        |        for the Window having the given handle.                      |
        |        Returns: false/true (un)successul                            |
        \*-------------------------------------------------------------------*/
Boolean MyWindowFocusHandler :: WindowIndex(IWindowHandle ihwnd,unsigned *pu)
{
 unsigned u;
 for (u = 0 ; u < uCount ;u++)
   if (ppWindows[u] && (ppWindows[u])->handle() == ihwnd)
      break;
 if (u < uCount) { *pu = u;
                   return true;
                 }
 return false;
}

          /*-----------------------------------------------------------------*\
          |    Boolean Add(IWindow *)                                         |
          |       Add the given pointer to the array if not already there.    |
          |    Returns: false/true : (un)sucessful                            |
          \*-----------------------------------------------------------------*/
Boolean MyWindowFocusHandler :: Add(IWindow * pW)
{
 unsigned u;
 if (pW && !WindowPtr(pW->handle()) && PtrArrayIndex(&u))
    { ppWindows[u] = pW;
      return true;
    }
 return false;
}

          /*-----------------------------------------------------------------*\
          |    Boolean Remove(IWindow *)                                      |
          |       Remove the given pointer from the array.                    |
          |    Returns: false/true : (un)sucessful                            |
          \*-----------------------------------------------------------------*/
Boolean MyWindowFocusHandler :: Remove(IWindow * pW)
{
 unsigned u;
 if (pW && pW->handle().isValid() && WindowIndex(pW->handle(),&u))
    { ppWindows[u] = 0;
      return true;
    }
 return false;
}

          /*-----------------------------------------------------------------*\
          |    IHandler & handleEventsFor(IWindow *)                          |
          |    IHandler & stopHandlingEventsFor(IWindow * )                   |
          |       Methods to wrap the IHandler's methods                      |
          \*-----------------------------------------------------------------*/
IHandler & MyWindowFocusHandler :: handleEventsFor(IWindow * pW)
{
 if (Add(pW))
   return MyFocusHandler::handleEventsFor(pW);
 return *this;
}

IHandler & MyWindowFocusHandler :: stopHandlingEventsFor(IWindow * pW)
{

 if (Remove(pW))
   return MyFocusHandler::stopHandlingEventsFor(pW);
 return *this;

}

          /*-----------------------------------------------------------------*\
          |    void StopAll()                                                 |
          |      Stop the Handler for all Windows in the array.               |
          \*-----------------------------------------------------------------*/
void MyWindowFocusHandler :: StopAll()
{
 unsigned u;
 for (u = 0 ; u < uCount ; u++)
   if (ppWindows[u])
    { IHandler::stopHandlingEventsFor(ppWindows[u]);
      ppWindows[u] = 0;
    }
//     this->stopHandlingEventsFor(ppWindows[u]);
}

          /*-----------------------------------------------------------------*\
          |    Boolean losingFocus(IEvent& evt)                               |
          |      Method called by 'MyFocusHandler':                           |
          |        Only call our virtual method if a control lost             |
          |        focus and none of the other controls or their kids         |
          |        registered with us, gained it.                             |
          |                                                                   |
          |     Note: handling the control's kids was added to                |
          |           accomodate composite controls such as                   |
          |           spinbuttons and combo boxes since we're                 |
          |           also receiving focus messages for their kids.           |
          \*-----------------------------------------------------------------*/

Boolean MyWindowFocusHandler :: losingFocus(IEvent& evt)
{
//jt return !Ours(IWindowHandle(evt.parameter1().asUnsignedLong()))?
//jt          LostFocus(evt) : false;
}


        /*-------------------------------------------------------------------*\
        |     Boolean Ours(IWindowHandle ihwnd)                               |
        |        Indicate whether one of the windows in the array             |
        |        either has the given handle or is the parent of a            |
        |        child who does.                                              |
        \*-------------------------------------------------------------------*/
Boolean MyWindowFocusHandler :: Ours(IWindowHandle ihwnd)
{
 unsigned u;
 for (u = 0 ; u < uCount ; u++)
   if ( ppWindows[u] &&
       (((ppWindows[u])->handle() == ihwnd) || WindowsKid(ppWindows[u],ihwnd)))
      break;
 return (u < uCount);
}

        /*-------------------------------------------------------------------*\
        |     Boolean WindowsKid(IWindow *,IWindowHandle)                     |
        |        Indicate whether the given handle is that of                 |
        |        a child of the given window.                                 |
        \*-------------------------------------------------------------------*/
Boolean MyWindowFocusHandler :: WindowsKid(IWindow * pW,IWindowHandle ihwnd)
{
 if (pW) { IWindow::ChildCursor crsr(*pW);
           for (crsr.setToFirst(); crsr.isValid() && pW->childAt(crsr) != ihwnd;
                crsr.setToNext());
           return (crsr.isValid());
         }
 return false;
}




  /*-------------------------------------------------------------------------*\
  |                         MyActiveAppHandler Class                          |
  \*-------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                        Ctor                         |
        \*---------------------------------------------------*/
MyActiveAppHandler :: MyActiveAppHandler()
                    : IHandler()
                     ,fActive(true)
{}

          /*-----------------------------------------------------------------*\
          |    IHandler & handleEventsFor(IFrameWindow *)                     |
          |    IHandler & stopHandlingEventsFor(IFrameWindow * )              |
          |       Methods to wrap the IHandler's methods for frames           |
          \*-----------------------------------------------------------------*/
IHandler & MyActiveAppHandler :: handleEventsFor(IFrameWindow * pFW)
{ return IHandler::handleEventsFor(pFW);}
IHandler & MyActiveAppHandler :: stopHandlingEventsFor(IFrameWindow * pFW)
{ return IHandler::stopHandlingEventsFor(pFW);}

      /*---------------------------------------------------------------------*\
      |     Boolean dispatchHandlerEvent(IEvent&)                             |
      |       Method overloads that of IHandler and uses                      |
      |         the WM_FOCUSCHANGE message to                                 |
      |          1. retain the activity status                                |
      |          2. Call its virtual methods                                  |
      |              gainingActivation()                                      |
      |              losingActivation()                                       |
      |        depending on whether the activation involves                   |
      |        another PM queue.                                              |
      |       Returns false/true : message (not)handled -                     |
      |                            false will keep message from               |
      |                            traveling up the owner chain)              |
      \*---------------------------------------------------------------------*/

Boolean MyActiveAppHandler :: dispatchHandlerEvent(IEvent& evt)
{
//jt if ((evt.eventId() == WM_FOCUSCHANGE)                           &&
//jt     (WinQueryWindowULong((HWND)evt.handle(),QWL_HMQ) !=
//jt      WinQueryWindowULong((HWND)(evt.parameter1().asUnsignedLong()),QWL_HMQ)))
//jt    return (fActive = evt.parameter2().number1())? gainingActivation()
//jt                                                 : losingActivation();
 return false;
}

//#endif

