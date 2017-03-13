#ifndef EUOGMAIN_HPP
#define EUOGMAIN_HPP

#define INCL_IFRAMEWINDOW
#define INCL_ISTATICTEXT
#define INCL_IHELPWINDOW
#define INCL_IHELPHANDLER
#define INCL_ICOMMANDHANDLER
#define INCL_IMESSAGEBOX
#define INCL_ISTRING
#define INCL_IPUSHBUTTON
#define INCL_IMENUBAR
#define INCL_IKEYBOARDHANDLER
#define INCL_ICNRHANDLER
#define INCL_ICNRMENUHANDLER
#define INCL_ICONTAINERCONTROL
#define INCL_ICONTAINERCOLUMN
#define INCL_IMOUSECLICKHANDLER
#define INCL_IMOUSCLICKEVENT
#define INCL_ICNRHELPEVENT
#define INCL_IUSERHANDLER
#define INCL_ICNREDITHANDLER
#define INCL_IPOPUPMENU
#define INCL_IMULTILINEEDIT
#include "uicl.hpp"

//#include "keyhdr.hpp"
#include "xpln2.hpp"
#include "xpln2l.hpp"
#include "xplnw.hpp"
#include "prf.hpp"
#include "rpo2l.hpp"
#include "system.hpp"
#include "pkfeatl.hpp"
#include "pkfeat.hpp"
#include "feat.hpp"
#include "featl.hpp"
#include "mainw.hpp"
#include "basew.hpp"
#include "euoutil.hpp"                  //flw

//******************************************************************************
// forward class declaration
//******************************************************************************
class RMDiscResoW;
class RMGenW;
class Profile;
/////////////////////////////////////////////////////////////////////
class EnforceUpTo3DigitsMLE : public IKeyboardHandler
{
 public:                                                        //*73252  HECC: 07/30/2002
  void setCnrCtl(IContainerControl* cnrCtl){theCnrCtl = cnrCtl;}//*73252  HECC: 07/30/2002
 protected:
  Boolean characterKeyPress(IKeyboardEvent&);
  Boolean virtualKeyPress(IKeyboardEvent &);
 private:                                                       //*73252 HECC: 07/30/2002
  IContainerControl* theCnrCtl;                                 //*73252 HECC: 07/30/2002
};
/////////////////////////////////////////////////////////////////////
class CFRRow : public IContainerObject
{
  IString        strAAS
                ,strBase
                ,strCRAD
                ,strDate
                ,strDesc
                ,strMismType  //F104628 RERC 01/30/2006
                ,strExpand
                ,strExpln
                ,strFeat
                ,strKey
                ,strMRDB
                ,strMachModel
                ,strSNum;
  Feat           feat;
  RMDiscResoW *  Mywin;
  Boolean        fMachine;
  XWindow     * pParent;     //ZOR 48548

  void           featRowInit(EUOPProfile *);

 public:

  CFRRow(Feat &,IResourceId, EUOPProfile *,RMDiscResoW *); // feature row
  CFRRow(Feat &,IPointerHandle , EUOPProfile *,RMDiscResoW *); // feature row
  CFRRow(IResourceId,Feat &,EUOPProfile *,RMDiscResoW *,XWindow * pParent); // machine row   ZOR 48548

  static unsigned long aasOffset() { return offsetof(CFRRow,strAAS);}
  static unsigned long baseOffset() { return offsetof(CFRRow,strBase);}
  static unsigned long cradOffset() { return offsetof(CFRRow,strCRAD);}
  static unsigned long dateOffset() { return offsetof(CFRRow,strDate);}
  static unsigned long descOffset() { return offsetof(CFRRow,strDesc);}
  static unsigned long machtypeOffset() { return offsetof(CFRRow,strMismType);} //F104628 RERC 01/30/2006
  static unsigned long expandOffset() { return offsetof(CFRRow,strExpand);}
  static unsigned long explnOffset() { return offsetof(CFRRow,strExpln);}
  static unsigned long featOffset() { return offsetof(CFRRow,strFeat);}
  static unsigned long machModelOffset() { return offsetof(CFRRow,strMachModel);}
  static IString       machineKey( Feat & );
  static IString       title( Feat & , IString strSensedDate, IString strReceiveDate ); //*F73252
  static unsigned long mrdbOffset() { return offsetof(CFRRow,strMRDB);}

  const IString & descStr() { return strDesc;}
  const char    * description() const { return (const char *)strDesc; }
//  IString         displaySerialNoStr() { return feat.displaySerialNoStr() ;}
  const IString & expandStr() { return strExpand;}
  const IString & explainStr() { return strExpln;}
  const char    * feature() const { return (isMachineRow()? "" : feat.feature()); }
  void            handleSelectedChange( IContainerControl *, Boolean acquired );
  Boolean         isGenericFeature() { return feat.isGeneric();}
  Boolean         isMachineRow() const { return fMachine;}
  const IString & keyStr() { return strKey;}
  const char    * machine() const { return feat.machine(); }
  IString         machineKey(void);
  const char    * model() const { return feat.model(); }
  int             qtyAAS() { return strAAS.asInt(); }
  int             qtyMRDB() { return strMRDB.asInt(); }              //edw01/03/2000
  int             qtyBase() { return strBase.asInt(); }
  int             qtyBaseFeat() { return feat.qtyBase();}
  char            mismTypeAU() { return feat.mismType();}   //F104628 RERC 30/01/2006
  void            re_Set(Feat & f,EUOPProfile *);
  Feat          & rowsFeat() { return feat; }
  const char    * serialNo() const { return feat.serialNo(); }
  void            setBaseQty(const IString & str)
                       { feat.setBase(str.asInt());
                         strBase = IString(feat.qtyBase());
                       }
  void            setExpandStr(const char * s) { strExpand = IString(s); }
  void            setStatus(Feat::Status s) { feat.setStatus(s);}

  const char    * machineDescr() { return feat.machineDescStr(); }  //* 73686  HECC:  10/15/2002
  char            category() {return feat.category();}              //* 73686  HECC:  10/15/2002

  const char    * customerNumber() { return feat.customerNumber(); } //* 83402  HECC:  07/28/2003
  const char    * systNumberFlag() {return feat.systNumberFlag(); }  //* 83402  HECC:  07/28/2003
  const char    * systemType() { return feat.systemType(); }         //* 83402  HECC:  07/28/2003
  const char    * systemNum() { return feat.systemNum(); }           //* 83402  HECC:  07/28/2003


 protected:
  void            handleOpen(IContainerControl*);

};

///////////////////////////////////////////////////////////

class CFRRowSortUp : public IContainerControl::CompareFn
{
   virtual int isEqual (IContainerObject* p1,
                        IContainerObject* p2,
                        IContainerControl* ccr) const;
};

///////////////////////////////////////////////////////////

class GenRow : public IContainerObject
{
  RMGenW *         Mywin;
  Boolean          fMachine;
  IString          strDesc
                  ,strFeat
                  ,strMach
                  ,strModel;

 public:
   GenRow( SpecificFeat &, RMGenW* win );
   GenRow( PKFeatDesc  &, RMGenW* win );
   GenRow( CFRRow  &, RMGenW* win );
  ~GenRow() {}

   const char *   description() const { return (const char *)strDesc; }
   const char *   feature() const { return (const char *)strFeat;}
   Boolean        isMachine() { return fMachine; }
   const char *   machine() const { return (const char *)strMach; }
   const char *   model() const { return (const char *)strModel; }

 protected:
   virtual void handleOpen( IContainerControl* container );
   virtual void handleSelectedChange( IContainerControl *, Boolean acquired );
};

///////////////////////////////////////////////////////////
class ExpandFilter : public IContainerControl::FilterFn
{
  Boolean fExpandMachine;
public:
  ExpandFilter ( ) : fExpandMachine(false) {}
  Boolean
         isMemberOf( IContainerObject* object,
                     IContainerControl* container) const;
};

//////////////////////////////////////////////////////////////////////////////
class RMDiscResoW : public IFrameWindow
                   ,public BaseWnd
                   ,public ICnrMenuHandler
                   ,public ICnrEditHandler
                   ,public IMouseHandler
                   ,public ICnrHandler
                   ,public ResMessage
{
  IContainerControl       * cnrCtl;
  IContainerColumn        * colAAS
                        , * colBase
                        , * colCRAD
                        , * colDate
                        , * colDesc
                        , * colExpl
                        , * colFeature
                        , * colFlag
                        , * colIcon
                        , * colMRDB
                        , * colMachModel
                        , * colMachType;        //F104628 RERC 01/30/2006
  ExpandFilter              expandFilter;
  Boolean                   fAllOrDisc
                        ,   fAutoApply
                        ,   fAllowEdit
                        ,   fEditing
                        ,   fS390               //edw/07/01/1998
                        ,   fEndEdit            //F73252  HECC - 08/01/2002
                        ,   fMyXW;

  IString                   strSensedDate     //* F73252
                        ,   strReceiveDate;   //* F73252


  IMenuBar                  menuBar;
  EnforceUpTo3DigitsMLE     mle3DigitHdr;
  CFRRow                  * pExpandObj;
  RMGenW                  * pGenW;
  XWindow                 * pParent;
  ProductKnowledge        * pProdInfo;
  RPO2List                * pRPO2Lst;
  RMDiscResoW             * pResoW;
  RMSystem                * pSysInfo;
  EUOPProfile             * pUsrProfile;
  XPLN2List               * pXList;
  XPLN2List               * pXPLN2Lst;
  XPLNWndw                * pXW;
  FeatList                * pFList;
  IWindow                 * pLeftCntCtlW;

  void          Sort();
  void          addMachineRows(FeatList &);
  void          addRow(CFRRow *);
  void          addRow(Feat &f);
  Boolean       applyDisc(CFRRow *);
  Boolean       containsMachineRowFor(CFRRow & R);
  void          createExplainList( CFRRow * cnrObj );
  void          editBase();
  void          filter(Boolean fScroll = true);
  Boolean       handleEmptyContainer();
  int           iconFor(Feat &);
  Boolean       includeStatus( Feat::Status status );
  CFRRow *      locateRowFor(Feat &);
  CFRRow *      machineWithoutFeatureRows(const IString & MachineKey);
  void          removeRow(CFRRow *);

 public:
  RMDiscResoW( unsigned long windowID
                    ,const char * title
                    ,EUOPProfile * pUsrProf
                    ,XWindow * pParent
                    ,Boolean bS390 = false                           //edw07/01/1998
                    ,IHelpWindow  * pHW = 0
                    ,XPLNWndw     * pXplainW = 0
                    ,unsigned long closingMsg = 0
                    ,RMDiscResoW * pResolutionWindow = 0 );

  IString   getSensedDate(){return strSensedDate;}     //*F73252 Retrieves the sensed date from container object
  IString   getReceivedDate(){return strReceiveDate;}  //*F73252 Retrieves the receive date from container object


  ~RMDiscResoW ();

  Boolean      applyGen(CFRRow *, GenRow *,unsigned qty);

  //* F73252 Begin new logic: Definitions for two new parameters strSensedDate &
  //* strReceiveDate are to be received by the RMDiscResoW class so they can be
  //* displayed in both, the Matches & Mismatches windows when these dialogs
  //* be populated  HECC; 07/18/2002

  Boolean      load( FeatList *flst, RPO2List *rpolst,
                     XPLN2List *explst,RMSystem *sinfo,
                     ProductKnowledge *pinfo ,
                     IString strSensedDatePar = " ", IString strReceiveDatePar = " ", //*F73252
                     Boolean fAlsoResoW = false);
  //* F73252 End new logic.

  Boolean      isDiscWindow() { return (pResoW != 0);}
  void         setCntrTitle(CFRRow * pObj);
  void         setMenuPer( CFRRow * );
  void         updateRPOCollection( CFRRow *, GenRow * pG= 0,unsigned qty = 0);
  Boolean      updated(EUOPProfile *);
  void         expand_Collapse(CFRRow *);
  int          numberOfObjects(){ return cnrCtl->objectCount(); }    //edw07/01/1998
  void         autoResolve(boolean bUseCurrentBase = false);         //edw07/01/1998
  void         recordMismatches();                                   //edw01/03/2000
  void         printMismatches();                                    //F45449
  void         saveMismatches();                                     //F45449
  void         FormatPrintSave(IString sFileName);                   //F45449
  void         printMismatchesColumns(char chrMismatchType);         //F107280  RERC 04/10/2006
  void         saveMismatchesColumns(char chrMismatchType, FILE* pTempFile); //F107280  RERC 04/10/2006
 protected:
  Boolean      beginEdit( ICnrBeginEditEvent&)
              ,command(ICommandEvent&)
              ,endEdit( ICnrEndEditEvent&)
              ,enter( ICnrEnterEvent&)    //*_F73252 (To check if Enter was pressed to edit container)
              ,help(ICnrHelpEvent& evt)
              ,makePopUpMenu(IMenuEvent&)
              ,mouseClicked(IMouseClickEvent& mouEvent)
              ,selectedChanged(ICnrEmphasisEvent& event)
              ,userMessage(IEvent&);
};

//////////////////////////////////////////////////////////////////////////////
class RMGenW : public IFrameWindow
              ,public BaseWnd
              ,public ICnrHandler
              ,public ICnrMenuHandler
{
  IContainerControl    *   cnrCtl;
  IContainerColumn     *   colIcon
                     , *   colText;
  CFRRow               *   pDiscRow;
  RMDiscResoW          *   pDiscW;
  IPushButton          *   pPBCancel
                     , *   pPBHelp
                     , *   pPBOK;
  ISpinButton          *   pSPBBase;

  void               addButtons();
  void               addSpinButton();
  void               cleanUp();

 public:
  RMGenW( unsigned long windowID ,RMDiscResoW * pOwner,IHelpWindow * pHW = 0);
  ~RMGenW (){};

  IPushButton &      okButton() { return *pPBOK ; }
  void               populate( ProductKnowledge & , CFRRow * );

 protected:

  Boolean      command(ICommandEvent&);
  Boolean      makePopUpMenu(IMenuEvent&);

};

#endif
