#ifndef _AB_SYSTEM_HPP_
#define _AB_SYSTEM_HPP_
 /*--------------------------------------------------------------------------*\
 |    File Name  : SYSTEM.HPP                                                           |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |   RMSystem - containing data and methods associated with the               |
 |            unit to be Reconciled by the application                        |
 | F86752 march 2004 ZOR  this method is defined                              |
 |             Process_06_RecordV31(char * Buffer, Boolean fMRDB, FILE * f)   |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |               RM/2 specific Includes                |
        \*---------------------------------------------------*/
#include "machl.hpp"                    // Machine
#include "pkfeatl.hpp"                  // Product Knowledge
#include "featl.hpp"                    // FeatList Collection
#include "mfeatl.hpp"                   // MFeatList (array of MFeat object)  //edw 4/28/98
#include "blfeatl.hpp"                  // BLFeatList (Backlog Collection)
#include "cfrsw.hpp"                    // Software releated objects
#include "cfr10.hpp"                    // Subsystem Record List
#include "cfr05.hpp"                    // Comment Record List
#include "xpln2l.hpp"                   // Resolution Explanations Collection
#include "rpo2l.hpp"                    // RPO's Collection
#include "cfrv.hpp"                     // CFRecV & CFRecVL (variable length CFR Record objects
#include "pcol6.hpp"                    // Print Format Headers
#include <isrtset.h>                    // ISorted Set


        /*---------------------------------------------------*\
        |               Forward declarations                  |
        \*---------------------------------------------------*/
class XWindow;                          // Application's Main Window

#pragma pack(4)

        /*---------------------------------------------------*\
        |   System Data Structure                             |
        |                                                     |
        | This structure is maintained by rmsystem and        |
        | accompanies the object when saved.                  |
        |                                                     |
        | The CFRStore object is used to retain the System's  |
        |  CFR record numbers when the object is written to a |
        |  file. This data is used when the object is rebuild.|
        |  One object for every list.                         |
        |  ( The object is written to a file without its      |
        |    Record Lists. The records in these lists         |
        |    are numbered when collected ( via the 'pickup()' |
        |    method) throughout the System                    |
        |    and written separately into the file.            |
        |    After the object is read from the file, it can   |
        |    identify its records in the list read separately |
        |    and rebuild its own Lists ( via the 'delivery()' |
        |    method).                                         |
        \*---------------------------------------------------*/
struct System_Data{
          char       Name[2][SYSTEM_NAME_SIZE + 1];     // AAS & MRDB
          char       Title[2][SYSTEM_TITLE_SIZE + 1];   // AAS & MRDB
          char       UserId[2][SYSTEM_USERID_SIZE + 1]; // AAS & MRDB
          char       Number[2][SYSTEM_NUMBER_SIZE+1];   // AAS & MRDB
          char       DateRec53[2][SYSTEM_DATE_SIZE+1];  // AAS & MRDB
          char       InFiles[3][_MAX_PATH + 1];         // AAS,MRDB & System-Specific-Descriptions File Names
          char       MsgFile[MAX_FILENAME_SIZE + 1];    // Log File Name
          char       CustNum[CUSTOM_NUMBER_SIZE + 1];   // Customer Number
          IDate      FileDate[2];                       // AAS & MRDB
          int        iSecurity[2];                      // AAS & MRDB
          int        iVersion[2];                       // AAS & MRDB
       unsigned long ulCheckSum[2];                     // AAS & MRDB
          unsigned   uTotalMachs;
          unsigned   uTotalFeats;
          unsigned   uTotalCFRecs;
          unsigned   uSerNoSeq;   // next new Machine Serial# Sequence
          CFRStore   CFRStoreInfo[ SYSTEM_NMBR_CFR_LISTS ];
        };

class RMSystem
{
 CFRecVL  CFRLists[ SYSTEM_NMBR_CFR_LISTS ]; // System-specific CFR Records
 System_Data *pSD;
 CFRecV   * pARec;                           // Commonly used variable
 Machine * pAMachine;                        // Commonly used variable
 MFeat   * pAFeature;                        // Commonly used variable
 MachineList ML;                             // Machines
 ProductKnowledge PK;
 FeatList   FL;                              // Feature Collection
 Boolean fAS4;                               // iSeries system is loaded //F73732 EDW 07/25/02
// PlacementList L55;
 CFRInstL      L55;                          // Incoming Placement Records
 CFRInstL      L56;                          // Incoming Connection Records
 CFRInstL      L54Orphan;                    // Orphan 54, 55, 56 records //edw05/15/98
 SoftMachList  SWDiscList;                   // Software Discrepancy Collection
 CFR10List     L10[2];                       // Subsystem Records
 CFR05List     L05[2];                       // Comment Records
 CFR52List     L52[2];                       // Exception Records
 BLFeatList    L19_50;                       // Harware and Software Backlog Features
 XPLN2List   XPL;                            // Resolution Explanations
 RPO2List    RPOL;                           // Generated RPO's
 char *      BufK;                           // 1-K Utility Buffer
 char *      BufF;                           // File Name Buffer
 char *      ReadBuff;                       // Read Buffer
 PKFeatDescList  MDBFeatDescL;               // System-specific Descriptions
 Tellem          tellem;
 unsigned    RefNotesNo[2];                  // Comment Linkage

 Boolean      Process_00_Record(char *,Boolean fMRDB);
 void         Process_01_Record(char *,Boolean fMRDB);
 void         Process_03_Record(char *,Boolean fMRDB);
 void         Process_07_Record(char *,Boolean fMRDB);
 int          Process_06_RecordV31(char * Buffer, Boolean fMRDB, FILE * f);   //F86752 1373
 Boolean      collectSWDiscrepancies(FILE *,const Record_17 *);
 Boolean      collectSWDiscrepancies(FILE *f,const Record_SW_V31 *);
 int          Process_19_50_Record(char *,Boolean fMRDB, FILE * f);
 int          Process_19_50_RecordV31(char *,Boolean fMRDB, FILE * f, IString strSysId);  //VBB09192002 D74157
 int          Process_25_Record(char *,Boolean fMRDB, FILE * f);
 int          Process_25_RecordV31(char *,Boolean fMRDB, FILE * f);
 int          Process_47_49_Record(char * Buffer,Boolean fMRDB, FILE * f);
 int          Process_47_49_RecordV31(char * Buffer,Boolean fMRDB, FILE * f);
 int          Process_17_Record(char * Buffer,Boolean fMRDB, FILE * f);  //* F72273  HECC: 06/27/2002
 int          Process_38_RecordV31(char * Buffer,Boolean fMRDB, FILE * f);  //* F86750  HECC: 01/13/2004
 Boolean      editFirstRecord_54(const char * Buffer,Boolean fMRDB);
 void         Process_53_Record(Record_53 *,Boolean fMRDB);
 void         ProcessOrphanRecord(char * Buffer,Boolean fMRDB);
 char *       readCFRLine(FILE *);
 char *       readCFRLine(FILE *f,char * Buffer, unsigned BufLen);
 Boolean      rebuildFL(FeatListMem &);
// void         StubXPLN();
// void         StubRPOL();
 Boolean      delivery(MFeatList &,CFRecVL &);
 void         pickUp(MFeatList &,CFRecVL &);
 void         release();
 Boolean      addCFR(CFRecV &,Boolean fMRDB);
 Boolean      addCFR(char *,Boolean fMRDB);
 IString      newSerialNo();
 Boolean      appendCheckSumRecord(const char * szFileName);
 MFeat  *     findProcessFeature(const char *szType,const char * szModel,
                                 const char *szSerial,const char * szFeature);
// MFeat  *     findProcessOEMFeature(const char *szType,const char * szModel,
//                                    const char *szSerial,const char * szFeature);
// MFeat *      findProcessNonOEMFeature(const char *szType,const char * szModel,
//                                       const char *szSerial,const char * szFeature);
  IString     strExecPath;
  IString     strSysId;                               //edw06/04/1998
  int         iSysCnt;                                //edw06/04/1998
  IString str54, strSerial;                           //57519 - MAMT - 12/29/2000
  ISortedSet<IString> sSystemRecordSet;               //edw06/04/1998


public:
  RMSystem();
  ~RMSystem();
  const char *   name(Boolean fMRDB = true) const
                         { return (const char *)(pSD->Name[fMRDB]); }
  const char *   title(Boolean fMRDB = 0) const
                         { return (const char *)(pSD->Title[fMRDB]); }
  const char *   userId(Boolean fMRDB = 0) const
                         { return (const char *)(pSD->UserId[fMRDB]); }
  const char *   number(Boolean fMRDB = true) const
                         { return (const char *)(pSD->Number[fMRDB]); }
  const char *   dateRec53(Boolean fMRDB = true) const
                         { return (const char *)(pSD->DateRec53[fMRDB]); }

  //* ICB15040873 Begin new logic: The date that must be returned as "Date received at IBM must be
  //* the position '0' of the  pSD->FileDate[] array, because the position '0' is filled from the
  //* AAS file.
  //* IDate &        fileDate(Boolean fMRDB = true) { return pSD->FileDate[fMRDB];}
  IDate &        fileDate(Boolean fMRDB = false) { return pSD->FileDate[fMRDB];}
  //* ICB15040873 End new logic:  HECC - 08/13/2003

  IDate          senseDate();
  long           senseDateYYYYMMDD();
  int            security(Boolean fMRDB = 0) { return pSD->iSecurity[fMRDB];}
  int            version(Boolean fMRDB = 0) { return pSD->iVersion[fMRDB];}
  IString        subsystemDescription(Machine & M,Boolean fMRDB = false)
                    { return L10[fMRDB].description(M.subsystemRecordId(fMRDB)); }
  unsigned       subsystemReferenceNotesNumber(Machine & M, Boolean fMRDB = false)
                 { return L10[fMRDB].referenceNotesNumber(M.subsystemRecordId(fMRDB));}

  MachineList &   machList() { return ML;}
  unsigned       lastErrorNumber() { return tellem.errorNumber();}
  const char *   lastErrorText() { return tellem.errorText();}
  Boolean        setMsgParms(const char * StringFile, const char * OutputFile)
                                { return tellem.set(StringFile,OutputFile); }
  XPLN2List      &   explanations() { return XPL;}
  FeatList       &   featlist() { return FL;}
  RPO2List       &   rpos() { return RPOL;}
  SoftMachList   &   softwareDiscrepancyList() { return SWDiscList;}
  CFR10List      &   subsystemRecordList(Boolean fMRDB = false) { return L10[fMRDB];}
  CFR05List      &   commentRecordList(Boolean fMRDB = false) { return L05[fMRDB];}
  CFR52List      &   exceptionRecordList(Boolean fMRDB = false) { return L52[fMRDB];}
  BLFeatList     &   backlogList() { return L19_50;}
  PKFeatDescList &   MRDescList() { return  MDBFeatDescL;}
//PlacementList  &   l55() { return L55;};
  CFRInstL       &   l55() { return L55;};
  CFRInstL       &   l56() { return L56;};
  ProductKnowledge & pk() { return  PK;};
//  PKFeatList     &   pkfeats() { return PKFeatL;}
  int             RMSystem::Write(FILE *f);
//  Boolean      WriteCFR(const char * szFileName);
  const System_Data * sysData() {return pSD;}
  ostream&        SysDataOut(ostream& os);
  void            SystemOut();
 void         XPLOut();
 const char *    message (  unsigned long Id
                           ,const char*   sz1 = 0
                           ,const char*   sz2 = 0
                           ,const char*   sz3 = 0
                           ,const char*   sz4 = 0
                           ,const char*   sz5 = 0
                           ,const char*   sz6 = 0
                           ,const char*   sz7 = 0
                           ,const char*   sz8 = 0
                           ,const char*   sz9 = 0 );
  int         appendRPOsToCFR(FILE *f,Boolean f20Format = true);
  Boolean     updateWithAlteredGeneric(Feat & f,const char * newFeature,
                                       const char * newDescription,
                                       Feat * pAffectedFeat = 0);
  Boolean     updateWithAlteredGeneric(Feat & f,const char * newFeature,
                                       unsigned qty,
                                       const char * newDescription,
                                       Feat * pAffectedFeat = 0);
  Boolean     updateWithMachinedGeneric(Feat & fG,const char * type,
                                        const char * model,
                                        const char * desc,
                                        FeatList &);
  Boolean     updateWithMachinedGeneric(Feat & fG,unsigned qty,
                                        const char * type,
                                        const char * model,
                                        const char * desc,
                                        FeatList & FList);
  const char *  customer() const
                  { return (const char *)(pSD->CustNum); }
  unsigned    Format( unsigned PageW,unsigned PageL,char ** pBuf,
                      Columns::FrameStyle fs = Columns::Basic,
                      FILE *f = 0 , const char * strftimeFormat = 0);
 unsigned     getSWDiscListf(Boolean fMRDB, SoftMachList &);
 unsigned     getInventorySWList(Boolean fMRDB, FeatList &);
 int          filesVersion(const char * FileName);
 Boolean      checkVersion(int iVersion,Boolean fMRDB);
 Boolean      applyUnresolvedFeature(Feat &, unsigned qty);

 IString      currentPKFile() {return PK.currentPKFile();}
  friend ostream& operator<< (ostream& os, RMSystem & sys)
                {
                  sys.SysDataOut(os) << endl
                                     << sys.machList() << endl
                                     << sys.explanations() << endl
                                     << sys.rpos() << endl;
                  return os;
                }

#ifndef UNITTEST
protected:
#endif

 Boolean RMSystem::re_Set(const char * AASFile,const char * MRDBFile,
                        const char * MRDBDescFile, const char * MsgStringsFile);
 void         setFileNames(const char * AASFile,const char * MRDBFile = 0,
                           const char * MRDescFile = 0,
                           const char * MsgStringsFile = 0);
 Boolean      getPK(Boolean fReadMRDBDescFile = 0,Boolean fForcePKRead = 0);
 Boolean      getPKX(Boolean fReadMDBDescFile = 0 ,Boolean fForcePKRead = 0,
                     char * szCompiledFeatFile = 0);
// Boolean      readCFRFile(Boolean fMRDB);
// Boolean      readCFRFile(XWindow *,Boolean fMRDB);
 Boolean      readCFRFile(XWindow *,Boolean fMRDB,Boolean bAS4 );  //Include bAS4 flag to reconcile iSeries SW.  HECC 72273 06/27/2002

 Boolean      readCFRFileV31(XWindow * pXW,Boolean fMRDB, Boolean bAS4);  /* F73732 EDW 07/25/02 */
 unsigned long fCheckSum(const char * file);
 Boolean      runCheckSum(const char * file, unsigned long * pSum);
 Boolean      runConversion(XWindow * pXW,const char * fileFrom,const char * fileTo,
                            unsigned version);
// void         resolveNoFeatMachines();
 void         resolveLoneMachines();
 Boolean      check4DifferentModelMachines(XWindow * pXW, Boolean b390 );   //53915 MAMT 28/02/2001
 void         resolveMatches();
 void         resolveGenerics();
 void         resolveMRCombos();
 void         resolveNonEquals();

//F72273  Begin:  Include bAS4 flag and iSeries SW features table
//to reconcile iSeries SW.     HECC  07/02/2002
// void         resolveOthers();
 void         resolveOthers( Boolean bAS4, ISortedSet<IString>* pISeriesSWTable );
//F72273  End.

//F72273  New method implemented to reconcile the iSeries SW features
void          resolveSoftwareForISeries( ISortedSet<IString>* pISeriesSWTable );

 int          resolveUsingNoCompareTable( ISortedSet<IString>* pNoCompTable,
                                          ISortedSet<IString>* pQuickFixTable,
                                          ISortedSet<IString>* pFeature54Table );//edw07/06/1998
                                          //53915 - MAMT - 08/31/2000 added Feature54Table
 //49222 - MAMT - 06/27/2000
 unsigned     resolveUsingQuickFixTable( ISortedSet<IString>* pQuickFixTable,
                                         IString strType, IString strModel,
                                         IString strFeat, unsigned uBaseQty );
 unsigned     findPrime(IString PrimeF);
 //49222 - MAMT - 06/27/2000
 Boolean      reBuildState( const char * szFileName);
 Boolean      floatFirstMRDBMachine();
 Boolean      fWriteCFR(const char * szFileName,Boolean fMRDB,
                        Boolean fAppendRPOs, Boolean bSetLockingFlag);
 int          fWriteCFRV30(FILE *f,Boolean fMRDB, Boolean fAppendRPOs);
 int          fWriteCFRV31(FILE *f,Boolean fMRDB, Boolean fAppendRPOs, Boolean bSetLockingFlag);
 Boolean      saveState( const char * szFileName);
 void         applyPK();

//F72273  Begin:  Include bAS4 flag and iSeries SW features table
//to reconcile iSeries SW.     HECC  07/02/2002
// void         generateFeatList();
 void         generateFeatList( Boolean bAS4 );
//F72273 End.

 void         generateFeatList();
 void         generateBacklogList();
 Boolean      usingCompiledPK() {return PK.usingCompiledPK();}
 Boolean      savePK() { return PK.save();}
};


#pragma pack()
#endif

