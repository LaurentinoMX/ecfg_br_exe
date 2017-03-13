#ifndef _DJL_IMOUSEMOVEHANDLER_
#define _DJL_IMOUSEMOVEHANDLER_

#include <ihandler.hpp>
#include "immveevt.hpp"

class IMouseMoveHandler : public IHandler
{
  protected:
    virtual Boolean
      mouseMove(IMouseMoveEvent& moveEvt) = 0;

  public:
      IMouseMoveHandler ( ) {;};

    Boolean
      dispatchHandlerEvent(IEvent& evt);
};
#endif
