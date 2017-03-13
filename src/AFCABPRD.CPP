 #include "AFCPRODS.HPP"

// File AFCABPRD.CPP
// Implementation of class AbstractProduct
//----------------------------------------------------------------------------
// AbstractProduct:
//      An abstract class representing a hardware or software which can
//      be installed, initial order, MES, or Upgrade.
//----------------------------------------------------------------------------

unsigned AbstractProduct::uLengthAbRec = 80 + sizeof("\n");

AbstractProduct::AbstractProduct()
{
    uOffsetToProductRecord = 0;
    uCountFeatures = -1;
    setApplied( False );
    setDetail( (DetailObject *) NULL );
};

//----------------------------------------------------------------------------------
// Function: void AbstractProduct::ProcessProductIDRec
//
// Parses a ProductID (95) record.  This function is called by Product or Mes::ParseLoad()
// Input:         Reference to ProductID record
//                Action Type
//                    'L'(oad)  = extract all details (when loading a file)
//                    'P'(arse) = extract minimal details (when parsing a raw file)
//                    'C'(onvert) = the CFREPORT has been processed by the previous version
//                                  of Base Retrieval and this CFREPORT's control records (AA, AB, AC)
//                                  are being converted to the format expected by this version
// Assigns the extracted values to the appropriate members of Product or Mes
//----------------------------------------------------------------------------------

void AbstractProduct::ProcessProductIDRec( IString&           sProductIdRec,
                                           char               cActionType)
{
    unsigned int  uPositionOrderSerial;

    // Determine whether to extract the serial number or order number
    if ( sOriginalRecordType == IString(STR_25RecType) || sOriginalRecordType == IString(STR_49RecType) // Installed Product
         || sOriginalRecordType == IString(STR_08RecType) || sOriginalRecordType == IString(STR_47RecType) //IC66572 MAMT 10/11/2000
         || sOriginalRecordType == IString(STR_19RecType) || sOriginalRecordType == IString(STR_50RecType) ) // If Mes/Upgrade, extract base Product serial
         uPositionOrderSerial = 20;
    else                             // Initial Order Product
         uPositionOrderSerial = 29;

    // Extract various fields and strip leading/trailing blanks
    sProductIdRec << uPositionOrderSerial >> sOrderSerial >> 9
      << 51 >> sSystemTypeNumber >> 11
      << 63 >> sSwDesignatedCpuTypeSerial >> 13
      >> sCustNum >> 7;

    sSystemTypeNumber.insert( IString(STR_Hyphen), 4 );
    sOrderSerial.strip();
    sSwDesignatedCpuTypeSerial.strip();
    if ( sSwDesignatedCpuTypeSerial.size() > 0 )
       sSwDesignatedCpuTypeSerial.insert( IString(STR_Hyphen), 4 );

    // If parsing a new file, no need to load the other details
    if ( cActionType == 'P' )
         return;

    // Extract more details
    IString    sCradYear;
    IString    sCradMonth;
    IString    sCradDay;
    IString    sProductStatus;

    sProductIdRec << 83
                  >> sCradYear >> 4 >> sCradMonth >> 2 >> sCradDay >> 2
         << 99  >> sProductStatus >> 1
         << 143 >> sDescription;

    switch( sProductStatus[1] )
    {
      case 'I':
            eProductStatus = INSTALLED;
            break;
      case 'U':
            eProductStatus = UNINSTALLED;
            break;
      case 'O':
            eProductStatus = ONORDER;
            break;
      case 'S':
            eProductStatus = SHIPPED;
            break;
      case ' ':
            eProductStatus = NEWLYCONFIGURED;
            break;
    }

    // Merge YYYY, MM, and DD into IDate
    //19970530 edw fix to prevent bad crad date when crad date is empty
    if ( sCradMonth.isDigits() && sCradDay.isDigits() && sCradYear.isDigits() )
       {
       IDate tempCRAD = IDate( (IDate::Month) sCradMonth.asInt(),
                                           sCradDay.asInt(),
                                           sCradYear.asInt() );
       setCrad( tempCRAD );
       }
};

//----------------------------------------------------------------------------------
// Function AbstractProduct::CreateFeatures()
//
// Receives a Feature/Quantity/Action string of and creates Features
// for each Feature/Quantity/Action.  Action is available only for
// Mes/Upgrade
//    Input:        Reference to sFeatures
//                        (e.g., Columns 11 -- end of string of record 50)
//                  integer value containing the size of each Feature/Quantity/Action.
//                        (Values are 12 = Product, 13 = Mes)
//----------------------------------------------------------------------------------

void AbstractProduct::CreateFeatures( IString&            sFeatures,
                                      const unsigned int  uSizeFetActQty )
{
   int  iFeatureQty = sFeatures.size() / uSizeFetActQty;

   IString       sFetQtyAct;
   for ( int i = 0; i < iFeatureQty; i++ )
   {
        sFetQtyAct = sFeatures.subString( i*uSizeFetActQty + 1, uSizeFetActQty );
        Feature    myFeature( sFetQtyAct );
        myFeatureSet.add(myFeature);
   }
};

//----------------------------------------------------------------------------------
// Function: void AbstractProduct::AddFeatureDescription()
//
// Extracts the description from the Feature record (96) and assigns it to the
// Feature.
//----------------------------------------------------------------------------------
void AbstractProduct::AddFeatureDescription( IString&       sFeatureRec,
                                             unsigned long  uOffsetFeatureRec )
{
   IString       sFeatureNumber;

   // Extract the Feature/RPQ/PART number
   sFeatureRec >> 2 >> sFeatureNumber >> 7;
   sFeatureNumber.strip();

   // Changed by GARGJ on 12/09/96.  An MES can have duplicate features.  Therefore,
   // add the description to all the Features with the same Feature/RPQ/PART number.
   FeatureCollection::Cursor FeatureCrs( myFeatureSet );

   for( myFeatureSet.locateElementWithKey( sFeatureNumber, FeatureCrs );
        FeatureCrs.isValid();
        myFeatureSet.locateNextElementWithKey( sFeatureNumber, FeatureCrs )
      )
   {
      Feature* pFeature = (Feature*)&FeatureCrs.element();
      sFeatureRec >> 50 >> pFeature->sDescription;
      pFeature->uOffset = uOffsetFeatureRec;
   }

   // Find the Feature and add the description
   //   Feature& aFeature = myFeatureSet.elementWithKey( sFeatureNumber );
   //   sFeatureRec >> 50 >> aFeature.sDescription;
   //   aFeature.uOffset = uOffsetFeatureRec;
};

//----------------------------------------------------------------------------------
// Function: AbstractProduct::CreateAbRecord()
//
// Creates AB record from the data contained in Product or Mes.
// The AB record created can be written to the output file when:
//    1. Parsing a raw file (BackEnd::ParseCFREPORT())
//    2. Saving a file (BackEnd::SaveCFREPORT())
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
//                                   -----
//                                    80
//                                   -----

// *** This function creates the record consisting of only the fields 1, 3, and 5
//     listed above. It inserts blanks in the remaining fields.  The remaining fields
//     are inserted by Product::CreateAbRecord() or Mes::CreateAbRecord()
//----------------------------------------------------------------------------------

void AbstractProduct::CreateAbRecord( IString& sAbRecord )
{
   // Insert values of fields 1, 3, and 5
   sAbRecord =      STR_AbRecType
                +   IString::copy( " ", 4 )
                +   sCurrentStatus + IString::copy( " ", 20 - sCurrentStatus.size() )
                +   IString(" ")
                +   sProdType;

   // Pad with blanks
   sAbRecord.leftJustify( 80 );
};

//----------------------------------------------------------------------------------
void AbstractProduct::ParseAbRecord( IString& sAbRecord )
{
   // Extract Status and category
   sAbRecord >> 6 >> sCurrentStatus >> 20 >> 1 >> sProdType >> 2;
   sCurrentStatus.strip();
   if ( sCurrentStatus == IString("APPLIED") )
      setApplied( True );

   // F86750 Begin logic: Adds the Services category
   // Set the category
   //* ( sProdType[1] == 'H' ) ? eProdType = HW : eProdType = SW;

   if (sProdType == "HW")
      eProdType = HW;
   else if (sProdType == "SW")
      eProdType = SW;
   else if (sProdType == "SV")
      eProdType = SV;

   // F86750 End logic:  HECC - 01/15/2004

};

//===================================================================
// Function: Product::ConvertAbRecord()
// Converts AB record's format from old to new if the file was processed by
// previous version of Base Retrieval.  The following are the formats
// of AB records.
//
// OLD--------
//   recordType       2      Record Type,  const "AB"
//   SystemKey        4      index in sSystems.
//   status           20     INSTALLED, APPLIED, DELETED
//   cpu              1      1,0
//   HwSw             2      HW, SW
//   InitStatus       20     INSTALLED, INITIAL, MES
//
// NEW--------
//      1     RecordType               2      Record Type,  const "AB"
//      2     SystemID                 4      SystemID
//      3     Status                  20      INSTALLED, APPLIED, DELETED
//      4     CpuFlag                  1      1,0
//      5     HwSw                     2      HW, SW
//      6     CurrentTypeModel         8      For HW Upgrades
//      7     FlagHasMes               1      Flag = 1 if has MESes, 0 otherwise
//      8     BaseProductOffset        8      File offset to the base product if this is an MES/Upgrade
//      9     Blanks                  34
//                                   -----
//                                    80
//                                   -----
//===================================================================

void AbstractProduct::ConvertAbRecord( IString& sOldAbRecord, IString& sNewAbRecord )
{
   // Copy the first 5 fields from old to new AB record
   sNewAbRecord = IString::copy( " ", 80 );
   sNewAbRecord.overlayWith( sOldAbRecord.subString(1,29) );
}

//----------------------------------------------------------------------------------
IBoolean printFet(Feature const& item, void *)
{
   cout.fill('-');
   cout <<  item.sNumber <<  setw(4)  << item.iQuantity;

   cout <<  setw(40) << item.sDescription;
   cout <<  setw(5)  << item.eAction << setw(5) << endl;

   return True;
};

//----------------------------------------------------------------------------------
void AbstractProduct::test()
{
cout << "sOriginalRecordType      =" <<  sOriginalRecordType       << "=\n";
cout << "uOffsetToProductRecord   =" <<  uOffsetToProductRecord    << "=\n";
cout << "sCurrentStatus           =" <<  sCurrentStatus            << "=\n";
cout << "eProdType                =" <<  eProdType                 << "=\n";
cout << "sProdType                =" <<  sProdType                 << "=\n";
cout << "sOrderSerial             =" <<  sOrderSerial              << "=\n";
cout << "sSwDesignatedCpuTypeSerial=" << sSwDesignatedCpuTypeSerial<< "=\n";
cout << "sDescription             =" <<  sDescription              << "=\n";
cout << "sSystemTypeNumber        =" <<  sSystemTypeNumber         << "=\n";
cout << "sCustNum                 =" <<  sCustNum                  << "=\n";
cout << "crad                     =" <<  crad.asString("%Y-%m-%d") << "=\n";
cout << "# of Features            =" <<  myFeatureSet.numberOfElements() << "=\n";
cout << "************************************\n";
myFeatureSet.allElementsDo(printFet);
cout << "************************************\n";
};

//=======================================================================
// set and get methods
//=======================================================================

void AbstractProduct::setApplied( IBoolean b )
{
   bApplied = b;
   if ( bApplied )
   {
      sCurrentStatus = IString("APPLIED");
#ifndef GARGJ_TEST
      strApplied = LOADSTRING(IDS_Applied);
#endif
   }
   else
   {
      sCurrentStatus = IString("DELETED");
      strApplied = "";
   }
}

void AbstractProduct::setCrad( const IDate& craDate )
{
   crad = craDate;
   strCrad = crad.asString(STR_FormatDate);
}
void AbstractProduct::setDetail( DetailObject * pDetailObj)
{
   pDetail = pDetailObj;
}

DetailObject*  AbstractProduct::detail()           { return pDetail;           }
IString        AbstractProduct::description()      { return sDescription;      }
IBoolean       AbstractProduct::applied()          { return bApplied;          }
IDate          AbstractProduct::date()             { return crad;              }
int            AbstractProduct::category()         { return eProdType;         }
IString        AbstractProduct::systemTypeNumber() { return sSystemTypeNumber; }
unsigned int   AbstractProduct::status()           { return eProductStatus;    }
IString        AbstractProduct::customerNumber()   { return sCustNum;          }

//=======================================================================
// Reformat values when switching locale
//=======================================================================

void AbstractProduct :: format()
{
   if ( bApplied )
#ifndef GARGJ_TEST
      strApplied = LOADSTRING(IDS_Applied);
#endif

   strCrad = crad.asString(STR_FormatDate);
}
