#include "AFCINCLS.H"
#include "AF9APPLY.H"

//----------------------------------------------------------------------------
// CfReportFileStats
//      Objects which are displayed in the container of the main
//      window.  Contains everything AA record needs.
//----------------------------------------------------------------------------
#ifndef OA_CLASS_CFREPORTFILESTATS
   #define OA_CLASS_CFREPORTFILESTATS

#ifdef GARGJ_TEST
  class CfReportFileStats
#else
  class CfReportFileStats : public IContainerObject
#endif
{
 public:
    CfReportFileStats( IString sFileName );

    void                    CfrInitialize();    // Initializes almost everything to 0 or NULL
    void                    ParseAaRecord(IString& sAArecord);
    int                     IsFileSupported(IString&, IBoolean&);
    void                    ParseHeader( IString& sCfrHeader );
    static void             UpdateHeader( IString& sCfrHeader );
    void                    CreateAaRecord(IString& sAArecord);

    void                    test();

// Set and Get Methods
    void                    setRetrievalDate( const IDate& );
    void                    setCountSystems( const unsigned int& );
    void                    setCountUnassigned( const unsigned int& );
    void                    setCountUnapplied( const unsigned int& );
    void                    setUsable(const IBoolean b);
    void                    setName( IString );
    unsigned int            Systems();
    unsigned int            Unassigned();
    unsigned int            Unapplied();
    IString                 name();
    IDate                   date();
    IBoolean                usable();


// Methods needed by front end
    static unsigned long    offsetName();
    static unsigned long    offsetDate();
    static unsigned long    offsetSystems();
    static unsigned long    offsetProducts();
    static unsigned long    offsetOrders();
    void                    format();

    void                    setChanges( const unsigned int );
    unsigned int            changes();

// ---------------------------

    IString                 sCurrentAppVersion;
    IString                 sCfrFileName;              // for example, "TEST"
    IString                 sCfrFullFileName;          // for example, "C:\ORDAPPLY\TEST.BIR"
    fstream                 CfrFileStream;
    unsigned int            uOffsetFirstACrecord;      // Unassigned System (AC) record
    IString                 sAppVersion;
    unsigned int            uCountCpu;
    unsigned int            uCountMes;
    unsigned int            uCountNew;
    unsigned int            uCountOrders;
    unsigned int            uCountInstalled;
    unsigned int            uCountAppliedNew;
    unsigned int            uCountRemovedNew;
    unsigned int            uCountAppliedMes;
    unsigned int            uCountRemovedMes;
    unsigned int            uErrorCode;

    IString                 sCfrVers;            // columns 3-4 of record 00
    IString                 sCfrCreateDate;      // columns 5-12 of record 00
    IString                 sConfigCountryCode;  // columns 35-36 of record 00

private:
    void                    setRetrievalDate( const IString& );
    void                    setCountSystems( const IString& );
    void                    setCountUnassigned( const IString& );
    void                    setCountUnapplied( const IString& );


    IDate                   retrievalDate;
    IString                 sRetrievalDate;
    IString                 sCountSystems;
    IString                 sCountUnassigned;
    IString                 sCountUnapplied;

    unsigned int            uCountUnapplied;
    unsigned int            uCountUnassigned;
    unsigned int            uCountSystems;       // Number of systems excluding UNASSIGNED
    unsigned int            uiChanges;
    IBoolean                bUsable;
};

#endif

