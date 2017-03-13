#include <ISet.h>
#include <IString.hpp>
#include <istparse.hpp>

//----------------------------------------------------------------------------
// System
//      Used only by BackEnd::ParseCFREPORT()
//----------------------------------------------------------------------------

#ifndef OA_CLASS_SYSTEM
   #define OA_CLASS_SYSTEM

class System
{
 public:
// Methods
   System ( unsigned int uNewSystemId );
   System ( unsigned int uSysId,
            IString      sTypeSerial,
            IString      sOrigTypeSerial,
            IString      sSysTypeNum,
            IString      sCustNum );

   void            ParseAcRecord( IString& sAcRecord );
   IString         CreateAcRecord();
   static IString  ConvertAcRecord( IString& sOldAcRecord, unsigned int iSystemId );

// Attributes
   IBoolean        bHasCpu;   /* TRUE if it contains a CPU, FALSE otherwise*/
   IString         sSystemName;
   IString         sCpuTypeSerial;
   IString         sOrigCpuTypeSerial;
   IString         sSystemTypeNumber;
   IString         sCustomerNumber;
   unsigned int    uSysProdCount;
   unsigned int    uSystemId;
//    (Numeric ID for the system.  It's value is index in the list of Systems)

  // required for Key Sorted Set key() function below
  unsigned int const&   SystemNumber() const
      {  return uSystemId; }
};

// -----------------------------------------------------
// These functions are required by Key Sorted Set
inline unsigned int const&      key( System const& aSystem )   // Key access
{   return aSystem.SystemNumber();  }
inline long compare( System const& Sys1, System const& Sys2 )  // Key ordering
{  return compare( Sys1.SystemNumber(), Sys2.SystemNumber() );  }


#endif

