#define INCL_DOS
#define INCL_PM
#include <os2.h>
#include <istring.hpp>
#include <iwindow.hpp>
#include "keyhndlr.hpp"

ISelectiveKeyHandler :: ISelectiveKeyHandler ( ) :
  IKeyboardHandler ( ),
  theLimitString   ((IString *) NULL),
  toCaps           ((Boolean)  false)
{
  theLimitString = new IString;
};

ISelectiveKeyHandler :: ISelectiveKeyHandler ( const IString & aString ) :
  IKeyboardHandler ( ),
  theLimitString   ((IString *) NULL),
  toCaps           ((Boolean)  false)
{
  theLimitString = new IString(aString);
};

ISelectiveKeyHandler :: ISelectiveKeyHandler ( const Boolean caps ) :
  IKeyboardHandler ( ),
  theLimitString   ((IString *)  NULL),
  toCaps           ((Boolean)    caps)
{
  theLimitString = new IString;
};

ISelectiveKeyHandler :: ISelectiveKeyHandler ( const IString & aString,
                       const Boolean   caps ) :
  IKeyboardHandler ( ),
  theLimitString   ((IString *) NULL),
  toCaps           ((Boolean)   caps)
{
  theLimitString = new IString(aString);
};

void ISelectiveKeyHandler :: limitInputTo ( const IString & aString )
{
  *theLimitString = aString;
};

Boolean ISelectiveKeyHandler :: characterKeyPress(IKeyboardEvent& keyevt)
{
  if ( !keyevt.isCharacter() || keyevt.isAltDown() || keyevt.isCtrlDown()
       || keyevt.isUpTransition() )
    return False;
  char charCode = keyevt.character();
  if ( isCapitalizing() &&
       (charCode >= 'a') &&
       (charCode <= 'z') )
    {
    charCode += ('A'-'a');
    MPARAM tempParam = (MPARAM)keyevt.parameter2();
    USHORT usVirt = SHORT2FROMMP(tempParam);
    keyevt.setResult(keyevt.window()->sendEvent(keyevt.eventId(),
                                                keyevt.parameter1(),
                                                MPFROM2SHORT(charCode,usVirt)));
    return True;
    };
  if ((theLimitString->length() != 0) && !theLimitString->includes(charCode))
    {
    //jt    WinAlarm( HWND_DESKTOP, WA_WARNING );
    MessageBeep(MB_ICONEXCLAMATION);  //jt
    return True;
    };
  return False;
};
