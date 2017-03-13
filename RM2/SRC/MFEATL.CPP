 /*--------------------------------------------------------------------------*\
 |    Filename - MFEATL.CPP                                                   |
 |      contains source for the following RM/2 class implementations:         |
 |                                                                            |
 |   MFeatList - Array of MFeat object pointers.                              |
 |                 as defined in MFEAT.HPP                                    |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                      Includes                       |
        \*---------------------------------------------------*/
#include <isynonym.hpp>                          // Definition of Boolean etc
#include <string.h>

        /*---------------------------------------------------*\
        |               RM/2-specific includes                |
        \*---------------------------------------------------*/
#include "mfeatl.hpp"

    /*-----------------------------------------------------------------------*\
    |       'C' Machine object compare function called by                     |
    |       class' 'Find'and sorting methods.                                 |
    \*-----------------------------------------------------------------------*/
int MFeat_comp(const void *p1,const void * p2)
{
 return ((const MFeat *)p1)->compare(*(const MFeat *)p2);
}

        /*---------------------------------------------------*\
        |                        Ctor                         |
        \*---------------------------------------------------*/
MFeatList::MFeatList()
         : PtrRecArr(sizeof(MFeat),10,3,MFeat_comp)
{}

    /*-----------------------------------------------------------------------*\
    |   void re_Sort()                                                        |
    |       Note: Normally, when adding, the array will remain sorted.        |
    |             However, when the array element is altered, the order       |
    |             may no longer be valid.                                     |
    |             (This can occur during the reconciliation of generic        |
    |              features when only the name changes, in which case the     |
    |              object is retained.                                        |
    |            StopOrdering() removes the compare function stored with      |
    |                           the array.                                    |
    |            QSort()        sorts and re-stores the compare function.     |
    \*-----------------------------------------------------------------------*/
void MFeatList::re_Sort()
{
 StopOrdering();
 QSort(MFeat_comp);
}

// Binary search using compare function
MFeat * MFeatList::Find(const char *szFeature)
{
 MFeat mf(szFeature);
 return Find(mf);
}

    /*-----------------------------------------------------------------------*\
    |   MFeat * FindOEMFeature(const char *szFeature)                         |
    |       Return the array element (pointer to an MFeat object)             |
    |       for the given feature name that is marked as an OEM.              |
    |       Note: Linear search since, during resolution, there can           |
    |             be two generic features with the same name:                 |
    |             one from the MRDB file and the other tagged as OEM          |
    |             by the user, if the latter was a part of the former.        |
    |             i.e. If there's a generic feature 'A' with a quantity       |
    |                  of 5, the user can designate 3 of them as OEM.         |
    |                  Before the user resolves the 2 remaining 'A''s,        |
    |                  we'll be in the aforementioned state,                  |
    |                  especially when that partial resolution is saved.      |
    \*-----------------------------------------------------------------------*/
MFeat * MFeatList::FindOEMFeature(const char *szFeature)
{
 MFeat mf(szFeature), *pF;
 for (int i = 0, N = NumberElements() ; i < N ; i++ )
   if ((pF = element(i)) && (mf == *pF) && pF->isMarkedAsOEM())
     return pF;
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |   MFeat * FindUnmarkedFeature(const char *szFeature)                    |
    |       Return the array element (pointer to an MFeat object)             |
    |       for the given feature name that is NOT marked.                    |
    \*-----------------------------------------------------------------------*/
MFeat * MFeatList::FindUnmarkedFeature(const char *szFeature)
{
 MFeat mf(szFeature), *pF;
 for (int i = 0, N = NumberElements() ; i < N ; i++ )
  if (pF = element(i))
   if (mf == *pF)
    if (!pF->isMarked())
     return pF;
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |   MFeat * Find(MFeat & mf)                                              |
    |       Return the array element (pointer to an MFeat object)             |
    |       equaling the given object.                                        |
    \*-----------------------------------------------------------------------*/
MFeat * MFeatList::Find(MFeat & mf)
{
 return (MFeat *)PtrRecArr::Find((const void *)&mf);
}

    /*-----------------------------------------------------------------------*\
    |   MFeat * FindRecord(unsigned uRecNumber)                               |
    |       Return the array element (pointer to an MFeat object)             |
    |       which has the given record number.                                |
    \*-----------------------------------------------------------------------*/
MFeat * MFeatList::FindRecord(unsigned uRecNumber)
{
 for (int i = 0, N = NumberElements() ; i < N ; i++ )
   if (element(i)->number() == uRecNumber)
     return element(i);
 return 0;
}

    /*-----------------------------------------------------------------------*\
    |   MFeat * element(unsigned i)                                           |
    |       Return the i-th ( 0 based ) array element.                        |
    \*-----------------------------------------------------------------------*/
MFeat * MFeatList::element(unsigned i)
{
 return (MFeat *)(PtrArray::Element(i));
}

    /*-----------------------------------------------------------------------*\
    |   MFeatList & remove(MFeat *pF)                                         |
    |       remove the given array element and                                |
    |       release all storage obtained for it.                              |
    \*-----------------------------------------------------------------------*/
MFeatList & MFeatList::remove(MFeat *pF)
{
 pF->Release();
 DeletePtr(pF);
 delete pF;
 return *this;
}

    /*-----------------------------------------------------------------------*\
    |   void Release(Boolean fRipple)                                         |
    |       call 'delete' for all array elements and optionally               |
    |       call Release() for all objects in the array                       |
    |       i.e. they will then release all obtained storage                  |
    |            that they contain.                                           |
    \*-----------------------------------------------------------------------*/
void MFeatList::Release(Boolean fRipple)
{
 for (int i = 0, N = NumberElements(); i < N ;i++ ) {
   if (fRipple)
     element(i)->Release();
   delete element(i);
   }
 EmptyOut();
}

int MFeatList::Write(FILE *f)
{
 int n = 0 ,Total = 0;
 for (int i = 0, N = NumberElements() ; n >= 0 && i < N ;i++ )
    Total += ( n = element(i)->Write(f));
 return (n >= 0? Total: -1);
}

int MFeatList::Write(FILE *f,MFeat::CFRType type)
{
 int n = 0 ,Total = 0;
 for (int i = 0, N = NumberElements() ; n >= 0 && i < N ;i++ )
    Total += ( n = element(i)->Write(f,type));
 return (n >= 0? Total: -1);
}

    /*-----------------------------------------------------------------------*\
    |   Boolean MachineList::fReadB(FILE *f)                                  |
    |       Add the given number of MFeat's stored at the current             |
    |       position of the given file to the array.                          |
    |       i.e. create the objects and add their addresses                   |
    \*-----------------------------------------------------------------------*/
int MFeatList::fReadB(FILE *f , unsigned count)
{
 MFeat *pF;
 int rc = PARR_OK;
 void * pv;
 if (pv = malloc(LRecl())) {
   for (int i = 0; rc == PARR_OK && i < count; i++)
     if (fread(pv,LRecl(),1,f) != 1)
       rc = PARR_READ;
     else if ( pF = new MFeat()) {
            pF->BaseInit(*(MFeat *)pv);
            Add(pF);
            }
          else rc = PARR_NO_MEM;
   free(pv);
   }
 else rc = PARR_NO_MEM;
 return rc;
}

    /*-----------------------------------------------------------------------*\
    |    int LoadFile(char *filename)                                         |
    |       Open and read the given binary file ( only                        |
    |       if its size is a multiple of the MFeat object's length) and       |
    |       add the objects via their pointers                                |
    |       to the array.                                                     |
    \*-----------------------------------------------------------------------*/
int MFeatList::fReadB(char *filename)
{
 FILE *f;
 if (!(f = fopen(filename,"rb")))
   return PARR_NO_FILE;
 int i = _filelength(_fileno(f));
 int rc = !(i%LRecl())?  fReadB(f, i/LRecl()) : PARR_LRECL;
 fclose(f);
 return rc;
}

 /*--------------------------------------------------------------------------*\
 |                         Non-Compiled testing code                          |
 \*--------------------------------------------------------------------------*/
#ifdef UNITTEST

#include <iostream.h>

int  MFOut(void * p)
{
 MFeat * pmf = (MFeat *)p;
 cout << *pmf << endl;
 return 0;
}

void main(void)
{
 MFeat *pMF;
 if (1)
   {
    CFRec * pR1 = new CFRec("111111111111111");
    CFRec * pR2 = new CFRec("222222222222222");
    CFRec * pR3 = new CFRec("333333333333333");
    CFRec * pR4 = new CFRec("444444444444444");
    CFRec * pR5 = new CFRec("555555555555555");
    CFRec * pR6 = new CFRec("666666666666666");
    CFRec * pR7 = new CFRec("777777777777777");
    CFRec * pR8 = new CFRec("888888888888888");
    CFRec * pR9 = new CFRec("999999999999999");
    MFeat MF1("F12345",MFeat::AASType,pR1);
    MFeat MF2("F23456",MFeat::MRDBType,pR2);
    MFeat MF3("F34567");
    MFeat MF4("F45678");
    MFeat MF5("F56789");
    MF1.addCFR(MFeat::AASType,pR3);
    MF1.addCFR(MFeat::MRDBType,pR4);
    MF1.addCFR(MFeat::AASType,pR5);
    MF2.addCFR(MFeat::AASType,pR6);
    MF2.addCFR(MFeat::MRDBType,pR7);
    MF2.addCFR(MFeat::MRDBType,pR8);
    MF2.addCFR(MFeat::AASType,pR9);

    MFeatList MFL;
    MFL.Add(&MF5);
    MFL.Add(&MF3);
    MFL.Add(&MF1);
    MFL.Add(&MF2);
    MFL.Add(&MF4);
    cout << "WalkOut :" << endl;
    MFL.CWalk(MFOut);
    cout << endl << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
    cout << "cout << List :" << endl;
    cout << MFL << endl;
    cout << endl << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
    if (pMF = MFL.Find("F34567"))
       cout << "Found Feature ""F34567"": "<< endl << *pMF;
    cout << endl << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
    if (pMF = MFL.Find(MF1))
      cout << "Found Feature 'MF1': " << endl << *pMF;

#if 0
    MFL.AddFeature("F56789",1);
    MFL.AddFeature("F56789",0);
    MFL.AddFeature("F66789");
    MFL.AddFeature("F66789");
    MFL.AddFeature("F66789",0);
#endif
    cout << endl << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
    if (!MFL.WriteToFile("MFEATL.TST"))
      {
       MFeatList MFL2;
       cout << endl << "Reading list from file :" << endl;
       if (!MFL2.LoadFile("MFEATL.TST"))
          cout << MFL2 << endl;
       MFL2.Release();
      }
    cout << endl;
    cout << endl;

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




