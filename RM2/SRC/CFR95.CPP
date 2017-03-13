 /*--------------------------------------------------------------------------*\
 |    Filename - CFR08.CPP                                                    |
 |      contains source for the following RM/2 class implementations:         |
 |                                                                            |
 |       CFR54  - To access the CFReport's Record Type 54                     |
 |                as defined in 'euopdef.h'                                   |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |              Standard Library Includes              |
        \*---------------------------------------------------*/
#include <string.h>
#include <stddef.h>

        /*---------------------------------------------------*\
        |                  ICLUI's Includes                   |
        \*---------------------------------------------------*/
#include <isynonym.hpp>                          // Definition of Boolean etc
#include <istring.hpp>

#include "cfr54.hpp"
#include "cfr95.hpp"                             // this file's include

Record_95 * p95;

        /*---------------------------------------------------*\
        |                        Ctors                        |
        \*---------------------------------------------------*/
CFR95::CFR95()
      : CFRecV(sizeof(Record_95))
{
 setType(95);
}

CFR95::CFR95(Record_54 * p54)
      : CFRecV(sizeof(Record_95))
{
 setType(95);
 if (p54) {
   setProductType(p54->MachType);
   //setProductModel(p54->MachMod);  /* F73732 EDW 07/25/2002 */
   setProductModel(p54->MachMod+4);  /* F73732 EDW 07/25/2002 */
   setSerialNo(p54->SerialNo);
   p95 = (Record_95 *)record();
   int len54 = strlen((char *)p54);
   if (len54 > offsetof(Record_54,custnum))
     memcpy(p95->CustomerNo           ,p54->custnum  ,ALMIN(sizeof(p54->custnum  ),sizeof(p95->CustomerNo           )));
   if (len54 > offsetof(Record_54,refmach))
     memcpy(p95->DesignatedCPUType    ,p54->refmach  ,ALMIN(sizeof(p54->refmach  ),sizeof(p95->DesignatedCPUType    )));
   if (len54 > offsetof(Record_54,euoprso))
     memcpy(p95->DesignatedCPUSer_Ord ,p54->euoprso  ,ALMIN(sizeof(p54->euoprso  ),sizeof(p95->DesignatedCPUSer_Ord )));
   if (len54 > offsetof(Record_54,CradDate))
     memcpy(p95->CradDate             ,p54->CradDate ,ALMIN(sizeof(p54->CradDate ),sizeof(p95->CradDate             )));
   if (len54 > offsetof(Record_54,ordernum))
     memcpy(p95->OrderNo              ,p54->ordernum ,ALMIN(sizeof(p54->ordernum ),sizeof(p95->OrderNo              )));
   if (len54 > offsetof(Record_54,Sequence))
     memcpy(p95->SequenceField        ,p54->Sequence ,ALMIN(sizeof(p54->Sequence ),sizeof(p95->SequenceField        )));
   if (len54 > offsetof(Record_54,InstDate))
     CFR54::setCFRValiDateYYYYMMDD(p95->InstallDate,CFR54::cfrDateYMD(p54->InstDate));
   if (len54 > offsetof(Record_54,stsqflag))
     memcpy(p95->StepSeqFlagForSW     ,p54->stsqflag ,ALMIN(sizeof(p54->stsqflag ),sizeof(p95->StepSeqFlagForSW     )));
   if (len54 > offsetof(Record_54,QualText))
     setDescription(p54->QualText);
   if (len54 > offsetof(Record_54,SorcFlag))
      *p95->SourceFlag = p54->SorcFlag;
   }
}

    /*-----------------------------------------------------------------------*\
    |  CFRecV * createANew()                                                  |
    |       'New' a record from the given components                          |
    \*-----------------------------------------------------------------------*/
CFRecV * CFR95::createANew( const char *szType
                           ,const char *szModel
                           ,const char *szSerialNo)
{
 CFRecV * p = new CFRecV(sizeof(Record_95));
 p->setType(95);
 setProductType(p,szType);
 setProductModel(p,szModel);
 setSerialNo(p,szSerialNo);
 return p;
}

    /*-----------------------------------------------------------------------*\
    |  Boolean  setProductType(CFRecV * pRec, const char *szType)             |
    |       set the Product's Type in the record                              |
    |       if record's long enough                                           |
    \*-----------------------------------------------------------------------*/
Boolean  CFR95::setProductType(CFRecV * pRec, const char *szType)
{
 if (pRec && szType && (pRec->type()==95) && (p95 = (Record_95 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_95,ProductType)+sizeof(p95->ProductType)))) {
   CFRPaddCopySZ(p95->ProductType,szType,sizeof(p95->ProductType));
   return true;
   }
 return false;
}

CFR95 & CFR95::setProductType(const char *szType)
{
 CFRPaddCopySZ(((Record_95 *)record())->ProductType,szType,sizeof(p95->ProductType));
 return *this;
}

CFR95 & CFR95::setProductModel(const char *szModel)
{
 CFRPaddCopySZ(((Record_95 *)record())->ProductModel,szModel,sizeof(p95->ProductModel));
 return *this;
}

    /*-----------------------------------------------------------------------*\
    |  Boolean  setProductModel(CFRecV * pRec, const char *szModel)           |
    |       set the Machine's Model in the record                             |
    |       if record's long enough                                           |
    \*-----------------------------------------------------------------------*/
Boolean  CFR95::setProductModel(CFRecV * pRec, const char *szModel)
{
 if (pRec && szModel && (pRec->type()==95) && (p95 = (Record_95 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_95,ProductModel)+sizeof(p95->ProductModel)))) {
   CFRPaddCopySZ(p95->ProductModel,szModel,sizeof(p95->ProductModel));
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   const char * serialNo(const char * Buf)                               |
    |       return the pointer to the record's serialNo                       |
    |       which is NOT null-terminated                                      |
    \*-----------------------------------------------------------------------*/
const char * CFR95::serialNo(const char * Buf)
{
 if ((p95 = (Record_95 *)Buf) && (CFRecType(Buf)==95) &&
     (strlen(Buf) >= (offsetof(Record_95,SerialNo)+sizeof(p95->SerialNo))))
   return p95->SerialNo;
 return "";
}

    /*-----------------------------------------------------------------------*\
    |  Boolean  setSerialNo(CFRecV * pRec, const char *szNo)                  |
    |       set the Serial # in the record                                    |
    |       if record's long enough                                           |
    \*-----------------------------------------------------------------------*/
Boolean  CFR95::setSerialNo(CFRecV * pRec, const char *szNo)
{
 if (pRec && szNo && (pRec->type()==95) && (p95 = (Record_95 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_95,SerialNo)+sizeof(p95->SerialNo)))) {
   CFRPaddCopySZ(p95->SerialNo,szNo,sizeof(p95->SerialNo));
   return true;
   }
 return false;
}

CFR95 & CFR95::setSerialNo(const char *szNo)
{
 CFRPaddCopySZ(((Record_95 *)record())->SerialNo,szNo,sizeof(p95->SerialNo));
 return *this;
}

    /*-----------------------------------------------------------------------*\
    |  Boolean  setDescription(CFRecV * pRec, const char *Desc)               |
    |       set the  description (stripped) in the record                     |
    |       if record's long enough                                           |
    \*-----------------------------------------------------------------------*/
Boolean  CFR95::setDescription(CFRecV * pRec, const char *Desc)
{
 if (pRec && Desc && (pRec->type()==95) && (p95 = (Record_95 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_95,Description)+sizeof(p95->Description)))) {
   mystrstrip(mystrncpy(p95->Description,Desc,sizeof(p95->Description)-1));
   return true;
   }
 return false;
}

CFR95 & CFR95::setDescription(const char *Desc)
{
 mystrstrip(mystrncpy(((Record_95 *)record())->Description,Desc,sizeof(p95->Description)));
 return *this;
}

    /*-----------------------------------------------------------------------*\
    |  const char * description(CFRecV * pRec)                                |
    |       return the pointer to the record's description string             |
    \*-----------------------------------------------------------------------*/
const char * CFR95::description(CFRecV * pRec)
{
 if (pRec && (pRec->type()==95) && (p95 = (Record_95 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_95,Description)+1)))
   return p95->Description;
 return "";
}

IString CFR95::descriptionString()
{
 return IString(((Record_95 *)record())->Description);
}

Boolean CFR95::description(CFRecV * pRec,IString & str)
{
 str = IString(description(pRec));
 return (str != IString());
}

    /*-----------------------------------------------------------------------*\
    |   Boolean CFR95::customerNumber(CFRecV * pRec,IString & str)            |
    |       set the given IString to the record's customer number             |
    |  New function:  D83402    HECC - 07/28/2003                             |
    \*-----------------------------------------------------------------------*/

Boolean CFR95::customerNumber(CFRecV * pRec,IString & str)
{
 str = IString(customerNumber(pRec));
 str = str.subString(1,7);
 return (str != IString());
}
    /*-----------------------------------------------------------------------*\
    |   const char * CFR95::customerNumber(CFRecV * pRec)                     |
    |       return the pointer to the record's customer number string         |
    |  New function:  D83402    HECC - 07/28/2003                             |
    \*-----------------------------------------------------------------------*/

const char * CFR95::customerNumber(CFRecV * pRec)
{
 if (pRec && (pRec->type()==95) && (p95 = (Record_95 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_95,CustomerNo)+1)))
   return p95->CustomerNo;
 return "";
}

    /*-----------------------------------------------------------------------*\
    |   Boolean CFR95::systNumberFlag(CFRecV * pRec,IString & str)            |
    |       set the given IString to the record's system number flag          |
    |  New function:  D83402    HECC - 07/28/2003                             |
    \*-----------------------------------------------------------------------*/

Boolean CFR95::systNumberFlag(CFRecV * pRec,IString & str)
{
 str = IString(systNumberFlag(pRec));
 str = str.subString(1,1);
 return (str != IString());
}
    /*-----------------------------------------------------------------------*\
    |   const char * CFR95::systNumberFlag(CFRecV * pRec)                     |
    |       return the pointer to the record's system number flag             |
    |  New function:  D83402    HECC - 07/28/2003                             |
    \*-----------------------------------------------------------------------*/

const char * CFR95::systNumberFlag(CFRecV * pRec)
{
 if (pRec && (pRec->type()==95) && (p95 = (Record_95 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_95,SystemNoFlag)+1)))
   return p95->SystemNoFlag;
 return "";
}

    /*-----------------------------------------------------------------------*\
    |   Boolean CFR95::systemType(CFRecV * pRec,IString & str)                |
    |       set the given IString to the record's system type number          |
    |  New function:  D83402    HECC - 07/28/2003                             |
    \*-----------------------------------------------------------------------*/

Boolean CFR95::systemType(CFRecV * pRec,IString & str)
{
 str = IString(systemType(pRec));
 str = str.subString(1,4);
 return (str != IString());
}
    /*-----------------------------------------------------------------------*\
    |   const char * CFR95::systemType(CFRecV * pRec)                         |
    |       return the pointer to the record's system type number             |
    |  New function:  D83402    HECC - 07/28/2003                             |
    \*-----------------------------------------------------------------------*/

const char * CFR95::systemType(CFRecV * pRec)
{
 if (pRec && (pRec->type()==95) && (p95 = (Record_95 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_95,SystemType)+1)))
   return p95->SystemType;
 return "";
}

    /*-----------------------------------------------------------------------*\
    |   Boolean CFR95::systemNum(CFRecV * pRec,IString & str)                 |
    |       set the given IString to the record's system number               |
    |  New function:  D83402    HECC - 07/28/2003                             |
    \*-----------------------------------------------------------------------*/

Boolean CFR95::systemNum(CFRecV * pRec,IString & str)
{
 str = IString(systemNum(pRec));
 str = str.subString(1,7);
 return (str != IString());
}
    /*-----------------------------------------------------------------------*\
    |   const char * CFR95::systemNum(CFRecV * pRec)                          |
    |       return the pointer to the record's system number                  |
    |  New function:  D83402    HECC - 07/28/2003                             |
    \*-----------------------------------------------------------------------*/

const char * CFR95::systemNum(CFRecV * pRec)
{
 if (pRec && (pRec->type()==95) && (p95 = (Record_95 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_95,SystemNo)+1)))
   return p95->SystemNo;
 return "";
}


Boolean  CFR95::setNonSoftwareRecordLength(CFRecV * pRec)
{
 if (pRec && (pRec->type()==95) && (p95 = (Record_95 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_95,Description)+45))) {
   p95->Description[45] = '\0';
   return true;
   }
 return false;
}

int CFR95::fWriteCFR(FILE * f,CFRecV * pRec,Boolean fSoftware)
{
 if (pRec && f)
   if (!fSoftware) {
      CFRecV rec(*pRec);
      setNonSoftwareRecordLength(&rec);
      return rec.fWriteCFR(f)? 1 : -1;
      }
   else return pRec->fWriteCFR(f)? 1 : -1;
 else return 0;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean installDate(CFRecV * pRec,IDate & date)                       |
    |       set the Install date from the given 95-Record,                    |
    |       returning false if not available                                  |
    \*-----------------------------------------------------------------------*/
Boolean CFR95::installDate(CFRecV *pRec,IDate & date)
{
 if (pRec && (pRec->type()==95) && (p95 = (Record_95 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_95,InstallDate)+sizeof(p95->InstallDate))))
   return cfrDateYYYYMMDD(p95->InstallDate,date);
 return false;
}

Boolean CFR95::setInstallDate(CFRecV * pRec,IDate & date)
{
 if (pRec && (pRec->type()==95) && (p95 = (Record_95 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_95,InstallDate)+sizeof(p95->InstallDate))))
   return CFR54::setCFRValiDateYYYYMMDD(p95->InstallDate,date);
 return false;
}

Boolean CFR95::installDate(const Record_95 *pR95,IDate & date)
{ return pR95? cfrDateYYYYMMDD(pR95->InstallDate,date) : false; }

Boolean CFR95::cradDate(const Record_95 *pR95,IDate & date)
{ return pR95? cfrDateYYYYMMDD(pR95->CradDate,date) : false; }

unsigned CFR95::referenceNotesNumber(CFRecV * pRec)
{
 if (pRec && (pRec->type()==95) && (p95 = (Record_95 *)(pRec->record())) &&
     (strlen((char *)p95) >= (offsetof(Record_95,RefNotesNmbr)+sizeof(p95->RefNotesNmbr))))
// return myantou(p95->RefNotesNmbr,sizeof(p95->RefNotesNmbr));
   return cfrUnsigned(p95->RefNotesNmbr,sizeof(p95->RefNotesNmbr));
 return 0;
}

unsigned CFR95::referenceNotesNumber()
{
 return referenceNotesNumber((CFRecV *)this);
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
   }

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif

}

#endif


#if 0
#endif
