#ifndef _AB_XPLNCNTL_HPP_
#define _AB_XPLNCNTL_HPP_
 /*--------------------------------------------------------------------------*\
 |    File Name  : XPLNCNTL.HPP                                               |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |      ExplainCntrl  - Explain Control                                       |
 |                      Control consists of Groupbox surrounding              |
 |                      a  read-only MulitLineEntry Box displaying            |
 |                      a list of explanations of the 'XPLNList' class.       |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/
#define INCL_ICOMMANDHANDLER
#define INCL_IGROUPBOX
#define INCL_IMULTICELLCANVAS
#define INCL_IMULTILINEEDIT
#define INCL_IWINDOW
#include "uicl.hpp"

        /*---------------------------------------------------*\
        |               Class specific includes               |
        \*---------------------------------------------------*/
#include "xpln2l.hpp"                           // XPLNList class

#pragma pack(4)
class ExplainCntrl : public IMultiLineEdit
{
  XPLN2List       * pXPLNList;
 public:
  ExplainCntrl(unsigned long Id, IWindow * Parent, IWindow * Owner,
               XPLN2List * pList = 0);
 ~ExplainCntrl() {};
  void        FillMLE(XPLN2List *,Boolean fEmptyFirst = true);
};

class ExplainCntrlGB : public IMultiCellCanvas
{
  IGroupBox        GBExplain;
  ExplainCntrl     MLEExplain;
 public:
    ExplainCntrlGB(unsigned long Id, IWindow * Parent, IWindow * Owner,
                   XPLN2List * pList = 0);
    ~ExplainCntrlGB() {};
    IWindow   * ExplainWindow() {return &MLEExplain;}
    void        FillMLE(XPLN2List * pList,Boolean fEmptyFirst = true)
                   { MLEExplain.FillMLE(pList,fEmptyFirst);}
};

class ExplainCntrlMCC : public IMultiCellCanvas
{
  ExplainCntrl     MLEExplain;
 public:
    ExplainCntrlMCC(unsigned long Id, IWindow * Parent, IWindow * Owner,
                    XPLN2List * pList = 0);
    ~ExplainCntrlMCC() {};
    IWindow   * ExplainWindow() { return &MLEExplain; }
    void        FillMLE(XPLN2List * pList,Boolean fEmptyFirst = true)
                   { MLEExplain.FillMLE(pList,fEmptyFirst);}
};
#pragma pack()
#endif
