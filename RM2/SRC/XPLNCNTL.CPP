 /*--------------------------------------------------------------------------*\
 |    Filename - XPLNCNTL.CPP                                                 |
 |      contains source for the following EUOP class implementation:          |
 |      ExplainCntrl       - Explain Control                                  |
 |                           Control consists of Groupbox surrounding         |
 |                           a  read-only MulitLineEntry Box displaying       |
 |                           a list of explanations of the                    |
 |                           'XPLNList' class.                                |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/

#define INCL_IRESOURCEID
#include "uicl.hpp"

        /*---------------------------------------------------*\
        |               EUOP-specific includes                |
        \*---------------------------------------------------*/
#include "xpln2l.hpp"                           // XPLNList
#include "xplncntl.h"                            // Control Ids
#include "xplncntl.hpp"                          // this control's include

          /*-------------------------------------------------*\
          |                       Ctor                        |
          \*-------------------------------------------------*/

ExplainCntrl::ExplainCntrl(unsigned long ulId, IWindow * Parent,
                           IWindow * Owner, XPLN2List * pList)
             : IMultiLineEdit( ulId,Parent,Owner, IRectangle(),
                               IMultiLineEdit::defaultStyle()
                               | IMultiLineEdit::readOnly
                               | IMultiLineEdit::ignoreTab
                               | IControl::tabStop
                             )
{
//  setColor(IMultiLineEdit::background,IColor(IColor::cyan));
  FillMLE(pList);
}

          /*-----------------------------------------------------------------*\
          |   void FillMLE(XPLNList *pList,Boolean fEmptyFirst)               |
          |     Method fills the MLE with the given Explanation               |
          |     List, optionally removing the current content.                |
          |     Entries are separated by an empty line.                       |
          |     Returns : nothing                                             |
          \*-----------------------------------------------------------------*/
void ExplainCntrl :: FillMLE(XPLN2List *pList,Boolean fEmptyFirst)
{
  enableUpdate( 0); //jt  disableRefresh();
 if (fEmptyFirst && numberOfLines()) removeAll();
 if (pXPLNList = pList) {
   XPLN2List::Cursor crsr(*pList);
   forCursor(crsr)
     addLineAsLast((char *)pList->displayMLEText(pList->elementAt(crsr))).addLineAsLast("\n");
   }
  enableUpdate(); //jt enableRefresh();
}

///////////////////////////////////

ExplainCntrlGB::ExplainCntrlGB(unsigned long ulId, IWindow * Parent,
                               IWindow * Owner, XPLN2List * pList)
               : IMultiCellCanvas(ulId,Parent,Owner)
                ,MLEExplain( XPLNCNTL_MLE_EXPLAIN,this,this)
                ,GBExplain(XPLNCNTL_GB_EXPLAIN,this,this)
{
 GBExplain.setText(IResourceId(XPLNCNTL_STR_GB_EXPLAIN));
 addToCell(&GBExplain     ,2, 2, 3, 4);
 addToCell(&MLEExplain    ,3, 4      );
 setColumnWidth(3,0,true);
 setRowHeight(4,0,true);
 setRowHeight(6,IMultiCellCanvas::defaultCell().height());
 setColumnWidth(5,IMultiCellCanvas::defaultCell().width()/2);
 MLEExplain.FillMLE(pList);
}

ExplainCntrlMCC::ExplainCntrlMCC(unsigned long ulId, IWindow * Parent,
                                 IWindow * Owner, XPLN2List * pList)
               : IMultiCellCanvas(ulId,Parent,Owner)
                ,MLEExplain( XPLNCNTL_MLE_EXPLAIN,this,this)
{
 addToCell(&MLEExplain    ,1,1      );
 setColumnWidth(1,0,true);
 setRowHeight(1,0,true);
 MLEExplain.FillMLE(pList);
}

#if 0
void ExplainCntrl :: FillMLE(XPLN2List *pList,Boolean fEmptyFirst)
{
 if (fEmptyFirst && MLEExplain.numberOfLines()) MLEExplain.removeAll();
 if (pList) {
   XPLN2List::XPLN2ListCursor crsr(*pList);
   forCursor(crsr) {
     MLEExplain.addLineAsLast((char *)pList->displayMLEText(crsr));
     MLEExplain.addLineAsLast("\n");
     }
   }
}
#endif
