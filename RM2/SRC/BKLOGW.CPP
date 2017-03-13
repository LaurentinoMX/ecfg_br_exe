#define INCL_WINSTDCNR       //   container control class
#define INCL_PM
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream.h>

#include "bklogw.hpp"
#include "bklogw.h"
#include "prf.hpp"

#define MOUSECLICK_EXPAND      WM_USER+1

//////////////////////////////////////////////////////////////////
int BLRowSortUp :: isEqual (IContainerObject* p1, IContainerObject* p2,
                            ICnrCtl * cnr) const
{
 return strcmp((char *)(((BLRow *)p1)->keyStr()),
               (char *)(((BLRow *)p2)->keyStr()));
}
//////////////////////////////////////////////////////////////////
Boolean BLRowExpandFilter:: isMemberOf( IContainerObject * pObj,
                                        IContainerControl* container) const
{
 BLRow * pR = ( BLRow * )pObj;
 if (pR->isMachineRow()) {
   ((BLRowExpandFilter *)this)->fExpandMachine = (pR->expandStr() == FMINUS);
   return true;
   }
 return fExpandMachine;
}
//////////////////////////////////////////////////////////////////
IString BLRow::machineKey( BLFeat & f)
{
 IString str(f.isFirstMRDBMachine()? "0" : "1");
 str += IString(f.machine()) + IString(f.model()) + IString(f.serialNo());
 return str;
}

BLRow::BLRow( BLFeat & f, EUOPProfile * pProf, RMBkLogW * win, Boolean fMachRow)
      :IContainerObject(title(f),IResourceId(fMachRow? ICO_4010 : ICO_4005))
      ,strKey(machineKey(f))
      ,fMachine(fMachRow)
      ,pMywin( win )
{
 if (fMachRow) {
   strMachModel = IString(f.machine()) + IString(SLASH) + IString(f.model());
   f.machineQty_Action( strQty, strAction );
   strDesc = f.machineDescription();
   strExpand = IString((f.isFeaturelessMachine()? FBLANK : FPLUS));
   }
 else {
   f.qty_Action( strQty, strAction);
   f.description(strDesc);
   strFeat = IString(f.feature());
   strKey += strFeat;
   IDate idate;
   strCSDDate = (pProf && f.csdDate(idate))?
             pProf->FormatNLSDateString(idate.monthOfYear(),
                                        idate.dayOfMonth(),
                                        idate.year() % 100) : IString();
   strCRADDate = (pProf && f.cradDate(idate))?
             pProf->FormatNLSDateString(idate.monthOfYear(),
                                        idate.dayOfMonth(),
                                        idate.year() % 100) : IString();
   }
}

IString BLRow::title(BLFeat & f)
{
 IString str(f.machine());
 str += "/"; str += f.model();
 str += "   Serial# "; str += f.displaySerialNoStr();
 return str;
}

void BLRow::handleSelectedChange(IContainerControl * pCnr,Boolean fAcquired )
{
 if ( fAcquired )
   pMywin->setCntrTitle(this);
}

/////////////////////////////////////////////////////////////////////////////

RMBkLogW::RMBkLogW( unsigned long windowId
                   ,IWindow *Parent
                   ,IHelpWindow  * pHW
                   ,unsigned long closingMsg)
         :IFrameWindow( IFrameWindow::titleBar        |
                        IFrameWindow::systemMenu      |
                        IFrameWindow::sizingBorder    |
                        IFrameWindow::hideButton      |
                        IFrameWindow::maximizeButton
                        ,windowId)
        ,BaseWnd(this,Parent,closingMsg,pHW)
        ,pExpandRow(false)
        ,pLeftCntCtlW(0)
{
/*//JT #if __IBMCPP__ == 300
 cnrCtl=new IContainerControl( CNR_BKLOG, this, this,IRectangle(),
                               IContainerControl::defaultStyle() |
                               IContainerControl::noSharedObjects );
#else
 cnrCtl = new IContainerControl( CNR_BKLOG, this, this );
#endif*/
 cnrCtl = new IContainerControl(CNR_BKLOG, this, this, IRectangle(), IContainerControl::defaultStyle() | IContainerControl::pmCompatible);

 setClient( cnrCtl );
 cnrCtl->enableTabStop();
 cnrCtl->showTitle().showTitleSeparator()
                    .setTitleAlignment( IContainerControl::left );

 colExpand=new IContainerColumn( BLRow::expandOffset() );
 /*//JT colIcon=new IContainerColumn( IContainerObject :: iconOffset(),
                               IContainerColumn::defaultHeadingStyle(),
                               IContainerColumn::icon |
                               IContainerColumn::alignVerticallyCentered );*/
 colIcon = new IContainerColumn(IContainerColumn::isIcon,
                                       IContainerColumn::defaultHeadingStyle(),
                                       IContainerColumn::icon);

 colMachModel=new IContainerColumn( BLRow::machModelOffset() );
 colMachModel->showSeparators().setHeadingText( MACH_STR );
 colAction = new IContainerColumn( BLRow::actionOffset() );
 colAction->showSeparators().setHeadingText( ACTION_STR );
 colQty = new IContainerColumn( BLRow::qtyOffset() );
 colQty->justifyData( IContainerColumn::centeredVertically,
                      IContainerColumn::right )
         .showSeparators().setHeadingText( QTY_STR );
 colFeature = new IContainerColumn( BLRow::featOffset() );
 colFeature->showSeparators( IContainerColumn::horizontalSeparator )
            .setHeadingText( FEATURE_STR );
 colDesc = new IContainerColumn( BLRow::descOffset() );
 colDesc->showSeparators().setHeadingText( DESC_STR );
 colCRAD = new IContainerColumn( BLRow::cradDateOffset() );
 colCRAD->showSeparators().setHeadingText( CRAD_STR );
 colCSD = new IContainerColumn( BLRow::csdDateOffset());
 colCSD->showSeparators( IContainerColumn::horizontalSeparator )
        .setHeadingText( CSD_STR );
 cnrCtl->addColumn( colExpand )
        .addColumn( colIcon )
        .addColumn( colAction )
        .addColumn( colQty )
        .addColumn( colMachModel )
        .addColumn( colFeature )
        .addColumn( colDesc )
        .addColumn( colCRAD )
        .addColumn( colCSD );
 cnrCtl->setDeleteColumnsOnClose()
        .setDeleteObjectsOnClose()
        .setAutoDeleteObject();
 cnrCtl->showDetailsView()
        .enableDrawItem()
        .setMixedTargetEmphasis();
 cnrCtl->show();

// start handlers
 ICnrHandler::handleEventsFor(cnrCtl);
//jt IMouseHandler::handleEventsFor( cnrCtl );
 IMouseHandler::handleEventsFor( this);
//Attach the mouseclick handler to the child windows of the container
//to handle the button1 singleclick to expand/collapse a machine
/*jt
 IWindow *pContainerKid = 0;
 IWindow::ChildCursor cursor( *cnrCtl );
 for (cursor.setToFirst(); cursor.isValid() && !pLeftCntCtlW; cursor.setToNext())
   if ((pContainerKid = new IWindow( childAt(cursor))))// &&
       //JT pContainerKid->id() == CID_LEFTDVWND)
     IMouseHandler::handleEventsFor( pLeftCntCtlW = pContainerKid);
   else delete pContainerKid;
*/
}

RMBkLogW::~RMBkLogW()
{
 if (pLeftCntCtlW) delete pLeftCntCtlW;
}

void RMBkLogW :: Sort(void)
{
 BLRowSortUp sortUp;
 cnrCtl->sort(sortUp);
}

void RMBkLogW::expand_Collapse(BLRow * pR)
{
 if (pR && pR->isMachineRow() && (pR->expandStr() != FBLANK)) {
   pR->setExpandStr((pR->expandStr() == FMINUS)? FPLUS : FMINUS);
   setCntrTitle(pR);
   filter();
   }
}

void RMBkLogW::filter(Boolean fScroll)
{
 IContainerObject * pObj = fScroll? cnrCtl->cursoredObject() : 0;
 cnrCtl->filter(expandFilter);
 if (pObj)
   cnrCtl->scrollToObject(pObj);
}

#if __IBMCPP__ == 300
Boolean RMBkLogW::load( BLFeatList * pFL, EUOPProfile * pProf )
{
 if (!pFL || !pProf)
   return false;
 IString lastKey,thisKey;
 BLFeatList::Cursor csr( *pFL );
 Able(false);
 cnrCtl->setRefreshOff();
 cnrCtl->deleteAllObjects();
 if ( (  !pFL->isEmpty() ) ) {
   ICnrAllocator allocator(pFL->numberOfElements() * 2,sizeof(BLRow));
   forCursor(csr) {
     BLFeat & f = pFL->elementAt(csr);
     thisKey = BLRow::machineKey(f);
     if (thisKey != lastKey) {
       new(allocator) BLRow(f,pProf,this,true);
       lastKey = thisKey;
       }
       new(allocator) BLRow(f,pProf,this);
     }
   cnrCtl->addObjects(allocator);
   filter();
   }
 cnrCtl->setRefreshOn();
 cnrCtl->refresh();
 Able(true);
 return true;
}
#else
Boolean RMBkLogW::load( BLFeatList * pFL, EUOPProfile * pProf )
{
 if (!pFL || !pProf)
   return false;
 IString lastKey,thisKey;
 BLFeatList::Cursor csr( *pFL );
 Able(false);
 cnrCtl->setRefreshOff();
 cnrCtl->deleteAllObjects();
 if ( (  !pFL->isEmpty() ) ) {
   forCursor(csr) {
     BLFeat & f = pFL->elementAt(csr);
     thisKey = BLRow::machineKey(f);
     if (thisKey != lastKey) {
       cnrCtl->addObject( new BLRow(f,pProf,this,true) );
       lastKey = thisKey;
       }
      cnrCtl->addObject( new BLRow(f,pProf,this) );
     }
   filter();
   }
 cnrCtl->setRefreshOn();
 cnrCtl->refresh();
 Able(true);
 return true;
}
#endif

void RMBkLogW::setCntrTitle(BLRow * pR)
{ cnrCtl->setTitle(pR? pR->iconText() : IString(BLANK)); }

Boolean RMBkLogW::mouseClicked( IMouseClickEvent& evt )
{
 if ( evt.mouseButton() == IMouseClickEvent::button1 )   //JT
  if ( evt.mouseAction() == IMouseClickEvent::up ) {
    IContainerColumn * pCol=cnrCtl->columnUnderPoint(evt.mousePosition());
    pExpandRow = (BLRow *) cnrCtl->objectUnderPoint( evt.mousePosition() );
    if (pExpandRow &&  ( pCol == colExpand ) && pExpandRow->isMachineRow() &&
        ( pExpandRow->expandStr() != FBLANK ) ) {
      if (cnrCtl->isSelected(pExpandRow))
        handle().postEvent(MOUSECLICK_EXPAND);
//    else 'selectedChanged() will handle it
      }
    else pExpandRow = 0;
    }
 return( false );
}

Boolean RMBkLogW::userMessage(IEvent& evt)
{
 switch(evt.eventId()) {
   case MOUSECLICK_EXPAND: {
     BLRow * p = pExpandRow;
     pExpandRow = 0;
     expand_Collapse(p);
     return true;
     }
   }
 return false;
}

Boolean  RMBkLogW::selectedChanged(ICnrEmphasisEvent& evt)
{
 BLRow * pObj = (BLRow *)evt.object();
 if (pExpandRow && cnrCtl->isSelected(evt.object()) && pExpandRow == pObj)
   handle().postEvent(MOUSECLICK_EXPAND);
 else pObj->handleSelectedChange(cnrCtl,true);
 return true;
}

Boolean RMBkLogW::help(ICnrHelpEvent& evt)
{
 if (helpWindow())
   helpWindow()->show(IHelpWindow::general);
 return true;
}
