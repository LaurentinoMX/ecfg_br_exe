//jt extern "C"
//jt   {
   #define INCL_DOSPROCESS          // dossleep
   #define INCL_PM
   #define INCL_DOSNLS
   #define INCL_WIN
   #define INCL_WINSHELLDATA
   #include <os2.h>

   #include <stdlib.h>
   #include <stdio.h>
//jt   }

        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/
#define INCL_IAPPLICATION
#include "uicl.hpp"
        /*---------------------------------------------------*\
        |               EUOP specific includes                |
        \*---------------------------------------------------*/
#include "euoutil.hpp"
#include "prfw4.h"                               // resource IDs
#include "prfw4.hpp"                             // this class' include
#include "euopids.h"

        /*---------------------------------------------------*\
        |       Messages                                      |
        \*---------------------------------------------------*/
#define PRFLWND_THRD_WRITE_PRFL   WM_USER + 1

void parmStoreProfile(void *pV)
{
 *(((OLFnParms *)pV)->pulRC) =
    (unsigned long)((EUOPProfile *)(((OLFnParms *)pV)->pOLArg))->WriteProfile();
}

unsigned long ulStoreProfile(void *pV)             //used
{
//jt  DosSleep(500);
 Sleep(500); //edw
 return (unsigned long)((EUOPProfile *)pV)->WriteProfile();
}

void StoreProfile(void *pV)
{ ((EUOPProfile *)pV)->WriteProfile(); }

void _System parmSysLinkStoreProfile(unsigned long ul)
{
 *(((SysFnParms *)ul)->pulRC) =
    (unsigned long)((EUOPProfile *)(((SysFnParms *)ul)->ulArg))->WriteProfile();
}
unsigned long _System ulSysLinkStoreProfile(unsigned long ul)
{ return (unsigned long)((EUOPProfile *)ul)->WriteProfile(); }

void  _System SysLinkStoreProfile(unsigned long ul)
{ ((EUOPProfile *)ul)->WriteProfile(); }

EUOPProfileWndw :: EUOPProfileWndw(unsigned long ulId,
                                   EUOPProfile *pProf,
                                   IWindow *     Parent,
                                   IHelpWindow  *pHW,
                                   unsigned long ClosingMsg)
            : IFrameWindow( IFrameWindow::titleBar
                            | IFrameWindow::systemMenu
//                          | IFrameWindow::dialogBorder
                            | IFrameWindow::sizingBorder
                            | IFrameWindow::dialogBackground
                           ,ulId
                          )
            ,BaseWnd(this,Parent,ClosingMsg,pHW)
            ,Reslib(IApplication::current().userResourceLibrary())
            ,MyXHndlr(this,this)
            ,NotifyThread(handle())
            ,pRBDsplyAll(0)
{
 SetDisabledPointer(ISystemPointerHandle(ISystemPointerHandle::wait));
 setExceptionFunction(&MyXHndlr);
 if (fMyProfile = ((pProfile = pProf) == NULL )) {
//     pProfile = new EUOPProfile(true);     //syncwrite
   pProfile = new EUOPProfile();
   pProfile->ReadProfile();
  }
 pMCCControl = new IMultiCellCanvas(PRFLWNDW_MCCCONTROL, this, this);
 pMCCControl->setAutoDeleteObject();
 setClient( pMCCControl );
 CreateControls();
 CreatePBs();
 if (!pHW)
   pPBHelp->disable();                         // no help available
 ArrangeControls();
 SetControls();
 SetInitialSize();
}

EUOPProfileWndw :: ~EUOPProfileWndw()
{
 if (fMyProfile && pProfile)  delete pProfile;
}

IWindow & EUOPProfileWndw :: setFocus()
{
 if (pRBDsplyAll->isSelected())
   pRBDsplyAll->setFocus();
 else pRBDsplyDscrp->setFocus();
 return *this;
}

unsigned long EUOPProfileWndw :: showModally()
{
 EUOPProfileWndw::setFocus();
 return IFrameWindow::showModally();
}

       /*--------------------------------------------------------------------*\
       |   Re_SetControls(EUOPProfile * prof)                                 |
       |       Public method to set the controls to reflect                   |
       |       the current or given EUOPProfile values.                       |
       |       Returns: flase/true - (un)successful.                          |
       \*--------------------------------------------------------------------*/
Boolean EUOPProfileWndw :: Re_SetControls(EUOPProfile * prof)
{
 if (prof) {
   if (fMyProfile && pProfile) {
     delete pProfile;
     fMyProfile = false ;
     }
   pProfile = prof;
   }
 if (pProfile) {
   SetControls();
   return true;
   }
 return false;
}

          /*-----------------------------------------------------------------*\
          |   void SetControls(Boolean fDefault)                              |
          |       Method to set the controls to reflect                       |
          |       the current or defaullt EUOPProfile values.                 |
          |       Returns: nothing.                                           |
          \*-----------------------------------------------------------------*/
void EUOPProfileWndw :: SetControls(Boolean fDefault)
{
 if (pProfile->GetBooleanKey(EUOPProfile::DsplyAll,fDefault))
    pRBDsplyAll->select();
 else pRBDsplyDscrp->select();

 if (pProfile->GetBooleanKey(EUOPProfile::PrcssGenRPO,fDefault))
     pCBGenRPO->select();
 else pCBGenRPO->deselect();

//edw pEFWorkDir->setText(fDefault? IString(".") :
//edw                               pProfile->GetStringKey(EUOPProfile::WorkDir));
//jt pEFWorkDir->setText(pProfile->GetStringKey(EUOPProfile::WorkDir,fDefault));
}

          /*-----------------------------------------------------------------*\
          |    void ArrangeControls()                                         |
          |       Method places and spaces the controls in our MultiCanvas    |
          |       Client.                                                     |
          |       Returns: nothing                                            |
          \*-----------------------------------------------------------------*/
void EUOPProfileWndw :: ArrangeControls()
{
 pMCCControl->addToCell(pGBDsply       , 2, 2, 3, 5)
             .addToCell(pRBDsplyAll    , 3, 4      )
             .addToCell(pRBDsplyDscrp  , 3, 5      )
             .addToCell(pCBGenRPO      , 2, 8, 3   );
//             .addToCell(pTXTWorkDir    , 2,10, 3   )
//             .addToCell(pEFWorkDir     , 3,11      );

 pMCCControl->setRowHeight(3,2*IMultiCellCanvas::defaultCell().height())
             .setRowHeight(12,IMultiCellCanvas::defaultCell().height())
             .setColumnWidth(6,IMultiCellCanvas::defaultCell().width());
}

          /*-----------------------------------------------------------------*\
          |    void CreateControls()                                          |
          |       Method creates  the controls in our                         |
          |       MulticellCanvas -> ViewPort Client.                         |
          |       Returns: nothing                                            |
          \*-----------------------------------------------------------------*/
void EUOPProfileWndw :: CreateControls()
{
  pGBDsply = new IGroupBox(PRFLWND_GB_DSPLY ,pMCCControl,pMCCControl);
  pGBDsply->setText(PRFLWND_STR_GB_DSPLY).setAutoDeleteObject();

  pRBDsplyAll = new IRadioButton(PRFLWND_RB_DSPLYALL, pMCCControl,pMCCControl);
  pRBDsplyAll->setText(IResourceId(PRFLWND_STR_RB_DSPLYALL))
              .enableGroup().enableTabStop().setAutoDeleteObject();

  pRBDsplyDscrp = new IRadioButton(PRFLWND_RB_DSPLYDSCRP,
                                   pMCCControl,pMCCControl);
  pRBDsplyDscrp->setAutoDeleteObject();
  pRBDsplyDscrp->setText(IResourceId(PRFLWND_STR_RB_DSPLYDSCRP));

  pCBGenRPO = new ICheckBox(PRFLWND_CB_GENRPO,
                            pMCCControl,pMCCControl);
  pCBGenRPO->enableGroup().enableTabStop().setAutoDeleteObject();
  pCBGenRPO->setText(IResourceId(PRFLWND_STR_CB_GENRPO));

  pTXTWorkDir = new IStaticText(PRFLWND_GB_WORKDIR ,pMCCControl,pMCCControl);
  pTXTWorkDir->setAutoDeleteObject();
  pTXTWorkDir->setText(IResourceId(PRFLWND_STR_GB_WORKDIR));
//jt  pEFWorkDir = new IEntryField(PRLWND_EF_WORKDIR, pMCCControl,pMCCControl);
//jt  pEFWorkDir->enableGroup().enableTabStop().setAutoDeleteObject();
  // entryfield doesn't calculate its minimum height properly on certain VGA
  // drivers, but the static text control does
//jt  pEFWorkDir->setMinimumSize(ISize(pEFWorkDir->minimumSize().width(),
//jt                                   pTXTWorkDir->minimumSize().height()));
//                                   pEFWorkDir->minimumSize().height()*1.2));
}

          /*-----------------------------------------------------------------*\
          |    void CreatePBs()                                               |
          |       Method builds the Pushbutton extension:                     |
          |         1. Create the pushbuttons and add them to a SetCanvas     |
          |         2. Calculate and set the canvas' minimum size             |
          |         3. Add the extension and make it visible                  |
          |       Return: nothing                                             |
          \*-----------------------------------------------------------------*/
void EUOPProfileWndw :: CreatePBs()
{
  unsigned short usPBCount = 5;
  pSCPBs = new ISetCanvas(PRFLWND_SCV_PBS,this, this);
  pSCPBs->setAutoDeleteObject();
  pSCPBs->setDeckCount(1).setDeckOrientation(ISetCanvas::horizontal)
         .setPackType(ISetCanvas::even).setAlignment(ISetCanvas::centerCenter);

  pPBSave = new IPushButton(DID_OK,pSCPBs,pSCPBs);
  pPBSave->enableDefault().setText(IResourceId(ID_STR_ACC_SAVE))
          .enableTabStop().enableGroup().setAutoDeleteObject();

  pPBDefault = new IPushButton(PRFLWND_PB_DEFAULT,pSCPBs,pSCPBs);
  pPBDefault->setText(IResourceId(ID_STR_ACC_DEFAULT)).setAutoDeleteObject();

  pPBReset = new IPushButton(PRFLWND_PB_RESET,pSCPBs,pSCPBs);
  pPBReset->setText(IResourceId(ID_STR_ACC_RESET)).setAutoDeleteObject();

  pPBCancel = new IPushButton(DID_CANCEL,pSCPBs,pSCPBs);
  pPBCancel->setText(IResourceId(ID_STR_CANCEL)).setAutoDeleteObject();

  pPBHelp = new IPushButton(PRFLWND_PB_HELP,pSCPBs,pSCPBs);
//  , IRectangle(),
//                            IPushButton::classDefaultStyle | IPushButton::help);
//                            IPushButton::classDefaultStyle | IPushButton::help);
  pPBHelp->setAutoDeleteObject();
  pPBHelp->setText(IResourceId(ID_STR_HELP));

  pSCPBs->setMinimumSize( ISize(pPBCancel->minimumSize().width() * usPBCount +
                                 + pSCPBs->pad().width() * (usPBCount-1)
                                 + pSCPBs->margin().width() * 2
                                ,pPBCancel->minimumSize().height()
                                 + pSCPBs->margin().width() * 3
                               )
                        );
  addExtension(pSCPBs,belowClient,(unsigned long)pSCPBs->minimumSize().height());//,none);
  pSCPBs->show();
}

Boolean EUOPProfileWndw::command( ICommandEvent& evt )
{
 switch ( evt.commandId() ) {
   case DID_OK:         //save button was perssed
     if (ReadControls() && !pProfile->SyncWrite()) {
//          pProfile->WriteProfile();
//jt       Able(false);
       NotifyThread.start((ulOptlinkFnPtr)ulStoreProfile,
                          pProfile,PRFLWND_THRD_WRITE_PRFL);
       }
     return true;
   case PRFLWND_PB_RESET:
   case PRFLWND_PB_DEFAULT:
     SetControls(evt.commandId() == PRFLWND_PB_DEFAULT);
     return true;
   case DID_CANCEL:
           /*------------------------------------------------------*\
           | ReadControls() will allow for a separate profile in    |
           | memory, possible different from that on file.          |
           | Currently, this is not desired: they want the window   |
           | to show the INI file values everytime it is shown, for |
           | which purpose we re-set the controls before the        |
           | windows disappears in preparation for the next time    |
           | it's shown.                                            |
           \*------------------------------------------------------*/
//      ReadControls();
     SetControls();
     NotifyMyClosing(DID_CANCEL);
     return true;
   case PRFLWND_PB_HELP:
     helpWindow()->show(IHelpWindow::general);
     return true;
   }
 return false;
}

Boolean EUOPProfileWndw :: userMessage(IEvent& evt)
{
 switch(evt.eventId()) {
   case PRFLWND_THRD_WRITE_PRFL:
     Able(true);
     NotifyMyClosing(evt.parameter1().asUnsignedLong());
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
void EUOPProfileWndw :: SetInitialSize()
{
 pMCCControl->sizeTo(pMCCControl->minimumSize());
 setMinimumSize(ISize(ALMAX(pMCCControl->minimumSize().width(),
                            pSCPBs->minimumSize().width()),
                            minimumSize().height()));
// sizeTo(minimumSize());
 sizeTo(ISize(ALMIN(IWindow::desktopWindow()->size().width() * 0.9,
                    minimumSize().width()),
              ALMIN(IWindow::desktopWindow()->size().height() * 0.9,
                    minimumSize().height())
             ));
}

          /*-----------------------------------------------------------------*\
          |     void ReadControls()                                           |
          |     Set the controls' values in the profile.                      |
          |     Returns : nothing                                             |
          \*-----------------------------------------------------------------*/
Boolean EUOPProfileWndw::ReadControls()
{
 IString dir;
//jt if (editWorkDirectory(dir)) {
//jt   pProfile->SetStringKey(EUOPProfile::WorkDir,dir);
   pProfile->SetBooleanKey(EUOPProfile::DsplyAll,
                           pRBDsplyAll->isSelected());
   pProfile->SetBooleanKey(EUOPProfile::DsplyDscrp,
                           pRBDsplyDscrp->isSelected());
   pProfile->SetBooleanKey(EUOPProfile::PrcssGenRPO,
                           pCBGenRPO->isSelected());
   return true;
//jt   }
//jt return false;
}
/*edw
Boolean EUOPProfileWndw :: editWorkDirectory(IString & str)
{
 str = IString::stripBlanks(pEFWorkDir->text());
 IString strErr;
 if (str.length()) {
   if (str[str.length()] == '\\') {    // not zero based !!!!!!
     str[str.length()] = '\0';
     pEFWorkDir->setText(str);
     }
   IString dir = IString((char *)str);
   dir += IString("\\$$$.$$$");
   if (writableFile((char *)dir))
     return true;
   strErr = Reslib.loadString(PRFLWND_STR_ERR_BAD_WORKDIR);
   }
 else strErr = Reslib.loadString(PRFLWND_STR_ERR_NO_WORKDIR);
 pEFWorkDir->selectRange();
 pEFWorkDir->setFocus();
 MyXHndlr.HandleMsgResponse((char * )strErr,
                            (char *)IApplication::current().
                                    userResourceLibrary().
                                    loadString(PRFLWND_STR_ERR_TITLE));
 return false;
}    */

Boolean EUOPProfileWndw :: PressedEnter(IKeyboardEvent & evt)
{
 if ( evt.window() == pPBSave    ||
      evt.window() == pPBDefault ||
      evt.window() == pPBReset   ||
      evt.window() == pPBCancel  ||
      evt.window() == pPBHelp ) {
   evt.handle().sendEvent(BM_CLICK);
   return true;
   }
 return false;
}

Boolean EUOPProfileWndw :: PressedTab(IKeyboardEvent & evt)
{
 if (evt.handle() == pPBSave->handle()    ||
     evt.handle() == pPBDefault->handle() ||
     evt.handle() == pPBReset->handle()   ||
     evt.handle() == pPBCancel->handle()  ||
     evt.handle() == pPBHelp->handle()) {
  EUOPProfileWndw::setFocus();
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
ISize EUOPProfileWndw :: calcMinimumSize() const
{ /*jt
 long lBorderH = WinQuerySysValue(HWND_DESKTOP,SV_CYSIZEBORDER);
  return pMCCControl->minimumSize() +
         ISize(WinQuerySysValue(HWND_DESKTOP,SV_CXSIZEBORDER) * 2,
                   (lBorderH * 3)
                  + WinQuerySysValue(HWND_DESKTOP,SV_CYTITLEBAR)
                  + pSCPBs->minimumSize().height());  jt*/
}

 /*--------------------------------------------------------------------------*\
 |                                Test Driver                                 |
 \*--------------------------------------------------------------------------*/
#ifdef DRIVER
void main()
{
// IApplication::current().setArgs(argc, argv);
 EUOPProfile * pProfile = new EUOPProfile();
 if (!(pProfile->SetResources(IApplication::current())))
   exit(3) ;
  EUOPProfileWndw mainWindow(WND_PRF);
  mainWindow.Center();
  mainWindow.show();
  IApplication::current().run();
  delete pProfile;
}                                                 /* end main                 */
#endif

  /*-------------------------------------------------------------------------*\
  |                        Unused - non compiled code                         |
  \*-------------------------------------------------------------------------*/
