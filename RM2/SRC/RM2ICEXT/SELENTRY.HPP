#ifndef _DJL_ISELECTIVEENTRY_
#define _DJL_ISELECTIVEENTRY_

#include <ientryfd.hpp>
#include "keyhndlr.hpp" // SelectiveKeyHandler

class ISelectiveEntry : public IEntryField, public ISelectiveKeyHandler
{
  public:
    ISelectiveEntry(unsigned long id,
                    IWindow* parent,
                    IWindow* owner,
                    const IRectangle& initial= IRectangle(),
                    const Style& style = defaultStyle() );

    ISelectiveEntry(unsigned long id,
                    IWindow* parent);

    ISelectiveEntry(const IWindowHandle& handle);

    virtual ~ISelectiveEntry();
};
#endif
