//=======================================================================
// FILE NAME        AF9ORDRS.HPP
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

#ifndef AF9ORDRS_HPP
#define AF9ORDRS_HPP

#include <icmdhdr.hpp>                  // ICommandHandler
#include <icnrcol.hpp>                  // IContainerColumn
#include <icnrhdr.hpp>                  // ICnrHandler
#include <icnrmhdr.hpp>                 // ICnrMenuHandler
#include <ifocshdr.hpp>                 // IFocusHandler
#include <iframe.hpp>                   // IFrameWindow
#include <iframhdr.hpp>                 // IFrameHandler
#include <inotebk.hpp>                  // INoteBook
#include <ipagehdr.hpp>                 // IPageHandler
#include <isizehdr.hpp>                 // IResizeHandler

#include "af9main.hpp"                  // MainWindow

class IContainerControl;
class IHelpWindow;
class IMenuBar;
class INotebook;

class SelectDateDialog;
class Product;
class Mes;
class Profile;

class OrdersNotebook : public IFrameWindow, public ICommandHandler,
                       public ICnrHandler, public ICnrMenuHandler,
                       public IPageHandler, public IResizeHandler,
                       public IFocusHandler, public IFrameHandler
   {
   public:
   enum { enumSortProduct=1, enumSortBase, enumSortSerial,
          enumSortOrder, enumSortDate };

   OrdersNotebook(MainWindow* pOwner);
   IContainerControl* pcnrOrders[2];    // Order containers
   ULONG ulSelected[2];                 // Selected object count
   USHORT usFocus;                      // Index of container with focus
   SHORT sSort;                         // Sort column
   Boolean bAscending;                  // Ascending sort sequence
   MainWindow* pMain;                   // Main frame window
   Profile* pProfile;                   // User profile
   void clearNotebook();                // Remove all orders and MESs
   void initializeNotebook();           // Return to default state
   ULONG selected(Boolean bApplied);    // Count un/applied in selected rows
   ~OrdersNotebook();                   // Destructor for Orders Notebook - 44753 flw
   protected:
   Boolean command(ICommandEvent& cmdEvt);
   Boolean select(IPageSelectEvent& pagEvt);
   Boolean help(IPageHelpEvent& helpEvt);
   Boolean selectedChanged(ICnrEmphasisEvent& cnrEvt);
   Boolean enter(ICnrEnterEvent& enterEvt);
   Boolean help(ICnrHelpEvent& helpEvt);
   Boolean makePopUpMenu(IMenuEvent& menuEvt);
   Boolean modelessResults(IFileDialog* endingDialog);
   Boolean windowResize(IResizeEvent& sizeEvt);
   Boolean gotFocus(IControlEvent& ctlEvt);
   Boolean activated(IFrameEvent& frmEvt);
   Boolean closed(IFrameEvent& cmdEvt);         //edw12/18/97

   private:

   class MenuHandler : public IMenuHandler
      {
      public:
      OrdersNotebook* pWindow;
      protected:
      Boolean menuShowing(IMenuEvent& menuEvt, ISubmenu& subMenu);
      };
   MenuHandler menuHandler;             // Subclass IMenuHandler

   Boolean bPopup;                      // Selected from popup, not pulldown
   SelectDateDialog* pSelectDate;       // Select by Date dialog
   IMenuBar* pMenu;                     // Menu bar
   IHelpWindow* pHelp;                  // Main help window
   INotebook* pNotebook;                // Client notebook
   IContainerColumn* pcolCPUNew;        // New container CPU column
   IContainerColumn* pcolAppliedNew;    // New container applied column
   IContainerColumn* pcolProductNew;    // New container product column
   IContainerColumn* pcolOrderNew;      // New container order column
   IContainerColumn* pcolDateNew;       // New container date column
   IContainerColumn* pcolAppliedMES;    // MES container applied column
   IContainerColumn* pcolBaseMES;       // MES container base product column
   IContainerColumn* pcolSerialMES;     // MES container serial column
   IContainerColumn* pcolTargetMES;     // MES container target product column
   IContainerColumn* pcolOrderMES;      // MES container order column
   IContainerColumn* pcolDateMES;       // MES container date column
   IContainerColumn* pcolGroupMES;      // MES container group column
   void openDetail();                   // Open product detail
   void openSelectDate();               // Open Select by Date
   void openProductInfo();              // Open product info dialog
   void buildNotebook();                // Add pages to the client notebook
   void applyOrders(Boolean bApply);    // Apply or remove selected orders
   void apply(Product* pProduct, Boolean bApply);
                                        // Apply or remove an order
   void apply(Mes* pMES, Boolean bApply);
                                        // Apply or remove an MES
   void sortOrders();                   // Sort orders by product, serial, etc
   void selectAll(Boolean bSelect);     // Select/deselect all objects
   void selectDate();                   // Select by order date
   void createColumn(IContainerColumn** ppColumn, ULONG ulOffset,
                     IContainerColumn::HorizontalAlignment align);
                                        // Add a column to the container
   void setHeadingText(IContainerColumn* pColumn, ULONG ulString);
                                        // Set two-line heading text
   void adjustTabWidth();               // Adjust tab width for longest string
   void reformat();                     // Format dates for new locale
   };
#endif
