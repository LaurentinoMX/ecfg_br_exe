
        /*---------------------------------------------------*\
        |          PM and Standard Library includes           |
        \*---------------------------------------------------*/
//jt extern "C"
//jt   {
    #define INCL_PM
    #define INCL_DOSNLS
    #define INCL_WIN
    #define INCL_WINSHELLDATA
    #include <os2.h>

    #include <stdio.h>
//jt   }

#define INCL_IPOPUPMENU
#include "uicl.hpp"

        /*---------------------------------------------------*\
        |               EUOP-specific includes                |
        \*---------------------------------------------------*/
#include "cfrsw.hpp"
#include "myxcphdr.hpp"                          // MyExceptionHandler
#include "myfcshdr.hpp"                          // MyFocusHandler
#include "prf.hpp"                               // EUOPProfile
#include "swdw.h"
#include "myptrhdr.hpp"
#include "swdw.hpp"                            // this window's include
#include "euoutil.hpp"

#include "euopids.h"

void SWCnrObj :: handleOpen(IContainerControl * container)
{  MessageBeep(MB_ICONEXCLAMATION);     //jt
}//jt WinAlarm(HWND_DESKTOP,WA_ERROR);  }

Boolean SWCnrObj::operator==(SWCnrObj const& other)  const
{
  return ((strType  == other.strType)   &&
          (strModel == other.strModel)  &&
          (strFeat  == other.strFeat)   &&
          (strDesc  == other.strDesc) );
}

Boolean SWCnrObj::operator< (SWCnrObj const& other) const
{
  if (strType == other.strType)
    if (strModel == other.strModel)
      if (strFeat == other.strFeat)
        return (strDesc < other.strDesc);
      else return (strFeat < other.strFeat);
    else return (strModel < other.strModel);
  else return (strType < other.strType);
}

Boolean SWCnrObj::operator> (SWCnrObj const& other) const
{ return !(*this < other);}

Boolean SWCnrObj::IsEqual(SoftMach const & sm)
{ return ((strType    == sm.strType())      &&
          (strModel   == sm.strModel())     &&
          (strFeat    == sm.strFeature())   &&
          (strDesc    == sm.strDescription()));
}

///////////////////////////////////////////////////////////////

SWDWndw :: SWDWndw( unsigned long ulId
                   ,SoftMachList *pList
                   ,IHelpWindow  * pHW
                   ,IWindow *     Parent
                   ,unsigned long ClosingMsg
                   ,long NumberOfLines         // minimum height
                    )
  : IFrameWindow( IFrameWindow::titleBar
                  | IFrameWindow::systemMenu
                  | IFrameWindow::sizingBorder
                  | IFrameWindow::maximizeButton
                  | IFrameWindow::hideButton
                 ,ulId)
   ,BaseWnd(this,Parent,ClosingMsg,pHW)
   ,SC(SWDW_ID_SC,this,this)
   ,PBOK(DID_OK,&SC,&SC)
   ,PBHelp(SWDW_ID_PB_HELP,&SC,&SC)
   ,pSWList(pList)
   ,ulLines(NumberOfLines)
{
 Init();
}

void SWDWndw :: Init()
{
 ulObjH = ulObjW = 0;
 CreateClient();
 CreateExtension();
 if (!helpWindow())
   PBHelp.disable();

 ICnrHandler::handleEventsFor(pCnrCtl);         // for handleOpen
 ICnrMenuHandler::handleEventsFor(pCnrCtl);
                     /*-------------------------------------*\
                     | If container is NOT the frame's       |
                     | client, the Shift-F10 menu trigger is |
                     | routed via the accelarator table to   |
                     | the frame. Supposed to get fixed with |
                     | CSD CM0006. Handler is only           |
                     | attachable to a container.            |
                     \*-------------------------------------*/
#ifndef NO_MULTICELL
//  ICnrMenuHandler::handleEventsFor(this);
#endif
  FillList();                                    // fill it with the List data
}

SWDWndw :: ~SWDWndw()
{
}

void SWDWndw :: CreateClient()
{
#ifdef NO_MULTICELL
 pCnrCtl = new IContainerControl( SWDW_ID_CNR ,this ,this,
                                  IRectangle()
                                 ,  IContainerControl::defaultStyle()
                                  | IContainerControl::readOnly );
 setClient(pCnrCtl);
#else
 pMCCControls = new IMultiCellCanvas(SWDW_ID_MCC,this,this);
 pMCCControls->setAutoDeleteObject();
 pCnrCtl = new IContainerControl( SWDW_ID_CNR ,pMCCControls ,pMCCControls,
                                  IRectangle()
                                 ,  IContainerControl::defaultStyle()
                                  | IContainerControl::readOnly
                                  | IContainerControl::pmCompatible );//jt
 setClient(pMCCControls);
 pGB = new IGroupBox(SWDW_ID_GB, pMCCControls,pMCCControls);
 pGB->setAutoDeleteObject();
#endif

 pCnrCtl->enableGroup().enableTabStop();
// pCnrCol1 = new IContainerColumn(offsetof(SWCnrObj,strType));
 pCnrCol1 = new IContainerColumn(SWCnrObj::typeOffset());
 pCnrCol1->justifyData(IContainerColumn::centeredVertically,
                       IContainerColumn::right)
          .setHeadingText(SWDW_ID_STR_COL_TYPE).showSeparators();
 pCnrCol2 = new IContainerColumn(SWCnrObj::modelOffset());
 pCnrCol2->justifyData().setHeadingText(SWDW_ID_STR_COL_MODEL).showSeparators();
 pCnrCol3 = new IContainerColumn(SWCnrObj::featureOffset());
 pCnrCol3->justifyData().setHeadingText(SWDW_ID_STR_COL_FEAT).showSeparators();
 pCnrCol4 = new IContainerColumn(SWCnrObj::descriptionOffset());
 pCnrCol4->setHeadingText(SWDW_ID_STR_COL_DESC)
          .showSeparators(IContainerColumn::horizontalSeparator);
 pCnrCtl->addColumn(pCnrCol1)
         .addColumn(pCnrCol2)
         .addColumn(pCnrCol3)
         .addColumn(pCnrCol4)
         .showDetailsView()
         .setDeleteObjectsOnClose()
         .setDeleteColumnsOnClose()
         .setAutoDeleteObject();
#ifndef NO_MULTICELL
 ArrangeControls();
#endif
// setMinimumSize(minimumSize());
}

          /*-----------------------------------------------------------------*\
          |   void ArrangeControls()                                          |
          |      Method places and spaces the controls in our MultiCanvas     |
          |      Client.                                                      |
          |      Returns: nothing                                             |
          \*-----------------------------------------------------------------*/
void SWDWndw :: ArrangeControls()
{
  pMCCControls->addToCell(pGB         ,2, 2, 3, 4)
               .addToCell(pCnrCtl     ,3, 4      );
  pMCCControls->setColumnWidth(3,0,true)
               .setColumnWidth(5,IMultiCellCanvas::defaultCell().width())
               .setRowHeight(3,IMultiCellCanvas::defaultCell().height())
               .setRowHeight(4,0,true)
               .setRowHeight(6,IMultiCellCanvas::defaultCell().height());
}

void SWDWndw :: CreateExtension()
{
 SC.setDeckCount(1).setDeckOrientation(ISetCanvas::horizontal)
   .setPackType(ISetCanvas::expanded).setAlignment(ISetCanvas::centerCenter);
 PBOK.enableDefault().setText(IResourceId(ID_STR_ACC_OK))
     .enableTabStop().enableGroup();
 PBHelp.disableMouseClickFocus().setText(IResourceId(ID_STR_HELP));
 addExtension(&SC,belowClient,(unsigned long)SC.minimumSize().height());
}

          /*-----------------------------------------------------------------*\
          |   void SizeToMaximumTextWidth()                                   |
          |       Method sizes the window to a width which allows             |
          |       the Container to show the longest entry columnized          |
          |       display text (up to a limit of 90% of the screen width)     |
          |       and a height required to show the minimum number of         |
          |       items.                                                      |
          |       Returns : nothing                                           |
          |       Note:                                                       |
          |          There is a problem with the container returning the      |
          |          correct display sizes for the current objects after      |
          |          objects have been modified or,in our case, deleted       |
          |          and another set added: If the container has not been     |
          |          shown with a set of objects different from the original  |
          |          set, 'detailsObjectRectangle()' returns a height of 2 and |
          |          the width of the original objects.                       |
          |          When this window is shown modally, we cannot show        |
          |          the container first and then 'SizeToMaximumTextWidth()'  |
          |          to get the current dimensions.                           |
          |          Also, I have a suspicion that messages generated by a    |
          |          'show()' might very well be arriving after a call        |
          |          to this method.                                          |
          |          Therefore, we retain those of the first set of objects.  |
          |          (Peter Hagar on C++CLASS forum 6-2-94)                   |
          |          Turns out it works if the container is shown before      |
          |          filling it.                                              |
          \*-----------------------------------------------------------------*/

Boolean SWDWndw :: SizeObjects()
{
 if (!ulObjH || !ulObjW || isVisible()) {
   IContainerControl::ObjectCursor CntCrsr(*pCnrCtl);
   CntCrsr.setToFirst();
   if (CntCrsr.isValid()) {
     IContainerObject *pObj = pCnrCtl->objectAt(CntCrsr);
     IRectangle irct(pCnrCtl->detailsObjectRectangle(pObj,0));
     ulObjW = irct.width();
     if (pCnrCtl->splitBarOffset())
       ulObjW += (pCnrCtl->detailsObjectRectangle(pObj,true).width()+10);
     ulObjW = ALMIN(IWindow::desktopWindow()->size().width()*0.9,ulObjW);
     ulObjW = ALMAX(pCnrCtl->minimumSize().width(),ulObjW);
     ulObjH = irct.height();
     return true;
     }
   }
 return false;
}

void SWDWndw :: SizeToMaximumTextWidth()
{
/*jt  SizeObjects();
                                                 // ulLines might be altered
 unsigned long ulH = ALMIN((ulLines+2)*ulObjH + 5,
                           IWindow::desktopWindow()->size().height() * 0.9);
 ulH = ALMAX(pCnrCtl->minimumSize().height(),ulH);
 pCnrCtl->sizeTo(ISize(ulObjW,ulH));
 IRectangle irct(pCnrCtl->rect());
#ifdef NO_MULTICELL
// moveSizeToClient(pCnrCtl->rect());
#else
 irct.expandBy(IPair(2*IMultiCellCanvas::defaultCell().width(),
                     3*IMultiCellCanvas::defaultCell().height()));
#endif
 irct.moveTo(IPoint((IWindow::desktopWindow()->size().width()-irct.width())/2,
                  (IWindow::desktopWindow()->size().height()-irct.height())/2));
 moveSizeToClient(irct); jt*/
}

IWindow & SWDWndw :: setFocus()
{
 pCnrCtl->setFocus();
 return *this;
}

unsigned long SWDWndw :: showModally()
{
 SWDWndw::setFocus();
 return IFrameWindow::showModally();
}

          /*-----------------------------------------------------------------*\
          |    ISize calcMinimumSize() const                                  |
          |       Method, called by 'IFrameWindow', returns the minimum       |
          |       size for the window.                                        |
          |       Method is overridden since IFrameWindow's method            |
          |       does not poperly take into account the Info Area and        |
          |       PushButton Extension.                                       |
          |       Returns: ISize : minimum size.                              |
          \*-----------------------------------------------------------------*/

ISize SWDWndw :: calcMinimumSize() const
{
/*jt long lBorderH = WinQuerySysValue(HWND_DESKTOP,SV_CYSIZEBORDER);
#ifdef NO_MULTICELL
 return ISize(pCnrCtl->minimumSize().width() +
               WinQuerySysValue(HWND_DESKTOP,SV_CXSIZEBORDER) * 2,
              pCnrCtl->minimumSize().height() +
               lBorderH  * 3 + WinQuerySysValue(HWND_DESKTOP,SV_CYTITLEBAR));
#else
 return ISize(pMCCControls->minimumSize().width() +
               WinQuerySysValue(HWND_DESKTOP,SV_CXSIZEBORDER) * 2,
            pMCCControls->minimumSize().height() +
             lBorderH  * 3 + WinQuerySysValue(HWND_DESKTOP,SV_CYTITLEBAR));
#endif   jt*/
 return ISize(pCnrCtl->minimumSize().width() + 10,
              pCnrCtl->minimumSize().height() + 10);
}

Boolean SWDWndw::help(ICnrHelpEvent& evt)
{
// int i2 = evt.controlWindow()->id();
 if (helpWindow())
    helpWindow()->show(IHelpWindow::general);
 //jt else WinAlarm(HWND_DESKTOP,WA_ERROR);
 else  MessageBeep(MB_ICONEXCLAMATION);     //jt
 return true;
}

Boolean SWDWndw::SetSplitBar()
{
 IContainerControl::ObjectCursor CntCrsr(*pCnrCtl);
 CntCrsr.setToFirst();
 if (CntCrsr.isValid()) {
   IContainerObject *pObj = pCnrCtl->objectAt(CntCrsr);
   unsigned long ulSplit =
       pCnrCtl->detailsObjectRectangle(pObj,pCnrCol1).width() +
       pCnrCtl->detailsObjectRectangle(pObj,pCnrCol2).width();
   pCnrCtl->setDetailsViewSplit(pCnrCol3,ulSplit-1);
   return true;
   }
 return false;
}

void  SWDWndw :: FillList(Boolean fEmptyFirst)
{
 pCnrCtl->setRefreshOff();
 if (fEmptyFirst)
    pCnrCtl->deleteAllObjects();
 SoftMachList::Cursor crsr(*pSWList);
 forCursor(crsr)
     pCnrCtl->addObject(new SWCnrObj(pSWList->elementAt(crsr)));
 pCnrCtl->setRefreshOn();
 SetSplitBar();
// SizeObjects();
 pCnrCtl->refresh();
}

void  SWDWndw :: FillList(SoftMachList * pList)
{
 pSWList = pList;
 FillList();
}

Boolean SWDWndw :: PressedEnter(IKeyboardEvent & evt)
{
 if ( evt.handle() == PBOK.handle()  ||
      evt.handle() == PBHelp.handle() ) {
   evt.handle().sendEvent(BM_CLICK);
   return true;
   }
 //jt WinAlarm(HWND_DESKTOP, WA_NOTE);
 MessageBeep(MB_ICONEXCLAMATION);     //jt

 return false;
}

Boolean SWDWndw :: command(ICommandEvent & evt)
{
 Boolean fProcessed = true;
 switch (evt.commandId()) {
   case SWDW_ID_POP_SORT_TYPE:
   case SWDW_ID_POP_SORT_MODEL:
   case SWDW_ID_POP_SORT_FEAT:
   case SWDW_ID_POP_SORT_DESC: {
/*jt     HPOINTER hptr = WinQueryPointer(HWND_DESKTOP);
     WinSetPointer(HWND_DESKTOP,
                   WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,false));
     Sort(evt.commandId());
     WinSetPointer(HWND_DESKTOP,hptr);   jt*/
     break;
     }
   case DID_OK:
   case SWDW_ID_POP_CANCEL:
     NotifyMyClosing(DID_CANCEL);
     break;
   case SWDW_ID_PB_HELP:
   case SWDW_ID_POP_HELP:
     helpWindow()->show(IHelpWindow::general);
     break;
   default:
     fProcessed = false;
   }
 return fProcessed;
}

Boolean SWDWndw :: makePopUpMenu(IMenuEvent& menuEvent)
{
 IPopUpMenu * popUp = new IPopUpMenu(SWDW_ID_POPUP, menuEvent.window());
//  IPopUpMenu * popUp = new IPopUpMenu(SWDW_ID_POPUP, this);
//  popUp->setFont(IFont(&MCCControls));
 popUp->setAutoDeleteObject();
 if (!helpWindow())
   popUp->disableItem(SWDW_ID_POP_HELP);
#if 0
 if (!pCnrCtl->objectCount())
   popUp->disableItem(SWDW_ID_POP_SORT);
#endif
 IPoint pt(menuEvent.mousePosition());
 IRectangle rct(pCnrCtl->rect());
 if (pt.x() >= 0 && pt.x() <= rct.width() &&
     pt.y() >= 0 && pt.y() <= rct.height())
    popUp->show(pt);
 else  popUp->show(IPoint());
 return true;
}

int SWDWndwSort1 :: isEqual (IContainerObject* object1,
                             IContainerObject* object2,
                             ICnrCtl * container) const
{
 const SWCnrObj * p1 = (SWCnrObj *)object1
              , * p2 = (SWCnrObj *)object2;
 return ( (*p1 < *p2)? -1 : ( (*p1 == *p2)? 0 : 1 ));
}

int SWDWndwSort2 :: isEqual (IContainerObject* object1,
                             IContainerObject* object2,
                             ICnrCtl * container) const
{
 const SWCnrObj * p1 = (SWCnrObj *)object1
              , * p2 = (SWCnrObj *)object2;
 int i = strcmp(p1->model(),p2->model());
 if (!i)
   if (!(i = strcmp(p1->type(),p2->type())))
     if (!(i = strcmp(p1->feature(),p2->feature())))
       i = strcmp(p1->description(),p2->description());
 return i;
}

int SWDWndwSort3 :: isEqual (IContainerObject* object1,
                             IContainerObject* object2,
                             ICnrCtl * container) const
{
 const SWCnrObj * p1 = (SWCnrObj *)object1
              , * p2 = (SWCnrObj *)object2;
 int i = strcmp(p1->feature(),p2->feature());
 if (!i)
   if (!(i = strcmp(p1->type(),p2->type())))
     if (!(i = strcmp(p1->model(),p2->model())))
       i = strcmp(p1->description(),p2->description());
 return i;
}

int SWDWndwSort4 :: isEqual (IContainerObject* object1,
                             IContainerObject* object2,
                             ICnrCtl * container) const
{
 const SWCnrObj * p1 = (SWCnrObj *)object1
              , * p2 = (SWCnrObj *)object2;
 int i = strcmp(p1->description(),p2->description());
 if (!i)
   if (!(i = strcmp(p1->type(),p2->type())))
     if (!(i = strcmp(p1->model(),p2->model())))
       i = strcmp(p1->feature(),p2->feature());
 return i;
}

void SWDWndw :: Sort(unsigned type)
{
 ICnrCtl::CompareFn* comparison = 0;
 switch(type) {
   case SWDW_ID_POP_SORT_TYPE:
     comparison = new SWDWndwSort1; break;
   case SWDW_ID_POP_SORT_MODEL:
     comparison = new SWDWndwSort2; break;
   case SWDW_ID_POP_SORT_FEAT:
     comparison = new SWDWndwSort3; break;
   default :
     comparison = new SWDWndwSort4;
   }
 pCnrCtl->sort(*comparison);
 delete comparison;
}

        /*---------------------------------------------------*\
        |                     Test Driver                     |
        \*---------------------------------------------------*/

#ifdef DRIVER


int main(int argc, char **argv)
{
 IApplication::current().setArgs(argc, argv);
 EUOPProfile * pProfile = new EUOPProfile();
 if (!(pProfile->SetResources(IApplication::current())))
   exit(3) ;
 SoftMachList *pL = new SoftMachList;
 pL->add(SoftMach("Typ1","M1","Feat1","Description 1"));
 pL->add(SoftMach("Typ3","M1","Feat1","Description 2"));
 pL->add(SoftMach("Typ2","M1","Feat1","Description 3"));
 pL->add(SoftMach("Typ1","M1","Feat1","Description 5"));
 pL->add(SoftMach("Typ1","M1","Feat1","Description 4"));

 SWDWndw *pW = new SWDWndw(WND_SWD,pL);

 pL->add(SoftMach("Typ1","M1","Feat1","The straggler with a longer description which goes on and on!"));
 pW->FillList(pL);
 pW->Sort();

//jt  pW->SizeToMaximumTextWidth();
 pW->Center();
 pW->show();
 IApplication::current().run();
 delete pW;
}
#endif


        /*---------------------------------------------------*\
        |             Unused - non compiled code              |
        \*---------------------------------------------------*/

#if 0

#endif
