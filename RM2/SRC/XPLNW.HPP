#ifndef _AB_XPLNW_HPP_
#define _AB_XPLNW_HPP_
 /*--------------------------------------------------------------------------*\
 |    File Name  : XPLNW.HPP                                                  |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |      XPLNWndw         - Explanation Window                                 |
 |                                                                            |
 |      Window has the following features or peculiarities:                   |
 |         Minimum Size  - The window can NOT be sized smaller by the         |
 |                         user than the width occupied by the                |
 |                         pushbuttons and a height required to show the      |
 |                         number of lines given as a parameter               |
 |                         (defaulting to 10 with a minimum of 3).            |
 |                                                                            |
 |         Data Reentrant- The same instance of the window can be given       |
 |                         different Explanation without having to re-create  |
 |                         it. For multiple appearances of Lists, the window  |
 |                         has been designed to be 'shown' and 'hid' and      |
 |                         given data via the 'FillMLE' method.               |
 |                                                                            |
 |         Initial State - Invisible                                          |
 |                         To be positioned by the caller.                    |
 |                         The initial and minimum height of the              |
 |                            window is dependend on the                      |
 |                            'NumberofLines' parmeter which defaults         |
 |                            to 10 while anything less than 3 is ignored.    |
 |                                                                            |
 |         Interface     - There is only one response to the window:          |
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
 |                           itself.                                          |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/

#define INCL_IPUSHBUTTON
#define INCL_IPUSHBUTTON
#define INCL_IRESOURCELIBRARY
#define INCL_ISETCANVAS
#include "uicl.hpp"

        /*---------------------------------------------------*\
        |               EUOP specific includes                |
        \*---------------------------------------------------*/
#include "basew.hpp"
#include "mytrkhdr.hpp"                          // track handler
#include "xplncntl.hpp"                          // ExplainCntrl

class EUOPProfile;                               // profile forward declaration

                                                 // minimum number of vertical
#define XPLNWND_MIN_NMBR_LINES 3                 // lines

#pragma pack(4)
class XPLNWndw : public IFrameWindow
                ,public BaseWnd
                ,public MyTrackHandler
{
  public:
    XPLNWndw( unsigned long windowId
             ,XPLN2List *
             ,IHelpWindow * pHW = 0
             ,IWindow * Parent = 0
             ,unsigned long ClosingMsg = 0
             ,long NumberOfLines = 12
            );
   ~XPLNWndw();
    void            Add2(XPLN2List *)
                 ,  Fill(XPLN2List *);
    IWindow &       setFocus();
    unsigned long   showModally();
  protected:
    virtual void    CreatePBs();
    Boolean         command(ICommandEvent&),
                    SetMinTrackSize(IPoint&),
                    PressedEnter(IKeyboardEvent &),
                    userMessage(IEvent&);
    ISize           calcMinimumSize() const;

  private:
    ISetCanvas           * pSCPBs;
    IPushButton          * pPBCancel,
                         * pPBHelp;
    IResourceLibrary     & rReslib;
    XPLN2List            * pXPLNList;
    ExplainCntrlMCC        XPLNCntl;
};
#pragma pack()
#endif
