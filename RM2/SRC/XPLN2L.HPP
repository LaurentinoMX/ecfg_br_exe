#ifndef _AB_XPLN2L_HPP_
#define _AB_XPLN2L_HPP_
 /*--------------------------------------------------------------------------*\
 |    File Name  : XPLNLST.HPP                                                |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implemantation(s):                                              |
 |                                                                            |
 |     XPLN2List    - A sorted collection of XPLN2 objects                    |
 |                    (messages generated as a result of the resolution       |
 |                    process available for the user's viewing),              |
 |                    as described in XPLN2.HPP.                              |
 |                                                                            |
 |      XPLNListCursor - A cursor for the above                               |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |  fReadB() return codes                              |
        \*---------------------------------------------------*/
#define  XPLNL_ERR_NO_XPLNL_FILE   1
#define  XPLNL_ERR_NO_FILE         2
#define  XPLNL_ERR_READ_FILE       3
                                                 // align Collection classes
                                                 // structures on 4 byte
#pragma pack(4)                                  // boundries
#include <isrtset.h>
#pragma pack()

#include "xpln2.hpp"                              // XPLN Class
#include <iostream.h>
#include <stdio.h>

#pragma pack(4)
 class XPLN2List : public ISortedSet<XPLN2>
{
    IString strMLEDisplay;

//    Boolean   writeIString(FILE *,const IString &, unsigned *);
//    Boolean   loadIString(FILE *,IString &);

  public:
    class XPLN2ListCursor : public ISortedSet<XPLN2>::Cursor
       {
         public:
            XPLN2ListCursor(XPLN2List & xpl) : ISortedSet<XPLN2>::Cursor(xpl) {}
       };
//    IString & displayMLEText(XPLN2ListCursor &);
    IString & displayMLEText(XPLN2  &);
    unsigned   fWriteBLength();
    Boolean    fWriteB(char * szFilename, unsigned * pu = 0);
    Boolean    fWriteB(FILE * f,unsigned * pu = 0);
    unsigned   fReadB(char * szFilename);
    unsigned   fReadB(FILE * f);
    IString &  keyString(XPLN2 &,IString & );
    XPLN2List & allElementsFor(XPLN2List & List,
                               char * szMachine   = 0,
                               char * szModel     = 0,
                               char * szSerialNo  = 0,
                               char * szFeature   = 0,
                               char * szText      = 0);

    XPLN2List & allElementsFor( XPLN2List & List
                               ,const IString & Machine  = IString()
                               ,const IString & Model    = IString()
                               ,const IString & SerialNo = IString()
                               ,const IString & Feature  = IString()
                               ,const IString & Text     = IString());

    ostream&   out(ostream&);

    friend ostream& operator<< (ostream& os, XPLN2List & L)
                 {
                  os   << "          ----> Explanation List ( "
                       << L.numberOfElements()
                       << " elements) <---- " << endl;
                  Cursor crsr(L);
                  forCursor(crsr)
                   {
                    os << L.elementAt(crsr) << endl;
                    os << L.displayMLEText(L.elementAt(crsr)) << endl;
                   }
                  return os;
                 }


//friend ostream& operator<< (ostream& os, XPLN2List & x) { return x.out(os);}
};
#pragma pack()
#endif
