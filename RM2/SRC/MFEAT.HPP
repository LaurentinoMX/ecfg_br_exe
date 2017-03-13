 /*--------------------------------------------------------------------------*\
 |    Filename - MFEAT.HPP                                                    |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |     MFeat    - A CFReport Machine Feature Object.                          |
 |     CFRStore - Retains information of a MFeats's Record List               |
 |                when the object is written to a file.                       |
 |                This data is used when the object is rebuild                |
 |                from a file.                                                |
 |                One object for every list.                                  |
 |                ( MFeat's are written to a file without their               |
 |                  Record Lists. The records in these lists                  |
 |                  are numbered when collected ( via MFeat's 'pickup()'      |
 |                  method) throughout the System                             |
 |                  and written separately into the file.                     |
 |                  When the object is read from the file, it can then        |
 |                  identify the records in the list read separately          |
 |                  and rebuild its Record Lists ( via MFeat's                |
 |                  'delivery()' method), thereby restoring itself            |
 |                  to its former state).                                     |
 |  F86752   include Resolve_IS for features reconciled by Invntory Services  |
 |  march-2004
 \*--------------------------------------------------------------------------*/
#ifndef _AB_MFEAT_HPP_
#define _AB_MFEAT_HPP_

#include "euodef.h"                        // CFReport structures
#include "cfrec.hpp"                       // CFRec
#include "cfrv.hpp"                        // CFRecVL
#include "cfr54.hpp"                       // CFR54
#include "cfr96.hpp"                       // CFR96

#include <idate.hpp>                       // ICLUI's IDate
#include <isrtset.h>                       // ISorted Set

#pragma pack(4)
class CFRStore
{
   friend class MFeat;
   friend class Machine;
   friend class RMSystem;
   unsigned RecNmbr;
   unsigned qty;
 public:
   CFRStore() : RecNmbr(0), qty(0) {}
   CFRStore(const CFRStore& other){ *this = other;}
   CFRStore &  operator=(CFRStore const& other);
};

class MFeat
{
 public:

   enum CFRType
     {
       AASType = 0
      ,MRDBType
      ,OtherType
     };

   enum TagType
     {
       Undone         = 0
      ,Resolved
      ,Resolved_EUOP
      ,Unresolved
      ,Resolved_User
      ,NoCompareTblResolved    //edw10/15/99
      ,Resolved_EUOP_S390
      ,ISeriesSWTblResolved    //F72273  HECC: 07/03/2002
      ,Potential_Machine = 10
      ,Resolved_IS             // ZOR F86752 req 1373
     };
    char           mismatchType() { return cMismType;}  //F104628 RERC 30/01/2006
    MFeat();
    MFeat(const MFeat& other) { *this = other; }
    MFeat(const char * szFeature) { init(szFeature); }
    MFeat(const char * szFeature, Boolean fgeneric, Boolean fMachinereable);

    MFeat(Record_54 & rc) { init(rc.Feature); }
    MFeat(Record_96 * p96) { init(p96->Feat_RPQ_PartNmbr); }
    ~MFeat(){}

    MFeat & operator=(MFeat const& other);
    Boolean operator==(MFeat const& other) const
                     { return !compare(other);}
    Boolean operator< (MFeat const& other) const
                     { return (compare(other) < 0);}
    Boolean operator> (MFeat const& other) const
                     { return (compare(other) > 0);}

    MFeat &        duplicate(MFeat const& other);
    const char *   feature() const { return (const char *)Feat; }
    int            Write(FILE *f);
    int            Write(FILE *f,CFRType);
    int            fWriteCFR(FILE *f, CFRType type,unsigned version);
    unsigned       qtyCFRList(CFRType type)
                     { return CFRLists[type].NumberElements();}
    CFRecVL   &  cfrList(CFRType type) { return CFRLists[type];}
    CFRecVL   &  cfrList(Boolean fMRDB) { return CFRLists[fMRDB];}
    unsigned       number() { return Nmbr;}
    unsigned       qty(CFRType type) { return Qty[type];}
    void           setQty(CFRType type,unsigned q) { Qty[type] = q;}
    void           setNumber(unsigned u) { Nmbr = u;}
    void           setBase(unsigned u) { Base = u;}
    void           setMsgInd(Boolean f) { fMsg = f;}
    void           setMRInd(Boolean f) { fMR = f;}
    Boolean        isMachineReadable() { return fMR;}
    Boolean        hasMessages(){ return fMsg;}
    unsigned       baseNmbr() { return Base;}
    void           setTag(TagType t) { Tag = t;}
    TagType        tag() { return Tag;}

    Boolean        addCFR(CFRType,CFRecV *,Boolean fKeepCount = true);
    Boolean        addCFR(CFRType,CFRecV &,Boolean fKeepCount = true);
    Boolean        addCFR(CFRType type,const char *,Boolean fKeepCount = true);
    Boolean        addCFR(Boolean fMRDB,Record_54 & r,Boolean fKeepCount = true);

    int            compare(const char * szFeat) const;
    int            compare(MFeat const& other) const
                      { return compare(other.Feat); }
    void           pickUp(CFRecVL &);
    Boolean        delivery(CFRecVL &);
    void           BaseInit(MFeat const&);
    void           Release();
    Boolean        resolveMatches();
    Boolean        resolveUsingNoCompareTable( IString strType );   //edw07/07/1998
                   //49222-MAMT-13/08/2000 eliminate ISortedSet<IString>* pNoCompTable
    void           resolveUsingNoCompareTable1( ISortedSet<IString>* pNoCompTable,
                                                IString strType,    //edw07/07/1998
                                                IString strModel ); //49222-MAMT-13/08/2000
    Boolean        setInstType(char type,Boolean fMRDB = true);
    Boolean        setFeature(const char * szFeature,Boolean fMRDB = true);
    Boolean        setInstallDate(long lYMD, Boolean fMRDB = false);
    Boolean        setInstallDate(const IDate &, Boolean fMRDB = false);
    Boolean        setCRADDate(const IDate & date, Boolean fMRDB = true);
    Boolean        setCRADDate(long lYMD, Boolean fMRDB = true);
    Boolean        setCSDDate(const IDate & date, Boolean fMRDB = false);
    Boolean        setCSDDate(long lYMD, Boolean fMRDB = false);
    long           installDateYMD(Boolean fMRDB = false);
    char           instanceType(Boolean fMRDB = true);
    long           cradDateYMD(Boolean fMRDB = true);
    long           csdDateYMD(Boolean fMRDB = false);
    Boolean        installDate(IDate &,Boolean fMRDB = false);
    Boolean        cradDate(IDate &,Boolean fMRDB = true);
    Boolean        csdDate(IDate &,Boolean fMRDB = false);
//    Boolean        instId(IString & str,Boolean fMRDB = true);

    Boolean        isInventory();
    Boolean        isBackLog();
    Boolean        isGeneric(){return fGeneric;}
    void           setGenericInd(Boolean f) { fGeneric = f;}
    Boolean        isOrderable(Boolean fMRDB);
    Boolean        hasDescription(Boolean fMRDB);
    const char *   description(Boolean fMRDB = false);
    Boolean        description(IString & str,Boolean fMRDB = false);
    Boolean        setDescription(const char * Desc,Boolean fMRDB = true);
    CFR54::SourceType   source();

    Boolean        removeRecords(unsigned N, CFRType type);
    Boolean        copyRecords(MFeat * pOther, unsigned N,
                               CFRType toType, CFRType fromType,
                               int toVersion,Boolean fMove, Boolean fKeepCount,
                               const char * szMachineType = 0,
                               const char * szMachineModel = 0,
                               const char * szSerialNo = 0);
    Boolean        getConnections( Boolean fMRDB,unsigned InstanceId,
                                   CFRInstL & L55_In, CFRInstL & L55_Out,
                                   CFRInstL & L56_In, CFRInstL & L56_Out);
    Boolean        getConnections( CFRType type,unsigned InstanceId,
                                    CFRInstL & L55_In, CFRInstL & L55_Out,
                                    CFRInstL & L56_In, CFRInstL & L56_Out);
    void           setModel(const char *szModel);
    void           setSerialNo(const char *szNmbr);
    Boolean        markAsOEM();
    Boolean        isMarkedAsOEM();
    void           markAsPartial();
    Boolean        isMarkedAsPartial();
    Boolean        isMarked();
    void           unMark();
    unsigned       referenceNotesNumber(Boolean fMRDB = false);

    friend ostream& operator<< (ostream& os, MFeat & feat)
             {
               os << "Feature: '"  << feat.feature() << "'" << endl
                  << "Machine Readble : " << (feat.isMachineReadable()? "Y" : "N") << endl
                  << "Base  : " << feat.baseNmbr() << endl
                  << "AAS qty  : " << feat.qty(MFeat::AASType) << endl
                  << "MRDB qty : " << feat.qty(MFeat::MRDBType) << endl
                  << "Other qty : " << feat.qty(MFeat::OtherType) << endl
                  << "Other qty : " << feat.qty(MFeat::OtherType) << endl
                  << "AAS-List qty  : " << feat.qtyCFRList(MFeat::AASType) << endl
                  <<  feat.cfrList(MFeat::AASType) << endl
                  << "MRDB-List qty : " << feat.qtyCFRList(MFeat::MRDBType) << endl
                  <<  feat.cfrList(MFeat::MRDBType) << endl
                  << "Other-List qty : " << feat.qtyCFRList(MFeat::OtherType) << endl
                  <<  feat.cfrList(MFeat::OtherType) << endl;
               return os;
             }

 private:
   char       Feat[FEAT_SIZE + 1];
   CFRStore   CFRStoreInfo[ FEAT_NMBR_CFR_LISTS ];
   CFRecVL    CFRLists[ FEAT_NMBR_CFR_LISTS ];
   unsigned   Qty[ FEAT_NMBR_CFR_LISTS ];
   unsigned   Nmbr;                  // machine's feature number
   unsigned   Base;
   char       cMismType;            // F104628 RERC 30/01/2006 Mismatch Type, either Accepted or Unaccepted
   TagType    Tag;                   // Status
   Boolean    fMsg;                 // Messages Indicator
   Boolean    fMR;                  // Machinereadable Indicator
   Boolean    fGeneric;
   char       cMark;

   void      init(const char * feature = 0);
};
#pragma pack()
#endif

