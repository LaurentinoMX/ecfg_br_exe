#ifndef _AB_MSGTHRD_HPP_
#define _AB_MSGTHRD_HPP_
 /*--------------------------------------------------------------------------*\
 |    File Name  : MSGTHRD.HPP                                                |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |      INotifyThread  - Run different types of functions in a                |
 |                       thread and optionally notify a window                |
 |                       upon completion, possibly with the                   |
 |                       function's result                                    |
 |                                                                            |
 |     In addition to type functions inherited from the IThread               |
 |     class (OptlinkFnPtr, SystemFnPtr and IReference<IThreadFn> &),         |
 |     the following function types are supported:                            |
 |                                                                            |
 |       2 functions types using structures (as defined below)                |
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

   /*------------------------------------------------------------------------*\
   |                            IBMClass includes                             |
   \*------------------------------------------------------------------------*/
#include <ithread.hpp>
#include <ievent.hpp>
#include <ihandle.hpp>


     /*----------------------------------------------------------------------*\
     |      Structures to be able to deliver the argument                     |
     |      and Return Code pointer as one parameter (the                     |
     |      structure's address) to the function to be                        |
     |      threaded.                                                         |
     |      Note: The _System type function receives                          |
     |            the structure's address as an unsigned                      |
     |            long and will have to type-cast it.                         |
     \*----------------------------------------------------------------------*/
#pragma pack(4)
typedef struct _OLFnParms
  { void                     * pOLArg;
    unsigned long            * pulRC;
    IWindowHandle              ihwndProgress;
  }OLFnParms;

typedef struct _SysFnParms
  { unsigned long              ulArg
                            ,* pulRC;
    IWindowHandle              ihwndProgress;
  }SysFnParms;
#pragma pack()

          /*-----------------------------------------------------------------*\
          |     Define the type of functions handled in addition to           |
          |      those inherited from the IThread class                       |
          \*-----------------------------------------------------------------*/
typedef void          (_Optlink *parmOptlinkFnPtr)(OLFnParms *);
typedef void          (_System  *parmSystemFnPtr)(unsigned long);
typedef unsigned long (_Optlink *ulOptlinkFnPtr)(void *);
typedef unsigned long (_System  *ulSystemFnPtr)(unsigned long);

          /*-----------------------------------------------------------------*\
          |        Internal function types. Declared here for class           |
          |        declaration but transparent to caller                      |
          \*-----------------------------------------------------------------*/
enum _funcType
  { THREAD_FN  = 1
   ,OPTLINK_FN
   ,PARM_OPTLINK_FN
   ,UL_OPTLINK_FN
   ,SYSTEM_FN
   ,PARM_SYSTEM_FN
   ,UL_SYSTEM_FN
  };

#pragma pack(4)
class INotifyThread : public IThread
{
  void preStart (_funcType,void *,unsigned long,void *,unsigned long ulArg = 0);
  protected:
    IWindowHandle         ntfyWindow;
    IWindowHandle         prgrsWindow;
    unsigned long         ntfyEventId;
    IEventParameter1      ntfyParam1;
    IEventParameter2      ntfyParam2;

  public:
      INotifyThread ();
      INotifyThread ( const IWindowHandle notifyHandle
                     ,const unsigned long notifyId = 0
                     ,const IEventParameter1 param1 = IEventParameter1()
                     ,const IEventParameter2 param2 = IEventParameter1());
      INotifyThread ( const IWindowHandle notifyHandle
                     ,const unsigned long notifyId
                     ,const IEventParameter1 param1
                     ,const IEventParameter2 param2
                     ,const IReference<IThreadFn> &aFnObjRef);
      INotifyThread ( const IWindowHandle notifyHandle
                     ,const unsigned long notifyId
                     ,const IEventParameter1 param1
                     ,const IEventParameter2 param2
                     ,OptlinkFnPtr    pfn
                     ,void          * anArg);
      INotifyThread ( const IWindowHandle notifyHandle
                     ,const unsigned long notifyId
                     ,const IEventParameter1 param1
                     ,const IEventParameter2 param2
                     ,SystemFnPtr     pfn
                     ,unsigned long   anArg);
      INotifyThread ( const IWindowHandle notifyHandle
                     ,const unsigned long notifyId
                     ,const IEventParameter1 param1
                     ,const IEventParameter2 param2
                     ,ulSystemFnPtr  pfn
                     ,unsigned long  anArg);
      INotifyThread ( const IWindowHandle notifyHandle
                     ,const unsigned long notifyId
                     ,const IEventParameter1 param1
                     ,const IEventParameter2 param2
                     ,ulOptlinkFnPtr  pfn
                     ,void          * anArg);
      INotifyThread ( const IWindowHandle notifyHandle
                     ,const IEventParameter1 param1
                     ,const IEventParameter2 param2
                     ,const unsigned long notifyId
                     ,const IWindowHandle progressHandle
                     ,parmSystemFnPtr  pfn
                     ,unsigned long   anArg);
      INotifyThread ( const IWindowHandle notifyHandle
                     ,const unsigned long notifyId
                     ,const IEventParameter1 param1
                     ,const IEventParameter2 param2
                     ,const IWindowHandle progressHandle
                     ,parmOptlinkFnPtr  pfn
                     ,void          * anArg);
      ~INotifyThread ( ) {};

    const IWindowHandle
                 notifyWindowHandle ( ) const { return ntfyWindow; };
    const IWindowHandle
                 progressWindowHandle ( ) const { return prgrsWindow; };
    void         setNotifyWindowHandle ( const IWindowHandle aHandle )
                          { ntfyWindow = aHandle; };
    void         setProgressWindowHandle ( const IWindowHandle aHandle )
                          { prgrsWindow = aHandle; };
    void         setNotifyEventId ( const unsigned long anId )
                          { ntfyEventId = anId; };
    void         setNotifyParam1 ( const IEventParameter1 aParam )
                          { ntfyParam1 = aParam; };
    void         setNotifyParam2 ( const IEventParameter2 aParam )
                          { ntfyParam2 = aParam; };
    void         start ( const IReference<IThreadFn> &aFnObjRef);
    void         start ( OptlinkFnPtr    pfn
                        ,void          * anArg
                        ,unsigned long lEventId = 0);
    void         start ( SystemFnPtr     pfn
                        ,unsigned long   anArg
                        ,unsigned long lEventId = 0);
    void         start ( parmOptlinkFnPtr  pfn
                        ,void        * anArg
                        ,unsigned long lEventId = 0);
    void         start ( ulOptlinkFnPtr  pfn
                        ,void          * anArg
                        ,unsigned long lEventId = 0);
    void         start ( ulSystemFnPtr  pfn
                        ,unsigned long  anArg
                        ,unsigned long lEventId = 0);
    void         start ( unsigned long  anArg
                        ,parmSystemFnPtr  pfn
                        ,unsigned long lEventId = 0);
    const IEventParameter1
                 notifyParam1 ( ) const { return ntfyParam1; };
    const IEventParameter2
                 notifyParam2 ( ) const { return ntfyParam2; };
    const unsigned long
                 notifyEventId ( ) const { return ntfyEventId; };

};
#pragma pack()

 /*---------------------------------------------------------------------------*\
 |   Programming Sample to run a CLASS' METHOD in a separate                   |
 |     thread with message notification:                                       |
 |                                                                             |
 |     class CLASS                                                             |
 |     {                                                                       |
 |        .                                                                    |
 |        .                                                                    |
 |        void   METHOD();                                                     |
 |        unsigned long HowYouDo();                                            |
 |     }                                                                       |
 |     class CALLER                                                            |
 |     {                                                                       |
 |       .                                                                     |
 |       .                                                                     |
 |      unsigned long Result;                                                  |
 |      INotifyThread MsgThread;                                               |
 |      CLASS * pClass;                                                        |
 |      void   Doit();                                                         |
 |     }                                                                       |
 |                                                                             |
 |                                                                             |
 |     void vOLFunctionParm(void *pV)                                          |
 |     {                                                                       |
 |      *(((OLFnParms *)pV)->pulRC) =                                          |
 |         (unsigned long)((CLASS *)(((OLFnParms *)pV)->pOLArg))->METHOD();    |
 |     }                                                                       |
 |                                                                             |
 |     unsigned long ulOLFunction(void *pV)                                    |
 |     {                                                                       |
 |      return (unsigned long)((CLASS *)pV)->METHOD();                         |
 |     }                                                                       |
 |                                                                             |
 |     void vOLFunction(void *pV)                                              |
 |     {                                                                       |
 |      ((CLASS *)pV)->METHOD();                                               |
 |     }                                                                       |
 |                                                                             |
 |     void _System vSLFunctionParm(unsigned long ul)                          |
 |     {                                                                       |
 |      *(((SysFnParms *)ul)->pulRC) =                                         |
 |         (unsigned long)((CLASS *)(((SysFnParms *)ul)->ulArg))->Method();    |
 |     }                                                                       |
 |                                                                             |
 |     unsigned long _System ulSLFunction(unsigned long ul)                    |
 |     {                                                                       |
 |      return (unsigned long)((CLASS *)ul)->METHOD();                         |
 |     }                                                                       |
 |                                                                             |
 |     void  _System vSLFunction(unsigned long ul)                             |
 |     {                                                                       |
 |      ((CLASS *)ul)->METHOD();                                               |
 |     }                                                                       |
 |                                                                             |
 |                                                                             |
 |     void CALLER::Doit()                                                     |
 |     {                                                                       |
 |      MsgThread.setNotifyWindowHandle(handle());                             |
 |      pClass = new CLASS();                                                  |
 |                        // retain sone data while functions working          |
 |      MsgThread.setNotifyParam2(IEventParameter2(SOME_DATA));                |
 |       if (getenv("DOIT_OLINK_NO_RC"))                                       |
 |         { MsgThread.setNotifyParam1(IEventParameter1(true)); // force OK RC |
 |           MsgThread.start((IThread::OptlinkFnPtr)vOLFunction,               |
 |                             pClass,UM_THREAD_DONE);                         |
 |         }                                                                   |
 |       else if (getenv("DOIT_SLINK_NO_RC"))                                  |
 |                  MsgThread.start((IThread::SystemFnPtr)vSLFunction,         |
 |                              (unsigned long)pClass,UM_THREAD_DONE);         |
 |       else if (getenv("DOIT_SLINK_RC"))                                     |
 |                MsgThread.start((ulSystemFnPtr)ulSLFunction,                 |
 |                                (unsigned long)pCLass,                       |
 |                                UM_THREAD_DONE_RC_IN_MP1);                   |
 |       else if (getenv("DOIT_OLINK_PARM"))                                   |
 |               MsgThread.start((parmOptlinkFnPtr)vOLFunctionParm,            |
 |                                pClass,UM_THREAD_DONE_RC_IN_MP1);            |
 |       else if (getenv("DOIT_SLINK_PARM"))                                   |
 |               MsgThread.start((unsigned long)pClass,                        |
 |                               (parmSystemFnPtr)vSLFunctionParm,             |
 |                                UM_THREAD_DONE_RC_IN_MP1);                   |
 |       else if (getenv("DOIT_OLINK_RC"))                                     |
 |               MsgThread.start((ulOptlinkFnPtr)ulOLFunction,                 |
 |                                pClass, UM_THREAD_DONE_RC_IN_MP1);           |
 |     }                                                                       |
 |                                                                             |
 |    Boolean CALLER :: userMessage(IEvent& evt)                               |
 |    {                                                                        |
 |     switch(evt.eventId())                                                   |
 |       { case UM_THREAD_DONE:                                                |
 |          Result = pClass->DowYouDo(); return true;                          |
 |         case UM_THREAD_DONE_RC_IN_MP1:                                      |
 |          Result = evt.parameter1().asUnsignedLong(); return true            |
 |       }                                                                     |
 |     return false;                                                           |
 |    }                                                                        |
 \*---------------------------------------------------------------------------*/


#endif

