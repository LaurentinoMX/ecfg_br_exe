// CR23 3-30-95 [AB] Add RM2 signature to CFReport after 53-record
// CR24 3-30-95 [AB] Warn user for old MRDB files
// CR28 4-07-95 [AB] move install date in print from features to machines
// edw  4-28-98 Changed logic to collect all 54 records. Not just the first one
//              for each feature because you can have multiple 54's for a feature.
// edw06/04/1998 Added logic to read and write CFReports with multiple systems.
// edw06/05/1998 Added logic to read and write CFReports with multiple systems.
//               Since all machines, regardless the system it is apart of, have
//               unique type, model, serials:
//                  1. We read all the machines contained in the AAS File
//                     and record the system the machines belong to.
//                  2. If the MRDB file contains stacked V20 reports, resequence
//                     the 54, 55, and 56 instance ids to avoid conflicts.
//                  3. Read all the machines contained in the MRDB file and store
//                     the records with the cooresponding AAS record.
//                  4. If a machine is read in the MRDB file that did not exist
//                     in the AAS file, give it a new system id.  All MRDB records
//                     with no AAS matches will to given this same system id.
//                  5. Reconcile all the machines and features.
//                  6. Write the new CFReport a writing a system at a time along
//                     with all the machines that belong to the system.
// edw06/05/1998 Modify readCFRFileV31 so that it do not process 54, 55, 56 records. We
//               will only process the 54, 55, and 56 records in the MRDB file.
// erg 03/30/2000  D48444 In order to return the entire line and it can be processed.
// hecc 06/27/2002 F72273 Two new methods were added to support the software for
//                        iSeries configurations reconciliation, these are:
//                        a) Process_17_Record.  This method is to read the MRDB SW
//                        features into the machine object.
//                        b) resolveSoftwareForISeries. This is the new method that
//                        contains the complete logic to perform the SW features of
//                        iSeries configurations reconciliation.
// edw  07/25/2002 F73732 Completion of F72273 (iSeries SW reconciliation support:
//                        1) Modify rule #1 in  resolveSoftwareForISeries
//                           per "Release 3.2  Design Change Request 5 - Requirement 1256"
//                           (This is the case when a SW record is reported in
//                           both AAS and MRDB)
//                        2) Temporarily change the tag for SIPO records to 'Hardware'
//                           to allow these type iSeries SW records to be reconciled like
//                           hardware instead of using the rules defined in
//                           resolveSoftwareForISeries (per design change request #5)
//                        3) Modify the logic in the Process_17_Record method to fix
//                           bugs in the logic for reading RT17s from the MRDB report
//                        4) Modify the logic in Process_47_49_RecordV31 to fix bugs in the
//                           logic for reading RT47/49 from the AAS report
// VBB  09/19/2002 D74157 Add code to allow transfer of 19 records in AAS report through
//                        reconciliation.
// edw  09/26/2002 D74158 Fixed a defect in the resolveSoftwareForISeries method which
//                        causes reconciliation to hang when rule #3 is executed
// HECC 10/18/2002 F73686 Added necessary logic to the RPO2 constructor to handle the
//                        two new parameters: Machine description and Product Category.
// HECC 04/12/2002 D74629. In case that the Machine record be skipped, logic must
//                        also omit 'all' records associated with this machine
//                        (including the 'stray' 54 record)
// HECC 07/28/2003 D83402 Added necessary logic to the RPO2 constructor to handle the
//                        four new parameters: Customer number, Flag system number, System type,
//                        and System number.
// HECC 01/13/2004 F86750 Added necessary logic to process the RT 38 for services products.
//
// ZOR  march-2004 F86752 MRPD+ Convergence (Reconciliation mods).
//                        it process the 06 record wich have the reconciled data from Inventory Services
//                        it reads the FL record to check if ALL, SOME or NONE data was reconciled
// ZOR april-2004 D89910  ignore non INV 06 records  in the reconciliation process
// ZOR 19/11/2004 D95997  inlcude non INV 06 records in the Output file

// ELRR 147652   Add logic to let the 05 records pass thru Base retrieval in zseries to the RET file

 /*--------------------------------------------------------------------------*\
 |    Filename - SYSTEM.CPP                                                   |
 |      contains source for the following RM/2 class implementations:         |
 |                                                                            |
 |   RMSystem - containing data and methods associated with the               |
 |            unit to be Reconciled by the application                        |
 \*--------------------------------------------------------------------------*/
/* edw11/17/98
extern "C"
    {
extern unsigned long _System cfrcksum(char*, char*);
    }
#pragma import(cfrcksum,,"OS2CKCFR",0)
*/

        /*---------------------------------------------------*\
        |              Standard Library Includes              |
        \*---------------------------------------------------*/
#include <fstream.h>

#define INCL_DOSPROCESS                          // dossleep
#define INCL_DOS                                 // dosscanenv
#include <os2.h>

        /*---------------------------------------------------*\
        |                  ICLUI's Includes                   |
        \*---------------------------------------------------*/
#include <isynonym.hpp>                          // Definition of Boolean etc
#include <imsgbox.hpp>                           //edwtest

#define INCL_IAPPLICATION
#define INCL_IACCESSERROR
#define INCL_IPROFILE
#include "uicl.hpp"

        /*---------------------------------------------------*\
        |               RM/2 specific Includes                |
        \*---------------------------------------------------*/
#include "euophlp.h"                             // Help Panel Ids
#include "euodef.h"                              // CFReport structures
#include "euomsg.h"                              // Message string Ids
#include "euoutil.hpp"                           // Utilities Tellem
#include "cfrv.hpp"                              // CFRecV CFRecVL
#include "cfr08.hpp"                             // CFR08 CFR25
#include "cfr19_50.hpp"                          // CFR19_50
#include "cfr54.hpp"                             // CFR54
#include "cfr95.hpp"                             // CFR95
#include "system.hpp"                            // this file's declarations
#include "rpodata.hpp"
#include "prf.hpp"
                                                 // XWindow (Application's Main
#include "mainw.hpp"                             // Window)
#include "mainw.h"                               // Reconcilation constants


        /*---------------------------------------------------*\
        |                       Defines                       |
        \*---------------------------------------------------*/
#define MSGFILEEXT      "MSG"
#define READBUF_SIZE 10240

        /*---------------------------------------------------*\
        |                   Static strings                    |
        \*---------------------------------------------------*/
char *pRM2SignatureRecord = "05         10   This CFReport is generated by Base Retrieval";
char *pUSRPRF_PKDIR = "dataDir";
char *pUSRPRF_APP   = "BaseRetrieval";
char *pLastSensed   = "Last Sensed:  ";
IString BiiName;    //ELRR 147652
        /*---------------------------------------------------*\
        |                       Macros                        |
        \*---------------------------------------------------*/
#define REPORT_ERROR(nmbr)                                                \
   return tellem.say(true,EUO_MSG_INTERNAL_ERROR,(char *)IString(nmbr),   \
                     findFileName(__FILE__),(char *)IString(__LINE__));

 /*--------------------------------------------------------------------------*\
 |                                RMSystem Class                                |
 \*--------------------------------------------------------------------------*/

unsigned long RMSystem :: fCheckSum(const char * file)
{
// return cfrcksum((char *)file, "82151");   // OS2CKCFR.DLL blows
 unsigned long Sum = 0;
 runCheckSum(file,&Sum);
 return Sum;
}

        /*-------------------------------------------------------------------*\
        |         Ctor                                                        |
        |        Allocate and init System Data storage                        |
        |        Allocate utility buffers                                     |
        \*-------------------------------------------------------------------*/
RMSystem::RMSystem()
       : pAMachine(0)
        ,pAFeature(0)
{
   pSD = new System_Data;
   memset(pSD,0,sizeof(*pSD));
   pARec = new CFRecV();
   BufK = new char[1024];
   BufF = new char[_MAX_PATH];
                                                 // will be allocated at first
   ReadBuff = 0;                                   // File-read

   // get program path
   strExecPath = EUOPProfile::strExecPath;

   iSysCnt = 0;                                  // edw06/04/1998

   // edw-delete dataDir key value if different from the install directory
   if ( IProfile::userProfile().containsKeyName(pUSRPRF_PKDIR,pUSRPRF_APP) )
      {
      IString strDataDir=IProfile::userProfile().elementWithKey(pUSRPRF_PKDIR,pUSRPRF_APP);
      if (strDataDir != strExecPath )
         IProfile::userProfile().deleteElementWithKey(pUSRPRF_PKDIR,pUSRPRF_APP);
      }
}

        /*-------------------------------------------------------------------*\
        |         Dtor                                                        |
        |        Delete Ctor-allocated storage                                |
        \*-------------------------------------------------------------------*/
RMSystem::~RMSystem()
{
 delete pSD;
 delete pARec;
 delete[] BufK;
 delete[] BufF;
 if (ReadBuff)
   delete[] ReadBuff;
 release();
}

    /*-----------------------------------------------------------------------*\
    |   void release()                                                        |
    |       Release the storage associated with the current System.           |
    \*-----------------------------------------------------------------------*/
void RMSystem::release()
{
 for (int i = 0, I = (sizeof(CFRLists)/sizeof(CFRLists[0])) ; i < I ; i++ )
   CFRLists[i].Release();                        // System-specific CFR Records
 for (i=0;i<2;i++) {
   L05[i].Release();                             // Comment Array
   L10[i].Release();                             // SubSystem Array
   L52[i].Release();                             // Exception Array
   }
 ML.Release(true);                               // Machine Array
 L55.Release();                                  // Connection Array
 L56.Release();                                  // Placement Array
                                                 // System-specific Feature
 MDBFeatDescL.release();                         // Description Array
 RPOL.removeAll();                               // RPO Collection
 FL.removeAll();                                 // Feature Collection
 L19_50.removeAll();                             // Backlog Collection
                                                 // Software Discrepancy
 SWDiscList.removeAll();                         // Collection
                                                 // Resolution Explanation
 XPL.removeAll();                                // Collection
}

    /*-----------------------------------------------------------------------*\
    |    Boolean re_Set()                                                     |
    |       Prepare for the resolution of a new system, as follows:           |
    |          Null-out the System Data structure and copy the                |
    |            given file names: AAS, MRDB, Description File                |
    |            (the System's *.DSC file) and Message Strings File names.    |
    |            (This structure is maintained by system and accompanies      |
    |             the object when saved to file)                              |
    |          Release storage associated with the previous system.           |
    |          Use the AAS filename with an 'MSG' extension as the            |
    |            new system's Message File where discrepancies and/or         |
    |            assumptions are logged. Hand over the Message String File    |
    |            and the Message File to the Tellem object, since it          |
    |            handles the retrieval, formatting and writing of             |
    |            messages.                                                    |
    \*-----------------------------------------------------------------------*/
Boolean RMSystem::re_Set(const char * AASFile,const char * MRDBFile,
                       const char * MRDescFile, const char * MsgStringsFile)
{
 memset(pSD,0,sizeof(*pSD));
 setFileNames(AASFile,MRDBFile,MRDescFile,MsgStringsFile);
 release();                                      // release storage
 strcpy(BufF,AASFile);
 return tellem.set(MsgStringsFile,SetFileExtension(BufF,MSGFILEEXT)); //jt
// return tellem.set(MsgStringsFile,MakeFileName(BufF,AASFile,"MSG"));
}

void RMSystem::setFileNames(const char * AASFile,const char * MRDBFile,
                          const char * MRDescFile, const char * MsgStringsFile)
{
 if (AASFile)
   mystrncpy(pSD->InFiles[0],AASFile,sizeof(pSD->InFiles[0])-1);
 if (MRDBFile)
   mystrncpy(pSD->InFiles[1],MRDBFile,sizeof(pSD->InFiles[1])-1);
 if (MRDescFile)
   mystrncpy(pSD->InFiles[2],MRDescFile,sizeof(pSD->InFiles[2])-1);
 if (MsgStringsFile)
   mystrncpy(pSD->MsgFile,MsgStringsFile,sizeof(pSD->MsgFile)-1);
}

    /*-----------------------------------------------------------------------*\
    |   const char * RMSystem::message ()                                       |
    |       Return pointer to a formatted string using the                    |
    |       formatting string from the Message String File with               |
    |       the given Id and substituting its '%s' placeholders               |
    |       with the given null-terminated strings.                           |
    \*-----------------------------------------------------------------------*/
const char * RMSystem::message (  unsigned long Id
                               ,const char*   sz1
                               ,const char*   sz2
                               ,const char*   sz3
                               ,const char*   sz4
                               ,const char*   sz5
                               ,const char*   sz6
                               ,const char*   sz7
                               ,const char*   sz8
                               ,const char*   sz9)
{ return tellem.message(Id,sz1,sz2,sz3,sz4,sz5,sz6,sz7,sz8,sz9);}

    /*-----------------------------------------------------------------------*\
    |   IString newSerialNo()                                                 |
    |       Return a new Machine Serial Number for the current system         |
    |        (this is needed when, during resolution, generic features        |
    |        are resolved by the user with machines), as follows:             |
    |                                                                         |
    |        Increment and retrieve the latest serial number for this         |
    |          system, stored in the System Data Structure which is           |
    |          maintained for every system.                                   |
    |        Prefix the number's string version with "0000ID" and return.     |
    |        Note: the number will wrap after 999.                            |
    \*-----------------------------------------------------------------------*/
IString RMSystem::newSerialNo()
{
 char  Buf[4];
 unsigned u = ++(pSD->uSerNoSeq)%1000;
 Buf[0]  = '0' + (char)(u/100);
 Buf[1] = '0' +  (char)((u%100)/10);
 Buf[2] = '0' + (char)(u%10);
 Buf[3] = '\0';
 return IString(IString("0000ID") + IString(Buf));
}

    /*-----------------------------------------------------------------------*\
    |   Boolean getPK(Boolean fReadMDBDescFile, Boolean fForcePKRead)         |
    |       Establish the Product Knowledge for the current system            |
    |       and indicate success, as follows:                                 |
    |                                                                         |
    |         If the System Specific Description File is to be read, use      |
    |         the file name retained in the System Data structure in          |
    |           the 're_Set()' method.                                        |
    |         Hand the Description array's address over to the Product        |
    |           Knowledge.( the list is contained in this object, but         |
    |           PK uses it ).                                                 |
    |         If we haven't read the General Product Knowledge or if we're    |
    |           forced to do so:                                              |
    |            Retrieve from the OS2.INI file, the directory where          |
    |              the required Product Knowledge files                       |
    |               (FEATURE.TXT and GENERIC.TXT -OR- their compiled          |
    |                version FEATURE.$$F) are to be found.                    |
    |              The entry in this profile will have been made by the       |
    |              install program. If not, create the application's          |
    |              key entry at this time in the 'catch' block, using the     |
    |              current directory.                                         |
    |            Create the filenames and have 'PK' build itself from them.   |
    \*-----------------------------------------------------------------------*/
Boolean RMSystem::getPK(Boolean fReadMDBDescFile, Boolean fForcePKRead)
{
 if (fReadMDBDescFile && !MDBFeatDescL.readFile(pSD->InFiles[2]))
   return tellem.say(true,EUO_MSG_NO_FILE,findFileName(pSD->InFiles[2]));
 PK.setMRDescListPtr(&MDBFeatDescL);
 if ( fForcePKRead || !PK.pkfl().NumberElements()) {
   IString strPKDir;
   try {
     strPKDir=IProfile::userProfile().elementWithKey(pUSRPRF_PKDIR,pUSRPRF_APP);
     }
   catch (const IAccessError & exc) {
     //strPKDir = IApplication::current().argv(0);//jt
     //strPKDir = strPKDir.remove(strPKDir.lastIndexOf('\\'));//jt
     strPKDir =  strExecPath;
     IProfile::userProfile().addOrReplaceElementWithKey(pUSRPRF_PKDIR,strPKDir,
                                                        pUSRPRF_APP);
     }
   if (!PK.build(&tellem,(char *)MakeFileSpecStr((char *)strPKDir,"FEATURE","TXT"),
                 (char *)MakeFileSpecStr((char *)strPKDir,"GENERIC","TXT")))
     return tellem.say(true,EUO_MSG_NO_FILE,"FEATURE.TXT' and 'GENERIC.TXT");
   }
 return true;
}

Boolean RMSystem :: getPKX(Boolean fReadMDBDescFile,Boolean fForcePKRead,
                         char * szCompiledFeatFile)
{
 if (fReadMDBDescFile && !MDBFeatDescL.readFile(pSD->InFiles[2]))
   return tellem.say(true,EUO_MSG_NO_FILE,findFileName(pSD->InFiles[2]));
 PK.setMRDescListPtr(&MDBFeatDescL);
 if ( fForcePKRead || !PK.pkfl().NumberElements()) {
   IString strPKDir;
   try {
     strPKDir=IProfile::userProfile().elementWithKey(pUSRPRF_PKDIR,pUSRPRF_APP);
     }
   catch (const IAccessError & exc) {
     //strPKDir = IApplication::current().argv(0);
     //strPKDir = strPKDir.remove(strPKDir.lastIndexOf('\\'));
     strPKDir =  strExecPath;
     IProfile::userProfile().addOrReplaceElementWithKey(pUSRPRF_PKDIR,strPKDir,
                                                        pUSRPRF_APP);
     }
   if (!PK.buildX(&tellem,szCompiledFeatFile,
                 (char *)MakeFileSpecStr((char *)strPKDir,"FEATURE","TXT"),
                 (char *)MakeFileSpecStr((char *)strPKDir,"GENERIC","TXT")))
     return tellem.say(true,EUO_MSG_NO_FILE,"FEATURE.TXT' and 'GENERIC.TXT");
   }
 return true;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean  addCFR(char * Buf,Boolean fMRDB)                             |
    |       add the given buffer as an AAS or MRDB CFR Record after           |
    |       making a copy of it.                                              |
    |       These are records we're not or no longer interested in,           |
    |       other than writing them out to the new CFReport                   |
    |       (such as the comment records or those already processed)          |
    \*-----------------------------------------------------------------------*/
Boolean  RMSystem :: addCFR(char * Buf,Boolean fMRDB)
{
 CFRecV * pRec = new CFRecV(Buf);;
 if (pRec)
   CFRLists[fMRDB].AddasLast(pRec);
 return (pRec != 0);
}

    /*-----------------------------------------------------------------------*\
    |   Boolean addCFR(CFRecV & rec,Boolean fMRDB)                            |
    |       add the given CFR Record as an AAS or MRDB CFR Record after       |
    |       making a copy of it.                                              |
    |       These are records we're not or no longer interested in,           |
    |       other than writing them out to the new CFReport                   |
    |       (such as the comment records or those already processed)          |
    \*-----------------------------------------------------------------------*/
Boolean  RMSystem :: addCFR(CFRecV & rec,Boolean fMRDB)
{
 CFRecV * pRec = new CFRecV(rec);;
 if (pRec)
   CFRLists[fMRDB].AddasLast(pRec);
 return (pRec != 0);
}

    /*-----------------------------------------------------------------------*\
    |   Boolean editVersion(int iVersion,Boolean fMRDB)                       |
    |       Currently we handle only AAS' version 30 and MRDB's               |
    |              version 20.                                                |
    \*-----------------------------------------------------------------------*/
Boolean RMSystem::checkVersion(int iVersion,Boolean fMRDB)
{
// if ( (fMRDB && (iVersion != 20)) || (!fMRDB && (iVersion != 30)) )
//   return tellem.say(true,EUO_MSG_WRONG_VERSION,
//                     findFileName(pSD->InFiles[fMRDB]), fMRDB? "20":"30");
 switch(iVersion) {
   case 20:
   case 30:
   case 31: return true;
   default: return tellem.say(true,EUO_MSG_WRONG_VERSION,
                            findFileName(pSD->InFiles[fMRDB]), "20, 30 or 31");
   }
}

    /*-----------------------------------------------------------------------*\
    |   Boolean Process_00_Record(CFR_00 * p00,Boolean fMRDB)                 |
    |       Process the given AAS or MRDB 00-Record:                          |
    |                                                                         |
    |         Retain some data in the System Data structure:                  |
    |            Establish, edit and, if OK, retain its version:              |
    |            The file date both as an IDate and string:                   |
    |              Had some problems with version 20 file since some came     |
    |               with YYMMDD data while others came with format MMDDYY.    |
    |              We handle both cases. (It's not a 54-Record, but           |
    |              'CFR54' has the proper static methods).                    |
    |            The name from its 'System' field.                            |
    |            The title from its 'Title' field.                            |
    |            The User Id, if the record is long enough to contain it.     |
    |         Copy as much as will fit from the record's System field to its  |
    |            aidName field.                                               |
    |         Add the record.                                                 |
    \*-----------------------------------------------------------------------*/
Boolean RMSystem::Process_00_Record(char * Buf,Boolean fMRDB)
{
 int version = CFRecType(((CFR_00 *)Buf)->Version);
 if (!checkVersion(version,fMRDB))
   return false;
 pSD->iVersion[fMRDB] = version;
 switch(version) {
   case 20:
   case 30: {
     CFR_00 * p00 = (CFR_00 *)Buf;
     mystrstrip( mystrncpy(p00->Title, " " ,1) );// edw temp fix for title problem occurring if no title is in input buffer
                                                 // handle both YYMMDD and
     if ( version == 20) {                       // MMDDYY cases.
       if (!CFR54::cfrDateYYMMDD(p00->Date,pSD->FileDate[fMRDB]))
         CFR54::cfrDate(p00->Date,pSD->FileDate[fMRDB]);
       }
     else CFR54::cfrDate(p00->Date,pSD->FileDate[fMRDB]);
     mystrstrip(mystrncpy(pSD->Name[fMRDB],p00->System,sizeof(p00->System)));
     mystrstrip(mystrncpy(pSD->Title[fMRDB],p00->Title,sizeof(p00->Title)));
     if (strlen((char *)p00) > (p00->UserId - (char *)p00))
       mystrstrip(mystrncpy(pSD->UserId[fMRDB],p00->UserId,sizeof(p00->UserId)));
     memcpy(p00->aidName,p00->System,
            ALMIN(sizeof(p00->System),sizeof(p00->aidName)));
     break;
     }
   case 31: {
     CFR_00V31 * p00V31 = (CFR_00V31 *)Buf;
     cfrDateYYYYMMDD(p00V31->Date,pSD->FileDate[fMRDB]);
     mystrstrip(mystrncpy(pSD->Title[fMRDB],p00V31->Description,
                          sizeof(p00V31->Description)));
     break;
     }
   default: return tellem.say(true,EUO_MSG_WRONG_VERSION,
                                findFileName(pSD->InFiles[fMRDB]),
                                (char *)IString(version));
   }
 addCFR(Buf,fMRDB);
 return true;
}

    /*-----------------------------------------------------------------------*\
    |   void Process_01_Record(char * Buf,Boolean fMRDB)                      |
    |       Add the record the CFR records.                                   |
    \*-----------------------------------------------------------------------*/
void RMSystem::Process_01_Record(char * Buf,Boolean fMRDB)
{
 //edw addCFR(Buf,fMRDB);                            //edw06/04/1998
}

    /*-----------------------------------------------------------------------*\
    |   void Process_03_Record(char * Buf,Boolean fMRDB)                      |
    |       Process the given buffer as an AAS or MRDB 03-Record:             |
    |         Retain the numerical value of the third byte as                 |
    |          the System's security classification and add the record.       |
    \*-----------------------------------------------------------------------*/
void RMSystem::Process_03_Record(char * Buf,Boolean fMRDB)
{
 pSD->iSecurity[fMRDB] = Buf[2] - '0';
 addCFR(Buf,fMRDB);
}
 /*-------------------------------------------------------------------------------*\
 |   void Process_06_RecordV31(char * Buf,Boolean fMRDB, FILE * f)                 |
 |       Process the given buffer to check the kind of 06 record:                  |
 |     a.	If noe(NOME) of the records have been reconciled, then process the file  |
 |       as they would be processed today i.e. reconcile the invret.cfr and        |
 |        invret.mrd files and create RPOs, the .BIR, and .RET files as usual.     |
 |     b.	If al(ALL)  of the records have been reconciled, then Base Retrieval wi  |
 |       read the other 06 IS records to determine the NMRD, MRD, and final        |
 |       quantities and place them in a collection of reconciled items.            |
 |     c.	If soe(MIXED) of the records have been reconciled, then the other        |
 |       06 IS records will be processed to create a list of reconciled systems    |
 |       and their features' AAS/NMRD, MRD, and final quantities.                  |
 |       The Reconciliation process will begin as usual, but each of the systems   |
 |       will be checked against the list of reconciled systems.                   |
 |          i.	If a system has already been reconciled, then place it and it       |
 |            s features in the Matches window, designating the NMRD, MRD, and     |
 |            final quantities.                                                    |
 |          ii.	If the system has not already been reconciled, then process it     |
 |             as usual, showing the features in the Mismatches and Matches        |
 |             containers.                                                         |
 \*-------------------------------------------------------------------------------*/

int RMSystem::Process_06_RecordV31(char * Buffer, Boolean fMRDB, FILE * f)   //ZOR F86752 req 1373
{
  int rc = RC_OK;
 char * NextLine;
 fpos_t pos;
 //Machine * pThisMachine = 0;
 Boolean fDone = false;
 IString strRecord = Buffer;
// pAFeature = 0;
 IString strISflag = strRecord.subString(13,2);
 IString strIStype = strRecord.subString(15,5).strip();
 int i,I,j,J,ireclong;
 int k,K, y=0;
 IString AASQty, MRDQty, BaseQty;
 int idMsg;
 IString pMsg;

// if the .CFR file has the label NONE it will jump this process, and will resolve data as normal
 if (strISflag=="FL" && strIStype!="NONE")
  {
// if the .CFR has MIXED or ALL it will change the tag of the machines and features
// to Process_IS and Resolved_IS, and will modify the quantities with the data from 06 records
  if( strIStype=="MIXED" || strIStype == "ALL" )
     {
     while(!fDone && !fgetpos(f,&pos) && (NextLine = readCFRLine(f,BufK,1024)))
       {
// after record 06FL it will read the next record MS to update the quantities of the features
       y=54;
       strRecord = NextLine;
       ireclong=strRecord.length();
       if(strRecord.subString(10,3)=="INV" && strRecord.subString(13,2) == "MS" )    //89910
          {
           char* szType=strRecord.subString(15,4);
           char* szModel=strRecord.subString(19,3);
           char* szSerial=strRecord.subString(22,9);


          if (pAMachine = ML.FindProcessMachine(szType,szModel,szSerial))
            {
//if the machine in the 06 record is in the list of machines that were loaded from records 25
//it will add a message in the explain window and change tag of the machine
               idMsg = EUO_MSG_RESOLVED_IS;
               IString strDesc = IString::stripBlanks(IString(pAMachine->description()));
               tellem.say(false,idMsg, pAMachine->type(),pAMachine->model(),
                          pAMachine->serialNo(), pAMachine->description());
               pMsg = IString( tellem.message(idMsg,0,strDesc,0) );

               if (pMsg.size())
                  {
                  XPL.add(XPLN2(pAMachine->type(),pAMachine->model(),
                                pAMachine->serialNo(),0,
                                (const char *)(pMsg)));
                  pAMachine->setMsgInd(true);
                  pMsg = "";
                  }

             pAMachine->setTag(Machine::Process_IS);
             for (k = 0, K = pAMachine->mfeatList().NumberElements(); k < K ; k++)
               if (pAFeature = pAMachine->mfeatList().element(k))
                {
                 if(y<ireclong)
                 {
// for each feature in the machine it will change the quantities from 06 record
                  AASQty= strRecord.subString(y,5);
                  MRDQty= strRecord.subString(y+5,5);
                  BaseQty= strRecord.subString(y+10,5);
                  pAFeature->setBase(BaseQty.asInt());
                  pAFeature->setQty(MFeat::AASType,AASQty.asInt());
                  pAFeature->setQty(MFeat::MRDBType,MRDQty.asInt());
                  pAFeature->setTag(MFeat::Resolved_IS);
                  y=y+22;
                 }
                 }
             }

          }
       }

     }

 }
 return rc;

}
void RMSystem::Process_07_Record(char * Buf,Boolean fMRDB)
{
 IString strVPDIds = STR_VPDTranIds;                 //edw06/04/1998
 IString strRecord;                                  //edw06/04/1998

 mystrncpy(pSD->Name[fMRDB],(char *)CFR07::description(Buf),sizeof(pSD->Name[0]));
// RefNotesNo[fMRDB] = CFR07::referenceNotesNumber(Buf);
// L05[fMRDB].addSignatureRec(RefNotesNo[fMRDB]);
//edw addCFR(Buf,fMRDB);                                //edw06/04/1998
//edw addCFR(pRM2SignatureRecord,fMRDB);                //edw06/04/1998
 iSysCnt++;                                             //edw06/04/1998
 strSysId = strVPDIds.word( iSysCnt );                  //edw06/04/1998
 strRecord=Buf;                                         //edw06/04/1998
 if ( strRecord.subString( 1, 2 ) != STR_07RecType )    //edw06/04/1998
    {                                                   //edw06/04/1998
    strRecord = STR_07RecType;                          //edw06/04/1998
    strRecord.overlayWith( strSysId + STR_NoMatches, 6);//edw06/04/1998
    }                                                   //edw06/04/1998
 else                                                   //edw06/04/1998
    strRecord.overlayWith( strSysId, 6);                //edw06/04/1998
 sSystemRecordSet.add( strRecord );                     //edw06/04/1998
}

    /*-----------------------------------------------------------------------*\
    |   Boolean editFirstRecord_54(const char * Buf,Boolean fMRDB)            |
    |       Edit the given buffer as an AAS or MRDB 'first 54 Record'         |
    |         (the first 54 record following the machine entry)               |
    |         which for Version 30 has to have a blank feature field.         |
    \*-----------------------------------------------------------------------*/
Boolean RMSystem::editFirstRecord_54(const char * Buf,Boolean fMRDB)
{
 if ((version(fMRDB) == 30) && !CFR54::blankFeature(Buf))
   return tellem.say(false,EUO_MSG_NON_BLANK_1ST_54,
                     findFileName(pSD->InFiles[fMRDB]),Buf);
 return true;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean CollectSWDiscrepancies(FILE *f,const Record_17 * p17)         |
    |                                                                         |
    |     If this (MRDB) record represents a Software descripancy             |
    |       ( Check the Machine List for AAS Software entries with            |
    |         identical Type and Model (matches()):                           |
    |       If the next record is of type 54:                                 |
    |         Get the address of its non-null-terminated feature and          |
    |         install date.                                                   |
    |       Otherwise, put the record back, use an empty feature string       |
    |         and a non-date (there no way to indicate the absence of         |
    |         a date in the IDate class - we're using our own)                |
    |       Use the feature and date to add a new element to the              |
    |         Software Discrepancy List.                                      |
    |                                                                         |
    |     Indicate whether the Discrepancy List was added to.                 |
    \*-----------------------------------------------------------------------*/
Boolean RMSystem::collectSWDiscrepancies(FILE *f,const Record_17 * p17)
{
 Boolean fFound = false;
 for (int n=0,N=ML.NumberElements(); !fFound && (n < N) ; n++)
   fFound = ML.element(n)->isSoftwareMatch(p17);
 if (!fFound) {
   const char * feat = "";
   IDate date(SoftMach::nonDate());
   fpos_t pos;
   if (!fgetpos(f,&pos)) {
     char * NextLine = readCFRLine(f,BufK,1024);
     if (NextLine && (CFRecType(NextLine) == 54)) {
       feat = CFR54::feature(NextLine);
       CFR54::installDate(NextLine,date);
       }
     else fsetpos(f,&pos);
     }
   SWDiscList.add(SoftMach(p17->Type,p17->Model,feat,0,date));
   }
 return fFound;
}

Boolean RMSystem::collectSWDiscrepancies(FILE *f,const Record_SW_V31 * pSW)
{
 Boolean fFound = false;
 for (int n=0,N=ML.NumberElements(); !fFound && (n < N) ; n++)
   fFound = ML.element(n)->isSoftwareMatch(pSW);
 if (!fFound) {
   IString strFeat;
   IDate date(SoftMach::nonDate());
   fpos_t pos;
   Boolean fDone = false;
   char * NextLine;
   while(!fDone && !fgetpos(f,&pos) && (NextLine = readCFRLine(f,BufK,1024)))
     switch (CFRecType(NextLine)) {
       case 5: break;
       case 95:
         CFR95::installDate((const Record_95 *)NextLine,date);
         break;
       case 96:
         strFeat = CFR96::strFeature((const Record_96 *)NextLine);
         fDone = true;
         break;
       default:
         fsetpos(f,&pos);                        // put record back
         fDone = true;
       }
// if (strFeat.length())
     SWDiscList.add(SoftMach(pSW->Type,pSW->Model,(const char *)strFeat,0,date));
   }
 return fFound;
}

    /*-----------------------------------------------------------------------*\
    |   int Process_19_50_Record(char * Buffer,Boolean fMRDB, FILE * f)       |
    |                                                                         |
    |     Process the Hardware (19) and Software (50) Backlog Records:        |
    |                                                                         |
    |     Determine the record type, machine type and model.                  |
    |     Read the subsequent 54-Records in the file:                         |
    |       The first 54-Record:                                              |
    |          Create a new machine with the 54's serial#, add it to          |
    |            the machine list and add the given record as the             |
    |            machine's Machine Record.                                    |
    |          For a version-30 file, add the 54-Record as the machine's      |
    |            First 54 Record.                                             |
    |       If the 54-Record represents a feature not already contained       |
    |         in the machine's list of features, create a new-one             |
    |         and add it.                                                     |
    |       Add the 54-Record to the feature's CFR Record List.               |
    |     If another record is read, put it back ensuring at least one        |
    |       54-Record was read and, if not, a message is logged and a         |
    |       'SKIP' return code is issued which will cause this machine        |
    |       to be skipped.                                                    |
    |     Reaching EOF, implies that the required 98 closure Record           |
    |       was missing and the return code will signal for                   |
    |       the termination of the resolution process.                        |
    \*-----------------------------------------------------------------------*/
int RMSystem::Process_19_50_Record(char * Buffer,Boolean fMRDB, FILE * f)
{
 int rc = RC_OK, fileVersion = version(fMRDB);
 char * NextLine;
 Record_54 * p54;
 Boolean fDone = false;
 Machine * pM;
 IString strModel;
 const char * pType;
 Machine::TagType tag;
 fpos_t pos;
 Boolean fFirst54 = true;

 if (CFRecType(Buffer) == 19) {
   tag = Machine::Backlog_HW;
   pType = CFR19::machineType(Buffer);
   CFR19::model(Buffer,strModel);
   }
 else  {
   tag = Machine::Backlog_SW;
   pType = CFR50::machineType(Buffer);
   CFR50::model(Buffer,strModel);
   }
 while(!fDone && !fgetpos(f,&pos) && (NextLine = readCFRLine(f,BufK,1024)))
    switch (CFRecType(NextLine)) {
      case 54: {
        p54 = (Record_54 *)NextLine;
        //if (p54->InstType == 'S') break; //jt2/18/98
        if (fFirst54) {
          fFirst54 = false;
          pM = new Machine(pType,(char *)strModel,CFR54::serialNo(NextLine));
          pM->setTag(tag);
          ML.Add(pM);
          pM->setMachineRec(fMRDB,Buffer,fileVersion);
          if (fileVersion == 30) {
            pM->addInfoRec(fMRDB,NextLine);
            break;
            }
          }
        MFeat * pF = new MFeat(*p54);
        if (!(pAFeature = pAMachine->mfeatList().Find(*pF)))
          pAMachine->mfeatList().Add(pAFeature = pF);
        else delete pF;
        pAFeature->addCFR(fMRDB? MFeat::MRDBType : MFeat::AASType,*p54);
        break;
        }
      default:
        if (fFirst54) {
          rc = RC_SKIP;
          tellem.say(false,EUO_MSG_NO_54,
                     findFileName(pSD->InFiles[fMRDB]),Buffer);
          }
        fsetpos(f,&pos);                         // put record back
        fDone = true;
      }
 if (!fDone) {
   rc = RC_ABORT;
   tellem.say(true,EUO_MSG_NO_98,findFileName(pSD->InFiles[fMRDB]));
   }
 return rc;
}

int RMSystem::Process_19_50_RecordV31(char * Buffer,Boolean fMRDB, FILE * f, IString strSysId)  //VBB09192002 D74157
{
 int rc = RC_OK;
 char * NextLine;
 Boolean fDone = false;
 Machine * pThisMachine = 0;
 IString strModel,strType;
 Machine::TagType tag;
 fpos_t pos;
 if (CFRecType(Buffer) == 19) {
   tag = Machine::Backlog_HW;
   strType = CFR19V31::type(Buffer);
   strModel = CFR19V31::model(Buffer);
   }
 else  {
   tag = Machine::Backlog_SW;
   strType = CFR50V31::type(Buffer);
   strModel = CFR50V31::model(Buffer);
   }
 while(!fDone && !fgetpos(f,&pos) && (NextLine = readCFRLine(f,BufK,1024)))
   switch (CFRecType(NextLine)) {
     case 5: L05[fMRDB].add(NextLine); break;
     case 54:
       if (pThisMachine)
         pThisMachine->add54Rec(fMRDB,NextLine);
       else {
         rc = RC_SKIP;
         tellem.say(false,EUO_MSG_NO_95,
                    findFileName(pSD->InFiles[fMRDB]),Buffer);
         }
       break;
     case 95:
       if (!pThisMachine) {
         pThisMachine = new Machine((char *)strType,(char *)strModel,
                                    CFR95::serialNo(NextLine));
         pThisMachine->setTag(tag);
         pThisMachine->setSystemId(strSysId);       //VBB09192002 D74157
         ML.Add(pThisMachine);
         pThisMachine->setMachineRec(fMRDB,Buffer,31);
         }
       pThisMachine->addInfoRec(fMRDB,NextLine);
       break;
     case 96:
       if (pThisMachine) {
         MFeat * pF = new MFeat((Record_96 *)NextLine);
         if (!(pAFeature = pThisMachine->mfeatList().Find(*pF)))
           pThisMachine->mfeatList().Add(pAFeature = pF);
         else delete pF;
         pAFeature->addCFR(fMRDB? MFeat::MRDBType : MFeat::AASType,NextLine);
         }
       else {
         rc = RC_SKIP;
         tellem.say(false,EUO_MSG_NO_95,
                    findFileName(pSD->InFiles[fMRDB]),Buffer);
         }
       break;
     default:
       fsetpos(f,&pos);                          // put record back
       fDone = true;
     }
 if (!fDone) {
   rc = RC_ABORT;
   tellem.say(true,EUO_MSG_NO_98,findFileName(pSD->InFiles[fMRDB]));
   }
 return rc;
}

    /*-----------------------------------------------------------------------*\
    |    int Process_25_Record(char * Buffer,Boolean fMRDB, FILE * f)         |
    |                                                                         |
    |     Process the AAS or MRDB Machine Records (08 & 25):                  |
    |                                                                         |
    |     Read the subsequent Records in the file and process per type:       |
    |       54:                                                               |
    |          The first 54-Record:                                           |
    |             Edit the record and, if OK, ensure it's not a duplicate     |
    |               which occurs when we already have a machine from the      |
    |               same AAS or MRDB file that is tagged for processing       |
    |               (as opposed to AAS_Passthru machines - 08 Records in      |
    |               the AAS File)                                             |
    |               Skip the machine if the edit fails.                       |
    |             If the Machine List does not contain this machine,          |
    |               create a new machine and add it.                          |
    |             Add the given record as the machine's Machine Record.       |
    |             If the record is an AAS type 08, tag the machine as         |
    |               an 'AAS_Passthru'.                                        |
    |             If the file has version 30 or if it's version 20 and        |
    |               the 54's feature field is blank:                          |
    |                  Add the 54-Record as the machine's First 54 Record.    |
    |                  Set the machine's serial # from the prefered MRDB      |
    |                    record (the Serial Number's first 5 characters       |
    |                    are disregarded in its compare, but differences      |
    |                    in these bytes have occurred).                       |
    |             Otherwise, create a copy of the 54-Record, blank out its    |
    |               feature and add it as the machine's First 54 Record.      |
    |                                                                         |
    |          If the 54-Record represents a feature not already contained    |
    |             in the machine's list of features, create a new-one         |
    |             and add it.                                                 |
    |          Add the 54-Record to the feature's CFR Record List.            |
    |          Retain the record's Customer Number in the System Data         |
    |             structure if not already done so.                           |
    |                                                                         |
    |       5: Add these comment records to a local list.                     |
    |                                                                         |
    |       Other:                                                            |
    |         Put the record back ensuring at least one                       |
    |           54-Record was read and, if not, a message is logged and a     |
    |           'SKIP' return code is issued which will cause this machine    |
    |           to be skipped.                                                |
    |         If we read in any comment records, add the accumulated          |
    |           list to that of the machine.                                  |
    |                                                                         |
    |     Reaching EOF, implies that the required 98 closure Record           |
    |       was missing and the return code will signal for                   |
    |       the termination of the resolution process.                        |
    \*-----------------------------------------------------------------------*/
int RMSystem::Process_25_Record(char * Buffer,Boolean fMRDB, FILE * f)
{
 int rc = RC_OK, fileVersion = version(fMRDB);
 char * NextLine;
 Record_54 * p54;
 fpos_t pos;
 CFRecVL passThruList;
 Boolean fDone = false,
         fFirst54 = true;
 // F86752  when the MRD file is read, it checks if the system exist and is Resolved_IS then don't process it
 Boolean ISflag = false;  // F86752 req 1373
 pAFeature = 0;
 while(!fDone && !fgetpos(f,&pos) && (NextLine = readCFRLine(f,BufK,1024)))
   switch (CFRecType(NextLine)) {
     case 54: {
       p54 = (Record_54 *)NextLine;
       //if (p54->InstType == 'S') /*rc = RC_SKIP;*/ break; //jt2/18/98
       if (fFirst54) {
         fFirst54 = false;
         str54 = BufK;                        //begin 57519 - MAMT - 12/29/2000
         str54.strip();
         strSerial = str54.subString(28, 9);  //end   57519 - MAMT - 12/29/2000
         char* szType=str54.subString(10,4);    // F86752 req 1373
         char* szModel=str54.subString(18,3);   //F86752 req 1373
         char* szSerial=str54.subString(28,9).strip();   //F86752 req 1373

         if (CFR08::edit(Buffer,fileVersion,tellem,
                         findFileName(pSD->InFiles[fMRDB])) &&
             editFirstRecord_54(NextLine,fMRDB)) {
           Machine * pM=new Machine((Record_08 *)Buffer,
                                    CFR54::serialNo(NextLine));
        //    if (!(pAMachine = ML.FindProcessMachine(*pM)))                      //f86752 req 1373
        // it will find the machine in to know if it was Resolved by Inventory services,
        // it it does then it wont load this machine.
           if (!(pAMachine = ML.FindProcessMachine(szType,szModel,szSerial)))   //F86752 req 1373
             ML.Add(pAMachine = pM);
           else {
             if (pAMachine->tag()==Machine::Process_IS  )   // F86752 1373
                ISflag=true;                                //F86752
             delete pM;
             if (pAMachine->hasData(fMRDB)) {
               rc = RC_SKIP;
               tellem.say(false,EUO_MSG_DUPE_MACH,
                          findFileName(pSD->InFiles[fMRDB]),NextLine);
               }
             }
          if(!ISflag)     //F86752
           {
           if (rc == RC_OK) {
             pAMachine->setIPProdInd(CFR08::prodInd(Buffer));
             pAMachine->setMachineRec(fMRDB,Buffer,fileVersion);
             if (!fMRDB && (CFRecType(Buffer) == 8))
               pAMachine->setTag(Machine::AAS_Passthru);
             if ((fileVersion == 30)  ||
                 ( (fileVersion == 20) && CFR54::blankFeature(NextLine))) {
               pAMachine->addInfoRec(fMRDB,NextLine);
               if (fMRDB)                        // we prefer MRDB's serial no
                 pAMachine->setSerialNo((char *)CFR54::serialNo(NextLine));
               break;
               }
             else if (fMRDB) {                   // create a first_54
                    CFRecV Temp(NextLine);
                    CFR54::setFeature((Record_54 *)Temp.record()," ");
                    pAMachine->addInfoRec(fMRDB,Temp.record());
                    }
             }
            } //F86752 req 1373

           }
         else rc = RC_SKIP;  //dup mach in same file
         }

       //* D74629. In case that the Machine record be skipped, logic must
       //* also omit 'all' records associated with this machine (including
       //* the 'stray' 54 record)
       //* Begin New logic.   HECC: 12/04/2002
      if(!ISflag)  //F86752
        {
       if (rc == RC_SKIP)
          break;
       //* End New logic.   HECC: 12/04/2002


       //edw05/15/98 - if not first 54 record read
       if (p54->InstType == 'S')  ///*rc = RC_SKIP;*/ break; //jt2/18/98
          {
            MFeat * pF = new MFeat(*p54);
            pAMachine->MachSecFeatList.AddasLast(pAFeature = pF);
            pAFeature->addCFR(fMRDB? MFeat::MRDBType : MFeat::AASType,*p54);
            break;
          }
       if (rc == RC_OK) {
         MFeat * pF = new MFeat(*p54);
         if (!(pAFeature = pAMachine->mfeatList().Find(*pF)))
           pAMachine->mfeatList().Add(pAFeature = pF);
         //else delete pF;   //edw 4/28/98 - replace with else below
         else
            {
               pAMachine->DuplicateRec54List.AddasLast(pAFeature = pF);
            }
         pAFeature->addCFR(fMRDB? MFeat::MRDBType : MFeat::AASType,*p54);
         if (!strlen(pSD->CustNum))
           mystrstrip(mystrncpy(pSD->CustNum,p54->custnum,sizeof(p54->custnum)));
         }
        } //F86752
       else fDone = true;
       break;
       }                                         // end case 54
     case 5:                                     // comment record
       if (!pAFeature)
         passThruList.Add(new CFRecV(NextLine));
       else pAFeature->addCFR(MFeat::OtherType,NextLine);
       break;
     default:
       if (fFirst54) {
         rc = RC_SKIP;
         tellem.say(false,EUO_MSG_NO_54, findFileName(pSD->InFiles[fMRDB]),
                    Buffer);
         }
       else if (passThruList.NumberElements())
              pAMachine->otherRecList(fMRDB) += passThruList;
       fsetpos(f,&pos);                          // put record back
       fDone = true;
     }
 if (!fDone) {
   rc = RC_ABORT;
   tellem.say(true,EUO_MSG_NO_98,findFileName(pSD->InFiles[fMRDB]));
   }
 return rc;
}

int RMSystem::Process_25_RecordV31(char * Buffer,Boolean fMRDB, FILE * f)
{
 int rc = RC_OK;
 char * NextLine;
 fpos_t pos;
 Machine * pThisMachine = 0;
 Boolean fDone = false;
 pAFeature = 0;
 IString strDB; //89910
 while(!fDone && !fgetpos(f,&pos) && (NextLine = readCFRLine(f,BufK,1024)))
   switch (CFRecType(NextLine)) {
     case 5: L05[fMRDB].add(NextLine); break;
     case 54:
       if (pThisMachine)
         //edw pThisMachine->add54Rec(fMRDB,NextLine);
         continue; //edw
       else {
         rc = RC_SKIP;
         tellem.say(false,EUO_MSG_NO_95,
                    findFileName(pSD->InFiles[fMRDB]),Buffer);
         }
       break;
     case 95:
       if (!pThisMachine) {
         Machine * pM=new Machine((Record_08V31 *)Buffer,
                                  CFR95::serialNo(NextLine));
         if (!(pThisMachine = ML.FindProcessMachine(*pM)))
           ML.Add(pThisMachine = pM);
         else {
           delete pM;
           if (pThisMachine->hasData(fMRDB)) {
             rc = RC_SKIP;
             tellem.say(false,EUO_MSG_DUPE_MACH,
                        findFileName(pSD->InFiles[fMRDB]),NextLine);
             }
           }
         if (rc == RC_OK) {
           pThisMachine->setMachineRec(fMRDB,Buffer,31);
           if (!fMRDB && (CFRecType(Buffer) == 8))
             pThisMachine->setTag(Machine::AAS_Passthru);
           pThisMachine->addInfoRec(fMRDB,NextLine);  //edw06/08/1999
           }
         }
       //edw06/03/1999-move line below to if clause above
       //pThisMachine->addInfoRec(fMRDB,NextLine);
       break;
     case 96:
       if (pThisMachine) {
         MFeat * pF = new MFeat((Record_96 *)NextLine);
         if (!(pAFeature = pThisMachine->mfeatList().Find(*pF)))
           pThisMachine->mfeatList().Add(pAFeature = pF);
         else delete pF;
         pAFeature->addCFR(fMRDB? MFeat::MRDBType : MFeat::AASType,NextLine);
         }
       else {
         rc = RC_SKIP;
         tellem.say(false,EUO_MSG_NO_95,
                    findFileName(pSD->InFiles[fMRDB]),Buffer);
         }
       break;
    case 6:      //89910
        strDB= IString(NextLine[9])+IString(NextLine[10])+IString(NextLine[11]);  //89910
        if (strDB=="INV")     //89910
          {
           fsetpos(f,&pos);                          // put record back
           fDone = true;
          }
        else
          {
           // ZOR  95997  inlcude non INV 06 records
           pThisMachine->add06Rec(fMRDB,NextLine);
          }
        break;    //89910
     default:
       fsetpos(f,&pos);                          // put record back
       fDone = true;
     }
 if (!fDone) {
   rc = RC_ABORT;
   tellem.say(true,EUO_MSG_NO_98,findFileName(pSD->InFiles[fMRDB]));
   }
 else pAMachine = pThisMachine;                  // current machine
 return rc;
}

    /*-----------------------------------------------------------------------*\
    |    int Process_38_RecordV31(char * Buffer,Boolean fMRDB,FILE * f)       |
    |                                                                         |
    |     Process the AAS Services Record (38):                               |
    |                                                                         |
    |     Read the subsequent Records in the file and process per type:       |
    |       95:                                                               |
    |          The first 54-Record:                                           |
    |             Use the 54's serial# and ensure that such a machine         |
    |               is not yet contained in the Machine List in which         |
    |               case a message is logged and a 'SKIP' return code is      |
    |               issued which will cause this duplicate machine            |
    |               to be skipped.                                            |
    |                                                                         |
    |             Create a new machine with the 54's serial#, add it to       |
    |               the machine list and add the given record as the          |
    |               machine's Machine Record.                                 |
    |                                                                         |
    |    F86750  HECC: 01/13/2004                                             |
    \*-----------------------------------------------------------------------*/

int RMSystem::Process_38_RecordV31(char * Buffer,Boolean fMRDB, FILE * f)
{
 int rc = RC_OK;
 char * NextLine;
 fpos_t pos;
 Machine * pThisMachine = 0;
 Boolean fDone = false;
 pAFeature = 0;

 while(!fDone && !fgetpos(f,&pos) && (NextLine = readCFRLine(f,BufK,1024)))
   switch (CFRecType(NextLine)) {
     case 5: L05[fMRDB].add(NextLine);
       break;
     case 95:
       if (!pThisMachine) {
         Machine * pM=new Machine((Record_38_V31 *)Buffer,
                                  CFR95::serialNo(NextLine));
         if (!(pThisMachine = ML.FindProcessMachine(*pM)))
           ML.Add(pThisMachine = pM);
         else {
           delete pM;
           if (pThisMachine->hasData(fMRDB)) {
             rc = RC_SKIP;
             tellem.say(false,EUO_MSG_DUPE_MACH,
                        findFileName(pSD->InFiles[fMRDB]),NextLine);
             }
           }
         if (rc == RC_OK) {
           pThisMachine->setMachineRec(fMRDB,Buffer,31);
           pThisMachine->addInfoRec(fMRDB,NextLine);
           }
         }
       break;
     case 96:
       if (pThisMachine) {
         MFeat * pF = new MFeat((Record_96 *)NextLine);
         if (!(pAFeature = pThisMachine->mfeatList().Find(*pF)))
           pThisMachine->mfeatList().Add(pAFeature = pF);
         else delete pF;
         pAFeature->addCFR(fMRDB? MFeat::MRDBType : MFeat::AASType,NextLine);
         }
       else {
         rc = RC_SKIP;
         tellem.say(false,EUO_MSG_NO_95,
                    findFileName(pSD->InFiles[fMRDB]),Buffer);
         }
       break;
     default:
       fsetpos(f,&pos);                          // put record back
       fDone = true;
     }
 if (!fDone) {
   rc = RC_ABORT;
   tellem.say(true,EUO_MSG_NO_98,findFileName(pSD->InFiles[fMRDB]));
   }
 else pAMachine = pThisMachine;                  // current machine
 return rc;
}

    /*-----------------------------------------------------------------------*\
    |    int Process_47_49_Record(char * Buffer,Boolean fMRDB,FILE * f)       |
    |                                                                         |
    |     Process the AAS Software Records (47 & 49):                         |
    |                                                                         |
    |     Read the subsequent Records in the file and process per type:       |
    |       54:                                                               |
    |          The first 54-Record:                                           |
    |             Use the 54's serial# and ensure that such a machine         |
    |               is not yet contained in the Machine List in which         |
    |               case a message is logged and a 'SKIP' return code is      |
    |               issued which will cause this duplicate machine            |
    |               to be skipped.                                            |
    |                                                                         |
    |             Create a new machine with the 54's serial#, add it to       |
    |               the machine list and add the given record as the          |
    |               machine's Machine Record.                                 |
    |             For a version-30 file, add the 54-Record as the machine's   |
    |                First 54 Record.                                         |
    |          If the 54-Record represents a feature not already contained    |
    |             in the machine's list of features, create a new-one         |
    |             and add it.                                                 |
    |          Add the 54-Record to the feature's CFR Record List.            |
    |                                                                         |
    |       5: Add these comment records to a local list.                     |
    |                                                                         |
    |       Other:                                                            |
    |         Put the record back ensuring at least one                       |
    |           54-Record was read and, if not, a message is logged and a     |
    |           'SKIP' return code is issued which will cause this machine    |
    |           to be skipped.                                                |
    |         If we read in any comment records, add the accumulated          |
    |           list to that of the machine.                                  |
    |                                                                         |
    |     Reaching EOF, implies that the required 98 closure Record           |
    |       was missing and the return code will signal for                   |
    |       the termination of the resolution process.                        |
    \*-----------------------------------------------------------------------*/
int RMSystem::Process_47_49_Record(char * Buffer,Boolean fMRDB,FILE * f)
{
 int rc = RC_OK,fileVersion = version(fMRDB);
 char * NextLine;
 Record_54 * p54;
 fpos_t pos;
 Machine * pM;
 CFRecVL passThruList;                           // comments
 Boolean fDone = false,
         fFirst54 = true;
 if (fMRDB)
   return RC_SKIP;

 while(!fDone && !fgetpos(f,&pos) && (NextLine = readCFRLine(f,BufK,1024)))
   switch (CFRecType(NextLine)) {
     case 54: {
       p54 = (Record_54 *)NextLine;
       if (fFirst54) {
         fFirst54 = false;
         if (CFRecType(Buffer) == 47)
           pM = new Machine((Record_47 *)Buffer, CFR54::serialNo(NextLine));
         else pM = new Machine((Record_49 *)Buffer, CFR54::serialNo(NextLine));
         if (ML.Find(*pM)) {
           delete pM;
           rc = RC_SKIP;
           tellem.say(false,EUO_MSG_DUPE_MACH,findFileName(pSD->InFiles[0]),
                      Buffer);
           break;
           }
         ML.Add(pAMachine = pM);
         pAMachine->setMachineRec(fMRDB,Buffer,fileVersion);
         if (fileVersion == 30) {
           pAMachine->addInfoRec(fMRDB,NextLine);
           break;
           }
         }
       MFeat * pF = new MFeat(*p54);
       if (!(pAFeature = pAMachine->mfeatList().Find(*pF)))
         pAMachine->mfeatList().Add(pAFeature = pF);
       else delete pF;
       pAFeature->addCFR(fMRDB? MFeat::MRDBType : MFeat::AASType,*p54);
       break;
       }                                         // end case 54
     case 5:                                     // comment record
       passThruList.Add(new CFRecV(NextLine));
       break;
     default:
       if (fFirst54) {
         rc = RC_SKIP;
         tellem.say(false,EUO_MSG_NO_54,findFileName(pSD->InFiles[fMRDB]),
                    Buffer);
         }
       else if (passThruList.NumberElements())
              pAMachine->otherRecList(fMRDB) += passThruList;
       fsetpos(f,&pos);                          // put record back
       fDone = true;
     }
 if (!fDone) {
   rc = RC_ABORT;
   tellem.say(true,EUO_MSG_NO_98,findFileName(pSD->InFiles[fMRDB]));
   }
 return rc;
}

int RMSystem::Process_47_49_RecordV31(char * Buffer,Boolean fMRDB, FILE * f)
{
 int rc = RC_OK;
 char * NextLine;
 Boolean fDone = false;
 Machine * pThisMachine = 0;
 fpos_t pos;
 pAMachine = 0;
 IString strDB; //ZOR 95997
 while(!fDone && !fgetpos(f,&pos) && (NextLine = readCFRLine(f,BufK,1024)))
   switch (CFRecType(NextLine)) {
     case 5: L05[fMRDB].add(NextLine); break;
     case 95:
       if (!fAS4) {  /* F73732 EDW 07/25/02 - if not iSeries, execute original case 95 logic */
         if (!pThisMachine) {
           pThisMachine = new Machine((Record_SW_V31 *)Buffer,
                                     CFR95::serialNo(NextLine));
           ML.Add(pThisMachine);
           pThisMachine->setMachineRec(fMRDB,Buffer,31);
           }
         pThisMachine->addInfoRec(fMRDB,NextLine);
         break;
       }
       else {  /* F73732 EDW 07/25/02 - begin - if iSeries, execute new case 95 logic */
         if (!pThisMachine) {
           Machine * pM = new Machine((Record_SW_V31 *)Buffer,"");

           /* Temporarily change the tag for any SIPO type records
              so that SIPOs are reconciled like Hardware
           */
           if (!strcmp(pM->type(), SIPO_TYPE) && !strcmp(pM->model(), SIPO_MODEL)) {
             pM->setTag (Machine::Process);
             pM->setSIPOFlag(true);          //VBB09192002 D74157
             }

           if (pThisMachine = ML.Find(*pM))
             {
             delete pM;
             if (pThisMachine->hasData(fMRDB))
               {
               rc = RC_SKIP;
               }
             }
           else
             ML.Add(pThisMachine = pM);
           if (rc == RC_OK)
             {
             pThisMachine->setMachineRec(fMRDB,Buffer,31);
             pThisMachine->addInfoRec(fMRDB,NextLine);
             }
           }
         else
           pThisMachine->addInfoRec(fMRDB,NextLine);
         break;
       }
       /* F73732 EDW 07/25/02 - end */
     case 96:
       if (pThisMachine) {
         MFeat * pF = new MFeat((Record_96 *)NextLine);
         if (!(pAFeature = pThisMachine->mfeatList().Find(*pF)))
           pThisMachine->mfeatList().Add(pAFeature = pF);
         else delete pF;
         pAFeature->addCFR(fMRDB? MFeat::MRDBType : MFeat::AASType,NextLine);
         }
       else {
         rc = RC_SKIP;
         tellem.say(false,EUO_MSG_NO_95,
                    findFileName(pSD->InFiles[fMRDB]),Buffer);
         }
       break;
     case 6:      //ZOR    95997
        strDB= IString(NextLine[9])+IString(NextLine[10])+IString(NextLine[11]);  //ZOR 95997
        if (strDB!="INV")     //ZOR  95997
          {
            pThisMachine->add06Rec(fMRDB,NextLine);  // include non INV 06 records
          }
     default:
       fsetpos(f,&pos);                          // put record back
       fDone = true;
     }
 if (!fDone) {
   rc = RC_ABORT;
   tellem.say(true,EUO_MSG_NO_98,findFileName(pSD->InFiles[fMRDB]));
   }
 else pAMachine = pThisMachine;                  // current machine
 return rc;
}
 /*-----------------------------------------------------------------------*\
 |    int Process_17_Record(char * Buffer,Boolean fMRDB,FILE * f)          |
 |                                                                         |
 |     Process the MRDB Software Records (17), instead of collect it as a  |
 |     SW discrepancy in case the configuration be an iSeries or AS400     |
 |     F72273   HECC-06/27/2002                                            |
 \*-----------------------------------------------------------------------*/

int RMSystem::Process_17_Record(char * Buffer,Boolean fMRDB, FILE * f)
{
 int rc = RC_OK,fileVersion = version(fMRDB);
 char * NextLine;
 Record_17 * p17;
 Record_54 * p54;
 IString strTempRec = "";
 IString strTempBuf = "";
 fpos_t pos;
 Machine * pM;
 CFRecVL passThruList;                    // Comments object
 Boolean fDone = false,
         fFirst54 = true;
 int iNumFeat = 0;

 while(!fDone && !fgetpos(f,&pos) && (NextLine = readCFRLine(f,BufK,1024)))
   switch (CFRecType(NextLine))
     {
     case 98:         /* This is the last RT 17 of the file because it is followed by a RT 98 */
     case 17:         /* A RT 17 is followed by a RT 17 instead of a RT 54 */
       if (fFirst54)  /* If flag is On, and the next read line is a RT 17  */
          {           /* the preceeding RT 17 doesn't have any RT 54       */
          p17 = (Record_17 *)Buffer;

          /* F73732 EDW 07/25/02 - SerialNums are not currently populated in RT 17s, so we
             will hardcode it be a null string
           */
          pM = new Machine((Record_17 *)Buffer, "");
          //pM = new Machine((Record_17 *)Buffer, "0000000");  /* SerialNo. doesn't exists in a RT 17 */
                                                               /* so, it had to be hardcoded          */

          /* F73732 EDW 07/25/02 - temporarily change the tag for any
             SIPO type records so that SIPOs are reconciled like Hardware
          */
          if (!strcmp(pM->type(), SIPO_TYPE) && !strcmp(pM->model(), SIPO_MODEL)) {
             pM->setTag (Machine::Process);
             pM->setSIPOFlag(true);          //VBB09192002 D74157
             }

          //if (ML.Find(*pM))                //F73732 EDW 07/25/02
          if (pAMachine = ML.Find(*pM))      //F73732 EDW 07/25/02
             {
             delete pM;
             if (pAMachine->hasData(fMRDB))  //F73732 EDW 07/25/02
                {
                rc = RC_SKIP;
                tellem.say(false,EUO_MSG_DUPE_MACH,    /* Duplicate Machine entry */
                           findFileName(pSD->InFiles[0]), Buffer);
                }
             //fsetpos(f,&pos);              //F73732 EDW 07/25/02
             //fDone = true;                 //F73732 EDW 07/25/02
             //break;                        //F73732 EDW 07/25/02
             }
          else                               //F73732 EDW 07/25/02
             ML.Add(pAMachine = pM);
          if (rc == RC_OK)                   //F73732 EDW 07/25/02
             {
                pAMachine->setMachineRec(fMRDB,Buffer,fileVersion);
                strTempBuf = "54      I" + IString(pAMachine->type()) + "    " +
                                           IString(pAMachine->model());      //F73732 EDW 07/25/02
                // pAMachine->addInfoRec(fMRDB,Buffer);                      //F73732 EDW 07/25/02
                pAMachine->addInfoRec(fMRDB,strTempBuf);                     //F73732 EDW 07/25/02
             }
          /* F73732 EDW 07/25/02 - begin
             The following logic is adding missing feature type RT54s.
             This is now being done by Machine::checkMachineRecQuantities as
             this is where it is done for the other machine record type. So,
             this logic can be omitted.

          strTempRec = Buffer;
          strTempRec = strTempRec.subString(17);

          if (strTempRec.length())
             {
             while (strTempRec.length())
                   {
                   iNumFeat = atoi(strTempRec.subString(7,2));
                   while ( iNumFeat >0 )
                         {
                         strTempBuf = "54      I" + IString(pAMachine->type()) +
                                      "    " + IString(pAMachine->model()) +
                                      " "  + strTempRec.subString(1,6).strip().rightJustify(6,' ');
                         Buffer = strTempBuf;

                         p54 = (Record_54 *)Buffer;
                         MFeat * pF = new MFeat(*p54);
                         if (!(pAFeature = pAMachine->mfeatList().Find(*pF)))
                            pAMachine->mfeatList().Add(pAFeature = pF);
                         else
                            delete pF;
                         pAFeature->addCFR(fMRDB? MFeat::MRDBType : MFeat::AASType,*p54);
                         if (!strlen(pSD->CustNum))
                            mystrstrip(mystrncpy(pSD->CustNum,p54->custnum,sizeof(p54->custnum)));
                         iNumFeat--;
                         }
                   strTempRec = strTempRec.subString(9);
                   }
             }
          F73732 EDW 07/25/02 */
          fsetpos(f,&pos);                          /* Put record back */
          fDone = true;
          break;
          }                                         /* End of 'Case 17' */
       else
          {
          fsetpos(f,&pos);  /* If next record is a RT 17 and the fFirst54 */
          fDone = true;     /* flag is OFF, it means that, at least, one  */
          break;            /* RT 54 was read in the preceeding product   */
          }                 /* and the actual RT17 belongs to another product. */
     case 54:
       {
       p54 = (Record_54 *)NextLine;
       if (fFirst54)
          {
          fFirst54 = false;
          //pM = new Machine((Record_17 *)Buffer, CFR54::serialNo(NextLine)); //F73732 EDW 07/25/02
          pM = new Machine((Record_17 *)Buffer, "");                          //F73732 EDW 07/25/02

          /* F73732 EDW 07/25/02 - temporarily change the tag for any
             SIPO type records so that SIPOs are reconciled like Hardware
          */
          if (!strcmp(pM->type(), SIPO_TYPE) && !strcmp(pM->model(), SIPO_MODEL)) {
             pM->setTag (Machine::Process);
             pM->setSIPOFlag(true);          //VBB09192002 D74157
             }

          if (pAMachine = ML.Find(*pM))
             {
             delete pM;
             if (pAMachine->hasData(fMRDB))  //F73732 EDW 07/25/02
                {
                rc = RC_SKIP;
                tellem.say(false,EUO_MSG_DUPE_MACH,       /* Duplicate Machine entry */
                           findFileName(pSD->InFiles[0]), Buffer);
                }
             //break;                        //F73732 EDW 07/25/02
             }
          else                               //F73732 EDW 07/25/02
             ML.Add(pAMachine = pM);
          if (rc == RC_OK)                   //F73732 EDW 07/25/02
             {
             pAMachine->setMachineRec(fMRDB,Buffer,fileVersion);
             if ((fileVersion == 30)  ||
                 ( (fileVersion == 20) && CFR54::blankFeature(NextLine)))
                 {
                 pAMachine->addInfoRec(fMRDB,NextLine);
                 //if (fMRDB)                        // we prefer MRDB's serial no //F73732 EDW 07/25/02
                 //   pAMachine->setSerialNo((char *)CFR54::serialNo(NextLine));   //F73732 EDW 07/25/02
                 }
             else if (fMRDB)                 //F73732 EDW 07/25/02
                {
                CFRecV Temp(NextLine);
                CFR54::setFeature((Record_54 *)Temp.record(), " ");
                pAMachine->addInfoRec(fMRDB,Temp.record());
                }
             }
          break;
          }
       else
          {
          MFeat * pF = new MFeat(*p54);
          if (!(pAFeature = pAMachine->mfeatList().Find(*pF)))
             pAMachine->mfeatList().Add(pAFeature = pF);
          else
             delete pF;
          pAFeature->addCFR(fMRDB? MFeat::MRDBType : MFeat::AASType,*p54);
          if (!strlen(pSD->CustNum))
             mystrstrip(mystrncpy(pSD->CustNum,p54->custnum,sizeof(p54->custnum)));
          break;
          }
       }                                         // End of 'Case 54'
     case 5:                                     // It's a comment record
       passThruList.Add(new CFRecV(NextLine));
       break;
     default:
       if (fFirst54)
          {
          rc = RC_SKIP;
          tellem.say(false,EUO_MSG_NO_54,
                     findFileName(pSD->InFiles[fMRDB]), Buffer);
          }
       else
          if (passThruList.NumberElements())
              pAMachine->otherRecList(fMRDB) += passThruList;
       fsetpos(f,&pos);                          // Put record back
       fDone = true;
     }
 if (!fDone)
    {
    rc = RC_ABORT;
    tellem.say(true,EUO_MSG_NO_98,findFileName(pSD->InFiles[fMRDB]));
    }
 return rc;
}

    /*-----------------------------------------------------------------------*\
    |   void Process_53_Record(Record_53 * p53,Boolean fMRDB)                 |
    |       Process the given AAS or MRDB System Header Record :              |
    |                                                                         |
    |         Retain some data in the System Data structure:                  |
    |            The System Number.                                           |
    |            The Date.                                                    |
    |         Add the record.                                                 |
    \*-----------------------------------------------------------------------*/
void RMSystem::Process_53_Record(Record_53 * p53,Boolean fMRDB)
{
 int len = strlen((char *)p53);
 if (len > offsetof(Record_53,Number))
   mystrstrip(mystrncpy(pSD->Number[fMRDB],p53->Number,sizeof(p53->Number)));
 else p53->Number[0] = '\0';
 if (len > offsetof(Record_53,Date))
   mystrstrip(mystrncpy(pSD->DateRec53[fMRDB],p53->Date,sizeof(p53->Date)));
 else pSD->DateRec53[fMRDB][0] = '\0';
 addCFR((char *)p53,fMRDB);
}

    /*-----------------------------------------------------------------------*\
    |   void ProcessOrphanRecord(char * Buffer,Boolean fMRDB)                 |
    |       If the logic directs the read record here, it represents a record |
    |       for which there is no machine.                                    |
    \*-----------------------------------------------------------------------*/
void RMSystem::ProcessOrphanRecord(char * Buffer,Boolean fMRDB)
{
 tellem.say(false,EUO_MSG_ORPHAN_RECORD,
            findFileName(pSD->InFiles[fMRDB]),Buffer);
}

    /*-----------------------------------------------------------------------*\
    |   char * readCFRLine(FILE *f,char * Buffer, unsigned BufLen)            |
    |       Read the given number of bytes at the current position            |
    |       of the given file to the given buffer, stripping white            |
    |       spaces at the rear.                                               |
    \*-----------------------------------------------------------------------*/
char * RMSystem::readCFRLine(FILE *f,char * Buffer, unsigned BufLen)
{
 if (f && Buffer && BufLen)
   if (fgets(Buffer,BufLen,f))
     return Buffer;          // erg 03/30/2000  D48444
  // return mystrrearstrip(Buffer);
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |   char * readCFRLine(FILE *f)                                           |
    |       Read the line at the current position of the given                |
    |       file to our own buffer, stripping white                           |
    |       spaces at the rear.                                               |
    \*-----------------------------------------------------------------------*/
char * RMSystem::readCFRLine(FILE *f)
{
 if (ReadBuff || (ReadBuff = new char[READBUF_SIZE]))
   if (fgets(ReadBuff,READBUF_SIZE,f))
     return mystrrearstrip(ReadBuff);
 return 0;
}

int RMSystem::filesVersion(const char * FileName)
{
 FILE  *f;
 int version = -1;
 char * Buf;
 if (f = fopen(FileName,"r")) {
   if (Buf = readCFRLine(f))
     if (CFRecType(Buf) == 0)
       version = CFRecType(((CFR_00 *)Buf)->Version);
   fclose(f);
   }
 return version;
}

Boolean RMSystem :: runCheckSum(const char * file, unsigned long * pSum)
{            /*jt
 RESULTCODES resultCodes;
 IString     pgm("CKSMCFR2.EXE")
           , args(IApplication::current().argv(0));
 PVOID pv; jt*/
 Boolean fOK = false;
 /*jt if (file && pSum &&
     !DosAllocSharedMem(&pv,0,100,PAG_READ | PAG_WRITE | fSHARE | PAG_COMMIT) &&
     !DosGetSharedMem (pv,PAG_READ | PAG_WRITE)) {
   args.remove(args.lastIndexOf('\\'));
   args += IString('\\') + pgm + IString('@') + IString(file) +
           IString(' ') + IString((unsigned long)pv) + IString("@@@");
   args.translate('@',IString('\0'));            // Change spaces to NULLs
   APIRET rc = DosExecPgm(BufK,100,EXEC_SYNC,args,NULL,&resultCodes,pgm);
   if (!rc && resultCodes.codeResult) {
     *pSum = *((unsigned long *)pv);
     fOK = true;
     }
   else tellem.say(true,EUO_MSG_ERR_CHECKSUM_RUN,(char *)pgm,
                (char *)IString(rc? rc : resultCodes.codeResult));
   DosFreeMem(pv);
   }             jt*/
 return fOK;
}

/* edw-11/17/98
#if 0
// Counted on DosExit in CKSMFR.EXE to store a ULONG in resultCodes.codeResult
// turns out that only a part of the number (for which the utility returns
// 10 digits) is stored. (like type-casted to a SHORT)
Boolean RMSystem :: runCheckSum(const char * file, unsigned long * pSum)
{
 RESULTCODES resultCodes;
 IString     pgm("CKSMCFR.EXE");
 APIRET      rc = 0;
 IString     args =  strExecPath;
 // args = IApplication::current().argv(0);
 // args.remove(args.lastIndexOf('\\'));
 args += IString('\\') + pgm         +
         IString('@') + IString(file) + IString("@@@");
 args.translate('@',IString('\0'));              // Change spaces to NULLs
//  fprintf(stderr,"DosExecPgm Args: %s\n",args); // was for debugging
 resultCodes.codeTerminate = resultCodes.codeResult = 0;
 if (rc = DosExecPgm(BufK,100,EXEC_SYNC,args,NULL,&resultCodes,pgm))
   return tellem.say(true,EUO_MSG_ERR_CONVERSION_START,
                     (char *)pgm,(char *)IString(rc));
 *pSum = resultCodes.codeResult;
  return true;
}
#endif
*/


#if 0
Boolean  RMSystem :: runConversion(XWindow * pXW,const char * fileFrom,
                                 const char * fileTo, unsigned version)
{
  RESULTCODES resultCodes;
  IString     pgm("CFCONVRT.EXE");
  APIRET      rc = 0;
  IString     args =  strExecPath;
  unsigned long ulPID;
  //args = IApplication::current().argv(0);
  //args.remove(args.lastIndexOf('\\'));
  args += IString('\\') + pgm               +
          IString('@') + IString(fileFrom) +
          IString(' ') + IString(fileTo)   +
          IString(' ') + IString(version)  + IString("@@@");
  args.translate('@',IString('\0'));             // Change spaces to NULLs
//  fprintf(stderr,"DosExecPgm Args: %s\n",args); // was for debugging
  resultCodes.codeTerminate = resultCodes.codeResult = INT_MAX;
  if (rc = DosExecPgm(BufK,100,EXEC_ASYNCRESULT,args,NULL,&resultCodes,pgm))
    return tellem.say(true,EUO_MSG_ERR_CONVERSION_START,
                      (char *)pgm,(char *)IString(rc));
                                                 // the Process ID is stored in
                                                 // the first unsigned long
  pXW->setHostPID(ulPID = resultCodes.codeTerminate);
  rc = DosWaitChild(DCWA_PROCESS,DCWW_WAIT,&resultCodes,&ulPID,ulPID);
  pXW->setHostPID(0);
  if (rc)
    return tellem.say(true,EUO_MSG_ERR_CONVERSION_WAIT,(char *)IString(rc));
  if (resultCodes.codeResult)
    return tellem.say(true,EUO_MSG_ERR_CONVERSION,
                      (char *)pgm,(char *)IString(resultCodes.codeResult));
  return true;
}
#endif
Boolean  RMSystem :: runConversion(XWindow * pXW,const char * fileFrom,
                                 const char * fileTo, unsigned version)
{      /*jt
 RESULTCODES resultCodes;
 IString     pgm("CFCONVRT.EXE")
           , args(IApplication::current().argv(0));
 unsigned long ulPID;
 args.remove(args.lastIndexOf('\\'));
 args += IString('\\') + pgm + IString('@') +
                 IString(fileFrom) + IString(' ') + IString(fileTo)   +
                 IString(' ') + IString(version)  + IString("@@@");
 args.translate('@',IString('\0'));              // Change spaces to NULLs
 resultCodes.codeTerminate = resultCodes.codeResult = INT_MAX;
 APIRET rc = DosExecPgm(BufK,100,EXEC_ASYNCRESULT,args,NULL,&resultCodes,pgm);
 if (!rc) {
                                                 // the Process ID is stored in
                                                 // the first unsigned long
    pXW->setHostPID(ulPID = resultCodes.codeTerminate);
    rc = DosWaitChild(DCWA_PROCESS,DCWW_WAIT,&resultCodes,&ulPID,ulPID);
    pXW->setHostPID(0);
    if (rc)
      return tellem.say(true,EUO_MSG_ERR_CONVERSION_WAIT,(char *)IString(rc));
    if (resultCodes.codeResult)
      return tellem.say(true,EUO_MSG_ERR_CONVERSION,(char *)pgm,
                        (char *)IString(resultCodes.codeResult));
    return true;
    }
  else  return tellem.say(true,EUO_MSG_ERR_CONVERSION_START,
                          (char *)pgm,(char *)IString(rc));  jt*/
}

    /*------------------------------------------------------------------------*\
    |   Boolean readCFRFile(XWindow * pXW,Boolean fMRDB)                       |
    |       Read the AAS or MRDB file:                                         |
    |         Open the file and, if unsuccessfull, have a message              |
    |            logged and return indicating failure.                         |
    |         Read the records and call the appropriate methods as per         |
    |           their type with the folllowing additional explanations:        |
    |                                                                          |
    |           5-52-57:                                                       |
    |              These comment records are assumed to pertain to the         |
    |              last feature processed and therefore, are stored            |
    |              with that feature. This is done to somewhat preserve        |
    |              the relative placements of the comments in the              |
    |              AAS or MRDB files.                                          |
    |              If the comments appear near the top of the file             |
    |              (before any features in the file have been processed),      |
    |              the comments are assumed to pertain to the system and       |
    |              therefore, are stored with the system.                      |
    |              The machine processes have logic to store comments          |
    |              with the machines and their features.                       |
    |                                                                          |
    |           8-25:                                                          |
    |              We're required to preserve the order with respect           |
    |              to the first machine read from the MRDB file, while         |
    |              the remainder is allowed to be alphabetically sorted.       |
    |              To that purpose, that first machine is tagged as such       |
    |              at this time.                                               |
    |                                                                          |
    |           53:                                                            |
    |              If the MRDB file's sensed date is 30 days or older,         |
    |              the user is to decide whether to continue the resolution.   |
    |              Since we're in a thread, we format an appropriate           |
    |              message text here and have the main window show it and      |
    |              retrieve the user's response.                               |
    |              If we're are to quit, we do so after logging a message.     |
    |                                                                          |
    |              We leave our signature by creating a special comment        |
    |              record and adding it after the 53-Record to                 |
    |              the System CFR Records. (No signature record will           |
    |              be generated if the MRDB file does not contain a 53 Record. |
    |                                                                          |
    |           55: Accumulate Placement Records in a list to be used          |
    |               when writing the resolved CFReport                         |
    |                                                                          |
    |           56: Accumulate Connection Records in a list to be used         |
    |               when writing the resolved CFReport                         |
    |                                                                          |
    |         Check if the report was terminated with the trailer              |
    |           record (98) and log as a discrepancy when missing.             |
    |         Edit the machines' CFReport Records with respect to their        |
    |           read-in features and report discrepancies.                     |
    |         Retrieve and set the descriptions of the Software Discrepancies. |
    \*------------------------------------------------------------------------*/
Boolean RMSystem::readCFRFile(XWindow * pXW,Boolean fMRDB,Boolean bAS4)  //Include bAS4 flag to reconcile iSeries SW.  HECC 72273 06/27/2002
{
 Boolean fProcess54s = false;
 Boolean fDone = false;
 Boolean fTaggedCPU = false;
 Boolean fNewSystem = false;                     //edw06/05/1998
 Boolean olddata53  = false;                     //57519 - MAMT - 12/29/2000
 unsigned RecType;
 FILE  *f;
 int rc = RC_OK;
 char * Buf;
 IString str53, strTypeNum;
 pAMachine = 0;
 pAFeature = 0;
 fAS4 = bAS4; /* F73732 EDW 07/25/02 */
 if (!(f = fopen(pSD->InFiles[fMRDB],"r")))
   return tellem.say(true,EUO_MSG_NO_FILE,findFileName(pSD->InFiles[fMRDB]));
 while (!fDone && (rc != RC_ABORT) && (Buf = readCFRLine(f)))
   switch(RecType = CFRecType(Buf)) {
     case 0:
       if (!Process_00_Record(Buf,fMRDB))
         rc = RC_ABORT;
       break;
     case 3:
       Process_03_Record(Buf,fMRDB);
       break;
     case 52:
     case 57:
     case 5:                                     // comments
       if (pAFeature)
                                                 // assume last feature's
         pAFeature->addCFR(MFeat::OtherType,Buf);
       else addCFR(Buf,fMRDB);                   // assume a System comment
       break;
     case 8:
     case 25:
       switch ( rc = Process_25_Record(Buf,fMRDB,f)) {
         case RC_SKIP:
           fProcess54s = false;                  // skip subsequent features
           break;
         case RC_ABORT: break;
         default:
           fProcess54s = true;
           if ( pAMachine->getSystemId().length() == 0 )  //edw06/05/1998
              {
              if ( !fNewSystem )
                 {
                 fNewSystem = true;
                 Process_07_Record(Buf, fMRDB);
                 }
              pAMachine->setSystemId(strSysId);
              }
           if (fMRDB && !fTaggedCPU && pAMachine &&
               (pAMachine->tag() == Machine::Process))
             pAMachine->setFirstMRDBInd(fTaggedCPU = true);
         }
       break;
     case 17:
                                                 // accumulate MRDB Software
       if ( (fMRDB) &&                           // Discrepancies
            (!bAS4) )                            // If configuration isn't iSeries or AS400, only collect the SW
                                                 // discrepancies  F72273   HECC-06/27/2002
          collectSWDiscrepancies(f,(Record_17 *)Buf);

       /* Begin: If the current configuration is iSeries, addd the MRDB SW */
       /* to the MachineList.  F72273   HECC-06/27/2002  */
       else
          if (fMRDB && bAS4)
             {
             rc = Process_17_Record(Buf,fMRDB,f);
             if (rc == RC_OK)
                {
                if (pAMachine)
                   {
                   pAMachine->setSubsystemRecordId(false,L10[fMRDB].NumberElements());
                   pAMachine->setSystemId(strSysId);
                   }
                }
             }
       /* End: F72273   HECC-06/27/2002  */

       fProcess54s = false;                      // skip subsequent features
       break;


     case 19:
     case 50:                                    // accumulate in Backlog List
       switch ( rc = Process_19_50_Record(Buf,fMRDB,f)) {
         case RC_SKIP:
           fProcess54s = false;                  // skip subsequent features
           break;
         case RC_ABORT: break;
         default: fProcess54s = true;
         }
       break;
     case 26:                                    // skip this machine
       fProcess54s = false;                      // skip subsequent features
       break;
     case 47:
     case 49:                                    // AAS software
       switch ( rc = Process_47_49_Record(Buf,fMRDB,f)) {
         case RC_SKIP:
           fProcess54s = false;                  // skip subsequent features
           break;
         case RC_ABORT: break;
         default: fProcess54s = true;
         }
       break;
     case 53:
       Process_53_Record((Record_53 *)Buf,fMRDB);
       str53 = Buf;
       str53.strip();
//       strTypeNum = str53.subString(5, 4) + STR_Hyphen + str53.subString(10, 7); old logic 57519
       if (fMRDB)
         if ((senseDate() + 30) < IDate::today())
           {
           olddata53 = true;            //57519 - MAMT - 12/27/2000
           pXW->iUseOldMRDBUserChoice = 1; //edw12/29/99
              /*--------------------------------------------------------*\
              | There is a rare side effect here : if there's no         |
              | message-string file such that 'tellem' can't fing the    |
              | message, the IMessageText class returns a string to that |
              | effect which is stuffed in the shown message box         |
              | instead. However, we're showing buttons for a 'do you    |
              | want to continue'-type of text                           |
              \*--------------------------------------------------------*/
 /*          if ( pXW->WWHandleMessage( //old logic 57519 - MAMT - 12/27/2000
                                (char *)tellem.message(EUO_MSG_OLD_MRDB_PICK, strTypeNum, pLastSensed, senseDate().asString( IDate::yyyy )),
                                 IDH_PNL_MSGBOX_OLD_MRDB,
                                 IMessageBox::yesNoButton
                                 |IMessageBox::applicationModal
                                 |IMessageBox::warningIcon
                                 |IMessageBox::defButton2
                                 |IMessageBox::moveable
                                 |IMessageBox::systemModal) == IMessageBox::no)
                                                 // CR24
                                                 //ZOR 060900 def 51688
             {
             fclose(f);                          //edw 05/07/1998
             return tellem.say(false,EUO_MSG_OLD_MRDB_QUIT);
             }
           else //edw12/29/99
             {
             pXW->iUseOldMRDBUserChoice = 2;
             } */
           }
    //   addCFR(pRM2SignatureRecord,fMRDB);        // CR23
        addCFR(str53,fMRDB);   // 95887    save 53 record  in CFRLists
       break;
     //edw05/15/98 - Modified cases 54: 55: and 56: to handle orphan records
     case 54: {
       //edw if (fProcess54s)
       //edw  ProcessOrphanRecord(Buf,fMRDB);
       Record_54 *p54;
       p54 = (Record_54*)Buf;
       //Begin new logic 57519 - MAMT - 12/27/2000
       if (olddata53)
          {
           strTypeNum = str53.subString(5, 4) + STR_Hyphen + strSerial;
           olddata53  = false;
           if ( pXW->WWHandleMessage(
                               (char *)tellem.message(EUO_MSG_OLD_MRDB_PICK, strTypeNum, pLastSensed, senseDate().asString( IDate::yyyy )),
                                IDH_PNL_MSGBOX_OLD_MRDB,
                                IMessageBox::yesNoButton
                                |IMessageBox::applicationModal
                                |IMessageBox::warningIcon
                                |IMessageBox::defButton2
                                |IMessageBox::moveable
                                |IMessageBox::systemModal) == IMessageBox::no)
                                                // CR24
                                                //ZOR 060900 def 51688
              {
              fclose(f);                          //edw 05/07/1998
              return tellem.say(false,EUO_MSG_OLD_MRDB_QUIT);
              }
            else //edw12/29/99
              {
              pXW->iUseOldMRDBUserChoice = 2;
              }
          }
       //End new logic 57519 - MAMT - 12/27/2000

       MFeat * pF = new MFeat(*p54);
       pAMachine->MachSecFeatList.AddasLast(pAFeature = pF);
       pAFeature->addCFR(fMRDB? MFeat::MRDBType : MFeat::AASType,*p54);
       break;
       }
     case 55:                                    // Placement Records
       L55.Add(new CFRInst(Buf));
       break;
     case 56:                                    // Connection Records
       L56.Add(new CFRInst(Buf));
       break;
     case 98:
       //edw fDone = true;                       //edw06/04/1998
       break;
     default: tellem.say(false,EUO_MSG_UNKNOWN_REC_TYPE,
                         findFileName(pSD->InFiles[fMRDB]),
                         (char *)IString(IString(Buf[0])+IString(Buf[1])),Buf);
     }
  if (rc != RC_ABORT) {
    if (!fDone)
      tellem.say(false,EUO_MSG_NO_98,findFileName(pSD->InFiles[fMRDB]));
    for (int i =  0, N = ML.NumberElements(); i < N ; i++)
      ML.element(i)->checkMachRecQuantities(fMRDB,tellem,
                                            findFileName(pSD->InFiles[fMRDB]));
    if (fMRDB) {
      SoftMachList::Cursor crsr(SWDiscList);
      forCursor(crsr) {
        SoftMach & sm = SWDiscList.elementAt(crsr);
        sm.setDescription(PK.description(sm.type(),sm.model(),sm.feature()));
        }
      }
    }
  fclose(f);
  return (rc != RC_ABORT);
}

Boolean RMSystem::readCFRFileV31(XWindow * pXW,Boolean fMRDB, Boolean bAS4)  /* F73732 EDW 07/25/02 */
{
 Boolean fSkip = true;
 Boolean fDone = false;
 Boolean fTaggedCPU = false;
 Boolean bFlagSkip = false;  //F86752 req 1373
 unsigned RecType;
 FILE  *f;
 int rc = RC_OK;
 char * Buf;
 pAMachine = 0;
 pAFeature = 0;
 IString strDB; //89910
 fAS4 = bAS4; /* F73732 EDW 07/25/02 */
 if (!(f = fopen(pSD->InFiles[fMRDB],"r")))
   return tellem.say(true,EUO_MSG_NO_FILE,findFileName(pSD->InFiles[fMRDB]));
// while (!fDone && (rc != RC_ABORT) && (Buf = readCFRLine(f)))
 while ( (rc != RC_ABORT) && (Buf = readCFRLine(f)))
   switch(RecType = CFRecType(Buf)) {
     case 0:
       if (!Process_00_Record(Buf,fMRDB))
         rc = RC_ABORT;
       break;
     case 1: Process_01_Record(Buf,fMRDB); break;
     case 3: Process_03_Record(Buf,fMRDB); break;
     case 5: L05[fMRDB].add(Buf); break;
     case 7: Process_07_Record(Buf,fMRDB); break;
     case 6:             // F86752 process 06 records
       // it will read the first FL record, and the MS records will be read in the method
       // so it will skip the next records because they are already processed.
        strDB= IString(Buf[9])+IString(Buf[10])+IString(Buf[11]);  //89910
        if (strDB=="INV")     //89910
         {
         if(!bFlagSkip)
           {
           Process_06_RecordV31(Buf,fMRDB,f);        //1373
           bFlagSkip=true;
           }
         }
        break;
     case 8:
     case 25:
       rc = Process_25_RecordV31(Buf,fMRDB,f);
       if (rc == RC_OK) {
         if (fMRDB) {
           if (!fTaggedCPU && pAMachine && (pAMachine->tag()==Machine::Process))
             pAMachine->setFirstMRDBInd(fTaggedCPU = true);
           }
         else if (pAMachine)
            {
                pAMachine->setSubsystemRecordId(false,L10[fMRDB].NumberElements());
                pAMachine->setSystemId(strSysId);                 //edw06/04/1998
            }
         }
       else fSkip = (rc == RC_SKIP);
       break;
     case 10:                                    // Subsystem Records
       if (!fMRDB)
         L10[fMRDB].add(L10[fMRDB].NumberElements() + 1,Buf);
       break;
     case 19:
     case 50:                                    // accumulate in Backlog List
       rc = Process_19_50_RecordV31(Buf,fMRDB,f,strSysId);  //VBB09192002 D74157
       fSkip = (rc == RC_SKIP);
       break;
     case 26:                                    // skip this machine
       fSkip = true;                             // skip subsequent features
       break;

     //* F86750  Begin logic:  Process the services RT 38 records
     case 38:                                    // Process Services RT 38records
       rc = Process_38_RecordV31(Buf,fMRDB,f);
       if (rc == RC_OK)  {
          if (pAMachine)
             {
              pAMachine->setSubsystemRecordId(false,L10[fMRDB].NumberElements());
              pAMachine->setSystemId(strSysId);
             }
          }
       else fSkip = (rc == RC_SKIP);
       break;
     //* F86750  End logic: HECC: 01/13/2004

     case 47:
     case 49:                                    // AAS software
                                                 // accumulate MRDB Software
       if (fMRDB) {                              // Discrepancies
         if (RecType == 47)
           collectSWDiscrepancies(f,(const Record_SW_V31 *)Buf);
         fSkip = true;                           // skip subsequent features
         }
       else {
         rc = Process_47_49_RecordV31(Buf,fMRDB,f);
         if (rc == RC_OK) {
           if (pAMachine)
              {
              pAMachine->setSubsystemRecordId(false,L10[fMRDB].NumberElements());
              pAMachine->setSystemId(strSysId);                 //edw06/04/1998
              }
           }
         else fSkip = (rc == RC_SKIP);
         }
       break;
     case 52:
       if (pAMachine)
         L52[fMRDB].add(L10[fMRDB].NumberElements(),Buf);
       break;
     case 53:
       Process_53_Record((Record_53 *)Buf,fMRDB);
       if (fMRDB)
         if ((senseDate() + 30) < IDate::today())
           {
           pXW->iUseOldMRDBUserChoice = 1; //edw12/29/99
           if ( pXW->WWHandleMessage(
                                (char *)tellem.message(EUO_MSG_OLD_MRDB_PICK),
                                 IDH_PNL_MSGBOX_OLD_MRDB,
                                 IMessageBox::yesNoButton
                                 |IMessageBox::applicationModal
                                 |IMessageBox::warningIcon
                                 |IMessageBox::defButton2
                                 |IMessageBox::systemModal
                                 |IMessageBox::moveable) == IMessageBox::no)
                                                 // CR24
             {
             fclose(f);                          //edw 05/07/1998
             return tellem.say(false,EUO_MSG_OLD_MRDB_QUIT);
             }
           else //edw12/29/99
             {
             pXW->iUseOldMRDBUserChoice = 2;
             }
           }
     //  addCFR(aRM2SignatureRecord,fMRDB);        // CR23
         addCFR(Buf,fMRDB);    //95887    save 53 record  in CFRLists
       break;
     case 57:
       if (pAFeature)
                                                 // assume last feature's
         pAFeature->addCFR(MFeat::OtherType,Buf);
       else addCFR(Buf,fMRDB);                   // assume System's
       break;
     case 54:
        break; //edw
     case 95:
     case 96:
       if (!fSkip)
         ProcessOrphanRecord(Buf,fMRDB);
       break;
     case 55:                                    // Placement Records
       //edw L55.Add(new CFRInst(Buf));
       break;
     case 56:                                    // Connection Records
       //edw L56.Add(new CFRInst(Buf));
       break;
     case 98:
       fDone = true;
       break;
     case 99:
       //Commented for F47640 - flw
       //pSD->ulCheckSum[fMRDB]=IString::stripBlanks(IString(Buf+2,10,0)).asUnsigned();
       break;
     default: tellem.say(false,EUO_MSG_UNKNOWN_REC_TYPE,
                         findFileName(pSD->InFiles[fMRDB]),
                         (char *)IString(IString(Buf[0])+IString(Buf[1])),Buf);
     }
  fclose(f);
  //Commented for F47640 - flw
  //if ( pSD->ulCheckSum[fMRDB] &&
    // (pSD->ulCheckSum[fMRDB] != fCheckSum(pSD->InFiles[fMRDB])))
   //  (pSD->ulCheckSum[fMRDB] != cfrcksum(pSD->InFiles[fMRDB],"82151")))
   // return tellem.say(true,EUO_MSG_ERR_BAD_CHECKSUM,findFileName(pSD->InFiles[fMRDB]));
  if (rc != RC_ABORT) {
    if (!fDone)
      tellem.say(false,EUO_MSG_NO_98,findFileName(pSD->InFiles[fMRDB]));

    for (int i =  0, N = ML.NumberElements(); i < N ; i++)
      ML.element(i)->checkMachRecQuantities(fMRDB,tellem,
                                            findFileName(pSD->InFiles[fMRDB]));
    if (fMRDB) {
      SoftMachList::Cursor crsr(SWDiscList);
      forCursor(crsr) {
        SoftMach & sm = SWDiscList.elementAt(crsr);
        sm.setDescription(PK.description(sm.type(),sm.model(),sm.feature()));
        }
      }
    }
  return (rc != RC_ABORT);
}

    /*-----------------------------------------------------------------------*\
    |   Boolean saveState( const char * szFileName)                           |
    |       Save the System to the given file as follows:                     |
    |                                                                         |
    |       Collect the features and all CFReport Records througout           |
    |         the system.                                                     |
    |       For storage of the FeatList only:                                 |
    |         Have the machines set their storage numbers before doing        |
    |         the same for the FeatList elements.                             |
    |       Information about the file and every section of records           |
    |       it is to contain, is collected in an array of 'packing lists'     |
    |         as defined in EUODEF.H :                                        |
    |           struct PackList{ unsigned   Type;   // signature              |
    |                            unsigned   LRecl;  // record length          |
    |                            unsigned   Count;  // record count           |
    |                            unsigned   Offset; // file offset            |
    |                          };                                             |
    |       This array is written out at the top of the file with its         |
    |       first element containing the file's data as follows:              |
    |                        Count :  packing list array count                |
    |                        Offset:  file size                               |
    |                        LRecl :  packing list length                     |
    |       The System Data and record lists are then appended.               |
    |       Notes:                                                            |
    |             Currently the FeatList is not saved to the file since       |
    |             it can easily be rebuild. The logic works and is left       |
    |             here, although commented out, in anticipation of the        |
    |             Feat object becoming more involved.                         |
    |                                                                         |
    |             The CFRStore objects are used to retain the System's        |
    |              CFR record numbers when the object is written to a         |
    |              file. This data is used when the object is rebuild         |
    |              ( The System is written to a file without its              |
    |                Record Lists. The records in these lists                 |
    |                are numbered when collected ( via the 'pickup()'         |
    |                method) throughout the System                            |
    |                and written separately into the file.                    |
    |                After the System is read from the file, it can           |
    |                identify its records in the list read separately         |
    |                and rebuild its own Lists ( via the 'delivery()'         |
    |                method).                                                 |
    \*-----------------------------------------------------------------------*/
Boolean RMSystem::saveState( const char * szFileName)
{
 CFRecVL   RecList;
 MFeatList   MFList;
 int i,j;
 PackList *pPL1, *pPL2;

 pSD->uTotalMachs = ML.NumberElements();
 pickUp(MFList,RecList);
                                                 // for FL's storage only
// ML.SetStorageNumbers();                   // machines before feats
// FL.SetStorageNumbers();

 pSD->uTotalMachs = ML.NumberElements();
 pSD->uTotalFeats = MFList.NumberElements();
 pSD->uTotalCFRecs = RecList.NumberElements();
                                                 // build packlist
 pPL1 = pPL2 = (PackList *)BufK;
 pPL2->Offset = 0;
 pPL2->Type = PL_SIGNATURE;
 pPL2->LRecl = sizeof(*pPL1);
   {
    pPL2++;
    pPL2->Type = PL_SYSDATA;
    pPL2->LRecl = sizeof(*pSD);
    pPL2->Count = 1;

    pPL2++;
    pPL2->Type = PL_MACHINES;
    pPL2->LRecl = ML.fWriteBLength();
    pPL2->Count = 1;

    pPL2++;
    pPL2->Type = PL_MFEATS;
    pPL2->LRecl = MFList.LRecl();
    pPL2->Count = MFList.NumberElements();

    pPL2++;
    pPL2->Type = PL_CFRECS;
    pPL2->LRecl = RecList.fWriteBLength();
    pPL2->Count = 1;

    pPL2++;
    pPL2->Type = PL_XPLNS;
    pPL2->LRecl = XPL.fWriteBLength();
    pPL2->Count = 1;

    pPL2++;
    pPL2->Type = PL_RPO2S;
    pPL2->LRecl = sizeof(RPO2);
    pPL2->Count = RPOL.numberOfElements();

#if 0
     pPL2++;
     pPL2->Type = PL_FEATS;
     pPL2->LRecl = sizeof(Feat);
     pPL2->Count = FL.numberOfElements();
#endif

    pPL2++;
                                                 // System-specific
    pPL2->Type = PL_MDBFEATDESC;                 // descriptions
    pPL2->LRecl = MDBFeatDescL.LRecl();
    pPL2->Count = MDBFeatDescL.NumberElements();

    pPL2++;
    pPL2->Type  = PL_L55;                        // Placements
    pPL2->LRecl = L55.fWriteBLength();
    pPL2->Count = 1;

    pPL2++;
    pPL2->Type  = PL_L56;                        // Connections
    pPL2->LRecl = L56.fWriteBLength();
    pPL2->Count = 1;

    pPL2++;
    pPL2->Type = PL_SWDISCREPS;                  // Software discrepancies
    pPL2->LRecl = SWDiscList.fWriteBLength();
    pPL2->Count = 1;

    pPL2++;
    pPL2->Type = PL_L10_AAS;                     // AAS Subsystem Records
    pPL2->LRecl = L10[0].fWriteBLength();
    pPL2->Count = 1;

    pPL2++;
    pPL2->Type = PL_L10_MRDB;                    // MRDB Subsystem Records
    pPL2->LRecl = L10[1].fWriteBLength();
    pPL2->Count = 1;

    pPL2++;
    pPL2->Type = PL_L05_AAS;                     // AAS Comments
    pPL2->LRecl = L05[0].fWriteBLength();
    pPL2->Count = 1;

    pPL2++;
    pPL2->Type = PL_L05_MRDB;                    // MRDB Comments
    pPL2->LRecl = L05[1].fWriteBLength();
    pPL2->Count = 1;

    pPL2++;
    pPL2->Type = PL_L52_AAS;                     // AAS Comments
    pPL2->LRecl = L52[0].fWriteBLength();
    pPL2->Count = 1;

    pPL2++;
    pPL2->Type = PL_L52_MRDB;                    // MRDB Comments
    pPL2->LRecl = L52[1].fWriteBLength();
    pPL2->Count = 1;
   }

 pPL1->Count = pPL2 - pPL1 + 1;                  // number of packing lists
 for (i = 1, pPL2 = pPL1+1 , j = pPL1->Count; i < j ; i++,pPL1++,pPL2++)
                                                 // set offsets
    pPL2->Offset = pPL1->Offset + (pPL1->Count * pPL1->LRecl);
 pPL2 = (PackList *)BufK;                        // go back to frist element
                                                 // set filesize
 pPL2->Offset = pPL1->Offset + (pPL1->Count * pPL1->LRecl);

 FILE *f;                                        // write to file
 Boolean fOK = false;
// if (f=fopen(MakeFileName(BufF,findFileName(szFileName),"$$$"),"wb"))
 if (f=fopen(szFileName,"wb")) {
   fwrite(BufK,(++pPL2)->Offset,1,f);            // write packing list array
   fwrite(pSD,sizeof(*pSD),1,f);                 // System Data structure
      {                                          // the records lists
        fOK = (
               ML.fWriteB(f)                   &&
               MFList.WriteToFile(f)           &&
               RecList.fWriteB(f)              &&
               XPL.fWriteB(f)                  &&
               RPOL.writeToFile(f)             &&
//               FL.writeToFile(f)               &&
               MDBFeatDescL.WriteToFile(f)     &&
               L55.fWriteB(f)                  &&
               L56.fWriteB(f)                  &&
               SWDiscList.fWriteB(f)           &&
               L10[0].fWriteB(f)               &&
               L10[1].fWriteB(f)               &&
               L05[0].fWriteB(f)               &&
               L05[1].fWriteB(f)               &&
               L52[0].fWriteB(f)               &&
               L52[1].fWriteB(f)
              );
      }
   fclose(f);
   }
 else tellem.say(true,EUO_MSG_ERR_OPEN_FILE,szFileName);
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    void pickUp(MFeatList & FList, CFRecVL & RecList)                    |
    |       Accumulate all MFeat objects and CFReport records                 |
    |        throughout the System in the given lists, as follows:            |
    |                                                                         |
    |       For all the System's machines, collect their CFReport             |
    |        records, their features and their features' CFReport             |
    |        records in the given lists.                                      |
    |                                                                         |
    |       Add the System's CFReport records to the List                     |
    |         after 'stamping' them with consecutive numbers,                 |
    |         the first of which is the number of elements in the             |
    |         list plus 1.                                                    |
    |       Retain the first number and the number of features                |
    |         in a CFRStore object such that we may locate the                |
    |         features when the System is rebuild.                            |
    \*-----------------------------------------------------------------------*/
void RMSystem::pickUp(MFeatList & FeatList, CFRecVL & RecList)
{
 int i,j,k;
 for ( i = 0 , j = ML.NumberElements(); i < j ; i++)
   ML.element(i)->pickUp(FeatList,RecList);

// append the system's additional  CFRecords
 CFRecV * pRec;
 unsigned RecNumber = RecList.NumberElements() + 1 ;
 for ( i = 0; i < SYSTEM_NMBR_CFR_LISTS ; i++ )
   if (k = pSD->CFRStoreInfo[i].qty = CFRLists[i].NumberElements())
     for (j = 0; j < k ; j++) {
       if (!j)
         pSD->CFRStoreInfo[i].RecNmbr = RecNumber;
       pRec = CFRLists[i].element(j);
       pRec->setId(RecNumber++);
       RecList.AddasLast(pRec);
       }
   else pSD->CFRStoreInfo[i].RecNmbr = 0;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean rebuildFL(FeatListMem & FLM)                                 |
    |       Rebuild the System's FeatList from the given                      |
    |         list which was read from a saved-System-file.                   |
    |       Feat objects contain pointers to Machine and MFeat objects.       |
    |       The following will re-set those pointers:                         |
    |                                                                         |
    |        For every element in the given list:                             |
    |          Determine the Machine and Feature storage number.              |
    |          Find the machine via this number in the Machine List.          |
    |          If there is no Feature Number, it's a featureless machine      |
    |          Otherwise, locate the feature in the machine's list.           |
    |          Set the element's pointers and add it to the list.             |
    \*-----------------------------------------------------------------------*/
Boolean  RMSystem :: rebuildFL(FeatListMem & FLM)
{
 unsigned Mnmbr,Fnmbr,i,I,k,K,n,N;
 Boolean fFound = true;
 Feat * pF;
 for (n = 0,N = FLM.NumberElements(); n < N ; n++)
   if (pF = (Feat *)FLM.Element(n)) {
     Fnmbr = pF->storageNumber();
     Mnmbr = pF->machineStorageNumber();
     for (fFound=0,i = 0,I = ML.NumberElements(); !fFound && (i < I) ; i++)
       if ( pAMachine = ML.element(i) )
         if (pAMachine->number() == Mnmbr)
           if (!Fnmbr) {
             fFound = true;
             pF->setPointers(pAMachine,0);
             }
           else for (k = 0, K = pAMachine->mfeatList().NumberElements();
                     !fFound &&  (k < K) ; k++)
                  if (pAFeature = pAMachine->mfeatList().element(k))
                    if (fFound = (pAFeature->number() == Fnmbr))
                      pF->setPointers(pAMachine,pAFeature);
      (fFound && FL.add(*pF));
     }
 return fFound;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean reBuildState( const char * szFileName)                        |
    |       Restore the Product Knowledge's Lists from the                    |
    |       given file to have been written by the 'saveState()' method.      |
    |                                                                         |
    |       Information about the file and every section of records           |
    |       it contains, is stored in an array of 'packing lists'             |
    |         as defined in EUODEF.H :                                        |
    |           struct PackList{ unsigned   Type;   // signature              |
    |                            unsigned   LRecl;  // record length          |
    |                            unsigned   Count;  // record count           |
    |                            unsigned   Offset; // file offset            |
    |                          };                                             |
    |       This array is written out at the top of the file with its         |
    |         first element containing the file's data as follows:            |
    |                        Count :  packing list array count                |
    |                        Offset:  file size                               |
    |                        LRecl :  packing list length                     |
    |                                                                         |
    |       Read the packing list array, edit it and, with the data           |
    |         for the file's record segments which starts with the            |
    |         second array element, read the System Data structure            |
    |         and call the lists' file read methods.                          |
    |                                                                         |
    |       Have the elements in the Machine List rebuild their               |
    |         features and re-distribute the CFReport Records troughout.      |
    |       Re-generate the Feature and Backlog Lists.                        |
    |       Use the AAS filename from the System Data just read with          |
    |         an 'MSG' extension as the new system's Message File.            |
    |         Hand over the System Dtat's Message String File                 |
    |         and the Message File to the Tellem object, since it             |
    |         handles the retrieval, formatting and writing of                |
    |         messages.                                                       |
    |       Remove the given file.                                            |
    \*-----------------------------------------------------------------------*/
Boolean RMSystem::reBuildState( const char * szFileName)
{
 Boolean fOK;
 FILE *f;
 MFeatList FList;
 CFRecVL RList;
 FeatListMem  FLM;
 unsigned i;
 PackList * pPL = (PackList *)BufK;
 int rc = 0;

 release();
 if (!(f = fopen(szFileName,"rb")))
// if (!(f=fopen(MakeFileName(BufF,szFileName,"$$$"),"rb")))
   return tellem.say(true,EUO_MSG_NO_FILE,szFileName);
 i = _filelength(_fileno(f));
 if ( i >= sizeof(*pPL))                         // at least a packing list
   if (fread(BufK,sizeof(*pPL),1,f) == 1)        // get first packing list
     if ( PL_SIGNATURE == pPL->Type)             // check signature
       if ( i == pPL->Offset) {                  // check file size
         fseek(f,0,SEEK_SET);                    // to top of file
                                                 // read array
         if (fread(BufK,pPL->Count * pPL->LRecl,1,f) == 1) {
                                                 // for all stored segments
           for (fOK = true,i = 1 ; fOK && !rc && i < pPL->Count; i++)
             switch ( (pPL+i)->Type ) {
               case PL_SYSDATA:
                 if (fOK = (fread(pSD,(pPL+i)->LRecl,1,f) == 1))
                   (pPL+i)->Type = 0;            // check this one off
                 break;
               case PL_MACHINES:
                 if (fOK = (ML.fReadB_rc(f) == PARR_OK))
                   (pPL+i)->Type = 0;            // check this one off
                 break;
               case PL_MFEATS:
                 if (fOK = (FList.fReadB(f,(pPL+i)->Count) == PARR_OK))
                   (pPL+i)->Type = 0;            // check this one off
                 break;
               case PL_CFRECS:
                 if (fOK = RList.fReadB(f)) {
                   (pPL+i)->Type = 0;            // check this one off
                   RList.sort();
                   }
                 break;
               case PL_XPLNS:
                 if (fOK = (XPL.fReadB(f) == PARR_OK))
                   (pPL+i)->Type = 0;            // check this one off
                 break;
               case PL_RPO2S:
                 if (fOK = RPOL.loadFile(f,(pPL+i)->Count))
                   (pPL+i)->Type = 0;            // check this one off
                 break;
#if 0
               case PL_FEATS:
                 if (fOK = FL.loadFile(f,FLM,(pPL+i)->Count))
                   (pPL+i)->Type = 0;
                 break;
#endif
               case PL_MDBFEATDESC:
                 if (fOK = MDBFeatDescL.fReadB(f,(pPL+i)->Count))
                   (pPL+i)->Type = 0;            // check this one off
                 break;
               case PL_L55:
                 if (fOK = L55.fReadB(f))
                   (pPL+i)->Type = 0;            // check this one off
                 break;
               case PL_L56:
                 if (fOK = L56.fReadB(f))
                   (pPL+i)->Type = 0;            // check this one off
                 break;
               case PL_SWDISCREPS:
                 if (fOK = (SWDiscList.fReadB(f) == PARR_OK))
                   (pPL+i)->Type = 0;            // check this one off
                 break;
               case PL_L10_AAS:
                 if (fOK = L10[0].fReadB(f))
                   (pPL+i)->Type = 0;            // check this one off
                 break;
               case PL_L10_MRDB:
                 if (fOK = L10[1].fReadB(f))
                   (pPL+i)->Type = 0;            // check this one off
                 break;
               case PL_L05_AAS:
                 if (fOK = L05[0].fReadB(f))
                   (pPL+i)->Type = 0;            // check this one off
                 break;
               case PL_L05_MRDB:
                 if (fOK = L05[1].fReadB(f))
                   (pPL+i)->Type = 0;            // check this one off
                 break;
               case PL_L52_AAS:
                 if (fOK = L52[0].fReadB(f))
                   (pPL+i)->Type = 0;            // check this one off
                 break;
               case PL_L52_MRDB:
                 if (fOK = L52[1].fReadB(f))
                   (pPL+i)->Type = 0;            // check this one off
                 break;

               default : rc = RBS_ERR_UNKNOWN_TYPE;
               }
                                                 // got-em all?
           for (i = 1 ;!rc &&  i < pPL->Count; i++)
             if ((pPL+i)->Type)
               rc = RBS_ERR_GOTEM_ALL + i;
           }
         else rc = RBS_ERR_READ_FILE;
         }
       else rc =  RBS_ERR_DAMAGED_FILE;
     else rc = RBS_ERR_NO_EUOP_FILE;
   else rc = RBS_ERR_READ_FILE;
 else rc = RBS_ERR_NO_EUOP_FILE;
 fclose(f);

// if (!rc && (!delivery(FList,RList) || !rebuildFL(FLM)))
 if (!rc && !delivery(FList,RList))
   rc = RBS_ERR_INTERNAL;

 if (rc) {
   ML.Release();
   REPORT_ERROR(rc);
   }
 else {
   strcpy(BufF,pSD->InFiles[0]);
   tellem.set(pSD->MsgFile,SetFileExtension(BufF,MSGFILEEXT));

   /*F72273 This call to method had to be changed because it expects the  */
   /*parameter bAS4 wich is the flag that indicates if this configuration */
   /*is iSeries or not.             */
   /*Begin logic   HECC: 07/10/2002 */
   /* generateFeatList();           */
   generateFeatList( false );


   generateBacklogList();
   remove(BufF);
   }
 return ( rc == 0);
}

void  RMSystem :: XPLOut()
{
 XPL.out(cout);
}

    /*-----------------------------------------------------------------------*\
    |    Boolean floatFirstMRDBMachine()                                      |
    |       Float the first-read MRDB machine to the top of                   |
    |        Machine List                                                     |
    \*-----------------------------------------------------------------------*/
Boolean RMSystem::floatFirstMRDBMachine()
{
 int i,I;
// FL.removeAll();
 for ( i = 0 , I = ML.NumberElements(); i < I ; i++)
   if ( (pAMachine = ML.element(i) ) && pAMachine->isFirstMRDB()) {
     if (i) {                                    // if not -> already on top
       ML.DeletePtr(pAMachine);
       ML.Push(pAMachine);
       }
     return true;
     }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    void generateFeatList()                                              |
    |       Add all features whose machine is tagged to be processed or       |
    |        featureless machines tagged as such to the Feature collection    |
    |        and sort it.                                                     |
    |                                                                         |
    |    Note:                                                                |
    |    F72273:  This function was changed to receive the bAS4 flag as a     |
    |             parameter.                                                  |
    |             Also, the 'if' condition was changed so the SW products     |
    |             wich TAG is set to 'SOFTWARE' and belongs to an iSeries     |
    |             configuration (flag bAS4 set to TRUE) be processed too      |
    |             by this function.                      HECC: 07/05/2002     |
    |                                                                         |
    \*-----------------------------------------------------------------------*/
void RMSystem::generateFeatList( Boolean bAS4 )  /* F72273 Receive the bAS4 flag parameter */
{
 int i,I,k,K,id = 1;
 FL.removeAll();
 for ( i = 0 , I = ML.NumberElements(); i < I ; i++)
   if ( (pAMachine = ML.element(i))  &&
        ( (pAMachine->tag() == Machine::Process) ||
          (pAMachine->tag() == Machine::Software && bAS4) || //ZOR 86752 /* F72273 To process SW machines wich TAG = Software  */
          (pAMachine->tag() == Machine::Process_IS))) // F86752 to include process_IS 1373 /* F72273 To process SW machines wich TAG = Software  */
                                                             /* and bAS4 is TRUE (iSeries configuration)    (HECC) */
     if ( K = pAMachine->mfeatList().NumberElements())
       for (k = 0; k < K ; k++)
         FL.add(Feat(pAMachine,pAMachine->mfeatList().element(k),id++));
     else FL.add(Feat(pAMachine,0,id++));        // featurless machine
 FL.SortUp();
}

    /*-----------------------------------------------------------------------*\
    |    void generateBacklogList()                                           |
    |       Add all features whose machine is tagged as backlog or            |
    |        featureless machines tagged as such to the Backlog collection    |
    |        and sort it.                                                     |
    \*-----------------------------------------------------------------------*/
void RMSystem::generateBacklogList()
{
 int i,I,k,K;
 L19_50.removeAll();
 for ( i = 0 , I = ML.NumberElements(); i < I ; i++)
   if ( (pAMachine = ML.element(i) ) &&
        ( (pAMachine->tag() == Machine::Backlog_HW) ||
          (pAMachine->tag() == Machine::Backlog_SW) ) )
     if ( K = pAMachine->mfeatList().NumberElements())
       for (k = 0; k < K ; k++)
         L19_50.add(BLFeat(pAMachine,pAMachine->mfeatList().element(k)));
     else L19_50.add(BLFeat(pAMachine,0));       // featureless machine
 L19_50.Sort();
}

    /*-----------------------------------------------------------------------*\
    |    Boolean delivery(MFeatList & FList, CFRecVL & RecList)               |
    |       Have the elements in the Machine List rebuild their               |
    |         features and re-distribute their CFReport Records               |
    |         via their own delivery() method.                                |
    |                                                                         |
    |         Restore the object's records from the given Record List,        |
    |         using the storage info retained in the CFRStoreInfo             |
    |          (the first record number and the number                        |
    |           of records were established and stored by the 'pickUp()'      |
    |           method called when the System was saved to a file.            |
    |           Storage occupied by these records was obtained                |
    |           when the given list was built, and we're adding               |
    |           only the pointers to the list)                                |
    \*-----------------------------------------------------------------------*/
Boolean  RMSystem :: delivery(MFeatList & FeatList, CFRecVL & RecList)
{
 int i,j,k;
 Boolean fOK = true;
 CFRecV *pRec;
 for ( i = 0 , j = ML.NumberElements(); fOK  && (i < j) ; i++)
    fOK = ML.element(i)->delivery(FeatList,RecList);

                                                 // restore the system's
                                                 // additional CFRecords
 for ( i = 0; fOK && i < SYSTEM_NMBR_CFR_LISTS ; i++ )
   for (j = 0,k = pSD->CFRStoreInfo[i].RecNmbr; fOK && j < pSD->CFRStoreInfo[i].qty; j++,k++)
     if (fOK = ((pRec = RecList.find(k)) != 0))
       CFRLists[i].AddasLast(pRec);
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    void applyPK()                                                       |
    |       Have all machines to be processed be the benificiary              |
    |         of Product Knowledge.                                           |
    \*-----------------------------------------------------------------------*/
void RMSystem::applyPK()
{
 for (int  i =  0, I = ML.NumberElements(); i < I ; i++)
   if ((pAMachine = ML.element(i)) && (pAMachine->tag() == Machine::Process) )
     pAMachine->applyPK(PK);
}

    /*-----------------------------------------------------------------------*\
    |    void resolveOthers()                                                 |
    |       The last 'resolve' method, handling those items leftover.         |
    |          Process those machines marked 'AAS-Passthru                    |
    |             (these are machines from the AAS CFReport                   |
    |              which are not to be used in the resolution process         |
    |              and are to be passed through as 'resolved'                 |
    \*-----------------------------------------------------------------------*/

//F72273 Include bAS4 flag and iSeries SW features table
//to reconcile iSeries SW.                       HECC  07/02/2002
void RMSystem::resolveOthers( Boolean bAS4, ISortedSet<IString>* pISeriesSWTable )
{

 int i,I,k,K;
 for ( i =  0, I = ML.NumberElements(); i < I ; i++)
   if (pAMachine = ML.element(i))
     switch(pAMachine->tag()) {
        case Machine::AAS_Passthru : pAMachine->processAASPassthru(); break;
        case Machine::Software:

          /* F72273: Logic added, in case the read machine is a Software   */
          /* product of an iSeries configuration, process the conciliation */
          /* HECC   Begin: 07/10/2002 */
          if (bAS4)
             resolveSoftwareForISeries(pISeriesSWTable);
          else
          /* End: 07/10/2002 */
             {
             for (k = 0, K = pAMachine->mfeatList().NumberElements(); k < K ; k++)
               if (pAFeature = pAMachine->mfeatList().element(k))
                 pAFeature->setBase(pAFeature->qty(MFeat::AASType));
             }
          break;
        case Machine::Process:
         pAMachine->consumeUnresolveds();
         break;

        /* F86750  Begin logic: For services type-model products simply change */
        /* the status to "Resolved" so they can be written to the .RET file  */
        case Machine::Services:
         pAMachine->resolveServicesRecs();
         break;
        /* F86750  End logic: HECC: 01/13/2004 */

        }
}

    /*-----------------------------------------------------------------------*\
    |    void resolveSoftwareForISeries(pISeriesSWTable)                      |
    |       The last 'resolve' method, handling those items leftover.         |
    |          Process those machines marked 'AAS-Passthru                    |
    |             (these are machines from the AAS CFReport                   |
    |              which are not to be used in the resolution process         |
    |              and are to be passed through as 'resolved'                 |
    \*-----------------------------------------------------------------------*/
//F72273.   HECC  07/02/2002
void RMSystem::resolveSoftwareForISeries( ISortedSet<IString>* pISeriesSWTable )
   {

   int i,I,j,J,k,K,idMsg,cntFeat;
   IString pMsg;
   IString strType, strModel, strFeat;
   Boolean bIsSWFeatTbl = false;
   unsigned AASQty,MRQty;

   if (pAMachine->tag() == Machine::Software)
      {
      cntFeat = pAMachine->mfeatList().NumberElements();

      if (cntFeat == 0)
         {
         /* If the actual SW product is a featureless machine */

         idMsg = EUO_MSG_NOFEATMACH_MRDB_Y_AAS_N;
         tellem.say(false,idMsg, pAMachine->type(),pAMachine->model(),
                    pAMachine->serialNo(), pAMachine->description());
         pMsg = IString( tellem.message(idMsg,pAMachine->type(),
                                        pAMachine->model(),pAMachine->serialNo(),
                                        pAMachine->description()) );
         //pAMachine->copyRecords(pAMachine,false,true,          //F73732 EDW 07/25/02
         //                       version(false),false,false);   //F73732 EDW 07/25/02
         if (pMsg.size())
            {
            XPL.add(XPLN2(pAMachine->type(),pAMachine->model(),
                          pAMachine->serialNo(),0,(const char *)(pMsg)));
            pAMachine->setMsgInd();
            pMsg = "";
            }
         }

      for (k = 0; k < cntFeat ; k++)
         {
         pAFeature = pAMachine->mfeatList().element(k);
         AASQty = pAFeature->qty(MFeat::AASType);
         MRQty = pAFeature->qty(MFeat::MRDBType);

         /* F73732 EDW 7/25/2002 - First, make sure all sw features have
            descriptions set on both sides
         */
         IString str;
         if (pAFeature->qty(MFeat::MRDBType))
            {
            if (!pAFeature->hasDescription(true))
               if (pAFeature->isGeneric() &&
                   PK.strGenericDescription(pAMachine->type(),
                                            pAMachine->model(),pAFeature->feature(),str))
                  pAFeature->setDescription((char *)str);
               else
                  pAFeature->setDescription(pAFeature->hasDescription(false)?
                                            pAFeature->description(false) :
                                            PK.description(pAMachine->type(),
                                                 pAMachine->model(),pAFeature->feature()));
            }
         if (pAFeature->qty(MFeat::AASType) && !pAFeature->hasDescription(false))
            pAFeature->setDescription(pAFeature->hasDescription(true)?
                                      pAFeature->description(true) :
                                      PK.description(pAMachine->type(),pAMachine->model(),
                                                     pAFeature->feature()),false);

         /* F73732 EDW 07/25/2002 - Apply iSeries SW reconciliation rules */
         /* F73732 EDW 07/25/2002 - Product record reported in MR and AAS (RULE #1) */
         IString strDesc = IString::stripBlanks(IString(pAFeature->description()));
         if (pAMachine->hasAASData() && pAMachine->hasMRDBData())
            {

            /* If the feature exists in MR and AAS reconcile to MR quantity
             */
            if ( (MRQty>0) && (AASQty>0) )
               {
               idMsg = EUO_MSG_ISERIES_MRDB_Y_AAS_Y_R1;
               pMsg = IString( tellem.message(idMsg, pAFeature->feature(),strDesc,
                (char *)IString(pAFeature->qty(MFeat::MRDBType)) ) );
               pAFeature->setBase(pAFeature->qty(MFeat::MRDBType));
               pAFeature->setTag(MFeat::ISeriesSWTblResolved);
               }
            /* If the feature exists in MR but not in AAS reconcile to MR quantity
             */
            else if ( (MRQty>0) && !(AASQty>0) )
               {
               idMsg = EUO_MSG_ISERIES_MRDB_Y_AAS_N_R1;
               pMsg = IString( tellem.message(idMsg, pAFeature->feature(),strDesc,
                (char *)IString(pAFeature->qty(MFeat::MRDBType)) ) );
               pAFeature->setBase(pAFeature->qty(MFeat::MRDBType));
               pAFeature->setTag(MFeat::ISeriesSWTblResolved);
               }
            /* If the feature exists in AAS but not in MR reconcile to AAS quantity
             */
            else //if ( !(MRQty>0) && (AASQty>0) )
               {
               idMsg = EUO_MSG_ISERIES_MRDB_N_AAS_Y_R1;
               pMsg = IString( tellem.message(idMsg, pAFeature->feature(),strDesc,
                (char *)IString(pAFeature->qty(MFeat::AASType)) ) );
               pAFeature->setBase(pAFeature->qty(MFeat::AASType));
               pAFeature->setTag(MFeat::ISeriesSWTblResolved);
               }

            tellem.say(false,idMsg, pAMachine->type(),pAMachine->model(),
                       pAMachine->serialNo(), pAMachine->description());
            //pAMachine->copyRecords(pAMachine,false,true,          //F73732 EDW 07/25/02
            //                       version(false),false,false);   //F73732 EDW 07/25/02
            if (pMsg.size())
               {
               XPL.add(XPLN2(pAMachine->type(),pAMachine->model(),
                             pAMachine->serialNo(),pAFeature->feature(),
                             (const char *)(pMsg)));
               pAFeature->setMsgInd(true);
               pMsg = "";
               }
            }

         /* F73732 EDW 07/25/2002 - Product record reported in MR but not in AAS (RULE #2) */
         else if (!pAMachine->hasAASData() && pAMachine->hasMRDBData())
            {
            /* Reconcile all these features using MR quantity */
            idMsg = EUO_MSG_ISERIES_MRDB_Y_AAS_N_R2;
            tellem.say(false,idMsg, pAMachine->type(),pAMachine->model(),
                       pAMachine->serialNo(), pAMachine->description());
            pMsg = IString( tellem.message(idMsg,
                                           pAFeature->feature(),strDesc,
                                           (char *)IString(pAFeature->qty(MFeat::MRDBType)) ) );
            //pAMachine->copyRecords(pAMachine,false,true,         //F73732 EDW 07/25/02
            //                       version(false),false,false);  //F73732 EDW 07/25/02
            pAFeature->setBase(pAFeature->qty(MFeat::MRDBType));
            pAFeature->setTag(MFeat::ISeriesSWTblResolved);

            if (pMsg.size())
               {
               XPL.add(XPLN2(pAMachine->type(),pAMachine->model(),
                             pAMachine->serialNo(),pAFeature->feature(),
                             (const char *)(pMsg)));
               pAFeature->setMsgInd(true);
               pMsg = "";
               }
            }

         /* F73732 EDW 07/25/2002 - Product record reported in AAS
            but not in MR (RULES 3 & 4) */
         else
            {
            /* Search for the feature in the SW exception table */
            pAFeature = pAMachine->mfeatList().element(k);
            strType = pAMachine->type();
            strModel = pAMachine->model();
            strFeat = pAMachine->mfeatList().element(k)->feature();

            if (strFeat == "")
               strFeat = "****";

            IString strTypeFeat1 = strType + "*** " + strFeat + "    ";
            IString strTypeFeat2 = strType + "**"   + strModel.subString(3, 1) + " "
                                   + strFeat + "    ";
            IString strTypeFeat3 = strType + "*" + strModel.subString(2, 1) + "*"
                                   + " "    + strFeat + "    ";
            IString strTypeFeat4 = strType + "*" + strModel.subString(2, 2) + " "
                                   + strFeat + "    ";
            IString strTypeFeat5 = strType + strModel.subString(1, 1) + "** "
                                   + strFeat + "    ";
            IString strTypeFeat6 = strType + strModel.subString(1, 1) + "*"
                                   + strModel.subString(3, 1) + " " + strFeat + "    ";
            IString strTypeFeat7 = strType + strModel.subString(1, 2) + "* "
                                   + strFeat + "    ";
            IString strTypeFeat8 = strType + strModel + " " + strFeat + "    ";

            IString strTypeFeat9 = strType + strModel + " ****    ";
            IString strTypeFeat10 = strType + strModel + " ***"
                                    + strFeat.subString(4, 1) + "    ";
            IString strTypeFeat11 = strType + strModel + " **"
                                    + strFeat.subString(3, 1) + "*    ";
            IString strTypeFeat12 = strType + strModel + " **"
                                    + strFeat.subString(3, 2) + "    ";
            IString strTypeFeat13 = strType + strModel + " *"
                                    + strFeat.subString(2, 1) + "**    ";
            IString strTypeFeat14 = strType + strModel + " *"
                                    + strFeat.subString(2, 1) + "*"
                                    + strFeat.subString(4, 1) + "    ";
            IString strTypeFeat15 = strType + strModel + " *"
                                    + strFeat.subString(2, 2) + "*    ";
            IString strTypeFeat16 = strType + strModel + " *"
                                    + strFeat.subString(2, 3) + "    ";
            IString strTypeFeat17 = strType + strModel + " "
                                    + strFeat.subString(1, 1) + "***    ";
            IString strTypeFeat18 = strType + strModel + " "
                                    + strFeat.subString(1, 1) + "**"
                                    + strFeat.subString(4, 1) + "    ";
            IString strTypeFeat19 = strType + strModel + " "
                                    + strFeat.subString(1, 1) + "*"
                                    + strFeat.subString(3, 1) + "*    ";
            IString strTypeFeat20 = strType + strModel + " "
                                    + strFeat.subString(1, 1) + "*"
                                    + strFeat.subString(3, 2) + "    ";
            IString strTypeFeat21 = strType + strModel + " "
                                    + strFeat.subString(1, 2) + "**    ";
            IString strTypeFeat22 = strType + strModel + " "
                                    + strFeat.subString(1, 2) + "*"
                                    + strFeat.subString(4, 1) + "    ";
            IString strTypeFeat23 = strType + strModel + " "
                                    + strFeat.subString(1, 3) + "*    ";
            IString strTypeFeat24 = strType + strModel + " "
                                    + strFeat + "    ";

            if ( pISeriesSWTable->contains( strTypeFeat1 ) ||
                 pISeriesSWTable->contains( strTypeFeat2 ) ||
                 pISeriesSWTable->contains( strTypeFeat3 ) ||
                 pISeriesSWTable->contains( strTypeFeat4 ) ||
                 pISeriesSWTable->contains( strTypeFeat5 ) ||
                 pISeriesSWTable->contains( strTypeFeat6 ) ||
                 pISeriesSWTable->contains( strTypeFeat7 ) ||
                 pISeriesSWTable->contains( strTypeFeat8 ) ||
                 pISeriesSWTable->contains( strTypeFeat9 ) ||
                 pISeriesSWTable->contains( strTypeFeat10 ) ||
                 pISeriesSWTable->contains( strTypeFeat11 ) ||
                 pISeriesSWTable->contains( strTypeFeat12 ) ||
                 pISeriesSWTable->contains( strTypeFeat13 ) ||
                 pISeriesSWTable->contains( strTypeFeat14 ) ||
                 pISeriesSWTable->contains( strTypeFeat15 ) ||
                 pISeriesSWTable->contains( strTypeFeat16 ) ||
                 pISeriesSWTable->contains( strTypeFeat17 ) ||
                 pISeriesSWTable->contains( strTypeFeat18 ) ||
                 pISeriesSWTable->contains( strTypeFeat19 ) ||
                 pISeriesSWTable->contains( strTypeFeat20 ) ||
                 pISeriesSWTable->contains( strTypeFeat21 ) ||
                 pISeriesSWTable->contains( strTypeFeat22 ) ||
                 pISeriesSWTable->contains( strTypeFeat23 ) ||
                 pISeriesSWTable->contains( strTypeFeat24 )    )
               bIsSWFeatTbl = true;
            else
               bIsSWFeatTbl = false;

            /* if the feature is in AAS, but not in MR and the feature
               exists in the SW features table, resolve using AAS quantity (RULE #4)
            */
            if ( (MRQty==0) &&
                 (AASQty>0) &&
                 (bIsSWFeatTbl)  )
               {
               idMsg = EUO_MSG_ISERIES_MRDB_N_AAS_Y_TBL_Y_R4;
               tellem.say(false,idMsg, pAMachine->type(),pAMachine->model(),
                          pAMachine->serialNo(), pAMachine->description());
               pMsg = IString( tellem.message(idMsg,
                                              pAFeature->feature(),strDesc,
                                              (char *)IString(pAFeature->qty(MFeat::AASType)) ) );

               pAFeature->setBase(pAFeature->qty(MFeat::AASType));
               pAFeature->setTag(MFeat::ISeriesSWTblResolved);

               if (pMsg.size())
                  {
                  XPL.add(XPLN2(pAMachine->type(),pAMachine->model(),
                                pAMachine->serialNo(),pAFeature->feature(),
                                (const char *)(pMsg)));
                  pAFeature->setMsgInd(true);
                  pMsg = "";
                  }
               }

            /* if the feature is in AAS, but not in MR and the feature
               is not in the SW features table, set base to zero and
               leave as a mismatch for user to reconcile manually (RULE #3)
            */
            if ( (MRQty==0) &&
                 (AASQty>0) &&
                 (!bIsSWFeatTbl) )
               {
               /* D74158 EDW 09/26/02 - The explanation message log should only
                  be set when a feature has been automatically resolved.
                  Since in this case, the feature remains unresolved, we should
                  not be attempting to set the message log

               tellem.say(false,idMsg, pAMachine->type(),pAMachine->model(),
                          pAMachine->serialNo(), pAMachine->description());
               pMsg = IString( tellem.message(idMsg,pAMachine->type(),
                                              pAMachine->model(),pAMachine->serialNo(),
                                              pAMachine->description()) );
               */
               pAFeature->setBase(0);
               pAFeature->setTag(MFeat::Unresolved);
               }
            }
         }
      /* F73732 EDW 07/25/02 - begin
         If SW record is not reported in the AAS report, copy
         the records reported in the MRDB to the AAS structures
      */
      if (!pAMachine->hasAASData())
         {
         pAMachine->copyRecords(pAMachine,false,true,
                                version(false),false,false);
         }
      /* F73732 EDW 07/25/02 - begin */
      }
   }


    /*-----------------------------------------------------------------------*\
    |   Boolean check4DifferentModelMachines(XWindow * pXW)                   |
    |                                                                         |
    |       Have the user determine the model number for those                |
    |       machines which appear in opposite files (AAS and MRDB)            |
    |       and who differ in model number only.                              |
    |                                                                         |
    |       Since we're in a thread, the main window will obtain              |
    |       the user's response via its 'WWShowMismatch()' method             |
    |       which displays a window showing a message                         |
    |       stating what model number was encountered in what file            |
    |       and two radio buttons giving the options. The message             |
    |       and radio buttton texts are formulated here with the              |
    |       help of Message String File entries.                              |
    |       From this window, the user is also able to cancel which           |
    |       should result in terminating this system's resolution.            |
    |                                                                         |
    |       For every machine to be processed which appeared in               |
    |       one file but not the other:                                       |
    |         If another machine is found with the same type and serial#      |
    |          but different model:                                           |
    |            Determine what model number appeared in which file and       |
    |              use the Tellem object to format the appropriate message    |
    |              and radio button texts.                                    |
    |            Have the window shown and obtain the reponse:                |
    |              0 : return false indicating the user's desire to cancel    |
    |                  after logging that decision.                           |
    |             1-2: (representing the first or second radio button)        |
    |                  Determine the user's choice of model in such a         |
    |                    way that we'll always keep the machine               |
    |                    from the MRDB file (see below).                      |
    |                  Use both machines' setModel() method to propegate      |
    |                    the new Model name troughout.                        |
    |                  Move the AAS machine's AAS records over and eliminate  |
    |                    it from the Machine List after releasing its         |
    |                    associated storage.                                  |
    |                  Set the flag to start the search for these             |
    |                    types of machine anew, since the array of            |
    |                    Machines has shrunk.                                 |
    \*-----------------------------------------------------------------------*/
Boolean  RMSystem::check4DifferentModelMachines(XWindow * pXW , Boolean b390)
{
 Boolean fAgain = false;
 Machine *pMMatch,* pMTo, *pMFrom;
 IString strAAS("AAS") ,strMRDB("MRDB") ,strText,strF1,strF2,strB1,strB2, strNull;
 strNull = "";  // MAMT 53915 28/02/2001 strNull is also in the above line
 unsigned long ulResponse;
 for ( int i =  0, I = ML.NumberElements(); i < I ; i = (fAgain? 0 : (i+1)) ) {
  fAgain = false;
  if ((pAMachine = ML.element(i)) && (pAMachine->tag() == Machine::Process) )
                                                 // AAS mach without MRDB
    if (!pAMachine->hasMRDBData() ||             // partner
                                                 // MRDB mach without AAS
        !pAMachine->hasAASData() )               // partner
      for (int j =  i + 1; !fAgain && j < I ; j++)
        if ((pMMatch = ML.element(j)) && (pMMatch->tag() == Machine::Process) )
          if (!pAMachine->compareTypeSerial(*pMMatch)) {
            strF1 = IString();
            ulResponse = 0;
                  /*------------------------------------------------*\
                  | Always move data from AAS to MRDB Machine to     |
                  | retain the the machine's PK indicators such as   |
                  | Machine Readable and Generic etc. which are only |
                  | triggered if there is MRDB data                  |
                  \*------------------------------------------------*/
                                                 // mach has AAS but no MRDB
            if (!pAMachine->hasMRDBData()) {     // data
              if (pMMatch->hasMRDBData() &&      // match has MRDB data
                  !pMMatch->hasAASData()) {      // and no AAS data
                strF1 = strAAS,strF2 = strMRDB;
                pMTo = pMMatch, pMFrom = pAMachine;
                }
              }
                                                 // mach has MRDB but no AAS
            else {                               // data
              if (pMMatch->hasAASData() &&       // match has AAS data
                  !pMMatch->hasMRDBData()) {     // and no MRDB data
                strF1 = strMRDB,strF2 = strAAS;
                pMTo = pAMachine, pMFrom = pMMatch;
                }
              }
            if (strF1 != IString()) {
                if (!b390) // MAMT 53915 28/02/2001 include b390 flag for S390
                   {
                    strText = IString( tellem.message(EUO_MSG_MODELS_DIFFER_PICK,
                                       (char *)strF1, pAMachine->type(),
                                       pAMachine->model(),(char *)strF2,
                                       pMMatch->type(),pMMatch->model(),
                                       (char * )pMMatch->displaySerialNoStr()) );//jt
                    strB1 = IString( tellem.message(EUO_MSG_MODELS_DIFFER_PROCEED,
                                       pAMachine->model()) );                     //jt
                    strB2 = IString( tellem.message(EUO_MSG_MODELS_DIFFER_PROCEED,
                                       pMMatch->model()) );                       //jt
                    ulResponse = pXW->WWShowMismatch((char *)strB1,(char *)strB2,
                                                     (char *)strText);
                   }
                else
                   { // MAMT 53915 28/02/2001 begin new code
                    strText = IString( tellem.message(EUO_MSG_MODELS_DIFFER_PICKS390,
                                       (char *)strF1, pAMachine->type(),
                                       pAMachine->model(),(char *)strF2,
                                       pMMatch->type(),pMMatch->model(),
                                       (char * )pMMatch->displaySerialNoStr()) );//jt
                    ulResponse = pXW->WWShowMismatchS390((char *)strText);

//             DWORD DataSize = 256;
             HKEY key;
  if (RegOpenKeyEx (HKEY_CURRENT_USER, REGPATH, 0, KEY_WRITE, &key) == ERROR_SUCCESS)      //48163 - flw
     {
       RegSetValueEx(key, STR_Placement_s390, 0, REG_SZ, (CONST BYTE*)STR_Placement_Ys390, IString(STR_Placement_Ys390).length()+1);
     }
  RegCloseKey (key);

                   } // MAMT 53915 28/02/2001 end new code
              }
            if (!ulResponse)
              if (!b390) // MAMT 53915 28/02/2001 include b390 flag for S390
                 return tellem.say(true,EUO_MSG_MODELS_DIFFER_QUIT,(char *)strF1,
                                pAMachine->type(),pAMachine->model(),
                                (char *)strF2,pMMatch->type(),pMMatch->model(),
                                (char * )pMMatch->displaySerialNoStr());
              else       // MAMT 53915 28/02/2001 to return false value
                 return false;

            IString strNewModel((ulResponse == 1)?
                                       pAMachine->model() : pMMatch->model());
            pMTo->setModel((char *)strNewModel); // be anal retentive: clobber
                                                 // model in both machines
            pMFrom->setModel((char *)strNewModel);

            IString strNewSysId((ulResponse == 1)?                                          //VBB09192002 D74157
                                       pAMachine->getSystemId() : pMMatch->getSystemId());  //VBB09192002 D74157
            pMTo->setSystemId((char *)strNewSysId); // be anal retentive: clobber           //VBB09192002 D74157
                                                    // strSysId in both machines            //VBB09192002 D74157
            pMFrom->setSystemId((char *)strNewSysId);                                       //VBB09192002 D74157

            pMFrom->copyRecords(pMTo,false,false,version(false),true,true);
            pMTo->setSubsystemRecordId(false,pMFrom->subsystemRecordId(false));
            pMFrom->Release();                   // free its resources
            delete pMFrom;
            ML.DeletePtr(pMFrom);                // remove form array
            fAgain = true;
            }
  }
 return true;
}

    /*-----------------------------------------------------------------------*\
    |   void resolveLoneMachines()                                            |
    |      (those appearing in only the AAS or MRDB file                      |
    |       are assumed to be resolved)                                       |
    |                                                                         |
    |       For every machine to be processed:                                |
    |         If an AAS machine has no MRDB partner:                          |
    |           Copy its Machine Record and its 1st-54-Record                 |
    |             from its AAS side to its MRDB side.                         |
    |           If the machine has features:                                  |
    |             Add every feature's AAS Record to its MRDB side,            |
    |             log the assumption and generate a similar explanation.      |
    |           Otherwise, just log and explain.                              |
    |         Otherwise, if an MRDB machine has no AAS partner:               |
    |                      Copy its Machine Record and its 1st-54-Record      |
    |                        from its MRDB side to its AAS side.              |
    |                      If the machine has features:                       |
    |                        Add every feature's MRDB Record to its           |
    |                          AAS side, log the assumption and generate      |
    |                          a similar explanation.                         |
    |                        If the file's version is 20, set the feature's   |
    |                          description from the Product Knowledge since   |
    |                          that version CFReport does not carry feature   |
    |                          descriptions.                                  |
    |                      Otherwise, log, explain and set the Machine's      |
    |                         description similarly and for the same          |
    |                         reasons.                                        |
    |         If an explanation messsage was created:                         |
    |            Set the machine's message indicator, create an               |
    |               explanation with the message and add it to the list.      |
    |                                                                         |
    |         Note: this method  duplicates the the whole machine on the      |
    |               opposite side. Resolution is then achieved by the         |
    |               Machine's resolveMatches() method.                        |
    \*-----------------------------------------------------------------------*/
// new version using the Machine's copyRecords method
void RMSystem::resolveLoneMachines()
{
 int i,I,j,J,k,idMsg,cntFeat;
 IString pMsg;  //JT IMessageText * pMsg;
//jt for ( i =  0, I = ML.NumberElements(),pMsg = 0; i < I ; i++)//,pMsg = 0)
 for ( i =  0, I = ML.NumberElements(); i < I ; i++)
   {
   if ((pAMachine = ML.element(i)) &&
       (pAMachine->tag() == Machine::Process) &&
       (!pAMachine->hasMRDBData() || !pAMachine->hasAASData())) {
     cntFeat = pAMachine->mfeatList().NumberElements();
                                                 // aas mach has no mrdb
     if (!pAMachine->hasMRDBData()) {            // partner
       idMsg = cntFeat? EUO_MSG_MACH_AAS_Y_MRDB_N :
                        EUO_MSG_NOFEATMACH_AAS_Y_MRDB_N;
       tellem.say(false,idMsg, pAMachine->type(),pAMachine->description());
//jt       pMsg = new IMessageText(idMsg,pSD->MsgFile,pAMachine->type(),
       pMsg = IString( tellem.message(idMsg,pAMachine->type(),
                               pAMachine->model(),pAMachine->serialNo()) );
                                                 // AAS -> MRDB
       pAMachine->copyRecords(pAMachine,true,false,version(true),false,false);
       }
                                                 // mrdb mach has no AAS
     else if (!pAMachine->hasAASData()) {        // partner
            idMsg = cntFeat? EUO_MSG_MACH_MRDB_Y_AAS_N :
                             EUO_MSG_NOFEATMACH_MRDB_Y_AAS_N;
            tellem.say(false,idMsg, pAMachine->type(),pAMachine->model(),
                       pAMachine->serialNo(), pAMachine->description());
//jt            pMsg = new IMessageText(idMsg,pSD->MsgFile,pAMachine->type(),
            pMsg = IString( tellem.message(idMsg,pAMachine->type(),
                                    pAMachine->model(),pAMachine->serialNo(),
                                    pAMachine->description()) );
                                                 // MRDB -> AAS
            pAMachine->copyRecords(pAMachine,false,true,version(false),false,
                                   false);
            if (version(true) == 20)
              pAMachine->setDescription(PK.description( pAMachine->type(),
                                                        pAMachine->model())
                                                       ,false);
                                                 // end MRDB mach without
            }                                    // partner

     for (k = 0; k < cntFeat ; k++)
       if ((pAFeature = pAMachine->mfeatList().element(k)) &&
           (J = pAFeature->cfrList(MFeat::MRDBType).NumberElements()))
         for (j = 0; j < J ; j++) {
           pAFeature->setBase(pAFeature->qty(
              (idMsg == EUO_MSG_MACH_AAS_Y_MRDB_N)?
                     MFeat::AASType : MFeat::MRDBType));
           if( XWindow::bShowS390Results )     //edw01/07/2000
              pAFeature->setTag(MFeat::Resolved_EUOP_S390);
           else
              pAFeature->setTag(MFeat::Resolved_EUOP);
           }

     if (pMsg.size()) { //jt
        XPL.add(XPLN2(pAMachine->type(),pAMachine->model(),
                      pAMachine->serialNo(),0,(const char *)(pMsg)));//jt
        pAMachine->setMsgInd();
//jt        delete pMsg;
        pMsg = "";
        }
     }                                           // end machine to be processed
   }
}

    /*-----------------------------------------------------------------------*\
    |    void resolveMatches()                                                |
    |       For all all machines to be processed                              |
    |         Resolve all their features whose AAS and MRDB quantities        |
    |          are equal by marking those features as resolved                |
    |          and setting their base quantities.                             |
    \*-----------------------------------------------------------------------*/
void RMSystem::resolveMatches()
{
 int i,I,j,J;
  for ( i =  0, I = ML.NumberElements(); i < I ; i++)
    if ((pAMachine = ML.element(i)) && (pAMachine->tag() == Machine::Process) )
      for ( j =  0, J = pAMachine->mfeatList().NumberElements(); j < J ; j++)
        pAMachine->mfeatList().element(j)->resolveMatches();
}

    /*-----------------------------------------------------------------------*\
    |    int resolveUsingNoCompareTable()                                     |
    |       For all machines to be processed                                  |
    |         Resolve all their unresolved features by using the S390         |
    |          No Compare table logic.  Mark the features as resolved         |
    |          and set their base quantities.                                 |
    |       No Compare Table logic:                                           |
    |          if ( feature not in No Compare Table )                         |
    |             resolved to MRDB quantity;                                  |
    |          else                                                           |
    |             resolved to AAS quantity;                                   |
    \*-----------------------------------------------------------------------*/
int RMSystem::resolveUsingNoCompareTable( ISortedSet<IString>* pNoCompTable,
    ISortedSet<IString>* pQuickFixTable, ISortedSet<IString>* pFeature54Table )
{                       // 53915 - MAMT - 08/31/2000 include pFeature54Table
 int i,I,j,J;
 Boolean bGenRPO;
 IString strType, strFeat, strModel, strDesc, strSerial;
 unsigned uAASQty, uMRDBQty, uBaseQty ; //49222 - MAMT - 06/27/2000 53915 uMRDBQty
 RPO2 *pRPO;

// begin new logic 49222 - MAMT - 13/08/2000
  for ( i =  0, I = ML.NumberElements(); i < I ; i++)
    if ((pAMachine = ML.element(i)) && (pAMachine->tag() == Machine::Process) )
      for ( j =  0, J = pAMachine->mfeatList().NumberElements(); j < J ; j++)
         {
         strType = pAMachine->type();
         strModel = pAMachine->model();
         pAMachine->mfeatList().element(j)->resolveUsingNoCompareTable1( pNoCompTable, strType, strModel );
         }
// end new logic 49222 - MAMT - 13/08/2000

  for ( i =  0, I = ML.NumberElements(); i < I ; i++)
    if ((pAMachine = ML.element(i)) && (pAMachine->tag() == Machine::Process) )
      for ( j =  0, J = pAMachine->mfeatList().NumberElements(); j < J ; j++)
         {
         pRPO = 0;
         strType = pAMachine->type();
         strModel = pAMachine->model();
         bGenRPO =
         pAMachine->mfeatList().element(j)->resolveUsingNoCompareTable( strType );

         if ( bGenRPO )
            {
            strFeat = pAMachine->mfeatList().element(j)->feature();
            uAASQty = pAMachine->mfeatList().element(j)->qty(MFeat::AASType);
            uBaseQty = pAMachine->mfeatList().element(j)->baseNmbr();

// 49222 - MAMT - 06/27/2000
            uBaseQty = resolveUsingQuickFixTable( pQuickFixTable, strType, strModel, strFeat, uBaseQty );
            pAMachine->mfeatList().element(j)->setBase(uBaseQty);
//            strModel = pAMachine->model();    49222 - MAMT - 06/27/2000
            strDesc = pAMachine->description();
            strSerial = pAMachine->serialNo();
// begin new logic 53915 - MAMT - 08/31/2000
            uMRDBQty = pAMachine->mfeatList().element(j)->qty(MFeat::MRDBType);
           if (uBaseQty < uMRDBQty)
               {
               IString sOutputRec54;
               sOutputRec54 = strType + strModel + strFeat; //+ strSerial;
               pFeature54Table->add(sOutputRec54);
               }
// end new logic 53915 - MAMT - 08/31/2000
            if ( uAASQty != uBaseQty )
               {
               pRPO = new RPO2( strType, strModel, strSerial, strFeat, strDesc,
                               uAASQty > uBaseQty ? uAASQty - uBaseQty : uBaseQty - uAASQty,
                               uAASQty > uBaseQty ? RPO_CODE_DELETE : RPO_CODE_ADD,

                               pAMachine->customerNumber(), //* 83402 HECC: 07/28/2003
                               pAMachine->systNumberFlag(), //* 83402 HECC: 07/28/2003
                               pAMachine->systemType(),     //* 83402 HECC: 07/28/2003
                               pAMachine->systemNum(),      //* 83402 HECC: 07/28/2003

                               pAMachine->description(),    // F73686   HECC: 10/16/2002
                               pAMachine->category() );     // F73686   HECC: 10/16/2002
               RPOL.add( *pRPO );
               delete pRPO;
               }
            }
         }

}

    /*-----------------------------------------------------------------------*\
    |    unsigned resolveUsingQuickFixTable() 49222 - MAMT - 12/07/2000       |
    |       For all machines to be processed                                  |
    |         Decrement the quantity for features that exists in the          |
    |          QuickFix table(coreq --> AAS).  according with the prime       |
    |          (features that exits in MRDB file) that owns to the same table |
    |       Quick Fix table logic:                                            |
    |          case                                                           |
    |            single                                                       |
    |              if quantity(coreq)>quantity(prime) then                    |
    |                 quantity(coreq)=quantity(prime)                         |                                     |
    |              else;                                                      |
    |            multi or                                                     |
    |              if quantity(coreq)>sum_quantity(prime1 to primeN) then     |
    |                 quantity(coreq)=sum_quantity(prime1 to primeN)          |                                     |
    |              else;                                                      |
    |            multi &                                                      |
    |              if quantity(coreq)<quantity(prime1) then                   |
    |                 quantity(coreq)=quantity(prime1)                        |                                     |
    |              else;                                                      |
    \*-----------------------------------------------------------------------*/
unsigned RMSystem::resolveUsingQuickFixTable( ISortedSet<IString>* pQuickFixTable, IString strType, IString strModel, IString strFeat, unsigned uAASQty1 )
{
 int k,K;
 unsigned uMRDBQty, sumMRDBQty;
 IString strDesc, strSerial, QFelement, QFelement1, strTypeFeat1,
 strTypeFeat2, strTypeFeat3, strTypeFeat4, strTypeFeat5, strTypeFeat6,
 strTypeFeat7, strTypeFeat8, typeproc, prime, coreq, coreq1, prime1;

// cases *** , **A , *A* , *AB , A** , A*B , AB* , ABC
// new logic 55982 MAMT 31/10/00 change size of feature to 8 (with spaces) to match with the table
 strTypeFeat1 = strType + "*** " + strFeat.rightJustify(8,' ');
 strTypeFeat2 = strType + "**"   + strModel.subString(3, 1) + " " + strFeat.rightJustify(8,' ');
 strTypeFeat3 = strType + "*"    + strModel.subString(2, 1) + "*"  + " " + strFeat.rightJustify(8,' ');
 strTypeFeat4 = strType + "*"    + strModel.subString(2, 2) + " " + strFeat.rightJustify(8,' ');
 strTypeFeat5 = strType + strModel.subString(1, 1) + "** " + strFeat.rightJustify(8,' ');
 strTypeFeat6 = strType + strModel.subString(1, 1) + "*"
                        + strModel.subString(3, 1) + " "   + strFeat.rightJustify(8,' ');
 strTypeFeat7 = strType + strModel.subString(1, 2) + "* "  + strFeat.rightJustify(8,' ');
 strTypeFeat8 = strType + strModel + " " + strFeat.rightJustify(8,' ');

 for ( k =  1, K = pQuickFixTable->numberOfElements(); k <= K ; k++)
    {
     QFelement = pQuickFixTable->elementAtPosition(k);
     // change size from 12 to 16 55982 MAMT
     if ( QFelement.subString(1,16) == strTypeFeat1 ||
          QFelement.subString(1,16) == strTypeFeat2 ||
          QFelement.subString(1,16) == strTypeFeat3 ||
          QFelement.subString(1,16) == strTypeFeat4 ||
          QFelement.subString(1,16) == strTypeFeat5 ||
          QFelement.subString(1,16) == strTypeFeat6 ||
          QFelement.subString(1,16) == strTypeFeat7 ||
          QFelement.subString(1,16) == strTypeFeat8 )
        {
         /*typeproc = QFelement.subString(14,8);
         coreq    = QFelement.subString(9,4);
         prime    = QFelement.subString(23,4); old logic 55982 */

         typeproc = QFelement.subString(18,8);         //55982 MAMT 31/10/00
         coreq    = QFelement.subString(9,8).strip();  //55982 MAMT 31/10/00
         prime    = QFelement.subString(27,8).strip(); //55982 MAMT 31/10/00

         if (typeproc == "single  ")
           {
            uMRDBQty = findPrime( prime );
            if (uAASQty1 > uMRDBQty)
                uAASQty1 = uMRDBQty;
            k = K;
           }
         else
            if (typeproc == "multi or")
              {
               sumMRDBQty = 0;
               coreq1     = coreq;
               uMRDBQty = findPrime( prime );
               sumMRDBQty += uMRDBQty;
               while ( (k+1 <= K) && ( coreq == coreq1 ) )
                 {
                  QFelement1 = pQuickFixTable->elementAtPosition(k+1);
                  /* coreq1     = QFelement1.subString(9,4);
                  prime1     = QFelement1.subString(23,4); old logic 55982 */
                  coreq1     = QFelement1.subString(9,8).strip();  //55982 MAMT 31/10/00
                  prime1     = QFelement1.subString(27,8).strip(); //55982 MAMT 31/10/00
                  if (coreq == coreq1)
                    {
                     uMRDBQty = findPrime( prime1 );
                     sumMRDBQty += uMRDBQty;
                    }
                  k++;
                 }
               if (uAASQty1 > sumMRDBQty)
                   uAASQty1 = sumMRDBQty;
               k = K;
              }
            else
               if (typeproc == "multi & ")
                 {
                  uMRDBQty = findPrime( prime );
                  if (uAASQty1 != uMRDBQty)
                      uAASQty1 = uMRDBQty;
                  k = K;
                 }
        }
    }
 return uAASQty1;
}

    /*-----------------------------------------------------------------------*\
    |    unsigned FindPrime()                 49222 - MAMT - 12/07/2000       |
    |       Find the selected feature and later obtain the MRDB quantity      |
    |       to use in the QuickFixTable                                       |
    \*-----------------------------------------------------------------------*/
unsigned RMSystem::findPrime(IString PrimeF)
{
 int l,L;
 unsigned qtyf = 0;
 unsigned qtyf1= 0;
 for ( l =  0, L = pAMachine->mfeatList().NumberElements(); l < L ; l++)
   if (PrimeF == pAMachine->mfeatList().element(l)->feature())
     {
      qtyf = pAMachine->mfeatList().element(l)->baseNmbr();
      l = L;
     }
 return qtyf;
}

    /*-----------------------------------------------------------------------*\
    |   void resolveGenerics()                                                |
    |                                                                         |
    |       We're about to resolve generic features, but we'ld like           |
    |         to have those generic features which are not                    |
    |         resolved by that logic to be marked as candidates               |
    |         for the resolution of Generic Machines (an MRDB                 |
    |         generic feature entry for which there is an equivalent          |
    |         AAS machine). This should cut down on processing time.          |
    |                                                                         |
    |       Determine if there is any machine which has no MRDB               |
    |         records and is equivalent to a generic feature.                 |
    |       Call the resolveGeneric() method for all machines to be           |
    |         processed and in the process have them mark aforementioned      |
    |         candidates depending on whether such a machine is present.      |
    |       If a 'generic machine' was present:                               |
    |         Consider all processing machines which have no                  |
    |           MRDB data and for which a list of equivalent                  |
    |           generic features can be obtained:                             |
    |             Look among the other machines for a feature which           |
    |               is marked as an aforementioned candidate and              |
    |               appears in the equivalent feature list:                   |
    |                 Log our reasoning, generate a similar explanation,      |
    |                    add it tot the list of explanations and set the      |
    |                    the machine's Message Indicator.                     |
    |                 Discard a MRDB CFReport Record from the feature,        |
    |                    and if that was the last-one, release the            |
    |                    feature's associated storage and remove it           |
    |                    from its machine's list of features.                 |
    |                 Copy the generic machine's AAS records to its           |
    |                    MRDB side:                                           |
    |                      Copy its Machine Record and its 1st-54-Record.     |
    |                      For every feature that machine contains:           |
    |                         Copy its AAS records to its MRDB side.          |
    |                         Set the feature's base quantity to its          |
    |                           AAS quantity and marked it                    |
    |                           'resolved by us                               |
    |                                                                         |
    |         Change the tag of all leftover 'potential machine' features     |
    |           to 'unresolved' and set their base quantity to their MRDB     |
    |           quantity.                                                     |
    |        Note: This resolution is done partially at this System           |
    |              level since the 'generic machine' is different             |
    |              from that of the generic equivalent and, therefore         |
    |              cannot be done on a Machine-level.                         |
    \*-----------------------------------------------------------------------*/
    /*-----------------------------------------------------------------------*\
    |   int GenMachFeat_comp(const void *p1,const void * p2)                  |
    |     'C' compare function  comparing two buffers as strings              |
    |      used by the local PtrStrArr object.                                |
    \*-----------------------------------------------------------------------*/
int GenMachFeat_comp(const void *p1,const void * p2)
{
 return strcmp((char *)p1,(char *)p2);
}
// new version using the copyRecords methods
void RMSystem::resolveGenerics()
{
 Machine *pM2;
 MFeat *pMF;
 IString strGeneric;
 int i,I,j,k,K,m,M;
 Boolean fTagMaybeMachines = false;
 PtrStrArr GenMachFeatList(5,3,GenMachFeat_comp);

 for ( i =  0, I = ML.NumberElements(); !fTagMaybeMachines && (i < I) ; i++)
   if ((pAMachine = ML.element(i)) && (pAMachine->tag() == Machine::Process) )
     if (!pAMachine->qtyCFR(MFeat::MRDBType))
       fTagMaybeMachines = PK.isGenericMachine( pAMachine->type()
                                               ,pAMachine->model());

 for ( i =  0, I = ML.NumberElements() ; i < I ; i++)
   if ((pAMachine = ML.element(i)) && (pAMachine->tag() == Machine::Process) )
     pAMachine->resolveGenerics(PK,fTagMaybeMachines,RPOL,pSD->MsgFile,XPL);

 if (fTagMaybeMachines) {
   for ( i =  0; i < I ; i++)
     if ((pAMachine = ML.element(i))            &&
         (pAMachine->tag() == Machine::Process) &&
         !pAMachine->qtyCFR(MFeat::MRDBType)    &&
         PK.genericMachines(pAMachine->type(),
                            pAMachine->model(),
                            GenMachFeatList))
       for ( j =  0 ; j < I ; j++)
         if ((pM2 = ML.element(j)) && (pM2 != pAMachine))
           for ( k =  0; k < pM2->mfeatList().NumberElements() ; k++)
             if (pAFeature = pM2->mfeatList().element(k))
               if (pAFeature->tag() == MFeat::Potential_Machine)
                 if (GenMachFeatList.Find(pAFeature->feature()) ) {
                   tellem.say(false,EUO_MSG_RES_GENERIC_MACH,
                              pAFeature->feature(), pAFeature->description(),
                              pAMachine->type(),pAMachine->model());
//jt                   IMessageText msgt( EUO_MSG_RES_GENERIC_MACH, pSD->MsgFile,
                   IString msgt = IString( tellem.message( EUO_MSG_RES_GENERIC_MACH,
                                      pAFeature->feature(),
                                      pAFeature->description(),
                                      pAMachine->type(),pAMachine->model()) );
                   pAMachine->setMsgInd();
                   XPL.add(XPLN2(pAMachine->type(),pAMachine->model(),
                                 pAMachine->serialNo(),0,(const char *)msgt));
                   pAFeature->removeRecords(1,MFeat::MRDBType);
                   if (!pAFeature->qty(MFeat::MRDBType)) {
                     pAFeature->Release();
                     delete pAFeature;
                     pM2->mfeatList().DeleteIndex(k--);
                     }
                                                 // copy AAS side to MRDB side
                                                 // and tag as resolved
                   pAMachine->copyRecords(pAMachine,true,false,version(true),
                                          false,true);
                   if (M = pAMachine->mfeatList().NumberElements())
                     for (m = 0; m < M ; m++)
                       if (pMF = pAMachine->mfeatList().element(m)) {
                         pMF->setBase(pMF->qty(MFeat::AASType));
//                         pMF->setQty(MFeat::MRDBType,pMF->qty(MFeat::AASType));
                         pMF->setTag(MFeat::Resolved_EUOP);
                         }
                   }
   for ( i =  0, I = ML.NumberElements(); i < I ; i++)
     if (pAMachine = ML.element(i))
       for ( k =  0, K = pAMachine->mfeatList().NumberElements(); k < K ; k++)
         if ((pAFeature = pAMachine->mfeatList().element(k))     &&
                                                 // unresolved generics
             (pAFeature->tag() == MFeat::Potential_Machine)) {
           pAFeature->setTag(MFeat::Unresolved);
           pAFeature->setBase(pAFeature->qty(MFeat::MRDBType));
           }
   }
}

    /*-----------------------------------------------------------------------*\
    |    void resolveMRCombos()                                               |
    |         Call the same method for all machines to be processed.          |
    \*-----------------------------------------------------------------------*/
void RMSystem::resolveMRCombos()
{
 for (int i =  0, I = ML.NumberElements(); i < I ; i++)
   if ((pAMachine = ML.element(i)) && (pAMachine->tag() == Machine::Process) )
     pAMachine->resolveMRCombos(PK,pSD->MsgFile,XPL);
}

    /*-----------------------------------------------------------------------*\
    |    void RMSystem::resolveNonEquals()                                      |
    |       Determine the sense date and call the same                        |
    |       method for all machines to be processed.                          |
    \*-----------------------------------------------------------------------*/
void RMSystem::resolveNonEquals()
{
 int i, I = ML.NumberElements(), date = senseDateYYYYMMDD();
 for ( i =  0; i < I ; i++)
   if ((pAMachine = ML.element(i)) && (pAMachine->tag() == Machine::Process) )
     pAMachine->resolveNonEquals(date,RPOL,pSD->MsgFile,XPL);
}

    /*-----------------------------------------------------------------------*\
    |   IDate senseDate()                                                     |
    |       Have the MRDB 53-Record's date string saved in the System Data    |
    |       structure (having the YYYYMMDD format) converted                  |
    |       and return the resulting IDate.                                   |
    \*-----------------------------------------------------------------------*/
IDate RMSystem::senseDate()
{
 IDate date;
 cfrDateYYYYMMDD(pSD->DateRec53[true],date);
 return date;
}

    /*-----------------------------------------------------------------------*\
    |   long senseDateYYYYMMDD()                                              |
    |       Have the MRDB 53-Record's date string saved in the System Data    |
    |       structure (having the YYYYMMDD format) converted                  |
    |       and return the resulting long.                                    |
    \*-----------------------------------------------------------------------*/
long RMSystem::senseDateYYYYMMDD()
{
 return CFR54::cfrDateYYYYMMDD(pSD->DateRec53[true]);
}

 /*--------------------------------------------------------------------------*\
 |                          Write a CFR code section                          |
 \*--------------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------*\
    |   int appendRPOsToCFR(FILE *f,Boolean f20Format)                        |
    |       Append the generated RPO's as 54-Record by machine                |
    |        as 19-records to the current position of the given               |
    |        CFReport and return the number of records written.               |
    |                                                                         |
    |       Sort the RPO list to ensure we get RPO's grouped by               |
    |          machine                                                        |
    |       For every different machine in the RPO list:                      |
    |         Create and queue a 19-Record on the local list.                 |
    |         Create and queue a 54-Record with a blank feature               |
    |           (this is the required '1st-54-Record')                        |
    |           Depending on the version, either chop the record              |
    |             at its version-20 size or set its description.              |
    |         For every RPO with the same Machine:                            |
    |             Create and queue a 54-Record on the local list.             |
    |             Depending on the version, either chop the record            |
    |               at its version-20 size or set its description.            |
    |             Add to the created 19-Record's feature array using          |
    |               the RPO's feature, quantity and the appropriate           |
    |               action.                                                   |
    |       Have the local list write itself to the given file, retain        |
    |         it returned number of written records and                       |
    |          release it associated storage (the records created here).      |
    |       Return the number of written records.                             |
    \*-----------------------------------------------------------------------*/
int RMSystem::appendRPOsToCFR(FILE *f,Boolean f20Format)
{
 int cnt = 0;
 CFRecVL L54;
 CFRecV * pR19 = 0, *p54;
 RPO2 LastRPO;
 RPOL.SortUp();
 RPO2List::Cursor crsr(RPOL);
 if (RPOL.numberOfElements()) {
   forCursor(crsr) {
     RPO2 & rpo = RPOL.elementAt(crsr);
     if (!rpo.sameMachine(LastRPO)) {
       LastRPO = rpo;
       L54.AddasLast(pR19 = CFR19::createANew((char *)rpo.machineType(),
                                              (char*)rpo.machineModel()));
       L54.AddasLast(p54 = CFR54::createANew((char *)rpo.machineType(),
                                             (char*)rpo.machineModel(),
                                             (char*)rpo.serialNum()," "));
       if (f20Format)
         ((Record_54 *)p54->record())->Sequence[0] = '\0';
       else CFR54::setDescription(p54,(char *)rpo.description());
       }
     L54.AddasLast(p54 = CFR54::createANew( (char *)rpo.machineType(),
                                            (char*)rpo.machineModel(),
                                            (char*)rpo.serialNum(),
                                            (char *)rpo.featureCode()));
     if (f20Format)
       ((Record_54 *)p54->record())->Sequence[0] = '\0';
     else CFR54::setDescription(p54,(char *)rpo.description());
     CFR19::appendFeat(*pR19,(char *)rpo.featureCode(),rpo.qty(),
                       (rpo.action() == RPO_CODE_ADD? '1': '2'));
     }
   cnt = L54.fWriteCFR(f);
   L54.Release();
   }
 return cnt;
}

Boolean RMSystem::appendCheckSumRecord(const char * szFileName)
{
 FILE *f;
 IString strSum(fCheckSum(szFileName));
// IString strSum(cfrcksum((char *)szFileName,"82151"));
 if ((f=fopen(szFileName,"r+")) && !fseek(f,0,SEEK_END)) {
   fputs((char *)IString("\n99" + IString((char *)strSum,10,' ')),f);
   fclose(f);
   return true;
   }
 return false;
}
    /*-----------------------------------------------------------------------*\
    |   Boolean fWriteCFR()                                                   |
    |       Write the System as a CFReport having the given name, from        |
    |       the AAS or MRDB Records in either Version 20 or 30 format,        |
    |       optionally appending RPOs and indicate success:                   |
    |                                                                         |
    |           Open the file and write the System's CFR records.             |
    |           Call upon the Machine List to write itself, handing           |
    |              over the Placement and Connection Lists (as                |
    |              explained below)                                           |
    |           Write the generated the Placement and Connection Lists.       |
    |           Optionally, append the RPO's.                                 |
    |           Generate and append the trailer record (98) using             |
    |             the number of records written and close the file.           |
    |           Release the storage associated with the generated             |
    |             Placement and Connection Lists.                             |
    |                                                                         |
    |       Notes:                                                            |
    |           Some background on the Placement and Connection Lists:        |
    |                Some Machine Features have additional                    |
    |                information associated with them in the form of other    |
    |                Records which are tied to the feature records            |
    |                via their common 'InstanceId' field.                     |
    |                In a CFReport, these ids need to start at 1 and be       |
    |                consecutive, while the records are to be grouped         |
    |                together by type and appended to the CFReport.           |
    |                During reconciliation, a feature might disappear         |
    |                which could violate the Instance Id order.               |
    |                We handle the Placement (55-Record) and Connection       |
    |                (56-Record) records which are accumulated in lists       |
    |                as the CFReport are read in.                             |
    |                These 'In' lists are used as a source for the 'Out'      |
    |                lists which will consist of those records that belong    |
    |                to features left over after the reconciliation process,  |
    |                but re-numbered with fresh Instance Ids.                 |
    \*-----------------------------------------------------------------------*/

Boolean RMSystem::fWriteCFR(const char * szFileName,Boolean fMRDB,
                          Boolean fAppendRPOs, Boolean bSetLockingFlag)
{
 FILE *f;
 char Rec98[] = "98      ";
 int cnt;
 Boolean fOK = false;
 BiiName=szFileName; //ELRR 147652

 if (f=fopen(szFileName,"w")) {
   //edw
   cnt = (version(fMRDB) == 31)? fWriteCFRV31(f,fMRDB,fAppendRPOs,bSetLockingFlag) :
                                 fWriteCFRV30(f,fMRDB,fAppendRPOs);
   //cnt = fWriteCFRV31(f,fMRDB,fAppendRPOs);
   if (cnt > 0) {
     IString strCnt(++cnt);
     strcpy(&Rec98[2 + 5 - ALMIN(strCnt.length(),5)],(char *)strCnt);
     fputs(Rec98,f);
     fOK = true;
     }
   fclose(f);
   //Commented for F47640 - flw
   //if (pSD->ulCheckSum[fMRDB])
     //appendCheckSumRecord(szFileName);
   }
 else tellem.say(true,EUO_MSG_ERR_OPEN_FILE,szFileName);
 return fOK;
}

int RMSystem::fWriteCFRV30(FILE *f,Boolean fMRDB, Boolean fAppendRPOs)
{
 CFRInstL L55_Out(false);                        // unsorted
 CFRInstL L56_Out(false);                        // unsorted
 unsigned InstId = 1;                            // first Instance Id
 int i,i2;
 if ((i2 = CFRLists[fMRDB].fWriteCFR(f)) == CFRLists[fMRDB].NumberElements())
   if (((i = ML.fWriteCFR(f,fMRDB, &InstId,L55,L55_Out,L56,L56_Out) ) >= 0 )) {
     i += i2;
     i += (L55_Out.fWriteA(f)? L55_Out.NumberElements() : 0);
     i += (L56_Out.fWriteA(f)? L56_Out.NumberElements() : 0);
     if (fAppendRPOs)
       i += appendRPOsToCFR(f);
     }
 L55_Out.Release();
 L56_Out.Release();
 return i;
}

int RMSystem::fWriteCFRV31(FILE *f,Boolean fMRDB, Boolean fAppendRPOs, Boolean bSetLockingFlag)
{
 unsigned InstId = 1;                            // firsrt Instance Id
 int i,i2, i3=0;             // 95887 i3 added
 IString strNewLine = STR_ProfileDelimiter;      //edw06/04/1998
 IString strSys, strCurId;                       //edw06/04/1998
 IString str01(STR_Rec01Text);                   //edw06/04/1998

 //ELRR 147652
 char buf[3000];
 int JOnce=0,FCount=0,C05count=0,j=0,count=0,count2=0;
 char *bii[4000];
 IString ix;
 int MultiStart[1000];
 int MultiEnd[1000];

 BiiName.remove(BiiName.lastIndexOf('.') + 1);
 BiiName += IString("bii");
 // ELRR 147652

 //edwtest i = L05[fMRDB].fWriteCFR(f,CFR05::TopOfReport);
 i = 0;                                          //edwtest
 if ((i2 = CFRLists[fMRDB].fWriteCFR(f))==CFRLists[fMRDB].NumberElements()) {
    i += i2;
    //edw07/01/1998 Set locking indicator if
    if ( bSetLockingFlag )
       {
       fputs(STR_CFRLockIndicator + strNewLine, f);
       i++;
       }
    ISortedSet<IString>::Cursor crs( sSystemRecordSet );
    forCursor( crs )                             //edw06/04/1998
       {
       i += L05[fMRDB].fWriteCFR(f,CFR05::TopOfSystem);
       strSys = sSystemRecordSet.elementAt( crs );
       strCurId = strSys.subString(6, 1);

       fputs(strSys + strNewLine,f);
       i += 3;                                 //95887
       i3 = CFRLists[1].fWrite53CFR(f, 2);     //95887  to read 53 record from CFRLists
       i ++;                                   //95887
       fputs(pRM2SignatureRecord+ strNewLine, f);

// START ELRR 147652
    //Open File to extract 05 records


if (JOnce==0)
{
  FILE * pFile;
  pFile = fopen (BiiName,"r");
  if (pFile==NULL)
  {
    MessageBox(0,BiiName,"Error opening file",0);
   return FALSE;
  }

  while( fgets(buf,3000,pFile) )
  {
  bii[count] =  (char *) malloc(3000);
  strcpy(bii[count],buf);
  count++;
  }
  fclose(pFile);

 //count 05 records in CFr and add them to the BIR file

 for(j=0;j<count;j++)
{
 ix=bii[j];
 if(ix.subString(1,2)=="05")
  {  i++;
     fputs(ix, f);
  }
JOnce=1;
}
}

count=0;
j=0;
// END ELRR 147652


       fputs(str01 + strNewLine, f);

       if (((i += ML.fWriteCFRV31(f,fMRDB, &InstId,L52[fMRDB],L55,L56,
                                  L10[fMRDB],L05[fMRDB], strCurId ) ) >= 0 )) {
          if (fAppendRPOs)
            i += appendRPOsToCFR(f);
          i += L05[fMRDB].fWriteCFR(f,CFR05::BottomOfSystem);
          }
       }
    i += L05[fMRDB].fWriteCFR(f,CFR05::BottomOfReport);
    }
 return i;
}

 /*--------------------------------------------------------------------------*\
 |                    Discrepancy resolution code section                     |
 \*--------------------------------------------------------------------------*/
    /*-----------------------------------------------------------------------*\
    |   MFeat * findProcessFeature()                                          |
    |       Return the pointer to the MFeat object belonging to               |
    |       a machinne that's to be processed and corresponding               |
    |       to the given data.                                                |
    \*-----------------------------------------------------------------------*/
MFeat * RMSystem::findProcessFeature(const char *szType,const char * szModel,
                                   const char *szSerial,const char * szFeature)
{
 Machine * pM;
 MFeat * pF = 0;
 if (pM = ML.FindProcessMachine(szType,szModel,szSerial))
   pF = pM->mfeatList().Find(szFeature);
 return pF;
}

#if 0
    /*-----------------------------------------------------------------------*\
    |   MFeat * findProcessOEMFeature()                                       |
    |       Return the pointer to an "OEM" MFeat object belonging to          |
    |       a machinne that's to be processed and corresponding               |
    |       to the given data.                                                |
    \*-----------------------------------------------------------------------*/
MFeat * RMSystem::findProcessOEMFeature(const char *szType,const char * szModel,
                                      const char *szSerial,const char * szFeature)
{
 Machine * pM;
 MFeat * pF = 0;
 if (pM = ML.FindProcessMachine(szType,szModel,szSerial))
   pF = pM->mfeatList().FindOEMFeature(szFeature);
 return pF;
}

    /*-----------------------------------------------------------------------*\
    |   MFeat * findProcessNonOEMFeature()                                    |
    |       Return the pointer to a non-"OEM" MFeat object belonging to       |
    |       a machinne that's to be processed and corresponding               |
    |       to the given data.                                                |
    \*-----------------------------------------------------------------------*/
MFeat * RMSystem::findProcessNonOEMFeature(const char *szType,const char * szModel,
                                         const char *szSerial,const char * szFeature)
{
 Machine * pM;
 MFeat * pF = 0;
 if (pM = ML.FindProcessMachine(szType,szModel,szSerial))
   pF = pM->mfeatList().FindNonOEMFeature(szFeature);
 return pF;
}
#endif

    /*-----------------------------------------------------------------------*\
    |   Boolean updateWithAlteredGeneric()                                    |
    |       Update the system with the given generic Feature of               |
    |       which the user has designated the given number as                 |
    |       specific features of given name and description.                  |
    |       (Features designated as 'OEM' keep their generic                  |
    |        names while their descriptions are preceded with                 |
    |        "(OEM) ")                                                        |
    |                                                                         |
    |         If the new feature name is "OEM"                                |
    |           If there's already such a feature among the                   |
    |              generic's Machine's features:                              |
    |                Adjust that feature's base quantity                      |
    |                Remember if the generic's base quantity                  |
    |                  is all used up, otherwise adjust it.                   |
    |           Otherwise,                                                    |
    |              If the quantity would use up the generic's base:           |
    |                 "OEM"-mark the generic, mark it 'resolved by us'        |
    |                   and set its base quantity.                            |
    |              Otherwise   (partial quantity)                             |
    |                  Create a new Feature, duplicating the generic's        |
    |                    MFeat object data.                                   |
    |                 "OEM"-mark the feature, mark it 'resolved by us'        |
    |                   and set its base quantity.                            |
    |                 Add the feature to the generic's machine's              |
    |                   list of features                                      |
    |                 Establish a Feat object with                            |
    |                    a Feature-List-unique Id, add it to the              |
    |                    Feature List, and have that re-sorted on             |
    |                    account of the addition.                             |
    |                 Adjust the generic's base quantity.                     |
    |         Otherwise, (non-"OEM")                                          |
    |           Use the Feature List to quickly look for a feature            |
    |             having the new name and the same machine as the             |
    |             generic's.                                                  |
    |           If the feature already exists:                                |
    |              Mark it 'resolved by us' and adjust its base quantity.     |
    |           Otherwise,                                                    |
    |              Create a new Feature, duplicating the generic's            |
    |                 MFeat object data.                                      |
    |              Set the feature's new name, its new description            |
    |                (either given or obtained from Product Knowledge),       |
    |                mark it 'resolved by us' and set its base quantity.      |
    |              Add the feature to the generic's machine's                 |
    |                list of features                                         |
    |              Establish a Feat object with a Feature-List-unique Id,     |
    |                add it to the Feature List, and have that re-sorted      |
    |                on account of the addition.                              |
    |           Remember if the generic's base quantity is all used up,       |
    |              otherwise adjust it.                                       |
    |         If the generic's base quantity is used up,                      |
    |            it's eliminated:                                             |
    |               Remove the generic from the Feature List.                 |
    |               Remove the generic from its Machine's                     |
    |                  list of features.                                      |
    |                                                                         |
    |       Notes:                                                            |
    |          On "OEM'-ed features:                                          |
    |            During resolution, there can                                 |
    |             be two generic features with the same name:                 |
    |             one from the MRDB file and the other tagged as OEM          |
    |             by the user, if the latter was a part of the former.        |
    |             e.g. If there's a generic feature 'A' with a quantity       |
    |                  of 5, the user can designate 3 of them as OEM.         |
    |                  Before the user resolves the 2 remaining 'A''s,        |
    |                  we'll be in the aforementioned state,                  |
    |                  especially when that partial resolution is saved.      |
    \*-----------------------------------------------------------------------*/
Boolean RMSystem::updateWithAlteredGeneric(Feat & fG,const char * newFeat,
                                         unsigned qty,const char * newDesc,
                                         Feat * pAffectedFeat)
{
 if (!fG.pM || !fG.pF)
   return false;
 if (fG.sameFeature(newFeat)) {
   if (pAffectedFeat)
     *pAffectedFeat = fG;
   return applyUnresolvedFeature(fG, qty);
   }
 Boolean fOK = true, fRemoveGen = false, fFound;
 MFeatList & MFL = fG.pM->mfeatList();
 if (!strcmp(newFeat,"OEM"))
   if (pAFeature = MFL.FindOEMFeature(fG.feature())) {
     pAFeature->setBase(pAFeature->baseNmbr() + qty);
     if (!(fRemoveGen = (qty >= fG.qtyBase())))
       fG.setBase(fG.qtyBase() - qty);
     if (pAffectedFeat)
       *pAffectedFeat = Feat(fG.pM,pAFeature,0);
     }
   else if (qty >= fG.qtyBase()) {
          fG.markAsOEM();
          fG.setStatus( Feat::Resolved_User );
          fG.setBase(qty);
          fG.pF->copyRecords(fG.pF,1,MFeat::AASType,MFeat::MRDBType,
                             fG.pM->machineRecVersion(false),false,false);
          if (pAffectedFeat)
            *pAffectedFeat = fG;
          }
        else {
          pAFeature = new MFeat();
          pAFeature->duplicate(*(fG.pF)).setBase(qty);
          pAFeature->setTag( MFeat::Resolved_User );
          pAFeature->markAsOEM();
          pAFeature->copyRecords(pAFeature,1,MFeat::AASType,MFeat::MRDBType,
                                 fG.pM->machineRecVersion(false),false,false);
          MFL.Add(pAFeature);
          Feat f(fG.pM,pAFeature,FL.greatestID() +1);
          FL.add(f);
          if (pAffectedFeat)
            *pAffectedFeat = f;
//          FL.add(Feat(fG.pM,pAFeature,FL.greatestID() +1));
          FL.SortUp();
          fG.setBase(fG.qtyBase() - qty);
          }
 else {
                                                 // look for new-one in system
   FeatList::Cursor crsr(FL);                    // via the FeatList
   for (fFound = false,FL.setToFirst(crsr); !fFound && crsr.isValid();
        FL.setToNext(crsr)) {
     Feat & fL = FL.elementAt(crsr);
     if (fFound = (fL.sameMachine(fG) && fL.sameFeature(newFeat) &&
                   !fL.isMarkedAsOEM())) {
       fL.setBase(fL.qtyBase() + qty);           // found new generic
       fL.setStatus( Feat::Resolved_User );
       if (pAffectedFeat)
         *pAffectedFeat = fL;
       }
     }
   if (!fFound) {                                // greate the new feature
     pAFeature = new MFeat();
     fG.pF->setInstType('I');                    // was generic
     pAFeature->duplicate(*(fG.pF)).setFeature(newFeat);
     pAFeature->unMark();
     pAFeature->setBase(qty);
     pAFeature->setTag( MFeat::Resolved_User );
     pAFeature->setDescription(newDesc? newDesc :
                           PK.description(fG.machine(),fG.model(),newFeat));
     pAFeature->copyRecords(pAFeature,1,MFeat::AASType,MFeat::MRDBType,
                            fG.pM->machineRecVersion(false),false,false);
     MFL.Add(pAFeature);
     Feat f(fG.pM,pAFeature,FL.greatestID() +1);
     FL.add(f);
     if (pAffectedFeat)
       *pAffectedFeat = f;
//     FL.add(Feat(fG.pM,pAFeature,FL.greatestID() +1));
     FL.SortUp();
     }
   if (!(fRemoveGen = (qty >= fG.qtyBase())))
     fG.setBase(fG.qtyBase() - qty);
   }
 if (fRemoveGen) {                               // eliminate old generic
   MFL.remove(fG.pF);
   FeatList::Cursor crsr(FL);
   for (fOK = false,FL.setToFirst(crsr); !fOK && crsr.isValid();
        FL.setToNext(crsr))
     if (fG.compareMachFeat(FL.elementAt(crsr))==0)
       if (!FL.elementAt(crsr).isMarkedAsOEM()) {
         fOK = true;
         FL.removeAt(crsr);
                 /*------------------------------------------*\
                 | keep from FL.setToNext(crsr) with an       |
                 | invalid cursor on account of the removeAt  |
                 | -> sometimes it blows                      |
                 \*------------------------------------------*/
         break;
         }
   }
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean updateWithAlteredGeneric()                                    |
    |       Update the system with the given generic Feature                  |
    |       which the user has designated as a                                |
    |       specific feature of given name and description.                   |
    \*-----------------------------------------------------------------------*/
Boolean RMSystem::updateWithAlteredGeneric(Feat & fG,const char * newFeat,
                                         const char * newDesc,
                                         Feat * pAffectedFeat )
{
// return updateWithAlteredGeneric(fG,newFeat,1,newDesc,pAffectedFeat); // test
 return updateWithAlteredGeneric(fG,newFeat,fG.qtyBase(),newDesc,pAffectedFeat);
}

    /*-----------------------------------------------------------------------*\
    |   Boolean updateWithMachinedGeneric()                                   |
    |       Update the system with the given generic Feature of               |
    |       which the user has designated the given number as                 |
    |       machines of given type, model and description.                    |
    |       Add the to be generated Feat object(s) to the                     |
    |       given list and indicate success.                                  |
    |                                                                         |
    |         If not given, get the machine's description from                |
    |           the Product Knowledge.                                        |
    |         Generate a Machine Record                                       |
    |         For the given number of machines:                               |
    |            Create a Machine with the given data and                     |
    |              a freshly generated Serial # and add it                    |
    |              to the Machine List.                                       |
    |            Set the machine's MRDB Machine Record with                   |
    |              the generated record.                                      |
    |            Create a 54-Record with a blank feature                      |
    |              and set it as the machine's required MRDB                  |
    |              '1st-54-Record'.                                           |
    |            Set the machine's description at this time so it will        |
    |              be propegated to the just added '1st-54-Record'.           |
    |            Establish a featureless-machine Feat object with             |
    |              a Feature-List-unique Id and add it to the                 |
    |              Feature List, as well as the given List                    |
    |         Sort the Feature List on account of its new elements.           |
    |         Release the Machine Record.                                     |
    |         If the generic's base quantity is used up,                      |
    |            it's eliminated:                                             |
    |               Remove the feature from the Feature List.                 |
    |               Remove the feature from its Machine's                     |
    |                  list of features.                                      |
    |               If the generic was its machine's last                     |
    |                 feature, release the machine's associated               |
    |                 storage and remove it from the Machine List.            |
    |         Otherwise, adjust the generic's base quantity.                  |
    \*-----------------------------------------------------------------------*/
Boolean RMSystem::updateWithMachinedGeneric(Feat & fG,unsigned qty,
                                          const char * type,
                                          const char * model,
                                          const char * desc,
                                          FeatList & FList)
{
 Boolean fOK = true;
 unsigned n = qty;
 IDate date;
 if (!fG.pM || !fG.pF)
   return false;
 else pAMachine = fG.pM, pAFeature = fG.pF;
 FList.removeAll();
 if (qty) {
   CFRecV * pMRec[2], *pInfoRec;
   for (int i = 0; i < 2 ; i++)
     switch (version(i)) {
       case 20:
       case 30:
         pMRec[i] = CFR08::createANew(type,model,fG.productIndicator());
         break;
       case 31:
         pMRec[i] = CFR08V31::createANew(type,model);
         break;
       default: return false;
       }
   IString strDesc(desc? desc : PK.description(type,model));
   while(n--) {
     Machine * pM = new Machine(type,model,(char *)newSerialNo());
                                                 // duplicates record
     pM->setMachineRec(false,*pMRec[0],version(false));
     pM->setMachineRec(true,*pMRec[1],version(true));

     for (int i = 0; i < 2; i++) {
       pM->setSubsystemRecordId(i,pAMachine->subsystemRecordId(i));
       switch (version(i)) {
         case 20:
         case 30:
           pInfoRec = CFR54::createANew(type,model,pM->serialNo()," ");
           break;
         case 31:
           pInfoRec = CFR95::createANew(type,model,pM->serialNo());
           break;
         default: pInfoRec = 0;
         }
       if (pInfoRec) {
         pM->addInfoRec(i,pInfoRec->record());   // duplicates the record
         if (pAMachine->installDate(date,i))
           pM->setInstallDate(date,i);
         pM->setDescription((char *)strDesc,i);
         delete pInfoRec;
         }
       }
     ML.Add(pM);
     Feat f(pM,0,FL.greatestID() +1);
     FL.add(f),FList.add(f);
     }
   FL.SortUp();
   delete pMRec[0], delete pMRec[1];

   if (qty >= fG.qtyBase()) {
     FeatList::Cursor crsr(FL);
     for (fOK = false,FL.setToFirst(crsr); !fOK && crsr.isValid();
          FL.setToNext(crsr))
       if (fG.compareMachFeat(FL.elementAt(crsr))==0)
         if (!FL.elementAt(crsr).isMarkedAsOEM()) {
           fOK = true;
           FL.removeAt(crsr);
                 /*------------------------------------------*\
                 | keep from FL.setToNext(crsr) with an       |
                 | invalid cursor on account of the removeAt  |
                 | -> sometimes it blows                      |
                 \*------------------------------------------*/
           break;
           }
     pAMachine->mfeatList().remove(pAFeature);
     if (!pAMachine->mfeatList().NumberElements()) {
       pAMachine->Release();
       ML.DeletePtr(pAMachine);                  // remove form array
       delete pAMachine;
       }
     }
   else fG.setBase(fG.qtyBase() - qty);
   }
 return fOK;
}


    /*-----------------------------------------------------------------------*\
    |   Boolean updateWithMachinedGeneric()                                   |
    |       Update the system with the given generic Feature                  |
    |       which the user has designated as                                  |
    |       machines of given type, model and description.                    |
    |       Add the to be generated Feat object(s) to the                     |
    |       given list and indicate success.                                  |
    \*-----------------------------------------------------------------------*/
Boolean RMSystem::updateWithMachinedGeneric(Feat & fG,const char * type,
                                          const char * model,
                                          const char * desc,
                                          FeatList & FList)
{
//  return updateWithMachinedGeneric(fG,1,type,model,desc,FList); // testing partial
  return updateWithMachinedGeneric(fG,fG.qtyBase(),type,model,desc,FList);
}
//#if 0
    /*-----------------------------------------------------------------------*\
    |   Boolean applyUnresolvedFeature()                                      |
    |                  especially when that partial resolution is saved.      |
    \*-----------------------------------------------------------------------*/
Boolean RMSystem::applyUnresolvedFeature(Feat & feat, unsigned qty)
{
 Boolean fOK = ( (feat.pM != 0) && (feat.pF != 0));
 if (fOK) {
   if (feat.isMarkedAsPartial())
     if (pAFeature = feat.pM->mfeatList().FindUnmarkedFeature(feat.feature())) {
       pAFeature->setBase(pAFeature->baseNmbr() + qty);
//       pAFeature->setQty(MFeat::AASType,
//                         pAFeature->qty(MFeat::AASType) + feat.qtyAAS());
//       pAFeature->setQty(MFeat::MRDBType,
//                         pAFeature->qty(MFeat::MRDBType) + feat.qtyMRDB());

       const char *featSave = feat.feature();
       if( qty >= feat.qtyBase() )
          {
          feat.pM->mfeatList().remove(feat.pF);
//          FeatList::Cursor crsr(FL);
//          for (fOK = false,FL.setToFirst(crsr); !fOK && crsr.isValid();
//               FL.setToNext(crsr))
//            if (feat.compareMachFeat(FL.elementAt(crsr))==0)
//              if (FL.elementAt(crsr).isMarkedAsPartial()) {
//                 fOK = true;
//                 FL.removeAt(crsr);
                    /*------------------------------------------*\
                    | keep from FL.setToNext(crsr) with an       |
                    | invalid cursor on account of the removeAt  |
                    | -> sometimes it blows                      |
                    \*------------------------------------------*/
//                 break;
//                 }
          }
       else
          feat.setBase(feat.qtyBase() - qty );

       pAFeature->setTag(MFeat::Resolved_User );
       }
     else fOK = false;
   //else feat.setStatus( Feat::Resolved_User );
   //edw 050697, replace else above
   else
      {
      MFeatList & MFL = feat.pM->mfeatList();
      pAFeature = new MFeat();
//      feat.pF->setInstType('I');                    // was generic
      pAFeature->duplicate(*(feat.pF)).setFeature(feat.feature());
      pAFeature->setBase(qty);
      pAFeature->setTag( MFeat::Resolved_User );
//      pAFeature->setDescription(newDesc? newDesc :
//                           PK.description(fG.machine(),fG.model(),newFeat));
      pAFeature->copyRecords(pAFeature,1,MFeat::AASType,MFeat::MRDBType,
                            feat.pM->machineRecVersion(false),false,false);
      MFL.Add(pAFeature);
//      Feat f(feat.pM,pAFeature,FL.greatestID() +1);
//      FL.add(f);
//      if (pAffectedFeat)
//        *pAffectedFeat = f;
//     FL.add(Feat(fG.pM,pAFeature,FL.greatestID() +1));
//      FL.SortUp();
      feat.pF->markAsPartial();
      if( qty >= feat.qtyBase() )
          {
          feat.pM->mfeatList().remove(feat.pF);
//          FeatList::Cursor crsr(FL);
//          for (fOK = false,FL.setToFirst(crsr); !fOK && crsr.isValid();
//               FL.setToNext(crsr))
//            if (feat.compareMachFeat(FL.elementAt(crsr))==0)
//              if (FL.elementAt(crsr).isMarkedAsPartial()) {
//                 fOK = true;
//                 FL.removeAt(crsr);
                    /*------------------------------------------*\
                    | keep from FL.setToNext(crsr) with an       |
                    | invalid cursor on account of the removeAt  |
                    | -> sometimes it blows                      |
                    \*------------------------------------------*/
//                 break;
//                 }
          }
       else
          feat.setBase(feat.qtyBase() - qty );
      }
   }
 return fOK;
}
//#endif


 /*--------------------------------------------------------------------------*\
 |                 Print-Format related functions and methods                 |
 \*--------------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------*\
    |   unsigned skipLine(char * Buffer,unsigned * pLines,FILE *f)            |
    |      'C' function to skip a line in the given buffer or                 |
    |      current position of the given file, to increment the               |
    |      given line count and return the number of bytes written.           |
    \*-----------------------------------------------------------------------*/
unsigned skipLine(char * Buffer,unsigned * pLines,FILE *f)
{
 unsigned bytes = 0;
 if (f) {
   if (fputs("\x0D\x0A",f) != EOF)
     bytes = 2;
   }
 else if (Buffer)
        *Buffer++  = '\x0D',*Buffer++ = '\x0A',bytes = 2;
 if (pLines && bytes)
   (*pLines)++;
 return bytes;
}

    /*-----------------------------------------------------------------------*\
    |   unsigned RMSystem::getInventorySWList(Boolean fMRDB, FeatList &L)       |
    |      Load the given List with Software Feat objects generated           |
    |      from the AAS or MRDB file and return their quantity.               |
    |                                                                         |
    |      For all Machines based on 17,47 and 49 Type records                |
    |        read from the AAS or MRDB File:                                  |
    |          Add all their features as Feat objects.                        |
    |      return collection count                                            |
    \*-----------------------------------------------------------------------*/
unsigned RMSystem::getInventorySWList(Boolean fMRDB, FeatList &L)
{
 int i,I,k,K,id=1;
 L.removeAll();
 for (i = 0 , I = ML.NumberElements(); i < I ; i++)
   if ((pAMachine = ML.element(i)) && pAMachine->machineRec(fMRDB).length())
     switch(pAMachine->machineRec(fMRDB).type()) {
       case 17:
       case 47:
       case 49:
         if ( K = pAMachine->mfeatList().NumberElements()) {
           for (k = 0; k < K ; k++)
             if ((pAFeature = pAMachine->mfeatList().element(k)) &&
                  pAFeature->qty(fMRDB? MFeat::MRDBType : MFeat::AASType))
               L.add(Feat(pAMachine,pAFeature,id++));
           }
         else L.add(Feat(pAMachine,0,id++));
       }
 return L.numberOfElements();
}

    /*-----------------------------------------------------------------------*\
    |   unsigned getSWListf(Boolean fMRDB, SoftMachList & )                   |
    |      Load the given List with SoftMach objects generated                |
    |      from the AAS or MRDB file and return their quantity.               |
    |                                                                         |
    |      For all Machines based on 17,47 and 49 Type records                |
    |        read from the AAS or MRDB File:                                  |
    |          Add all their features as SoftMach objects.                    |
    |      return collection count                                            |
    \*-----------------------------------------------------------------------*/
unsigned RMSystem::getSWDiscListf(Boolean fMRDB, SoftMachList & SWDL)
{
 int i,I,k,K;
 IDate date;
 SWDiscList.removeAll();
 for (i = 0 , I = ML.NumberElements(); i < I ; i++)
   if ((pAMachine = ML.element(i)) && pAMachine->machineRec(fMRDB).length())
     switch(pAMachine->machineRec(fMRDB).type()) {
       case 17:
       case 47:
       case 49:
         for (k=0,K=pAMachine->mfeatList().NumberElements(); k < K ; k++)
           if ((pAFeature = pAMachine->mfeatList().element(k)) &&
               pAFeature->qty(fMRDB? MFeat::MRDBType : MFeat::AASType))
             SWDiscList.add(SoftMach(pAMachine->type(),pAMachine->model(),
                              pAFeature->feature(),pAFeature->description(),
                              pAFeature->installDate(date,fMRDB)?
                                                  date : SoftMach::nonDate()));
       }
 return SWDiscList.numberOfElements();
}

    /*-----------------------------------------------------------------------*\
    |   unsigned Format()                                                     |
    |      Print-format the object to either the given buffer or              |
    |      current position of the given file and return the                  |
    |      number of bytes written.                                           |
    |      The date fields will be formatted as per the given                 |
    |      strftime()-type date formatting string.                            |
    |                                                                         |
    |         Enforce our ranges for width and length.                        |
    |         Use the default Date formatting String, if not given.           |
    |         If given no file, nor buffer address, allocate a buffer.        |
    |         Compose the report's title and use it to                        |
    |           establish the page header.                                    |
    |         Create the section headers and lines.                           |
    |         Format and write the different sections.                        |
    |         Return the number of bytes written.                             |
    |      Notes:                                                             |
    |         If a 'machine' straddles pages, its header on subsequent        |
    |         pages will be appended with a column showing the                |
    |         continuation text like "Continue" or an abreviation             |
    |         if we're cramped for space.                                     |
    |                                                                         |
    |         To illustrate some of the terms used in the section comments:   |
    |                                                                         |
    |              This is a Header or Line                                   |
    |           /------+-----+-------------\                                  |
    |           | Type |Model|Serial Number|     Column Texts                 |
    |           |------+-----+-------------|                                  |
    |           | TTTT | MMM |  12-34567   |     Header/Line Texts            |
    |                                                                         |
    |              Header with Continuation column                            |
    |           /------+-----+-------------+-------------\                    |
    |           | Type |Model|Serial Number| Continued.. |                    |
    |           |------+-----+-------------+-------------/                    |
    |           | TTTT | MMM |  12-34567   |                                  |
    |                                                                         |
    |              Header with a Line subheader                               |
    |           /------+-----+-------------\                                  |
    |           | Type |Model|Serial Number|                                  |
    |           |------+-----+-------------|                                  |
    |           | TTTT | MMM |  12-34567   |                                  |
    |           |-----+-------+-------------------+---------\                 |
    |           | Qty |Feature| Description       | Install |                 |
    |           |     |       |                   |  Date   |                 |
    |           |-----+-------+-------------------+---------|                 |
    |           |  2  |F223456| This is feature   |12-14-94 |                 |
    |           |  2  |F223456| This is feature   |12-14-94 |                 |
    |           \-----+-------+-------------------+---------/  closure line   |
    |                                                                         |
    |           The difference between a Header and a Line is                 |
    |             that the former can write the latter as a                   |
    |             subheader.                                                  |
    |                                                                         |
    |             A Pageheader                                                |
    |                                                                         |
    |         System : S123456   sensed : 12-8-94                 Page  1     |
    |         -------------------------------------------------------------   |
    \*-----------------------------------------------------------------------*/
                                                 // static default
                                                 // strftime()-type date
char *pSTRFTIME_FRMT = "%m/%d/%y";               // formatting string
unsigned RMSystem :: Format( unsigned PageW,       // page width
                           unsigned PageL,       // page length
                           char ** pBuf,         // buffer address
                           Columns::FrameStyle fs,
                           FILE *f,
                           const char * strftimeFormat)
{
 int i;
 unsigned Line = 0
         ,n, N, j, J ,uT = 0;
 PageW = ALMIN(ALMAX(60,PageW),100);
 PageL = ALMIN(ALMAX(20,PageL),80);
 char * B = 0;
 if (!strftimeFormat)
   strftimeFormat = pSTRFTIME_FRMT;
 if (!f)
   { if (!pBuf)
       return 0;
     B = new char [100000];
     *pBuf = B;
   }
 IDate date;
                                                 // pageheader
 IString title = IString("Customer : ") + IString(customer()) +
                 IString("  System : ") + IString(name()
//               + IString("  sensed : ")
//               + Res.fileDate().asString(strftimeFormat);
                        );
 PageHdr Hdr(PageW,(char *)title,fs,"Page");     // page header

 ItemHdr MachHdr(fs)                             // Machine Header
        ,RPOHdr(fs)                              // RPO's Header
        ,SWHdr(fs)                               // Software Header
        ,BLHdr(fs);                              // Backlog Header
 Columns  FeatLine(fs)                           // Feature Line
         ,RPOLine(fs)                            // RPO Line
         ,SWLine(fs)                             // Software Line
         ,BLLine(fs);                            // Backlog Line
 IString strMach(" Machine ")
        ,strModel(" Model ")
        ,strSerNo(" Serial # ")
        ,strDesc("Description")
        ,strInstDate("Install Date")
//        ,strInstDate(" Inst. Date ")
        ,strInstallDate(" Install Date ")
        ,strCSDate(" CS Date ")
        ,strQty(" Qty ")
        ,strAction(" Action ")
        ,strFeat(" Feature ")
        ,strProgram(" Program ")
        ,strContinued(" (continued) ")
        ,strCont("cont")
        ,strCSDDate(" CSD Date ")
        ,strUnavail("Unavailable")
        ,strNA("N.A.")
        ,strNone("(None)")
        ,str,str2,strQ,strA;
 const char *pCont;

              /*------------------------------------------------*\
              | Establish the section headers' and lines'        |
              | columns by adding the texts they are to contain. |
              | (2nd parm is the width while the 3rd is the      |
              | text's justification within the column. If no    |
              | width is given, the text's width is taken)       |
              \*------------------------------------------------*/
 MachHdr.add(Column(strMach));
 MachHdr.add(Column(strModel));
 MachHdr.add(Column(strSerNo));
 MachHdr.add(Column(strDesc,30,Column::Left));
 MachHdr.add(Column(strInstDate));               // CR28

 BLHdr.add(Column(strAction,16));
 BLHdr.add(Column(strQty));
 BLHdr.add(Column(strMach));
 BLHdr.add(Column(strModel));
 BLHdr.add(Column(strDesc,30,Column::Left));

 BLLine.add(Column(strAction,16));
 BLLine.add(Column(strQty));
 BLLine.add(Column(strFeat));
 BLLine.add(Column(strDesc,30,Column::Left));
// BLLine.add(Column(strCSDate,0,Column::Center,Column::Left));
 BLLine.add(Column(strCSDate));

 FeatLine.add(Column(strFeat));
 FeatLine.add(Column(strQty));
 FeatLine.add(Column(strDesc,30,Column::Left));
// FeatLine.add(Column(strInstDate,0,Column::Center,Column::Left));
 FeatLine.add(Column(strInstallDate));

 SWHdr.add(Column(strProgram));
 SWHdr.add(Column(strModel));
 SWHdr.add(Column(strInstallDate));
 SWHdr.add(Column(strDesc,30,Column::Left));

 RPOLine.add(Column(strFeat));
 RPOLine.add(Column(strQty));
 RPOLine.add(Column(strDesc,30,Column::Left));

 uT = Hdr.write(B,&Line,false,f);                // first header

    /*-----------------------------------------------------------------------*\
    |              Base Hardware Section                                      |
    |      Determine the continuation text according how                      |
    |        much space is available.                                         |
    |      Write the section subheader.                                       |
    |      For all machines to be processed:                                  |
    |        Set the machine's header texts                                   |
    |          (type, model, serial#, description and install date).          |
    |        If there not enough lines left to write some machine             |
    |          and feature lines, write a new page header after               |
    |          a pagebreak.                                                   |
    |        If this machine has features:                                    |
    |           Write the machine header with the Feature Line Header         |
    |             as subheader.                                               |
    |           For all the machine's features:                               |
    |             If there are less than 2 lines left:                        |
    |               Close the previous Feature Line, write a new page         |
    |                 header after a pagebreak and write the machine          |
    |                 header with the Feature Line Header as subheader.       |
    |             Set the Feature Line Texts                                  |
    |               (feature, base quantity, description etc)                 |
    |             Write the Feature Line Texts                                |
    |           Close the machine's last Feature Line.                        |
    |        Otherwise, write Machine header for this featureless             |
    |                   machine and close it off.                             |
    |        Skip a Line.                                                     |
    |      If there were no machines, write a centered 'None' line.           |
    \*-----------------------------------------------------------------------*/

 pCont = (PageW < ( MachHdr.totalWidth() + strContinued.length()))?
                       (const char *)strCont : (const char *)strContinued ;

 uT += Hdr.writeSubHeader("Base Hardware",B? B+uT : 0,&Line,Column::Center,f);

 Machine * pM;
 MFeat * pF;
 for ( n = 0 , N = ML.NumberElements(); n < N ; n++)
  if ( (pM = ML.element(n) ) && (pM->tag() == Machine::Process)) {
    str = IString(pM->installDate(date)? date.asString(strftimeFormat): strNA);
    MachHdr.setTexts(pM->type(),pM->model(),(char *)(pM->displaySerialNoStr()),
                     (char *)(pM->description()),(char *)str);
    if ( (PageL - Line) <= (MachHdr.rows() + 4))
      uT += Hdr.write(B? B+uT : 0,&Line,true,f); // true: insert pagebreak
    if ( J = pM->mfeatList().NumberElements()) {
      uT += MachHdr.Write(B? B+uT : 0,FeatLine,&Line,0,f);
      for (j = 0; j < J ; j++) {
        if ( Line >= (PageL-1)) {
          uT += FeatLine.closeOff(B? B+uT : 0,&Line,f);
                                                 // true: insert pagebreak
          uT += Hdr.write(B? B+uT : 0,&Line,true,f);
          uT += MachHdr.Write(B? B+uT : 0,FeatLine,&Line,pCont,f);
          }
        if ((pF = pM->mfeatList().element(j)) && pF->baseNmbr()) {
          str = IString(pF->installDate(date)?
                                        date.asString(strftimeFormat): strNA);
          FeatLine.setTexts( pF->feature()
                            ,(char *)IString(pF->baseNmbr())
//                            ,pF->description()
                            ,(char *)(IString::stripBlanks(IString(pF->description())))
//                            ,(char *)str);
                            ," ");               // CR28
          uT += (FeatLine.writeText(B? B+uT : 0,&Line,f));
          }
        }
      uT += FeatLine.closeOff(B? B+uT : 0,&Line,f);
      }
    else {
      uT += MachHdr.Write(B? B+uT : 0,&Line,0,f);
      uT += MachHdr.closeOff(B? B+uT : 0,&Line,f);
      }
    uT += skipLine(B? B+uT : 0,&Line,f);
    }
 if (!N)
   uT += Hdr.writeJustText((char *)strNone,B? B+uT : 0,&Line,Column::Center,f);

    /*-----------------------------------------------------------------------*\
    |                       RPOs Section                                      |
    |                                                                         |
    |      Use the Machine Header, but remove its Install Date Column         |
    |      Write the section subheader.                                       |
    |      For all the RPOs in the collection:                                |
    |          If the RPO's machine differs from the last (or is the first)   |
    |             Obtain the RPO's machine description from the machine       |
    |                by locating the machine using the RPO's                  |
    |                type, model and serial#                                  |
    |             Set the machine's header texts                              |
    |               (type, model, serial#, description).                      |
    |             If this is not the first RPO, close the previous RPO Line.  |
    |             If there are not enough lines left on the page to write     |
    |               a machine and some RPO Lines, write a new page header     |
    |               after a pagebreak.                                        |
    |             Write the machine header with the RPO Line Header           |
    |               as subheader.                                             |
    |          If there are less than 2 lines left:                           |
    |             Close the previous RPO Line, write a new page               |
    |                header after a pagebreak and write the machine           |
    |                header with the RPO Line Header as subheader.            |
    |          Create the RPO's quantity string ('+' or '-' prefixed).        |
    |          Set the RPO Line Texts (feature, quantity, description )       |
    |          Write the RPO Line Texts                                       |
    |          Close the RPO Line if its the last-one for this machine        |
    |      If there were no RPOs, write a centered 'None' line.               |
    |      Skip a Line.                                                       |
    \*-----------------------------------------------------------------------*/

 MachHdr.remove(*(MachHdr.element(4)));          // CR28

 if ( (PageL - Line) <= (RPOHdr.rows() + 8))
   uT += Hdr.write(B? B+uT : 0,&Line,true,f);
 uT += Hdr.writeSubHeader("Generated RPO's",B? B+uT : 0,&Line,Column::Center,f);

 RPO2 LastRPO;
 RPO2List::Cursor RPOCrsr(RPOL);
 for (n = 1, N = RPOL.numberOfElements(), RPOCrsr.setToFirst();
      RPOCrsr.isValid(); RPOCrsr.setToNext(), n++) {
   RPO2 & rpo = RPOL.elementAt(RPOCrsr);
   if (!rpo.sameMachine(LastRPO)) {
     LastRPO = rpo;
     str=IString((pM=ML.Find(rpo.machineType(),
                             rpo.machineModel(),
                             rpo.serialNum()))? pM->description() : IString());
     MachHdr.setTexts((char *)rpo.machineType(),(char *)rpo.machineModel(),
                      (char *)rpo.serialNum(),(char *)str);
     if (n != 1)                                 // not the first-one
       uT += RPOLine.closeOff(B? B+uT : 0,&Line,f);
     if ( (PageL - Line) <= (MachHdr.rows() + 4))
       uT += Hdr.write(B? B+uT : 0,&Line,true,f);
     uT += MachHdr.Write(B? B+uT : 0,RPOLine,&Line,0,f);
     }
   if ( Line >= (PageL-1)) {                     // < 2 lines left
     uT += RPOLine.closeOff(B? B+uT : 0,&Line,f);
     uT += Hdr.write(B? B+uT : 0,&Line,true,f);
     uT += MachHdr.Write(B? B+uT : 0,RPOLine,&Line,pCont,f);
     }
   i = rpo.signedQty();
   str = (i < 0)? IString(i) : IString('+') + IString(i);
   RPOLine.setTexts((char *)rpo.featureCode(),(char *)str,
                    (char *)rpo.description());
   uT += (RPOLine.writeText(B? B+uT : 0,&Line,f));
   if (n == N)                                   // machine's last
     uT += RPOLine.closeOff(B? B+uT : 0,&Line,f);
   }
 if (!N)                                         // no RPO's
   uT += Hdr.writeJustText((char *)strNone,B? B+uT : 0,&Line,Column::Center,f);
 uT += skipLine(B? B+uT : 0,&Line,f);

    /*------------------------------------------------------------------------*\
    |                   Backlog Section                                        |
    |      Determine the Backlog Header's continuation text according          |
    |         how much space is available.                                     |
    |      If there are less than 8 lines left on the page,                    |
    |        write a new page header after a pagebreak.                        |
    |      Write the section subheader.                                        |
    |      For all the elements in the Backlog collection:                     |
    |         If the Backlog's machine differs from the last (or is the first) |
    |            Obtain the object's quantity and action strings.              |
    |            Set the Backlog Header's second column text to either         |
    |              "Machine" or Program", depending on whether the             |
    |              object is a Hardware or Software backlog.                   |
    |            Set the Backlog Header's texts                                |
    |               (action, quantity,type, model and description).            |
    |            If this is not the first Backlog and the previous-one         |
    |               had features, close the previous Backlog Line.             |
    |            If there are less than 4 lines left on the page               |
    |               Write a new page header after a pagebreak.                 |
    |            Otherwise, skip a line.                                       |
    |            If the Backlog's machine has no features,                     |
    |               Write the Backlog Header and close it off.                 |
    |            Otherwise, write the Backlog Header with the                  |
    |               Backlog Line Header as subheader.                          |
    |         If the Backlog's machine has features:                           |
    |            If there are less than 2 lines left on the page               |
    |               Close of the Backlog Line.                                 |
    |               Write a new page header after a pagebreak.                 |
    |               Write the Backlog Header with an appended                  |
    |                  Continue Column.                                        |
    |            Obtain strings for the object's Action, Quantity,             |
    |               CSD Date and Description and set them as                   |
    |               the Backlog Line's texts.                                  |
    |            Write the Backlog Line Texts.                                 |
    |            Close the Blacklog Line if this is the collection's           |
    |               last element.                                              |
    |      If there were no Backlogs, write a centered 'None' line.            |
    |      Skip a Line.                                                        |
    \*------------------------------------------------------------------------*/

 pCont = (PageW < ( BLHdr.totalWidth() + strContinued.length()))?
                    (const char *)strCont : (const char *)strContinued ;

 if ( (PageL - Line) <= (BLHdr.rows() + 8))
   uT += Hdr.write(B? B+uT : 0,&Line,true,f);
 uT += Hdr.writeSubHeader("Backlog",B? B+uT : 0,&Line,Column::Center,f);

 BLFeatList & BLL = backlogList();
 Machine dummy;
 BLFeat LastBL(&dummy,0);
 BLFeatList::Cursor BLcrsr(BLL);
 for (n = 1, N = BLL.numberOfElements(), BLcrsr.setToFirst();
      BLcrsr.isValid(); BLcrsr.setToNext(), n++) {
   BLFeat & BL = BLL.elementAt(BLcrsr);
   if (!BL.sameMachine(LastBL)) {
     BL.machineQty_Action(strQ,strA);
     BLHdr.element(2)->setHeaderText(BL.isSoftware()? strProgram : strMach);
     BLHdr.setTexts((char *)strA,(char *)strQ,BL.machine(),BL.model(),
                    (char *)BL.machineDescription());
                                                 // close previous machine if
     if (n != 1)                                 // it has features
       if (!LastBL.isFeaturelessMachine())
         uT += BLLine.closeOff(B? B+uT : 0,&Line,f);
     if ( (PageL - Line) <= (BLHdr.rows() + 4))
       uT += Hdr.write(B? B+uT : 0,&Line,true,f);
     else uT += skipLine(B? B+uT : 0,&Line,f);
     if (BL.isFeaturelessMachine()) {
       uT += BLHdr.Write(B? B+uT : 0,&Line,0,f);
       uT += BLHdr.closeOff(B? B+uT : 0,&Line,f);
       }
     else uT += BLHdr.Write(B? B+uT : 0,BLLine,&Line,0,f);
     LastBL = BL;
     }                                           // a new machine
   if (!BL.isFeaturelessMachine()) {
     if ( Line >= (PageL-1)) {                   // no more room on page
       uT += BLLine.closeOff(B? B+uT : 0,&Line,f);
       uT += Hdr.write(B? B+uT : 0,&Line,true,f);
       uT += BLHdr.Write(B? B+uT : 0,BLLine,&Line,pCont,f);
       }
     BL.description(str2);
     str = IString( BL.csdDate(date)? date.asString(strftimeFormat) : strNA);
     BL.qty_Action(strQ,strA);
     BLLine.setTexts((char *)strA,(char *)strQ,BL.feature(),
                     (char *)str2,(char *)str);
     uT += (BLLine.writeText(B? B+uT : 0,&Line,f));
     if (n == N)
       uT += BLLine.closeOff(B? B+uT : 0,&Line,f);
     }                                           // machine with features
   }                                             // for all collection elements
 if (!N)
   uT += Hdr.writeJustText((char *)strNone,B? B+uT : 0,&Line,Column::Center,f);
 uT += skipLine(B? B+uT : 0,&Line,f);

    /*-----------------------------------------------------------------------*\
    |                   Software Discrepancies Section                        |
    |      Determine the Backlog Header's continuation text according         |
    |         how much space is available.                                    |
    |      If there are less than 8 lines left on the page,                   |
    |        write a new page header after a pagebreak.                       |
    |      Write the section subheader.                                       |
    |      For all the elements in the Discrepancies collection:              |
    |         Determine the object's Install Date string in the               |
    |           requested format ( or "N.A." if unavailable)                  |
    |         Set the Sofware Header's Texts:                                 |
    |           (type,model,install date and description)                     |
    |         If this is the first element                                    |
    |            Write the Header's Column headers only.                      |
    |         Otherwise, if there's less than 2 lines left:                   |
    |            Close off the current Header                                 |
    |            Write a new page header after a pagebreak.                   |
    |            Write the Header's Column headers only.                      |
    |         Write the Header's Texts.                                       |
    |         Close the Header if this is the collection's                    |
    |            last element.                                                |
    |      If there were no elements, write a centered 'None' line.           |
    |      Skip a Line.                                                       |
    \*-----------------------------------------------------------------------*/

 pCont = (PageW < ( SWHdr.totalWidth() + strContinued.length()))?
                       (const char *)strCont : (const char *)strContinued ;

 if ( (PageL - Line) <= (SWHdr.rows() + 8))
    uT += Hdr.write(B? B+uT : 0,&Line,true,f);
 uT += Hdr.writeSubHeader("Software Discrepancies",B? B+uT : 0,&Line,
                          Column::Center,f);

 SoftMachList::Cursor SWDcrsr(SWDiscList);
 for (n = 1, N = SWDiscList.numberOfElements(), SWDcrsr.setToFirst();
      SWDcrsr.isValid(); SWDcrsr.setToNext(), n++) {
   SoftMach & SM = SWDiscList.elementAt(SWDcrsr);
   str = (SM.date() == SoftMach::nonDate())?
                             strNA : SM.date().asString(strftimeFormat);
   SWHdr.setTexts(SM.type(),SM.model(),(char *)str ,SM.description());
//                   ,(char *)SM.date().asString(strftimeFormat)
   if (n == 1)
     uT += SWHdr.WriteHdrOnly(B? B+uT : 0,&Line,0,f);
   else if ( Line >= (PageL-1)) {
          uT += SWHdr.closeOff(B? B+uT : 0,&Line,f);
          uT += Hdr.write(B? B+uT : 0,&Line,true,f);
          uT += SWHdr.WriteHdrOnly(B? B+uT : 0,&Line,pCont,f);
          }
   SWHdr.writeText(B? B+uT : 0,&Line,f);
   if (n == N)
     uT += SWHdr.closeOff(B? B+uT : 0,&Line,f);
   }                                             // for all elements
 if (!N)
   uT += Hdr.writeJustText((char *)strNone,B? B+uT : 0,&Line,Column::Center,f);
 uT += skipLine(B? B+uT : 0,&Line,f);

#if 0
    /*-----------------------------------------------------------------------*\
    |                       Software Inventory                                |
    |      (Written out as single feateureless Programs, using                |
    |       logic similar to that of the Software Discrepancies)              |
    \*-----------------------------------------------------------------------*/
 pCont = (PageW < ( SWHdr.totalWidth() + strContinued.length()))?
                       (const char *)strCont : (const char *)strContinued ;

 if ( (PageL - Line) <= (SWHdr.rows() + 8))
    uT += Hdr.write(B? B+uT : 0,&Line,true,f);
 uT += Hdr.writeSubHeader("Software Inventory",B? B+uT : 0,&Line,
                          Column::Center,f);

 SoftMachList SWIList;
 getSWDescListf(false,SWIList);
 SoftMachList::Cursor SWIcrsr(SWIList);
 for (n = 1, N = SWIList.numberOfElements(), SWIcrsr.setToFirst();
      SWIcrsr.isValid(); SWIcrsr.setToNext(), n++)
   { SoftMach & SM = SWIList.elementAt(SWIcrsr);
     str = (SM.date() == SoftMach::nonDate())?
                             strNA : SM.date().asString(strftimeFormat);
     SWHdr.setTexts(SM.type(),SM.model(),(char *)str,SM.description());
     if (n == 1)
        uT += SWHdr.WriteHdrOnly(B? B+uT : 0,&Line,0,f);
     else if ( Line >= (PageL-1))
            { uT += SWHdr.closeOff(B? B+uT : 0,&Line,f);
              uT += Hdr.write(B? B+uT : 0,&Line,true,f);
              uT += SWHdr.WriteHdrOnly(B? B+uT : 0,&Line,pCont,f);
           }
     SWHdr.writeText(B? B+uT : 0,&Line,f);
     if (n == N)
       uT += SWHdr.closeOff(B? B+uT : 0,&Line,f);
   }
 if (!N)
   uT += Hdr.writeJustText((char *)strNone,B? B+uT : 0,&Line,Column::Center,f);

#endif

    /*-----------------------------------------------------------------------*\
    |                       Software Inventory                                |
    |      Use the Machine Header as the Software Header after                |
    |         restoring its Install Date Column.                              |
    |      If there are less than 8 lines left on the page,                   |
    |        write a new page header after a pagebreak.                       |
    |      Write the section subheader.                                       |
    |      Get a Collection of Invenoried Software.                           |
    |      If the collection's empty:                                         |
    |         Write a centered 'None' line.                                   |
    |      Otherwise,                                                         |
    |         Determine the Software Header's continuation text according     |
    |            how much space is available.                                 |
    |         Set the Software Header 'Machine' Column's text to "Program".   |
    |         For all the elements in the collection:                         |
    |            If the Software's program differs from the last              |
    |                    (or is the first):                                   |
    |               Determine the object's Install Date.                      |
    |               Set the Software Header's texts                           |
    |                 (type, model,serial#, description and date).            |
    |               If this is not the first object and the previous-one      |
    |                  had features, close the previous Software Line.        |
    |               If there are less than 4 lines left on the page           |
    |                  Write a new page header after a pagebreak.             |
    |               Otherwise, skip a line.                                   |
    |               If the object's program has no features,                  |
    |                  Write the Software Header and close it off.            |
    |               Otherwise, write the Software Header with the             |
    |                  Software Line Header as subheader.                     |
    |            If the Software's program has features:                      |
    |               If there are less than 2 lines left on the page           |
    |                  Close of the Software Line.                            |
    |                  Write a new page header after a pagebreak.             |
    |                  Write the Software Header with an appended             |
    |                     Continue Column.                                    |
    |               Obtain strings for the object's Description and           |
    |                  Install Date.                                          |
    |               Set the Software Line's Texts                             |
    |                  ( feature, AAS quantity, description)                  |
    |               Write the Software Line Texts.                            |
    |               Close the Software Line if this is the                    |
    |                  collection's last element.                             |
    \*-----------------------------------------------------------------------*/

 MachHdr.add(Column(strInstDate));               // CR28

 if ( (PageL - Line) <= (BLHdr.rows() + 8))
   uT += Hdr.write(B? B+uT : 0,&Line,true,f);
 uT += Hdr.writeSubHeader("Software Inventory",B? B+uT : 0,&Line,
                          Column::Center,f);

 FeatList SWIL;
 if (getInventorySWList(false,SWIL)) {
   Machine dummy;
   Feat LastSWI(&dummy,0,0);
   MachHdr.element(0)->setHeaderText(strProgram);
   pCont = (PageW < ( MachHdr.totalWidth() + strContinued.length()))?
                      (const char *)strCont : (const char *)strContinued ;
   FeatList::Cursor SWILcrsr(SWIL);
   for (n = 1, N = SWIL.numberOfElements(), SWILcrsr.setToFirst();
        SWILcrsr.isValid(); SWILcrsr.setToNext(), n++) {
     Feat & SWI = SWIL.elementAt(SWILcrsr);
     if (!SWI.sameMachine(LastSWI)) {
       str = IString(SWI.MachineInstallDate(date)?
                     date.asString(strftimeFormat): strNA);
       MachHdr.setTexts(SWI.machine(),SWI.model(),
                        (char *)(SWI.displaySerialNoStr()),
                        (char *)(SWI.machineDescStr()),(char *)str);
                                                 // close previous machine if
       if (n != 1)                               // it has features
         if (!LastSWI.isFeaturelessMachine())
           uT += FeatLine.closeOff(B? B+uT : 0,&Line,f);
       if ( (PageL - Line) <= (MachHdr.rows() + 4))
         uT += Hdr.write(B? B+uT : 0,&Line,true,f);
       else uT += skipLine(B? B+uT : 0,&Line,f);
       if (SWI.isFeaturelessMachine()) {
         uT += MachHdr.Write(B? B+uT : 0,&Line,0,f);
         uT += MachHdr.closeOff(B? B+uT : 0,&Line,f);
         }
       else uT += MachHdr.Write(B? B+uT : 0,FeatLine,&Line,0,f);
       LastSWI = SWI;
       }
     if (!SWI.isFeaturelessMachine()) {
       if ( Line >= (PageL-1)) {                 // no more room on this page
         uT += FeatLine.closeOff(B? B+uT : 0,&Line,f);
         uT += Hdr.write(B? B+uT : 0,&Line,true,f);
         uT += MachHdr.Write(B? B+uT : 0,FeatLine,&Line,pCont,f);
         }
       SWI.description(str2);
//       str = IString( SWI.installDate(date)?   // CR28
//                      date.asString(strftimeFormat) : strNA);
       FeatLine.setTexts(SWI.feature(),(char *)IString(SWI.qtyAAS())
                        ,(char *)str2
//                        ,(char *)str);  // CR28
                        ," ");                   // CR28
       uT += (FeatLine.writeText(B? B+uT : 0,&Line,f));
       if (n == N)
         uT += FeatLine.closeOff(B? B+uT : 0,&Line,f);
       }                                         // program with features
     }                                           // for all collection elements
   }
 else uT += Hdr.writeJustText((char *)strNone,B? B+uT : 0,&Line,Column::Center,f);

        /*---------------------------------------------------*\
        |            Null-terminate File or Buffer            |
        \*---------------------------------------------------*/
 if (f)
   uT++,fputs("\x00",f);
 else *(B+uT++) = '\0';
 return uT;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
 /*--------------------------------------------------------------------------*\
 |                           Compiled testing code                            |
 \*--------------------------------------------------------------------------*/

void RMSystem :: SystemOut()
{

 SysDataOut(cout);
 cout << endl;
 cout << ML;
 cout << endl;
// cout << XPL;
 XPL.out(cout);
 cout << endl;
 cout << RPOL;
 cout << endl;
}

ostream& RMSystem::SysDataOut(ostream& os)
{
 os   << "                 ----> System Data <---- " << endl
      << "Name(AAS)      :'"  << pSD->Name[0] << "' " << endl
      << "Name(MRDB)     :'" << pSD->Name[1] << "' " << endl
      << "Title(AAS)     :'"  << pSD->Title[0] << "' " << endl
      << "Title(MRDB)    :'" << pSD->Title[1] << "' " << endl
      << "FileDtw(AAS)    :'" << pSD->FileDate[0] << "' " << endl
      << "FileDtw(MRDB)    :'" << pSD->FileDate[1] << "' " << endl
      << "UserId(AAS)    :'"  << pSD->UserId[0] << "' " << endl
      << "UserId(MRDB)   :'" << pSD->UserId[1] << "' "
      << endl;

 os   << "Number(AAS)    :'"  << pSD->Number[0] << "' " << endl
      << "Number(MRDB)   :'" << pSD->Number[1] << "' " << endl
      << "DateRec53(AAS)  :'"  << pSD->DateRec53[0] << "' " << endl
      << "DateRec53(MRDB) :'" << pSD->DateRec53[1] << "' " << endl
      << "Security(AAS)  : "  << pSD->iSecurity[0] << " " << endl
      << "Security(MRDB) : " << pSD->iSecurity[1]
      << endl;

 os   << "TotalMachs     : "  << pSD->uTotalMachs  << " " << endl
      << "TotalFeats     : "  << pSD->uTotalFeats << " " << endl
      << "TotalCFRecs    : "  << pSD->uTotalCFRecs << endl;

 return os;
}


 /*--------------------------------------------------------------------------*\
 |                         Non-Compiled testing code                          |
 \*--------------------------------------------------------------------------*/
#ifdef UNITTEST

#include <iostream.h>


void MachineOut(int n,Machine &m)
{
 MFeat * pF;
 CFRecV * pR;
 int i,j,k,l;
 cout << endl
      << "Machine "
      << n
      << " : "
      << m.type()
      << "  "
      << m.model()
      << "  "
      << m.serialNo()
      << endl;

 MFeatList  & MFL = m.mfeatList();
 j = MFL.NumberElements();
 for (i = 0; i < j ; i++)
   {
    pF = (MFeat *)MFL[i];
    cout << "Feature : "
         << pF->feature()
         << "  AAS qty  "
         << pF->qty(MFeat::AASType)
         << "  MRDB qty  "
         << pF->qty(MFeat::MRDBType)
         << endl;

    cout << "AAS List only: " << endl;

   CFRecVL & CFRL = pF->cfrList(MFeat::AASType);
   l = CFRL.NumberElements();
   for (k = 0; k < l ; k++)
     {
      pR = (CFRecV *)CFRL[k];
      cout << "CFRec : "
           << pR->id()
           << "  Data : "
           << pR->record()
           << endl;
     }
   }
}



int main(int argc,char **argv)
{
 char Buffer[20];
 if (argc < 3)
  { cout << "Usage : System AASFile MDBFile [MDBDescription File]";
    exit(1);
  }
 if (1)
   {
     Boolean fOK;
     RMSystem Sys;
     cout << "System name  "
           << Sys.name()
           << endl;
     mystrncpy(Buffer,(argc > 3)? argv[3] : "TST13_17.DSC",15);
     Sys.re_Set(argv[1],argv[2],Buffer,"EUOENU.MSG");

     Sys.getPK(true);
     Sys.readCFRFile(false);
     Sys.readCFRFile(true);
//     Sys.StubXPLN();
//     Sys.StubRPOL();
     Sys.applyPK();
     Sys.resolveMatches();
     Sys.resolveGenerics();
     Sys.resolveMRCombos();
     Sys.resolveNonEquals();
     Sys.resolveOthers();
     Sys.generateFeatList();
     Sys.savePK();

     cout << "FeatList : " << endl;
     cout << Sys.featlist() << endl;
     cout << "PlacementList : " << endl;
     cout << Sys.l55() << endl;
     cout << "56List : " << endl;
     cout << Sys.l56() << endl;
     Sys.SystemOut();

     Sys.fWriteCFR(MakeFileName(Buffer,argv[1],"NW1"),1,1);
     Sys.saveState(MakeFileName(Buffer,argv[1],"$$$"));

     if (Sys.reBuildState(Buffer))
       if (Sys.getPK())
          {
            cout << "FeatList : " << endl;
            cout << Sys.featlist() << endl;
            cout << "PlacementList : " << endl;
            cout << Sys.l55() << endl;
            cout << Sys.l56() << endl;
            Sys.fWriteCFR(MakeFileName(Buffer,argv[1],"NW2"),1,1);
            Sys.SystemOut();
          }
#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif
   }

// delete pSys;
#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif
  exit(0);
}


#endif


#if 0
 /*--------------------------------------------------------------------------*\
 |                                 Test Stubs                                 |
 \*--------------------------------------------------------------------------*/
void RMSystem::StubRPOL()
{
 RPO2 r1( "Typ1","Mod1","Serial1","Feat2","Description 1",1,'A');
 RPO2 r2( "Typ2","Mod2","Serial2","Feat3","Description 2",2,'B');
 RPO2 r3( "Typ3","Mod3","Serial3","Feat4","Description 3",3,'C');
 RPO2 r4( "Typ4","Mod4","Serial4","Feat5","Description 4",4,'D');
 RPO2 r5( "Typ5","Mod5","Serial5","Feat6","Description 5",5,'E');
 RPO2 r6( "Typ6","Mod6","Serial6","Feat7","Description 6",6,'F');
 RPO2 r7( "Typ7","Mod7","Serial7","Feat8","Description 7",7,'G');
 RPOL.add(r5);
 RPOL.add(r7);
 RPOL.add(r3);
 RPOL.add(r2);
 RPOL.add(r4);
 RPOL.add(r1);
 RPOL.add(r6);
 RPOL.SortUp();
}

void RMSystem::StubXPLN()
{
    XPLN2 x1("Mach 1","Mod 1","Ser 1","Feat 1","Text 1");
    XPLN2 x2("Mach 2","Mod 2","Ser 2","Feat 2","Text 2");
    XPLN2 x3("Mach 3","Mod 1","Ser 1","Feat 3","Text 1");
    XPLN2 x4("Mach 2","Mod 2","Ser 2","Feat 4","Text 2");
    XPLN2 x5("Mach 1","Mod 1","Ser 1","Feat 5","Text 1");
    XPLN2 x6("Mach 2","Mod 2","Ser 2","Feat 6","Text 2");
    XPLN2 x7("Mach 3","Mod 1","Ser 1","Feat 7","Text 1");
    XPLN2 x8("Mach 2","Mod 2","Ser 2","Feat 8","Text 2");
    XPLN2 x9("Mach 1",0);
    XPL.add(x8);
    XPL.add(x7);
    XPL.add(x6);
    XPL.add(x5);
    XPL.add(x4);
    XPL.add(x3);
    XPL.add(x2);
    XPL.add(x1);
    XPL.add(x9);
}
#endif
