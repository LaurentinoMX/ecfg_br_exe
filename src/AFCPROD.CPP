#include "AFCPRODS.HPP"

// Implementation of classes Product and MesSortedArray
//----------------------------------------------------------------------------
// Product:
//      Stores the data from Installed Hardware/Software
//      (records 25, 49, and 95) or Initial Order Hardware/
//      Software (records 08, 47, and 95) of CfReport (V31).
//----------------------------------------------------------------------------

// Constructor.  Initializes members.
Product::Product()
{
    uCurrentSystemId = UINT_MAX;
    bHasMes = False;
    bHasAppliedMes = False;
    uSeqLastAppliedMes = 0;
    uSeqFirstDeletedMes = UINT_MAX;
};

//----------------------------------------------------------------------------
// Function: void Product::ParseLoad()
//
// Parses a Product record.  This function is called by BackEnd::ParseCFREPORT()
// and BackEnd::LoadCFREPORT().
// Calls ProcessProductIDRec() to process the ProductID(95) record
// Input:
//                Action Type
//                    'L'(oad)  = extract all details (when loading a file)
//                    'P'(arse) = extract minimal details (when parsing a raw file)
//                    'C'(onvert) = the CFREPORT has been processed by the previous version
//                                  of Order Apply and this CFREPORT's control records (AA, AB, AC)
//                                  are being converted to the format expected by this version
//                Reference to Product Record
//                Reference to ProductID record
//                Byte offset of the record in the file
//                Reference to the CPU list
// Assigns the extracted values to the appropriate members of Product
//----------------------------------------------------------------------------------
void Product::ParseLoad (   char               cActionType,
                            IString&           sProductRec,
                            IString&           sProductIdRec,
                            unsigned long      uProductRecOffset,
                            unsigned long      uProductIdRecOffset,
                            CpuList&           myCpuList
                        )
{
    uOffsetToProductRecord = uProductRecOffset;
    uOffsetToProductIdRecord = uProductIdRecOffset;

    IString       sType, sModel;

    sOriginalRecordType = sProductRec.subString( 1,2 );
    // Parse Installed or Initial Order Product Record
    if ( sOriginalRecordType == IString(STR_47RecType) || sOriginalRecordType == IString(STR_49RecType) )
       sProductRec >> 2 >> sType >> 4 >> sModel >> 3 >> sQuantity >> 5;      // SW
    else
       /* F86750 Begin logic: Assigns service Type-Model to the product object */
       if ( sOriginalRecordType == IString(STR_38RecType) )
          sProductRec >> 2 >> sType >> 4 >> sModel >> 3 >> 1 >> sQuantity >> 5; // SV
       else
       /* F86750 Ends logic:  HECC - 01/15/2004 */
          sProductRec >> 2 >> sType >> 4 >> 1 >> sModel >> 3 >> sQuantity >> 5; // HW

    sQuantity.strip();
    sOriginalTypeModel = sType + STR_Hyphen + sModel;
    if ( 0 == sCurrentTypeModel.size() )   // Make the sCurrentTypeModel and sOriginalTypeModel same
       sCurrentTypeModel = sOriginalTypeModel;

    // Determine if it is an Installed Product
    if ( sOriginalRecordType == IString(STR_25RecType) || sOriginalRecordType == IString(STR_49RecType) )
       sCurrentStatus = IString("INSTALLED");

    // Extract data from ProductID record (95 record)
    ProcessProductIDRec( sProductIdRec, cActionType );

    // If Parsing a new file
    if ( cActionType == 'P' )
    {
       // Assign the category
       if ( sOriginalRecordType == IString(STR_47RecType) || sOriginalRecordType == IString(STR_49RecType) )
       {
          eProdType = SW;
          sProdType = IString( "SW" );
       }
       else

       /* F86750 Begin logic: Add the services tag to the product object */
       if ( sOriginalRecordType == IString(STR_38RecType) )
       {
          eProdType = SV;
          sProdType = IString( "SV" );
       }
       else
       /* F86750 Ends logic:  HECC - 01/15/2004 */

       {
          eProdType = HW;
          sProdType = IString( "HW" );
       }

       // Find out if the product is CPU.
       if ( True == myCpuList.IsCpu(sOriginalTypeModel) )
          eProdType = CPU;

       // Assign it to System 1
       uCurrentSystemId = 1;

       // If it is not an Installed Order Product, make the status APPLIED
       if ( sCurrentStatus != IString("INSTALLED") )
          setApplied( True );
    }

    // Count the number of features and load them if 'L'oad
    IString       sFeatures;
    if ( category() == SW )
         sProductRec >> 14 >> sFeatures;
    else

    /* F86750 Begin logic: Extract the features string from the correct position for a services record.*/
       if ( category() == SV )
         sProductRec >> 35 >> sFeatures;
       else
    /* F86750 End logic:  HECC - 01/14/2004   */
         sProductRec >> 15 >> sFeatures;

    /* F86750 Begin logic: If we have a services product divide by 13 because of the 'Action' extra character */
    if ( category() == SV )
       uCountFeatures = sFeatures.size() / 13;
    else
    /* F86750 End logic:  HECC - 01/14/2004   */
       uCountFeatures = sFeatures.size() / 12;

    if ( cActionType == 'L' )
    {
       /* F86750 Begin new logic: If we have a services product the feature string length is 13 */
       if ( category() == SV )
          CreateFeatures( sFeatures, 13 );
       else
       /* F86750 End new logic:  HECC - 01/14/2004   */

       CreateFeatures( sFeatures, 12 );

       if ( category() == CPU )
          setIcon(IResourceId(IDI_Processor));
    }
};

//----------------------------------------------------------------------------------
// Function: void Product::CreateAbRecord()
//
// Creates an AB record which can be written to the output file.  Calls
// AbstractProduct::CreateAbRecord() to insert the fields which are commmon
// to Product and Mes.
// Following is the format of AB record
// ProdInfo: (AB)
//      1     RecordType               2      Record Type,  const "AB"
//      2     SystemID                 4      SystemID
//      3     Status                  20      INSTALLED, APPLIED, DELETED
//      4     CpuFlag                  1      1,0
//      5     HwSw                     2      HW, SW
//      6     CurrentTypeModel         8      For HW Upgrades (format: type-model)
//      7     FlagHasMes               1      Flag = 1 if has MESes, 0 otherwise
//      8     BaseProductOffset        8      File offset to the base product if this is an MES/Upgrade
//      9     Blanks                  34
// This function inserts the values of fields 2, 4, 6, and 7 listed above.
//----------------------------------------------------------------------------------
void Product::CreateAbRecord( IString& sAbRecord )
{
   // Call the base class function for values of fields 1, 3, and 5
   AbstractProduct::CreateAbRecord( sAbRecord );

   // Insert the value of field 2, 4, 6, and 7
   sAbRecord.overlayWith( IString(uCurrentSystemId), 3 );

   ( category() == CPU ) ? sAbRecord[27] = '1' : sAbRecord[27] = '0';

   sAbRecord.overlayWith( sCurrentTypeModel, 30 );

   ( bHasMes == True ) ? sAbRecord[38] = '1' : sAbRecord[38] = '0';
};

//----------------------------------------------------------------------------------
void Product::ParseAbRecord( IString& sAbRecord )
{
   // Extract fields common between Product and Mes
   AbstractProduct::ParseAbRecord( sAbRecord );

   // Extract SystemID, CpuFlag, CurrentTypeModel, and FlagHasMes
   IString          sSystemId;
   sAbRecord >> 2 >>
      sSystemId >> 4
      << 30 >>
      sCurrentTypeModel >> 8;

   sCurrentTypeModel.strip();
   uCurrentSystemId = sSystemId.asUnsigned();
   if ( sAbRecord[27] == '1' )
      eProdType = CPU;

   ( sAbRecord[38] == '1' ) ? bHasMes = True : bHasMes = False;
};


//=================================================================================
// Function: ConvertAbRecord()
// See description of AbstractProduct::ConvertAbRecord() for details.
//=================================================================================
void Product::ConvertAbRecord( IString& sOldAbRecord, IString& sNewAbRecord )
{
   AbstractProduct::ConvertAbRecord( sOldAbRecord, sNewAbRecord );

   // subtract 1 from SystemId and overwrite it
   IString        sSystemId;
   sNewAbRecord >> 2 >> sSystemId >> 4;
   sSystemId = IString( sSystemId.asUnsigned() - 1 );
   sSystemId.leftJustify( 4 );
   sNewAbRecord.overlayWith( sSystemId, 3 );
}

//=================================================================================
// Function: IBoolean DoesMesMatch( Mes & )
//
//=================================================================================
IBoolean Product::DoesMesMatch( Mes& anMes )
{
   if ( sSystemTypeNumber == anMes.sSystemTypeNumber && sOrderSerial == anMes.sOrderSerial )
      return True;
   else
      return False;
}

//=================================================================================
// Function: Product::AssociateMes( Mes *pMes )
// Steps:
// 1. Assigns a sequence number to Mes if it does not have it (last Mes sequence + 1)
// 2. Add the pointer to the Mes to the Product's MesPointer container
//
//=================================================================================
void Product::AssociateMes( Mes *pMes )
{
   bHasMes = True;

   // Assign a sequence number to the Mes if not already assigned
   if ( pMes->uSequence == 0 )
      pMes->uSequence = myMesList.numberOfElements() + 1;

   // Set the base product offset in Mes
   pMes->uOffsetBaseProductRec = uOffsetToProductRecord;

   // Set the original type-model in the Mes
   pMes->sOriginalTypeModel = sOriginalTypeModel;

   // Add the pointer to Mes to the container
   myMesList.add( pMes );
   UpdateDetails();
};


//======================================================================================
// Function: Product::UpdateDetails()
// Updates the values of following fields if the Product has "APPLIED Upgrades".
//      sCurrentTypeModel
//      sLastAppMesOrderNo  (Order number of the last applied Mes)
//
// Also sets the value of bHasAppliedMes.
//======================================================================================

void Product::UpdateDetails()
{
    // Initialize
    sCurrentTypeModel       = sOriginalTypeModel;
    sLastAppMesOrderNo      = NULL;
    bHasAppliedMes          = False;

    // Iterate the Mes list
#ifdef GARGJ_TEST
 cout << "=================================\n";
#endif

    Mes *pMes = myMesList.pFirstMes;
    while ( pMes )
    {
      if ( pMes->applied() )
      {
         bHasAppliedMes = True;
         sLastAppMesOrderNo = pMes->sMesOrderNumber;
         if ( pMes->IsUpgrade() )
            sCurrentTypeModel = pMes->sTargetTypeModel;
      }
      pMes = pMes->pNextMes;

#ifdef GARGJ_TEST
      IString sTemp = IString("BaseTypeModel =")+pMes->sBaseTypeModel+IString("=\n");
      cout << sTemp;
      sTemp =    IString("TargetTypeModel =")+IString(pMes->sTargetTypeModel)+IString("=\n");
      cout << sTemp;
      sTemp = IString("Sequence = ") + IString(pMes->uSequence) + IString("=\n");
      cout << sTemp;
      cout.flush();
#endif

    }
};

//=================================================================================
// Function: Product::ApplyRemoveMes( char cActionCode )
// Called by BackEnd::ApplyRemoveMes().  Based on the values of
// cActionCode and (Product::uSeqLastAppliedMes or Product::uSeqFirstDeletedMes),
// changes the status of this Product's Mes Orders.
// Calls UpdateDetails().
//=================================================================================
void Product::ApplyRemoveMes( char cActionCode, unsigned int& uCountProcessed )
{
   Mes *pMes = myMesList.pFirstMes;
   while ( pMes )
   {
      // Change status
      switch ( cActionCode )
      {
         case 'A':
              if ( pMes->uSequence <= uSeqLastAppliedMes )
              {
                 // if the current status is being changed, then increment uCountProcessed
                 if ( pMes->applied() == False )
                    uCountProcessed++;
                 pMes->setApplied( True );
              }
              break;

         case 'R':
              if ( pMes->uSequence >= uSeqFirstDeletedMes )
              {
                 // if the current status is being changed, then increment uCountProcessed
                 if ( pMes->applied() == True )
                    uCountProcessed++;
                 pMes->setApplied( False );
              }
              break;
      } /* end .. switch*/
      pMes = pMes->pNextMes;
   }

   UpdateDetails();
   UpdateFeatures();
};

//=================================================================================
// Function: void Product::DestroyAllMes()
// Destroys all the Mes objects associated with this Product.  This function is
// used by BackEnd::ConvertPreviousCFREPORT().
//=================================================================================
void Product::DestroyAllMes()
{  myMesList.removeAll();  }

//=================================================================================
// Function: UpdateFeatures()
// If this Product has applied Mes(es):
//      1. Deletes UpdatedFeatureSet if it exists
//      2. Creates a new UpdatedFeatureSet
//      3. For each Mes of this Product
//           Process each Feature of the APPLIED Mes (ie, depending upon
//           Feature::eAction, ADD or REMOVE the Feature from this
//           Product's FeatureSet)
//
//=================================================================================
void Product::UpdateFeatures()
{
   // Delete existing UpdatedFeatureSet
   if ( myUpdatedFeatureSet.numberOfElements() > 0 )
      myUpdatedFeatureSet.removeAll();

   if ( HasAppliedMes() == False )
      return;

   // Create a new FeatureSet from the base Product FeatureSet
   myUpdatedFeatureSet = myFeatureSet;

   // Process Features of each applied Mes
   Mes *pMes = myMesList.pFirstMes;
   while ( pMes )
   {
      if ( pMes->applied() )
         ProcessMesFeatures( pMes );
      pMes = pMes->pNextMes;
   }
}

// Returns a pointer to the set of Features
FeatureCollection & Product::FeatureSet()
{
   UpdateFeatures();
   if ( HasAppliedMes() )
      return myUpdatedFeatureSet;
   else
      return myFeatureSet;
}

//=================================================================================
// Function: ProcessMesFeatures( Mes * )
// ADDs or REMOVEs the Features in the (base) Product.  This function is called
// by Product::UpdateFeatures()
//=================================================================================

void Product::ProcessMesFeatures( Mes *pMes )
{
   // Process each Feature of the Mes
   FeatureCollection::Cursor MesFeatureCrs ( pMes->myFeatureSet );
   forCursor( MesFeatureCrs )
   {
      Feature& MesFeature = (pMes->myFeatureSet).elementAt( MesFeatureCrs );
      IString  sFeatureKey = key( MesFeature );
      switch( MesFeature.eAction )
      {
         case Feature::ADD:  // Add
                  // if the Feature already exists in the base Product, increment its quantity
                  if ( myUpdatedFeatureSet.containsElementWithKey(sFeatureKey) )
                  {
                     Feature& aFeature = myUpdatedFeatureSet.elementWithKey( sFeatureKey );
                     aFeature.iQuantity += MesFeature.iQuantity;
                  }
                  else // add the new Feature to the baseProduct
                  {
                     Feature tempFeature = MesFeature;
                     tempFeature.eAction = Feature::NONE;
                     myUpdatedFeatureSet.add( tempFeature );
                  }
                  break;

         case Feature::REMOVE:  // Remove
                  if ( myUpdatedFeatureSet.containsElementWithKey(sFeatureKey) )
                  {
                     Feature& aFeature = myUpdatedFeatureSet.elementWithKey( sFeatureKey );
                     aFeature.iQuantity -= MesFeature.iQuantity;
                     if ( aFeature.iQuantity < 1 )
                        IBoolean bRemove = myUpdatedFeatureSet.removeElementWithKey( sFeatureKey );
                  }
                  break;
      } // end..switch

   } // end..forCursor
}

//=================================================================================
// Function: CreateProductRecord()
//
// If the Product has APPLIED Mes(es), this function creates an Installed Product
// Record (Record type 25 (Hardware) or type 49 (Software)) which can be written
// to the output CFREPORT.  It uses Features in myUpdatedFeatureSet for constructing
// feature/quantity pairs.
//=================================================================================

IString Product::CreateProductRecord()
{
   UpdateFeatures();
   if ( HasAppliedMes() == False )
      return IString("");

   IString  sProductRec;
   if ( category() == SW )
   {
      sProductRec = IString(STR_49RecType)              +
                    sCurrentTypeModel.subString(1,4);
   }
   else
   {
      sProductRec = IString(STR_25RecType)              +
                    sCurrentTypeModel.subString(1,4)    +
                    IString(" ");
   }

   sProductRec += sCurrentTypeModel.subString(6,3);
   IString  sQty = sQuantity;
   sQty.rightJustify(5);
   sProductRec += sQty;

   // Add Features
   FeatureCollection::Cursor FeatureCrs( myUpdatedFeatureSet );
   forCursor( FeatureCrs )
   {
      Feature& aFeature = myUpdatedFeatureSet.elementAt( FeatureCrs );
      sProductRec += aFeature.FeatureQty();
   }

   return sProductRec;
}

//=================================================================================
// Function: UpdateProdIdRec( IString& )
// Receives a Product ID record (record 95), and modifies it.
//
// If this Product is Initial Order Product, this function modifies:
//
//     INSTALL DATE (cols. 91-98)         = Today
//     PRODUCT STATUS (COL. 99)           = 'I'nstalled (column 99)
//
// If the Product has applied Mes(es), it modifies the following fields also:
//     PRODUCT TYPE, MODEL (COLS. 3-9)    = sCurrentTypeModel
//     ORIG ORDER NUM (cols. 38-46)       = copy it from cols. 29-37
//     ORDER NUMBER                       = Order number of the last APPLIED Mes
//
// This function is called by BackEnd::GenerateCFREPORT() to update ProductID record.
//
//=================================================================================

void Product::UpdateProdIdRec( IString& sProductIdRec )
{
   if ( sCurrentStatus != IString("INSTALLED") || HasAppliedMes() )
   {
      // PRODUCT STATUS
      sProductIdRec[99] = 'I';

      // INSTALL DATE
      sProductIdRec.overlayWith( (IDate::today()).asString("m%d"), 91 );  /* F73732 EDW 07/25/2002 */
   }

   if ( HasAppliedMes() )
   {
      // TYPE, MODEL
      if ( sCurrentTypeModel != sOriginalTypeModel )
      {
         sProductIdRec.overlayWith( sCurrentTypeModel.subString(1,4), 3 );
         sProductIdRec.overlayWith( sCurrentTypeModel.subString(6,3), 7 );
      }

      // Copy ORDER NUMBER to ORIG ORDER NUM
      IString sOrderNum;
      sProductIdRec >> 28 >> sOrderNum >> 9;
      sProductIdRec.overlayWith( sOrderNum, 38 );

      // ORDER NUMBER
      if ( sLastAppMesOrderNo.size() < 9 )
         sLastAppMesOrderNo.leftJustify( 9 );  // U.S. - left aligned, padded with blanks
      sProductIdRec.overlayWith( sLastAppMesOrderNo, 29 );
   }
}

//=================================================================================
void Product::test()
{
   AbstractProduct::test();
   cout << "sOriginalTypeModel       =" <<  sOriginalTypeModel    << "=\n";
   cout << "sCurrentTypeModel        =" <<  sCurrentTypeModel     << "=\n";
   cout << "bHasMES                  =" <<  bHasMes               << "=\n";
   cout << "uCurrentSystemId         =" <<  uCurrentSystemId      << "=\n";

   IString   sAbRecord;
   CreateAbRecord( sAbRecord );
   cout << "AB Record =" << sAbRecord << "=\n";

   // Print out this Product's Mes Orders
   cout << "MES Orders================================\n";
   MesPointersKeySortedSet::Cursor myCursor( myMesPtrSet );
   forCursor(myCursor)
   {
      MesPointer& ptrToMes = myMesPtrSet.elementAt( myCursor );
      cout << "XBaseTypeModel =" << ptrToMes->sBaseTypeModel << "=\n";
      cout << "XTargetTypeModel ="<< ptrToMes->sTargetTypeModel << "=\n";
      cout << "XSequence = " << ptrToMes->uSequence << endl;
      cout.flush();
   }
};


//=======================================================================
// Offsets for container columns
//=======================================================================

unsigned long Product :: offsetApplied()
   { return offsetof(Product, strApplied); }
unsigned long Product :: offsetTypeModel()
   { return offsetof(Product, sCurrentTypeModel); }
unsigned long Product :: offsetOrder()
   { return offsetof(Product, sOrderSerial); }
unsigned long Product :: offsetDate()
   { return offsetof(Product, strCrad); }

// Set and get methods
IString  Product::typeModel()       { return sCurrentTypeModel; }
IString  Product::order()           { return sOrderSerial; }
IBoolean Product::HasAppliedMes()   { return bHasAppliedMes; }


//===========================================================================
// Implementation of class MesList
//

MesList::MesList()
{
   elementCount = 0;
   pFirstMes = (Mes *) NULL;
};

unsigned int MesList::numberOfElements()
{  return elementCount; };


void MesList::add( Mes *pMes )
{
   if ( numberOfElements() == 0 )
      pFirstMes = pMes;
   else
   {
      Mes *pPrevious = (Mes *) NULL;
      Mes *pCurrent   = pFirstMes;
      while (  pCurrent && (pMes->uSequence > pCurrent->uSequence) )
      {
          pPrevious = pCurrent;
          pCurrent = pCurrent->pNextMes;
      }

      if ( pPrevious == NULL )       // Insert at the beginning
      {
         pMes->pNextMes = pFirstMes;
         pFirstMes = pMes;
      }
      else
      {
         pMes->pNextMes = pCurrent;
         pPrevious->pNextMes = pMes;
      }
   }
   elementCount++;
};

void MesList::removeAll()
{
   if ( numberOfElements() > 0 )
   {
      Mes *pCurrent = pFirstMes;
      while ( pCurrent )
      {
         Mes *pTemp = pCurrent;
         pCurrent = pCurrent->pNextMes;
         delete pTemp;
      }
   }
   pFirstMes = (Mes *) NULL;
   elementCount = 0;
};

