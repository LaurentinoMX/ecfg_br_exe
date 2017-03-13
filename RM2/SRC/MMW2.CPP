 /*-----------------------------------------------------------------------------*\
 |    Filename - MMW.CPP                                                         |
 |      contains source for the following EUOP class implementations:            |
 |                                                                               |
 |      MMOWnd           - EUOP's Mismatch Window                                |
 |                         Window allows the user to view text and               |
 |                         select the appropriate radio button.                  |
 |                                                                               |
 |                                                                               |
 |         Data Reentrant- The same instance of the window can be given          |
 |                         different data without having to re-create            |
 |                         it. For multiple appearances of Lists, the window     |
 |                         has been designed to be 'shown' and 'hid' and         |
 |                         given data via its public methods.                    |
 |                         This was done to counter the annoyingly long          |
 |                         time it takes to build the window using IBMClass.     |
 |                                                                               |
 |         Initial State - Invisible                                             |
 |                         To be positioned, shown and/or focused by the caller. |
 |                                                                               |
 |         Interface     - The user's radio button preference upon               |
 |                         quiting this window is communicated with the          |
 |                         following response:                                   |
 |                           0  - User canceled                                  |
 |                           1  - User chose radiobutton 1                       |
 |                           2  - User chose radiobutton 2                       |
 |                                                                               |
 |                           If the window was instantiated with a               |
 |                           non-zero 'ClosingMsg' parameter:                    |
 |                             That message will be sent with the                |
 |                             aforementioned response as                        |
 |                             'parameter1'                                      |
 |                                                                               |
 |                           If the window was instantiated modally,             |
 |                           it will me dismissed with the reponse.              |
 |                                                                               |
 |                           If no 'ClosingMsg' was given and the                |
 |                           window was not shown modally, it will               |
 |                           close itself.                                       |
 \*-----------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |          PM and Standard Library Includes           |
        \*---------------------------------------------------*/
//jt extern "C"
//jt   {
   #define INCL_PM
   #define INCL_DOSNLS
   #define INCL_WIN
   #define INCL_WINSHELLDATA
   #include <os2.h>

   #include <stdlib.h>
//jt   }

        /*---------------------------------------------------*\
        |               EUOP specific includes                |
        \*---------------------------------------------------*/
#include "euophlp.h"                             // help IDs
#include "mmw2.hpp"                              // this class' include
#include "mmw2.h"

#include "euoutil.hpp"

#include "euopids.h"

        /*---------------------------------------------------*\
        |                        Ctor                         |
        \*---------------------------------------------------*/
MMWnd :: MMWnd( unsigned long ulId
               ,const IString& text
               ,const IString& textRB1
               ,const IString& textRB2
               ,Boolean bS390  //53915 MAMT 28/02/2001
               ,IHelpWindow *pHW
               ,unsigned long IconId
               ,unsigned HelpPanelId
               ,IWindow * Parent
               ,unsigned long ClosingMsg)
        : IFrameWindow( IFrameWindow::titleBar
                        | IFrameWindow::systemMenu
                        | IFrameWindow::sizingBorder
                       ,ulId )
            ,BaseWnd(this,Parent,ClosingMsg,pHW)
            ,MyXHndlr(this)
            ,strTxt(text)
            ,strTxtRB1(textRB1)
            ,strTxtRB2(textRB2)
            ,ulIcon(IconId)
            ,HPanelId(HelpPanelId? HelpPanelId : IDH_PNL_MMWND)
            ,fS390(bS390)  //53915 MAMT 28/02/2001
{
 setExceptionFunction(&MyXHndlr);
 CreateClient();
 if (!pHW)
    pPBHelp->disable();                         // no help available
 FillMLE();
 SetInitialSize();
 //pRB1->select();  53915 old logic 28/02/2001
}

MMWnd :: ~MMWnd()
{}

          /*-----------------------------------------------------------------*\
          |   IWindow & setFocus()                                            |
          |       Method to override the inherited method to                  |
          |       set the focus to the first control.                         |
          \*-----------------------------------------------------------------*/
IWindow & MMWnd :: setFocus()
{
 if (pRB1->isSelected())
     pRB1->setFocus();          
 else pRB2->setFocus();
 return *this;
}

          /*-----------------------------------------------------------------*\
          |   unsigned long  showModally()                                    |
          |       Method to override the inherited method to                  |
          |       set the focus to the first control.                         |
          |       Returns: the inherited method's return value.               |
          \*-----------------------------------------------------------------*/
unsigned long MMWnd :: showModally()
{
  if (!fS390)    //53915 MAMT 28/02/2001            
    {
     MMWnd::setFocus();
    }
 return IFrameWindow::showModally();
}

          /*-----------------------------------------------------------------*\
          |    void ArrangeControls()                                         |
          |       Method places and spaces the controls in our MultiCanvas    |
          |       Client.                                                     |
          |       Returns: nothing                                            |
          \*-----------------------------------------------------------------*/
void MMWnd :: ArrangeControls()
{
  if (!fS390)    //53915 MAMT 28/02/2001 Begin new logic            
    {
     pMCCControl->addToCell(pIcon   , 2, 2      )
                 .addToCell(pMLE    , 4, 2, 1, 2)
  //             .addToCell(pMLE    , 4, 2      )
                 .addToCell(pRB1    , 4, 5      )
                 .addToCell(pRB2    , 4, 7      );
    }
  else           
    {
     pMCCControl->addToCell(pIcon     , 2, 2      )
                 .addToCell(pMLE      , 4, 2, 1, 2)
                 .addToCell(pTXTPPAC1 , 4, 5      )
                 .addToCell(pTXTPPAC2 , 4, 6      )
                 .addToCell(pTXTPPAC3 , 4, 7      )
                 .addToCell(pTXTPPAC4 , 4, 8      );
    }            //53915 MAMT 28/02/2001 End new logic

 pMCCControl->setRowHeight(1,IMultiCellCanvas::defaultCell().height())
             .setRowHeight(3,IMultiCellCanvas::defaultCell().height(),true)
             .setRowHeight(4,IMultiCellCanvas::defaultCell().height())
             .setRowHeight(8,IMultiCellCanvas::defaultCell().height())
             .setColumnWidth(1,IMultiCellCanvas::defaultCell().width())
             .setColumnWidth(3,IMultiCellCanvas::defaultCell().width())
             .setColumnWidth(4,0,true)
             .setColumnWidth(5,IMultiCellCanvas::defaultCell().width());
}

          /*-----------------------------------------------------------------*\
          |    void CreateClient()                                            |
          |       Method creates  the client                                  |
          |       Returns: nothing                                            |
          \*-----------------------------------------------------------------*/
void MMWnd :: CreateClient()
{
 IString vPPAC1 = "RECONCILIATION WILL BE TERMINATED!"; //53915 MAMT 28/02/2001 
 IString vPPAC2 = "PLEASE CONTACT THE POUGHKEEPSIE";    //53915 MAMT 28/02/2001 
 IString vPPAC3 = "PRODUCT ASSISTANCE CENTER (PPAC)";   //53915 MAMT 28/02/2001 
 IString vPPAC4 = "AT (845) 435-1100 OR T/L 295-1100";  //53915 MAMT 28/02/2001 
 pVPClient = new IViewPort(MMWND_VPORT,this,this);
 pVPClient->setAutoDeleteObject();
 pMCCControl = new IMultiCellCanvas(MMWND_MCC, pVPClient, pVPClient);
 pMCCControl->setAutoDeleteObject();
 setClient( pVPClient );
 pIcon = new IIconControl(MMWND_ICON,pMCCControl,pMCCControl);
 if (ulIcon)
   pIcon->setIcon(ulIcon);
 else pIcon->setIcon(ISystemPointerHandle::error);
 pIcon->setAutoDeleteObject();
 pMLE = new IMultiLineEdit( MMWND_MLE,pMCCControl,pMCCControl, IRectangle(),
                 IMultiLineEdit::defaultStyle()
                 | IMultiLineEdit::readOnly
                 | IMultiLineEdit::ignoreTab
                 | IControl::tabStop);
 pMLE->setAutoDeleteObject();
// pMLE->setColor(IMultiLineEdit::background,IColor(IColor::cyan));

  if (!fS390)    //53915 MAMT 28/02/2001 Begin new logic                       
    {
     pRB1 = new IRadioButton(MMWND_RB_1, pMCCControl,pMCCControl);
     pRB1->setAutoDeleteObject();
     pRB1->enableGroup().enableTabStop();
     pRB1->setText((char *)strTxtRB1);

     pRB2 = new IRadioButton(MMWND_RB_2, pMCCControl,pMCCControl);
     pRB2->setAutoDeleteObject();
     pRB2->setText((char *)strTxtRB2);
    }
  else
    {
     pTXTPPAC1 = new IStaticText(MMWND_TEXT_PPAC1 ,pMCCControl,pMCCControl);
     pTXTPPAC1->setAutoDeleteObject();
     pTXTPPAC1->setText((char *)vPPAC1);
     
     pTXTPPAC2 = new IStaticText(MMWND_TEXT_PPAC2 ,pMCCControl,pMCCControl);
     pTXTPPAC2->setAutoDeleteObject();
     pTXTPPAC2->setText((char *)vPPAC2);

     pTXTPPAC3 = new IStaticText(MMWND_TEXT_PPAC3 ,pMCCControl,pMCCControl);
     pTXTPPAC3->setAutoDeleteObject();
     pTXTPPAC3->setText((char *)vPPAC3);

     pTXTPPAC4 = new IStaticText(MMWND_TEXT_PPAC4 ,pMCCControl,pMCCControl);
     pTXTPPAC4->setAutoDeleteObject();
     pTXTPPAC4->setText((char *)vPPAC4);
    }            //53915 MAMT 28/02/2001 End new logic 

 ArrangeControls();
 CreatePBs();
}

void MMWnd :: FillMLE()
{
 if (pMLE->numberOfLines())
    pMLE->removeAll();
#if 0
 int brk = 0, strt = 1;
 for (;brk = strTxt.indexOf("\\n",strt); strt += (brk +2) )
   pMLE->addLineAsLast((char *)strTxt.subString(strt,brk-strt));
 pMLE->addLineAsLast((char *)strTxt.subString(strt));
#endif
  char * pc, * pc1;
  for (pc = (char *)strTxt ; pc1 = strstr(pc,"\\n");pc = pc1 + 2)
    { *pc1 = '\0';
      pMLE->addLineAsLast(pc);
    }
  pMLE->addLineAsLast(pc);
}

void MMWnd :: SetRB1Text(const IString & str)
{ pRB1->setText(strTxtRB1=str); }

void MMWnd :: SetRB2Text(const IString & str)
{ pRB2->setText(strTxtRB2=str); }

void MMWnd :: SetText(const IString & str)
{
 strTxt = str;
 FillMLE();
}

void MMWnd :: SetIcon(unsigned long ul)
{
 if (ulIcon = ul)
   { pIcon->setIcon((IResourceId)ulIcon);
     pIcon->show();
   }
 else pIcon->hide();
}

          /*-----------------------------------------------------------------*\
          |    void CreatePBs()                                               |
          |       Method builds the Pushbutton extension:                     |
          |         1. Create the pushbuttons and add them to a SetCanvas     |
          |         2. Calculate and set the canvas' minimum size             |
          |         3. Add the extension and make it visible                  |
          |       Return: nothing                                             |
          \*-----------------------------------------------------------------*/
void MMWnd :: CreatePBs()
{
  unsigned short usPBCount = 2;
  
  pSCPBs = new ISetCanvas(MMWND_SCV_PBS,this, this);
  pSCPBs->setAutoDeleteObject();
  pSCPBs->setDeckCount(1).setDeckOrientation(ISetCanvas::horizontal)
         .setPackType(ISetCanvas::even).setAlignment(ISetCanvas::centerCenter);

  if (fS390)    //53915 MAMT 28/02/2001                       
    {
     pPBCancel = new IPushButton(DID_CANCEL,pSCPBs,pSCPBs, IRectangle(),
                             IPushButton::classDefaultStyle |
                             IPushButton::defaultButton);
     pPBCancel->setAutoDeleteObject();
     pPBCancel->setText(IResourceId(ID_STR_CANCEL)).enableGroup().enableTabStop();
    }
  else 
    {
     pPBOK = new IPushButton(DID_OK,pSCPBs,pSCPBs, IRectangle(),
                             IPushButton::classDefaultStyle |
                             IPushButton::defaultButton);
     pPBOK->setAutoDeleteObject();
     pPBOK->setText(IResourceId(ID_STR_ACC_OK)).enableGroup().enableTabStop();
     pPBCancel = new IPushButton(DID_CANCEL,pSCPBs,pSCPBs);
     pPBCancel->setAutoDeleteObject();
     pPBCancel->setText(IResourceId(ID_STR_CANCEL));
    }
    
  pPBHelp = new IPushButton(MMWND_PB_HELP,pSCPBs,pSCPBs);
  pPBHelp->setAutoDeleteObject();
  pPBHelp->setText(IResourceId(ID_STR_HELP));

  pSCPBs->setMinimumSize( ISize(pPBHelp->minimumSize().width() * usPBCount +
                                 + pSCPBs->pad().width() * (usPBCount-1)
                                 + pSCPBs->margin().width() * 2
                                ,pPBHelp->minimumSize().height()
                                 + pSCPBs->margin().width() * 3
                               )
                        );
  addExtension(pSCPBs,belowClient,(unsigned long)pSCPBs->minimumSize().height());
  pSCPBs->show();
}

          /*-----------------------------------------------------------------*\
          |    Boolean command(ICommandEvent & evt)                           |
          |       Method, called by ICommandHandler, processes                |
          |       the following cases:                                        |
          |                                                                   |
          |               (OK pushbutton)                                     |
          |             Report back which radiobutton is selected (1 or 2)    |
          |                                                                   |
          |                (Help PushButton)                                  |
          |             Tell the Help Window to show our Help Panel.          |
          |                                                                   |
          |       Returns: false/true : message (not)/handled                 |
          \*-----------------------------------------------------------------*/
Boolean MMWnd::command( ICommandEvent& evt )
{
 switch (evt.commandId()) {
   case DID_OK:
     NotifyMyClosing(pRB1->isSelected()? 1 : 2);
     return true;
   case DID_CANCEL:
     NotifyMyClosing(0);
     return true;
   case MMWND_PB_HELP:
     helpWindow()->show(IResourceId(HPanelId));
     return true;
   }
 return false;
}

          /*-----------------------------------------------------------------*\
          |   void SetInitialSize()                                           |
          |     Method sizes the window to its minimum size which is          |
          |     the minimum size of the MultiCellCanvas containing the        |
          |     controls.                                                     |
          |     Returns : nothing                                             |
          \*-----------------------------------------------------------------*/
void MMWnd :: SetInitialSize()
{
 pMLE->setMinimumSize(ISize(IWindow::desktopWindow()->size().width() / 3,
                            IWindow::desktopWindow()->size().height() / 6));
 pMCCControl->sizeTo(pMCCControl->minimumSize());
 setMinimumSize(minimumSize());
 sizeTo(minimumSize());
}


          /*-----------------------------------------------------------------*\
          |    Boolean virtualKeyPress(IKeyboardEvent & evt)                  |
          |       Method handles the following virtual keys:                  |
          |                                                                   |
          |             Enter       -> Push the button with                   |
          |                            the focus ( since it's their           |
          |                            messages which are being               |
          |                            intercepted)                           |
          |                                                                   |
          |             tab         -> Special logic required to properly     |
          |                            tab out of the pushbutton group to     |
          |                            the first control in Client.           |
          |       Returns: false/true : message (not)/handled                 |
          \*-----------------------------------------------------------------*/
Boolean MMWnd :: PressedEnter(IKeyboardEvent & evt)
{
 if ( evt.window() == pPBOK     ||
      evt.window() == pPBCancel ||
      evt.window() == pPBHelp ) {
      evt.handle().sendEvent(BM_CLICK);
   return true;
   }
 return false;
}

Boolean MMWnd :: PressedTab(IKeyboardEvent & evt)
{
 if (evt.handle() == pPBOK->handle()   ||
     evt.handle() == pPBHelp->handle() ||
     evt.handle() == pPBCancel->handle()) {
     if (!fS390)    //53915 MAMT 28/02/2001            
      {
       MMWnd::setFocus();
      } 
  return true;
  }
 return false;
}

          /*-----------------------------------------------------------------*\
          |    ISize calcMinimumSize() const                                  |
          |       Method, called by 'IFrameWindow', returns the minimum       |
          |       size for the window.                                        |
          |       Method is overridden since IFrameWindow's method            |
          |       does not poperly take into account the Info Area.           |
          |       Returns: ISize : minimum size.                              |
          \*-----------------------------------------------------------------*/

ISize MMWnd :: calcMinimumSize() const
{
/*jt  long lBorderH = WinQuerySysValue(HWND_DESKTOP,SV_CYSIZEBORDER);
  return pMCCControl->minimumSize() +
         ISize(WinQuerySysValue(HWND_DESKTOP,SV_CXSIZEBORDER) * 2,
               (lBorderH * 3)
               + WinQuerySysValue(HWND_DESKTOP,SV_CYTITLEBAR)
               + pSCPBs->minimumSize().height());   jt*/
 if (fS390)    //53915 MAMT 28/02/2001            
   {
    return pMCCControl->minimumSize()+
           ISize( 40, 40+ pSCPBs->minimumSize().height());
   }
 else  
   {
    return pMCCControl->minimumSize()+
           ISize( 10, 10+ pSCPBs->minimumSize().height());
   }
}


 /*--------------------------------------------------------------------------*\
 |                                Test Driver                                 |
 \*--------------------------------------------------------------------------*/
#ifdef DRIVER
void main()
{
  MMWnd mainWindow(WND_MM,
                   8889,
                   IString("This is a very long string of text for a long winded message to be shown in the multi line edit box!"),
                   IString("Button 1 text"),
                   IString("Button 2 text"));
  mainWindow.Center();
  mainWindow.show();
  IApplication::current().run();
                                                 /* end main                 */
#endif

  /*-------------------------------------------------------------------------*\
  |                        Unused - non compiled code                         |
  \*-------------------------------------------------------------------------*/
