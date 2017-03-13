#ifndef _AB_XPLN2_HPP_
   #define _AB_XPLN2_HPP_
 /*--------------------------------------------------------------------------*\
 |    File Name  : XPLN2.HPP                                                  |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |      XPLN   -   RM/2's Explanation                                         |
 |                 messages generated as a result of the resolution           |
                   process available for the user's viewing),                 |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/
#include <iostream.h>
#include <istring.hpp>
#include <isrtset.h>

class XPLN2
{
  IString strMach ;
  IString strModel;
  IString strSerNo;
  IString strFeat ;
  IString strText ;

 public:

  XPLN2(const IString & Machine = IString(),
       const IString & Model = IString(),
       const IString & SerialNo = IString(),
       const IString & Feature = IString(),
       const IString & Text = IString())
          : strMach(Machine)
           ,strModel(Model)
           ,strSerNo(SerialNo)
           ,strFeat(Feature)
           ,strText(Text) {}
  XPLN2(const char * szMachine  = 0,
       const char * szModel    = 0,
       const char * szSerialNo = 0,
       const char * szFeature  = 0,
       const char * szText     = 0)
          : strMach (szMachine)
           ,strModel(szModel)
           ,strSerNo(szSerialNo)
           ,strFeat (szFeature)
           ,strText (szText) {}
  XPLN2() {}

  IString const& machineString()  const { return strMach; }
  IString const& modelString()    const { return strModel;}
  IString const& serialNoString() const { return strSerNo;}
  IString const& featureString()  const { return strFeat ;}
  IString const& textString()     const { return strText ;}

  const char *         text() const    { return (char *)strText;  }
  const char *         machine() const { return (char *)strMach; }
  const char *         model() const   { return (char *)strModel; }
  const char *         feature() const { return (char *)strFeat ; }
  const char *         serialNo() const{ return (char *)strSerNo ; }

  XPLN2 & operator=(XPLN2 const& other) { if (this != &other)
                                            { strMach  = other.strMach ;
                                              strModel = other.strModel;
                                              strSerNo = other.strSerNo;
                                              strFeat  = other.strFeat ;
                                              strText  = other.strText ;
                                            }
                                          return *this;
                                        }
                                                 // For any Set we need to
#if 0                                                 // provide element equality.
  Boolean operator==(XPLN2 const& other)  const
     {return ( (!other.strMach.length()  || (strMach == other.strMach  ))  &&
               (!other.strModel.length() || (strModel == other.strModel))  &&
               (!other.strSerNo.length() || (strSerNo == other.strSerNo))  &&
               (!other.strFeat.length()  || (strFeat  == other.strFeat ))
               );}
#endif
  Boolean operator==(XPLN2 const& other)  const
     {return ( (strMach == other.strMach)    &&
               (strModel == other.strModel)  &&
               (strSerNo == other.strSerNo)  &&
               (strFeat  == other.strFeat)
             );}

                                                 // For a Sorted Set we need to
                                                 // provide element
                                                 // comparision.
  Boolean operator< (XPLN2 const& other) const
         { if (strMach == other.strMach)
             if (strModel == other.strModel)
               if (strSerNo == other.strSerNo)
                 if (strFeat == other.strFeat)
                   return (strText < other.strText);
                 else return (strFeat < other.strFeat);
               else return (strSerNo < other.strSerNo);
             else return (strModel < other.strModel);
           else return (strMach < other.strMach);
         }

  friend ostream& operator<< (ostream& os, XPLN2 const& x)
                        {
                          os << "Machine : '"     << x.machine()
                             << "' Model : '"     << x.model()
                             << "' SerialNo : '"  << x.serialNo()
                             << "' Feature : '"   << x.feature()
                             << "' Text : '"      << x.text()
                             << "'";
                          return os;
                        }
};

#endif
