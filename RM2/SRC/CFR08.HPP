 /*--------------------------------------------------------------------------*\
 |    Filename - CFR08.HPP                                                    |
 |       declarations for the following RM/2 class implementations:           |
 |                                                                            |
 |       CFR08V31                                                             |
 |       CFR08  - To access the CFReport's Record Type 08                     |
 |                as defined in 'euopdef.h'                                   |
 \*--------------------------------------------------------------------------*/

#ifndef _AB_CFR08_HPP_
#define _AB_CFR08_HPP_

#include <fstream.h>

#include "euodef.h"
#include "euoutil.hpp"
#include "cfrv.hpp"

#pragma pack(4)
class CFR08 : public CFRecV
{
   static int         featIndex(CFRecV &,const char * szFeature);
   static Feat78 *    featPtr(CFRecV &,unsigned index);
   static Feat78 *    featPtr(CFRecV & rec,const char * szFeature);
 public:
  CFR08();
  CFR08(Record_08V31 *);
  static Boolean      edit(char * Buffer, int version, Tellem & tellem,
                           const char * FileName);
  static unsigned     numberFeat(CFRecV &);
  static unsigned     numberFeat(Record_08 *);
  static Boolean      containsFeat(CFRecV &,const char * szFeature);
  static Boolean      featQty(CFRecV &, const char * szFeature, unsigned * pu);
  static Boolean      featParts(CFRecV &,const char * szFeature,
                                char * Target, unsigned * pu );
  static Boolean      featParts(CFRecV &, unsigned Index, char * Target,
                                unsigned * pu );
  static Boolean      setFeatParts(CFRecV &, unsigned Index,unsigned qty,
                                   char *szFeature);
  static Boolean      setFeatQty(CFRecV &,const char * szFeature,unsigned qty );
  static Boolean      stripOffFeat(CFRecV &,unsigned index);
  static void         appendFeat(CFRecV &, const char *szFeature, unsigned qty);

  static const char * machineType(const char * Buffer);
  static const char * model(const char * Buffer);
  static char         prodInd(const char * Buffer)
                        {return ((Record_08 *)Buffer)->cIPProd;}
  static CFRecV *     createANew( const char *szMachine ,const char *szModel,
                                  char cProductIndicator = ' ');
  static unsigned     modelQty(const char * Buffer);
  static Boolean      setModelQty(CFRecV & rec,unsigned qty);
  static Boolean      setModel(CFRecV &, const char *szModel);
  CFR08 &             setModel(const char *szModel);
  CFR08 &             setMachineType(const char *szType);
  CFR08 &             setModelQty(unsigned qty);
};

class CFR08V31 : public CFRecV
{
   static int featIndex(CFRecV &,const char * szFeature);
   static Feat75 * featPtr(CFRecV &,unsigned index);
   static Feat75 * featPtr(CFRecV & rec,const char * szFeature);
 public:
  CFR08V31();
  CFR08V31(Record_08 *);

  static Boolean    edit(char * Buffer, int version, Tellem & tellem,
                         const char * FileName);
  static unsigned   numberFeat(CFRecV &);
  static unsigned   numberFeat(Record_08V31 *);
  static Boolean    containsFeat(CFRecV &,const char * szFeature);
  static Boolean    featQty(CFRecV &, const char * szFeature, unsigned * pu);
  static Boolean    featParts(CFRecV &,const char * szFeature,
                              char * Target, unsigned * pu );
  static Boolean    featParts(CFRecV &, unsigned Index, char * Target,
                              unsigned * pu );
  static Boolean    setFeatParts(CFRecV &, unsigned Index,unsigned qty,
                                 char *szFeature);
  static Boolean    setFeatQty(CFRecV &,const char * szFeature,unsigned qty );
  static Boolean    stripOffFeat(CFRecV &,unsigned index);
  static void       appendFeat(CFRecV &, const char *szFeature, unsigned qty);

  static const char * machineType(const char * Buffer);
  static const char * model(const char * Buffer);
  static CFRecV *     createANew( const char *szMachine ,const char *szModel );
  static unsigned     productQty(const char * Buffer);
  static Boolean      setProductQty(CFRecV & rec,unsigned qty);
  static Boolean      setModel(CFRecV &, const char *szModel);

  CFR08V31 &          setModel(const char *szModel);
  CFR08V31 &          setMachineType(const char *szType);
  CFR08V31 &          setProductQty(unsigned qty);
};
#pragma pack()
#endif

