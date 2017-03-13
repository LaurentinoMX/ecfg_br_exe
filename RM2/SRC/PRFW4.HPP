#ifndef _AB_PRFW_HPP_
#define  _AB_PRFW_HPP_
 /*--------------------------------------------------------------------------*\
 |    File Name  : PRFW.HPP                                                   |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |       EUOPProfileWndw    - EUOP's Profile Window, allowing the             |
 |                            user to set his or her EUOP Profile.            |
 |                                                                            |
 |      Window has the following features or peculiarities:                   |
 |                                                                            |
 |       Construction    - Frame with a IMultiCellCanvas                      |
 |                         within a IViewPort client, extended                |
 |                         by a ISetCanvas with pushbuttons and               |
 |                         an optional Info Area.                             |
 |                                                                            |
 |       Initial State   - Invisible                                          |
 |                         To be positioned by the caller.                    |
 |                         The Explain control hidden.                        |
 |                                                                            |
 |       Interface       - The user's preference upon quiting                 |
 |                         this window is communicated with the               |
 |                         following response:                                |
 |                           DID_CANCEL - User canceled                       |
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

#define INCL_ICHECKBOX
#define INCL_IENTRYFIELD
#define INCL_IGROUPBOX
#define INCL_IMULTICELLCANVAS
#define INCL_IPUSHBUTTON
#define INCL_IRADIOBUTTON
#define INCL_IRESOURCELIBRARY
#define INCL_ISETCANVAS
#define INCL_ISTATICTEXT
#define INCL_ISTRING
#include "uicl.hpp"

        /*---------------------------------------------------*\
        |               EUOP specific includes                |
        \*---------------------------------------------------*/
#include "msgthrd.hpp"                           // message thread
#include "myxcphdr.hpp"                          // exception handler
#include "basew.hpp"
#include "prf.hpp"                               // EUOP Profile

        /*---------------------------------------------------*\
        |                Forward declarations                 |
        \*---------------------------------------------------*/
class EUOPProfile;
class INotifyThread;

#pragma pack(4)
class EUOPProfileWndw : public IFrameWindow
                       ,public BaseWnd
{
   MyExceptionHandler  MyXHndlr;
  public:
    EUOPProfileWndw(unsigned long Id,
                    EUOPProfile * Prof = 0,
                    IWindow * Parent = 0,
                    IHelpWindow * pHW = 0,
                    unsigned long ClosingMsg = 0);
   ~EUOPProfileWndw();
    Boolean             Re_SetControls(EUOPProfile * profile = 0);
    IWindow           & setFocus();
    unsigned long       showModally();
  protected:
    Boolean             PressedEnter(IKeyboardEvent &keyEvent);
    Boolean             PressedTab(IKeyboardEvent &keyEvent);
    Boolean             command(ICommandEvent& cmdEvent);
    ISize               calcMinimumSize() const;
    Boolean             userMessage(IEvent&);
    virtual void        SetInitialSize();
    void                CreatePBs();
    Boolean             ReadControls();
    void                CreateControls();
    void                SetControls(Boolean fDefault = false);
    void                ArrangeControls();
    Boolean             editWorkDirectory(IString &);
    IMultiCellCanvas  * pMCCControl;
    ISetCanvas        * pSCPBs;
    IGroupBox         * pGBDsply;
    IRadioButton      * pRBDsplyAll,
                      * pRBDsplyDscrp;
    ICheckBox         * pCBGenRPO;
    IStaticText       * pTXTWorkDir;
    IEntryField      *  pEFWorkDir;
    IPushButton       * pPBSave,
                      * pPBDefault,
                      * pPBReset,
                      * pPBCancel,
                      * pPBHelp;
    EUOPProfile       * pProfile;
    Boolean             fMyProfile;
    INotifyThread       NotifyThread;
    IResourceLibrary  & Reslib;
};
#pragma pack()

#endif

