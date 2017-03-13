#include "AFCINCLS.H"

//----------------------------------------------------------------------------
// Feature
//      Stores the data from records 25, 49, 08, 47, 19, 50 (Feature number,
//      quantity, action code), and 96 (Feature description) of CfReport (V31).
//----------------------------------------------------------------------------
#ifndef OA_CLASS_FEATURE
   #define OA_CLASS_FEATURE

class Feature
{
 public:
    Feature (IString& sFtQtyAct)
    {
        IString  sQuantity;
        sFtQtyAct >> sNumber >> 7 >> sQuantity >> 5;
        sNumber.strip();
        iQuantity = sQuantity.asUnsigned();

        eAction = NONE;
        if ( 13 == sFtQtyAct.size() )
        {
           if ( sFtQtyAct[13] == '1' )
              eAction = ADD;
           else if ( sFtQtyAct[13] == '2' )
              eAction = REMOVE;
        }
    }

    IString FeatureQty()  // Creates qty/feature string that can be included in
                          // an Installed Product record (25 or 49)
    {
        IString sQtyFeat = sNumber;
        sQtyFeat.leftJustify( 7 );

        IString sQty(iQuantity);
        sQty.rightJustify(5);

        sQtyFeat += sQty;
        return sQtyFeat;
    }

    IString         sNumber;                // Feature/RPQ/PARTNUMBER
    int             iQuantity;
    IString         sDescription;
    enum            { NONE, ADD, REMOVE } eAction;
    unsigned long   uOffset;               // Offset to 96 record in the file with extra records

    IString const&   FeatureNumber() const      // required for Key Sorted Set key() function below
      {  return sNumber; }
};

// -------------------------------------------------------------------
// These functions are required by Key Sorted Set
inline IString const&      key( Feature const& aFeature )                // Key Access
{   return aFeature.FeatureNumber();  }
inline long compare( Feature const& Feature1, Feature const& Feature2 )  // Key ordering
{  return compare( Feature1.FeatureNumber(), Feature1.FeatureNumber() );  }

#endif
