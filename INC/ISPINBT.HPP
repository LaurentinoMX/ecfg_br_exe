#ifndef _ISPINNUM_
  #define _ISPINNUM_
/*******************************************************************************
* FILE NAME: ispinnum.hpp                                                      *
*                                                                              *
* DESCRIPTION:                                                                 *
*   Declaration of the classes:                                                *
*     INumericSpinButton                                                       *
*                                                                              *
* COPYRIGHT:                                                                   *
*   IBM Open Class Library                                                     *
*   (C) Copyright International Business Machines Corporation 1992, 1996       *
*   Licensed Material - Program-Property of IBM - All Rights Reserved.         *
*   US Government Users Restricted Rights - Use, duplication, or disclosure    *
*   restricted by GSA ADP Schedule Contract with IBM Corp.                     *
*                                                                              *
*******************************************************************************/

#include <ispinbas.hpp>
#include <irect.hpp>

class IRange;
class INumericSpinButtonData;

#pragma pack(4)

class INumericSpinButton : public IBaseSpinButton {
typedef IBaseSpinButton
  Inherited;

public :
class Style;
/*------------------------------- Constructors -------------------------------*/
  INumericSpinButton ( unsigned long     id,
                       IWindow*          parent,
                       IWindow*          owner,
                       const IRectangle& initial = IRectangle(),
                       const Style&      style   = defaultStyle() );

  INumericSpinButton ( unsigned long     id,
                       IWindow*          parent );

  INumericSpinButton ( const IWindowHandle& handle );

virtual
 ~INumericSpinButton ( );

/*---------------------------------- Styles ----------------------------------*/
INESTEDBITFLAGCLASSDEF3(Style, INumericSpinButton, IBaseSpinButton,
                        IWindow, IControl);

static const Style
  IC_IMPORTU classDefaultStyle,
  IC_IMPORTU padWithZeros;

static Style
  defaultStyle      ( );

static void
  setDefaultStyle   ( const Style&    style );

  virtual unsigned long
    convertToGUIStyle ( const IBitFlag& style,
                        Boolean         extendedOnly = false ) const;

/*------------------------- Number Range Operations --------------------------*/
virtual INumericSpinButton
 &setRange ( const IRange& aNewRange,
             Boolean       override = false ),
 &spinTo   ( long          aValue,
             Boolean       spinToClosest = false ),
 &setValue ( long aValue );

virtual IRange
  range ( ) const;

virtual long
  value ( ) const;

virtual Boolean
  isSpinFieldValid ( Boolean caseSensitive = false ) const;


/*-------------------------- Observer Notification ---------------------------*/
virtual INumericSpinButton
 &enableNotification ( Boolean enable=true );

protected:


private:
/*------------------------------ Hidden Members ------------------------------*/
INumericSpinButton ( const INumericSpinButton& spinButton );
INumericSpinButton
 &operator=        ( const INumericSpinButton& spinButton );

/*--------------------------------- Private ----------------------------------*/
static Style
  currentDefaultStyle;

INumericSpinButtonData
 *fNumericSpinButtonData;

};  // INumericSpinButton

INESTEDBITFLAGCLASSFUNCS( Style, INumericSpinButton);

  #include <ispinnum.inl>

#pragma pack()

#define ISpinButton  INumericSpinButton

#endif // _ISPINNUM_
