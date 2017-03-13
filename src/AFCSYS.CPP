#include "AFCINCLS.H"
#include "AFCSYS.HPP"

// Implementation of class System

// Constructors
System::System ( unsigned int uNewSystemId )
{
  uSystemId     = uNewSystemId;
  uSysProdCount = 0;
  bHasCpu       = False;
};

System::System ( unsigned int uSysId,
                 IString      sTypeSerial,
                 IString      sOrigTypeSerial,
                 IString      sSysTypeNum,
                 IString      sCustNum )
{
   bHasCpu               = False;
   sCpuTypeSerial        = sTypeSerial;
   sOrigCpuTypeSerial    = sOrigTypeSerial;
   sSystemTypeNumber     = sSysTypeNum;
   sCustomerNumber       = sCustNum;
   uSysProdCount         = 0;
   uSystemId             = uSysId;
};


//=================================================================================
// NEW AC record format
//    recordType        2      Record Type, const "AC"
//    systemID          6
//    Productcount      6
//    SystemName       40
//    Blanks           26
//                    ----
//     TOTAL        =  80
//=================================================================================

//=================================================================================
// Function: ParseAcRecord()
// Parses an AC record.

void System::ParseAcRecord( IString& sAcRecord )
{
   IString       sSystemId, sProductCount;
   sAcRecord >> 2 >>
       sSystemId >> 6 >>
       sProductCount >> 6 >>
       sSystemName >> 40;

   uSystemId = sSystemId.asUnsigned();
   uSysProdCount = sProductCount.asUnsigned();
   sSystemName.strip();
};

//=================================================================================
// Function: System::CreateAcRecord()
// Creates a new AC system record which can be written to the output file.
//
//=================================================================================
IString System::CreateAcRecord()
{
   IString  sNewAcRecord = IString("AC") + IString::copy( " ", 78 );
   sNewAcRecord.overlayWith( IString(uSystemId), 3 );
   sNewAcRecord.overlayWith( IString(uSysProdCount), 9 );

   if ( sSystemName.size() > 40 )
      sNewAcRecord.overlayWith( sSystemName.subString(1, 40), 15 );
   else
      sNewAcRecord.overlayWith( sSystemName, 15 );

   return sNewAcRecord;
};


//=================================================================================
// Function: System::ConvertAcRecord()
// Receives an old format AC record and the systemID; creates a new AC system record
// which can be written to the output file.
//
// OLD AC record format
//    recordType      2      Record Type,  const "AC"
//    Key             4      index For product line in V31 input
//    ProductKey      4      index For line in product array.
//    Mes             4      count of Mes products
//    New             4      count of Mes products
//    Installed       4      count of INSTALLED products
//    Orders          4      count of orders (INIITIAL & Mes) pro
//    AppliedNew      4      count of APPLIED INIITIAL order prod
//    RemovedNew      4      count of APPLIED INIITIAL order prod
//    AppliedMes      4      count of APPLIED Mes products
//    RemovedMes      4      count of APPLIED Mes products
//    Name            22     System name
//    GenName         n      file name To generate as
//
//=================================================================================

IString System::ConvertAcRecord( IString& sOldAcRecord, unsigned int uInputSystemId )
{
   // Extract system name
   IString  sOldRecSystemName;
   sOldAcRecord >> 42 >> sOldRecSystemName >> 22;

   // Create new AC record
   IString  sNewAcRecord = IString("AC") + IString::copy( " ", 78 );
   sNewAcRecord.overlayWith( IString(uInputSystemId), 3 );
   sNewAcRecord.overlayWith( sOldRecSystemName, 15 );

   return sNewAcRecord;
};
