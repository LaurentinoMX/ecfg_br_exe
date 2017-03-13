 /*--------------------------------------------------------------------------*\
 |    Filename - CFR10.HPP                                                    |
 |       declarations for the following RM/2 class implementations:           |
 |                                                                            |
 |       CFR05  - To manipulate the CFReport's Record Type 05                 |
 |                                                                            |
 |       CFR05List - An array CFR05 record pointers.                          |
 \*--------------------------------------------------------------------------*/

#ifndef _AB_CFR05_HPP_
#define _AB_CFR05_HPP_

#include <iostream.h>

#include "euodef.h"
#include "euoutil.hpp"
#include "cfrv.hpp"

#pragma pack(4)
class CFR05 : public CFRecV
{
 public:

   enum PrintType
     {
       None = 0
      ,BeforeNextMachine
      ,BeforeTotals
      ,AfterTotals
      ,AfterHardware
      ,TopOfReport
      ,BottomOfReport
      ,TopOfSystem
      ,BottomOfSystem
      ,TopOfSubSystem
      ,BottomOfSubSystem
     };

  static PrintType    printType(char );
  static PrintType    printType(CFRecV *);
  static unsigned     referenceNotesNumber(CFRecV *);
};

class CFR05List : public CFRecVL
{
 public:
  Boolean  add(const char * Buffer);
  Boolean  addSignatureRec(unsigned RefNotesNo);
  unsigned fWriteCFR(FILE *,CFR05::PrintType,unsigned referenceNo = 0);
  unsigned fWriteCFR(FILE *f,unsigned referenceNo);
  friend ostream& operator<< (ostream& os, CFR05List & L)
               {
                os   << "          ----> Comment Record List ( "
                     << L.NumberElements()
                     << " elements) <---- " << endl;
                 for (int i = 0, N = L.NumberElements() ; i < N ; i++ )
                   os << *L.element(i) << endl;
                 return os;
               }
};

class CFR52List : public CFRecVL
{
 public:
  Boolean  add(int id,const char * Buffer);
  unsigned fWriteCFR(FILE *,int id);
  friend ostream& operator<< (ostream& os, CFR52List & L)
               {
                os   << "          ----> Exception Record List ( "
                     << L.NumberElements()
                     << " elements) <---- " << endl;
                 for (int i = 0, N = L.NumberElements() ; i < N ; i++ )
                   os << "ID : " << L.element(i)->id() << endl
                      << *L.element(i) << endl;
                 return os;
               }
};
#pragma pack()
#endif

