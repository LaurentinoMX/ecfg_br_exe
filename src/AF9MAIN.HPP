//=======================================================================
// FILE NAME      AF9MAIN.HPP
//
// CLASS NAME     MainWindow
//
// DESCRIPTION    This class is the main frame window of the Apply
//                Orders CPF.
// MODIFICATION LOG:
// 07/01/2002  EDW   D72623   Added prototype for cleanTempFiles method
// 07/08/2002  HECC  F72273   Added prototype for loadISeriesSWFeatureTable
//                            method and two new variables: bAS4 to know
//                            if the actual configuration is AS400 or not and
//                            the structure iSeriesSWTableSet with the
//                            information to reconcile SW features.
// 04/28/2003  HECC  F79548   Added definition of validProdFamilyPath() to verify
//                            if the given Product Family directory where the .TBL
//                            files will be picked up, exists  or not.
// 02/10/2004  HECC  D87694   Added function definition to change from long to short
//                            the path for a given filename.
// 10/15/2004  ZOR   F94021   add the 53 record under 07 record in the RET file    is commented becauseof 95887
// 07/26/2005  HECC  F100160  Add the necessary flags to check if the configuration
//                            belongs to Blue Horizon or xSeries family.
//=======================================================================
// Copyright (c) International Business Machines Corp. 1996
// All rights reserved
//=======================================================================

#ifndef AF9MAIN_HPP
#define AF9MAIN_HPP

#define INCL_NOBASEAPI
#define INCL_NOPMAPI
#include <os2.h>

#include <icmdhdr.hpp>                  // ICommandHandler
#include <icnrcol.hpp>                  // IContainerColumn
#include <icnrhdr.hpp>                  // ICnrHandler
#include <icnrmhdr.hpp>                 // ICnrMenuHandler
#include <ifilehdr.hpp>                 // IFileDialogHandler
#include <iframe.hpp>                   // IFrameWindow
#include <iframhdr.hpp>                 // IFrameHandler
#include <isizehdr.hpp>                 // IResizeHandler
#include <istring.hpp>                  // IString
#include <isrtset.h>                    // ISortedSet
#include "afccpuls.hpp"                 // CPU lookup table

class IAccelerator;
class IContainerControl;
class IDynamicLinkLibrary;
class IHelpWindow;
class IMenuBar;
class IResourceLibrary;
class IStaticText;

class BackEnd;
class Profile;
class CfReportFileStats;

class OrdersNotebook;
class SystemsNotebook;
class DetailWindow;
class FileNameDialog;
class OptionsDialog;
class RetrSource;
class Product;
class Mes;
class ReconcileData;               //47698 - flw
class Rtrnret;                     //53363 - zor


class MainWindow : public IFrameWindow, public ICommandHandler,
                   public ICnrHandler, public ICnrMenuHandler,
                   public IFileDialogHandler, public IResizeHandler,
                   public IFrameHandler
   {
   public:
   enum { enumPathBrowse=1, enumPathGenerate, enumFileConfig };
   enum { enumSortName=1, enumSortDate };
   enum { enumFileSave=1, enumFileGenerate, enumFileRename, enumFileName, enumFileName1 };
   //52012 include enumFileName1 MAMT 27/07/2000
   static IString strExecPath;          // Path to executable file
   MainWindow(ULONG ulWindow);
   BackEnd* pBackEnd;                   // Backend data manager
   IHelpWindow* helpWindow();           // Return pointer to help window
   IString strBrowsePath;               // Path for browsing files
   IString strGeneratePath;             // Path for generating files
   IString strSaveName;                 // File name to be saved
   IString oldfileB, oldfileR;          // File to keep the old BIR and RET
                                        // version with other extension 52012 MAMT
   IString tempfileB, tempfileR;        // File to keep the old BIR and RET version 52012 MAMT
   IString strGenerateName;             // File name to be generated
   IString strProductModel;             // Product model for current session
   IFrameWindow* activeWindow;          // Last window with focus
   ULONG ulReturnCode;                  // Return code to caller
   SHORT sSort;                         // Sort column
   Boolean bLock;                       // Disable conflicting actions
   Boolean bConfig;                     // Called by configurator
   Boolean bAscending;                  // Ascending sort sequence
   Boolean bResolve;                    // Resolve the data retrieved
   Boolean bSort;                       // ZOR IC01552
   Boolean bSascending;                 // ZOR IC01552
   Boolean bExpanded;                   // ZOR 48548
   Boolean bTile;                       // 47971 MAMT 21/08/2000
   Boolean bUseMRDB;                    // Use the retrieved MRDB data as the base configuration
   Boolean bUseAAS;                     // Use the retrieved AAS data as the base configuration
   Boolean bMultSysVPDTran;             // Retrieved data is from S/390 VPDTran CFReports
   Boolean bMultSysStackedMRDB;         // Retrieved data contains multiple systems
   Boolean bS390;                       // S/390 data retrieved from the host
   Boolean bAS4;                        // iSeries or AS400 data retrieved from the host HECC: 06/27/2002 F72273
   Boolean bFirstHWRec;                 // First HW record in either retrieved file recorded.
   Boolean bOnlyRename;                 // Flag to Only Rename option  49608 - MAMT
  // IString strMRD;                      // MRD filename ZOR 10/15/2004 F94021
   IString strMRDMulti;                 // MRD used to add multy system support R1570
   Boolean bInvRet;                     // Use The RET Modification for the IS function R1570
   Boolean bHostUsed;                   // Signal if host method was used R1570
   Boolean bIsBlueHor;                  // To check if the configuration is from Blue Horizon HECC:07/26/2005 F100160
   Boolean bIsXseries;                  // To check if it is a xSeries configuration  HECC:07/26/2005 F100160


   CfReportFileStats* pFile;            // Current file object
   CfReportFileStats* pSelectedFile;    // File selected for menu option
   IContainerControl* pcnrFiles;        // Client files container
   OrdersNotebook* pOrders;             // Orders notebook
   SystemsNotebook* pSystems;           // Systems notebook
   DetailWindow* pDetail;               // Product detail window
   FileNameDialog* pFileName;           // File name dialog
   ReconcileData* pRec;                 // Reconciliation dialog  47698 - flw
   Profile* pProfile;                   // User profile
   void openDetail(Product* pProduct, Boolean bLastObject=true); //EDS-chg 2-21
                                        // Open product detail window
   void openDetail(Mes* pMES, Boolean bLastObject=true );        //Open MES detail window
   void reloadDetail(Product* pProduct);// Refresh product detail window
   void setPointer(Boolean bBusy);      // Display the busy pointer
   USHORT loadNoCompareTable( IString strExecPath );             //edw07/07/1998
   ISortedSet<IString> noCompTableSet;                           //edw07/07/1998
   USHORT loadQuickFixTable( IString strExecPath );              //49222-MAMT-06/27/2000
   ISortedSet<IString> QuickFixTableSet;                         //49222-MAMT-06/27/2000
   ISortedSet<IString>* pFeature54Table;                        //53915-MAMT-08/31/2000

   ISortedSet<IString> iSeriesSWTableSet;                       //hecc 06/27/2002 F72273
   USHORT loadISeriesSWFeatureTable( IString strExecPath );     //hecc 06/27/2002 F72273

   void addMismatch( IString s );       // Add mismatch entry to strMismatches               //edw12/29/1999
   void addUserChoice( IString s );     // Add user decision entry to strUserChoices         //edw12/29/1999
   IString getMismatches(){ return strMismatches; }                                          //edw12/29/1999
                                        // Return reconciliation mismatches
   IString getUserChoices(){ return strUserChoices; }                                        //edw12/29/1999
                                        // Return reconciliation user choices
   Boolean validProdFamilyPath( IString strPath );     //F79548 - HECC 04/28/2003

   IString ConvertToShortPathName( IString strPath );  //D87694 - HECC: 04/10/2004

   protected:
   Boolean command(ICommandEvent& cmdEvt);
   Boolean systemCommand(ICommandEvent& cmdEvt);
   Boolean enter(ICnrEnterEvent& enterEvt);
   Boolean help(ICnrHelpEvent& helpEvt);
   Boolean makePopUpMenu(IMenuEvent& menuEvt);
   Boolean modelessResults(IFileDialog* endingDialog);
   Boolean windowResize(IResizeEvent& sizeEvt);
   Boolean activated(IFrameEvent& frmEvt);
   Boolean closed(IFrameEvent& frmEvt);
   Boolean validate(IFileDialogEvent& fdEvt);

   private:

   class MenuHandler : public IMenuHandler
      {
      public:
      MainWindow* pWindow;
      protected:
      Boolean menuShowing(IMenuEvent& menuEvt, ISubmenu& subMenu);
      };
   MenuHandler menuHandler;             // Subclass IMenuHandler

   Boolean bProfile;                    // Valid profile loaded
   Boolean bMinimized;                  // Main window is minimized
   Boolean bValid;                      // FileDialog file selection is valid
   USHORT usPath;                       // Path being selected
   IStaticText* ptxPath;                // Display browse path
   IMenuBar* pMenu;                     // Pull-down menu
   IAccelerator* pAccelerator;          // Accelerator table
   IHelpWindow* pHelp;                  // Help window
   OptionsDialog* pOptions;             // Options dialog
   RetrSource* pRetSrc;                 // Retrieval source dialog
   IContainerColumn* pcolOpen;          // Open icon container column
   IContainerColumn* pcolName;          // Name container column
   IContainerColumn* pcolDate;          // Date container column
   IContainerColumn* pcolSystems;       // Systems container column
   IContainerColumn* pcolProducts;      // Products container column
   IContainerColumn* pcolOrders;        // Orders container column
   IStaticText* ptxStatus;              // Status frame extension
   IStaticText* ptxProgress;            // Progress text
   IString strFirstHWRecType;           // Mach type of the first HW record in either retrieved file
   IString strLocalRetrFile;            // Local configuration file to use for retrieval
   IString getSensedDate(IString strMRD);     // Get sensed date of MRDB data                //edw12/04/1997
   Boolean hasVPDTransData(IString strCFR);   // Retrieved AAS data contains VPDTran data    //edw05/29/1998
   Boolean hasStackedMRDB(IString strMRD);    // Multiple MRDB reports are in retrieved data //edw05/29/1998
   IString createMRDBFile(IString strCFR);    // Move VPDTran data to a V20 CFReport         //edw05/29/1998
   Boolean resequenceInstIds(IString strMRD); // Resequence inst ids of stacked MRDB reports //edw06/05/1998
   IString getProductModel();                 // Get the ISM product model that is running   //edw06/18/1998
   Boolean regenerateSerialNbrs(IString);     // Generate unique SN's for products w/ SN's of zero //edw12/15/1998
   void openNotebooks();                // Open orders and systems notebooks
   void openFileName(USHORT usOption);  // Open file name dialog
   void openHost();                     // Open host retrieval window
   void openOptions();                  // Open options dialog
   void openRetrievalOptions();         // Open retrieval source dialog
   void openPrefsOptions();             // Open preference dialog                            // F62721 EDW 01/21/2002
   void openProductInfo();              // Open product info dialog
   Boolean findResources();             // Locate a resource DLL
   Boolean loadResources();             // Construct resources from DLL
   void listFiles();
   void sortFiles();                    // Sort files by name or date
   void saveFile();                     // Save updated copy of file
   void generateFile();                 // Generate output file
   void renameFile();                   // Rename BIR file
   void deleteFile();                   // Delete BIR file
   void deleteFile1();                  // Delete BIR file 52012 mamt 08/08/2000
   void loadProfile();                  // Load the user profile
   void selectDirectory(IString& str);  // Select default path
   Boolean confirmFileSave();           // Confirm discard of unsaved file
   Boolean confirmFileRetrieve();       // Confirm retrieval of new BIR
   void createColumn(IContainerColumn** ppColumn, ULONG ulOffset,
                     IContainerColumn::HorizontalAlignment align);
                                        // Add a column to the container
   void setHeadingText(IContainerColumn* pColumn, ULONG ulString);
                                        // Set two-line heading text
   void switchLocale();                 // Switch to new locale
   void returnRetrievalFile();          // Return .RET file to configurator
   void reformat();                     // Format dates for new locale
   void startUp1();                     // Start loading in separate thread
   void startUp2();                     // Finish loading in main thread
   void showProgress();                 // Show progress box in separate thread
   void cleanTempFiles(IString strBII, IString strCFR,
                      IString strMRT, IString strCFT); // remove temp files created in openHost // D72623 EDW 07/01/2002
   CpuList  myCpuList;                  // CPU lookup table w/ ISM model field
   IString strUserChoices;              // Users reconciliation choices                      //edw12/29/1999
   IString strMismatches;               // Reconciliation mismatches                         //edw12/29/1999
   };
#endif
