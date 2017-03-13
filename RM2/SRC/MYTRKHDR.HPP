#ifndef _AB_MYTRKHDR_HPP_
#define _AB_MYTRKHDR_HPP_
 /*--------------------------------------------------------------------------*\
 |    File Name  : MYTRKHDR.HPP                                               |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |      MyTrackHandler  - Restrict a frame's sizing capabilities              |
 |                        via the following virtual methods:                  |
 |                           SetMinTrackSize(IPoint&)                         |
 |                           SetMaxTrackSize(IPoint&)                         |
 |                           SetTrackBoundary(IRectangle&)                    |
 |                           SetMaximizedRectangle(IRectangle&)               |
 |                                                                            |
 |      Note : Handler can only be started for sizeable                       |
 |             frame windows via the 'StartIfSizable' method                  |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/
#include <ihandle.hpp>
#include <ipoint.hpp>
#include <iframe.hpp>
#include <irect.hpp>
#include <ievent.hpp>
#include <ihandler.hpp>
#include <iswp.hpp>

#pragma pack(4)
class MyTrackHandler : public IHandler
{
//  IHandler    &   handleEventsFor(IWindow *) {return *this;};
  protected:
    virtual Boolean SetMinTrackSize(IPoint&) { return false;}
    virtual Boolean SetMaxTrackSize(IPoint&) { return false;}
    virtual Boolean SetTrackBoundary(IRectangle&) { return false;}
    virtual Boolean SetMaximizedRectangle(IRectangle&) { return false;}
  public:
                    MyTrackHandler ( ) : IHandler() {}
      virtual      ~MyTrackHandler ( ) {}
      Boolean       StartIfSizable(IFrameWindow *)
                 ,  dispatchHandlerEvent(IEvent&);
};
#pragma pack()
#endif

