 /*--------------------------------------------------------------------------*\
 |    Filename - MSGTHRD.CPP                                                  |
 |      contains source for the following class implementation:               |
 |      INotifyThread  - Run different types of functions in a                |
 |                       thread and optionally notify a window                |
 |                       upon completion, possibly with the                   |
 |                       function's result                                    |
 |                                                                            |
 |     In addition to type functions inherited from the IThread               |
 |     class (OptlinkFnPtr, SystemFnPtr and IReference<IThreadFn> &),         |
 |     the following function types are supported:                            |
 |                                                                            |
 |       2 functions types using structures (as defined in MSGTHRD.HPP)       |
 |         to combine the argument and return code into a single              |
 |         parameter to the threaded function.                                |
 |                                                                            |
 |             parmOptlinkFnPtr: takes a 'OLFnParms' structure                |
 |                               pointer as an argument, while                |
 |                               using the '_Optlink' calling                 |
 |                               convention and returning nothing.            |
 |                                                                            |
 |             parmSystemFnPtr : takes (a '_SysFnParms' structure             |
 |                               pointer as) an unsigned long argument        |
 |                               while using the '_System' calling            |
 |                               convention and returning nothing.            |
 |                                                                            |
 |       2 function types taking the argument and returning                   |
 |         the result:                                                        |
 |                                                                            |
 |             ulOptlinkFnPtr  : takes any pointer as an argument,            |
 |                               while using the '_Optlink' calling           |
 |                               convention and returning an                  |
 |                               'unsigned long'                              |
 |                                                                            |
 |             ulSystemFnPtr   : takes and returns an 'unsigned long'         |
 |                               as an argument and return code               |
 |                               respectively, while using the                |
 |                               '_System' calling convention.                |
 |                                                                            |
 |     Notes: Appended, Programming Sample to run a class' member             |
 |            function                                                        |
 \*--------------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------*\
    |                            IBMClass includes                            |
    \*-----------------------------------------------------------------------*/
#include <iwindow.hpp>

    /*-----------------------------------------------------------------------*\
    |                         Class-specific includes                         |
    \*-----------------------------------------------------------------------*/
#include "msgthrd.hpp"

    /*-----------------------------------------------------------------------*\
    |                                 defines                                 |
    \*-----------------------------------------------------------------------*/

        /*-------------------------------------------------------------------*\
        |  struct _funcParams                                                 |
        |      Structure used to carry pertinent data to the                  |
        |      threaded function 'callFnAndNotify' which will                 |
        |      call the function to be run and optionally                     |
        |      notify the function's ending and result.                       |
        |      Structure is declared here since it's transparent              |
        |      to the caller and no class declaration depends on it           |
        \*-------------------------------------------------------------------*/
#pragma pack(4)
struct _funcParams
{
  enum _funcType             fnType;             // function type
  INotifyThread            * theThread;          // our class pointer
  void                     * fn;                 // function to be run
  void                     * optLinkArg;         // _Optlink type argument
  unsigned long              systemArg;          // _System type argument
  IReference < IThreadFn >   thrdFnRef;          // class member function
  Boolean                    autoInitPM;         // Init PM flag
};
#pragma pack()

       /*--------------------------------------------------------------------*\
       |    void callFnAndNotify ( struct _funcParams * )                     |
       |       (Non-member thread function.)                                  |
       |       Call the to be run function from our thread                    |
       |       and possibly notify the caller:                                |
       |                                                                      |
       |         Get addressability to our class, init the                    |
       |         return code and run the function according                   |
       |         to its type:                                                 |
       |          THREAD_FN     :                                             |
       |             Class member function.                                   |
       |                                                                      |
       |                 Just dispatch the function stored in our             |
       |                 structure as is.                                     |
       |                 It'll have its class at its disposal for             |
       |                 anything it might need and notify.                   |
       |                                                                      |
       |          SYSTEM_FN    :                                              |
       |              Straight _System type function returning nothing.       |
       |                                                                      |
       |                 Use the IThread type and cast the function           |
       |                 from our structure and give it its argument.         |
       |                                                                      |
       |          PARM_SYSTEM_FN:                                             |
       |              Straight _System type function taking the               |
       |              address (as an unsigned long) of a 'SysFnParms'         |
       |              structure which contains the function's argument        |
       |              and return code pointer.                                |
       |                                                                      |
       |                  Locally declare the structure and stuff if          |
       |                  with the local Return Code's address and            |
       |                  function's argument. Next, cast the function to     |
       |                  its proper type and give it the structure's         |
       |                  pointer as an unsigned long argument.               |
       |                  Upon completion, set the function's result in       |
       |                  our class.                                          |
       |                                                                      |
       |          UL_SYSTEM_FN:                                               |
       |              Straight _System type function taking an                |
       |              argument and returning a result of                      |
       |              'unsigned long'.                                        |
       |                                                                      |
       |                  Cast the function to its proper type and            |
       |                  give it its argument.                               |
       |                  Upon completion, set the function's result in       |
       |                  our class.                                          |
       |                                                                      |
       |          OPTLINK_FN:                                                 |
       |              Straight _OptLink type function taking a                |
       |              pointer argument                                        |
       |                                                                      |
       |                  Cast the function to its proper type and            |
       |                  give it its argument.                               |
       |                                                                      |
       |          PARM_OPTLINK_FN:                                            |
       |              Straight _OptLink type function taking the              |
       |              address of an 'OLFnParms' structure which               |
       |              contains the function's argument and return             |
       |              code pointer.                                           |
       |                                                                      |
       |                  Locally declare the structure and stuff if          |
       |                  with the local Return Code's address and            |
       |                  function's argument. Next, cast the function to     |
       |                  its proper type and give it the structure's         |
       |                  pointer as an argument.                             |
       |                  Upon completion, set the function's result in       |
       |                  our class.                                          |
       |                                                                      |
       |          UL_OPTLINK_FN:                                              |
       |              Straight _OptLink type function taking an               |
       |              argument and returning a result of                      |
       |              'unsigned long'.                                        |
       |                                                                      |
       |                  Cast the function to its proper type and            |
       |                  give it its argument.                               |
       |                  Upon completion, set the function's result in       |
       |                  our class.                                          |
       |                                                                      |
       |         Notify the caller with the optional result in MPARAM1        |
       |         if it gave its handle.                                       |
       |                                                                      |
       |         De-allocate the structure received as an argument and        |
       |         allocated in member function 'preStar'                       |
       \*--------------------------------------------------------------------*/
void callFnAndNotify ( struct _funcParams * parms )
{
 INotifyThread * theThread = parms->theThread;
 unsigned long ulRC = 0;
 switch (parms->fnType)
   {
    case THREAD_FN:
      (parms->thrdFnRef)->run();
      break;
    case SYSTEM_FN:
      ((IThread::SystemFnPtr)(parms->fn))(parms->systemArg);
      break;
    case PARM_SYSTEM_FN:
      { SysFnParms sysParms;
        sysParms.pulRC = &ulRC;
        sysParms.ulArg = parms->systemArg;
        sysParms.ihwndProgress = theThread->progressWindowHandle();
        ((parmSystemFnPtr)parms->fn)((unsigned long)&sysParms);
        theThread->setNotifyParam1(IEventParameter1(ulRC));
       break;
      }
    case UL_SYSTEM_FN:
        ulRC = ((ulSystemFnPtr)parms->fn)(parms->systemArg);
        theThread->setNotifyParam1(IEventParameter1(ulRC));
       break;
    case OPTLINK_FN:
        ((IThread::OptlinkFnPtr)parms->fn)(parms->optLinkArg);
        break;
    case PARM_OPTLINK_FN:
      { OLFnParms olParms;
        olParms.pulRC = &ulRC;
        olParms.pOLArg = parms->optLinkArg;
        olParms.ihwndProgress = theThread->progressWindowHandle();
        ((parmOptlinkFnPtr)parms->fn)(&olParms);
        theThread->setNotifyParam1(IEventParameter1(ulRC));
        break;
      }
    case UL_OPTLINK_FN:
        ulRC = ((ulOptlinkFnPtr)parms->fn)(parms->optLinkArg);
        theThread->setNotifyParam1(IEventParameter1(ulRC));
        break;
    }
  if (theThread->notifyWindowHandle())
    theThread->notifyWindowHandle().postEvent(theThread->notifyEventId(),
                                              theThread->notifyParam1(),
                                              theThread->notifyParam2());
  delete parms;
}


  /*-------------------------------------------------------------------------*\
  |                            INotifyThread Class                            |
  \*-------------------------------------------------------------------------*/
       /*--------------------------------------------------------------------*\
       |   Ctors                                                              |
       |     Common parameters:                                               |
       |          IWindowHandle aHandle : the window to be notified           |
       |          unsigned long anId    ; the notification message Id         |
       |          IEventParameter1 param1                                     |
       |          IEventParameter2 param2 : the message parameters            |
       |                                                                      |
       |     Notes: If a result is to be returned, it'll be contained         |
       |               in the message's MPARAM1                               |
       |            If a function pointer is given, the thread will           |
       |               be started according to its type.                      |
       \*--------------------------------------------------------------------*/
INotifyThread::INotifyThread ()
              : IThread ( )
               ,ntfyWindow(0)
               ,prgrsWindow(0)
               ,ntfyEventId(0)
               ,ntfyParam1(0)
               ,ntfyParam2(0)
{}

INotifyThread::INotifyThread( const IWindowHandle aHandle
                             ,const unsigned long anId
                             ,const IEventParameter1 param1
                             ,const IEventParameter2 param2
                            )
              : IThread ( )
               ,ntfyWindow(aHandle)
               ,ntfyEventId(anId)
               ,ntfyParam1(param1)
               ,ntfyParam2(param2)
{}      //used

INotifyThread::INotifyThread (const IWindowHandle aHandle
                             ,const unsigned long anId
                             ,const IEventParameter1 param1
                             ,const IEventParameter2 param2
                             ,const IReference<IThreadFn> &aFnObjRef)
              : IThread ( )
               ,ntfyWindow(aHandle)
               ,ntfyEventId(anId)
               ,ntfyParam1(param1)
               ,ntfyParam2(param2)
{ start(aFnObjRef); }

INotifyThread::INotifyThread ( const IWindowHandle aHandle
                              ,const unsigned long anId
                              ,const IEventParameter1 param1
                              ,const IEventParameter2 param2
                              ,OptlinkFnPtr    pfn
                              ,void          * anArg)
              : IThread ( )
               ,ntfyWindow(aHandle)
               ,ntfyEventId(anId)
               ,ntfyParam1(param1)
               ,ntfyParam2(param2)
{ start (pfn, anArg); }

INotifyThread::INotifyThread ( const IWindowHandle aHandle
                              ,const unsigned long anId
                              ,const IEventParameter1 param1
                              ,const IEventParameter2 param2
                              ,SystemFnPtr     pfn
                              ,unsigned long   anArg)
              : IThread ( )
               ,ntfyWindow(aHandle)
               ,ntfyEventId(anId)
               ,ntfyParam1(param1)
               ,ntfyParam2(param2)
{ start (pfn, anArg); }

INotifyThread::INotifyThread ( const IWindowHandle aHandle
                              ,const unsigned long anId
                              ,const IEventParameter1 param1
                              ,const IEventParameter2 param2
                              ,ulSystemFnPtr  pfn
                              ,unsigned long  anArg)
              : IThread ( )
               ,ntfyWindow(aHandle)
               ,ntfyEventId(anId)
               ,ntfyParam1(param1)
               ,ntfyParam2(param2)
{ start (pfn, anArg); }

INotifyThread::INotifyThread ( const IWindowHandle aHandle
                              ,const unsigned long anId
                              ,const IEventParameter1 param1
                              ,const IEventParameter2 param2
                              ,ulOptlinkFnPtr  pfn
                              ,void          * anArg)
              : IThread ( )
               ,ntfyWindow(aHandle)
               ,ntfyEventId(anId)
               ,ntfyParam1(param1)
               ,ntfyParam2(param2)
{ start (pfn, anArg); }

INotifyThread::INotifyThread ( const IWindowHandle ntfyHandle
                              ,const IEventParameter1 param1
                              ,const IEventParameter2 param2
                              ,const unsigned long anId
                              ,const IWindowHandle prgrsHandle
                              ,parmSystemFnPtr  pfn
                              ,unsigned long   anArg)
              : IThread ( )
               ,ntfyWindow(ntfyHandle)
               ,prgrsWindow(prgrsHandle)
               ,ntfyEventId(anId)
               ,ntfyParam1(param1)
               ,ntfyParam2(param2)
{ start (pfn, anArg); }

INotifyThread::INotifyThread ( const IWindowHandle ntfyHandle
                              ,const unsigned long anId
                              ,const IEventParameter1 param1
                              ,const IEventParameter2 param2
                              ,const IWindowHandle prgrsHandle
                              ,parmOptlinkFnPtr  pfn
                              ,void          * anArg)
             : IThread ( )
              ,ntfyWindow(ntfyHandle)
              ,prgrsWindow(prgrsHandle)
              ,ntfyEventId(anId)
              ,ntfyParam1(param1)
              ,ntfyParam2(param2)
{ start (pfn, anArg); }


  /*-------------------------------------------------------------------------*\
  |                        Starters - by function type                        |
  \*-------------------------------------------------------------------------*/
void  INotifyThread::start ( OptlinkFnPtr    pfn
                            ,void          * anArg
                            ,unsigned long lEventId)
{ preStart(OPTLINK_FN,pfn,lEventId,anArg); }

void INotifyThread::start ( SystemFnPtr     pfn
                           ,unsigned long   anArg
                           ,unsigned long lEventId)
{ preStart(SYSTEM_FN,pfn,lEventId,0,anArg); }

void INotifyThread::start ( parmOptlinkFnPtr  pfn
                           ,void        * anArg
                           ,unsigned long lEventId)
{ preStart(PARM_OPTLINK_FN,pfn,lEventId,anArg); }

void INotifyThread::start ( ulOptlinkFnPtr  pfn
                           ,void          * anArg
                           ,unsigned long lEventId)  //used
{ preStart(UL_OPTLINK_FN,pfn,lEventId,anArg); }

void INotifyThread::start ( ulSystemFnPtr  pfn
                           ,unsigned long  anArg
                           ,unsigned long lEventId)
{ preStart(UL_SYSTEM_FN,pfn,lEventId,0,anArg); }

void INotifyThread::start ( unsigned long  anArg
                           ,parmSystemFnPtr  pfn
                           ,unsigned long lEventId)
{ preStart(PARM_SYSTEM_FN,pfn,lEventId,0,anArg); }


       /*--------------------------------------------------------------------*\
       |    void  preStart(_funcType  Type      // our function type          |
       |                   ,void * pfn          // function to be threaded    |
       |                   ,unsigned long ulId  // notifying message id       |
       |                   ,void   * pvArg      // _OptLink funtion argument  |
       |                   ,unsigned long ulArg // _System function argument  |
       |                  )                                                   |
       |        Set the message ID, allocate our structure                    |
       |        for 'callFnAndNotify', stuff it with the                      |
       |        remaining parameters and spin off the function                |
       |        via our inherited 'start' method                              |
       \*--------------------------------------------------------------------*/
void INotifyThread :: preStart ( _funcType  Type
                                 ,void * pfn
                                 ,unsigned long ulId
                                 ,void   * pvArg
                                 ,unsigned long ulArg)
{
  if (ulId) ntfyEventId = ulId;
  struct _funcParams * parms = new struct _funcParams;
  parms->fnType     = Type;
  parms->theThread  = this;
  parms->fn = pfn;
  parms->optLinkArg = pvArg;
  parms->systemArg = ulArg;
  IThread::start( (OptlinkFnPtr)callFnAndNotify,(void *) parms);
}

       /*--------------------------------------------------------------------*\
       |    void start ( const IReference < IThreadFn > &aFnObjRef)           |
       |        start a class member method                                   |
       |        Allocate our structure for 'callFnAndNotify',                 |
       |        stuff it with the remaining parameters and spin               |
       |        off the function via our inherited 'start' method             |
       \*--------------------------------------------------------------------*/
void INotifyThread :: start ( const IReference < IThreadFn > &aFnObjRef)
{
 struct _funcParams * parms = new struct _funcParams;
 parms->fnType     = THREAD_FN;
 parms->theThread  = this;
 parms->thrdFnRef  = aFnObjRef;
 IThread::start((OptlinkFnPtr)callFnAndNotify,(void *) parms);
}

 /*--------------------------------------------------------------------------*\
 |   Programming Sample to run a CLASS' METHOD in a separate                  |
 |     thread with message notification:                                      |
 |                                                                            |
 |     class CLASS                                                            |
 |     {                                                                      |
 |        .                                                                   |
 |        .                                                                   |
 |        void   METHOD();                                                    |
 |        unsigned long HowYouDo();                                           |
 |     }                                                                      |
 |     class CALLER                                                           |
 |     {                                                                      |
 |       .                                                                    |
 |       .                                                                    |
 |      unsigned long Result;                                                 |
 |      INotifyThread MsgThread;                                              |
 |      CLASS * pClass;                                                       |
 |      void   Doit();                                                        |
 |     }                                                                      |
 |                                                                            |
 |                                                                            |
 |     void vOLFunctionParm(void *pV)                                         |
 |     {                                                                      |
 |      *(((OLFnParms *)pV)->pulRC) =                                         |
 |         (unsigned long)((CLASS *)(((OLFnParms *)pV)->pOLArg))->METHOD();   |
 |     }                                                                      |
 |                                                                            |
 |     unsigned long ulOLFunction(void *pV)                                   |
 |     {                                                                      |
 |      return (unsigned long)((CLASS *)pV)->METHOD();                        |
 |     }                                                                      |
 |                                                                            |
 |     void vOLFunction(void *pV)                                             |
 |     {                                                                      |
 |      ((CLASS *)pV)->METHOD();                                              |
 |     }                                                                      |
 |                                                                            |
 |     void _System vSLFunctionParm(unsigned long ul)                         |
 |     {                                                                      |
 |      *(((SysFnParms *)ul)->pulRC) =                                        |
 |         (unsigned long)((CLASS *)(((SysFnParms *)ul)->ulArg))->Method();   |
 |     }                                                                      |
 |                                                                            |
 |     unsigned long _System ulSLFunction(unsigned long ul)                   |
 |     {                                                                      |
 |      return (unsigned long)((CLASS *)ul)->METHOD();                        |
 |     }                                                                      |
 |                                                                            |
 |     void  _System vSLFunction(unsigned long ul)                            |
 |     {                                                                      |
 |      ((CLASS *)ul)->METHOD();                                              |
 |     }                                                                      |
 |                                                                            |
 |                                                                            |
 |     void CALLER::Doit()                                                    |
 |     {                                                                      |
 |      MsgThread.setNotifyWindowHandle(handle());                            |
 |      pClass = new CLASS();                                                 |
 |                        // retain sone data while functions working         |
 |      MsgThread.setNotifyParam2(IEventParameter2(SOME_DATA));               |
 |       if (getenv("DOIT_OLINK_NO_RC"))                                      |
 |         { MsgThread.setNotifyParam1(IEventParameter1(true));// force OK RC |
 |           MsgThread.start((IThread::OptlinkFnPtr)vOLFunction,              |
 |                             pClass,UM_THREAD_DONE);                        |
 |         }                                                                  |
 |       else if (getenv("DOIT_SLINK_NO_RC"))                                 |
 |                  MsgThread.start((IThread::SystemFnPtr)vSLFunction,        |
 |                              (unsigned long)pClass,UM_THREAD_DONE);        |
 |       else if (getenv("DOIT_SLINK_RC"))                                    |
 |                MsgThread.start((ulSystemFnPtr)ulSLFunction,                |
 |                                (unsigned long)pCLass,                      |
 |                                UM_THREAD_DONE_RC_IN_MP1);                  |
 |       else if (getenv("DOIT_OLINK_PARM"))                                  |
 |               MsgThread.start((parmOptlinkFnPtr)vOLFunctionParm,           |
 |                                pClass,UM_THREAD_DONE_RC_IN_MP1);           |
 |       else if (getenv("DOIT_SLINK_PARM"))                                  |
 |               MsgThread.start((unsigned long)pClass,                       |
 |                               (parmSystemFnPtr)vSLFunctionParm,            |
 |                                UM_THREAD_DONE_RC_IN_MP1);                  |
 |       else if (getenv("DOIT_OLINK_RC"))                                    |
 |               MsgThread.start((ulOptlinkFnPtr)ulOLFunction,                |
 |                                pClass, UM_THREAD_DONE_RC_IN_MP1);          |
 |     }                                                                      |
 |                                                                            |
 |    Boolean CALLER :: userMessage(IEvent& evt)                              |
 |    {                                                                       |
 |     switch(evt.eventId())                                                  |
 |       { case UM_THREAD_DONE:                                               |
 |          Result = pClass->DowYouDo(); return true;                         |
 |         case UM_THREAD_DONE_RC_IN_MP1:                                     |
 |          Result = evt.parameter1().asUnsignedLong(); return true           |
 |       }                                                                    |
 |     return false;                                                          |
 |    }                                                                       |
 \*--------------------------------------------------------------------------*/


