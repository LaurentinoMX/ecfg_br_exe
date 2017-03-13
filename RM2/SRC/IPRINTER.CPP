#define  IC_TRACE_DEVELOP
#include <itrace.hpp>

/********************************************************************/
/*  This is the  IPrinter class                                     */
/*                                                                  */
/********************************************************************/
#define INCL_GPI
#define INCL_DEV
#define INCL_SPL
#define INCL_SPLDOSPRINT
#define INCL_WINERRORS

//jt extern "C"
//jt {
   #include <os2.h>
   #include <memory.h>
//jt }

#include <iexcbase.hpp>
#include <ifont.hpp>
#include <ilistbox.hpp>
#include <imcelcv.hpp>
#include <iprofile.hpp>
#include <ipushbut.hpp>
#include <isetcv.hpp>
#include <istring.hpp>
#include <ithread.hpp>

#include "iprinter.h"
#include "iprinter.hpp"

/********************************************************************/
/*  Constructor                                                     */
/********************************************************************/
IPrinter :: IPrinter( unsigned uCopies )
          : IPrinterQueueHandler( )
          , printerOpen(0)
          , font(0)
          , hab(0)
          , hdc(0)
          , hps(0)
          , orientation(unknown )
          , iCurrentFormIndex ( -1 )
          , ulCopies( uCopies )
{
   open( );
}

IPrinter :: IPrinter( IString QName, unsigned uCopies, IString newDriverData )
          : IPrinterQueueHandler( QName, newDriverData )
          , printerOpen(0)
          , font(0)
          , hab(0)
          , hdc(0)
          , hps(0)
          , orientation(unknown )
          , iCurrentFormIndex ( -1 )
          , ulCopies( uCopies )
{
   open( );
}

IPrinter :: IPrinter( IString QName, IString newDriverData, unsigned uCopies )
          : IPrinterQueueHandler( QName, newDriverData )
          , printerOpen(0)
          , font(0)
          , hab(0)
          , hdc(0)
          , hps(0)
          , orientation(unknown )
          , iCurrentFormIndex ( -1 )
          , ulCopies( uCopies )
{
   open( );
}

IPrinter :: ~IPrinter()
{
   if( printerOpen )
      close();
}

/********************************************************************/
/*  Function   :  Open                                              */
/********************************************************************/
void IPrinter :: open( )
{
}   /*  end of Open */


/********************************************************************/
/*  IPrinter::print with only the IString parameter. Other params   */
/*  used are from member variables.                                 */
/********************************************************************/
void IPrinter :: print( IString line, unsigned newWidth )
{
   width = newWidth;
   printLine( line, pointSize, currentPos, width );
}

/********************************************************************/
/*  IPrinter::print with all parameters, with the possible exception*/
/*  of width which defaults to 0 to print the whole line.           */
/********************************************************************/
void IPrinter :: print( IString line, IPoint& position, unsigned newWidth )
{
   width = newWidth;
   printLine( line, pointSize, position, width );
}


/********************************************************************/
/*  This function is called after all it's parameters have been     */
/*  resolved in the IPrinter::print member functions                */
/*  This member function modifies the callers pos reference to      */
/*  indicate upon return the position where we will print next.     */
/*  The user can then examine the variable and determine whether    */
/*  the next line will be positioned properly, or whether a new     */
/*  page is required.                                               */
/********************************************************************/
void IPrinter :: printLine( IString line, int pointSize, IPoint& pos, unsigned width )
{
}

void IPrinter :: setFontSize( unsigned long newPointSize )
{
   if( font != 0 )
   {
      font->endUsingFont( hps );
      font->setPointSize( newPointSize, hps );
      pointSize = newPointSize;
      lLeading = pointSize * res.y() / 72 ;
      font->beginUsingFont( hps );
   } /* endif */
}

// reset the font to the printer's default font
void IPrinter :: setFont( )
{
   if( font != 0 )
   {
      font->endUsingFont( hps );
      font = 0;
//jt      os2-edw
//jt      FONTMETRICS fm;
//jt      GpiQueryFontMetrics( hps, sizeof( FONTMETRICS ), &fm );
//jt      lLeading = fm.lEmHeight;
//edw win95
      TEXTMETRIC tm;
      GetTextMetrics( hdc, &tm);
      lLeading = tm.tmHeight;
   } /* endif */
}


void IPrinter :: setFont( IFont* xpFont, unsigned long newPointSize )
{
   Boolean fontChanged = false;

   if( ( newPointSize != 0 ) && ( pointSize != newPointSize ) )
      fontChanged = true;

   if( newPointSize != 0 )
      pointSize = newPointSize;
   else
      pointSize = xpFont->pointSize();

   /*****************************************************************/
   /*  if the font is changing...                                   */
   /*****************************************************************/
   if( xpFont != font )
      fontChanged = true;

   /**************************************************************/
   /*  If a fonts has been previously defined, stop using it     */
   /**************************************************************/
   if( fontChanged )
   {
      if( font != 0 )
         font->endUsingFont( hps );

      xpFont->setPointSize( pointSize, hps );
      xpFont->beginUsingFont( hps );
      font = xpFont;
      lLeading = pointSize;    //edwtest lLeading = pointSize * res.y() / 72 ;
   }
}

IString IPrinter :: jobProperties( ) const
{
   IString driverData( pDriverData
                     , driverDataLen );

   return( driverData );
}

IString IPrinter :: queueDescription( )  const
{
   //edw --not called
   IString qDescription; //jt ( pQueueInfo[selectedQueue].pszComment );
   return( qDescription );
}

IString IPrinter :: queueDriverName( )   const
{
   //edw--os2
   //edw IString qDriverName( pQueueInfo[selectedQueue].pszDriverName );

   //edw--win95
   //edw LPDEVNAMES lpDevNames = (LPDEVNAMES)GlobalLock(pd.hDevNames);
   //edw IString qDriverName = (char*) lpDevNames + lpDevNames->wDriverOffset;
   //edw GlobalUnlock(pd.hDevNames);

   //edw return( qDriverName );
   return(""); //edw
}

Boolean IPrinter :: getForm()
{
      return TRUE;
}

/********************************************************************/
/*  Returns current margin settings in pels                         */
/********************************************************************/
unsigned IPrinter :: margin( Margin whichMargin )
{
   return ( margins[whichMargin] );
}

/*************************************************************************
 *  Function   :  setJobProperties
 *
 *  Description:  Query job properties (print properties) of the printer
 *                driver.
 *
 *  Parameters :  PPRQINFO3       pointer to queue information structure
 *
 *  API's      :  DevPostDeviceModes
 *
 *  Return     :  TRUE if ok, otherwise FALSE
 *
 *************************************************************************/
void IPrinter :: setJobProperties(  )
{
}  /* End of QueryJobProperties */

void IPrinter :: startJob( IString jobName)
{
}

void IPrinter :: newPage()
{

}

void IPrinter :: cancelJob()
{
}

void IPrinter :: endJob()
{

}

void IPrinter :: close()
{
}

void IPrinter :: getPrinterResolution(  )
{
}

/********************************************************************/
/*  PrinterDialog Constructor                                       */
/********************************************************************/
IPrinterDialog :: IPrinterDialog( IWindow* owner, IString QName )
                : IFrameWindow( PRINTER_SELECT
                              , desktopWindow()
                              , owner
//                              , IRectangle( ISize( 250, 300 ))
                              , IRectangle()
                              , IFrameWindow::dialogBorder | IFrameWindow::titleBar
                              , "Select Printer" )
                , IPrinterQueueHandler( QName )
                , ownerWindow( owner )
                , clientCanvas( WND_CANVAS, this, this )
                , listBox( LB_PRINTERS, &clientCanvas, &clientCanvas )
                , OkPressed( true )
{

}

unsigned long IPrinterDialog :: showModally()
{
return 0;
}

/********************************************************************/
/*  Add printer queues to listbox                                   */
/********************************************************************/
void IPrinterDialog :: addPrinters()
{
}

Boolean IPrinterDialog :: enter( IControlEvent& evt )
{
   return true;
}

Boolean IPrinterDialog :: command(ICommandEvent& cmdevt)
{
   return(false);                          //Allow Default Processing to occur
} /* end IPrinterDialog :: command(...) */

/********************************************************************/
/*  IPrinterQueueHandler contructor                                 */
/********************************************************************/
IPrinterQueueHandler :: IPrinterQueueHandler( IString QName, IString newDriverData )
                  : qName( QName )
                  , pDriverData( 0 )
                  , driverDataLen( 0 )
                  , pQueueInfo( 0 )
                  , numberOfQs( 0 )
{
}

/********************************************************************/
/*  IPrinterQueueHandler destructor                                 */
/********************************************************************/
IPrinterQueueHandler :: ~IPrinterQueueHandler(  )
{
}


/********************************************************************/
/*  Returns TRUE if pQueueInfo contains valid information.          */
/*  FALSE if there was an error.                                    */
/*  This function allocates pQueueInfo.                             */
/********************************************************************/
Boolean IPrinterQueueHandler :: getQInfo( )
{
   return true;
} /* end getQInfo */

void IPrinterQueueHandler :: getDefaultSystemQueue( )
{
} /* end getDefaultSystemQueue */

/*************************************************************************
 *
 *   Function   :  findQueue
 *
 *   Description:  Finds the queue name in the PRQINFO3 structure.
 *
 *   Parameters :  PSZ          pointer to the name of the queue
 *                 PPRQINFO3    pointer to queue selection structure 1
 *                 int        number of queue names to compare
 *
 *   API's      :  [none]
 *
+ *************************************************************************/
unsigned IPrinterQueueHandler :: findQueue(PSZ pszQueueName, _PRQINFO3* pQueueInfo, unsigned lCount)
{
}  /* End of findQueue */
