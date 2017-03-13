#define INCL_PM
#include <os2.h>
#include <ihandler.hpp>
#include "userhdr.hpp"

Boolean IUserHandler :: dispatchHandlerEvent(IEvent& event)
{
  if (event.eventId() >= WM_USER)
    {
    return userMessage(event);
    }
  else
    return false;
};
