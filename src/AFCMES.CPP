#include "AFCPRODS.HPP"

// MES.HPP - Implementation of class Mes
//----------------------------------------------------------------------------
// Mes
//      Stores the data from records 19, 50, and 95 of CfReport (V31).  Represents
//      MES/Upgrade Hardware or Software.
// Note:  The description and other values will be obtained
// from 2nd 95 record if this is an upgrade.
//----------------------------------------------------------------------------

// Constructor
Mes::Mes()
{
   uSequence               = 0;
//   uCountMesSteps          = 0;
//   bHasSequence            = False;
   uOffsetBaseProductRec   = 0;
   pNextMes = (Mes *) NULL;
};

//---------------------------------------------------------------------------
// Function: void Mes::ParseLoad()
//
// Parses a Hardware/Software MES/Upgrade record.  This function is called by BackEnd::ParseCFREPORT()
// and BackEnd::LoadCFREPORT().
// Calls ProcessProductIDRec() to process the ProductID(95) record
// *** It is assumed that this function receives 2nd ProductID(95) record if
//     it is Hardware Model upgrade.
// Input:
//                Action Type
//                    'L'(oad)  = extract all details (when loading a file)
//                    'P'(arse) = extract minimal details (when parsing a raw file)
//                    'C'(onvert) = the CFREPORT has been processed by the previous version
//                                  of Order Apply and this CFREPORT's control records (AA, AB, AC)
//                                  are being converted to the format expected by this version
//                Reference to Proudct Record
//                Reference to ProductID record
//                Byte offset of the record in the file
//                Reference to the CPU list
// Assigns the extracted values to the appropriate members of Mes
//----------------------------------------------------------------------------------
void Mes::ParseLoad (char               cActionType,
                     IString&           sProductRec,
                     IString&           sProductIdRec,
                     unsigned long      uProductRecOffset,
                     unsigned long      uProductIdRecOffset,
                     CpuList&           myCpuList)
{
   uOffsetToProductRecord = uProductRecOffset;
   uOffsetToProductIdRecord = uProductIdRecOffset;

   sOriginalRecordType = sProductRec.subString( 1,2 );
   IString       sType, sModel, sTargetType, sTargetModel;
   if ( sOriginalRecordType == IString(STR_50RecType) )  // SW
      sProductRec >> 2 >> sType >> 4 >> sModel >> 3 >> sProductActionCode >> 1;
   else                                                  // HW
   {
      sProductRec >> 2 >> sType >> 4 >> sModel >> 3 >>
          sTargetType >> 4 >> sTargetModel >> 3 >> sProductActionCode >> 1;
      // If 'To Product Type' and 'To Product Model' are not blank, then this is an upgrade
      if ( !sTargetType.isWhiteSpace() && !sTargetModel.isWhiteSpace() )
          sTargetTypeModel = sTargetType + STR_Hyphen + sTargetModel;
   }
   sBaseTypeModel = sType + STR_Hyphen + sModel;

   // Extract data from ProductID record (95 record)
   ProcessProductIDRec( sProductIdRec, cActionType );

   // If Parsing a new file
   if ( cActionType == 'P' )
   {
      // Set the category
      if ( sOriginalRecordType == IString(STR_50RecType) )
      {
         eProdType = SW;
         sProdType = IString( "SW" );
      }
      else
      {
         eProdType = HW;
         sProdType = IString( "HW" );
      }

      // Set status to APPLIED
      setApplied( True );
   }

   // Calculate the number of Features, Load the Features if 'L'oad
   IString       sFeatures;
   if ( category() == SW )
         sProductRec >> 10 >> sFeatures;
   else
         sProductRec >> 17 >> sFeatures;
   uCountFeatures = sFeatures.size() / 13;
   if ( cActionType == 'L' )
      CreateFeatures( sFeatures, 13 );

};

//----------------------------------------------------------------------------------
// Function: void Mes::ProcessProductIDRec
// Extracts Mes specific fields of a ProductId record.  Calls AbstractProduct::ProcessProductIDRec().
//
//----------------------------------------------------------------------------------
void Mes::ProcessProductIDRec( IString&           sProductIdRec,
                               char               cActionType)
{
   AbstractProduct::ProcessProductIDRec( sProductIdRec, cActionType );

   sProductIdRec >> 28 >> sMesOrderNumber >> 9;
   sMesOrderNumber.strip();

   // Extract the sequence number
   IString    sMesSequence;
   sProductIdRec >> 99 >> sMesSequence >> 3;
   sMesSequence.strip();
   if ( sMesSequence.size() > 0 )
      uSequence = sMesSequence.asUnsigned();
}

//----------------------------------------------------------------------------------
// Function: IsUpgrade()
// Returns True if this Mes is an Upgrade; returns False otherwise.

IBoolean Mes::IsUpgrade()
{
   if ( sTargetTypeModel.size() > 0 )
      return True;
   else
      return False;
}

//----------------------------------------------------------------------------------
// Function Mes::CreateAbRecord().
//
// Creates an AB record which can be written to the output file.  Calls
// AbstractProduct::CreateAbRecord() to insert the fields which are commmon
// to Product and Mes.
//
// Following is the format of AB record
// ProdInfo: (AB)
//      1     RecordType               2      Record Type,  const "AB"
//      2     SystemID                 4      SystemID
//      3     Status                  20      INSTALLED, APPLIED, DELETED
//      4     CpuFlag                  1      1,0
//      5     HwSw                     2      HW, SW
//      6     CurrentTypeModel         8      For HW Upgrades
//      7     FlagHasMes               1      Flag = 1 if has MESes, 0 otherwise
//      8     BaseProductOffset        8      File offset to the base product if this is an MES/Upgrade
//      9     Blanks                  34
// This function inserts the value of field 8 listed above.
//-----------------------------------------------------------------------------------
void Mes::CreateAbRecord( IString& sAbRecord )
{
   AbstractProduct::CreateAbRecord( sAbRecord );

   sAbRecord.overlayWith( IString(uOffsetBaseProductRec), 39 );
};

//----------------------------------------------------------------------------------
void Mes::ParseAbRecord( IString& sAbRecord )
{
   // Extract fields common between Product and Mes
   AbstractProduct::ParseAbRecord( sAbRecord );

   // Extract BaseProductOffset
   IString       sOffsetBaseProductRec;
   sAbRecord << 39 >> sOffsetBaseProductRec >> 8;
   uOffsetBaseProductRec = sOffsetBaseProductRec.asUnsigned();
};

//----------------------------------------------------------------------------------
void Mes::test()
{
    AbstractProduct::test();
    cout << "==============================================================\n";
    cout << "  sBaseTypeModel          =" <<  sBaseTypeModel          << "=\n";
    cout << "  sTargetTypeModel        =" <<  sTargetTypeModel        << "=\n";
    cout << "  sMesOrderNumber         =" <<  sMesOrderNumber         << "=\n";
    cout << "  sProductActionCode      =" <<  sProductActionCode      << "=\n";
    cout << "  uSequence               =" <<  uSequence               << "=\n";
//    cout << "  uCountMesSteps          =" <<  uCountMesSteps          << "=\n";
//    cout << "  bHasSequence            =" <<  bHasSequence            << "=\n";
    cout << "  uOffsetBaseProductRec   =" <<  uOffsetBaseProductRec   << "=\n";

    IString   sAbRecord;
    CreateAbRecord( sAbRecord );
    cout << "AB Record =" << sAbRecord << "=" << endl;
}

//=======================================================================
// Various set and get methods
//=======================================================================

IString Mes::baseProduct()    { return sBaseTypeModel;   }
IString Mes::serial()         { return sOrderSerial;     }
IString Mes::order()          { return sMesOrderNumber;  }
IString Mes::targetProduct()  { return sTargetTypeModel; }
FeatureCollection& Mes::FeatureSet()
{  return myFeatureSet;  }


//=======================================================================
// Offsets for container columns
//=======================================================================

unsigned long Mes :: offsetApplied()
   { return offsetof(Mes, strApplied); }
unsigned long Mes :: offsetBaseProduct()
   { return offsetof(Mes, sBaseTypeModel); }
unsigned long Mes :: offsetSerial()
   { return offsetof(Mes, sOrderSerial); }
unsigned long Mes :: offsetTargetProduct()
   { return offsetof(Mes, sTargetTypeModel); }
unsigned long Mes :: offsetOrder()
   { return offsetof(Mes, sMesOrderNumber); }
unsigned long Mes :: offsetDate()
   { return offsetof(Mes, strCrad); }
unsigned long Mes :: offsetGroupSequence()
   { return offsetof(Mes, sMesGroupSequence); }
