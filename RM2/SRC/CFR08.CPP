 /*--------------------------------------------------------------------------*\
 |    Filename - CFR08.CPP                                                    |
 |      contains source for the following RM/2 class implementations:         |
 |                                                                            |
 |       CFR08  - To access the CFReport's Record Type 08                     |
 |                as defined in 'euopdef.h'                                   |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |              Standard Library Includes              |
        \*---------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stddef.h>

        /*---------------------------------------------------*\
        |                  ICLUI's Includes                   |
        \*---------------------------------------------------*/
#include <isynonym.hpp>                          // Definition of Boolean etc

        /*---------------------------------------------------*\
        |               RM/2-specific includes                |
        \*---------------------------------------------------*/
#include "cfrfq.hpp"
#include "cfr08.hpp"                             // this file's includes
#include "euomsg.h"                              // Message Ids

Record_08 * p8;
Record_08V31 * p8V31;

 /*--------------------------------------------------------------------------*\
 |                                CFR08 Class                                 |
 \*--------------------------------------------------------------------------*/
        /*-------------------------------------------------------*\
        |                          Ctors                          |
        \*-------------------------------------------------------*/

CFR08::CFR08()
      : CFRecV(offsetof(Record_08,Feats))
{
 setType(8);
}

CFR08::CFR08(Record_08V31  * p8V31)
      : CFRecV(offsetof(Record_08,Feats) + (CFR08V31::numberFeat(p8V31) * sizeof(Feat78)))
{
 setType(8);
 if (p8V31) {
   setModel((const char *)IString::stripBlanks(IString(p8V31->Model,sizeof(p8V31->Model))));
   setMachineType((const char *)IString::stripBlanks(IString(p8V31->Machine,sizeof(p8V31->Machine))));
   setModelQty(myantou(p8V31->ProductQty,sizeof(p8V31->ProductQty)));
   for (int i = 0, I = CFR08V31::numberFeat(p8V31); i < I ; i++)
     F7Q(&(p8V31->Feats[i])).copyTo(&((Record_08 *)record())->Feats[i]);
   }
}

    /*-----------------------------------------------------------------------*\
    |    const char * machineType(const char *Buf)                            |
    |       Return the pointer to the 08 Record's MachineType                 |
    |       for the given buffer.                                             |
    |       WARNING: will not point to a null-terminated string.              |
    \*-----------------------------------------------------------------------*/
const char * CFR08::machineType(const char *Buf)
{
 Record_08 *p08 = (Record_08 *)Buf;
 if (p08 && (strlen(Buf) >= (offsetof(Record_08,Machine)+1)))
   return p08->Machine;
 return "";
}

    /*-----------------------------------------------------------------------*\
    |    const char * model(const char *Buf)                                  |
    |       Return the pointer to the 08 Record's Model                       |
    |       for the given buffer.                                             |
    |       WARNING: will not point to a null-terminated string.              |
    \*-----------------------------------------------------------------------*/
const char * CFR08::model(const char *Buf)
{
 Record_08 *p08 = (Record_08 *)Buf;
 if (p08 && (strlen(Buf) >= (offsetof(Record_08,Model)+1)))
   return p08->Model;
 return "";
}

    /*-----------------------------------------------------------------------*\
    |    unsigned modelQty(const char * Buffer)                               |
    |       Return the 08 Record's Model Quantity                             |
    |       for the given buffer.                                             |
    \*-----------------------------------------------------------------------*/
unsigned CFR08::modelQty(const char * Buffer)
{
 Record_08 * p08 = (Record_08 *)Buffer;
 if (p08 && (strlen(Buffer) >=
             (offsetof(Record_08,ModelQty)+sizeof(p08->ModelQty))))
   return myantou(p08->ModelQty,sizeof(p08->ModelQty));
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setModel(CFRecV & rec,const char *szModel)                   |
    |       Set the given Model right-justified in the 08 Record.             |
    \*-----------------------------------------------------------------------*/
Boolean CFR08::setModel(CFRecV & rec,const char *szModel)
{
 Record_08 * p08 = (Record_08 *)rec.record();
 if (p08 && (rec.length() >=
             (offsetof(Record_08,Model)+sizeof(p08->Model)))) {
   CFRPaddCopySZ(p08->Model,szModel,sizeof(p08->Model));
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    CFR08 & setModel(const char *szModel)                                |
    |       Set the given Model right-justified.                              |
    \*-----------------------------------------------------------------------*/
CFR08 & CFR08::setModel(const char *szModel)
{
 CFRPaddCopySZ(((Record_08 *)record())->Model,szModel,sizeof(p8->Model));
 return *this;
}

    /*-----------------------------------------------------------------------*\
    |    CFR08 & setMachineType(const char *szModel)                          |
    |       Set the given Machine Type right-justified.                       |
    \*-----------------------------------------------------------------------*/
CFR08 & CFR08::setMachineType(const char *szType)
{
 CFRPaddCopySZ(((Record_08 *)record())->Machine,szType,sizeof(p8->Machine));
 return *this;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setModelQty(CFRecV & rec,unsigned qty)                       |
    |       Set the given Model Quantity right-justified in                   |
    |       the 08 Record.                                                    |
    \*-----------------------------------------------------------------------*/
Boolean CFR08::setModelQty(CFRecV & rec,unsigned qty)
{
 char szQty[20];
 Record_08 * p08 = (Record_08 *)rec.record();
 if (p08 && (rec.length() >=
             (offsetof(Record_08,ModelQty)+sizeof(p08->ModelQty)))) {
   CFRPaddCopySZ(p08->ModelQty,_itoa(qty,szQty,10),sizeof(p08->ModelQty));
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    CFR08 & setModelQty(unsigned qty)                                    |
    |       Set the given Model Quantity right-justified in                   |
    |       the 08 Record.                                                    |
    \*-----------------------------------------------------------------------*/
CFR08 & CFR08::setModelQty(unsigned qty)
{
 CFRPaddCopySZ(((Record_08 *)record())->ModelQty,(char *)IString(qty),sizeof(p8->ModelQty));
 return *this;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean edit(char * Buffer, int version, Tellem & tellem,            |
    |                 const char * FileName)                                  |
    |       Edit the 08 Record in the given buffer and report                 |
    |       descrepancies to the given file for the following                 |
    |       items:                                                            |
    |         Machine Quantity must be 0.                                     |
    |         Additional Quantity must be 0.                                  |
    |         Notes field must be '0'.                                        |
    |         Model Quantity must be 1.                                       |
    \*-----------------------------------------------------------------------*/
Boolean CFR08::edit(char * Buffer, int version, Tellem & tellem,
                    const char * FileName)
{
 Record_08 * p08 = (Record_08 *)Buffer;
 char Buf[QTY_SIZE+1];
 char * file = !FileName? (char *)FileName : "";
 Boolean fOK =
    (atoi(mystrstrip(mystrncpy(Buf,p08->MachineQty,sizeof(p08->MachineQty)))))?
        false: true;
 if (!fOK)
   tellem.say(false,EUO_MSG_25_MACH_QTY,file);
 if (p08->cAddQty != '0') {
   fOK = false;
   tellem.say(false,EUO_MSG_25_ADD_QTY,file);
   }
 if (p08->cNotes != '0') {
   fOK = false;
   tellem.say(false,EUO_MSG_25_NOTES,file);
   }
 if (atoi(mystrncpy(Buf,p08->ModelQty,sizeof(p08->ModelQty))) != 1) {
   fOK = false;
   tellem.say(false,EUO_MSG_25_MODEL_QTY,file);
   }
 if (!fOK)
   tellem.say(Buffer);
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    unsigned numberFeat(CFRecV & rec)                                    |
    |       Return the number of different features in the 08 Record.         |
    \*-----------------------------------------------------------------------*/
unsigned CFR08::numberFeat(CFRecV & rec)
{
 int FeatBytes = rec.length() - offsetof(Record_08,Feats);
 return (FeatBytes > 0) ? FeatBytes/sizeof(Feat78) : 0;
}

    /*-----------------------------------------------------------------------*\
    |    unsigned numberFeat(Record_08 * pRec)                                |
    |       Return the number of different features in the 08 Record.         |
    \*-----------------------------------------------------------------------*/
unsigned CFR08::numberFeat(Record_08 * pRec)
{
 if (!pRec)
  return 0;
 int FeatBytes = strlen((char *)pRec) - offsetof(Record_08,Feats);
 return (FeatBytes > 0) ? FeatBytes/sizeof(Feat78) : 0;
}

    /*-----------------------------------------------------------------------*\
    |    int featIndex(CFRecV & rec,const char * szFeature)                   |
    |       Return the 0-based index of the given feature in                  |
    |       the 08 Record or -1 when not found.                               |
    \*-----------------------------------------------------------------------*/
int CFR08::featIndex(CFRecV & rec,const char * szFeature)
{
 Record_08 * p08 = (Record_08 *)rec.record();
 for ( int i = 0, N = numberFeat(rec) ; i < N ; i ++) {
   F7Q fq(&(p08->Feats[i]));
   if (!strcmp(szFeature,fq.feature()))
     return i;
   }
 return -1;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean containsFeat(CFRecV & rec,const char * szFeature)            |
    |       Indicate whether the given feature exists in                      |
    |       the 08 Record.                                                    |
    \*-----------------------------------------------------------------------*/
Boolean CFR08::containsFeat(CFRecV & rec,const char * szFeature)
{
 return (featIndex(rec,szFeature) >= 0);
}

    /*-----------------------------------------------------------------------*\
    |    Feat78 * featPtr(CFRecV & rec,unsigned index)                        |
    |       Return the pointer to the 08 Record's feature record              |
    |       for the given feature index.                                      |
    \*-----------------------------------------------------------------------*/
Feat78 * CFR08::featPtr(CFRecV & rec,unsigned index)
{
 return (index < numberFeat(rec))?
           &(((Record_08 *)rec.record())->Feats[index]) : 0;
}

    /*-----------------------------------------------------------------------*\
    |    Feat78 * featPtr(CFRecV & rec,const char * szFeature)                |
    |       Return the pointer to the 08 Record's feature record              |
    |       for the given feature name.                                       |
    \*-----------------------------------------------------------------------*/
Feat78 * CFR08::featPtr(CFRecV & rec,const char * szFeature)
{
 Record_08 * p08 = (Record_08 *)rec.record();
 for ( int i = 0, N = numberFeat(rec) ; i < N ; i ++) {
   F7Q fq(&(p08->Feats[i]));
   if (!strcmp(szFeature,fq.feature()))
     return &(p08->Feats[i]);
   }
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean featQty(CFRecV & rec, const char * szFeature,                |
    |                    unsigned * pu)                                       |
    |       Set the quantity of the given feature in the given                |
    |       08 Record indicating succes/failure.                              |
    \*-----------------------------------------------------------------------*/
Boolean CFR08::featQty(CFRecV & rec, const char * szFeature, unsigned * pu)
{
 Feat78 * pF78 = featPtr(rec,szFeature);
 if (pF78) {
   F7Q fq(pF78);
   *pu = fq.quantity();
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean featParts(CFRecV & rec,const char * szFeature,                |
    |                     char * Target, unsigned * pu )                      |
    |       Copy the name and quantity of the given feature                   |
    |       in the given 08 Record indicating succes/failure.                 |
    \*-----------------------------------------------------------------------*/
Boolean CFR08::featParts(CFRecV & rec,const char * szFeature,
                         char * Target, unsigned * pu )
{
 Feat78 * pF78 = featPtr(rec,szFeature);
 if (pF78) {
   F7Q fq(pF78);
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
    |       in the given 08 Record indicating succes/failure.                 |
    \*-----------------------------------------------------------------------*/
Boolean CFR08::featParts(CFRecV &rec , unsigned Index, char * Target,
                         unsigned * pu )
{
 Feat78 * pF78 = featPtr(rec,Index);
 if (pF78) {
   F7Q fq(pF78);
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
    |       in the given 08 Record indicating succes/failure.                 |
    |       Note: failure is signaled for 08 Records which                    |
    |             have fewer features than the given Index.                   |
    \*-----------------------------------------------------------------------*/
Boolean CFR08::setFeatParts(CFRecV & rec, unsigned Index,unsigned qty,
                            char *szFeature)
{
 Feat78 * pF78 = featPtr(rec,Index);
 if (pF78) {
   F7Q fq(pF78);
   fq.setQuantity(qty);
   if (szFeature)
     fq.setFeature(szFeature);
   fq.copyTo(pF78);
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setFeatQty(CFRecV & rec,const char * szFeature,              |
    |                       unsigned qty )                                    |
    |       Set the given quantity of the given feature                       |
    |       in the given 08 Record indicating succes/failure.                 |
    |       Note: failure is signaled when the 08 Record                      |
    |             does not contain the given feature.                         |
    \*-----------------------------------------------------------------------*/
Boolean CFR08::setFeatQty(CFRecV & rec,const char * szFeature,unsigned qty )
{
 Feat78 * pF78 = featPtr(rec,szFeature);
 if (pF78) {
   F7Q fq(pF78);
   fq.setQuantity(qty);
   fq.copyTo(pF78);
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean stripOffFeat(CFRecV & rec,unsigned index)                    |
    |       Truncate the given 08 Record at the given feature index.          |
    |       i.e. if given n, the feature will be array element n-1.           |
    |       Note: This causes a replacement of the underlying                 |
    |             buffer which will unable to hold additional                 |
    |             feature elements.                                           |
    \*-----------------------------------------------------------------------*/
Boolean CFR08::stripOffFeat(CFRecV & rec,unsigned index)
{
 CFRecV temp(rec);
 Feat78 * pF78 = featPtr(temp,index);
 if (pF78) {
   *(char *)pF78 = '\0';
   rec = temp;
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    void appendFeat(CFRecV & rec, const char *szFeature,                 |
    |                    unsigned qty)                                        |
    |       Expand the 08 Record's feature array with an element              |
    |       for the given feature name and quantity.                          |
    |       Note: This causes a replacement of the underlying                 |
    |             buffer.                                                     |
    \*-----------------------------------------------------------------------*/
void CFR08::appendFeat(CFRecV & rec, const char *szFeature, unsigned qty)
{
 Feat78 f78;
 F7Q fq(szFeature,qty);
 fq.copyTo(&f78);
 CFRecV tmp(sizeof(f78),(char *)&f78);
 rec += tmp;
}

    /*-----------------------------------------------------------------------*\
    |    CFRecV * createANew( const char *szMachine ,                         |
    |                         const char *szModel ,char cProdInd)             |
    |       'New' an 08 Record for the given data.                            |
    \*-----------------------------------------------------------------------*/
CFRecV * CFR08::createANew( const char *szMachine
                           ,const char *szModel
                           ,char cProdInd)
{
 CFRecV * p = new CFRecV(offsetof(Record_08,Feats));
 Record_08 * p08 = (Record_08 *)(p->record());
 p->setType(8);
 CFRPaddCopySZ(p08->Machine,szMachine,sizeof(p08->Machine));
 CFRPaddCopySZ(p08->MachineQty,"0",sizeof(p08->MachineQty));
 p08->cAddQty = p08->cNotes = '0';
 p08->cIPProd = cProdInd;
 CFRPaddCopySZ(p08->Model,szModel,sizeof(p08->Model));
 CFRPaddCopySZ(p08->ModelQty,"1",sizeof(p08->ModelQty));
 return p;
}

////////////////////////////////////////////////////////////

 /*--------------------------------------------------------------------------*\
 |                               CFR08V31 Class                               |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |                        Ctors                        |
        \*---------------------------------------------------*/

CFR08V31::CFR08V31()
      : CFRecV(offsetof(Record_08V31,Feats))
{
 setType(8);
}

CFR08V31::CFR08V31(Record_08  * p8)
      : CFRecV(offsetof(Record_08V31,Feats) + (CFR08::numberFeat(p8) * sizeof(Feat75)))
{
 setType(8);
 if (p8) {
   setModel((const char *)IString::stripBlanks(IString(p8->Model,sizeof(p8->Model))));
   setMachineType((const char *)IString::stripBlanks(IString(p8->Machine,sizeof(p8->Machine))));
   setProductQty(myantou(p8->ModelQty,sizeof(p8->ModelQty)));
   for (int i = 0, I = CFR08::numberFeat(p8); i < I ; i++)
     F7Q(&(p8->Feats[i])).copyTo(&((Record_08V31 *)record())->Feats[i]);
   }
}

    /*-----------------------------------------------------------------------*\
    |    const char * machineType(const char *Buf)                            |
    |       Return the pointer to the 08 Record's MachineType                 |
    |       for the given buffer.                                             |
    |       WARNING: will not point to a null-terminated string.              |
    \*-----------------------------------------------------------------------*/
const char * CFR08V31::machineType(const char *Buf)
{
 Record_08V31 *p08 = (Record_08V31 *)Buf;
 if (p08 && (strlen(Buf) >= (offsetof(Record_08V31,Machine)+1)))
   return p08->Machine;
 return "";
}

    /*-----------------------------------------------------------------------*\
    |    const char * model(const char *Buf)                                  |
    |       Return the pointer to the 08 Record's Model                       |
    |       for the given buffer.                                             |
    |       WARNING: will not point to a null-terminated string.              |
    \*-----------------------------------------------------------------------*/
const char * CFR08V31::model(const char *Buf)
{
 Record_08V31 *p08 = (Record_08V31 *)Buf;
 if (p08 && (strlen(Buf) >= (offsetof(Record_08V31,Model)+1)))
   return p08->Model;
 return "";
}

    /*-----------------------------------------------------------------------*\
    |          unsigned productQty(const char * Buffer)                       |
    |             Return the 08 Record's Product Quantity                     |
    |             for the given buffer.                                       |
    \*-----------------------------------------------------------------------*/
unsigned CFR08V31::productQty(const char * Buffer)
{
 Record_08V31 * p08 = (Record_08V31 *)Buffer;
 if (p08 && (strlen(Buffer) >=
             (offsetof(Record_08V31,ProductQty)+sizeof(p08->ProductQty))))
   return myantou(p08->ProductQty,sizeof(p08->ProductQty));
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |       Boolean setModel(CFRecV & rec,const char *szModel)                |
    |           Set the given Model right-justified in the 08 Record.         |
    \*-----------------------------------------------------------------------*/
Boolean CFR08V31::setModel(CFRecV & rec,const char *szModel)
{
 Record_08V31 * p08 = (Record_08V31 *)rec.record();
 if (p08 && (rec.length() >=
             (offsetof(Record_08V31,Model)+sizeof(p08->Model)))) {
   CFRPaddCopySZ(p08->Model,szModel,sizeof(p08->Model));
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    CFR08V31 & setModel(const char *szModel)                             |
    |       Set the given Model right-justified.                              |
    \*-----------------------------------------------------------------------*/
CFR08V31 & CFR08V31::setModel(const char *szModel)
{
 CFRPaddCopySZ(((Record_08V31 *)record())->Model,szModel,sizeof(p8V31->Model));
 return *this;
}

    /*-----------------------------------------------------------------------*\
    |    CFR08V31 & setMachineType(const char *szModel)                       |
    |       Set the given Machine Type right-justified.                       |
    \*-----------------------------------------------------------------------*/
CFR08V31 & CFR08V31::setMachineType(const char *szType)
{
 CFRPaddCopySZ(((Record_08V31 *)record())->Machine,szType,sizeof(p8V31->Machine));
 return *this;
}

    /*-----------------------------------------------------------------------*\
    |       Boolean setProductQty(CFRecV & rec,unsigned qty)                  |
    |       Set the given Product Quantity right-justified in                 |
    |       the 08 Record.                                                    |
    \*-----------------------------------------------------------------------*/
Boolean CFR08V31::setProductQty(CFRecV & rec,unsigned qty)
{
 char szQty[20];
 Record_08V31 * p08 = (Record_08V31 *)rec.record();
 if (p08 && (rec.length() >=
             (offsetof(Record_08V31,ProductQty)+sizeof(p08->ProductQty)))) {
   CFRPaddCopySZ(p08->ProductQty,_itoa(qty,szQty,10),sizeof(p08->ProductQty));
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    CFR08V31 & setProductQty(unsigned qty)                               |
    |       Set the given Product Quantity right-justified in                 |
    |       the 08 Record.                                                    |
    \*-----------------------------------------------------------------------*/
CFR08V31 & CFR08V31::setProductQty(unsigned qty)
{
 CFRPaddCopySZ(((Record_08V31 *)record())->ProductQty,(char *)IString(qty),sizeof(p8V31->ProductQty));
 return *this;
}

    /*-----------------------------------------------------------------------*\
    |          Boolean edit(char * Buffer, int version, Tellem & tellem,      |
    |                       const char * FileName)                            |
    |             Edit the 08 Record in the given buffer and report           |
    |             descrepancies to the given file for the following           |
    |             items:                                                      |
    |               Product Quantity must be 1.                               |
    \*-----------------------------------------------------------------------*/
Boolean CFR08V31::edit(char * Buffer, int version, Tellem & tellem,
                    const char * FileName)
{
 Record_08V31 * p08 = (Record_08V31 *)Buffer;
 char Buf[QTY_SIZE+1];
 char * file = !FileName? (char *)FileName : "";

 Boolean fOK = true;
 if (atoi(mystrncpy(Buf,p08->ProductQty,sizeof(p08->ProductQty))) != 1) {
   fOK = false;
   tellem.say(false,EUO_MSG_25_MODEL_QTY,file);
   }
 if (!fOK)
   tellem.say(Buffer);
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |          unsigned numberFeat(CFRecV & rec)                              |
    |             Return the number of different features in the 08 Record.   |
    \*-----------------------------------------------------------------------*/
unsigned CFR08V31::numberFeat(CFRecV & rec)
{
 int FeatBytes = rec.length() - offsetof(Record_08V31,Feats);
 return (FeatBytes > 0) ? FeatBytes/sizeof(Feat75) : 0;
}

    /*-----------------------------------------------------------------------*\
    |          unsigned numberFeat(Record_08V31 * pRec)                       |
    |             Return the number of different features in the 08 Record.   |
    \*-----------------------------------------------------------------------*/
unsigned CFR08V31::numberFeat(Record_08V31 * pRec)
{
 if (!pRec)
   return 0;
 int FeatBytes = strlen((char *)pRec) - offsetof(Record_08V31,Feats);
 return (FeatBytes > 0) ? FeatBytes/sizeof(Feat75) : 0;
}

    /*-----------------------------------------------------------------------*\
    |          int featIndex(CFRecV & rec,const char * szFeature)             |
    |             Return the 0-based index of the given feature in            |
    |             the 08 Record or -1 when not found.                         |
    \*-----------------------------------------------------------------------*/
int CFR08V31::featIndex(CFRecV & rec,const char * szFeature)
{
 Record_08V31 * p08 = (Record_08V31 *)rec.record();
 for ( int i = 0, N = numberFeat(rec) ; i < N ; i ++) {
   F7Q fq(&(p08->Feats[i]));
   if (!strcmp(szFeature,fq.feature()))
     return i;
   }
 return -1;
}

    /*-----------------------------------------------------------------------*\
    |          Boolean containsFeat(CFRecV & rec,const char * szFeature)      |
    |             Indicate whether the given feature exists in                |
    |             the 08 Record.                                              |
    \*-----------------------------------------------------------------------*/
Boolean CFR08V31::containsFeat(CFRecV & rec,const char * szFeature)
{
 return (featIndex(rec,szFeature) >= 0);
}

    /*-----------------------------------------------------------------------*\
    |      Feat75 * featPtr(CFRecV & rec,unsigned index)                      |
    |      Return the pointer to the 08 Record's feature record               |
    |      for the given feature index.                                       |
    \*-----------------------------------------------------------------------*/
Feat75 * CFR08V31::featPtr(CFRecV & rec,unsigned index)
{
 return (index < numberFeat(rec))?
           &(((Record_08V31 *)rec.record())->Feats[index]) : 0;
}

    /*-----------------------------------------------------------------------*\
    |      Feat75 * featPtr(CFRecV & rec,const char * szFeature)              |
    |      Return the pointer to the 08 Record's feature record               |
    |      for the given feature name.                                        |
    \*-----------------------------------------------------------------------*/
Feat75 * CFR08V31::featPtr(CFRecV & rec,const char * szFeature)
{
 Record_08V31 * p08 = (Record_08V31 *)rec.record();
 for ( int i = 0, N = numberFeat(rec) ; i < N ; i ++) {
   F7Q fq(&(p08->Feats[i]));
   if (!strcmp(szFeature,fq.feature()))
     return &(p08->Feats[i]);
   }
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |          Boolean featQty(CFRecV & rec, const char * szFeature,          |
    |                          unsigned * pu)                                 |
    |             Set the quantity of the given feature in the given          |
    |             08 Record indicating succes/failure.                        |
    \*-----------------------------------------------------------------------*/
Boolean CFR08V31::featQty(CFRecV & rec, const char * szFeature, unsigned * pu)
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
    |         Boolean featParts(CFRecV & rec,const char * szFeature,          |
    |                           char * Target, unsigned * pu )                |
    |             Copy the name and quantity of the given feature             |
    |             in the given 08 Record indicating succes/failure.           |
    \*-----------------------------------------------------------------------*/
Boolean CFR08V31::featParts(CFRecV & rec,const char * szFeature,
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
    |          Boolean featParts(CFRecV &rec , unsigned Index, char * Target, |
    |                            unsigned * pu )                              |
    |             Copy the feature name and quantity of the 'Index'-th        |
    |             element of the feature array                                |
    |             in the given 08 Record indicating succes/failure.           |
    \*-----------------------------------------------------------------------*/
Boolean CFR08V31::featParts(CFRecV &rec , unsigned Index, char * Target,
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

    /*------------------------------------------------------------------------*\
    |          Boolean setFeatParts(CFRecV & rec, unsigned Index,unsigned qty, |
    |                               char *szFeature)                           |
    |             Set the given feature name and quantity of the 'Index'-th    |
    |             element of the feature array                                 |
    |             in the given 08 Record indicating succes/failure.            |
    |             Note: failure is signaled for 08 Records which               |
    |                   have fewer features than the given Index.              |
    \*------------------------------------------------------------------------*/
Boolean CFR08V31::setFeatParts(CFRecV & rec, unsigned Index,unsigned qty,
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
    |          Boolean setFeatQty(CFRecV & rec,const char * szFeature,        |
    |                             unsigned qty )                              |
    |             Set the given quantity of the given feature                 |
    |             in the given 08 Record indicating succes/failure.           |
    |             Note: failure is signaled when the 08 Record                |
    |                   does not contain the given feature.                   |
    \*-----------------------------------------------------------------------*/
Boolean CFR08V31::setFeatQty(CFRecV & rec,const char * szFeature,unsigned qty )
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
    |          Boolean stripOffFeat(CFRecV & rec,unsigned index)              |
    |             Truncate the given 08 Record at the given feature index.    |
    |             i.e. if given n, the feature will be array element n-1.     |
    |             Note: This causes a replacement of the underlying           |
    |                   buffer which will unable to hold additional           |
    |                   feature elements.                                     |
    \*-----------------------------------------------------------------------*/
Boolean CFR08V31::stripOffFeat(CFRecV & rec,unsigned index)
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
    |      void appendFeat(CFRecV & rec, const char *szFeature,               |
    |                      unsigned qty)                                      |
    |         Expand the 08 Record's feature array with an element            |
    |         for the given feature name and quantity.                        |
    |         Note: This causes a replacement of the underlying               |
    |               buffer.                                                   |
    \*-----------------------------------------------------------------------*/
void CFR08V31::appendFeat(CFRecV & rec, const char *szFeature, unsigned qty)
{
 Feat75 f75;
 F7Q fq(szFeature,qty);
 fq.copyTo(&f75);
 CFRecV tmp(sizeof(f75),(char *)&f75);
 rec += tmp;
}

    /*-----------------------------------------------------------------------*\
    |         CFRecV * createANew( const char *szMachine ,                    |
    |                              const char *szModel )                      |
    |            'New' an 08 Record for the given data.                       |
    \*-----------------------------------------------------------------------*/
                                                 // NK
CFRecV * CFR08V31::createANew( const char *szMachine 
                              ,const char *szModel )
{
 CFRecV * p = new CFRecV(offsetof(Record_08V31,Feats));
 Record_08V31 * p08 = (Record_08V31 *)(p->record());
 p->setType(8);
 CFRPaddCopySZ(p08->Machine,szMachine,sizeof(p08->Machine));
 CFRPaddCopySZ(p08->Model,szModel,sizeof(p08->Model));
 CFRPaddCopySZ(p08->ProductQty,"1",sizeof(p08->ProductQty));
 return p;
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
    FILE *f;
     ofstream emsg;
     emsg.open ("CFR08.MSG");
    CFRecV rec1("089021       2111      500       1   9063       1   9922       3");
    CFRecV rec2("089022       0000      01A       1   9063       2   9110       2   9174       1   9369       1");
    CFRecV rec3("089023       0000      001       1   9063       1   9174       3   9366       1   9369       2   9922       2");
    CFRecV rec4("089028       0000      500       1   2936       2   773        1   774        1   775        1   9063       1  9922        2");
    CFRecV rec5("089029       0000      500       1   773        1   1111       3");
    char Target[50];
    unsigned u;
    int i;

    cout <<  "rec1 : " << rec1 << endl;
    cout <<  "rec2 : " << rec2 << endl;
    cout <<  "rec3 : " << rec3 << endl;
    cout <<  "rec4 : " << rec4 << endl;
    cout <<  "rec5 : " << rec5 << endl;
    i = CFR08::featParts(rec3,"   9174",Target,&u);
    cout << "featParts(rec3,'   9174') :" << i << endl;
    if (i)
      cout << "Target : '" << Target << "' qty : " << u << endl;
    i = CFR08::featParts(rec2,3,Target,&u);
    cout << "featParts(rec2,3) :" << i << endl;
    if (i)
      cout << "Target : '" << Target << "' qty : " << u << endl;
    i = CFR08::featParts(rec2,4,Target,&u);
    cout << "featParts(rec2,4) :" << i << endl;
    if (i)
      cout << "Target : '" << Target << "' qty : " << u << endl;

    cout << "rec2.setFeatQty('   9110',99)" << endl;
    CFR08::setFeatQty(rec2,"   9110",99);
    cout << "rec2.appendFeat('4444',44)" << endl;
    CFR08::appendFeat(rec2,"4444",44);
    rec2.setType(44);
    cout <<  "rec2.type() : " << rec2.type() << endl;
    cout <<  "rec2 : " << rec2 << endl;
    cout << "rec5.stripOffFeat(1)" << endl;
    CFR08::stripOffFeat(rec5,1);
    cout <<  "rec5 : " << rec5 << endl;
//    CFR08::edit(rec1,"filename ",emsg);
//    emsg.close();

    unsigned Bytes = 0, Total = 0;
    Total += rec1.fWriteBLength();
    Total += rec2.fWriteBLength();
    Total += rec3.fWriteBLength();
    Total += rec4.fWriteBLength();
    Total += rec5.fWriteBLength();
    cout << endl << "Total fWriteBLength() : " << Total << endl;
    Total = 0;

    if (f = fopen("CFR08.TST","wb"))
      {
        if (rec1.fWriteB(f,&Bytes))
          Total += Bytes;
        if (rec2.fWriteB(f,&Bytes))
          Total += Bytes;
        if (rec3.fWriteB(f,&Bytes))
          Total += Bytes;
        if (rec4.fWriteB(f,&Bytes))
          Total += Bytes;
        if (rec5.fWriteB(f,&Bytes))
          Total += Bytes;
        cout << endl << "Total bytes written : " << Total  << endl;
        fclose(f);
        if (f = fopen("CFR08.TST","rb"))
          {
           CFR08 rec11;
           CFR08 rec12;
           CFR08 rec13;
           CFR08 rec14;
           CFR08 rec15;
           Total = 0;
           if (rec5.fReadB(f,&Bytes))
              Total += Bytes;
           if (rec4.fReadB(f,&Bytes))
              Total += Bytes;
           if (rec3.fReadB(f,&Bytes))
              Total += Bytes;
           if (rec2.fReadB(f,&Bytes))
              Total += Bytes;
           if (rec1.fReadB(f,&Bytes))
              Total += Bytes;
           fclose(f);
           cout << endl << "Total bytes read : " << Total  << endl;
           cout <<  "rec5 : " << rec5 << endl;
           cout <<  "rec4 : " << rec4 << endl;
           cout <<  "rec3 : " << rec3 << endl;
           cout <<  "rec2 : " << rec2 << endl;
           cout <<  "rec1 : " << rec1 << endl;
           if (f = fopen("CFR08.TOT","w"))
             {
               rec5.fWriteA(f);
               rec4.fWriteA(f);
               rec3.fWriteA(f);
               rec2.fWriteA(f);
               rec1.fWriteA(f);
               fclose(f);
             }
          }
      }
                                                 // ///////////V31/////////////-
                                                 // //////////////////


     emsg.open ("CFR08V31.MSG");
                                                 /* ************************-*/
                                                 /* ************************-*/
                                                 /* ************             */
                                                 /* rec type 2, machine 4,   */
                                                 /* cfoutflag 1, model 3,    */
                                                 /* prod Qty 5,              */
                                                 /* feature 7, Qty 5 (with   */
                                                 /* the last 2 fields        */
                                                 /* repeating)               */
                                                 /* ************************-*/
                                                 /* ************************-*/
                                                 /* ************             */
  cout << "Record Layout" << endl;
  cout << "-------------" << endl;
  cout << "Rec Type | Mach Type | CFOut Flg | Model | Prod Qty |  Ftr  | Qty " << endl;
  cout << "--------   ---------   ---------   -----   --------    ---    --- " << endl;
  cout << "08         9021        blank       500            1    9063     3 " << endl;
  cout << "__________________________________________________________________" << endl << endl;
    CFRecV rec1("089021 500    19063       19922       3");
    CFRecV rec2("089021 01A    19063       39174       2");
    CFRecV rec3("089021 001    12936       1773        1");
    CFRecV rec4("089021 500    1773        21111       2");
    char Target[50];
    unsigned u;
    int i;

    cout <<  "********************************************************" << endl << endl;
    cout <<  "** rec1 : " << rec1  << endl;
    cout <<  "** rec2 : " << rec2  << endl;
    cout <<  "** rec3 : " << rec3  << endl;
    cout <<  "** rec4 : " << rec4  << endl;
    cout <<  "********************************************************" << endl << endl;
    i = CFR08V31::featParts(rec2,"9174   ",Target,&u);
    cout << "featParts(rec2,'9174   ') :" << i << endl;
    if (i)
      cout << "Target : '" << Target << "' qty : " << u << endl;
    i = CFR08V31::featParts(rec2,3,Target,&u);
    cout << "featParts(rec2,3) :" << i << endl;
    if (i)
      cout << "Target : '" << Target << "' qty : " << u << endl;
    i = CFR08V31::featParts(rec2,4,Target,&u);
    cout << "featParts(rec2,4) :" << i << endl;
    if (i)
      cout << "Target : '" << Target << "' qty : " << u << endl;

    cout << "rec4.setFeatQty('1111   ',99)" << endl;
    CFR08V31::setFeatQty(rec4,"1111   ",99);

    cout << "rec2.appendFeat('4444   ',44)" << endl;
    CFR08V31::appendFeat(rec2,"4444   ",44);
    rec2.setType(44);
    cout <<  "rec2.type() : " << rec2.type() << endl;
    cout <<  "rec2 : " << rec2 << endl;
//    cout << "rec4.stripOffFeat(1)" << endl;
//    CFR08V31::stripOffFeat(rec4,1);
    cout <<  "rec4 : " << rec4 << endl;
    cout <<  "rec2.machineType() : " << CFR08V31::machineType(rec2.record()) << endl << endl;
    cout <<  "rec2.model() : " << CFR08V31::model(rec2.record()) << endl << endl;
//    CFR08V31::edit(rec1,"filename ",emsg);
    emsg.close();

    cout <<  "********************************************" << endl;
    unsigned Bytes = 0, Total = 0;
    Total += rec1.fWriteBLength();
    Total += rec2.fWriteBLength();
    Total += rec3.fWriteBLength();
    Total += rec4.fWriteBLength();
    cout << endl << "Total fWriteBLength() : " << Total << endl;
    Total = 0;

    if (f = fopen("CFR08V31.TST","wb"))
      {
        if (rec1.fWriteB(f,&Bytes))
          Total += Bytes;
        if (rec2.fWriteB(f,&Bytes))
          Total += Bytes;
        if (rec3.fWriteB(f,&Bytes))
          Total += Bytes;
        if (rec4.fWriteB(f,&Bytes))
          Total += Bytes;
        cout << endl << "Total bytes written : " << Total  << endl;
        fclose(f);
        if (f = fopen("CFR08V31.TST","rb"))
          {
           CFR08V31 rec11;
           CFR08V31 rec12;
           CFR08V31 rec13;
           CFR08V31 rec14;
           Total = 0;
           if (rec4.fReadB(f,&Bytes))
              Total += Bytes;
           if (rec3.fReadB(f,&Bytes))
              Total += Bytes;
           if (rec2.fReadB(f,&Bytes))
              Total += Bytes;
           if (rec1.fReadB(f,&Bytes))
              Total += Bytes;
           fclose(f);
           cout << endl << "Total bytes read : " << Total  << endl;
           cout <<  "rec4 : " << rec4 << endl;
           cout <<  "rec3 : " << rec3 << endl;
           cout <<  "rec2 : " << rec2 << endl;
           cout <<  "rec1 : " << rec1 << endl;
    cout <<  "********************************************" << endl;
           if (f = fopen("CFR08V31.TOT","w"))
             {
               rec4.fWriteA(f);
               rec3.fWriteA(f);
               rec2.fWriteA(f);
               rec1.fWriteA(f);
               fclose(f);
             }
          }

      }


   }

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif

}

#endif

