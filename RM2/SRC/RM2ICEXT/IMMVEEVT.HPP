#ifndef _DJL_IMOUSEMOVEEVNT_
#define _DJL_IMOUSEMOVEEVNT_

#include <ipoint.hpp>
#include <ievent.hpp>

class IMouseMoveEvent : public IEvent
{
  public:
      IMouseMoveEvent (IEvent& event);

    IPoint
      mousePosition ( ) const;

    unsigned short
      hitTest ( ) const,
      keyCode ( ) const;
};
#endif
