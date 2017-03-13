#include <winerror.H>
#include "AFCCPULS.HPP"

// ------------------------------------------------------------------------
// Class CpuList
// ------------------------------------------------------------------------

//==================================================================
// Function: CpuList::LoadCpuList()
//
// Reads the CPU list from "cpulook.tbl".
// Loads the list into ISortedSet <IString> myCpuSet.
// Returns:
//       NO_ERROR         Successful
//       IDS_ErrorOpenCpuList_Text     Unable to find/open the file
//       IDS_ErrorReadCpuList_Text     Error reading
//==================================================================
//edw06/29/1998 The CPULOOK.TBL file format has changed from "TTTT  MMM"
//              to "TTTT  MMM   PPPPPP" where PPPPPP is the ISM product group.
//              LoadCpuList() has changed to utilize the new field.


int CpuList::LoadCpuList( const IString& sExecPath )
{
   // Open the CPU list file
   ifstream  CpuFile( sExecPath + STR_CpuTable );
   if ( CpuFile.fail() )
      return IDS_ErrorOpenCpuList_Text;

   // Read each CPU Type-Model-Product and store it in myCpuSet
   IString  sTypeModel;
   myCpuSet.removeAll();
   while( 1 )
   {
      sTypeModel = IString::lineFrom( CpuFile );
      if ( CpuFile.eof() )
          return NO_ERROR;
      if ( CpuFile.fail() )
          return IDS_ErrorReadCpuList_Text;

      // convert format of type-model-product from "TTTT  MMM  PPPPPP" to "TTTT-MMM-PPPPPP"
      sTypeModel.change( "  ", STR_Hyphen );

      // Add to the set
      try  {
          myCpuSet.add(sTypeModel);
      }
      catch ( IOutOfMemory )
      {
          return IDS_ErrorOutOfMem_Text;
      }
   } // end .. while
}

// Returns TRUE if the type-model is in myCpuSet, FALSE otherwise
// The input sTypeModel must of the format "TTTT-MMM" where TTTT=Type
// and MMM=Model.

IBoolean CpuList::IsCpu( IString& sTypeModel )
{
   IString sSetItem;
   IEqualitySequence<IString>::Cursor crs( myCpuSet );    //edw02/11/99
   forCursor( crs )
      {
         sSetItem = myCpuSet.elementAt( crs );
         if ( sSetItem.indexOf( sTypeModel ) )
            return true;
      }

   return false;
}

//edw06/29/1998
//Returns the ISMProductModel for a given machine type.
IString CpuList::getISMProductModel( IString& sType )
{
   IString sSetItem;
   //edwISortedSet<IString>::Cursor crs( myCpuSet );
   IEqualitySequence<IString>::Cursor crs( myCpuSet );    //edw02/11/99
   forCursor( crs )
      {
      sSetItem = myCpuSet.elementAt( crs );
      if ( sSetItem.subString(1, 4) == sType )
         {
         sSetItem = sSetItem.remove( 5, 4 );
         if ( sSetItem.indexOf( STR_Hyphen ) )
            return ( sSetItem.subString(6) );
         else
            return ( STR_Blank1 );
         }
      }
   return ( STR_Blank1 );
}

IBoolean printItem(IString const& item, void *)
{
   cout << item << "\n";
   return True;
}

void CpuList::test()
{
//   myCpuSet.allElementsDo( printItem );
   cout << "Elm count = " << myCpuSet.numberOfElements() << endl;
   IString temp = myCpuSet.firstElement();   cout << temp << endl;
   temp = myCpuSet.lastElement();   cout << temp << endl;
}

/*
main()
{
   CpuList  clist;
   int iError = clist.LoadCpuList( IString("cpulook.tbl") );
   cout << "Error = " << iError << endl;

   IString  testval1("0709-001");
   if ( clist.IsCpu( testval1 ) )
         cout << testval1 << "is a CPU\n";

   testval1 = IString("0709-XX1");
   if ( ! clist.IsCpu( testval1 ) )
       cout << testval1 << "is NOT a CPU\n";

   clist.test();
}
*/
