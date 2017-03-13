#include "ialctls.hpp"
#include "alctls.h"
#include "istattxt.hpp"

CHAR szFontFace[] = "8.Helv";

const ProgressBase::Style ProgressBase::no_percentage   = ALCTLS_NOPERCENT      ;
const ProgressBase::Style ProgressBase::vertical        = ALCTLS_VERTICAL       ;
const ProgressBase::Style ProgressBase::border_furrow   = ALCTLS_BORDER_FURROW  ;
const ProgressBase::Style ProgressBase::border_inni     = ALCTLS_BORDER_INNI    ;
const ProgressBase::Style ProgressBase::border_outi     = ALCTLS_BORDER_OUTI    ;
const ProgressBase::Style ProgressBase::border_standard = ALCTLS_BORDER_STANDARD;
const ProgressBase::Style ProgressBase::border_system   = ALCTLS_BORDER_SYSTEM;
const ProgressBase::Style ProgressBase::visible         = WS_VISIBLE;

ProgressBase::Style ProgressBase::currentDefaultStyle = ProgressBase::border_inni
                                                        | ProgressBase::visible;

const Counter::Style Counter::digits1         = ALCTLS_1_DIGIT;
const Counter::Style Counter::digits2         = ALCTLS_2_DIGIT;
const Counter::Style Counter::digits3         = ALCTLS_3_DIGIT;
const Counter::Style Counter::digits4         = ALCTLS_4_DIGIT;
const Counter::Style Counter::digits5         = ALCTLS_5_DIGIT;
const Counter::Style Counter::digits6         = ALCTLS_6_DIGIT;
const Counter::Style Counter::digits7         = ALCTLS_7_DIGIT;
const Counter::Style Counter::digits8         = ALCTLS_8_DIGIT;
const Counter::Style Counter::digits9         = ALCTLS_9_DIGIT;
const Counter::Style Counter::digits10        = ALCTLS_10_DIGIT;

Counter::Style Counter::currentDefaultStyle = ProgressBase::defaultStyle()
                                              | Counter::digits3 ;

const SpinDate::Style SpinDate::format_MDY     = ALCTLS_DT_FORMAT_MDY ;
const SpinDate::Style SpinDate::format_DMY     = ALCTLS_DT_FORMAT_DMY ;
const SpinDate::Style SpinDate::format_YMD     = ALCTLS_DT_FORMAT_YMD ;
const SpinDate::Style SpinDate::format_profile = ALCTLS_DT_FORMAT_PRF ;
const SpinDate::Style SpinDate::text_months    = ALCTLS_DT_TEXT_MONTHS;

SpinDate::Style SpinDate::currentDefaultStyle = ProgressBase::defaultStyle()
                                                | SpinDate::text_months;

Boolean ProgressBase::fLinked = false;



ProgressBase::ProgressBase(unsigned long style)
             :ulStyle(style)
{
 if (!fLinked) { //jt ForceLink();
                 fLinked = true;
               }
}

void ProgressBase :: setDefaultStyle ( ProgressBase::Style style )
{ currentDefaultStyle = style; }

IWindowHandle ProgressBase :: handle ( ) const
{ return whProgress; }

ProgressBase::Style ProgressBase :: defaultStyle ( )
{ return currentDefaultStyle; }

void ProgressBase ::SetValue(unsigned long ul)
{ whProgress.sendEvent(ALCTLM_SETVALUE,ul); }

void ProgressBase ::GetValue(unsigned long * pul)
{ whProgress.sendEvent(ALCTLM_GETVALUE,(unsigned long)pul); }

void ProgressBase ::SetDelta(unsigned long ul)
{ whProgress.sendEvent(ALCTLM_SETDELTA,ul); }

void ProgressBase ::GetDelta(unsigned long * pul)
{ whProgress.sendEvent(ALCTLM_SETDELTA,(unsigned long)pul); }

void ProgressBase ::Increment()
{ whProgress.sendEvent(ALCTLM_INCREMENT); }

void ProgressBase ::Decrement()
{ whProgress.sendEvent(ALCTLM_DECREMENT); }

void ProgressBase ::GetRange(unsigned long *pulLower,unsigned long *pulUpper)
{
 whProgress.sendEvent(ALCTLM_GETRANGE,(unsigned long)pulLower,
                      (unsigned long)pulUpper);
}

void ProgressBase ::SetColors(unsigned long ulFore,unsigned long ulAft)
{ whProgress.sendEvent(ALCTLM_SETCOLORS,ulFore,ulAft); }

void ProgressBase ::GetColors(unsigned long * pulFore,unsigned long * pulAft)
{
 whProgress.sendEvent(ALCTLM_GETCOLORS,(unsigned long)pulFore,
                     (unsigned long)pulAft);
}

void ProgressBase ::DelayHide(unsigned long miliSecs)
{ whProgress.sendEvent(ALCTLM_DELAY_HIDE,miliSecs); }

void ProgressBase ::SetRange(unsigned long ulLower,unsigned long ulUpper)
{ whProgress.sendEvent(ALCTLM_SETRANGE,ulLower,ulUpper); }

void ProgressBase ::Reset(void)
{ whProgress.sendEvent(ALCTLM_RESET); }

ISize ProgressBase :: minimumSize() const
{ return calcMinimumSize();}

ISize ProgressBase :: calcMinimumSize() const
{
 unsigned long ulW , ulH;
 whProgress.sendEvent(ALCTLM_MINIMUM_SIZE,(unsigned long)&ulW, (unsigned long)&ulH);
 return (ISize(ulW,ulH));
}

//////////////////////////////////////
Thermometer::Thermometer(unsigned long id, IWindow* parentDialog)
            :ProgressBase(0)
{
 IWindow * pW = new IWindow(id,parentDialog);
 if (pW && (whProgress = pW->handle())) {
   pW->setAutoDestroyWindow(true);
//jt  WinSetPresParam(whProgress,PP_FONTNAMESIZE,sizeof(szFontFace), (PVOID)szFontFace);
   }
}

Thermometer::Thermometer(unsigned long ulId,
                         IWindow* pwndParent,
                         IWindow* pwndOwner,
                         const IRectangle& rectInit,
                         const Style& style)
             :ProgressBase(style.asUnsignedLong())
{
 if (whProgress = IWindow::create( ulId ,0 ,ulStyle ,THERMOMETERCLASS
                                  ,pwndParent->handle()
                                  ,pwndOwner? pwndOwner->handle()
                                             :IWindowHandle()
                                  ,rectInit ,0,0)) {
//jt    WinSetPresParam(whProgress,PP_FONTNAMESIZE,sizeof(szFontFace), (PVOID)szFontFace);
//   IControl::setFont(IFont("8.Helv"));
    IWindow::startHandlingEventsFor(whProgress);
    }
}

//////////////////////////////////////
ProgressBar::ProgressBar(unsigned long id, IWindow* parentDialog)
            :ProgressBase(0)
{
 IWindow * pW = new IWindow(id,parentDialog);
 if (pW && (whProgress = pW->handle()))
     pW->setAutoDestroyWindow(true);
}

ProgressBar::ProgressBar(unsigned long ulId,
                         IWindow* pwndParent,
                         IWindow* pwndOwner,
                         const IRectangle& rectInit,
                         const Style& style)
            :ProgressBase(style.asUnsignedLong())
{
  if (whProgress = IWindow::create( ulId ,0 ,ulStyle ,PROGRESSBARCLASS
                                   ,pwndParent->handle()
                                   ,pwndOwner? pwndOwner->handle()
                                             : IWindowHandle()
                                   ,rectInit ,0,0))
     IWindow::startHandlingEventsFor(whProgress);
}

/////////////////////////////////////////////

MinSecTimer::~MinSecTimer()
{ whProgress.sendEvent(ALCTLM_STOP_COUNTING); }

MinSecTimer::MinSecTimer(unsigned long id, IWindow* parentDialog)
            :ProgressBase(0)
{
 IWindow * pW = new IWindow(id,parentDialog);
 if (pW && (whProgress = pW->handle()))
     pW->setAutoDestroyWindow(true);
}

MinSecTimer::MinSecTimer(unsigned long ulId,
                         IWindow* pwndParent,
                         IWindow* pwndOwner,
                         const IRectangle& rectInit,
                         const Style& style)
             :ProgressBase(style.asUnsignedLong())
{
//edwtest
 IStaticText * pW = new IStaticText(ulId, pwndParent);

// if (pW && (whProgress = pW->handle()))
//     pW->setAutoDestroyWindow(true);
/*edwtest
  if (whProgress = IWindow::create( ulId,0,ulStyle, MINSECTIMERCLASS
                                   ,pwndParent->handle()
                                   ,pwndOwner? pwndOwner->handle(): IWindowHandle()
                                   ,rectInit ,0,0))
*/
     IWindow::startHandlingEventsFor(whProgress);
}

void MinSecTimer :: SetTime(unsigned long ulMins,unsigned long ulSecs)
{ whProgress.sendEvent(ALCTLM_TIME_SET,ulMins,ulSecs); }

void MinSecTimer :: StartCountingUp(void)
{ whProgress.sendEvent(ALCTLM_START_COUNTING_UP); }

void MinSecTimer :: StartCountingDown(void)
{ whProgress.sendEvent(ALCTLM_START_COUNTING_DOWN); }

void MinSecTimer :: StopCounting(void)
{ whProgress.sendEvent(ALCTLM_STOP_COUNTING); }

void MinSecTimer :: LeaveWakeUpCall(IWindowHandle hndl ,unsigned long msg)
{ //jt whProgress.sendEvent(ALCTLM_LEAVE_WAKE_UP_CALL,(unsigned long)hndl,msg); }
  whProgress.sendEvent(ALCTLM_LEAVE_WAKE_UP_CALL); }

////////////////////////////////////
Counter::Counter(unsigned long id, IWindow* parentDialog)
        :ProgressBase(0)
{
 IWindow * pW = new IWindow(id,parentDialog);
 if (pW && (whProgress = pW->handle()))
     pW->setAutoDestroyWindow(true);
}

Counter::~Counter()
{ whProgress.sendEvent(ALCTLM_STOP_COUNTING); }

Counter::Counter(unsigned long ulId,
                 IWindow* pwndParent,
                 IWindow* pwndOwner,
                 const IRectangle& rectInit,
                 const Style& style)
        :ProgressBase(style.asUnsignedLong())
{
  if (whProgress = IWindow::create( ulId ,0 ,ulStyle, COUNTERCLASS
                                   ,pwndParent->handle()
                                   ,pwndOwner? pwndOwner->handle()
                                             : IWindowHandle()
                                   ,rectInit ,0,0))
     IWindow::startHandlingEventsFor(whProgress);
}

void Counter :: SetCount(unsigned long ulCount)
{ whProgress.sendEvent(ALCTLM_SETVALUE,ulCount); }

void Counter :: StartCountingUp(void)
{ whProgress.sendEvent(ALCTLM_START_COUNTING_UP); }

void Counter :: StartCountingDown(void)
{ whProgress.sendEvent(ALCTLM_START_COUNTING_DOWN); }

void Counter :: StopCounting(void)
{ whProgress.sendEvent(ALCTLM_STOP_COUNTING); }

void Counter :: LeaveWakeUpCall(IWindowHandle hndl ,unsigned long msg)
{ //jt whProgress.sendEvent(ALCTLM_LEAVE_WAKE_UP_CALL,(unsigned long)hndl,msg); }
  whProgress.sendEvent(ALCTLM_LEAVE_WAKE_UP_CALL); }

Counter::Style Counter :: defaultStyle ( )
{ return currentDefaultStyle; }

////////////////////////////////////

SpinDate::SpinDate(unsigned long id, IWindow* parentDialog)
         :ProgressBase(0)
         ,hndlTabOutRight(IWindowHandle())
         ,hndlTabOutLeft(IWindowHandle())
{
 IWindow * pW = new IWindow(id,parentDialog);
 if (!pW || !(whProgress = pW->handle()))
        //    WinAlarm(HWND_DESKTOP,WA_NOTE);
        MessageBeep(MB_ICONEXCLAMATION);  //jt
 else {
        pW->setAutoDestroyWindow(true);
        TabOutHandler.setOwnerWindow(this);
        TabOutHandler.handleEventsFor(pW);
      }
}

SpinDate::SpinDate(unsigned long ulId,
          IWindow* pwndParent,
          IWindow* pwndOwner,
          const IRectangle& rectInit,
          const Style& style)
        :ProgressBase(style.asUnsignedLong())
         ,hndlTabOutRight(IWindowHandle())
         ,hndlTabOutLeft(IWindowHandle())
{
  if (whProgress = IWindow::create( ulId ,0 ,ulStyle ,SPINDATECLASS
                                   ,pwndParent->handle()
                                   ,pwndOwner? pwndOwner->handle()
                                             : IWindowHandle()
                                   ,rectInit ,0,0)) {
   IWindow::startHandlingEventsFor(whProgress);
   TabOutHandler.setOwnerWindow(this);
   TabOutHandler.handleEventsFor(this);
   }
}

SpinDate::Style SpinDate:: defaultStyle ( )
{ return currentDefaultStyle; }

void SpinDate::SetLowerBound(unsigned short usM,unsigned short usD,
                             unsigned short usY)
{
 whProgress.sendEvent(ALCTLM_DATE_SET_LIMIT,
                      IEventData(usM,usD),IEventData(usY,false));
}

void SpinDate::SetUpperBound(unsigned short usM,unsigned short usD,
                             unsigned short usY)
{
 whProgress.sendEvent(ALCTLM_DATE_SET_LIMIT,
                      IEventData(usM,usD),IEventData(usY,true));
}

Boolean SpinDate::Set(const IDate& date)
{
 return Set((unsigned short)date.monthOfYear(),
            (unsigned short)date.dayOfMonth(), (unsigned short)date.year());
}

Boolean SpinDate::Set(unsigned short usM,unsigned short usD,
                      unsigned short usY)
{
 return (Boolean)whProgress.sendEvent(ALCTLM_DATE_SET,
                                 IEventData(usM,usD),IEventData(usY,0));
}

Boolean SpinDate::Add(unsigned short usM,unsigned short usD,
                      unsigned short usY)
{
 return (Boolean)whProgress.sendEvent(ALCTLM_DATE_ADD,
                                      IEventData(usM,usD),IEventData(usY,0));
}

Boolean SpinDate::Subtract(unsigned short usM,unsigned short usD,
                           unsigned short usY)
{
 return (Boolean)whProgress.sendEvent(ALCTLM_DATE_SUBTRACT,
                                      IEventData(usM,usD),IEventData(usY,0));
}

void SpinDate::Get(unsigned short * pusM,unsigned short * pusD,
                   unsigned short * pusY)
{
 unsigned long ul1,ul2;
 whProgress.sendEvent(ALCTLM_DATE_GET,&ul1,&ul2);
/* //jt
 *pusM = LOUSHORT(ul1);
 *pusD = HIUSHORT(ul1);
 *pusY = LOUSHORT(ul2);
*/
}

void SpinDate::Get(IDate & date)
{
 unsigned short usM,usD,usY;
 Get(&usM,&usD,&usY);
 date = IDate((IDate::Month)(int)usM,(int)usD,(int)usY);
}

Boolean SpinDate::SetMonthsNames(char ** Arr12)
{return //jt whProgress.sendEvent(ALCTLM_DATE_SET_MONTH_NAMES,(unsigned long)Arr12);}
  whProgress.sendEvent(ALCTLM_DATE_SET_MONTH_NAMES,(unsigned long)Arr12);}

void SpinDate::SetFocus(void)
{ whProgress.sendEvent(ALCTLM_SET_FOCUS);}

void SpinDate::SetToday(void)
{ whProgress.sendEvent(ALCTLM_DATE_SET_TODAY);}

Boolean SpinDate::TabOut(Boolean fRight)
//jt { return WinSetFocus(HWND_DESKTOP,fRight? (HWND)hndlTabOutRight: (HWND)hndlTabOutLeft); }
{ return 1; }

///////////////

Boolean SpinDateTabOutHandler :: userMessage(IEvent& evt)
{
 switch(evt.eventId()) {
   case  ALCTLM_TAB_OUT_LEFT:
   case  ALCTLM_TAB_OUT_RIGHT:
     return owner->TabOut(evt.eventId()==true);
   }
 return false;
}
