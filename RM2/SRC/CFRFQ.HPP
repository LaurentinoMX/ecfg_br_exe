 /*--------------------------------------------------------------------------*\
 |    Filename - CFR19_50.HPP                                                 |
 |      declarations for the following RM/2 class implementations:            |
 |                                                                            |
 |      F7Q       - To manipulate a CFRecord's feature (of 7 bytes) and       |
 |                  quantity combination.                                     |
 |      F7QA      - To manipulate a CFRecord's feature (of 7 bytes),          |
 |                  quantity and Action combination.                          |
 |                                                                            |
 \*--------------------------------------------------------------------------*/

#ifndef _AB_CFRFQ_HPP_
#define _AB_CFRFQ_HPP_

#include <fstream.h>

#include "euodef.h"
#include "euoutil.hpp"

#pragma pack(4)
class F7Q
{
 char       szFeature[8];
 unsigned   Qty;

 public:
  F7Q() : Qty(0) { *szFeature = '\0';}
  ~F7Q(){}
  F7Q(const F7Q & other) { *this = other; }
  F7Q(const char * szFeat, unsigned quantity);
  F7Q( const Feat44 *);
  F7Q( const Feat74 *);
  F7Q( const Feat75 *);
  F7Q( const Feat78 *);

  F7Q &   operator=(F7Q const& other);

  Feat44 *  copyTo(Feat44 *);
  Feat74 *  copyTo(Feat74 *);
  Feat75 *  copyTo(Feat75 *);
  Feat78 *  copyTo(Feat78 *);

  F7Q  &         paddCopyQuantity( char * Buf, unsigned BufLen);
  F7Q  &         paddCopyFeature( char * Buf, unsigned BufLen);
  F7Q  &         copyFeaturePadd( char * Buf, unsigned BufLen);
  const char *   feature() const { return (const char *)szFeature; }
  unsigned       quantity() const { return Qty; }
  void           setFeature(const char * szFeat);
  void           setQuantity(unsigned u) { Qty = u; }

  friend ostream& operator<< (ostream& os, F7Q const& f)
                       { return os << " Feature : '" << f.feature()
                                   << "'  Qty : " << f.quantity();
                       }
};

class F7QA : public F7Q
{
  char       Action;

 public:
  F7QA() : F7Q() { Action = ' ';}
  ~F7QA(){}
  F7QA(const F7QA & other) { *this = other; }
  F7QA(const char * szFeat, unsigned quantity, char action);
  F7QA( const FeatR19 * );
  F7QA( const Feat751 * );
  F7QA( const FeatR50 * );
  F7QA( const FeatR38 * );    //* F86750  HECC: 01/13/2004

  F7QA &       operator=(F7QA const& other);

  FeatR19 *    copyTo(FeatR19 * pRec);
  FeatR50 *    copyTo(FeatR50 * pRec);
  Feat751 *    copyTo(Feat751 * pRec);
  FeatR38 *    copyTo(FeatR38 * pRec);  //* F86750  HECC: 01/13/2004

  char         action() const { return Action; }
  void         setAction(char a) { Action = a; }

  friend ostream& operator<< (ostream& os, F7QA const& f)
                       { return os << " Feature : '" << f.feature()
                                   << "'  Qty : " << f.quantity()
                                   << "'  Action : " << f.action();
                       }
};

#pragma pack()
#endif

