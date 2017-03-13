//=======================================================================
// FILE NAME        AF9SYSTM.HPP
//
// CLASS NAME       SystemsNotebook
//
// DESCRIPTION      This class presents the Systems notebook in the Apply
//                  Orders CPF.
//=======================================================================
// Copyright (c) International Business Machines Corp. 1996
// All rights reserved
//=======================================================================

#ifndef AF9SYSTM_HPP
#define AF9SYSTM_HPP

#include <icmdhdr.hpp>                  // ICommandHandler
#include <icnrcol.hpp>                  // IContainerColumn
#include <icnrhdr.hpp>                  // ICnrHandler
#include <icnrmhdr.hpp>                 // ICnrMenuHandler
#include <ifocshdr.hpp>                 // IFocusHandler
#include <iframe.hpp>                   // IFrameWindow
#include <iframhdr.hpp>                 // IFrameHandler
#include <ikeyhdr.hpp>                  // IKeyboardHandler
#include <inotebk.hpp>                  // INoteBook
#include <ipagehdr.hpp>                 // IPageHandler
#include <isizehdr.hpp>                 // IResizeHandler

#include "af9main.hpp"                  // MainWindow

class IContainerControl;
class IHelpWindow;
class IMenuBar;
class INotebook;

class SystemEdit;
class Profile;

class SystemsNotebook : public IFrameWindow, public ICommandHandler,
                        public ICnrHandler, public ICnrMenuHandler,
                        public IPageHandler, public IResizeHandler,
                        public IFocusHandler, public IKeyboardHandler,
                        public IFrameHandler
   {
   public:
   SystemsNotebook(MainWindow* pOwner);
   USHORT usSystems;                    // Number of system notebook pages
   IContainerControl** pcnrProducts;    // Product containers
   INotebook* pNotebook;                // Client notebook
   MainWindow* pMain;                   // Main frame window
   ULONG* ulSelected;                   // Selected object count
   USHORT usFocus;                      // Index of container with focus
   Boolean bLock;                       // Disable conflicting actions
   Profile* pProfile;                   // User profile
   void clearNotebook();                // Remove all products and pages
   void buildNotebook(USHORT usNew);    // Add all notebook pages
   void initializeNotebook();           // Return to default state
   USHORT pageNumber();                 // Index of top page in notebook
   IString& systemName(USHORT usPage);  // Text of specified notebook tab
   void addPage(USHORT usPage, IString& strText); // Add a new page
   void setPage(USHORT usPage, USHORT usNewPage, IString& strText);
                                        // Change the settings of a page
   void sortProducts(USHORT usIndex);   // Sort products by type-model
   void openAllDetail();                // Show product detail for all items   //edw09/21/00
   ~SystemsNotebook(); //flw destructor


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
   Boolean virtualKeyPress(IKeyboardEvent& keyEvt);
   Boolean activated(IFrameEvent& frmEvt);
   Boolean closed(IFrameEvent& cmdEvt);        //edw12/18/97

   private:

   class MenuHandler : public IMenuHandler
      {
      public:
      SystemsNotebook* pWindow;
      protected:
      Boolean menuShowing(IMenuEvent& menuEvt, ISubmenu& subMenu);
      };
   MenuHandler menuHandler;             // Subclass IMenuHandler

   Boolean bPopup;                      // Selected from popup, not pulldown
   ULONG ulContainer;                   // Increment container IDs
   SystemEdit* pSystemEdit;             // System edit dialog
   IMenuBar* pMenu;                     // Menu bar
   IHelpWindow* pHelp;                  // Main help window
   IContainerColumn** pcolCPU;          // CPU icon column
   IContainerColumn** pcolProduct;      // Product column
   IContainerColumn** pcolOrder;        // Order column
   void openSystem(USHORT usMode);      // New, move, or rename system
   void openDetail();                   // Show product detail
   void openProductInfo();              // Open product info dialog
   void allocateSystems(USHORT usNew);  // Allocate space for new system count
   void buildContainer(USHORT usIndex); // Build new container
   void appendPage(IString& strText, USHORT usIndex);
                                        // Add page at end of notebook
   void deletePage();                   // Delete a notebook page
   void adjustTabWidth();               // Adjust tab width for longest string
   void moveProducts();                 // Assign or remove products
   void selectAll(Boolean bSelect);     // Select/deselect all objects
   void createColumn(IContainerColumn** ppColumn, ULONG ulOffset,
                     IContainerColumn::HorizontalAlignment align);
                                        // Add a column to the container
   void setHeadingText(IContainerColumn* pColumn, ULONG ulString);
                                        // Set two-line heading text
   void reformat();                     // Format dates for new locale
   };
#endif
