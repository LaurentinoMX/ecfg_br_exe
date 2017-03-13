#include "AFCINCLS.H"
#include "AFCBKEND.HPP"

//===========================================================
// Function: ListFiles()
//
// Overall logic
// 1. Get the list of files from the directory given by the user
// 2. if files are found, do the following for each file:
//       Open
//       Determine if this application supports the file
//       If yes and needs to be processed as a new file
//             call BackEnd::ParseCFREPORT()
//       else set the error for this particular file
//
// Return:
//         NO_ERROR
//         Errorcode if error getting the file list
//            OR Successful getting the file list but none of them
//            are valid/supported.
//
//===========================================================


int BackEnd::ListFiles()
{
    // Path of the directory of retrieval files
    IString     sDirName = pMainWindow->strBrowsePath;

    // Get the file names
    ISet<IString>  FileNameSet;
    int iRC = GetFileNames( sDirName + STR_PathDelimiter + STR_WildCard + STR_ExtensionBIR,
                            FileNameSet );

    if (iRC)
    {
      if (iRC == IDS_ErrorFileNotFound_Text)
         return NO_ERROR;
      else
         return iRC;
    }

    int         iCountSuccessful = 0;
    // Iterate over file list
    ISet<IString>::Cursor myCursor( FileNameSet );
    forCursor( myCursor )
    {
        IString  sFileName = FileNameSet.elementAt( myCursor );
        IString  sTempFileName = sFileName;
        IString  sFileBase = sTempFileName.remove( sFileName.lastIndexOf('.') );  /* D50412 edw10/03/2000 */

        // Create CfReportFileStats
        CfReportFileStats   *pMyCfrStats = new CfReportFileStats( sFileBase );
        pMyCfrStats->sCfrFullFileName = sDirName + STR_PathDelimiter + sFileName;

        // A macro to set the error code for the current file,
        // add the file to the container, and continue
        #define   SETERROR(X)                                 \
        {                                                     \
            pMyCfrStats->uErrorCode = X;                      \
            pMyCfrStats->setUsable(False);                    \
            pMainWindow->pcnrFiles->addObject( pMyCfrStats ); \
            pMyCfrStats->test();                              \
            continue;                                         \
        }

        // Open the file
        ifstream            InputCfReport( pMyCfrStats->sCfrFullFileName );
        if ( InputCfReport.fail() )
            SETERROR(IDS_ErrorInputFileOpen_Text)

        // Read the first record and close the file
        IString sFirstRecord = IString::lineFrom( InputCfReport );
        if ( InputCfReport.eof() || 0==sFirstRecord.size() )
            SETERROR(IDS_ErrorInvalidCfreport_Text)
        if ( InputCfReport.fail() )
            SETERROR(IDS_ErrorFileRead_Text)
        InputCfReport.close();

        IBoolean bProcessAsNew = True;       // Assume that the file is to be processed as new

        // Determine if the input file is valid/supported and whether it is to be processed as
        // new or old file
        int iFileSuptRc = pMyCfrStats->IsFileSupported( sFirstRecord, bProcessAsNew );
        if ( iFileSuptRc )
            SETERROR(iFileSuptRc)
        else if ( bProcessAsNew )
        {
            pMyCfrStats->CfrInitialize();
            iRC = ParseCFREPORT(*pMyCfrStats);
            if (iRC)
               SETERROR(iRC)
        }

#ifdef GARGJ_TEST
        pMyCfrStats->test();
#endif
        // Add to the container
        pMainWindow->pcnrFiles->addObject( pMyCfrStats );
        iCountSuccessful++;

    } // end of forCursor
    if ( iCountSuccessful > 0 )
       return NO_ERROR;
    else
       return IDS_ErrorAllFilesUnusable_Text;
};
#undef   SETERROR

/* Test paths
1. Invalid directory path
2. File(s) not found
3. 1 file found
4. > 1 file found
5. File processed by too old version of Retrieve Shell/2
6. File processed by old version of Retrieve Shell but is supported
7. invalid CFREPORT (binary chars,
                    first rec not 00 or AA)
8. CFR version <> 31
*/
