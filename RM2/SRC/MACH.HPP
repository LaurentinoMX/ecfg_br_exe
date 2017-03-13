#ifndef _AB_MACH_HPP_
#define _AB_MACH_HPP_
 /*--------------------------------------------------------------------------*\
 |    File Name  :  MACH.HPP                                                  |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |      Machine      -   RM2/2's Machine Class                                |
 |                                                                            |
 |       Some background on Partial Reconciliation ( the ability              |
 |            to write and retrieve the machine to and from                   |
 |            a file):                                                        |
 |              All variable length allocated storage associated with         |
 |              the machine (i.e. its features (MFL) and CFReport Records     |
 |              (CFRLists)) are not contained within the                      |
 |              object when it's stored in a file.                            |
 |              Except for the 'MachRecs' records, these objects are          |
 |              numbered when collected (via the 'pickUp()' method)           |
 |              throughout the System and written separately to the file.     |
 |              The MachRecs records are appended to the object in the file.  |
 |              After the object is read from the file (including the         |
 |              appended MachRecs records), it is then                        |
 |              able (via that retained storage information) to identify      |
 |              its objects among the Lists presented in the                  |
 |              'delivery()' method), thereby restoring itself                |
 |              to its former state).                                         |
 | F86752       add a new tag for systems reconciled by Inventory Services   |
 | march-2004
 | D95997 11/19/04 inlcude add06rec method                                    |
 \*--------------------------------------------------------------------------*/

#include "cfrv.hpp"                   // CFRecVL
#include "mfeatl.hpp"                 // MFeatList
#include "pkfeatl.hpp"                // ProductKnowledge
#include "cfr05.hpp"                  // Commnet Records

class RPO2List;                       // RPO Collection
class XPLN2List;                      // Explanation Collection

#pragma pack(4)

class Machine
{

public:

   enum TagType                                // Statuses
     {
       Process        = 0
      ,AAS_Passthru
      ,Software
      ,Backlog_HW
      ,Backlog_SW
      ,Services   //* F86750 Added the Services machines TAG identifier  HECC: 01/13/2004
      ,Process_IS   //ZOR F86752 req  1373
     };

  Machine() { init(); }
  ~Machine(){}
  Machine(const Machine & other) { *this = other;}
  Machine(const char * szType,const  char * szModel,const char * szSerialNo = 0)
                            { init(szType,szModel,szSerialNo); }
  Machine(Record_54 *);
  Machine(Record_47 *    , const char * serialNo);
  Machine(Record_49 *    , const char * serialNo);
  Machine(Record_SW_V31 *, const char * serialNo);
  Machine(Record_08V31 * , const char * serialNo);
  Machine(Record_08    * , const char * serialNo);
  Machine(Record_17    * , const char * serialNo);  /* F72273  HECC: 06/28/2002 */
  Machine(Record_38_V31 * , const char * serialNo); /* F86750  HECC: 01/13/2004 */


  Machine & operator=(Machine const&);
  Boolean operator==(Machine const& other) const { return !compare(other);}
  Boolean operator< (Machine const& other) const { return (compare(other) < 0);}
  Boolean operator> (Machine const& other) const { return (compare(other) > 0);}

  void           setTag(TagType t) { Tag = t;}
  TagType        tag() { return Tag;}
  const char *   type() const { return (const char *)Type; }
  const char *   model() const { return (const char *)Mod; }
  IString        description();
  unsigned       number() { return Nmbr;}
  void           setNumber(unsigned u) { Nmbr = u;}
  Boolean        setDescription(const char * Desc, Boolean fMRDB);
  Boolean        installDate(IDate &,Boolean fMRDB = false);
  Boolean        setInstallDate(IDate &,Boolean fMRDB = false);
  const char *   serialNo() const { return (const char *)SerNo; }
  IString &      displaySerialNo(IString &);
  IString        displaySerialNoStr();
  int            Write(FILE *f);
  int            fWriteCFR(FILE *f,Boolean fMRDB,
                           unsigned * pInstId,
                           CFRInstL & L55_In, CFRInstL & L55_Out,
                           CFRInstL & L56_In, CFRInstL & L56_Out);
  int            fWriteCFRV31(FILE *f,Boolean fMRDB,
                              unsigned * pInstId,
                              CFRInstL & L55_In, CFRInstL & L55_Out,
                              CFRInstL & L56_In, CFRInstL & L56_Out,
                              CFR05List & CommentRecList);
  void           setSerialNo(char * SerialNo);
  void           setSubsystemRecordId(Boolean fMRDB,int id)
                                   { SubsysRecId[fMRDB] = id;}
  int            subsystemRecordId(Boolean fMRDB)const
                                   { return SubsysRecId[fMRDB];}
  void Machine::BaseInit(Machine const&);
  int            compareBySubsystem(Machine const& other,Boolean fMRDB = false) const;
  int            compareByType(Machine const& other) const;
  int            compare(const char * szType,const char * szMod,
                         const char * szSerial= 0) const;
  int            compareTypeSerial(const char * szType,
                                   const char * szSerial) const;
  int            compare(Machine const& other) const
                    {return compare(other.Type,other.Mod,other.SerNo);}
  int            compareTypeSerial(Machine const& other) const
                    {return compareTypeSerial(other.Type,other.SerNo);}
  MFeatList &    mfeatList() { return MFL;}
  CFRecV &       machineRec(Boolean fMRDB) { return MachRecs[fMRDB];}
  CFRecVL &      infoRecList(Boolean fMRDB);
  CFRecVL &      rec54List(Boolean fMRDB);
  CFRecVL &      otherRecList(Boolean fMRDB);

  unsigned       machineRecVersion(Boolean fMRDB)
                      { return machineRec(fMRDB).id();}
  void           setFirstMRDBInd(Boolean f = true) { fFirstMRDB = f;}
  Boolean        isFirstMRDB()const { return fFirstMRDB;}
  Boolean        hasAASData() { return (MachRecs[0].length() != 0);}
  Boolean        hasMRDBData() { return (MachRecs[1].length() != 0);}
  Boolean        hasData(Boolean fMRDB) { return (MachRecs[fMRDB].length() != 0);}
  void           setMsgInd(Boolean f = true) { fMsg = f;}
  unsigned       modelQty(Boolean fMRDB = true);
  Boolean        setModelQty(unsigned qty,Boolean fMRDB = true);
  Boolean        hasMessages(){ return fMsg;}
  void           setMachineRec(Boolean fMRDB,char *,unsigned version = 30);
  void           setMachineRec(Boolean fMRDB,CFRecV &r,unsigned version = 30);
  Boolean        addInfoRec(Boolean fMRDB,const char *);
  Boolean        addInfoRec(Boolean fMRDB,CFRecV & r);
  Boolean        add54Rec(Boolean fMRDB,const char *);
  Boolean        add54Rec(Boolean fMRDB,CFRecV & r);
  Boolean        add06Rec(Boolean fMRDB,const char *); // ZOR 95997
  Boolean        addOtherRec(Boolean fMRDB,CFRecV &);
  Boolean        addOtherRec(Boolean fMRDB,const char *);
  void           Release();
  Boolean        setIPProdInd(char Ind);
  char           productIndicator() { return cIPProdInd;}
  Boolean        checkMachRecQuantities( Boolean fMRDB
                                        ,Tellem & tellem
                                        ,const char * CFRFile
                                        ,Boolean fCreateMissingFeatures = true);

  Boolean        setMachRecFromCFRList(Boolean fMRDB);
  Boolean        fWriteB(FILE *f, unsigned *pBytes = 0);
  unsigned       fWriteBLength();
  Boolean        fReadB(FILE *f, unsigned *pBytes = 0);
  void           pickUp(MFeatList &, CFRecVL &);
  Boolean        delivery(MFeatList &, CFRecVL &);
  void           processAASPassthru();
  void           resolveGenerics(  ProductKnowledge &
                                 , Boolean fTagPotentialMachines
                                 , RPO2List &
                                 , const char *msgFile
                                 , XPLN2List & );
    void         resolveMRCombos( ProductKnowledge & PK
                                 ,const char *msgFile
                                 ,XPLN2List & XPLNL);
  void           resolveNonEquals(  long MRFileDate   // Sense Date
                                  , RPO2List &
                                  , const char *msgFile
                                  , XPLN2List & );
  unsigned       qtyCFR(MFeat::CFRType);
  void           applyPK( ProductKnowledge & );
  Boolean        isSoftwareMatch(const Record_17 *,Boolean fMRDB = false);
  Boolean        isSoftwareMatch(const Record_SW_V31 *,Boolean fMRDB = false);
  void           setModel(const char * szModel);
  Boolean        copyRecords(Machine * pToM,Boolean fMRDB_To,Boolean fMRDB_From,
                             int toVersion, Boolean fMove,Boolean fUpdateFeatureCount);
  void           getConnections(unsigned * pNextInstanceId, CFRecVL & Rec54List,
                                CFRInstL & L55_In, CFRInstL & L55_Out,
                                CFRInstL & L56_In, CFRInstL & L56_Out);
  unsigned       referenceNotesNumber(Boolean fMRDB = false);
  void           consumeUnresolveds();
  void           resolveServicesRecs();    // F86750: Change the RT 38 features to resolved  HECC - 01/13/2004
  MFeatList      MachSecFeatList;          // Product's secondary features  //edw 5/15/98
  MFeatList      DuplicateRec54List;       // Collection of dup. 54 records //edw 4/28/98
  void           setSystemId(IString strId){strSystemId = strId;}           //edw06/04/1998
  IString        getSystemId(){return strSystemId;}                         //edw06/04/1998
  void           setSIPOFlag(Boolean b) {bIsSIPO = b;}                      //VBB09192002 D74157

  //* F73686. Begin New Logic: Returns the Product category bassed in the Tag
  //* contained in the Machine object.
  char           category() { return (Tag == Machine::Software ||
                                      Tag == Machine::Backlog_SW ?
                                      'S':'H'); }
  //* F73686. End New Logic.


  //* D83402. Begin New Logic: Adds four new functions to return the data of the RT 95 of the product that the
  //* RPO belongs to:
  IString        customerNumber();    //* Returns the customber number of the product which the RPO belongs to.
  IString        systNumberFlag();    //* Returns the system number flag of the product which the RPO belongs to.
  IString        systemType();        //* Returns the system type of the product which the RPO belongs to.
  IString        systemNum();         //* Returns the system number of the product which the RPO belongs to.
  //* D83402. End New Logic.    HECC - 07/28/2003


  friend ostream& operator<< (ostream& os, Machine & m) {
                  IString str;
                   m.displaySerialNo(str);
                  os  << "Machine "
                      << " -> Type: '" << m.type()
                      << "' Model: '" << m.model()
                      << "' SerialNo: '" << m.serialNo()
                      << "' DisplaySerialNo: '" << str
                      << "' Messages: " << (m.hasMessages()? "Y" : "N")
                      << "' AAS Subsystem Id : " << m.subsystemRecordId(false)
                      << endl
                      << "machineRec(AAS) : " << endl << m.machineRec(false) << endl
                      << "machineRec(MRDB): " << endl << m.machineRec(true) << endl
                      << "infoRecList(AAS).NumberElements() : " << m.infoRecList(false).NumberElements() << endl
                      << m.infoRecList(false) << endl
                      << "infoRecList(MRDB).NumberElements() : " << m.infoRecList(true).NumberElements() << endl
                      << m.infoRecList(true) << endl
                      << m.mfeatList();
                  return os;
                  }
 private:
   char       Type[MACH_TYPE_SIZE + 1];           // type
   char       Mod[MACH_MODEL_SIZE + 1];           // model
   char       SerNo[MACH_SERIALNO_SIZE + 1];      // serial #
   char       cIPProdInd;                         // Product Indicator
   TagType    Tag;                            // Status field
   Boolean    bIsSIPO;                        // SIPO Indicator             VBB09192002 D74157
   MFeatList  MFL;                            // Machines's features
   CFRStore   CFRStoreInfo[ MACH_NMBR_CFR_LISTS ]; //CFRLists storage info
   CFRecVL    CFRLists[ MACH_NMBR_CFR_LISTS ]; // Machine's different record lists:
                                               // Index    Type
                                               //   0      AAS Info Recs
                                               //   1      BII Info Recs
                                               //   2      AAS 54 Recs
                                               //   3      BII 54 Recs
                                               //   4      AAS Other Recs
                                               //   5      BII Other Recs
   CFRStore   FeatStoreInfo;                   //  MFL storage onfo
   CFRecV     MachRecs[2];              // machine record e.g. 08,19 47 etc
   Boolean    fMsg;                     // Message Indicator
   Boolean    fFirstMRDB;               // System's first machine Indicator
   unsigned   Nmbr;                     // System's Machine Storage number
   int        SubsysRecId[2];           // Subsystem Record Id
   void       init(const char * type = 0,const char * mod = 0,
                   const char * serNo = 0);
   void       subInit(TagType);
   IString    strSystemId;              // System that contains the machine   //edw06/04/1998
};


#pragma pack()
#endif


