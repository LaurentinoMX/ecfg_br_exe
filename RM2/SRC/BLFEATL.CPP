 /*--------------------------------------------------------------------------*\
 |    Filename - BLFEATL.CPP                                                  |
 |      contains source for the following RM/2 class implementations:         |
 |                                                                            |
 |     BLFeat   - Backlog Feature object, merely containing the pointers      |
 |                to the Backlog Machine and MFeat objects.                   |
 |                Methods to manipulate the underlying CFReport machine       |
 |                and feature objects, do so directly. There is no copy.      |
 |                Intended for EUI for which also can use the ID data         |
 |                (easier when working with containers).                      |
 |                                                                            |
 |     BLFeatList - A collection of 'BLFeat' objects                          |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |              Standard Library Includes              |
        \*---------------------------------------------------*/
#include <io.h>
#include <stdio.h>
#include <limits.h>

        /*---------------------------------------------------*\
        |                   ICLUI Includes                    |
        \*---------------------------------------------------*/
#include <isynonym.hpp>                          // Definition of Boolean etc

#define INCL_IAPPLICATION
#define INCL_IRESOURCELIBRARY
#include "uicl.hpp"

        /*---------------------------------------------------*\
        |               RM/2-specific includes                |
        \*---------------------------------------------------*/
#include "blfeatl.hpp"                           // this file's declarations
#include "cfr19_50.hpp"                          // CFR19 & CFR50
#include "mainw.h"

 /*--------------------------------------------------------------------------*\
 |                                BLFeat Class                                |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |                     Assignment                      |
        \*---------------------------------------------------*/
BLFeat & BLFeat::operator=(BLFeat const& other)
{
 if (&other != this)
   memcpy(this,&other,sizeof(*this));
 return *this;
}

    /*-----------------------------------------------------------------------*\
    |    int compare(BLFeat const& other) const                               |
    |       Compares the Machine and MFeat objects alphabetically.            |
    \*-----------------------------------------------------------------------*/
int BLFeat::compare(BLFeat const& other) const
{
 int i = pM->compare(*(other.pM));
 if (!i && pF)
   i = pF->compare(*(other.pF));
 return i;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean description(IString & str)                                   |
    |       Set the given string to the Feature's AAS description.            |
    \*-----------------------------------------------------------------------*/
Boolean BLFeat :: description(IString & str)
{
 if (pF)
   return pF->description(str,false);
 else str = IString();
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    char machineAction()                                                 |
    |       returns '?' when unavailable                                      |
    \*-----------------------------------------------------------------------*/
char BLFeat::machineAction()
{
 if (pM) {
   CFRecV & rec = pM->machineRec(false);
   if (rec.length())
     switch(rec.type()) {
       case 19: return ((const Record_19 *)(rec.record()))->Action;
       case 50: return ((const Record_50 *)(rec.record()))->Action;
       }
   }
 return '?';
}

    /*-----------------------------------------------------------------------*\
    |       Set the object's (feature) quantity and action (as the            |
    |       corresponding resource string) in the given strings.              |
    \*-----------------------------------------------------------------------*/
void BLFeat::qty_Action(IString & strQty,IString & strAct)
{
 unsigned u;
 char Action;
 strQty = strAct = IString();
 unsigned resid = 0;
 if (pM && pF) {
   CFRecV & rec = pM->machineRec(false);
   if (rec.length())
     switch(rec.type()) {
       case 19:
         if (CFR19::featParts(rec,pF->feature(),0,&u,&Action)) {
           strQty = IString(u);
           switch(Action) {
             case '1': resid = BL_STR_FEAT_HW_1 ; break;
             case '2': resid = BL_STR_FEAT_HW_2 ; break;
             case '3': resid = BL_STR_FEAT_HW_3 ; break;
             case '4': resid = BL_STR_FEAT_HW_4 ; break;
             case '6': resid = BL_STR_FEAT_HW_6 ; break;
             case '7': resid = BL_STR_FEAT_HW_7 ; break;
             case '8': resid = BL_STR_FEAT_HW_8 ; break;
             }
           }
         break;
       case 50:
         if (CFR50::featParts(rec,pF->feature(),0,&u,&Action)) {
           strQty = IString(u);
           switch(Action) {
             case '1': resid = BL_STR_FEAT_SW_1 ; break;
             case '2': resid = BL_STR_FEAT_SW_2 ; break;
             case '3': resid = BL_STR_FEAT_SW_3 ; break;
             case '4': resid = BL_STR_FEAT_SW_4 ; break;
             case '5': resid = BL_STR_FEAT_SW_5 ; break;
             case 'A': resid = BL_STR_FEAT_SW_A ; break;
             case 'B': resid = BL_STR_FEAT_SW_B ; break;
             }
           }
         break;
       }
   if (resid)
     strAct = IApplication::current().userResourceLibrary().loadString(resid);
   }
}

    /*-----------------------------------------------------------------------*\
    |       Set the object's (machine) quantity and action (as the            |
    |       corresponding resource string) in the given strings.              |
    \*-----------------------------------------------------------------------*/
void BLFeat::machineQty_Action(IString & strQty,IString & strAct)
{
 strQty = strAct = IString();
 unsigned resid = 0;
 if (pM) {
   CFRecV & rec = pM->machineRec(false);
   if (rec.length())
     switch(rec.type()) {
       case 19:
         strQty = CFR19::qty(rec.record());
         switch( ((const Record_19 *)(rec.record()))->Action ) {
           case '1': resid = BL_STR_MACH_HW_1 ; break;
           case '2': resid = BL_STR_MACH_HW_2 ; break;
           case '3': resid = BL_STR_MACH_HW_3 ; break;
           case '4': resid = BL_STR_MACH_HW_4 ; break;
           case '5': resid = BL_STR_MACH_HW_5 ; break;
           case '6': resid = BL_STR_MACH_HW_6 ; break;
           case 'A': resid = BL_STR_MACH_HW_A ; break;
           case 'B': resid = BL_STR_MACH_HW_B ; break;
           case 'C': resid = BL_STR_MACH_HW_C ; break;
           }
         break;
       case 50:
         strQty = CFR50::qty(rec.record());
         switch( ((const Record_50 *)(rec.record()))->Action ) {
           case '1': resid = BL_STR_MACH_SW_1 ; break;
           case '2': resid = BL_STR_MACH_SW_2 ; break;
           case '3': resid = BL_STR_MACH_SW_3 ; break;
           case '4': resid = BL_STR_MACH_SW_4 ; break;
           case '5': resid = BL_STR_MACH_SW_5 ; break;
           case 'A': resid = BL_STR_MACH_SW_A ; break;
           case 'B': resid = BL_STR_MACH_SW_B ; break;
           case 'C': resid = BL_STR_MACH_SW_C ; break;
           }
         break;
       }
   if (resid)
     strAct = IApplication::current().userResourceLibrary().loadString(resid);
   }
}

    /*-----------------------------------------------------------------------*\
    |    char action()                                                        |
    |       returns the feature's action ('?' when unavailable)               |
    \*-----------------------------------------------------------------------*/
char BLFeat::action()
{
 unsigned u;
 char Action;
 Boolean fOK = false;
 if (pM && pF) {
   CFRecV & rec = pM->machineRec(false);
   if (rec.length())
     switch(rec.type()) {
       case 19: fOK = CFR19::featParts(rec,pF->feature(),0,&u,&Action); break;
       case 50: fOK = CFR50::featParts(rec,pF->feature(),0,&u,&Action); break;
       }
   }
 return fOK? Action : '?';
}

    /*-----------------------------------------------------------------------*\
    |    int signedQty(void)                                                  |
    |       returns the feature's AAS quantity such that a negative           |
    |       is returned when the feature's action represents a                |
    |       discontinuation.                                                  |
    \*-----------------------------------------------------------------------*/
int BLFeat::signedQty(void)
{
 unsigned u;
 char Action;
 int qty = 0;
 if (pM && pF) {
   CFRecV & rec = pM->machineRec(false);
   if (rec.length())
     switch(rec.type()) {
       case 19:
         if (CFR19::featParts(rec,pF->feature(),0,&u,&Action))
           qty = (Action == '2'? -u : u);
         break;
       case 50:
         if (CFR50::featParts(rec,pF->feature(),0,&u,&Action))
           qty = (Action == '2'? -u : u);
         break;
       }
   }
 return qty;
}

    /*-----------------------------------------------------------------------*\
    |    IString machineQty(void)                                             |
    |       returns the machine's quantity as a IString.                      |
    \*-----------------------------------------------------------------------*/
IString BLFeat::machineQty(void)
{
 if (pM) {
   CFRecV & rec = pM->machineRec(false);
   if (rec.length())
     switch(rec.type()) {
       case 19: return CFR19::qty(rec.record());
       case 50: return CFR50::qty(rec.record());
       }
   }
 return IString();
}

    /*-----------------------------------------------------------------------*\
    |   unsigned long cradDateYMD()                                           |
    |     return the CRAD Date in YYYYMMDD format.                            |
    \*-----------------------------------------------------------------------*/
unsigned long BLFeat::cradDateYMD()
{
 unsigned long ul = 0;
 if (pF)
   if (!(ul = pF->cradDateYMD(true)))
     ul = pF->cradDateYMD(false);
 return ul;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean cradDate(IDate &date)                                         |
    |       set the given IDate to the object's CRAD Date                     |
    |       returning false if not available                                  |
    \*-----------------------------------------------------------------------*/
Boolean BLFeat::cradDate(IDate &date)
{
 Boolean fOK = false;
 if (pF)
   if (!(fOK = pF->cradDate(date,true)))
     fOK = pF->cradDate(date,false);
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean csdDate(IDate &date)                                          |
    |       set the given IDate to the object's CSD Date                      |
    |       returning false if not available                                  |
    \*-----------------------------------------------------------------------*/
Boolean BLFeat::csdDate(IDate &date)
{
 Boolean fOK = false;
 if (pF)
   if (!(fOK = pF->csdDate(date,true)))
     fOK = pF->csdDate(date,false);
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |   unsigned long csdDateYMD()                                            |
    |     return the CSD Date in YYYYMMDD format.                             |
    \*-----------------------------------------------------------------------*/
unsigned long BLFeat::csdDateYMD()
{
 unsigned long ul = 0;
 if (pF)
   if (!(ul = pF->csdDateYMD(true)))
     ul = pF->csdDateYMD(false);
 return ul;
}

ostream & BLFeat::out(ostream& os)
{
  IString ser,desc;
  description(desc);
  return os  << "Machine "
             << " -> Type: '" << machine()
             << "' Model: '" << model()
             << "' Feature : '"  << feature()
             << "' displaySerialNo : '" << displaySerialNo(ser)
             << "' signedQty() : '" << signedQty()
             << "' csdDateYMD() : '" << csdDateYMD()
             << "' cradDateYMD() : '" << cradDateYMD()
             << "' description() : '" << desc
             << "'";
}


    /*-----------------------------------------------------------------------*\
    |       inline 'C' compare functions for the BLFeatList collection        |
    \*-----------------------------------------------------------------------*/
inline long BLFeatCompareUp(const BLFeat& blf1,const BLFeat& blf2)
     { return (( blf1 == blf2)? 0 : ((blf1 < blf2)? -1 : 1));}
inline long BLFeatCompareDown(const BLFeat& blf1,const BLFeat& blf2)
     { return (-1*BLFeatCompareUp(blf1,blf2)); }

 /*--------------------------------------------------------------------------*\
 |                              BLFeatList Class                              |
 \*--------------------------------------------------------------------------*/

void BLFeatList :: Sort(Boolean fUp)
{ this->sort(fUp ? BLFeatCompareUp : BLFeatCompareDown); }

 /*--------------------------------------------------------------------------*\
 |                         Non-Compiled testing code                          |
 \*--------------------------------------------------------------------------*/

#ifdef UNITTEST

void main(void)
{

///////////////////////////////////////////

 if (1)
   {

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

