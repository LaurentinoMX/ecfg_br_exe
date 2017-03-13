 /*--------------------------------------------------------------------------*\
 |    Filename - CFR10.HPP                                                    |
 |       declarations for the following RM/2 class implementations:           |
 |                                                                            |
 |       CFR10  - To manipulate the CFReport's Record Type 10                 |
 |                                                                            |
 |       CFR10List - An array CFR10 record pointers.                          |
 \*--------------------------------------------------------------------------*/

#ifndef _AB_CFR10_HPP_
#define _AB_CFR10_HPP_

#include <iostream.h>

#include "euodef.h"
#include "euoutil.hpp"
#include "cfrv.hpp"

#pragma pack(4)
class CFR07 : public CFRecV
{
 public:
  static unsigned   referenceNotesNumber(const char * Buffer);
  static IString    description(const char * Buffer);
};

class CFR10 : public CFRecV
{
 public:
  static unsigned   referenceNotesNumber(CFRecV *);
  static IString    description(CFRecV *);
};

class CFR10List : public CFRecVL
{
 public:
  Boolean  add(int id, const char * Buffer);
  IString  description(int id);
  unsigned referenceNotesNumber(int id);
  unsigned fWriteCFR(FILE *,int id);
  friend ostream& operator<< (ostream& os, CFR10List & L)
               {
                os   << "          ----> Subsystem Record List ( "
                     << L.NumberElements()
                     << " elements) <---- " << endl;
                 for (int i = 0, N = L.NumberElements() ; i < N ; i++ )
                   os << *L.element(i) << endl;
                 return os;
               }
};
#pragma pack()
#endif

