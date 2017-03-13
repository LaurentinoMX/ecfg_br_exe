#include <stdio.h>
#include <winerror.H>
#include "AFCCFRST.HPP"

//=============================================================
// Constructor
CfReportFileStats::CfReportFileStats( IString sFileName )
{
    CfrInitialize();
    sCfrFileName = sFileName;
    sCurrentAppVersion = IString("02.00");
}

//=============================================================
// Initializes almost everything to 0 or NULL
void CfReportFileStats::CfrInitialize()
{
    setCountSystems( 0 );
    setCountUnassigned( 0 );
    setCountUnapplied( 0 );
    uOffsetFirstACrecord   = 0;
    uCountCpu              = 0;
    uCountMes              = 0;
    uCountNew              = 0;
    uCountOrders           = 0;
    uCountInstalled        = 0;
    uCountAppliedNew       = 0;
    uCountRemovedNew       = 0;
    uCountAppliedMes       = 0;
    uCountRemovedMes       = 0;
    uErrorCode             = NO_ERROR;
    setUsable(True);
    setChanges( 0 );
}

//=============================================================
// Looks at the first record in the input file and determines
// whether the file is supported by this application or not.
// Sets bProcessAsNew = False if the file need NOT be processed as a
// new file.
// Returns   NO_ERROR                   the file is supported
//           INVALID_CFREPORT
//           CFREPORT_NOT_SUPPORTED

int CfReportFileStats::IsFileSupported(IString&  sFirstRecord,
                                       IBoolean& bProcessAsNew)
{
   // Unsupported if all the characters are not ASCII
   if ( ! sFirstRecord.isASCII() )
      return IDS_ErrorInvalidCfreport_Text;

   // If the first record is AA record, find out the application version
   // which parsed this file.  If the version is too old, process the
   // file as if it is a new file.
   if ( sFirstRecord.subString(1,2) == IString("AA") )
   {
      ParseAaRecord( sFirstRecord );
      if ( sAppVersion > sCurrentAppVersion )
         return IDS_ErrorOaFileVersNotSup_Text;
      else if ( sAppVersion >= STR_SupportedAppVersion )  // Supported existing file
         bProcessAsNew = False;
      else                                        // New file
         CfrInitialize();
   }
   // Error if the CFREPORT version is not supported
   else if ( sFirstRecord.subString(1,2) == IString("00") )
   {
      ParseHeader( sFirstRecord );
      if ( sCfrVers != IString("31") )
         return IDS_ErrorCFRNotSupported_Text;
   }
   else    /* Invalid file (that is, the first record is neither AA nor 00)*/
      return IDS_ErrorInvalidCfreport_Text;

   return NO_ERROR;
}

//=============================================================
// Parses AA record of a file which has extra records
void CfReportFileStats::ParseAaRecord( IString& sFirstRecord )
{
    IString    sCntSystems;
    IString    sCountCpu;
    IString    sCountMes;
    IString    sCountNew;
    IString    sCountInstalled;
    IString    sCountOrders;
    IString    sCountAppliedNew;
    IString    sCountRemovedNew;
    IString    sCountAppliedMes;
    IString    sCountRemovedMes;
    IString    sCntUnassigned;

    // Parse AA record
    sFirstRecord >> 2 >>
      sCntSystems >> 4 >>
      sCountCpu >> 4 >>
      sCountMes >> 4 >>
      sCountNew >> 4 >>
      sCountInstalled >> 4 >>
      sCountOrders >> 4 >>
      sCountAppliedNew >> 4 >>
      sCountRemovedNew >> 4 >>
      sCountAppliedMes >> 4 >>
      sCountRemovedMes >> 4 >>
      sCntUnassigned >> 4 >>
      sCfrCreateDate >> 8 >> 2 >>
      sAppVersion >> 6;

    sAppVersion.strip();
    // convert the values from IStrings to unsigned long
    setCountSystems( sCntSystems.asUnsigned() - 1 );/* exclude UNASSIGNED system*/
    uCountCpu              = sCountCpu.asUnsigned();
    uCountMes              = sCountMes.asUnsigned();
    uCountNew              = sCountNew.asUnsigned();
    uCountInstalled        = sCountInstalled.asUnsigned();
    uCountOrders           = sCountOrders.asUnsigned();
    uCountAppliedNew       = sCountAppliedNew.asUnsigned();
    uCountRemovedNew       = sCountRemovedNew.asUnsigned();
    uCountAppliedMes       = sCountAppliedMes.asUnsigned();
    uCountRemovedMes       = sCountRemovedMes.asUnsigned();
    setCountUnassigned( sCntUnassigned );
    setCountUnapplied( uCountRemovedNew + uCountRemovedMes );

    // Set date
    setRetrievalDate( sCfrCreateDate );
}

//=============================================================
// Creates an AA record for writing to the CfReport
void CfReportFileStats::CreateAaRecord(IString& sAArecord)
{
    // Convert numbers to IString
    IString sCntSystems ( Systems() + 1 );   /* Add 1 for UNASSIGNED system*/
    IString sCountCpu ( uCountCpu );
    IString sCountMes ( uCountMes );
    IString sCountNew ( uCountNew );
    IString sCountInstalled ( uCountInstalled );
    IString sCountOrders ( uCountOrders );
    IString sCountAppliedNew ( uCountAppliedNew );
    IString sCountRemovedNew ( uCountRemovedNew );
    IString sCountAppliedMes ( uCountAppliedMes );
    IString sCountRemovedMes ( uCountRemovedMes );
    IString sCntUnassigned ( Unassigned() );

    // Pad the IString with blanks
    sCntSystems.leftJustify( 4 );
    sCountCpu.leftJustify( 4 );
    sCountMes.leftJustify( 4 );
    sCountNew.leftJustify( 4 );
    sCountInstalled.leftJustify( 4 );
    sCountOrders.leftJustify( 4 );
    sCountAppliedNew.leftJustify( 4 );
    sCountRemovedNew.leftJustify( 4 );
    sCountAppliedMes.leftJustify( 4 );
    sCountRemovedMes.leftJustify( 4 );
    sCntUnassigned.leftJustify( 4 );
    sCfrCreateDate.leftJustify( 8 );

    IString sAppVersString = "OA" + sCurrentAppVersion + " ";

    // Construct the record
    sAArecord =
        "AA"                   +
        sCntSystems            +
        sCountCpu              +
        sCountMes              +
        sCountNew              +
        sCountInstalled        +
        sCountOrders           +
        sCountAppliedNew       +
        sCountRemovedNew       +
        sCountAppliedMes       +
        sCountRemovedMes       +
        sCntUnassigned         +
        sCfrCreateDate         +
        sAppVersString         +
        IString::copy( " ", 58 );                  /* Blanks for future use*/
}

//=============================================================
// Parses Header record in CFReport
void CfReportFileStats::ParseHeader( IString& sCfrHeader )
{
   // Extract Version, Creation Date, and Country Code
   sCfrHeader >> 2 >> sCfrVers >> 2 >> sCfrCreateDate >> 8;
   sCfrHeader << 35 >> sConfigCountryCode >> 2;

   setRetrievalDate( sCfrCreateDate );
}

//=============================================================
// Function: UpdateHeader()
// Receives a header record (record 00) and modifies its two values:
//  1. CONFIG CREATION DATE (columns 5-12)
//  2. CFREPORT DESCRIPTION (columns 42-81)
// BackEnd::GenerateCFREPORT() writes the updated header record to the
// output file.
//=============================================================

void CfReportFileStats::UpdateHeader( IString& sCfrHeader )
{
   sCfrHeader.overlayWith( (IDate::today()).asString("m%d"), 5 );
   sCfrHeader.overlayWith( IString("Modified by Base Retrieval (OA02.00)"), 42 );
}

//=============================================================
void CfReportFileStats::test()
{
   cout << name() << "::" << date().asString() << "::"
        << Systems() << "::" <<  Unassigned() << "::" <<
        Unapplied() << "::" << "ErrorCode = " << uErrorCode << endl;

    cout << "sCurrentAppVersion    =" << sCurrentAppVersion  << "=\n";
    cout << "sCfrFileName          =" << sCfrFileName  << "=\n";
    cout << "sCfrFullFileName      =" << sCfrFullFileName  << "=\n";
    cout << "uOffsetFirstACrecord  = " << uOffsetFirstACrecord<< endl;
    cout << "uCountCpu             = " << uCountCpu           << endl;
    cout << "uCountMes             = " << uCountMes           << endl;
    cout << "uCountNew             = " << uCountNew           << endl;
    cout << "uCountOrders          = " << uCountOrders        << endl;
    cout << "uCountInstalled       = " << uCountInstalled     << endl;
    cout << "uCountAppliedNew      = " << uCountAppliedNew    << endl;
    cout << "uCountRemovedNew      = " << uCountRemovedNew    << endl;
    cout << "uCountAppliedMes      = " << uCountAppliedMes    << endl;
    cout << "uCountRemovedMes      = " << uCountRemovedMes    << endl;
    cout << "uErrorCode            = " << uErrorCode          << endl;
    cout << "sAppVersion           =" << sAppVersion         << "=\n";
    cout << "sCfrVers              =" << sCfrVers            << "=\n";
    cout << "sConfigCountryCode    =" << sConfigCountryCode  << "=\n";

     IString sNewAArec;
     CreateAaRecord(sNewAArec);
     cout << "AA=" << sNewAArec << "=" << endl;

}


//=======================================================================
// Offsets for container columns. Needed by Front End
//=======================================================================

unsigned long CfReportFileStats :: offsetName()
   { return offsetof(CfReportFileStats, sCfrFileName); }
unsigned long CfReportFileStats :: offsetDate()
   { return offsetof(CfReportFileStats, sRetrievalDate); }
unsigned long CfReportFileStats :: offsetSystems()
   { return offsetof(CfReportFileStats, sCountSystems); }
unsigned long CfReportFileStats :: offsetProducts()
   { return offsetof(CfReportFileStats, sCountUnassigned); }
unsigned long CfReportFileStats :: offsetOrders()
   { return offsetof(CfReportFileStats, sCountUnapplied); }

//=======================================================================
// Reformat values when switching locale
//=======================================================================
void CfReportFileStats::format()
{
   if ( bUsable )
      sRetrievalDate = retrievalDate.asString(STR_FormatDate);
}


//=================== Various Set Methods ====================
void CfReportFileStats::setRetrievalDate( const IDate& retDate )
{
   retrievalDate  = retDate;
   sRetrievalDate = retDate.asString(STR_FormatDate);
}
void CfReportFileStats::setRetrievalDate( const IString& strDate )
{
   if ( strDate.size() == 8 )
   {
      IString  sRetrievalYear, sRetrievalMonth, sRetrievalDay;
      strDate >> sRetrievalYear >> 4 >> sRetrievalMonth >> 2 >> sRetrievalDay >> 2;
      retrievalDate = IDate( (IDate::Month) sRetrievalMonth.asInt(),
                              sRetrievalDay.asInt(),
                              sRetrievalYear.asInt() );
   }
   sRetrievalDate = retrievalDate.asString(STR_FormatDate);
}

// Count Systems
void CfReportFileStats::setCountSystems( const unsigned int& uCntSystems)
{
   uCountSystems = uCntSystems;
   sCountSystems = IString( uCntSystems );
}
void CfReportFileStats::setCountSystems( const IString& sCntSystems )
{
   uCountSystems = sCntSystems.asUnsigned();
   sCountSystems = sCntSystems;
   sCountSystems.strip();
}

// Count Unassigned
void CfReportFileStats::setCountUnassigned( const IString& sCntUnassigned )
{
   uCountUnassigned = sCntUnassigned.asUnsigned();
   sCountUnassigned = sCntUnassigned;
   sCountUnassigned.strip();
}
void CfReportFileStats::setCountUnassigned( const unsigned int& uCntUnassigned )
{
   uCountUnassigned = uCntUnassigned;
   sCountUnassigned = IString( uCntUnassigned );
}

// Count Unapplied
void CfReportFileStats::setCountUnapplied( const unsigned int& uCntUnapplied )
{
   uCountUnapplied = uCntUnapplied;
   sCountUnapplied = IString( uCntUnapplied );
}
void CfReportFileStats::setCountUnapplied( const IString& sCntUnapplied )
{
   uCountUnapplied = sCntUnapplied.asUnsigned();
   sCountUnapplied = sCntUnapplied;
   sCountUnapplied.strip();
}


// count of changes
void CfReportFileStats::setChanges(const unsigned int count )
{ uiChanges = count; }

void CfReportFileStats::setName( IString sFileName )
{
   IString sOldFileName = sCfrFileName;
   sCfrFileName = sFileName;

   // change the sCfrFullFileName also
   sCfrFullFileName.change( sOldFileName, sFileName );
}

void CfReportFileStats::setUsable(const Boolean b)
{
   bUsable = b;
   if ( bUsable == False )
   // Set all the displayed fields (except sCfrFileName) to STR_FileUnusable
   {
     sRetrievalDate = IString(STR_FileUnusable);
     setCountSystems( IString(STR_FileUnusable) );
     setCountUnassigned( IString(STR_FileUnusable) );
     setCountUnapplied( IString(STR_FileUnusable) );
   }
}

//=================== Various Get Methods ====================

IString      CfReportFileStats::name()       { return sCfrFileName; }
IDate        CfReportFileStats::date()       { return retrievalDate; }
unsigned int CfReportFileStats::Systems()    { return uCountSystems; }
unsigned int CfReportFileStats::Unassigned() { return uCountUnassigned; }
unsigned int CfReportFileStats::Unapplied()  { return uCountUnapplied; }
unsigned int CfReportFileStats::changes()    { return uiChanges; }
IBoolean     CfReportFileStats::usable()     { return bUsable; }
