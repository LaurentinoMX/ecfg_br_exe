 /*--------------------------------------------------------------------------*\
 |    File Name  :  MACH.HPP                                                  |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |     BLFeat   - Backlog Feature object, merely containing the pointers      |
 |                to the Backlog Machine and MFeat objects.                   |
 |                Methods to manipulate the underlying CFReport machine       |
 |                and feature objects, do so directly. There is no copy.      |
 |                Intended for EUI for which also can use the ID data         |
 |                (easier when working with containers).                      |
 |                                                                            |
 |     BLFeatList - A collection of 'BLFeat' objects                          |
 \*--------------------------------------------------------------------------*/
#ifndef _AB_BLFEATL_HPP_
#define _AB_BLFEATL_HPP_

//#include <isrtset.h>
#include <iseq.h>                             // ISequence
#include <iostream.h>                         // ostream

#include "euodef.h"                           // CFReport structures
#include "mfeat.hpp"                          // MFeat
#include "mach.hpp"                           // Machine

#pragma pack(4)

class BLFeat
{
  friend class RMSystem;
  friend class BLFeatList;

    Machine * pM;
    MFeat   * pF;
    int       ID;
    BLFeat();
    void      setPointers(Machine * m,MFeat * f) { pM = m; pF = f;}

 public:

    BLFeat(const BLFeat& other) { *this = other;}
    BLFeat(Machine * m,MFeat * f,int id = 0) : pM(m), pF(f), ID(id) {}
    ~BLFeat(){}

    BLFeat &  operator=(BLFeat const& other);
    Boolean operator==(BLFeat const& other) const
                     { return !compare(other);}
    Boolean operator< (BLFeat const& other) const
                     { return (compare(other) < 0);}
    Boolean operator> (BLFeat const& other) const
                     { return (compare(other) > 0);}
    int            compare(BLFeat const& other) const;
    Boolean        sameMachine(BLFeat const & other) const
                       { return  (pM->compare(*(other.pM)) == 0);}

    unsigned long  cradDateYMD();
    unsigned long  csdDateYMD();
    Boolean        cradDate(IDate &date);
    Boolean        csdDate(IDate &date);
    Boolean        isHardware() { return (pM->tag() == Machine::Backlog_HW);}
    Boolean        isSoftware() { return (pM->tag() == Machine::Backlog_SW);}
    Boolean        description(IString & str);
    IString        machineDescription() { return pM->description();}
    unsigned       qty() { return (pF? pF->qty(MFeat::AASType): 0);}
    int            signedQty();
    int            id() { return ID;}
    void           setId(int i) { ID = i;}
    char           machineAction();
    void           machineQty_Action(IString & strQty,IString & strAction);
    void           qty_Action(IString & strQty,IString & strAct);
    char           action();
    const char *   feature() const { return pF? pF->feature():""; }
    const char *   machine() const { return pM->type(); }
    const char *   model() const { return pM->model(); }
    const char *   serialNo() const { return pM->serialNo(); }
    IString        machineQty();
    IString &      displaySerialNo(IString & str)
                           { return pM->displaySerialNo(str);}
    IString        displaySerialNoStr(){return pM->displaySerialNoStr();}
    ostream &      out(ostream& os);
    friend ostream& operator<< (ostream& os, BLFeat & f) { return f.out(os);}
    Boolean        isFeaturelessMachine(){ return (pF == 0);}
    Boolean        isFirstMRDBMachine(){ return pM? pM->isFirstMRDB(): false;}
};

// class BLFeatList : public ISortedSet<BLFeat>
class BLFeatList :  public ISequence<BLFeat>
{
  public:

    void           Sort(Boolean fUp = true);
    friend ostream& operator<< (ostream& os, BLFeatList & L)
                 {
                  os   << "          ----> Backlog Features List ( "
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

