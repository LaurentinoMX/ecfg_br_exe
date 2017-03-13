/********************************************************************/
/*  NOTE: that due a restriction in OS/2 2.1 a printer job MUST     */
/*  be started before any fonts can be defined for that job.        */
/*  In other words you MUST issue a "startJob( jobName )" call      */
/*  before any IFonts can be defined for this printer.              */
/********************************************************************/
#ifndef __IPRINTER_H
#define __IPRINTER_H


struct _PRQINFO3;
struct _DRIVDATA;

#include <iframe.hpp>
#include <icmdhdr.hpp>
#include <ihandle.hpp>
#include <iselhdr.hpp>
#include <imcelcv.hpp>
#include <ilistbox.hpp>
#include <ihandle.hpp>           //IHandle edw

const long qNotFound = -1;

/********************************************************************/
/*  Forward class declarations.                                     */
/********************************************************************/
class IFont;
class ISetCanvas;
class IPushButton;

class IPrinterDialog;

class IPrinterQueueHandler
{
public:       /*----------------------------------------------------*/
   /*****************************************************************/
   /*  Constructor                                                  */
   /*****************************************************************/
   IPrinterQueueHandler( IString QName = IString(), IString newDriverData = IString());

   /*****************************************************************/
   /*  Destructor                                                   */
   /*****************************************************************/
   virtual ~IPrinterQueueHandler();

   /*****************************************************************/
   /*  Accessor member functions                                    */
   /*****************************************************************/
   IString queueName() const { return qName;}

   unsigned numberOfQueues() const { return numberOfQs; }

protected:    /*----------------------------------------------------*/

   /*****************************************************************/
   /*  Protected member functions                                   */
   /*****************************************************************/
   Boolean getQInfo();

   unsigned findQueue( char *pszQueueName
                     , _PRQINFO3* pQueueInfo
                     , unsigned lCount );

   void getDefaultSystemQueue();

   /*****************************************************************/
   /*  Protected data members                                       */
   /*****************************************************************/
   unsigned numberOfQs, selectedQueue;

   IString          qName;

   struct _PRQINFO3* pQueueInfo;

   struct _DRIVDATA* pDriverData;
   unsigned driverDataLen;

};

/********************************************************************/
/*  IPrinter class                                                  */
/********************************************************************/
class IPrinter : public IPrinterQueueHandler
{
public:       /*----------------------------------------------------*/
   /*****************************************************************/
   /*  Constructors                                                 */
   /*****************************************************************/
   IPrinter ( unsigned ulCopies = 1 );                // Default Queue
   IPrinter( IString QName, unsigned ulCopies = 1, IString newDriverData = IString() );
   IPrinter( IString QName, IString newDriverData = IString(), unsigned ulCopies = 1 );

   /*****************************************************************/
   /*  Virtual destructors, since we have virtual functions         */
   /*****************************************************************/
   virtual ~IPrinter();

   /*****************************************************************/
   /*  Enumerations                                                 */
   /*****************************************************************/
   enum Orientation
   {
      unknown, portrait, landscape
   };

   enum Margin
   {
      left, right, top, bottom
   };

   enum IPrinterExceptions
   {
      noPrinterInstalled = 1,
      noFontDefined,
      devOpenDCFailed,
      gpiCreatePSFailed,
      requestedQNotFound
   };

   /*****************************************************************/
   /*  Accessor member functions                                    */
   /*****************************************************************/
   IPoint  pageSize()        const { return pgSize;}
   IPoint  resolution()      const { return res;}
   IPoint  availableArea()   const { return printableArea;}
   IPoint  currentPosition() const { return currentPos;}

   IPresSpaceHandle HPS () const { return hps; }

   IHandle HDC () const { return hdc; }

   IString formName()      const { return formsName; }

   IString jobProperties()   const    ;
   IString queueDescription() const    ;
   IString queueDriverName()  const    ;

   int formOrientation() const { return orientation; } ;
   int formIndex( ) const { return iCurrentFormIndex; } ;

   unsigned leading() const { return lLeading;}
   unsigned margin( Margin whichMargin );

   /*****************************************************************/
   /*  Public member functions                                      */
   /*****************************************************************/
   virtual void print( IString line, IPoint& pos, unsigned newWidth = 0 );
   virtual void print( IString line, unsigned newWidth = 0 );

   void cancelJob(),
        endJob(),
        newPage(),
        setFont(), // reset to default font
        setFont( IFont* aFont, unsigned long newPointSize = 0 ),
        setFontSize( unsigned long newPointSize ),
        setJobProperties( ),
        startJob( IString jobName );
        setContext(IHandle h) {hdc = h; return 0;}    //edw



private:      /*----------------------------------------------------*/
   /*****************************************************************/
   /*  Private member functions                                     */
   /*****************************************************************/
   Boolean getForm(),
           getPrinterParms();

   void close(),
        fontChanged( IFont* aFont ),
        getPrinterResolution(),
        open( ),
        printLine( IString line, int pointSize, IPoint& pos, unsigned width );

   /*****************************************************************/
   /*  Private data members                                         */
   /*****************************************************************/

   Boolean printerOpen;

   //edw unsigned long  hdc;

   IHandle  hdc;

   IAnchorBlockHandle hab;

   IPresSpaceHandle  hps;

   IFont * font;

   IPoint  pgSize,
           res,
           currentPos,
           printableArea;

   IString formsName;

   int orientation,
       iCurrentFormIndex;

   unsigned margins[4],
            lLeading,
            ulCopies,
            width;

   unsigned long pointSize;
};

/********************************************************************/
/*  Printer dialog class                                            */
/********************************************************************/
class IPrinterDialog : public IFrameWindow
                     , public ICommandHandler
                     , public ISelectHandler
                     , public IPrinterQueueHandler
{
public:       /*----------------------------------------------------*/
   /*****************************************************************/
   /*  Constructor                                                  */
   /*****************************************************************/
   IPrinterDialog ( IWindow *owner, IString QName = IString() );

   /*****************************************************************/
   /*  Destructor                                                   */
   /*****************************************************************/
   virtual ~IPrinterDialog() {};

   /*****************************************************************/
   /*  Public member functions                                      */
   /*****************************************************************/
   Boolean pressedOK( ) const {return OkPressed;}

   unsigned long showModally();

protected:    /*----------------------------------------------------*/
   virtual Boolean command ( ICommandEvent& cmdevt );
   virtual Boolean enter   ( IControlEvent& evt );

private:      /*----------------------------------------------------*/
   /*****************************************************************/
   /*  Private member functions                                     */
   /*****************************************************************/
   void addPrinters();

   /*****************************************************************/
   /*  Private data members                                         */
   /*****************************************************************/
   Boolean           OkPressed;
   IMultiCellCanvas  clientCanvas;
   ISetCanvas      * buttonCanvas;

   IPushButton     * okButton,
                   * cancelButton;

   IListBox          listBox;
   IWindow         * ownerWindow;
};


#endif
