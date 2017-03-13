#ifndef _AB_SWDW_HPP_
#define _AB_SWDW_HPP_

        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/

#define INCL_ICNRHANDLER
#define INCL_ICNRMENUHANDLER
#define INCL_ICONTAINERCOLUMN
#define INCL_ICONTAINERCONTROL
#define INCL_ICONTAINEROBJECT
#define INCL_IGROUPBOX
#define INCL_IMULTICELLCANVAS
#define INCL_IPUSHBUTTON
#define INCL_ISETCANVAS
#include "uicl.hpp"

#include "basew.hpp"
#include "swdw.h"

        /*---------------------------------------------------*\
        |                Forward declarations                 |
        \*---------------------------------------------------*/
class SoftMachList;
class SWDWndw;

#pragma pack(4)
class SWCnrObj : public IContainerObject
{
 IString    strType
         ,  strModel
         ,  strFeat
         ,  strDesc;

 public:

  SWCnrObj(SoftMach & sm) :  strType(sm.type())
                            ,strModel(sm.model())
                            ,strFeat(sm.feature())
                            ,strDesc(sm.description()){}

  Boolean      operator==(SWCnrObj const& other) const;
  Boolean      operator< (SWCnrObj const& other) const;
  Boolean      operator> (SWCnrObj const& other) const;
  Boolean      IsEqual(SoftMach const & sm);
  const char *   type() const        { return (char *)strType;  }
  const char *   model() const       { return (char *)strModel; }
  const char *   feature() const     { return (char *)strFeat; }
  const char *   description() const { return (char *)strDesc; }
  static unsigned long typeOffset()
             { return offsetof(SWCnrObj,strType);}
  static unsigned long modelOffset()
             { return offsetof(SWCnrObj,strModel);}
  static unsigned long featureOffset()
             { return offsetof(SWCnrObj,strFeat);}
  static unsigned long descriptionOffset()
             { return offsetof(SWCnrObj,strDesc);}
  virtual void handleOpen(IContainerControl *);
};

class SWDWndwSort1 : public IContainerControl::CompareFn
{
   virtual int isEqual (IContainerObject* object1,
                        IContainerObject* object2,
                        IContainerControl* container) const;
};

class SWDWndwSort2 : public IContainerControl::CompareFn
{
   virtual int isEqual (IContainerObject* object1,
                        IContainerObject* object2,
                        IContainerControl* container) const;
};

class SWDWndwSort3 : public IContainerControl::CompareFn
{
   virtual int isEqual (IContainerObject* object1,
                        IContainerObject* object2,
                        IContainerControl* container) const;
};

class SWDWndwSort4 : public IContainerControl::CompareFn
{
   virtual int isEqual (IContainerObject* object1,
                        IContainerObject* object2,
                        IContainerControl* container) const;
};

class SWDWndw :  public IFrameWindow
                ,public BaseWnd
                ,public ICnrMenuHandler
                ,public ICnrHandler
{
  public:
    SWDWndw( unsigned long windowId
             ,SoftMachList *
             ,IHelpWindow  * pHW = 0
             ,IWindow * Parent = 0
             ,unsigned long ClosingMsg = 0
             ,long NumberofLines = 10
            );

   ~SWDWndw();
    void      FillList(SoftMachList *)
           ,  SizeToMaximumTextWidth()
           ,  Sort(unsigned type = SWDW_ID_POP_SORT_TYPE);
    IWindow & setFocus();
    unsigned long   showModally();

  protected:
    Boolean         PressedEnter(IKeyboardEvent &)
                ,   command(ICommandEvent&)
                ,   makePopUpMenu(IMenuEvent&)
                ,   help(ICnrHelpEvent&);
    IMultiCellCanvas  * pMCCControls;
    IGroupBox         * pGB;
    ISize           calcMinimumSize() const;
    void            CreateClient()
                ,   CreatePBs()
                ,   FillList(Boolean fEmptyFirst = true);
    void            addSourceEmphasis( const IMenuEvent& menuEvent ) {};


  private:
    unsigned long          ulLines
                       ,   ulObjW
                       ,   ulObjH;
    ISetCanvas             SC;
    IPushButton            PBOK;
    IPushButton            PBHelp;
    IContainerControl    * pCnrCtl;
    IContainerColumn     * pCnrCol1
                       , * pCnrCol2
                       , * pCnrCol3
                       , * pCnrCol4;
    SoftMachList         * pSWList;
    void                   Init()
                       ,   CreateExtension()
                       ,   ArrangeControls();
    Boolean                SizeObjects()
                       ,   SetSplitBar();
};
#pragma pack()
#endif

