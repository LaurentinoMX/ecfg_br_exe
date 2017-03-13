 /*--------------------------------------------------------------------------*\
 |    Filename - CFR96.HPP                                                    |
 |      declarations for the following RM/2 class implementations:            |
 |                                                                            |
 |       CFR96  - To access the CFReport's Record Type 96                     |
 |                as defined in 'euopdef.h'                                   |
 \*--------------------------------------------------------------------------*/
#ifndef _AB_CFR96_HPP_
#define _AB_CFR96_HPP_

#include <fstream.h>
#include "euodef.h"
#include "euoutil.hpp"
#include "cfrv.hpp"

#pragma pack(4)
class CFR96 : public CFRecV
{
 public:
   CFR96();
   CFR96(Record_54 *);
   CFR96(const char * feature);
   static Boolean  setFeature(CFRecV *, const char *szFeature);
   static Boolean  setDescription(CFRecV *, const char *Desc);
   static const char * description(CFRecV *);
          Boolean  setFeature(const char *szFeature);
          Boolean  setDescription(const char *Desc);
   static IString  descriptionString(CFRecV *);
   static Boolean  description(CFRecV * pRec,IString & str);
          IString  descriptionString();
   static unsigned referenceNotesNumber(CFRecV *);
          unsigned referenceNotesNumber();
   static IString  strFeature(const Record_96 *p96);
   static Boolean  setNonSoftwareRecordLength(CFRecV *);
   static int      fWriteCFR(FILE * ,CFRecV *,Boolean fSoftware = false);

 protected:
   static Boolean  setFeature(Record_96 * p96, const char *szFeature);
};

#pragma pack()
#endif


