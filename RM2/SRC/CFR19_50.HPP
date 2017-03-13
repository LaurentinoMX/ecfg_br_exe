 /*--------------------------------------------------------------------------*\
 |    Filename - CFR19_50.HPP                                                 |
 |      declarations for the following RM/2 class implementations:            |
 |                                                                            |
 |       CFR19                                                                |
 |       CFR19V31  - To access the CFReport's Record Type 19                  |
 |                   (Hardware Backlog) as defined in 'euopdef.h'             |
 |                                                                            |
 |       CFR50                                                                |
 |       CFR50V31  - To access the CFReport's Record Type 50                  |
 |                   (Software Backlog) as defined in 'euopdef.h'             |
 |                                                                            |
 \*--------------------------------------------------------------------------*/

#ifndef _AB_CFR19_50_HPP_
#define _AB_CFR19_50_HPP_

#include <fstream.h>

#include "euodef.h"
#include "euoutil.hpp"
#include "cfrv.hpp"

#pragma pack(4)

class CFR19
{
   static int featIndex(CFRecV &,const char * szFeature);
   static FeatR19 * featPtr(CFRecV &,unsigned index);
   static FeatR19 * featPtr(CFRecV & rec,const char * szFeature);

 public:
  static unsigned   numberFeat(CFRecV &);
  static Boolean    containsFeat(CFRecV &,const char * szFeature);
  static Boolean    featQty(CFRecV &, const char * szFeature, unsigned * pu);
  static Boolean    featParts(CFRecV &,const char * szFeature,
                              char * Target, unsigned * pu = 0,
                              char * pAction = 0 );
  static Boolean    featParts(CFRecV &, unsigned Index, char * Target,
                              unsigned * pu = 0, char * pAction = 0 );
  static Boolean    setFeatParts(CFRecV &, unsigned Index, unsigned * pQty,
                            char *szFeature = 0, char * pAction = 0);

  static Boolean    setFeatQty(CFRecV &,const char * szFeature,unsigned qty );
  static Boolean    stripOffFeat(CFRecV &,unsigned index);
  static void       appendFeat(CFRecV &, const char *szFeature, unsigned qty,
                               char Action);

  static const char * machineType(const char * Buffer);
  static const char * serialNo(const char *Buffer);
  static const char * toModel(const char *Buffer);
  static const char * fromModel(const char *Buffer);
  static void         model(const char * Buffer,IString & str);
  static char         action(const char * Buffer)
                        { return ((const Record_19 *)Buffer)->Action;}
  static IString      qty(const char * Buffer);
  static CFRecV *     createANew( const char *szMachine ,const char *szModel,
                                  char Action = '4');

};

class CFR19V31
{
   static int       featIndex(CFRecV & rec,const char * szFeature);
   static Feat751 *  featPtr(CFRecV & rec,unsigned index);
   static Feat751 *  featPtr(CFRecV & rec,const char * szFeature);

 public:
   static IString   type(const char *Buf);
   static IString   model(const char *Buf);
   static IString   toType(const char *Buf);
   static IString   toModel(const char *Buf);
   static IString   fromType(const char *Buf);
   static IString   fromModel(const char *Buf);
   static unsigned  action(const char *Buf);
   static unsigned  numberFeat(CFRecV & rec);
   static Boolean   containsFeat(CFRecV & rec,const char * szFeature);
   static Boolean   featQty(CFRecV & rec,const char * szFeature,unsigned *);
   static Boolean   featParts(CFRecV & rec,const char * szFeature,
                              char * Target, unsigned * pu , char * pAction);
   static Boolean   featParts(CFRecV &rec , unsigned Index, char * Target,
                              unsigned * pu , char * pAction);
   static Boolean   setFeatParts(CFRecV & rec, unsigned Index,unsigned * pQty,
                                 char *szFeature, char * pAction);
   static Boolean   setFeatQty(CFRecV & rec,const char * szFeature,
                               unsigned qty );
   static Boolean   stripOffFeat(CFRecV & rec,unsigned index);
   static void      appendFeat(CFRecV & rec, const char *szFeature,
                               unsigned qty, char Action);
   static CFRecV *  createANew( const char *szMachine ,const char *szModel ,
                                char Action);
};


class CFR50
{
   static int featIndex(CFRecV &,const char * szFeature);
   static FeatR50 * featPtr(CFRecV &,unsigned index);
   static FeatR50 * featPtr(CFRecV & rec,const char * szFeature);

 public:
  static unsigned   numberFeat(CFRecV &);
  static Boolean    containsFeat(CFRecV &,const char * szFeature);
  static Boolean    featQty(CFRecV &, const char * szFeature, unsigned * pu);
  static Boolean    featParts(CFRecV &,const char * szFeature,
                              char * Target, unsigned * pu = 0,
                              char * pAction = 0 );
  static Boolean    featParts(CFRecV &, unsigned Index, char * Target,
                              unsigned * pu = 0, char * pAction = 0 );
  static Boolean    setFeatParts(CFRecV &, unsigned Index, unsigned * pQty,
                            char *szFeature = 0, char * pAction = 0);

  static Boolean    setFeatQty(CFRecV &,const char * szFeature,unsigned qty );
  static Boolean    stripOffFeat(CFRecV &,unsigned index);
  static void       appendFeat(CFRecV &, const char *szFeature, unsigned qty,
                               char Action);

  static const char * machineType(const char * Buffer);
  static void         model(const char * Buffer,IString & str);
  static const char * model(const char *Buf);
  static char         action(const char * Buffer)
                        { return ((const Record_50 *)Buffer)->Action;}
  static IString      qty(const char * Buffer);

};

class CFR50V31
{
   static int       featIndex(CFRecV & rec,const char * szFeature);
   static Feat751 *  featPtr(CFRecV & rec,unsigned index);
   static Feat751 *  featPtr(CFRecV & rec,const char * szFeature);

 public:
   static IString   type(const char *Buf);
   static IString   model(const char *Buf);
   static unsigned  action(const char *Buf);
   static unsigned  numberFeat(CFRecV & rec);
   static Boolean   containsFeat(CFRecV & rec,const char * szFeature);
   static Boolean   featQty(CFRecV & rec,const char * szFeature,unsigned *);
   static Boolean   featParts(CFRecV & rec,const char * szFeature,
                              char * Target, unsigned * pu , char * pAction);
   static Boolean   featParts(CFRecV &rec , unsigned Index, char * Target,
                              unsigned * pu , char * pAction);
   static Boolean   setFeatParts(CFRecV & rec, unsigned Index,unsigned * pQty,
                                 char *szFeature, char * pAction);
   static Boolean   setFeatQty(CFRecV & rec,const char * szFeature,
                               unsigned qty );
   static Boolean   stripOffFeat(CFRecV & rec,unsigned index);
   static void      appendFeat(CFRecV & rec, const char *szFeature,
                               unsigned qty, char Action);
};

#pragma pack()
#endif

