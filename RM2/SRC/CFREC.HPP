 /*--------------------------------------------------------------------------*\
 |    Filename - CFREC.HPP                                                    |
 |      declarations for the following RM/2 class implementations:            |
 |                                                                            |
 |       CFRec     - Fixed Length (MAX_CFREC_SIZE defined in                  |
 |                   EUODEF.H) CFReport record.                               |
 |       CFRecList - Array of CFRec object pointers.                          |
 |       Placement - To access the CFReport's Placement Record                |
 |                   ( Type 55) as defined in 'euopdef.h'                     |
 |   PlacementList - Array of Placement object pointers.                      |
 \*--------------------------------------------------------------------------*/

#ifndef _AB_CFREC_HPP_
#define _AB_CFREC_HPP_

#include <fstream.h>

#include "euodef.h"
#include "euoutil.hpp"
#include "parr.hpp"

#pragma pack(4)
class CFRec
{
 char     Data[MAX_CFREC_SIZE + 1];
 unsigned nmbr;

public:
  CFRec();
  CFRec(const CFRec&);
  CFRec(char * data,unsigned number = 0);

  CFRec &    operator=(CFRec const& other);

  const char *         data()const { return Data; }
  Boolean        readLine(FILE *);
  Boolean        Write(FILE *f);
  Boolean        readLine(ifstream &);
  unsigned       dataLength() { return strlen(Data);}
  unsigned       type();
  void           setType(unsigned nmbr);
  unsigned       number(){ return nmbr;}
  static unsigned maxDataSize() { return MAX_CFREC_SIZE;}
  void           setData(const char *);
  void           setNumber(unsigned u) { nmbr = u; }
  char *         copyData(char * Target);

  friend ostream& operator<< (ostream& os, CFRec const& rec)
                       { return os << rec.Data; }
};

class Placement : public CFRec
{
  char       InstId[CFREC_INSTID_SIZE + 1];
 public:
  Placement(const char * instid = 0)
           { mystrncpy(InstId,instid,sizeof(InstId)-1); }
  Placement(const Placement& other) { *this = other;}
  Placement(CFR_55 & r55) : CFRec((char *)&r55)
//         { mystrstrip(mystrncpy(InstId,r55.InstId,sizeof(InstId)-1)); }
           { mystrncpy(InstId,r55.InstId,sizeof(InstId)-1); }
  ~Placement(){}
  Placement &  operator=(const Placement & other);
  Boolean operator==(Placement const& other) const
                     { return !compare(other);}
  Boolean operator< (Placement const& other) const
                     { return (compare(other) < 0);}
  Boolean operator> (Placement const& other) const
                     { return (compare(other) > 0);}
  int         compare(Placement const& other) const
                     { return strcmp(InstId,other.InstId);}
  const char *   instId() const { return (const char *)InstId;}

  friend ostream& operator<< (ostream& os, Placement const& P)
                       { os << "Id : '" << P.instId()
                            << "' Data : " << P.data();
                         return os;
                       }
};

class CFRecList : public PtrRecArr
{
  friend class ostream;
  friend class PlacementList;
  CFRecList(unsigned, unsigned, unsigned, comp_fn_tp);
 public:
  CFRecList() : PtrRecArr(sizeof(CFRec)) {}
  virtual    ~CFRecList(){}
  virtual void  Release();
  CFRec *       FindRecord(unsigned uRecNumber);
  CFRec *       element(unsigned index);
  int           Write(FILE *f);
  int           LoadFile(char *filename);
  int           LoadFile(FILE *f, unsigned count);

  friend ostream& operator<< (ostream& os, CFRecList & list)
                {
                 for (int i = 0, N = list.NumberElements() ; i < N ; i++ )
                   {
                    os << *list.element(i);
                    if ( list.element(i)->dataLength())
                    os << endl;
                   }
                 return os;
                }
};

class PlacementList : public CFRecList
{
  PlacementList(unsigned, unsigned, unsigned, comp_fn_tp);
 public:
  PlacementList();
  ~PlacementList(){}
  void          Release();
  Placement *   Find(const char * instId);
  Placement *   Find(Placement &);
  Placement *   element(unsigned index);
  friend ostream& operator<< (ostream& os, PlacementList & list)
                {
                 for (int n = 0, N = list.NumberElements() ; n < N ; n++ )
                    os << *list.element(n) << endl;
                 return os;
                }
};

#pragma pack()
#endif

