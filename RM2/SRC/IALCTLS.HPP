#ifndef _AB_ALCTLS_HPP_
#define _AB_ALCTLS_HPP_

#define INCL_WIN              // for WinSetPresParam  only
#include <os2.h>
//#include <ifont.hpp>
#include <irect.hpp>
#include <icontrol.hpp>
#include <ievtdata.hpp>
#include <idate.hpp>
//#include <iwindow.hpp>
#include "myusrhdr.hpp"
//#include "controls.h"

//class ProgressBase : public IWindow
class ProgressBase : public IControl
{
 public:
  INESTEDBITFLAGCLASSDEF0(Style, ProgressBase);

  static const Style  no_percentage,
                      visible,
                      vertical,
                      border_furrow,
                      border_inni,
                      border_outi,
                      border_system,
                      border_standard;

        ProgressBase(unsigned long Style);

  void           setDefaultStyle (ProgressBase::Style);
  IWindowHandle  handle ( ) const;
  void           SetValue(unsigned long);
  void           GetValue(unsigned long *);
  void           GetRange(unsigned long *pulLower,unsigned long *pulUpper);
  void           SetRange(unsigned long ulLower,unsigned long ulUpper);
  void           SetColors(unsigned long ulFore,unsigned long ulAft);
  void           GetColors(unsigned long * pulFore,unsigned long * pulAft);
  void           SetDelta(unsigned long);
  void           GetDelta(unsigned long *);
  void           DelayHide(unsigned long miliSecs = 0 );
  void           Increment(void);
  void           Decrement(void);
  void           Reset(void);
  ISize          minimumSize() const;

  static Style   defaultStyle ( );

 protected:
    ISize calcMinimumSize() const;

 private:
  friend class Thermometer;
  friend class ProgressBar;
  friend class MinSecTimer;
  friend class Counter;
  friend class SpinDate;
  IWindowHandle whProgress;
  static Style  currentDefaultStyle;
  static Boolean fLinked;
  unsigned long ulStyle;
};


class ProgressBar : public ProgressBase
{
 public:

        ProgressBar(unsigned long id, IWindow* parentDialog);
        ProgressBar( unsigned long ulId
                    ,IWindow* pwndParent
                    ,IWindow* pwndOwner
                    ,const IRectangle& initial= IRectangle()
                    ,const Style& style = defaultStyle() );

};

class Thermometer : public ProgressBase
{
 public:

        Thermometer(unsigned long id, IWindow* parentDialog);
        Thermometer( unsigned long ulId
                    ,IWindow* pwndParent
                    ,IWindow* pwndOwner
                    ,const IRectangle& initial= IRectangle()
                    ,const Style& style = defaultStyle() );

};

class MinSecTimer : public ProgressBase
{
 public:

        MinSecTimer(unsigned long id, IWindow* parentDialog);
        MinSecTimer( unsigned long ulId
                    ,IWindow* pwndParent
                    ,IWindow* pwndOwner
                    ,const IRectangle& initial= IRectangle()
                    ,const Style& style = defaultStyle() );
       ~MinSecTimer();
  void           SetTime(unsigned long ulMins,unsigned long ulSecs);
  void           StartCountingUp(void);
  void           StartCountingDown(void);
  void           LeaveWakeUpCall(IWindowHandle,unsigned long msg);
  void           StopCounting(void);
};

class Counter : public ProgressBase
{
 public:

  INESTEDBITFLAGCLASSDEF1(Style, Counter,ProgressBase);

  static const Style  digits1,
                      digits2,
                      digits3,
                      digits4,
                      digits5,
                      digits6,
                      digits7,
                      digits8,
                      digits9,
                      digits10;

        Counter(unsigned long id, IWindow* parentDialog);
        Counter( unsigned long ulId
                ,IWindow* pwndParent
                ,IWindow* pwndOwner
                ,const IRectangle& initial= IRectangle()
                ,const Style& style = defaultStyle() );
       ~Counter();
  void           SetCount(unsigned long ulCount);
  void           StartCountingUp(void);
  void           StartCountingDown(void);
  void           LeaveWakeUpCall(IWindowHandle,unsigned long msg);
  void           StopCounting(void);
  static Style   defaultStyle ( );

 private:
  static Style  currentDefaultStyle;
};


class SpinDate;                 // forward declaration of class
class SpinDateTabOutHandler : public MyUserMsgHandler
{
  public:
    SpinDateTabOutHandler() : owner(0) {}
    void setOwnerWindow(SpinDate *pW) { owner = pW; }
  protected:
    Boolean userMessage(IEvent&);
  private:
    SpinDate * owner;
};

class SpinDate : public ProgressBase
{
 public:

  INESTEDBITFLAGCLASSDEF1(Style, SpinDate,ProgressBase);

  static const Style  format_MDY,
                      format_DMY,
                      format_YMD,
                      format_profile,
                      text_months;

       SpinDate(unsigned long id, IWindow* parentDialog);
       SpinDate( unsigned long ulId
                ,IWindow* pwndParent
                ,IWindow* pwndOwner
                ,const IRectangle& initial= IRectangle()
                ,const Style& style = defaultStyle() );
  static Style   defaultStyle ( );
  void           SetUpperBound(unsigned short usM,unsigned short usD,
                               unsigned short usY);
  void           SetLowerBound(unsigned short usM,unsigned short usD,
                               unsigned short usY);
  Boolean        Set(unsigned short usM,unsigned short usD,
                     unsigned short usY);
  Boolean        Set(const IDate& date);
  Boolean        SetMonthsNames(char ** Array12);
  Boolean        Add(unsigned short usM,unsigned short usD,
                     unsigned short usY);
  Boolean        Subtract(unsigned short usM,unsigned short usD,
                          unsigned short usY);
  void           Get(unsigned short * pusM,unsigned short * pusD,
                     unsigned short * pusY);
  void           Get(IDate& date);
  void           SetTabOutHandles(IWindowHandle Left, IWindowHandle Right)
                  { hndlTabOutLeft = Left; hndlTabOutRight = Right;}
  void           SetFocus(void);
  void           SetToday(void);
  Boolean        TabOut(Boolean fRight);

 private:
  SpinDateTabOutHandler TabOutHandler;
  static Style   currentDefaultStyle;
  IWindowHandle  hndlTabOutRight;
  IWindowHandle  hndlTabOutLeft;
  void           SetValue(unsigned long){}
  void           GetValue(unsigned long *){}
  void           GetRange(unsigned long *,unsigned long *){}
  void           SetRange(unsigned long,unsigned long){}
  void           SetDelta(unsigned long){}
  void           GetDelta(unsigned long *){}
};

INESTEDBITFLAGCLASSFUNCS(Style, Counter);

INESTEDBITFLAGCLASSFUNCS(Style, SpinDate);

#endif

