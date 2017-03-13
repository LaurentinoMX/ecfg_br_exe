//=======================================================================
// FILE NAME        AF9DTOBJ.HPP
//
// CLASS NAME       DetailObject
//
// DESCRIPTION      This class represents a line of information in the
//                  detail window of the Order Apply CPF.
//=======================================================================
// Copyright (c) International Business Machines Corp. 1996
// All rights reserved
//=======================================================================

#ifndef AF9DTOBJ_HPP
#define AF9DTOBJ_HPP

#define INCL_NOBASEAPI
#define INCL_NOPMAPI
#include <os2.h>

#include <icnrobj.hpp>                  // IContainerObject
#include <idate.hpp>                    // IDate

#include "af9apply.h"                   // Resource ids

class Product;
class Mes;

class DetailObject : public IContainerObject
   {
   public:
   DetailObject();

   Boolean trailer();
   Product* product();
   Mes* MES();

   //Added to retrieve private members of Detail Object
   IString getLabel(DetailObject* pDetail);
   IString getQuantity(DetailObject* pDetail);
   IString getStatus(DetailObject* pDetail);
   IString getText(DetailObject* pDetail);

   void setLabel(const ULONG ul);
   void setLabel(const IString& str);
   void setQuantity(const IString& str);
   void setStatus(const ULONG ul);
   void setText(const ULONG ul);
   void setText(const IString& str);
   void setTrailer(Boolean b);
   void setProduct(Product* p);
   void setMES(Mes* p);

   void format();

   static ULONG offsetLabel();
   static ULONG offsetQuantity();
   static ULONG offsetStatus();
   static ULONG offsetText();

   private:
   Boolean bTrailer;
   ULONG ulLabel;
   IString strLabel;
   IString strQuantity;
   ULONG ulStatus;
   IString strStatus;
   ULONG ulText;
   IString strText;
   Product* pProduct;
   Mes* pMES;
   };
#endif
