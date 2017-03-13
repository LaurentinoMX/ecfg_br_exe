 /*--------------------------------------------------------------------------*\
 |    Filename - CFR96.CPP                                                    |
 |      contains source for the following RM/2 class implementations:         |
 |                                                                            |
 |       CFR96  - To access the CFReport's Record Type 96                     |
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

#include "cfr96.hpp"                             // this file's include

Record_96 * p96;

        /*--------------------------------------------------------------*\
        |                             Ctors                              |
        \*--------------------------------------------------------------*/
CFR96::CFR96()
      : CFRecV(sizeof(Record_96))
{
 setType(96);
}

CFR96::CFR96(Record_54 * p54)
      : CFRecV(sizeof(Record_96))
{
 setType(96);
 if (p54) {
   setFeature((const char *)IString::stripBlanks(IString(p54->Feature,sizeof(p54->Feature),0)));
                                                 // only 54's version 30
   int len54 = strlen((char *)p54);              // contain following fields
   if (len54 > offsetof(Record_54,QualText))
     setDescription(p54->QualText);
   p96 = (Record_96 *)record();
   if (len54 > offsetof(Record_54,vrmtext))
     memcpy(p96->VRM_Text,p54->vrmtext,ALMIN(sizeof(p54->vrmtext),
                                                 // lengths differ
                                             sizeof(p96->VRM_Text)));
   if (len54 > offsetof(Record_54,release))
     memcpy(p96->Release,p54->release,sizeof(p96->Release));
   if (len54 > offsetof(Record_54,modlevel))
     memcpy(p96->Mod,p54->modlevel,sizeof(p96->Mod));
   }
}

CFR96::CFR96(const char * Feature)
      : CFRecV(sizeof(Record_96))
{
 setType(96);
 setFeature(Feature);
}

#if 0
CFR96::CFR96(Record_54 * p54)
{
 if (p54) {
   int len54 = strlen((char *)p54);
   CFRecV tmp(sizeof(Record_96));
   tmp.setType(96);
   if (len54 > offsetof(Record_54,Feature))
     setFeature(&tmp,p54->Feature);
   if (len54 > offsetof(Record_54,QualText))
     setDescription(&tmp,p54->QualText);
   p96 = (Record_96 *)tmp.record();
   if (len54 > offsetof(Record_54,vrmtext))
     memcpy(p96->VRM_Text,p54->vrmtext,ALMIN(sizeof(p54->vrmtext),
                                                 // lengths differ
                                             sizeof(p96->VRM_Text)));
   if (len54 > offsetof(Record_54,release))
     memcpy(p96->Release,p54->release,sizeof(p96->Release));
   if (len54 > offsetof(Record_54,modlevel))
     memcpy(p96->Mod,p54->modlevel,sizeof(p96->Mod));
   *(CFRecV *)this = tmp;
   }
}
#endif

Boolean  CFR96::setFeature(Record_96 * p96, const char *szFeature)
{
 if (p96 && szFeature)
   CFRCopySZPadd(p96->Feat_RPQ_PartNmbr,szFeature,sizeof(p96->Feat_RPQ_PartNmbr));
 return true;
}

    /*-----------------------------------------------------------------------*\
    |  Boolean  setFeature(CFRecV * pRec, const char *szFeature)              |
    |       set the Feature in the record                                     |
    |       if record's long enough                                           |
    \*-----------------------------------------------------------------------*/
Boolean  CFR96::setFeature(CFRecV * pRec, const char *szFeature)
{
 if (pRec && szFeature && (pRec->type()==96) && (p96 = (Record_96 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_96,Feat_RPQ_PartNmbr)+sizeof(p96->Feat_RPQ_PartNmbr))))
   return setFeature(p96,szFeature);
 return false;
}

Boolean  CFR96::setFeature(const char *szFeature)
{
 return setFeature((CFRecV *)this,szFeature);
}

    /*-----------------------------------------------------------------------*\
    |  Boolean  setDescription(CFRecV * pRec, const char *Desc)               |
    |       set the  description (stripped) in the record                     |
    |       if record's long enough                                           |
    \*-----------------------------------------------------------------------*/
Boolean  CFR96::setDescription(CFRecV * pRec, const char *Desc)
{
 if (pRec && Desc && (pRec->type()==96) && (p96 = (Record_96 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_96,Description)+sizeof(p96->Description)))) {
   mystrstrip(mystrncpy(p96->Description,Desc,sizeof(p96->Description)-1));
   return true;
   }
 return false;
}

Boolean  CFR96::setDescription(const char *szDesc)
{
 return setDescription((CFRecV *)this,szDesc);
}

    /*-----------------------------------------------------------------------*\
    |  const char * description(CFRecV * pRec)                                |
    |       return the pointer to the record's description string             |
    \*-----------------------------------------------------------------------*/
const char * CFR96::description(CFRecV * pRec)
{
 if (pRec && (pRec->type()==96) && (p96 = (Record_96 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_96,Description)+1)))
   return p96->Description;
 return "";
}

IString CFR96::strFeature(const Record_96 *p96)
{
 return p96? IString::stripBlanks(IString(p96->Feat_RPQ_PartNmbr,sizeof(p96->Feat_RPQ_PartNmbr),0)) : IString();
}

IString CFR96::descriptionString()
{
 return IString(description((CFRecV *)this));
}

Boolean CFR96::description(CFRecV * pRec,IString & str)
{
 str = IString::stripBlanks(IString(description(pRec)));
 return (str.length() != 0);
}

unsigned CFR96::referenceNotesNumber(CFRecV * pRec)
{
 if (pRec && (pRec->type()==96) && (p96 = (Record_96 *)(pRec->record())) &&
     (strlen((char *)p96) >= (offsetof(Record_96,RefNotesNmbr)+sizeof(p96->RefNotesNmbr))))
   return cfrUnsigned(p96->RefNotesNmbr,sizeof(p96->RefNotesNmbr));
 return 0;
}

unsigned CFR96::referenceNotesNumber()
{
 return referenceNotesNumber((CFRecV *)this);
}

Boolean  CFR96::setNonSoftwareRecordLength(CFRecV * pRec)
{
 if (pRec && (pRec->type()==96) && (p96 = (Record_96 *)(pRec->record())) &&
     (pRec->length() >= (offsetof(Record_96,Description)+45))) {
   p96->Description[45] = '\0';
   return true;
   }
 return false;
}

int CFR96::fWriteCFR(FILE * f,CFRecV * pRec,Boolean fSoftware)
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
