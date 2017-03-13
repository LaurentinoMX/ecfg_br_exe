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

#include "cfr54.hpp"                             // this file's include
#include "cfr95.hpp"

Record_54 * p54;

        /*---------------------------------------------------*\
        |                        Ctors                        |
        \*---------------------------------------------------*/
CFR54::CFR54(Record_96 * p96, const char * szMachineType,
             const char * szMachineModel, const char * szSerialNo)
{
 if (p96) {
   CFRecV tmp(sizeof(Record_54));
   tmp.setType(54);
   setInstType(&tmp,'I');
   setFeature(&tmp,(const char *)IString::stripBlanks(IString(p96->Feat_RPQ_PartNmbr,sizeof(p96->Feat_RPQ_PartNmbr),0)));
   setMachType(&tmp,szMachineType);
   setMachModel(&tmp,szMachineModel);
   setSerialNo(&tmp,szSerialNo);
   setDescription(&tmp,p96->Description);
   p54 = (Record_54 *)tmp.record();
   memcpy(p54->vrmtext,p96->Mod,ALMIN(sizeof(p54->vrmtext),
                                                 // lengths differ
                                      sizeof(p96->VRM_Text)));
   memcpy(p54->release,p96->Release,sizeof(p54->release));
   memcpy(p54->modlevel,p96->Mod,sizeof(p54->modlevel));
//   initSerial_OrderNo(p54);
   *(CFRecV *)this = tmp;
   }
}

CFR54::CFR54(Record_95 * p95)
{
 if (p95) {
   IDate idate;
   CFRecV tmp(sizeof(Record_54));
   tmp.setType(54);
   setInstType(&tmp,'I');
   setMachType(&tmp,p95->ProductType);
   setMachModel(&tmp,(char *)IString::stripBlanks(IString(p95->ProductModel,sizeof(p95->ProductModel),'\0')));
   setSerialNo(&tmp,p95->SerialNo);
   p54 = (Record_54 *)tmp.record();
   memcpy(p54->custnum  ,p95->CustomerNo           ,ALMIN(sizeof(p54->custnum  ),sizeof(p95->CustomerNo           )));
   memcpy(p54->refmach  ,p95->DesignatedCPUType    ,ALMIN(sizeof(p54->refmach  ),sizeof(p95->DesignatedCPUType    )));
   memcpy(p54->euoprso  ,p95->DesignatedCPUSer_Ord ,ALMIN(sizeof(p54->euoprso  ),sizeof(p95->DesignatedCPUSer_Ord )));
//   memcpy(p54->CradDate ,p95->CradDate             ,ALMIN(sizeof(p54->CradDate ),sizeof(p95->CradDate             )));
   if (CFR95::cradDate(p95,idate))
     setCRADDate(&tmp,idate);
   memcpy(p54->ordernum ,p95->OrderNo              ,ALMIN(sizeof(p54->ordernum ),sizeof(p95->OrderNo              )));
   memcpy(p54->Sequence ,p95->SequenceField        ,ALMIN(sizeof(p54->Sequence ),sizeof(p95->SequenceField        )));
   if (CFR95::installDate(p95,idate))
     setInstallDate(&tmp,idate);
//   memcpy(p54->InstDate ,p95->InstallDate          ,ALMIN(sizeof(p54->InstDate ),sizeof(p95->InstallDate          )));
   memcpy(p54->stsqflag ,p95->StepSeqFlagForSW     ,ALMIN(sizeof(p54->stsqflag ),sizeof(p95->StepSeqFlagForSW     )));
   p54->SorcFlag = *p95->SourceFlag;
   *(CFRecV *)this = tmp;
   }
}

void CFR54::initSerial_OrderNo(Record_54 * p54)
{
 if (p54)
   memcpy(p54->euoprso,(char *)IString(0,sizeof(p54->euoprso),'0'),
          sizeof(p54->euoprso));
}

    /*-----------------------------------------------------------------------*\
    |   Boolean cfrValiDateMMDDYYFields( const char * p, unsigned *pM         |
    |                                   ,unsigned *pD ,unsigned *pY)          |
    |       validate the given MMDDYY date string                             |
    |       and set its components with centuries added,                      |
    |       if valid.                                                         |
    \*-----------------------------------------------------------------------*/
Boolean CFR54::cfrValiDateMMDDYYFields( const char * p, unsigned *pM
                                       ,unsigned *pD ,unsigned *pY)
{
 if (p                  &&
     ISDIGIT(*p)        &&
     ISDIGIT(*(p+1))    &&
     ISDIGIT(*(p+2))    &&
     ISDIGIT(*(p+3))    &&
     ISDIGIT(*(p+4))    &&
     ISDIGIT(*(p+5))) {
   *pM = (DIGIT(*p) * 10) + DIGIT(*(p+1));
   *pY = ((*(p+4) > '4')? 1900 : 2000) + (DIGIT(*(p+4)) * 10) + DIGIT(*(p+5));
   *pD = (DIGIT(*(p+2)) * 10) + DIGIT(*(p+3));
   return IDate::isValid(IDate::Month(*pM),*pD,*pY);
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean cfrValiDateYYMMDDFields( const char * p, unsigned *pM         |
    |                                   ,unsigned *pD ,unsigned *pY)          |
    |       validate the given YYMMDD date string                             |
    |       and set its components with centuries added,                      |
    |       if valid.                                                         |
    \*-----------------------------------------------------------------------*/
Boolean CFR54::cfrValiDateYYMMDDFields( const char * p, unsigned *pM
                                       ,unsigned *pD ,unsigned *pY)
{
 if (p                  &&
     ISDIGIT(*p)        &&
     ISDIGIT(*(p+1))    &&
     ISDIGIT(*(p+2))    &&
     ISDIGIT(*(p+3))    &&
     ISDIGIT(*(p+4))    &&
     ISDIGIT(*(p+5))) {
   *pD = (DIGIT(p[4]) * 10) + DIGIT(p[5]);
   *pM = (DIGIT(p[2]) * 10) + DIGIT(p[3]);
   *pY = ((p[0] > '4')? 1900 : 2000) + (DIGIT(p[0]) * 10) + DIGIT(p[1]);
   return IDate::isValid(IDate::Month(*pM),*pD,*pY);
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean CFR54::setCFRValiDate( char * p, unsigned M                   |
    |                                 ,unsigned D ,unsigned Y)                |
    |       write a n MMDDYY date string at the given                         |
    |       address from the given components when valid.                     |
    \*-----------------------------------------------------------------------*/
Boolean CFR54::setCFRValiDate( char * p, unsigned M
                              ,unsigned D ,unsigned Y)
{
 if (p && IDate::isValid(IDate::Month(M),D,Y)) {
   if (Y > 99)
     Y %= 100;
   *p++ = ((char )(M/10) + '0');
   *p++ = ((char )(M%10) + '0');
   *p++ = ((char )(D/10) + '0');
   *p++ = ((char )(D%10) + '0');
   *p++ = ((char )(Y/10) + '0');
   *p   = ((char )(Y%10) + '0');
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean CFR54::setCFRValiDateYYYYMMDD( char * p, unsigned M           |
    |                                         ,unsigned D ,unsigned Y)        |
    |       write a n YYYYMMDD date string at the given                       |
    |       address from the given components when valid.                     |
    \*-----------------------------------------------------------------------*/
Boolean CFR54::setCFRValiDateYYYYMMDD( char * p, unsigned M
                                      ,unsigned D ,unsigned Y)
{
 if (p && IDate::isValid(IDate::Month(M),D,Y)) {
   Y += (Y < 50? 2000 : ( Y < 99? 1900 : 0));
   CFRPaddCopySZ(p,(char *)IString((Y*10000)+(M*100)+ D),8,'0');
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean setCFRValiDateYYYYMMDD( char * p, long l)                     |
    |       write a n YYYYMMDD date string at the given                       |
    |       address from the given long when valid.                           |
    \*-----------------------------------------------------------------------*/
Boolean CFR54::setCFRValiDateYYYYMMDD( char * p, long l)
{
 unsigned  Y = l/10000;
 unsigned  M = (l-(Y*10000))/100;
 unsigned  D = (l-(Y*10000))%100;
 return setCFRValiDateYYYYMMDD(p,M,D,Y);
}

    /*-----------------------------------------------------------------------*\
    |   Boolean setCFRValiDate( char * p, long l)                             |
    |       write a n MMDDYY date string at the given                         |
    |       address from the given long when valid.                           |
    \*-----------------------------------------------------------------------*/
Boolean CFR54::setCFRValiDate( char * p, long l)
{
 unsigned  Y = l/10000;
 unsigned  M = (l-(Y*10000))/100;
 unsigned  D = (l-(Y*10000))%100;
 return setCFRValiDate(p,M,D,Y);
}

    /*-----------------------------------------------------------------------*\
    |   Boolean setCFRValiDateYYYYMMDD( char * p,const IDate & date)          |
    |       write a n YYYYMMDD date string at the given                       |
    |       address from the given date object.                               |
    \*-----------------------------------------------------------------------*/
Boolean CFR54::setCFRValiDateYYYYMMDD( char * p,const IDate & date)
{
 return setCFRValiDateYYYYMMDD(p,date.monthOfYear(),date.dayOfMonth(),date.year());
}

    /*-----------------------------------------------------------------------*\
    |   Boolean setCFRValiDate( char * p,const IDate & date)                  |
    |       write a n MMDDYY date string at the given                         |
    |       address from the given date object.                               |
    \*-----------------------------------------------------------------------*/
Boolean CFR54::setCFRValiDate( char * p,const IDate & date)
{
 return setCFRValiDate(p,date.monthOfYear(),date.dayOfMonth(),date.year());
}

    /*-----------------------------------------------------------------------*\
    |   long CFR54::cfrDateYMD(const char * p)                                |
    |       return a numerical YYYYMMDD date from a                           |
    |       valid given MMDDYY string.                                        |
    \*-----------------------------------------------------------------------*/
long CFR54::cfrDateYMD(const char * p)
{
 unsigned m,d,y;
 return (cfrValiDateMMDDYYFields(p,&m,&d,&y)? ((y*10000)+(m*100)+ d): 0);
}

    /*-----------------------------------------------------------------------*\
    |   long CFR54::cfrDateYYYYMMDD(const char * p)                           |
    |       return a numerical YYYYMMDD date from a                           |
    |       valid given YYYYMMDD string.                                      |
    \*-----------------------------------------------------------------------*/
long CFR54::cfrDateYYYYMMDD(const char * p)
{
 unsigned m,d,y;
 return (cfrValiDateYYYYMMDDFields(p,&m,&d,&y)? ((y*10000)+(m*100)+ d): 0);
}

    /*-----------------------------------------------------------------------*\
    |   Boolean cfrDate(const char * p,IDate & date)                          |
    |       set the given date object from a                                  |
    |       valid given MMDDYY string.                                        |
    \*-----------------------------------------------------------------------*/
Boolean CFR54::cfrDate(const char * p,IDate & date)
{
 unsigned m,d,y;
 if (cfrValiDateMMDDYYFields(p,&m,&d,&y)) {
   date = IDate(IDate::Month(m),d,y);
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean cfrDateYYMMDD(const char * p,IDate & date)                    |
    |       set the given date object from a                                  |
    |       valid given YYMMDD string.                                        |
    \*-----------------------------------------------------------------------*/
Boolean CFR54::cfrDateYYMMDD(const char * p,IDate & date)
{
 unsigned m,d,y;
 if (cfrValiDateYYMMDDFields(p,&m,&d,&y)) {
   date = IDate(IDate::Month(m),d,y);
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |  CFRecV * createANew()                                                  |
    |       'New' a record from the given components                          |
    \*-----------------------------------------------------------------------*/
CFRecV * CFR54::createANew( const char *szType
                           ,const char *szModel
                           ,const char *szSerialNo
                           ,const char *szFeature
                           ,char instType)
{
 CFRecV * p = new CFRecV(sizeof(Record_54));
 p->setType(54);
 setMachType(p,szType);
 setMachModel(p,szModel);
 setSerialNo(p,szSerialNo);
 setFeature(p,szFeature);
 setInstType(p,instType);
// initSerial_OrderNo((Record_54 *)(p->record()));
 return p;
}

    /*-----------------------------------------------------------------------*\
    |  Boolean  setMachType(CFRecV * pRec, const char *szType)                |
    |       set the Machine's Type in the record                              |
    |       if record's long enough                                           |
    \*-----------------------------------------------------------------------*/
Boolean  CFR54::setMachType(CFRecV * pRec, const char *szType)
{
 if (pRec && szType && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,MachType)+sizeof(p54->MachType)))) {
   CFRPaddCopySZ(p54->MachType,szType,sizeof(p54->MachType));
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |  Boolean  enforceVersion20Format(CFRecV * pRec)                         |
    |       Indicates whether the record was altered.                         |
    \*-----------------------------------------------------------------------*/
Boolean  CFR54::enforceVersion20Format(CFRecV * pRec)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() > (offsetof(Record_54,CsdDate)+sizeof(p54->CsdDate)))) {
   p54->Sequence[0] = '\0';
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    unsigned CFR54::fWriteCFR(FILE * f,CFRecVL & L54, unsigned version)  |
    |     Write the list as per version                                       |
    |       for 31:                                                           |
    |         Chop a copy of the record at its V31 length before writing      |
    |           it to the file.                                               |
    \*-----------------------------------------------------------------------*/
unsigned CFR54::fWriteCFR(FILE * f,CFRecVL & L54, unsigned version)
{
 if (version == 31) {
   CFRecV  rec, * pRec;
   for (int i=0,I=L54.NumberElements(); i < I ; i++)
     if (pRec = L54.element(i)) {
     rec = *pRec;
     enforceVersion20Format(&rec);
     rec.fWriteCFR(f);
     }
   return i;
   }
 else return L54.fWriteCFR(f);
}

    /*-----------------------------------------------------------------------*\
    |  Boolean  setMachModel(CFRecV * pRec, const char *szModel)              |
    |       set the Machine's Model in the record                             |
    |       if record's long enough                                           |
    \*-----------------------------------------------------------------------*/
Boolean  CFR54::setMachModel(CFRecV * pRec, const char *szModel)
{
 if (pRec && szModel && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,MachMod)+sizeof(p54->MachMod)))) {
   CFRPaddCopySZ(p54->MachMod,szModel,sizeof(p54->MachMod));
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |  Boolean  setMachModel(Record_54 * p54, const char *szModel)            |
    |       set the Machine's Model in the given buffer                       |
    \*-----------------------------------------------------------------------*/
Boolean  CFR54::setMachModel(Record_54 * p54, const char *szModel)
{
 if (p54 && szModel) {
   CFRPaddCopySZ(p54->MachMod,szModel,sizeof(p54->MachMod));
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |  Boolean  setSerialNo(CFRecV * pRec, const char *szNo)                  |
    |       set the Serial # in the record                                    |
    |       if record's long enough                                           |
    \*-----------------------------------------------------------------------*/
Boolean  CFR54::setSerialNo(CFRecV * pRec, const char *szNo)
{
 if (pRec && szNo && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,MachMod)+sizeof(p54->SerialNo)))) {
   CFRPaddCopySZ(p54->SerialNo,szNo,sizeof(p54->SerialNo));
   return true;
   }
 return false;
}

Boolean  CFR54::setFeature(Record_54 * p, const char *szFeature)
{
 if (p)
   CFRPaddCopySZ(p->Feature,szFeature,sizeof(p->Feature));
 return true;
}

    /*-----------------------------------------------------------------------*\
    |  Boolean  setFeature(CFRecV * pRec, const char *szFeature)              |
    |       set the Feature in the record                                     |
    |       if record's long enough                                           |
    \*-----------------------------------------------------------------------*/
Boolean  CFR54::setFeature(CFRecV * pRec, const char *szFeature)
{
 if (pRec && szFeature && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,Feature)+sizeof(p54->Feature))))
   return setFeature(p54,szFeature);
 return false;
}

    /*-----------------------------------------------------------------------*\
    |  Boolean  setDescription(CFRecV * pRec, const char *Desc)               |
    |       set the  description (stripped) in the record                     |
    |       if record's long enough                                           |
    \*-----------------------------------------------------------------------*/
Boolean  CFR54::setDescription(CFRecV * pRec, const char *Desc)
{
 if (pRec && Desc && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,QualText)+sizeof(p54->QualText)))) {
//   mystrncpy(p54->QualText,Desc,sizeof(p54->QualText)-1);
   mystrstrip(mystrncpy(p54->QualText,Desc,sizeof(p54->QualText)-1));
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |  Boolean CFR54::setDescription(char * Buf, const char *Desc)            |
    |       set the  description (stripped) in the given buffer               |
    \*-----------------------------------------------------------------------*/
Boolean CFR54::setDescription(char * Buf, const char *Desc)
{
 if (p54 = (Record_54 *)Buf)
    mystrstrip(mystrncpy(p54->QualText,Desc,sizeof(p54->QualText)-1));
 return true;
}

    /*-----------------------------------------------------------------------*\
    |  const char * description(CFRecV * pRec)                                |
    |       return the pointer to the record's description string             |
    \*-----------------------------------------------------------------------*/
const char * CFR54::description(CFRecV * pRec)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,QualText)+1)))
   return p54->QualText;
 return "";
}

    /*-----------------------------------------------------------------------*\
    |  const char * description(CFRecV * pRec)                                |
    |       set the given IString to the record's description                 |
    \*-----------------------------------------------------------------------*/
Boolean CFR54::description(CFRecV * pRec,IString & str)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (strlen((char *)p54) >= (offsetof(Record_54,QualText)+1))) {
   str = IString::stripBlanks(IString(p54->QualText,sizeof(p54->QualText),0));
   return true;
   }
 str = IString();
 return false;
}


    /*-----------------------------------------------------------------------*\
    |   const char * serialNo(const char * Buf)                               |
    |       return the pointer to the record's serialNo                       |
    |       which is NOT null-terminated                                      |
    \*-----------------------------------------------------------------------*/
const char * CFR54::serialNo(const char * Buf)
{
 if ((p54 = (Record_54 *)Buf) &&
     (strlen(Buf) >= (offsetof(Record_54,SerialNo)+sizeof(p54->SerialNo))))
   return p54->SerialNo;
 return "";
}

    /*-----------------------------------------------------------------------*\
    |   Boolean CFR54::customerNumber(CFRecV * pRec,IString & str)            |
    |       set the given IString to the record's customer number             |
    |  New function:  D83402    HECC - 07/28/2003                             |
    \*-----------------------------------------------------------------------*/
Boolean CFR54::customerNumber(CFRecV * pRec,IString & str)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (strlen((char *)p54) >= (offsetof(Record_54,custnum)+1))) {
   str = IString::stripBlanks(IString(p54->custnum,sizeof(p54->custnum),0));
   return true;
   }
 str = IString();
 str = str.subString(1,7);
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   const char * CFR54::customerNumber(CFRecV * pRec)                     |
    |       return the pointer to the record's customer number string         |
    |  New function:  D83402    HECC - 07/28/2003                             |
    \*-----------------------------------------------------------------------*/

const char * CFR54::customerNumber(CFRecV * pRec)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,custnum)+1)))
   return p54->custnum;
 return "";
}

    /*-----------------------------------------------------------------------*\
    |   const char * feature(CFRecV *pRec)                                    |
    |       return the pointer to the record's feature                        |
    |       which is NOT null-terminated                                      |
    \*-----------------------------------------------------------------------*/
const char * CFR54::feature(CFRecV *pRec)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,Feature)+sizeof(p54->Feature))))
   return p54->Feature;
 return "";
}
const char * CFR54::feature(const char * Buf)
{
 if ((p54 = (Record_54 *)Buf) &&
     (strlen(Buf) >= (offsetof(Record_54,Feature)+sizeof(p54->Feature))))
   return p54->Feature;
 return "";
}


IString CFR54::strFeature(const char * Buf)
{
 return IString::stripBlanks(IString(feature(Buf)));
}

    /*-----------------------------------------------------------------------*\
    |   Boolean instId(CFRecV * pRec,IString & str)                           |
    |       set the given IString to the record's Instance Id                 |
    \*-----------------------------------------------------------------------*/
Boolean CFR54::instId(CFRecV * pRec,IString & str)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,InstId)+sizeof(p54->InstId)))) {
   str = IString::stripBlanks(IString(p54->InstId,sizeof(p54->InstId),0));
   return true;
   }
 str = IString();
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean setInstId(CFRecV * pRec,const char * instid)                  |
    |       set the given Instance Id in the record                           |
    \*-----------------------------------------------------------------------*/
Boolean CFR54::setInstId(CFRecV * pRec,const char * instid)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,InstId)+sizeof(p54->InstId)))) {
   CFRPaddCopySZ(p54->InstId,instid,sizeof(p54->InstId));
   return true;
   }
 return false;
}


    /*-----------------------------------------------------------------------*\
    |   Boolean setInstType(CFRecV *pRec, char c)                             |
    |       set the given Instance Type in the record                         |
    \*-----------------------------------------------------------------------*/
Boolean CFR54::setInstType(CFRecV *pRec, char c)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,InstType)+1))) {
   p54->InstType = c;
   return true;
   }
 return false;
}

Boolean CFR54::isGeneric(CFRecV *pRec)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,InstType)+1)))
   return (p54->InstType == 'U');
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean setInstallDate(CFRecV *pRec,long lYMD)                        |
    |       Note: the long's assumed to be in format YYYYMMDD                 |
    \*-----------------------------------------------------------------------*/
Boolean CFR54::setInstallDate(CFRecV *pRec,long lYMD)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,InstDate)+sizeof(p54->InstDate))))
   return setCFRValiDate(p54->InstDate,lYMD);
 return false;
}

Boolean CFR54::setInstallDate(CFRecV *pRec,const IDate & date)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,InstDate)+sizeof(p54->InstDate))))
   return setCFRValiDate(p54->InstDate,date);
 return false;
}

//      Note: the long's assumed to be in format YYYYMMDD
Boolean CFR54::setCRADDate(CFRecV *pRec,long lYMD)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,CradDate)+sizeof(p54->CradDate))))
   return setCFRValiDate(p54->CradDate,lYMD);
 return false;
}

Boolean CFR54::setCRADDate(CFRecV *pRec,const IDate & date)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,CradDate)+sizeof(p54->CradDate))))
   return setCFRValiDate(p54->CradDate,date);
 return false;
}

//      Note: the long's assumed to be in format YYYYMMDD
Boolean CFR54::setCSDDate(CFRecV *pRec,long lYMD)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,CsdDate)+sizeof(p54->CsdDate))))
   return setCFRValiDateYYYYMMDD(p54->CsdDate,lYMD);
 return false;
}

Boolean CFR54::setCSDDate(CFRecV *pRec,const IDate & date)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,CsdDate)+sizeof(p54->CsdDate))))
   return setCFRValiDateYYYYMMDD(p54->CsdDate,date);
 return false;
}

//      Note: the returned numeric value is in format YYYYMMDD
long CFR54::installDateYMD(CFRecV *pRec)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,InstDate)+sizeof(p54->InstDate))))
   return cfrDateYMD(p54->InstDate);
 return 0;
}

//      Note: the returned numeric value is in format YYYYMMDD
long CFR54::cradDateYMD(CFRecV *pRec)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,CradDate)+sizeof(p54->CradDate))))
    return cfrDateYMD(p54->CradDate);
 return 0;
}

//      Note: the returned numeric value is in format YYYYMMDD
long CFR54::csdDateYMD(CFRecV *pRec)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,CsdDate)+sizeof(p54->CsdDate))))
//    return cfrDateYMD(p54->CsdDate);
   return myantou(p54->CsdDate,sizeof(p54->CsdDate));
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean installDate(char * Buff,IDate & date)                         |
    |       set the Install date from the given 54-Record buffer,             |
    |       returning false if not available                                  |
    \*-----------------------------------------------------------------------*/
Boolean CFR54::installDate(char * Buff,IDate & date)
{
 if ( (p54 = (Record_54 *)Buff) &&
     (strlen(Buff) >= (offsetof(Record_54,InstDate)+sizeof(p54->InstDate))))
   return cfrDate(p54->InstDate,date);
 return false;
}

Boolean CFR54::installDate(CFRecV *pRec,IDate & date)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,InstDate)+sizeof(p54->InstDate))))
   return cfrDate(p54->InstDate,date);
 return false;
}

Boolean CFR54::cradDate(CFRecV * pRec,IDate & date)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,CradDate)+sizeof(p54->CradDate))))
   return cfrDate(p54->CradDate,date);
 return false;
}

Boolean CFR54::csdDate(CFRecV * pRec,IDate & date)
{
 unsigned u = CFR54::csdDateYMD(pRec);
 unsigned  Y = u/10000;
 u -= Y*10000;
 unsigned  M = u/100;
 unsigned  D = u%100;
 if (IDate::isValid(IDate::Month(M),D,Y)) {
   date = IDate(IDate::Month(M),D,Y);
   return true;
   }
 return false;
}

CFR54::SourceType CFR54::source(CFRecV *pRec)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,SorcFlag)+1)))
    switch(p54->SorcFlag) {
      case 'I': return Inventory;
      case 'B': return BackLog;
      case 'M': return MRDB;
      }
 return Unknown;
}

Boolean CFR54::isInventory(CFRecV *pRec)
{
 return (source(pRec) == Inventory);
}

Boolean CFR54::isBackLog(CFRecV * pRec)
{
 return (source(pRec) == BackLog);
}

Boolean CFR54::isOrderable(CFRecV * pRec)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,InstType)+1)))
   return (p54->InstType == 'I');
 return false;
}

char CFR54::instanceType(CFRecV * pRec)
{
 if (pRec && (pRec->type()==54) && (p54 = (Record_54 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_54,InstType)+1)))
   return p54->InstType;
 return ' ';
}

    /*-----------------------------------------------------------------------*\
    |   Boolean CFR54::blankFeature(const char * Buffer)                      |
    |       Indicate whether the record's feature consists                    |
    |       solely of blanks                                                  |
    \*-----------------------------------------------------------------------*/
Boolean CFR54::blankFeature(const char * Buffer)
{
 if ((p54 = (Record_54 *)Buffer) &&
     (strlen(Buffer) >= (offsetof(Record_54,Feature)+sizeof(p54->Feature)))) {
   for (int i=sizeof(p54->Feature) - 1; i >= 0 && p54->Feature[i] == ' ';i--);
   return (i < 0);
   }
 return false;
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
