 /*--------------------------------------------------------------------------*\
 |    Filename - CFRSW.CPP    (CFReport Software objects)                     |
 |      contains source for the following RM/2 class implementations:         |
 |                                                                            |
 |       CFR47     - To access the CFReport's Record Type 47                  |
 |                   as defined in 'euopdef.h'                                |
 |       CFR49     - To access the CFReport's Record Type 49                  |
 |                   as defined in 'euopdef.h'                                |
 |       CFR35     - To convert CFReport's Record Types 47 and 49             |
 |                   to type 35 as defined in 'euopdef.h'                     |
 |       CFR17     - To convert CFReport's Record Types 47 and 49             |
 |                   to type 17 as defined in 'euopdef.h'                     |
 |       SoftMach  - RM2  Object representing                                 |
 |                   CFReport's SoftWare Machine (Record 08) entries          |
 |     SoftMachList- A collection (Sorted Set) of SoftMach's                  |
 \*--------------------------------------------------------------------------*/
// Modification Log:
// F73732   EDW   07/25/02 Completion of F72273 (SW reconciliation 
//                         support for iSeries)
//
        /*---------------------------------------------------*\
        |                      Includes                       |
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
#include "cfrsw.hpp"

 /*--------------------------------------------------------------------------*\
 |                                CFR35 Class                                 |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |    Ctors ( used for conversion to a 35 Record )     |
        \*---------------------------------------------------*/
CFR35::CFR35(Record_49 * p49)
{
 if (p49) {
  int i = strlen((char *)p49) - offsetof(Record_49,Feats);
  int qtyFeat =  (i > 0) ? i/sizeof(Feat74) : 0;
  i = offsetof(Record_35,Feats);
  CFRecV tmp(i + (qtyFeat * sizeof(Feat44)),(const char *)p49);
  tmp.setType(35);
  Record_35 * p35 = (Record_35 *)tmp.record();
  for (i = 0 ; i < qtyFeat ; i++) {
    memcpy(p35->Feats[i].Feature,p49->Feats[i].Feature,4);
    memcpy(p35->Feats[i].Qty,p49->Feats[i].Qty,4);
    }
  *(CFRecV *)this = tmp;
  }
}

CFR35::CFR35(Record_47 * p47)
{
 if (p47) {
  int i = strlen((char *)p47) - offsetof(Record_47,Feats);
  int qtyFeat =  (i > 0) ? i/sizeof(Feat74) : 0;
  i = offsetof(Record_35,Feats);
  CFRecV tmp(i + (qtyFeat * sizeof(Feat44)));
  tmp.setType(35);
  Record_35 * p35 = (Record_35 *)tmp.record();
  memcpy(p35->Type,p47->Type,sizeof(p35->Type));
  memcpy(p35->Model,p47->Model,sizeof(p35->Model));
  memcpy(p35->Qty,p47->Qty,sizeof(p35->Qty));
  for (i = 0 ; i < qtyFeat ; i++) {
    memcpy(p35->Feats[i].Feature,p47->Feats[i].Feature,4);
    memcpy(p35->Feats[i].Qty,p47->Feats[i].Qty,4);
    }
  *(CFRecV *)this = tmp;
  }
}

    /*-------------------------------------------------------*\
    |                       Assignment                        |
    \*-------------------------------------------------------*/
CFR35 & CFR35::operator=(CFR35 const& other)
{ if (&other != this)
    *(CFRecV *)this = *(CFRecV *)&other;
 return *this;
}

 /*--------------------------------------------------------------------------*\
 |                                CFR17 Class                                 |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |    Ctors ( used for conversion to a 17 Record )     |
        \*---------------------------------------------------*/
CFR17::CFR17(Record_47 * p47)
{
 if (p47) {
  int i = strlen((char *)p47) - offsetof(Record_47,Feats);
  int qtyFeat =  (i > 0) ? i/sizeof(Feat74) : 0;
  i = offsetof(Record_17,Feats);
  CFRecV tmp(i + (qtyFeat * sizeof(Feat44)),(const char *)p47);
  tmp.setType(17);
  Record_17 * p17 = (Record_17 *)tmp.record();
  for (i = 0 ; i < qtyFeat ; i++) {
      CFRCopySZPadd(p17->Feats[i].Feature,
                    (char *)IString::stripBlanks(IString(p47->Feats[i].Feature,sizeof(p47->Feats[i].Feature),'\0')),
                    sizeof(p17->Feats[i].Feature));
      CFRPaddCopySZ(p17->Feats[i].Qty,
                    (char *)IString::stripBlanks(IString(p47->Feats[i].Qty,sizeof(p47->Feats[i].Qty),'\0')),
                    sizeof(p17->Feats[i].Qty));
//    memcpy(p17->Feats[i].Feature,p47->Feats[i].Feature,4);
//    memcpy(p17->Feats[i].Qty,p47->Feats[i].Qty,4);
    }
  *(CFRecV *)this = tmp;
  }
}

CFR17::CFR17(Record_49 * p49)
{
 if (p49) {
  int i = strlen((char *)p49) - offsetof(Record_49,Feats);
  int qtyFeat =  (i > 0) ? i/sizeof(Feat74) : 0;
  i = offsetof(Record_17,Feats);
  CFRecV tmp(i + (qtyFeat * sizeof(Feat44)));
  tmp.setType(17);
  Record_17 * p17 = (Record_17 *)tmp.record();
  memcpy(p17->Type,p49->Type,sizeof(p17->Type));
  memcpy(p17->Model,p49->Model,sizeof(p17->Model));
  memcpy(p17->Qty,p49->Qty,sizeof(p17->Qty));
  for (i = 0 ; i < qtyFeat ; i++) {
    CFRCopySZPadd(p17->Feats[i].Feature,
                  (char *)IString::stripBlanks(IString(p49->Feats[i].Feature,sizeof(p49->Feats[i].Feature),'\0')),
                  sizeof(p17->Feats[i].Feature));
      CFRPaddCopySZ(p17->Feats[i].Qty,
                    (char *)IString::stripBlanks(IString(p49->Feats[i].Qty,sizeof(p49->Feats[i].Qty),'\0')),
                    sizeof(p17->Feats[i].Qty));
//    memcpy(p17->Feats[i].Feature,p49->Feats[i].Feature,4);
//    memcpy(p17->Feats[i].Qty,p49->Feats[i].Qty,4);
    }
  *(CFRecV *)this = tmp;
  }
}

    /*-------------------------------------------------------*\
    |                       Assignment                        |
    \*-------------------------------------------------------*/
CFR17 & CFR17::operator=(CFR17 const& other)
{ if (&other != this)
    *(CFRecV *)this = *(CFRecV *)&other;
 return *this;
}
/* F73732 EDW 07/25/02 - begin */
    /*-----------------------------------------------------------------------*\
    |    unsigned numberFeat(CFRecV & rec)                                    |
    |       Return the number of different features in the 17 Record.         |
    \*-----------------------------------------------------------------------*/
unsigned CFR17::numberFeat(CFRecV &rec)
{
 int FeatBytes = rec.length() - offsetof(Record_17,Feats);
 return (FeatBytes > 0) ? FeatBytes/sizeof(Feat44) : 0;
}

unsigned CFR17::numberFeat(Record_17 * p17)
{
 if (!p17)
  return 0;
 int FeatBytes = strlen((char *)p17) - offsetof(Record_17,Feats);
 return (FeatBytes > 0) ? FeatBytes/sizeof(Feat44) : 0;
}

    /*-----------------------------------------------------------------------*\
    |    int featIndex(CFRecV & rec,const char * szFeature)                   |
    |       Return the 0-based index of the given feature in                  |
    |       the 17 Record or -1 when not found.                               |
    \*-----------------------------------------------------------------------*/
int CFR17::featIndex(CFRecV & rec,const char * szFeature)
{
 Record_17 * p17 = (Record_17 *)rec.record();
 for ( int i = 0, N = numberFeat(rec) ; i < N ; i ++) {
   F7Q fq(&(p17->Feats[i]));
   if (!strcmp(szFeature,fq.feature()))
     return i;
   }
 return -1;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean containsFeat(CFRecV & rec,const char * szFeature)            |
    |       Indicate whether the given feature exists in                      |
    |       the 17 Record.                                                    |
    \*-----------------------------------------------------------------------*/
Boolean CFR17::containsFeat(CFRecV & rec, const char * szFeature)
{
 return (featIndex(rec,szFeature) >= 0);
}

    /*-----------------------------------------------------------------------*\
    |    Feat44 * featPtr(CFRecV & rec,const char * szFeature)                |
    |       Return the pointer to the 17 Record's feature record              |
    |       for the given feature name.                                       |
    \*-----------------------------------------------------------------------*/
Feat44 * CFR17::featPtr(CFRecV & rec,const char * szFeature)
{
 Record_17 * p17 = (Record_17 *)rec.record();
 for ( int i = 0, N = numberFeat(rec) ; i < N ; i ++) {
   F7Q fq(&(p17->Feats[i]));
   if (!strcmp(szFeature,fq.feature()))
     return &(p17->Feats[i]);
   }
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |    Feat44 * featPtr(CFRecV & rec,unsigned index)                        |
    |       Return the pointer to the 17 Record's feature record              |
    |       for the given feature index.                                      |
    \*-----------------------------------------------------------------------*/
Feat44 * CFR17::featPtr(CFRecV & rec,unsigned index)
{
 return (index < numberFeat(rec))?
           &(((Record_17 *)rec.record())->Feats[index]) : 0;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean featQty(CFRecV & rec, const char * szFeature,                |
    |                    unsigned * pu)                                       |
    |       Set the quantity of the given feature in the given                |
    |       47 Record indicating succes/failure.                              |
    \*-----------------------------------------------------------------------*/
Boolean CFR17::featQty(CFRecV & rec, const char * szFeature, unsigned * pu)
{
 Feat44 * pF44 = featPtr(rec,szFeature);
 if (pF44) {
   F7Q fq(pF44);
   *pu = fq.quantity();
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean featParts(CFRecV & rec,const char * szFeature,                |
    |                     char * Target, unsigned * pu )                      |
    |       Copy the name and quantity of the given feature                   |
    |       in the given 17 Record indicating succes/failure.                 |
    \*-----------------------------------------------------------------------*/
Boolean CFR17::featParts(CFRecV & rec,const char * szFeature,
                         char * Target, unsigned * pu )
{
 Feat44 * pF44 = featPtr(rec,szFeature);
 if (pF44) {
   F7Q fq(pF44);
   *pu = fq.quantity();
   strcpy(Target,fq.feature());
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean featParts(CFRecV &rec , unsigned Index, char * Target,       |
    |                      unsigned * pu )                                    |
    |       Copy the feature name and quantity of the 'Index'-th              |
    |       element of the feature array                                      |
    |       in the given 17 Record indicating succes/failure.                 |
    \*-----------------------------------------------------------------------*/
Boolean CFR17::featParts(CFRecV &rec , unsigned Index, char * Target,
                         unsigned * pu )
{
 Feat44 * pF44 = featPtr(rec,Index);
 if (pF44) {
   F7Q fq(pF44);
   *pu = fq.quantity();
   strcpy(Target,fq.feature());
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setFeatParts(CFRecV & rec, unsigned Index,unsigned qty,      |
    |                         char *szFeature)                                |
    |       Set the given feature name and quantity of the 'Index'-th         |
    |       element of the feature array                                      |
    |       in the given 17 Record indicating succes/failure.                 |
    |       Note: failure is signaled for 17 Records which                    |
    |             have fewer features than the given Index.                   |
    \*-----------------------------------------------------------------------*/
Boolean CFR17::setFeatParts(CFRecV & rec, unsigned Index,unsigned qty,
                            char *szFeature)
{
 Feat44 * pF44 = featPtr(rec,Index);
 if (pF44) {
   F7Q fq(pF44);
   fq.setQuantity(qty);
   if (szFeature)
     fq.setFeature(szFeature);
   fq.copyTo(pF44);
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setFeatQty(CFRecV & rec,const char * szFeature,              |
    |                       unsigned qty )                                    |
    |       Set the given quantity of the given feature                       |
    |       in the given 17 Record indicating succes/failure.                 |
    |       Note: failure is signaled when the 47 Record                      |
    |             does not contain the given feature.                         |
    \*-----------------------------------------------------------------------*/
Boolean CFR17::setFeatQty(CFRecV & rec,const char * szFeature,unsigned qty )
{
 Feat44 * pF44 = featPtr(rec,szFeature);
 if (pF44) {
   F7Q fq(pF44);
   fq.setQuantity(qty);
   fq.copyTo(pF44);
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean stripOffFeat(CFRecV & rec,unsigned index)                    |
    |       Truncate the given 17 Record at the given feature index.          |
    |       i.e. if given n, the feature will be array element n-1.           |
    |       Note: This causes a replacement of the underlying                 |
    |             buffer which will unable to hold additional                 |
    |             feature elements.                                           |
    \*-----------------------------------------------------------------------*/
Boolean CFR17::stripOffFeat(CFRecV & rec,unsigned index)
{
 CFRecV temp(rec);
 Feat44 * pF44 = featPtr(temp,index);
 if (pF44) {
   *(char *)pF44 = '\0';
   rec = temp;
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    void appendFeat(CFRecV & rec, const char *szFeature,                 |
    |                    unsigned qty)                                        |
    |       Expand the 17 Record's feature array with an element              |
    |       for the given feature name and quantity.                          |
    |       Note: This causes a replacement of the underlying                 |
    |             buffer.                                                     |
    \*-----------------------------------------------------------------------*/
void CFR17::appendFeat(CFRecV & rec, const char *szFeature, unsigned qty)
{
 Feat44 f44;
 F7Q fq(szFeature,qty);
 fq.copyTo(&f44);
 CFRecV tmp(sizeof(f44),(char *)&f44);
 rec += tmp;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setModel(CFRecV & rec,const char *szModel)                   |
    |       Set the given Model right-justified in the 17 Record.             |
    \*-----------------------------------------------------------------------*/
Boolean CFR17::setModel(CFRecV & rec,const char *szModel)
{
 Record_17 * p17 = (Record_17 *)rec.record();
 if (p17 && (rec.length() >=
             (offsetof(Record_17,Model)+sizeof(p17->Model)))) {
   CFRPaddCopySZ(p17->Model,szModel,sizeof(p17->Model));
   return true;
   }
 return false;
}
/* F73732 EDW 07/25/02 - end */

 /*--------------------------------------------------------------------------*\
 |                                CFR47 Class                                 |
 \*--------------------------------------------------------------------------*/
CFR47 :: CFR47(Record_SW_V31 * pSW)
       : CFRecV(offsetof(Record_47,Feats) + (CFR_SW_V31::numberFeat(pSW) * sizeof(Feat74)))
{
 setType(47);
 if (pSW) {
   Record_47 * p47 = (Record_47 *)record();
   memcpy(p47->Type,pSW->Type,4);
   memcpy(p47->Model,pSW->Model,3);
   memcpy(p47->Qty,&(pSW->Qty[1]),4);
   for (int i = 0, I = CFR_SW_V31::numberFeat(pSW) ; i < I ; i++) {
     CFRCopySZPadd(p47->Feats[i].Feature,
                   (char *)IString::stripBlanks(IString(pSW->Feats[i].Feature,sizeof(pSW->Feats[i].Feature),'\0')),
                   sizeof(p47->Feats[i].Feature));
     CFRPaddCopySZ(p47->Feats[i].Qty,
                     (char *)IString::stripBlanks(IString(pSW->Feats[i].Qty,sizeof(pSW->Feats[i].Qty),'\0')),
                     sizeof(p47->Feats[i].Qty));
     }
   }
}

CFR47 :: CFR47(Record_17 * p17)
{
 int i = strlen((char *)p17) - offsetof(Record_17,Feats);
 int qtyFeat =  (i > 0) ? i/sizeof(Feat44) : 0;
 i = offsetof(Record_47,Feats);
 CFRecV tmp(i + (qtyFeat * sizeof(Feat74)));
 tmp.setType(47);
 Record_47 * p47 = (Record_47 *)tmp.record();
 memcpy(p47->Type,p17->Type,sizeof(p47->Type));
 memcpy(p47->Model,p17->Model,sizeof(p47->Model));
 memcpy(p47->Qty,p17->Qty,sizeof(p47->Qty));
 for (i = 0 ; i < qtyFeat ; i++) {
   CFRCopySZPadd(p47->Feats[i].Feature,
                 (char *)IString::stripBlanks(IString(p17->Feats[i].Feature,sizeof(p17->Feats[i].Feature),'\0')),
                 sizeof(p47->Feats[i].Feature));
     CFRPaddCopySZ(p47->Feats[i].Qty,
                   (char *)IString::stripBlanks(IString(p47->Feats[i].Qty,sizeof(p47->Feats[i].Qty),'\0')),
                   sizeof(p47->Feats[i].Qty));
   }
 *(CFRecV *)this = tmp;
}

CFR47 & CFR47::operator=(CFR47 const & other)
{ if (&other != this)
    *(CFRecV *)this = *(CFRecV *)&other;
 return *this;
}



    /*-----------------------------------------------------------------------*\
    |    unsigned numberFeat(CFRecV & rec)                                    |
    |       Return the number of different features in the 47 Record.         |
    \*-----------------------------------------------------------------------*/
unsigned CFR47::numberFeat(CFRecV &rec)
{
 int FeatBytes = rec.length() - offsetof(Record_47,Feats);
 return (FeatBytes > 0) ? FeatBytes/sizeof(Feat74) : 0;
}

unsigned CFR47::numberFeat(Record_47 * p47)
{
 if (!p47)
  return 0;
 int FeatBytes = strlen((char *)p47) - offsetof(Record_47,Feats);
 return (FeatBytes > 0) ? FeatBytes/sizeof(Feat74) : 0;
}

    /*-----------------------------------------------------------------------*\
    |    int featIndex(CFRecV & rec,const char * szFeature)                   |
    |       Return the 0-based index of the given feature in                  |
    |       the 47 Record or -1 when not found.                               |
    \*-----------------------------------------------------------------------*/
int CFR47::featIndex(CFRecV & rec,const char * szFeature)
{
 Record_47 * p47 = (Record_47 *)rec.record();
 for ( int i = 0, N = numberFeat(rec) ; i < N ; i ++) {
   F7Q fq(&(p47->Feats[i]));
   if (!strcmp(szFeature,fq.feature()))
     return i;
   }
 return -1;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean containsFeat(CFRecV & rec,const char * szFeature)            |
    |       Indicate whether the given feature exists in                      |
    |       the 47 Record.                                                    |
    \*-----------------------------------------------------------------------*/
Boolean CFR47::containsFeat(CFRecV & rec, const char * szFeature)
{
 return (featIndex(rec,szFeature) >= 0);
}

    /*-----------------------------------------------------------------------*\
    |    Feat74 * featPtr(CFRecV & rec,const char * szFeature)                |
    |       Return the pointer to the 47 Record's feature record              |
    |       for the given feature name.                                       |
    \*-----------------------------------------------------------------------*/
Feat74 * CFR47::featPtr(CFRecV & rec,const char * szFeature)
{
 Record_47 * p47 = (Record_47 *)rec.record();
 for ( int i = 0, N = numberFeat(rec) ; i < N ; i ++) {
   F7Q fq(&(p47->Feats[i]));
   if (!strcmp(szFeature,fq.feature()))
     return &(p47->Feats[i]);
   }
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |    Feat74 * featPtr(CFRecV & rec,unsigned index)                        |
    |       Return the pointer to the 47 Record's feature record              |
    |       for the given feature index.                                      |
    \*-----------------------------------------------------------------------*/
Feat74 * CFR47::featPtr(CFRecV & rec,unsigned index)
{
 return (index < numberFeat(rec))?
           &(((Record_47 *)rec.record())->Feats[index]) : 0;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean featQty(CFRecV & rec, const char * szFeature,                |
    |                    unsigned * pu)                                       |
    |       Set the quantity of the given feature in the given                |
    |       47 Record indicating succes/failure.                              |
    \*-----------------------------------------------------------------------*/
Boolean CFR47::featQty(CFRecV & rec, const char * szFeature, unsigned * pu)
{
 Feat74 * pF74 = featPtr(rec,szFeature);
 if (pF74) {
   F7Q fq(pF74);
   *pu = fq.quantity();
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean featParts(CFRecV & rec,const char * szFeature,                |
    |                     char * Target, unsigned * pu )                      |
    |       Copy the name and quantity of the given feature                   |
    |       in the given 47 Record indicating succes/failure.                 |
    \*-----------------------------------------------------------------------*/
Boolean CFR47::featParts(CFRecV & rec,const char * szFeature,
                         char * Target, unsigned * pu )
{
 Feat74 * pF74 = featPtr(rec,szFeature);
 if (pF74) {
   F7Q fq(pF74);
   *pu = fq.quantity();
   strcpy(Target,fq.feature());
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean featParts(CFRecV &rec , unsigned Index, char * Target,       |
    |                      unsigned * pu )                                    |
    |       Copy the feature name and quantity of the 'Index'-th              |
    |       element of the feature array                                      |
    |       in the given 47 Record indicating succes/failure.                 |
    \*-----------------------------------------------------------------------*/
Boolean CFR47::featParts(CFRecV &rec , unsigned Index, char * Target,
                         unsigned * pu )
{
 Feat74 * pF74 = featPtr(rec,Index);
 if (pF74) {
   F7Q fq(pF74);
   *pu = fq.quantity();
   strcpy(Target,fq.feature());
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setFeatParts(CFRecV & rec, unsigned Index,unsigned qty,      |
    |                         char *szFeature)                                |
    |       Set the given feature name and quantity of the 'Index'-th         |
    |       element of the feature array                                      |
    |       in the given 47 Record indicating succes/failure.                 |
    |       Note: failure is signaled for 47 Records which                    |
    |             have fewer features than the given Index.                   |
    \*-----------------------------------------------------------------------*/
Boolean CFR47::setFeatParts(CFRecV & rec, unsigned Index,unsigned qty,
                            char *szFeature)
{
 Feat74 * pF74 = featPtr(rec,Index);
 if (pF74) {
   F7Q fq(pF74);
   fq.setQuantity(qty);
   if (szFeature)
     fq.setFeature(szFeature);
   fq.copyTo(pF74);
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setFeatQty(CFRecV & rec,const char * szFeature,              |
    |                       unsigned qty )                                    |
    |       Set the given quantity of the given feature                       |
    |       in the given 47 Record indicating succes/failure.                 |
    |       Note: failure is signaled when the 47 Record                      |
    |             does not contain the given feature.                         |
    \*-----------------------------------------------------------------------*/
Boolean CFR47::setFeatQty(CFRecV & rec,const char * szFeature,unsigned qty )
{
 Feat74 * pF74 = featPtr(rec,szFeature);
 if (pF74) {
   F7Q fq(pF74);
   fq.setQuantity(qty);
   fq.copyTo(pF74);
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean stripOffFeat(CFRecV & rec,unsigned index)                    |
    |       Truncate the given 47 Record at the given feature index.          |
    |       i.e. if given n, the feature will be array element n-1.           |
    |       Note: This causes a replacement of the underlying                 |
    |             buffer which will unable to hold additional                 |
    |             feature elements.                                           |
    \*-----------------------------------------------------------------------*/
Boolean CFR47::stripOffFeat(CFRecV & rec,unsigned index)
{
 CFRecV temp(rec);
 Feat74 * pF74 = featPtr(temp,index);
 if (pF74) {
   *(char *)pF74 = '\0';
   rec = temp;
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    void appendFeat(CFRecV & rec, const char *szFeature,                 |
    |                    unsigned qty)                                        |
    |       Expand the 47 Record's feature array with an element              |
    |       for the given feature name and quantity.                          |
    |       Note: This causes a replacement of the underlying                 |
    |             buffer.                                                     |
    \*-----------------------------------------------------------------------*/
void CFR47::appendFeat(CFRecV & rec, const char *szFeature, unsigned qty)
{
 Feat74 f74;
 F7Q fq(szFeature,qty);
 fq.copyTo(&f74);
 CFRecV tmp(sizeof(f74),(char *)&f74);
 rec += tmp;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setModel(CFRecV & rec,const char *szModel)                   |
    |       Set the given Model right-justified in the 47 Record.             |
    \*-----------------------------------------------------------------------*/
Boolean CFR47::setModel(CFRecV & rec,const char *szModel)
{
 Record_47 * p47 = (Record_47 *)rec.record();
 if (p47 && (rec.length() >=
             (offsetof(Record_47,Model)+sizeof(p47->Model)))) {
   CFRPaddCopySZ(p47->Model,szModel,sizeof(p47->Model));
   return true;
   }
 return false;
}

 /*--------------------------------------------------------------------------*\
 |                                CFR49 Class                                 |
 \*--------------------------------------------------------------------------*/
CFR49 :: CFR49(Record_SW_V31 * pSW)
       : CFRecV(offsetof(Record_49,Feats) + (CFR_SW_V31::numberFeat(pSW) * sizeof(Feat74)))
{
 setType(49);
 if (pSW) {
   Record_49 * p49 = (Record_49 *)record();
   memcpy(p49->Type,pSW->Type,4);
   memcpy(p49->Model,pSW->Model,3);
   memcpy(p49->Qty,&(pSW->Qty[1]),4);
   for (int i = 0, I = CFR_SW_V31::numberFeat(pSW) ; i < I ; i++) {
     CFRCopySZPadd(p49->Feats[i].Feature,
                   (char *)IString::stripBlanks(IString(pSW->Feats[i].Feature,sizeof(pSW->Feats[i].Feature),'\0')),
                   sizeof(p49->Feats[i].Feature));
     CFRPaddCopySZ(p49->Feats[i].Qty,
                     (char *)IString::stripBlanks(IString(pSW->Feats[i].Qty,sizeof(pSW->Feats[i].Qty),'\0')),
                     sizeof(p49->Feats[i].Qty));
     }
   }
}

CFR49 & CFR49::operator=(CFR49 const & other)
{ if (&other != this)
    *(CFRecV *)this = *(CFRecV *)&other;
 return *this;
}

    /*-----------------------------------------------------------------------*\
    |    unsigned numberFeat(CFRecV & rec)                                    |
    |       Return the number of different features in the 49 Record.         |
    \*-----------------------------------------------------------------------*/
unsigned CFR49::numberFeat(CFRecV &rec)
{
 int FeatBytes = rec.length() - offsetof(Record_49,Feats);
 return (FeatBytes > 0) ? FeatBytes/sizeof(Feat74) : 0;
}

unsigned CFR49::numberFeat(Record_49 * p49)
{
 if (!p49)
   return 0;
 int FeatBytes = strlen((char *)p49) - offsetof(Record_49,Feats);
 return (FeatBytes > 0) ? FeatBytes/sizeof(Feat74) : 0;
}

    /*-----------------------------------------------------------------------*\
    |    int featIndex(CFRecV & rec,const char * szFeature)                   |
    |       Return the 0-based index of the given feature in                  |
    |       the 49 Record or -1 when not found.                               |
    \*-----------------------------------------------------------------------*/
int CFR49::featIndex(CFRecV & rec,const char * szFeature)
{
 Record_49 * p49 = (Record_49 *)rec.record();
 for ( int i = 0, N = numberFeat(rec) ; i < N ; i ++) {
   F7Q fq(&(p49->Feats[i]));
   if (!strcmp(szFeature,fq.feature()))
     return i;
   }
 return -1;
}

    /*-----------------------------------------------------------------------*\
    |    Feat74 * featPtr(CFRecV & rec,const char * szFeature)                |
    |       Return the pointer to the 49 Record's feature record              |
    |       for the given feature name.                                       |
    \*-----------------------------------------------------------------------*/
Feat74 * CFR49::featPtr(CFRecV & rec,const char * szFeature)
{
 Record_49 * p49 = (Record_49 *)rec.record();
 for ( int i = 0, N = numberFeat(rec) ; i < N ; i ++) {
   F7Q fq(&(p49->Feats[i]));
   if (!strcmp(szFeature,fq.feature()))
     return &(p49->Feats[i]);
   }
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |    Feat74 * featPtr(CFRecV & rec,unsigned index)                        |
    |       Return the pointer to the 49 Record's feature record              |
    |       for the given feature index.                                      |
    \*-----------------------------------------------------------------------*/
Feat74 * CFR49::featPtr(CFRecV & rec,unsigned index)
{
 return (index < numberFeat(rec))?
           &(((Record_49 *)rec.record())->Feats[index]) : 0;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean containsFeat(CFRecV & rec,const char * szFeature)            |
    |       Indicate whether the given feature exists in                      |
    |       the 49 Record.                                                    |
    \*-----------------------------------------------------------------------*/
Boolean CFR49::containsFeat(CFRecV & rec, const char * szFeature)
{
 return (featIndex(rec,szFeature) >= 0);
}

    /*-----------------------------------------------------------------------*\
    |    Boolean featQty(CFRecV & rec, const char * szFeature,                |
    |                    unsigned * pu)                                       |
    |       Set the quantity of the given feature in the given                |
    |       49 Record indicating succes/failure.                              |
    \*-----------------------------------------------------------------------*/
Boolean CFR49::featQty(CFRecV & rec, const char * szFeature, unsigned * pu)
{
 Feat74 * pF74 = featPtr(rec,szFeature);
 if (pF74) {
   F7Q fq(pF74);
   *pu = fq.quantity();
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean featParts(CFRecV & rec,const char * szFeature,                |
    |                     char * Target, unsigned * pu )                      |
    |       Copy the name and quantity of the given feature                   |
    |       in the given 49 Record indicating succes/failure.                 |
    \*-----------------------------------------------------------------------*/
Boolean CFR49::featParts(CFRecV & rec,const char * szFeature,
                         char * Target, unsigned * pu )
{
 Feat74 * pF74 = featPtr(rec,szFeature);
 if (pF74) {
   F7Q fq(pF74);
   *pu = fq.quantity();
   strcpy(Target,fq.feature());
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean featParts(CFRecV &rec , unsigned Index, char * Target,       |
    |                      unsigned * pu )                                    |
    |       Copy the feature name and quantity of the 'Index'-th              |
    |       element of the feature array                                      |
    |       in the given 49 Record indicating succes/failure.                 |
    \*-----------------------------------------------------------------------*/
Boolean CFR49::featParts(CFRecV &rec , unsigned Index, char * Target,
                         unsigned * pu )
{
 Feat74 * pF74 = featPtr(rec,Index);
 if (pF74) {
   F7Q fq(pF74);
   *pu = fq.quantity();
   strcpy(Target,fq.feature());
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setFeatParts(CFRecV & rec, unsigned Index,unsigned qty,      |
    |                         char *szFeature)                                |
    |       Set the given feature name and quantity of the 'Index'-th         |
    |       element of the feature array                                      |
    |       in the given 49 Record indicating succes/failure.                 |
    |       Note: failure is signaled for 49 Records which                    |
    |             have fewer features than the given Index.                   |
    \*-----------------------------------------------------------------------*/
Boolean CFR49::setFeatParts(CFRecV & rec, unsigned Index,unsigned qty,
                            char *szFeature)
{
 Feat74 * pF74 = featPtr(rec,Index);
 if (pF74) {
   F7Q fq(pF74);
   fq.setQuantity(qty);
   if (szFeature)
     fq.setFeature(szFeature);
   fq.copyTo(pF74);
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setFeatQty(CFRecV & rec,const char * szFeature,              |
    |                       unsigned qty )                                    |
    |       Set the given quantity of the given feature                       |
    |       in the given 49 Record indicating succes/failure.                 |
    |       Note: failure is signaled when the 49 Record                      |
    |             does not contain the given feature.                         |
    \*-----------------------------------------------------------------------*/
Boolean CFR49::setFeatQty(CFRecV & rec,const char * szFeature,unsigned qty )
{
 Feat74 * pF74 = featPtr(rec,szFeature);
 if (pF74) {
   F7Q fq(pF74);
   fq.setQuantity(qty);
   fq.copyTo(pF74);
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean stripOffFeat(CFRecV & rec,unsigned index)                    |
    |       Truncate the given 49 Record at the given feature index.          |
    |       i.e. if given n, the feature will be array element n-1.           |
    |       Note: This causes a replacement of the underlying                 |
    |             buffer which will unable to hold additional                 |
    |             feature elements.                                           |
    \*-----------------------------------------------------------------------*/
Boolean CFR49::stripOffFeat(CFRecV & rec,unsigned index)
{
 CFRecV temp(rec);
 Feat74 * pF74 = featPtr(temp,index);
 if (pF74) {
   *(char *)pF74 = '\0';
   rec = temp;
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    void appendFeat(CFRecV & rec, const char *szFeature,                 |
    |                    unsigned qty)                                        |
    |       Expand the 49 Record's feature array with an element              |
    |       for the given feature name and quantity.                          |
    |       Note: This causes a replacement of the underlying                 |
    |             buffer.                                                     |
    \*-----------------------------------------------------------------------*/
void CFR49::appendFeat(CFRecV & rec, const char *szFeature, unsigned qty)
{
 Feat74 f74;
 F7Q fq(szFeature,qty);
 fq.copyTo(&f74);
 CFRecV tmp(sizeof(f74),(char *)&f74);
 rec += tmp;
}

    /*-----------------------------------------------------------------------*\
    |    int compare(CFRecV & rec,const Record_17 * p17)                      |
    |       Compare the given 49 Record with a 17 Record by                   |
    |       comparing their type and model.                                   |
    \*-----------------------------------------------------------------------*/
int CFR49::compare(CFRecV & rec,const Record_17 * p17)
{
 Record_49 * p49 = (Record_49 *)rec.record();
 int i = memcmp(p49->Type,p17->Type,sizeof(p49->Type));
 if (!i) i = memcmp(p49->Model,p17->Model,sizeof(p49->Model));
 return i;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setModel(CFRecV & rec,const char *szModel)                   |
    |       Set the given Model right-justified in the 49 Record.             |
    \*-----------------------------------------------------------------------*/
Boolean CFR49::setModel(CFRecV & rec,const char *szModel)
{
 Record_49 * p49 = (Record_49 *)rec.record();
 if (p49 && (rec.length() >=
             (offsetof(Record_49,Model)+sizeof(p49->Model)))) {
   CFRPaddCopySZ(p49->Model,szModel,sizeof(p49->Model));
   return true;
   }
 return false;
}

 /*--------------------------------------------------------------------------*\
 |                               SoftMach Class                               |
 \*--------------------------------------------------------------------------*/
    /*-------------------------------------------------------*\
    |                       Assignment                        |
    \*-------------------------------------------------------*/
SoftMach & SoftMach::operator=(SoftMach const& other)
{
 if (this != &other) {
   Type  = other.Type;
   Model = other.Model;
   Feature = other.Feature;
   ulDate = other.ulDate;
   Desc  = other.Desc;
   }
 return *this;
}

    /*-------------------------------------------------------*\
    |                        Equaltity                        |
    \*-------------------------------------------------------*/
Boolean SoftMach::operator==(SoftMach const& other)  const
{
 return ( (!other.Type.length()    || (Type == other.Type  ))     &&
          (!other.Model.length()   || (Model == other.Model))     &&
          (!other.Feature.length() || (Feature == other.Feature)) &&
          (ulDate == other.ulDate)                                &&
          (!other.Desc.length()    || (Desc == other.Desc)) );
}

    /*-------------------------------------------------------*\
    |                       Comparsion                        |
    \*-------------------------------------------------------*/
Boolean SoftMach::operator< (SoftMach const& other) const
{
 if (Type == other.Type)
   if (Model == other.Model)
     if (Feature == other.Feature)
       return (Desc <  other.Desc);
     else return (Feature <  other.Feature);
   else return (Model < other.Model);
 else return (Type < other.Type);
}

 /*--------------------------------------------------------------------------*\
 |                             SoftMachList Class                             |
 \*--------------------------------------------------------------------------*/

#define SOFTMACHL_SIGNATURE  121319

    /*-----------------------------------------------------------------------*\
    |    ostream& out(ostream& os)                                            |
    |       Used for testing.                                                 |
    \*-----------------------------------------------------------------------*/
ostream& SoftMachList::out(ostream& os)
{
 Cursor crsr(*this);
 forCursor(crsr)
   os << elementAt(crsr) << endl;
 return os;
}

    /*-----------------------------------------------------------------------*\
    |    unsigned fWriteBLength()                                             |
    |       Return the number of bytes the collection would                   |
    |       occupy if it was to be written to a binary file                   |
    |       by method  'fWriteB'.                                             |
    \*-----------------------------------------------------------------------*/
unsigned SoftMachList::fWriteBLength()
{
 unsigned Len = 2 * sizeof(unsigned);
 Cursor crsr(*this);
 forCursor(crsr)
   Len += ( ( 4 *  sizeof(unsigned) )                       +
            (elementAt(crsr).strType().length() + 1)        +
            (elementAt(crsr).strModel().length() + 1)       +
            (elementAt(crsr).strFeature().length() + 1)     +
            (elementAt(crsr).strDescription().length() + 1) +
            sizeof(unsigned long)
          );
 return Len;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean fWriteB(char * szFileName, unsigned * pu)                    |
    |       Write the collection in binary format to the given                |
    |       filename and report the filelength                                |
    \*-----------------------------------------------------------------------*/
Boolean SoftMachList::fWriteB(char * szFileName, unsigned * pu)
{
 FILE * f;
 Boolean fOK = ((f = fopen(szFileName,"wb")) != 0);
 if (fOK) {
   fOK = fWriteB(f,pu);
   fclose(f);
   }
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean fWriteB(FILE * file, unsigned * pu)                          |
    |       Write the collection in binary format at the                      |
    |       current position of the given file                                |
    |       and report the number of bytes written                            |
    \*-----------------------------------------------------------------------*/
Boolean SoftMachList::fWriteB(FILE * f, unsigned * pu)
{
 unsigned U2[2];
 Boolean fOK;
 U2[0] = SOFTMACHL_SIGNATURE;
 U2[1] = numberOfElements();
 unsigned long ul;
 if (fOK = (fwrite(U2,sizeof(U2),1,f) == 1)) {
   U2[0] = sizeof(U2);
   Cursor crsr(*this);
   for (crsr.setToFirst(); fOK && crsr.isValid();crsr.setToNext()) {
     ul = elementAt(crsr).dateJulian();
     if (fOK = (fWriteBIString(f,elementAt(crsr).strType(),U2)        &&
                fWriteBIString(f,elementAt(crsr).strModel(),U2)       &&
                fWriteBIString(f,elementAt(crsr).strFeature(),U2)     &&
                fWriteBIString(f,elementAt(crsr).strDescription(),U2) &&
                ((fwrite(&ul,sizeof(unsigned long),1,f) == 1))))
       U2[0] += sizeof(unsigned long);
     }
   }
 if (pu) *pu = U2[0];
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    unsigned fReadB(FILE * f)                                            |
    |       Load the collection from the given filename                       |
    |       which is to have been created by the 'fWriteB' method             |
    |       and return the result                                             |
    |       as per the codes defined in CFRSW.HPP.                            |
    \*-----------------------------------------------------------------------*/
unsigned SoftMachList::fReadB(char * szFileName)
{
 FILE * f;
 int rc = 0;
 if (f = fopen(szFileName,"rb")) {
   if ( _filelength(_fileno(f) >= (2 * sizeof(unsigned))))
     rc = fReadB(f);
   else rc = SOFTMACHL_ERR_NO_SOFTMACHL_FILE;
   fclose(f);
   }
 else rc = SOFTMACHL_ERR_NO_FILE;
 return rc;
}

    /*-----------------------------------------------------------------------*\
    |    unsigned fReadB(FILE * f)                                            |
    |       Load the collection from the current position                     |
    |       of the given binary file and return the result                    |
    |       as per the codes defined in CFRSW.HPP.                            |
    \*-----------------------------------------------------------------------*/
unsigned SoftMachList::fReadB(FILE * f)
{
 unsigned U2[2];
 IString typeStr, modStr, featStr, descStr;
 int rc = 0;
 unsigned long ul;
 if (fread(U2,sizeof(U2),1,f) == 1)
   if ( SOFTMACHL_SIGNATURE == U2[0])
     for ( int i = 0 ,I = U2[1]; !rc && (i < I) ; i++)
       if (fReadBIString(f,typeStr)       &&
           fReadBIString(f,modStr)        &&
           fReadBIString(f,featStr)       &&
           fReadBIString(f,descStr)       &&
           (fread(&ul,sizeof(unsigned long),1,f) == 1))
           this->add(SoftMach(typeStr,modStr,featStr,descStr,IDate(ul)));
       else rc = SOFTMACHL_ERR_READ_FILE;
   else rc = SOFTMACHL_ERR_NO_SOFTMACHL_FILE;
 else rc = SOFTMACHL_ERR_READ_FILE;
 return rc;
}

 /*--------------------------------------------------------------------------*\
 |                              CFR_SW_V31 Class                              |
 \*--------------------------------------------------------------------------*/


CFR_SW_V31::CFR_SW_V31(Record_47  * p47)
      : CFRecV(offsetof(Record_SW_V31,Feats) + (CFR47::numberFeat(p47) * sizeof(Feat75)))
{
 setType(47);
 if (p47) {
   Record_SW_V31 * pSW = (Record_SW_V31 *)record();
   memcpy(pSW->Type,p47->Type,4);
   memcpy(pSW->Model,p47->Model,3);
   memcpy(&(pSW->Qty[1]),p47->Qty,4);
   for (int i = 0, I = CFR47::numberFeat(p47) ; i < I ; i++) {
     CFRCopySZPadd(pSW->Feats[i].Feature,
                   (char *)IString::stripBlanks(IString(p47->Feats[i].Feature,sizeof(p47->Feats[i].Feature),'\0')),
                   sizeof(pSW->Feats[i].Feature));
     CFRPaddCopySZ(pSW->Feats[i].Qty,
                     (char *)IString::stripBlanks(IString(p47->Feats[i].Qty,sizeof(p47->Feats[i].Qty),'\0')),
                     sizeof(pSW->Feats[i].Qty));
     }
   }
}

CFR_SW_V31::CFR_SW_V31(Record_49  * p49)
      : CFRecV(offsetof(Record_SW_V31,Feats) + (CFR49::numberFeat(p49) * sizeof(Feat75)))
{
 setType(49);
 if (p49) {
   Record_SW_V31 * pSW = (Record_SW_V31 *)record();
   memcpy(pSW->Type,p49->Type,4);
   memcpy(pSW->Model,p49->Model,3);
   memcpy(&(pSW->Qty[1]),p49->Qty,4);
   for (int i = 0, I = CFR49::numberFeat(p49) ; i < I ; i++) {
     CFRCopySZPadd(pSW->Feats[i].Feature,
                   (char *)IString::stripBlanks(IString(p49->Feats[i].Feature,sizeof(p49->Feats[i].Feature),'\0')),
                   sizeof(pSW->Feats[i].Feature));
     CFRPaddCopySZ(pSW->Feats[i].Qty,
                     (char *)IString::stripBlanks(IString(p49->Feats[i].Qty,sizeof(p49->Feats[i].Qty),'\0')),
                     sizeof(pSW->Feats[i].Qty));
     }
   }
}

CFR_SW_V31 & CFR_SW_V31::operator=(CFR_SW_V31 const & other)
{ if (&other != this)
    *(CFRecV *)this = *(CFRecV *)&other;
 return *this;
}

IString CFR_SW_V31::strModel(const Record_SW_V31 * pSW)
{
 return pSW? IString::stripBlanks(IString(pSW->Model,sizeof(pSW->Model),0)) : IString();
}

IString CFR_SW_V31::strType(const Record_SW_V31  * pSW)
{
 return pSW? IString::stripBlanks(IString(pSW->Type,sizeof(pSW->Type),0)) : IString();
}

    /*-----------------------------------------------------------------------*\
    |    unsigned numberFeat(CFRecV & rec)                                    |
    |       Return the number of different features in the 47 Record.         |
    \*-----------------------------------------------------------------------*/
unsigned CFR_SW_V31::numberFeat(CFRecV &rec)
{
 int FeatBytes = rec.length() - offsetof(Record_SW_V31,Feats);
 return (FeatBytes > 0) ? FeatBytes/sizeof(Feat75) : 0;
}

unsigned CFR_SW_V31::numberFeat(Record_SW_V31 * pSW)
{
 if (!pSW)
   return 0;
 int FeatBytes = strlen((char *)pSW) - offsetof(Record_SW_V31,Feats);
 return (FeatBytes > 0) ? FeatBytes/sizeof(Feat75) : 0;
}

    /*-----------------------------------------------------------------------*\
    |    int featIndex(CFRecV & rec,const char * szFeature)                   |
    |       Return the 0-based index of the given feature in                  |
    |       the 47 Record or -1 when not found.                               |
    \*-----------------------------------------------------------------------*/
int CFR_SW_V31::featIndex(CFRecV & rec,const char * szFeature)
{
 Record_SW_V31 * pSW = (Record_SW_V31 *)rec.record();
 for ( int i = 0, N = numberFeat(rec) ; i < N ; i ++) {
   F7Q fq(&(pSW->Feats[i]));
   if (!strcmp(szFeature,fq.feature()))
     return i;
   }
 return -1;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean containsFeat(CFRecV & rec,const char * szFeature)            |
    |       Indicate whether the given feature exists in                      |
    |       the 47 Record.                                                    |
    \*-----------------------------------------------------------------------*/
Boolean CFR_SW_V31::containsFeat(CFRecV & rec, const char * szFeature)
{
 return (featIndex(rec,szFeature) >= 0);
}

    /*-----------------------------------------------------------------------*\
    |    Feat75 * featPtr(CFRecV & rec,const char * szFeature)                |
    |       Return the pointer to the SW Record's feature record              |
    |       for the given feature name.                                       |
    \*-----------------------------------------------------------------------*/
Feat75 * CFR_SW_V31::featPtr(CFRecV & rec,const char * szFeature)
{
 Record_SW_V31 * pSW = (Record_SW_V31 *)rec.record();
 for ( int i = 0, N = numberFeat(rec) ; i < N ; i ++) {
   F7Q fq(&(pSW->Feats[i]));
   if (!strcmp(szFeature,fq.feature()))
     return &(pSW->Feats[i]);
   }
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |    Feat75 * featPtr(CFRecV & rec,unsigned index)                        |
    |       Return the pointer to the SW Record's feature record              |
    |       for the given feature index.                                      |
    \*-----------------------------------------------------------------------*/
Feat75 * CFR_SW_V31::featPtr(CFRecV & rec,unsigned index)
{
 return (index < numberFeat(rec))?
           &(((Record_SW_V31 *)rec.record())->Feats[index]) : 0;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean featQty(CFRecV & rec, const char * szFeature,                |
    |                    unsigned * pu)                                       |
    |       Set the quantity of the given feature in the given                |
    |       SW Record indicating succes/failure.                              |
    \*-----------------------------------------------------------------------*/
Boolean CFR_SW_V31::featQty(CFRecV & rec, const char * szFeature, unsigned * pu)
{
 Feat75 * pF75 = featPtr(rec,szFeature);
 if (pF75) {
   F7Q fq(pF75);
   *pu = fq.quantity();
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean featParts(CFRecV & rec,const char * szFeature,                |
    |                     char * Target, unsigned * pu )                      |
    |       Copy the name and quantity of the given feature                   |
    |       in the given SW Record indicating succes/failure.                 |
    \*-----------------------------------------------------------------------*/
Boolean CFR_SW_V31::featParts(CFRecV & rec,const char * szFeature,
                         char * Target, unsigned * pu )
{
 Feat75 * pF75 = featPtr(rec,szFeature);
 if (pF75) {
   F7Q fq(pF75);
   *pu = fq.quantity();
   strcpy(Target,fq.feature());
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean featParts(CFRecV &rec , unsigned Index, char * Target,       |
    |                      unsigned * pu )                                    |
    |       Copy the feature name and quantity of the 'Index'-th              |
    |       element of the feature array                                      |
    |       in the given SW Record indicating succes/failure.                 |
    \*-----------------------------------------------------------------------*/
Boolean CFR_SW_V31::featParts(CFRecV &rec , unsigned Index, char * Target,
                         unsigned * pu )
{
 Feat75 * pF75 = featPtr(rec,Index);
 if (pF75) {
   F7Q fq(pF75);
   *pu = fq.quantity();
   strcpy(Target,fq.feature());
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setFeatParts(CFRecV & rec, unsigned Index,unsigned qty,      |
    |                         char *szFeature)                                |
    |       Set the given feature name and quantity of the 'Index'-th         |
    |       element of the feature array                                      |
    |       in the given 47 Record indicating succes/failure.                 |
    |       Note: failure is signaled for 47 Records which                    |
    |             have fewer features than the given Index.                   |
    \*-----------------------------------------------------------------------*/
Boolean CFR_SW_V31::setFeatParts(CFRecV & rec, unsigned Index,unsigned qty,
                            char *szFeature)
{
 Feat75 * pF75 = featPtr(rec,Index);
 if (pF75) {
   F7Q fq(pF75);
   fq.setQuantity(qty);
   if (szFeature)
     fq.setFeature(szFeature);
   fq.copyTo(pF75);
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setFeatQty(CFRecV & rec,const char * szFeature,              |
    |                       unsigned qty )                                    |
    |       Set the given quantity of the given feature                       |
    |       in the given 47 Record indicating succes/failure.                 |
    |       Note: failure is signaled when the 47 Record                      |
    |             does not contain the given feature.                         |
    \*-----------------------------------------------------------------------*/
Boolean CFR_SW_V31::setFeatQty(CFRecV & rec,const char * szFeature,unsigned qty )
{
 Feat75 * pF75 = featPtr(rec,szFeature);
 if (pF75) {
   F7Q fq(pF75);
   fq.setQuantity(qty);
   fq.copyTo(pF75);
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean stripOffFeat(CFRecV & rec,unsigned index)                    |
    |       Truncate the given 47 Record at the given feature index.          |
    |       i.e. if given n, the feature will be array element n-1.           |
    |       Note: This causes a replacement of the underlying                 |
    |             buffer which will unable to hold additional                 |
    |             feature elements.                                           |
    \*-----------------------------------------------------------------------*/
Boolean CFR_SW_V31::stripOffFeat(CFRecV & rec,unsigned index)
{
 CFRecV temp(rec);
 Feat75 * pF75 = featPtr(temp,index);
 if (pF75) {
   *(char *)pF75 = '\0';
   rec = temp;
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    void appendFeat(CFRecV & rec, const char *szFeature,                 |
    |                    unsigned qty)                                        |
    |       Expand the SW Record's feature array with an element              |
    |       for the given feature name and quantity.                          |
    |       Note: This causes a replacement of the underlying                 |
    |             buffer.                                                     |
    \*-----------------------------------------------------------------------*/
void CFR_SW_V31::appendFeat(CFRecV & rec, const char *szFeature, unsigned qty)
{
 Feat75 f75;
 F7Q fq(szFeature,qty);
 fq.copyTo(&f75);
 CFRecV tmp(sizeof(f75),(char *)&f75);
 rec += tmp;
}


 /*--------------------------------------------------------------------------*\
 |                         Non-Compiled testing code                          |
 \*--------------------------------------------------------------------------*/
#ifdef UNITTEST

//#include "euoutil.cpp"
#include <stdlib.h>
#include <iostream.h>


void main(void)
{
 if (1)
   {
    CFRecV rec1("475750   AFC   10400      10401      10409      10414      10457      13406      15700      19001      1");
    CFRecV rec2("475756   030   10191      10200      11552      13326      13444      13465      15005      15050      15052      15700      16092      16093      19001      1");
    CFRecV rec3("475756   102   10004      12000      14148      19001      1");
    CFRecV rec4("475756   105   10007      12000      14169      19001      1");
    CFRecV rec5("475756   218   10054      15805      19001      1");
    char Target[50];
    unsigned u;
    int i;

    cout <<  "rec1 : " << rec1 << endl;
    cout <<  "rec2 : " << rec2 << endl;
    cout <<  "rec3 : " << rec3 << endl;
    cout <<  "rec4 : " << rec4 << endl;
    cout <<  "rec5 : " << rec5 << endl;
    i = CFR47::featParts(rec2,"3326   ",Target,&u);
    cout << "featParts(rec2,'3326   ') :" << i << endl;
    if (i)
      cout << "Target : '" << Target << "' qty : " << u << endl;
    i = CFR47::featParts(rec2,6,Target,&u);
    cout << "featParts(rec2,6) :" << i << endl;
    if (i)
      cout << "Target : '" << Target << "' qty : " << u << endl;
    i = CFR47::numberFeat(rec2);
    cout << "numberFeat(rec2) : " << i << endl;
    cout << "featParts(rec2," << i << ") :"
         << CFR47::featParts(rec2,i,Target,&u) << endl;

    cout << "rec2.setFeatQty('1552   ',99)" << endl;
    CFR47::setFeatQty(rec2,"1552   ",99);
    cout << "rec2.setFeatQty('3326',66)" << endl;
    CFR47::setFeatQty(rec2,"3326",66);

    cout << "rec2.appendFeat('4444',44)" << endl;
    CFR47::appendFeat(rec2,"4444",44);
    cout << "numberFeat(rec2) : " << CFR47::numberFeat(rec2) << endl;

    rec2.setType(44);
    cout <<  "rec2.type() : " << rec2.type() << endl;
    cout <<  "rec2 : " << rec2 << endl;
    cout << "rec1.stripOffFeat(2)" << endl;
    CFR47::stripOffFeat(rec1,2);
    cout <<  "rec1 : " << rec1 << endl;
   }


 if (1)
   {
    cout << " ============CFR49 test out output================" << endl;
    CFRecV rec1("495750AFC   10400      10401      10409      10414      10457      13406      15700      19001      1");
    CFRecV rec2("495756030   10191      10200      11552      13326      13444      13465      15005      15050      15052      15700      16092      16093      19001      1");
    CFRecV rec3("495756102   10004      12000      14148      19001      1");
    CFRecV rec4("495756105   10007      12000      14169      19001      1");
    CFRecV rec5("495756218   10054      15805      19001      1");
    char Target[50];
    unsigned u;
    int i;

    cout <<  "rec1 : " << rec1 << endl;
    cout <<  "rec2 : " << rec2 << endl;
    cout <<  "rec3 : " << rec3 << endl;
    cout <<  "rec4 : " << rec4 << endl;
    cout <<  "rec5 : " << rec5 << endl;
    i = CFR49::featParts(rec2,"3326   ",Target,&u);
    cout << "featParts(rec2,'3326   ') :" << i << endl;
    if (i)
      cout << "Target : '" << Target << "' qty : " << u << endl;
    i = CFR49::featParts(rec2,6,Target,&u);
    cout << "featParts(rec2,6) :" << i << endl;
    if (i)
      cout << "Target : '" << Target << "' qty : " << u << endl;
    i = CFR49::numberFeat(rec2);
    cout << "numberFeat(rec2) : " << i << endl;
    cout << "featParts(rec2," << i << ") :"
         << CFR49::featParts(rec2,i,Target,&u) << endl;

    cout << "rec2.setFeatQty('1552   ',99)" << endl;
    CFR49::setFeatQty(rec2,"1552   ",99);
    cout << "rec2.setFeatQty('3326',66)" << endl;
    CFR49::setFeatQty(rec2,"3326",66);

    cout << "rec2.appendFeat('4444',44)" << endl;
    CFR49::appendFeat(rec2,"4444",44);
    cout << "numberFeat(rec2) : " << CFR49::numberFeat(rec2) << endl;

    rec2.setType(44);
    cout <<  "rec2.type() : " << rec2.type() << endl;
    cout <<  "rec2 : " << rec2 << endl;
    cout << "rec1.stripOffFeat(2)" << endl;
    CFR49::stripOffFeat(rec1,2);
    cout <<  "rec1 : " << rec1 << endl;
   }

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif

}

#endif
#if 0
#endif

