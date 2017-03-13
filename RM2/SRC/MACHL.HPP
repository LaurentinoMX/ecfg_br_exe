#ifndef _AB_MACHL_HPP_
#define _AB_MACHL_HPP_
 /*--------------------------------------------------------------------------*\
 |    File Name  : MACHL.HPP                                                  |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |   MachineList - Array of Machine object pointers.                          |
 |                 as defined in MACH.HPP                                     |
 |                                                                            |
 \*--------------------------------------------------------------------------*/

#define  MACHL_ERR_NO_MACHL_FILE   1
#define  MACHL_ERR_NO_FILE         2
#define  MACHL_ERR_READ_FILE       3


#include "parr.hpp"
#include "mach.hpp"
#include "cfr05.hpp"
#include "cfr10.hpp"

#pragma pack(4)
class MachineList : public PtrRecArr
{
  MachineList( unsigned, unsigned, comp_fn_tp){}  // hide
 protected:
  int          fWriteSubSystemBottom(FILE *f,int,
                                       CFR52List &,
                                       CFRInstL  &,
                                       CFRInstL  &,
                                       CFR10List &,
                                       CFR05List &);
 public:
  MachineList();
  Machine *     Find(const char *szType,const char * szModel,
                     const char *szSerial);
  Machine *     Find(Machine & other);
  Machine *     FindBS(Machine & other);    // binary search
  Machine *     FindProcessMachine(Machine & other);
  Machine *     FindProcessMachine( const char * szType
                                   ,const char * szModel
                                   ,const char * szSerial);
  Machine *     MachineList::FindProcessTypeSerialMachine(Machine & other);
  Machine *     element(unsigned index);
//  int           Write(FILE *f);
  int           fWriteCFR(FILE *f,Boolean fMRDB,
                           unsigned * pInstId,
                           CFRInstL & L55_In, CFRInstL & L55_Out,
                           CFRInstL & L56_In, CFRInstL & L56_Out);
  int           fWriteCFRV31(FILE *f,Boolean fMRDB, unsigned * pNextInstId,
                             CFR52List & L52_In, CFRInstL & L55_In,
                             CFRInstL & L56_In,
                             CFR10List & SubSysRecList,
                             CFR05List & CommentRecList,
                             IString sSysId );                  //edw06/04/1998
  void        Release(Boolean fRipple = false);
  Boolean     fReadB(FILE *f);
  Boolean     fReadB(char * szFileName);
  int         fReadB_rc(FILE *f);
  int         fReadB_rc(char * szFileName);
  Boolean     fWriteB(FILE *f, unsigned * pBytesWrit = 0);
  Boolean     fWriteB(char * szFileName, unsigned * pBytesWrit = 0);
  unsigned    fWriteBLength();
  MachineList &  SetStorageNumbers();

  friend ostream& operator<< (ostream& os, MachineList & list)
                {
                 for (int i = 0, N = list.NumberElements() ; i < N ; i++ )
                   os << *list.element(i) << endl << endl;
                 return os;
                }

//  virtual      ~MachineList(){;}
};

#pragma pack()
#endif

