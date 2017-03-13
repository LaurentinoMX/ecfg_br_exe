#ifndef _AB_RPO2_HPP_
#define _AB_RPO2_HPP_
 /*--------------------------------------------------------------------------*\
 |    File Name  : RPO.HPP                                                    |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |      RPO    -   EUOP's RPO                                                 |
 \*--------------------------------------------------------------------------*/
#include "euodef.h"

        /*---------------------------------------------------*\
        |               Column position defines               |
        \*---------------------------------------------------*/
#define RPO_COL1 0
#define RPO_COL2 7
#define RPO_COL3 10

        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/
#include <istring.hpp>

#define RPO_CODE_CHANGE 'C'
#define RPO_CODE_ADD    'A'
#define RPO_CODE_DELETE 'D'

#pragma pack(4)
class RPO2
{
  char Type  [MACH_TYPE_SIZE + 1];
  char Mod   [MACH_MODEL_SIZE + 1];
  char SerNo [MACH_SERIALNO_SIZE + 1];
  char Feat  [FEAT_SIZE + 1];
  char Desc  [FEAT_DESC_SIZE +1];
  unsigned  Qty;
  char Code;

  //* F73686 New Variables to store the category the RPO feature belongs to
  //* (Hardware or Software) and the Product description wich the  RPO feature is
  //* part  HECC: 10/14/2002
  char ProdDesc [FEAT_DESC_SIZE +1];
  char ProdCat;

  //* D83402 New four variables to store next data of the RPO record:
  //* Customer number, System number flag, System type and System number. HECC:  07/28/2003
  char CustNum [CUSTOM_NUMBER_SIZE +1];
  char SystemFlag [2];
  char SysType [MACH_TYPE_SIZE +1];
  char SysNum  [SYSTEM_NUMBER_SIZE +1];

  //* End new logic: HECC:  07/28/2003

  int         compare(RPO2 const&) const;

public:

  RPO2();
  ~RPO2(){}
  RPO2(const RPO2&);
  RPO2(const char * szType, const char * szModel,const char * szSerialNo,
       const char * szFeat,const char *szDesc,unsigned quantity,char Action,
       const char * customer, const char * sysFlag,   //* D83402  HECC: 07/28/2003
       const char * sysType,  const char * sysNum,    //* D83402  HECC: 07/28/2003
       const char * strProdDesc, char cProdCat );   //* 73686   HECC: 10/14/2002

  RPO2 &  operator=(RPO2 const& other);
  Boolean operator==(RPO2 const& other) const;
  Boolean operator< (RPO2 const& other) const;
  Boolean operator> (RPO2 const& other) const;

  virtual char    * formatText(char * Buffer, unsigned iBuffSize);
  ostream &         out(ostream&)const ;
  const char *      text() const { return (const char *)Desc; }
  unsigned          qty()  const { return Qty; };
  int               signedQty() const
                         { return (Code == RPO_CODE_ADD)? Qty : -Qty;}

  //* F73686 Returns the Feature action for the RT 19 according to the Code data
  //* present in the retrieved record from the RPO list.        HECC: 09/06/2002
  int               featureAct() const
                         { return (Code == RPO_CODE_ADD)? 1 : 2;}

  void              setQty(unsigned u) {Qty = u; }
  char              action() const { return Code; }
  void              setAction(char a) { Code = a; }
  Boolean           equals(const char * szType, const char * szModel,
                           const char * szSerialNo, const char * szFeat);
  Boolean           sameMachine(RPO2 const&) const;

  static Boolean   setFromFormattedText(char * Buffer,int *pId, char *pCode = 0);

  friend ostream& operator<< (ostream& os, RPO2 const& r)
                        { return r.out(os); }

  IString  machineType()  const { return IString(Type ); };
  IString  machineModel() const { return IString(Mod  ); };
  IString  serialNum()    const { return IString(SerNo); };
  IString  featureCode()  const { return IString(Feat ); };
  IString  description()  const { return IString(Desc ); };

  //* F73686 Returns variables: Machine description and Product category (HW/SW)
  //* HECC: 10/14/2002

  IString  machineDescr() const { return IString(ProdDesc ); };
  IString  productCat()   const { return IString(ProdCat ); };

  //* D83402 Begin new logic: Returns variables: Customer number, System number flag, System type and
  //* System number.

  IString  customerNumber() const { return IString(CustNum ); };
  IString  systNumberFlag() const { return IString(SystemFlag ); };
  IString  systemType()     const { return IString(SysType ); };
  IString  systemNum()      const { return IString(SysNum ); };

  //* D83402 End new logic:  HECC:  07/28/2003

};
#pragma pack()
#endif

