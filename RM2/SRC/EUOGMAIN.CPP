//==============================================================================
// FILE NAME                    EUOGMAIN.CPP
//
// MODIFICATIONS
//
//    DCG   02/11/02 IC51638    Added code to verify if a Generic Feature has
//                              more than one corresponding specific feature. If
//                              it does not have more than one, then the Generic
//                              Features window will not show up.
//    HECC  07/03/2002 F72273   Includes the new status definition ISeriesSWTblResolved
//                              in all the Feat objects of the iSeries SW features that
//                              were reconcilled.
//    HECC  07/18/2002 F73252   Changes to logic to improve usability for EMEA:
//                               a) Display new icons to show how features were resolved
//                               b) Display the Received date at IBM and Sensed date in
//                                  the Titles of the Matches and Mismatches windows
//                               c) Rows and columns of the Matches & Mismatches windows
//                                  were reduced by changing the font and its size to
//                                  "Arial 8" and the horizontal spacing for each row.
//                               d) The CRAD and Install dates were removed.
//                               e) The mouse click and keyboard actions to edit the
//                                  Base and Quantity container columns were changed, so
//                                  an object in the container will be opened for
//                                  editing with a mouse single click or Enter key.
//                               f) The keyboard actions were changed, so an edited
//                                  object in the container will be closed with a
//                                  mouse single click or Enter key.
//
//    EDW   07/25/2002 F73732   Completion of F72273.  Added call to 'Sort' at end
//                              of 'RMDiscRescW::load' function.  This will ensure that
//                              the objects in the containers are properly sorted after
//                              being loaded.
//    HECC  08/26/2002 ICA68115 No helpbutton and Windows explorer style in the
//                              'Save file' and 'Save As' FileDialogs
//    HECC  10/18/2002 F73686   Added necessary logic to handle two new parameters
//                              in the RPO2 class:  Machine description and
//                              Product Category.
//    HECC  07/28/2003 D83402   Added necessary logic to handle four new parameters
//                              in the RPO2 class: Customer number, System number flag,
//                              System type and System number.
//     ZOR  march-2004 F86752   Include the tag Resolved_IS on include status
//                              on SelectedChanged() and Editbase() checks the tag, because
//                              if the features has Resolved_IS then it can not be editable
//    RERC  01/30/2006 F104628  Added logic to read the Mismatch Type field from the
//                              nocomp.tbl and show the data into Mismatches Dialog,
//                              RET file and the Mismatches text report.
//    RERC  02/21/2006 D106441  Eliminate the definitions for the text of Mismatches in the
//                              EUOGMAIN.CPP and define that strings into AF9APPLY.H
//    HECC  03/09/2006 F106814  The sorting criteria in the Mismatches dialog must be changed
//                              so the features records be sorted by:
//                              Type-Mod-Serial-Mismatch Type-Feature
//    RERC  04/10/2006 F107280  Interchange the position of the columns AAS and MRDB
//                              on the output files: the text report of the Mismatches Dialog
//                              and the .RET file.
//    SAQP  06/12/2008 F124059  Added code to support 'exclude' flag on no compare table. FC
//                              excluded do not show on Mismatches Window. Req1506
//==============================================================================


#define INCL_WINSTDCNR       //   container control class
#define INCL_PM
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream.h>

#include "euogmain.hpp"
#include "euophlp.h"
#include "euogmain.h"
#include "xplnw.h"
#include "euomsg.h"
#include "euopids.h"
#include "mainw.hpp"                    //flw
#include "mainw.h"                      //flw
#include "euopiprt.hpp"                 //flw
#include "af9profl.hpp"
#include <ispinhdr.hpp>                 // ISpinHandler jt
#include <ispinnum.hpp>                 // ISpinButton, jt
#include <ifiledlg.hpp>                 // IFILEDIALOG, flw

#define MOUSECLICK_EXPAND      WM_USER+1

//edw01/03/2000
//ZOR IC65198
//ZOR 49221a  06/18/00

extern Boolean is1704; // take the varible from AF9MAIN.CPP REQ 1704
#define STR_Results0           "Serial numbers have been modified to use the machine reported value"   //REQ1704
#define STR_Results1           "*************************************************************"
#define STR_Results2           "RECONCILIATION MISMATCHES                                    "
#define STR_Results3           "-------------------------------------------------------------"
#define STR_Results4           "Rec.                                                         "
//F107280 Begin: RERC 04/10/2006 Interchange the MRDB/AAS columns into output file and RET file
//#define STR_Results5           "Qty AAS MRDB Type/Mod/ SN      /FC     Description           "
#define STR_Results5           "Qty MRDB AAS Type/Mod/ SN      /FC     Description           "
//F107280 End: RERC 04/10/2006
//F104628 Begin: RERC 01/30/2006  Add new text to the .RET file and the Mismatches text report
//#define STR_Results6           "These mismatches are critical and must be understood or resolved before proceeding."
//#define STR_Results7           "These mismatches are of less concern but should be understood before proceeding."
//F104628 End:   RERC 01/30/2006
//flw-44549  02/01/2000
#define STR_MMFileDefault      "Mismatch.txt"
#define STR_MMPrintJobName     "MISMATCH.PRT"
char *pPRTMM_TEMP_FILE =       "pPRTMM_TEMP_FILE";

// feature row constructor
CFRRow::CFRRow( Feat & f,IResourceId icon, EUOPProfile * pProf,
                RMDiscResoW * win)
              :IContainerObject( title(f, win->getSensedDate(), win->getReceivedDate()),
                                 icon)  //*F73252
              ,feat(f)
              ,strFeat(f.feature())
              ,strBase(f.qtyBase())
              ,strAAS(f.qtyAAS())
              ,strMismType( (f.mismType() == ACCEPTED_CHR)?  ACCEPTED_STR:          //F104628 RERC 30/01/2006
                          ( (f.mismType() == UNACCEPTED_CHR)? UNACCEPTED_STR:      //F104628 RERC 30/01/2006
                          ( (f.mismType() == EXCLUDED_CHR)? EXCLUDED_STR: "" ) ))    // F124059 SAQP 06/12/2008  This compare is only for debugging reasons
                                                                                     // taking away this checking won't change output
              ,strMRDB(f.qtyMRDB())
              ,strExpln(f.hasMessages()? STAR : BLANK)
              ,strKey(machineKey(f))
              ,fMachine(false)
              ,Mywin( win )
{
 featRowInit(pProf);
}

// feature row constructor
CFRRow::CFRRow( Feat & f,IPointerHandle hndl, EUOPProfile * pProf,
                RMDiscResoW * win)
              :IContainerObject( title(f, win->getSensedDate(), win->getReceivedDate()),
                                 hndl)  //*F73252
              ,feat(f)
              ,strFeat(f.feature())
              ,strBase(f.qtyBase())
              ,strAAS(f.qtyAAS())
              ,strMRDB(f.qtyMRDB())
              ,strExpln(f.hasMessages()? STAR : BLANK)
              ,strKey(machineKey(f))
              ,fMachine(false)
              ,Mywin( win )
{
 featRowInit(pProf);
}

void CFRRow:: re_Set(Feat & f,EUOPProfile * pProf)
{
 feat = f;
 strFeat = IString(f.feature());
 strBase = IString(f.qtyBase());
 strAAS  = IString(f.qtyAAS());
 strMRDB = IString(f.qtyMRDB());
 strExpln= IString(f.hasMessages()? STAR : BLANK);
 strKey  = machineKey(f);
 featRowInit(pProf);
}

void CFRRow::featRowInit(EUOPProfile * pProfile)
{
 IDate idate;
 feat.description(strDesc);
 strCRAD = (feat.isBackLog() && pProfile && feat.cradDate(idate))?
           pProfile->FormatNLSDateString(idate.monthOfYear(),
                                         idate.dayOfMonth(),
                                         idate.year() % 100) : IString();

 //F106814 Begin new logic: Added the MismatchType parameter, so the sorting criteria in the Mismatches window
 //changes as follows: from Type-Model-Serial-Feature  to Type-Model-Serial-Mismatch Type-Feature

 if (strMismType == UNACCEPTED_STR)
   strKey += BLANK + strMismType;  //Blank Needed to sort: Unaccepted features first and then Accepted features.
 else
   strKey += strMismType;
 //F106814 End new logic:  HECC  03/09/2006

 strKey  += strFeat;

 enableDataUpdate();
}

//*F73252 Changes to function to receive the parameters with the new information
//*to display: Date received at IBM and Date sensed, in the header of both Matches
//*and Mismatches windows.
//*HECC: 07/18/2002
IString CFRRow::title(Feat & f, IString strSensedDate, IString strReceiveDate )
{
IString str(f.machine());
 str += "/"; str += f.model();
 str += "   Serial# "; str += f.displaySerialNoStr();

 //* F73252 Adds to the first title line, the received at IBM date.
 //* Begin new logic. HECC: 07/18/2002

 str += NEW_LINE_CHAR;
 str += RECEIVE_DATE_STR;
 str += strReceiveDate;
 str += BLANK;
 str += DASH;
 str += BLANK;
 str += SENSED_DATE_STR;
 str += strSensedDate;

 //* F73252End new logic.

 return str;
}

// machine row constructor   //ZOR 48548
CFRRow::CFRRow(IResourceId icon,Feat & f, EUOPProfile * pProf,
               RMDiscResoW * win,XWindow * pParent)
              :IContainerObject( title(f, win->getSensedDate(), win->getReceivedDate()),
                                 icon)  //*F73252
              ,feat(f)
              ,strMachModel(f.machine())
              ,strExpand(f.isFeaturelessMachine()? FBLANK : FPLUS)
              ,strDesc(f.machineDescStr())
              ,strExpln(f.hasMachineMessages()? STAR : BLANK)
              ,strKey(machineKey(f))
              ,fMachine(true)
              ,Mywin( win )
{
 IDate idate;
 strMachModel += IString(SLASH) + IString(f.model());
 strDate = (pProf && f.MachineInstallDate(idate))?
           pProf->FormatNLSDateString(idate.monthOfYear(),
                                      idate.dayOfMonth(),
                                      idate.year() % 100) : IString();
 // ZOR 48548
 if (strExpand != FBLANK)
    {
    if (pParent->bExpan){
       strExpand = FMINUS;
       }
    }
 //ZOR 48548
}

void CFRRow::handleSelectedChange( IContainerControl * pCnr, Boolean fAcquired )
{
 if ( fAcquired ) {
   Mywin->setCntrTitle(this);
   Mywin->setMenuPer(this);
   }
}

void CFRRow::handleOpen( IContainerControl * pCnr)
{
 if (fMachine && (strExpand != FBLANK))
   Mywin->expand_Collapse(this);
}

IString CFRRow::machineKey( Feat & f)
{
 IString str(f.isFirstMRDBMachine()? "0" : "1");
 str += IString(f.machine()) + IString(f.model()) + IString(f.serialNo());
 return str;
}

IString CFRRow::machineKey()
{
 IString str(feat.isFirstMRDBMachine()? "0" : "1");
 str += IString(feat.machine()) + IString(feat.model()) +
        IString(feat.serialNo());
 return str;
}

/////////////////////////////////////////////////////////////////////////

int CFRRowSortUp :: isEqual (IContainerObject* p1,
                             IContainerObject* p2,
                             ICnrCtl * cnr) const
{
 return strcmp((char *)(((CFRRow *)p1)->keyStr()),
               (char *)(((CFRRow *)p2)->keyStr()));
}

///////////////////////////////////////////////////////////////////////////
Boolean ExpandFilter:: isMemberOf( IContainerObject* object,
                                   IContainerControl* container) const
{
 CFRRow * pRow = ( CFRRow * )object;
 if (pRow->isMachineRow()) {
   ((ExpandFilter *)this)->fExpandMachine = (pRow->expandStr() == FMINUS);
   return true;
   }
 return fExpandMachine;
}
///////////////////////////////////////////////////////////////////////

RMDiscResoW::RMDiscResoW( unsigned long windowID
                         ,const char * windowTitle
                         ,EUOPProfile * pUsrProf
                         ,XWindow * Parent
                         ,Boolean bS390
                         ,IHelpWindow  * pHW
                         ,XPLNWndw     * pXplainW
                         ,unsigned long closingMsg
                         ,RMDiscResoW * pResoWindow )
      :IFrameWindow( windowTitle
                    ,windowID
                    ,IFrameWindow::titleBar       |
                     IFrameWindow::systemMenu     |
                     IFrameWindow::maximizeButton |
                     IFrameWindow::hideButton     |
                     IFrameWindow::sizingBorder   |
                     IFrameWindow::accelerator    )
      ,BaseWnd(this,Parent,closingMsg,pHW)
      ,menuBar((pResoWindow? WND_DISC : WND_RESO),this)
      ,pParent(Parent)
      ,pResoW(pResoWindow)
      ,fAutoApply(true)
      ,fAllOrDisc(false)
      ,pUsrProfile( pUsrProf )
      ,pExpandObj(false)
      ,pXW(pXplainW)
      ,fMyXW(false)
      ,fEditing(false)
      ,pGenW(0)
      ,pRPO2Lst(0)
      ,pXPLN2Lst(0)
      ,pSysInfo(0)
      ,pProdInfo(0)
      ,pXList(0)
      ,pFList(0)
      ,pLeftCntCtlW(0)
      ,fAllowEdit(true)
      ,fS390(bS390)
      ,fEndEdit(0)      //F73252  HECC - 08/01/2002
{

 if ( fMyXW = ( pXW == 0) )
   pXW = new XPLNWndw( WND_XPLN, pXList, pHW);
 menuBar.disableItem(isDiscWindow()? MI_SELECTED_DISC : MI_SELECTED_RESO);
#if __IBMCPP__ == 300
 cnrCtl=new IContainerControl( (pResoWindow? CNR_DISC : CNR_RESO), this, this,
                               IRectangle(),
                               IContainerControl::defaultStyle() |
                               IContainerControl::noSharedObjects );
#else
 cnrCtl=new IContainerControl((pResoWindow? CNR_DISC : CNR_RESO),this,this,
                               IRectangle(),
                               IContainerControl::defaultStyle() |
                               IContainerControl::noSharedObjects |//jt1/21/98
                               IContainerControl::pmCompatible);//jt-chg
#endif
 setClient( cnrCtl );
 cnrCtl->enableTabStop();
 cnrCtl->showTitle().showTitleSeparator()
                    .setTitleAlignment( IContainerControl::left );

 //*F73252  Changes the font and its size of each row in the container to font 'Arial'
 //*with a size of 8 as the design document explains, decrease the line spacing
 //*between the rows in both, Matches and Mismatches rows.
 //* Begin new logic:  HECC  07/19/2002

   //Define variable parameters for font,and size that will be applied.
   IFont font;
   ULONG ulPoints = 8;
   IString sFont = ARIAL_FONT_STR;

   //Set the font and the size
   font = IFont(sFont, ulPoints);

   //Set the container font style to the font defined before.
   cnrCtl->setFont(font);

   // Adjust the icon size to the container font size.
   ULONG ulCharHeight = font.maxSize().height();
   cnrCtl->setIconSize(ISize(ulCharHeight, ulCharHeight));

   // Sets the vertical spacing between rows in Matches and Mismatches window to zero,
   // as this is minium possible value.
   ULONG ulSpaces = 0;
   cnrCtl->setLineSpacing(ulSpaces);

 //* End new logic:  HECC  07/19/2002

if (!helpWindow())
   menuBar.disableItem( MI_GENHELP ).disableItem( MI_HELP );

 colFlag=new IContainerColumn( CFRRow::expandOffset() );
 /*jt colIcon=new IContainerColumn( IContainerObject :: iconOffset(),
                               IContainerColumn::defaultHeadingStyle(),
                               IContainerColumn::icon |
                               IContainerColumn::alignVerticallyCentered );*/

 //colIcon = new IContainerColumn(IContainerColumn::isIcon);
 colIcon = new IContainerColumn(IContainerColumn::isIcon,
                                       IContainerColumn::defaultHeadingStyle(),
                                       IContainerColumn::icon);

 colMachModel=new IContainerColumn(CFRRow::machModelOffset());
 colMachModel->showSeparators().setHeadingText( MACH_STR );
 colBase=new IContainerColumn( CFRRow::baseOffset() );
 //edw12/16/98-SR6551 change title of Mismatches Cnr column from 'Base' to 'Quantity'
 if ( pResoWindow )
    colBase->setHelpId( IDH_PNL_DISCWND_CL_BASE )
         .enableDataUpdate()
         .showSeparators( IContainerColumn::horizontalSeparator )
         .justifyData( IContainerColumn::centeredVertically,
                       IContainerColumn::right )
         .justifyHeading( IContainerColumn::centeredVertically,
                          IContainerColumn::right )
         .setHeadingText( QTY_STR );
 else
    colBase->setHelpId( IDH_PNL_DISCWND_CL_BASE )
         .enableDataUpdate()
         .showSeparators( IContainerColumn::horizontalSeparator )
         .justifyData( IContainerColumn::centeredVertically,
                       IContainerColumn::right )
         .justifyHeading( IContainerColumn::centeredVertically,
                          IContainerColumn::right )
         .setHeadingText( BASE_STR );

 colExpl=new IContainerColumn( CFRRow::explnOffset() );
 colExpl->justifyData( IContainerColumn::centeredVertically,
                       IContainerColumn::left )
         .showSeparators( IContainerColumn::verticalSeparator );
 colMRDB=new IContainerColumn(CFRRow::mrdbOffset()  );
 colMRDB->showSeparators().setHeadingText( MRDB_STR );
 colAAS=new IContainerColumn( CFRRow::aasOffset() );
 colAAS->showSeparators().setHeadingText( AAS_STR );
 colFeature=new IContainerColumn( CFRRow::featOffset() );
 colFeature->showSeparators( IContainerColumn::horizontalSeparator )
            .setHeadingText( FEATURE_STR );
 colDesc=new IContainerColumn( CFRRow::descOffset( ));
 colDesc->showSeparators().setHeadingText( DESC_STR );

 //*F73252 Begin New logic: Both columns, CRAD and Install Date must be removed as
 //*design document requires.   HECC:  07/18/2002
 //*colCRAD=new IContainerColumn( CFRRow::cradOffset( ));
 //*colCRAD->showSeparators().setHeadingText( CRAD_STR );
 //*colDate=new IContainerColumn( CFRRow::dateOffset( ));
 //*colDate->showSeparators( IContainerColumn::horizontalSeparator )
 //*        .setHeadingText( IDATE_STR );

// add the columns to the container
cnrCtl->addColumn( colFlag )
        .addColumn( colIcon )
        .addColumn( colBase )
        .addColumn( colExpl )
        .addColumn( colMRDB )
        .addColumn( colAAS )
        .addColumn( colMachModel )
        .addColumn( colFeature )
        .addColumn( colDesc );
 //*      .addColumn( colCRAD )  F73252
 //*      .addColumn( colDate ); F73252
 //*F73252 End new logic
 //F104628 BEGIN.  RERC 30/01/2006
 //        If the Mismatches Dialog is for S390 then add 'Mismatch Type' column
 if ((pResoWindow) && (bS390))
     {
     colMachType=new IContainerColumn( CFRRow::machtypeOffset( ));
     colMachType->showSeparators().setHeadingText( MACHTYPE_STR );
     cnrCtl->addColumn( colMachType );
     }
 //F104628 END.  RERC 30/01/2006
 cnrCtl->setDeleteColumnsOnClose()
        .setDeleteObjectsOnClose()
        .setAutoDeleteObject();

 cnrCtl->showDetailsView()
        .enableDrawItem()
        .setMixedTargetEmphasis();

 ICnrEditHandler::handleEventsFor(cnrCtl);
 mle3DigitHdr.setCnrCtl(cnrCtl);           //*73252 HECC: 07/30/2002
 ICnrEditHandler::setMLEHandler(&mle3DigitHdr);
 ICnrHandler::handleEventsFor(cnrCtl);
 ICnrMenuHandler::handleEventsFor(cnrCtl);
 //jt IMouseHandler::handleEventsFor( cnrCtl );


//edw Containers do not have child windows in Win95, therefore attach the
//mouseclick handler to the entire window

 //*F73252 This change will enable the single mouse click event
 //* Begin new logic:  HECC  08/01/2002
 //*IMouseHandler::handleEventsFor(this);
 IMouseHandler::handleEventsFor(cnrCtl);
 //* End new logic:  HECC  08/01/2002


//Attach the mouseclick handler to the child windows of the container
//to handle the button1 singleclick to expand/collapse a machine
/*edw
 IWindow *pContainerKid = 0;
 IWindow::ChildCursor cursor( *cnrCtl );
 for (cursor.setToFirst(); cursor.isValid() && !pLeftCntCtlW; cursor.setToNext())
   {
   if ((pContainerKid = new IWindow( childAt(cursor))) &&
       pContainerKid->id() == CID_LEFTDVWND)
     IMouseClickHandler::handleEventsFor( pLeftCntCtlW = pContainerKid);
   else delete pContainerKid;

   }
edw*/
 //edw07/01/1998-Disable editting base if S/390 user
 if ( fS390 )
    fAllowEdit = false;
 SetDisabledPointer(ISystemPointerHandle(ISystemPointerHandle::wait));
 cnrCtl->show();
}

void RMDiscResoW :: Sort(void)
{
 CFRRowSortUp sortUp;
 cnrCtl->sort(sortUp);
}

int RMDiscResoW::iconFor(Feat &f)
{
 //* F73252 Begin new logic. As this function will now be used to display an
 //* icon to show how feature was resolved, it has to be completely redefined
 //*return f.isGeneric()? ICO_4007 :
 //*       (f.isBackLog()? ICO_4005 : (f.qtyAAS()? ICO_4006 : ICO_4008));

if ( !isDiscWindow() )
   return (f.qtyBase() == 0)? 0 :      //* Item  will not be included in the base
           ( (f.qtyBase() == f.qtyMRDB())? ICO_4013 :   //* Resolved with MRDB Qty
             ((f.qtyBase() == f.qtyAAS())? ICO_4014 :   //* Resolved with AAS Qty
                                           ICO_4015) ); //* Resolved with a user-entered value
else
   return 0;
 //* End new logic
}

void RMDiscResoW :: addRow(Feat &f)
{
 CFRRow * pR = new CFRRow(f,IResourceId(iconFor(f)),pUsrProfile,this);
 addRow(pR);
 delete pR;
}
void RMDiscResoW :: addRow(CFRRow *pR)
{
 //* F73252 Get id of icon used to resolve feature
 int iBase = iconFor(pR->rowsFeat());

 Boolean f;
 if (cnrCtl && pR) {
   if (f = cnrCtl->isRefreshOn())
     cnrCtl->setRefreshOff();
   //edw12/5/97-Don't display compass-like icons per customer request.  I do this
   //by sending an icon id of zero when constructing the feature row objects.
   //cnrCtl->addObject(new CFRRow( pR->rowsFeat(),pR->icon(),pUsrProfile,this));

   //* F73252 Add an icon to the row which indicates how the feature is being resolved
   cnrCtl->addObject(new CFRRow( pR->rowsFeat(),iBase,pUsrProfile,this));

   if (!containsMachineRowFor(*pR))
     cnrCtl->addObject( new CFRRow(ICO_4010,pR->rowsFeat(),pUsrProfile,this,pParent)); //ZOR 48548
   Sort();
   if (f)
     cnrCtl->setRefreshOn();
   }
}

void RMDiscResoW :: removeRow(CFRRow *pR)
{
 cnrCtl->removeObject( pR );
 if (pR = machineWithoutFeatureRows(pR->machineKey()))
   cnrCtl->removeObject( pR );
 handleEmptyContainer();
}

void RMDiscResoW :: addMachineRows(FeatList & FL)
{
 FeatList::Cursor csr(FL);
 Boolean f = cnrCtl->isRefreshOn();
 if (f) cnrCtl->setRefreshOff();
 forCursor(csr)
   cnrCtl->addObject(new CFRRow(ICO_4010,FL.elementAt(csr),pUsrProfile,this,pParent));  //ZOR 48548
 Sort();
 if (f) cnrCtl->setRefreshOn();
}

//assumes objects are sorted up
Boolean RMDiscResoW :: containsMachineRowFor(CFRRow & R)
{
 IString key(R.machineKey());
 Boolean fFound = false;
 IContainerControl::ObjectCursor crsr( *cnrCtl ) ;
 forCursor(crsr) {
   const IString & key2 = ((CFRRow *)(cnrCtl->objectAt(crsr)))->keyStr();
   if ( (fFound = (key == key2)) || (key < key2))
     break;
   }
 return fFound;
}

CFRRow * RMDiscResoW :: locateRowFor(Feat & f)
{
 IString key(CFRRow::machineKey(f) + IString(f.feature()));
 CFRRow * pObj = 0;
 Boolean fFound = false;
 IContainerControl::ObjectCursor crsr( *cnrCtl ) ;
 forCursor(crsr) {
   pObj = (CFRRow *)(cnrCtl->objectAt(crsr));
   const IString & key2 = pObj->keyStr();
   if ( (fFound = (key == key2)) || (key < key2))
     break;
   }
 return fFound? pObj : 0;
}

CFRRow * RMDiscResoW :: machineWithoutFeatureRows(const IString & MKey)
{
 Boolean fFound = false;
 CFRRow * pObj, * pMObj = 0;
 IContainerControl::ObjectCursor crsr( *cnrCtl ) ;
 IString key2;
 forCursor(crsr) {
   pObj = (CFRRow *)(cnrCtl->objectAt(crsr));
   if (!pMObj) {
     if (pObj->isMachineRow()) {
       const IString & key = pObj->keyStr();
       if ( MKey == key )
         pMObj = pObj;
       else if (MKey < key)
               break;
       }
     }
   else {
     key2 = pObj->machineKey();
     if ( (fFound = (MKey == key2)) || (MKey < key2))
       break;
     }
   }
 return (fFound? 0 : pMObj);
}

Boolean RMDiscResoW::handleEmptyContainer()
{
 if (!cnrCtl->objectCount()) {
   cnrCtl->setTitle( BLANK );
   if (isDiscWindow()) {
     pParent->SetResolvedState();
     menuBar.disableItem( MI_SELECTED_DISC );
     menuBar.disableItem( MI_MIS_FILE );
     }
   else menuBar.disableItem( MI_SELECTED_RESO );
   return true;
   }
 return false;
}

void RMDiscResoW::setMenuPer(CFRRow * pObj)
{
 if (isDiscWindow()) {
   if (pObj)
     if (pObj->isMachineRow()) {
       if (pObj->explainStr() == STAR) {
         menuBar.enableItem( MI_SELECTED_DISC )
                .enableItem( MI_EXPL )
                .disableItem( MI_EDISCBASE )
                .disableItem( MI_DISC_APPLY );
         }
       else  menuBar.disableItem( MI_SELECTED_DISC );
       }
     else {
       menuBar.enableItem( MI_SELECTED_DISC );
       if (pObj->explainStr() == STAR)
         menuBar.enableItem( MI_EXPL );
       else menuBar.disableItem( MI_EXPL );
       if (fAllowEdit)
         menuBar.enableItem( MI_EDISCBASE );
       else menuBar.disableItem( MI_EDISCBASE );
       }
   else  menuBar.disableItem( MI_SELECTED_DISC );
   }
 else {
   if (pObj)
     if (pObj->isMachineRow()) {
       if (pObj->explainStr() == STAR) {
         menuBar.enableItem( MI_SELECTED_RESO )
                .enableItem( MI_EXPL )
                .disableItem( MI_ERESOBASE );
         }
       else  menuBar.disableItem( MI_SELECTED_RESO );
       }
     else {
       menuBar.enableItem( MI_SELECTED_RESO );
       if (pObj->explainStr() == STAR)
         menuBar.enableItem( MI_EXPL );
       else menuBar.disableItem( MI_EXPL );
       if (fAllowEdit)
         menuBar.enableItem( MI_ERESOBASE );
       else menuBar.disableItem( MI_ERESOBASE );
       }
   else  menuBar.disableItem( MI_SELECTED_RESO );
   }
}

void RMDiscResoW::filter(Boolean fScroll)
{
 IContainerObject * pObj = fScroll? cnrCtl->cursoredObject() : 0;
 cnrCtl->filter(expandFilter);
 if (pObj)
   cnrCtl->scrollToObject(pObj);  // pmCompatible will cause it to hung
}

Boolean RMDiscResoW::help(ICnrHelpEvent& evt)
{
 if (helpWindow())
    helpWindow()->show(IHelpWindow::general);
 //jt else WinAlarm(HWND_DESKTOP,WA_ERROR);
 else MessageBeep(MB_ICONEXCLAMATION);  //jt

 return true;
}

Boolean RMDiscResoW::userMessage(IEvent& evt)
{
 switch(evt.eventId()) {
   case XPLNW_CL_MSG_DISC:
   case XPLNW_CL_MSG_RESO:
     pXW->hide();
     setFocus();
     return true;
   case MOUSECLICK_EXPAND: {
     CFRRow * p = pExpandObj;
     pExpandObj = 0;
     expand_Collapse(p);
     return true;
     }
   }
 return false;
}

Boolean RMDiscResoW::includeStatus( Feat::Status status )
{
 if (isDiscWindow())
   if (fAutoApply)
     switch (status) {
       case Feat::Unresolved: return true;
       }
   else switch (status) {
          case Feat::Resolved_EUOP:
          case Feat::ISeriesSWTblResolved:    /* F72273   HECC: 07/03/2002 */
          case Feat::Resolved_IS:             /*ZOR F86752 req 1373*/
          case Feat::Unresolved: return true;
          }
 else if (fAutoApply)
        if (fAllOrDisc)
          switch (status) {
            case Feat::Unresolved: return false;
            default: return true;
            }
        else switch (status) {
               case Feat::ISeriesSWTblResolved:    /* F72273   HECC: 07/03/2002 */
               case Feat::Resolved_IS:             /*ZOR F86752 req 1373*/
               case Feat::Resolved_User: return true;
            }
      else if (fAllOrDisc)
             switch (status) {
               case Feat::Resolved_EUOP:
               case Feat::ISeriesSWTblResolved:    /* F72273   HECC: 07/03/2002 */
               case Feat::Resolved_IS:             /*ZOR F86752 req 1373*/
               case Feat::Unresolved: return false;
               default: return true;
               }
          else switch (status) {
                 case Feat::ISeriesSWTblResolved:    /* F72273   HECC: 07/03/2002 */
                 case Feat::Resolved_IS:             /*ZOR F86752 req 1373*/
                 case Feat::Resolved_User: return true;
                 }
  return false;
}

Boolean RMDiscResoW :: updated(EUOPProfile * pProf)
{
 pUsrProfile = pProf;
 if (fAllOrDisc != pProf->GetBooleanKey( EUOPProfile::DsplyAll))
   return load(pFList,pRPO2Lst,pXPLN2Lst,pSysInfo,pProdInfo);
 return false;
}

//* F73252 Begin new logic: Two new parameters strSensedDate & strReceiveDate are
//* to be received by the RMDiscResoW class so they can be displayed in both, the
//* Matches & Mismatches windows when these dialogs be populated  HECC; 07/18/2002
Boolean RMDiscResoW::load( FeatList * pFL, RPO2List * pRPOL,
                                   XPLN2List *pEXPL,RMSystem * pSI,
                                   ProductKnowledge *pPK,
                                   IString strSensedDatePar, IString strReceiveDatePar, //*F73252
                                   Boolean fAlsoResoW)

//* End New logic.

{
 if (!pFL || !pRPOL || !pEXPL || !pSI || !pPK)
   return false;
 IString lastKey,thisKey;
 FeatList::Cursor csr( *pFL );

 pFList = pFL;
 pRPO2Lst = pRPOL;
 pXPLN2Lst = pEXPL;
 pSysInfo = pSI;
 pProdInfo = pPK;

 //* F73252 Assigns the received parameters to the global variables of the class
 //* RMDiscResoW.   Begin  HECC: 07/18/2002
 strSensedDate  = strSensedDatePar;
 strReceiveDate = strReceiveDatePar;
 //* End  HECC: 07/18/2002

 fAllOrDisc = pUsrProfile->GetBooleanKey( EUOPProfile::DsplyAll);
 Able(false);
 cnrCtl->setRefreshOff();
 cnrCtl->deleteAllObjects();
 if ( (  !pFL->isEmpty() ) ) {
   forCursor(csr) {
     Feat & f = pFL->elementAt(csr);
     // F124059 if feature is marked as exclude, it is not shown on Mismatches Window
     if (f.mismType() != EXCLUDED_CHR)      // F124059 SAQP 06/12/2008
      if (includeStatus( f.status())) {
       thisKey = CFRRow::machineKey(f);
       if (thisKey != lastKey) {
         cnrCtl->addObject( new CFRRow(ICO_4010,f,pUsrProfile,this,pParent) );  // ZOR 48548
         lastKey = thisKey;
         }
       //edw12/5/97-Don't display compass-like icons per customer request.  I do this
       //by sending an icon id of zero when constructing the feature row objects.
       //cnrCtl->addObject(new CFRRow(f,IResourceId(iconFor(f)),pUsrProfile,this));

       //* F73252 Begin new logic. New icons will be displayed to show how
       //* feature was resolved, so the call to the function iconFor(f) will be
       //* active again.
       //* HECC:  07/17/2002
       //*cnrCtl->addObject(new CFRRow(f,IResourceId(0),pUsrProfile,this));
        cnrCtl->addObject( new CFRRow(f,IResourceId(iconFor(f)),pUsrProfile,this) );
       //* End new logic

       }
     }
   Sort();  /* F73732 EDW 07/25/02 */
   filter();
   }
 Able(true); //jt
 cnrCtl->setRefreshOn();
 cnrCtl->refresh();
 handleEmptyContainer();
 Able(true);
 if (isDiscWindow() && fAlsoResoW )
   pResoW->load( pFL, pRPOL, pEXPL, pSI,pPK);
 return true;
}

//edw07/01/1998   Modified to accept boolean argument 10/19/99
void RMDiscResoW::autoResolve(boolean bUseCurrentBase)
{
 CFRRow* pObj = 0;
 setFocus();
 IContainerControl::ObjectCursor crs( *cnrCtl );
 forCursor( crs )
    {
    pObj = (CFRRow* )cnrCtl->objectAt( crs );
    if ( pObj && pObj->isMachineRow() )
       if ( pObj->expandStr() == FPLUS )
          {
          expand_Collapse( pObj );    //Expands the machine which invalidates the cursor
          crs.setToFirst();           //Needed to re-validate the cursor
          }
    }
 forCursor( crs )
    {
    pObj = (CFRRow* )cnrCtl->objectAt( crs );
    if ( pObj && !pObj->isMachineRow() )
        {
        if( !bUseCurrentBase )       //edw10/19/99
           pObj->setBaseQty( (IString)pObj->qtyAAS() );
        applyDisc( pObj );           //Remove object from mismatches container which invalidates the cursor
        crs.setToFirst();            //Needed to re-validate the cursor
        }
    }
}
/**************************************************************************************************
   F107280 RERC 04/10/2006 The next function has changed the logic so this function
                           is replaced for other with the same name
 **************************************************************************************************
//edw01/03/2000
void RMDiscResoW::recordMismatches()
{
 if( !numberOfObjects() )
    return;
 else
 {
    pParent->pMain->addMismatch( (IString)STR_Results1 );
    pParent->pMain->addMismatch( (IString)STR_Results2 );
    pParent->pMain->addMismatch( (IString)STR_Results3 );
    pParent->pMain->addMismatch( (IString)STR_Results4 );
    pParent->pMain->addMismatch( (IString)STR_Results5 );  //ZOR IC65198
 }

 CFRRow* pObj = 0;
 IString sTemp;
 char pszTemp[256];
 setFocus();
 IContainerControl::ObjectCursor crs( *cnrCtl );
 forCursor( crs )
    {
    pObj = (CFRRow* )cnrCtl->objectAt( crs );
    if ( pObj && pObj->isMachineRow() )
       if ( pObj->expandStr() == FPLUS )
          {
          expand_Collapse( pObj );    //Expands the machine which invalidates the cursor
          crs.setToFirst();           //Needed to re-validate the cursor
          }
    }
 Boolean bHeaderAU = FALSE; //F104628 RERC 01/30/2006
 forCursor( crs )
    {
    pObj = (CFRRow* )cnrCtl->objectAt( crs );

    //F104628 RERC  The next code was replaced to the logic to add a new format to .RET
    //              file that is below of next lines commented.
    if ( pObj && !pObj->isMachineRow() )
        {
        //ZOR IC65198
        //ZOR 49221a 06/18/00
        //IString serialNum= pObj->serialNo();
        //serialNum= serialNum.subString(5);

     //F104628 RERC Code to write the Unacceptable Features and its header
     if ( pObj && !pObj->isMachineRow() && ( pObj->mismTypeAU() == UNACCEPTED_CHR ))
        {
           if (!bHeaderAU)
               {
                 pParent->pMain->addMismatch( BLANK );
                 //pParent->pMain->addMismatch( (IString)STR_Results6 );
                 pParent->pMain->addMismatch( (IString)STR_REC05_TextUnacceptable );//D106441
                 bHeaderAU = TRUE;
               }
        sprintf( pszTemp, "%-4d %-3d %-3d %-4s %-3s %-4s %-4s %-s",
                        pObj->qtyBaseFeat(), pObj->qtyAAS(), pObj->qtyMRDB(),
                        pObj->machine(), pObj->model(), pObj->serialNo(), pObj->feature(), pObj->description() );
        pParent->pMain->addMismatch( pszTemp );

        }
    }
    //F104628 BEGIN.  RERC  30/01/2006
 //                Write the Accepted features in the .RET file
 bHeaderAU = FALSE;
 forCursor( crs )
    {
    pObj = (CFRRow* )cnrCtl->objectAt( crs );
    if ( pObj && !pObj->isMachineRow() && pObj->mismTypeAU() == ACCEPTED_CHR )
        {
           if (!bHeaderAU)
               {
                 pParent->pMain->addMismatch( BLANK );
                 //pParent->pMain->addMismatch( (IString)STR_Results7 );
                 pParent->pMain->addMismatch( (IString)STR_REC05_TextAcceptable ); //D106441
                 bHeaderAU = TRUE;
               }
        sprintf( pszTemp, "%-4d %-3d %-3d %-4s %-3s %-4s %-4s %-s",
                        pObj->qtyBaseFeat(), pObj->qtyAAS(), pObj->qtyMRDB(),
                        pObj->machine(), pObj->model(), pObj->serialNo(), pObj->feature(), pObj->description() );
        pParent->pMain->addMismatch( pszTemp );
        }
    }
    //F104628 END.  RERC  30/01/2006
       bHeaderAU = FALSE;
       forCursor( crs )
          {
            pObj = (CFRRow* )cnrCtl->objectAt( crs );
            if ( pObj && !pObj->isMachineRow()&& pObj->mismTypeAU() == NULL )
               {
                  if (!bHeaderAU)
                     {
                       pParent->pMain->addMismatch( BLANK );
                       bHeaderAU = TRUE;
                     }
                 sprintf( pszTemp, "%-4d %-3d %-3d %-4s %-3s %-4s %-4s %-s",
                        pObj->qtyBaseFeat(), pObj->qtyAAS(), pObj->qtyMRDB(),
                        pObj->machine(), pObj->model(), pObj->serialNo(), pObj->feature(), pObj->description() );
                 pParent->pMain->addMismatch( pszTemp );
               }
          }

 pParent->pMain->addMismatch( (IString)STR_Results1 );
}
***************************************************************************************************/

//=================================================================================================
//
// FUNCTION:                 recordMismatches()
// MODIFY:
// F107280 RERC 04/10/2006   Create the .RET file from the mismatches dialog.
//=================================================================================================
void RMDiscResoW::recordMismatches()
{
 if( !numberOfObjects() )
    return;
 else
 {
    if(is1704==true)
    pParent->pMain->addMismatch( (IString)STR_Results0 );     //REQ1704

    pParent->pMain->addMismatch( (IString)STR_Results1 );
    pParent->pMain->addMismatch( (IString)STR_Results2 );
    pParent->pMain->addMismatch( (IString)STR_Results3 );
    pParent->pMain->addMismatch( (IString)STR_Results4 );
    pParent->pMain->addMismatch( (IString)STR_Results5 );
 }

 CFRRow* pObj = 0;
 IString sTemp;
 setFocus();
 IContainerControl::ObjectCursor crs( *cnrCtl );
 forCursor( crs )
    {
    pObj = (CFRRow* )cnrCtl->objectAt( crs );
    if ( pObj && pObj->isMachineRow() )
       if ( pObj->expandStr() == FPLUS )
          {
          expand_Collapse( pObj );    //Expands the machine which invalidates the cursor
          crs.setToFirst();           //Needed to re-validate the cursor
          }
    }

    printMismatchesColumns(UNACCEPTED_CHR ); // Print the critical features
    printMismatchesColumns(ACCEPTED_CHR );   // Print the acceptable features
    printMismatchesColumns(NULL );           // Print the features that are not classify
                                             // for critical and/or Acceptable

    pParent->pMain->addMismatch( (IString)STR_Results1 );
}

//=================================================================================================
//
// FUNCTION:                 printMismatchesColumns(char chrMismatchType)
//                           char chrMismatchType - Mismatch Type of the feature ('A', 'U')
//
// MODIFY:
// F107280 RERC 04/10/2006   Print the mismatches columns on RET file
//=================================================================================================
void RMDiscResoW::printMismatchesColumns(char chrMismatchType)
{
 CFRRow* pObj = 0;
 char pszTemp[256];
 setFocus();
 IContainerControl::ObjectCursor crs( *cnrCtl );
 Boolean bHeaderAU = FALSE;
 IString strText;

 // According to the value of Mismatch Type is assigned the text.
 if (chrMismatchType == UNACCEPTED_CHR)
 {
  strText = STR_REC05_TextUnacceptable;
 }
 else
  if (chrMismatchType == ACCEPTED_CHR)
  {
     strText = STR_REC05_TextAcceptable;
  }
  else
     strText = BLANK;

 forCursor( crs )
    {
    pObj = (CFRRow* )cnrCtl->objectAt( crs );

    if ( pObj && !pObj->isMachineRow() && ( pObj->mismTypeAU() == chrMismatchType ))
        {
           if (!bHeaderAU)
               {
                 pParent->pMain->addMismatch( BLANK );
                 if (strText != BLANK)
                    pParent->pMain->addMismatch( (IString)strText );
                 bHeaderAU = TRUE;
               }
           sprintf( pszTemp, "%-4d %-3d %-3d %-4s %-3s %-4s %-4s %-s",
                        pObj->qtyBaseFeat(), pObj->qtyMRDB(), pObj->qtyAAS(),
                        pObj->machine(), pObj->model(), pObj->serialNo(), pObj->feature(), pObj->description() );
           pParent->pMain->addMismatch( pszTemp );
        }
    }
}

RMDiscResoW::~RMDiscResoW()
{
 if ( pGenW )    delete pGenW;
 if ( pXList )   delete pXList;
 if (pLeftCntCtlW) delete pLeftCntCtlW;
 if (fMyXW && pXW) delete pXW;
}

void RMDiscResoW::createExplainList( CFRRow * cnrObj )
{
 if ( !pXList )
   pXList = new XPLN2List;
 else pXList->removeAll();
 if (pXPLN2Lst)
   pXPLN2Lst->allElementsFor( *pXList, cnrObj->machine(), cnrObj->model(),
                              cnrObj->serialNo(), cnrObj->feature() );
}

//******************************************************************************
// This function is called whenever there is a change in selection emphasis of
// a container object. This is called before handleSelectedChange().
//******************************************************************************
Boolean  RMDiscResoW::selectedChanged(ICnrEmphasisEvent& event)
{
 CFRRow * pObj = (CFRRow *)event.object();
 if (pExpandObj && cnrCtl->isSelected(event.object()) && pExpandObj == pObj)
   handle().postEvent(MOUSECLICK_EXPAND);
 else
   {
   Feat feat1(pObj->rowsFeat());    // F86752 1373
   if(feat1.pF->tag() != MFeat::Resolved_IS )     //ZOR F86752 1373
     {
      pObj->handleSelectedChange(cnrCtl,true);

   //*F73252 After an object is selected (with a single left-click or the up/down arrow
   //*       keys) it must be opened for editing.
   //* Begin: HECC: 08/01/2002
      cnrCtl->editObject( pObj, colBase);

   //* To select the edit field so once it is opened for editing, the first pressed
   //* key will cause the old content of the field be automatically deleted.
      if (cnrCtl->currentEditMLE())
       cnrCtl->currentEditMLE()->selectRange();
   //* End: HECC: 08/01/2002
     } // 86752
   }

 return true;
}

void RMDiscResoW::expand_Collapse(CFRRow * pR)
{
 if (pR && pR->isMachineRow() && (pR->expandStr() != FBLANK)) {
#if __IBMCPP__ == 300
   IPointerHandle hdl(mousePointer());
   setMousePointer(ISystemPointerHandle(ISystemPointerHandle::wait));
#else
   //jt HPOINTER hptr = WinQueryPointer(HWND_DESKTOP);
   //jt WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP,SPTR_WAIT,false));
#endif
   pR->setExpandStr((pR->expandStr() == FMINUS)? FPLUS : FMINUS);
   setCntrTitle(pR);
   filter();
#if __IBMCPP__ == 300
   setMousePointer(hdl);
#else
   //jt WinSetPointer(HWND_DESKTOP,hptr);
#endif
   }
}

void RMDiscResoW::setCntrTitle(CFRRow * pObj)
{
 cnrCtl->setTitle(pObj? pObj->iconText() : IString(BLANK));
}

Boolean RMDiscResoW::mouseClicked( IMouseClickEvent& evt )
{
 if ( evt.mouseButton() == IMouseClickEvent::button1 )
   if ( evt.mouseAction() == IMouseClickEvent::doubleClick )
      applyDisc((CFRRow *)cnrCtl->cursoredObject());

   //*F73252 If a mouse single click event occurs over a feature that is not
   //        being edited in the container, this event will open it for editing.
   //        if the same event occurs when the container is being edited, do
   //        nothing and continue the process to end the edition of the container.
   // Begin new logic.   HECC: 07/26/2002
   else if ( evt.mouseAction() == IMouseClickEvent::click)
           {
           if (!fEndEdit)
              editBase();
           else
              fEndEdit = false;
           }
   // End new logic.   HECC: 07/26/2002

   else if ( evt.mouseAction() == IMouseClickEvent::up ) {
           IContainerColumn * pCol=cnrCtl->columnUnderPoint(evt.mousePosition());
           pExpandObj = (CFRRow *) cnrCtl->objectUnderPoint( evt.mousePosition() );
           if (pExpandObj &&  ( pCol == colFlag ) && pExpandObj->isMachineRow() &&
              ( pExpandObj->expandStr() != FBLANK ) ) {
              if (cnrCtl->isSelected(pExpandObj))
                 handle().postEvent(MOUSECLICK_EXPAND);
      //         else 'selectedChanged() will handle it
              }
           else pExpandObj = 0;
           }

 return( false );
}

void RMDiscResoW::editBase()
{
 IContainerObject * pObj = cnrCtl->cursoredObject();
 CFRRow * pCFRObj = (CFRRow * )cnrCtl->cursoredObject() ;       //F86752 req 1373
 Feat feat1(pCFRObj->rowsFeat());    //F86752 req 1373
 if(feat1.pF->tag() != MFeat::Resolved_IS )     //ZOR F86752 req  1373
  {
  if (pObj)
    {
    cnrCtl->editObject( pObj, colBase);
    //*F73252  Selects the edit field so once it is opened for editing, the first pressed
    //*        key will cause the old content of the field be automatically deleted.
    //* Begin new logic:  HECC- 07/31/2002
    if (cnrCtl->currentEditMLE())
        cnrCtl->currentEditMLE()->selectRange();
    //* End new logic:  HECC- 07/31/2002
     }
  } //86752
}

Boolean RMDiscResoW::beginEdit( ICnrBeginEditEvent & evt)
{
 ICnrEditHandler::beginEdit(evt);
 fEditing = true;
 return true;
}

Boolean RMDiscResoW::endEdit( ICnrEndEditEvent & evt)
{
 CFRRow * pObj = (CFRRow *)evt.object();

 if (pObj) {
   IString text(evt.newText());
   ICnrEditHandler::endEdit(evt);
   if (text.length())
     pObj->setBaseQty(text);
   else {
     pObj->setBaseQty(IString("0"));
     cnrCtl->refresh();
     }

   //edw01/22/98-if clause add to update RPO list when base is edited in Reso Window
   if ( !isDiscWindow() )
      {
      updateRPOCollection ( pObj );

      //*F73252 After the edition of the container ends, Icon must be changed to show
      //*       if the Base quantity was set by the user.
      //*Begin new logic:  HECC- 07/31/2002
      pObj->setIcon(iconFor(pObj->rowsFeat()));
      cnrCtl->refresh();
      }
   //*End new logic:  HECC- 07/31/2002

   fEndEdit = true;

   }
 fEditing = false;
 return true;
}

Boolean RMDiscResoW::enter( ICnrEnterEvent & evt)
//*_F73252. New method. Catches an 'Enter pressed' event when it occurs and starts the
//  edition of the container.
 {
    if ( evt.enterPressed() )
         editBase();
    return true;
 }


Boolean RMDiscResoW::makePopUpMenu( IMenuEvent& evt )
{
 IPopUpMenu *pPopUp = 0;
 if (fEditing)
   cnrCtl->closeEdit();
 CFRRow * pObj = (CFRRow * )cnrCtl->cursoredObject();
 if ( pObj )
   if (isDiscWindow()) {
     if ( pPopUp = new IPopUpMenu( MI_POPMENU_DISC, evt.window())) {
       if (pObj->explainStr() == STAR)
         pPopUp->enableItem( MI_EXPL );
       else pPopUp->disableItem( MI_EXPL );
       if (!pObj->isMachineRow()) {
         if (fAllowEdit)
           pPopUp->enableItem( MI_EDISCBASE );
         else pPopUp->disableItem( MI_EDISCBASE );
         pPopUp->enableItem( MI_DISC_APPLY );
         }
       else pPopUp->disableItem( MI_EDISCBASE ).disableItem( MI_DISC_APPLY);
       }
     }
   else if ( pPopUp = new IPopUpMenu( MI_POPMENU_RESO, evt.window())) {
          if (pObj->explainStr() == STAR)
            pPopUp->enableItem( MI_EXPL );
          else pPopUp->disableItem( MI_EXPL );
          if (!pObj->isMachineRow() && fAllowEdit )
            pPopUp->enableItem( MI_ERESOBASE );
          else pPopUp->disableItem( MI_ERESOBASE );
          }
 if (pPopUp) {
   cnrCtl->setCursor( pObj );
   pPopUp->setAutoDeleteObject();
   pPopUp->show( evt.mousePosition() );
   }
 return true;
}

Boolean RMDiscResoW::command( ICommandEvent& evt )
{
 CFRRow * pObj;
 switch ( evt.commandId() ) {
   case MI_EXPL:
     if ( pObj = ( CFRRow * )(cnrCtl->cursoredObject())) {
       createExplainList( pObj );
       pXW->Fill( pXList );
       pXW->setClosingMsg(isDiscWindow()? XPLNW_CL_MSG_DISC :XPLNW_CL_MSG_RESO);
       pXW->setClosingMsgOwner((IWindow *)this);
       if ( !pXW->isVisible() )
         pXW->show();
       pXW->setFocus();
       }
     break;
   case MI_DISC_APPLY:
       Able(false);
       applyDisc((CFRRow *)(cnrCtl->cursoredObject()));
       Able(true);
     break;
   case MI_GENHELP:
     if (helpWindow())
       helpWindow()->show( IHelpWindow::general );
     return( true );
   case MI_EDISCBASE:
   case MI_ERESOBASE:
       editBase();
     break;
   case MI_MIS_SAVE:
       saveMismatches();
     break;
   case MI_MIS_PRINT:
       printMismatches();
     break;
   }
 return( false );
}


Boolean RMDiscResoW::applyDisc(CFRRow * pObj)
{

 if (isDiscWindow() && pObj && !pObj->isMachineRow()) {
   if (pObj->isGenericFeature() && !fS390 && pObj->qtyBase() ) {
     //edw 05-02-97  added below
     //edw 01-20-99  removed below and added 'pObj->qtyBase()' clause in above if statement
     //if (pObj->qtyBase() == 0)
     //   {
     //   IMessageBox msgBase(this);
     //   IMessageBox::Style style = IMessageBox::cancelButton |
     //                               IMessageBox::warningIcon |
     //                               IMessageBox::moveable;
     //    msgBase.setTitle("Generic Base is zero");
     //    msgBase.show("The Base of a Generic Feature CANNOT be zero!",
     //                        style, 0);
     //    return false;
     //    }

     disableNow();

     // DCG 02/11/02 IC51638 Begin
     SpecificFeatList SFL;
     if( pProdInfo->specificFeat_Desc(SFL,pObj->feature(),pObj->machine(),pObj->model()) > 1)
     {
     // DCG 02/11/02 IC51638 End

       if ( !pGenW )
         pGenW = new RMGenW( WND_GEN, this, helpWindow() );
       pGenW->populate( *pProdInfo, pObj );
//     pResoW->Able(false);
       pGenW->setFocus();
       pGenW->showModally();
//       if (pGenW->result() == DID_OK)
       delete pGenW, pGenW = 0;
//     pResoW->Able(true);
       Able(true);
       setFocus();

     // DCG 02/11/02 IC51638 Begin
     }
     else
     {
       // If just one specific feature is found, do not show
       // the Generic Features window to apply feature
       SpecificFeatList::Cursor crsrF(SFL);
       if ( crsrF.setToFirst() ) {
         GenRow *pTempObj;
         pTempObj = new GenRow(SFL.elementAt(crsrF),(RMGenW *)NULL);
         applyGen(pObj, pTempObj, pObj->qtyBase());
         delete pTempObj;
         Able(true);
       }
     }
     // DCG 02/11/02 IC51638 Begin

     }
   else {
     pObj->setStatus(Feat::Resolved_User);
     //edw 050797
     //pResoW->addRow(pObj);
     Feat feat1(pObj->rowsFeat());
     CFRRow *pObj2 = pResoW->locateRowFor(feat1);
     if (pObj2){
        Feat feat2(pObj2->rowsFeat());
        feat2.pF->setBase(feat1.qtyBase() + feat2.qtyBase());
        feat2.pF->setQty(MFeat::AASType, feat1.qtyAAS() + feat2.qtyAAS());
        feat2.pF->setQty(MFeat::MRDBType, feat1.qtyMRDB() + feat2.qtyMRDB());
        updateRPOCollection(pObj);
        feat1.pM->mfeatList().remove(feat1.pF);
        pObj2->re_Set(feat2,pUsrProfile);
        pResoW->refresh(); //edw
        }
     else{
        pResoW->addRow(pObj);
        updateRPOCollection(pObj);
        }
     cnrCtl->removeObject( pObj );
     if (pObj = machineWithoutFeatureRows(pObj->machineKey()))
       cnrCtl->removeObject( pObj );
     }
   handleEmptyContainer();
 //     filter();
   pResoW->filter(false);
   return true;
   }
 return false;
}

Boolean RMDiscResoW::applyGen(CFRRow * pR, GenRow * pG, unsigned qty)
{
 if (isDiscWindow() && pR && pG) {
   Boolean fReset = (qty < pR->qtyBase());

   if (pG->isMachine()) {
     FeatList FL;
     pSysInfo->updateWithMachinedGeneric(pR->rowsFeat(),qty,pG->machine(),
                                         pG->model(),pG->description(),FL);
     pResoW->addMachineRows(FL);
     }
   else {
     Feat featAffected(pR->rowsFeat());  //dummy-init to trigger constructor
     pSysInfo->updateWithAlteredGeneric(pR->rowsFeat(),pG->feature(),qty,
                                        pG->description(),&featAffected);
     Feat featUpdate = featAffected;
     if ( featUpdate.pF = featUpdate.pM->mfeatList().FindUnmarkedFeature(pG->feature()) )
        featAffected.pF = featUpdate.pF;
     CFRRow * pRow2 = pResoW->locateRowFor(featAffected);
     if (pRow2){
       pRow2->re_Set(featAffected,pUsrProfile);
       pResoW->refresh(); //edw
       }
     else{
        pResoW->addRow(featAffected);
        }

     if (strcmp(pG->description(),"OEM"))
       updateRPOCollection(pR,pG,qty);
     }
   if (fReset)
     {
       pR->re_Set(pR->rowsFeat(),pUsrProfile);
       cnrCtl->refresh();
     }
   else removeRow(pR);
 //     filter();
   pResoW->filter(false);
   return true;
   }
 return false;
}

void RMDiscResoW::updateRPOCollection( CFRRow * pR, GenRow * pG, unsigned qty)
{
 int iAAS =  pR->qtyAAS(), iNewBase;
 if (!qty)
   qty = pR->qtyBase();
 iNewBase = qty;
 long lRPOPos;
 const char *pchF, *pchD, *pchM, *pchMod, *pchSN = "";

 const char *pchMD = ""; //* Machine description: 73686  HECC:  10/15/2002
 char pchPC;             //* Product Category:    73686  HECC:  10/15/2002

 IString strCN;    //* Customer Number:     83402  HECC:  07/28/2003
 IString strSNF;   //* System Number flag:  83402  HECC:  07/28/2003
 IString strST;    //* System Type:         83402  HECC:  07/28/2003
 IString strSNM;   //* System Number:       83402  HECC:  07/28/2003

 if (pG) {
   if (pG->isMachine()) {
     pchM = pG->machine();
     pchMod = pG->model();
     }
   else {
     pchM = pR->machine();
     pchMod = pR->model();
     pchSN = pR->serialNo();
     }
   pchF = pG->feature();
   pchD = pG->description();
   }
 else {
   pchM =   pR->machine();
   pchMod = pR->model();
   pchSN =  pR->serialNo();
   pchF =   pR->feature();
   pchD =   pR->description();

   //* 83402  Begin New logic:  Retrieves and assigns to the RPO2 object the four new
   //* RPO parameters:  Customer number, System number flag, System type and System number. HECC:  07/28/2003
   strCN  = pR->customerNumber();
   strSNF = pR->systNumberFlag();
   strST  = pR->systemType();
   strSNM = pR->systemNum();
   //* End New logic:  HECC:  07/28/2003


   //* 73686  Begin New logic:  Retrieves and assigns to the RPO2 object the two new
   //* RPO parameters:  Machine Description and Product Category. HECC:  10/15/2002
   pchMD = pR->machineDescr();
   pchPC = pR->category();
   //* End New logic:  HECC:  10/15/2002

 }
 if (lRPOPos = pRPO2Lst->locate(pchM,pchMod,pchSN,pchF)) {
   RPO2 const & rpo = pRPO2Lst->elementAtPosition(lRPOPos);
   //edw01/22/98 iNewBase += rpo.signedQty();
   pRPO2Lst->removeElement(rpo);
   }
 if ( iAAS != iNewBase) {
   pRPO2Lst->add(RPO2(pchM,pchMod,pchSN,pchF,pchD, abs(iAAS-iNewBase),
                      iNewBase > iAAS? RPO_CODE_ADD : RPO_CODE_DELETE,
                      strCN, strSNF, strST, strSNM, //* 83402   HECC:  07/28/2003
                      pchMD, pchPC ));  //* 73686   HECC:  10/15/2002
   pRPO2Lst->SortUp();
   }
 pParent->changedRPOList();
}

//F45449-flw
void RMDiscResoW::saveMismatches()
{
   IFileDialog::Settings settings;
   settings.setSaveAsDialog();
   settings.setTitle(getString(IDS_MMDIALOGTITLE));
   settings.setFileName( pParent->pMain->pProfile->strGeneratePath + STR_PathDelimiter + STR_MMFileDefault);
   IFileDialog::Style fileStyle = IFileDialog::defaultStyle() |
   //*                            IFileDialog::helpButton;     // ICA68115   HECC: 08/27/2002
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

//F45449-flw
void RMDiscResoW  :: printMismatches()
{
   if( !numberOfObjects() )
      return;
   else
      {
      FormatPrintSave(pPRTMM_TEMP_FILE);
      }

   EUOPPrint prt;
   Boolean fOK;
   Boolean fPrinter = prt.init();
   if( fPrinter )
   {
      Boolean fOK = false;
      IString JobName = STR_MMPrintJobName;
      if (fOK = prt.fSpool(pPRTMM_TEMP_FILE,JobName,
                        (char *)PageHdr::strPageBreak))
         pParent->WWHandleMessage((char *)getString(MAINW_MSG_SPOOL_JOB,
                                       (char *)JobName,
                                       (char *)prt.printerQueue()));
      else
         if ( prt.lastErrorText().length())
            pParent->WWHandleMessage((char *)prt.lastErrorText());
   }
   else
      pParent->WWHandleMessage((char *)getString(IDS_ERR_NOPRINTERS));
   return;
}

/**************************************************************************************************
   F107280 RERC 04/10/2006 The next function has changed the logic so this function
                           is replaced for other with the same name
 **************************************************************************************************
//F45449-flw
void RMDiscResoW  :: FormatPrintSave(IString t1)
{
    FILE* pTempFile = fopen(t1, "w");
    if( !pTempFile )
       return;

    // Format Header Lines
    fprintf( pTempFile,"\n\n%10s%-s\n", STR_Blank1,(IString)STR_Results1);
    fprintf( pTempFile,"%30s%s\n", STR_Blank1,(IString)STR_Results2);
    fprintf( pTempFile,"%10s%-s\n\n", STR_Blank1,(IString)STR_Results3);
    fprintf( pTempFile,"%10s%s\n", STR_Blank1,(IString)STR_Results4);
    fprintf( pTempFile,"%10s%s\n", STR_Blank1,(IString)STR_Results5);    //ZOR IC65198
    fprintf( pTempFile,"%10s%-s\n", STR_Blank1,(IString)STR_Results3);   //ZOR IC65198

    CFRRow* pObj = 0;
    IString sTemp;
    char pszTemp[256];
    setFocus();
    IContainerControl::ObjectCursor crs( *cnrCtl );
    forCursor( crs )
       {
       pObj = (CFRRow* )cnrCtl->objectAt( crs );
       if ( pObj && pObj->isMachineRow() )
          if ( pObj->expandStr() == FPLUS )
             {
             expand_Collapse( pObj );    //Expands the machine which invalidates the cursor
             crs.setToFirst();           //Needed to re-validate the cursor
             }
       }
    Boolean bHeaderAU = FALSE; //F104628 RERC 30/01/2006
    forCursor( crs )
       {
       pObj = (CFRRow* )cnrCtl->objectAt( crs );
       //if ( pObj && !pObj->isMachineRow() )
       if ( pObj && !pObj->isMachineRow() && ( pObj->mismTypeAU() == UNACCEPTED_CHR )) //F104628
          {
     // ZOR IC65198
     // ZOR 49221a 06/18/00
     //IString serialNum= pObj->serialNo();
     //serialNum= serialNum.subString(5);
//        fprintf( pTempFile, "%10s      %-5d     %-5d %-5d %-4s %-3s %-8s %-s\n",
          if (!bHeaderAU)
               {
                 fprintf( pTempFile,"%10s\n", STR_Blank1);

                 //fprintf( pTempFile,"%10s%s\n", STR_Blank1,(IString)STR_Results6);
                 fprintf( pTempFile,"%10s%s\n", STR_Blank1,(IString)STR_REC05_TextUnacceptable); //D106441
                 bHeaderAU = TRUE;
               }
          fprintf( pTempFile, "%10s %-4d%-3d %-3d %-4s %-3s %-4s %-4s %-s\n",
                        STR_Blank1, pObj->qtyBaseFeat(), pObj->qtyAAS(), pObj->qtyMRDB(),
                        pObj->machine(), pObj->model(), pObj->serialNo(), pObj->feature(), pObj->description() );
          }
       }
    bHeaderAU = FALSE; //F104628 RERC 30/01/2006. Print Accepted Mismatch Type
    forCursor( crs )
       {
       pObj = (CFRRow* )cnrCtl->objectAt( crs );
       // if ( pObj && !pObj->isMachineRow() )
       if ( pObj && !pObj->isMachineRow() && ( pObj->mismTypeAU() == ACCEPTED_CHR ))
          {
          if (!bHeaderAU)
               {
                 fprintf( pTempFile,"%10s\n", STR_Blank1);
                 //fprintf( pTempFile,"%10s%s\n", STR_Blank1,(IString)STR_Results7);
                 fprintf( pTempFile,"%10s%s\n", STR_Blank1,(IString)STR_REC05_TextAcceptable);//D106441
                 bHeaderAU = TRUE;
               }
          fprintf( pTempFile, "%10s %-4d%-3d %-3d %-4s %-3s %-4s %-4s %-s\n",
                        STR_Blank1, pObj->qtyBaseFeat(), pObj->qtyAAS(), pObj->qtyMRDB(),
                        pObj->machine(), pObj->model(), pObj->serialNo(), pObj->feature(), pObj->description() );
          }
       }
    bHeaderAU = FALSE; //F104628 RERC 30/01/2006. Print all features where mismatch type is empty
    forCursor( crs )
       {
       pObj = (CFRRow* )cnrCtl->objectAt( crs );
       if ( pObj && !pObj->isMachineRow() && ( pObj->mismTypeAU() != UNACCEPTED_CHR )&& ( pObj->mismTypeAU() != ACCEPTED_CHR ))
          {
          if (!bHeaderAU)
               {
                 fprintf( pTempFile,"%10s\n", STR_Blank1);
                 bHeaderAU = TRUE;
               }
          fprintf( pTempFile, "%10s %-4d%-3d %-3d %-4s %-3s %-4s %-4s %-s\n",
                        STR_Blank1, pObj->qtyBaseFeat(), pObj->qtyAAS(), pObj->qtyMRDB(),
                        pObj->machine(), pObj->model(), pObj->serialNo(), pObj->feature(), pObj->description() );
          }
       }
       //F104638 END.  RERC  30/01/2006
    fprintf( pTempFile,"\n%10s%-s\n", STR_Blank1,(IString)STR_Results1);
    fclose(pTempFile);
}
**************************************************************************************************/
//=================================================================================================
//
// FUNCTION:                 FormatPrintSave()
// MODIFY:
// F107280 RERC 04/10/2006   Function created to print/save the Mismatches columns in the text
//                           report output.
//=================================================================================================
void RMDiscResoW  :: FormatPrintSave(IString t1)
{
    FILE* pTempFile = fopen(t1, "w");
    if( !pTempFile )
       return;
    // Format Header Lines
    if(is1704==true)
    fprintf( pTempFile,"\n\n%10s%-s\n", STR_Blank1,(IString)STR_Results0);//REQ1704

    fprintf( pTempFile,"\n\n%10s%-s\n", STR_Blank1,(IString)STR_Results1);
    fprintf( pTempFile,"%30s%s\n", STR_Blank1,(IString)STR_Results2);
    fprintf( pTempFile,"%10s%-s\n\n", STR_Blank1,(IString)STR_Results3);
    fprintf( pTempFile,"%10s%s\n", STR_Blank1,(IString)STR_Results4);
    fprintf( pTempFile,"%10s%s\n", STR_Blank1,(IString)STR_Results5);
    fprintf( pTempFile,"%10s%-s\n", STR_Blank1,(IString)STR_Results3);

    CFRRow* pObj = 0;
    IContainerControl::ObjectCursor crs( *cnrCtl );

    forCursor( crs )
       {
       pObj = (CFRRow* )cnrCtl->objectAt( crs );
       if ( pObj && pObj->isMachineRow() )
          if ( pObj->expandStr() == FPLUS )
             {
             expand_Collapse( pObj );    //Expands the machine which invalidates the cursor
             crs.setToFirst();           //Needed to re-validate the cursor
             }
       }
       saveMismatchesColumns(UNACCEPTED_CHR, pTempFile);  //Print the critical features
       saveMismatchesColumns(ACCEPTED_CHR, pTempFile);    //Print the acceptable features
       saveMismatchesColumns(NULL, pTempFile );           //Print the features that are not classify
                                                          //for critical and/or acceptable

    fprintf( pTempFile,"\n%10s%-s\n", STR_Blank1,(IString)STR_Results1);
    fclose(pTempFile);
}
//=================================================================================================
//
// FUNCTION:                 saveMismatchesColumns()
//                           char chrMismatchType - Mismatch Type of the feature ('A', 'U')
//                           FILE* pTempFile      - Output File
// MODIFY:
// F107280 RERC 04/10/2006   Function created to print/save the Mismatches columns in the text
//                           report output according to the classification of the mismatch type.
//=================================================================================================
void RMDiscResoW::saveMismatchesColumns(char chrMismatchType, FILE* pTempFile)
{
 CFRRow* pObj = 0;
 char pszTemp[256];
 setFocus();
 IContainerControl::ObjectCursor crs( *cnrCtl );
 Boolean bHeaderAU = FALSE;
 IString strText;
  // According to the value of Mismatch Type the text is assigned.
 if (chrMismatchType == UNACCEPTED_CHR)
 {
  strText = STR_REC05_TextUnacceptable;
 }
 else
  if (chrMismatchType == ACCEPTED_CHR)
  {
     strText = STR_REC05_TextAcceptable;
  }
 else
     strText = BLANK;

 forCursor( crs )
       {
       pObj = (CFRRow* )cnrCtl->objectAt( crs );
       if ( pObj && !pObj->isMachineRow() && ( pObj->mismTypeAU() == chrMismatchType))
          {
          if (!bHeaderAU)
               {
                 fprintf( pTempFile,"%10s\n", STR_Blank1);
                 if (strText != STR_Blank1)
                    fprintf( pTempFile,"%10s%s\n", STR_Blank1,(IString)strText);
                 bHeaderAU = TRUE;
               }
          fprintf( pTempFile, "%10s %-4d%-3d %-3d %-4s %-3s %-4s %-4s %-s\n",
                        STR_Blank1, pObj->qtyBaseFeat(), pObj->qtyMRDB(), pObj->qtyAAS(),
                        pObj->machine(), pObj->model(), pObj->serialNo(), pObj->feature(), pObj->description() );
          }
       }
}

/////////////////////////////////////////////////////////////////////

GenRow::GenRow( SpecificFeat & F, RMGenW * win )
       : IContainerObject( IString(F.strCode() + IString(DASH) +
                                   F.strDescription())
                          ,ICO_4012)
        ,strFeat(F.strCode())
        ,strDesc(F.strDescription())
        ,fMachine(false)
        ,Mywin( win )
{}

GenRow::GenRow( PKFeatDesc  & FD, RMGenW* win )
       : IContainerObject( IString(IString(FD.type()) + IString(DASH) +
                                   IString(FD.model()) + IString(DASH) +
                                   IString(FD.description()))
                          ,ICO_4012)
        ,strFeat(FD.feature())
        ,strDesc(FD.description())
        ,strMach(FD.type())
        ,strModel(FD.model())
        ,fMachine(true)
        ,Mywin( win )
{}

GenRow::GenRow( CFRRow  & R, RMGenW* win )
      : IContainerObject(IString(R.descStr()), ICO_4007)
       ,strFeat(R.feature())
       ,strDesc(R.descStr())
       ,fMachine(false)
       ,Mywin( win )
{}

void GenRow::handleOpen( IContainerControl * pCnr )
{
 IContainerObject * p = pCnr->cursoredObject();
 if (p && pCnr->parentObject( p ))
// Mywin->handle().postEvent(IWindow::command,DID_OK);
   Mywin->okButton().click();
}

void GenRow::handleSelectedChange( IContainerControl * pCnr,
                                    Boolean fAcquired )
{
 IContainerObject * p = pCnr->cursoredObject();
 if ( fAcquired && p && pCnr->parentObject( p ))
   Mywin->okButton().enable();
 else Mywin->okButton().disable();
}

//////////////////////////////////////////////////////////////////////

RMGenW::RMGenW( unsigned long windowId
               ,RMDiscResoW * pOwner
               ,IHelpWindow  * pHW)
       : IFrameWindow( IFrameWindow::titleBar       |
                       IFrameWindow::systemMenu     |
                       IFrameWindow::maximizeButton |
                       IFrameWindow::sizingBorder   |
                       IFrameWindow::accelerator,
                       windowId )
        ,BaseWnd(this,0,0,pHW)
        ,pDiscW(pOwner)
{
 setOwner(pOwner);
 cnrCtl = new IContainerControl( CNR_GEN, this, this, IRectangle(), IContainerControl::defaultStyle() | IContainerControl::pmCompatible);//jt-chg
 setClient( cnrCtl );
 cnrCtl->enableTabStop();
 cnrCtl->setTitleAlignment( IContainerControl::left );
 cnrCtl->showTitle().showTitleSeparator();

 //jt colIcon = new IContainerColumn( IContainerObject :: iconOffset() );
 colIcon = new IContainerColumn(IContainerColumn::isIcon,
                                       IContainerColumn::defaultHeadingStyle(),
                                       IContainerColumn::icon);

 //jt colText = new IContainerColumn( IContainerObject::iconTextOffset() );
 colText = new IContainerColumn( IContainerColumn::isIconViewText );
 cnrCtl->addColumn( colIcon ).addColumn( colText );

/*edw cnrCtl->showTreeIconView()
        .enableDrawItem()
        .setMixedTargetEmphasis(); */
 cnrCtl->showTreeTextView()                           //edw 12/9/97
        .enableDrawItem()
        .setMixedTargetEmphasis();
 cnrCtl->setDeleteColumnsOnClose()
        .setDeleteObjectsOnClose()
        .setAutoDeleteObject();

 addButtons();
 addSpinButton();
 // pPBOK->disable();  jt2/10/98

 ICnrHandler::handleEventsFor( cnrCtl );
 ICnrMenuHandler::handleEventsFor(cnrCtl);
 ISpinHandler::handleEventsFor(this);     //jt

 ISize dSize=desktopWindow()->size();
 sizeTo( ISize( dSize.width() * 0.3, dSize.height() * 0.5 ) );
 moveTo(IPoint( dSize.width() * 0.6, dSize.height() * 0.1 ));
 cnrCtl->show();
}

void RMGenW::populate( ProductKnowledge & PK, CFRRow * pRow )
{
 SpecificFeatList SFL;
 PKFeatSS         ML;
 SpecificFeatList::Cursor crsrF(SFL);
 PKFeatSS::Cursor crsrM(ML);
 GenRow * pObj1, *pObj2;
 cleanUp();
 PK.specificFeat_Desc(SFL,pRow->feature(),pRow->machine(),pRow->model());
 PK.machinesMapping2GenericFeature(pRow->feature(),ML);
 pObj1 = new GenRow(*pRow,this);
 cnrCtl->addObject(pObj1);
 forCursor(crsrF)
   if (pObj2 = new GenRow(SFL.elementAt(crsrF),this))
     cnrCtl->addObject(pObj2,pObj1);
 forCursor(crsrM)
   if (pObj2 = new GenRow(ML.elementAt(crsrM),this))
     cnrCtl->addObject(pObj2,pObj1);
 cnrCtl->setTitle(IString(IString(pRow->machine()) + IString(SLASH) +
                          IString(pRow->model()) + IString(" Serial# ") +
                          IString(pRow->serialNo())));
 cnrCtl->collapseTree();//jt
 cnrCtl->expandTree();
 cnrCtl->refresh(); //jt
 pSPBBase->setRange(IRange(1,pRow->qtyBaseFeat())).spinTo(pRow->qtyBaseFeat());
 pDiscRow = pRow;
}

void RMGenW::addButtons()
{
 ISetCanvas * pSC = new ISetCanvas( WND_BUTTONS, this, this );
 pSC->setDefaultStyle( ISetCanvas::rightAlign );
 pSC->setAutoDeleteObject();
 pSC->setDeckCount(3).setDeckOrientation(ISetCanvas::vertical);
 pSC->setPackType(ISetCanvas::expanded).setAlignment(ISetCanvas::bottomCenter);

 pPBOK = new IPushButton( DID_OK, pSC,pSC,IRectangle(),
                          IPushButton::classDefaultStyle );
 pPBOK->setAutoDeleteObject();
 pPBOK->setText(IResourceId(ID_STR_ACC_OK)).enableGroup().enableTabStop();

 pPBCancel = new IPushButton(DID_CANCEL,pSC,pSC, IRectangle(),
                             IPushButton::classDefaultStyle |
                             IPushButton::defaultButton);
 pPBCancel->setText( CANCEL_BTN_STR );
 pPBCancel->setAutoDeleteObject();

 pPBHelp = new IPushButton( MI_GENHELP, pSC, pSC,IRectangle(),
                            IPushButton::classDefaultStyle );
 pPBHelp->setText( HELP_BTN_STR );
 pPBHelp->setAutoDeleteObject();

 addExtension(pSC,belowClient,
             (unsigned long)(pPBOK->minimumSize().height() +
                             (pSC->margin().width() * 2) ));
 pSC->show();
}

void RMGenW::addSpinButton()
{
 IMultiCellCanvas *pMCC = new IMultiCellCanvas( WND_GEN_SPB_MCC ,this,this);
 pMCC->setAutoDeleteObject();
/*jt pSPBBase = new ISpinButton( WND_GEN_SPB , pMCC,pMCC, IRectangle(),
                             ISpinButton::defaultStyle() &
                             ~ISpinButton::allCharacters |
                             ISpinButton::readOnly       |
                             ISpinButton::numericOnly);    */

   INumericSpinButton::Style style = INumericSpinButton::classDefaultStyle
                                     | IControl::tabStop;

   pSPBBase = new INumericSpinButton( WND_GEN_SPB, pMCC,pMCC, IRectangle(), style);
   pSPBBase->disableDataUpdate();

 pSPBBase->setAutoDeleteObject();
 pSPBBase->setAlignment(INumericSpinButton::center);  //jt
 pSPBBase->setLimit(3);
 pSPBBase->enableGroup().enableTabStop();
 IStaticText * pTXTBase = new IStaticText( WND_GEN_SPB_TXT ,pMCC,pMCC);
 pTXTBase->setAutoDeleteObject();
 pTXTBase->setAlignment( IStaticText::centerLeft ).setText("Quantity");
 pMCC->addToCell(pSPBBase     , 2, 2);
 pMCC->addToCell(pTXTBase     , 4, 2);
 pMCC->setRowHeight( 1,IMultiCellCanvas::defaultCell().height()* 0.5);
 pMCC->setRowHeight( 3,IMultiCellCanvas::defaultCell().height()* 0.5);
 addExtension(pMCC,belowClient, (unsigned long)(pMCC->minimumSize().height()),
             IFrameWindow::none);
 pMCC->show();
}

Boolean RMGenW::command( ICommandEvent& evt )
{
 switch (evt.commandId()) {
   case MI_GEN_APPLY:
   case DID_OK: {
     GenRow * pGR = (GenRow *)(cnrCtl->cursoredObject());
     if (pGR) {
       Boolean fDismiss =  (pSPBBase->value() == pDiscRow->qtyBaseFeat());
       pDiscW->applyGen(pDiscRow,pGR,pSPBBase->value());
       if (fDismiss)
         dismiss(DID_OK);
       else {
          pSPBBase->setRange(IRange(1,pDiscRow->qtyBaseFeat()))
                     .spinTo(pDiscRow->qtyBaseFeat());
          }
       }
     return true;
     }
   case DID_CANCEL:
     dismiss(DID_CANCEL);
     return true;
   case MI_GENHELP:
     helpWindow()->show(IHelpWindow::general );
     return true;
   }
 return false;
}

void RMGenW::cleanUp()
{
 cnrCtl->deleteAllObjects();
 cnrCtl->setTitle( BLANK );
}

Boolean RMGenW::makePopUpMenu( IMenuEvent& evt )
{
 IPopUpMenu *pPopUp;
 IContainerObject * pObj = popupMenuObject();
 if (pObj && cnrCtl->parentObject(pObj) &&
     (pPopUp = new IPopUpMenu( MI_POPMENU_GEN, evt.window()))) {
   pPopUp->setAutoDeleteObject();
   pPopUp->show( evt.mousePosition() );
   }
 //JT else WinAlarm(HWND_DESKTOP,WA_ERROR);
 else  MessageBeep(MB_ICONEXCLAMATION);  //jt

 return true;
}

//////////////////////////////////////////////////////////////////////

Boolean EnforceUpTo3DigitsMLE :: virtualKeyPress(IKeyboardEvent & evt)
{
// Equate the newLine key (enter with the arrow) with the enter ( on the
// keypad) It makes no sense to get a new line in the MLE when it's used to
// enter a number

//jt if (evt.virtualKey() == IKeyboardEvent::newLine) {
 switch(evt.virtualKey()) {
   case IKeyboardEvent::enter:
   case IKeyboardEvent::newLine:
      //jt   IEventParameter2 param2(evt.parameter2().number1(), VK_RETURN);
           // note: if you send the message, you'll blow in PMMERGE
      //jt   evt.window()->postEvent(IWindow::character, evt.parameter1(), param2);
        //jt   evt.setResult(true);
      theCnrCtl->closeEdit();  //*73252 Start the endEdit event to stop the
      return true;             //*container edition.  HECC: 07/30/2002
   }

 return false;

}

Boolean EnforceUpTo3DigitsMLE :: characterKeyPress(IKeyboardEvent & evt)
{
 if ( evt.isCharacter() && !evt.isAltDown() &&
      !evt.isCtrlDown() && !evt.isUpTransition() ) {
   char chr = evt.character();
   if ((chr >= '0') && (chr <= '9')) {
     IMultiLineEdit * evtWnd = ( IMultiLineEdit * )evt.window();
     if ( evtWnd && ( evtWnd->textLength() < 3 ) )
        return false;
     }
   }
 //jt WinAlarm( HWND_DESKTOP, WA_ERROR );
 MessageBeep(MB_ICONEXCLAMATION);  //jt

 return true;
}
//////////////////////////////////////////////////////////////////////

