#include "AF9MAIN.HPP"
#include "AF9APPLY.H"

#include "AFCINCLS.H"
#include "AFCSYS.HPP"
#include "AFCCPULS.HPP"
#include "AFCFEAT.HPP"

#include <iksbag.h>
//typedef              IKeySortedSet<Feature, IString>    FeatureKeySortedSet;
typedef              IKeySortedBag<Feature, IString>    FeatureCollection;

class DetailObject;

//----------------------------------------------------------------------------
// AbstractProduct:
//      An abstract class representing a hardware or software which can
//      be installed, initial order, MES, or Upgrade.
//----------------------------------------------------------------------------

#ifndef OA_CLASS_ABSTRACTPRODUCT
   #define OA_CLASS_ABSTRACTPRODUCT

#ifdef GARGJ_TEST
  class AbstractProduct
#else
  class AbstractProduct : public IContainerObject
#endif
{
 public:
    AbstractProduct();

    virtual void    ParseLoad (char               cActionType,
                               IString&           sProductRec,
                               IString&           sProductIdRec,
                               unsigned long      uProductRecOffset,
                               unsigned long      uProductIdRecOffset,
                               CpuList&           myCpuList     ) = 0;

    virtual void    test();

    void            AddFeatureDescription( IString&       sFeatureRec,
                                           unsigned long  uOffsetFeatureRec );
    virtual void    ParseAbRecord( IString& sAbRecord );
    virtual void    CreateAbRecord( IString& sAbRecord );
    static  void    ConvertAbRecord( IString& sOldAbRecord, IString& sNewAbRecord );
    virtual FeatureCollection &   FeatureSet() = 0;

// Set and get methods
    void            setApplied( IBoolean );
    void            setCrad( const IDate& crad );
    void            setDetail( DetailObject * );
    void            setRec54Offset( unsigned long ulOffset )
                                    {ulRecord54Offset = ulOffset;}    /*edw1*/

    IBoolean        applied();
    IDate           date();
    int             category();
    DetailObject*   detail();
    IString         description();
    IString         systemTypeNumber();
    unsigned int    status();
    IString         customerNumber();

// Method needed by front end
    void            format();

    IString         sOriginalRecordType;         // The record type in the input CFReport
    unsigned long   uOffsetToProductRecord;      // in CFReportFile with extra records
    unsigned long   uOffsetToProductIdRecord;
    unsigned long   ulRecord54Offset;                                 /*edw1*/

    IString         sOrderSerial;               // Order or Serial number
   // If the object is Mes, this stores the order/serial number of the base product

   //* F86750 Begin logic: SV added for the Services products
   //* enum            { CPU, HW, SW } eProdType;
    enum            { CPU, HW, SW, SV } eProdType;
   //* F86750 End logic:   HECC - 01/15/2004

    IString         sSwDesignatedCpuTypeSerial;
    IString         sDescription;
    IString         sSystemTypeNumber;
    enum            { INSTALLED, UNINSTALLED, ONORDER, SHIPPED, NEWLYCONFIGURED }
                    eProductStatus;            // column 99 of 95 record
    IString         sCustNum;

    IString         sCurrentStatus;    // "INSTALLED", "APPLIED", or "DELETED"
    static unsigned uLengthAbRec;
    unsigned int    uCountFeatures;
    FeatureCollection  myFeatureSet;
    DetailObject    *pDetail;

protected:
    virtual void    ProcessProductIDRec( IString& sProductIdRec,
                                         char     cActionType);
    void            CreateFeatures( IString&            sFeatures,
                                    const unsigned int  uSizeFetActQty );

    IBoolean        bApplied;
    IString         strApplied;
    IDate           crad;
    IString         strCrad;

    IString         sProdType;                  // "HW" or "SW"
};

#endif

class Product;    // declaration of Product is after class declaration of Mes

//----------------------------------------------------------------------------
// Mes
//      Stores the data from records 19, 50, and 95 of CfReport (V31).  Represents
//      MES/Upgrade Hardware or Software.
// Note:  The description and other values will be obtained
// from 2nd 95 record if this is an upgrade.
//----------------------------------------------------------------------------
#ifndef OA_CLASS_MES
   #define OA_CLASS_MES

class Mes : public AbstractProduct
{
 public:
    Mes();
    void            ParseLoad (char               cActionType,
                               IString&           sProductRec,
                               IString&           sProductIdRec,
                               unsigned long      uProductRecOffset,
                               unsigned long      uProductIdRecOffset,
                               CpuList&           myCpuList     );

    void            ProcessProductIDRec( IString&           sProductIdRec,
                                         char               cActionType);
    IBoolean        IsUpgrade();

    void            ParseAbRecord( IString& sAbRecord );
    void            CreateAbRecord( IString& sAbRecord );
    FeatureCollection &   FeatureSet();

// Various set and get methods
   IString          order();
   IString          targetProduct();
   IString          baseProduct();
   IString          serial();

// Methods needed by front end
   static unsigned long offsetApplied();
   static unsigned long offsetBaseProduct();
   static unsigned long offsetSerial();
   static unsigned long offsetTargetProduct();
   static unsigned long offsetOrder();
   static unsigned long offsetDate();
   static unsigned long offsetGroupSequence();
//

    void            test();

    IString         sOriginalTypeModel;       // The original type-model of the base product
    IString         sMesGroupSequence;        // Format: Group(Sequence:Total Mes Count)
    IString         sBaseTypeModel;           // cols 3-9 of records 19 or 50
    IString         sTargetTypeModel;         // cols 10-16 of record 19
    IString         sMesOrderNumber;          // cols 29-37 of 95 record

    IString         sProductActionCode;
    unsigned int    uSequence;                // cols 100-102 of ProductId record
                                              // otherwise
    unsigned long   uOffsetBaseProductRec;
    Product         * pBaseProduct;
    Mes             * pNextMes;

   unsigned int const&   SequenceNumber() const      // required for Key Sorted Set key() function below
      {  return uSequence; }
};

// -----------------------------------------------------
// These functions are required by Key Sorted Set
inline unsigned int const&      key( Mes const& anMes )  // Key access
{  return anMes.SequenceNumber(); }
//inline long compare( Mes const& Mes1, Mes const& Mes2 )  // Key ordering
//{  return compare( Mes1.SequenceNumber(), Mes2.SequenceNumber() );  }

#endif

//===========================================================================
// MesList:
//   Maintains a linked list of pointers to Mes(es) associated with this Product.
//   The list is ordered(ascending) by the sequence number of the Mes.
//===========================================================================

#ifndef OA_CLASS_MESLIST
  #define OA_CLASS_MESLIST

class MesList
{
 public:

   MesList();
   void         add( Mes * );
   unsigned int numberOfElements();
   void         removeAll();
   Mes          *pFirstMes;

 private:
   unsigned int elementCount;
};

#endif
//----------------------------------------------------------------------------
// Product:
//      Stores the data from Installed Hardware/Software
//      (records 25, 49, and 95) or Initial Order Hardware/
//      Software (records 08, 47, and 95) of CfReport (V31).
//----------------------------------------------------------------------------
#ifndef OA_CLASS_PRODUCT
   #define OA_CLASS_PRODUCT

class Product : public AbstractProduct
{
 public:
    Product();                                   // Initialize everything to NULL or 0

    void    ParseLoad (char               cActionType,
                       IString&           sProductRec,
                       IString&           sProductIdRec,
                       unsigned long      uProductRecOffset,
                       unsigned long      uProductIdRecOffset,
                       CpuList&           myCpuList     );

    void            ParseAbRecord( IString& sAbRecord );
    void            CreateAbRecord( IString& sAbRecord );
    static void     ConvertAbRecord( IString& sOldAbRecord, IString& sNewAbRecord );
    IBoolean        DoesMesMatch( Mes& );
    void            AssociateMes( Mes * );       // Associates the MES to itself.
                                                 // It sets bHasMes to True

    void            ApplyRemoveMes( char cActionCode, unsigned int& uCountProcessed );
    void            UpdateDetails();
    void            UpdateFeatures();
    FeatureCollection &   FeatureSet();
    IString         CreateProductRecord();
    void            UpdateProdIdRec( IString& sProductIdRec );
    void            DestroyAllMes();
    void            test();

// Methods needed by front end
   static unsigned long offsetApplied();
   static unsigned long offsetTypeModel();
   static unsigned long offsetOrder();
   static unsigned long offsetDate();
// Get methods
   IString          typeModel();
   IString          order();

   IBoolean         HasAppliedMes();


    IString         sOriginalTypeModel;         // Type/Model of the base product
    IString         sCurrentTypeModel;
    IString         sQuantity;                  // value in cols 10-14 of records 47 and 49,
                                                // value in cols 11-15 of records 08 or 25.


// Current Type/Model will be different than the sOriginalTypeModel if
// this Product has applied Upgrades; otherwise,
// sOriginalTypeModel and sCurrentTypeModel will have same values.

    IBoolean        bHasMes;                    // TRUE if has MESes, FALSE otherwise


// The following two members are used by BackEnd::ApplyRemoveMes() and Product::ApplyRemoveMes()
    unsigned int    uSeqLastAppliedMes;
    unsigned int    uSeqFirstDeletedMes;

// Container of pointers to this Product's Mes

    typedef         IElemPointer<Mes>  MesPointer;
    typedef         IKeySortedSet<MesPointer, unsigned int> MesPointersKeySortedSet;
    MesPointersKeySortedSet       myMesPtrSet;
    MesList                myMesList;

    unsigned int    uCurrentSystemId;

private:
   IBoolean         bHasAppliedMes;
   IString          sLastAppMesOrderNo;
   FeatureCollection            myUpdatedFeatureSet;
   void             ProcessMesFeatures( Mes *pMes );

// Possible statuses of an Order
// 1. REMOVED
// 2. APPLIED and UNASSIGNED
// 3. APPLIED and ASSIGNED

public:

// These functions are required by ISet
   IBoolean         operator==( Product const& aProduct ) const
      {  if ( sCurrentTypeModel == aProduct.sCurrentTypeModel &&
              sOrderSerial == aProduct.sOrderSerial )
            return True;
         else
            return False;
      }

   IBoolean         operator<( Product const& aProduct ) const
      {  if ( sCurrentTypeModel < aProduct.sCurrentTypeModel &&
              sOrderSerial < aProduct.sOrderSerial )
            return True;
         else
            return False;
      }
};

#endif

