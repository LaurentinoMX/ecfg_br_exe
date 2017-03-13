#ifndef _AB_USRHDR_HPP_
#define _AB_USRHDR_HPP_
 /*--------------------------------------------------------------------------*\
 |    File Name  : USRHDR.HPP                                                 |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration and source                         |
 |      of the following class implementation:                                |
 |                                                                            |
 |      MyUserMsgHandler  - Provide access to user-messages by routing        |
 |                      them to the virtual                                   |
 |                           userMessage(IEvent&)                             |
 |                      method                                                |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                     PM Includes                     |
        \*---------------------------------------------------*/
extern "C"
  {
   #define INCL_WINMESSAGEMGR
   #include <os2.h>
  }

        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/
#include <ihandler.hpp>

#pragma pack(4)
class MyUserMsgHandler : public IHandler
{
  public:
               MyUserMsgHandler() : IHandler() {};
     virtual  ~MyUserMsgHandler() {};
  protected:
    virtual Boolean userMessage(IEvent&) {return false;}
    Boolean dispatchHandlerEvent(IEvent& evt)
       { return (evt.eventId() >= WM_USER)? userMessage(evt) : false; }
};
#pragma pack()
#endif
