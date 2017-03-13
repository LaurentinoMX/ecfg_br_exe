#define INCL_PM
#include <os2.h>
#include <ihandler.hpp>
#include "immvehdr.hpp"

Boolean IMouseMoveHandler :: dispatchHandlerEvent(IEvent& event)
{
  if (event.eventId() == WM_MOUSEMOVE)
    {
    IMouseMoveEvent temp(event);
    return mouseMove(temp);
    }
  else
    return false;
};
