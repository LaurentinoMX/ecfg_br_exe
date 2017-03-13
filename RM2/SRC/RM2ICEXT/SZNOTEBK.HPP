#ifndef _DJL_AUTOSIZINGNOTEBOOK_
#define _DJL_AUTOSIZINGNOTEBOOK_
#ifndef _IPOINT_
  #include <ipoint.hpp>
#endif
#ifndef _IWINDOW_
  #include <iwindow.hpp>
#endif
#ifndef _INOTEBK_
  #include <inotebk.hpp>
#endif

class AutoSizingNotebook : public INotebook
{
  public:
    AutoSizingNotebook   ( unsigned long        windowId,
                           IWindow*             parent,
                           IWindow*             owner,
                           const IRectangle&    initial = IRectangle(),
                           const Style&         style = defaultStyle() ) :
      INotebook(windowId,parent,owner,initial,style),
      majorTabSize(0,0),
      minorTabSize(0,0) {};
    AutoSizingNotebook   ( unsigned long        windowId,
                           IWindow*             parentAndOwner ) :
      INotebook(windowId,parentAndOwner),
      majorTabSize(0,0),
      minorTabSize(0,0) {};
    AutoSizingNotebook   ( const IWindowHandle& handle ) :
      INotebook(handle),
      majorTabSize(0,0),
      minorTabSize(0,0) {};

    IPageHandle
      addFirstPage      ( const PageSettings& pageInfo,
                          IWindow*            window = 0 ),
      addLastPage       ( const PageSettings& pageInfo,
                          IWindow*            window = 0 ),
      addPageBefore     ( const PageSettings& pageInfoToAdd,
                          const IPageHandle&  referencePage,
                          IWindow*            window = 0 ),
      addPageAfter      ( const PageSettings& pageToAdd,
                          const IPageHandle&  referencePage,
                          IWindow*            window = 0 ),
      addPageBefore     ( const PageSettings& pageInfo,
                          const Cursor&       cursor,
                          IWindow*            window = 0 ),
      addPageAfter      ( const PageSettings& pageInfo,
                          const Cursor&       cursor,
                          IWindow*            window = 0 );

  protected:
    ISize calcMinimumSize() const;
    void setTabSize(const PageSettings &);
    
  private:
    ISize majorTabSize;
    ISize minorTabSize;
};
#endif
