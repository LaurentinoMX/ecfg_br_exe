 /*--------------------------------------------------------------------------*\
 |    Filename - CFR19_50.CPP                                                 |
 |      contains source for the following RM/2 class implementations:         |
 |                                                                            |
 |       F7Q       - To manipulate a CFRecord's feature (of 7 bytes) and      |
 |                   quantity combination.                                    |
 |       F7QA      - To manipulate a CFRecord's feature (of 7 bytes),         |
 |                   quantity and Action combination.                         |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                     C Includes                      |
        \*---------------------------------------------------*/
#include <isynonym.hpp>                          // Definition of Boolean etc
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

        /*---------------------------------------------------*\
        |               RM/2-specific includes                |
        \*---------------------------------------------------*/
#include "cfrfq.hpp"

 /*--------------------------------------------------------------------------*\
 |                                 F7Q Class                                  |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |                        Ctors                        |
        \*---------------------------------------------------*/
F7Q::F7Q(const char * szFeat, unsigned q)
{
 setFeature(szFeat);
 Qty = q;
}

F7Q::F7Q( const Feat44 * p)
{
 Qty = myantou(p->Qty,sizeof(p->Qty));
 setFeature(p->Feature);
}

F7Q::F7Q( const Feat74 * p)
{
 Qty = myantou(p->Qty,sizeof(p->Qty));
 setFeature(p->Feature);
}

F7Q::F7Q( const Feat75 * p)
{
 Qty = myantou(p->Qty,sizeof(p->Qty));
 setFeature(p->Feature);
}

F7Q::F7Q( const Feat78 * p)
{
 Qty = myantou(p->Qty,sizeof(p->Qty));
 setFeature(p->Feature);
}

void F7Q::setFeature(const char * szFrom)
{ mystrstrip(mystrncpy(szFeature,szFrom,sizeof(szFeature)-1)); }

    /*-------------------------------------------------------*\
    |                       Assignment                        |
    \*-------------------------------------------------------*/
F7Q & F7Q::operator=(F7Q const& other)
{
 if (this != &other) {
   strcpy(szFeature,other.szFeature);
   Qty = other.Qty;
   }
 return *this;
}

F7Q & F7Q::paddCopyQuantity( char * Buf, unsigned BufLen)
{
 char szQty[10];
 if (Buf && BufLen)
   CFRPaddCopySZ(Buf,_itoa(Qty,szQty,10),BufLen);
 return *this;
}

F7Q & F7Q::paddCopyFeature( char * Buf, unsigned BufLen)
{
 if (Buf && BufLen)
   CFRPaddCopySZ(Buf,szFeature,BufLen);
 return *this;
}

F7Q & F7Q::copyFeaturePadd( char * Buf, unsigned BufLen)
{
 if (Buf && BufLen)
   CFRCopySZPadd(Buf,szFeature,BufLen);
 return *this;
}

Feat44 * F7Q::copyTo(Feat44 * p)
{
 paddCopyQuantity(p->Qty,sizeof(p->Qty));
 CFRPaddCopySZ(p->Feature,szFeature,sizeof(p->Feature));
 return p;
}

Feat74 * F7Q::copyTo(Feat74 * p)
{
 paddCopyQuantity(p->Qty,sizeof(p->Qty));
 paddCopyFeature(p->Feature,sizeof(p->Feature));
 return p;
}

Feat75 * F7Q::copyTo(Feat75 * p)
{
 paddCopyQuantity(p->Qty,sizeof(p->Qty));
 copyFeaturePadd(p->Feature,sizeof(p->Feature));
 return p;
}

Feat78 * F7Q::copyTo(Feat78 * p)
{
 paddCopyQuantity(p->Qty,sizeof(p->Qty));
 paddCopyFeature(p->Feature,sizeof(p->Feature));
 return p;
}

/////////////////////////////////////////////

 /*--------------------------------------------------------------------------*\
 |                                 F7QA Class                                 |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |                        Ctors                        |
        \*---------------------------------------------------*/

F7QA::F7QA(const char * szFeat, unsigned q, char a)
     :F7Q(szFeat,q)
{
 setAction(a);
}

F7QA::F7QA( const FeatR19 * p)
     :F7Q(p->Feature,myantou(p->Qty,sizeof(p->Qty)))
{ Action = p->Action; }

F7QA::F7QA( const Feat751 * p)
     :F7Q(p->Feature,myantou(p->Qty,sizeof(p->Qty)))
{ Action = p->Action; }

F7QA::F7QA( const FeatR50 * p)
     :F7Q(p->Feature,myantou(p->Qty,sizeof(p->Qty)))
{ Action = p->Action; }

/* F86750  Begin logic: Added definitions for the Feature-Qty-Action RT 38 information */
F7QA::F7QA( const FeatR38 * p)
     :F7Q(p->Feature,myantou(p->Qty,sizeof(p->Qty)))
{ Action = p->Action; }
/* F86750  End logic: HECC: 01/13/2004 */

F7QA & F7QA::operator=(F7QA const& other)
{
 if (this != &other) {
   *(F7Q *)this = *(F7Q *)&other;
   Action = other.Action;
   }
 return *this;
}

    /*-----------------------------------------------------------------------*\
    |    FeatR19 * copyTo(FeatR19 * pRec)                                     |
    |       Copy the feature and quantity to the given 19 Record's            |
    |       feature record                                                    |
    \*-----------------------------------------------------------------------*/
FeatR19 * F7QA::copyTo(FeatR19 * pRec)
{
 paddCopyFeature(pRec->Feature,sizeof(pRec->Feature));
 paddCopyQuantity(pRec->Qty,sizeof(pRec->Qty));
 pRec->Action = Action;
 pRec->Filler[0] = pRec->Filler[1] = '0';
 memset(&(pRec->Filler[2]),' ',sizeof(pRec->Filler)-2);
 return pRec;
}

    /*-----------------------------------------------------------------------*\
    |    FeatR50 * copyTo(FeatR50 * pRec)                                     |
    |       Copy the feature and quantity to the given 50 Record's            |
    |       feature record                                                    |
    \*-----------------------------------------------------------------------*/
FeatR50 * F7QA::copyTo(FeatR50 * pRec)
{
 paddCopyFeature(pRec->Feature,sizeof(pRec->Feature));
 paddCopyQuantity(pRec->Qty,sizeof(pRec->Qty));
 pRec->Action = Action;
 pRec->Filler[0] = '0';
 memset(&(pRec->Filler[1]),' ',sizeof(pRec->Filler)-1);
 return pRec;
}

Feat751 * F7QA::copyTo(Feat751 * pRec)
{
 copyFeaturePadd(pRec->Feature,sizeof(pRec->Feature));
 paddCopyQuantity(pRec->Qty,sizeof(pRec->Qty));
 pRec->Action = Action;
 return pRec;
}

    /*-----------------------------------------------------------------------*\
    |    FeatR38 * copyTo(FeatR38 * pRec)                                     |
    |       Copy the feature and quantity to the given 38 Record's            |
    |       feature record                                                    |
    |    F86750  HECC: 01/13/2004                                             |
    \*-----------------------------------------------------------------------*/
FeatR38 * F7QA::copyTo(FeatR38 * pRec)
{
 paddCopyFeature(pRec->Feature,sizeof(pRec->Feature));
 paddCopyQuantity(pRec->Qty,sizeof(pRec->Qty));
 pRec->Action = Action;
 return pRec;
}


///////////////////////////////////////
#ifdef UNITTEST

//#include "euoutil.cpp"
#include <stdlib.h>
#include <iostream.h>


void main(void)
{
 if (1)
   {


   }

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif

}

#endif
