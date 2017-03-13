#ifndef _DJL_ICTRLEVNT_
#define _DJL_ICTRLEVNT_

#include <ievent.hpp>

class ICtrlEvent : public IEvent
{
  public:
      ICtrlEvent (IEvent& event);

    unsigned short
      controlId ( ) const;

    unsigned short
      notifyCode ( ) const;
};
#endif
