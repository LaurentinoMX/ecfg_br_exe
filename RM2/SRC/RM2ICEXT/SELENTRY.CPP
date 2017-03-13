#define INCL_DOS
#define INCL_PM
#include <os2.h>
#include <istring.hpp>
#include <iwindow.hpp>
#include "selentry.hpp"

ISelectiveEntry :: ISelectiveEntry(unsigned long id,
                                   IWindow* parent,
                                   IWindow* owner,
                                   const IRectangle& initial,
                                   const Style& style) :
                   IEntryField(id,parent,owner,initial,style),
                   ISelectiveKeyHandler()
{
  ISelectiveKeyHandler::handleEventsFor(this);
}

ISelectiveEntry :: ISelectiveEntry(unsigned long id,
                                   IWindow* parent) :
                   IEntryField(id,parent),
                   ISelectiveKeyHandler()
{
  ISelectiveKeyHandler::handleEventsFor(this);
}

ISelectiveEntry :: ISelectiveEntry(const IWindowHandle& handle) :
                   IEntryField(handle),
                   ISelectiveKeyHandler()
{
  ISelectiveKeyHandler::handleEventsFor(this);
}

ISelectiveEntry :: ~ISelectiveEntry()
{
  ISelectiveKeyHandler::stopHandlingEventsFor(this);
}
