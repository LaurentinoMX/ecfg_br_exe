 /*--------------------------------------------------------------------------*\
 |    Filename - PKFEAT.CPP                                                   |
 |      contains source for the following RM/2 class implementations:         |
 |                                                                            |
 |       PKBase - Common object containing Machine Type, Model and            |
 |                Feature Name shared by the following classes:               |
 |                                                                            |
 |    PKFeatDesc- Product Knowledge Feature Description,                      |
 |                containing in addition to the Base                          |
 |                (Type, Model and Feature), the description                  |
 |                                                                            |
 |       PKFeat - Product Knowledge Feature, containing in addition           |
 |                to the Base (Type, Model and Feature)                       |
 |                2 (machine readable) features and their quantities.         |
 |                (This encapsulates the entries in the ASCII Product         |
 |                Knowledge File distributed with the application.            |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                  ICLUI's Includes                   |
        \*---------------------------------------------------*/
#include <isynonym.hpp>                          // Definition of Boolean etc

        /*---------------------------------------------------*\
        |               RM/2 specific Includes                |
        \*---------------------------------------------------*/
#include "euoutil.hpp"                           // utilities
#include "pkfeat.hpp"                            // this file's declarations


 /*--------------------------------------------------------------------------*\
 |                                PKBase Class                                |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |                        Ctors                        |
        \*---------------------------------------------------*/
PKBase::PKBase()
{
 memset(this,0,sizeof(*this));
}

PKBase::PKBase( const char * type
               ,const char * model
               ,const char * feat)
{
 mystrstrip(mystrncpy(MType,type,sizeof(MType)-1));
 mystrstrip(mystrncpy(MMod,model,sizeof(MMod)-1));
 mystrstrip(mystrncpy(FeatC,feat,sizeof(FeatC)-1));
}

PKBase::PKBase(const PKBase & other)
{
 *this = other;
}

    /*-------------------------------------------------------*\
    |                   Compare (straight)                    |
    \*-------------------------------------------------------*/
int PKBase::compBase(PKBase const& other) const
{
  int i = strcmp(MType,other.MType);
  if (!i && !(i = strcmp(MMod,other.MMod)))
     i = strcmp(FeatC,other.FeatC);
  return i;
}

    /*-----------------------------------------------------------------------*\
    |    int compBaseFind(PKBase const& other)                                |
    |       Compare to be used if the given object's Model                    |
    |        can contain wildcard characters.                                 |
    \*-----------------------------------------------------------------------*/
int PKBase::compBaseFind(PKBase const& other) const
{
  int i = strcmp(MType,other.MType);
  if (!i)
    if (IString(other.MMod).isLike(MMod))
      i = strcmp(FeatC,other.FeatC);
    else i = strcmp(MMod,other.MMod);
  return i;
}

    /*-----------------------------------------------------------------------*\
    |    int compBaseSort(PKBase const& other)                                |
    |       Compare to be used in sorting such that the Model's               |
    |       wilcard characters ('*' and '?') are collated after digits        |
    |       and letters while '?' is ranked before '*'.                       |
    |       (This is necessary to be able to locate specific Models           |
    |       before those containing wildcards in a linear search)             |
    \*-----------------------------------------------------------------------*/
int PKBase::compBaseSort(PKBase const& other) const
{
  int i = strcmp(MType,other.MType);
  if (!i && !(i = myWildCardSortCompare(MMod,other.MMod)))
     i = strcmp(FeatC,other.FeatC);
  return i;
}

    /*-------------------------------------------------------*\
    |                       Assignment                        |
    \*-------------------------------------------------------*/
PKBase & PKBase::operator=(PKBase const& other)
{
 if (this != &other)
   memcpy(this,&other,sizeof(*this));
 return *this;
}

    /*-------------------------------------------------------*\
    |                        Equality                         |
    \*-------------------------------------------------------*/
Boolean PKBase::operator==(PKBase const& other)  const
{
 return !compBase(other);
}

    /*-------------------------------------------------------*\
    |                       Inequality                        |
    \*-------------------------------------------------------*/
Boolean PKBase::operator< (PKBase const& other) const
{
 return (compBase(other) < 0);
}
Boolean PKBase::operator> (PKBase const& other) const
{
 return (compBase(other) > 0);
}

    /*-----------------------------------------------------------------------*\
    |    Boolean isLike()                                                     |
    |       Compare given Type and Model (which may contain                   |
    |       wild characters) and a straight compare on the given              |
    |       feature                                                           |
    \*-----------------------------------------------------------------------*/
Boolean PKBase::isLike( const char * type
                       ,const char * model
                       ,const char * feature)
{
 return  ( IString(type).isLike(MType) &&
           IString(model).isLike(MMod) &&
           !strcmp(feature,FeatC));
}

ostream & PKBase ::out(ostream& os)const
{
  os << "  MType : '"          << MType
     << "' MMod : '"           << MMod
     << "' FeatC : '"         <<  FeatC
     << "'";
  return os;
}

 /*--------------------------------------------------------------------------*\
 |                                PKFeat Class                                |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |                        Ctors                        |
        \*---------------------------------------------------*/
PKFeat::PKFeat()
       : PKBase()

{
  *MRFeatC1  = *MRFeatC2 = '\0';
  Qty1 = Qty2 = 0;
}

PKFeat::PKFeat( const char * type
               ,const char * model
               ,const char * aasFeat
               ,const char * featMR1
               ,unsigned qty1
               ,const char * featMR2
               ,unsigned qty2)
      :PKBase( type
              ,model
              ,aasFeat)
      ,Qty1(qty1)
      ,Qty2(qty2)
{
 mystrstrip(mystrncpy(MRFeatC1,featMR1,sizeof(MRFeatC1)-1));
 mystrstrip(mystrncpy(MRFeatC2,featMR2,sizeof(MRFeatC2)-1));
}

PKFeat::PKFeat(const PKFeat & other)
{
 *this = other;
}

    /*-------------------------------------------------------*\
    |                   Compare (straight)                    |
    \*-------------------------------------------------------*/
int PKFeat::compare(PKFeat const& other) const
{
  int i;

  if (!(i = compBase(*(const PKBase *)&other))      &&
      !(i = strcmp(MRFeatC1,other.MRFeatC1))  &&
      !(i = strcmp(MRFeatC2,other.MRFeatC2))  &&
      !(i = (Qty1 - other.Qty1)))
        i = (Qty2 - other.Qty2);
  return i;
}

    /*-----------------------------------------------------------------------*\
    |    int compareSort(PKFeat const& other) const                           |
    |       Method to be used in sorting such that the Model's                |
    |       wilcard characters ('*' and '?') are collated after digits        |
    |       and letters while '?' is ranked before '*'.                       |
    |       (This is necessary to be able to locate specific Models           |
    |       before those containing wildcards in a linear search)             |
    \*-----------------------------------------------------------------------*/
int PKFeat::compareSort(PKFeat const& other) const
{
  int i;

  if (!(i = compBaseSort(*(const PKBase *)&other))  &&
      !(i = strcmp(MRFeatC1,other.MRFeatC1))  &&
      !(i = strcmp(MRFeatC2,other.MRFeatC2))  &&
      !(i = (Qty1 - other.Qty1)))
        i = (Qty2 - other.Qty2);
  return i;
}

    /*-------------------------------------------------------*\
    |                       Assignment                        |
    \*-------------------------------------------------------*/
PKFeat & PKFeat::operator=(PKFeat const& other)
{
 if (this != &other) {
   *(PKBase *)this = *(const PKBase *)&other;          // handles the PKBase part
   mystrncpy(MRFeatC1,other.MRFeatC1,sizeof(MRFeatC1)-1);
   mystrncpy(MRFeatC2,other.MRFeatC2,sizeof(MRFeatC2)-1);
   Qty1 = other.Qty1;
   Qty2 = other.Qty2;
   }
 return *this;
}

    /*-------------------------------------------------------*\
    |                        Equality                         |
    \*-------------------------------------------------------*/
Boolean PKFeat::operator==(PKFeat const& other)  const
{
 return !compare(other);
}

    /*-------------------------------------------------------*\
    |                       Inequality                        |
    \*-------------------------------------------------------*/
Boolean PKFeat::operator< (PKFeat const& other) const
{
 return (compare(other) < 0);
}
Boolean PKFeat::operator> (PKFeat const& other) const
{
 return (compare(other) > 0);
}

ostream & PKFeat ::out(ostream& os)const
{
  PKBase::out(os);
  os << "  MRFeat1 : '"    << MRFeatC1
     << "'  MRQty1 : "   << Qty1
     << " MRFeat2 : '"   << MRFeatC2
     << "'  MRQty2 : "   << Qty2;
  return os;
}

 /*--------------------------------------------------------------------------*\
 |                              PKFeatDesc Class                              |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |                        Ctors                        |
        \*---------------------------------------------------*/
PKFeatDesc::PKFeatDesc()
           : PKBase()
{
 *Desc = '\0';
}

PKFeatDesc::PKFeatDesc( const char * type
                       ,const char * model
                       ,const char * feat
                       ,const char * description)
      :PKBase( type
              ,model
              ,feat)
{
 mystrstrip(mystrncpy(Desc,description,sizeof(Desc)-1));
}

PKFeatDesc::PKFeatDesc(const PKFeatDesc & other)
{
 *this = other;
}

    /*-------------------------------------------------------*\
    |                   compare (straight)                    |
    \*-------------------------------------------------------*/
int PKFeatDesc::compare(PKFeatDesc const& other) const
{
 return  compBase(*(const PKBase *)&other);
}

    /*-------------------------------------------------------*\
    |                       Assignmnet                        |
    \*-------------------------------------------------------*/
PKFeatDesc & PKFeatDesc::operator=(PKFeatDesc const& other)
{
 if (this != &other) {
   *(PKBase *)this = *(const PKBase *)&other;
   mystrncpy(Desc,other.Desc,sizeof(Desc)-1);
   }
 return *this;
}

    /*-------------------------------------------------------*\
    |                        Equality                         |
    \*-------------------------------------------------------*/
Boolean PKFeatDesc::operator==(PKFeatDesc const& other)  const
{
 return !compare(other);
}

    /*-------------------------------------------------------*\
    |                       Inequality                        |
    \*-------------------------------------------------------*/
Boolean PKFeatDesc::operator< (PKFeatDesc const& other) const
{
 return (compare(other) < 0);
}
Boolean PKFeatDesc::operator> (PKFeatDesc const& other) const
{
 return (compare(other) > 0);
}

ostream & PKFeatDesc ::out(ostream& os)const
{
  PKBase::out(os);
  os << "  description : '"  << Desc << "'";
  return os;
}

 /*--------------------------------------------------------------------------*\
 |                         Non-Compiled testing code                          |
 \*--------------------------------------------------------------------------*/
#ifdef UNITTEST

#include <iostream.h>


void main(void)
{
 char Buff[200];
 char c;
 int Id;

 if (1)
   {
    PKFeat r1( "Typ1","Mod1","AFeat1","M1Feat1", 2, "M1Feat2",2);
    cout << r1 << endl;

    PKFeatDesc d1( "Typ1","Mod1","AFeat1","A Description");
    cout << d1 << endl;

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif

   }

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif
}

#endif

