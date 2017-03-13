#ifndef _AB_MYFCSHDR_HPP_
#define _AB_MYFCSHDR_HPP_
 /*--------------------------------------------------------------------------*\
 |    File Name  : MYFCSHDR.HPP                                               |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |      MyFocusHandler  -  Catches WM_FOCUSCHANGE messages and                |
 |                         makes them available via its virtual               |
 |                         methods:                                           |
 |                           gettingFocus(IEvent&)                            |
 |                           losingFocus(IEvent&)                             |
 |                                                                            |
 |            Notes: attach to individual controls.                           |
 |                       ------------------                                   |
 |      MyWindowFocusHandler - Being based on 'MyFocusHandler',               |
 |                             it hides 'losingFocus', replacing              |
 |                             it with                                        |
 |                                   LostFocus(IEvent&)                       |
 |                             which is only called when a control            |
 |                             loses focus which is not gained by any         |
 |                             other controls handled by the class'           |
 |                             instance.                                      |
 |                       ------------------                                   |
 |      MyEFSetSelectHandler - Sets the contents of the handled               |
 |                             EntryField to 'selected' when the              |
 |                             control gains focus.                           |
 |                                                                            |
 |          Notes: attachable only to IEntryFields.                           |
 |                       ------------------                                   |
 |      MyActiveAppHandler   -  Maintains the active status of an             |
 |                              application which can be obtained             |
 |                              via     'isActive()',                         |
 |                              while routing a change of that status         |
 |                              to its virtual methods:                       |
 |                                 gainingActivation()                        |
 |                                 losingActivation()                         |
 |                                                                            |
 |          Notes:  attachable only to IFrameWindows (preferably              |
 |                  the top level window since the messaging                  |
 |                  travels up the owner chain)                               |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                  IBMClass Includes                  |
        \*---------------------------------------------------*/
#include <ihandler.hpp>
#include <iframe.hpp>
#include <ientryfd.hpp>

#pragma pack(4)
class MyFocusHandler : public IHandler
{
  public:
              MyFocusHandler() : IHandler(){};
    virtual  ~MyFocusHandler(){};
  protected:
    virtual Boolean gettingFocus(IEvent&) { return false;};
    virtual Boolean losingFocus(IEvent&) { return false;};
    Boolean dispatchHandlerEvent(IEvent&);
};

class MyEFSetSelectHandler : public MyFocusHandler
{
  public:
              MyEFSetSelectHandler() : MyFocusHandler() {};
    virtual   ~MyEFSetSelectHandler() {};
    IHandler    &    handleEventsFor(IEntryField *);
    IHandler    &    stopHandlingEventsFor(IEntryField *);
  protected:
    virtual Boolean gettingFocus(IEvent&);
  private:
    IHandler    &    handleEventsFor(IWindow *) {return *this;};
    IHandler    &    stopHandlingEventsFor(IWindow *) {return *this;};
//  IHandler    &    enable(Boolean){return *this;};
//  IHandler    &    disable(){return *this;};
//  Boolean          isEnabled()const;
};


#define  ARRAY_INCREMENT  5
class MyWindowFocusHandler : public MyFocusHandler
{
    IWindow  **   ppWindows
            ,**   ppTemp;
    unsigned      uCount;

    IWindow **    GetNWindowPtrs(unsigned);
    IWindow *     WindowPtr(IWindowHandle);
    Boolean       PtrArrayIndex(unsigned *)
               ,  losingFocus(IEvent&)
               ,  WindowIndex(IWindowHandle,unsigned *)
               ,  Add(IWindow *)
               ,  Ours(IWindowHandle)
               ,  WindowsKid(IWindow *,IWindowHandle)
               ,  Remove(IWindow *);
    void          StopAll();
  public:
              MyWindowFocusHandler(unsigned Count = ARRAY_INCREMENT);
    virtual   ~MyWindowFocusHandler();
    IHandler    &    handleEventsFor(IWindow *);
    IHandler    &    stopHandlingEventsFor(IWindow *);
  protected:
    virtual Boolean LostFocus(IEvent&) { return false;};
};

class MyActiveAppHandler : public IHandler
{
    Boolean fActive;
    IHandler    &    handleEventsFor(IWindow *) {return *this;};
    IHandler    &    stopHandlingEventsFor(IWindow *) {return *this;};
  public:
                     MyActiveAppHandler();
    virtual         ~MyActiveAppHandler(){};
    Boolean          isActive() { return fActive;};
    IHandler    &    handleEventsFor(IFrameWindow *);
    IHandler    &    stopHandlingEventsFor(IFrameWindow *);
  protected:
    virtual Boolean gainingActivation() { return false;};
    virtual Boolean losingActivation() { return false;};
    Boolean dispatchHandlerEvent(IEvent&);
};

#pragma pack()

#endif





