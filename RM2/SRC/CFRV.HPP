 /*--------------------------------------------------------------------------*\
 |    Filename - CFRV.HPP                                                     |
 |      decalrations for the following RM/2 class implementations:            |
 |                                                                            |
 |       CFRecV    - Variable length CFReport record object.                  |
 |       CFRecVL   - Array of CFRecV object pointers.                         |
 |       CFRInst   - To access the CFReport's Placement Records               |
 |                   (Type 55) and Connection Records (Type 56).              |
 |                   (Can handle any CFReport Record that starts              |
 |                    with a CFR_Inst structure as defined in                 |
 |                    EUOPDEF.H)                                              |
 |       CFRInstL  - Array of CFRInstL object pointers.                       |
 \*--------------------------------------------------------------------------*/
#ifndef _AB_CFRV_HPP_
#define _AB_CFRV_HPP_

#include <fstream.h>
#include <stdio.h>
#include <idate.hpp>

#include "euodef.h"
#include "euoutil.hpp"
#include "parr.hpp"

#define CFRV_ERR_READ_FILE    1
#define CFRV_ERR_NO_CFRV_FILE 2
#define CFRV_ERR_NO_FILE      3

#pragma pack(4)

class CFRecV
{
     int   I[2];   // I[0] : id  I[1] : reclen
     char* Rec;

     void set (const char* const& pch);

     void set (unsigned l, char c);

     void set (unsigned l,const char *pch);
     void set (unsigned l,const char *pch, char );

    inline void unSet() { delete[] Rec; }

  public:

     CFRecV (const char* pch = 0)           { set(pch); }
     CFRecV (unsigned len, char c = ' ') { set(len,c); }
     CFRecV (unsigned len,const char* pch) { set(len,pch); }
     CFRecV (unsigned len,const char* pch, char c ) { set(len,pch,c); }
     CFRecV (const char * pch,int id) { set(pch); I[0] = id; }
     CFRecV (CFRecV const& other);
    ~CFRecV ()                     { unSet(); }
     CFRecV& operator= (CFRecV const& other);
     CFRecV& operator+= (CFRecV const& other);
     Boolean operator== (CFRecV const& other) const
                         { return compare(other) == 0; }
     Boolean operator== (char* pch) const
                         { return strcmp(Rec, pch) == 0; }
     Boolean operator!= (CFRecV const& other) const
                         { return compare(other) != 0; }
     Boolean operator< (CFRecV const& other) const
                         { return compare(other) < 0; }
     Boolean operator> (CFRecV const& other) const
                         { return compare(other) > 0; }
                  operator const char * () const
                         { return (const char *)Rec; }
     int          compare(CFRecV const& other) const
                         { return strcmp(Rec, other.Rec); }
     int const&      length() const {return I[1];}
     int const&      id() const {return I[0];}
     const char *    record() { return (const char *)Rec;}
     void            setId(int i) {I[0] = i;}
     void            setLength(int i) {I[1] = i;}        /* F72273 HECC: 07/12/2002 */
     void            setRec(IString str)
                           {strcpy(Rec, str);}           /* F72273 HECC: 07/12/2002 */
     int             type() { return (I[1] > 1)? CFRecType(Rec) : -1;}
     void            setType(unsigned nmbr)
                            { if (I[1] > 1)
                                setCFRecType(Rec,nmbr);
                            }
     Boolean         fWriteA(FILE *f);
     Boolean         fWriteCFR(FILE *f);
     unsigned        fWriteBLength() { return (sizeof(I) + I[1]);}
     Boolean         fWriteB(FILE *f, unsigned * pBytesWrit = 0);
     Boolean         fReadB(FILE *f,unsigned * pBytesRead = 0);
     virtual void    init() {}

     friend ostream& operator<< (ostream& os, CFRecV const& rec)
                               { return os << rec.record(); }

  protected:
     char *          record()   const {return Rec;}

};

class CFRecVL : public PtrRecArr
{
  CFRecVL(unsigned, unsigned, unsigned, comp_fn_tp);
 public:
  CFRecVL();
  ~CFRecVL(){}
  virtual void  Release();
  CFRecV *      find(int id);
  CFRecV *      find(const CFRecV &);
  CFRecV *      find(const char * record);
  CFRecV *      element(unsigned index)const;
  Boolean       fWriteA(FILE *f);
  Boolean       duplicate(const CFRecVL & other);
  unsigned      fWriteBLength();
  unsigned      fWriteCFR(FILE *);
  unsigned      fWrite53CFR(FILE *f,int n);  // 95887
  Boolean       fWriteB(FILE *f, unsigned * pBytesWrit = 0);
  Boolean       fReadB(FILE *f);
  int           fReadB_rc(FILE *f);
  void          sort();

  friend ostream& operator<< (ostream& os, CFRecVL & list)
                { for (int n = 0, N = list.NumberElements() ; n < N ; n++ )
                    os << *list.element(n) << endl;
                  return os;
                }
};

class CFRInst : public CFRecV
{
  char       InstId[CFREC_INSTID_SIZE + 1];

 public:

  CFRInst(const char * Buffer = 0) : CFRecV(Buffer){init();}
  CFRInst(const CFRInst & other) { *this = other;}
  CFRInst(const CFR_Inst & rInst) : CFRecV((const char *)&rInst){init();}
  ~CFRInst(){}
  CFRInst  &  operator=(const CFRInst & other);
  Boolean operator==(CFRInst const& other) const
                     { return !compare(other);}
  Boolean operator< (CFRInst const& other) const
                     { return (compare(other) < 0);}
  Boolean operator> (CFRInst const& other) const
                     { return (compare(other) > 0);}
  int         compare(CFRInst const& other) const
                     { return strcmp(InstId,other.InstId);}
  void        init();
  const char *   instId() const { return (const char *)InstId;}
  Boolean        setInstId(const char * instid);
  Boolean        setInstId(unsigned id);

  friend ostream& operator<< (ostream& os, CFRInst const& cfr)
                       { os << "Id : '" << cfr.instId()
                            << "' Record : " << cfr.record();
                         return os;
                       }
};

class CFRInstL : public PtrRecArr
{
  CFRInstL(unsigned, unsigned, unsigned, comp_fn_tp);
 public:
  CFRInstL(Boolean fSorted = true);
  ~CFRInstL(){}
  virtual void  Release();
  CFRInst *       find(int id);
  CFRInst *       find(const CFRInst &);
  CFRInst *       find(const char * instId);
  CFRInst *       element(unsigned index);
  Boolean         fWriteA(FILE *);
  unsigned        fWriteBLength();
  Boolean       fWriteB(FILE *f, unsigned * pBytesWrit = 0);
  Boolean       fReadB(FILE *f);
  int           fReadB_rc(FILE *f);

  friend ostream& operator<< (ostream& os, CFRInstL & list)
                { for (int n = 0, N = list.NumberElements() ; n < N ; n++ )
                    os << *list.element(n) << endl;
                  return os;
                }
};

#pragma pack()
#endif
