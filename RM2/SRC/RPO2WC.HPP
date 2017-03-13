#ifndef _AB_RPOWC_HPP_
#define _AB_RPOWC_HPP_
 /*--------------------------------------------------------------------------*\
 |    File Name  : RPOWC.HPP                                                  |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |      RPOsWndw         - RPO Window                                         |
 |                         Window allows the user to view and prune           |
 |                         a list of RPOs in the detailed view of a           |
 |                         container.                                         |
 |                                                                            |
 |      Window has the following features or peculiarities:                   |
 |         Minimum Size  - The window can NOT be sized smaller by the         |
 |                         user than the width occupied by the                |
 |                         pushbuttons and a height required to show the      |
 |                         number of items given as a parameter               |
 |                         (defaulting to 10).                                |
 |                                                                            |
 |         Data Reentrant- The same instance of the window can be given       |
 |                         different RPO Lists without having to re-create    |
 |                         it. For multiple appearances of Lists, the window  |
 |                         has been designed to be 'shown' and 'hid' and      |
 |                         given data via the FillContainer() method.         |
 |                         This was done to counter the annoyingly long       |
 |                         time it takes to build the window using IBMClass.  |
 |                                                                            |
 |         Initial State - Invisible                                          |
 |                         To be positioned by the caller.                    |
 |                         The initial and minimum height of the              |
 |                            window is depenend on the                       |
 |                            'NumberofLines' parmeter which defaults         |
 |                            to 10 while anything less than 3 is ignored.    |
 |                                                                            |
 |         Interface     - The user's preference upon quiting                 |
 |                         this window is communicated with the               |
 |                         following response:                                |
 |                           DID_CANCEL - User canceled                       |
 |                           DID_OK     - User wants the current              |
 |                                        RPO List saved.                     |
 |                                                                            |
 |                           If the window was instantiated with a            |
 |                           non-zero 'ClosingMsg' parameter:                 |
 |                             That message will be sent with the             |
 |                             aforementioned response as                     |
 |                             'parameter1'                                   |
 |                                                                            |
 |                           If the window was instantiated modally,          |
 |                           it will me dismissed with the reponse.           |
 |                                                                            |
 |                           If no 'ClosingMsg' was given and the             |
 |                           window was not shown modally, it will            |
 |                           close itself.                                    |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/

#define INCL_ICNRHANDLER
#define INCL_ICNRMENUHANDLER
#define INCL_ICONTAINERCOLUMN
#define INCL_ICONTAINERCONTROL
#define INCL_ICONTAINEROBJECT
#define INCL_IGROUPBOX
#define INCL_IINFOAREA
#define INCL_IMOUSECLICKHANDLER
#define INCL_IMULTICELLCANVAS
#define INCL_IPUSHBUTTON
#define INCL_ISETCANVAS
#include "uicl.hpp"

        /*---------------------------------------------------*\
        |               EUOP specific includes                |
        \*---------------------------------------------------*/
#include "mytrkhdr.hpp"
#include "myfcshdr.hpp"                          // MyFocusHandler
#include "basew.hpp"

        /*---------------------------------------------------*\
        |                Forward declarations                 |
        \*---------------------------------------------------*/
class RPO2List;
class EUOPProfile;
class RPOsWndw;

                                                 // minimum number of listbox
#define  RPOSWND_MIN_NMBR_LINES  3               // lines

#pragma pack(4)

class RPOCnrObj : public IContainerObject
{
 IString    strNmbr
         ,  strAction
         ,  strMMF
         ,  strDesc;
 RPOsWndw * pWin;

public:
  RPOCnrObj(RPO2 &,RPOsWndw *);

  Boolean      operator==(RPOCnrObj const& other) const;
  Boolean      operator< (RPOCnrObj const& other) const;
  Boolean      operator> (RPOCnrObj const& other) const;
  Boolean      IsEqual(RPO2 const & rpo);
  virtual void handleOpen(IContainerControl *);
  static unsigned long quantityOffset()
             { return offsetof(RPOCnrObj,strNmbr);}
  static unsigned long actionOffset()
             { return offsetof(RPOCnrObj,strAction);}
  static unsigned long machineModelFeatureOffset()
             { return offsetof(RPOCnrObj,strMMF);}
  static unsigned long descriptionOffset()
             { return offsetof(RPOCnrObj,strDesc);}
};

class RPOsWndwUpSort : public IContainerControl::CompareFn
{
   virtual int isEqual (IContainerObject* object1,
                        IContainerObject* object2,
                        IContainerControl* container) const;
};

class RPOsWndwDownSort : public IContainerControl::CompareFn
{
   virtual int isEqual (IContainerObject* object1,
                        IContainerObject* object2,
                        IContainerControl* container) const;
};


class RPOsWndw : public IFrameWindow
                ,public BaseWnd
                ,public ICnrMenuHandler
                ,public ICnrHandler
                ,public IMouseHandler
                ,public MyTrackHandler
                ,public MyWindowFocusHandler
{
  public:
    RPOsWndw( unsigned long windowId
             ,RPO2List *
             ,EUOPProfile * Prof = 0
             ,IWindow * Parent = 0
             ,IHelpWindow * pHW  = 0
             ,unsigned long ClosingMsg = 0
             ,long NumberofLines = 10
            );

   ~RPOsWndw();
    void      FillList(RPO2List *)
           ,  SizeToMaximumTextWidth()
           ,  Sort(Boolean fUp = true);
    IWindow & setFocus();
    unsigned long   showModally();
  protected:
    Boolean         SetMinTrackSize(IPoint&)
                ,   SetupInfoArea()
                ,   command(ICommandEvent&)
                ,   gettingFocus(IEvent&)
                ,   keysHelpId(IEvent& )
                ,   LostFocus(IEvent&)
                ,   makePopUpMenu(IMenuEvent&)
                ,   menuEnded(IMenuEvent&)
                ,   menuSelected(IMenuEvent&)
                ,   mouseClicked(IMouseClickEvent&)
                ,   PressedEnter(IKeyboardEvent &)
                ,   help(ICnrHelpEvent&);
    IMultiCellCanvas  * pMCCControls;
    IGroupBox         * pGBRPOs;
    ISize           calcMinimumSize() const;
    void            CreateClient()
                ,   CreatePBs()
                ,   FillList(Boolean fEmptyFirst = true);
    void            addSourceEmphasis( const IMenuEvent& menuEvent ) {}; //not

  private:
    unsigned long          ulLines
                       ,   ulObjW
                       ,   ulObjH
                       ,   ulInfoText;
    ISetCanvas           * pSCPBs;
    IContainerControl    * pCnrCtl;
    IContainerColumn     * pCnrCol1
                       , * pCnrCol2
                       , * pCnrCol3
                       , * pCnrCol4;
    IPushButton          * pPBSave
                       , * pPBProcRPO         //* 73686 HECC: 08/29/2002
                       , * pPBCancel
                       , * pPBHelp;
    RPO2List              * pRPOList;
    EUOPProfile          * pProfile;
    IInfoArea            * pInfo;
    Boolean                fMyProfile
                       ,   fAutoGen;

    void                   ArrangeControls()
                       ,   SizeObjects()
                       ,   PruneList()
                       ,   ProcessList();     //* 73686 HECC: 09/02/2002
    Boolean                HasSelection()
                       ,   RPOInContainer(RPO2 &)
                       ,   SetSplitBar();
    int                    iSorted;
    IString                strAASFile;        //* 73686 HECC: 09/02/2002
    IString                strOrFwdPath;      //* 73686 HECC: 09/09/2002
    Boolean                bIsS390;           //* 73686 HECC: 09/09/2002
    IString                strRpoCfrFile;     //* 73686 HECC: 09/09/2002
};
#pragma pack()
#endif
