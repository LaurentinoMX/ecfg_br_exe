//=======================================================================
//FILE NAME        AFCV2031.HPP
//
//DESCRIPTION      AFCV2031 reads a Version 20 CFREPORT file and creates
//                 a Version 31 equivalent file.
//=======================================================================
//
//Modifications:
//
//Date     Programmer  Rev    Description of Change
//----     ----------  -----  ---------------------
//12/15/97  E. Wilson   1.0    Creation of AFCV2031.HPP
//
//
//
//=======================================================================
#ifndef AFCV2031_HPP
#define AFCV2031_HPP

#include <iostream.h>
#include <fstream.h>
#include <istring.hpp>

class Convert2031
   {
   public:
   Convert2031();                                          // Constructor
   ~Convert2031();                                         // Destructor
   int convertCFReport( IString sFileName );               // Convert a V20 CFReport to a V31

   private:
   int iProdCnt;                                           // Product count
   int iLineCnt;                                           // Number of lines in created CFReport
   ifstream ifs;                                           // Input file stream
   ofstream ofs;                                           // Output file stream
   Boolean bRec03;                                         // The security record is written
   unsigned long int ulHwInitial;
   unsigned long int ulHwInstalled;
   unsigned long int ulHwProposed;
   unsigned long int ulHwOrder;
   unsigned long int ulSwInitial;
   unsigned long int ulSwInstalled;
   unsigned long int ulSwProposed;
   unsigned long int ulSwOrder;
   IString *pHwInitial;
   IString *pHwInstalled;
   IString *pHwProposed;
   IString *pHwOrder;
   IString *pSwInitial;
   IString *pSwInstalled;
   IString *pSwProposed;
   IString *pSwOrder;
   int allocateArrays( IString strInFileName );            // Allocate space for the HW/SW arrays
   void writeArrays( IString sProd, IString sProdInfo1,
     IString *pFeat, int iFeatCnt, IString sProdInfo2=" ");// Write processed records to output arrays
   void writeOutputFile();                                 // Write all data to the output file in the correct v31 order
   IString makeDesc(IString sDesc, IString sCnt);          // Make a product/feature description field
   IString makeProdInfoRec(IString sType, IString sModel,
                      IString sSerial, IString sProdCnt);  // Construct a V31 95 Record
   void createSecurityRec();                               // Write a security record if one is not in inputfile
   int processRec00( IString strRecord );                  // Process a V20 record 00
   int processHWRec( IString strRecord );                  // Process a V20 record 08, 25 ,or 26
   int processRec19( IString strRecord );                  // Process a V20 record 19
   int processRec17( IString strRecord );                  // Process a V20 record 17
   int processSWRec( IString strRecord );                  // Process a V20 record 35 or 36
   int processRec37( IString strRecord );                  // Process a V20 record 37
   };

#endif

