
// CR26 3-30-95 [AB] Workaround date logic for AAS-only machine readable feat
//      4-28-98 edw - Added logic to correctly added 54, 55, 56 records to CFReport
 /*--------------------------------------------------------------------------*\
 |    Filename - MACH.CPP                                                     |
 |      contains source for the following RM/2 class implementations:         |
 |                                                                            |
 |     Machine    - A CFReport Machine object.                                |
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
 \*--------------------------------------------------------------------------*/
//Modification Log:
// edw       05/10/99      fWriteCFRV31-Modified to write V20 Information Record
//                         (first 54 record).
// hecc (66806) 11/21/2001 When reconciliation assigns a the default quantity
//                         on the mismatches panel, it will be taken from the
//                         MRPD quantity from the .mrr file, instead of use
//                         the AAS quantity from the CFREPORT.
// hecc (F72273) 06/28/2002  Define the Ctors for the RT 17 contained in the MRDB file.
// edw  (F73732) 07/25/2002  Completion of F72273 (iSeries SW reconciliation support:
//                           1) The tag for SIPO records updated in fWriteCFRV31 because
//                              they are being temporarily changed to 'Hardware' to allow these SW
//                              records to be reconciled like hardware.
//                           2) Updated the checkMachineRecQuantities method so that
//                              the machine record for RT17s are also updated.  This will
//                              eliminate the need for doing this in RMSystem::Process_17_Record
//                              as the checkMachineRecQuantities method is the proper place
//                              for this functionality
// VBB  09/19/2002 D74157  Add code to allow transfer of 19 records in AAS report through
//                         reconciliation.
// HECC 10/18/2002 F73686  Added necessary logic to call to the two methods description()
//                         and category() wich will return the Machine description and
//                         the category (Hw/Sw) of the machine.
// HECC 07/28/2003 D83402  Added necessary logic to call the four additional methods: customerNumber(),
//                         systNumberFlag(), systemType() and systemNum() which will return:
//                         the customer number, flag system number, system type and system number.
// ZOR  11/19/2004 D95997  add 06 records in OtherList object
// HECC 11/11/2005 D104285   Add code to avoid process of those machines that were already
//                           reconciled in Inventory Services, which tag is Process_IS, as
//                           the process of these causes an inaccurate RT98 in .RET file




        /*---------------------------------------------------*\
        |                      Includes                       |
        \*---------------------------------------------------*/
#include <isynonym.hpp>                          // Definition of Boolean etc

        /*---------------------------------------------------*\
        |                   ICLUI Includes                    |
        \*---------------------------------------------------*/
#define INCL_IMESSAGETEXT
#include "uicl.hpp"

        /*---------------------------------------------------*\
        |               RM/2-specific includes                |
        \*---------------------------------------------------*/
#include "euodef.h"                              // CFReport declarations
#include "euoutil.hpp"                           // utilities
#include "rpo2l.hpp"                             // RPO List
#include "xpln2l.hpp"                            // Explanation List
#include "mach.hpp"                              // this file includes
                                                 // CFReport's 08 Record
#include "cfr08.hpp"                             // objects
//#include "cfr08V31.hpp"                // CFReport's Version 31 08-Record objects
#include "cfr95.hpp"                             // CFReport's Record-95 object
                                                 // CFRReport's Software
#include "cfrsw.hpp"                             // objects
#include "euomsg.h"                              // Message Ids


 /*--------------------------------------------------------------------------*\
 |                               Machine Class                                |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |                        Ctors                        |
        \*---------------------------------------------------*/
Machine::Machine(Record_47 * p47, const char * serialNo)
{
 mystrstrip(mystrncpy(Type,p47->Type,ALMIN(sizeof(p47->Type),sizeof(Type)-1)));
 mystrstrip(mystrncpy(Mod,p47->Model,ALMIN(sizeof(p47->Model),sizeof(Mod)-1)));
 mystrstrip(mystrncpy(SerNo,serialNo,sizeof(SerNo)-1));
 subInit(Machine::Software);
}

Machine::Machine(Record_49 * p49, const char * serialNo)
{
 mystrstrip(mystrncpy(Type,p49->Type,ALMIN(sizeof(p49->Type),sizeof(Type)-1)));
 mystrstrip(mystrncpy(Mod,p49->Model,ALMIN(sizeof(p49->Model),sizeof(Mod)-1)));
 mystrstrip(mystrncpy(SerNo,serialNo,sizeof(SerNo)-1));
 subInit(Machine::Software);
}

Machine::Machine(Record_SW_V31 * pSW, const char * serialNo)
{
 mystrstrip(mystrncpy(Type,pSW->Type,ALMIN(sizeof(pSW->Type),sizeof(Type)-1)));
 mystrstrip(mystrncpy(Mod,pSW->Model,ALMIN(sizeof(pSW->Model),sizeof(Mod)-1)));
 mystrstrip(mystrncpy(SerNo,serialNo,sizeof(SerNo)-1));
 subInit(Machine::Software);
}

Machine::Machine(Record_08V31 * p831, const char * serialNo)
{
 mystrstrip(mystrncpy(Type,p831->Machine,ALMIN(sizeof(p831->Machine),sizeof(Type)-1)));
 mystrstrip(mystrncpy(Mod,p831->Model,ALMIN(sizeof(p831->Model),sizeof(Mod)-1)));
 mystrstrip(mystrncpy(SerNo,serialNo,sizeof(SerNo)-1));
 subInit(Machine::Process);
}

/* F86750  Begin logic: Arguments added for a RT 38 to store  */
/* the services records.                     HECC: 01/13/2004 */
Machine::Machine(Record_38_V31 * p38V31, const char * serialNo)
{
 mystrstrip(mystrncpy(Type,p38V31->Machine,ALMIN(sizeof(p38V31->Machine),sizeof(Type)-1)));
 mystrstrip(mystrncpy(Mod,p38V31->Model,ALMIN(sizeof(p38V31->Model),sizeof(Mod)-1)));
 mystrstrip(mystrncpy(SerNo,serialNo,sizeof(SerNo)-1));
 subInit(Machine::Services);
}
/* F86750  End logic:   HECC: 01/13/2004 */

Machine::Machine(Record_08 * p8, const char * serialNo)
{
 mystrstrip(mystrncpy(Type,p8->Machine,ALMIN(sizeof(p8->Machine),sizeof(Type)-1)));
 mystrstrip(mystrncpy(Mod,p8->Model,ALMIN(sizeof(p8->Model),sizeof(Mod)-1)));
 mystrstrip(mystrncpy(SerNo,serialNo,sizeof(SerNo)-1));
 subInit(Machine::Process);
}

Machine::Machine(Record_54 * p54)
{
 mystrstrip(mystrncpy(Type,p54->MachType,ALMIN(sizeof(p54->MachType),sizeof(Type)-1)));
 mystrstrip(mystrncpy(Mod,p54->MachMod,ALMIN(sizeof(p54->MachMod),sizeof(Mod)-1)));
 mystrstrip(mystrncpy(SerNo,p54->SerialNo,ALMIN(sizeof(p54->SerialNo),sizeof(SerNo)-1)));
 subInit(Machine::Process);
}

/* F72273  Begin: Arguments added for a RT 17 to store the SW */
/* records in the MRDB file.                 HECC: 06/28/2002 */

Machine::Machine(Record_17 * p17, const char * serialNo)
{
 mystrstrip(mystrncpy(Type,p17->Type,ALMIN(sizeof(p17->Type),sizeof(Type)-1)));
 mystrstrip(mystrncpy(Mod,p17->Model,ALMIN(sizeof(p17->Model),sizeof(Mod)-1)));
 mystrstrip(mystrncpy(SerNo,serialNo,sizeof(SerNo)-1));
 subInit(Machine::Software);
}
/* F72273  End: Arguments added for a RT 17 to store the SW */


    /*-----------------------------------------------------------------------*\
    |    void init()                                                          |
    |       Private default initializer use dby Ctors                         |
    \*-----------------------------------------------------------------------*/
void Machine::init(const char * szType,const char * szModel,const char * szSerialNo)
{
 mystrstrip(mystrncpy(Type,szType,sizeof(Type)-1));
 mystrstrip(mystrncpy(Mod,szModel,sizeof(Mod)-1));
 mystrstrip(mystrncpy(SerNo,szSerialNo,sizeof(SerNo)-1));
 subInit(Machine::Process);
}

void Machine::subInit(TagType type)
{
 Tag = type;
 cIPProdInd = ' ';
 fMsg = fFirstMRDB = bIsSIPO = false;                       //VBB09192002 D74157
 Nmbr = 0;
 memset(SubsysRecId,0,sizeof(SubsysRecId));
}

void Machine::setSerialNo(char * SerialNo)
{
 mystrstrip(mystrncpy(SerNo,SerialNo,sizeof(SerNo)-1));
}

Boolean Machine::setIPProdInd(char Ind)
{
 if (cIPProdInd != ' ') {                        // only do it once
   cIPProdInd = Ind;
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   int compareByAASSubsystem(Machine const& other) const                 |
    |       compare alphabetically after taking into account the first        |
    |         MRDB file's first machine and the AAS Subsystem Records Ids.    |
    \*-----------------------------------------------------------------------*/
int Machine :: compareBySubsystem(Machine const& other,Boolean fMRDB ) const
{
 int i;
 if (!isFirstMRDB())
   if (!other.isFirstMRDB())
     if (subsystemRecordId(fMRDB))
       if (other.subsystemRecordId(fMRDB))
         if (!(i=subsystemRecordId(fMRDB)-other.subsystemRecordId(fMRDB)))
           if (!(i=compareByType(other)))
             return compare(other);
           else return i;
         else return i;
       else return -1;                           // other is bigger
     else                                        // I've no subsystem
       if (!other.subsystemRecordId(fMRDB))
         if (!(i=compareByType(other)))
           return compare(other);
         else return i;
       else return 1;                            // I'm bigger
   else return 1;                                // other is first
 else return other.isFirstMRDB()? 0 : -1;
}

    /*-----------------------------------------------------------------------*\
    |   int compareByType(Machine const& other) const                         |
    |       compare Hardware before any other type                            |
    \*-----------------------------------------------------------------------*/
int Machine :: compareByType(Machine const& other) const
{
if ((Tag == Backlog_HW) && ((other.Tag == Software) || other.bIsSIPO))          //VBB09192002 D74157
   return -1;                                                                   //VBB09192002 D74157
else if ((other.Tag == Backlog_HW) && ((Tag == Software) || bIsSIPO))           //VBB09192002 D74157
   return 1;                                                                    //VBB09192002 D74157
else                                                                            //VBB09192002 D74157
   return Tag - other.Tag;
}

    /*-----------------------------------------------------------------------*\
    |    int compare()                                                        |
    |       compare alphabetically in the parameter order, disregarding       |
    |       the first 5 bytes of the serial #.                                |
    \*-----------------------------------------------------------------------*/
int Machine::compare( const char * szType
                     ,const char * szModel
                     ,const char * szSerialNo) const
{
  int i = strcmp(Type,szType);
  //if (!i && !(i = strcmp(Mod,szModel)) && szSerialNo)       /* F73732 EDW 07/25/02 */
  if (!i && !(i = strcmp(Mod,szModel)) && strlen(szSerialNo)) /* F73732 EDW 07/25/02 */
//    i = strcmp(SerNo,szSerialNo);
    i = compSerialNo(SerNo,szSerialNo);
  return i;
}

    /*-----------------------------------------------------------------------*\
    |    int compare()                                                        |
    |       compare alphabetically in the parameter order, disregarding       |
    |       the first 5 bytes of the serial #.                                |
    \*-----------------------------------------------------------------------*/
int Machine::compareTypeSerial( const char * szType
                               ,const char * szSerialNo) const
{
  int i = strcmp(Type,szType);
  if (!i)
    i = compSerialNo(SerNo,szSerialNo);
  return i;
}

    /*-------------------------------------------------------*\
    |                       Assignment                        |
    \*-------------------------------------------------------*/
Machine & Machine::operator=(Machine const& other)
{
 int i,I;
 if (&other != this) {
   BaseInit(other);                              // copies all but Lists
   MFL = other.MFL;
   for ( i = 0 ; i < MACH_NMBR_CFR_LISTS ; i++ )
     CFRLists[i] = other.CFRLists[i];
   for (i=0,I=(sizeof(MachRecs)/sizeof(MachRecs[0])) ; i < I ; i++)
     MachRecs[i] = other.MachRecs[i];
   }
 return *this;
}

    /*-----------------------------------------------------------------------*\
    |    void BaseInit(MFeat const& other)                                    |
    |       Copies all but the CFReport Record Lists                          |
    |       and List of Features                                              |
    \*-----------------------------------------------------------------------*/
void Machine::BaseInit(Machine const& other)
{
 mystrncpy(Type,other.Type,sizeof(Type)-1);
 mystrncpy(Mod,other.Mod,sizeof(Mod)-1);
 mystrncpy(SerNo,other.SerNo,sizeof(SerNo)-1);
 cIPProdInd = other.cIPProdInd;
 Tag = other.Tag;
 for (int i = 0,I = (sizeof(CFRStoreInfo)/sizeof(CFRStoreInfo[0])); i < I ;i++)
   CFRStoreInfo[i] = other.CFRStoreInfo[i];
 FeatStoreInfo = other.FeatStoreInfo;
 fMsg = other.fMsg;
 fFirstMRDB = other.fFirstMRDB;
 Nmbr = other.Nmbr;
 memcpy(SubsysRecId,other.SubsysRecId,sizeof(SubsysRecId));
}

    /*-----------------------------------------------------------------------*\
    |   Boolean re_Move(Machine * pToM,Boolean fMRDB,int version,             |
    |       Move the CFReport records                                         |
    |       of the given type to that of the given machine, or                |
    |       eliminate them if no feature is given.                            |
    |       Keep the given machine's CFReport record (e.g. 08,19,47 etc)      |
    |       in sync with its list of features which might have grown          |
    |       as a result.                                                      |
    \*-----------------------------------------------------------------------*/

// new version to replace re_move in anticipation for V31
Boolean Machine::copyRecords(Machine * pToM,Boolean fMRDB_To,Boolean fMRDB_From,
                             int toVersion, Boolean fMove,
                             Boolean fUpdateFeatureCount)
{
 CFRecV * pRec;
 int i,I,qty;
 if ((this == pToM) && (fMRDB_From == fMRDB_To))
   return false;
 if (MachRecs[fMRDB_From].length()) {
   CFRecVL & ListInfo = infoRecList(fMRDB_From);
   CFRecVL & ListOther = otherRecList(fMRDB_From);
                /*-----------------------------------------*\
                | Don't copy the other records              |
                | (e.g.Comments) for which the Comment      |
                | records versions are incompatible         |
                \*-----------------------------------------*/
   Boolean fCopyOtherRecords = true;
   if (pToM) {
     switch (machineRecVersion(fMRDB_From)) {
       case 20:
       case 30:
         switch(toVersion) {
           case 20:
           case 30:                              // V30 -> V30
             pToM->setMachineRec(fMRDB_To,(char *)MachRecs[fMRDB_From].record(),
                                 toVersion);

             if (pRec = ListInfo.element(0))
               pToM->addInfoRec(fMRDB_To,*pRec); // will create 'newed' ones

             break;
           case 31: {                            // V30 -> V31
             fCopyOtherRecords = false;
             if (pRec = ListInfo.element(0))
               pToM->addInfoRec(fMRDB_To,
                           (const char *)CFR95((Record_54 *)(pRec->record())));
             if (Tag != Software) {
               CFR08V31 cfr8((Record_08 *)MachRecs[fMRDB_From].record());
               pToM->setMachineRec(fMRDB_To,cfr8,toVersion);
               }
             else {
               const char * pch = MachRecs[fMRDB_From].record();
               switch (CFRecType(pch)) {
                 case 17: {
                   CFR47 cfr47((Record_17 *)pch);
                   CFR_SW_V31 cfrSW((Record_47 *)cfr47.record());
                   pToM->setMachineRec(fMRDB_To,cfrSW,toVersion);
                   break;
                   }
                 case 47: {
                   CFR_SW_V31 cfrSW((Record_47 *)pch);
                   pToM->setMachineRec(fMRDB_To,cfrSW,toVersion);
                   break;
                   }
                 case 49: {
                   CFR_SW_V31 cfrSW((Record_49 *)pch);
                   pToM->setMachineRec(fMRDB_To,cfrSW,toVersion);
                   break;
                   }
                 }
               }
             break;
             }
           }
         break;
       case 31:
         switch(toVersion) {
           case 31:                              // V31 -> V31
             pToM->setMachineRec(fMRDB_To,(char *)MachRecs[fMRDB_From].record(),
                                 toVersion);
             for (i = 0 , I = ListInfo.NumberElements(); i < I ; i++)
               if (pRec = ListInfo.element(i))
                 pToM->addInfoRec(fMRDB_To,*pRec);
             break;
           case 20:
           case 30: {                            // V31 -> V30
             fCopyOtherRecords = false;
             if (Tag != Software) {
               CFR08 rfr8((Record_08V31 *)MachRecs[fMRDB_From].record());
               pToM->setMachineRec( fMRDB_To,rfr8,toVersion);
               }
             else {
               const char * pch = MachRecs[fMRDB_From].record();
               switch (CFRecType(pch)) {
                 case 17: {
                   CFR47 cfr47((Record_SW_V31 *)pch);
                   CFR17 cfr17((Record_47 *)cfr47.record());
                   pToM->setMachineRec(fMRDB_To,cfr17,toVersion);
                   break;
                   }
                 case 47: {
                   CFR47 cfr47((Record_SW_V31 *)pch);
                   pToM->setMachineRec(fMRDB_To,cfr47,toVersion);
                   break;
                   }
                 case 49: {
                   CFR49 cfr49((Record_SW_V31 *)pch);
                   pToM->setMachineRec(fMRDB_To,cfr49,toVersion);
                   break;
                   }
                 }
               }
             if (pRec = ListInfo.element(0))
               pToM->addInfoRec(fMRDB_To,
                                (const char *)CFR54((Record_95 *)(pRec->record())));
             break;
             }
           }
         break;
       }

     if (fCopyOtherRecords)
       for (i = 0 , I = ListOther.NumberElements(); i < I ; i++)
         if (pRec = ListOther.element(i))
           pToM->addOtherRec(fMRDB_To,*pRec);

     }
   if (fMove) {
     MachRecs[fMRDB_From] = CFRecV();            // empty this machines' out

     for (i = 0 , I = ListInfo.NumberElements(); i < I ; i++)
       if (pRec = (CFRecV *)ListInfo.Pop())
         delete pRec;

     for (i = 0 , I = ListOther.NumberElements(); i < I ; i++)
       if (pRec = (CFRecV *)ListOther.Pop())
         delete pRec;
     }
                                                 // do the features
   MFeat::CFRType toType = fMRDB_To? MFeat::MRDBType : MFeat::AASType;
   MFeat::CFRType fromType = fMRDB_From? MFeat::MRDBType : MFeat::AASType;
   MFeat * pF, *pToF;
   for (i=0,I=MFL.NumberElements(); i < I ; i++) // do the features
     if ( pF = MFL.element(i))
       if (qty = pF->qty(fromType)) {
         if (pToM) {
           if (!(pToF = pToM->mfeatList().Find(*pF)))
             pToM->mfeatList().Add(pToF = new MFeat(pF->feature()));
           }
         else pToF = 0;
         pF->copyRecords(pToF,qty,toType,fromType,toVersion,fMove,
                         fUpdateFeatureCount,type(),model(),serialNo());
         }

// if (pToM)                       // re-sync its Machine Record
//   pToM->checkMachRecQuantities(fMRDB_To,tellem,0,false);
   return true;
   }
 return false;
}
//#endif

CFRecVL & Machine :: infoRecList(Boolean fMRDB)
{
 return CFRLists[fMRDB? MACH_MRDB_INFO_REC_LIST : MACH_AAS_INFO_REC_LIST];
}
CFRecVL & Machine :: rec54List(Boolean fMRDB)
{
 return CFRLists[fMRDB? MACH_MRDB_54_REC_LIST : MACH_AAS_54_REC_LIST];
}
CFRecVL & Machine :: otherRecList(Boolean fMRDB)
{
 return CFRLists[fMRDB? MACH_MRDB_OTHER_REC_LIST : MACH_AAS_OTHER_REC_LIST];
}

    /*-----------------------------------------------------------------------*\
    |    void setModel(const char * szModel)                                  |
    |       Copies the given feature name in the object as well as            |
    |       the CFReport Record List and features.                            |
    \*-----------------------------------------------------------------------*/
void Machine::setModel(const char * szModel)
{
 CFRecV * pRec;
 mystrstrip(mystrncpy(Mod,szModel,sizeof(Mod)-1));
 for (int i=0,I=(sizeof(MachRecs)/sizeof(MachRecs[0])) ; i < I ; i++) {
   CFRecV & rM = MachRecs[i];
   if (rM.length())
     switch(machineRecVersion(i)) {
       case 20:
       case 30:
         switch(rM.type()) {
           case 47: CFR47::setModel(rM,szModel);break;
           case 49: CFR49::setModel(rM,szModel);break;
           default: CFR08::setModel(rM,szModel);
           }
         break;
       case 31:
         CFR08V31::setModel(rM,szModel);
         break;
       }
   }
 for (int n = 0; n < 2; n++) {                   // do AAS and MRDB info Recs
   CFRecVL & ListMRDB = infoRecList(n);
   for (i = 0,I = ListMRDB.NumberElements(); i < I ; i++)
   if (pRec = ListMRDB.element(i))
     switch (pRec->type()) {
       case 54: CFR54::setMachModel(pRec,szModel);break;
       case 95: CFR95::setProductModel(pRec,szModel);break;
       }
   }
 for ( i = 0 , I = MFL.NumberElements(); i < I ; i++)
   MFL.element(i)->setModel(szModel);
}

    /*-----------------------------------------------------------------------*\
    |    unsigned fWriteBLength()                                             |
    |       Return the number of bytes the the object would                   |
    |       occupy if it was to be written to a binary file                   |
    |       by method  'fWriteB'.                                             |
    \*-----------------------------------------------------------------------*/
unsigned Machine::fWriteBLength()
{
 unsigned len = sizeof(*this);
 for (int i=0,I=(sizeof(MachRecs)/sizeof(MachRecs[0])) ; i < I ; i++)
   len += MachRecs[i].fWriteBLength();
 return len;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean fWriteB(FILE *f, unsigned * pBytesWrit)                      |
    |       Write the object                                                  |
    |       out to the current position of the given binary file              |
    |       and report the number of bytes written.                           |
    \*-----------------------------------------------------------------------*/
Boolean Machine::fWriteB(FILE *f, unsigned *pBytes )
{
 Boolean fOK = false;
 unsigned u = 0, len = sizeof(*this);
 if (fOK = (fwrite(this,len,1,f) == 1)) {
   u = len;
   for (int i=0,I=(sizeof(MachRecs)/sizeof(MachRecs[0])); fOK && (i < I);i++)
     if ( fOK = MachRecs[i].fWriteB(f,&len))
       u += len;
   }
 if (pBytes) *pBytes = u;
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean fReadB(FILE *f, unsigned *pBytes )                            |
    |       copy the Machine stored at the current                            |
    |       position of the given file to the object                          |
    |       and set the number of bytes read.                                 |
    |       Note: Only to be used for a file segment written by 'fWriteB'.    |
    \*-----------------------------------------------------------------------*/
Boolean Machine::fReadB(FILE *f, unsigned *pBytes )
{
 Boolean fOK = false;
 unsigned u = 0, len = sizeof(*this);
 char *Buff = new char[len];
 if (fOK = (Buff != 0)) {
   if (fOK = (fread(Buff,len,1,f) == 1)) {
     u = len;
     BaseInit(*(Machine *)Buff);
     for (int i=0,I=(sizeof(MachRecs)/sizeof(MachRecs[0]));fOK && (i < I);i++)
       if ( fOK = MachRecs[i].fReadB(f,&len))
         u += len;
     }
   delete[] Buff;
   }
 if (pBytes) *pBytes = u;
 return fOK;
}

int Machine::Write(FILE *f)
{
 int i;
 if ( (MachRecs[0].fWriteA(f) || MachRecs[1].fWriteA(f)) &&
//      (RecFirst54[0].Write(f) || RecFirst54[1].Write(f)) &&
      ((i = MFL.Write(f)) >=0 ))
   return (i+2);
 return -1;
}

    /*-----------------------------------------------------------------------*\
    |   IString description()                                                 |
    |       Return the description string stripped of blanks                  |
    |       from the first MRDB 54-Record if there's no                       |
    |       AAS counterpart.                                                  |
    \*-----------------------------------------------------------------------*/
IString Machine :: description()
{
 IString str;
 CFRecV * pRec;
 CFRecVL & List = infoRecList(false);
 if (!List.NumberElements() || !(pRec = List.element(0)) || !pRec->length()) {
   CFRecVL & List2 = infoRecList(true);
   if (!List2.NumberElements() || !(pRec = List2.element(0)) || !pRec->length())
     return str;
   }
 switch (pRec->type()) {
   case 54: CFR54::description(pRec,str); break;
   case 95: CFR95::description(pRec,str); break;
   }
 return str;
}

    /*-----------------------------------------------------------------------*\
    |   IString customerNumber()                                              |
    |       Return the customer number string stripped of blanks              |
    |       from the first MRDB 54-Record if there's no                       |
    |       AAS counterpart.                                                  |
    |                                                                         |
    |   New: D83402   HECC - 07/28/2003                                       |
    \*-----------------------------------------------------------------------*/
IString Machine :: customerNumber()
{
 IString strCustNum = "";
 CFRecV * pRec;
 CFRecVL & List = infoRecList(false);
 if (!List.NumberElements() || !(pRec = List.element(0)) || !pRec->length()) {
   CFRecVL & List2 = infoRecList(true);
   if (!List2.NumberElements() || !(pRec = List2.element(0)) || !pRec->length())
     return strCustNum;
   }
 switch (pRec->type()) {
   case 54: CFR54::customerNumber(pRec,strCustNum); break;
   case 95: CFR95::customerNumber(pRec,strCustNum); break;
       break;
   }
 return strCustNum;
}

    /*-----------------------------------------------------------------------*\
    |   char systNumberFlag()                                                 |
    |       Return the system number flag string stripped of blanks           |
    |                                                                         |
    |   New: D83402   HECC - 07/28/2003                                       |
    \*-----------------------------------------------------------------------*/
IString Machine :: systNumberFlag()
{
 IString strSysNumFlag = "";
 CFRecV * pRec;
 CFRecVL & List = infoRecList(false);
 if (!List.NumberElements() || !(pRec = List.element(0)) || !pRec->length()) {
   CFRecVL & List2 = infoRecList(true);
   if (!List2.NumberElements() || !(pRec = List2.element(0)) || !pRec->length())
     return strSysNumFlag;
   }
 switch (pRec->type()) {
   case 95: CFR95::systNumberFlag(pRec,strSysNumFlag); break;
   default:
       strSysNumFlag = "";

   }
 return strSysNumFlag;
}

    /*-----------------------------------------------------------------------*\
    |   IString systemType()                                                  |
    |       Return the system type string stripped of blanks                  |
    |                                                                         |
    |   New: D83402   HECC - 07/28/2003                                       |
    \*-----------------------------------------------------------------------*/
IString Machine :: systemType()
{
 IString strSysType = "";
 CFRecV * pRec;
 CFRecVL & List = infoRecList(false);
 if (!List.NumberElements() || !(pRec = List.element(0)) || !pRec->length()) {
   CFRecVL & List2 = infoRecList(true);
   if (!List2.NumberElements() || !(pRec = List2.element(0)) || !pRec->length())
     return strSysType;
   }
 switch (pRec->type()) {
   case 95: CFR95::systemType(pRec,strSysType); break;
   default:
       strSysType = "";

   }
 return strSysType;
}

    /*-----------------------------------------------------------------------*\
    |   IString systemNum()                                                   |
    |       Return the system number string stripped of blanks                |
    |                                                                         |
    |   New: D83402   HECC - 07/28/2003                                       |
    \*-----------------------------------------------------------------------*/
IString Machine :: systemNum()
{
 IString strSysNum = "";
 CFRecV * pRec;
 CFRecVL & List = infoRecList(false);
 if (!List.NumberElements() || !(pRec = List.element(0)) || !pRec->length()) {
   CFRecVL & List2 = infoRecList(true);
   if (!List2.NumberElements() || !(pRec = List2.element(0)) || !pRec->length())
     return strSysNum;
   }
 switch (pRec->type()) {
   case 95: CFR95::systemNum(pRec,strSysNum); break;
   default:
       strSysNum = "";

   }
 return strSysNum;
}


    /*-----------------------------------------------------------------------*\
    |   Boolean installDate(char * Buff,IDate & date)                         |
    |       set the object's Install date,                                    |
    |       returning false if not available                                  |
    \*-----------------------------------------------------------------------*/
Boolean Machine :: installDate(IDate & date,Boolean fMRDB)
{
 CFRecV * pRec;
 CFRecVL & List = infoRecList(fMRDB);
 if (List.NumberElements() && (pRec = List.element(0)) && pRec->length())
   switch (pRec->type()) {
     case 54: return CFR54::installDate(pRec,date);
     case 95: return CFR95::installDate(pRec,date);
     }
 return false;
}

Boolean Machine :: setInstallDate(IDate & date,Boolean fMRDB)
{
 CFRecV * pRec;
 CFRecVL & List = infoRecList(fMRDB);
 if (List.NumberElements() && (pRec = List.element(0)) && pRec->length())
   switch (pRec->type()) {
     case 54: return CFR54::setInstallDate(pRec,date);
     case 95: return CFR95::setInstallDate(pRec,date);
     }
 return false;
}

Boolean  Machine :: setDescription(const char * Desc, Boolean fMRDB)
{
 CFRecV * pRec;
 CFRecVL & List = infoRecList(fMRDB);
 if (List.NumberElements() && (pRec = List.element(0)) && pRec->length())
   switch (pRec->type()) {
     case 54: return CFR54::setDescription(pRec,Desc);
     case 95: return CFR95::setDescription(pRec,Desc);
     }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   int fWriteCFR()                                                       |
    |       Write the machine out to the current position                     |
    |       of the given ASCII file from the given type                       |
    |       'buckets' (AAS or MRDB) and return the number of                  |
    |       records written to the file, as follows:                          |
    |       Software and PassThru object ( objects as a result of             |
    |           08 Records found in the AAS CFReport) are always              |
    |           written from the AAS ' buckets'                               |
    |       Lone Machines (those without any features) are written            |
    |           from the other 'buckets' if the requested-ones are            |
    |           empty.                                                        |
    |       The machine Records' (e.g. 08,19 47 etc) feature arrays           |
    |           are rebuild according to the features contained               |
    |           in the appropriate feature List.                              |
    |       Processed non-Software objects have the type of their             |
    |       machine records (most likely 25 type records) set to '08'.        |
    |                                                                         |
    |       For non-Software objects, the following records are               |
    |         written (in order):                                             |
    |            Machine records                                              |
    |            First 54 records (the first 54 records in the CFReport       |
    |                              following the machine record read in       |
    |                              or generated)                              |
    |            Pass-Thru records ( those records which are not considered   |
    |                                 but carried along e.g Comment records)  |
    |            The object's features as follows:                            |
    |                Their  54-Records, after their Serial                    |
    |                   Numbers are set to that of the machine and            |
    |                   their Instance Ids are established as a rsult         |
    |                   of their potential Placement and Connection records.  |
    |                For version 20 only: the same number of 54-Records       |
    |                   as the feature's quantity.                            |
    |                The feature's Pass-Thru (e.g.comment) records.           |
    |                                                                         |
    |       For Software objects,The following records are                    |
    |         written (in order):                                             |
    |            Machine records as follows:                                  |
    |               47 & 49 type records are converted to 17 type records,    |
    |               while other type records are merely set to '17'.          |
    |            First 54 records,      as explained above.                   |
    |            Pass-Thru records,     as explained above.                   |
    |            The object's features, as explained above.                   |
    |                                                                         |
    |       Notes: a return value of -1 indicates failure.                    |
    |              Some background on Connections/Placements:                 |
    |                Some Machine Features have additional                    |
    |                information associated with them in the form of other    |
    |                Records which are tied to the feature's 54-Record        |
    |                via their common 'InstanceId' field.                     |
    |                In a CFReport, these ids need to start at 1 and be       |
    |                consecutive, while the records are to be grouped         |
    |                together by type and appended to the CFReport.           |
    |                During reconciliation, a feature might disappear         |
    |                which could violate the Instance Id order.               |
    |                We handle the Placement (55-Record) and Connection       |
    |                (56-Record) records which are accumulated in lists       |
    |                as the CFReport are read in.                             |
    |                These 'In' lists are used as a source                    |
    |                (via the feature's Instance Id) for the 'Out' lists      |
    |                which will consist of those records that belong to       |
    |                features left over after the reconciliation process,     |
    |                but re-numbered with fresh consecutive Instance Ids.     |
    \*-----------------------------------------------------------------------*/
// new version  that doesn't switch to the AAS side for Software and
// AAS_Passthru which are expected to have been copied/converted to the
// MRDB size if we're writing from there.
int Machine::fWriteCFR(FILE *f,Boolean fMRDB,
                       unsigned * pInstId,
                       CFRInstL & L55_In, CFRInstL & L55_Out,
                       CFRInstL & L56_In, CFRInstL & L56_Out)
{
 MFeat * pF;
 int i,I,cnt = 0,cnt2,N;
                 /*-----------------------------*\
                 | we're supposed to output      |
                 | version 20. It's hardcoded    |
                 | here.                         |
                 \*-----------------------------*/
 int outputVersion = 20;
 MFeat::CFRType type = fMRDB? MFeat::MRDBType : MFeat::AASType;
 CFRecV & rM = MachRecs[fMRDB];
 if (rM.length()) {
   if (Tag != Software) {
     if (Tag == Process)
       rM.setType(8);
     if (Tag != AAS_Passthru)
       setMachRecFromCFRList(fMRDB);
     cnt = rM.fWriteCFR(f)? 1 : -1;
     if (cnt > 0)                                // always write info record
       cnt = infoRecList(fMRDB).fWriteCFR(f)? ++cnt : -1;
     cnt += otherRecList(fMRDB).fWriteCFR(f);    // passthru recs
     for (i=0,I=MFL.NumberElements(); (cnt > 0) && (i < I) ; i++)
       if ( pF = MFL.element(i))
         if  (N = pF->baseNmbr()) {
           pF->setSerialNo(SerNo);
           if (pF->getConnections(type,*pInstId,
                                  L55_In,L55_Out,L56_In,L56_Out))
             (*pInstId)++;
           cnt = ((cnt2 = pF->fWriteCFR(f,type,outputVersion)) > 0)?
                  cnt + cnt2 : -1;
           if (cnt > 0 && pF->qtyCFRList(MFeat::OtherType))
             cnt = ((cnt2 = pF->fWriteCFR(f,MFeat::OtherType,outputVersion)) > 0)?
                    cnt + cnt2 : -1;
           }
         else cnt = -1;
     }
   else {                                        // software
     switch(rM.type()) {
       case 49:
//         cnt = CFR35((Record_49 *)rM.record()).fWriteCFR(f)? 1 : -1;
         cnt = CFR17((Record_49 *)rM.record()).fWriteCFR(f)? 1 : -1;
         break;
       case 47:
//         cnt = CFR35((Record_47 *)rM.record()).fWriteCFR(f)? 1 : -1;
         cnt = CFR17((Record_47 *)rM.record()).fWriteCFR(f)? 1 : -1;
         break;
       default:
//         rM.setType(35);
         rM.setType(17);
         cnt = rM.fWriteCFR(f)? 1 : -1;
       }
     if (cnt > 0) {
       cnt = infoRecList(fMRDB).fWriteCFR(f)? ++cnt : -1;
       if (cnt > 0)
                                                 // passthru recs
         cnt += otherRecList(fMRDB).fWriteCFR(f);
       }
     for (i=0,I=MFL.NumberElements(); (cnt > 0) && (i < I) ; i++)
       if ( pF = MFL.element(i)) {
         cnt = ((cnt2 = pF->fWriteCFR(f,type,outputVersion)) >= 0)? cnt + cnt2 : -1;
         if (cnt > 0 && pF->qtyCFRList(MFeat::OtherType))
           cnt = ((cnt2 = pF->fWriteCFR(f,MFeat::OtherType,outputVersion)) >= 0)?
                  cnt + cnt2 : -1;
         }
       else cnt = -1;
     }
   }
 return cnt;
}

    /*-----------------------------------------------------------------------*\
    |   int fWriteCFRV31(FILE *f,Boolean fMRDB,                               |
    |                          unsigned * pInstId,                            |
    |                          CFRInstL & L55_In, CFRInstL & L55_Out,         |
    |                          CFRInstL & L56_In, CFRInstL & L56_Out)         |
    |   int fWriteCFR()                                                       |
    |       Write the machine out to the current position                     |
    |       of the given ASCII file from the given type                       |
    |       'buckets' (AAS or MRDB), copy the appropriate Placement or        |
    |       Connection for all the machine's features into the given          |
    |       'Out' Lists from the given 'In' Lists and return the number of    |
    |       records written to the file, as follows:                          |
    |       Software and PassThru object ( objects as a result of             |
    |           08 Records found in the AAS CFReport) are always              |
    |           written from the AAS ' buckets'                               |
    |       The machine Records' (e.g. 08,19 47 etc) feature arrays           |
    |           are rebuild according to the features contained               |
    |           in the appropriate feature List.                              |
    |       Processed non-Software objects have the type of their             |
    |       machine records (most likely 25 type records) set to '08'.        |
    |                                                                         |
    |       For non-Software objects, the following records are               |
    |         written (in order):                                             |
    |            Machine records                                              |
    |            Info  Records                                                |
    |            Pass-Thru records ( those records which are not considered   |
    |                                 but carried along e.g Comment records)  |
    |            The object's features as follows:                            |
    |                Their  96-Records, after their Serial                    |
    |                   Numbers are set to that of the machine and            |
    |                   their Instance Ids are established as a rsult         |
    |                   of their potential Placement and Connection records.  |
    |                The feature's Pass-Thru (e.g.comment) records.           |
    |            The MRDB file's 54-Records for written out features having   |
    |              Connection or Placement Records associated with them.      |
    |                                                                         |
    |       For Software objects,The following records are                    |
    |         written (in order):                                             |
    |            Machine records as follows:                                  |
    |            Machine Record                                               |
    |            Info Record                                                  |
    |            Pass-Thru records.                                           |
    |            The object's features.                                       |
    |                                                                         |
    |       Notes: a return value of -1 indicates failure.                    |
    |              Some background on Connections/Placements:                 |
    |                Some Machine Features have additional                    |
    |                information associated with them in the form of other    |
    |                Records which are tied to the feature's 54-Record        |
    |                via their common 'InstanceId' field.                     |
    |                In a CFReport, these ids need to start at 1 and be       |
    |                consecutive, while the records are to be grouped         |
    |                together by type and appended to the CFReport.           |
    |                During reconciliation, a feature might disappear         |
    |                which could violate the Instance Id order.               |
    |                We handle the Placement (55-Record) and Connection       |
    |                (56-Record) records which are accumulated in lists       |
    |                as the CFReport are read in.                             |
    |                These 'In' lists are used as a source                    |
    |                (via the feature's Instance Id) for the 'Out' lists      |
    |                which will consist of those records that belong to       |
    |                features left over after the reconciliation process,     |
    |                but re-numbered with fresh consecutive Instance Ids.     |
    \*-----------------------------------------------------------------------*/
// new version  that doesn't switch to the AAS side for Software and
// AAS_Passthru which are expected to have been copied/converted to the
// MRDB size if we're writing from there.
int Machine::fWriteCFRV31(FILE *f,Boolean fMRDB,
                          unsigned * pNextInstId,
                          CFRInstL & L55_In, CFRInstL & L55_Out,
                          CFRInstL & L56_In, CFRInstL & L56_Out,
                          CFR05List & CommentRecList )
{
 CFRecV * pRec;
 CFRecVL ConnectionRec54List;
 MFeat * pF;
 int i,I,j,J,cnt = 0,cnt2,N,RefNoteNo;
 MFeat::CFRType type = fMRDB? MFeat::MRDBType : MFeat::AASType;
 CFRecV & rM = MachRecs[fMRDB];

/* F73732 EDW 07/25/02
   Change the tag for any SIPO type records
   back to 'Software' before writing it.  The tag was temporarily
   changed to 'Hardware' for reconciliation purposes
 */
if (!strcmp(this->type(), SIPO_TYPE) && !strcmp(this->model(), SIPO_MODEL))
   setTag (Machine::Software);

 if (rM.length()) {
   if (Tag != Software) {
     if (Tag == Process)
       rM.setType(8);
     if (Tag != AAS_Passthru)
       setMachRecFromCFRList(fMRDB);
     cnt = rM.fWriteCFR(f)? 1 : -1;
                      /*-------------------------------------*\
                      | Always write Info Record but chop off |
                      | the Description for non-Software      |
                      | using a copy of the record            |
                      \*-------------------------------------*/
     if (cnt > 0)
       for (i=0,I=infoRecList(fMRDB).NumberElements(); (cnt>=0) && (i<I) ; i++)
         if (pRec = infoRecList(fMRDB).element(i))
           cnt = ((cnt2 = CFR95::fWriteCFR(f,pRec,false)) > 0)? cnt + cnt2 : -1;
     if (RefNoteNo = referenceNotesNumber(fMRDB))
       cnt += CommentRecList.fWriteCFR(f,RefNoteNo);
     cnt += otherRecList(fMRDB).fWriteCFR(f);    // passthru recs
     for (i=0,I=MFL.NumberElements(); (cnt > 0) && (i < I) ; i++)
       if ( pF = MFL.element(i))
//         if  (N = pF->baseNmbr()) {
         if  ((N = pF->baseNmbr()) || (Tag == Backlog_HW) ||     //VBB09192002 D74157 D74157 D74157
              (Tag == Backlog_SW)) {                             //VBB09192002 D74157 D74157 D74157
           pF->setSerialNo(SerNo);
                      /*-------------------------------------*\
                      | Write the Feature Record but chop off |
                      | the Description for non-Software      |
                      | using a copy.                         |
                      \*-------------------------------------*/
           if (pRec = pF->cfrList(fMRDB).element(0))
             cnt = ((cnt2 = CFR96::fWriteCFR(f,pRec,false)) > 0)? cnt + cnt2 : -1;
           else cnt = ((cnt2 = pF->fWriteCFR(f,type,31)) > 0)?
                      cnt + cnt2 : -1;
           if (RefNoteNo = pF->referenceNotesNumber(fMRDB))
             cnt += CommentRecList.fWriteCFR(f,RefNoteNo);
//           if (cnt > 0 && pF->qtyCFRList(MFeat::OtherType))
//             cnt = ((cnt2 = pF->fWriteCFR(f,MFeat::OtherType,31)) > 0)?
//                    cnt + cnt2 : -1;
           if (cnt > 0)
             switch(machineRecVersion(true)) {
               case 20:
               case 30:
                 if (pRec = pF->cfrList(MFeat::MRDBType).element(0))
                   ConnectionRec54List.AddasLast(pRec);
                 //edw 4/28/98 - add 'if clause' below to get other 54 records for features
                 //            that have qty greater than 1 and thus more than 1 54 record
                 if ( DuplicateRec54List.NumberElements() )
                    {
                    MFeat * pDupRec54;
                    CFRecV * pCFRec54;
                    int t,T;
                    for (t=0, T=DuplicateRec54List.NumberElements(); t < T; t++)
                       {
                       if ( pDupRec54=DuplicateRec54List.element(t) )
                          {
                          IString str1 = pDupRec54->feature();
                          IString str2 = pF->feature();
                          if ( !(strcmp(str1, str2) ) )
                             if (pCFRec54 = pDupRec54->cfrList(MFeat::MRDBType).element(0))
                                ConnectionRec54List.AddasLast(pCFRec54);
                          }
                       }
                    }
                 break;
               case 31:
                 for (j = 0,J = rec54List(true).NumberElements(); j < J ; j++)
                   if (pRec = rec54List(true).element(j))
                     if (IString(pF->feature()) == CFR54::strFeature(pRec->record()))
                       ConnectionRec54List.AddasLast(pRec);
               }
           }
//edw
//         else cnt = -1;                          // a non-resolved feature


                                                              //edw-write V20 54 info record
//     if (cnt > 0)                                             //edw05/10/1999

     //* F86750 Begin logic: The RT 38 services records must also be avoided to be processed
     //* if ((cnt > 0) && (Tag != Backlog_HW) && (Tag != Backlog_SW))   //edw05/10/1999   //VBB09192002 D74157 D74157 D74157

     //* D104285 Begin logic: The tag Process_IS of the machines reconciled in Inventory Services
     //* must also be added to avoid the process of those machines as they are already reconciled
     //* if ((cnt > 0) && (Tag != Backlog_HW) && (Tag != Backlog_SW) &&
     //*                                         (Tag != Services)     )

     if ((cnt > 0) && (Tag != Backlog_HW) && (Tag != Backlog_SW) &&
                                             (Tag != Services)   &&
                                             (Tag != Process_IS)    )
     //* D104285 End logic:  HECC - 11/16/2005

     //* F86750 End logic:   HECC - 01/15/2004

       cnt = infoRecList(true).fWriteCFR(f)? ++cnt : -1;      //edw05/10/1999

     //edw05/15/98 -add 'if clause' to get machine's secondary features
     if ( MachSecFeatList.NumberElements() )
        {
        MFeat *pSecFeat;
        CFRecV *pCFSecFeat;
        int t,T;
        for (t=0, T=MachSecFeatList.NumberElements(); t < T; t++)
           if ( pSecFeat=MachSecFeatList.element(t) )
              if (pCFSecFeat=pSecFeat->cfrList(MFeat::MRDBType).element(0))
                 ConnectionRec54List.AddasLast(pCFSecFeat);
        }

     getConnections(pNextInstId,ConnectionRec54List,L55_In,L55_Out,L56_In,L56_Out);
     cnt += CFR54::fWriteCFR(f,ConnectionRec54List,31);
     }
   else {                                        // software
     /* F72273 Creates the software product record  47, 49  with the  */
     /* information of the features struture in the Machine structure */
     /* HECC: 07/12/2002 */
     setMachRecFromCFRList(fMRDB);
     cnt = rM.fWriteCFR(f)? 1 : -1;
     if (cnt > 0) {
       cnt = infoRecList(fMRDB).fWriteCFR(f)? ++cnt : -1;
       if (cnt > 0)
                                                 // passthru recs
         cnt += otherRecList(fMRDB).fWriteCFR(f);
       }
     for (i=0,I=MFL.NumberElements(); (cnt > 0) && (i < I) ; i++)
       if ( pF = MFL.element(i)) {

         /* F72273 New If clause. Needed to write only those SW features wich */
         /* base quantity is > 0, as is made in the HW products. */
         /* Begin new logic. F72273 */
         if (N = pF->baseNmbr())
            {
            if (pRec = pF->cfrList(MFeat::AASType).element(0))
              cnt = ((cnt2 = CFR96::fWriteCFR(f,pRec,false)) > 0)? cnt + cnt2 : -1;
            else
              if (pRec = pF->cfrList(MFeat::MRDBType).element(0))
                 {
                 pRec->setType(96);
                 IString strTempRec = pRec->record();
                 strTempRec = strTempRec.subString(1,2) +
                              strTempRec.subString(21,7).leftJustify(7).strip();
                 pRec->setRec(strTempRec);
                 int iLen96 = strTempRec.length();
                 pRec->setLength(iLen96);
                 cnt = ((cnt2 = CFR96::fWriteCFR(f,pRec,false)) > 0)? cnt + cnt2 : -1;
                 }
         /* cnt = ((cnt2 = pF->fWriteCFR(f,type,31)) >= 0)? cnt + cnt2 : -1; */
         /* F72273 Ends new logic  HECC  07/12/2002 */

            if (cnt > 0 && pF->qtyCFRList(MFeat::OtherType))
              cnt = ((cnt2 = pF->fWriteCFR(f,MFeat::OtherType,31)) >= 0)?
                     cnt + cnt2 : -1;
            }
         }
       else cnt = -1;
     }                                           // end software
   }
 return cnt;
}

unsigned Machine::modelQty(Boolean fMRDB)
{
 if (MachRecs[fMRDB].length())
   switch(machineRecVersion(fMRDB)) {
     case 20:
     case 30: return CFR08::modelQty(MachRecs[fMRDB].record());
     case 31: return CFR08V31::productQty(MachRecs[fMRDB].record());
     }
 return 0;
}

Boolean Machine::setModelQty(unsigned qty,Boolean fMRDB)
{
 if (MachRecs[fMRDB].length())
   switch(machineRecVersion(fMRDB)) {
     case 20:
     case 30: return CFR08::setModelQty(MachRecs[fMRDB],qty);
     case 31: return CFR08V31::setProductQty(MachRecs[fMRDB],qty);
     }
 return false;
}

void Machine::setMachineRec(Boolean fMRDB,char *p, unsigned version)
{
 MachRecs[fMRDB] = CFRecV(p);
 MachRecs[fMRDB].setId(version);
}

void Machine :: setMachineRec(Boolean fMRDB,CFRecV &r,unsigned version)
{
 MachRecs[fMRDB] = r;
 MachRecs[fMRDB].setId(version);
}

    /*-----------------------------------------------------------------------*\
    |    Boolean addOtherRec(CFRType type,const char * Buffer)                |
    |       add the given buffer as a CFR Record after making a copy of it.   |
    |       These are records we're not interested in other than writing      |
    |       them out to the new CFReport (such as the comment records         |
    \*-----------------------------------------------------------------------*/
Boolean Machine :: addOtherRec(Boolean fMRDB,const char * Buf)
{
 CFRecV * pRec = new CFRecV(Buf);
 if (pRec)
   infoRecList(fMRDB).AddasLast(pRec);
 return (pRec != 0);
}
Boolean Machine :: addInfoRec(Boolean fMRDB,const char * Buf)
{
         /*---------------------------------------------------*\
         | 54's may come in at its version-20 length of 74     |
         | bytes - we want to ensure that we get storage for   |
         | the longer version since we store the machine's     |
         | description there                                   |
         \*---------------------------------------------------*/
 CFRecV * pRec;
 if (CFRecType(Buf) == 54)
   pRec = new CFRecV(sizeof(Record_54),Buf,' ');
 else pRec = new CFRecV(Buf);
 if (pRec)
   infoRecList(fMRDB).AddasLast(pRec);
 return (pRec != 0);
}
Boolean Machine :: add54Rec(Boolean fMRDB,const char * Buf)
{
 CFRecV * pRec = new CFRecV(Buf);
 if (pRec)
   rec54List(fMRDB).AddasLast(pRec);
 return (pRec != 0);
}
Boolean Machine :: add06Rec(Boolean fMRDB,const char * Buf)       // ZOR 95997
{
  CFRecV * pRec = new CFRecV(Buf);
 if (pRec)
   otherRecList(fMRDB).AddasLast(pRec);
 return (pRec != 0);
}
Boolean Machine :: addOtherRec(Boolean fMRDB,CFRecV & rec)
{
 CFRecV * pRec = new CFRecV(rec);
 if (pRec)
   infoRecList(fMRDB).AddasLast(pRec);
 return (pRec != 0);
}
Boolean Machine :: addInfoRec(Boolean fMRDB,CFRecV & rec)
{
 CFRecV * pRec = new CFRecV(rec);
 if (pRec)
   infoRecList(fMRDB).AddasLast(pRec);
 return (pRec != 0);
}
Boolean Machine :: add54Rec(Boolean fMRDB,CFRecV & rec)
{
 CFRecV * pRec = new CFRecV(rec);;
 if (pRec)
   rec54List(fMRDB).AddasLast(pRec);
 return (pRec != 0);
}

    /*-----------------------------------------------------------------------*\
    |    void pickUp(MFeatList & FList, CFRecVL & RecList)                    |
    |       Add the object's features to the given Feature List               |
    |         after 'stamping' them with consecutive numbers,                 |
    |         the first of which is the number of features in the             |
    |         given list plus 1.                                              |
    |       Retain the first number and the number of features                |
    |         in a CFRStore object such that we may locate the                |
    |         features when the Machine is rebuild.                           |
    |       Have the features store their CFReport Records in a               |
    |         similar manner by calling their 'pickUp' method                 |
    |         with the given Record List.                                     |
    |       Add the object's Pass-Thru Records ( the machines's comment       |
    |         records) to the same List and retain the storage                |
    |         data for the subsequent re-build process                        |
    |                                                                         |
    |       (Since this storage data is within the object, it will be written |
    |        to a file if the user decides to 'Save' the current state.       |
    |        Thus, when the user 'Open's a stored state, this machine         |
    |        object (including the storage info) is read from the file        |
    |        and the object can locate its records when the Lists are         |
    |        presented during the rebuilding process (delivery()              |
    \*-----------------------------------------------------------------------*/
void Machine::pickUp(MFeatList & FList, CFRecVL & RecList)
{
 MFeat * pFeat;
 int i,j,k;
 if ( FeatStoreInfo.qty = MFL.NumberElements() )
   for ( i = 0; i < FeatStoreInfo.qty ; i++) {
     pFeat = MFL.element(i);
     pFeat->pickUp(RecList);
     if (!i)
       FeatStoreInfo.RecNmbr = FList.NumberElements() + 1;
     pFeat->setNumber(FeatStoreInfo.RecNmbr + i);
     FList.AddasLast(pFeat);
     }
 else FeatStoreInfo.RecNmbr = 0;
// append the machine's additional  CFRecords
 CFRecV * pRec;
 unsigned RecNumber = RecList.NumberElements() + 1 ;
 for ( i = 0; i < MACH_NMBR_CFR_LISTS ; i++ )
   if (k = CFRStoreInfo[i].qty = CFRLists[i].NumberElements())
     for (j = 0; j < k ; j++) {
       if (!j)
         CFRStoreInfo[i].RecNmbr = RecNumber;
       pRec = CFRLists[i].element(j);
       pRec->setId(RecNumber++);
       RecList.AddasLast(pRec);
       }
   else CFRStoreInfo[i].RecNmbr = 0;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean delivery(MFeatList & FList, CFRecVL & RecList)               |
    |       Rebuild the object's features and CFReport records                |
    |         from the incoming lists, as follows:                            |
    |                                                                         |
    |         Use the storage information in 'FeatStoreInfo' to               |
    |          select the features in the given list that belong              |
    |          to this machine and them to the object's list.                 |
    |          (the first feature number and this machine's number            |
    |           of features were established and stored by the 'pickUp()'     |
    |           method called when the System was saved to a file.            |
    |           Storage occupied by these features was obtained               |
    |           when the given list was built, and we're adding               |
    |           only the pointers to the list)                                |
    |                                                                         |
    |         For every obtained feature, call its 'delivery()' method        |
    |          with the given record list such that it may re-establish       |
    |          its records.                                                   |
    |                                                                         |
    |         Restore the object's records from the same Record List,         |
    |         using the storage info retained in the CFRStoreInfo             |
    |         objects and similar logic                                       |
    \*-----------------------------------------------------------------------*/
Boolean  Machine :: delivery(MFeatList & FList, CFRecVL & RecList)
{
 MFeat   * pAFeature;
 CFRecV * pRec;
 int i,j,k;
 Boolean fOK = true;
 for (i = 0, j = FeatStoreInfo.RecNmbr; fOK && i < FeatStoreInfo.qty; i++,j++)
   if (fOK = ((pAFeature = FList.FindRecord(j)) != 0)) {
     pAFeature->delivery(RecList);
     MFL.Add(pAFeature);
     }
                                                 // restore the machine's
                                                 // additional CFRecords
 for ( i = 0; fOK && i < MACH_NMBR_CFR_LISTS ; i++ )
   for (j = 0,k = CFRStoreInfo[i].RecNmbr; fOK && j < CFRStoreInfo[i].qty; j++,k++)
     if (fOK = ((pRec = RecList.find(k)) != 0))
       CFRLists[i].AddasLast(pRec);
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    void Release()                                                       |
    |       release all obtained storage associated with                      |
    |       this machine i.e. The list of features and                        |
    |       CFReport Records pertaining to the object                         |
    \*-----------------------------------------------------------------------*/
void Machine::Release()
{
 for (int i = 0; i < MACH_NMBR_CFR_LISTS ; i++ )
   CFRLists[i].Release();
 MFL.Release(true);
}

    /*-----------------------------------------------------------------------*\
    |    Boolean isSoftwareMatch(const Record_17 * p17)                       |
    |       Indicate whether this machine is a Software machine               |
    |       which appeared in the AAS file and whose Type and                 |
    |       Model compare to those of the given CFReport 17-Record            |
    |       (method is used to establish Software descripancies.              |
    \*-----------------------------------------------------------------------*/
Boolean Machine::isSoftwareMatch(const Record_17 * p17,Boolean fMRDB)
{
  return ((Tag == Software)    &&
          MachRecs[fMRDB].length() &&
          !CFR49::compare(MachRecs[fMRDB],p17));
}

Boolean Machine::isSoftwareMatch(const Record_SW_V31 * pSW,Boolean fMRDB)
{
  return ((Tag == Software)  && MachRecs[fMRDB].length() &&
          (IString(Type) == CFR_SW_V31::strType(pSW))    &&
          (IString(Mod) == CFR_SW_V31::strModel(pSW)));
}

    /*------------------------------------------------------------------------*\
    |    Boolean checkMachRecQuantities()                                      |
    |       Edit the machine's AAS or MRDB CFReport Record with                |
    |       respect to its read-in features, reporting discrepancies           |
    |       to the given file and optionally creating new features             |
    |       with their CFReport Records for those features that                |
    |       appear in the Machine Record but were not read-in,                 |
    |       as follows:                                                        |
    |                                                                          |
    |          Skip those object tagged Hardware and Software Backlog.         |
    |          For version 20:                                                 |
    |            For every feature in the Machine Record, check its quantity   |
    |              against the number of records read-in for that feature:     |
    |                If the quantities differ:                                 |
    |                  Write the discrepancy to the message file.              |
    |                  Set the feature's quantity to that of the Machine       |
    |                     Record.                                              |
    |                If so requested, create missing features and/or           |
    |                  CFReport records.                                       |
    |            Report as discrepancy any feature read-in but not             |
    |              appearing in the Machine Record.                            |
    |                                                                          |
    |          For version 30:                                                 |
    |            For 47-type and 49-type Records:                              |
    |              For every feature in the Machine Record, check its quantity |
    |                against the number of records read-in for that feature:   |
    |                    Set the feature's quantity to that of the Machine     |
    |                       Record.                                            |
    |                    If the read-in quantity is not 1:                     |
    |                       Write the discrepancy to the message file.         |
    |              Report as discrepancy any feature read-in but not           |
    |                 appearing in the Machine Record.                         |
    |                                                                          |
    |            Otherwise:                                                    |
    |              For every feature in the Machine Record, check its quantity |
    |                against the number of records read-in for that feature:   |
    |                    Set the feature's quantity to that of the Machine     |
    |                       Record.                                            |
    |                    If the read-in quantity is not 1:                     |
    |                       Write the discrepancy to the message file.         |
    |                    If so requested, create missing features and/or       |
    |                      CFReport records.                                   |
    |              Report as discrepancy any feature read-in but not           |
    |                 appearing in the Machine Record.                         |
    \*------------------------------------------------------------------------*/

Boolean Machine::checkMachRecQuantities( Boolean fMRDB
                                        ,Tellem & tellem
                                        ,const char * CFRFile
                                        ,Boolean fCreateMissingFeatures)
{
//  fCreateMissingFeatures = false; // for testing
 char szFeature[20];
 unsigned cntMachineRec,cntRecsRead,cntRecsExpected,i,N,version;
 MFeat * pF;
 Boolean fOK = true;
 char * file = CFRFile? (char *)CFRFile : "";
 MFeat::CFRType FType = (fMRDB? MFeat::MRDBType : MFeat::AASType);
 CFRecV & rec = MachRecs[fMRDB];
 if (!rec.length())
   return true;
                                                 // don't do backlog
 if ( (Tag == Backlog_HW) || (Tag == Backlog_SW))
   return true;
 switch(version = machineRecVersion(fMRDB)) {
   case 20:
   case 30:
     switch(rec.type()) {
       case 47:
         for ( i = 0 , N = CFR47::numberFeat(rec); i < N ; i++) {
           CFR47::featParts(rec,i,szFeature,&cntMachineRec);
           if ( pF = MFL.Find(szFeature)) {
             cntRecsRead = pF->qty(FType);
             pF->setQty(FType,cntMachineRec);    // machine record's qty rules
             }
           else cntRecsRead = 0;
           cntRecsExpected = (version == 20)? cntMachineRec : 1;
           if (cntRecsExpected != cntRecsRead) {
             fOK = false;
             tellem.say(false,EUO_MSG_54_DIFF_25_QTY,file,
                        (char *)IString(cntRecsRead),
                        (char *)IString(cntRecsExpected),
                        szFeature,Type,Mod,SerNo);
             }
           }
         for ( i = 0 , N = MFL.NumberElements(); i < N ; i++)
           if (pF = MFL.element(i))
             if ( ( cntRecsRead = pF->qty(FType)) &&
                  !(CFR47::containsFeat(rec,pF->feature()))) {
               tellem.say(false,EUO_MSG_54_NO_25_QTY,file,
                     (char *)IString(cntRecsRead),pF->feature(),Type,Mod,SerNo);
               fOK = false;
               }
         break;
       case 49:
         for ( i = 0 , N = CFR49::numberFeat(rec); i < N ; i++) {
           CFR49::featParts(rec,i,szFeature,&cntMachineRec);
           if ( pF = MFL.Find(szFeature)) {
             cntRecsRead = pF->qty(FType);
             pF->setQty(FType,cntMachineRec);    // machine record's qty rules
             }
           else cntRecsRead = 0;
           cntRecsExpected = (version == 20)? cntMachineRec : 1;
           if (cntRecsExpected != cntRecsRead) {
             fOK = false;
             tellem.say(false,EUO_MSG_54_DIFF_25_QTY,file,
                        (char *)IString(cntRecsRead),
                        (char *)IString(cntRecsExpected),
                        szFeature,Type,Mod,SerNo);
             }
           }
         for ( i = 0 , N = MFL.NumberElements(); i < N ; i++)
           if (pF = MFL.element(i))
             if ( ( cntRecsRead = pF->qty(FType)) &&
                  !(CFR49::containsFeat(rec,pF->feature()))) {
               tellem.say(false,EUO_MSG_54_NO_25_QTY,file,
                     (char *)IString(cntRecsRead),pF->feature(),Type,Mod,SerNo);
               fOK = false;
               }
         break;
       /* F73732 EDW07/25/02 - start */
       case 17:
         for ( i = 0 , N = CFR17::numberFeat(rec); i < N ; i++) {
           CFR17::featParts(rec,i,szFeature,&cntMachineRec);
           if ( pF = MFL.Find(szFeature)) {
             cntRecsRead = pF->qty(FType);
             pF->setQty(FType,cntMachineRec);    // machine record's qty rules
             }
           else cntRecsRead = 0;
           cntRecsExpected = (version == 20)? cntMachineRec : 1;
           if (cntRecsExpected != cntRecsRead) {
             fOK = false;
             tellem.say(false,EUO_MSG_54_DIFF_25_QTY,file,
                        (char *)IString(cntRecsRead),
                        (char *)IString(cntRecsExpected),
                        szFeature,Type,Mod,SerNo);
             }
           if (!cntRecsRead && fCreateMissingFeatures) {
             CFRecV * pRec = CFR54::createANew(Type,Mod,SerNo,szFeature);
             if (!pF)
               MFL.Add(pF = new MFeat(szFeature));
                                                 // 'news' the record
             pF->addCFR(FType,*(Record_54 *)(pRec->record()));
             pF->setQty(FType,cntMachineRec);
             delete pRec;
             }
           }
         for ( i = 0 , N = MFL.NumberElements(); i < N ; i++)
           if (pF = MFL.element(i))
             if ( ( cntRecsRead = pF->qty(FType)) &&
                  !(CFR17::containsFeat(rec,pF->feature()))) {
               tellem.say(false,EUO_MSG_54_NO_25_QTY,file,
                     (char *)IString(cntRecsRead),pF->feature(),Type,Mod,SerNo);
               fOK = false;
               }
         break;
         /* F73732 EDW07/25/02  - end */
       case  8:
       case 25:
         for ( i = 0 , N = CFR08::numberFeat(rec); i < N ; i++) {
           CFR08::featParts(rec,i,szFeature,&cntMachineRec);
           if ( pF = MFL.Find(szFeature)) {
             cntRecsRead = pF->qty(FType);
             pF->setQty(FType,cntMachineRec);    // machine record's qty rules
             }
           else cntRecsRead = 0;
           cntRecsExpected = (version == 20)? cntMachineRec : 1;
           if (cntRecsExpected != cntRecsRead) {
             fOK = false;
             tellem.say(false,EUO_MSG_54_DIFF_25_QTY,file,
                        (char *)IString(cntRecsRead),
                        (char *)IString(cntRecsExpected),
                        szFeature,Type,Mod,SerNo);
             }
           if (!cntRecsRead && fCreateMissingFeatures) {
             CFRecV * pRec = CFR54::createANew(Type,Mod,SerNo,szFeature);
             if (!pF)
               MFL.Add(pF = new MFeat(szFeature));
                                                 // 'news' the record
             pF->addCFR(FType,*(Record_54 *)(pRec->record()));
             pF->setQty(FType,cntMachineRec);
             delete pRec;
             }
           }
         for ( i = 0 , N = MFL.NumberElements(); i < N ; i++)
           if (pF = MFL.element(i))
             if ( ( cntRecsRead = pF->qty(FType)) &&
                  !(CFR08::containsFeat(rec,pF->feature()))) {
               tellem.say(false,EUO_MSG_54_NO_25_QTY,file,
                     (char *)IString(cntRecsRead),pF->feature(),Type,Mod,SerNo);
               fOK = false;
               }
         break;
       default: fOK = false;
       }
     break;                                      // versions 20 & 30
   case 31:
     switch(rec.type()) {
       case  8:
       case 25:
         for ( i = 0 , N = CFR08V31::numberFeat(rec); i < N ; i++) {
           CFR08V31::featParts(rec,i,szFeature,&cntMachineRec);
           if ( pF = MFL.Find(szFeature))
             pF->setQty(FType,cntMachineRec);    // machine record's qty rules
           else {
             fOK = false;
             tellem.say(false,EUO_MSG_54_DIFF_25_QTY,file,"0","1",
                        szFeature,Type,Mod,SerNo);
             if (fCreateMissingFeatures) {
                if (!pF)
                  MFL.Add(pF = new MFeat(szFeature));
                CFR96 r96(szFeature);
                pF->addCFR(FType,r96);
                pF->setQty(FType,cntMachineRec);
                }
             }
           }
         for ( i = 0 , N = MFL.NumberElements(); i < N ; i++)
           if (pF = MFL.element(i))
             if ( ( cntRecsRead = pF->qty(FType)) &&
                  !(CFR08V31::containsFeat(rec,pF->feature()))) {
               fOK = false;
               tellem.say(false,EUO_MSG_54_NO_25_QTY,file,
                     (char *)IString(cntRecsRead),pF->feature(),Type,Mod,SerNo);
               }
         break;
       case 47:
       case 49:
         for ( i = 0 , N = CFR_SW_V31::numberFeat(rec); i < N ; i++) {
           CFR_SW_V31::featParts(rec,i,szFeature,&cntMachineRec);
           if ( pF = MFL.Find(szFeature))
             pF->setQty(FType,cntMachineRec);    // machine record's qty rules
           else {
             fOK = false;
             tellem.say(false,EUO_MSG_54_DIFF_25_QTY,file,"1","1",
                        szFeature,Type,Mod,SerNo);
             }
           }
         for ( i = 0 , N = MFL.NumberElements(); i < N ; i++)
           if (pF = MFL.element(i))
             if ( ( cntRecsRead = pF->qty(FType)) &&
                  !(CFR_SW_V31::containsFeat(rec,pF->feature()))) {
               tellem.say(false,EUO_MSG_54_NO_25_QTY,file,
                     (char *)IString(cntRecsRead),pF->feature(),Type,Mod,SerNo);
               fOK = false;
               }
         break;
       default: fOK = false;
       }
     break;                                      // version 31
   default: fOK = false;                         // unknown version
   }
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    void setMachRecFromCFRList(Boolean fMRDB)                            |
    |       Set the AAS or MRDB Machine Record in sync with                   |
    |       the features contained within the machine, as follows:            |
    |                                                                         |
    |         Eliminate the Machine Record's current array of                 |
    |           feature-quantity combinations.                                |
    |         Rebuild the array from the features in the object's             |
    |         Feature List taking the AAS or MRDB quantities for              |
    |         for 47 and 49 type machines, but the base quantities            |
    |         for the other type of machines for which also OEM-marked        |
    |         features are skipped.                                           |
    |           (The latter situation should not occur but better safe        |
    |             than sorry: During resolution, generic features with        |
    |             a quantity of more that one, can exist in two states:       |
    |             the original feature and 'OEM-marked'. In that state,       |
    |             the Feature List will not contain only unique               |
    |             elements. By the time this method is called                 |
    |             (currently when generating the new CFReport) the List       |
    |             should once again be unique, since we're only writing       |
    |             resolved congigurations, but then again ....                |
    \*-----------------------------------------------------------------------*/
Boolean Machine::setMachRecFromCFRList(Boolean fMRDB)
{
 Boolean fOK = false;
 MFeat * pF;
 CFRecV & rec = MachRecs[fMRDB];
 MFeat::CFRType FType = (fMRDB? MFeat::MRDBType : MFeat::AASType);
 unsigned n,N,qty;
 if (fOK = (rec.length() !=0))
   switch(machineRecVersion(fMRDB)) {
     case 20:
     case 30:
       switch(rec.type()) {
         case 47:
           CFR47::stripOffFeat(rec,0);
           for (n = 0, N = MFL.NumberElements() ; n < N ; n++)
             if ( (pF = MFL.element(n)) && ( qty = pF->qty(FType)))
               CFR47::appendFeat(rec,pF->feature(),qty);
           break;
         case 49:
           CFR49::stripOffFeat(rec,0);
           for (n = 0, N = MFL.NumberElements() ; n < N ; n++)
             if ( (pF = MFL.element(n)) && ( qty = pF->qty(FType)))
               CFR49::appendFeat(rec,pF->feature(),qty);
           break;
         case  8:
         case 25:
           CFR08::stripOffFeat(rec,0);
           for (n = 0, N = MFL.NumberElements() ; n < N ; n++)
             if ( (pF = MFL.element(n)) && ( qty = pF->baseNmbr()))
               if (!pF->isMarked())
                 CFR08::appendFeat(rec,pF->feature(),qty);
           break;
         default:fOK = false;
         }
       break;                                    // versions 20 & 30
     case 31:
       switch(rec.type()) {
         case  8:
         case 25:
           CFR08V31::stripOffFeat(rec,0);
           for (n = 0, N = MFL.NumberElements() ; n < N ; n++)
             if ( (pF = MFL.element(n)) && ( qty = pF->baseNmbr()))
               if (!pF->isMarked())
                 CFR08V31::appendFeat(rec,pF->feature(),qty);
           break;
         case 47:
         case 49:
           CFR_SW_V31::stripOffFeat(rec,0);
           for (n = 0, N = MFL.NumberElements() ; n < N ; n++)

           /*F72273 As Software products are now to be reconciled, the */
           /*base quantities are to be written to the .RET file.  AAS  */
           /*quantities are not used any more        HECC:  07/12/2002 */
           /*  if ( (pF = MFL.element(n)) && ( qty = pF->qty(FType))) */

             if ( (pF = MFL.element(n)) && ( qty = pF->baseNmbr()))
               CFR_SW_V31::appendFeat(rec,pF->feature(),qty);
           break;
           break;
         default: fOK = false;
         }
       break;                                    // version 31
     default: fOK = false;                       // unknown version
     }
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    unsigned qtyCFR(MFeat::CFRType type)                                 |
    |       Return the machine's number of features                           |
    |       from the given CFReport type                                      |
    \*-----------------------------------------------------------------------*/
unsigned Machine::qtyCFR(MFeat::CFRType type)
{
 for (unsigned i = 0, total = 0, N = MFL.NumberElements(); i < N ; i++)
   total += MFL.element(i)->qty(type);
 return total;
}

    /*-----------------------------------------------------------------------*\
    |    void applyPK( ProductKnowledge & PK )                                |
    |       Apply the given Product Knowledge to all features                 |
    |       in the List, as follows:                                          |
    |         Set the feature's Machine Readable Indicator                    |
    |         For those features which have MRDB records:                     |
    |           Set their Generic Indicator.                                  |
    |         Ensure descriptions appear in both type records.                |
    \*-----------------------------------------------------------------------*/
void Machine::applyPK( ProductKnowledge & PK )
{
 MFeat * pF;
 IString str;
 for (int i = 0 , I = MFL.NumberElements(); i < I ; i++)
   if (pF = MFL.element(i)) {
     if (pF->qty(MFeat::MRDBType)) {
       pF->setGenericInd(PK.isGenericFeature(Type,Mod,pF->feature()));
       if (!pF->hasDescription(true))
         if (pF->isGeneric() &&
             PK.strGenericDescription(Type,Mod,pF->feature(),str))
           pF->setDescription((char *)str);
         else pF->setDescription( PK.description(Type,Mod,pF->feature()));
       }
     if (pF->qty(MFeat::AASType) && !pF->hasDescription(false))
       pF->setDescription(pF->hasDescription(true)? pF->description(true) :
                                  PK.description(Type,Mod,pF->feature()),false);
     pF->setMRInd( PK.isMachineReadable(Type,Mod,pF->feature()));
     }
}


    /*-----------------------------------------------------------------------*\
    |    void resolveGenerics()                                               |
    |       Consider all features in the machine's list which                 |
    |       are generics and remain to be resolved, as follows:               |
    |                                                                         |
    |      A: Get a list of equivalent specific features                      |
    |           for the generic feature.                                      |
    |      B: Look in the machine's feature list for one                      |
    |           that might qualify as an equivalent, but                      |
    |           skip and mark such a candidate as 'Resolved' if its           |
    |           MRDB and AAS quantities correspond.                           |
    |         Determine the quantity we can apply towards the                 |
    |           generic: the lesser of the generic's MRDB quantity            |
    |           and the candidate's AAS quantity.                             |
    |         Move that that number of MRDB records from the generic to       |
    |           the candidate (It's immaterial if for version 30,             |
    |           there's only one record: the called method will handle        |
    |           it and maintain the candidate's MRDB feature count.           |
    |         Re_Set the candidate's feature since that method will           |
    |           propegate the name to the newly added records.                |
    |         Set the candidate's Instance Type to 'I' since it               |
    |           could be a 'U', which we're not allowed to put out.           |
    |         Set the candidate's base in sync with its new MRDB              |
    |           quantity.                                                     |
    |         If the candidate has a MRDB quantity:                           |
    |           Mark it 'resolved by us', add an explanation to               |
    |             the given list, set its message indicator and               |
    |             add an RPO to the given list if there's a difference        |
    |             between the candidate's AAS and MRDB quantities.            |
    |         Otherwise, mark the candidate unresolved.                       |
    |         Update the count we're trying to resolve with the               |
    |           the quantity we applied and re-start at 'B' if there's        |
    |           some left.                                                    |
    |                                                                         |
    |      C: If the generic feature has been completely consumed             |
    |            (i.e. it has no longer a MRDB quantity), release             |
    |            it's obtained storage and remove it form the                 |
    |            machine's feature list.                                      |
    |         Otherwise bring its base quantity in sync with its              |
    |            MRDB quantity and optionally mark it as a potential          |
    |            machine, the search for which follows this logic             |
    |            and consists of trying to match generic MRDB                 |
    |            features with AAS machines.                                  |
    |            (Tagging them here is done to cut down on the                |
    |             processing time for the Generic Machine logic               |
    |             since we're visiting every generic feature                  |
    |             here already).                                              |
    |                                                                         |
    |    'parrstr_comp'                                                       |
    |       is the 'C' function to compare elements from a 'PtrStrArr'        |
    |       (Pointer to String Array) used by the method.                     |
    \*-----------------------------------------------------------------------*/
int parrstr_comp(const void *p1,const void * p2)
{
 return strcmp((char *)p1,(char *)p2);
}

void Machine::resolveGenerics( ProductKnowledge & PK
                              ,Boolean fTagPotentialMachines
                              , RPO2List & RPOL
                              ,const char *msgFile
                              ,XPLN2List & XPLNL )
{
 MFeat * pFG,*pF;
 unsigned UseQty, GenQty, i,I,n,AASQty,MRQty;
 PtrStrArr GenL(5,3,parrstr_comp);
 for ( i = 0 , I = MFL.NumberElements(); i < I ; i++)
   if ( (pFG = MFL.element(i)) && (pFG->tag() == MFeat::Undone) &&
        pFG->isGeneric()) {
     if (PK.specificFeatures(Type,Mod,pFG->feature(),GenL)) {
       GenQty = pFG->qty(MFeat::MRDBType);
       for (n = 0 ; GenQty && n < I ; n++)
         if  ( (pF = MFL.element(n))         &&
               (pF->tag() == MFeat::Undone)  &&
               (pF != pFG)                   &&
               GenL.Find(pF->feature())      &&
               !pF->resolveMatches()) {
           UseQty = ALMIN((AASQty = pF->qty(MFeat::AASType)),GenQty);
           pFG->copyRecords(pF,UseQty,MFeat::MRDBType,MFeat::MRDBType,
                            machineRecVersion(true),true,false);
           pF->setFeature(pF->feature());        // for propegating reasons
           pF->setInstType('I');
           pF->setBase(MRQty = (pF->qty(MFeat::MRDBType)+UseQty));
           if (MRQty) {
             pF->setTag(MFeat::Resolved_EUOP);
             IString strDescMR(pF->description())
                    ,strDescAAS(pF->description(false));

             if (AASQty != MRQty)
               RPOL.add(RPO2(Type,Mod,SerNo,pF->feature(),
                             (char *)IString::stripBlanks(strDescAAS),
                             AASQty > MRQty? AASQty - MRQty : MRQty - AASQty,
                             AASQty > MRQty? RPO_CODE_DELETE : RPO_CODE_ADD,
                             customerNumber(), systNumberFlag(), systemType(), systemNum(), //* 83402 HECC: 07/28/2003
                             description(), category() ) );  //* 73686   HECC:  10/15/2002
             pF->setMsgInd(true);
//jt             IMessageText msgt( EUO_MSG_RES_W_AAS_FEATCODE, msgFile,
             Tellem tellem;
             IString msgt = IString( tellem.message( EUO_MSG_RES_W_AAS_FEATCODE,
                                pFG->feature(),
                                (char *)IString::stripBlanks(strDescMR),
                                pF->feature(),
                                (char *)IString::stripBlanks(strDescAAS)) );
             XPLNL.add(XPLN2(Type,Mod,SerNo,pF->feature(),(const char *)msgt));
             //edw 050797
             if (!AASQty)
             pF->copyRecords(pF,1,MFeat::AASType,MFeat::MRDBType,
                             machineRecVersion(false),false,false);
             }
           else pF->setTag(MFeat::Unresolved);
           GenQty -= UseQty;
           }
       if (!pFG->qty(MFeat::MRDBType)) {
         pFG->Release();
         delete pFG;
         MFL.DeleteIndex(i--);
         }
       else {
         pFG->setTag(fTagPotentialMachines?
                               MFeat::Potential_Machine : MFeat::Unresolved);
         pFG->setBase( pFG->qty(MFeat::MRDBType) );
         }
       }
     else pFG->setTag(fTagPotentialMachines?
                           MFeat::Potential_Machine : MFeat::Unresolved);
     }
}

    /*-----------------------------------------------------------------------*\
    |    void processAASPassthru()                                            |
    |       Set the base quantity of all the machine's features               |
    |       to that of their AAS quantities and mark the features             |
    |       'resolved' (these are machines from the AAS CFReport              |
    |       which are not to be used in the resolution process                |
    |       and are to be passed through as 'resolved'                        |
    \*-----------------------------------------------------------------------*/
void Machine::processAASPassthru()
{
 MFeat * pF;
 for ( int i = 0 , I = MFL.NumberElements(); i < I ; i++)
   if (pF = MFL.element(i)) {
     pF->setBase(pF->qty(MFeat::AASType));
     pF->setTag(MFeat::Resolved);
     }
}
    /*-----------------------------------------------------------------------*\
    |    void resolveMRCombos()                                               |
    |       Consider all features in the machine's list which                 |
    |       which have an AAS quantity, remain to be resolved                 |
    |       and for which there is an equivalent feature                      |
    |       combination as determined by the given Product                    |
    |       Knowledge, as follows:                                            |
    |                                                                         |
    |         If among the features (having no AAS quantities)                |
    |           we can find the determined combination:                       |
    |                                                                         |
    |           Determine the quantities for the maximum number of            |
    |             combinations we might apply towards the the                 |
    |             combination-equivalent feature.                             |
    |           Remove those numbers of MRDB records from the                 |
    |             the features making up the combination.                     |
    |             If that leaves them with no MRDB quanties:                  |
    |                release their obtained storage and remove                |
    |                them from the machine's feature list.                    |
    |             otherwise, leave them in the resolution game.               |
    |           Increment the combination-equivalent feature's base           |
    |             with the quantity we were able to apply, mark it            |
    |             'un/resolved' depending on whether its                      |
    |             new base quantity now equals its AAS quantity and           |
    |             copy the applied quantity number of AAS Records to          |
    |             its MRDB side ( the 'addCFR()' will take care of            |
    |             the count)                                                  |
    |           Set the feature's message indicator and add the               |
    |             created messages (one for each use combination feature)     |
    |             to the given list.                                          |
    \*-----------------------------------------------------------------------*/
void Machine::resolveMRCombos( ProductKnowledge & PK
                              ,const char *msgFile
                              ,XPLN2List & XPLNL)
{
 MFeat * pFC, *pFM1, *pFM2;
 unsigned qtyAAS, qtyMR1, qtyMR2, qtyReq1, qtyReq2, qtyFit,i;
 IString MRFeat1, MRFeat2;
 Boolean fOK;
 for ( i = 0 ; i < MFL.NumberElements() ; i++)
   if ( (pFC = MFL.element(i))              &&
        (pFC->tag() == MFeat::Undone)       &&
        (qtyAAS = pFC->qty(MFeat::AASType)) &&
        PK.comboMRFeatFromAASFeat(Type,Mod,pFC->feature(),
                                  MRFeat1,&qtyReq1,MRFeat2,&qtyReq2)) {
     fOK = true, pFM1 = pFM2 = 0;                // re-set - we're in a loop
     if ( qtyReq1 &&
          ( !(pFM1 = MFL.Find((char *)MRFeat1)) ||
             (pFM1->tag() != MFeat::Undone)     ||
             pFM1->qty(MFeat::AASType)          ||
            ((qtyMR1 = pFM1->qty(MFeat::MRDBType)) < qtyReq1)))
       fOK = false;
     if (fOK && qtyReq2 &&
          ( !(pFM2 = MFL.Find((char *)MRFeat2)) ||
             (pFM2->tag() != MFeat::Undone)     ||
             pFM2->qty(MFeat::AASType)          ||
            ((qtyMR2 = pFM2->qty(MFeat::MRDBType)) < qtyReq2)))
       fOK = false;
     if (fOK) {
       qtyFit = qtyReq1? (qtyMR1/qtyReq1) : 1;
       qtyFit = qtyReq2? ALMIN(qtyFit,(qtyMR2/qtyReq2)) : qtyFit;
       qtyFit = ALMIN(qtyFit,qtyAAS);
       if ( qtyReq1) {
//jt         IMessageText msgt( EUO_MSG_RES_COMBO, msgFile,
         Tellem tellem;
         IString msgt = IString( tellem.message( EUO_MSG_RES_COMBO,
                            (char *)IString(qtyReq1 * qtyFit),
                            pFM1->feature(), (char *)IString(qtyFit),
                            pFC->feature()) );
         XPLNL.add(XPLN2(Type,Mod,SerNo,pFC->feature(),(const char *)msgt));
//         pFM1->re_MoveCFRecs(0,(qtyReq1 * qtyFit),MFeat::MRDBType);
         pFM1->removeRecords(qtyReq1 * qtyFit,MFeat::MRDBType);
         if (!pFM1->qty(MFeat::MRDBType)) {
           pFM1->Release();
           delete pFM1;
           MFL.DeletePtr(pFM1);
           }
//       else  pFM1->setTag(MFeat::Unresolved);
// let it qualify for the non-equal algorithm as well
         }
       if ( qtyReq2 ) {
//jt         IMessageText msgt( EUO_MSG_RES_COMBO, msgFile,
         Tellem tellem;
         IString msgt = IString( tellem.message( EUO_MSG_RES_COMBO,
                           (char *)IString(qtyReq2 * qtyFit),
                           pFM2->feature(), (char *)IString(qtyFit),
                           pFC->feature(),
                     (char *)IString::stripBlanks(IString(pFC->description()))) );
         XPLNL.add(XPLN2(Type,Mod,SerNo,pFC->feature(),(const char *)msgt));
//         pFM2->re_MoveCFRecs(0,(qtyReq2 * qtyFit),MFeat::MRDBType);
         pFM2->removeRecords(qtyReq2 * qtyFit,MFeat::MRDBType);
         if (!pFM2->qty(MFeat::MRDBType)) {
           pFM2->Release();
           delete pFM2;
           MFL.DeletePtr(pFM2);
           }
//       else  pFM2->setTag(MFeat::Unresolved);
// let it qualify for the non-equal algorithm as well
         }
       pFC->setBase( pFC->baseNmbr() + qtyFit);
       pFC->setTag( (pFC->qty(MFeat::AASType) == pFC->baseNmbr())?
                                    MFeat::Resolved_EUOP : MFeat::Unresolved);
       if (qtyFit) {
         pFC->setMsgInd(true);
         pFC->copyRecords(pFC,qtyFit,MFeat::MRDBType,MFeat::AASType,
                          machineRecVersion(true),false,true);
//         pFC->setQty(MFeat::MRDBType,pFC->qty(MFeat::MRDBType) + qtyFit);
//       while(qtyFit--)
//         if (pRec = pFC->cfrList(MFeat::AASType).element(0))
//           pFC->addCFR(MFeat::MRDBType,pRec);
         }
       }
     }
}

    /*-----------------------------------------------------------------------*\
    |    void resolveNonEquals()                                              |
    |       Consider all features in the machine's list which                 |
    |       remain to be resolved and have different AAS                      |
    |       and MRDB quantities, as follows:                                  |
    |                                                                         |
    |       If there's a MRDB quantity:                                       |
    |          Set the feature's base to the MRDB quantity and mark           |
    |            it 'resolved by us'.                                         |
    |          Create the appropriate message(s) and set the                  |
    |            the feature's message indicator.                             |
    |          Create an RPO to reflect the difference between the            |
    |            the base and AAS quantities.                                 |
    |                                                                         |
    |       Otherwise,                                                        |
    |          If it's a machine-readable feature:                            |
    |             Set the feature's base to the AAS quantity (there will      |
    |               be some) and mark it 'resolved by us'.                    |
    |             Create the appropriate message(s) and set the               |
    |               the feature's message indicator.                          |
    |                                                                         |
    |          Otherwise,                                                     |
    |             If an AAS Date can be determined:                           |
    |               Mark the feature 'resolved by us' and create              |
    |                 appropriate messages.                                   |
    |               If the given Sensed Date precedes the AAS Date            |
    |                  Set the feature's base quantity to that of its         |
    |                    AAS quantity and create additional messages.         |
    |               Otherwise,                                                |
    |                  Set the feature's base quantity to 0 and               |
    |                    create additional messages.                          |
    |             Otherwise, mark the feature 'unresolved'.                   |
    |                                                                         |
    |       If messages were created, add them to the given                   |
    |           message List while a created RPO is added to its              |
    |           given list.                                                   |
    \*-----------------------------------------------------------------------*/
// new version that generates missing feature records when feature is considered
// resolved
void Machine::resolveNonEquals(  long lSensedDate
                               , RPO2List & RPOL
                               , const char *msgFile
                               , XPLN2List & XPLNL )
{
#define NO_OF_STR   4                    //jt
 MFeat * pF;
 RPO2 * pRPO;
 unsigned AASQty,MRQty,i,I,m,M;
 IString   pMT[NO_OF_STR];               //jt IMessageText * pMT[4];
 Tellem    tellem;                       //jt
 IString strDesc;
 long AASDate;
 for ( i = 0 , I = MFL.NumberElements(); i < I ; i++)
   if (pF = MFL.element(i))
     if (pF->tag() == MFeat::Undone)
       if ( (AASQty = pF->qty(MFeat::AASType))  !=
            (MRQty = pF->qty(MFeat::MRDBType)) ) {
//jt         memset(pMT,0,sizeof(pMT)),
         pRPO = 0;    // init - we're in a loop
         strDesc = IString::stripBlanks(IString(pF->description()));
         if (MRQty) {
           pF->setBase(MRQty);
           pF->setTag(MFeat::Resolved_EUOP);
           pF->setMsgInd(true);
           if (AASQty)                           // msg: conflicting quantities
             pMT[0] = IString( tellem.message(EUO_MSG_CONQUAN, pF->feature(), //jt
                                       strDesc, (char *)IString(AASQty),
                                       (char *)IString(MRQty)) );
           else {
             pMT[0] = IString( tellem.message(EUO_MSG_MRDB_Y_AAS_N,    //jt msgFile,
                                       pF->feature(), strDesc) );
             pF->copyRecords(pF,MRQty,MFeat::AASType,MFeat::MRDBType,
                             machineRecVersion(false),false,false,
                             type(),model());
             }
           pMT[1] = IString( tellem.message(EUO_MSG_RES_MRDB_QUAN,     //jt msgFile,
                                     pF->feature(),
                                     strDesc,(char *)IString(MRQty)) );
           pRPO = new RPO2(Type,Mod,SerNo,pF->feature(),strDesc,
                           AASQty > MRQty? AASQty - MRQty : MRQty - AASQty,
                           AASQty > MRQty? RPO_CODE_DELETE : RPO_CODE_ADD,
                           customerNumber(), systNumberFlag(), systemType(), systemNum(), //* 83402 HECC: 07/28/2003
                           description(), category() );  //* 73686   HECC:  10/15/2002
           }
         else if (!pF->isMachineReadable()) {
                pF->setBase(AASQty);
                pF->setTag(MFeat::Resolved_EUOP);
                pF->setMsgInd(true);
                pF->copyRecords(pF,AASQty,MFeat::MRDBType,MFeat::AASType,
                                machineRecVersion(true),false,false,
                                type(),model());
                pMT[0] = IString( tellem.message(EUO_MSG_AAS_Y_MRDB_N,  //jt msgFile,
                                          pF->feature(), strDesc) );
                pMT[1] = IString( tellem.message(EUO_MSG_NOT_MACH_READ, //jt msgFile,
                                          pF->feature(), strDesc) );
                pMT[2] = IString( tellem.message(EUO_MSG_RES_AAS_QUAN,  //jt msgFile,
                                          pF->feature(),strDesc,
                                          (char *)IString(AASQty)) );
                }
              else {
// CR26 3-30-95 [AB] Workaround date logic for AAS-only machine-readable feat
                AASDate = 0;
//              AASDate = pF->isInventory() ?
//                                    pF->installDateYMD() : pF->cradDateYMD();
                if (AASDate) {
                  pMT[0] = IString( tellem.message(EUO_MSG_AAS_Y_MRDB_N,  //jt msgFile,
                                            pF->feature(), strDesc) );
                  pMT[1] = IString( tellem.message(EUO_MSG_MACH_READ_NOT_IDENT, //jt msgFile,
                                            pF->feature(),
                                            strDesc, Type,Mod) );
                  if (AASDate >= lSensedDate) {
                    pF->setBase(AASQty);
                    pMT[2] = IString( tellem.message(EUO_MSG_NOT_INSTALLED, //jt msgFile,
                                            Type,Mod) );
                    pMT[3] = IString( tellem.message(EUO_MSG_RES_AAS_QUAN, //jt  msgFile,
                                              pF->feature(),strDesc,
                                              (char *)IString(AASQty)) );
                    }
                  else {
                    pF->setBase(0);
                    pMT[2] = IString( tellem.message(EUO_MSG_RES_AAS_QUAN0, //jt msgFile,
                                              pF->feature()) );
                    pRPO = new RPO2(Type,Mod,SerNo,pF->feature(),
                                    strDesc,AASQty,RPO_CODE_DELETE,
                                    customerNumber(), systNumberFlag(), //* 83402 HECC: 07/28/2003
                                    systemType(), systemNum(),          //* 83402 HECC: 07/28/2003
                                    description(), category() ); //* 73686   HECC:  10/15/2002
                    }
                  pF->setTag(MFeat::Resolved_EUOP);
                  pF->setMsgInd(true);
                  }
                else pF->setTag(MFeat::Unresolved);
                }
         if (pRPO) {
           RPOL.add(*pRPO);
           delete pRPO;
           }
         for (m = 0; m < NO_OF_STR ; m++)//jt
           if (pMT[m].size()) {          //jt
             XPLNL.add(XPLN2(Type,Mod,SerNo,pF->feature(),
                             (const char *)(pMT[m])));//jt
//jt             delete pMT[m];
             pMT[m] = "";                //jt
            }
         }
}

void Machine::consumeUnresolveds()
{
 MFeat * pF;
 for (int qty = 0,i = 0 , I = MFL.NumberElements(); i < I ; i++)
    if (pF = MFL.element(i))
       if (pF->tag() == MFeat::Unresolved)
          {
          if (qty = ALMIN(pF->qty(MFeat::AASType),pF->qty(MFeat::MRDBType))) {
             MFeat * pF2 = new MFeat();
             pF2->duplicate(*pF);
             MFL.Add(pF2);

             pF->setBase(qty);
             pF->setQty(MFeat::AASType,qty);
             pF->setQty(MFeat::MRDBType,qty);
             pF->setTag(MFeat::Resolved_EUOP);

             //edw 050797
             //if ( pF2->qty(MFeat::AASType) > pF2->qty(MFeat::MRDBType) )
             //   pF2->setGenericInd(false);                   //AAS overflow
             //else
             //   pF2->setGenericInd(true);                    //MRDB overflow

             pF2->markAsPartial();
             //pF2->setBase(pF2->baseNmbr() - qty);            //edw07/01/1998
             pF2->setQty(MFeat::AASType,pF2->qty(MFeat::AASType)-qty);
             pF2->setQty(MFeat::MRDBType,pF2->qty(MFeat::MRDBType)-qty);
         //*    pF2->setBase( pF2->qty(MFeat::AASType) );       //edw07/01/1998
             pF2->setBase( pF2->qty(MFeat::MRDBType) );         //66806 HECC: 11/21/2001
             }
          }
       else if ( pF->tag() == MFeat::Undone )               //edw07/01/1998
          pF->setBase( pF->qty(MFeat::AASType) );           //edw07/01/1998
}

    /*-----------------------------------------------------------------------*\
    |   void Machine::resolveServicesRecs()                                   |
    |       As the information related to the services RT 38 are not going    |
    |       to be reconciled, simply change the status of all features to     |
    |       'resolved' as per feature requirement these features must be      |
    |       written in the Matches screen without any reconciliation process  |
    |                                                                         |
    |   New: F86750  HECC - 01/13/2004                                        |
    |                                                                         |
    \*-----------------------------------------------------------------------*/

void Machine::resolveServicesRecs()
{
 MFeat * pF;
 for (int qty = 0,i = 0 , I = MFL.NumberElements(); i < I ; i++)
    if (pF = MFL.element(i))
       if ( pF->tag() == MFeat::Undone )  {
          pF->setTag( MFeat::Resolved );
          pF->setBase( pF->qty(MFeat::AASType) );
          }
}


    /*-----------------------------------------------------------------------*\
    |   IString & displaySerialNo(IString & str)                              |
    |       Return the machine's serial # as a display                        |
    |       string by eliminating the fourth character and                    |
    |       blanking out bytes 1 & 2 if they're zeroes, otherwise             |
    |       hyphenating at the third position                                 |
    \*-----------------------------------------------------------------------*/
IString & Machine:: displaySerialNo(IString & str)
{
 char Buf[MACH_SERIALNO_SIZE + 1];
 CFRPaddCopySZ(Buf,SerNo,MACH_SERIALNO_SIZE);
 Buf[MACH_SERIALNO_SIZE] = '\0';
 Buf[2] = Buf[3] = ' ';
 strcpy(Buf+2,Buf+3);                            // eliminate 4th character
 if (Buf[0] == '0' && Buf[1] == '0')
   Buf[0] = Buf[1] = Buf[2] = ' ';
 else Buf[2] = '-';
 str = IString(Buf);
 return str;
}

IString Machine:: displaySerialNoStr()
{
 IString str;
 displaySerialNo(str);
 return str;
}

    /*-------------------------------------------------------------------------------*\
    |    void Machine::getConnections(unsigned * pNextInstanceId, CFRecV & Rec54List, |
    |                                 CFRInstL & L55_In, CFRInstL & L55_Out,          |
    |                                 CFRInstL & L56_In, CFRInstL & L56_Out)          |
    |      Determine whether the 54-Records in the given pointer list have Connection |
    |      Records associated with them and if so, add the Connection records         |
    |      (found in the 'In' Files) to the 'Out' File using the new                  |
    |      Instance Ids to tie the records together.                                  |
    |      Remove the pointer from the 54-Record array if there is no Connection      |
    |      or Placement record since in version 31, there are only 54 records         |
    |      for features with those type of records.                                   |
    |                                                                                 |
    |       Some background on Connections/Placements:                                |
    |         Some Machine Features have additional                                   |
    |         information associated with them in the form of other                   |
    |         Records which are tied to the feature's 54-Record                       |
    |         via their common 'InstanceId' field.                                    |
    |         In a CFReport, these ids need to start at 1 and be                      |
    |         consecutive, while the records are to be grouped                        |
    |         together by type and appended to the CFReport.                          |
    |         During reconciliation, a feature might disappear                        |
    |         which could violate the Instance Id order.                              |
    |         We handle the Placement (55-Record) and Connection                      |
    |         (56-Record) records which are accumulated in lists                      |
    |         as the CFReport are read in.                                            |
    |         These 'In' lists are used as a source (via the 54-Record's              |
    |         Instance Id) for the 'Out' lists                                        |
    |         which will consist of those records that belong to features             |
    |         left over after the reconciliation process, but re-numbered             |
    |         with fresh consecutive Instance Ids.                                    |
    |         Note: since a 54-record has only one Instance Id field,                 |
    |               we're unable to handle a feature that has both                    |
    |               connections and placements.                                       |
    \*-------------------------------------------------------------------------------*/
//edw 05/08/98-changed logic to support an 54 record with (0 to 1) 55 records
//             AND (0 to n) 56 records per request of the S/390 product group.
//edw 05/20/98-changed logic to eliminate the regeneration of instance ids per
//             request of S/390 group.  Apparently it's ok that the ids are not
//             consecutive

void Machine::getConnections(unsigned * pNextInstanceId, CFRecVL & Rec54List,
                             CFRInstL & L55_In, CFRInstL & L55_Out,
                             CFRInstL & L56_In, CFRInstL & L56_Out)
{
 CFRecV * pRec;
 IString strInstId;
 const char* s56Inst;
 CFRInst * pConn1, * pConn2, * p56Inst;
 Boolean fPlace_Conn = false;

 for (int i = 0, I = Rec54List.NumberElements(); i < I ; i++,fPlace_Conn = false)
   if ((pRec = Rec54List.element(i)) && CFR54::instId(pRec,strInstId)) {
     if (fPlace_Conn = ((pConn1 = L55_In.find((char *)strInstId)) &&
                        (pConn2 = new CFRInst(*pConn1)))) {
        //edw pConn2->setInstId(*pNextInstanceId);
        L55_Out.AddasLast(pConn2);
        }

     for (int j = 0, J = L56_In.NumberElements(); j < J; j++)
        {
        p56Inst = L56_In.element(j);
        s56Inst = p56Inst->instId();
        IString str56Inst = s56Inst;
        if ( str56Inst == strInstId)
           {
           pConn2 = new CFRInst(*p56Inst);
           //edw pConn2->setInstId(*pNextInstanceId);
           L56_Out.AddasLast(pConn2);
           fPlace_Conn = true;
           }
        }
     //if (fPlace_Conn = ((pConn1 = L56_In.find((char *)strInstId)) &&
     //                   (pConn2 = new CFRInst(*pConn1)))) {
     //  pConn2->setInstId(*pNextInstanceId);
     //  L56_Out.AddasLast(pConn2);
     //  }
     if (fPlace_Conn){
        //edw CFR54::setInstId(pRec,(char *)IString((*pNextInstanceId)++));
        }
     else
        {
        Record_54 *p54;
        p54 = (Record_54 *)(pRec->record());
        if (p54->InstType != 'S')
           {
           CFR54::setInstId(pRec," ");
           Rec54List.DeletePtr(pRec);                // remove form array
           i--;
           }
        }
     }
}

unsigned Machine::referenceNotesNumber(Boolean fMRDB)
{
 return CFR95::referenceNotesNumber(infoRecList(fMRDB).element(0));
}


 /*--------------------------------------------------------------------------*\
 |                         Non-Compiled testing code                          |
 \*--------------------------------------------------------------------------*/
#ifdef UNITTEST

#include <iostream.h>


void main(void)
{

 Machine *pM1, *pM2;
 MFeat * pF,*pF2;
 if (1)
   {
      CFRecV rec1("5411111111111112345");
      CFRecV rec2("2522222222222222245");
      CFRecV rec3("0033333333333333345");
      CFRecV rec4("0044444444444444444");
      CFRecV rec5("0055555555555555555");
      pF = new MFeat("F12345");
      pF->addCFR(MFeat::AASType,&rec1);
      pF->addCFR(MFeat::AASType,&rec2);
      pF->addCFR(MFeat::AASType,&rec3);
      pF2 = new MFeat("F23456");
      pF2->addCFR(MFeat::AASType,&rec4);
      pF2->addCFR(MFeat::AASType,&rec5);
      pM1 = new Machine("typ3","model3","260000333");
      pM1->mfeatList().Add(pF);
      pM2 = new Machine("typ4","model4","444");
      pM2->mfeatList().Add(pF2);
      cout << *pM1;

      pF->setBase(3);
      pF2->setBase(4);
      pM1->setMachRecFromCFRList(0);
      pM1->setMachRecFromCFRList(1);
      pM2->setMachRecFromCFRList(0);
      pM2->setMachRecFromCFRList(1);
      pM1->setMachCFR(0,"259021       2111      500       1   9063       1   9922       3");
      pM1->setMachCFR(1,"089021       2111      500       1   9063       1   9922       3");
      pM2->setMachCFR(0,"259021       2111      500       1   9063       1");
      pM2->setMachCFR(1,"089021       2111      500       1   9063       1");




      FILE *f;
      unsigned Total = 0, Bytes = 0;
      Total += pM1->fWriteBLength();
      Total += pM1->fWriteBLength();
      cout << endl << "Total fWriteBLength() : " << Total << endl;
      Total = 0;
      if (f = fopen("MACH.TST","wb"))
        {
         if (pM1->fWriteB(f,&Bytes))
           Total += Bytes;
         if (pM2->fWriteB(f,&Bytes))
           Total += Bytes;

         cout << endl << "Total bytes written : " << Total  << endl;
         fclose(f);
         if (f = fopen("MACH.TST","rb"))
           {
            Machine * pM3 = new Machine();
            Machine * pM4 = new Machine();
            Total = 0;
            if (pM3->fReadB(f,&Bytes))
               Total += Bytes;
            if (pM4->fReadB(f,&Bytes))
               Total += Bytes;
            fclose(f);
            cout << endl << "Total bytes read : " << Total  << endl;
            cout << *pM3 << endl;
            cout << *pM4 << endl;
            cout << " pM3->cfrMach(0).numberFeat() : "
                 << CFR08::numberFeat(pM3->cfrMach(0)) << endl;
//          cout << " pM3->cfrMach(1).numberFeat() : "
//               << pM3->cfrMach(1).numberFeat() << endl;
//          cout << " pM4->cfrMach(0).numberFeat() : "
//               << pM4->cfrMach(0).numberFeat() << endl;
//          cout << " pM4->cfrMach(1).numberFeat() : "
//               << pM4->cfrMach(1).numberFeat() << endl;
            delete pM3;
            delete pM4;
           }
        }
   }

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif
 delete pM1;
 delete pM2;
 delete pF;
 delete pF2;
#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif


}

#endif
#if 0
#endif

