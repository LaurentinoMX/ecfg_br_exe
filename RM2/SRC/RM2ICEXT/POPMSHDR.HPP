#ifndef _DJL_POPUPMOUSECLICKHDR_
#define _DJL_POPUPMOUSECLICKHDR_

#include <ihandler.hpp>
#include <imoushdr.hpp> //<imchdr.hpp>
#include <ireslib.hpp>
#include <ipopmenu.hpp>

class PopupMouseClickHandler : public IMouseHandler
{
  private:
    IPopUpMenu  * thePopupMenu;

  protected:
    Boolean
      mouseClicked(IMouseClickEvent& event);

  public:
    PopupMouseClickHandler () :
      thePopupMenu(0) {};
    PopupMouseClickHandler (const IResourceId &aMenuResId,
                            IWindow * owner) :
      thePopupMenu(new IPopUpMenu(aMenuResId,owner)) {};
    PopupMouseClickHandler (IPopUpMenu * aPopup) :
      thePopupMenu(aPopup) {};

  IPopUpMenu *
    popupMenu() { return thePopupMenu; };

  void
    setPopupMenu(IPopUpMenu * aPopup)
      { delete thePopupMenu;
        thePopupMenu = aPopup; };
};
#endif

