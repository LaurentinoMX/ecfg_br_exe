 /*--------------------------------------------------------------------------*\
 |    Filename - CFRV.CPP                                                     |
 |      contains source for the following RM/2 class implementations:         |
 |                                                                            |
 |          CFR10  - To manipulate the CFReport's Record Type 10              |
 |                                                                            |
 |          CFR10List - An array CFR10 record pointers.                       |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                      Includes                       |
        \*---------------------------------------------------*/
#include <isynonym.hpp>                          // Definition of Boolean etc
#include <string.h>
#include <stdio.h>

        /*---------------------------------------------------*\
        |               RM/2-specific includes                |
        \*---------------------------------------------------*/
#include "cfr10.hpp"

 /*--------------------------------------------------------------------------*\
 |                                CFR07 Class                                 |
 \*--------------------------------------------------------------------------*/
unsigned CFR07::referenceNotesNumber(const char * Buf)
{
 Record_07 * p7 = (Record_07 *)Buf;
 if (p7 && (CFRecType(Buf)==7) &&
     (strlen(Buf)>=(offsetof(Record_07,RefNotesNmbr)+sizeof(p7->RefNotesNmbr))))
   return cfrUnsigned(p7->RefNotesNmbr,sizeof(p7->RefNotesNmbr));
 return 0;
}

IString CFR07::description(const char * Buf)
{
 IString str;
 Record_07 * p7 = (Record_07 *)Buf;
 if (p7 && (CFRecType(Buf)==7) &&
     (strlen(Buf) >= (offsetof(Record_07,Description)+1)))
   str = IString::stripBlanks(IString(p7->Description,sizeof(p7->Description),0));
 return str;
}

 /*--------------------------------------------------------------------------*\
 |                                CFR10 Class                                 |
 \*--------------------------------------------------------------------------*/
unsigned CFR10::referenceNotesNumber(CFRecV * pRec)
{
 Record_10 * p10;
 if (pRec && (pRec->type()==10) && (p10 = (Record_10 *)(pRec->record())) &&
     (strlen((char *)p10) >= (offsetof(Record_10,RefNotesNmbr)+sizeof(p10->RefNotesNmbr))))
   return cfrUnsigned(p10->RefNotesNmbr,sizeof(p10->RefNotesNmbr));
 return 0;
}

IString CFR10::description(CFRecV * pRec)
{
 IString str;
 Record_10 * p10;
 if (pRec && (pRec->type()==10) && (p10 = (Record_10 *)(pRec->record())) &&
     (strlen((char *)p10) >= (offsetof(Record_10,Description)+1)))
   str = IString::stripBlanks(IString(p10->Description,sizeof(p10->Description),0));
 return str;
}

 /*--------------------------------------------------------------------------*\
 |                              CFR10List Class                               |
 \*--------------------------------------------------------------------------*/
Boolean CFR10List :: add(int id, const char * Buffer)
{
 if (Buffer && (strlen(Buffer) > (offsetof(Record_10,Description)+1))) {
   CFRecV * p = new CFRecV(offsetof(Record_10,Filler),Buffer,' ');
   p->setId(id);
   Add(p);
   return true;
   }
 return false;
}

IString CFR10List::description(int id)
{
 return CFR10::description(find(id));
}

unsigned CFR10List::referenceNotesNumber(int id)
{
 return CFR10::referenceNotesNumber(find(id));
}

unsigned CFR10List::fWriteCFR(FILE *f,int id)
{
 CFRecV * pRec = find(id);
 return (pRec && pRec->fWriteCFR(f))? 1 : 0;
}
//////////////////////////////////////////////////////////////

#ifdef UNITTEST

#include <stdlib.h>
#include <iostream.h>

char *p1  = "10123description 1";
char *p2  = "10223description 2";
char *p3  = "10323description 3";
char *p4  = "10423description 4";
char *p5  = "10523description 5";
char *p6  = "10623description 6";


void main(void)
{
 if (1)
   {
    CFR10List L10;
    L10.add(1,p1);
    L10.add(2,p2);
    L10.add(3,p3);
    L10.add(4,p4);
    L10.add(5,p5);
    L10.add(6,p6);

    cout <<  "L10 : " << L10 << endl;
    cout <<  "L10.description(3) : " << L10.description(3) << endl;
    cout <<  "L10.referenceNotesNumber(3) : " << L10.referenceNotesNumber(3) << endl;
   }

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif

}

#endif


//////////////////////////////////////////////////////////////
