
#include <isynonym.hpp> // Definition of Boolean etc
#include "euoutil.hpp"
#include "rpo2.hpp"

char * AddColumn( char * pszTarget, char * pszSource,
                  int iStart, int iBuffSize)
{
 if (pszTarget && pszSource) {
   char * p1 = pszTarget + strlen(pszTarget);
   char * p2 = pszTarget + iStart;
   while ( p1 < p2)                            // fill in with blanks
     *p1++ = ' ';
   for (p1 = pszTarget + iBuffSize; (p2 < p1) && *pszSource;)
     *p2++ = *pszSource++;                     // append source
   *p2 = 0;
   }
 return pszTarget;
}

/////////////////


RPO2::RPO2()
{
 memset(this,0,sizeof(*this));
 Code = ' ';
}

RPO2::RPO2( const char * type
           ,const char * model
           ,const char * serialNo
           ,const char * feat
           ,const char * desc
           ,unsigned qty
           ,char code
           ,const char * customer  //* D83402  HECC: 07/28/2003
           ,const char * sysFlag   //* D83402  HECC: 07/28/2003
           ,const char * sysType   //* D83402  HECC: 07/28/2003
           ,const char * sysNum    //* D83402  HECC: 07/28/2003
           ,const char * prodDsc  //* F73686  HECC: 10/15/2002
           ,char prodCat )        //* F73686  HECC: 10/15/2002
     : Qty(qty)
      ,Code(code)
      ,ProdCat(prodCat)           //* F73686  HECC: 10/15/2002
{
 mystrstrip(mystrncpy(Type,type,sizeof(Type)-1));
 mystrstrip(mystrncpy(Mod,model,sizeof(Mod)-1));
 mystrstrip(mystrncpy(SerNo,serialNo,sizeof(SerNo)-1));
 mystrstrip(mystrncpy(Feat,feat,sizeof(Feat)-1));
 mystrstrip(mystrncpy(Desc,desc,sizeof(Desc)-1));
 mystrstrip(mystrncpy(ProdDesc,prodDsc,sizeof(ProdDesc)-1)); //* F73686  HECC: 10/15/2002

 mystrstrip(mystrncpy(CustNum,customer,sizeof(CustNum)-1)); //* D83402  HECC: 07/28/2003
 mystrstrip(mystrncpy(SystemFlag,sysFlag,sizeof(SystemFlag)-1)); //* D83402  HECC: 07/28/2003
 mystrstrip(mystrncpy(SysType,sysType,sizeof(SysType)-1));  //* D83402  HECC: 07/28/2003
 mystrstrip(mystrncpy(SysNum,sysNum,sizeof(SysNum)-1));     //* D83402  HECC: 07/28/2003

}

RPO2::RPO2(const RPO2 & other)
{
 *this = other;
}

Boolean RPO2 :: equals(const char * szType, const char * szModel,
                        const char * szSerialNo, const char * szFeat)
{
  return (!strcmp(Type,szType)               &&
          !strcmp(Mod,szModel)               &&
          !compSerialNo(SerNo,szSerialNo)    &&
          !strcmp(Feat,szFeat));
}

Boolean RPO2 :: sameMachine(RPO2 const& other) const
{
 return (!strcmp(Type,other.Type)           &&
         !strcmp(Mod,other.Mod)             &&
         !compSerialNo(SerNo,other.SerNo));
}

int RPO2::compare(RPO2 const& other) const
{
  int i;
  if (!(i = strcmp(Type,other.Type))           &&
      !(i = strcmp(Mod,other.Mod))             &&
//      !(i = strcmp(SerNo,other.SerNo))       &&
      !(i = compSerialNo(SerNo,other.SerNo))   &&
      !(i = strcmp(Feat,other.Feat))           &&
      !(i = ( Code - other.Code)))
        i = strcmp(Desc,other.Desc);
  return i;
}

RPO2 & RPO2::operator=(RPO2 const& other)
{
 if (this != &other)
   memcpy(this,&other,sizeof(*this));
 return *this;
}

Boolean RPO2::operator==(RPO2 const& other)  const
{
 return !compare(other);
}

Boolean RPO2::operator< (RPO2 const& other) const
{
 return (compare(other) < 0);
}

Boolean RPO2::operator> (RPO2 const& other) const
{
 return (compare(other) > 0);
}

     /*----------------------------------------------------------------------*\
     |   char * FormatText(char * Buffer, int iBuffSize )                     |
     |       Method to format an RPO into a target display string             |
     |       with columns ensuring that the resulting string does             |
     |       not exceed a given length.                                       |
     |       Column positions defined in RPO.HPP                              |
     |       Return the given target string pointer.                          |
     \*----------------------------------------------------------------------*/

char * RPO2 :: formatText(char * Buffer, unsigned iBuffSize )
{
 if (Buffer && iBuffSize) {
   *Buffer = 0;
   IString str(Qty);
   AddColumn(Buffer,(char *)str,RPO_COL1,iBuffSize);
   str = IString(Code);
   AddColumn(Buffer,(char *)str,RPO_COL2,iBuffSize);
   AddColumn(Buffer,(char *)Desc,RPO_COL3,iBuffSize);
   }
 return Buffer;
}


ostream & RPO2 ::out(ostream& os)const
{
  os << "  Type : '"           << Type
     << "' Model : '"          << Mod
     << "' SerialNo : '"       << SerNo
     << "' Feature : '"        << Feat
     << "' Code : '"           << Code
     << "' Qty : "             << Qty
     << "  Description : '"    << Desc
     << "'";
  return os;
}


////////////////////////////////////////////////////////////
#ifdef UNITTEST

#include <iostream.h>


void main(void)
{
 char Buff[200];
 char c;
 int Id;

 if (1)
   {
    RPO2 r1( "Typ1","Mod1","Serial1","Feat1","Description 1",2,'A');
    cout << r1 << endl;
    cout <<"Formatted Text '" << r1.formatText(Buff,sizeof(Buff)) << "'\n";
//    r1.setFromFormattedText (Buff,&Id,&c);
//    cout <<"setFromFormattedText : Code '" << c << "' Id : " << Id << endl;

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif

   }

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif
}

#endif
