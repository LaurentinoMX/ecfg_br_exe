 /*--------------------------------------------------------------------------*\
 |    Filename - MYTINERS.CPP                                                 |
 |      contains source for the following class implementation:               |
 |      MyTimers  - Start, stop and maintain info on PM Timers                |
 |                  for a given window.                                       |
 |                  Timer messages are routed with their IDs                  |
 |                  to the virtual 'timerTick' method.                        |
 |                  A dynamic array of reusable structures is                 |
 |                  used to retain timer data.                                |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |          PM and Standard Library includes           |
        \*---------------------------------------------------*/
//jt extern "C"
//jt  {
    #define INCL_WINTIMER
    #include <os2.h>

    #include <string.h>
//jt  }

        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/
#include <ithread.hpp>
#include <iwindow.hpp>

        /*---------------------------------------------------*\
        |               EUOP-specific includes                |
        \*---------------------------------------------------*/
#include "mytimers.hpp"

        /*---------------------------------------------------*\
        |                        Ctor                         |
        \*---------------------------------------------------*/
MyTimers :: MyTimers(IWindow *pW, unsigned short Count)
          : IHandler()
           ,pWindow(pW)
           ,usCount(Count)
{
 hab = IThread::current().anchorBlock();
 pTimers = getNTimers(usCount);
 if (pWindow && pWindow->isValid())
   handleEventsFor(pWindow);
}

        /*---------------------------------------------------*\
        |                        Dtor                         |
        \*---------------------------------------------------*/
MyTimers ::  ~MyTimers()
{
 stopAll();
 IHandler::stopHandlingEventsFor(pWindow);
 delete pTimers;
}

        /*-------------------------------------------------------------------*\
        |     Timer * getNTimers(unsigned short)                              |
        |        Get an array of a given number of empty timer                |
        \*-------------------------------------------------------------------*/
Timer *  MyTimers :: getNTimers(unsigned short us)
{
 pTemp = new Timer[us];
 memset(pTemp,0,usCount * sizeof(*pTemp));
 return pTemp;
}

        /*-------------------------------------------------------------------*\
        |     Timer * getTimer()                                              |
        |        Get an available Timer structure, reallocating               |
        |        the array if needed.                                         |
        |        Available timers are those with no Id.                       |
        |        Returns: Timer's address                                     |
        \*-------------------------------------------------------------------*/
Timer *  MyTimers :: getTimer()
{
 for (i = 0; i < usCount && pTimers[i].lId; i++);
 if (i == usCount) {
   pTemp = getNTimers(++usCount);
   memcpy(pTemp,pTimers,i * sizeof(*pTemp));
   delete pTimers;
   pTimers = pTemp;
   }
 return (pTimers+i);
}

        /*-------------------------------------------------------------------*\
        |     Boolean myTimer(long)                                           |
        |        Indicates whether we ahve a Timer with the                   |
        |        given Id.                                                    |
        |        Returns: true/false                                          |
        \*-------------------------------------------------------------------*/
Boolean MyTimers :: myTimer(long lId)
{
 for (i = 0; i < usCount && pTimers[i].lId != lId; i++);
 return (i < usCount);
}

        /*-------------------------------------------------------------------*\
        |     Timer * findTimer(long)                                         |
        |        Returns the Timer with the given Id or                       |
        |        an empty-one, in that order                                  |
        |        Returns: Timer * : null/pointer : (un)successful             |
        \*-------------------------------------------------------------------*/
Timer *  MyTimers :: getTimer(long lId)
{
 if (!(pTemp = findTimer(lId)))
   pTemp = getTimer();
 return pTemp;
}

        /*-------------------------------------------------------------------*\
        |     Timer * findTimer(long)                                         |
        |        Returns the address of the Timer with the given Id.          |
        |        Returns: Timer * : null/pointer : (un)successful             |
        \*-------------------------------------------------------------------*/
Timer *  MyTimers :: findTimer(long lId)
{
 for (i = 0; i < usCount && pTimers[i].lId != lId; i++);
 return (i < usCount)? pTimers+i : 0;
}

        /*-------------------------------------------------------------------*\
        |     Timer * stop(long)                                              |
        |        Stop the Timer with the given Id                             |
        |        Returns: false/true: (un)successful                          |
        \*-------------------------------------------------------------------*/
Boolean MyTimers :: stop(long lId)
{ return stopTimer(findTimer(lId)); }

        /*-------------------------------------------------------------------*\
        |     Boolean stopTimer(Timer *)                                      |
        |        Stop the given Timer ensuring it's timing                    |
        |        and mark it available.                                       |
        |        Tiny bug: Ensure the window's handle is still                |
        |                  valid: the window might have been closed           |
        |                  already. IBMClass is barely throwing               |
        |                  exceptions in 2.1 and blows in DDEMUI              |
        |                  in 2.0.                                            |
        |        Returns: false/true: (un)successful                          |
        \*-------------------------------------------------------------------*/
Boolean MyTimers :: stopTimer(Timer *pTMR)
{
 if (pTMR && pTMR->lId) {
   if (pWindow && pWindow->handle().isValid())
     //jt WinStopTimer(hab,pWindow->handle(),pTMR->lId);
     KillTimer(pWindow->handle(),pTMR->lId);
   pTMR->lId = pTMR->lTime = 0;
   return true;
   }
 return false;
}

        /*-------------------------------------------------------------------*\
        |     Boolean startTimer(Timer *pTMR,long lId, long lTimeOut)         |
        |        Start the given Timer with given Id for the given            |
        |        Timeout ( restart it if it's the same Id)                    |
        |        and mark it unavailable.                                     |
        |        Returns: false/true: (not)started                            |
        \*-------------------------------------------------------------------*/
Boolean MyTimers :: startTimer(Timer *pTMR,long lId, long lTimeOut)
{
 if (pTMR && lId) {
   if (pTMR->lId == lId)
     stopTimer(pTMR);
   if ( (pTMR->lTime = lTimeOut) && (pWindow && pWindow->handle().isValid())) {
     //jt WinStartTimer(hab,pWindow->handle(),pTMR->lId = lId,lTimeOut);
     SetTimer(pWindow->handle(),pTMR->lId = lId,lTimeOut, NULL);
     return true;
     }
   }
 return false;
}

        /*-------------------------------------------------------------------*\
        |     Boolean start(long lId, long lTimeOut)                          |
        |        Start a Timer with given Id for the given                    |
        |        Timeout ( restart it if it's already timing).                |
        |        Returns: false/true: (not)started                            |
        \*-------------------------------------------------------------------*/
Boolean MyTimers :: start(long lId,long lTimeOut)
{
 if (lTimeOut && lId) {
   if (!(pTemp = getTimer(lId)))
     pTemp = getTimer();
   return startTimer(pTemp,lId,lTimeOut);
   }
 return false;
}

        /*-------------------------------------------------------------------*\
        |     long time(long lId)                                             |
        |        Return the TimeOut period for the                            |
        |        Timer with the given Id.                                     |
        \*-------------------------------------------------------------------*/
long MyTimers :: time(long lId)
{ return ((pTemp = getTimer(lId))? pTemp->lTime : 0); }

        /*-------------------------------------------------------------------*\
        |     void stopAll()                                                  |
        |        Stop all timers.                                             |
        |        returns: nothing                                             |
        \*-------------------------------------------------------------------*/
void MyTimers :: stopAll()
{
 for (i = 0; i < usCount; i++)
   stopTimer(pTimers+i);
}

        /*-------------------------------------------------------------------*\
        |     Boolean dispatchHandlerEvent(IEvent& evt)                       |
        |        Catch the WM_TIMER messages and if generated                 |
        |        by one of our timers, route them to our virtual              |
        |        method with the Id.                                          |
        |        returns: false/true - message handled                        |
        \*-------------------------------------------------------------------*/
Boolean MyTimers::dispatchHandlerEvent(IEvent& evt)
{
 return ((evt.eventId() == WM_TIMER) && myTimer(evt.parameter1()))?
         timerTick(evt.parameter1().asUnsignedLong()) : false;
}

