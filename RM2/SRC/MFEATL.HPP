#ifndef _AB_MFEATL_HPP_
#define _AB_MFEATL_HPP_
 /*--------------------------------------------------------------------------*\
 |    File Name  : MFEATL.HPP                                                 |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      RM/2 class implementation(s):                                         |
 |                                                                            |
 |   MFeatList  - Array of MFeat object pointers.                             |
 |                 as defined in MFEAT.HPP                                    |
 |                                                                            |
 \*--------------------------------------------------------------------------*/

#pragma pack(4)
#include "mfeat.hpp"
#include "parr.hpp"
class MFeatList : public PtrRecArr
{
  MFeatList( unsigned, unsigned, comp_fn_tp){} //hide
 public:
  MFeatList();
  virtual    ~MFeatList(){}

  MFeat *     Find(const char *szFeature);
  MFeat *     FindOEMFeature(const char *szFeature);
  MFeat *     FindUnmarkedFeature(const char *szFeature);
  MFeat *     Find(MFeat & mfeat);
  MFeat *     FindRecord(unsigned uRecNumber);
  MFeat *     element(unsigned index);
  MFeatList & remove(MFeat *);
  void        Release(Boolean fRipple = false);
  int         Write(FILE *f);
  int         Write(FILE *,MFeat::CFRType);
  int         fReadB(char *filename);
  int         fReadB(FILE *f , unsigned count);
  void        re_Sort(void);

  friend ostream& operator<< (ostream& os, MFeatList & list)
                {
                 for (int i = 0, N = list.NumberElements() ; i < N ; i++ )
                   os <<  *list.element(i) << endl;
                 return os;
                }
};

#pragma pack()
#endif
