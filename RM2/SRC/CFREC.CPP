 /*--------------------------------------------------------------------------*\
 |    Filename - CFREC.CPP                                                    |
 |      contains source for the following RM/2 class implementations:         |
 |                                                                            |
 |       CFRec     - Fixed Length (MAX_CFREC_SIZE defined in                  |
 |                   EUODEF.H) CFReport record.                               |
 |       CFRecList - Array of CFRec object pointers.                          |
 |       Placement - To access the CFReport's Placement Record                |
 |                   ( Type 55) as defined in 'euopdef.h'                     |
 |   PlacementList - Array of Placement object pointers.                      |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                     C Includes                      |
        \*---------------------------------------------------*/
#include <isynonym.hpp>                          // Definition of Boolean etc
#include <string.h>
#include <stdio.h>

        /*---------------------------------------------------*\
        |               RM/2-specific includes                |
        \*---------------------------------------------------*/
#include "cfrec.hpp"

 /*--------------------------------------------------------------------------*\
 |                                CFRec Class                                 |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |                        Ctors                        |
        \*---------------------------------------------------*/
CFRec::CFRec()
      : nmbr(0)
{
 memset(Data,0,sizeof(Data));
}

CFRec::CFRec(char * data,unsigned number)
      : nmbr(number)
{
 mystrncpy(Data,data,sizeof(Data)-1);
}

CFRec::CFRec(const CFRec & other)
{
 *this = other;
}

    /*-----------------------------------------------------------------------*\
    |    void setData(const char * data)                                      |
    |       set the record from the given buffer                              |
    \*-----------------------------------------------------------------------*/
void CFRec::setData(const char * data)
{
 mystrncpy(Data,data,sizeof(Data)-1);
}

    /*-----------------------------------------------------------------------*\
    |    unsigned CFRec::type()                                               |
    |       return the record's type.                                         |
    \*-----------------------------------------------------------------------*/
unsigned CFRec::type()
{
 if (ISDIGIT(Data[0]) && ISDIGIT(Data[1]))
   return (((Data[0] - '0') * 10) + (Data[1] - '0'));
 else return 0;
}

    /*-----------------------------------------------------------------------*\
    |    void setType(unsigned type)                                          |
    |       set the record's type.                                            |
    \*-----------------------------------------------------------------------*/
void CFRec::setType(unsigned type)
{
 Data[0] = '0' + (char)((type%100)/10);
 Data[1] = '0' + (char)(type%10);
}

    /*-------------------------------------------------------*\
    |                       Assignment                        |
    \*-------------------------------------------------------*/
CFRec & CFRec::operator=(CFRec const& other)
{
 if (this != &other) {
   memcpy(Data,other.Data,sizeof(Data));
   nmbr = other.nmbr;
   }
 return *this;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean  readLine(ifstream & fin)                                    |
    |       Read a line from the given stream after                           |
    |       emptying out the record.                                          |
    \*-----------------------------------------------------------------------*/
Boolean  CFRec::readLine(ifstream & fin)
{
 memset(Data,0,sizeof(Data));
 return fin.getline(Data,sizeof(Data) - 1)? true : false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean  readLine(FILE * f)                                          |
    |       Read a line at the current position of a given                    |
    |       file after nulling out the record.                                |
    \*-----------------------------------------------------------------------*/
Boolean  CFRec::readLine(FILE * f)
{
 memset(Data,0,sizeof(Data));
// return fread(Data,sizeof(char),sizeof(Data) - 1,f)? true : false;
// return fgets(Data,sizeof(Data),f)? true : false;
 if (fgets(Data,sizeof(Data),f)) {
   mystrrearstrip(Data);
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean  Write(FILE * f)                                             |
    |       Write the line to the current position of a given                 |
    |       file after ensuring the record's last character                   |
    |       is a linefeed.                                                    |
    \*-----------------------------------------------------------------------*/
Boolean CFRec::Write(FILE *f)
{
// return (fputs(Data,f) != EOF);
 return (fputs(ensureLF(Data),f) != EOF);
}

    /*-----------------------------------------------------------------------*\
    |    char * copyData(char * Target)                                       |
    |       copy the record to the given target.                              |
    \*-----------------------------------------------------------------------*/
char * CFRec::copyData(char * Target)
{
 return (char *)memcpy(Target,Data,sizeof(Data));
}

 /*--------------------------------------------------------------------------*\
 |                              CFRecList Class                               |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |                        Ctor                         |
        \*---------------------------------------------------*/
CFRecList::CFRecList(unsigned lrecl, unsigned init, unsigned exp, comp_fn_tp f)
          :PtrRecArr(lrecl,init,exp,f)
{}

    /*-----------------------------------------------------------------------*\
    |    CFRec * element(unsigned i)                                          |
    |       Return the i-th ( 0 based ) array element.                        |
    \*-----------------------------------------------------------------------*/
CFRec * CFRecList::element(unsigned i)
{
 return (CFRec *)(PtrArray::Element(i));
}

    /*-----------------------------------------------------------------------*\
    |    void Release()                                                       |
    |       call 'delete' for all array elements.                             |
    \*-----------------------------------------------------------------------*/
void CFRecList::Release()
{
 for (int i = 0 , N = NumberElements(); i < N ;i++ )
   delete element(i);
}

    /*-----------------------------------------------------------------------*\
    |    CFRec * FindRecord(unsigned uRecNumber)                              |
    |       return the array element with the given record number.            |
    \*-----------------------------------------------------------------------*/
CFRec * CFRecList::FindRecord(unsigned uRecNumber)
{
 for (int i = 0, N = NumberElements() ; i < N ; i++ )
   if ( element(i)->number() == uRecNumber)
     return element(i);
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |    int CFRecList::Write(FILE *f)                                        |
    |       write all records in the array as strings at the                  |
    |       current position of the given file, returning                     |
    |       the number of lines written.                                      |
    \*-----------------------------------------------------------------------*/
int CFRecList::Write(FILE *f)
{
 Boolean fOK = true;
 for (int i = 0, N = NumberElements() ; fOK && i < N ;i++ )
   fOK = element(i)->Write(f);
 return (fOK ? N : -1);
}

    /*-----------------------------------------------------------------------*\
    |    int LoadFile(FILE * f, unsigned count)                               |
    |       Read the given file at its current position                       |
    |       adding the given number of fixed length                           |
    |       malloced records to the array.                                    |
    \*-----------------------------------------------------------------------*/
int CFRecList::LoadFile(FILE * f, unsigned count)
{
 void *pv;
 for (int i = 0, rc = PARR_OK ; rc == PARR_OK && i < count; i++)
   if (pv = malloc(LRecl()))
     if (fread(pv,LRecl(),1,f) != 1)
       rc = PARR_READ;
     else Add(pv);
   else rc = PARR_NO_MEM;
 return rc;
}

    /*-----------------------------------------------------------------------*\
    |    int LoadFile(char *filename)                                         |
    |       Open and read the given binary file ( only                        |
    |       if its size is a multiple of the record length) and               |
    |       add the number of records it contains                             |
    |       to the array.                                                     |
    \*-----------------------------------------------------------------------*/
int CFRecList::LoadFile(char *filename)
{
 FILE *f;
 if (!(f = fopen(filename,"rb")))
   return PARR_NO_FILE;
 int i = _filelength(_fileno(f));
 int rc = !(i%LRecl())?  LoadFile(f, i/LRecl()) : PARR_LRECL;
 fclose(f);
 return rc;
}

 /*--------------------------------------------------------------------------*\
 |                              Placement Class                               |
 \*--------------------------------------------------------------------------*/
    /*-------------------------------------------------------*\
    |                       Assignment                        |
    \*-------------------------------------------------------*/
Placement &  Placement::operator=(const Placement & other)
{
 if (this != &other) {
   *(CFRec *)this = *(CFRec *)&other;
   strcpy(InstId,other.InstId);
   }
 return *this;
}

 /*--------------------------------------------------------------------------*\
 |                            PlacementList Class                             |
 \*--------------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------*\
    |       'C' Placement compare function called by                          |
    |       class' 'Find' method.                                             |
    \*-----------------------------------------------------------------------*/
int Placement_comp(const void *p1,const void * p2)
{
 return ((Placement *)p1)->compare(*(Placement *)p2);
}

        /*---------------------------------------------------*\
        |                        Ctor                         |
        \*---------------------------------------------------*/
PlacementList::PlacementList()
         : CFRecList(sizeof(Placement),10,3,Placement_comp)
{}

    /*-----------------------------------------------------------------------*\
    |   Placement * Find(const char * instId)                                 |
    |       find the object in the array via its given                        |
    |       Instance Id and return its pointer.                               |
    \*-----------------------------------------------------------------------*/
Placement * PlacementList::Find(const char * instId)
{
 Placement P(instId);
 return Find(P);
}

    /*-----------------------------------------------------------------------*\
    |   Placement * Find(Placement & P)                                       |
    |       find the given object in the array and return its                 |
    |       pointer.                                                          |
    \*-----------------------------------------------------------------------*/
Placement * PlacementList::Find(Placement & P)
{
 return (Placement *)PtrRecArr::Find((const void *)&P);
}

      /*---------------------------------------------------------------------*\
      |    Placement * element(unsigned i)                                    |
      |       Return the i-th ( 0 based ) array element.                      |
      \*---------------------------------------------------------------------*/
Placement * PlacementList::element(unsigned i)
{
 return (Placement *)(PtrArray::Element(i));
}

    /*-----------------------------------------------------------------------*\
    |    void Release()                                                       |
    |       call 'delete' for all array elements.                             |
    \*-----------------------------------------------------------------------*/
void PlacementList::Release()
{
 for (int i = 0, N = NumberElements(); i < N ;i++ )
   delete element(i);
 EmptyOut();
}

 /*--------------------------------------------------------------------------*\
 |                         Non-Compiled testing code                          |
 \*--------------------------------------------------------------------------*/
#ifdef UNITTEST

//#include "euoutil.cpp"
#include <stdlib.h>
#include <iostream.h>

int  Out(char *sz,CFRec &rec)
{
 cout << sz
      << "Number : "
      << rec.number()
      << "  Type : "
      << rec.type()
      << "  Data : "
      << rec.data()
      << endl;
 return 0;
}

int  CFRecOut(void * p)
{
 CFRec Rec = *(CFRec *)p;
 cout << "Number : "
      << Rec.number()
      << "  Type : "
      << Rec.type()
      << "  Data : "
      << Rec.data()
      << endl;
 return 0;
}

void main(void)
{
 CFRec rec1("5411111111111112345");
 CFRec rec2("2522222222222222245",1);
 CFRec rec3("0033333333333333345",2);
 CFRec rec4(rec1);
 CFRec rec5;
 if (1)
   {
    CFRecList CFRL;
    Out("rec1 ",rec1);
    Out("rec2 ",rec2);
    Out("rec3 ",rec3);
    Out("rec4 ",rec4);
    Out("rec5 ",rec5);

    cout << endl << "rec5 = rec2 : " <<endl;
    rec5 = rec2;
    Out("rec5 ",rec5);

    cout << endl << "List: : " <<endl;
    CFRL.Add(&rec1);
    CFRL.Add(&rec2);
    CFRL.Add(&rec3);
    CFRL.Add(&rec4);
    CFRL.Add(&rec5);
    CFRL.CWalk(CFRecOut);
    cout << endl;
    cout << endl;

    cout << CFRL;
   }

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif

}

#endif

#if 0
#endif



