 /*--------------------------------------------------------------------------*\
 |    Filename - RPOWC.CPP                                                    |
 |      contains source for the following EUOP class implementation:          |
 |                                                                            |
 |      RPOsWndw         - RPO Window                                         |
 |                         Window allows the user to view and prune           |
 |                         a list of RPOs in the detailed view of a           |
 |                         container.                                         |
 |----------------------------------------------------------------------------|
 |   Change Log:                                                              |
 |                                                                            |
 |   HECC      08/28/2002  Added necessary logic to display a new button in   |
 |                         RPO List window 'Submit' and the necessary logic   |
 |                         to begin the process of the RPO List once this     |
 |                         button be pressed.                                 |
 |                                                                            |
 \*--------------------------------------------------------------------------*/

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

        /*---------------------------------------------------*\
        |          IBMClass includes - see RPOW.HPPP          |
        \*---------------------------------------------------*/
#define INCL_IFONT
#define INCL_IPOPUPMENU
#define INCL_ITITLE
#include "uicl.hpp"

        /*---------------------------------------------------*\
        |               EUOP-specific includes                |
        \*---------------------------------------------------*/
#include "rpo2.hpp"                              // RPO
#include "myxcphdr.hpp"                          // MyExceptionHandler
#include "prf.hpp"                               // EUOPProfile
#include "rpo2wc.h"                              // RPOsWndw Ids
#include "rpo2wc.hpp"                            // this window's include
#include "rpo2l.hpp"                             // RPOList && RPOpList

#include "euoutil.hpp"
#include "euopids.h"

#include "mainw.hpp"                             //* 73686   HECC: 10/12/2002

        /*---------------------------------------------------*\
        |                       defines                       |
        \*---------------------------------------------------*/
#define SORTED_UP    1
#define SORTED_DOWN  0
#define SORTED_NOT  -1

        /*---------------------------------------------------*\
        |             RPO Container Object Class              |
        \*---------------------------------------------------*/
RPOCnrObj::RPOCnrObj(RPO2 & rpo,RPOsWndw *pW)
          : pWin(pW)
           ,strNmbr(rpo.qty())
           ,strDesc(rpo.description())
           ,strAction(rpo.action())
{
 strMMF = rpo.machineType() + IString("-")  +
          rpo.machineModel() + IString("-") +
          rpo.featureCode();
}


void RPOCnrObj :: handleOpen(IContainerControl * container)
{ pWin->sendEvent(WM_COMMAND,IEventParameter1(RPOSWND_POP_DELETE)); }

Boolean RPOCnrObj::operator==(RPOCnrObj const& other)  const
{ return ((strMMF==other.strMMF) && (strAction==other.strAction));}

Boolean RPOCnrObj::operator< (RPOCnrObj const& other) const
{ return ((strMMF == other.strMMF)? strAction < other.strAction :
                                    strMMF < other.strMMF);}

Boolean RPOCnrObj::operator> (RPOCnrObj const& other) const
{ return ((strMMF == other.strMMF)? strAction > other.strAction :
                                    strMMF > other.strMMF);}

//Boolean RPOCnrObj::operator> (RPOCnrObj const& other) const
//{ return ((strNmbr.asInt() == other.strNmbr.asInt())? strTxt > other.strTxt
//                                : strNmbr.asInt() > other.strNmbr.asInt());}

Boolean RPOCnrObj::IsEqual(RPO2 const & rpo)
{  return ((strNmbr.asInt() == rpo.qty())                 &&
          (strDesc         == IString(rpo.description())) &&
          (strAction       == IString(rpo.action()))      &&
          (strMMF          == rpo.machineType()+"-"+rpo.machineModel()+"-"+rpo.featureCode()) );}

RPOsWndw :: RPOsWndw(unsigned long ulId
                     ,RPO2List * pList
                     ,EUOPProfile *pProf
                     ,IWindow *     Parent
                     ,IHelpWindow * pHW
                     ,unsigned long ClosingMsg
                     ,long NumberOfLines         // minimum height
                    )
  : IFrameWindow( IFrameWindow::titleBar
                  | IFrameWindow::accelerator
                  | IFrameWindow::systemMenu
                  | IFrameWindow::sizingBorder
                 ,ulId)
   ,BaseWnd(this,Parent,ClosingMsg,pHW)
   ,pRPOList(pList)
   ,pProfile(pProf)
   ,ulLines(NumberOfLines)
   ,iSorted(SORTED_NOT)
   ,fAutoGen(false)
{
 ulObjH = ulObjW = 0;

 //* Gets the name of the CFReport file, if the configuration is S390
 //* and the Order Forward application execution path.
 //* 73686 HECC: 09/02/2002

 XWindow* pParent = ( XWindow* )Parent;
 strAASFile = pParent->getAASFile();

 MainWindow* pMain = pParent->getMainWindow();
 strOrFwdPath = pMain->strExecPath + STR_OrderForward;
 bIsS390      = pMain->bS390;

 //* End new logic: 73686  HECC - 09/02/2002

 if (fMyProfile = (pProfile == NULL )) {         // get profile if not given
   pProfile = new EUOPProfile(true);
   pProfile->SetResources(IApplication::current());
   pProfile->ReadProfile();
   ITitle(this).setObjectText(IResourceId(ulId));
   }
 CreateClient();
 if (!pHW)
   pPBHelp->disable();
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
 MyTrackHandler::StartIfSizable(this);          // enable minimum size handler
 FillList();                                    // fill it with the List data
//  if (fAutoGen = pProfile->GetBooleanKey(EUOPProfile::PrcssGenRPOAuto))
//    pPBSave->disable();
}
          /*-------------------------------------------------*\
          |      Dtor - free obtained storage resources       |
          \*-------------------------------------------------*/
RPOsWndw :: ~RPOsWndw()
{
 if (fMyProfile && pProfile)  delete pProfile;
}

void RPOsWndw :: CreateClient()
{
#ifdef NO_MULTICELL
 pCnrCtl = new IContainerControl( RPOSWND_CNR_RPOS ,this ,this,
                                  IRectangle()
                                 ,(IContainerControl::defaultStyle()
                                   | IContainerControl::multipleSelection
                                   | IContainerControl::readOnly )
                                 & ~IContainerControl::singleSelection);
 setClient(pCnrCtl);
#else
 pMCCControls = new IMultiCellCanvas(RPOSWND_MCC,this,this);
 pMCCControls->setAutoDeleteObject();
 pCnrCtl = new IContainerControl( RPOSWND_CNR_RPOS ,pMCCControls ,pMCCControls,
                                  IRectangle()
                                 ,(IContainerControl::defaultStyle()
                                   | IContainerControl::multipleSelection
                                   | IContainerControl::readOnly
                                   | IContainerControl::pmCompatible) //jt-chg
                                 & ~IContainerControl::singleSelection);
 setClient(pMCCControls);
 pGBRPOs = new IGroupBox(RPOSWND_GB, pMCCControls,pMCCControls);
 pGBRPOs->setAutoDeleteObject();
#endif

 pCnrCtl->enableGroup().enableTabStop();
 pCnrCol1 = new IContainerColumn(RPOCnrObj::machineModelFeatureOffset());
 pCnrCol1->justifyData(IContainerColumn::centeredVertically,
                          IContainerColumn::centered)
          .setHeadingText(RPOSWND_STR_COL_MMF).showSeparators();
 pCnrCol2 = new IContainerColumn(RPOCnrObj::actionOffset());
 pCnrCol2->justifyData().setHeadingText(RPOSWND_STR_COL_ACTION).showSeparators();
 pCnrCol3 = new IContainerColumn(RPOCnrObj::quantityOffset());
 pCnrCol3->justifyData().setHeadingText(RPOSWND_STR_COL_NUMBER).showSeparators();
 pCnrCol4 = new IContainerColumn(RPOCnrObj::descriptionOffset());
 pCnrCol4->setHeadingText(RPOSWND_STR_COL_DESC)
          .showSeparators(IContainerColumn::horizontalSeparator);
 pCnrCtl->addColumn(pCnrCol1)
         .addColumn(pCnrCol2)
         .addColumn(pCnrCol3)
         .addColumn(pCnrCol4)
         .showDetailsView()
         .setDeleteObjectsOnClose()
         .setDeleteColumnsOnClose()
         .setAutoDeleteObject();
                                                 // create pushbuttons in a
 CreatePBs();                                    // SetCanvas Extension
 SetupInfoArea();
 pCnrCtl->setMinimumSize(ISize(pSCPBs->minimumSize().width(),
                               pSCPBs->minimumSize().height() * 2));
#ifndef NO_MULTICELL
 ArrangeControls();
#endif
 setMinimumSize(minimumSize());
}

          /*-----------------------------------------------------------------*\
          |   void ArrangeControls()                                          |
          |      Method places and spaces the controls in our MultiCanvas     |
          |      Client.                                                      |
          |      Returns: nothing                                             |
          \*-----------------------------------------------------------------*/
void RPOsWndw :: ArrangeControls()
{
  pMCCControls->addToCell(pGBRPOs  ,2, 2, 3, 4)
               .addToCell(pCnrCtl  ,3, 4      );
  pMCCControls->setColumnWidth(3,0,true)
               .setRowHeight(3,IMultiCellCanvas::defaultCell().height())
               .setRowHeight(4,0,true)
               .setRowHeight(6,IMultiCellCanvas::defaultCell().height())
               .setColumnWidth(5,IMultiCellCanvas::defaultCell().width());
}

          /*-----------------------------------------------------------------*\
          |   void SizeToMaximumTextWidth()                                   |
          |       Method sizes the window to a width which allows             |
          |       the Container to show the longest RPO columnized            |
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
          |          Alseo I have a suspicion that messages generated by a    |
          |          'show()' might very well be arriving after a call        |
          |          to this method.                                          |
          |          Therefore, we retain those of the first set of objects.  |
          |          (Peter Hagar on C++CLASS forum 6-2-94)                   |
          |          Turns out it works if the container is shown before      |
          |          filling it.                                              |
          \*-----------------------------------------------------------------*/

void RPOsWndw :: SizeObjects()
{
  if (isVisible()) {
   IContainerControl::ObjectCursor CntCrsr(*pCnrCtl);
   CntCrsr.setToFirst();
   if (CntCrsr.isValid()) {
     IContainerObject *pObj = pCnrCtl->objectAt(CntCrsr);
     IRectangle irct(pCnrCtl->detailsObjectRectangle(pObj,0));
     ulObjW = irct.width();
         //If there is a splitbar, only the left window's rectangle is returned!
     if (pCnrCtl->splitBarOffset())
       ulObjW += (pCnrCtl->detailsObjectRectangle(pObj,true).width()+10);
     ulObjW = ALMIN(IWindow::desktopWindow()->size().width()*0.9,ulObjW);
     ulObjW = ALMAX(pCnrCtl->minimumSize().width(),ulObjW);
     ulObjH = ALMAX(irct.height(),2*IMultiCellCanvas::defaultCell().height());
     }
   }
 else {
   ulObjW = pCnrCtl->minimumSize().width();
   ulObjH = 2*IMultiCellCanvas::defaultCell().height();
   }
}

void RPOsWndw :: SizeToMaximumTextWidth()
{
  SizeObjects();
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
//     moveSizeToClient(pMCCControls->rect());
#endif
 irct.moveTo(IPoint((IWindow::desktopWindow()->size().width()-irct.width())/2,
                  (IWindow::desktopWindow()->size().height()-irct.height())/2));
// irct.moveTo(position());
 moveSizeToClient(irct);

}

          /*-----------------------------------------------------------------*\
          |   IWindow & setFocus()                                            |
          |       Method to override the inherited method to                  |
          |       set the focus to the Conatainer.                            |
          \*-----------------------------------------------------------------*/
IWindow & RPOsWndw :: setFocus()
{
 pCnrCtl->setFocus();
 return *this;
}

          /*-----------------------------------------------------------------*\
          |   unsigned long  showModally()                                    |
          |       Method to override the inherited method to                  |
          |       set the focus to the ListBox.                               |
          |       Returns: the inherited method's return value.               |
          \*-----------------------------------------------------------------*/
unsigned long RPOsWndw :: showModally()
{
 RPOsWndw::setFocus();
 return IFrameWindow::showModally();
}

          /*-----------------------------------------------------------------*\
          |    Boolean  SetMinTrackSize(IPoint & ipoint)                      |
          |       Method called by 'MyTrackHandler' returning our calculated  |
          |       minimum size.                                               |
          |       Returns: true to indicate 'message processed'               |
          \*-----------------------------------------------------------------*/
Boolean RPOsWndw :: SetMinTrackSize(IPoint & ipoint)
{
 ipoint = minimumSize();
 return true;
}

          /*-----------------------------------------------------------------*\
          |    Boolean SetupInfoArea()                                        |
          |       Method determines if an Information Area is required        |
          |       using the EUOPProfile.                                      |
          |       If needed, the area is built and the proper handlers        |
          |       are enbled for those controls that the user will be         |
          |       shown information for when the controls gain focus.         |
          |       Returns: false/true : Info Area (not)built                  |
          \*-----------------------------------------------------------------*/
Boolean RPOsWndw :: SetupInfoArea()
{
 if (pProfile->GetBooleanKey(EUOPProfile::ViewGenRPOInfo)) {
   pInfo = new IInfoArea(this);
   pInfo->setAutoDeleteObject();
   pInfo->setAlignment(IStaticText::topLeftWrapped);
   setExtensionSize(pInfo,((int)IFont(pInfo).maxCharHeight())*2);
   pInfo->setInactiveText(ulInfoText = RPOSWND_STR_INFO_INACT );
   MyWindowFocusHandler::handleEventsFor(pPBSave);
   MyWindowFocusHandler::handleEventsFor(pPBCancel);
   MyWindowFocusHandler::handleEventsFor(pPBProcRPO);   //* 73686 HECC: 08/28/2002
//jt   MyWindowFocusHandler::handleEventsFor(pPBHelp);
   MyWindowFocusHandler::handleEventsFor(pCnrCtl);
   IMouseHandler::handleEventsFor(pPBSave);
   IMouseHandler::handleEventsFor(pPBCancel);
   IMouseHandler::handleEventsFor(pPBProcRPO);          //* 73686 HECC: 08/28/2002
//jt   IMouseHandler::handleEventsFor(pPBHelp);
   }
 else pInfo = 0;
 return (pInfo != 0);
}

          /*-----------------------------------------------------------------*\
          |    Boolean gettingFocus(IEvent& evt)                              |
          |       Method, called by 'MyWindowFocusHandler', sets the          |
          |       Info Area text string according to the control which        |
          |       gained focus.                                               |
          |       The text is only set if it differs from that currently      |
          |       displayed to cut down on flicker.                           |
          |       Returns: false : to allow proper focus to be set by system. |
          \*-----------------------------------------------------------------*/
Boolean RPOsWndw :: gettingFocus(IEvent& evt)
{
 unsigned long ulText;
 switch (evt.window()->id()) {
   case DID_OK:          ulText = RPOSWND_STR_INFO_SAVE;    break;
   case DID_CANCEL:      ulText = RPOSWND_STR_INFO_CANCEL;  break;
   case RPOSWND_PB_HELP: ulText = RPOSWND_STR_INFO_HELP;    break;
   default:              ulText = RPOSWND_STR_INFO_INACT;
   }
 if (pInfo && (ulInfoText != ulText))
   pInfo->setText(ulInfoText = ulText);
 return false;
}

          /*-----------------------------------------------------------------*\
          |    Boolean LostFocus(IEvent&)                                     |
          |       Method, called by 'MyWindowFocusHandler', when              |
          |       one of the controls it handles loses focus and              |
          |       none of the others gain it.                                 |
          |       Set the Area text to the default string if                  |
          |       there is an Info Area.                                      |
          |       Returns: false : to allow proper focus to be set by         |
          |       system.                                                     |
          \*-----------------------------------------------------------------*/
Boolean RPOsWndw :: LostFocus(IEvent& evt)
{
 if ( pInfo)
   pInfo->setText(ulInfoText = RPOSWND_STR_INFO_INACT);
 return false;
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

ISize RPOsWndw :: calcMinimumSize() const
{
 ISize isizeSCPBs(pSCPBs->minimumSize());
/*jt
 long lBorderH = WinQuerySysValue(HWND_DESKTOP,SV_CYSIZEBORDER);
#ifdef NO_MULTICELL
 return ISize(isizeSCPBs.width() +
               WinQuerySysValue(HWND_DESKTOP,SV_CXSIZEBORDER) * 2,
            pCnrCtl->minimumSize().height() + isizeSCPBs.height() +
//          (pInfo? (pInfo->size().height() + lBorderH) : 0) +
            (pInfo? ((((int)IFont(pInfo).maxCharHeight())*2)+lBorderH) : 0) +
            lBorderH  * 3 + WinQuerySysValue(HWND_DESKTOP,SV_CYTITLEBAR));
#else
 return ISize(isizeSCPBs.width() +
               WinQuerySysValue(HWND_DESKTOP,SV_CXSIZEBORDER) * 2,
            pMCCControls->minimumSize().height() + isizeSCPBs.height() +
//          (pInfo? (pInfo->size().height() + lBorderH) : 0) +
            (pInfo? ((((int)IFont(pInfo).maxCharHeight())*2)+lBorderH) : 0) +
            lBorderH  * 3 + WinQuerySysValue(HWND_DESKTOP,SV_CYTITLEBAR));
#endif  jt*/
 return ISize(isizeSCPBs.width() + 10,

            pCnrCtl->minimumSize().height() + isizeSCPBs.height() +
//          (pInfo? (pInfo->size().height() + lBorderH) : 0) +
            (pInfo? ((((int)IFont(pInfo).maxCharHeight())*2)+10) : 0) + 10 );
}

Boolean RPOsWndw::help(ICnrHelpEvent& evt)
{
// int i2 = evt.controlWindow()->id();
 if ( helpWindow())
     helpWindow()->show(IHelpWindow::general);
  else MessageBeep(MB_ICONEXCLAMATION);  //jt
 return true;
}

          /*-----------------------------------------------------------------*\
          |    void CreatePBs()                                               |
          |       Method builds the Pushbutton extension:                     |
          |         1. Create the pushbuttons and add them to a SetCanvas     |
          |         2. Calculate and set the canvas' minimum size             |
          |         3. Add the extension and make it visible                  |
          |       Return: nothing                                             |
          |                                                                   |
          |    F73686  Creates and adds the new button to start the process   |
          |    of the RPO list    (08/28/2002)                                |
          \*-----------------------------------------------------------------*/

void RPOsWndw :: CreatePBs()
{
  //* unsigned short usPBCount = 3;  //* 73686 HECC: 08/28/2002
  unsigned short usPBCount = 4;      //* 73686 HECC: 08/28/2002

  pSCPBs = new ISetCanvas(RPOSWND_PB_SCV,this, this);
  pSCPBs->setAutoDeleteObject();
  pSCPBs->setDeckOrientation(ISetCanvas::vertical)
          .setPackType(ISetCanvas::expanded);

  pPBSave = new IPushButton(DID_OK,pSCPBs,pSCPBs);
  pPBSave->setAutoDeleteObject();
  pPBSave->enableDefault().setText(IResourceId(ID_STR_ACC_SAVE))
          .enableTabStop().enableGroup();

  //* Begin new logic:  Create the new pushbutton that will process the
  //* RPO's list that was generated in the reconciliation process.
  //* F73686 HECC: 08/28/2002
  pPBProcRPO = new IPushButton(RPOSWND_PB_SUBMIT,pSCPBs,pSCPBs);
  pPBProcRPO->setAutoDeleteObject();
  pPBProcRPO->setText(IResourceId(RPOSWND_STR_SUBMIT));

  //* If the configuration in process is S390, don't process the RPO list.
  if ( bIsS390 )
     pPBProcRPO->disable();

  //* End. F73686 HECC: 08/28/2002

  pPBCancel = new IPushButton(DID_CANCEL,pSCPBs,pSCPBs);
  pPBCancel->setAutoDeleteObject();
  pPBCancel->setText(IResourceId(ID_STR_CANCEL));

  pPBHelp = new IPushButton(RPOSWND_PB_HELP,pSCPBs,pSCPBs);
  pPBHelp->setAutoDeleteObject();
  pPBHelp->setText(IResourceId(ID_STR_HELP));

  ISize isize( pPBCancel->minimumSize().width() * usPBCount
                + pSCPBs->pad().width() * (usPBCount-1)
                + pSCPBs->margin().width() * 2
                  ,pPBCancel->minimumSize().height()
                + pSCPBs->margin().width() * 2 );

  pSCPBs->setDeckCount(usPBCount).setDeckOrientation(ISetCanvas::vertical);
  pSCPBs->setMinimumSize(isize);
  addExtension(pSCPBs,belowClient,(unsigned long)pSCPBs->minimumSize().height());
  pSCPBs->show();
}

          /*-----------------------------------------------------------------*\
          |    Boolean mouseClicked(IMouseClickEvent& evt)                    |
          |       Method, called by 'IMouseHandler', sets the Info Area  |
          |       text corresponding to the control that's being clicked      |
          |       ( if the currently displayed text is different)             |
          |       Returns: false : to allow proper action by the system.      |
          \*-----------------------------------------------------------------*/
Boolean RPOsWndw :: mouseClicked(IMouseClickEvent& evt)
{
 if (pInfo) {
   unsigned long ulText = RPOSWND_STR_INFO_INACT;
   if (!(evt.mouseAction() == IMouseClickEvent::up))
     switch (evt.window()->id()) {
       case DID_OK:          ulText = RPOSWND_STR_INFO_SAVE;    break;
       case DID_CANCEL:      ulText = RPOSWND_STR_INFO_CANCEL;  break;
       case RPOSWND_PB_HELP: ulText = RPOSWND_STR_INFO_HELP;    break;
       }
   if (ulInfoText != ulText)
     pInfo->setText(ulInfoText = ulText);
   }
  return false;
}

Boolean RPOsWndw::SetSplitBar()
{
 IContainerControl::ObjectCursor CntCrsr(*pCnrCtl);
 CntCrsr.setToFirst();
 if (CntCrsr.isValid()) {
   IContainerObject *pObj = pCnrCtl->objectAt(CntCrsr);
/*  unsigned long ulSplit=pCnrCtl->detailsObjectRectangle(pObj,pCnrCol1).width() +
                         pCnrCtl->detailsObjectRectangle(pObj,pCnrCol2).width() +
                         pCnrCtl->detailsObjectRectangle(pObj,pCnrCol3).width();
   pCnrCtl->setDetailsViewSplit(pCnrCol3,ulSplit-1); jt*/
   return true;
   }
 return false;
}

Boolean RPOsWndw::RPOInContainer(RPO2 &rpo)
{
 IContainerControl::ObjectCursor CntCrsr (*pCnrCtl);
 Boolean fFound= false;
 for (CntCrsr.setToFirst(); !fFound && CntCrsr.isValid(); CntCrsr.setToNext())
   fFound = ((RPOCnrObj *)(pCnrCtl->objectAt(CntCrsr)))->IsEqual(rpo);
 return fFound;
}

Boolean RPOsWndw::HasSelection()
{
 IContainerControl::ObjectCursor CntCrsr (*pCnrCtl);
 forCursor(CntCrsr)
   if (pCnrCtl->isSelected(pCnrCtl->objectAt(CntCrsr)))
      return true;
 return false;
}

          /*-----------------------------------------------------------------*\
          |    void  RPOsWndw :: ProcessList()                                |
          |    F73686  (New)                                                  |
          |       Begins the proceses that creates the .CFR file with the     |
          |       information of the Rpo list, begins Order Forward to send   |
          |       it and, in case it is not available, displays a Save As...  |
          |       dialog to let the user manage this file on his own.         |
          \*-----------------------------------------------------------------*/

void  RPOsWndw :: ProcessList()
{
   //* The RPO information must be saved first in the container.
   PruneList();

   //* Creates the new CFR with the information of the RPO list
   pRPOList->writeListToCFReport(strAASFile, strRpoCfrFile);

   //* Starts execution of the Order Forward application
   LONG lRC = pRPOList->sendRpoCfrToOrderForward(strOrFwdPath, strRpoCfrFile);

   //* In case Order Forward is not available, display a Save As... dialog to let
   //* the user manage this file.
   if ( lRC == -1 )
      pRPOList->saveRpoCfrToDisk(strRpoCfrFile);

}


void  RPOsWndw :: PruneList()
{
 Boolean fDelete;
 RPO2List::Cursor RPOCrsr(*pRPOList);
 RPO2 rpo;
 while (pRPOList->numberOfElements() > pCnrCtl->objectCount()) {
   for (RPOCrsr.setToFirst(), fDelete = false ;
        !fDelete && RPOCrsr.isValid() ;RPOCrsr.setToNext()) {
     rpo = pRPOList->elementAt(RPOCrsr);
     fDelete = !RPOInContainer(rpo);
     }
   if (fDelete)
     pRPOList->removeElement(rpo);
   }
}

          /*-----------------------------------------------------------------*\
          |    void  FillList(Boolean fEmptyFirst)                            |
          |       Protected method to fill the container                      |
          |         with the current RPO List, optionally emptying            |
          |         the container first.                                      |
          |       Returns: nothing                                            |
          \*-----------------------------------------------------------------*/
void  RPOsWndw :: FillList(Boolean fEmptyFirst)
{
 pCnrCtl->setRefreshOff();
 if (fEmptyFirst)
    pCnrCtl->deleteAllObjects();
 RPO2List::Cursor RPOCrsr(*pRPOList);
 forCursor(RPOCrsr)
   pCnrCtl->addObject(new RPOCnrObj(pRPOList->elementAt(RPOCrsr),this));
 iSorted = SORTED_NOT;
 pCnrCtl->setRefreshOn();
 SetSplitBar();
// SizeObjects();
 pCnrCtl->refresh();
}

          /*-----------------------------------------------------------------*\
          |    void  FillList(RPOList * pList)                                |
          |       Public method to fill the current listbox with the          |
          |       given list of RPOs                                          |
          |       Returns: nothing                                            |
          \*-----------------------------------------------------------------*/
void  RPOsWndw :: FillList(RPO2List * pList)
{
 pRPOList = pList;
 FillList();
// if (!pRPOList->numberOfElements())
//   pPBSave->disable();
}

Boolean RPOsWndw :: PressedEnter(IKeyboardEvent & evt)
{
 if ( evt.window() == pPBSave      ||
      evt.window() == pPBCancel    ||
      evt.window() == pPBProcRPO   || //*73686 HECC: 08/28/2002
      evt.window() == pPBHelp ) {
   evt.handle().sendEvent(BM_CLICK);
   return true;
   }
  return false;
}

          /*-----------------------------------------------------------------*\
          |    Boolean command(ICommandEvent & evt)                           |
          |       Method, called by ICommandHandler, processes                |
          |       the following cases:                                        |
          |                                                                   |
          |          RPOSWND_POP_DELETE:                                      |
          |                (from the Popup Menu)                              |
          |             Remove all highlighted entries.                       |
          |                                                                   |
          |          RPOSWND_POP_RESTORE:                                     |
          |                (from the Popup Menu)                              |
          |             Re-load the proper RPO List.                          |
          |                                                                   |
          |             Enable the Save pushbutton when there                 |
          |             are elements in the List since the user               |
          |             might have removed all elements from                  |
          |             the current list whereupon the button                 |
          |             was disabled.                                         |
          |                                                                   |
          |          RPOSWND_POP_SORT_UP:                                     |
          |                (from the Popup Menu)                              |
          |             Sort the proper list in ascending order               |
          |                                                                   |
          |          RPOSWND_POP_SORT_DOWN:                                   |
          |                (from the Popup Menu)                              |
          |             Sort the proper list in descending order              |
          |                                                                   |
          |          RPOSWND_POP_SAVE:                                        |
          |                (from the Popup Menu)                              |
          |             Push the Save button ( see DID_OK)                    |
          |                                                                   |
          |          DID_OK:                                                  |
          |                ( the Save PushButton)                             |
          |             The user is satisfied with the current                |
          |             list: report that fact to our parent.                 |
          |                                                                   |
          |          DID_CANCEL:                                              |
          |                ( the Cancel PushButton)                           |
          |             Restore the original list from the save-one           |
          |             since the user might have removed elements            |
          |             from the current list before canceling;               |
          |             Report the user's intend to our parent.               |
          |                                                                   |
          |          RPOSWND_PB_HELP:                                         |
          |                ( the Help PushButton)                             |
          |             Tell the Help Window to show our Help Panel.          |
          |                                                                   |
          |       Returns: false/true : message (not)/handled                 |
          \*-----------------------------------------------------------------*/
Boolean RPOsWndw :: command(ICommandEvent & evt)
{
 Boolean fProcessed = true;
 switch (evt.commandId()) {
   case RPOSWND_POP_DELETE:
     if (!fAutoGen)
       pCnrCtl->deleteSelectedObjects();
           //jt     else WinAlarm(HWND_DESKTOP,WA_ERROR);
     else MessageBeep(MB_ICONEXCLAMATION);  //jt
     break;
   case RPOSWND_POP_RESTORE: {
/*jt     HPOINTER hptr = WinQueryPointer(HWND_DESKTOP);
     WinSetPointer(HWND_DESKTOP,
                   WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,false));  jt*/
     FillList();
     setFocus();
//jt     WinSetPointer(HWND_DESKTOP,hptr);
     break;
     }
   case RPOSWND_POP_SORT_UP:
   case RPOSWND_POP_SORT_DOWN: {
/*jt     HPOINTER hptr = WinQueryPointer(HWND_DESKTOP);
     WinSetPointer(HWND_DESKTOP,
                   WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,false)); */
     Sort(evt.commandId() == RPOSWND_POP_SORT_UP);
//jt     WinSetPointer(HWND_DESKTOP,hptr);
     break;
     }
   case RPOSWND_POP_SAVE:
     pPBSave->click();
     break;

   case DID_OK:
     PruneList();
#ifndef DRIVER
          NotifyMyClosing(evt.commandId());
#endif
     break;

   //* 73686 HECC: 08/29/2002
   case RPOSWND_PB_SUBMIT:
     ProcessList();
     break;
   //* 73686 HECC: 08/29/2002

   case DID_CANCEL:
     NotifyMyClosing(evt.commandId());
     break;
   case RPOSWND_PB_HELP:
     helpWindow()->show(IHelpWindow::general);
     break;
   default:
     fProcessed = false;
  }
 return fProcessed;
}

          /*-----------------------------------------------------------------*\
          |    Boolean makePopUpMenu(IMenuEvent& menuEvent)                   |
          |       Method, called by the IMenuHandler creates                  |
          |       the Popup Menu as follows:                                  |
          |          En/Dis-able the Delete Menu Item depending               |
          |             on whether any listbox items are selected.            |
          |          En/Dis-able the Save Menu Item depending                 |
          |             on whether there are any listbox items.               |
          |          En/Dis-able the Sort Menu Item depending                 |
          |             on whether there are at least 2 listbox items.        |
          |          Indicate via check marks if the shown list               |
          |             is currently sorted in the proper order.              |
          |       Note: Menu placement is discussed in the code.              |
          |       Returns: true : message handled                             |
          \*-----------------------------------------------------------------*/
Boolean RPOsWndw :: makePopUpMenu(IMenuEvent& menuEvent)
{
 IPopUpMenu * popUp = new IPopUpMenu(RPOSWND_POPUP, menuEvent.window());
//  IPopUpMenu * popUp = new IPopUpMenu(RPOSWND_POPUP, this);
//  popUp->setFont(IFont(&MCCControls));
 popUp->setAutoDeleteObject();
 if (fAutoGen) {
   popUp->disableItem(RPOSWND_POP_DELETE);
   popUp->disableItem(RPOSWND_POP_RESTORE);
   }
 else if (!HasSelection())
         popUp->disableItem(RPOSWND_POP_DELETE);

#if 0
 if (pCnrCtl->objectCount() > 1)
   if (iSorted == SORTED_UP) {
     popUp->checkItem(RPOSWND_POP_SORT_UP);
     popUp->disableItem(RPOSWND_POP_SORT_UP);
     }
   else {
     if (iSorted == SORTED_DOWN) {
       popUp->checkItem(RPOSWND_POP_SORT_DOWN);
       popUp->disableItem(RPOSWND_POP_SORT_DOWN);
       }
   }
 else popUp->disableItem(RPOSWND_POP_SORT);
#endif
              /*-----------------------------------------------*\
              | The message might have been generated as a      |
              | result of a mouseclick or a keystroke           |
              | combination, in which case the mouse might be   |
              | outside the container and it would              |
              | inappropriate to pop the menu at the mouse      |
              | position. In stead, the bottom-left position of |
              | the container is used. Reminder: the mouse      |
              | position and the menu position are in relation  |
              | to the Container!                               |
              \*-----------------------------------------------*/
 IPoint pt(menuEvent.mousePosition());
 IRectangle rct(pCnrCtl->rect());
 if (pt.x() >= 0 && pt.x() <= rct.width() &&
     pt.y() >= 0 && pt.y() <= rct.height())
    popUp->show(pt);
 else  popUp->show(IPoint());       // in relation to container
 return true;
}


          /*-----------------------------------------------------------------*\
          |    Boolean menuSelected(IControlEvent& evt)                       |
          |       Method, called by the IMenuHandler, is                      |
          |       used to set the appropriate Info Area text                  |
          |       for the  Popup Menu Item currently having                   |
          |       the focus.                                                  |
          |       Returns: false : message (not)handled                       |
          |                        (allow the sytem to paint                  |
          |                         the Menu Item's highlight                 |
          \*-----------------------------------------------------------------*/
Boolean RPOsWndw :: menuSelected(IMenuEvent& evt)
{
 if (pInfo) {
   switch (evt.menuItemId()) {
     case RPOSWND_POP_DELETE:    pInfo->setText(RPOSWND_STR_INFO_DELETE);    break;
     case RPOSWND_POP_RESTORE:   pInfo->setText(RPOSWND_STR_INFO_RESTORE);   break;
     case RPOSWND_POP_SORT:      pInfo->setText(RPOSWND_STR_INFO_SORT);      break;
     case RPOSWND_POP_SORT_UP:   pInfo->setText(RPOSWND_STR_INFO_SORT_UP);   break;
     case RPOSWND_POP_SORT_DOWN: pInfo->setText(RPOSWND_STR_INFO_SORT_DOWN); break;
     }
   }
 return false;
}

          /*------------------------------------------------------------------*\
          |    Boolean menuEnded(IMenuEvent& evt)                              |
          |       Method, called by the IMenuHandler, is                       |
          |       used to set the default Info Area text                       |
          |       when the Popup Menu closes.                                  |
          |       Make sure the overridden method is called back, since the    |
          |       container has to re-establish the list of objects  (deletion |
          |       could have taken place) and the app will blow big-time.      |
          \*------------------------------------------------------------------*/
Boolean RPOsWndw :: menuEnded(IMenuEvent& evt)
{
 if (pInfo && evt.menuItemId() == RPOSWND_POPUP)
   pInfo->setText(pInfo->inactiveText());
 return ICnrMenuHandler::menuEnded(evt);
}

Boolean RPOsWndw :: keysHelpId(IEvent& evt)
{
//evt.setResult(1000);
  return true;
}

int RPOsWndwUpSort :: isEqual (IContainerObject* object1,
                              IContainerObject* object2,
                              ICnrCtl * container) const
{
 const RPOCnrObj * pRPO1 = (RPOCnrObj *)object1
               , * pRPO2 = (RPOCnrObj *)object2;
 return ( (*pRPO1 < *pRPO2)? -1 : ( (*pRPO1 == *pRPO2)? 0 : 1 ));
}

int RPOsWndwDownSort :: isEqual (IContainerObject* object1,
                                IContainerObject* object2,
                                ICnrCtl * container) const
{
 const RPOCnrObj * pRPO1 = (RPOCnrObj *)object1
               , * pRPO2 = (RPOCnrObj *)object2;
 return ( (*pRPO1 < *pRPO2)? 1 : ( (*pRPO1 == *pRPO2)? 0 : -1 ));
}

void RPOsWndw :: Sort(Boolean fUp)
{
 ICnrCtl::CompareFn* comparison = 0;
 iSorted = fUp? SORTED_UP : SORTED_DOWN;
 if (fUp)
   comparison = new RPOsWndwUpSort;
 else comparison = new RPOsWndwDownSort;
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
 RPO2 * ApRPO[25];
 RPO2List *pRPOList = new RPO2List;
 ApRPO[0] = new RPO2(1,"Mach1","Mod1","Ser1","Feat1","Excptn1", 1234,'A');
 ApRPO[1] = new RPO2(2,"Mach1","Mod1","Ser1","Feat1","Excptn2", 2234,'A');
 ApRPO[2] = new RPO2(3,"Mach1","Mod1","Ser1","Feat1","Excptn3", 3234,'A');
 ApRPO[3] = new RPO2(4,"Mach1","Mod1","Ser1","Feat1","Excptn4", 4234,'A');
 ApRPO[4] = new RPO2(5,"Mach1","Mod1","Ser1","Feat1","Excptn5", 5234,'A');
 ApRPO[5] = new RPO2(6,"Mach1","Mod1","Ser1","Feat1","Excptn6", 6234,'A');
 ApRPO[6] = new RPO2(7,"Mach1","Mod1","Ser1","Feat1","Excptn7", 7234,'A');
 ApRPO[7] = new RPO2(8,"Mach1","Mod1","Ser1","Feat1","Excptn8", 8234,'A');
 ApRPO[8] = new RPO2(9,"Mach1","Mod1","Ser1","Feat1","Excptn9 make it longer", 9234,'A');
 ApRPO[9] = new RPO2(10,"Mach1","Mod1","Ser1","Feat1","Excptn10", 133,'A');
 ApRPO[10]= new RPO2(11,"Mach1","Mod1","Ser1","Feat1","Excptn11", 143,'A');
 int i;
 for (i=0; i < 11 ; i++)
   pRPOList->add(*ApRPO[i]);
 pRPOList->removeAtPosition(IPosition(5));

 pRPOList->SortUp();


 RPO2List *pRPOList2 = new RPO2List;
 pRPOList2->add(RPO2(12,"Mach2","Mod2","Ser2","Feat2","Exception1 and some more", 9234,'A'));
 pRPOList2->add(RPO2(13,"Mach2","Mod2","Ser2","Feat2","Exception2 and some more", 8234,'A'));
 pRPOList2->add(RPO2(14,"Mach2","Mod2","Ser2","Feat2","Exception3 and some more", 7234,'A'));
 pRPOList2->add(RPO2(15,"Mach2","Mod2","Ser2","Feat2","Exception4 and some more4", 6234,'A'));
 pRPOList2->add(RPO2(16,"Mach2","Mod2","Ser2","Feat2","Exception5 and some more", 5234,'A'));
 pRPOList2->add(RPO2(17,"Mach2","Mod2","Ser2","Feat2","Exception6 and some more", 4234,'A'));
 pRPOList2->add(RPO2(18,"Mach2","Mod2","Ser2","Feat2","Exception7 and some more", 3234,'A'));
 pRPOList2->add(RPO2(19,"Mach2","Mod2","Ser2","Feat2","Exception8 and some more", 2234,'A'));
 pRPOList2->add(RPO2(20,"Mach2","Mod2","Ser2","Feat2","Exception9 and some more", 1234,'A'));
 pRPOList2->add(RPO2(21,"Mach2","Mod2","Ser2","Feat2","Exception11 and some more", 143,'A'));
 pRPOList2->add(RPO2(22,"Mach2","Mod2","Ser2","Feat2","Exception10 and some more", 133,'A'));

// RPOsWndw mainWindow (WND_RPO,pRPOList);
 RPOsWndw *pMainWindow = new RPOsWndw(WND_RPO,pRPOList);

 pRPOList2->add(RPO2(23,"Mach2","Mod2","Ser2","Feat2","aaaaaaaaaaaa a long-winded addition", 333,'A'));
// pRPOList2->SortDown();
 pRPOList2->removeElement(RPO2(14,"Mach2","Mod2","Ser2","Feat2","Exception3 and some more", 7234,'A'));
 pMainWindow->FillList(pRPOList2);
// pMainWindow->FillList(pRPOList);
// RPOsWndw *pMainWindow = new RPOsWndw(WND_RPO,pRPOList2);

 pMainWindow->Sort();
//jt  pMainWindow->SizeToMaximumTextWidth();
 pMainWindow->Center();
 pMainWindow->show();
 IApplication::current().run();
 delete pMainWindow;
// delete pRPOList;
// delete pRPOList2;
 for (i=0; i < 11 ; i++)
   delete ApRPO[i];
 delete pRPOList2;
}
#endif


