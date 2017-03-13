 /*--------------------------------------------------------------------------*\
 |    File Name  : FEATL.HPP                                                  |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |  FeatListMem - An array of pointer to fixed length records, the storage    |
 |                of which is allocated and freed by the array class          |
 |                'PtrRecArrM' as in PARR.HPP                                 |
 |                                                                            |
 |     FeatList - A collection of Feat objects as in FEAT.HPP                 |
 \*--------------------------------------------------------------------------*/
#ifndef _AB_FEATL_HPP_
#define _AB_FEATL_HPP_

//#include <isrtset.h>
#include <iseq.h>                       // ISequence
#include "feat.hpp"                     // Feat
#include "parr.hpp"                     // PtrRecArrM
#include <iostream.h>                   // ostream

#pragma pack(4)

class FeatListMem : public PtrRecArrM
{
  FeatListMem( unsigned, unsigned, comp_fn_tp){} //hide
 public:
  FeatListMem() : PtrRecArrM(sizeof(Feat)){}
  ~FeatListMem(){}
};


// class FeatList : public ISortedSet<Feat>
 class FeatList : public ISequence<Feat>
{
  public:
    void       SetStorageNumbers();
    Boolean    writeToFile(char * szFilename, unsigned * pu = 0);
    Boolean    writeToFile(FILE * ,unsigned * pu = 0);
    Boolean    loadFile(char * szFilename);
    Boolean    loadFile(FILE *f , unsigned count, unsigned * pRead = 0);
    Boolean    loadFile(FILE *f , FeatListMem &, unsigned count,
                        unsigned * pRead = 0);
    int        greatestID();
    void       SortUp();
    Boolean    removeViaID(int id);

    ostream&   out(ostream&);

    friend ostream& operator<< (ostream& os, FeatList & L)
                 {
                  os   << "          ----> Features List ( "
                       << L.numberOfElements()
                       << " elements) <---- " << endl;
                   Cursor crsr(L);
                   forCursor(crsr)
                     os << L.elementAt(crsr) << endl;
                   return os;
                 }
};
#pragma pack()
#endif

