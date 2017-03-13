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
#include "cfr19_50.hpp"
#include "euomsg.h"

Record_19V31 *p19V31;
Record_50V31 *p50V31;

 /*--------------------------------------------------------------------------*\
 |                                CFR19 Class                                 |
 \*--------------------------------------------------------------------------*/
    /*-----------------------------------------------------------------------*\
    |    const char * machineType(const char *Buf)                            |
    |       Return the pointer to the 08 Record's Machine Type                |
    |       for the given buffer.                                             |
    |       WARNING: will not point to a null-terminated string.              |
    \*-----------------------------------------------------------------------*/
const char * CFR19::machineType(const char *Buf)
{
 Record_19 *p19 = (Record_19 *)Buf;
 if (p19 && (strlen(Buf) >= (offsetof(Record_19,Machine)+1)))
   return p19->Machine;
 return "";
}

    /*-----------------------------------------------------------------------*\
    |    const char * serialNo(const char *Buf)                               |
    |       Return the pointer to the 19 Record's Serial No.                  |
    |       for the given buffer.                                             |
    |       WARNING: will not point to a null-terminated string.              |
    \*-----------------------------------------------------------------------*/
const char * CFR19::serialNo(const char *Buf)
{
 Record_19 *p19 = (Record_19 *)Buf;
 if (p19 && (strlen(Buf) >= (offsetof(Record_19,ProdSerial)+1)))
   return p19->ProdSerial;
 return "";
}

    /*-----------------------------------------------------------------------*\
    |    const char * toModel(const char *Buf)                                |
    |       Return the pointer to the 19 Record's To-Model field              |
    |       for the given buffer.                                             |
    |       WARNING: will not point to a null-terminated string.              |
    \*-----------------------------------------------------------------------*/
const char * CFR19::toModel(const char *Buf)
{
 Record_19 *p19 = (Record_19 *)Buf;
 if (p19 && (strlen(Buf) >= (offsetof(Record_19,ToModel)+1)))
   return p19->ToModel;
 return "";
}

    /*-----------------------------------------------------------------------*\
    |    const char * fromModel(const char *Buf)                              |
    |       Return the pointer to the 19 Record's From-Model field            |
    |       for the given buffer.                                             |
    |       WARNING: will not point to a null-terminated string.              |
    \*-----------------------------------------------------------------------*/
const char * CFR19::fromModel(const char *Buf)
{
 Record_19 *p19 = (Record_19 *)Buf;
 if (p19 && (strlen(Buf) >= (offsetof(Record_19,FromModel)+1)))
   return p19->FromModel;
 return "";
}

    /*-----------------------------------------------------------------------*\
    |    IString qty(const char *Buf)                                         |
    |       Return the 19 Record's quantity as a IStirng                      |
    |       for the given buffer.                                             |
    \*-----------------------------------------------------------------------*/
IString CFR19::qty(const char *Buf)
{
 IString str;
 Record_19 *p19 = (Record_19 *)Buf;
 if (p19 && (strlen(Buf) >= (offsetof(Record_19,Qty)+1)))
   str = IString(p19->Qty,sizeof(p19->Qty),0);
 else str= IString();
 return str;
}

    /*-----------------------------------------------------------------------*\
    |    void model(const char * Buf,IString & str)                           |
    |       Set the given IStirng to the 19 Record's Model as per             |
    |       its Action.                                                       |
    \*-----------------------------------------------------------------------*/
void CFR19::model(const char * Buf,IString & str)
{
 str = IString(((action(Buf) == '5') || (action(Buf) == '4') )? toModel(Buf) : fromModel(Buf),3,0);
}

    /*-----------------------------------------------------------------------*\
    |    unsigned numberFeat(CFRecV & rec)                                    |
    |       Return the number of different features in the 19 Record.         |
    \*-----------------------------------------------------------------------*/
unsigned CFR19::numberFeat(CFRecV & rec)
{
 int FeatBytes = rec.length() - offsetof(Record_19,Feats);
#if 0
 if (FeatBytes > 0) {
   unsigned partialFeatBytes = FeatBytes%sizeof(FeatR19),
            qty =  FeatBytes/sizeof(FeatR19);
   if (partialFeatBytes >=  offsetof(FeatR19,Filler))
     qty++;
   return qty;
   }
 return 0;
#endif
 return (FeatBytes <= 0) ? 0 :
           (FeatBytes/sizeof(FeatR19) +
           ((FeatBytes%sizeof(FeatR19) >= offsetof(FeatR19,Filler))? 1: 0));
}

    /*-----------------------------------------------------------------------*\
    |    int featIndex(CFRecV & rec,const char * szFeature)                   |
    |       Return the 0-based index of the given feature in                  |
    |       the 19 Record or -1 when not found.                               |
    \*-----------------------------------------------------------------------*/
int CFR19::featIndex(CFRecV & rec,const char * szFeature)
{
 Record_19 * p19 = (Record_19 *)rec.record();
 for ( int i = 0, N = numberFeat(rec) ; i < N ; i ++) {
   F7QA fq(&(p19->Feats[i]));
   if (!strcmp(szFeature,fq.feature()))
     return i;
   }
 return -1;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean containsFeat(CFRecV & rec,const char * szFeature)            |
    |       Indicate whether the given feature exists in                      |
    |       the 19 Record.                                                    |
    \*-----------------------------------------------------------------------*/
Boolean CFR19::containsFeat(CFRecV & rec,const char * szFeature)
{
 return (featIndex(rec,szFeature) >= 0);
}

    /*-----------------------------------------------------------------------*\
    |    FeatR19 * featPtr(CFRecV & rec,unsigned index)                       |
    |       Return the pointer to the 19 Record's feature record              |
    |       for the given feature index.                                      |
    \*-----------------------------------------------------------------------*/
FeatR19 * CFR19::featPtr(CFRecV & rec,unsigned index)
{
 return (index < numberFeat(rec))?
           &(((Record_19 *)rec.record())->Feats[index]) : 0;
}

    /*-----------------------------------------------------------------------*\
    |    FeatR19 * featPtr(CFRecV & rec,const char * szFeature)               |
    |       Return the pointer to the 19 Record's feature record              |
    |       for the given feature name.                                       |
    \*-----------------------------------------------------------------------*/
FeatR19 * CFR19::featPtr(CFRecV & rec,const char * szFeature)
{
 Record_19 * p19 = (Record_19 *)rec.record();
 for ( int i = 0, N = numberFeat(rec) ; i < N ; i ++) {
   F7QA fq(&(p19->Feats[i]));
   if (!strcmp(szFeature,fq.feature()))
     return &(p19->Feats[i]);
   }
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean featQty(CFRecV & rec, const char * szFeature,                |
    |                    unsigned * pu)                                       |
    |       Return the quantity of the given feature in the given             |
    |       19 Record indicating succes/failure.                              |
    \*-----------------------------------------------------------------------*/
Boolean CFR19::featQty(CFRecV & rec, const char * szFeature, unsigned * pu)
{
 FeatR19 * pF19 = featPtr(rec,szFeature);
 if (pF19)
   *pu = F7QA(pF19).quantity();
 return (pF19 != 0);
}

    /*-----------------------------------------------------------------------*\
    |   Boolean featParts(CFRecV & rec,const char * szFeature,                |
    |                     char * Target, unsigned * pu, char * pAction)       |
    |       Copy name, quantity and action of the given feature               |
    |       in the given 19 Record indicating succes/failure.                 |
    \*-----------------------------------------------------------------------*/
Boolean CFR19::featParts(CFRecV & rec,const char * szFeature,
                         char * Target, unsigned * pu , char * pAction)
{
 FeatR19 * pF19 = featPtr(rec,szFeature);
 if (pF19) {
   F7QA fq(pF19);
   if (pu)
     *pu = fq.quantity();
   if (Target)
     strcpy(Target,fq.feature());
   if (pAction)
     *pAction = fq.action();
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean featParts(CFRecV &rec , unsigned Index,                       |
    |                     char * Target, unsigned * pu, char * pAction)       |
    |       Copy the name, quantity and action of the 'Index'-th              |
    |       element of the feature array                                      |
    |       in the given 19 Record indicating succes/failure.                 |
    \*-----------------------------------------------------------------------*/
Boolean CFR19::featParts(CFRecV &rec , unsigned Index, char * Target,
                         unsigned * pu , char * pAction)
{
 FeatR19 * pF19 = featPtr(rec,Index);
 if (pF19) {
   F7QA fq(pF19);
   if (pu)
     *pu = fq.quantity();
   if (Target)
     strcpy(Target,fq.feature());
   if (pAction)
     *pAction = fq.action();
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setFeatParts(CFRecV & rec, unsigned Index,                   |
    |                         unsigned * pQty, char *szFeature,               |
    |                         char * pAction)                                 |
    |       Set feature, name and action of the 'Index'-th                    |
    |       element of the feature array                                      |
    |       in the given 19 Record, indicating succes/failure.                |
    |       Note: failure is signaled for 19 Records which                    |
    |             have fewer features than the given Index.                   |
    \*-----------------------------------------------------------------------*/
Boolean CFR19::setFeatParts(CFRecV & rec, unsigned Index,unsigned * pQty,
                            char *szFeature, char * pAction)
{
 FeatR19 * pF19 = featPtr(rec,Index);
 if (pF19) {
   F7QA fq(pF19);
   if (pQty)
     fq.setQuantity(*pQty);
   if (szFeature)
     fq.setFeature(szFeature);
   if (pAction)
     fq.setAction(*pAction);
   fq.copyTo(pF19);
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setFeatQty(CFRecV & rec,const char * szFeature,              |
    |                       unsigned qty )                                    |
    |       Set the given quantity of the given feature                       |
    |       in the given 19 Record indicating succes/failure.                 |
    |       Note: failure is signaled when the 19 Record                      |
    |             does not contain the given feature.                         |
    \*-----------------------------------------------------------------------*/
Boolean CFR19::setFeatQty(CFRecV & rec,const char * szFeature,unsigned qty )
{
 FeatR19 * pF19 = featPtr(rec,szFeature);
 if (pF19) {
   F7QA fq(pF19);
   fq.setQuantity(qty);
   fq.copyTo(pF19);
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean stripOffFeat(CFRecV & rec,unsigned index)                    |
    |       Truncate the given 19 Record at the given feature index.          |
    |       i.e. if given n, the feature will be array element n-1.           |
    |       Note: This causes a replacement of the underlying                 |
    |             buffer which will unable to hold additional                 |
    |             feature elements.                                           |
    \*-----------------------------------------------------------------------*/
Boolean CFR19::stripOffFeat(CFRecV & rec,unsigned index)
{
 CFRecV temp(rec);
 FeatR19 * pF19 = featPtr(temp,index);
 if (pF19) {
   *(char *)pF19 = '\0';
   rec = temp;
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    void appendFeat(CFRecV & rec, const char *szFeature,                 |
    |                    unsigned qty, char Action)                           |
    |       Expand the 19 Record's feature array with an element              |
    |       for the given feature name, quantity and action.                  |
    |       Note: This causes a replacement of the underlying                 |
    |             buffer.                                                     |
    \*-----------------------------------------------------------------------*/
void CFR19::appendFeat(CFRecV & rec, const char *szFeature, unsigned qty,
                       char Action)
{
         /*---------------------------------------------------------*\
         | Sometimes a record is chopped of partially (if an editor  |
         | is used which chops blanks off lines) If there's enough   |
         | data for us ( i.e. feature , qty and action ) we'll       |
         | accept it since those fields are up front and contiguous. |
         | However, for appending we have to complete the partial    |
         | feature to bridge to the one to be appended.              |
         \*---------------------------------------------------------*/
 unsigned u = numberFeat(rec);
 FeatR19 * pF = (FeatR19 *)new char[sizeof(FeatR19)];
 if (u && ((rec.length() - offsetof(Record_19,Feats))%sizeof(FeatR19))) {
   const FeatR19 * p = featPtr(rec,--u);
   F7QA fq(p);
   stripOffFeat(rec,u);                          // see note above
   fq.copyTo(pF);
   rec += CFRecV(sizeof(FeatR19),(const char *)pF,' ');
   }
 F7QA(szFeature,qty,Action).copyTo(pF);
 rec += CFRecV(sizeof(FeatR19),(const char *)pF,' ');
 delete (char *)pF;
}

    /*-----------------------------------------------------------------------*\
    |    CFRecV * createANew( const char *szMachine ,                         |
    |                         const char *szModel ,char Action)               |
    |       'New' an 19 Record for the given data.                            |
    \*-----------------------------------------------------------------------*/
CFRecV * CFR19::createANew( const char *szMachine
                           ,const char *szModel
                           ,char Action)
{
 CFRecV * p = new CFRecV(offsetof(Record_19,Feats));
// CFRecV * p = new CFRecV(sizeof(Record_19));
 Record_19 * p19 = (Record_19 *)(p->record());
 p->setType(19);
 CFRPaddCopySZ(p19->Machine,szMachine,sizeof(p19->Machine));
 p19->Filler1[0] = '0';
 p19->Filler1[1] = '1';
 p19->Filler1[2] = '0';
 CFRPaddCopySZ(p19->ToModel,szModel,sizeof(p19->ToModel));
 p19->Qty[3] = '1';
 p19->Action = Action;
 p19->Filler2[0] = '0';
 p19->Filler3[0] = '0';
 return p;
}

////////////////////////////////////////////////////////////

 /*--------------------------------------------------------------------------*\
 |                               CFR19V31 Class                               |
 \*--------------------------------------------------------------------------*/
    /*-----------------------------------------------------------------------*\
    |    IString type(const char *Buf)                                        |
    |       Return the Product Type string.                                   |
    \*-----------------------------------------------------------------------*/
IString CFR19V31::type(const char *Buf)
{
 return (CFRecType(Buf) != 19)? IString() :
            IString::stripBlanks(IString(((Record_19V31 *)Buf)->ProductType,
                                 sizeof(p19V31->ProductType,0)));
}

    /*-----------------------------------------------------------------------*\
    |    IString model(const char *Buf)                                       |
    |       Return the Product Model string.                                  |
    \*-----------------------------------------------------------------------*/
IString CFR19V31::model(const char *Buf)
{
 IString str;
 return  (CFRecType(Buf) != 19)? IString() :
             IString::stripBlanks(IString(((Record_19V31 *)Buf)->ProductModel,
                                  sizeof(p19V31->ProductModel,0)));
}

    /*-----------------------------------------------------------------------*\
    |    IString toType(const char *Buf)                                      |
    |       Return the To Product Type string.                                |
    \*-----------------------------------------------------------------------*/
IString CFR19V31::toType(const char *Buf)
{
 return (CFRecType(Buf) != 19)? IString() :
     IString::stripBlanks(IString(((Record_19V31 *)Buf)->ToProductType,
                                   sizeof(p19V31->ToProductType,0)));
}

    /*-----------------------------------------------------------------------*\
    |    IString toModel(const char *Buf)                                     |
    |       Return the To Product Model string.                               |
    \*-----------------------------------------------------------------------*/
IString CFR19V31::toModel(const char *Buf)
{
 return (CFRecType(Buf) != 19)? IString() :
   IString::stripBlanks(IString(((Record_19V31 *)Buf)->ToProductModel,
                                  sizeof(p19V31->ToProductModel,0)));
}

    /*-----------------------------------------------------------------------*\
    |    IString fromType(const char *Buf)                                    |
    |       Return the From Product Type string.                              |
    \*-----------------------------------------------------------------------*/
IString CFR19V31::fromType(const char *Buf)
{ return type(Buf);}

    /*-----------------------------------------------------------------------*\
    |    IString fromModel(const char *Buf)                                   |
    |       Return the From Product Model string.                             |
    \*-----------------------------------------------------------------------*/
IString CFR19V31::fromModel(const char *Buf)
{ return model(Buf);}

    /*-----------------------------------------------------------------------*\
    |    char action(const char *Buf)                                         |
    |       Return the 19 Record's action                                     |
    \*-----------------------------------------------------------------------*/
unsigned CFR19V31::action(const char *Buf)
{ return (CFRecType(Buf) == 19)? ((Record_19V31 *)Buf)->Action : ' ';}


    /*-----------------------------------------------------------------------*\
    |    unsigned numberFeat(CFRecV & rec)                                    |
    |       Return the number of different features in the 19 Record.         |
    \*-----------------------------------------------------------------------*/
unsigned CFR19V31::numberFeat(CFRecV & rec)
{
 int FeatBytes = rec.length() - offsetof(Record_19V31,Feats);
 return (FeatBytes <= 0) ? 0 : FeatBytes/sizeof(Feat751);
}

    /*-----------------------------------------------------------------------*\
    |    int featIndex(CFRecV & rec,const char * szFeature)                   |
    |       Return the 0-based index of the given feature in                  |
    |       the 19 Record or -1 when not found.                               |
    \*-----------------------------------------------------------------------*/
int CFR19V31::featIndex(CFRecV & rec,const char * szFeature)
{
 Record_19V31 * p19 = (Record_19V31 *)rec.record();
 for ( int i = 0, N = numberFeat(rec) ; i < N ; i ++) {
   F7QA fq(&(p19->Feats[i]));
   if (!strcmp(szFeature,fq.feature()))
     return i;
   }
 return -1;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean containsFeat(CFRecV & rec,const char * szFeature)            |
    |       Indicate whether the given feature exists in                      |
    |       the 19 Record.                                                    |
    \*-----------------------------------------------------------------------*/
Boolean CFR19V31::containsFeat(CFRecV & rec,const char * szFeature)
{
 return (featIndex(rec,szFeature) >= 0);
}

    /*-----------------------------------------------------------------------*\
    |    Feat751 * featPtr(CFRecV & rec,unsigned index)                       |
    |       Return the pointer to the 19 Record's feature record              |
    |       for the given feature index.                                      |
    \*-----------------------------------------------------------------------*/
Feat751 * CFR19V31::featPtr(CFRecV & rec,unsigned index)
{
 return (index < numberFeat(rec))?
           &(((Record_19V31 *)rec.record())->Feats[index]) : 0;
}

    /*-----------------------------------------------------------------------*\
    |    Feat751 * featPtr(CFRecV & rec,const char * szFeature)               |
    |       Return the pointer to the 19 Record's feature record              |
    |       for the given feature name.                                       |
    \*-----------------------------------------------------------------------*/
Feat751 * CFR19V31::featPtr(CFRecV & rec,const char * szFeature)
{
 Record_19V31 * p19 = (Record_19V31 *)rec.record();
 for ( int i = 0, N = numberFeat(rec) ; i < N ; i ++) {
   F7QA fq(&(p19->Feats[i]));
   if (!strcmp(szFeature,fq.feature()))
     return &(p19->Feats[i]);
   }
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean featQty(CFRecV & rec, const char * szFeature,                |
    |                    unsigned * pu)                                       |
    |       Return the quantity of the given feature in the given             |
    |       19 Record indicating succes/failure.                              |
    \*-----------------------------------------------------------------------*/
Boolean CFR19V31::featQty(CFRecV & rec, const char * szFeature, unsigned * pu)
{
 Feat751 * pF = featPtr(rec,szFeature);
 if (pF)
   *pu = F7QA(pF).quantity();
 return (pF != 0);
}

    /*-----------------------------------------------------------------------*\
    |   Boolean featParts(CFRecV & rec,const char * szFeature,                |
    |                     char * Target, unsigned * pu, char * pAction)       |
    |       Copy name, quantity and action of the given feature               |
    |       in the given 19 Record indicating succes/failure.                 |
    \*-----------------------------------------------------------------------*/
Boolean CFR19V31::featParts(CFRecV & rec,const char * szFeature,
                            char * Target, unsigned * pu , char * pAction)
{
 Feat751 * pF = featPtr(rec,szFeature);
 if (pF) {
   F7QA fq(pF);
   if (pu)
     *pu = fq.quantity();
   if (Target)
     strcpy(Target,fq.feature());
   if (pAction)
     *pAction = fq.action();
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean featParts(CFRecV &rec , unsigned Index,                       |
    |                     char * Target, unsigned * pu, char * pAction)       |
    |       Copy the name, quantity and action of the 'Index'-th              |
    |       element of the feature array                                      |
    |       in the given 19 Record indicating succes/failure.                 |
    \*-----------------------------------------------------------------------*/
Boolean CFR19V31::featParts(CFRecV &rec , unsigned Index, char * Target,
                         unsigned * pu , char * pAction)
{
 Feat751 * pF = featPtr(rec,Index);
 if (pF) {
   F7QA fq(pF);
   if (pu)
     *pu = fq.quantity();
   if (Target)
     strcpy(Target,fq.feature());
   if (pAction)
     *pAction = fq.action();
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setFeatParts(CFRecV & rec, unsigned Index,                   |
    |                         unsigned * pQty, char *szFeature,               |
    |                         char * pAction)                                 |
    |       Set feature, name and action of the 'Index'-th                    |
    |       element of the feature array                                      |
    |       in the given 19 Record, indicating succes/failure.                |
    |       Note: failure is signaled for 19 Records which                    |
    |             have fewer features than the given Index.                   |
    \*-----------------------------------------------------------------------*/
Boolean CFR19V31::setFeatParts(CFRecV & rec, unsigned Index,unsigned * pQty,
                            char *szFeature, char * pAction)
{
 Feat751 * pF = featPtr(rec,Index);
 if (pF) {
   F7QA fq(pF);
   if (pQty)
     fq.setQuantity(*pQty);
   if (szFeature)
     fq.setFeature(szFeature);
   if (pAction)
     fq.setAction(*pAction);
   fq.copyTo(pF);
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setFeatQty(CFRecV & rec,const char * szFeature,              |
    |                       unsigned qty )                                    |
    |       Set the given quantity of the given feature                       |
    |       in the given 19 Record indicating succes/failure.                 |
    |       Note: failure is signaled when the 19 Record                      |
    |             does not contain the given feature.                         |
    \*-----------------------------------------------------------------------*/
Boolean CFR19V31::setFeatQty(CFRecV & rec,const char * szFeature,unsigned qty )
{
 Feat751 * pF = featPtr(rec,szFeature);
 if (pF) {
   F7QA fq(pF);
   fq.setQuantity(qty);
   fq.copyTo(pF);
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean stripOffFeat(CFRecV & rec,unsigned index)                    |
    |       Truncate the given 19 Record at the given feature index.          |
    |       i.e. if given n, the feature will be array element n-1.           |
    |       Note: This causes a replacement of the underlying                 |
    |             buffer which will unable to hold additional                 |
    |             feature elements.                                           |
    \*-----------------------------------------------------------------------*/
Boolean CFR19V31::stripOffFeat(CFRecV & rec,unsigned index)
{
 CFRecV temp(rec);
 Feat751 * pF = featPtr(temp,index);
 if (pF) {
   *(char *)pF = '\0';
   rec = temp;
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    void appendFeat(CFRecV & rec, const char *szFeature,                 |
    |                    unsigned qty, char Action)                           |
    |       Expand the 19 Record's feature array with an element              |
    |       for the given feature name, quantity and action.                  |
    |       Note: This causes a replacement of the underlying                 |
    |             buffer.                                                     |
    \*-----------------------------------------------------------------------*/
void CFR19V31::appendFeat(CFRecV & rec, const char *szFeature, unsigned qty,
                          char Action)
{
         /*---------------------------------------------------------*\
         | Sometimes a record is chopped of partially (if an editor  |
         | is used which chops blanks off lines) If there's enough   |
         | data for us ( i.e. feature , qty and action ) we'll       |
         | accept it since those fields are up front and contiguous. |
         | However, for appending we have to complete the partial    |
         | feature to bridge to the one to be appended.              |
         \*---------------------------------------------------------*/
 unsigned u = numberFeat(rec);
 Feat751 * pF = (Feat751 *)new char[sizeof(Feat751)];
 if (u && ((rec.length() - offsetof(Record_19V31,Feats))%sizeof(Feat751))) {
   const Feat751 * p = featPtr(rec,--u);
   F7QA fq(p);
   stripOffFeat(rec,u);                          // see note above
   fq.copyTo(pF);
   rec += CFRecV(sizeof(Feat751),(const char *)pF,' ');
   }
 F7QA(szFeature,qty,Action).copyTo(pF);
 rec += CFRecV(sizeof(Feat751),(const char *)pF,' ');
 delete (char *)pF;
}

    /*-----------------------------------------------------------------------*\
    |    CFRecV * createANew( const char *szMachine ,                         |
    |                         const char *szModel ,char Action)               |
    |       'New' an 19 Record for the given data.                            |
    \*-----------------------------------------------------------------------*/
CFRecV * CFR19V31::createANew( const char *szMachine
                              ,const char *szModel
                              ,char Action)
{
 CFRecV * p = new CFRecV(offsetof(Record_19V31,Feats));
// CFRecV * p = new CFRecV(sizeof(Record_19));
 Record_19V31 * p19 = (Record_19V31 *)(p->record());
 p->setType(19);
 CFRPaddCopySZ(p19->ProductType,szMachine,sizeof(p19->ProductType));
 CFRPaddCopySZ(p19->ProductModel,szModel,sizeof(p19->ProductModel));
 p19->Action = Action;
 return p;
}


///////////////////////////////////////////////////////////////////



 /*--------------------------------------------------------------------------*\
 |                                CFR50 Class                                 |
 \*--------------------------------------------------------------------------*/
    /*-----------------------------------------------------------------------*\
    |    const char * machineType(const char *Buf)                            |
    |       Return the pointer to the 08 Record's Machine Type                |
    |       for the given buffer.                                             |
    |       WARNING: will not point to a null-terminated string.              |
    \*-----------------------------------------------------------------------*/
const char * CFR50::machineType(const char *Buf)
{
 Record_50 *p50 = (Record_50 *)Buf;
 if (p50 && (strlen(Buf) >= (offsetof(Record_50,Machine)+1)))
   return p50->Machine;
 return "";
}

    /*-----------------------------------------------------------------------*\
    |    void model(const char * Buf,IString & str)                           |
    |       Set the given IString to the 50 Record's Model.                   |
    \*-----------------------------------------------------------------------*/
void CFR50::model(const char * Buf,IString & str)
{
 str = IString(model(Buf),3,0);
}

    /*-----------------------------------------------------------------------*\
    |    const char * model(const char *Buf)                                  |
    |       Return the pointer to the 50 Record's Model field                 |
    |       for the given buffer.                                             |
    |       WARNING: will not point to a null-terminated string.              |
    \*-----------------------------------------------------------------------*/
const char * CFR50::model(const char *Buf)
{
 Record_50 *p50 = (Record_50 *)Buf;
 if (p50 && (strlen(Buf) >= (offsetof(Record_50,Model)+1)))
   return p50->Model;
 return "";
}

    /*-----------------------------------------------------------------------*\
    |    IString qty(const char *Buf)                                         |
    |       Return the 50 Record's quantity as a IStirng                      |
    |       for the given buffer.                                             |
    \*-----------------------------------------------------------------------*/
IString CFR50::qty(const char *Buf)
{
 IString str;
 Record_50 *p50 = (Record_50 *)Buf;
 if (p50 && (strlen(Buf) >= (offsetof(Record_50,Qty)+1)))
   str = IString(p50->Qty,sizeof(p50->Qty),0);
 else str= IString();
 return str;
}

    /*-----------------------------------------------------------------------*\
    |    unsigned numberFeat(CFRecV & rec)                                    |
    |       Return the number of different features in the 19 Record.         |
    \*-----------------------------------------------------------------------*/
unsigned CFR50::numberFeat(CFRecV & rec)
{
 int FeatBytes = rec.length() - offsetof(Record_50,Feats);
 return (FeatBytes <= 0) ? 0 :
           (FeatBytes/sizeof(FeatR50) +
           ((FeatBytes%sizeof(FeatR50) >= offsetof(FeatR50,Filler))? 1: 0));
}

    /*-----------------------------------------------------------------------*\
    |    int featIndex(CFRecV & rec,const char * szFeature)                   |
    |       Return the 0-based index of the given feature in                  |
    |       the 19 Record or -1 when not found.                               |
    \*-----------------------------------------------------------------------*/
int CFR50::featIndex(CFRecV & rec,const char * szFeature)
{
 Record_50 * p50 = (Record_50 *)rec.record();
 for ( int i = 0, N = numberFeat(rec) ; i < N ; i ++) {
   F7QA fq(&(p50->Feats[i]));
   if (!strcmp(szFeature,fq.feature()))
     return i;
   }
 return -1;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean containsFeat(CFRecV & rec,const char * szFeature)            |
    |       Indicate whether the given feature exists in                      |
    |       the 50 Record.                                                    |
    \*-----------------------------------------------------------------------*/
Boolean CFR50::containsFeat(CFRecV & rec,const char * szFeature)
{
 return (featIndex(rec,szFeature) >= 0);
}

    /*-----------------------------------------------------------------------*\
    |    FeatR50 * featPtr(CFRecV & rec,unsigned index)                       |
    |       Return the pointer to the 50 Record's feature record              |
    |       for the given feature index.                                      |
    \*-----------------------------------------------------------------------*/
FeatR50 * CFR50::featPtr(CFRecV & rec,unsigned index)
{
 return (index < numberFeat(rec))?
           &(((Record_50 *)rec.record())->Feats[index]) : 0;
}

    /*-----------------------------------------------------------------------*\
    |    FeatR50 * featPtr(CFRecV & rec,const char * szFeature)               |
    |       Return the pointer to the 50 Record's feature record              |
    |       for the given feature name.                                       |
    \*-----------------------------------------------------------------------*/
FeatR50 * CFR50::featPtr(CFRecV & rec,const char * szFeature)
{
 Record_50 * p50 = (Record_50 *)rec.record();
 for ( int i = 0, N = numberFeat(rec) ; i < N ; i ++) {
   F7QA fq(&(p50->Feats[i]));
   if (!strcmp(szFeature,fq.feature()))
     return &(p50->Feats[i]);
   }
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean featQty(CFRecV & rec, const char * szFeature,                |
    |                    unsigned * pu)                                       |
    |       Set the quantity of the given feature in the given                |
    |       50 Record indicating succes/failure.                              |
    \*-----------------------------------------------------------------------*/
Boolean CFR50::featQty(CFRecV & rec, const char * szFeature, unsigned * pu)
{
 FeatR50 * pF50 = featPtr(rec,szFeature);
 if (pF50)
   *pu =  F7QA(pF50).quantity();
 return (pF50 != 0);
}

    /*-----------------------------------------------------------------------*\
    |   Boolean featParts(CFRecV & rec,const char * szFeature,                |
    |                     char * Target,unsigned * pu , char * pAction)       |
    |       Copy name, quantity and action of the given feature               |
    |       in the given 50 Record indicating succes/failure.                 |
    \*-----------------------------------------------------------------------*/
Boolean CFR50::featParts(CFRecV & rec,const char * szFeature,
                         char * Target, unsigned * pu , char * pAction)
{
 FeatR50 * pF50 = featPtr(rec,szFeature);
 if (pF50) {
   F7QA fq(pF50);
   if (pu)
     *pu = fq.quantity();
   if (Target)
     strcpy(Target,fq.feature());
   if (pAction)
     *pAction = fq.action();
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean featParts(CFRecV &rec , unsigned Index,                       |
    |                     char * Target, unsigned * pu , char * pAction)      |
    |       Copy the name, quantity and action of the 'Index'-th              |
    |       element of the feature array                                      |
    |       in the given 50 Record indicating succes/failure.                 |
    \*-----------------------------------------------------------------------*/
Boolean CFR50::featParts(CFRecV &rec , unsigned Index, char * Target,
                         unsigned * pu , char * pAction)
{
 FeatR50 * pF50 = featPtr(rec,Index);
 if (pF50) {
   F7QA fq(pF50);
   if (pu)
     *pu = fq.quantity();
   if (Target)
     strcpy(Target,fq.feature());
   if (pAction)
     *pAction = fq.action();
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setFeatParts(CFRecV & rec, unsigned Index,                   |
    |                         unsigned * pQty, char *szFeature,               |
    |                         char * pAction)                                 |
    |       Set feature, name and action of the 'Index'-th                    |
    |       element of the feature array                                      |
    |       in the given 50 Record, indicating succes/failure.                |
    |       Note: failure is signaled for 50 Records which                    |
    |             have fewer features than the given Index.                   |
    \*-----------------------------------------------------------------------*/
Boolean CFR50::setFeatParts(CFRecV & rec, unsigned Index,unsigned * pQty,
                            char *szFeature, char * pAction)
{
 FeatR50 * pF50 = featPtr(rec,Index);
 if (pF50) {
   F7QA fq(pF50);
   if (pQty)
     fq.setQuantity(*pQty);
   if (szFeature)
     fq.setFeature(szFeature);
   if (pAction)
     fq.setAction(*pAction);
   fq.copyTo(pF50);
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setFeatQty(CFRecV & rec,const char * szFeature,              |
    |                       unsigned qty )                                    |
    |       Set the given quantity of the given feature                       |
    |       in the given 50 Record indicating succes/failure.                 |
    |       Note: failure is signaled when the 50 Record                      |
    |             does not contain the given feature.                         |
    \*-----------------------------------------------------------------------*/
Boolean CFR50::setFeatQty(CFRecV & rec,const char * szFeature,unsigned qty )
{
 FeatR50 * pF50 = featPtr(rec,szFeature);
 if (pF50) {
   F7QA fq(pF50);
   fq.setQuantity(qty);
   fq.copyTo(pF50);
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean stripOffFeat(CFRecV & rec,unsigned index)                    |
    |       Truncate the given 50 Record at the given feature index.          |
    |       i.e. if given n, the feature will be array element n-1.           |
    |       Note: This causes a replacement of the underlying                 |
    |             buffer which will unable to hold additional                 |
    |             feature elements.                                           |
    \*-----------------------------------------------------------------------*/
Boolean CFR50::stripOffFeat(CFRecV & rec,unsigned index)
{
 CFRecV temp(rec);
 FeatR50 * pF50 = featPtr(temp,index);
 if (pF50) {
   *(char *)pF50 = '\0';
   rec = temp;
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    void appendFeat(CFRecV & rec, const char *szFeature,                 |
    |                    unsigned qty, char Action)                           |
    |       Expand the 50 Record's feature array with an element              |
    |       for the given feature name, quantity and action.                  |
    |       Note: This causes a replacement of the underlying                 |
    |             buffer.                                                     |
    \*-----------------------------------------------------------------------*/
void CFR50::appendFeat(CFRecV & rec, const char *szFeature, unsigned qty,
                       char Action)
{
         /*---------------------------------------------------------*\
         | Sometimes a record is chopped of partially (if an editor  |
         | is used which chops blanks off lines) If there's enough   |
         | data for us ( i.e. feature , qty and action ) we'll       |
         | accept it since those fields are up front and contiguous. |
         | However, for appending we have to complete the partial    |
         | feature to bridge to the one to be appended.              |
         \*---------------------------------------------------------*/
 unsigned u = numberFeat(rec);
 FeatR50 * pF = new FeatR50;
 if (u && ((rec.length() - offsetof(Record_50,Feats))%sizeof(FeatR50))) {
   const FeatR50 * p = featPtr(rec,--u);
   F7QA fq(p);
   stripOffFeat(rec,u);                          // see note above
   fq.copyTo(pF);
   rec += CFRecV(sizeof(FeatR50),(const char *)pF,' ');
   }
 F7QA(szFeature,qty,Action).copyTo(pF);
 rec += CFRecV(sizeof(FeatR50),(const char *)pF,' ');
 delete pF;
}

/////////////////////////////////////////////////////////////////////

 /*--------------------------------------------------------------------------*\
 |                               CFR50V31 Class                               |
 \*--------------------------------------------------------------------------*/
IString CFR50V31::type(const char *Buf)
{
 return  (CFRecType(Buf) != 50)? IString() :
      IString::stripBlanks(IString(((Record_50V31 *)Buf)->ProgramType,
                           sizeof(p50V31->ProgramType,0)));
}

IString CFR50V31::model(const char *Buf)
{
 return (CFRecType(Buf) != 50)? IString() :
   IString::stripBlanks(IString(((Record_50V31 *)Buf)->ProgramModel,
                        sizeof(p50V31->ProgramModel,0)));
}

unsigned CFR50V31::action(const char *Buf)
{ return (CFRecType(Buf) == 50)? ((Record_50V31 *)Buf)->Action : ' ';}

    /*-----------------------------------------------------------------------*\
    |    unsigned numberFeat(CFRecV & rec)                                    |
    |       Return the number of different features in the 50 Record.         |
    \*-----------------------------------------------------------------------*/
unsigned CFR50V31::numberFeat(CFRecV & rec)
{
 int FeatBytes = rec.length() - offsetof(Record_50V31,Feats);
 return (FeatBytes <= 0) ? 0 : FeatBytes/sizeof(Feat751);
}

    /*-----------------------------------------------------------------------*\
    |    int featIndex(CFRecV & rec,const char * szFeature)                   |
    |       Return the 0-based index of the given feature in                  |
    |       the 50 Record or -1 when not found.                               |
    \*-----------------------------------------------------------------------*/
int CFR50V31::featIndex(CFRecV & rec,const char * szFeature)
{
 Record_50V31 * p50 = (Record_50V31 *)rec.record();
 for ( int i = 0, N = numberFeat(rec) ; i < N ; i ++) {
   F7QA fq(&(p50->Feats[i]));
   if (!strcmp(szFeature,fq.feature()))
     return i;
   }
 return -1;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean containsFeat(CFRecV & rec,const char * szFeature)            |
    |       Indicate whether the given feature exists in                      |
    |       the 50 Record.                                                    |
    \*-----------------------------------------------------------------------*/
Boolean CFR50V31::containsFeat(CFRecV & rec,const char * szFeature)
{
 return (featIndex(rec,szFeature) >= 0);
}

    /*-----------------------------------------------------------------------*\
    |    Feat751 * featPtr(CFRecV & rec,unsigned index)                       |
    |       Return the pointer to the 50 Record's feature record              |
    |       for the given feature index.                                      |
    \*-----------------------------------------------------------------------*/
Feat751 * CFR50V31::featPtr(CFRecV & rec,unsigned index)
{
 return (index < numberFeat(rec))?
           &(((Record_50V31 *)rec.record())->Feats[index]) : 0;
}

    /*-----------------------------------------------------------------------*\
    |    Feat751 * featPtr(CFRecV & rec,const char * szFeature)               |
    |       Return the pointer to the 50 Record's feature record              |
    |       for the given feature name.                                       |
    \*-----------------------------------------------------------------------*/
Feat751 * CFR50V31::featPtr(CFRecV & rec,const char * szFeature)
{
 Record_50V31 * p50 = (Record_50V31 *)rec.record();
 for ( int i = 0, N = numberFeat(rec) ; i < N ; i ++) {
   F7QA fq(&(p50->Feats[i]));
   if (!strcmp(szFeature,fq.feature()))
     return &(p50->Feats[i]);
   }
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean featQty(CFRecV & rec, const char * szFeature,                |
    |                    unsigned * pu)                                       |
    |       Return the quantity of the given feature in the given             |
    |       50 Record indicating succes/failure.                              |
    \*-----------------------------------------------------------------------*/
Boolean CFR50V31::featQty(CFRecV & rec, const char * szFeature, unsigned * pu)
{
 Feat751 * pF = featPtr(rec,szFeature);
 if (pF)
   *pu = F7QA(pF).quantity();
 return (pF != 0);
}

    /*-----------------------------------------------------------------------*\
    |   Boolean featParts(CFRecV & rec,const char * szFeature,                |
    |                     char * Target, unsigned * pu, char * pAction)       |
    |       Copy name, quantity and action of the given feature               |
    |       in the given 50 Record indicating succes/failure.                 |
    \*-----------------------------------------------------------------------*/
Boolean CFR50V31::featParts(CFRecV & rec,const char * szFeature,
                            char * Target, unsigned * pu , char * pAction)
{
 Feat751 * pF = featPtr(rec,szFeature);
 if (pF) {
   F7QA fq(pF);
   if (pu)
     *pu = fq.quantity();
   if (Target)
     strcpy(Target,fq.feature());
   if (pAction)
     *pAction = fq.action();
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean featParts(CFRecV &rec , unsigned Index,                       |
    |                     char * Target, unsigned * pu, char * pAction)       |
    |       Copy the name, quantity and action of the 'Index'-th              |
    |       element of the feature array                                      |
    |       in the given 50 Record indicating succes/failure.                 |
    \*-----------------------------------------------------------------------*/
Boolean CFR50V31::featParts(CFRecV &rec , unsigned Index, char * Target,
                         unsigned * pu , char * pAction)
{
 Feat751 * pF = featPtr(rec,Index);
 if (pF) {
   F7QA fq(pF);
   if (pu)
     *pu = fq.quantity();
   if (Target)
     strcpy(Target,fq.feature());
   if (pAction)
     *pAction = fq.action();
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setFeatParts(CFRecV & rec, unsigned Index,                   |
    |                         unsigned * pQty, char *szFeature,               |
    |                         char * pAction)                                 |
    |       Set feature, name and action of the 'Index'-th                    |
    |       element of the feature array                                      |
    |       in the given 50 Record, indicating succes/failure.                |
    |       Note: failure is signaled for 50 Records which                    |
    |             have fewer features than the given Index.                   |
    \*-----------------------------------------------------------------------*/
Boolean CFR50V31::setFeatParts(CFRecV & rec, unsigned Index,unsigned * pQty,
                               char *szFeature, char * pAction)
{
 Feat751 * pF = featPtr(rec,Index);
 if (pF) {
   F7QA fq(pF);
   if (pQty)
     fq.setQuantity(*pQty);
   if (szFeature)
     fq.setFeature(szFeature);
   if (pAction)
     fq.setAction(*pAction);
   fq.copyTo(pF);
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setFeatQty(CFRecV & rec,const char * szFeature,              |
    |                       unsigned qty )                                    |
    |       Set the given quantity of the given feature                       |
    |       in the given 50 Record indicating succes/failure.                 |
    |       Note: failure is signaled when the 50 Record                      |
    |             does not contain the given feature.                         |
    \*-----------------------------------------------------------------------*/
Boolean CFR50V31::setFeatQty(CFRecV & rec,const char * szFeature,unsigned qty )
{
 Feat751 * pF = featPtr(rec,szFeature);
 if (pF) {
   F7QA fq(pF);
   fq.setQuantity(qty);
   fq.copyTo(pF);
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean stripOffFeat(CFRecV & rec,unsigned index)                    |
    |       Truncate the given 50 Record at the given feature index.          |
    |       i.e. if given n, the feature will be array element n-1.           |
    |       Note: This causes a replacement of the underlying                 |
    |             buffer which will unable to hold additional                 |
    |             feature elements.                                           |
    \*-----------------------------------------------------------------------*/
Boolean CFR50V31::stripOffFeat(CFRecV & rec,unsigned index)
{
 CFRecV temp(rec);
 Feat751 * pF = featPtr(temp,index);
 if (pF) {
   *(char *)pF = '\0';
   rec = temp;
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    void appendFeat(CFRecV & rec, const char *szFeature,                 |
    |                    unsigned qty, char Action)                           |
    |       Expand the 50 Record's feature array with an element              |
    |       for the given feature name, quantity and action.                  |
    |       Note: This causes a replacement of the underlying                 |
    |             buffer.                                                     |
    \*-----------------------------------------------------------------------*/
void CFR50V31::appendFeat(CFRecV & rec, const char *szFeature, unsigned qty,
                          char Action)
{
         /*---------------------------------------------------------*\
         | Sometimes a record is chopped of partially (if an editor  |
         | is used which chops blanks off lines) If there's enough   |
         | data for us ( i.e. feature , qty and action ) we'll       |
         | accept it since those fields are up front and contiguous. |
         | However, for appending we have to complete the partial    |
         | feature to bridge to the one to be appended.              |
         \*---------------------------------------------------------*/
 unsigned u = numberFeat(rec);
 Feat751 * pF = (Feat751 *)new char[sizeof(Feat751)];
 if (u && ((rec.length() - offsetof(Record_50V31,Feats))%sizeof(Feat751))) {
   const Feat751 * p = featPtr(rec,--u);
   F7QA fq(p);
   stripOffFeat(rec,u);                          // see note above
   fq.copyTo(pF);
   rec += CFRecV(sizeof(Feat751),(const char *)pF,' ');
   }
 F7QA(szFeature,qty,Action).copyTo(pF);
 rec += CFRecV(sizeof(Feat751),(const char *)pF,' ');
 delete (char *)pF;
}

///////////////////////////////////////////////////////////////////

///////////////////////////////////////
#ifdef UNITTEST

//#include "euoutil.cpp"
#include <stdlib.h>
#include <iostream.h>


void main(void)
{
 if (1)
   {
    FILE *f;
//199406010       F97   140           0                                     6050   2100
//54      U9406    F97       000038312       3715692                              093094        T05284   19940930                          0                                                           B'LIC' SYS UNIT 128MB RACK
//54      U9406    F97   6050000038312       3715692                              093094        T05284   19940930                          0                                                           BTWINAX WKST CTRL - ENHANCED
    CFRecV rec1("199406010       F97   140           0                                     6050   2100");
    char Target[50];
    unsigned u;
    int i;

    cout <<  "rec1 : " << rec1 << endl;
    i = CFR19::featParts(rec1,"   6050",Target,&u);
    cout << "featParts(rec1,'   6050') :" << i << endl;
    if (i)
      cout << "Target : '" << Target << "' qty : " << u << endl;
    i = CFR19::featParts(rec1,3,Target,&u);
    cout << "featParts(rec1,3) :" << i << endl;
    if (i)
      cout << "Target : '" << Target << "' qty : " << u << endl;
    i = CFR19::featParts(rec1,1,Target,&u);
    cout << "featParts(rec1,1) :" << i << endl;
    if (i)
      cout << "Target : '" << Target << "' qty : " << u << endl;

    cout << "rec1.setFeatQty('   6050',99)" << endl;
    CFR19::setFeatQty(rec1,"   6050",99);
    cout <<  "rec1 : " << rec1 << endl;

    cout << "rec1.setFeatQty('6050',66)" << endl;
    CFR19::setFeatQty(rec1,"6050",66);
    cout <<  "rec1 : " << rec1 << endl;

    cout << "rec1.appendFeat('2222',22,'2')" << endl;
    CFR19::appendFeat(rec1,"2222",22,'2');
    cout <<  "rec1 : " << rec1 << endl;

    cout << "rec1.appendFeat('3333',33,'3')" << endl;
    CFR19::appendFeat(rec1,"3333",33,'3');
    cout <<  "rec1 : " << rec1 << endl;

    rec1.setType(44);
    cout <<  "rec1.type() : " << rec1.type() << endl;

    cout << "rec1.stripOffFeat(1)" << endl;
    CFR19::stripOffFeat(rec1,1);
    cout <<  "rec1 : " << rec1 << endl;


   }

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif

}

#endif
