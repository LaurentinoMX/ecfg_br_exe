//=======================================================================
// FILE NAME        AF9ORDRS.CPP
//
// CLASS NAME       OrdersNotebook
//
// DESCRIPTION      This class presents the Orders notebook in the Apply
//                  Orders CPF.
//=======================================================================
// 11/11/99 flw - Added Destructor to handle General Protection fault error 44753
//
//=======================================================================
// Copyright (c) International Business Machines Corp. 1996
// All rights reserved
//=======================================================================

#define INCL_WINDIALOGS                 // WinAlarm
#include <os2.h>

#include <icnrcol.hpp>                  // IContainerColumn
#include <icnrctl.hpp>                  // IContainerControl
#include <icnrhdr.hpp>                  // ICnrHandler
#include <icnrmhdr.hpp>                 // ICnrMenuHandler
#include <ifocshdr.hpp>                 // IFocusHandler
#include <ifont.hpp>                    // IFont
#include <iframhdr.hpp>                 // IFrameHandler
#include <ihelp.hpp>                    // IHelpWindow
#include <imenubar.hpp>                 // IMenuBar
#include <imsgbox.hpp>                  // IMessageBox
#include <inotebk.hpp>                  // INotebook
#include <ipagehdr.hpp>                 // IPageHandler
#include <ipopmenu.hpp>                 // IPopUpMenu
#include <iprofile.hpp>                 // IProfile
#include <ireslib.hpp>                  // IResourceLibrary/IResourceId
#include <isizehdr.hpp>                 // IResizeHandler
#include <isubmenu.hpp>                 // ISubMenu
#include <isysmenu.hpp>                 // ISystemMenu
#include <ithread.hpp>                  // IThread
#include <ititle.hpp>                   // ITitle

#include "af9ordrs.hpp"                 // OrdersNotebook
#include "af9main.hpp"                  // MainWindow
#include "af9systm.hpp"                 // SystemsNotebook
#include "af9seldt.hpp"                 // SelectDateDialog
#include "afcprods.hpp"                 // Product, Mes
#include "afcbkend.hpp"                 // BackEnd
#include "af9profl.hpp"                 // Profile
#include "af9apply.h"                   // Resource ids

//=======================================================================
// FUNCTION NAME    OrdersNotebook
//
// DESCRIPTION      Construct an instance of the OrdersNotebook class.
//=======================================================================

OrdersNotebook :: OrdersNotebook(MainWindow* pOwner)
                : IFrameWindow(IResourceId(IDW_Orders), NULL,
                               NULL, IRectangle(),
                               IFrameWindow::defaultStyle() |
                               IFrameWindow::noMoveWithOwner)
   {

   // Capture the incoming parameters.

   pMain = pOwner;
   pSelectDate = (SelectDateDialog*)NULL;

   // Attach the appropriate icon.

   setIcon(IResourceId(IDI_ApplyOrders));

   // Disable close from the system menu.

   //edw ISystemMenu(this).disableItem(ISystemMenu::idClose);

   // Create the menu bar.

   pMenu = new IMenuBar(IDW_Orders, this);

   // Attach the help window.

   pHelp = pMain->helpWindow();
   pHelp->setAssociatedWindow(this);

   // Capture the user profile.

   pProfile = pMain->pProfile;

   // Create the client notebook and add the appropriate pages.

   //INotebook::setDefaultStyle(INotebook::defaultStyle() | INotebook::defaultStyle() |
                             // INotebook::pmCompatible);//jt-add-tap at Bottom,4/29/97

   pNotebook = new INotebook(IDC_NB_Orders, this, this);
   setClient(pNotebook);
   pNotebook->hide();
   buildNotebook();
   pNotebook->show();

   // Make this window visible to the menu handler.


   menuHandler.pWindow = this;

   // Add handlers for command, resize, frame, container, menu, focus, and
   // page messages.

   ICommandHandler::handleEventsFor(this);
   ICommandHandler::handleEventsFor(pNotebook);
   IResizeHandler::handleEventsFor(this);
   IFrameHandler::handleEventsFor(this);
   IPageHandler::handleEventsFor(pNotebook);
   for ( USHORT usIndex = 0; usIndex < 2; usIndex++ )
      {
      ICnrHandler::handleEventsFor(pcnrOrders[usIndex]);
      ICnrMenuHandler::handleEventsFor(pcnrOrders[usIndex]);
      IFocusHandler::handleEventsFor(pcnrOrders[usIndex]);
      }
   menuHandler.handleEventsFor(this);
   }

//=======================================================================
// FUNCTION NAME    command
//
// DESCRIPTION      Handle command messages.
//=======================================================================

Boolean OrdersNotebook :: command(ICommandEvent& cmdEvt)
   {
   switch ( cmdEvt.commandId() )
      {

      case IDMI_EditUndo:
         if ( pMain->pFile->changes() > 1 )
            {
            IMessageBox msg(this);
            IMessageBox::Style style = IMessageBox::okCancelButton |
                                       IMessageBox::defButton2 |
                                       IMessageBox::warningIcon |
                                       IMessageBox::moveable |
						IMessageBox::systemModal;
            msg.setTitle(IResourceId(IDS_Undo_Title));
            IMessageBox::Response response = msg.show(IResourceId(IDS_Undo_Text),
                                                      style, IDH_Undo);
            if ( response != IMessageBox::ok )
               break;
            }
         pMain->pBackEnd->CloseFile();
         pMain->pcnrFiles->setCursor(pMain->pFile);
         pMain->pFile = (CfReportFileStats*)NULL;
         pMain->postEvent(IWindow::command, IDMI_SelectedOpen);
         break;

      case IDMI_EditSelectDate:
         openSelectDate();
         break;

      case IDMI_EditSelectAll:
         selectAll(true);
         break;

      case IDMI_EditDeselectAll:
         selectAll(false);
         break;

      case IDMI_ViewSortA:
         sSort = abs(sSort);
         sortOrders();
         break;

      case IDMI_ViewSortD:
         sSort = -abs(sSort);
         sortOrders();
         break;

      case IDMI_SelectedApply:
         bPopup = false;
         applyOrders(true);
         break;

      case IDMI_PopupApply:
         bPopup = true;
         applyOrders(true);
         break;

      case IDMI_SelectedRemove:
         bPopup = false;
         applyOrders(false);
         break;

      case IDMI_PopupRemove:
         bPopup = true;
         applyOrders(false);
         break;

      case IDMI_SelectedDetail:
         bPopup = false;
         openDetail();
         break;

      case IDMI_PopupDetail:
         bPopup = true;
         openDetail();
         break;

      case IDMI_HelpUsing:
         pHelp->show(IHelpWindow::using);
         break;

      case IDMI_HelpProduct:
         openProductInfo();
         break;

      case UM_SelectDate:
         selectDate();
         break;

      case UM_SwitchLocale:
         reformat();
         break;

      default:
         return false;
      }
   return true;
   }

//=======================================================================
// FUNCTION NAME   closed
//
// DESCRIPTION     Handles system close events.
//=======================================================================
//edw12/18/97
Boolean OrdersNotebook :: closed(IFrameEvent& cmdEvt)
   {
   // Hide this window instead of closing it.
   //edw
   //if ( hasFocus() ||
   //     pNotebook->hasFocus() ||
   //     pcnrOrders[ORDERSNEW]->hasFocus() ||
   //     pcnrOrders[ORDERSMES]->hasFocus() )
   //   {
   //   //WinAlarm(HWND_DESKTOP, WA_WARNING);
   //   MessageBeep(MB_ICONEXCLAMATION); // --eds
   //   return true;
   //   }
   if ( pSelectDate )
      pSelectDate->sendEvent(IWindow::command, IDC_PB_Cancel);
   if ( pMain->hasFocus() )
      return false;
   hide();
   removeFromWindowList();
   pMain->setFocus();
   return true;
   }

//=======================================================================
// FUNCTION NAME    select
//
// DESCRIPTION      Handle notebook Select messages.
//=======================================================================

Boolean OrdersNotebook :: select(IPageSelectEvent& pagEvt)
   {

   // Set the keyboard focus to the new dialog.

   IPageHandle pgHandle = pagEvt.pageHandle();
   pNotebook->window(pgHandle)->setFocus();
   return false;
   }

//=======================================================================
// FUNCTION NAME    help
//
// DESCRIPTION      Handle page help events.
//=======================================================================

Boolean OrdersNotebook :: help(IPageHelpEvent& helpEvt)
   {
   pHelp->show(IResourceId(IDW_Orders));
   return true;
   }

//=======================================================================
// FUNCTION NAME    selectedChanged
//
// DESCRIPTION      Handle container select events.
//=======================================================================

Boolean OrdersNotebook :: selectedChanged(ICnrEmphasisEvent& cnrEvt)
   {
   if ( pcnrOrders[usFocus]->isSelected(cnrEvt.object()) )
      ulSelected[usFocus]++;
   else
      ulSelected[usFocus]--;
   return false;
   }

//=======================================================================
// FUNCTION NAME    enter
//
// DESCRIPTION      Handle container enter events.
//=======================================================================

Boolean OrdersNotebook :: enter(ICnrEnterEvent& enterEvt)
   {
   if ( enterEvt.object() )
      {
      bPopup = true;
      pcnrOrders[usFocus]->removeSelected(enterEvt.object());
      openDetail();
      }
   return false;
   }

//=======================================================================
// FUNCTION NAME    help
//
// DESCRIPTION      Handle container help events.
//=======================================================================

Boolean OrdersNotebook :: help(ICnrHelpEvent& helpEvt)
   {
   pHelp->show(IResourceId(IDW_Orders));
   return true;
   }

//=======================================================================
// FUNCTION NAME    makePopupMenu
//
// DESCRIPTION      Handle object select events.
//=======================================================================

Boolean OrdersNotebook :: makePopUpMenu(IMenuEvent& menuEvt)
   {
   IContainerObject* pObject = popupMenuObject();
   if ( !pObject )
      {
      //WinAlarm(HWND_DESKTOP, WA_WARNING);
      MessageBeep(MB_ICONEXCLAMATION); // --eds
      return false;
      }
   IPoint pt = menuEvt.mousePosition();
   if ( pcnrOrders[usFocus]->objectUnderPoint(pt) != pObject )
      pt = pcnrOrders[usFocus]->detailsObjectRectangle(pObject).center();
   IPopUpMenu* pPopUp = new IPopUpMenu(IDM_Popup, menuEvt.window());
   pPopUp->deleteItem(IDMI_PopupOpen);
   pPopUp->deleteItem(IDMI_PopupSave);
   pPopUp->deleteItem(IDMI_PopupSaveAs);
   //pPopUp->deleteItem(IDMI_PopupGenerate);  //47705 - flw
   pPopUp->deleteItem(IDMI_PopupRename);
   pPopUp->deleteItem(IDMI_PopupDelete);
   pPopUp->deleteItem(IDMI_PopupAssign);
   if ( pcnrOrders[usFocus]->isSelected(pObject) )
      {
      if ( !selected(true) )
         pPopUp->deleteItem(IDMI_PopupRemove);
      if ( !selected(false) )
         pPopUp->deleteItem(IDMI_PopupApply);
      }
   else
      {
      if ( usFocus == ORDERSNEW )
         {
         Product* pProduct = (Product*)pObject;
         if ( pProduct->applied() )
            pPopUp->deleteItem(IDMI_PopupApply);
         else
            pPopUp->deleteItem(IDMI_PopupRemove);
         }
      else
         {
         Mes* pMES = (Mes*)pObject;
         if ( pMES->applied() )
            pPopUp->deleteItem(IDMI_PopupApply);
         else
            pPopUp->deleteItem(IDMI_PopupRemove);
         }
      }
   pPopUp->setAutoDeleteObject();

   pPopUp->show(pt);
   pcnrOrders[usFocus]->setCursor(pObject);
   return false;
   }

//=======================================================================
// FUNCTION NAME    windowResize
//
// DESCRIPTION      Handle window resize events.
//=======================================================================

Boolean OrdersNotebook :: windowResize(IResizeEvent& sizeEvt)
   {
   if ( isVisible() && !isMinimized() )
      {
      pProfile->position[WINDOWORDERS] = position();
      pProfile->size[WINDOWORDERS] = size();
      if ( pMain->isMinimized() )
         {
         pMain->restore();
         setFocus();
         }
      }
   return false;
   }

//=======================================================================
// FUNCTION NAME    gotFocus
//
// DESCRIPTION      Handle focus events.
//=======================================================================

Boolean OrdersNotebook :: gotFocus(IControlEvent& ctlEvt)
   {
   ULONG ulId = ctlEvt.controlId();
   for ( USHORT usIndex = 0; usIndex < 2; usIndex ++ )
      if ( ulId == (IDC_CN_New + usIndex) )
         usFocus = usIndex;
   return false;
   }

//=======================================================================
// FUNCTION NAME    activated
//
// DESCRIPTION      Handle frame activation events.
//=======================================================================

Boolean OrdersNotebook :: activated(IFrameEvent& frmEvt)
   {
   pMain->activeWindow = this;
   return false;
   }

//=======================================================================
// FUNCTION NAME    openDetail
//
// DESCRIPTION      Open the detail dialog.
//=======================================================================

void OrdersNotebook :: openDetail()
   {
   HCURSOR hCur;                                  // EDS- ADDED 2-22-97
   hCur=SetCursor(LoadCursor(NULL, IDC_WAIT));    //
   ShowCursor(TRUE);                              //
   if ( usFocus == ORDERSNEW )
      {
      if ( bPopup )
         {
         Product* pProduct = (Product*)pcnrOrders[ORDERSNEW]->cursoredObject();
         if ( !pcnrOrders[ORDERSNEW]->isSelected(pProduct) )
            pMain->openDetail(pProduct);
         else
            bPopup = false;
         }
      if ( !bPopup )
         {
         IContainerControl::ObjectCursor crs(*pcnrOrders[ORDERSNEW],
                                              IContainerObject::selected);
         Product* pLastProduct = (Product*)crs.last();    // EDS-ADDED 2-22-97
         for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
            {
            Product* pProduct = (Product*)crs.current();
            if (pLastProduct == pProduct)                 // EDS-ADDED 2-22-97
               {
                 pMain->openDetail(pProduct,true);
               }
            else
               {
                 pMain->openDetail(pProduct,false);
               }
            }
         }
      }
   else
      {
      if ( bPopup )
         {
         Mes* pMES = (Mes*)pcnrOrders[ORDERSMES]->cursoredObject();
         if ( !pcnrOrders[ORDERSMES]->isSelected(pMES) )
            pMain->openDetail(pMES);
         else
            bPopup = false;
         }
      if ( !bPopup )
         {
         IContainerControl::ObjectCursor crs(*pcnrOrders[ORDERSMES],
                                             IContainerObject::selected);
         Mes* pLastMES = (Mes*)crs.last();              // EDS-ADDED 2-22-97
         for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
            {
            Mes* pMES = (Mes*)crs.current();
            if (pLastMES == pMES)                       // EDS-ADDED 2-22-97
               {
                 pMain->openDetail(pMES,true);
               }
            else
               {
                 pMain->openDetail(pMES,false);
               }
            }
         }
      }
   ShowCursor(FALSE);                             // EDS-ADDED 2-22-97
   SetCursor(hCur);                               //
   }

//=======================================================================
// FUNCTION NAME    openSelectDate
//
// DESCRIPTION      Open the Select by Date dialog.
//=======================================================================

void OrdersNotebook :: openSelectDate()
   {
   if ( !pSelectDate )
      {
      pSelectDate = new SelectDateDialog(this);
      }
   pSelectDate->show();
   pSelectDate->addToWindowList();
   pSelectDate->setFocus();
   }

//=======================================================================
// FUNCTION NAME    openProductInfo
//
// DESCRIPTION      Open the product information dialog.
//=======================================================================

void OrdersNotebook :: openProductInfo()
   {
   IMessageBox::Style style = IMessageBox::okButton |
                              IMessageBox::informationIcon |
                              IMessageBox::moveable |
                              IMessageBox::systemModal ;   //ZOR 060900 def 51688
   IMessageBox msg(this);
   msg.setTitle(IResourceId(IDS_Product_Title));
   IString strText = LOADSTRING(IDS_Product_Text);
   strText.change(STR_Substitute0, STR_Version);
   msg.show(strText, style);
   }

//=======================================================================
// FUNCTION NAME    clearNotebook
//
// DESCRIPTION      Remove all data from the notebook.
//=======================================================================

void OrdersNotebook :: clearNotebook()
   {
   pcnrOrders[ORDERSNEW]->setRefreshOff();
   pcnrOrders[ORDERSMES]->setRefreshOff();
   pcnrOrders[ORDERSNEW]->deleteAllObjects();
   pcnrOrders[ORDERSMES]->deleteAllObjects();
   if ( pSelectDate )
      pSelectDate->sendEvent(IWindow::command, IDC_PB_Cancel);
   }

//=======================================================================
// FUNCTION NAME    buildNotebook
//
// DESCRIPTION      Add the appropriate pages to the notebook.
//=======================================================================

void OrdersNotebook :: buildNotebook()
    {
   INotebook::PageSettings::Attribute attributeMajor =
       INotebook::PageSettings::majorTab |
       INotebook::PageSettings::autoPageSize |
       INotebook::PageSettings::statusTextOn;
   INotebook::PageSettings pgMajor = INotebook::PageSettings(attributeMajor);

   IString sText = LOADSTRING(IDS_NewOrders);
   pgMajor.setTabText(sText);
   pcnrOrders[ORDERSNEW] = new IContainerControl(IDC_CN_New, pNotebook, pNotebook, IRectangle(),
                                         IContainerControl::defaultStyle() | IContainerControl::pmCompatible);//jt-chg
   pcolCPUNew = new IContainerColumn(IContainerColumn::isIcon);
   pcolCPUNew->hideSeparators();
   createColumn(&pcolAppliedNew, Product::offsetApplied(),
                IContainerColumn::centered);
   createColumn(&pcolProductNew, Product::offsetTypeModel(),
                IContainerColumn::left);
   createColumn(&pcolOrderNew, Product::offsetOrder(),
                IContainerColumn::left);
   createColumn(&pcolDateNew, Product::offsetDate(),
                IContainerColumn::left);
   setHeadingText(pcolAppliedNew, IDS_Applied1);
   setHeadingText(pcolProductNew, IDS_Product1);
   setHeadingText(pcolOrderNew, IDS_Order1);
   setHeadingText(pcolDateNew, IDS_DateCRAD1);
   pcnrOrders[ORDERSNEW]->addColumn(pcolCPUNew);
   pcnrOrders[ORDERSNEW]->addColumn(pcolAppliedNew);
   pcnrOrders[ORDERSNEW]->addColumn(pcolProductNew);
   pcnrOrders[ORDERSNEW]->addColumn(pcolOrderNew);
   pcnrOrders[ORDERSNEW]->addColumn(pcolDateNew);
   pcolDateNew->hideSeparators(IContainerColumn::verticalSeparator);
   pcnrOrders[ORDERSNEW]->setTitle(IResourceId(IDS_NewOrders));
   pcnrOrders[ORDERSNEW]->showTitle();
   pcnrOrders[ORDERSNEW]->showDetailsView();
   pcnrOrders[ORDERSNEW]->setMultipleSelection();
   pcnrOrders[ORDERSNEW]->setIconSize(ISize(16, 16));
   pNotebook->addLastPage(pgMajor, pcnrOrders[ORDERSNEW]);

   sText = LOADSTRING(IDS_MESOrders);
   pgMajor.setTabText(sText);
   pcnrOrders[ORDERSMES] = new IContainerControl(IDC_CN_MES, pNotebook, pNotebook, IRectangle(),
                                  IContainerControl::defaultStyle() | IContainerControl::pmCompatible);//jt-chg
   createColumn(&pcolAppliedMES, Mes::offsetApplied(),
                IContainerColumn::centered);
   createColumn(&pcolBaseMES, Mes::offsetBaseProduct(),
                IContainerColumn::left);
   createColumn(&pcolSerialMES, Mes::offsetSerial(),
                IContainerColumn::left);
   createColumn(&pcolTargetMES, Mes::offsetTargetProduct(),
                IContainerColumn::left);
   createColumn(&pcolOrderMES, Mes::offsetOrder(),
                IContainerColumn::left);
   createColumn(&pcolDateMES, Mes::offsetDate(),
                IContainerColumn::left);
   createColumn(&pcolGroupMES, Mes::offsetGroupSequence(),
                IContainerColumn::centered);
   setHeadingText(pcolAppliedMES, IDS_Applied1);
   setHeadingText(pcolBaseMES, IDS_Base1);
   setHeadingText(pcolSerialMES, IDS_Serial1);
   setHeadingText(pcolTargetMES, IDS_Target1);
   setHeadingText(pcolOrderMES, IDS_Order1);
   setHeadingText(pcolDateMES, IDS_DateCRAD1);
   setHeadingText(pcolGroupMES, IDS_Group1);
   pcnrOrders[ORDERSMES]->addColumn(pcolAppliedMES);
   pcnrOrders[ORDERSMES]->addColumn(pcolBaseMES);
   pcnrOrders[ORDERSMES]->addColumn(pcolSerialMES);
   pcnrOrders[ORDERSMES]->addColumn(pcolTargetMES);
   pcnrOrders[ORDERSMES]->addColumn(pcolOrderMES);
   pcnrOrders[ORDERSMES]->addColumn(pcolDateMES);
   pcnrOrders[ORDERSMES]->addColumn(pcolGroupMES);
   pcolGroupMES->hideSeparators(IContainerColumn::verticalSeparator);
   pcnrOrders[ORDERSMES]->setTitle(IResourceId(IDS_MESOrders));
   pcnrOrders[ORDERSMES]->showTitle();
   pcnrOrders[ORDERSMES]->showDetailsView();
   pcnrOrders[ORDERSMES]->setMultipleSelection();
   pcnrOrders[ORDERSMES]->setIconSize(ISize(16, 16));
   pNotebook->addLastPage(pgMajor, pcnrOrders[ORDERSMES]);

   pNotebook->setBinding(INotebook::spiral);
   pNotebook->setMinorTabSize(ISize(0,0));
   pNotebook->setTabShape(INotebook::rounded);
   pNotebook->setOrientation(INotebook::backpagesRightTabsBottom);
   adjustTabWidth();
   }

//=======================================================================
// FUNCTION NAME    initializeNotebook
//
// DESCRIPTION      Return to the default state after loading.
//=======================================================================

void OrdersNotebook :: initializeNotebook()
   {
   sSort = pProfile->sSortOrders;
   sortOrders();

   ulSelected[ORDERSNEW] = ulSelected[ORDERSMES] = 0;

//jt   pNotebook->turnToPage(pNotebook->firstPage());
   pcnrOrders[ORDERSNEW]->setFocus();
   pcnrOrders[ORDERSNEW]->setRefreshOn();
   pcnrOrders[ORDERSMES]->setRefreshOn();
   pcnrOrders[ORDERSNEW]->refresh(true);
   pcnrOrders[ORDERSMES]->refresh(true);
   }

//=======================================================================
// FUNCTION NAME    applyOrders
//
// DESCRIPTION      Apply or remove selected orders.
//=======================================================================

void OrdersNotebook :: applyOrders(Boolean bApply)
   {
   if ( usFocus == ORDERSNEW )
      {
      if ( bPopup )
         {
         Product* pProduct = (Product*)pcnrOrders[ORDERSNEW]->cursoredObject();
         if ( !pcnrOrders[ORDERSNEW]->isSelected(pProduct) )
            apply(pProduct, bApply);
         else
            bPopup = false;
         }
      if ( !bPopup )
         {
         IContainerControl::ObjectCursor crs(*pcnrOrders[ORDERSNEW]);
         for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
            {
            Product* pProduct = (Product*)crs.current();
            if ( pcnrOrders[ORDERSNEW]->isSelected(pProduct) )
               apply(pProduct, bApply);
            }
         }
      }
   else
      {
      Boolean bMore = false;
      if ( bPopup )
         {
         Mes* pMES = (Mes*)pcnrOrders[ORDERSMES]->cursoredObject();
         if ( !pcnrOrders[ORDERSMES]->isSelected(pMES) )
            {
            if ( bApply )
               pMain->pBackEnd->ApplyRemoveMes('A', pMES, bMore);
            else
               pMain->pBackEnd->ApplyRemoveMes('R', pMES, bMore);
            apply(pMES, bApply);
            }
         else
            bPopup = false;
         }
      if ( !bPopup )
         {
         if ( bApply )
            pMain->pBackEnd->ApplyRemoveMes('A', bMore);
         else
            pMain->pBackEnd->ApplyRemoveMes('R', bMore);
         IContainerControl::ObjectCursor crs(*pcnrOrders[ORDERSMES]);
         for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
            {
            Mes* pMES = (Mes*)crs.current();
            if ( pcnrOrders[ORDERSMES]->isSelected(pMES) )
               apply(pMES, bApply);
            }
         }
      if ( bMore )
         {
         IMessageBox msg(this);
         IMessageBox::Style style = IMessageBox::okButton |
                                    IMessageBox::informationIcon |
                                    IMessageBox::moveable |
						IMessageBox::systemModal;
         msg.setTitle(IResourceId(IDS_AutoApply_Title));
         msg.show(IResourceId(IDS_AutoApply_Text), style, IDH_AutoApply);
         }
      }
   pMain->pSystems->sortProducts(SYSTEMSUNASSIGNED);
   pcnrOrders[usFocus]->refresh(true);
   pMain->pFile->setChanges(pMain->pFile->changes()+1);
   }

//=======================================================================
// FUNCTION NAME    apply
//
// DESCRIPTION      Apply or remove a new order.
//=======================================================================

void OrdersNotebook :: apply(Product* pProduct, Boolean bApply)
   {
   pcnrOrders[ORDERSNEW]->removeSelected(pProduct);
   if ( pProduct->applied() != bApply )
      {
      pProduct->setApplied(bApply);
      IContainerControl** pcnrProducts = pMain->pSystems->pcnrProducts;
      if ( bApply )
         {
         if ( pProduct->category() != AbstractProduct::CPU )
            {
            pcnrProducts[ORDERSNEW]->addObject(pProduct);
            pcnrProducts[ORDERSNEW]->refresh(true);
            }
         else
            {
            Boolean bApplied = false;
            for ( USHORT usIndex = 1; usIndex < pMain->pSystems->usSystems; usIndex++ )
               if ( !pcnrProducts[usIndex]->objectCount() )
                  {
                  pcnrProducts[usIndex]->addObject(pProduct);
                  pcnrProducts[usIndex]->refresh(true);
                  bApplied = true;
                  break;
                  }
            if ( !bApplied )
               {
               IString strText = LOADSTRING(IDS_AppliedCPU);
               pMain->pSystems->addPage(pMain->pSystems->usSystems, strText);
               pcnrProducts = pMain->pSystems->pcnrProducts;
               pcnrProducts[pMain->pSystems->usSystems-1]->addObject(pProduct);
               pcnrProducts[pMain->pSystems->usSystems-1]->refresh(true);
               }
            }
         }
      else
         for ( USHORT usIndex = 0; usIndex < pMain->pSystems->usSystems; usIndex++ )
            if ( pcnrProducts[usIndex]->containsObject(pProduct) )
               {
               pcnrProducts[usIndex]->removeObject(pProduct);
               pcnrProducts[usIndex]->refresh(true);
               break;
               }
      }
   }

//=======================================================================
// FUNCTION NAME    apply
//
// DESCRIPTION      Apply or remove an MES.
//=======================================================================

void OrdersNotebook :: apply(Mes* pMES, Boolean bApply)
   {
   pcnrOrders[ORDERSMES]->removeSelected(pMES);
   if ( pMES->applied() != bApply )
      pMES->setApplied(bApply);
   if ( pMES->pBaseProduct )
      {
      IContainerControl** pcnrProducts = pMain->pSystems->pcnrProducts;
      for ( USHORT usIndex = 0; usIndex < pMain->pSystems->usSystems; usIndex++ )
         if ( pcnrProducts[usIndex]->containsObject(pMES->pBaseProduct) )
            pcnrProducts[usIndex]->refresh(true);
      if ( pMES->pBaseProduct->detail() )
         pMain->reloadDetail(pMES->pBaseProduct);
      }
   }

//=======================================================================
// FUNCTION NAME    sortOrders
//
// DESCRIPTION      Sort the order list by product, order, or date.
//=======================================================================

void OrdersNotebook :: sortOrders()
   {
   class ProductSort : public IContainerControl::CompareFn
      {
      public:
      USHORT usFocus;
      Boolean bAscending;
      int isEqual(IContainerObject* pObj1, IContainerObject* pObj2,
                  IContainerControl* pCnr) const
         {
         Boolean bLess;
         if ( usFocus == ORDERSNEW )
            {
            Product* pProduct1 = (Product*)pObj1;
            Product* pProduct2 = (Product*)pObj2;
            bLess = ( pProduct1->typeModel() < pProduct2->typeModel() );
            }
         else
            {
            Mes* pMES1 = (Mes*)pObj1;
            Mes* pMES2 = (Mes*)pObj2;
            if ( pMES1->baseProduct() == pMES2->baseProduct() )
               bLess = ( pMES1->serial() < pMES2->serial() );
            else
               bLess = ( pMES1->baseProduct() < pMES2->baseProduct() );
            }
         if ( bLess )
            {
            if ( bAscending )
               return -1;
            else
               return 1;
            }
         else
            {
            if ( bAscending )
               return 1;
            else
               return -1;
            }
         }
      };
   class BaseSort : public IContainerControl::CompareFn
      {
      public:
      Boolean bAscending;
      int isEqual(IContainerObject* pObj1, IContainerObject* pObj2,
                  IContainerControl* pCnr) const
         {
         Boolean bLess;
         Mes* pMES1 = (Mes*)pObj1;
         Mes* pMES2 = (Mes*)pObj2;
         if ( pMES1->sOriginalTypeModel == pMES2->sOriginalTypeModel )
            {
            bLess = ( pMES1->sMesGroupSequence < pMES2->sMesGroupSequence );
            if ( !bAscending )
               bLess = !bLess;
            }
         else
            bLess = ( pMES1->sOriginalTypeModel < pMES2->sOriginalTypeModel );
         if ( bLess )
            {
            if ( bAscending )
               return -1;
            else
               return 1;
            }
         else
            {
            if ( bAscending )
               return 1;
            else
               return -1;
            }
         }
      };
   class OrderSort : public IContainerControl::CompareFn
      {
      public:
      USHORT usFocus;
      Boolean bAscending;
      int isEqual(IContainerObject* pObj1, IContainerObject* pObj2,
                  IContainerControl* pCnr) const
         {
         Boolean bLess;
         if ( usFocus == ORDERSNEW )
            {
            Product* pProduct1 = (Product*)pObj1;
            Product* pProduct2 = (Product*)pObj2;
            bLess = ( pProduct1->order() < pProduct2->order() );
            }
         else
            {
            Mes* pMES1 = (Mes*)pObj1;
            Mes* pMES2 = (Mes*)pObj2;
            bLess = ( pMES1->order() < pMES2->order() );
            }
         if ( bLess )
            {
            if ( bAscending )
               return -1;
            else
               return 1;
            }
         else
            {
            if ( bAscending )
               return 1;
            else
               return -1;
            }
         }
      };
   class SerialSort : public IContainerControl::CompareFn
      {
      public:
      Boolean bAscending;
      int isEqual(IContainerObject* pObj1, IContainerObject* pObj2,
                  IContainerControl* pCnr) const
         {
         Boolean bLess;
         Mes* pMES1 = (Mes*)pObj1;
         Mes* pMES2 = (Mes*)pObj2;
         bLess = ( pMES1->serial() < pMES2->serial() );
         if ( bLess )
            {
            if ( bAscending )
               return -1;
            else
               return 1;
            }
         else
            {
            if ( bAscending )
               return 1;
            else
               return -1;
            }
         }
      };
   class DateSort : public IContainerControl::CompareFn
      {
      public:
      USHORT usFocus;
      Boolean bAscending;
      int isEqual(IContainerObject* pObj1, IContainerObject* pObj2,
                  IContainerControl* pCnr) const
         {
         Boolean bLess;
         if ( usFocus == ORDERSNEW )
            {
            Product* pProduct1 = (Product*)pObj1;
            Product* pProduct2 = (Product*)pObj2;
            bLess = ( pProduct1->date() < pProduct2->date() );
            }
         else
            {
            Mes* pMES1 = (Mes*)pObj1;
            Mes* pMES2 = (Mes*)pObj2;
            bLess = ( pMES1->date() < pMES2->date() );
            }
         if ( bLess )
            {
            if ( bAscending )
               return -1;
            else
               return 1;
            }
         else
            {
            if ( bAscending )
               return 1;
            else
               return -1;
            }
         }
      };

   ProductSort sortProduct;
   BaseSort sortBase;
   OrderSort sortOrder;
   SerialSort sortSerial;
   DateSort sortDate;
   bAscending = ( sSort > 0 );
   USHORT usSort = abs(sSort);

   pMenu->uncheckItem(IDMI_ViewSortProduct);
   pMenu->uncheckItem(IDMI_ViewSortBase);
   pMenu->uncheckItem(IDMI_ViewSortOrder);
   pMenu->uncheckItem(IDMI_ViewSortSerial);
   pMenu->uncheckItem(IDMI_ViewSortDate);

   switch ( usSort )
      {
      case enumSortProduct:
         sortProduct.bAscending = bAscending;
         sortProduct.usFocus = ORDERSNEW;
         pcnrOrders[ORDERSNEW]->sort(sortProduct);
         sortProduct.usFocus = ORDERSMES;
         pcnrOrders[ORDERSMES]->sort(sortProduct);
         pMenu->checkItem(IDMI_ViewSortProduct);
         break;
      case enumSortBase:
         sortBase.bAscending = bAscending;
         pcnrOrders[ORDERSMES]->sort(sortBase);
         pMenu->checkItem(IDMI_ViewSortBase);
         break;
      case enumSortOrder:
         sortOrder.bAscending = bAscending;
         sortOrder.usFocus = ORDERSNEW;
         pcnrOrders[ORDERSNEW]->sort(sortOrder);
         sortOrder.usFocus = ORDERSMES;
         pcnrOrders[ORDERSMES]->sort(sortOrder);
         pMenu->checkItem(IDMI_ViewSortOrder);
         break;
      case enumSortSerial:
         sortSerial.bAscending = bAscending;
         pcnrOrders[ORDERSMES]->sort(sortSerial);
         pMenu->checkItem(IDMI_ViewSortSerial);
         break;
      case enumSortDate:
         sortDate.bAscending = bAscending;
         sortDate.usFocus = ORDERSNEW;
         pcnrOrders[ORDERSNEW]->sort(sortDate);
         sortDate.usFocus = ORDERSMES;
         pcnrOrders[ORDERSMES]->sort(sortDate);
         pMenu->checkItem(IDMI_ViewSortDate);
         break;
      }
   pProfile->sSortOrders = sSort;
   }

//=======================================================================
// FUNCTION NAME    selectAll
//
// DESCRIPTION      Select or deselect all objects in a container.
//=======================================================================

void OrdersNotebook :: selectAll(Boolean bSelect)
   {
   IContainerControl* pCnr = pcnrOrders[usFocus];
   IContainerControl::ObjectCursor crs(*pCnr);
   for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
      {
      IContainerObject* pObject = (IContainerObject*)crs.current();
      pCnr->setSelected(pObject, bSelect);
      }
   }

//=======================================================================
// FUNCTION NAME    selectDate
//
// DESCRIPTION      Select objects with order dates on or before the
//                  specified date.
//=======================================================================

void OrdersNotebook :: selectDate()
   {
   if ( usFocus == ORDERSNEW )
      {
      IContainerControl::ObjectCursor crs(*pcnrOrders[ORDERSNEW]);
      for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
         {
         Product* pProduct = (Product*)crs.current();
         pcnrOrders[ORDERSNEW]->setSelected(pProduct, pProduct->date() <= pProfile->dateSelect);
         }
      }
   else
      {
      IContainerControl::ObjectCursor crs(*pcnrOrders[ORDERSMES]);
      for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
         {
         Mes* pMES = (Mes*)crs.current();
         pcnrOrders[ORDERSMES]->setSelected(pMES, pMES->date() <= pProfile->dateSelect);
         }
      }
   pcnrOrders[usFocus]->setFocus();
   }

//=======================================================================
// FUNCTION NAME    createColumn
//
// DESCRIPTION      Create a new container column.
//=======================================================================

void OrdersNotebook :: createColumn(IContainerColumn** ppColumn, ULONG ulOffset,
                                    IContainerColumn::HorizontalAlignment align)
   {
   IContainerColumn* pColumn = new IContainerColumn(ulOffset);
   pColumn->showSeparators();
   pColumn->justifyHeading(IContainerColumn::bottom);
   pColumn->justifyData(IContainerColumn::centeredVertically, align);
   *ppColumn = pColumn;
   }

//=======================================================================
// FUNCTION NAME    setHeadingText
//
// DESCRIPTION      Build a two-line column heading.
//=======================================================================

void OrdersNotebook :: setHeadingText(IContainerColumn* pColumn, ULONG ulString)
   {
   IString line1 = LOADSTRING(ulString);
   IString line2 = LOADSTRING(ulString+1);
   if ( line1 == STR_Blank1 )
      pColumn->setHeadingText(line2);
   else
      pColumn->setHeadingText(line1 + "\n" + line2);
   }

//=======================================================================
// FUNCTION NAME    adjustTabWidth
//
// DESCRIPTION      Adjust the notebook tab width to fit the longest
//                  description.
//=======================================================================

void OrdersNotebook :: adjustTabWidth()
   {
   IFont iFont(pNotebook);
   ULONG ulWidth = 0;

   INotebook::Cursor crs(*pNotebook);
   for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
      {
      IString strText = pNotebook->pageSettings(crs).tabText();
      ULONG ulTest = iFont.textWidth(strText);
      if ( ulTest > ulWidth )
         ulWidth = ulTest;
      }

   ulWidth = ulWidth * 6 / 5;
   ULONG ulHeight = iFont.maxCharHeight() * 3 / 2;
   pNotebook->setMajorTabSize(ISize(ulWidth, ulHeight));
   }

//=======================================================================
// FUNCTION NAME    menuShowing
//
// DESCRIPTION      Handle submenu events.
//=======================================================================

Boolean OrdersNotebook::MenuHandler :: menuShowing(IMenuEvent& menuEvt,
                                                   ISubmenu& subMenu)
   {
   USHORT usFocus = pWindow->usFocus;
   ULONG ulCount = pWindow->pcnrOrders[usFocus]->objectCount();
   ULONG ulSelected = pWindow->ulSelected[usFocus];

   switch ( subMenu.id() )
      {
      case IDM_Edit:
         {
         if ( !pWindow->pMain->pFile->changes() )
            subMenu.disableItem(IDMI_EditUndo);
         if ( !ulSelected )
            subMenu.disableItem(IDMI_EditDeselectAll);
         if ( ulSelected == ulCount )
            subMenu.disableItem(IDMI_EditSelectAll);
         if ( !ulCount )
               subMenu.disableItem(IDMI_EditSelectDate);
         break;
         }

      case IDM_View:
         if ( !ulCount )
               subMenu.disableItem(IDMI_ViewSort);
         break;

      case IDMI_ViewSort:
         if ( usFocus == ORDERSNEW )
            {
            subMenu.deleteItem(IDMI_ViewSortBase);
            subMenu.deleteItem(IDMI_ViewSortSerial);
            }
         break;

      case IDMI_ViewSortProduct:
         if ( pWindow->bAscending )
            subMenu.checkItem(IDMI_ViewSortA);
         else
            subMenu.checkItem(IDMI_ViewSortD);
         pWindow->sSort = enumSortProduct;
         break;

      case IDMI_ViewSortBase:
         if ( pWindow->bAscending )
            subMenu.checkItem(IDMI_ViewSortA);
         else
            subMenu.checkItem(IDMI_ViewSortD);
         pWindow->sSort = enumSortBase;
         break;

      case IDMI_ViewSortSerial:
         if ( pWindow->bAscending )
            subMenu.checkItem(IDMI_ViewSortA);
         else
            subMenu.checkItem(IDMI_ViewSortD);
         pWindow->sSort = enumSortSerial;
         break;

      case IDMI_ViewSortOrder:
         if ( pWindow->bAscending )
            subMenu.checkItem(IDMI_ViewSortA);
         else
            subMenu.checkItem(IDMI_ViewSortD);
         pWindow->sSort = enumSortOrder;
         break;

      case IDMI_ViewSortDate:
         if ( pWindow->bAscending )
            subMenu.checkItem(IDMI_ViewSortA);
         else
            subMenu.checkItem(IDMI_ViewSortD);
         pWindow->sSort = enumSortDate;
         break;

      case IDM_Selected:
         if ( !ulCount )                    //edw 12/17/97 added this check
            {
            subMenu.disableItem(IDMI_SelectedApply);
            subMenu.disableItem(IDMI_SelectedRemove);
            subMenu.disableItem(IDMI_SelectedDetail);
            }
         else
            if ( !ulSelected )
               {
               subMenu.disableItem(IDMI_SelectedApply);
               subMenu.disableItem(IDMI_SelectedRemove);
               subMenu.disableItem(IDMI_SelectedDetail);
               }
            else
               {
               if ( !pWindow->selected(true) )
                  subMenu.disableItem(IDMI_SelectedRemove);
               if ( !pWindow->selected(false) )
                  subMenu.disableItem(IDMI_SelectedApply);
               }
         break;
      }
   return true;
   }

//=======================================================================
// FUNCTION NAME    reformat
//
// DESCRIPTION      Reformat dates for new locale.
//=======================================================================

void OrdersNotebook :: reformat()
   {
   setIcon(IResourceId(IDI_ApplyOrders));
   delete pMenu;
   pMenu = new IMenuBar(IDW_Orders, this);
   ITitle title(this, pMain->pFile->name(), LOADSTRING(IDS_OrdersTitle));
   pHelp = pMain->helpWindow();
   pHelp->setAssociatedWindow(this);
   pcnrOrders[ORDERSNEW]->setRefreshOff();
   pcnrOrders[ORDERSMES]->setRefreshOff();
   pcnrOrders[ORDERSNEW]->setTitle(IResourceId(IDS_NewOrders));
   setHeadingText(pcolAppliedNew, IDS_Applied1);
   setHeadingText(pcolProductNew, IDS_Product1);
   setHeadingText(pcolOrderNew, IDS_Order1);
   setHeadingText(pcolDateNew, IDS_DateCRAD1);
   pcnrOrders[ORDERSMES]->setTitle(IResourceId(IDS_MESOrders));
   setHeadingText(pcolAppliedMES, IDS_Applied1);
   setHeadingText(pcolBaseMES, IDS_Base1);
   setHeadingText(pcolSerialMES, IDS_Serial1);
   setHeadingText(pcolTargetMES, IDS_Target1);
   setHeadingText(pcolOrderMES, IDS_Order1);
   setHeadingText(pcolDateMES, IDS_DateCRAD1);
   setHeadingText(pcolGroupMES, IDS_Group1);
   IContainerControl::ObjectCursor crsProd(*pcnrOrders[ORDERSNEW]);
   for ( crsProd.setToFirst(); crsProd.isValid(); crsProd.setToNext() )
      {
      Product* pProduct = (Product*)crsProd.current();
      pProduct->format();
      }
   IContainerControl::ObjectCursor crsMES(*pcnrOrders[ORDERSMES]);
   for ( crsMES.setToFirst(); crsMES.isValid(); crsMES.setToNext() )
      {
      Mes* pMES = (Mes*)crsMES.current();
      pMES->format();
      }
   switch ( abs(sSort) )
      {
      case enumSortProduct:
         pMenu->checkItem(IDMI_ViewSortProduct);
         break;
      case enumSortBase:
         pMenu->checkItem(IDMI_ViewSortBase);
         break;
      case enumSortOrder:
         pMenu->checkItem(IDMI_ViewSortOrder);
         break;
      case enumSortSerial:
         pMenu->checkItem(IDMI_ViewSortSerial);
         break;
      case enumSortDate:
         pMenu->checkItem(IDMI_ViewSortDate);
         break;
      }
   pcnrOrders[ORDERSNEW]->setRefreshOn();
   pcnrOrders[ORDERSMES]->setRefreshOn();
   pcnrOrders[ORDERSNEW]->refresh();
   pcnrOrders[ORDERSMES]->refresh();
   if ( pSelectDate )
      {
      pSelectDate->close();
      pSelectDate = (SelectDateDialog*)NULL;
      }
   pNotebook->setTabText(pNotebook->firstPage(), LOADSTRING(IDS_NewOrders));
   pNotebook->setTabText(pNotebook->lastPage(), LOADSTRING(IDS_MESOrders));
   adjustTabWidth();
   }

//=======================================================================
// FUNCTION NAME    selected
//
// DESCRIPTION      Count the applied or unapplied in the set of
//                  selected objects.
//=======================================================================

ULONG OrdersNotebook :: selected(Boolean bApplied)
   {
   ULONG ulCount = 0;
   if ( ulSelected[usFocus] )
      {
      IContainerControl::ObjectCursor crs(*pcnrOrders[usFocus]);
      for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
         {
         if ( usFocus == ORDERSNEW )
            {
            Product* pProduct = (Product*)crs.current();
            if ( pcnrOrders[usFocus]->isSelected(pProduct) )
               if ( pProduct->applied() == bApplied )
                  ulCount++;
            }
         else
            {
            Mes* pMES = (Mes*)crs.current();
            if ( pcnrOrders[usFocus]->isSelected(pMES) )
               if ( pMES->applied() == bApplied )
                  ulCount++;
            }
         }
      }
   return ulCount;
   }

//=======================================================================
// FUNCTION NAME    ~OrdersNotebook()
//
// DESCRIPTION      Destructor for the Orders Notebook 44753 flw
//=======================================================================
 OrdersNotebook::~OrdersNotebook()
 {
   delete pMenu;
   delete pNotebook;
   delete pSelectDate;
   delete pcolCPUNew;
 }
