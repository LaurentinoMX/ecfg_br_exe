#ifndef _AB_MYXCPHDR_HPP_
#define _AB_MYXCPHDR_HPP_
 /*--------------------------------------------------------------------------*\
 |    File Name  : MYXCPHDR.HPP                                               |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |      MyExceptionHandler  -  The class is intended as a default             |
 |                             Exception Handler for a window                 |
 |                             which will show and/or log exceptions          |
 |                             as Ctor parameters.                            |
 |                                                                            |
 |      Direct interface with the handler (as opposed to IBMClass-            |
 |      thrown exceptions) is via the public methods:                         |
 |            HandleException                                                 |
 |            LogException                                                    |
 |            HandleExceptionResponse                                         |
 |                                                                            |
 |       Ctor parameters in order:                                            |
 |         parms:                                                             |
 |            * IWindow : The window for which the handler                    |
 |                        is to be set                                        |
 |            * IWindow : The owner for the message box to be                 |
 |                         popped up.                                         |
 |                         Default = null ->  no message boxes.               |
 |            IString&  :  The application name which will be appended        |
 |                         with the exception name in the title               |
 |                         of the Messagebox showing the exception.           |
 |                         Default : IString() -> none                        |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/
#include <iwindow.hpp>
#include <iexcbase.hpp>
#include <imsgbox.hpp>
#include <itrace.hpp>

#pragma pack(4)
class MyExceptionHandler : public IWindow::ExceptionFn
{
 public:
   MyExceptionHandler( IWindow *
                      ,IWindow * pMessageBoxOwner = 0
                      ,const IString& App = IString());
   virtual Boolean handleException(IException & ,IEvent & );
   Boolean HandleException(IException &,Boolean fReturn = true );
   void    LogException(IException &);
   void    ShowError(char * szTitle,char * szMsg,
                     char * szSourceFile,int iLine );
   IMessageBox::Response HandleExceptionResponse(IException&
                                                 ,IMessageBox::Style
                                                      style = IMessageBox::okButton
                                                              | IMessageBox::informationIcon
                                                              | IMessageBox::applicationModal
                                                              | IMessageBox::moveable
										  | IMessageBox::systemModal);
   IMessageBox::Response HandleMsgResponse( char * szMsg
                                           ,char * szTitle = 0
                                           ,unsigned long HelpId = 0
                                           ,IMessageBox::Style
                                                 style = IMessageBox::okButton
                                                         | IMessageBox::informationIcon
                                                         | IMessageBox::applicationModal
                                                         | IMessageBox::moveable
									   | IMessageBox::systemModal
                                           ,char * szSourceFile = 0
                                           ,int iLine = 0);

   IMessageBox::Response HandleErrMsgResponse( char * szMsg
                                              ,char * szTitle = 0
                                              ,unsigned long HelpId = 0
                                              ,IMessageBox::Style
                                                style = IMessageBox::okButton
                                                      | IMessageBox::errorIcon
                                                      | IMessageBox::applicationModal
                                                      | IMessageBox::moveable
									| IMessageBox::systemModal
                                             ,char * szSourceFile = 0
                                             ,int iLine = 0)
      { return HandleMsgResponse(szMsg,szTitle,HelpId,style,szSourceFile,iLine);}

 private:
   IWindow * Owner;
   IWindow * MsgBoxOwner;
   IString AppName;
   void TraceOut(IException &);
   void FormatTitle(IException &,IString &);
   void FormatText(IException &,IString &);
   const char * FindFileName(const char *szPath);
   char *       FormatErrorText( char * szMsg ,char * szSource ,int iLine);
};
#pragma pack()

#endif
