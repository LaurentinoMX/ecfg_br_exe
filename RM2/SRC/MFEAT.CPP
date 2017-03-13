 /*--------------------------------------------------------------------------*\
 |    Filename - MFEAT.CPP                                                    |
 |      contains source for the following RM/2 class implementations:         |
 |                                                                            |
 |     MFeat    - A CFReport Machine Feature.                                 |
 |     CFRStore - Retains information of a MFeats's Record List               |
 |                when the object is written to a file.                       |
 |                This data is used when the object is rebuild.               |
 |                One object for every list.                                  |
 |                ( MFeat's are written to a file without their               |
 |                  Record Lists. The records in these lists                  |
 |                  are numbered when collected ( via MFeat's 'pickup()'      |
 |                  method) throughout the System                             |
 |                  and written separately into the file.                     |
 |                  After the object is read from the file, it can            |
 |                  identify its records in the list read separately          |
 |                  and rebuild its own Lists ( via MFeat's                   |
 |                  'delivery()' method), thereby restoring itself            |
 |                  to its former state).                                     |
 |                                                                            |
 |                                                                            |
 | MODIFICATIONS:                                                             |
 |                                                                            |
 | RERC  2006/03/03  D106677  Function affected: resolveUsingNoCompareTable1()|
 |                            New code added in order to verify the presence  |
 |                            of the feature inside the No Compare Table,     |
 |                            no matter the information provided by the       |
 |                            mistmatch type column to determine the          |
 |                            acceptable/not acceptable state.                |
 |                                                                            |
 | SAQP 2008/06/19 F124059    Function affected: resolveUsingNoCompareTable() |
 |                            Features marked with an 'exclude' flag are set  |
 |                            with quantity 0, to avoid their automatic       |
 |                            reconciliation                                  |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |              Standard Library Includes              |
        \*---------------------------------------------------*/
#include <string.h>
#include <stddef.h>

        /*---------------------------------------------------*\
        |                   ICLUI Includes                    |
        \*---------------------------------------------------*/
#include <isynonym.hpp>                          // Definition of Boolean etc

        /*---------------------------------------------------*\
        |               RM/2-specific includes                |
        \*---------------------------------------------------*/
#include "mfeat.hpp"                             // this file's declarations
#include "euoutil.hpp"                           // Utilities
#include "mainw.hpp"

 /*--------------------------------------------------------------------------*\
 |                               CFRStore Class                               |
 \*--------------------------------------------------------------------------*/
CFRStore & CFRStore::operator=(CFRStore const& other)
{
 if (this != &other)
   memcpy(this,&other,sizeof(*this));
 return *this;
}

 /*--------------------------------------------------------------------------*\
 |                                MFeat Class                                 |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |                        Ctors                        |
        \*---------------------------------------------------*/
MFeat::MFeat()
{
 init();
}

MFeat::MFeat(const char * feature,Boolean fGen, Boolean fMachReadable)
{
 init(feature);
 fGeneric = fGen;
 fMR = fMachReadable;
 fMsg = false;
 cMark = ' ';
}

    /*-----------------------------------------------------------------------*\
    |    void init(const char * feature)                                      |
    |       Private common initialize method                                  |
    \*-----------------------------------------------------------------------*/
void MFeat::init(const char * feature)
{
 mystrstrip(mystrncpy(Feat,feature,sizeof(Feat)-1));
 Nmbr = Base = 0;
 memset(Qty,0,sizeof(Qty));                      // null out all quantities
 Tag = Undone;
 fGeneric = fMR = fMsg = false;
 cMark = ' ';
}

//int MFeat::compare(const char * szFeat) const
//{
// int i = (strlen(Feat) - strlen(szFeat));
// return (!i? strcmp(Feat,szFeat) : i);
//}

    /*-----------------------------------------------------------------------*\
    |    int compare(const char * szFeat) const                               |
    |       Sorts numerical features before alphabetical-ones                 |
    \*-----------------------------------------------------------------------*/
int MFeat::compare(const char * szFeat) const
{
 IString ThisFeat(Feat), OtherFeat(szFeat);
 int i = 0;
 if (ThisFeat != OtherFeat)
   if (ThisFeat.isDigits())
     i = OtherFeat.isDigits()? (ThisFeat.asInt() - OtherFeat.asInt()) : -1;
   else i = OtherFeat.isDigits()? 1 : strcmp(Feat,szFeat);
 return i;
}

    /*-------------------------------------------------------*\
    |                       Assignment                        |
    \*-------------------------------------------------------*/
MFeat & MFeat::operator=(MFeat const& other)
{
 if (&other != this) {
   BaseInit(other);                              // copies all but lists
   for (int i = 0, I = (sizeof(CFRLists)/sizeof(CFRLists[0])) ; i < I ; i++ )
     CFRLists[i] = other.CFRLists[i];
   }
 return *this;
}

    /*-----------------------------------------------------------------------*\
    |    MFeat & duplicate(MFeat const& other)                                |
    |       Copy the given object and and new the contents of the             |
    |       Record List. ( as opposed to the assignment operator, which       |
    |       copies the Lists which contain Record pointers.                   |
    \*-----------------------------------------------------------------------*/
MFeat & MFeat::duplicate(MFeat const& other)
{
 Release();
 CFRecV * pRec, *pRec2;
 int i,I,j,J,len;
 BaseInit(other);
 for (i = 0, I = (sizeof(CFRLists)/sizeof(CFRLists[0])) ; i < I ; i++ ) {
   CFRLists[i].EmptyOut();
   for (j = 0,J = other.CFRLists[i].NumberElements() ; j < J ; j++)
     if (pRec2 = other.CFRLists[i].element(j)) {
       switch(pRec2->type()) {
         case 54: len = sizeof(Record_54); break;
         case 96: len = sizeof(Record_96);
         }
       if (pRec = new CFRecV(ALMAX(len,pRec2->length()), pRec2->record(),' '))
         CFRLists[i].AddasLast(pRec);
       }
   }
 return *this;
}

    /*-----------------------------------------------------------------------*\
    |    void BaseInit(MFeat const& other)                                    |
    |       Copies all but the Record Lists                                   |
    \*-----------------------------------------------------------------------*/
void MFeat::BaseInit(MFeat const& other)
{
 mystrncpy(Feat,other.Feat,sizeof(Feat)-1);
 Nmbr    = other.Nmbr;
 Base    = other.Base;
 Tag     = other.Tag;
 fMsg = other.fMsg;
 fMR     = other.fMR;
 fGeneric = other.fGeneric;
 cMark = other.cMark;
 memcpy(Qty,other.Qty,sizeof(Qty));              // copy all quantities
 for (int i = 0,I = (sizeof(CFRStoreInfo)/sizeof(CFRStoreInfo[0])); i < I ;i++)
   CFRStoreInfo[i] = other.CFRStoreInfo[i];
}

    /*-----------------------------------------------------------------------*\
    |    Boolean addCFR(CFRType type,const char * Buffer)                     |
    |       add the given buffer as CFR Record and maintain the               |
    |       record count for the given type, as follows:                      |
    |       For 'OtherType': add a 'newed' record to the list                 |
    |                        and increment the count.                         |
    |                        (These are feature comment type records)         |
    |       Otherwise: only 'new' a record (of 54-Record size or larger,      |
    |                  such that subsequent longer descriptions will fit),    |
    |                  if it would be the first of that type and              |
    |                  increment the count.                                   |
    |                  (This was done to cut down on the storage              |
    |                   since multiple CFReport 54 records for the            |
    |                   same feature from the same CFR File (AAS or MRDB)     |
    |                   are identical.)                                       |
    |                                                                         |
    |       Note: The returned Boolean indicates whether a record was         |
    |             'newed'                                                     |
    \*-----------------------------------------------------------------------*/
Boolean MFeat::addCFR(CFRType type,const char * Buffer, Boolean fKeepCount)
{
 CFRecV * pRec;
 Boolean fOK;
 int len;
 if (type == OtherType) {
   if (fOK = ((pRec = new CFRecV(Buffer)) != 0)) {
     CFRLists[type].AddasLast(pRec);
     if (fKeepCount)
       Qty[type]++;
     }
   }
 else if (!(fOK = (CFRLists[type].NumberElements() != 0))) {
        switch(CFRecType(Buffer)) {
          case 54: len = sizeof(Record_54); break;
          case 96: len = sizeof(Record_96);
          default: fOK = false;
          }
        if (!fOK &&
            (fOK=((pRec=new CFRecV(ALMAX(len,strlen(Buffer)),Buffer,' '))!=0))) {
            CFRLists[type].AddasLast(pRec);
            if (fKeepCount)
              Qty[type] = 1;
            }
        }
      else if (fKeepCount)
             Qty[type]++;
 return fOK;
}

int MFeat::fWriteCFR(FILE *f, CFRType  type,unsigned version)
{
 int n = 0 ,N;
 if (type == OtherType) {
    n = CFRLists[type].fWriteCFR(f);
    n = (n == CFRLists[type].NumberElements())? n : -1;
    }
 else {
   CFRecV * pRec = CFRLists[type].element(0);
   if (pRec)
     switch(version) {
       case 20:
         if (pRec->type() == 54) {
           CFR54::enforceVersion20Format(pRec);
           for (N=baseNmbr();(n < N) && pRec->fWriteCFR(f) ;n++);
           n = ( n == N)? n : -1;
           }
         break;
       default: n = pRec->fWriteCFR(f)? 1 : -1;
       }
   }
 return n;
}

Boolean MFeat::addCFR(CFRType type,CFRecV *pRec,Boolean fKeepCount)
{
 return (pRec? addCFR(type,pRec->record(),fKeepCount):false);
}

Boolean MFeat::addCFR(CFRType type,CFRecV & rec,Boolean fKeepCount)
{
 return addCFR(type,rec.record(),fKeepCount);
}

Boolean MFeat::addCFR(Boolean fMRDB,Record_54 & r,Boolean fKeepCount)
{
 return addCFR(fMRDB? MRDBType: AASType,(char *)&r,fKeepCount);
}

int MFeat::Write(FILE *f)
{
 return CFRLists[AASType].fWriteA(f);
}

int MFeat::Write(FILE *f,CFRType type)
{
 return CFRLists[type].fWriteA(f);
}

    /*-----------------------------------------------------------------------*\
    |    void Release()                                                       |
    |       release the obtained storage associated with the object.          |
    |       i.e. the records in the Record Lists.                             |
    \*-----------------------------------------------------------------------*/
void MFeat::Release()
{
 for (int i = 0, I = (sizeof(CFRLists)/sizeof(CFRLists[0])) ; i < I ; i++ )
   CFRLists[i].Release();
}

    /*-----------------------------------------------------------------------*\
    |    void setModel(const char *szModel)                                   |
    |       Set the model number in all 54 Records of the Record lists.       |
    \*-----------------------------------------------------------------------*/
void MFeat::setModel(const char *szModel)
{
 CFRecV * pRec;
 for (int j,k,i = 0, I = (sizeof(CFRLists)/sizeof(CFRLists[0])) ; i < I ; i++ )
   for (j = 0,k = CFRLists[i].NumberElements() ; j < k ; j++)
     if (pRec = CFRLists[i].element(j))
       switch (pRec->type()) {
         case 54: CFR54::setMachModel(pRec,szModel);break;
         case 96: break;                         // doesn't have one
       }
}

    /*-----------------------------------------------------------------------*\
    |    void setSerialNo(const char *szNmbr)                                 |
    |       Set the model number in all 54 Records of the Record lists.       |
    \*-----------------------------------------------------------------------*/
void MFeat::setSerialNo(const char *szNmbr)
{
 CFRecV * pRec;
 for (int j,k,i = 0, I = (sizeof(CFRLists)/sizeof(CFRLists[0])) ; i < I ; i++ )
   for (j = 0,k = CFRLists[i].NumberElements() ; j < k ; j++)
     if (pRec = CFRLists[i].element(j))
       switch (pRec->type()) {
         case 54: CFR54::setSerialNo(pRec,szNmbr);break;
         case 96: break;                         // doesn't have one
       }
}

    /*-----------------------------------------------------------------------*\
    |    void pickUp(CFRecVL & RecList)                                       |
    |       For every Record List with elements:                              |
    |          Determine and retain the first record number and               |
    |             number of records.                                          |
    |          Number the records and add them to the given list.             |
    \*-----------------------------------------------------------------------*/
void MFeat::pickUp(CFRecVL & RecList)
{
 CFRecV * pRec;
 unsigned RecNumber = RecList.NumberElements() + 1 ;
 int i,I,j,k;
 for (i = 0, I = (sizeof(CFRLists)/sizeof(CFRLists[0])) ; i < I ; i++ )
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
    |    Boolean delivery(CFRecVL & RecList)                                  |
    |       For every Record List:                                            |
    |          Use its first record number and qty ( as retained              |
    |           in its CFRStore object) to determine the range                |
    |           of record numbers. Use these numbers to obtain the            |
    |           records from the given list.                                  |
    |           (Storage for the records in the given list was obtained       |
    |            when the list was build, and here we use the array's         |
    |            'AddasLast' method to merely add the record pointers)        |
    \*-----------------------------------------------------------------------*/
Boolean MFeat::delivery(CFRecVL & RecList)
{
 CFRecV * pRec;
 int i,I,j,k;
 Boolean fOK = true;
 for (i = 0,I=(sizeof(CFRStoreInfo)/sizeof(CFRStoreInfo[0])) ; i < I ; i++ )
   for (j = 0,k = CFRStoreInfo[i].RecNmbr; fOK && j < CFRStoreInfo[i].qty; j++,k++)
     if (fOK = ((pRec = RecList.find(k)) != 0))
       CFRLists[i].AddasLast(pRec);
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean resolveMatches()                                             |
    |       Mark the object as resolved and set its base quantity             |
    |       only if the AAS quantity equals that of the MRDB file             |
    |       and indicate whether the feature was resolved that way            |
    \*-----------------------------------------------------------------------*/
Boolean MFeat::resolveMatches()
{
 if ( Qty[AASType] == Qty[MRDBType] ) {
   Tag = Resolved;
   Base = Qty[MRDBType];
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean resolveUsingNoCompareTable()                                 |
    |       Resolve unresolved features by using the S390                     |
    |          No Compare table logic.  Mark the features as resolved         |
    |          and set their base quantities.                                 |
    |       No Compare Table logic:                                           |
    |          if ( feature not in No Compare Table )                         |
    |             resolved to MRDB quantity;                                  |
    |          else                                                           |
    |             resolved to AAS quantity;                                   |
    \*-----------------------------------------------------------------------*/
Boolean MFeat::resolveUsingNoCompareTable( IString strType )
{                                     //49222 - MAMT - 06/27/2000 eliminate pNoCompTable
 switch ( tag() )
    {
    case Undone:
    case Unresolved:
    case Potential_Machine:
       if( XWindow::bShowS390Results )     //edw10/19/1999
          Tag = NoCompareTblResolved;
       else
          Tag = Resolved;

/*       IString strTypeFeat = strType + (IString)feature(); 49222-MAMT-06/27/2000 old logic
       if ( pNoCompTable->contains( strTypeFeat ) )
          Base = Qty[AASType];
       else
          Base = Qty[MRDBType]; 49222-MAMT-06/27/2000 old logic
       if (strTypeFeat.indexOf("8P",1))     // erg  D47712  03072000
          Base = Qty[AASType];              // erg  D47712  03072000 */

       if ( Qty[MRDBType] &&  !Qty[AASType] )
          copyRecords(this,Qty[MRDBType],MFeat::AASType,MFeat::MRDBType,
                             31,false,false,strType);  //,model());
       if ( !Qty[MRDBType] &&  Qty[AASType] )
          copyRecords(this,Qty[AASType],MFeat::MRDBType,MFeat::AASType,
                             20,false,false,strType);  //,model());
       return true;
    }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    void resolveUsingNoCompareTable1() 49222 - MAMT 08/13/2000           |
    |       Resolve unresolved features by using the S390                     |
    |          No Compare table logic.  Mark the features as resolved         |
    |          and set their base quantities.                                 |
    |       No Compare Table logic:                                           |
    |          if ( feature not in No Compare Table )                         |
    |             resolved to MRDB quantity;                                  |
    |          else                                                           |
    |             resolved to AAS quantity;                                   |
    \*-----------------------------------------------------------------------*/
void MFeat::resolveUsingNoCompareTable1( ISortedSet<IString>* pNoCompTable, IString strType, IString strModel )
{
 switch ( tag() )
    {
    case Undone:
    case Unresolved:
    case Potential_Machine:

       IString nfeat = (IString)feature(); // new logic 55982 MAMT 31/10/00
// cases *** , **A , *A* , *AB , A** , A*B , AB* , ABC
       IString strTypeFeat1 = strType + "*** " + nfeat.rightJustify(8,' ');
       IString strTypeFeat2 = strType + "**"   + strModel.subString(3, 1) + " "
                              + nfeat.rightJustify(8,' ');
       IString strTypeFeat3 = strType + "*" + strModel.subString(2, 1) + "*"
                              + " "   + nfeat.rightJustify(8,' ');
       IString strTypeFeat4 = strType + "*" + strModel.subString(2, 2) + " "
                              + nfeat.rightJustify(8,' ');
       IString strTypeFeat5 = strType + strModel.subString(1, 1) + "** "
                              + nfeat.rightJustify(8,' ');
       IString strTypeFeat6 = strType + strModel.subString(1, 1) + "*"
                              + strModel.subString(3, 1) + " " + nfeat.rightJustify(8,' ');
       IString strTypeFeat7 = strType + strModel.subString(1, 2) + "* "
                              + nfeat.rightJustify(8,' ');
       IString strTypeFeat8 = strType + strModel + " "   + nfeat.rightJustify(8,' ');

//       IString strTypeFeat  = strType + (IString)feature(); old logic 55982
       /* D06677 BEGIN RERC 03/01/2006. The next code is obsolete. It is replaced
          by the code marked by the F104628 and D106677 */
       /*if ( pNoCompTable->contains( strTypeFeat1 ) ||
            pNoCompTable->contains( strTypeFeat2 ) ||
            pNoCompTable->contains( strTypeFeat3 ) ||
            pNoCompTable->contains( strTypeFeat4 ) ||
            pNoCompTable->contains( strTypeFeat5 ) ||
            pNoCompTable->contains( strTypeFeat6 ) ||
            pNoCompTable->contains( strTypeFeat7 ) ||
            pNoCompTable->contains( strTypeFeat8 ) )
          Base = Qty[AASType];
       else
          Base = Qty[MRDBType];                // new logic 55982 MAMT 31/10/00
      */
      //D106677 END RERC 03/01/2006
//         if (strTypeFeat.indexOf("8P",1))    // erg  D47712  03072000 old code MAMT
/*         if (strTypeFeat.indexOf("8P",1) || strTypeFeat.indexOf("SC",1) )  // 55686 temp change MAMT 20/10/2000
            Base = Qty[AASType];               // erg  D47712  03072000
         else
            Base = Qty[MRDBType]; old logic 55982 */

       /*F104628 BEGIN.  RERC 30/01/2006
                         Obtain the Mismatch Type information from the no compare table*/
       ICursor* myCursor = pNoCompTable->newCursor();
       pNoCompTable->setToFirst( *myCursor );
       const char * cReadElement;
       cMismType = ' ';
       forCursor( *myCursor )
                {
                 if ( strTypeFeat1 == pNoCompTable->elementAt( *myCursor).subString(1, 16) ||
                      strTypeFeat2 == pNoCompTable->elementAt( *myCursor).subString(1, 16) ||
                      strTypeFeat3 == pNoCompTable->elementAt( *myCursor).subString(1, 16) ||
                      strTypeFeat4 == pNoCompTable->elementAt( *myCursor).subString(1, 16) ||
                      strTypeFeat5 == pNoCompTable->elementAt( *myCursor).subString(1, 16) ||
                      strTypeFeat6 == pNoCompTable->elementAt( *myCursor).subString(1, 16) ||
                      strTypeFeat7 == pNoCompTable->elementAt( *myCursor).subString(1, 16) ||
                      strTypeFeat8 == pNoCompTable->elementAt( *myCursor).subString(1, 16)   )
                     {
                      cReadElement = pNoCompTable->elementAt( *myCursor);
                      cMismType = cReadElement[18];
                      Base = Qty[AASType];   //D106677 RERC  03/01/2006
                      break;
                     }
                  else
                      Base = Qty[MRDBType];  //D106677 RERC  03/01/2006
                }
        if (cMismType == ' ')
            cMismType = 'U';
        //F104628 END. RERC 30/01/2006
       if (cMismType == 'X')   // F124059 SAQP 06/19/2008 if the FC is marked as 'excluded',  the Base qty
        Base = 0;              // is set to 0, avoiding this feature to be reconciled automatically

    }
}


    /*-----------------------------------------------------------------------*\
    |    Boolean getConnections( Boolean fMRDB,unsigned InstanceId,           |
    |                            CFRInstL & L55_In, CFRInstL & L55_Out,       |
    |                            CFRInstL & L56_In, CFRInstL & L56_Out)       |
    |      Determine whether the feature has a Placement or Connection        |
    |      Record associated with it and if so, add the record                |
    |      (found in the 'In' Files) to the 'Out' File with the given         |
    |      new Instance Id while retaining that Id in the feature's           |
    |      AAS or MRDB 54-Record.                                             |
    |      Indicate whether a connection was made.                            |
    |                                                                         |
    |       Some background on Connections/Placements:                        |
    |         Some Machine Features have additional                           |
    |         information associated with them in the form of other           |
    |         Records which are tied to the feature's 54-Record               |
    |         via their common 'InstanceId' field.                            |
    |         In a CFReport, these ids need to start at 1 and be              |
    |         consecutive, while the records are to be grouped                |
    |         together by type and appended to the CFReport.                  |
    |         During reconciliation, a feature might disappear                |
    |         which could violate the Instance Id order.                      |
    |         We handle the Placement (55-Record) and Connection              |
    |         (56-Record) records which are accumulated in lists              |
    |         as the CFReport are read in.                                    |
    |         These 'In' lists are used as a source (via the feature's        |
    |         Instance Id) for the 'Out' lists                                |
    |         which will consist of those records that belong to features     |
    |         left over after the reconciliation process, but re-numbered     |
    |         with fresh consecutive Instance Ids.                            |
    |         Note: since a 54-record has only one Instance Id field,         |
    |               we're unable to handle a feature that has both            |
    |               connections and placements.                               |
    \*-----------------------------------------------------------------------*/
//edw 05/20/98-changed logic to eliminate the regeneration of instance ids per
//             request of S/390 group.  Apparently it's ok that the ids are not
//             consecutive

Boolean MFeat::getConnections( Boolean fMRDB,unsigned InstanceId,
                               CFRInstL & L55_In, CFRInstL & L55_Out,
                               CFRInstL & L56_In, CFRInstL & L56_Out)
{
 CFRecV * pRec;
 if ( !(pRec = CFRLists[fMRDB].element(0)) || (pRec->type() != 54))
   return false;
 Boolean fPlace_Conn = false;
 CFRInst * pConn1, * pConn2;
 IString strInstId;
 CFR54::instId(pRec,strInstId);
 if (strInstId.length()) {
   if (fPlace_Conn = ((pConn1 = L55_In.find((char *)strInstId)) &&
                     (pConn2 = new CFRInst(*pConn1)))) {
     //edw pConn2->setInstId(InstanceId);
     L55_Out.AddasLast(pConn2);
     }
   else if (fPlace_Conn = ((pConn1 = L56_In.find((char *)strInstId)) &&
                           (pConn2 = new CFRInst(*pConn1)))) {
          //edw pConn2->setInstId(InstanceId);
          L56_Out.AddasLast(pConn2);
          }
   if (fPlace_Conn){
     //edw CFR54::setInstId(pRec,(char *)IString(InstanceId));
     }
   else CFR54::setInstId(pRec," ");
   }
 return (fPlace_Conn);
}

Boolean MFeat::getConnections( CFRType type,unsigned InstanceId,
                               CFRInstL & L55_In, CFRInstL & L55_Out,
                               CFRInstL & L56_In, CFRInstL & L56_Out)
{
 return (type == MRDBType || type == AASType)?
  getConnections(type==MRDBType,InstanceId,L55_In,L55_Out,L56_In,L56_Out):false;
}

Boolean MFeat::setInstType(char type,Boolean fMRDB)
{
 return CFR54::setInstType(CFRLists[fMRDB].element(0),type);
}

unsigned MFeat::referenceNotesNumber(Boolean fMRDB)
{
 return CFR96::referenceNotesNumber(CFRLists[fMRDB].element(0));
}

    /*-----------------------------------------------------------------------*\
    |    Boolean markAsOEM()                                                  |
    |       Set the feature's MRDB 54-Record Instance Id to 'O'               |
    |       and prefix its description with 'OEM '                            |
    \*-----------------------------------------------------------------------*/
Boolean MFeat :: markAsOEM()
{
 CFRecV * pRec = CFRLists[1].element(0);
 if (pRec && pRec->length()) {
             /*--------------------------------------------------*\
             | this implies there is a MRDB record. These records |
             | are 'AddCFR-ed' i.e. there is enough storage for a |
             | complete Record-54 text                            |
             \*--------------------------------------------------*/
   cMark = 'O';
   return setDescription((char *)IString(IString("(OEM) ")+description(true)),true);
   }
 return false;
}

Boolean MFeat :: isMarkedAsOEM()
{
// return (CFR54::instanceType(CFRLists[1].element(0)) == 'O');
 return (cMark == 'O');
}

void MFeat :: markAsPartial()
{
 cMark = 'P';
}

Boolean MFeat :: isMarkedAsPartial()
{
 return (cMark == 'P');
}

Boolean MFeat :: isMarked()
{
 return (cMark != ' ');
}

void MFeat :: unMark()
{
  cMark = ' ';
}

    /*-----------------------------------------------------------------------*\
    |    Boolean setFeature(const char * Feature,Boolean fMRDB)               |
    |       Set the feature's name and propegate it throught the              |
    |       AAS and MRDB 54-Records.                                          |
    \*-----------------------------------------------------------------------*/
Boolean MFeat::setFeature(const char * Feature,Boolean fMRDB)
{
 CFRecV * pRec = CFRLists[fMRDB].element(0);
 Boolean fOK[2];
 for (int n = 0; n<2; n++) {
   fOK[n] = false;
   if (n == 1)
     pRec = CFRLists[fMRDB? 0 : 1].element(0);
   if (pRec && pRec->length())
     switch(pRec->type()) {
       case 54: fOK[n] = CFR54::setFeature(pRec,Feature);break;
       case 96: fOK[n] = CFR96::setFeature(pRec,Feature);break;
       }
   }
 if (fOK[0] || fOK[1]) {
   mystrstrip(mystrncpy(Feat,Feature,sizeof(Feat)-1));
   return true;
   }
 return false;
}

Boolean MFeat::setInstallDate(long lYMD,Boolean fMRDB)
{
 return CFR54::setInstallDate(CFRLists[fMRDB].element(0),lYMD);
}

Boolean MFeat::setInstallDate(const IDate & date,Boolean fMRDB)
{
 return CFR54::setInstallDate(CFRLists[fMRDB].element(0),date);
}

Boolean MFeat::setCRADDate(long lYMD,Boolean fMRDB)
{
 return CFR54::setCRADDate(CFRLists[fMRDB].element(0),lYMD);
}

Boolean MFeat::setCRADDate(const IDate & date,Boolean fMRDB)
{
 return CFR54::setCRADDate(CFRLists[fMRDB].element(0),date);
}

Boolean MFeat::setCSDDate(long lYMD,Boolean fMRDB)
{
 return CFR54::setCSDDate(CFRLists[fMRDB].element(0),lYMD);
}

Boolean MFeat::setCSDDate(const IDate & date,Boolean fMRDB)
{
 return CFR54::setCSDDate(CFRLists[fMRDB].element(0),date);
}

long MFeat::installDateYMD(Boolean fMRDB)
{
 return CFR54::installDateYMD(CFRLists[fMRDB].element(0));
}

char MFeat::instanceType(Boolean fMRDB)
{
 return CFR54::instanceType(CFRLists[fMRDB].element(0));
}

long MFeat::cradDateYMD(Boolean fMRDB)
{
 return CFR54::cradDateYMD(CFRLists[fMRDB].element(0));
}

long MFeat::csdDateYMD(Boolean fMRDB)
{
 return CFR54::csdDateYMD(CFRLists[fMRDB].element(0));
}

Boolean MFeat::installDate(IDate & date,Boolean fMRDB)
{
 return CFR54::installDate(CFRLists[fMRDB].element(0),date);
}

Boolean MFeat::cradDate(IDate & date,Boolean fMRDB)
{
 return CFR54::cradDate(CFRLists[fMRDB].element(0),date);
}

Boolean MFeat::csdDate(IDate & date,Boolean fMRDB)
{
 return CFR54::csdDate(CFRLists[fMRDB].element(0),date);
}

Boolean MFeat::isInventory()
{
 return CFR54::isInventory(CFRLists[AASType].element(0));
}

CFR54::SourceType MFeat::source()
{
 return CFR54::source(CFRLists[AASType].element(0));
}

Boolean MFeat::isOrderable(Boolean fMRDB)
{
 return CFR54::isOrderable(CFRLists[fMRDB].element(0));
}

Boolean MFeat::isBackLog()
{
 return CFR54::isBackLog(CFRLists[AASType].element(0));
}

    /*-----------------------------------------------------------------------*\
    |    Boolean description(IString & str,Boolean fMRDB)                     |
    |       Set the given string to the description of the                    |
    |       desired 54_Record (AAS or MRDB) using the other-one               |
    |       if the requested-one is unavailable.                              |
    \*-----------------------------------------------------------------------*/
Boolean MFeat::description(IString & str,Boolean fMRDB)
{
 CFRecV * pRec = CFRLists[fMRDB].element(0);
 if (!pRec)
   pRec = CFRLists[fMRDB? 0 : 1].element(0);
 if (pRec)
    switch (pRec->type()) {
      case 54: return CFR54::description(pRec,str);
      case 96: return CFR96::description(pRec,str);
      }
 return false;
}

Boolean  MFeat::hasDescription(Boolean fMRDB)
{
 CFRecV * pRec = CFRLists[fMRDB].element(0);
 IString str;
 if (pRec)
    switch (pRec->type()) {
      case 54: CFR54::description(pRec,str); break;
      case 96: CFR96::description(pRec,str); break;
      }
 return (str.length() != 0);
}

    /*-----------------------------------------------------------------------*\
    |    const char * description(Boolean fMRDB)                              |
    |       Return the pointer to the description of the                      |
    |       desired 54_Record (AAS or MRDB) using the other-one               |
    |       if the requested-one is unavailable.                              |
    \*-----------------------------------------------------------------------*/
const char * MFeat::description(Boolean fMRDB)
{
 CFRecV * pRec1 = CFRLists[fMRDB].element(0),
        * pRec2 = CFRLists[fMRDB? 0 : 1].element(0);
 const char *pc = "";
 int type = (pRec1? pRec1->type() : (pRec2? pRec2->type() : 0));
 switch (type) {
      case 54:
        pc = CFR54::description(pRec1);
        if (!strlen(pc))
          pc = CFR54::description(pRec2);
        break;
      case 96:
        pc = CFR96::description(pRec1);
        if (!strlen(pc))
          pc = CFR96::description(pRec2);
        break;
      }
 return pc;
}

Boolean MFeat::setDescription(const char * Desc,Boolean fMRDB)
{
 CFRecV * pRec = CFRLists[fMRDB].element(0);
 if (pRec)
   switch(pRec->type()) {
     case 54: return CFR54::setDescription(pRec,Desc);
     case 96: return CFR96::setDescription(pRec,Desc);
     }
 return false;
}

Boolean MFeat::removeRecords(unsigned N, CFRType type)
{
 CFRecV * pRec;
 int i;
 Boolean fOK = (N <= Qty[type]);
 if (fOK)
   if (type == OtherType) {                      // comment records
     for (i = 0 ; i < N ; i++)
       if (pRec = (CFRecV *)CFRLists[type].Pop())
         delete pRec;
     Qty[type] -= N;
     }
   else {
     Qty[type] -= N;
     if (!Qty[type])
       if (pRec = (CFRecV *)CFRLists[type].Pop())
         delete pRec;
     if (type == MRDBType)
       Base = (Base > N)?  Base - N : 0;
     }
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean re_MoveCFRecs(MFeat * pOther, unsigned N, CFRType type)       |
    |       Move the given number of records in the Record List               |
    |       of the given type to that of the given feature, or                |
    |       discard them if no feature is given.                              |
    |       Maintain the quantity counts and reset the base if it             |
    |       concerns the MRDB Record List.                                    |
    \*-----------------------------------------------------------------------*/
// new re_move version for v31
Boolean MFeat::copyRecords(MFeat * pOther, unsigned N,
                           CFRType toType, CFRType fromType,
                           int toVersion,Boolean fMove,Boolean fKeepCount,
                           const char * szMachineType,
                           const char * szMachineModel,
                           const char * szSerialNo)
{
 CFRecV * pRec;
 int i;
 if ((this == pOther) && (toType == fromType))
   return false;
 Boolean fOK = (N <= Qty[fromType]);
 if (fOK)
   if (fromType == OtherType) {                  // comment records
     if (pOther)
       for (i = 0 ; i < N ; i++)
                                                 // keep others by others
         if (pRec = CFRLists[fromType].element(i))
           pOther->addCFR(fromType,pRec);        // creates a 'newed'-one
     if (fMove) {
       for (i = 0 ; i < N ; i++)
         delete (CFRecV *)CFRLists[fromType].Pop();
       Qty[fromType] -= N;
       }
     }
   else {
     if (pOther)
       if (pRec = CFRLists[fromType].element(0)) // only take the first-one
         switch(pRec->type()) {
           case 54:
             switch(toVersion) {
               case 20:
               case 30:
                 for (i = 0 ; i < N ; i++)
                                                 // only 'news' first-one
                   pOther->addCFR(toType,pRec,fKeepCount);
                 break;
               case 31:
                 for (i = 0 ; i < N ; i++)
                   pOther->addCFR(toType,
                           CFR96((Record_54 *)(pRec->record())).record(),
                           fKeepCount);
                 break;
               }
             break;
           case 96:
             switch(toVersion) {
               case 20:
               case 30:
                 for (i = 0 ; i < N ; i++)
                   pOther->addCFR(toType,
                        CFR54((Record_96 *)(pRec->record()),
                              szMachineType,szMachineModel,szSerialNo).record(),
                        fKeepCount);
                 break;
               case 31:
                 for (i = 0 ; i < N ; i++)
                                                 // only 'news' first-one
                   pOther->addCFR(toType,pRec,fKeepCount);
                 break;
               }
           }                                     // end switch recordtype
     if (fMove) {
       Qty[fromType] -= N;
       if (!Qty[fromType])
         delete (CFRecV *)CFRLists[fromType].Pop();
       if (fromType == MRDBType)
         Base = (Base > N)?  Base - N : 0;
       }
     }                                           // non-othertype
 return fOK;
}

 /*--------------------------------------------------------------------------*\
 |                         Non-Compiled testing code                          |
 \*--------------------------------------------------------------------------*/
#ifdef UNITTEST

//#include "euoutil.cpp"
#include <stdlib.h>
#include <iostream.h>

void main(void)
{
 if (1)
   {
     CFRecV * pR1 = new CFRecV("11111111111111111111111111111");
     CFRecV * pR2 = new CFRecV("222222222222222222222222222222222222");
     CFRecV * pR3 = new CFRecV("3333333333333333333333333333333333333333");
     CFRecV * pR4 = new CFRecV("44444444444444444444444444444444");
     CFRecV * pR5 = new CFRecV("55555555555555555555555555555");
     CFRecV * pR6 = new CFRecV("6666666666666666666666666666666666");
     CFRecV * pR7 = new CFRecV("7777777777777777777777777777777777777");
     CFRecV * pR8 = new CFRecV("888888888888888888888888888");
     CFRecV * pR9 = new CFRecV("999999999999999999999999999999999999999999");
     MFeat MF1("F12345");
     MFeat MF2("F23456");
     MF1.addCFR(MFeat::AASType,pR3);
     MF1.addCFR(MFeat::MRDBType,pR4);
     MF1.addCFR(MFeat::AASType,pR5);
     MF2.addCFR(MFeat::AASType,pR6);
     MF2.addCFR(MFeat::MRDBType,pR7);
     MF2.addCFR(MFeat::MRDBType,pR8);
     MF2.addCFR(MFeat::AASType,pR9);
     MFeat MF3;

     cout << "MF1 :" << endl << MF1 << endl;
     int i = MF1.setFeature("FDelta");
     cout << endl << "MF1.setFeature('FDelta') rc :" << i << endl;
     cout << "MF1 :" << endl << MF1 << endl;

     i = MF1.setDescription("This is a long description");
     cout << endl << "MF1.setDescription('This is a long description') rc :" << i << endl;
     cout << "MF1 :" << endl << MF1 << endl;

    MF3 = MF1;
    MFeat MF4(MF2);
    cout << "MF1 :" << endl << MF1 << endl;
    cout << "MF2 :" << endl << MF2 << endl;
    cout << "MF3 :" << endl << MF3 << endl;
    cout << "MF4 :" << endl << MF4 << endl;

    cout << endl;
    cout << " MF1 < MF2 : "
         << (int)(MF1 < MF2)
         << endl;
    cout << " MF1 == MF2 : "
         << (int)(MF1 == MF2)
         << endl;
    cout << " MF1 > MF2 : "
         << (int)(MF1 > MF2)
         << endl;
    cout << " MF1 == MF3 : "
         << (int)(MF1 == MF3)
         << endl;

    IDate date;
    cout << "CFR54::cfrDateYMD('031594') : " << CFR54::cfrDateYMD("031594") << endl;
    cout << "CFR54::cfrDateYMD('022994') : "<< CFR54::cfrDateYMD("022994") << endl;
    if (CFR54::cfrDate("031594",date))
      cout << "CFR54::cfrDate('031594',IDate &) : " << date << endl;
    char Buff[100];
    Buff[6] = '\0';
    if (CFR54::setCFRValiDate(Buff,3,16,95))
       cout << "CFR54::setCFRValiDate(Buff,3,16,95) : " << Buff << endl;
    if (CFR54::setCFRValiDate(Buff,20250315))
      cout << "CFR54::setCFRValiDate(Buff,20250315) : " << Buff << endl;
    if (CFR54::setCFRValiDate(Buff,IDate(3,IDate::Month(3),93)))
      cout << "CFR54::setCFR4ValiDate(Buff,IDate(3,M3,93) : " << Buff << endl;

    CFRecVL List;
    MF1.pickUp(List);
    MF2.pickUp(List);
    cout << "pickup List :" << endl << List << endl;

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif
    delete pR1;
    delete pR2;
    delete pR3;
    delete pR4;
    delete pR5;
    delete pR6;
    delete pR7;
    delete pR8;
    delete pR9;
   }

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif

}

#endif


#if 0
Boolean MFeat::isGenericCFR(char * pRec)
{
 return ( ((Record_54 *)pRec)->InstType == 'U');
}
Boolean MFeat::checkGenerics(GenCodeList & GCL,ofstream & ErrFile,
                             char * CFRFile ,Boolean fDelete)
{
 Boolean fOK = true;
 for (int i = 0, N = CFRLists[MRDBType].NumberElements(); i < N ; i++)
   if (isGenericCFR(CFRLists[MRDBType].element(i)->data()))
     if (!GCL.containsGeneric(Feat))
        { fOK = false;
          ErrFile << CFRFile << " : Record has unknown Generic Code '"
                  << Feat << "'" << (fDelete? " and is ignored :" : " :") << endl
                  << *(CFRLists[MRDBType].element(i));
          if (fDelete)
            { delete CFRLists[MRDBType].element(i);
              CFRLists[MRDBType].DeleteIndex(i--);
              N--;
            }
        }
     else Tag = Generic;
 return fOK;
}
#endif

