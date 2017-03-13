//=======================================================================
// FILE NAME        SPECWND.HPP
//
// CLASS NAME       Special Window for Messages
//
// DESCRIPTION      
//
// Programmer:      Miguel Angel Martínez Tlahuel - 47973
//=======================================================================
// Copyright (c) International Business Machines Corp. 1998
// All rights reserved
//=======================================================================

#ifndef SPECWND_HPP
#define SPECWND_HPP

#include <icmdhdr.hpp>                  // ICommandHandler
#include <iframe.hpp>                   // IFrameWindow
#include <ikeyhdr.hpp>                  // IKeyboardHandler

#include "mainw.hpp"			    // XWindow

class IPushButton;
class IStaticText;

/*-------------------------------------------------------------------------*/
/* Class: SpecWindow                                                       */
/*-------------------------------------------------------------------------*/
class SpecWindow : public IFrameWindow,
                   public ICommandHandler,
                   public IKeyboardHandler
{
public:
   SpecWindow(XWindow* pOwner);
   ~SpecWindow();
   Boolean bContinue;
      
protected:
   Boolean command(ICommandEvent& cmdEvt);
   Boolean systemCommand(ICommandEvent& cmdEvt);
   Boolean virtualKeyPress(IKeyboardEvent& keyEvt);
 
private:
   XWindow*       pXWinOwn;         // Owner Window
   IPushButton*	ppbOk;		// OK Button
   IPushButton*	ppbCancel;		// Cancel Button
   IPushButton*	ppbHelp;		// Help Button

   void		buildControls();	// Create control objects
};

#endif
