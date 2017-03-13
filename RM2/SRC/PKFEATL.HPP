 /*--------------------------------------------------------------------------*\
 |    Filename - PKFEAT.HPP                                                   |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |     PKArr    - Product Knowledge Array.                                    |
 |                An array of pointers to fixed length records                |
 |                (PtrRecArr) with the added ability to have the              |
 |                array point to records in a contiguous block of             |
 |                storage. This ability is utilized when the                  |
 |                Product Knowledge is retrieved from its 'compiled'          |
 |                version file, to cut down on processing time.               |
 |                This object is the base of some of the following:           |
 |                                                                            |
 |   PKFeatList - An array of PKFeat pointers as described in PKFEAT.HPP      |
 |                sorted in a special way such that the features              |
 |                having Type and Model fields containing wildcard            |
 |                characters, are ranked behind specific-ones.                |
 |                Thus, using a linear search, the more specific entries      |
 |                are found first.                                            |
 |                                                                            |
 |   PKFeatDescList - An array of PKFeatDesc pointers (features with their    |
 |                    machine type and model and their descriptions as        |
 |                    described in PKFEAT.HPP) sorted like 'PKFeatList'       |
 |                                                                            |
 |   PKFeatSS       - A regularly sorted collection of PKFeatDesc objects.    |
 |                                                                            |
 |   PKNonMRList    - An array of PKBase pointers representing a list         |
 |                    of Non-machinereadable features.                        |
 |                                                                            |
 |   SpecificFeat   - An object to manipulate a feature and its description   |
 |                                                                            |
 |   SpecificFeatList- A collection of SpecificFeat objects for a             |
 |                     particular generic feature (type-model-feature)        |
 |                                                                            |
 |   ProductKnowledge - An object to retain and provide the System's          |
 |                      knowledge, containing:                                |
 |                       Product Knowledge Feature List                       |
 |                          currently supplied by FEATURE.TXT                 |
 |                       Product Knowledge Generic Feature List               |
 |                          currently supplied by GENERIC.TXT                 |
 |                       System's Machine Readable Feature Description List   |
 |                          this is different for every system being          |
 |                          resolved and is currently supplied by             |
 |                          the "System".DSC file downloaded with the         |
 |                          System's AAS and MRDB files.                      |
 |                       Non-Machinereadable Feature List                     |
 |                          a byproduct from generating the                   |
 |                          Feature List.                                     |
 \*--------------------------------------------------------------------------*/
#ifndef _AB_PKFEATL_HPP_
#define _AB_PKFEATL_HPP_

#include "pkfeat.hpp"
#include "parr.hpp"          // PtrRecArr
#include "euoutil.hpp"       // for Tellem

#pragma pack(4)

class PKArr : public PtrRecArr
{
  char * PointBuffer;

//  PKArr( unsigned, unsigned, unsigned,comp_fn_tp){} //hide
  PKArr & operator=(const PKArr &); //hide & don't define
  PKArr(const PKArr &); //hide & don't define

 protected:
  Boolean     releaseBuffer();

 public:
  PKArr ( unsigned   lrecl = 80
         ,unsigned   Initial_Size = 10
         ,unsigned   Automatic_Expansion = 3
         ,comp_fn_tp = 0);
  ~PKArr();
  Boolean     pointToMem( char *Storage, unsigned count);

};

class PKNonMRList : public PKArr
{

 public:
  PKNonMRList();
  ~PKNonMRList(){}
  PKBase  *   element(unsigned index);
  Boolean     contains( const char * szType
                       ,const char * szModel
                       ,const  char * szFeat);
  Boolean     fReadB(FILE *f, unsigned count);

  friend ostream& operator<< (ostream& os, PKNonMRList & list)
                {
                  os   << "    ----> PK Non-Machinereadable Feature List ( "
                       << list.NumberElements()
                       << " elements) <---- " << endl;
                 for (int i = 0, N = list.NumberElements() ; i < N ; i++ )
                    list.element(i)->out(os) << endl;
                 return (os << endl);
                }
};

class PKFeatList : public PKArr
{

  Boolean     quantityWord(const IString &,unsigned *);

 public:
  PKFeatList();
  ~PKFeatList();
  PKFeat   *   element(unsigned index);
  void        release();
  Boolean     readFile(char * szFileName,PKNonMRList * pNonMRList = 0);
  Boolean     readFile(char * szFileName,Tellem *, PKNonMRList * pNonMRList = 0);
  Boolean     fReadB(FILE *f, unsigned count);

  friend ostream& operator<< (ostream& os, PKFeatList & list)
                {
                  os   << "    ----> PK Feature List ( "
                       << list.NumberElements()
                       << " elements) <---- " << endl;
                 for (int i = 0, N = list.NumberElements() ; i < N ; i++ )
                    os << *(list.element(i)) << endl;
                 return (os << endl);
                }
};

class PKFeatDescList : public PKArr
{
  const PKFeatDesc * findLike( const char * type
                              ,const char * model
                              ,const char * feature);

 public:
  PKFeatDescList();
  ~PKFeatDescList();
  PKFeatDesc   *   element(unsigned index);
  void        release();
  Boolean     readFile(char * szFileName);
  Boolean     readFile(char * szFileName, Tellem *);
  Boolean     fReadB(FILE *f, unsigned count);
  const char * description( const char * szType
                           ,const char * szModel
                           ,const  char * szFeat);
  const char * descriptionLike( const char * szType
                               ,const char * szModel
                               ,const  char * szFeat);

  friend ostream& operator<< (ostream& os, PKFeatDescList & list)
                {
                  os   << "    ----> PK Feature Description List ( "
                       << list.NumberElements()
                       << " elements) <---- " << endl;
                 for (int i = 0, N = list.NumberElements() ; i < N ; i++ )
                    os << *(list.element(i)) << endl;
                 return (os << endl);
                }
};


#include <isrtset.h>

class SpecificFeat
{
  IString Code ;
  IString Desc;

 public:

  SpecificFeat(const IString & code, const IString & desc = IString())
                 : Code(code) ,Desc(desc){}
  SpecificFeat(const char * szCode, const char * szDesc = 0)
                 : Code(szCode) ,Desc(szDesc){}
  SpecificFeat() {}

  IString const& strCode()  const { return Code; }
  IString const& strDescription() const { return Desc;}
  const char *   code() const    { return (char *)Code;  }
  const char *   description() const { return (char *)Desc; }

  SpecificFeat & operator=(SpecificFeat const& other);
  Boolean operator==(SpecificFeat const& other)  const
     { return ( (Code == other.Code) && (Desc == other.Desc));}

  Boolean operator< (SpecificFeat const& other) const
         { return ( (Code == other.Code)?
                   (Desc < other.Desc) : (Code < other.Code)); }

  friend ostream& operator<< (ostream& os, SpecificFeat const& g)
                        {
                          os << "Code : '"     << g.code()
                             << "' Description : '" << g.description()
                             << "'";
                          return os;
                        }
};

class SpecificFeatList : public ISortedSet<SpecificFeat>
{
 IString MType ;
 IString MModel;
 IString Generic;

  public:
    class SpecificFeatListCursor : public ISortedSet<SpecificFeat>::Cursor
       {
         public:
            SpecificFeatListCursor(SpecificFeatList & GL) : ISortedSet<SpecificFeat>::Cursor(GL) {}
       };

    SpecificFeatList( const IString & generic
                     ,const IString & type = IString("*")
                     ,const IString & model = IString("*"))
                    : MType(type), MModel(model), Generic(generic){}
    SpecificFeatList( const char * szgeneric
                     ,const char * sztype = "*"
                     ,const char * szmodel = "*")
                    : MType(sztype), MModel(szmodel), Generic(szgeneric){}
    SpecificFeatList() {}

    IString const& strMachType()  const { return MType; }
    IString const& strMachModel() const { return MModel;}
    IString const& strGeneric() const { return Generic;}
    const char *   machType()  const { return (const char *)MType; }
    const char *   machModel() const { return (const char *)MModel;}
    const char *   generic() const { return (const char *)Generic;}
    void           setMachType(const IString & type) { MType = type; }
    void           setMachModel(const IString & model) { MModel = model;}
    void           setGeneric(const IString & gen) {  Generic = gen;}
    void           setMachType(const char * type) { MType = IString(type); }
    void           setMachModel(const char * model) { MModel = IString(model);}
    void           setGeneric(const char * gen) {  Generic = IString(gen);}

//    ostream&   out(ostream&);

    friend ostream& operator<< (ostream& os, SpecificFeatList & GL)
                 {
                  os << "Machine Type  : '"     << GL.machType()
                     << "' Machine Model : '" << GL.machModel()
                     << "' Generic : '" << GL.generic()
                     << "'" << endl;
                  Cursor crsr(GL);
                  forCursor(crsr)
                    os << GL.elementAt(crsr) << endl;
                  return os;
                 }


//friend ostream& operator<< (ostream& os, XPLN2List & x) { return x.out(os);}
};


class PKFeatSS : public ISortedSet<PKFeatDesc>
{
  public:
    friend ostream& operator<< (ostream& os, PKFeatSS & ss)
                 {
                  Cursor crsr(ss);
                  forCursor(crsr)
                    os << ss.elementAt(crsr) << endl;
                  return os;
                 }
};

class ProductKnowledge
{
 PKFeatList       PKFL;           // Product Knowledge Feature List
 PKFeatDescList   PKGenFDL;       // Product Knowledge Generic Feature List
 PKFeatDescList * pMdbFDL;        // System's Machine Readable Feature Description List Ptr
 PKNonMRList      PKNMRL;         // Non-Machine Readable Feature List
 char *           Buff;                   // utility buffer
 IString          FeatFile;               // current PK Feature file name
 Boolean          fUsingCompiledPK;       // using compiled version

 ProductKnowledge & operator=(const ProductKnowledge &); //hide & don't define
 ProductKnowledge(const ProductKnowledge &); //hide & don't define

 public:
  ProductKnowledge();
  ~ProductKnowledge();

 PKFeatList &  pkfl(){return PKFL;}
 PKFeatDescList & pkgenfdl(){ return PKGenFDL;}

  Boolean    save(char * szFileName = 0);
  int        reBuild(char * szFileName = 0);
  void       setMRDescListPtr( PKFeatDescList * p) { pMdbFDL = p;}
  Boolean    build( char * szPKFeatFile ,char *szPKGenericDescFile = 0);
  Boolean    buildNew( char * szPKFeatFile ,char *szPKGenericDescFile);
  Boolean    build(Tellem *, char * szPKFeatFile ,char *szPKGenericDescFile = 0);
  Boolean    buildX(Tellem * pTellem, char * szCompiledFeatFile ,
                   char * szPKFeatFile = 0 , char *szPKGenDescFile = 0);

  unsigned    specificFeatures( const char * szType
                               ,const char * szModel
                               ,const  char * szMRFeat
                               ,PtrStrArr & List);

  unsigned    specificFeat_Desc(
                                 SpecificFeatList & List
                                ,const IString & generic
                                ,const IString & type
                                ,const IString & model);

  unsigned    specificFeat_Desc( SpecificFeatList & List);
  unsigned    specificFeat_Desc(
                                SpecificFeatList & List
                               ,const char * Generic
                               ,const char * Type
                              ,const char * Model);

  Boolean     comboMRFeatFromAASFeat( const char * szType
                                     ,const char * szModel
                                     ,const char * szAASFeat
                                     ,IString & MRFeat1, unsigned *pQty1
                                     ,IString & MRFeat2, unsigned *pQty2);
  Boolean     machineFromMRFeature( const char * szMRFeat
                                   ,IString * pStrType = 0
                                   ,IString * pStrModel = 0);
  Boolean     isGenericMachine( const char * szType
                               ,const char *szModel
                               ,IString * pStrGeneric = 0);
  Boolean     isGenericFeature( const char * type
                               ,const char * model
                               ,const char * feature);
  unsigned    genericMachines( const char * szType
                              ,const char * szModel
                              ,PtrStrArr & List);
  unsigned    machinesMapping2GenericFeature( const char * feat,
                                              SpecificFeatList & List);
    unsigned  machinesMapping2GenericFeature( const char * feat,
                                              PKFeatSS & List);

  const char * genericDescription( const char * type
                                  ,const char * model
                                  ,const char * feature);
   Boolean  strGenericDescription( const char * type
                                  ,const char * model
                                  ,const char * feature
                                  ,IString & str);
  const char * description( const char * type
                           ,const char * model
                           ,const char * feature = 0);
   Boolean  strDescription( const char * type
                           ,const char * model
                           ,const char * feature
                          ,IString & str);
   Boolean  isMachineReadable( const char * type
                              ,const char * model
                              ,const char * feature);

   Boolean   usingCompiledPK() { return fUsingCompiledPK;}
   IString   currentPKFile() { return FeatFile;}
   static int check4CompiledPKFile(const char *);

   ostream&  PKOut(ostream& os);

  friend ostream& operator<< (ostream& os, ProductKnowledge & pk)
                {
                  return pk.PKOut(os);
                }

};

#pragma pack()
#endif


