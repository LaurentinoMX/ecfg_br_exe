#include "sznotebk.hpp"
#ifndef _IFONT_
  #include <ifont.hpp>
#endif
#ifndef _ISEQ_H
  #include <iseq.h>
#endif

ISize AutoSizingNotebook :: calcMinimumSize() const
{
  /* algorithm:
       a. iterate through pages in notebook to find page with largest
          minimum size
       b. save current notebook size
       c. size notebook to result of (a)
       d. use pageSize() to determine actual page size now
       e. restore notebook size to (b)
       f. return 2(a) - (d) as minimum size */

  INotebook::Cursor pageCursor(*this);
  ISize maxSize     = ISize(0,0);
  ISize curThisSize = ISize(0,0);
  ISize minThisSize = ISize(0,0);
  unsigned long tempWidth = 0;
  unsigned long tempHeight = 0;
  forCursor(pageCursor)
    {
    tempWidth  = window(pageCursor)->minimumSize().width();
    tempHeight = window(pageCursor)->minimumSize().height();
    if (tempWidth > maxSize.width())
      maxSize.setWidth(tempWidth);
    if (tempHeight > maxSize.height())
      maxSize.setHeight(tempHeight);
    };
  curThisSize = size();
  /* -- this is a cheat to get past the fact that this function is const -- */
  INotebook * pThis = (INotebook *)this;
  pThis->sizeTo(maxSize);
  minThisSize = (2 * maxSize) - pageSize();
  pThis->sizeTo(curThisSize);
  return minThisSize;
};

void AutoSizingNotebook :: setTabSize(const PageSettings & pageInfo)
{
  IFont thisFont(this);
  ISize tempSize = ISize(0,0);
  if (pageInfo.isMajorTab())
    {
    tempSize = ISize(thisFont.textWidth(pageInfo.tabText()) + thisFont.maxSize().width(),
                thisFont.maxSize().height() * 1.5);
    majorTabSize = ISize(max(tempSize.width(),majorTabSize.width()),
                         max(tempSize.height(),majorTabSize.height()));
    setMajorTabSize(majorTabSize);
    }
  else if (pageInfo.isMinorTab())
    {
    tempSize = ISize(thisFont.textWidth(pageInfo.tabText()) + thisFont.maxSize().width(),
                thisFont.maxSize().height() * 1.5);
    minorTabSize = ISize(max(tempSize.width(),minorTabSize.width()),
                         max(tempSize.height(),minorTabSize.height()));
    setMinorTabSize(minorTabSize);
    };
};

IPageHandle AutoSizingNotebook :: addFirstPage ( const PageSettings& pageInfo,
                                                 IWindow*            window  )
{
  IPageHandle pgHandle = INotebook::addFirstPage(pageInfo,window);
  setTabSize(pageInfo);
  return pgHandle;
};

IPageHandle AutoSizingNotebook :: addLastPage ( const PageSettings& pageInfo,
                                                IWindow*            window  )
{
  IPageHandle pgHandle = INotebook::addLastPage(pageInfo,window);
  setTabSize(pageInfo);  
  return pgHandle;
};

IPageHandle AutoSizingNotebook :: addPageBefore ( const PageSettings& pageInfoToAdd,
                                                  const IPageHandle&  referencePage,
                                                  IWindow*            window  )
{
  IPageHandle pgHandle = INotebook::addPageBefore(pageInfoToAdd,referencePage,window);
  setTabSize(pageInfoToAdd);
  return pgHandle;
};

IPageHandle AutoSizingNotebook :: addPageAfter ( const PageSettings& pageInfoToAdd,
                                                  const IPageHandle&  referencePage,
                                                  IWindow*            window  )
{
  IPageHandle pgHandle = INotebook::addPageAfter(pageInfoToAdd,referencePage,window);
  setTabSize(pageInfoToAdd);
  return pgHandle;
};

IPageHandle AutoSizingNotebook :: addPageBefore ( const PageSettings& pageInfo,
                                                  const Cursor&       cursor,
                                                  IWindow*            window  )
{
  IPageHandle pgHandle = INotebook::addPageBefore(pageInfo,cursor,window);
  setTabSize(pageInfo);
  return pgHandle;
};

IPageHandle AutoSizingNotebook :: addPageAfter ( const PageSettings& pageInfo,
                                                 const Cursor&       cursor,
                                                 IWindow*            window  )
{
  IPageHandle pgHandle = INotebook::addPageAfter(pageInfo,cursor,window);
  setTabSize(pageInfo);
  return pgHandle;
};
