 /*--------------------------------------------------------------------------*\
 |    Filename - PKFEAT.HPP                                                   |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
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
#ifndef _AB_PKFEAT_HPP_
#define _AB_PKFEAT_HPP_
#include "euodef.h"                    // CFReport structures

#include <iostream.h>
#include <istring.hpp>

#pragma pack(4)

class PKBase
{
  char     MType     [MACH_TYPE_SIZE + 1];
  char     MMod      [MACH_MODEL_SIZE + 1];
  char     FeatC     [FEAT_SIZE + 1];

public:

  PKBase();
  ~PKBase(){}
  PKBase(const PKBase&);
  PKBase(const char * Type, const char * Model, const char * Feat = 0);

  PKBase &  operator=(PKBase const& other);
  Boolean operator==(PKBase const& other) const;
  Boolean operator< (PKBase const& other) const;
  Boolean operator> (PKBase const& other) const;
  Boolean   isLike( const char * type
                   ,const char * model
                   ,const char * feature);

  int          compBaseSort(PKBase const& other) const;
  int          compBase(PKBase const& other) const;
  int          compBaseFind(PKBase const& other) const;
  ostream &    out(ostream&)const;
  const char * type()    { return (const char *)MType;}
  const char * model()   { return (const char *)MMod;}
  const char * feature() { return (const char *)FeatC;}
};

class PKFeat : public PKBase
{
  char     MRFeatC1[FEAT_SIZE + 1];
  char     MRFeatC2[FEAT_SIZE + 1];
  unsigned Qty1;
  unsigned Qty2;

public:

  PKFeat();
  ~PKFeat(){}
  PKFeat(const PKFeat&);
  PKFeat(const char * Type, const char * Model, const char * AASFeat = 0,
         const char * MRFeat1 = 0,unsigned Qty1 = 0,
         const char * MRFeat2 = 0,unsigned Qty2 = 0);

  PKFeat &  operator=(PKFeat const& other);
  Boolean operator==(PKFeat const& other) const;
  Boolean operator< (PKFeat const& other) const;
  Boolean operator> (PKFeat const& other) const;
  int     compare(PKFeat const&) const;
  int     compareSort(PKFeat const&) const;
  const char * featureMR1() { return (const char *) MRFeatC1;}
  const char * featureMR2() { return (const char *) MRFeatC2;}
  unsigned     qtyFeatureMR1() const { return Qty1;}
  unsigned     qtyFeatureMR2() const { return Qty2;}

  ostream &         out(ostream&)const ;

  friend ostream& operator<< (ostream& os, PKFeat const& r)
                        { return r.out(os); }



};

class PKFeatDesc : public PKBase
{
  char     Desc[FEAT_DESC_SIZE + 1];

public:

  PKFeatDesc();
  ~PKFeatDesc(){}
  PKFeatDesc(const PKFeatDesc&);
  PKFeatDesc(const char * Type, const char * Model, const char * Feat,
             const char * Description);

  PKFeatDesc &  operator=(PKFeatDesc const& other);
  Boolean operator==(PKFeatDesc const& other) const;
  Boolean operator< (PKFeatDesc const& other) const;
  Boolean operator> (PKFeatDesc const& other) const;
  int     compare(PKFeatDesc const&) const;
  const char * description()const { return (const char *) Desc;}

  ostream &         out(ostream&)const ;

  friend ostream& operator<< (ostream& os, PKFeatDesc const& r)
                        { return r.out(os); }



};

#pragma pack()
#endif





