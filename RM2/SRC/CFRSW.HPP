 /*--------------------------------------------------------------------------*\
 |    Filename - CFRSW.HPP    (CFReport Software objects)                     |
 |      declarations for the following RM/2 class implementations:            |
 |                                                                            |
 |       CFR47     - To access the CFReport's Record Type 47                  |
 |                   as defined in 'euopdef.h'                                |
 |       CFR49     - To access the CFReport's Record Type 49                  |
 |                   as defined in 'euopdef.h'                                |
 |       CFR35     - To convert CFReport's Record Types 47 and 49             |
 |                   to type 35 as defined in 'euopdef.h'                     |
 |       CFR17     - To convert CFReport's Record Types 47 and 49             |
 |                   to type 17 as defined in 'euopdef.h'                     |
 |       SoftMach  - RM2  Object representing                                 |
 |                   CFReport's SoftWare Machine (Record 08) entries          |
 |     SoftMachList- A collection (Sorted Set) of SoftMach's                  |
 \*--------------------------------------------------------------------------*/
// Modification Log:
// F73732   EDW   07/25/02 Completion of F72273 (SW reconciliation 
//                         support for iSeries)
//

#ifndef _AB_CFRSW_HPP_
#define _AB_CFRSW_HPP_

#include <fstream.h>
#include <iostream.h>

#define INCL_ISTRING
#define INCL_IDATE
#include "uicl.hpp"

#include <isrtset.h>

#include "euodef.h"
#include "euoutil.hpp"
#include "cfrv.hpp"

#pragma pack(4)
class CFR35 : public CFRecV
{
 public:
  CFR35(Record_49 *);
  CFR35(Record_47 *);
  CFR35(const CFR35& other) { *this = other;}
  CFR35 & operator=(CFR35 const& other);
};

class CFR17 : public CFRecV
{
    static int      featIndex(CFRecV &,const char * szFeature); /* F73732 EDW 07/25/02 */
    static Feat44 * featPtr(CFRecV &,const char * szFeature);   /* F73732 EDW 07/25/02 */
    static Feat44 * featPtr(CFRecV &,unsigned index);           /* F73732 EDW 07/25/02 */
 public:
  CFR17() : CFRecV(offsetof(Record_17,Feats)){}                 /* F73732 EDW 07/25/02 */
  CFR17(Record_49 *);
  CFR17(Record_47 *);
  CFR17(const CFR17& other) { *this = other;}
  CFR17 & operator=(CFR17 const& other);
  
  /* F73732 EDW 07/25/02 - start */
  static unsigned numberFeat(CFRecV &);
  static unsigned numberFeat(Record_17 *);
  static Boolean  containsFeat(CFRecV &, const char * szFeature);
  static Boolean  featQty(CFRecV &, const char * szFeature, unsigned * pu);
  static Boolean  featParts(CFRecV &,const char * szFeature,
                            char * Target, unsigned * pu );
  static Boolean  featParts(CFRecV &, unsigned Index, char * Target,
                            unsigned * pu );
  static Boolean  setFeatParts(CFRecV &, unsigned Index,unsigned qty,
                                char *szFeature);
  static Boolean  setFeatQty(CFRecV &,const char * szFeature,unsigned qty );
  static Boolean  stripOffFeat(CFRecV &,unsigned index);
  static void     appendFeat(CFRecV & rec, const char *szFeature, unsigned qty);
  static Boolean  setModel(CFRecV &,const char *szModel);
  /* F73732 EDW 07/25/02 - end */
};

class CFR47 : public CFRecV
{
    static int      featIndex(CFRecV &,const char * szFeature);
    static Feat74 * featPtr(CFRecV &,const char * szFeature);
    static Feat74 * featPtr(CFRecV &,unsigned index);
 public:
    CFR47() : CFRecV(offsetof(Record_47,Feats)){}
    CFR47(Record_SW_V31 *);
    CFR47(Record_17 *);
    CFR47(const CFR47 & other) { *this = other;}
    CFR47 & operator=(CFR47 const& other);

    static unsigned numberFeat(CFRecV &);
    static unsigned numberFeat(Record_47 *);
    static Boolean  containsFeat(CFRecV &, const char * szFeature);
    static Boolean  featQty(CFRecV &, const char * szFeature, unsigned * pu);
    static Boolean  featParts(CFRecV &,const char * szFeature,
                              char * Target, unsigned * pu );
    static Boolean  featParts(CFRecV &, unsigned Index, char * Target,
                              unsigned * pu );
    static Boolean  setFeatParts(CFRecV &, unsigned Index,unsigned qty,
                                  char *szFeature);
    static Boolean  setFeatQty(CFRecV &,const char * szFeature,unsigned qty );
    static Boolean  stripOffFeat(CFRecV &,unsigned index);
    static void     appendFeat(CFRecV & rec, const char *szFeature, unsigned qty);
    static Boolean  setModel(CFRecV &,const char *szModel);

};

class CFR49 : public CFRecV
{
    static int      featIndex(CFRecV &,const char * szFeature);
    static Feat74 * featPtr(CFRecV &,const char * szFeature);
    static Feat74 * featPtr(CFRecV &,unsigned index);
 public:
    CFR49() : CFRecV(offsetof(Record_49,Feats)){}
    CFR49(Record_SW_V31 *);
    CFR49(const CFR49 & other) { *this = other;}
    CFR49 & operator=(CFR49 const& other);

    static unsigned numberFeat(CFRecV &);
    static unsigned numberFeat(Record_49 *);
    static Boolean  containsFeat(CFRecV &, const char * szFeature);
    static Boolean  featQty(CFRecV &, const char * szFeature, unsigned * pu);
    static Boolean  featParts(CFRecV &,const char * szFeature,
                              char * Target, unsigned * pu );
    static Boolean  featParts(CFRecV &, unsigned Index, char * Target,
                              unsigned * pu );
    static Boolean  setFeatParts(CFRecV &, unsigned Index,unsigned qty,
                                  char *szFeature);
    static Boolean  setFeatQty(CFRecV &,const char * szFeature,unsigned qty );
    static Boolean  stripOffFeat(CFRecV &,unsigned index);
    static void     appendFeat(CFRecV & rec, const char *szFeature, unsigned qty);
    static int      compare(CFRecV &,const Record_17 *);
    static Boolean  setModel(CFRecV &,const char *szModel);
};


class SoftMach
{
  IString Type ;
  IString Model;
  IString Feature;
  IString Desc;
  unsigned long ulDate;

 public:

  static IDate  nonDate() { return IDate(1950,1);}

  SoftMach(const IString & type  = IString(),
           const IString & model = IString(),
           const IString & feat  = IString(),
           const IString & desc  = IString(),
//           const IDate   & date  = IDate() )
           const IDate   & date  = nonDate() )
          : Type(type)
           ,Model(model)
           ,Feature(feat)
           ,ulDate(date.julianDate())
           ,Desc(desc)
           {}
  SoftMach(const char * type  = 0,
           const char * model = 0,
           const char * feat  = 0,
           const char * szDesc = 0,
//           const IDate & date  = IDate() )
           const IDate   & date  = nonDate() )
          : Type(type,4,0)
           ,Model(model,3,0)
           ,Feature(feat,7,0)
           ,ulDate(date.julianDate())
           ,Desc(szDesc)
           {}
  SoftMach() {}

  IString const& strType()        const { return Type; }
  IString const& strModel()       const { return Model;}
  IString const& strFeature()     const { return Feature;}
  IString const& strDescription() const { return Desc;}
  IDate          date()           const { return IDate(ulDate);}
  unsigned long  dateJulian()     const { return ulDate;}

  const char *         type() const    { return (char *)Type;  }
  const char *         model() const   { return (char *)Model; }
  const char *         feature() const   { return (char *)Feature; }
  const char *         description() const { return (char *)Desc; }
  void                 setDescription(const char * desc)
                             { Desc = IString(desc);}

  SoftMach & operator=(SoftMach const& other);
                                                 // For any Set we need to
                                                 // provide element equality.
  Boolean operator==(SoftMach const& other)  const;

                                                 // For a Sorted Set we need to
                                                 // provide element
                                                 // comparision.
  Boolean operator< (SoftMach const& other) const;

  friend ostream& operator<< (ostream& os, SoftMach const& x)
                        {
                          return os << "Type : '"     << x.type()
                                    << "' Model : '"     << x.model()
                                    << "' Feature : '"     << x.feature()
                                    << "' Description : '"  << x.description()
                                    << "' Date : '"  << x.date().asString()
                                    << "'";
                        }
};


#define SOFTMACHL_ERR_NO_FILE              1
#define SOFTMACHL_ERR_NO_SOFTMACHL_FILE    2
#define SOFTMACHL_ERR_READ_FILE            3
class SoftMachList : public ISortedSet<SoftMach>
{
  public:
    unsigned   fWriteBLength();
    Boolean    fWriteB(char * szFilename, unsigned * pBytesWritten = 0);
    Boolean    fWriteB(FILE * f,unsigned * pBytesWritten = 0);
    unsigned   fReadB(char * szFilename);
    unsigned   fReadB(FILE *);

    ostream&   out(ostream&);

    friend ostream& operator<< (ostream& os, SoftMachList & L)
                 {
                  os   << "          ----> SoftWare List ( "
                       << L.numberOfElements()
                       << " elements) <---- " << endl;
                 Cursor crsr(L);
                   forCursor(crsr)
                     os << L.elementAt(crsr) << endl;
                  return os;
                 }

};

class CFR_SW_V31 : public CFRecV
{
    static int      featIndex(CFRecV &,const char * szFeature);
    static Feat75 * featPtr(CFRecV &,const char * szFeature);
    static Feat75 * featPtr(CFRecV &,unsigned index);
 public:
    CFR_SW_V31() : CFRecV(offsetof(Record_SW_V31,Feats)){}
    CFR_SW_V31(Record_47 *);
    CFR_SW_V31(Record_49 *);
    CFR_SW_V31(const CFR_SW_V31& other) { *this = other;}
    CFR_SW_V31 & operator=(CFR_SW_V31 const& other);

    static IString  strModel(const Record_SW_V31 *);
    static IString  strType(const Record_SW_V31  *);
    static unsigned numberFeat(CFRecV &);
    static unsigned numberFeat(Record_SW_V31 *);
    static Boolean  containsFeat(CFRecV &, const char * szFeature);
    static Boolean  featQty(CFRecV &, const char * szFeature, unsigned * pu);
    static Boolean  featParts(CFRecV &,const char * szFeature,
                              char * Target, unsigned * pu );
    static Boolean  featParts(CFRecV &, unsigned Index, char * Target,
                              unsigned * pu );
    static Boolean  setFeatParts(CFRecV &, unsigned Index,unsigned qty,
                                  char *szFeature);
    static Boolean  setFeatQty(CFRecV &,const char * szFeature,unsigned qty );
    static Boolean  stripOffFeat(CFRecV &,unsigned index);
    static void     appendFeat(CFRecV & rec, const char *szFeature, unsigned qty);

};


#pragma pack()
#endif

