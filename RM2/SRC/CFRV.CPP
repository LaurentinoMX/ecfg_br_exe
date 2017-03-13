 /*--------------------------------------------------------------------------*\
 |    Filename - CFRV.CPP                                                     |
 |      contains source for the following RM/2 class implementations:         |
 |                                                                            |
 |       CFRecV    - Variable length CFReport record object.                  |
 |       CFRecVL   - Array of CFRecV object pointers.                         |
 |       CFRInst   - To access the CFReport's Placement Records               |
 |                   (Type 55) and Connection Records (Type 56).              |
 |                   (Can handle any CFReport Record that starts              |
 |                    with a CFR_Inst structure as defined in                 |
 |                    EUOPDEF.H)                                              |
 |       CFRInstL  - Array of CFRInstL object pointers.                       |
 |   PlacementList - Array of Placement object pointers.                      |
 \*--------------------------------------------------------------------------*/
 //---------------------------------------------------------------------------
 // MODIFICATION LOG:
 // 07/01/2002 EDW   D72623 Fixed a bug in the CFRecV::fWriteCFR(FILE *f) method:
 //                  newline chars are sometimes in the middle of output records, so
 //                  this fix will strip all newline chars out of the record before
 //                  writing it to the output report.
 // 25/10/2004 ZOR   D95887 add new method  CFRecVL :: fWrite53CFR(FILE *f, int n)
 //                  to write the 53 record

        /*---------------------------------------------------*\
        |                      Includes                       |
        \*---------------------------------------------------*/
#include <isynonym.hpp>                          // Definition of Boolean etc
#include <string.h>
#include <stdio.h>

        /*---------------------------------------------------*\
        |               RM/2-specific includes                |
        \*---------------------------------------------------*/
#include "cfrv.hpp"

 /*--------------------------------------------------------------------------*\
 |                                CFRecV Class                                |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |                        Ctor                         |
        \*---------------------------------------------------*/
CFRecV :: CFRecV(CFRecV const& other)
{
 set(other.Rec);
 I[0] = other.I[0];
}
    /*-----------------------------------------------------------------------*\
    |    void set (const char* const& pch)                                    |
    |       copy the record from the null-terminated buffer                   |
    \*-----------------------------------------------------------------------*/
void CFRecV::set (const char* const& pch)
{
 I[1] = (pch? strlen(pch) : 0);
 Rec = new char[I[1] + 2];
 mystrncpy(Rec,pch,I[1]);
 I[0] = 0;
}

    /*-----------------------------------------------------------------------*\
    |    void CFRecV::set (unsigned l, char c)                                |
    |       allocate the record of the given length                           |
    |       and init it to the given character.                               |
    \*-----------------------------------------------------------------------*/
void CFRecV::set (unsigned l, char c)
{
 I[1] = ALMAX(l,1);
 Rec = new char[I[1] + 2];
 memset(Rec,c,I[1]);
 Rec[I[1]] = '\0';
 I[0] = 0;
}

    /*-----------------------------------------------------------------------*\
    |   void CFRecV::set (unsigned l,const char *pch)                         |
    |       allocate the record of the given length                           |
    |       and copy from the given buffer up to the                          |
    |       null terminator or as much as can fit.                            |
    \*-----------------------------------------------------------------------*/
void CFRecV::set (unsigned l,const char *pch)
{
 I[1] = l;
 Rec = new char[I[1] + 2];
 mystrncpy(Rec,pch,I[1]);
 I[0] = 0;
}

    /*-----------------------------------------------------------------------*\
    |   void CFRecV::set (unsigned l,const char *pch)                         |
    |       allocate the record of the given length                           |
    |       and copy from the given buffer up to the                          |
    |       null terminator (filling the remainder                            |
    |       with the given character) or  as much as can fit.                 |
    \*-----------------------------------------------------------------------*/
void CFRecV::set (unsigned l,const char *pch, char c)
{
 set(l,pch);
 stringpaddchar(Rec,c,l);
}

    /*-------------------------------------------------------*\
    |                       Assignment                        |
    \*-------------------------------------------------------*/
CFRecV& CFRecV :: operator= (CFRecV const& other)
{
 if (&other != this) {
   unSet();
   set(other.Rec);
   I[0] = other.I[0];                            // copy id
   }
 return *this;
}

    /*-----------------------------------------------------------------------*\
    |   += operator                                                           |
    |       append the record with another.                                   |
    \*-----------------------------------------------------------------------*/
CFRecV& CFRecV :: operator+= (CFRecV const& other)
{
 int newlen = I[1] + other.I[1];
 char * pc = new char[newlen + 2];
 memcpy(pc,Rec,I[1]);
 memcpy(pc+I[1],other.Rec,other.I[1]);
 pc[newlen] = '\0';
 I[1] = newlen;
 unSet();
 Rec = pc;
 return *this;
}

Boolean  CFRecV::fWriteCFR(FILE *f)
{
 return (fputs(ensureLF(stringreplace(Rec, "\n", " ", strlen(Rec))),f) != EOF);   /* D72623 EDW 07/01/2002 */
}

    /*-----------------------------------------------------------------------*\
    |   Boolean  fWriteA(FILE *f)                                             |
    |       write the record only as a string to the current                  |
    |       position of a given ASCI file.                                    |
    |       Note: appends a linefeed at all times.                            |
    \*-----------------------------------------------------------------------*/
Boolean  CFRecV :: fWriteA(FILE *f)
{
 Rec[I[1]] = '\n';
 Rec[I[1] + 1] = '\0';
 Boolean fOK = (fputs(Rec,f) != EOF);
 Rec[I[1]] = '\0';
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean CFRecV :: fWriteB(FILE *f, unsigned * pBytesWrit)             |
    |       write the whole object to the current                             |
    |       position of a given file and return the                           |
    |       number of bytes written.                                          |
    |       Note: the two ints containing id and record length                |
    |             are written upfront.                                        |
    \*-----------------------------------------------------------------------*/
Boolean CFRecV :: fWriteB(FILE *f, unsigned * pBytesWrit)
{
 Boolean fOK = false;
 unsigned u = 0;
 if (fOK = (fwrite(I,sizeof(I),1,f) == 1)) {
   u = sizeof(I);
   if (I[1] && (fOK = (fwrite(Rec,I[1],1,f) == 1)) )
     u += I[1];
   }
 if (pBytesWrit) *pBytesWrit = u;
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean CFRecV :: fReadB(FILE *f,unsigned * pBytesRead)               |
    |       read the whole object at the current                              |
    |       position of a given file and return the                           |
    |       number of bytes read.                                             |
    |       Note: Only to be used for a file made by 'fWriteB'.               |
    |             The two ints containing id and record length                |
    |             are read first.                                             |
    \*-----------------------------------------------------------------------*/
Boolean CFRecV :: fReadB(FILE *f,unsigned * pBytesRead)
{
 Boolean fOK = false;
 unsigned u = 0;
 if (fOK = (fread(I,sizeof(I),1,f) == 1)) {
   u = sizeof(I);
   unSet();
   if (I[1]) {
     if (Rec = new char[I[1]+2])
       if (fOK = (fread(Rec,I[1],1,f) == 1)) {
         Rec[I[1]] = '\0';
         u += I[1];
         init();
         }
     }
   else set((char *)0);
   }
 if (pBytesRead) *pBytesRead = u;
 return fOK;
}

 /*--------------------------------------------------------------------------*\
 |                               CFRecVL Class                                |
 \*--------------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------*\
    |       'C' CFRec object compare function called by                       |
    |       class' 'Find'and sorting methods.                                 |
    \*-----------------------------------------------------------------------*/
int CFRecV_comp(const void *p1,const void * p2)
{
// CFRecV * pR1 = (CFRecV *)p1,
//        * pR2 = (CFRecV *)p2;
// return pR1->id() - pR2->id();
 return (((CFRecV *)p1)->id() - ((CFRecV *)p2)->id());
}

        /*---------------------------------------------------*\
        |                        Ctor                         |
        \*---------------------------------------------------*/
CFRecVL :: CFRecVL()
         : PtrRecArr(sizeof(CFRecV))
{}

    /*-----------------------------------------------------------------------*\
    |   CFRecV * Find(const char * instId)                                    |
    |       find the object in the array via its given                        |
    |       Id and return its pointer.                                        |
    \*-----------------------------------------------------------------------*/
CFRecV * CFRecVL :: find(int id)
{
 CFRecV C(0,id);
 return find(C);
}

    /*-----------------------------------------------------------------------*\
    |   CFRecV * find(const CFRecV &)                                         |
    |       find the given object in the array and return its                 |
    |       pointer.                                                          |
    \*-----------------------------------------------------------------------*/
CFRecV * CFRecVL::find(const CFRecV & C)
{
 return (CFRecV *)PtrRecArr::Find((const void *)&C,CFRecV_comp);
}

      /*---------------------------------------------------------------------*\
      |    CFRecV * element(unsigned i)                                       |
      |       Return the i-th ( 0 based ) array element.                      |
      \*---------------------------------------------------------------------*/
CFRecV * CFRecVL::element(unsigned i)const
{
 return (CFRecV *)(PtrArray::Element(i));
}

    /*-----------------------------------------------------------------------*\
    |    void Release()                                                       |
    |       call 'delete' for all array elements                              |
    |       and mark the array as empty.                                      |
    \*-----------------------------------------------------------------------*/
void CFRecVL::Release()
{
 for (int n = 0, N = NumberElements(); n < N ;n++ )
   delete element(n);
 EmptyOut();
}

    /*-----------------------------------------------------------------------*\
    |    unsigned fWriteBLength()                                             |
    |       Return the number of bytes the records in the array               |
    |       would occupy if it was to be written to a binary file             |
    |       by method  'fWriteB'.                                             |
    \*-----------------------------------------------------------------------*/
unsigned CFRecVL::fWriteBLength()
{
 for (unsigned len = 2 * sizeof(int),n = 0,N = NumberElements() ; n < N ; n++ )
   len += element(n)->fWriteBLength();
 return len;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean CFRecVL :: fWriteA(FILE *f)                                  |
    |       Write all records at the current position of a                    |
    |       given ASCI file.                                                  |
    \*-----------------------------------------------------------------------*/
Boolean CFRecVL :: fWriteA(FILE *f)
{
 Boolean fOK = true;
 for (unsigned n = 0, N = NumberElements() ; fOK && (n < N) ; n++ )
   fOK = element(n)->fWriteA(f);
 return fOK;
}

unsigned CFRecVL :: fWriteCFR(FILE *f)
{
 for (unsigned n=0,N=NumberElements();(n < N) && element(n)->fWriteCFR(f) ;n++);
 return n;
}

// ZOR 95887       write every 53 record
unsigned CFRecVL :: fWrite53CFR(FILE *f, int n)
{
 element(n)->fWriteCFR(f);
 return n;
}
    /*-----------------------------------------------------------------------*\
    |    Boolean fWriteB(FILE *f, unsigned * pBytesWrit)                      |
    |       Write the array in binary format at the current                   |
    |       position of the given file and set                                |
    |       the number of bytes written                                       |
    |       Note: 2 ints are written up front containing                      |
    |             the class' signature and number of elements.                |
    \*-----------------------------------------------------------------------*/
#define CFRV_SIGNATURE 92774
Boolean CFRecVL :: fWriteB(FILE *f, unsigned * pBytesWrit)
{
 unsigned U2[2];
 unsigned len, Total = 0;
 Boolean fOK;
 U2[0] = CFRV_SIGNATURE;
 U2[1] = NumberElements();
 if (fOK = (fwrite(U2,sizeof(U2),1,f) == 1)) {
   Total = sizeof(U2);
   for (int i = 0; fOK && (i < U2[1]); i++)
     if (fOK = element(i)->fWriteB(f,&len))
       Total += len;
   }
 if (pBytesWrit)
   *pBytesWrit = Total;
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    unsigned fReadB(FILE * f)                                            |
    |       Add records at the current position of a given                    |
    |       file which is to have been created by the 'fWriteB' method.       |
    \*-----------------------------------------------------------------------*/
Boolean CFRecVL::fReadB(FILE *f)
{
 unsigned U2[2], n;
 CFRecV *p;
 Boolean fOK = false;
 if (fread(U2,sizeof(U2),1,f) == 1)
   if ( CFRV_SIGNATURE == U2[0])
     for (n = 0, fOK = true ; fOK && (n < U2[1]); n++)
       if (fOK = ((p = new CFRecV()) != 0))
         if (fOK = p->fReadB(f))
           Add(p);
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean duplicate(const CFRecVL & other)                             |
    |       Add 'newed' records from another to the array.                    |
    \*-----------------------------------------------------------------------*/
Boolean CFRecVL::duplicate(const CFRecVL & other)
{
 Boolean fOK = true;
 CFRecV *p;
 for (unsigned n = 0, N = other.NumberElements() ; fOK && (n < N) ; n++ )
  if (fOK = ( (p = new CFRecV(*(other.element(n)))) != 0))
     Add(p);
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    int fReadB_rc(FILE *f)                                               |
    |       Add records at the current position of a given                    |
    |       file which is to have been created by the 'fWriteB' method        |
    |       and return the result via codes as defined in CFRV.HPP.           |
    \*-----------------------------------------------------------------------*/
int CFRecVL::fReadB_rc(FILE *f)
{
 unsigned U2[2];
 CFRecV * p;
 int rc = 0;
 if (fread(U2,sizeof(U2),1,f) == 1)
   if ( CFRV_SIGNATURE == U2[0])
     for (int i = 0; !rc && (i < U2[1]); i++)
       if ( (p = new CFRecV()) && p->fReadB(f))
         Add(p);
       else rc = CFRV_ERR_READ_FILE;
   else rc = CFRV_ERR_NO_CFRV_FILE;
 else rc = CFRV_ERR_READ_FILE;
 return rc;
}

    /*-----------------------------------------------------------------------*\
    |    void  sort()                                                         |
    |       sort by calling the array's QSort method                          |
    |       with our comapre function                                         |
    \*-----------------------------------------------------------------------*/
void  CFRecVL :: sort()
{
 QSort(CFRecV_comp);
}

 /*--------------------------------------------------------------------------*\
 |                               CFRInst Class                                |
 \*--------------------------------------------------------------------------*/
    /*-------------------------------------------------------*\
    |                       Assignment                        |
    \*-------------------------------------------------------*/
CFRInst &  CFRInst::operator=(const CFRInst & other)
{ if (&other != this) {
    *(CFRecV *)this = *(CFRecV *)&other;
    init();
    }
  return *this;
}

    /*-----------------------------------------------------------------------*\
    |    void init()                                                          |
    |       set the object's Instance Id from its record                      |
    \*-----------------------------------------------------------------------*/
void CFRInst:: init()
{
 if (length() >= offsetof(CFR_Inst,Remainder))
  mystrstrip(mystrncpy(InstId,((CFR_Inst *)record())->InstId,sizeof(InstId)-1));
 else *InstId = '\0';
}

    /*-----------------------------------------------------------------------*\
    |   Boolean setInstId(unsigned id)                                        |
    |       set the given Instance Id in the record                           |
    \*-----------------------------------------------------------------------*/
Boolean CFRInst::setInstId(unsigned id)
{
 char Buf[10];
 return setInstId(_itoa(id,Buf,10));
}

    /*-----------------------------------------------------------------------*\
    |   Boolean setInstId(const char * instid)                                |
    |       set the Instance Id in the record as well                         |
    |       as the object                                                     |
    \*-----------------------------------------------------------------------*/
Boolean CFRInst::setInstId(const char * instid)
{
 mystrncpy(InstId,instid,sizeof(InstId)-1);
 if (length() >= offsetof(CFR_Inst,Remainder)) {
   CFRPaddCopySZ(((CFR_Inst *)record())->InstId,InstId,
                 sizeof(((CFR_Inst *)record())->InstId));
   return true;
   }
 return false;
}

 /*--------------------------------------------------------------------------*\
 |                               CFRInstL Class                               |
 \*--------------------------------------------------------------------------*/
    /*-----------------------------------------------------------------------*\
    |       'C' CFRInst object compare function called by                     |
    |       class' 'Find'and sorting methods.                                 |
    \*-----------------------------------------------------------------------*/
int CFRInst_comp(const void *p1,const void * p2)
{
 return ((CFRInst *)p1)->compare(*(CFRInst *)p2);
}

        /*---------------------------------------------------*\
        |                        Ctor                         |
        \*---------------------------------------------------*/
CFRInstL::CFRInstL(Boolean fSorted)
         : PtrRecArr(sizeof(CFRInst),10,3,fSorted ? CFRInst_comp : 0)
{}

    /*-----------------------------------------------------------------------*\
    |   CFRInst * find(const char * instId)                                   |
    |       find the object in the array via its given                        |
    |       Instance Id and return its pointer.                               |
    \*-----------------------------------------------------------------------*/
CFRInst * CFRInstL::find(const char * instId)
{
 CFRInst C;
 C.setInstId(instId);
 return find(C);
}

    /*-----------------------------------------------------------------------*\
    |   CFRInst * find(int id)                                                |
    |       find the object in the array via its given                        |
    |       Id and return its pointer.                                        |
    \*-----------------------------------------------------------------------*/
CFRInst * CFRInstL :: find(int id)
{
 CFRInst * p;
 for (int n = 0, N = NumberElements(); n < N ;n++ )
   if ((p = element(n)) && (p->id() == id))
     return p;
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |   CFRInst * find(const CFRInst & C)                                     |
    |       find the given object in the array and return its                 |
    |       pointer.                                                          |
    \*-----------------------------------------------------------------------*/
CFRInst * CFRInstL::find(const CFRInst & C)
{
 return (CFRInst *)PtrRecArr::Find((const void *)&C);
}

      /*---------------------------------------------------------------------*\
      |    CFRInst * element(unsigned i)                                      |
      |       Return the i-th ( 0 based ) array element.                      |
      \*---------------------------------------------------------------------*/
CFRInst * CFRInstL::element(unsigned i)
{
 return (CFRInst *)(PtrArray::Element(i));
}

      /*---------------------------------------------------------------------*\
      |    void Release()                                                     |
      |       call 'delete' for all array elements                            |
      |       and mark the array as empty.                                    |
      \*---------------------------------------------------------------------*/
void CFRInstL::Release()
{
 for (int n = 0, N = NumberElements(); n < N ;n++ )
   delete element(n);
 EmptyOut();
}

    /*-----------------------------------------------------------------------*\
    |    unsigned fWriteBLength()                                             |
    |       Return the number of bytes the records in the array               |
    |       would occupy if it was to be written to a binary file             |
    |       by method  'fWriteB'.                                             |
    \*-----------------------------------------------------------------------*/
unsigned CFRInstL::fWriteBLength()
{
 for (unsigned len = 2 * sizeof(int),n = 0,N = NumberElements() ; n < N ; n++ )
   len += element(n)->fWriteBLength();
 return len;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean CFRecVL :: fWriteA(FILE *f)                                  |
    |       Write all records at the current position of a                    |
    |       given ASCI file.                                                  |
    \*-----------------------------------------------------------------------*/
Boolean CFRInstL :: fWriteA(FILE *f)
{
 Boolean fOK = true;
 for (unsigned n = 0, N = NumberElements() ; fOK && (n < N) ; n++ )
   fOK = element(n)->fWriteA(f);
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean fWriteB(FILE *f, unsigned * pBytesWrit)                      |
    |       Write the array in binary format at the current                   |
    |       position of the given file and set                                |
    |       the number of bytes written                                       |
    |       Note: 2 ints are written up front containing                      |
    |             the class' signature and number of elements.                |
    \*-----------------------------------------------------------------------*/
Boolean CFRInstL :: fWriteB(FILE *f, unsigned * pBytesWrit)
{
 unsigned U2[2];
 unsigned len, Total = 0;
 Boolean fOK;
 U2[0] = CFRV_SIGNATURE;
 U2[1] = NumberElements();
 if (fOK = (fwrite(U2,sizeof(U2),1,f) == 1)) {
   Total = sizeof(U2);
   for (int i = 0; fOK && (i < U2[1]); i++)
     if (fOK = element(i)->fWriteB(f,&len))
       Total += len;
   }
 if (pBytesWrit)
   *pBytesWrit = Total;
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    unsigned fReadB(FILE * f)                                            |
    |       Add records at the current position of a given                    |
    |       file which is to have been created by the 'fWriteB' method.       |
    \*-----------------------------------------------------------------------*/
Boolean CFRInstL::fReadB(FILE *f)
{
 unsigned U2[2], n;
 CFRInst *p;
 Boolean fOK = false;
 if (fread(U2,sizeof(U2),1,f) == 1)
   if ( CFRV_SIGNATURE == U2[0])
     for (n = 0, fOK = true ; fOK && (n < U2[1]); n++)
       if (fOK = ((p = new CFRInst()) != 0))
         if (fOK = p->fReadB(f))
           Add(p);
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    int fReadB_rc(FILE *f)                                               |
    |       Add records at the current position of a given                    |
    |       file which is to have been created by the 'fWriteB' method        |
    |       and return the result via codes as defined in CFRV.HPP.           |
    \*-----------------------------------------------------------------------*/
int CFRInstL::fReadB_rc(FILE *f)
{
 unsigned U2[2];
 CFRInst * p;
 int rc = 0;
 if (fread(U2,sizeof(U2),1,f) == 1)
   if ( CFRV_SIGNATURE == U2[0])
     for (int i = 0; !rc && (i < U2[1]); i++)
       if ( (p = new CFRInst()) && p->fReadB(f))
         Add(p);
       else rc = CFRV_ERR_READ_FILE;
   else rc = CFRV_ERR_NO_CFRV_FILE;
 else rc = CFRV_ERR_READ_FILE;
 return rc;
}
//////////////////////////////////////////////////////////////

#ifdef UNITTEST

#include <stdlib.h>
#include <iostream.h>


void main(void)
{
 if (1)
   {
    FILE *f;
    CFRecV rec1("55     1U    089021       2111      500       1   9063       1   9922       3");
    CFRecV rec2("55     2U    089021       2111      500       1   9063       1   9922");
    CFRecV rec3("55     3I    089021       2111      500       1   9063       1");
    CFRecV rec4("55     4I    089021       2111      500       1   9063");
    CFRecV rec5("55     6I    089021       2111      500       1");
    CFRecV rec6("55     7I    089021       2111      500");
    CFRecV rec7("55     8I    089021       2111");
    CFRecV rec8("55     9I    089021");
    unsigned u;
    int i;

    cout <<  "rec1 : " << rec1 << endl;
    cout <<  "rec2 : " << rec2 << endl;
    cout <<  "rec3 : " << rec3 << endl;
    cout <<  "rec4 : " << rec4 << endl;
    cout <<  "rec5 : " << rec5 << endl;
    cout <<  "rec6 : " << rec6 << endl;
    cout <<  "rec7 : " << rec7 << endl;
    cout <<  "rec8 : " << rec8 << endl;
    CFRecVL List;
    List.Add(&rec1);
    List.Add(&rec2);
    List.Add(&rec3);
    List.Add(&rec4);
    List.Add(&rec5);
    List.Add(&rec6);
    List.Add(&rec7);
    List.Add(&rec8);

    unsigned Bytes = 0, Total = 0;
    cout << endl << "Total fWriteBLength() : " << List.fWriteBLength() << endl;

    if (f = fopen("CFRECVL.TST","wb"))
      {
        List.fWriteB(f,&Bytes);
        cout << endl << "Total bytes written : " << Bytes  << endl;
        fclose(f);
        if (f = fopen("CFRECVL.TST","rb"))
          {
           List.EmptyOut();
           List.fReadB(f);
           cout << endl << "The read list : " << endl;
           cout << List;
           if (f = fopen("CFRECVL.TOT","w"))
             {
               List.fWriteA(f);
               fclose(f);
             }
           List.Release();
          }

      }

    CFRInstL ListI;
    CFRInst recI1("55     1U    089021       2111      500       1   9063       1   9922       3");
    CFRInst recI2("55     2U    089021       2111      500       1   9063       1   9922");
    CFRInst recI3("55     3I    089021       2111      500       1   9063       1");
    CFRInst recI4("55     4I    089021       2111      500       1   9063");
    CFRInst recI5("55     6I    089021       2111      500       1");
    CFRInst recI6("55     7I    089021       2111      500");
    CFRInst recI7("55     8I    089021       2111");
    CFRInst recI8("55     9I    089021");
    CFRInst recI9;
    cout << "Default constructor for recI9 : " << endl << recI9 << endl;

    recI9 = recI1;
    cout << "recI9 = recI1 -> recI9 : " << endl << recI9 << endl;

    CFRInst *p1;
    p1 = &recI1;
    p1 = &recI2;
    p1 = &recI3;
    p1 = &recI4;
    p1 = &recI5;
    p1 = &recI6;
    p1 = &recI7;
    p1 = &recI8;
    ListI.Add(&recI1);
    ListI.Add(&recI2);
    ListI.Add(&recI3);
    ListI.Add(&recI4);
    ListI.Add(&recI5);
    ListI.Add(&recI6);
    ListI.Add(&recI7);
    ListI.Add(&recI8);

    Bytes = 0, Total = 0;
    cout << endl << "Total fWriteBLength() : " << ListI.fWriteBLength() << endl;

    if (f = fopen("CFRECVIL.TST","wb"))
      {
        ListI.fWriteB(f,&Bytes);
        cout << endl << "Total bytes written : " << Bytes  << endl;
        fclose(f);
        if (f = fopen("CFRECVIL.TST","rb"))
          {
           ListI.EmptyOut();
           ListI.fReadB(f);
           cout << endl << "The read list : " << endl;
           cout << ListI;
           if (f = fopen("CFRECVIL.TOT","w"))
             {
               ListI.fWriteA(f);
               fclose(f);
             }
           CFRInst *p;
           ListI.Release();
          }

      }
   }

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif

}

#endif


//////////////////////////////////////////////////////////////
