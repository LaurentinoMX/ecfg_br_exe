 /*--------------------------------------------------------------------------*\
 |    Filename - CFR54.HPP                                                    |
 |      declarations for the following RM/2 class implementations:            |
 |                                                                            |
 |       CFR54  - To access the CFReport's Record Type 54                     |
 |                as defined in 'euopdef.h'                                   |
 \*--------------------------------------------------------------------------*/
#ifndef _AB_CFR54_HPP_
#define _AB_CFR54_HPP_

#include <fstream.h>
#include "euodef.h"
#include "euoutil.hpp"
#include "cfrv.hpp"

#pragma pack(4)
class CFR54 : public CFRecV
{
 public:
   enum SourceType
     {
       Inventory
      ,BackLog
      ,MRDB
      ,Unknown
     };
   CFR54(Record_96 *,const char * szMachineType, const char * szMachineModel,
         const char * szSerialNo = 0);
   CFR54(Record_95 *);
   static Boolean cfrValiDateMMDDYYFields( const char * date, unsigned *pM
                                          ,unsigned *pD ,unsigned *pY);
   static Boolean cfrValiDateYYMMDDFields( const char * date, unsigned *pM
                                          ,unsigned *pD ,unsigned *pY);
   static Boolean setCFRValiDate( char * Target, unsigned M
                                 ,unsigned D ,unsigned Y);
   static Boolean setCFRValiDate( char * Target, long lYMD);
   static Boolean setCFRValiDate( char *Target,const IDate & date);
   static Boolean setCFRValiDateYYYYMMDD( char * Target, unsigned M
                                         ,unsigned D ,unsigned Y);
   static Boolean setCFRValiDateYYYYMMDD( char * Target, long lYMD);
   static Boolean setCFRValiDateYYYYMMDD( char *Target,const IDate & date);
   static long    cfrDateYMD(const char * CFRDate);
   static long    cfrDateYYYYMMDD(const char *);
   static Boolean cfrDate(const char *,IDate &);
   static Boolean cfrDateYYMMDD(const char *,IDate &);

   static Boolean enforceVersion20Format(CFRecV *);

   static Boolean  setMachType(CFRecV *, const char *szType);
   static Boolean  setMachModel(CFRecV *, const char *szModel);
   static Boolean  setMachModel(Record_54 *, const char *szModel);
   static Boolean  setSerialNo(CFRecV *, const char *szNo);
   static CFRecV * createANew( const char *szType
                              ,const char *szModel
                              ,const char *szSerialNo
                              ,const char *szFeature
                              ,char instType = 'I');

   static Boolean  setFeature(CFRecV *, const char *szFeature);
   static Boolean  setFeature(Record_54 * p, const char *szFeature);
   static Boolean  setDescription(CFRecV *, const char *Desc);
   static Boolean  setDescription(char * Buf, const char *Desc);
   static Boolean  isGeneric(CFRecV *);
   static Boolean  setInstallDate(CFRecV *,long lYMD);
   static Boolean  setInstallDate(CFRecV *,const IDate &);
   static Boolean  setCRADDate(CFRecV *,long lYMD);
   static Boolean  setCRADDate(CFRecV *,const IDate &);
   static Boolean  setCSDDate(CFRecV *,long lYMD);
   static Boolean  setCSDDate(CFRecV *,const IDate &);
   static Boolean  setInstType(CFRecV *, char type);
   static char     instanceType(CFRecV *);
   static long     installDateYMD(CFRecV *);
   static long     cradDateYMD(CFRecV *);
   static long     csdDateYMD(CFRecV *);
   static Boolean  installDate(CFRecV *,IDate &);
   static Boolean  installDate(char * Buffer,IDate &);
   static Boolean  cradDate(CFRecV *,IDate &);
   static Boolean  csdDate(CFRecV *,IDate &);
   static CFR54::SourceType source(CFRecV *);
   static Boolean  isInventory(CFRecV *);
   static Boolean  isBackLog(CFRecV *);
   static Boolean  isOrderable(CFRecV *);
   static const char * description(CFRecV *);
   static Boolean  description(CFRecV *, IString & str);
//   static const char * instId(CFRecV *);
   static Boolean  instId(CFRecV *,IString &);
   static Boolean  setInstId(CFRecV *,const char *id);
   static           blankFeature(const char * Buffer);
   static const char * serialNo(const char * Buffer);
   static const char * feature(const char * Buffer);
   static const char * feature(CFRecV *);
   static IString  strFeature(const char * Buffer);
   static unsigned fWriteCFR(FILE *,CFRecVL &, unsigned version);
   static void     initSerial_OrderNo(Record_54 *);

   //* D83402 Begin new logic: Added new function to return the additional value, customer number of the
   //* product the RPO belongs to, to create the associated RT 95 of the RT 19

   static Boolean      customerNumber(CFRecV * pRec,IString & str);
   static const char * customerNumber(CFRecV * pRec);

   //* D83402 End new logic:  HECC - 07/28/2003

};

#pragma pack()
#endif


