//===========================================================================
// FILE NAME        AF9DETL.CPP
//
// CLASS NAME       DetailWindow
//
// DESCRIPTION      This class is the product detail window of the Apply
//                  Orders CPF.
//===========================================================================
// Copyright (c) International Business Machines Corp. 1996
// All rights reserved
//===========================================================================
//  HECC  08/26/2002 ICA68115 - No helpbutton and Windows explorer style
//                              in the 'Save file' and 'Save As' FileDialogs
//===========================================================================

#include <imsgbox.hpp>
#include <icnrcol.hpp>                  // IContainerColumn
#include <icnrctl.hpp>                  // IContainerControl
#include <icnrhdr.hpp>                  // ICnrHandler
#include <iframe.hpp>                   // IFrameWindow
#include <ihelp.hpp>                    // IHelpWindow
#include <irect.hpp>                    // IRectangle
#include <ireslib.hpp>                  // IResourceLibrary/IResourceId
#include <isizehdr.hpp>                 // IResizeHandler
#include <isysmenu.hpp>                 // ISystemMenu
#include <ititle.hpp>                   // ITitle
#include <imenubar.hpp>                 // IMenuBar
#include <ifiledlg.hpp>                 // IFILEDIALOG, flw

#include "af9detl.hpp"                  // DetailWindow
#include "af9main.hpp"                  // MainWindow
#include "af9dtobj.hpp"                 // DetailObject
#include "afcprods.hpp"                 // Product, Mes
#include "afcbkend.hpp"                 // BackEnd
#include "af9profl.hpp"                 // Profile
#include "af9apply.h"                   // Resource ids


#define STR_DtlRptDefault      "DetailReport.txt"         //47703
#define STR_DtlPrintJobName    "DETAIL.PRT"               //47703
char *pPRTDTL_TEMP_FILE  =      "pPRTMM_TEMP_FILE";       //47703
#define STR_DtlHdr             "Product Detail Report"    //47703
#define STR_DltHdrLn           "***********************************************************"
char *strPageBreak =           "\x0D\x0C";

//=======================================================================
// FUNCTION NAME    DetailWindow
//
// DESCRIPTION      Construct an instance of the DetailWindow class.
//=======================================================================

DetailWindow :: DetailWindow(MainWindow* pOwner)
              : IFrameWindow(IResourceId(IDW_Detail), NULL,
                             NULL, IRectangle(),
                             IFrameWindow::defaultStyle() |
                             IFrameWindow::noMoveWithOwner)
   {

   // Initialize global variables.

   pMain = pOwner;

   // Attach the help window.

   pHelp = pMain->helpWindow();
   pHelp->setAssociatedWindow(this);

   // Capture the user profile.

   pProfile = pMain->pProfile;

   // Set the title for the current language.

   ITitle title(this, pMain->pFile->name(), LOADSTRING(IDS_DetailTitle));

   // Create menu bar.
   pMenu = new IMenuBar(IDW_Detail, this);

   // Create client container.

   pcnrDetail = new IContainerControl(IDC_CN_Detail, this, this, IRectangle(),
                          IContainerControl::defaultStyle() | IContainerControl::pmCompatible);//jt-chg
   createColumn(&pcolLabel, DetailObject::offsetLabel(),
                IContainerColumn::left);
   createColumn(&pcolQuantity, DetailObject::offsetQuantity(),
                IContainerColumn::right);
   createColumn(&pcolStatus, DetailObject::offsetStatus(),
                IContainerColumn::left);
   createColumn(&pcolText, DetailObject::offsetText(),
                IContainerColumn::left);
   pcnrDetail->addColumn(pcolLabel);
   pcnrDetail->addColumn(pcolQuantity);
   pcnrDetail->addColumn(pcolStatus);
   pcnrDetail->addColumn(pcolText);

   setClient(pcnrDetail);
   pcnrDetail->showDetailsView();
   pcnrDetail->hideDetailsViewTitles();
   pcnrDetail->hideTitle();

   // Add handlers for command, resize, and container messages.

   ICommandHandler::handleEventsFor(this);
   IResizeHandler::handleEventsFor(this);
   ICnrHandler::handleEventsFor(pcnrDetail);

   // Make this window visible and give it the keyboard focus.

   show();
   setFocus();
   }


//=======================================================================
// FUNCTION NAME    command
//
// DESCRIPTION      Handle command messages.
//=======================================================================

Boolean DetailWindow :: command(ICommandEvent& cmdEvt)
{
   switch ( cmdEvt.commandId() )
      {

      case MI_DETL_PRINT:
         printDetail();
         break;

      case MI_DETL_SAVE:
         saveDetail();
         break;

      default:
        break ;
      }
   return true;
}

//=======================================================================
// FUNCTION NAME    systemCommand
//
// DESCRIPTION      Handle system command events.
//=======================================================================

Boolean DetailWindow :: systemCommand(ICommandEvent& cmdEvt)
   {

   // Hide this window instead of closing it.

   if ( cmdEvt.commandId() == ISystemMenu::idClose )
      {
      hide();
      pMain->activeWindow->setFocus();
      removeFromWindowList();
      clearContainer();
      return true;
      }
   return false;
   }

//=======================================================================
// FUNCTION NAME    windowResize
//
// DESCRIPTION      Handle window resize events.
//=======================================================================

Boolean DetailWindow :: windowResize(IResizeEvent& sizeEvt)
   {
   if ( isVisible() && !isMinimized() )
      {
      pProfile->position[WINDOWDETAIL] = position();
      pProfile->size[WINDOWDETAIL] = size();
      if ( pMain->isMinimized() )
         {
         pMain->restore();
         setFocus();
         }
      }
   return false;
   }

//=======================================================================
// FUNCTION NAME    help
//
// DESCRIPTION      Handle container help events.
//=======================================================================

Boolean DetailWindow :: help(ICnrHelpEvent& helpEvt)
   {
   pHelp->show(IResourceId(IDW_Detail));
   return true;
   }

//=======================================================================
// FUNCTION NAME    createColumn
//
// DESCRIPTION      Create a new container column.
//=======================================================================

void DetailWindow :: createColumn(IContainerColumn** ppColumn, ULONG ulOffset,
                                  IContainerColumn::HorizontalAlignment align)
   {
   IContainerColumn* pColumn = new IContainerColumn(ulOffset);
   pColumn->hideSeparators();
   pColumn->justifyData(IContainerColumn::centeredVertically, align);
   *ppColumn = pColumn;
   }

//=======================================================================
// FUNCTION NAME    clearContainer
//
// DESCRIPTION      Disconnect all pointers, then delete all detail
//                  objects.
//=======================================================================

void DetailWindow :: clearContainer()
   {
   pcnrDetail->setRefreshOff();//jt-add to speed it up

   IContainerControl::ObjectCursor crs(*pcnrDetail);
   for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
      {
      DetailObject* pDetail = (DetailObject*)crs.current();
      if ( pDetail->product() )
         pDetail->product()->setDetail((DetailObject*)NULL);
      else if ( pDetail->MES() )
         pDetail->MES()->setDetail((DetailObject*)NULL);
      }
   pcnrDetail->deleteAllObjects();
   //pcnrDetail->setRefreshOn();//jt-test
   //pcnrDetail->refresh(true);//jt-test
   }

//=======================================================================
// FUNCTION NAME    addDesc
//
// DESCRIPTION      Add the remaining lines of a multiple-line description.
//=======================================================================

DetailObject* DetailWindow :: addDesc(IString& strDesc,
                                      DetailObject* pPrevious)
   {
   DetailObject* pDetail;
   ULONG ulOffset = strDesc.indexOf(STR_DescDelimiter);
   strDesc.remove(1, ulOffset);
   ulOffset = strDesc.indexOf(STR_DescDelimiter);
   while ( ulOffset )
      {
      pDetail = new DetailObject();
      pDetail->setText(strDesc.subString(1, ulOffset-1));
      pcnrDetail->addObjectAfter(pDetail, pPrevious);
      pPrevious = pDetail;
      strDesc.remove(1, ulOffset);
      ulOffset = strDesc.indexOf(STR_DescDelimiter);
      }
   pDetail = new DetailObject();
   pDetail->setText(strDesc);
   pcnrDetail->addObjectAfter(pDetail, pPrevious);
   pPrevious = pDetail;
   return pPrevious;
   }

//=======================================================================
// FUNCTION NAME    format
//
// DESCRIPTION      Reformat strings and dates for new locale.
//=======================================================================

void DetailWindow :: format()
   {
   ITitle title(this, pMain->pFile->name(), LOADSTRING(IDS_DetailTitle));
   pcnrDetail->setRefreshOff();
   IContainerControl::ObjectCursor crs(*pcnrDetail);
   for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
      {
      DetailObject* pDetail = (DetailObject*)crs.current();
      pDetail->format();
      }
   pcnrDetail->setRefreshOn();
   pcnrDetail->refresh();
   }

//=======================================================================
// FUNCTION NAME    add
//
// DESCRIPTION      Append the detail information for a product, if it
//                  is not already displayed.  In any case, scroll to
//                  that product.
//=======================================================================

void DetailWindow :: add(Product* pProduct, DetailObject* pInsert,
                        Boolean bLastObject)
   {
   pcnrDetail->setRefreshOff();
   DetailObject* pPrevious = (DetailObject*)NULL;
   if ( pInsert )
      {
      pPrevious = pInsert;
      }
   if ( !pProduct->detail() )
      {
      DetailObject* pDetail;

      pDetail = new DetailObject();
      pDetail->setLabel(IDS_DetailTypeModel);
      pDetail->setText(pProduct->typeModel());
      pDetail->setProduct(pProduct);
      pProduct->setDetail(pDetail);
      if ( pPrevious )
         pcnrDetail->addObjectAfter(pDetail, pPrevious);
      else
         pcnrDetail->addObject(pDetail);
      pPrevious = pDetail;

      pDetail = new DetailObject();
      pDetail->setLabel(IDS_DetailDescription);
      IString strDesc = pProduct->description();
      ULONG ulOffset = strDesc.indexOf(STR_DescDelimiter);
      if ( !ulOffset )
         pDetail->setText(strDesc);
      else
         pDetail->setText(strDesc.subString(1, ulOffset-1));
      pcnrDetail->addObjectAfter(pDetail, pPrevious);
      pPrevious = pDetail;
      if ( ulOffset )
         pPrevious = addDesc(strDesc, pPrevious);

      pDetail = new DetailObject();
      pDetail->setLabel(IDS_DetailHwSw);
      switch ( pProduct->category() )
         {
         case AbstractProduct::CPU:
         case AbstractProduct::HW:
            pDetail->setText(IDS_DetailHardware);
            break;
         case AbstractProduct::SW:
            pDetail->setText(IDS_DetailSoftware);
            break;
         }
      pcnrDetail->addObjectAfter(pDetail, pPrevious);
      pPrevious = pDetail;

      pDetail = new DetailObject();
      pDetail->setLabel(IDS_DetailQuantity);
      pDetail->setText(pProduct->sQuantity);
      pcnrDetail->addObjectAfter(pDetail, pPrevious);
      pPrevious = pDetail;

      pDetail = new DetailObject();
      if ( (pProduct->eProductStatus == AbstractProduct::INSTALLED) ||
           (pProduct->eProductStatus == AbstractProduct::UNINSTALLED) )
         pDetail->setLabel(IDS_DetailSerial);
      else
         pDetail->setLabel(IDS_DetailOrder);
      pDetail->setText(pProduct->order());
      pcnrDetail->addObjectAfter(pDetail, pPrevious);
      pPrevious = pDetail;

      if ( (pProduct->eProductStatus != AbstractProduct::INSTALLED) &&
           (pProduct->eProductStatus != AbstractProduct::UNINSTALLED) )
         {
         pDetail = new DetailObject();
         pDetail->setLabel(IDS_DetailDate);
         pDetail->setText(pProduct->date().asString(STR_FormatDate));
         pDetail->setProduct(pProduct);
         pcnrDetail->addObjectAfter(pDetail, pPrevious);
         pPrevious = pDetail;
         }

      pDetail = new DetailObject();
      pDetail->setLabel(IDS_DetailSystemTypeNumber);
      pDetail->setText(pProduct->systemTypeNumber());
      pcnrDetail->addObjectAfter(pDetail, pPrevious);
      pPrevious = pDetail;

      if ( pProduct->sSwDesignatedCpuTypeSerial.length() )
         {
         pDetail = new DetailObject();
         pDetail->setLabel(IDS_DetailSystemTypeSerial);
         pDetail->setText(pProduct->sSwDesignatedCpuTypeSerial);
         pcnrDetail->addObjectAfter(pDetail, pPrevious);
         pPrevious = pDetail;
         }

      pDetail = new DetailObject();
      pDetail->setLabel(IDS_DetailStatus);
      switch ( pProduct->eProductStatus )
         {
         case AbstractProduct::INSTALLED:
            pDetail->setText(IDS_StatusInstalled);
            break;
         case AbstractProduct::UNINSTALLED:
            pDetail->setText(IDS_StatusUninstalled);
            break;
         case AbstractProduct::ONORDER:
            pDetail->setText(IDS_StatusOnOrder);
            break;
         case AbstractProduct::SHIPPED:
            pDetail->setText(IDS_StatusShipped);
            break;
         case AbstractProduct::NEWLYCONFIGURED:
            pDetail->setText(IDS_StatusNewConfig);
            break;
         }
      pDetail->setProduct(pProduct);
      pcnrDetail->addObjectAfter(pDetail, pPrevious);
      pPrevious = pDetail;

      pDetail = new DetailObject();
      pDetail->setLabel(IDS_DetailCustomerNumber);
      pDetail->setText(pProduct->customerNumber());
      pcnrDetail->addObjectAfter(pDetail, pPrevious);
      pPrevious = pDetail;

      pDetail = new DetailObject();
      pDetail->setLabel(IDS_DetailFeatures);
      pcnrDetail->addObjectAfter(pDetail, pPrevious);
      pPrevious = pDetail;

      FeatureCollection::Cursor crs(pProduct->FeatureSet());
      for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
         {
         pDetail = new DetailObject();
         Feature* pFeature = (Feature*)&crs.element();
         pDetail->setLabel(pFeature->sNumber);
         pDetail->setQuantity(IString(pFeature->iQuantity));
         switch ( pFeature->eAction )
            {
            case Feature::ADD:
               pDetail->setStatus(IDS_ActionAdd);
               break;
            case Feature::REMOVE:
               pDetail->setStatus(IDS_ActionRemove);
               break;
            }
         strDesc = pFeature->sDescription;
         ulOffset = strDesc.indexOf(STR_DescDelimiter);
         if ( !ulOffset )
            pDetail->setText(strDesc);
         else
            pDetail->setText(strDesc.subString(1, ulOffset-1));
         pcnrDetail->addObjectAfter(pDetail, pPrevious);
         pPrevious = pDetail;
         if ( ulOffset )
            pPrevious = addDesc(strDesc, pPrevious);
         }

      pDetail = new DetailObject();
      pDetail->setLabel("---------------");
      pDetail->setTrailer(true);
      pcnrDetail->addObjectAfter(pDetail, pPrevious);
      }
   if (bLastObject) //EDS-add this line 2/21/97
      {
      pcnrDetail->setRefreshOn();
      pcnrDetail->refresh(true);
      pcnrDetail->scrollToObject(pProduct->detail());
      }
   }

//=======================================================================
// FUNCTION NAME    add
//
// DESCRIPTION      Append the detail information for an MES, if it
//                  is not already displayed.  In any case, scroll to
//                  that MES.
//=======================================================================

void DetailWindow :: add(Mes* pMES, DetailObject* pInsert, Boolean bLastObject)
   {
   pcnrDetail->setRefreshOff();
   DetailObject* pPrevious = (DetailObject*)NULL;
   if ( pInsert )
      {
      pPrevious = pInsert;
      }
   if ( !pMES->detail() )
      {
      DetailObject* pDetail;

      pDetail = new DetailObject();
      pDetail->setLabel(IDS_DetailTypeModel);
      pDetail->setText(pMES->baseProduct());
      pDetail->setMES(pMES);
      pMES->setDetail(pDetail);
      if ( pPrevious )
         pcnrDetail->addObjectAfter(pDetail, pPrevious);
      else
         pcnrDetail->addObject(pDetail);
      pPrevious = pDetail;

      if ( pMES->targetProduct().length() )
         {
         pDetail = new DetailObject();
         pDetail->setLabel(IDS_DetailTarget);
         pDetail->setText(pMES->targetProduct());
         pcnrDetail->addObjectAfter(pDetail, pPrevious);
         pPrevious = pDetail;
         }

      pDetail = new DetailObject();
      pDetail->setLabel(IDS_DetailDescription);
      IString strDesc = pMES->description();
      ULONG ulOffset = strDesc.indexOf(STR_DescDelimiter);
      if ( !ulOffset )
         pDetail->setText(strDesc);
      else
         pDetail->setText(strDesc.subString(1, ulOffset-1));
      pcnrDetail->addObjectAfter(pDetail, pPrevious);
      pPrevious = pDetail;
      if ( ulOffset )
         pPrevious = addDesc(strDesc, pPrevious);

      pDetail = new DetailObject();
      pDetail->setLabel(IDS_DetailHwSw);
      switch ( pMES->category() )
         {
         case AbstractProduct::CPU:
         case AbstractProduct::HW:
            pDetail->setText(IDS_DetailHardware);
            break;
         case AbstractProduct::SW:
            pDetail->setText(IDS_DetailSoftware);
            break;
         }
      pcnrDetail->addObjectAfter(pDetail, pPrevious);
      pPrevious = pDetail;

      pDetail = new DetailObject();
      pDetail->setLabel(IDS_DetailOrder);
      pDetail->setText(pMES->order());
      pcnrDetail->addObjectAfter(pDetail, pPrevious);
      pPrevious = pDetail;

      pDetail = new DetailObject();
      pDetail->setLabel(IDS_DetailSerial);
      pDetail->setText(pMES->serial());
      pcnrDetail->addObjectAfter(pDetail, pPrevious);
      pPrevious = pDetail;

      pDetail = new DetailObject();
      pDetail->setLabel(IDS_DetailDate);
      pDetail->setText(pMES->date().asString(STR_FormatDate));
      pDetail->setMES(pMES);
      pcnrDetail->addObjectAfter(pDetail, pPrevious);
      pPrevious = pDetail;

      pDetail = new DetailObject();
      pDetail->setLabel(IDS_DetailSystemTypeNumber);
      pDetail->setText(pMES->systemTypeNumber());
      pcnrDetail->addObjectAfter(pDetail, pPrevious);
      pPrevious = pDetail;

      if ( pMES->sSwDesignatedCpuTypeSerial.length() )
         {
         pDetail = new DetailObject();
         pDetail->setLabel(IDS_DetailSystemTypeSerial);
         pDetail->setText(pMES->sSwDesignatedCpuTypeSerial);
         pcnrDetail->addObjectAfter(pDetail, pPrevious);
         pPrevious = pDetail;
         }

      pDetail = new DetailObject();
      pDetail->setLabel(IDS_DetailStatus);
      switch ( pMES->eProductStatus )
         {
         case AbstractProduct::INSTALLED:
            pDetail->setText(IDS_StatusInstalled);
            break;
         case AbstractProduct::UNINSTALLED:
            pDetail->setText(IDS_StatusUninstalled);
            break;
         case AbstractProduct::ONORDER:
            pDetail->setText(IDS_StatusOnOrder);
            break;
         case AbstractProduct::SHIPPED:
            pDetail->setText(IDS_StatusShipped);
            break;
         case AbstractProduct::NEWLYCONFIGURED:
            pDetail->setText(IDS_StatusNewConfig);
            break;
         }
      pDetail->setMES(pMES);
      pcnrDetail->addObjectAfter(pDetail, pPrevious);
      pPrevious = pDetail;

      pDetail = new DetailObject();
      pDetail->setLabel(IDS_DetailCustomerNumber);
      pDetail->setText(pMES->customerNumber());
      pcnrDetail->addObjectAfter(pDetail, pPrevious);
      pPrevious = pDetail;

      pDetail = new DetailObject();
      pDetail->setLabel(IDS_DetailFeatures);
      pcnrDetail->addObjectAfter(pDetail, pPrevious);
      pPrevious = pDetail;

      FeatureCollection::Cursor crs(pMES->FeatureSet());
      for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
         {
         pDetail = new DetailObject();
         Feature* pFeature = (Feature*)&crs.element();
         pDetail->setLabel(pFeature->sNumber);
         pDetail->setQuantity(IString(pFeature->iQuantity));
         switch ( pFeature->eAction )
            {
            case Feature::ADD:
               pDetail->setStatus(IDS_ActionAdd);
               break;
            case Feature::REMOVE:
               pDetail->setStatus(IDS_ActionRemove);
               break;
            }
         strDesc = pFeature->sDescription;
         ulOffset = strDesc.indexOf(STR_DescDelimiter);
         if ( !ulOffset )
            pDetail->setText(strDesc);
         else
            pDetail->setText(strDesc.subString(1, ulOffset-1));
         pcnrDetail->addObjectAfter(pDetail, pPrevious);
         pPrevious = pDetail;
         if ( ulOffset )
            pPrevious = addDesc(strDesc, pPrevious);
         }

      pDetail = new DetailObject();
      pDetail->setLabel("---------------");
      pDetail->setTrailer(true);
      pcnrDetail->addObjectAfter(pDetail, pPrevious);
      }
   if (bLastObject) //EDS-add this line 2/22/97
      {
      pcnrDetail->setRefreshOn();
      pcnrDetail->refresh(true);
      pcnrDetail->scrollToObject(pMES->detail());
      }
   }

//=======================================================================
// FUNCTION NAME    reload
//
// DESCRIPTION      After applying or removing an MES, delete the old
//                  detail information for a product, then reload the
//                  current detail.
//=======================================================================

void DetailWindow :: reload(Product* pProduct)
   {
   pcnrDetail->setRefreshOff();
   DetailObject* pTrailer = pProduct->detail();
   pcnrDetail->setSelected(pTrailer);
   pcnrDetail->setMultipleSelection();
   IContainerControl::ObjectCursor crs(*pcnrDetail);
   for ( crs.setCurrent(pTrailer); crs.isValid(); crs.setToNext() )
      {
      pTrailer = (DetailObject*)crs.current();
      if ( pTrailer->trailer() )
         break;
      pcnrDetail->setSelected(pTrailer);
      }
   pcnrDetail->deleteSelectedObjects();
   pProduct->setDetail((DetailObject*)NULL);
   add(pProduct, pTrailer);
   pcnrDetail->setSelected(pProduct->detail());
   pcnrDetail->removeObject(pTrailer);
   delete pTrailer;
   pcnrDetail->setSingleSelection();
   pcnrDetail->setRefreshOn();
   pcnrDetail->refresh(true);
   }


//=======================================================================
// FUNCTION NAME     saveDeatil()
//
// DESCRIPTION      Saves Datail Report to file - 47703 flw
//=======================================================================
void DetailWindow::saveDetail()
{
   IFileDialog::Settings settings;
   settings.setSaveAsDialog();
   //settings.setTitle(getString(IDS_SAVEDTLDIALOGTITLE));
   settings.setFileName( pProfile->strGeneratePath + STR_PathDelimiter + STR_DtlRptDefault );
   IFileDialog::Style fileStyle = IFileDialog::defaultStyle() |
                                  //* IFileDialog::helpButton; // ICA68115   HECC: 08/27/2002
                                  IFileDialog::explorer;       // ICA68115   HECC: 08/27/2002

   IFileDialog fd((IWindow*)desktopWindow(), (IWindow*)this, settings, fileStyle);
   IString t1 = fd.fileName();

   if( !numberOfObjects() )
      return;
   else
      {
      FormatPrintSave(t1);
      }
   return;
}


//=======================================================================
// FUNCTION NAME     PrintDetail()
//
// DESCRIPTION      Destructor for the DetailWindow - 44753 flw
//=======================================================================
void DetailWindow :: printDetail()
{
   if( !numberOfObjects() )
      return;
   else
      {
      FormatPrintSave(pPRTDTL_TEMP_FILE);
      }

   EUOPPrint prt;
   Boolean fOK;
   Boolean fPrinter = prt.init();
   if( fPrinter )
   {
      Boolean fOK = false;
      IString JobName = STR_DtlPrintJobName;
      if (fOK = prt.fSpool(pPRTDTL_TEMP_FILE,JobName, strPageBreak))
       {
         IMessageBox msg(this);
         IMessageBox::Style style = IMessageBox::okButton |
                                    IMessageBox::informationIcon |
                                    IMessageBox::moveable |
						IMessageBox::systemModal;
         msg.setTitle(IResourceId(IDS_DTL_PRNT_HDR));
         IString strText = LOADSTRING(IDS_DTL_MSG_SPOOL_JOB);
         strText.change(STR_Substitute0, (char *)JobName);
         strText.change(STR_Substitute1, (char *)prt.printerQueue());
         msg.show(strText, style, 0);
         }
      else if ( prt.lastErrorText().length())
            {
              IMessageBox msg(this);
              IMessageBox::Style style = IMessageBox::okButton |
                                         IMessageBox::informationIcon |
                                         IMessageBox::moveable |
						     IMessageBox::systemModal;
              msg.setTitle(IResourceId(IDS_DTL_PRNT_HDR));
              IString strText = prt.lastErrorText();
              msg.show(strText, style, 0);
             }
       }
      else
          {
              IMessageBox msg(this);
              IMessageBox::Style style = IMessageBox::okButton |
                                         IMessageBox::informationIcon |
                                         IMessageBox::moveable |
							IMessageBox::systemModal;
              msg.setTitle(IResourceId(IDS_DTL_PRNT_HDR));
              IString strText = LOADSTRING(IDS_DTL_NOPRINTERS);
              msg.show(strText, style, 0);
             }
   return;
}



//=======================================================================
// FUNCTION NAME     FormatPrintSave()
//
// DESCRIPTION      Format print and save file  - 47703 flw
//=======================================================================
void DetailWindow :: FormatPrintSave(IString t1)
{
    int lncnt = 0;
    IString strLabel, strQuantity, strStatus, strText;
    FILE* pTempFile = fopen(t1, "w");
    if( !pTempFile )
       return;

   fprintf( pTempFile,"\n\n%16s%-s", STR_Blank1, (IString)STR_DltHdrLn);
   fprintf( pTempFile,"\n%38s%-s", STR_Blank1, (IString)STR_DtlHdr);
   fprintf( pTempFile,"\n%16s%-s\n", STR_Blank1, (IString)STR_DltHdrLn);

   DetailObject* pDetail;
   pcnrDetail->setRefreshOff();
   IContainerControl::ObjectCursor crs(*pcnrDetail);
   for ( crs.setToFirst(); crs.isValid(); crs.setToNext() )
      {
      if (lncnt > 60)
         {
         fprintf( pTempFile, "\n%s", strPageBreak);
         lncnt = 0;
         }
      pDetail = (DetailObject*)crs.current();
      strLabel = pDetail->getLabel(pDetail);
      strQuantity = pDetail->getQuantity(pDetail);
      strStatus = pDetail->getStatus(pDetail);
      strText = pDetail->getText(pDetail);
      fprintf( pTempFile,"\n%15s   %-5s  %-30s   %-5s", strLabel,strQuantity,strText, strStatus);
      lncnt++;
      }

   pcnrDetail->setRefreshOn();
   pcnrDetail->refresh(true);  /* D49901 edw10/03/2000 */
   fclose(pTempFile);
}

//=======================================================================
// FUNCTION NAME     ~DetailWindow()
//
// DESCRIPTION      Destructor for the DetailWindow - 44753 flw
//=======================================================================
 DetailWindow::~DetailWindow()
 {
  delete pcnrDetail;
  }
