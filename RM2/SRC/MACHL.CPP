 /*--------------------------------------------------------------------------*\
 |    Filename - MACHL.CPP                                                    |
 |      contains source for the following RM/2 class implementations:         |
 |                                                                            |
 |   MachineList - Array of Machine object pointers.                          |
 |                 as defined in MACH.HPP                                     |
 \*--------------------------------------------------------------------------*/

// VBB  09/19/2002 D74157 Add code to allow transfer of 19 records in AAS report through
//                        reconciliation to .BIR file.
//
// HECC 01/15/2004 F86750 Added a new case to write the RT 38 information to the .RET file
// ZOR march-2004 F86752 add the tag Process_IS in the findprocessmachine method


        /*---------------------------------------------------*\
        |                      Includes                       |
        \*---------------------------------------------------*/
#include <isynonym.hpp>                          // Definition of Boolean etc

        /*---------------------------------------------------*\
        |               RM/2-specific includes                |
        \*---------------------------------------------------*/
#include "machl.hpp"

        /*---------------------------------------------------*\
        |                       Defines                       |
        \*---------------------------------------------------*/
#define MACHL_SIGNATURE  81048

    /*-----------------------------------------------------------------------*\
    |       'C' Machine object compare function called by                     |
    |       class' 'Find'and sorting methods.                                 |
    \*-----------------------------------------------------------------------*/
int Machine_comp(const void *p1,const void * p2)
{
// return ((Machine *)p1)->compare(*(Machine *)p2);
 return ((Machine *)p1)->compareBySubsystem(*(Machine *)p2);
}

        /*---------------------------------------------------*\
        |                        Ctor                         |
        \*---------------------------------------------------*/
MachineList::MachineList()
//            : PtrRecArr(sizeof(Machine),10,3)
            : PtrRecArr(sizeof(Machine),10,3,Machine_comp)
{}

    /*-----------------------------------------------------------------------*\
    |   Machine * Find()                                                      |
    |       Find the object in the array and return its pointer               |
    \*-----------------------------------------------------------------------*/
// Linear Search
Machine * MachineList::Find( const char * szType
                            ,const char * szModel
                            ,const char * szSerial)
{
                                                 // getting kinda big for the
#if 0                                            // stack
 Machine mach(szType,szModel,szSerial);
 return (Machine *)PtrArray::Find((const void *)&mach);
#endif
 for (int i = 0, N = NumberElements(); i < N ; i++ )
   if(!element(i)->compare(szType,szModel,szSerial))
     return element(i);
 return 0;
}

// Binary Search
Machine * MachineList::FindBS(Machine & mach)
{
 return (Machine *)PtrArray::Find((const void *)&mach);
}
// Linear Search
Machine * MachineList::Find(Machine & mach)
{
 return (Machine *)PtrArray::Find((const void *)&mach,Machine_comp);
}

    /*-----------------------------------------------------------------------*\
    |   Machine * FindProcessMachine()                                        |
    |       Find the Machine object that is marked to be processed            |
    |       in the array and return its pointer.                              |
    |       e.g. bypasses 'Software Machines'                                 |
    |            of which there can be multiples                              |
    \*-----------------------------------------------------------------------*/
Machine * MachineList::FindProcessMachine(Machine & mach)
{
 Machine * pM;
 for (int i = 0, N = NumberElements(); i < N ; i++ )
// if ((pM = element(i)) && !pM->compareTypeSerial(mach))
   if ((pM = element(i)) && !pM->compare(mach))
     if (pM->tag() == Machine::Process)
       return pM;
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |    void setStorageNumbers()                                             |
    |       Give all machines a storage number.                               |
    |       This is necessary before a 'FeatList' is saved to a file.         |
    |       The machine's numbering became necessary upon the introduction    |
    |       of featureless machines.                                          |
    \*-----------------------------------------------------------------------*/
MachineList & MachineList :: SetStorageNumbers()
{
 for (int i = 1, N = NumberElements(); i < N ; i++ )
   element(i)->setNumber(i);
 return *this;
}
//  F86752 ZOR to find a machine that was reconciled by IS
Machine * MachineList::FindProcessMachine( const char * szType
                                          ,const char * szModel
                                          ,const char * szSerial)
{
 Machine * pM;
 for (int i = 0, N = NumberElements(); i < N ; i++ )
   if ((pM = element(i)) && !pM->compare(szType,szModel,szSerial))
     if (pM->tag() == Machine::Process ||(pM->tag() == Machine::Process_IS))     //F86752
       return pM;
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |   Machine * FindProcessTypeSerialMachine()                              |
    |       Find the Machine object by its Type and Serial # only             |
    |       that is marked to be processed                                    |
    |       in the array and return its pointer.                              |
    \*-----------------------------------------------------------------------*/
Machine * MachineList::FindProcessTypeSerialMachine(Machine & mach)
{
 Machine * pM;
 for (int i = 0, N = NumberElements(); i < N ; i++ )
   if ((pM = element(i)) && !pM->compareTypeSerial(mach))
     if (pM->tag() == Machine::Process)
       return pM;
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |   Machine * element(unsigned i)                                         |
    |       Return the i-th ( 0 based ) array element.                        |
    \*-----------------------------------------------------------------------*/
Machine * MachineList::element(unsigned i)
{
 return (Machine *)(PtrArray::Element(i));
}

    /*-----------------------------------------------------------------------*\
    |   void Release(Boolean fRipple)                                         |
    |       call 'delete' for all array elements and optionally               |
    |       call Release() for all machines in the array                      |
    |       i.e. they will then release all obtained storage                  |
    |            that they contain.                                           |
    \*-----------------------------------------------------------------------*/
void MachineList::Release(Boolean fRipple)
{
 for (int i = 0, N = NumberElements(); i < N ;i++ ) {
   if (fRipple)
     element(i)->Release();
   delete element(i);
   }
 EmptyOut();
}

//int MachineList::Write(FILE *f)
//{
// int n = 0 ,Total = 0;
// for (int i = 0, N = NumberElements() ; n >= 0 && i < N ;i++ )
//    Total += ( n = element(i)->Write(f));
// return (n >= 0? Total: -1);
//}

    /*-----------------------------------------------------------------------*\
    |                                                                         |
    |       Have the machines in the array write themselves                   |
    |       in CFReport format to the given ASCII file such that Hardware     |
    |       comes before Software and the latter is written from AAS data     |
    |       and return the number of records written.                         |
    |                                                                         |
    |       Some background on the incoming lists:                            |
    |       Some Machine Features have additional                             |
    |       information associated with them in the form of other             |
    |       Records which are tied to the feature's 54-Record                 |
    |       via their common 'InstanceId' field.                              |
    |       In a CFReport, these ids need to start at 1 and be                |
    |       consecutive, while the records are to be grouped                  |
    |       together by type and appended to the CFReport.                    |
    |       During reconciliation, a feature might disappear                  |
    |       which could violate the Instance Id order.                        |
    |       We handle the Placement (55-Record) and Connection                |
    |       (56-Record) records which are accumulated in lists                |
    |       as the CFReport are read in.                                      |
    |       These 'In' lists are used as a source for the 'Out' lists         |
    |       which will consist of those records that belong to features       |
    |       left over after the reconciliation process, but re-numbered       |
    |       with fresh Instance Ids.                                          |
    |       The 'Out' List are written to the file after all other records.   |
    \*-----------------------------------------------------------------------*/
// new version that copies AAS_Passthru and Software records from the
// AAS side to the MRDB side if needed
int MachineList::fWriteCFR(FILE *f,Boolean fMRDB, unsigned * pInstId,
                           CFRInstL & L55_In, CFRInstL & L55_Out,
                           CFRInstL & L56_In, CFRInstL & L56_Out)
{
 int n = 0 ,Total = 0 ,type;
 Machine * pM;
 for (int pass = 1; pass < 3; pass++)
   for ( int i = 0, N = NumberElements() ; n >= 0 && i < N ;i++ )
     if (pM = element(i))
       switch(pass) {
         case 1:
           type = pM->machineRec(fMRDB).length()? pM->machineRec(fMRDB).type() :
                       (pM->machineRec(!fMRDB).length()?
                              pM->machineRec(!fMRDB).type() : 0);
           switch(type) {
              case  8:
                if (fMRDB && (pM->tag() == Machine::AAS_Passthru) &&
                    !pM->hasMRDBData())
                  pM->copyRecords(pM,true,false,30,false,false);
              case 25:
                Total += ( n = pM->fWriteCFR(f,fMRDB,pInstId,
                                             L55_In,L55_Out,L56_In,L56_Out));
              }
           break;
         case 2:
           if (pM->hasAASData())
                                                 // software from AAS only
             switch(pM->machineRec(false).type()) {
                case 17:
                case 47:
                case 49:
                  if (fMRDB && !pM->hasMRDBData())
                    pM->copyRecords(pM,true,false,30,false,false);
                  Total += ( n = pM->fWriteCFR(f,fMRDB,pInstId,
                                               L55_In,L55_Out,L56_In,L56_Out));
                }
           break;
         }
 return (n >= 0? Total : -1);
}

int MachineList::fWriteSubSystemBottom(FILE *f,int SubSysRecId,
                                       CFR52List & L52,
                                       CFRInstL & L55,
                                       CFRInstL & L56,
                                       CFR10List & SubSysRecList,
                                       CFR05List & CommentRecList )
{
 int Total = 0, i;
 if (i = L55.NumberElements())
   Total = L55.fWriteA(f)? i : -1;
 if ((Total >= 0) && (i = L56.NumberElements()))
   Total = L56.fWriteA(f)? Total + i : -1;
 if (Total >= 0) {
   Total += L52.fWriteCFR(f,SubSysRecId);
   if (i = SubSysRecList.referenceNotesNumber(SubSysRecId))
     Total += CommentRecList.fWriteCFR(f,CFR05::BottomOfSubSystem,i);
   }
 L55.Release();
 L56.Release();
 return Total;
}

// new version that copies AAS_Passthru and Software records from the
// AAS side to the MRDB side if needed
int MachineList::fWriteCFRV31(FILE *f,Boolean fMRDB, unsigned * pNextInstId,
                              CFR52List & L52_In,
                              CFRInstL & L55_In,
                              CFRInstL & L56_In,
                              CFR10List & SubSysRecList,
                              CFR05List & CommentRecList,
                              IString strSysId )             //edw06/04/1998
{
 int n = 0 ,Total = 0 ,type;
 Machine * pM;
 int CurrentSubSysRecId = 0
    ,TopSubSysRecId     = 0
    ,MiddleSubSysRecId  = 0
    ,BottomSubSysRecId  = -1
    ,NewSubSysRecId     = 0;
 Boolean fWriteSubSystemRec = false;
 unsigned RefNotesNumber;
                                                 // unsorted Subsystem
 CFRInstL L55_Out(false)                         // Placements
                                                 // unsorted Subsystem
         ,L56_Out(false);                        // Connections
 for ( int i = 0, N = NumberElements() ; n >= 0 && i < N ;i++ )
   if ( ( pM = element(i) ) && ( pM->getSystemId() == strSysId) ) {
     NewSubSysRecId = pM->subsystemRecordId(fMRDB);
     if (NewSubSysRecId != CurrentSubSysRecId) { // new subsystem
       BottomSubSysRecId = CurrentSubSysRecId ;
       TopSubSysRecId = MiddleSubSysRecId = CurrentSubSysRecId = NewSubSysRecId;
       fWriteSubSystemRec = true;
       }
     if ( BottomSubSysRecId >= 0 ) {             // for previous subsystem
       Total+=fWriteSubSystemBottom(f,BottomSubSysRecId,L52_In,L55_Out,L56_Out,
                                    SubSysRecList,CommentRecList);
       BottomSubSysRecId = -1;                   // turn off flag
       }
     if ( CurrentSubSysRecId ) {                 // write subsystem record
       if (fWriteSubSystemRec) {
         Total += SubSysRecList.fWriteCFR(f,CurrentSubSysRecId);
         fWriteSubSystemRec = false;             // once per subsystem
         }
       RefNotesNumber = SubSysRecList.referenceNotesNumber(CurrentSubSysRecId);
       }
     type = pM->machineRec(fMRDB).length()? pM->machineRec(fMRDB).type() :
                       (pM->machineRec(!fMRDB).length()?
                              pM->machineRec(!fMRDB).type() : 0);
     switch(type) {
       case  8:
         if (fMRDB && (pM->tag() == Machine::AAS_Passthru) &&
             !pM->hasMRDBData())
           pM->copyRecords(pM,true,false,31,false,false);
       case 25:                                  // hardware
       case 19:                                  // backlog          //VBB09192002 D74157
       case 50:                                                      //VBB09192002 D74157
       case 38:   //* F86750 Case added to write also the services RT38 to the .RET file - HECC03152004
         Total += ( n = pM->fWriteCFRV31(f,fMRDB,pNextInstId, L55_In,L55_Out,
                                         L56_In,L56_Out,CommentRecList));
         break;

       case 17:
       case 47:
       case 49:
         if (fMRDB && !pM->hasMRDBData())
           pM->copyRecords(pM,true,false,31,false,false);
         if ( MiddleSubSysRecId ) {              // for previous subsystem
           Total += CommentRecList.fWriteCFR(f,CFR05::AfterHardware,
                                             RefNotesNumber);
           MiddleSubSysRecId = 0;                // turn off flag
           }
         if (pM->hasAASData())
           Total += ( n = pM->fWriteCFRV31(f,fMRDB,pNextInstId, L55_In,L55_Out,
                                           L56_In,L56_Out,CommentRecList));
       }
     }
 if (n > 0)                                      // do last subsystem
   Total+=fWriteSubSystemBottom(f,CurrentSubSysRecId,L52_In,L55_Out,L56_Out,
                                SubSysRecList,CommentRecList);

 return (n >= 0? Total : -1);
}

    /*-----------------------------------------------------------------------*\
    |    unsigned fWriteBLength()                                             |
    |       Return the number of bytes the List would                         |
    |       occupy if it was to be written to a binary file                   |
    |       by method  'fWriteB'.                                             |
    \*-----------------------------------------------------------------------*/
unsigned MachineList::fWriteBLength()
{
 unsigned Total = 2 * sizeof(unsigned);
 for (int n = 0, N = NumberElements() ; n < N ; n++ )
   Total +=  element(n)->fWriteBLength();
 return Total;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean fReadB(FILE *f)                                               |
    |       add Machines stored at the current                                |
    |       position of the given file to the array.                          |
    |       Note: Only to be used for a file segment made by 'fWriteB'.       |
    |             The two ints containing a signature and                     |
    |             the number of Machines contained in that file segment       |
    |             are read first.                                             |
    \*-----------------------------------------------------------------------*/
Boolean MachineList::fReadB(FILE *f)
{
 unsigned U2[2];
 Machine * pM;
 Boolean fOK = false;
 if ( _filelength(_fileno(f) >= (2 * sizeof(unsigned))))
   if (fread(U2,sizeof(U2),1,f) == 1)
     if ( MACHL_SIGNATURE == U2[0])
       for (int i = 0, fOK = true ; fOK && (i < U2[1]); i++)
         if (fOK = ((pM = new Machine()) != 0))
           if (fOK = pM->fReadB(f))
             Add(pM);
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean MachineList::fReadB(FILE *f)                                  |
    |       add Machines contained in the given                               |
    |       filename to the array.                                            |
    |       Note: Only to be used for a file made by 'fWriteB'.               |
    \*-----------------------------------------------------------------------*/
Boolean MachineList::fReadB(char * szFileName)
{
 FILE * f;
 Boolean fOK = ((f = fopen(szFileName,"rb")) != 0);
 if (fOK) {
   fOK = fReadB(f);
   fclose(f);
   }
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean fWriteB(FILE *f, unsigned * pBytesWrit)                      |
    |       Have the machines in the array write themselves                   |
    |       out to the current position of the given binary file              |
    |       and report the number of bytes written.                           |
    \*-----------------------------------------------------------------------*/
Boolean MachineList::fWriteB(FILE *f, unsigned * pBytesWrit)
{
 unsigned U2[2];
 unsigned len, Total = 0;
 Boolean fOK;
 U2[0] = MACHL_SIGNATURE;
 U2[1] = NumberElements();
 if (fOK = (fwrite(U2,sizeof(U2),1,f) == 1)) {
   Total = sizeof(U2);
   for (int i = 0; fOK && (i < U2[1]); i++)
     if (fOK = element(i)->fWriteB(f,&len))
       Total += len;
   }
 if (pBytesWrit)
   *pBytesWrit = Total;
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean fWriteB(char * szFileName, unsigned * pBytesWrit)            |
    |       Write the machines in the array out to the given filename         |
    |       and report the filesize                                           |
    \*-----------------------------------------------------------------------*/
Boolean MachineList::fWriteB(char * szFileName, unsigned * pBytesWrit)
{
 FILE * f;
 Boolean fOK = ((f = fopen(szFileName,"wb")) != 0);
 if (fOK) {
   fOK = fWriteB(f,pBytesWrit);
   fclose(f);
   }
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |   int fReadB_rc(FILE *f)                                                |
    |       add Machines stored at the current                                |
    |       position of a given file to the array and return the              |
    |       result as per the codes defined in MACHL.HPP.                     |
    |       Note: Only to be used for a file made by 'fWriteB'.               |
    |             The two ints containing a signature and                     |
    |             the number of Machines contained in the file segment        |
    |             are read first.                                             |
    \*-----------------------------------------------------------------------*/
int MachineList::fReadB_rc(FILE *f)
{
 unsigned U2[2];
 Machine * pM;
 int rc = PARR_OK;
 if (fread(U2,sizeof(U2),1,f) == 1)
   if ( MACHL_SIGNATURE == U2[0])
     for (int i = 0; !rc && (i < U2[1]); i++)
       if ( (pM = new Machine()) && pM->fReadB(f))
         Add(pM);
       else rc = MACHL_ERR_READ_FILE;
   else rc = MACHL_ERR_NO_MACHL_FILE;
 else rc = MACHL_ERR_READ_FILE;
 return rc;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean MachineList::fReadB(FILE *f)                                  |
    |       add Machines contained in the given                               |
    |       filename to the array and return                                  |
    |       result as per the codes defined in MACHL.HPP.                     |
    |       Note: Only to be used for a file made by 'fWriteB'.               |
    \*-----------------------------------------------------------------------*/
int MachineList::fReadB_rc(char * szFileName)
{
 FILE * f;
 int rc;
 if (f = fopen(szFileName,"rb")) {
   if ( _filelength(_fileno(f) >= (2 * sizeof(unsigned))))
     rc = fReadB_rc(f);
   else rc = MACHL_ERR_NO_MACHL_FILE;
   fclose(f);
   }
 else rc = MACHL_ERR_NO_FILE;
 return rc;
}

 /*--------------------------------------------------------------------------*\
 |                         Non-Compiled testing code                          |
 \*--------------------------------------------------------------------------*/
#ifdef UNITTEST

#include <iostream.h>

int  MOut(void * p)
{
 Machine * pM = (Machine *)p;
 cout << pM->type()
      << "  "
      << pM->model()
      << "  "
      << pM->serialNo()
      << endl;
 return 0;
}

void MachineOut(int n,Machine &m)
{
 MFeat * pF;
 CFRec * pR;
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
         << pF->qtyCFR(MFeat::AASType)
         << "  MRDB qty  "
         << pF->qtyCFR(MFeat::MRDBType)
         << endl;

    cout << "AAS List only: " << endl;

   CFRecList & CFRL = pF->cfrList(MFeat::AASType);
   l = CFRL.NumberElements();
   for (k = 0; k < l ; k++)
     {
      pR = (CFRec *)CFRL[k];
      cout << "CFRec : "
           << pR->number()
           << "  Data : "
           << pR->data()
           << endl;
     }
   }
}


void TestOut(MachineList & ML)
{
 int i,
     j = ML.NumberElements();
 Machine * pM;
 for (i = 0; i < j ; i++)
   {
     pM = (Machine *)ML[i];
     MachineOut(i,*pM);
   }
}

void main(void)
{

///////////////////////////////////////////

 if (1)
   {
    Machine    M1("mac1","mod1","12345789"),
               M2("mac2","mod2","22345789"),
               M3("mac3","mod3","32345789"),
               M4("mac4","mod4","42345789"),
               M5("mac5","mod5","52345789"),
            *  pAMachine;
    MachineList ML;
    ML.Add(&M5);
    ML.Add(&M3);
    ML.Add(&M1);
    ML.Add(&M2);
    ML.Add(&M4);

    CFRec * pRec11 = new CFRec("1111111111111111111");
    CFRec * pRec12 = new CFRec("1222222222222222222");

    MFeat * pF11 = new MFeat("F11111");
    MFeat * pF21 = new MFeat("F21111");
    MFeat * pAFeature;

    if (pAMachine = ML.Find("mac1","mod1","12345789"))
      {
        MFeatList & MFL = pAMachine->mfeatList();
        pAFeature = MFL.Find("F31111");
        if (!pAFeature)
                                                 // if (!(pAFeature =
                                                 // pAMachine->mfeatList().Find-
                                                 // ((char *)ThisFeature)))
                                                 // { MFeat mf(ThisFeature);
          { pAFeature = new MFeat("F31111");
                                                 // mf.cfrList().Add(pCFRec);
                                                 // mf.setQtyAAS(mf.qtyAAS()+1)-
                                                 // ;
                                                 // if (fMRDB)
                                                 // mf.setQtyMRDB(mf.qtyMRDB()+-
                                                 // 1);
                                                 // pAMachine->mfeatList().Add(-
                                                 // &mf);
                                                 // MFL.Add(&mf);
            pAFeature->addCFR(MFeat::AASType,pRec11);
            MFL.Add(pAFeature);
          }
      }

   TestOut(ML);

    cout << endl;
    cout << ML;

    delete pRec11;
    delete pRec12;
    delete pF11;
    delete pF21;
    delete pAFeature;

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif
   }
#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif
/////////////////////////////////////////////

}

#endif








///////////////////     Tests     //////////////////////////////////////////
#if 0

///////////////////////////////////
 if (1)
   {
    Machine    M1("mach1","mod1"),
               M2("mach2","mod2"),
               M3("mach3","mod3"),
               M4("mach4","mod4"),
               M5("mach5","mod5"),
               *pM2,
               *pM4,
               *pM;
    MachineListM ML;
    ML.Add(&M5);
    ML.Add(&M3);
    ML.Add(&M1);
    ML.Add(&M2);
    ML.Add(&M4);
    ML.CWalk(MOut);
    cout << endl;
    pM2 = new Machine("typ2","mod2","234567890");
    pM4 = new Machine("typ4","mod4","456789012");
    ML.Add(pM4);
    ML.Add(pM2);
    ML.CWalk(MOut);
    cout << endl;
    if (pM = ML.Find("typ2","mod2","234567890"))
       MOut(pM);
    if (pM = ML.Find(*pM4))
       MOut(pM);
    delete pM2;
    delete pM4;
#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif
   }
#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif
///////////////////////////////////////////

 if (1)
   {
    Machine    M1("mac1","mod1"),
               M2("mac2","mod2"),
               M3("mac3","mod3"),
               M4("mac4","mod4"),
               M5("mac5","mod5");
    CFRec * pRec11 = new CFRec("1111111111111111111");
    CFRec * pRec12 = new CFRec("1222222222222222222");
    CFRec * pRec21 = new CFRec("2111111111111111111");
    CFRec * pRec22 = new CFRec("2222222222222222222");
    CFRec * pRec31 = new CFRec("3111111111111111111");
    CFRec * pRec32 = new CFRec("3222222222222222222");
    CFRec * pRec41 = new CFRec("4111111111111111111");
    CFRec * pRec42 = new CFRec("4222222222222222222");
    CFRec * pRec51 = new CFRec("5111111111111111111");
    CFRec * pRec52 = new CFRec("5222222222222222222");
    MFeat * pF11 = new MFeat("F11111",1,1);
    MFeat * pF21 = new MFeat("F21111",1,1);
    MFeat * pF31 = new MFeat("F31111",1,1);
    MFeat * pF41 = new MFeat("F41111",1,1);
    MFeat * pF51 = new MFeat("F51111",1,1);
    MFeat * pF12 = new MFeat("F12222",1,1);
    pF11->cfrList().Add(pRec11);
    pF11->cfrList().Add(pRec12);
    pF21->cfrList().Add(pRec21);
    pF21->cfrList().Add(pRec22);
    pF31->cfrList().Add(pRec31);
    pF31->cfrList().Add(pRec32);
    pF41->cfrList().Add(pRec41);
    pF41->cfrList().Add(pRec42);
    pF51->cfrList().Add(pRec51);
    pF51->cfrList().Add(pRec52);

    M1.mfeatList().Add(pF11);
    M1.mfeatList().Add(pF12);
    M2.mfeatList().Add(pF21);
    M3.mfeatList().Add(pF31);
    M4.mfeatList().Add(pF41);
    M5.mfeatList().Add(pF51);

    MachineListM ML;
    ML.Add(&M5);
    ML.Add(&M3);
    ML.Add(&M1);
    ML.Add(&M2);
    ML.Add(&M4);

    TestOut(ML);

    delete pRec11;
    delete pRec12;
    delete pRec21;
    delete pRec22;
    delete pRec31;
    delete pRec32;
    delete pRec41;
    delete pRec42;
    delete pRec51;
    delete pRec52;
    delete pF11;
    delete pF21;
    delete pF31;
    delete pF41;
    delete pF51;
    delete pF12;

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif
   }
#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif
/////////////////////////////////////////////
#endif



