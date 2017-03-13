#include "AF9APPLY.H"
#include "AFCINCLS.H"

//----------------------------------------------------------------------------
// Class CpuList:
//      Stores a list of all types-models which are CPU.  Provides a
//      method to check if a type-model exists in the list.
//----------------------------------------------------------------------------
// Modification Log:
// edw06/29/1998 - added enum type definition and usOption parameter to LoadCpuList()
// edw02/11/1999 - Changed myCpuSet attribute from type ISortedSet to IEqualitySequence
//                 because it is taking too long to load the CpuLookup table into a Set.
//                 Loading into a Sequence is much faster. Although the isCpu() method
//                 maybe a little slower.
// edw02/11/1999 - Remove enum type definition. For better performance, we will
//                 load the CpuLookUp table only one way.

#ifndef OA_CLASS_CPULIST
   #define OA_CLASS_CPULIST

#include <ies.h>  //edw02/11/99

class CpuList
{
   public:
      int                  LoadCpuList( const IString& sExecPath );
      IBoolean             IsCpu( IString& sTypeModel );
      IString              getISMProductModel( IString& sType );

      void                 test();

   private:
      //edwISortedSet<IString>  myCpuSet;
      IEqualitySequence<IString> myCpuSet;    //edw02/11/99
};
#endif
