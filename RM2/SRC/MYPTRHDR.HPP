#ifndef _AB_MYPTRHDR_HPP_
#define _AB_MYPTRHDR_HPP_
 /*--------------------------------------------------------------------------*\
 |    File Name  : MYPTRHDR.HPP                                               |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |      MyHitTestHandler - WM_HITTEST messages dispatcher.                    |
 |                                                                            |
 |      MyPointerHandler - Manage the pointer by showing                      |
 |                         SPTR_WAIT or the given pointer                     |
 |                         over the handled disabled window,                  |
 |                         optionally activating the top                      |
 |                         level-one in the application.                      |
 |                                                                            |
 |      Optional Ctor parameters:                                             |
 |          ptr      : pointer of your choice                                 |
 |                     default( ISystemPointerHandle::wait )                  |
 |          f        : if true, a button1-click on the handled                |
 |                              window will activate the top level            |
 |                              window of the application.                    |
 |                     default(false)                                         |
 |                                                                            |
 |      Notes:                                                                |
 |         Only for PM window classes with the CS_HITTEST style set.          |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/
#include <ihandle.hpp>
#include <ipoint.hpp>
#include <ievent.hpp>
#include <ihandler.hpp>

#pragma pack(4)
class MyHitTestHandler : public IHandler
{
  protected:
    virtual Boolean testHit(IEvent&){return false;}
  public:
                MyHitTestHandler() : IHandler(){}
      virtual   ~MyHitTestHandler(){}
      Boolean dispatchHandlerEvent(IEvent& evt);
//           { return ((evt.eventId() == WM_HITTEST)? testHit(evt): false);}
};

class MyPointerHandler : public MyHitTestHandler
{
    IPointerHandle hndlBusyPtr;
    Boolean        peeking;
    Boolean        But1Activate;
  protected:
    Boolean testHit(IEvent&);
  public:
    MyPointerHandler( const IPointerHandle& ptr =
                              ISystemPointerHandle(ISystemPointerHandle::wait)
                     ,Boolean f = false
                    );
//      : MyHitTestHandler(), hndlBusyPtr(ptr), peeking(false), But1Activate(f){}
    IPointerHandle  pointerHandle () const { return hndlBusyPtr; }
    Boolean         SetPointer (IPointerHandle)
                 ,  SetPointer (unsigned long Id);
    unsigned        TrySetPointer(unsigned long Id);
//  void            SetPointer (const IPointerHandle& ptr) { hndlBusyPtr = ptr; }
};
#pragma pack()
#endif

