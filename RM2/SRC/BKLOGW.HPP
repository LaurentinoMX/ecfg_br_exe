#ifndef BKLOGW_HPP
#define BKLOGW_HPP

#define INCL_IFRAMEWINDOW
#define INCL_ISTRING
#define INCL_ICNRHANDLER
#define INCL_ICONTAINERCONTROL
#define INCL_ICONTAINERCOLUMN
#define INCL_IMOUSECLICKHANDLER
#define INCL_IMOUSCLICKEVENT
#define INCL_ICNRHELPEVENT
#include "uicl.hpp"

#include "mainw.hpp"
#include "blfeatl.hpp"
#include "basew.hpp"

class RMBkLogW;

class BLRowSortUp : public IContainerControl::CompareFn
{
   virtual int isEqual (IContainerObject* p1, IContainerObject* p2,
                        IContainerControl* ccr) const;

};

class BLRowExpandFilter : public IContainerControl::FilterFn
{
  Boolean fExpandMachine;
public:
  BLRowExpandFilter ( ) : fExpandMachine(false) {}
  Boolean isMemberOf( IContainerObject *, IContainerControl *) const;
};

class BLRow : public IContainerObject
{
  IString        strAction
                ,strCSDDate
                ,strCRADDate
                ,strDesc
                ,strExpand
                ,strFeat
                ,strKey
                ,strMachModel
                ,strQty;
  RMBkLogW    *  pMywin;
  Boolean        fMachine;

 public:

  BLRow(BLFeat &, EUOPProfile *,RMBkLogW *,Boolean fMachineRow = false); // feature row

  static unsigned long actionOffset()    { return offsetof(BLRow,strAction);}
  static unsigned long cradDateOffset()  { return offsetof(BLRow,strCRADDate);}
  static unsigned long csdDateOffset()   { return offsetof(BLRow,strCSDDate);}
  static unsigned long descOffset()      { return offsetof(BLRow,strDesc);}
  static unsigned long expandOffset()    { return offsetof(BLRow,strExpand);}
  static unsigned long featOffset()      { return offsetof(BLRow,strFeat);}
  static unsigned long machModelOffset() { return offsetof(BLRow,strMachModel);}
  static unsigned long qtyOffset()       { return offsetof(BLRow,strQty);}
  static IString       machineKey( BLFeat & );
  static IString       title( BLFeat & );

  const IString & expandStr() { return strExpand;}
  Boolean         isMachineRow() const { return fMachine;}
  const IString & keyStr() { return strKey;}
  void            setExpandStr(const char * s) { strExpand = IString(s); }
  void            handleSelectedChange( IContainerControl *, Boolean acquired );
};

////////////////////////////////////////////////////////////////////
class RMBkLogW : public IFrameWindow
                ,public BaseWnd
           //jt     ,public IMouseClickHandler
                ,public IMouseHandler
                ,public ICnrHandler
{
  IContainerControl       * cnrCtl;
  IContainerColumn        * colExpand
                        , * colQty
                        , * colAction
                        , * colCRAD
                        , * colDesc
                        , * colFeature
                        , * colIcon
                        , * colCSD
                        , * colMachModel;
  BLRowExpandFilter         expandFilter;
  IWindow                 * pLeftCntCtlW;
  BLRow                   * pExpandRow;

  void          Sort();
  void          expand_Collapse(BLRow *);
  void          filter(Boolean fScroll = true);

 public:
  RMBkLogW( unsigned long windowId ,IWindow *Parent
           ,IHelpWindow  * pHW , unsigned long closingMsg);
 ~RMBkLogW ();

  Boolean        load( BLFeatList *, EUOPProfile * pProf);
  void           setCntrTitle(BLRow *);

 protected:
     Boolean     mouseClicked(IMouseClickEvent&);
     Boolean     selectedChanged(ICnrEmphasisEvent&);
     Boolean     userMessage(IEvent&);
     Boolean     help(ICnrHelpEvent&);
};

#endif
