//=======================================================================
// FILE NAME        RTRNRET.HPP
//
// CLASS NAME       Return Retrieval Window for Messages
//
// DESCRIPTION
//
// Programmer:      Zoraya Obregon - 53363
//=======================================================================
// Copyright (c) International Business Machines Corp. 1998
// All rights reserved
//=======================================================================

#ifndef RTRNRET_HPP
#define RTRNRET_HPP

#include <icmdhdr.hpp>                  // ICommandHandler
#include <iframe.hpp>                   // IFrameWindow
#include <ikeyhdr.hpp>                  // IKeyboardHandler

#include "Af9main.hpp"                    // MainWindow

class IPushButton;
class IStaticText;

/*-------------------------------------------------------------------------*/
/* Class: ReturnRetrieval                                                  */
/*-------------------------------------------------------------------------*/
class ReturnRetrieval : public IFrameWindow,
                   public ICommandHandler,
                   public IKeyboardHandler
{
public:
   ReturnRetrieval(MainWindow* pOwner);
   ~ReturnRetrieval();
   Boolean bContinue;

protected:
   Boolean command(ICommandEvent& cmdEvt);
   Boolean systemCommand(ICommandEvent& cmdEvt);
   Boolean virtualKeyPress(IKeyboardEvent& keyEvt);

private:
   MainWindow*  pXWinOwn;         // Owner Window
   IPushButton* ppbOk;  // OK Button
   IPushButton* ppbCancel;  // Cancel Button
   IPushButton* ppbHelp;  // Help Button

   void  buildControls(); // Create control objects
};

#endif
