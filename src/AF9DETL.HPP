//=======================================================================
// FILE NAME      AF9DETL.HPP
//
// CLASS NAME     DetailWindow
//
// DESCRIPTION    This class is the product detail window of the Apply
//                Orders CPF.
//=======================================================================
// 11/11/99 flw - Added Destructor to handle General Protection fault error 44753
//
//=======================================================================
// Copyright (c) International Business Machines Corp. 1996
// All rights reserved
//=======================================================================

#ifndef AF9DETL_HPP
#define AF9DETL_HPP

#include <icmdhdr.hpp>                  // ICommandHandler
#include <icnrcol.hpp>                  // IContainerColumn
#include <icnrhdr.hpp>                  // ICnrHandler
#include <iframe.hpp>                   // IFrameWindow
#include <isizehdr.hpp>                 // IResizeHandler
#include <istring.hpp>                  // IString

#include "mainw.hpp"
#include "af9main.hpp"                  // MainWindow
#include "euopres.hpp"
#include "euopiprt.hpp"

class IContainerControl;
class IHelpWindow;
class IResourceLibrary;
class IMenuBar;                           //44703

class Product;
class Mes;
class DetailObject;
class Profile;
                                           //44703
class DetailWindow : public IFrameWindow, public ICommandHandler,
                     public ICnrHandler, public IResizeHandler,
                     public ICnrMenuHandler
   {
   public:
   DetailWindow(MainWindow* pOwner);
   IContainerControl* pcnrDetail;       // Client detail container
   void add(Product* pProduct, DetailObject* pInsert, Boolean bLastObject=true);
                                        // Append product detail
   void add(Mes* pMES, DetailObject* pInsert, Boolean bLastObject=true);
                                        // Append MES detail
   void reload(Product* pProduct);      // Refresh product detail after MES
   void format();                       // Format strings, dates for new locale
   ~DetailWindow();                     // Destructor - Added to handle GPF error 44753 -flw
   int numberOfObjects(){ return pcnrDetail->objectCount(); }    //47703 - flw

   protected:
   Boolean command(ICommandEvent& cmdEvt);
   Boolean systemCommand(ICommandEvent& cmdEvt);
   Boolean windowResize(IResizeEvent& sizeEvt);
   Boolean help(ICnrHelpEvent& helpEvt);
   void printDetail();                                        //F47703
   void saveDetail();                                         //F47703
   void FormatPrintSave(IString sFileName);                   //F47703

   private:
   XWindow * pParent;                   //F47703
   EUOPResolve * Res;
   MainWindow* pMain;                   // Main window
   IHelpWindow* pHelp;                  // Help window
   Profile* pProfile;                   // User profile
   IMenuBar* pMenu;                     // Menu bar     47703 - flw
   IContainerColumn* pcolLabel;         // Label container column
   IContainerColumn* pcolQuantity;      // Quantity container column
   IContainerColumn* pcolStatus;        // Status container column
   IContainerColumn* pcolText;          // Text container column
   void createColumn(IContainerColumn** ppColumn, ULONG ulOffset,
                     IContainerColumn::HorizontalAlignment align);
                                        // Add a column to the container
   void clearContainer();               // Remove detail, disconnect pointers
   DetailObject* addDesc(IString& strDesc, DetailObject* pPrevious);
                                        // Add multiple-line description
   };
#endif
