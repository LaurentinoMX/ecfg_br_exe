// #ifdef __OS2__
  #define INCL_DOSFILEMGR   // File System values
  #define INCL_DOSERRORS    // DOS error values
  #include <os2.h>

#include "AFCBKEND.HPP"

//************************************************************
// Name:            int BackEnd::GetFileNames (IString          sDirPath,
//                                             ISet<IString>&   FileNames)
//
// Gets the list of all the files in a directory matching a pattern.
//
// Input:           sDirPath           Directory to search
// Input/Output:    FileNamesSet       Set of filenames (IString)
//
// Returns:
//      NO_ERROR                        Successful
//      IDS_ErrorFileNotFound_Text      File(s) not found
//      IDS_ErrorGettingFileList_Text   Other Error
//**********************************************************

int BackEnd::GetFileNames( IString           sDirPath,
                           ISet<IString>&    FileNames )
{
     // HDIR         hdir;
     // ULONG        cFilenames;
     FILEFINDBUF3 findBuffer;
     // APIRET       rc;

     // cFilenames = 1;
     // hdir = HDIR_SYSTEM;

     // char * ptr = (char *) sDirPath;

   BOOL fRet = TRUE;
   HANDLE hFindFile = FindFirstFile( sDirPath, &findBuffer);


   /*  rc = DosFindFirst(ptr,
                       &hdir,           // Directory handle
                       FILE_NORMAL,     // File attribute to look
                       &findbuf,        // Result buffer
                       sizeof(findbuf), // Size of result buffer
                       &cFilenames,     // Number of matching names to look
                       FIL_STANDARD);   // Standard level of information


    switch (rc)
    {
        case NO_ERROR:
                    break;
        case ERROR_FILE_NOT_FOUND:
        case ERROR_NO_MORE_FILES:
                    return IDS_ErrorFileNotFound_Text;
        default:
                    return IDS_ErrorGettingFileList_Text;
    }
  */

  if( hFindFile == INVALID_HANDLE_VALUE) return IDS_ErrorGettingFileList_Text ;

   do
         {

        IString sFileNm(findBuffer.cFileName);
        FileNames.add(sFileNm);
        fRet = FindNextFile(hFindFile, &findBuffer);

       /*  switch (rc)
        {
            case NO_ERROR:
                    break;
            case ERROR_NO_MORE_FILES:
                    bMoreFiles = False;
                    break;
            default:
                    return IDS_ErrorGettingFileList_Text;
        }
      */

    }  while (fRet);

    fRet = FindClose(hFindFile);
    if (fRet)
      return NO_ERROR;
    else
      return IDS_ErrorGettingFileList_Text;
};

// #endif
