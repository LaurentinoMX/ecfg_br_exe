 /*--------------------------------------------------------------------------*\
 |    Filename - CFR54.HPP                                                    |
 |      declarations for the following RM/2 class implementations:            |
 |                                                                            |
 |       CFR54  - To access the CFReport's Record Type 54                     |
 |                as defined in 'euopdef.h'                                   |
 \*--------------------------------------------------------------------------*/
#ifndef _AB_CFR95_HPP_
#define _AB_CFR95_HPP_

#include <fstream.h>
#include "euodef.h"
#include "euoutil.hpp"
#include "cfrv.hpp"

class CFR54;

#pragma pack(4)
class CFR95 : public CFRecV
{
 public:
   enum SourceType
     {
       Inventory
      ,BackLog
      ,MRDB
      ,Unknown
     };
   CFR95(Record_54 *);
   CFR95();
   static Boolean      setProductType(CFRecV * pRec, const char *szType);
   CFR95 &             setProductType(const char *szType);
   static Boolean      setProductModel(CFRecV * pRec, const char *szModel);
   CFR95 &             setProductModel(const char *szModel);
   static Boolean      setSerialNo(CFRecV * pRec, const char *szNo);
   CFR95 &             setSerialNo(const char *szNo);
   static Boolean      setDescription(CFRecV * pRec, const char *Desc);
   CFR95 &             setDescription(const char *Desc);
   static const char * description(CFRecV * pRec);
   static const char * serialNo(const char * Buffer);
   IString             descriptionString();
   static Boolean      description(CFRecV * pRec,IString & str);
   static Boolean      installDate(CFRecV *pRec,IDate & date);
   static Boolean      setInstallDate(CFRecV * pRec,IDate & date);
   static Boolean      installDate(const Record_95 *,IDate &);
   static Boolean      cradDate(const Record_95 *,IDate &);
   static CFRecV *     createANew( const char *szType
                                  ,const char *szModel
                                  ,const char *szSerialNo);
   static Boolean      setNonSoftwareRecordLength(CFRecV *);
   static int          fWriteCFR(FILE *,CFRecV *,Boolean fSoftware = false);
   static unsigned     referenceNotesNumber(CFRecV *);
          unsigned     referenceNotesNumber();

   //* D83402 Begin new logic: Added four new functions to return four additional values of the
   //* product the RPO belongs to, to create the associated RT 95 of the RT 19, which are:  Customer number
   //* System number flag, System type number and System number.

   static Boolean      customerNumber(CFRecV * pRec,IString & str);
   static const char * customerNumber(CFRecV * pRec);

   static Boolean      systNumberFlag(CFRecV * pRec,IString & str);
   static const char * systNumberFlag(CFRecV * pRec);

   static Boolean      systemType(CFRecV * pRec,IString & str);
   static const char * systemType(CFRecV * pRec);

   static Boolean      systemNum(CFRecV * pRec,IString & str);
   static const char * systemNum(CFRecV * pRec);

   //* D83402 End new logic:  HECC - 07/28/2003

};

#pragma pack()
#endif


