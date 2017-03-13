 /*--------------------------------------------------------------------------*\
 |    Filename - MYXCPHDR.CPP                                                 |
 |      contains source for the following class implementation:               |
 |                                                                            |
 |      MyExceptionHandler  -  The class is intended as a default             |
 |                             Exception Handler for a window                 |
 |                             which will show and/or log exceptions          |
 |                             as described in the Ctor.                      |
 |                                                                            |
 |      Direct interface with the handler (as opposed to IBMClass-            |
 |      thrown exceptions) is via the public methods:                         |
 |            HandleException                                                 |
 |            LogException                                                    |
 |            HandleExceptionResponse                                         |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                       defines                       |
        \*---------------------------------------------------*/
                                                 // turn on entrance-exit
#define IC_TRACE_DEVELOP                         // signatures

        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/
#include "itrace.hpp"

#include <stdio.h>
char Buff[256];


        /*---------------------------------------------------*\
        |            Application-specific includes            |
        \*---------------------------------------------------*/
#include "myxcphdr.hpp"                          // this class' include

        /*-------------------------------------------------------------------*\
        |   Ctor                                                              |
        |     parms:                                                          |
        |        * IWindow : The window for which the handler                 |
        |                    is to be set                                     |
        |        * IWindow : The owner for the message box to be              |
        |                     popped up.                                      |
        |                     Default = NULL ->  no message boxes.            |
        |        IString&  :  The application name which will be appended     |
        |                     with the exception name in the title            |
        |                     of the Messagebox showing the exception.        |
        |                     Default : IString() -> none                     |
        \*-------------------------------------------------------------------*/
MyExceptionHandler :: MyExceptionHandler( IWindow * pW
                                         ,IWindow * pMBO
                                         ,const IString& App)
                      : Owner(pW)
                       ,MsgBoxOwner(pMBO)
                       ,AppName(App)
{
 if (pW && pW->isValid())
   pW->setExceptionFunction(this);
}

        /*-------------------------------------------------------------------*\
        |     Boolean handleException(IException&,IEvent&)                    |
        |       Divert the inherited but still                                |
        |       overloadable method to our own.                               |
        \*-------------------------------------------------------------------*/
Boolean MyExceptionHandler :: handleException(IException& exc,
                                              IEvent& event)
{ return HandleException(exc); }

        /*-------------------------------------------------------------------*\
        |     void LogException(IException&)                                  |
        |        method to handle the given exception as follows:             |
        |          If the application is being piped out to a file:           |
        |            Write the exception info to the file surrounded          |
        |              by the signature data of this method.                  |
        |        Returns: nothing                                             |
        \*-------------------------------------------------------------------*/
void MyExceptionHandler :: LogException(IException& exc)
{
 IFUNCTRACE_DEVELOP();
 TraceOut(exc);
}

        /*-------------------------------------------------------------------*\
        |     Boolean HandleException(IException&,Boolean)                    |
        |        method to handle the exception as follows:                   |
        |          If the application is being piped out to a file:           |
        |            Write the exception info to the file surrounded          |
        |              by the signature data of this method.                  |
        |          If class was so constructed,                               |
        |              Show a message box witthe the exception:               |
        |                 Append the exception name to the application        |
        |                 name and use the result as the title.               |
        |              Format the message text with the remainder of the      |
        |                 exception and show the box with just an OK button.  |
        |        Returns: the given Boolean :                                 |
        |         true  : Exception handled - continue dispatching events     |
        |         false : Exception's not handled and wil be re-thrown        |
        \*-------------------------------------------------------------------*/
Boolean MyExceptionHandler :: HandleException( IException& exception
                                              ,Boolean fReturn)
{
// IFUNCTRACE_DEVELOP();
// TraceOut(exception);
 if (MsgBoxOwner)
   { IMessageBox msgbox(MsgBoxOwner);
     IString str;
     FormatTitle(exception,str);
     msgbox.setTitle((char *)str);
     FormatText(exception,str);
     msgbox.show((char *)str, IMessageBox::okButton
                             | IMessageBox::informationIcon
                             | IMessageBox::applicationModal
                             | IMessageBox::moveable
				     | IMessageBox::systemModal);
   }
 return fReturn;
}

char * MyExceptionHandler::FormatErrorText( char * szMsg
                                           ,char * szSource
                                           ,int iLine)
{
 if (szSource)
   { sprintf(Buff,"%s (%s-%i)",szMsg,FindFileName(szSource),iLine);
     return Buff;
   }
 return szMsg;
}

void MyExceptionHandler :: ShowError( char * szTitle
                                     ,char * szMsg
                                     ,char * szSourceFile
                                     ,int iLine
                                    )
{
 IMessageBox msgbox( (MsgBoxOwner)? MsgBoxOwner : IWindow::desktopWindow());
 if (szTitle)
    msgbox.setTitle(szTitle);
 msgbox.show(FormatErrorText(szMsg,szSourceFile,iLine),
                               IMessageBox::okButton
                             | IMessageBox::informationIcon
                             | IMessageBox::applicationModal
                             | IMessageBox::moveable
				     | IMessageBox::systemModal);
}


IMessageBox::Response MyExceptionHandler ::
                            HandleMsgResponse( char * szMsg
                                              ,char * szTitle
                                              ,unsigned long HelpId
                                              ,IMessageBox::Style style
                                              ,char * szSourceFile
                                              ,int iLine)
{
 // making a local copy of the message since 'setTitle' below overwrites
 // the message if that pointer was typecasted from a resource string.
 IString msg(szMsg);
 IMessageBox::Response resp = IMessageBox::unknown;
 IMessageBox msgbox( (MsgBoxOwner)? MsgBoxOwner : IWindow::desktopWindow());
 if (szTitle)
    msgbox.setTitle(szTitle);
 resp = msgbox.show(FormatErrorText((char *)msg,szSourceFile,iLine),style,HelpId);
 return resp;
}

     /*----------------------------------------------------------------------*\
     |   IMessageBox::Response HandleExceptionResponse( IException&           |
     |                                                 ,IMessageBox::Style)   |
     |        method to handle the exception as per user as follows:          |
     |          If the application is being piped out to a file:              |
     |            Write the exception info to the file surrounded             |
     |              by the signature data of this method.                     |
     |          Show a message box with the the exception:                    |
     |            Append the exception name to the application                |
     |              name and use the result as the title.                     |
     |            Format the message text with the remainder of the           |
     |              exception and show the box with the given style.          |
     |            Obtain and return user's response after writing it          |
     |            to the trace file if appropriate.                           |
     |         Returns: IMessageBox::Response : user's response               |
     \*----------------------------------------------------------------------*/
IMessageBox::Response MyExceptionHandler ::
                            HandleExceptionResponse( IException& exception
                                                    ,IMessageBox::Style style)
{
 IFUNCTRACE_DEVELOP();
 TraceOut(exception);

#if 0

 unsigned long ulCnt = exception.textCount();
 const char * pszText = (ulCnt > 0) ? exception.text(ulCnt-1) :
                                       "No error text available";
 ITRACE_DEVELOP( exception.name());
// ITRACE_DEVELOP( IString("text count = ") + IString(ulCnt));
 ITRACE_DEVELOP( pszText );
// ulCnt = exception.locationCount();
// ITRACE_DEVELOP( IString("location count = ") + IString(ulCnt));
// const IExceptionLocation * pXLocation;
// while (ulCnt)
//   {  pXLocation = exception.locationAtIndex(--ulCnt);
//      if (pXLocation)
//        { ITRACE_DEVELOP( IString("location ") + IString(ulCnt) +
//                          IString(" :"));
//          ITRACE_DEVELOP( IString("file = ") + IString(pXLocation->fileName()));
//          ITRACE_DEVELOP( IString("function = ")
//                        + IString(pXLocation->functionName()));
//          ITRACE_DEVELOP( IString("lineNumber = ")
//                        + IString(pXLocation->lineNumber()));
//        }
//    }


#endif

 IMessageBox::Response resp = IMessageBox::unknown;
 IWindow * pW = (MsgBoxOwner)? MsgBoxOwner : IWindow::desktopWindow();
 IMessageBox msgbox(pW);
 IString str;
 FormatTitle(exception,str);
 msgbox.setTitle((char *)str);
 FormatText(exception,str);
// IMessageBox::Response resp = msgbox.show((char *)str,style);
 resp = msgbox.show((char *)str,style);
 ITRACE_DEVELOP( IString("User Response = ") + IString(resp));
 return resp;
// return (msgbox.show(pszText,style));
}

        /*-------------------------------------------------------------------*\
        |     void FormatTitle(IException&,IString&)                          |
        |        method to format an Exception Title in the given             |
        |        string for the given exception by appending the              |
        |        exception name to the application name.                      |
        |        Returns: nothing.                                            |
        \*-------------------------------------------------------------------*/
void MyExceptionHandler :: FormatTitle(IException& exc,IString& str)
{
 str = (AppName.size())? AppName + IString(" - ") + IString(exc.name())
                        : IString(exc.name());
// str = IString("EUOP - ");
// str = AppName + IString(" - ") + IString(exc.name());
// str += IString(exc.name());



}

        /*-------------------------------------------------------------------*\
        |     void FormatText(IException&,IString&)                           |
        |        method to format the Exception Text in the given             |
        |        string from the given exception by appending                 |
        |        the method, filename and line number for every               |
        |        location in the exception.                                   |
        |        Returns: nothing.                                            |
        \*-------------------------------------------------------------------*/
void MyExceptionHandler :: FormatText(IException& exc,IString& str)
{
 str = IString( ((exc.textCount() > 0) ? exc.text(exc.textCount()-1) :
                                       "No exception text available"));
 if (exc.locationCount())
   { const IExceptionLocation * pXLocation = exc.locationAtIndex(0);
     if (pXLocation)
       { str += IString(" [");
         str += IString(pXLocation->functionName());
         str += IString(" (");
         str += IString(FindFileName(pXLocation->fileName()));
         str += IString("-");
         str += IString(pXLocation->lineNumber());
         str += IString(")]");
       }
   }
}

        /*-------------------------------------------------------------------*\
        |    const char * FindFileName(const char *szPath)                    |
        |        method return the pointer to the filename in a given         |
        |        null terminated string containing a fully qualified          |
        |        file specification.                                          |
        \*-------------------------------------------------------------------*/
const char * MyExceptionHandler :: FindFileName(const char *szPath)
{
 if (!szPath) return szPath;
 const char *sz;
 for (sz = szPath ; *sz ; sz++){}
 for ( ; sz >= szPath && *sz != '\\' && *sz != ':' ; sz--){}
 return ++sz;
}

        /*-------------------------------------------------------------------*\
        |    void TraceOut(IException&)                                       |
        |        method to dump the exception data to a file                  |
        |        if the application is tracing and piping.                    |
        |        Returns nothing.                                             |
        \*-------------------------------------------------------------------*/
void MyExceptionHandler :: TraceOut(IException& exception)
{
 if (ITrace().isTraceEnabled())
   {
     unsigned long ulCnt = exception.textCount();
     const char * pszText = (ulCnt > 0) ? exception.text(ulCnt-1) :
                                          "No error text available";
     ITRACE_DEVELOP( exception.name());
     ITRACE_DEVELOP( IString("text count = ") + IString(ulCnt));
     ITRACE_DEVELOP( pszText );
     ulCnt = exception.locationCount();
     ITRACE_DEVELOP( IString("location count = ") + IString(ulCnt));
     const IExceptionLocation * pXLocation;
     while (ulCnt)
      {  pXLocation = exception.locationAtIndex(--ulCnt);
         if (pXLocation)
           { ITRACE_DEVELOP( IString("location ") + IString(ulCnt) +
                             IString(" :"));
             ITRACE_DEVELOP(IString("file = ")
                            + IString(pXLocation->fileName()));
             ITRACE_DEVELOP( IString("function = ")
                             + IString(pXLocation->functionName()));
             ITRACE_DEVELOP( IString("lineNumber = ")
                             + IString(pXLocation->lineNumber()));
           }
      }
   }
}

