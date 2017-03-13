//=======================================================================
// FILE NAME        AF9SYSTM.CPP
//
// CLASS NAME       SystemsNotebook
//
// DESCRIPTION      This class presents the Systems notebook in the Apply
//                  Orders CPF.
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
#include <ikeyhdr.hpp>                  // IKeyboardHandler
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

#include "af9systm.hpp"                 // SystemsNotebook
#include "af9main.hpp"                  // MainWindow
#include "af9sysed.hpp"                 // SystemEdit
#include "afcprods.hpp"                 // Product
#include "afcbkend.hpp"                 // BackEnd
#include "af9profl.hpp"                 // Profile
#include "af9apply.h"                   // Resource ids

enum { enumProduct, enumSerial, enumOrder, enumDate };

//=======================================================================
// FUNCTION NAME    SystemsNotebook
//
// DESCRIPTION      Construct an instance of the SystemsNotebook class.
//=======================================================================

SystemsNotebook :: SystemsNotebook(MainWindow* pOwner)
                 : IFrameWindow(IResourceId(IDW_Systems), NULL,
                                NULL, IRectangle(),
                                IFrameWindow::defaultStyle() |
                                IFrameWindow::noMoveWithOwner)
   {

   // Capture the incoming parameters.

   pMain = pOwner;
   pSystemEdit = (SystemEdit*)NULL;

   // Attach the appropriate icon.

   setIcon(IResourceId(IDI_ApplyOrders));

   // Disable close from the system menu.

   //edwISystemMenu(this).disableItem(ISystemMenu::idClose);

   // Create the menu bar.

   pMenu = new IMenuBar(IDW_Systems, this);

   // Attach the help window.

   pHelp = pMain->helpWindow();
   pHelp->setAssociatedWindow(this);

   // Capture the user profile.

   pProfile = pMain->pProfile;
   bLock = false;

   // Create the client notebook and add the appropriate pages.

   pNotebook = new INotebook(IDC_NB_Systems, this, this);
   setClient(pNotebook);
   pNotebook->enableTabStop();
   usSystems = 0;
   allocateSystems(1);
   ulContainer = SYSTEMSUNASSIGNED;
   buildContainer(SYSTEMSUNASSIGNED);
   addExtension(pcnrProducts[SYSTEMSUNASSIGNED], IFrameWindow::rightOfClient, 0.44, thickLine);
   pcnrProducts[SYSTEMSUNASSIGNED]->enableTabStop();
   pcnrProducts[SYSTEMSUNASSIGNED]->setTitle(IResourceId(IDS_Unassigned));

   // Add handlers for container, menu, focus, and keyboard events.

   ICnrHandler::handleEventsFor(pcnrProducts[SYSTEMSUNASSIGNED]);
   ICnrMenuHandler::handleEventsFor(pcnrProducts[SYSTEMSUNASSIGNED]);
   IFocusHandler::handleEventsFor(pcnrProducts[SYSTEMSUNASSIGNED]);
   IKeyboardHandler::handleEventsFor(pcnrProducts[SYSTEMSUNASSIGNED]);

   // Make this window visible to the menu handler.

   menuHandler.pWindow = this;

   // Add handlers for command, resize, frame, menu, and page events.

   ICommandHandler::handleEventsFor(this);
   ICommandHandler::handleEventsFor(pNotebook);
   IResizeHandler::handleEventsFor(this);
   IPageHandler::handleEventsFor(pNotebook);
   menuHandler.handleEventsFor(this);
   IFrameHandler::handleEventsFor(this);
   }

//=======================================================================
// FUNCTION NAME    command
//
// DESCRIPTION      Handle command messages.
//=======================================================================

Boolean SystemsNotebook :: command(ICommandEvent& cmdEvt)
   {
   switch ( cmdEvt.commandId() )
      {

      case IDMI_SystemNew:
         openSystem(SystemEdit::enumNew);
         break;

      case IDMI_SystemSettings:
         openSystem(SystemEdit::enumSettings);
         break;

      case IDMI_SystemDelete:
         deletePage();
         break;

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

      case IDMI_EditSelectAll:
         selectAll(true);
         break;

      case IDMI_EditDeselectAll:
         selectAll(false);
         break;

      case IDMI_SelectedAssign:
         bPopup = false;
         moveProducts();
         break;

      case IDMI_SelectedRemove:
         bPopup = false;
         moveProducts();
         break;

      case IDMI_PopupAssign:
         bPopup = true;
         moveProducts();
         break;

      case IDMI_PopupRemove:
         bPopup = true;
         moveProducts();
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
Boolean SystemsNotebook :: closed(IFrameEvent& cmdEvt)
   {
  //edw
  // if ( hasFocus() ||
  //      pNotebook->hasFocus() )
  //    {
  //    //WinAlarm(HWND_DESKTOP, WA_WARNING);//os2
  //    MessageBeep(MB_ICONEXCLAMATION);     //win95 --EDS
  //    return true;
  //    }
  // for ( USHORT usIndex = 0; usIndex < usSystems; usIndex++ )
  //    if ( pcnrProducts[usIndex]->hasFocus() )
  //       {
  //       //WinAlarm(HWND_DESKTOP, WA_WARNING); //os2
  //       MessageBeep(MB_ICONEXCLAMATION);      //win95 --EDS
  //       return true;
  //       }
   if ( pSystemEdit )
      pSystemEdit->sendEvent(IWindow::command, IDC_PB_Cancel);
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

Boolean SystemsNotebook :: select(IPageSelectEvent& pagEvt)
   {
   selectAll(false);//jt--7/22/97

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

Boolean SystemsNotebook :: help(IPageHelpEvent& helpEvt)
   {
   pHelp->show(IResourceId(IDW_Systems));
   return true;
   }

//=======================================================================
// FUNCTION NAME    selectedChanged
//
// DESCRIPTION      Handle container select events.
//=======================================================================

Boolean SystemsNotebook :: selectedChanged(ICnrEmphasisEvent& cnrEvt)
   {
   Product* pProduct = (Product*)cnrEvt.object();
   if ( pcnrProducts[usFocus]->isSelected(pProduct) )
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

Boolean SystemsNotebook :: enter(ICnrEnterEvent& enterEvt)
   {
   if ( enterEvt.object() )
      {
      bPopup = true;
      pcnrProducts[usFocus]->removeSelected(enterEvt.object());
      openDetail();
      }
   return false;
   }

//=======================================================================
// FUNCTION NAME    help
//
// DESCRIPTION      Handle container help events.
//=======================================================================

Boolean SystemsNotebook :: help(ICnrHelpEvent& helpEvt)
   {
   pHelp->show(IResourceId(IDW_Systems));
   return true;
   }

//=======================================================================
// FUNCTION NAME    makePopupMenu
//
// DESCRIPTION      Handle popup menu events.
//=======================================================================

Boolean SystemsNotebook :: makePopUpMenu(IMenuEvent& menuEvt)
   {
   Product* pProduct = (Product*)popupMenuObject();
   if ( !pProduct )
      {
      //WinAlarm(HWND_DESKTOP, WA_WARNING); //os2
      MessageBeep(MB_ICONEXCLAMATION);      //win95 --EDS
      return false;
      }
   IPoint pt = menuEvt.mousePosition();
   IContainerControl* pCnr = pcnrProducts[usFocus];
   if ( pCnr->objectUnderPoint(pt) != pProduct )
      pt = pCnr->detailsObjectRectangle(pProduct).center();
   IPopUpMenu* pPopUp = new IPopUpMenu(IDM_Popup, menuEvt.window());
   pPopUp->deleteItem(IDMI_PopupOpen);
   pPopUp->deleteItem(IDMI_PopupSave);
   pPopUp->deleteItem(IDMI_PopupSaveAs);
   //pPopUp->deleteItem(IDMI_PopupGenerate);    //47705 - flw
   pPopUp->deleteItem(IDMI_PopupRename);
   pPopUp->deleteItem(IDMI_PopupDelete);
   pPopUp->deleteItem(IDMI_PopupApply);
   if ( usFocus == SYSTEMSUNASSIGNED )
      {
      pPopUp->deleteItem(IDMI_PopupRemove);
      if ( !pNotebook->totalPages() )
         pPopUp->deleteItem(IDMI_PopupAssign);
      }
   else
      {
      pPopUp->deleteItem(IDMI_PopupAssign);
      if ( pProduct->category() == AbstractProduct::CPU )
         pPopUp->deleteItem(IDMI_PopupRemove);
      else
         if ( pCnr->isSelected(pProduct) )
            {
            IContainerControl::ObjectCursor crs(*pCnr);
            for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
               {
               Product* pProd = (Product*)crs.current();
               if ( pCnr->isSelected(pProd) )
                  if ( pProd->category() == AbstractProduct::CPU )
                     {
                     pPopUp->deleteItem(IDMI_PopupRemove);
                     break;
                     }
               }
            }
      }
   pPopUp->setAutoDeleteObject();

   pPopUp->show(pt);
   pCnr->setCursor(pProduct);
   return false;
   }

//=======================================================================
// FUNCTION NAME    windowResize
//
// DESCRIPTION      Handle window resize events.
//=======================================================================

Boolean SystemsNotebook :: windowResize(IResizeEvent& sizeEvt)
   {
   if ( isVisible() && !isMinimized() )
      {
      pProfile->position[WINDOWSYSTEMS] = position();
      pProfile->size[WINDOWSYSTEMS] = size();
      if ( pMain->isMinimized() )
         {
         pMain->restore();
         setFocus();
         }
      adjustTabWidth();
      }
   return false;
   }

//=======================================================================
// FUNCTION NAME    gotFocus
//
// DESCRIPTION      Handle focus events.
//=======================================================================

Boolean SystemsNotebook :: gotFocus(IControlEvent& ctlEvt)
   {
   ULONG ulId = ctlEvt.controlId();
   if ( ulId == IDC_CN_Assign )
      usFocus = SYSTEMSUNASSIGNED;
   else {
      if (usFocus == SYSTEMSUNASSIGNED)
         selectAll( false);//edw-7/24/97
      usFocus = pageNumber();
      }
   return false;
   }

//=======================================================================
// FUNCTION NAME    virtualKeyPress
//
// DESCRIPTION      Handle virtual keyboard events.
//=======================================================================

Boolean SystemsNotebook :: virtualKeyPress(IKeyboardEvent& keyEvt)
   {
   switch ( keyEvt.virtualKey() )
      {
      case IKeyboardEvent::tab:
         if ( usFocus == SYSTEMSUNASSIGNED )
            pcnrProducts[pageNumber()]->setFocus();
         else
            pcnrProducts[SYSTEMSUNASSIGNED]->setFocus();
         break;
      }
   return false;
   }

//=======================================================================
// FUNCTION NAME    activated
//
// DESCRIPTION      Handle frame activation events.
//=======================================================================

Boolean SystemsNotebook :: activated(IFrameEvent& frmEvt)
   {
   pMain->activeWindow = this;
   return false;
   }

//=======================================================================
// FUNCTION NAME    openSystem
//
// DESCRIPTION      Open the dialog to create, move, or rename a system.
//=======================================================================

void SystemsNotebook :: openSystem(USHORT usMode)
   {
   if ( !pSystemEdit )
      {
      pSystemEdit = new SystemEdit(this);
      pHelp->setAssociatedWindow(pSystemEdit);
      }
   pSystemEdit->loadDefaults(usMode);
   pSystemEdit->show();
   pSystemEdit->addToWindowList();
   }

//=======================================================================
// FUNCTION NAME    openDetail
//
// DESCRIPTION      Open the detail dialog.
//=======================================================================

void SystemsNotebook :: openDetail()
   {
   IContainerControl* pCnr = pcnrProducts[usFocus];
   HCURSOR hCur;
   if ( bPopup )
      {
      Product* pProduct = (Product*)pCnr->cursoredObject();
      if ( !pCnr->isSelected(pProduct) )
         pMain->openDetail(pProduct);
      else
         bPopup = false;
      }
   if ( !bPopup )
      {
      IContainerControl::ObjectCursor crs(*pCnr, IContainerObject::selected);

      Product* pLastProduct = (Product*)crs.last();  // EDS-ADDED 2-22-97
      hCur=SetCursor(LoadCursor(NULL, IDC_WAIT));    //
      ShowCursor(TRUE);                              //

      for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
         {
         Product* pProduct = (Product*)crs.current();
         if (pLastProduct == pProduct)               // EDS-ADDED 2-22-97
            { // LAST PRODUCT
              pMain->openDetail(pProduct,true);
            }
         else
            { // NOT LAST PRODUCT
              pMain->openDetail(pProduct,false);
            }
         }
      ShowCursor(FALSE);                             // EDS-ADDED 2-22-97
      SetCursor(hCur);                               //
      }
   }

//=======================================================================
// FUNCTION NAME    openAllDetail
//
// DESCRIPTION      Open the detail dialog for all items on all pages of
//                  the notebook.
// MODIFICATION
// Date             Key      Developer      Description
// 09/21/2000       D54593   E. Wilson      original
//=======================================================================

void SystemsNotebook :: openAllDetail()
   {
   for ( USHORT usIndex = 0; usIndex < usSystems; usIndex++ )
      {
      usFocus = usIndex;
      sendEvent(IWindow::command, IDMI_EditSelectAll);
      sendEvent(IWindow::command, IDMI_SelectedDetail);
      }
   usFocus = SYSTEMSUNASSIGNED;
   }

//=======================================================================
// FUNCTION NAME    openProductInfo
//
// DESCRIPTION      Open the product information dialog.
//=======================================================================

void SystemsNotebook :: openProductInfo()
   {
   IMessageBox::Style style = IMessageBox::okButton |
                              IMessageBox::informationIcon |
                              IMessageBox::moveable |
					IMessageBox::systemModal;
   IMessageBox msg(this);
   msg.setTitle(IResourceId(IDS_Product_Title));
   IString strText = LOADSTRING(IDS_Product_Text);
   strText.change(STR_Substitute0, STR_Version);
   msg.show(strText, style);
   }

//=======================================================================
// FUNCTION NAME    clearNotebook
//
// DESCRIPTION      Remove all pages from the notebook.
//=======================================================================

void SystemsNotebook :: clearNotebook()
   {
   for ( USHORT usIndex = 0; usIndex < usSystems; usIndex++ )
      pcnrProducts[usIndex]->setRefreshOff();

   if ( usSystems > 1 )
      {
      for ( USHORT usIndex = 1; usIndex < usSystems; usIndex++ )
         delete pcnrProducts[usIndex];
      pNotebook->removeAllPages();
      }
   pcnrProducts[SYSTEMSUNASSIGNED]->deleteAllObjects();
   ulContainer = usSystems = 1;
   if ( pSystemEdit )
      pSystemEdit->sendEvent(IWindow::command, IDC_PB_Cancel);
   }

//=======================================================================
// FUNCTION NAME    allocateSystems
//
// DESCRIPTION      Allocate structures for the new system count.
//=======================================================================

void SystemsNotebook :: allocateSystems(USHORT usNew)
   {
   IContainerControl** pcnrOld;
   ULONG* ulOld;
   IContainerColumn** pcolOld1;
   IContainerColumn** pcolOld2;
   IContainerColumn** pcolOld3;

   // Capture temporary copies of old pointers.

   if ( usSystems )
      {
      pcnrOld = pcnrProducts;
      ulOld = ulSelected;
      pcolOld1 = pcolCPU;
      pcolOld2 = pcolProduct;
      pcolOld3 = pcolOrder;
      }

   // Allocate pointers to arrays of new size.

   pcnrProducts = (IContainerControl**)malloc(usNew * sizeof(IContainerControl*));
   ulSelected = (ULONG*)malloc(usNew * sizeof(ULONG));
   pcolCPU = (IContainerColumn**)malloc(usNew * sizeof(IContainerColumn*));
   pcolProduct = (IContainerColumn**)malloc(usNew * sizeof(IContainerColumn*));
   pcolOrder = (IContainerColumn**)malloc(usNew * sizeof(IContainerColumn*));

   // Copy old pointers to new arrays, then free memory for old arrays.

   if ( usNew < usSystems )
      usSystems = usNew;
   if ( usSystems )
      {
      for ( USHORT usIndex = 0; usIndex < usSystems; usIndex++ )
         {
         pcnrProducts[usIndex] = pcnrOld[usIndex];
         ulSelected[usIndex] = ulOld[usIndex];
         pcolCPU[usIndex] = pcolOld1[usIndex];
         pcolProduct[usIndex] = pcolOld2[usIndex];
         pcolOrder[usIndex] = pcolOld3[usIndex];
         }
      free(pcnrOld);
      free(ulOld);
      free(pcolOld1);
      free(pcolOld2);
      free(pcolOld3);
      }

   // Note new system count.

   usSystems = usNew;
   }

//=======================================================================
// FUNCTION NAME    buildNotebook
//
// DESCRIPTION      Add the appropriate pages to the notebook.
//=======================================================================

void SystemsNotebook :: buildNotebook(USHORT usNew)
   {
   IPageHandler::stopHandlingEventsFor(pNotebook);

   // Allocate new arrays of pointers.

   allocateSystems(usNew);

   // Create new product containers and new notebook pages.

   for ( USHORT usIndex = 1; usIndex < usSystems; usIndex++ )
      {
      buildContainer(usIndex);
      pcnrProducts[usIndex]->setRefreshOff();
      IString strText = LOADSTRING(IDS_SystemTab);
      strText.change(STR_Substitute0, IString(usIndex));
      appendPage(strText, usIndex);
      }

   // Adjust the tab size to fit the largest description.

   adjustTabWidth();

   // Set the general notebook appearance.

   pNotebook->setMinorTabSize(ISize(0,0));
   pNotebook->setBinding(INotebook::spiral);
   pNotebook->setTabShape(INotebook::rounded);
   pNotebook->setOrientation(INotebook::backpagesRightTabsBottom);
   IPageHandler::handleEventsFor(pNotebook);
   }

//=======================================================================
// FUNCTION NAME    initializeNotebook
//
// DESCRIPTION      Return to the default state after loading.
//=======================================================================

void SystemsNotebook :: initializeNotebook()
   {
   for ( USHORT usIndex = 0; usIndex < usSystems; usIndex++ )
      {
      sortProducts(usIndex);
      ulSelected[usIndex] = 0;
      pcnrProducts[usIndex]->setRefreshOn();
      pcnrProducts[usIndex]->refresh(true);
      }

   // Put the first page on top, then focus on the unassigned list.

   //if ( pNotebook->totalPages() )
   //   pNotebook->turnToPage(pNotebook->firstPage());      //jt-2/5/98 this is a problem
   pcnrProducts[SYSTEMSUNASSIGNED]->setFocus();
   usFocus = SYSTEMSUNASSIGNED;
   }

//=======================================================================
// FUNCTION NAME    buildContainer
//
// DESCRIPTION      Build a new product container for the notebook.
//=======================================================================

void SystemsNotebook :: buildContainer(USHORT usIndex)
   {

   // Create a new product container.

   pcnrProducts[usIndex] = new IContainerControl(IDC_CN_Assign+ulContainer++,
                                                 pNotebook, pNotebook, IRectangle(),
                                                 IContainerControl::defaultStyle() | IContainerControl::pmCompatible);//jt-chg

   // Add columns for icon, type-model, and order number.

   pcolCPU[usIndex] = new IContainerColumn(IContainerColumn::isIcon);
   pcolCPU[usIndex]->hideSeparators();
   createColumn(&pcolProduct[usIndex], Product::offsetTypeModel(),
                IContainerColumn::left);
   createColumn(&pcolOrder[usIndex], Product::offsetOrder(),
                IContainerColumn::left);
   setHeadingText(pcolProduct[usIndex], IDS_Product1);
   setHeadingText(pcolOrder[usIndex], IDS_SerialOrder1);
   if ( usIndex )
      pcnrProducts[usIndex]->addColumn(pcolCPU[usIndex]);
   pcnrProducts[usIndex]->addColumn(pcolProduct[usIndex]);
   pcnrProducts[usIndex]->addColumn(pcolOrder[usIndex]);
   pcolOrder[usIndex]->hideSeparators(IContainerColumn::verticalSeparator);

   // Set container characteristics.

   pcnrProducts[usIndex]->setTitle(IResourceId(IDS_Assigned));
   pcnrProducts[usIndex]->setTitle(IResourceId(IDS_Assigned));
   setHeadingText(pcolProduct[usIndex], IDS_Product1);
   setHeadingText(pcolOrder[usIndex], IDS_SerialOrder1);
   pcnrProducts[usIndex]->showTitle();
   pcnrProducts[usIndex]->showDetailsView();
   pcnrProducts[usIndex]->setMultipleSelection();
   pcnrProducts[usIndex]->setDeleteObjectsOnClose();
   pcnrProducts[usIndex]->setDeleteColumnsOnClose();
   pcnrProducts[usIndex]->setIconSize(ISize(16, 16));
   }

//=======================================================================
// FUNCTION NAME    appendPage
//
// DESCRIPTION      Add the next page at the end of the notebook.
//=======================================================================

void SystemsNotebook :: appendPage(IString& strText, USHORT usIndex)
   {
   INotebook::PageSettings::Attribute attributeMajor =
       INotebook::PageSettings::majorTab |
       INotebook::PageSettings::autoPageSize |
       INotebook::PageSettings::statusTextOn;
   INotebook::PageSettings pgMajor = INotebook::PageSettings(attributeMajor);

   pgMajor.setTabText(strText);
   pNotebook->addLastPage(pgMajor, pcnrProducts[usIndex]);

   // Add handlers for container, menu, and focus events.

   ICnrHandler::handleEventsFor(pcnrProducts[usIndex]);
   ICnrMenuHandler::handleEventsFor(pcnrProducts[usIndex]);
   IFocusHandler::handleEventsFor(pcnrProducts[usIndex]);
   IKeyboardHandler::handleEventsFor(pcnrProducts[usIndex]);
   }

//=======================================================================
// FUNCTION NAME    adjustTabWidth
//
// DESCRIPTION      Adjust the notebook tab width to fit the longest
//                  description.
//=======================================================================

void SystemsNotebook :: adjustTabWidth()
   {
   if ( !pNotebook->totalPages() )
      return;

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
   if ( ulWidth < 50 )
      ulWidth = 50;
   else if ( size().width() )
      {
      ULONG ulMax = size().width() / 3;
      if ( ulWidth > ulMax )
         ulWidth = ulMax;
      }
   ULONG ulHeight = iFont.maxCharHeight() * 3 / 2;
   pNotebook->setMajorTabSize(ISize(ulWidth, ulHeight));
   }

//=======================================================================
// FUNCTION NAME    sortProducts
//
// DESCRIPTION      Sort the system lists by type-model.
//=======================================================================

void SystemsNotebook :: sortProducts(USHORT usIndex)
   {
   class ProductSort : public IContainerControl::CompareFn
      {
      public:
      int isEqual(IContainerObject* pObj1, IContainerObject* pObj2,
                  IContainerControl* pCnr) const
         {
         Product* pProduct1 = (Product*)pObj1;
         Product* pProduct2 = (Product*)pObj2;
         if ( pProduct1->category() < pProduct2->category() )
            return -1;
         if ( pProduct1->category() > pProduct2->category() )
            return 1;
         if ( pProduct1->typeModel() < pProduct2->typeModel() )
            return -1;
         else
            return 1;
         }
      };

   ProductSort sortProduct;

   pcnrProducts[usIndex]->sort(sortProduct);
   }

//=======================================================================
// FUNCTION NAME    moveProducts
//
// DESCRIPTION      Assign or remove selected products.
//=======================================================================

void SystemsNotebook :: moveProducts()
   {
   USHORT usTarget = SYSTEMSUNASSIGNED;
   if ( !usFocus )
      usTarget = pageNumber();
   pcnrProducts[usFocus]->setRefreshOff();
   pcnrProducts[usTarget]->setRefreshOff();
   if ( bPopup )
      {
      Product* pProduct = (Product*)pcnrProducts[usFocus]->cursoredObject();
      if ( !pcnrProducts[usFocus]->isSelected(pProduct) )
         {
         pcnrProducts[usFocus]->removeObject(pProduct);
         pcnrProducts[usTarget]->addObject(pProduct);
         }
      else
         bPopup = false;
      }
   if ( !bPopup )
      {
      IContainerControl::ObjectCursor crs(*pcnrProducts[usFocus]);
      for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
         {
         Product* pProduct = (Product*)crs.current();
         if ( pcnrProducts[usFocus]->isSelected(pProduct) )
            {
            pcnrProducts[usFocus]->removeSelected(pProduct);
            pcnrProducts[usTarget]->addObject(pProduct);
            pcnrProducts[usFocus]->setSelected(pProduct);
            }
         }
      pcnrProducts[usFocus]->removeSelectedObjects();
      ulSelected[usFocus] = 0;
      }
   sortProducts(usTarget);
   pcnrProducts[usFocus]->setRefreshOn();
   pcnrProducts[usTarget]->setRefreshOn();
   pcnrProducts[usFocus]->refresh(true);
   pcnrProducts[usTarget]->refresh(true);
   pMain->pFile->setChanges(pMain->pFile->changes()+1);
   }

//=======================================================================
// FUNCTION NAME    selectAll
//
// DESCRIPTION      Select or deselect all objects in a container.
//=======================================================================

void SystemsNotebook :: selectAll(Boolean bSelect)
   {
   IContainerControl* pCnr = pcnrProducts[usFocus];
   IContainerControl::ObjectCursor crs(*pCnr);
   for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
      {
      IContainerObject* pObject = (IContainerObject*)crs.current();
      pCnr->setSelected(pObject, bSelect);
      }
   }

//=======================================================================
// FUNCTION NAME    createColumn
//
// DESCRIPTION      Create a new container column.
//=======================================================================

void SystemsNotebook :: createColumn(IContainerColumn** ppColumn, ULONG ulOffset,
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

void SystemsNotebook :: setHeadingText(IContainerColumn* pColumn, ULONG ulString)
   {
   IString line1 = LOADSTRING(ulString);
   IString line2 = LOADSTRING(ulString+1);
   if ( line1 == STR_Blank1 )
      pColumn->setHeadingText(line2);
   else
      pColumn->setHeadingText(line1 + "\n" + line2);
   }

//=======================================================================
// FUNCTION NAME    pageNumber
//
// DESCRIPTION      Return the index of the notebook top page.
//=======================================================================

USHORT SystemsNotebook :: pageNumber()
   {
   USHORT usPages = pNotebook->totalPages();
   if ( !usPages )
      return usPages;
   IPageHandle pg = pNotebook->lastPage();
   while ( usPages )
      {
      if ( pg == pNotebook->topPage() )
         break;
      pg = pNotebook->previousPage(pg);
      usPages--;
      }
   return usPages;
   }

//=======================================================================
// FUNCTION NAME    systemName
//
// DESCRIPTION      Return the text shown on the tab of the specified
//                  notebook page.
//=======================================================================

IString& SystemsNotebook :: systemName(USHORT usPage)
   {
   static IString strText;
   if ( usPage < usSystems )
      {
      IPageHandle pg = pNotebook->firstPage();
      for ( USHORT usIndex = 2; usIndex <= usPage; usIndex++ )
         pg = pNotebook->nextPage(pg);
      strText = pNotebook->pageSettings(pg).tabText();
      }
   else
      strText = "";
   return strText;
   }

//=======================================================================
// FUNCTION NAME    addPage
//
// DESCRIPTION      Add a new page before the specified page number, with
//                  the specified tab text.
//=======================================================================

void SystemsNotebook :: addPage(USHORT usPage, IString& strText)
   {
   IPageHandler::stopHandlingEventsFor(pNotebook);
   allocateSystems(usSystems+1);
   buildContainer(usSystems-1);
   appendPage(strText, usSystems-1);
   setPage(usSystems-1, usPage, strText);
   ulSelected[usSystems-1] = 0;
   adjustTabWidth();
   IPageHandler::handleEventsFor(pNotebook);
   INotebook::Cursor crs(*pNotebook);
   crs.setToFirst();
   for ( USHORT usIndex = 1; usIndex < usPage; usIndex++ )
      crs.setToNext();
//jt   pNotebook->turnToPage(crs.current());
   }

//=======================================================================
// FUNCTION NAME    setPage
//
// DESCRIPTION      Change the settings (placement, description) of an
//                  existing page.
//=======================================================================

void SystemsNotebook :: setPage(USHORT usPage, USHORT usNewPage,
                                IString& strText)
   {
   if ( usPage != usNewPage )
      {
      IContainerControl* pcnrTemp = pcnrProducts[usPage];
      ULONG ulTemp = ulSelected[usPage];
      IContainerColumn* pcolTemp1 = pcolCPU[usPage];
      IContainerColumn* pcolTemp2 = pcolProduct[usPage];
      IContainerColumn* pcolTemp3 = pcolOrder[usPage];
      IString strTemp = systemName(usPage);

      USHORT usIndex;
      INotebook::Cursor crs(*pNotebook);
      if ( usNewPage > usPage)
         {
         crs.setToFirst();
         for ( usIndex = 1; usIndex < usPage; usIndex++ )
            crs.setToNext();
         for ( usIndex = usPage; usIndex < usNewPage; usIndex++ )
            {
            pcnrProducts[usIndex] = pcnrProducts[usIndex+1];
            ulSelected[usIndex] = ulSelected[usIndex+1];
            pcolCPU[usIndex] = pcolCPU[usIndex+1];
            pcolProduct[usIndex] = pcolProduct[usIndex+1];
            pcolOrder[usIndex] = pcolOrder[usIndex+1];
            pNotebook->setTabText(crs.current(), systemName(usIndex+1));
            crs.setToNext();
            }
         }
      else
         {
         crs.setToLast();
         for ( usIndex = usSystems-1; usIndex > usPage; usIndex-- )
            crs.setToPrevious();
         for ( usIndex = usPage; usIndex > usNewPage; usIndex-- )
            {
            pcnrProducts[usIndex] = pcnrProducts[usIndex-1];
            ulSelected[usIndex] = ulSelected[usIndex-1];
            pcolCPU[usIndex] = pcolCPU[usIndex-1];
            pcolProduct[usIndex] = pcolProduct[usIndex-1];
            pcolOrder[usIndex] = pcolOrder[usIndex-1];
            pNotebook->setTabText(crs.current(), systemName(usIndex-1));
            crs.setToPrevious();
            }
         }

      pcnrProducts[usNewPage] = pcnrTemp;
      ulSelected[usNewPage] = ulTemp;
      pcolCPU[usNewPage] = pcolTemp1;
      pcolProduct[usNewPage] = pcolTemp2;
      pcolOrder[usNewPage] = pcolTemp3;
      pNotebook->setTabText(crs.current(), strTemp);

      usIndex = 1;
      for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
         {
         pNotebook->setWindow(crs, pcnrProducts[usIndex++]);
         }
      }
   if ( strText != systemName(usNewPage) )
      {
      USHORT usIndex = 1;
      INotebook::Cursor crs(*pNotebook);
      for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
         {
         if ( usIndex == usNewPage )
            {
            pNotebook->setTabText(crs.current(), strText);
            break;
            }
         usIndex++;
         }
      adjustTabWidth();
      }
   pMain->pFile->setChanges(pMain->pFile->changes()+1);
   }

//=======================================================================
// FUNCTION NAME    deletePage
//
// DESCRIPTION      Delete a page from the notebook.
//=======================================================================

void SystemsNotebook :: deletePage()
   {
   USHORT usPage = pageNumber();

   IContainerControl* pCnr = pcnrProducts[usPage];
   if ( pCnr->objectCount() )
      {
      Product* pProduct = (Product*)pCnr->objectAt(0);
      if ( pProduct->category() == AbstractProduct::CPU )
         {
         IMessageBox msg(this);
         IMessageBox::Style style = IMessageBox::cancelButton |
                                    IMessageBox::warningIcon |
                                    IMessageBox::moveable |
						IMessageBox::systemModal;
         msg.setTitle(IResourceId(IDS_ErrorDeleteCPU_Title));
         msg.show(IResourceId(IDS_ErrorDeleteCPU_Text), style, IDH_ErrorDeleteCPU);
         return;
         }
      }

   // Deselect all before deleting page
   // Otherwise, will crash              MSN 2/21/97

   usFocus = usPage;
   selectAll(false);

   IPageHandler::stopHandlingEventsFor(pNotebook);

   if ( pCnr->objectCount() )
      {
      IContainerControl::ObjectCursor crsProd(*pCnr);
      for ( crsProd.setToFirst(); crsProd.isValid(); crsProd.setToNext() )
         {
         Product* pProduct = (Product*)crsProd.current();
         pCnr->removeSelected(pProduct);
         pcnrProducts[SYSTEMSUNASSIGNED]->addObject(pProduct);
         }
      sortProducts(SYSTEMSUNASSIGNED);
      }
   delete pCnr;
   for ( USHORT usIndex = usPage; usIndex < usSystems-1; usIndex++ )
      {
      pcnrProducts[usIndex] = pcnrProducts[usIndex+1];
      ulSelected[usIndex] = ulSelected[usIndex+1];
      pcolCPU[usIndex] = pcolCPU[usIndex+1];
      pcolProduct[usIndex] = pcolProduct[usIndex+1];
      pcolOrder[usIndex] = pcolOrder[usIndex+1];
      }
   allocateSystems(usSystems-1);

   INotebook::Cursor crsPage(*pNotebook);
   crsPage.setToFirst();
   for ( usIndex = 1; usIndex < usPage; usIndex++ )
      crsPage.setToNext();
   pNotebook->removePage(crsPage.current());

   IPageHandler::handleEventsFor(pNotebook);

   if ( pNotebook->totalPages() )
      {
      adjustTabWidth();
//jt      pNotebook->turnToPage(pNotebook->firstPage());
      usFocus = 1;
      }
   else
      {
      pcnrProducts[SYSTEMSUNASSIGNED]->setFocus();
      usFocus = SYSTEMSUNASSIGNED;
      }
   pMain->pFile->setChanges(pMain->pFile->changes()+1);
   }

//=======================================================================
// FUNCTION NAME    menuShowing
//
// DESCRIPTION      Handle submenu events.
//=======================================================================

Boolean SystemsNotebook::MenuHandler :: menuShowing(IMenuEvent& menuEvt,
                                                    ISubmenu& subMenu)
   {
   USHORT usFocus = pWindow->usFocus;
   ULONG ulCount = pWindow->pcnrProducts[usFocus]->objectCount();
   ULONG ulSelected = pWindow->ulSelected[usFocus];
   ULONG ulPages = pWindow->pNotebook->totalPages();

   switch ( subMenu.id() )
      {
      case IDM_System:
         if ( !ulPages )
            {
            subMenu.disableItem(IDMI_SystemSettings);
            subMenu.disableItem(IDMI_SystemDelete);
            }
         else if ( pWindow->bLock )
            {
            subMenu.disableItem(IDMI_SystemNew);
            subMenu.disableItem(IDMI_SystemSettings);
            subMenu.disableItem(IDMI_SystemDelete);
            }
         break;
      case IDM_Edit:
         if ( !pWindow->pMain->pFile->changes() )
            subMenu.disableItem(IDMI_EditUndo);
         else if ( pWindow->bLock )
            subMenu.disableItem(IDMI_EditUndo);
         if ( !ulSelected )
            subMenu.disableItem(IDMI_EditDeselectAll);
         if ( ulSelected == ulCount )
            subMenu.disableItem(IDMI_EditSelectAll);
         break;

      case IDM_Selected:
         if ( !ulCount )                    //edw 12/17/97 added this check
            {
            subMenu.disableItem(IDMI_SelectedAssign);
            subMenu.disableItem(IDMI_SelectedRemove);
            subMenu.disableItem(IDMI_SelectedDetail);
            }
         else
            if ( !ulSelected )
               {
               subMenu.disableItem(IDMI_SelectedAssign);
               subMenu.disableItem(IDMI_SelectedRemove);
               subMenu.disableItem(IDMI_SelectedDetail);
               }
            else
               {
               Product* pProduct = (Product*)pWindow->pcnrProducts[usFocus]->objectAt(0);
               if ( pWindow->pcnrProducts[usFocus]->isSelected(pProduct) )
                  if ( pProduct->category() == AbstractProduct::CPU )
                     subMenu.disableItem(IDMI_SelectedRemove);
               }
         if ( usFocus == SYSTEMSUNASSIGNED )
            {
            subMenu.disableItem(IDMI_SelectedRemove);
            if ( !ulPages )
               subMenu.disableItem(IDMI_SelectedAssign);
            }
         else
            subMenu.disableItem(IDMI_SelectedAssign);
         break;
      }
   return true;
   }

//=======================================================================
// FUNCTION NAME    reformat
//
// DESCRIPTION      Reformat dates for new locale.
//=======================================================================

void SystemsNotebook :: reformat()
   {
   setIcon(IResourceId(IDI_ApplyOrders));
   delete pMenu;
   pMenu = new IMenuBar(IDW_Systems, this);
   ITitle title(this, pMain->pFile->name(), LOADSTRING(IDS_SystemsTitle));
   pHelp = pMain->helpWindow();
   pHelp->setAssociatedWindow(this);
   for ( USHORT usIndex = 0; usIndex < usSystems; usIndex++ )
      {
      pcnrProducts[usIndex]->setRefreshOff();
      pcnrProducts[usIndex]->setTitle(IResourceId(IDS_Assigned));
      setHeadingText(pcolProduct[usIndex], IDS_Product1);
      setHeadingText(pcolOrder[usIndex], IDS_SerialOrder1);
      IContainerControl::ObjectCursor crs(*pcnrProducts[usIndex]);
      for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
         {
         Product* pProduct = (Product*)crs.current();
         pProduct->format();
         }
      pcnrProducts[usIndex]->setRefreshOn();
      pcnrProducts[usIndex]->refresh();
      }
   pcnrProducts[SYSTEMSUNASSIGNED]->setTitle(IResourceId(IDS_Unassigned));
   if ( pSystemEdit )
      {
      pSystemEdit->close();
      pSystemEdit = (SystemEdit*)NULL;
      }
   }


//=======================================================================
// FUNCTION NAME    ~SystemsNotebook
//
// DESCRIPTION      Destructor for Systems Notebook.
//=======================================================================

SystemsNotebook :: ~SystemsNotebook()
{
  delete pMenu;
  delete pNotebook;
  delete pSystemEdit;

 }
