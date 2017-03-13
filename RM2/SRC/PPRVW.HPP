#ifndef _AB_PPRVW_HPP_
#define _AB_PPRVW_HPP_

#define INCL_IEVENT
#define INCL_IGROUPBOX
#define INCL_IMENUHANDLER
#define INCL_IMESSAGEBOX
#define INCL_IMULTILINEEDIT
#define INCL_IPUSHBUTTON
#define INCL_IRESOURCELIBRARY
#define INCL_ISETCANVAS
#define INCL_ISTRING
#include "uicl.hpp"

#include "basew.hpp"
#include "mainw.hpp"

class MinSecTimer;

#pragma pack(4)
class PPrvCtl : public IMultiCellCanvas
{
    IWindow        * pParent,
                   * pOwner;
    IGroupBox        gb;
    IMultiLineEdit   MLE;
    void  ArrangeControls();
 public:
    PPrvCtl( unsigned long Id
           , IWindow * Parent
           , IWindow * Owner
           , unsigned width
           , unsigned lines
           , IString font
           , unsigned points
           );
    ~PPrvCtl() {};
    void        Fill(char * Buffer);
    Boolean     import(char * FileName);
    IMultiLineEdit & mle() {return MLE;}
    void        setGroupBoxText(char * t) { gb.setText(t); }
};

class PPrvW : public IFrameWindow
             ,public BaseWnd
             ,public IMenuHandler
             ,public ResMessage
             ,public MyWMHelpHandler
{
  public:
         PPrvW( unsigned long ulId
         ,char * B
         ,IWindow *  Parent
         ,IHelpWindow * pHW  = 0
         ,EUOPProfile *pProf = 0
         ,unsigned long ClosingMsg = 0
         ,Boolean  fPrintAvailable = true
         ,unsigned width = 78
         ,unsigned lines = 20
         ,IString font = IString("Courier")//jt
         ,unsigned points = 10
        );
   ~PPrvW();
    void            Fill(char * Buffer);
    Boolean         import(char * FileName);
    IWindow &       setFocus();
    unsigned long   showModally();
  protected:
//  ISize           calcMinimumSize() const;
    Boolean         makePopUpMenu(IMenuEvent&)
                   ,command(ICommandEvent&)
                   ,userMessage(IEvent&)
                   ,PressedEnter(IKeyboardEvent &)
                   ,wmhelpMessage(IEvent &);

  private:
    IWindow *              pParent;
    IString                ImportFile;
    IResourceLibrary     & rReslib;
    PPrvCtl                MLE;
    MyExceptionHandler     MyXHndlr;
    EUOPProfile          * pProfile;
    IMultiCellCanvas       MCCE;
    Boolean                fMyProfile,
                           fCanPrint;
    IPushButton          * pPBOK
                       , * pPBHelp;
    MinSecTimer         *  pTimer;
    IStaticText            TextTimer;
    void                   createExtension();
    void                   hideTimer();
    void                   showTimer();

};
#pragma pack()
#endif
