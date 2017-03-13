#ifndef _DJL_USERHDR_
#define _DJL_USERHDR_

class IUserHandler : public IHandler
{
  protected:
    virtual Boolean
      userMessage(IEvent& event) = 0;
    Boolean
      dispatchHandlerEvent(IEvent& evt);

  public:
      IUserHandler ( ) {};
};
#endif
