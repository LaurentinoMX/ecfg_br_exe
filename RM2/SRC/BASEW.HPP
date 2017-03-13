#ifndef _AB_BASEW_HPP_
#define _AB_BASEW_HPP_


        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/
#define INCL_ICOMMANDHANDLER
#define INCL_IFRAMEWINDOW
#define INCL_IHELPHANDLER
#define INCL_IHELPWINDOW
#define INCL_IKEYBOARDHANDLER
#include "uicl.hpp"

   /*------------------------------------------------------------------------*\
   |                         Class specific includes                          |
   \*------------------------------------------------------------------------*/
#include "myptrhdr.hpp"
#include "euoutil.hpp"
#include "myusrhdr.hpp"                            // user messages

#pragma pack(4)
class BaseWnd : public ICommandHandler
               ,public IKeyboardHandler
               ,public MyUserMsgHandler
{
public:
  enum YPlacement
    {
       bottom
      ,ycenter
      ,top
    };
  enum XPlacement
    {
       left  = top
      ,xcenter
      ,right
    };
   BaseWnd( IFrameWindow * pFW
           ,IWindow * ClosingMsgOwner = 0
           ,unsigned long ClosingMsg = 0
           ,IHelpWindow * pHW = 0
          );
  ~BaseWnd() {}
  void             Center();
  IWindow        * ClosingMsgOwner() {return MsgOwner;}
  unsigned long    ClosingMessage() { return ClosingMsg;}
  void             setClosingMsg(unsigned long Msg) { ClosingMsg = Msg;}
  void             setClosingMsgOwner(IWindow * pW) { MsgOwner = pW;}
  void             Able(Boolean fEn = true);
  void             PlaceWnd(XPlacement,YPlacement,IWindow * pParent);
  Boolean          SetDisabledPointer(IPointerHandle hndl)
                         { return ptrHdr.SetPointer(hndl); }
  void             SetupHelp(IHelpWindow *);
  IHelpWindow *    helpWindow() { return pHLPWndw;}
  IHelpHandler &   helpHandler() { return hlpHandler;}
  void             SetTitle(const char *);
  void             AbleSystemClose(Boolean fEnable = true);
  void             AbleSystemMenuItem(unsigned long Id,Boolean fEnable = true);
  void             AbleHelpHandler(Boolean fEnable = true);
  Boolean          disableNow();

protected:
  virtual void     NotifyMyClosing(unsigned long parm1 = 0
                                   ,unsigned long parm2 = 0);
  virtual Boolean  command(ICommandEvent&) { return false; }
  virtual Boolean  systemCommand(ICommandEvent&);
  virtual Boolean  virtualKeyPress(IKeyboardEvent &);
  virtual Boolean  PressedEsc_F3(IKeyboardEvent &);
  virtual Boolean  PressedEnter(IKeyboardEvent &) { return false;}
  virtual Boolean  PressedTab(IKeyboardEvent &) { return false;}

private:
  IWindow             * MsgOwner;
  IFrameWindow        * Me;
  unsigned long         ClosingMsg;
  MyPointerHandler      ptrHdr;
  IHelpWindow         * pHLPWndw;
  IHelpHandler          hlpHandler;
};
#pragma pack()

#endif

