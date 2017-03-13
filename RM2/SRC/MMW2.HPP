#ifndef _AB_MMW_HPP_
#define  _AB_MMW_HPP_
 /*-------------------------------------------------------------------*\
 |    File Name  : MMW.HPP                                             |
 |                                                                     |
 |    Description:                                                     |
 |      This file contains the declaration(s) of the following         |
 |      class implementation(s):                                       |
 |                                                                     |
 |       MMWnd -                                                       |
 |                                                                     |
 |       EUOP's Mismatch Window                                        |
 |       Window allows the user to view text and                       |
 |       select the appropriate radio button.                          |
 |                                                                     |
 |                                                                     |
 |         Data Reentrant -                                            |
 |           The same instance of the window can be given              |
 |           different data without having to re-create                |
 |           it. For multiple appearances of Lists, the window         |
 |           has been designed to be 'shown' and 'hid' and             |
 |           given data via its public methods.                        |
 |           This was done to counter the annoyingly long              |
 |           time it takes to build the window using IBMClass.         |
 |                                                                     |
 |         Initial State                                               |
 |           Invisible                                                 |
 |           To be positioned, shown and/or focused by the caller.     |
 |                                                                     |
 |         Interface                                                   |
 |         The user's radio button preference upon                     |
 |         quiting this window is communicated with the                |
 |         following response:                                         |
 |         1  - User chose radiobutton 1                               |
 |         2  - User chose radiobutton 2                               |
 |                                                                     |
 |         If the window was instantiated with a                       |
 |         non-zero 'ClosingMsg' parameter:                            |
 |         That message will be sent with the                          |
 |         aforementioned response as                                  |
 |         'parameter1'                                                |
 |                                                                     |
 |         If the window was instantiated modally,                     |
 |         it will be dismissed with the response.                     |
 |                                                                     |
 |         If no 'ClosingMsg' was given and the                        |
 |         window was not shown modally, it will                       |
 |         close itself.                                               |
 |                                                                     |
 \*-------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/

#define INCL_ICHECKBOX
#define INCL_IENTRYFIELD
#define INCL_IICONCONTROL
#define INCL_IMULTICELLCANVAS
#define INCL_IMULTILINEEDIT
#define INCL_IPUSHBUTTON
#define INCL_IRADIOBUTTON
#define INCL_ISETCANVAS
#define INCL_ISTATICTEXT
#define INCL_ISTRING
#define INCL_ISYSTEMMENU
#define INCL_IVIEWPORT
#include "uicl.hpp"

        /*---------------------------------------------------*\
        |               EUOP specific includes                |
        \*---------------------------------------------------*/
#include "myxcphdr.hpp"                          // exception handler
#include "basew.hpp"


#pragma pack(4)
class MMWnd : public IFrameWindow
             ,public BaseWnd
{
 MyExceptionHandler  MyXHndlr;
 void                FillMLE();
  public:
      MMWnd( unsigned long WindowId
            ,const IString& text = IString()
            ,const IString& textRB1 = IString()
            ,const IString& textRB2 = IString()
                     ,Boolean bS390 = false //53915 MAMT
            ,IHelpWindow * pHW = 0
            ,unsigned long IconId = 0
            ,unsigned HelpPanelId = 0
            ,IWindow * Parent = 0
            ,unsigned long ClosingMsg = 0);
   ~MMWnd();
    unsigned long       IconId() { return ulIcon;}
    void                SetIconId(unsigned long Id) {ulIcon=Id;}
    void                SetIcon(unsigned long IconId);
    void                SetText(const IString&);
    void                SetRB1Text(const IString&);
    void                setHelpPanelId(unsigned i) {HPanelId = i;};
    void                SetRB2Text(const IString&);
    IString           & Text(){return strTxt;}
    IString           & RB1Text(){return strTxtRB1;}
    IString           & RB2Text(){return strTxtRB2;}
    IWindow           & setFocus();
    unsigned long       showModally();
  protected:
    Boolean             command(ICommandEvent& cmdEvent);
    ISize               calcMinimumSize() const;
    virtual void        SetInitialSize();
    void                CreatePBs();
    void                CreateClient();
    void                ArrangeControls();
    Boolean             PressedEnter(IKeyboardEvent &);
    Boolean             PressedTab(IKeyboardEvent &);
    Boolean             fS390; //53915 MAMT 28/02/2001
    IViewPort         * pVPClient;
    IMultiCellCanvas  * pMCCControl;
    ISetCanvas        * pSCPBs;
    IIconControl      * pIcon;
    IMultiLineEdit    * pMLE;
    IRadioButton      * pRB1,
                      * pRB2;
    IStaticText       * pTXTPPAC1;  //53915 MAMT 28/02/2001
    IStaticText       * pTXTPPAC2;  //53915 MAMT 28/02/2001
    IStaticText       * pTXTPPAC3;  //53915 MAMT 28/02/2001
    IStaticText       * pTXTPPAC4;  //53915 MAMT 28/02/2001
    IPushButton       * pPBOK,
                      * pPBCancel,
                      * pPBHelp;
    IString             strTxt
                    ,   strTxtRB1
                    ,   strTxtRB2;
    unsigned long       ulIcon;
    unsigned            HPanelId;
};
#pragma pack()

#endif

