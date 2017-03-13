#ifndef _AB_PARR_HPP_
#define _AB_PARR_HPP_

#include <stdlib.h>
#include <io.h>
#include <stdio.h>
#include <string.h>

typedef int (* comp_fn_tp)(const void *obj1,const void *obj2);
typedef int (*walk_fn_tp)(void *obj1);

#define PARR_NOT_FOUND         -1
#define PARR_OK                0
#define PARR_BAD               1
#define PARR_FULL              2
#define PARR_NO_MEM            3
#define PARR_NO_FILE           4
#define PARR_LRECL             5
#define PARR_READ              6
#define PARR_INCOMPATIBLE      7
#define PARR_BAD_INDEX         8

#pragma pack(4)
class PtrArray
{
 friend class PtrStrArr;
 friend class PtrStrArrM;
 friend class PtrRecArr;
 friend class PtrRecArrM;
   unsigned       M
                , N
                , Exp;
   comp_fn_tp     comp_fn;
   void       **  P;
   int            RC;


   void       **  GetNPtrs(unsigned);
   int            AccomodateNPtrs(unsigned);
   int            OrderInsertPos(void *);
   int            LSearchX(const void *p,comp_fn_tp comp_fn_in = 0)const;
   int            BSearchX(const void *p)const;
   int            FindX(const void *p,comp_fn_tp comp_fn_in = 0)const;
   void           QSort(void);
   int            PtrX(const void *);
   int            CheckOrderIndex(unsigned index);
   int            MyInsert(void *p,unsigned insert,unsigned fEnforceOrder = 0);
 protected:
   void           Clear(void);
   virtual int    AppendData(void ** Arr, unsigned count);
   int            MyReplace( void *p, void *pRep, comp_fn_tp comp_fn_in = 0,
                             unsigned *pCount = 0, unsigned lrecl = 0);
 public:

  PtrArray( unsigned   Initial_Size = 10
           ,unsigned   Automatic_Expansion = 3
           ,comp_fn_tp Compare_Function = 0);
  ~PtrArray();
  PtrArray(const PtrArray&);
  PtrArray&  operator=(const PtrArray&);
  PtrArray&  operator+=(const PtrArray&);
  void *     operator[](int i);

  int         Expand(unsigned Expansion_number);
  virtual int Add(void *);
  int         AddArray(const PtrArray& other)
           ,  CWalk(walk_fn_tp fn);
  virtual int Visit(void *p){ return 1;}
  int         VisitAll();
  void *      Element(unsigned index)const;
  void *      Pop(int fFree = 0);
  void *      DeQue(int fFree = 0);
  int         Push(void *p) { return AddasFirst(p);}
  int         Que(void *p) { return AddasLast(p);}
  void *      LSearch(const void *p,comp_fn_tp comp_fn_in = 0);
  void *      BSearch(const void *p);
  void *      Find(const void *p,comp_fn_tp comp_fn_in = 0);
  int         Delete(const void *p,comp_fn_tp comp_fn_in = 0,int fFree = 0);
  int         DeletePtr(const void *p,int fFree = 0);
  int         DeleteIndex(int index,int fFree = 0);
  void *      NextPtr(const void *p);
  virtual int Insert(void *p,unsigned index);
  virtual int AddasFirst(void *p);
  void        StopOrdering(void);
  virtual int AddasLast(void *p);
  void        QSort(comp_fn_tp comp_fn_in);
  int         RemoveDupes(comp_fn_tp comp_fn_in = 0,int fFree = 0);
  unsigned    NumberElements(const void *p = 0,comp_fn_tp comp_fn_in = 0)const;
  void        Free(void);
  int         Point( void *Storage, unsigned recl, unsigned count);
  int         Replace1(void *p,void *pRep,int fFree = 1,
                        comp_fn_tp comp_fn_in = 0);
  void        EmptyOut(){N=0;}
};

class PtrStrArr : public PtrArray
{
  int         Delete(const void *,comp_fn_tp,int);
  int         DeletePtr(const void *p,int);
  int         DeleteIndex(int,int);
  int         RemoveDupes(comp_fn_tp,int);
  void *      Pop(int);
  void *      DeQue(int);
 protected:
  int         AppendString(char *p,int fDupe);
  int         MyParse( char *String, char *Delim,unsigned *pCount, int fDupe);
 public:
  PtrStrArr( unsigned   Initial_Size = 10
            ,unsigned   Automatic_Expansion = 3
            ,comp_fn_tp Compare_Function = 0);
  virtual    ~PtrStrArr(){}
  PtrStrArr(const PtrStrArr&);
  PtrStrArr&  operator=(const PtrStrArr&);
  PtrStrArr&  operator+=(const PtrStrArr&);
  PtrStrArr&  operator+(void *);
  PtrStrArr&  operator-(void *);

  int         AddArray(const PtrStrArr&);
  virtual int Delete(const void *p,comp_fn_tp comp_fn_in = 0);
  virtual int DeletePtr(void *);
  virtual int DeleteIndex(int index);
  virtual int RemoveDupes(comp_fn_tp comp_fn_in = 0);
  virtual void * Pop()   {return PtrArray::Pop(0);}
  virtual void * DeQue() {return PtrArray::DeQue(0);}
  int         WriteToFile(char *filename);
  virtual int Replace(void *p, void *pRep,unsigned *pCount = 0,
                      comp_fn_tp comp_fn_in = 0);
  virtual int Tokenize( char *String, char *Tokenchars, unsigned *pCount = 0);
  virtual int Parse( char *String, char *Delimeters,unsigned *pCount = 0);
  char *      element(unsigned index) { return (char *)Element(index);}
};


class PtrStrArrM : public PtrStrArr
{
 protected:
  int         AppendData(void **, unsigned);
 public:
  PtrStrArrM( unsigned   Initial_Size = 10
             ,unsigned   Automatic_Expansion = 3
             ,comp_fn_tp Compare_Function = 0);
  virtual    ~PtrStrArrM();
  PtrStrArrM(const PtrStrArrM&);
  PtrStrArrM&  operator=(const PtrStrArrM&);
  PtrStrArrM&  operator+=(const PtrStrArrM&);

  int         Add(void *);
  int         AddasFirst(void *);
  int         AddasLast(void *);
  int         AddArray(const PtrStrArrM&);
  int         Delete(const void *p,comp_fn_tp comp_fn_in = 0);
  int         DeletePtr(void *);
  int         DeleteIndex(int index);
  void *      Pop()   {return PtrArray::Pop(1);}
  void *      DeQue() {return PtrArray::DeQue(1);}
  int         Insert(void *p,unsigned index);
  int         LoadFile(char *filename);
  int         AddFile(char *fileName);
  int         Replace(void *p, void *pRep,unsigned *pCount = 0,
                      comp_fn_tp comp_fn_in = 0);
  int         RemoveDupes(comp_fn_tp comp_fn_in = 0);
  int         Tokenize( char *String, char *Tokenchars, unsigned *pCount = 0);
  int         Parse( char *String, char *Delimeters,unsigned *pCount = 0);
};

class PtrRecArr : public PtrArray
{
  unsigned    lrecl;
  int         Point( void *Storage, unsigned recl, unsigned count);
  int         Delete(const void *,comp_fn_tp,int);
  int         DeletePtr(const void *p,int);
  int         DeleteIndex(int,int);
  int         RemoveDupes(comp_fn_tp,int);
 protected:

 public:
  PtrRecArr ( unsigned   lrecl = 80
             ,unsigned   Initial_Size = 10
             ,unsigned   Automatic_Expansion = 3
             ,comp_fn_tp Compare_Function = 0);
  virtual    ~PtrRecArr(){}
  PtrRecArr(const PtrRecArr&);
  PtrRecArr&  operator=(const PtrRecArr&);
  PtrRecArr&  operator+=(const PtrRecArr&);
  PtrRecArr&  operator+(void *);
  PtrRecArr&  operator-(void *);

  int         AddArray(const PtrRecArr&);
  virtual int Delete(void *p,comp_fn_tp comp_fn_in = 0);
  virtual int DeletePtr(void *);
  virtual int DeleteIndex(int index);
  virtual void * Pop()   {return PtrArray::Pop(0);}
  virtual void * DeQue() {return PtrArray::DeQue(0);}
  virtual int RemoveDupes(comp_fn_tp comp_fn_in = 0);
  int         WriteToFile(char *filename);
  int         WriteToFile(FILE *);
  int         ReadFileM(FILE * f, unsigned count);
  int         ReadFileM(char *filename);
  virtual int Replace(void *p, void *pRep,unsigned *pCount = 0,
                      comp_fn_tp comp_fn_in = 0);
  virtual int Point( void *Storage, unsigned count);
  unsigned    LRecl() const { return lrecl;}
};


class PtrRecArrM : public PtrRecArr
{
  protected:
  int         AppendData(void **, unsigned);
 public:
  PtrRecArrM( unsigned   lrecl = 80
             ,unsigned   Initial_Size = 10
             ,unsigned   Automatic_Expansion = 3
             ,comp_fn_tp Compare_Function = 0);
  ~PtrRecArrM();
  PtrRecArrM(const PtrRecArrM&);
  PtrRecArrM&  operator=(const PtrRecArrM&);

  int         Add(void *p);
  int         AddArray(const PtrRecArrM&);
  int         Delete(void *p,comp_fn_tp comp_fn_in = 0);
  int         DeletePtr(void *);
  int         DeleteIndex(int index);
  void *      Pop()   {return PtrArray::Pop(1);}
  void *      DeQue() {return PtrArray::DeQue(1);}
  int         AddFile(char *filename);
  int         AddFromFile(FILE *f,unsigned count);
  int         LoadFile(char *filename);
  int         LoadFile(FILE *f,unsigned count);
  int         Replace(void *p, void *pRep,unsigned *pCount = 0,
                      comp_fn_tp comp_fn_in = 0);
  int         RemoveDupes(comp_fn_tp comp_fn_in = 0);
  int         Point( void *Storage, unsigned count);
};
#pragma pack()
#endif

