#ifndef _AB_WW_HPP_
#define _AB_WW_HPP_

   /*------------------------------------------------------------------------*\
   |                            IBMClass includes                             |
   \*------------------------------------------------------------------------*/
#define INCL_IHANDLER
#define INCL_ITHREAD
#define INCL_IOBJECTWINDOW
#include "uicl.hpp"

#define WW_UM_READY WM_USER+3333

#pragma pack(4)
class IWorkUserHandler : public IHandler
{
  public:
               IWorkUserHandler() : IHandler() {};
              ~IWorkUserHandler() {};
  protected:
    virtual Boolean WorkUserMessage(IEvent&) {return false;}
    Boolean dispatchHandlerEvent(IEvent& );
};

class WorkWindow : public IWorkUserHandler
{
 public:
    WorkWindow();
   ~WorkWindow();
    Boolean      StartUp(IWindowHandle ihwndParent,
                         unsigned long ReadyMessage = WW_UM_READY);
    void         CloseDown();
    Boolean      Busy() {return fBusy;};
   IEventResult sendEvent ( unsigned long           eventId,
                            const IEventParameter1 &parm1 = 0,
                            const IEventParameter2 &parm2 = 0 ) const
              , sendEvent ( const IEvent&  event ) const;
   Boolean      postEvent ( unsigned long           eventId,
                            const IEventParameter1 &parm1 = 0,
                            const IEventParameter2 &parm2 = 0 ) const
              , postEvent ( const IEvent&  event ) const;
  private:
  IObjectWindow  * pOW;
  IThread        * WorkThread;
  Boolean          fBusy;
  IWindowHandle    ihwndPa;
  unsigned long    ReadyMsg;
  void             StartWorkThread(void);
 protected:
    Boolean        WorkUserMessage(IEvent&);
    virtual Boolean   WorkWindowMessage(IEvent&) {return false;};
};
#pragma pack()
#endif

