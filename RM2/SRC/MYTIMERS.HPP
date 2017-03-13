#ifndef _AB_MYTIMERS_HPP_
#define _AB_MYTIMERS_HPP_
 /*--------------------------------------------------------------------------*\
 |    File Name  : MYTIMERS.HPP                                               |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |      MyTimers  - Start, stop and maintain info on PM Timers                |
 |                  for a given window.                                       |
 |                  Timer messages are routed with their IDs                  |
 |                  to the virtual 'timerTick' method.                        |
 |                  A dynamic array of reusable structures is                 |
 |                  used to retain timer data.                                |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/
#include <ihandler.hpp>

                                                 // common (test) IDs not used 
enum _IDTimers                                   // in the class
  {
    PRFLWND_TIMER1            =  1
   ,PRGRSSDLG_TIMER_HALFSEC
   ,PRGRSSWND_TIMER_HALFSEC
   ,PRGRSSWND_TIMER_DISMISS
   ,TIMERCNTL_SEC
   ,TIMERWND_TIMER_HALFSEC
   ,TIMERWND_TIMER_SEC
   ,TIMERWND_TIMER_DISMISS
   ,TIMER_TEMP
   ,TIMER_TEMP2
   ,TIMER_TEMP3
   ,TIMER_TEMP4
   ,TIMER_TEMP5
   ,TIMER_TEMP6
   ,TIMER_TEMP7
   ,TIMER_TEMP8
   ,TIMER_TEMP9
  };

typedef  struct _Timer { long     lId;
                         long     lTime;
                       }Timer;

class MyTimers : public IHandler
{
  IWindow           * pWindow;
  Timer             * pTimers
                   ,* pTemp
                   ,* getTimer()
                   ,* findTimer(long)
                   ,* getTimer(long)
                   ,* getNTimers(unsigned short);
  unsigned short      usCount;
  int                 i;
  Boolean             stopTimer(Timer *)
                   ,  startTimer(Timer *,long,long)
                   ,  myTimer(long lId);
  IAnchorBlockHandle  hab;
  public:
    MyTimers(IWindow *,unsigned short usCount = 2);
   ~MyTimers();
    Boolean           dispatchHandlerEvent(IEvent&)
                   ,  start(long Id,long TimeOut)
                   ,  stop(long Id);
    Boolean           timing(long Id) {return myTimer(Id);}
    long              time(long Id);
    void              stopAll();
  protected:
    virtual Boolean   timerTick(unsigned long Id) { return false;}
} ;
#endif
