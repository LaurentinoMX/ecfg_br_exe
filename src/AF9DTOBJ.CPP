//=======================================================================
// FILE NAME        AF9DTOBJ.CPP
//
// CLASS NAME       DetailObject
//
// DESCRIPTION      This class represents a line of information in the
//                  detail window of the Order Apply CPF.
//=======================================================================
// Copyright (c) International Business Machines Corp. 1996
// All rights reserved
//=======================================================================

#include <icnrobj.hpp>                  // IContainerObject
#include <ireslib.hpp>                  // IResourceLibrary

#include "af9dtobj.hpp"                 // DetailObject
#include "afcprods.hpp"                 // Product, Mes
#include "af9apply.h"                   // Resource ids

//=======================================================================
//
// Constructor
//
//=======================================================================

DetailObject :: DetailObject()
   {
   bTrailer = false;
   pProduct = (Product*)NULL;
   pMES = (Mes*)NULL;
   ulLabel = ulStatus = ulText = 0;
   }

//=======================================================================
//
// Functions to return object values
//
//=======================================================================

Boolean DetailObject :: trailer() { return bTrailer; }
Product* DetailObject :: product() { return pProduct; }
Mes* DetailObject :: MES() { return pMES; }
//47703
IString DetailObject :: getLabel(DetailObject* pDetail) {return pDetail->strLabel;}
IString DetailObject :: getQuantity(DetailObject* pDetail) {return pDetail->strQuantity;}
IString DetailObject :: getStatus(DetailObject* pDetail) {return pDetail->strStatus;}
IString DetailObject :: getText(DetailObject* pDetail) {return pDetail->strText;}

//=======================================================================
//
// Functions to set object values
//
//=======================================================================

void DetailObject :: setLabel(const ULONG ul)
   {
   ulLabel = ul;
   strLabel = LOADSTRING(ulLabel);
   }
void DetailObject :: setLabel(const IString& str)
   {
   ulLabel = 0;
   strLabel = str;
   }
void DetailObject :: setQuantity(const IString& str) { strQuantity = str; }
void DetailObject :: setStatus(const ULONG ul)
   {
   ulStatus = ul;
   strStatus = LOADSTRING(ulStatus);
   }
void DetailObject :: setText(const IString& str) { strText = str; }
void DetailObject :: setText(const ULONG ul)
   {
   ulText = ul;
   strText = LOADSTRING(ulText);
   }
void DetailObject :: setTrailer(Boolean b) { bTrailer = b; }
void DetailObject :: setProduct(Product* p) { pProduct = p; }
void DetailObject :: setMES(Mes* p) { pMES = p; }

//=======================================================================
//
// Reformat values when switching locale
//
//=======================================================================

void DetailObject :: format()
   {
   if ( ulLabel )
      {
      strLabel = LOADSTRING(ulLabel);
      if ( ulLabel == IDS_DetailDate )
         if ( pProduct )
            strText = pProduct->date().asString(STR_FormatDate);
         else if ( pMES )
            strText = pMES->date().asString(STR_FormatDate);
      }
   if ( ulStatus )
      strStatus = LOADSTRING(ulStatus);
   if ( ulText )
      strText = LOADSTRING(ulText);
   }

//=======================================================================
//
// Offsets for container columns
//
//=======================================================================

ULONG DetailObject :: offsetLabel()
   { return offsetof(DetailObject, strLabel); }
ULONG DetailObject :: offsetQuantity()
   { return offsetof(DetailObject, strQuantity); }
ULONG DetailObject :: offsetStatus()
   { return offsetof(DetailObject, strStatus); }
ULONG DetailObject :: offsetText()
   { return offsetof(DetailObject, strText); }
