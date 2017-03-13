#ifndef _AB_MINMXHDR_HPP_
#define _AB_MINMXDR_HPP_
 /*--------------------------------------------------------------------------*\
 |    File Name  : MYTRKHDR.HPP                                               |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |      MyMinMaxHandler - Route the WM_MINMAXFRAME messages                   |
 |                        to the following virtual methods:                   |
 |                          Maximizing(ISWP *)                                |
 |                          Minimizing(ISWP *)                                |
 |      Note : Handler can only be started for frames                         |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/

#include <ihandle.hpp>
#include <iframe.hpp>
#include <iswp.hpp>
#include <ievent.hpp>
#include <ihandler.hpp>
#include <iswp.hpp>

#pragma pack(4)
class MyMinMaxHandler : public IHandler
{
    IHandler    &   handleEventsFor(IWindow *) {return *this;};
  protected:
    virtual Boolean Maximizing(ISWP *) { return false;}
    virtual Boolean Minimizing(ISWP *) { return false;}
    virtual Boolean Restoring (ISWP *) { return false;}
  public:
                    MyMinMaxHandler ( ) : IHandler() {}
      virtual      ~MyMinMaxHandler ( ) {}
      IHandler &    handleEventsFor(IFrameWindow *pFW)
                    { return IHandler::handleEventsFor(pFW);}
      Boolean       dispatchHandlerEvent(IEvent&);
};

class MyDlgMinMaxHandler : public MyMinMaxHandler
{
  protected:
    virtual Boolean Maximizing(ISWP * );
    virtual Boolean Minimizing(ISWP * );
};
#pragma pack()
#endif

