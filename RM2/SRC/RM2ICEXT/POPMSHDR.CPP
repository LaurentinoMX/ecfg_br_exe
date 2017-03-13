#define INCL_PM
#include <os2.h>
#include "popmshdr.hpp"

Boolean PopupMouseClickHandler :: mouseClicked(IMouseClickEvent& event)
{
  if ( (event.mouseButton() == IMouseClickEvent::button2) &&
       (event.mouseAction() == IMouseClickEvent::click) )
    {
    thePopupMenu->show(event.mousePosition());
    return True;
    };
  return false;
};
