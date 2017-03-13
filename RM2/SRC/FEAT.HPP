 /*--------------------------------------------------------------------------*\
 |    File Name  : FEAT.HPP                                                   |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |     Feat     - Feature object wrapper, merely containing the pointers      |
 |                to the Machine and MFeat objects.                           |
 |                Methods to manipulate the underlying CFReport machine       |
 |                and feature objects, do so directly. There is no copy.      |
 |                Intended for EUI for which also can use the ID data         |
 |                (easier when working with containers)                       |
 |                The storage numbers int the object are used to establish    |
 |                a connection with the Machine and MFeat object during       |
 |                file IO.                                                    |
 | F86752         add a new tag Resolved_IS for the features reconciled by    |
 |  march-2004    Inventory Services                                          |
 \*--------------------------------------------------------------------------*/

#ifndef _AB_FEAT_HPP_
#define _AB_FEAT_HPP_

#include "euodef.h"                     // CFReport structures
#include "mfeat.hpp"                    // MFeat object
#include "mach.hpp"                     // Machine object

#pragma pack(4)

class Feat
{
  friend class RMSystem;
  friend class FeatList;
  friend class RMDiscResoW;

    Machine * pM;
    MFeat   * pF;
    unsigned  Nmbr;
    unsigned  MachNmbr;
    int       ID;
    Feat();
    void      setStorageNumbers();
    void      setPointers(Machine * m,MFeat * f) { pM = m; pF = f;}
    unsigned  storageNumber() { return Nmbr;}
    unsigned  machineStorageNumber() { return MachNmbr;}
 public:
   enum Status
     {
       Resolved       = MFeat::Resolved
      ,Resolved_EUOP  = MFeat::Resolved_EUOP
      ,Unresolved     = MFeat::Unresolved
      ,Resolved_User  = MFeat::Resolved_User
      ,ISeriesSWTblResolved  = MFeat::ISeriesSWTblResolved  /*F72273  HECC: 07/03/2002*/
      ,Resolved_IS    = MFeat::Resolved_IS   /* ZOR F86752 Req 1373*/
     };

    Feat(const Feat& other) { *this = other;}
    Feat(Machine * m,MFeat * f,int id) : pM(m), pF(f), ID(id) {}
    ~Feat(){}

    Feat &  operator=(Feat const& other);
    Boolean operator==(Feat const& other) const
                         { return !compare(other);}
    Boolean operator< (Feat const& other) const
                         { return (compare(other) < 0);}
    Boolean operator> (Feat const& other) const
                         { return (compare(other) > 0);}
    Boolean           sameMachine(Feat const& other) const
                         { return (pM->compare(*(other.pM)) == 0);}
    Boolean           sameFeature(Feat const& other) const
                         { return ((pF && other.pF)?
                               (pF->compare(*(other.pF)) == 0) : false);}
    Boolean           sameFeature(const char * szFeat) const
                         { return (pF? (pF->compare(szFeat) == 0) : false);}

    int               compare(Feat const& other) const;
    int               compareMachFeat(Feat const& other) const;
    Boolean        setInstallDate(unsigned long lYMD)
                         { return pF? pF->setInstallDate(lYMD):false;}
    Boolean        setInstallDate(const IDate & date)
                         { return pF? pF->setInstallDate(date):false;}
    Boolean        setCRADDate(const IDate & date)
                         { return pF? pF->setCRADDate(date):false;}
    Boolean        setCRADDate(unsigned long lYMD)
                         { return pF? pF->setCRADDate(lYMD):false;}
    void           setBase(unsigned u) { if (pF) pF->setBase(u);}
    void           setMRDBqty(unsigned q)
                            { if (pF) pF->setQty(MFeat::MRDBType,q);}
    void           setSelectedOEM() { if (pF) pF->setInstType('O');}
    unsigned long  installDateYMD() { return pF? pF->installDateYMD():0;}
    unsigned long  cradDateYMD() { return pF? pF->cradDateYMD():0;}
    Boolean        installDate(IDate & date)
                         { return pF? pF->installDate(date):false;}
    Boolean        MachineInstallDate(IDate & date,Boolean fMRDB = false)
                         { return pM? pM->installDate(date,fMRDB):false;}
    Boolean        markAsOEM() { return pF? pF->markAsOEM():false;}
    Boolean        isMarkedAsPartial() { return pF? pF->isMarkedAsPartial():false;}
    Boolean        isMarkedAsOEM() { return pF? pF->isMarkedAsOEM():false;}
    Boolean        cradDate(IDate &date)
                         { return pF? pF->cradDate(date):false;}
    Boolean        isInventory() { return pF? pF->isInventory():false;}
    Boolean        isMachineReadable()
                         { return pF? pF->isMachineReadable():false;}
    Boolean        isBackLog() { return pF? pF->isBackLog():false;}
    Boolean        isGeneric() { return pF? pF->isGeneric():false;}
//  const char *   description(Boolean fMRDB) { return pF->description(fMRDB);}
    Boolean        setDescription(const char * szDesc,Boolean fMRDB = true)
                       { return pF? pF->setDescription(szDesc,fMRDB):false;}
    Boolean        machineDescription(IString & str);
    char           productIndicator() {return (pM? pM->productIndicator() : ' ');}
    Boolean        description(IString & str,Boolean fMRDB = false);
    Boolean        setFeature(const char * szFeature,Boolean fMRDB = true)
                       { return pF? pF->setFeature(szFeature,fMRDB):false;}
    unsigned       qtyBase() { return pF? pF->baseNmbr():0;}
    unsigned       qtyAAS() { return pF? pF->qty(MFeat::AASType):0;}
    unsigned       qtyMRDB() { return pF? pF->qty(MFeat::MRDBType):0;}
    CFR54::SourceType source() { return pF? pF->source(): CFR54::Unknown;}
    int            id() { return ID;}
    void           setId(int i) { ID = i;}
    Boolean        hasMessages(){ return pF? pF->hasMessages(): false;}
    Boolean        hasMachineMessages(){ return pM? pM->hasMessages(): false;}
    Boolean        isFirstMRDBMachine(){ return pM? pM->isFirstMRDB(): false;}
    Boolean        isFeaturelessMachine(){ return (pF == 0);}
    Feat &         setStatus(Status s);
    Status         status();
    const char *   feature() const { return pF? pF->feature():""; }
    char           mismType()  { return pF? pF->mismatchType():' '; }    //F104628 RERC 30/01/2006
    const char *   machine() const { return pM->type(); }
    const char *   model() const { return pM->model(); }
    const char *   serialNo() const { return pM->serialNo(); }
    void           displaySerialNo(IString & str) { pM->displaySerialNo(str);}
    IString        displaySerialNoStr(){return pM->displaySerialNoStr();}
    IString        machineDescStr(){return pM->description();}
    ostream &      out(ostream& os);

    //* F73686. Begin New Logic: Returns the Tag to find out if the product is HW or SW
    char           category() {return pM->category();}
    //* F73686. End New Logic.

    //* D83402. Begin New Logic: Returns additional information of machine's customer, system type, system number
    //* and system number flag.

    const char *   customerNumber() {return pM->customerNumber();}
    const char *   systNumberFlag() {return pM->systNumberFlag();}
    const char *   systemType() {return pM->systemType();}
    const char *   systemNum() {return pM->systemNum();}

    //* D83402  End New Logic:  HECC - 07/28/2003


    friend ostream& operator<< (ostream& os, Feat & f) { return f.out(os);}
};
#pragma pack()
#endif
