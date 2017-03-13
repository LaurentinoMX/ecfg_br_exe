#define INCL_PM
#include <os2.h>
#include <ihandler.hpp>
#include "ictrlhdr.hpp"

Boolean ICtrlHandler :: dispatchHandlerEvent(IEvent& event)
{
  if (event.eventId() == WM_CONTROL)
    {
    ICtrlEvent temp(event);
    return control(temp);
    }
  else
    return false;
};
