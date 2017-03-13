#ifndef _DJL_ICTRLHANDLER_
#define _DJL_ICTRLHANDLER_

#include <ihandler.hpp>
#include "ictrlevt.hpp"

class ICtrlHandler : public IHandler
{
  protected:
    virtual Boolean
      control(ICtrlEvent& ctrlEvt) = 0;

  public:
      ICtrlHandler ( ) {;};

    Boolean
      dispatchHandlerEvent(IEvent& evt);
};
#endif
