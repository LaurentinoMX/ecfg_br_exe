#ifndef _AB_EUOPRES_HPP_
#define _AB_EUOPRES_HPP_
 /*--------------------------------------------------------------------------*\
 |    File Name  : EUOPRES.HPP                                                |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |   EUOPResolve - allowing for the retention of the system name              |
 |                 to be Reconciled by the application with                   |
 |                 methods to                                                 |
 |                      Open                                                  |
 |                      Resolve                                               |
 |                      Save                                                  |
 |                      Write                                                 |
 |                    the System class on which it is based.                  |
 |                                                                            |
 |                 The ResMessage class (in EUOUTIL.HPP) allows for the       |
 |                     formatting of Message File Strings                     |
 |                                                                            |
 \*--------------------------------------------------------------------------*/

#include <isrtset.h>                     // ISorted Set      edw07/01/1998
#include "system.hpp"                    // RMSystem class

#pragma pack(4)

#ifndef UNITTEST

class XWindow;

class EUOPResolve : public RMSystem
                   ,public ResMessage
{
  XWindow * pW;
  IString CurrentSystem;
protected:
  Boolean convert(XWindow *, const char * szFile,Boolean fMRDB,
                  IString & strConvertedFile,int version);
public:
  EUOPResolve(XWindow * p) : RMSystem(), pW(p){}
  ~EUOPResolve(){}

  Boolean resolveX( const char * AASFile
                   ,const char * MRDBFile
                   ,const char * MRDBDescFile
                   ,const char * MsgStringsFile
                   ,ISortedSet<IString>* pNoCompTable      //edw07/01/1998
                   ,ISortedSet<IString>* pQuickFixTable    //49222-MAMT-06/27/2000
                   ,ISortedSet<IString>* pFeature54Table  //53915-MAMT-08/31/2000
                   ,ISortedSet<IString>* pISeriesSWTable   //F72273 -HECC-06/28/2002
                   ,char * szCompiledFeatFile = 0
                   ,Boolean fForcePKRead = 0
                   ,Boolean bS90 = 0      //edw07/01/1998
                   ,Boolean bAS4 = 0 );   //Include bAS4 flag to reconcile iSeries SW.  HECC 72273 06/27/2002
                          
 Boolean resolve(const char * AASFile,const char * MRDBFile,
                 const char * MRDBDescFile, const char * MsgStringsFile);
 Boolean  reBuild( const char * szFileName);
 Boolean  writeReport(const char * szFileName,Boolean fAppendRPOs = false,Boolean bSetLockingFlag = false);
 Boolean  save(const char * szFileName);
 const IString &  currentSystem() { return CurrentSystem;}
};

#else

class EUOPResolve : public RMSystem
{

public:
  EUOPResolve() : RMSystem(){}
  ~EUOPResolve(){}

 Boolean resolve(const char * AASFile,const char * MRDBFile,
                 const char * MRDBDescFile, const char * MsgStringsFile);
 Boolean  reBuild( const char * szFileName);
 Boolean  writeReport(const char * szFileName);
 Boolean  save(const char * szFileName);
};

#endif

#pragma pack()
#endif
