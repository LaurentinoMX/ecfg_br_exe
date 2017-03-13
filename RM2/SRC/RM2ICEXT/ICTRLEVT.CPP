#define INCL_PM
#include <os2.h>
#include "ictrlevt.hpp"

ICtrlEvent :: ICtrlEvent (IEvent& event) :
  IEvent(event)
{
};

unsigned short ICtrlEvent :: controlId ( ) const
{
  return parameter1().number1();
};

unsigned short ICtrlEvent :: notifyCode ( ) const
{
  return parameter1().number2();
};
