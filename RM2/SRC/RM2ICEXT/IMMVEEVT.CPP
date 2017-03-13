#include "immveevt.hpp"

IMouseMoveEvent :: IMouseMoveEvent (IEvent& event) :
  IEvent(event)
{
};

IPoint IMouseMoveEvent :: mousePosition ( ) const
{
  IEventData tempParam = parameter1();
  return IPoint(tempParam.number1(),tempParam.number2());
};

unsigned short IMouseMoveEvent :: hitTest ( ) const
{
  return parameter2().number1();
};

unsigned short IMouseMoveEvent :: keyCode ( ) const
{
  return parameter2().number2();
};
