
// D131060 10/08/2009 ELRR
// BR is leaving temporary files with numerical extensions
// e.g. file.1, file.2 etc. in the user directory because a commented line was left  by the user edwtest .
// One of the logic actions of the int BackEnd::Close_Cfrs_And_Return function
// is to remove the temp file if an error was found in the Cfr.
// We did not find any feature or defect that supports the deactivation of the code
// due to this reason we are activating the code line again.



#include <sys\stat.h>   //* F81710: Added for the stat() function.

#include "AFCBKEND.HPP"
#include "AFCCFRST.HPP"
#include "AFCCPULS.HPP"
#include "AFCPRODS.HPP"
#include "AF9ORDRS.HPP"



BackEnd::BackEnd( MainWindow *ptrMainWin )
{
   pMainWindow = ptrMainWin;
};

int BackEnd::LoadCpuList()
{

   //* 81710 Begins new logic: Per feature requirement, the entire LoadCpuList()
   //* function must be redesigned completely.
   //* Now, the Cpulook.tbl file will be the same for every model directory,
   //* and the process to load the correct file is:
   //* a) Look into all the available directories in cfgviews
   //* b) Select the file with the most recent timestamp.
   //* c) If this file is not found in any directory, return an error message.


   //* 79548 Begins new logic:  Adds the Product Family to the path where the
   //* CPULOOK.TBL file is located and passes this new parameter to the method
   //* to correctly load the file from the directory that corresponds to the
   //* Product Family set by the user in the profile.
   //*    IString strCPUFilePath;
   //*    int iRC = 0;
   //*
   //*    IString strBRetExecPath = pMainWindow->strExecPath;
   //*
   //*    if (pMainWindow->bAS4)
   //*       strCPUFilePath = strBRetExecPath + STR_AS400 + STR_PathDelimiter;
   //*    else
   //*       if (pMainWindow->bS390)
   //*          strCPUFilePath = strBRetExecPath + STR_S390 + STR_PathDelimiter;
   //*       else
   //*          strCPUFilePath = strBRetExecPath + STR_RS6K + STR_PathDelimiter;
   //*
   //*      * return myCpuList.LoadCpuList( pMainWindow->strExecPath );
   //*      * First check if the CPULOOK.TBL file exists in the Product Family
   //*      * directory selected in the profile
   //*    iRC = myCpuList.LoadCpuList( strCPUFilePath );
   //*
   //*      * If it doesn't exist there, check now in the execution path of Base Retrieval
   //*      * and check if the file exists here.
   //*    if (iRC)
   //*       iRC = myCpuList.LoadCpuList( strBRetExecPath );
   //*
   //*    return iRC;
   //*      * 79548 Ends new logic:    HECC - 04/11/2003







   WIN32_FIND_DATA FileData;
   HANDLE hSearch;
   DWORD dwAttrs;

   struct stat buffLast;
   struct stat buffNew;

   buffLast.st_ctime = 0;
   buffNew.st_ctime = 0;

   IString strNewPath;
   IString strCPUFilePath = "";

   //* Creates the variables needed to check all the available directories in cfgviews
   //* (i.e. c:\econfig\cfgviews\*) and the execution path of Base Retrieval.
   IString strBRetExecPath = pMainWindow->strExecPath;
   IString strPathDirs = pMainWindow->strExecPath + STR_WildCard;

   IBoolean fFinished = FALSE;
   int iRC = 0;

   //* Opens the search to check for all the files and directories availables in the
   //* execution path of Base Ret. (Generically, econfig\cfgviews\* )
   hSearch = FindFirstFile(strPathDirs, &FileData);

   //* If an error occurs, set the error message and return, because no cpulook.tbl is
   //* going to be found.
   if (!hSearch)
      return IDS_ErrorOpenCpuList_Text;

   while (!fFinished)
      {
       //* Assigns the retrieved name (directory or file) to check if it is or not a
       //* directory.
       strNewPath = strBRetExecPath + IString(FileData.cFileName);
       dwAttrs = GetFileAttributes(strNewPath);
       if ( (dwAttrs & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY )
            {
            //* If the retrieved name is a directory, adds it the name of the CPULOOK.TBL
            //* file to find out if it exists or not here
            strNewPath = strNewPath + STR_PathDelimiter + STR_CpuTable;

            //* In case that the cpulook file exists in this directory and its timestamp
            //* be more recent or equal to the newest file found until this moment,(If any)
            //* save this path and its timestamp as the more recent.
            if ( (stat(strNewPath, &buffNew)==0) &&
                 (buffNew.st_ctime >= buffLast.st_ctime) )
                 {
                  strCPUFilePath = strNewPath;
                  buffLast.st_ctime = buffNew.st_ctime;
                 }
            }

       //* Retrieves the next file/directory name
       if (!FindNextFile(hSearch, &FileData))
          {
           //* If no file/directory name was retrieved, then set to true the flag to end
           //* the process.
           fFinished = TRUE;

           //* If error was not produced because no more files were found in the directory
           //* then the logic should return the error message explaining that the cpulook.tbl
           //* couldn't be found.
           if (GetLastError() != ERROR_NO_MORE_FILES)
              return IDS_ErrorOpenCpuList_Text;

          }
      }


   //* Remove the name of the CPULOOK.tbl file from the path as this will be assigned
   //* in the next called function.
   ULONG ulOffset = strCPUFilePath.lastIndexOf(STR_PathDelimiter);
   if(ulOffset)
      strCPUFilePath.remove(ulOffset+1);

   //* Checks if the CPULOOK.TBL file was found in any of the model directories
   //* (wich would mean that a valid path for this file have been assigned to this
   //* variable) In case it doesn't exist, return the error message.
   if (strCPUFilePath != "")
      iRC = myCpuList.LoadCpuList( strCPUFilePath );
   else
      iRC = IDS_ErrorOpenCpuList_Text;

   return iRC;

   //* return myCpuList.LoadCpuList( pMainWindow->strExecPath );
   //* 81710 Ends new logic:    HECC - 06/02/2003

};

//=======================================================================
// Function: BackEnd::ApplyRemoveMes( char cActionCode )
// Processes the Mes Orders the user has selected.  cActionCode can be
// 'A'(pply) or 'R'(emove).
//
//=======================================================================

typedef ISet <Product *>       ProductPtrSet; // Set of pointers to Products which are
                                              // associated with the selected Mes Orders

void BackEnd::ApplyRemoveMes( char cActionCode, IBoolean& bProcessedMore )
{
   ProductPtrSet  myProductPtrSet;

   // For each base Product:
   //    In case of 'A'pply, find the Mes Order with the highest sequence number
   //    In case of 'R'emove, find the Mes Order with the lowest sequence number

   unsigned int uCountSelected = 0;

   IContainerControl *pcnrMes = pMainWindow->pOrders->pcnrOrders[ORDERSMES];
   IContainerControl::ObjectCursor mcrs(*pcnrMes);
   forCursor (mcrs)
   {
      Mes* pMes = (Mes*)mcrs.current();
      if ( pcnrMes->isSelected(pMes) )
      {
         uCountSelected++;

         // Save the pointer to all the base Products of these Mes Orders
         Product *ptrBaseProduct = pMes->pBaseProduct;
         myProductPtrSet.add( ptrBaseProduct );

         switch ( cActionCode )
         {
            case 'A':
                if ( pMes->uSequence > ptrBaseProduct->uSeqLastAppliedMes )
                   ptrBaseProduct->uSeqLastAppliedMes = pMes->uSequence;
                break;

            case 'R':
                if ( pMes->uSequence < ptrBaseProduct->uSeqFirstDeletedMes )
                   ptrBaseProduct->uSeqFirstDeletedMes = pMes->uSequence;
                break;
         }                                                 /* end .. switch*/
      }
   }

   // Apply or Remove Mes Orders for each base Product
   unsigned int uCountProcessed = 0;
   ProductPtrSet::Cursor myProdCursor( myProductPtrSet );
   forCursor( myProdCursor )
   {
      Product * pBaseProduct = myProductPtrSet.elementAt( myProdCursor );
      pBaseProduct->ApplyRemoveMes( cActionCode, uCountProcessed );

      // Initialize these values
      pBaseProduct->uSeqLastAppliedMes = 0;
      pBaseProduct->uSeqFirstDeletedMes = UINT_MAX;
   }

   ( uCountProcessed > uCountSelected ) ? bProcessedMore = True : bProcessedMore = False;
};

//=======================================================================
// Function: BackEnd::ApplyRemoveMes( char cActionCode, Mes *pMes )
// Processes the Mes Order(s) based on the selected Mes.  cActionCode can be
// 'A'(pply) or 'R'(emove).
//
//=======================================================================

void BackEnd::ApplyRemoveMes( char cActionCode, Mes *pMes, IBoolean& bProcessedMore )
{
   Product *ptrBaseProduct = pMes->pBaseProduct;

   switch ( cActionCode )
   {
      case 'A':
          if ( pMes->uSequence > ptrBaseProduct->uSeqLastAppliedMes )
             ptrBaseProduct->uSeqLastAppliedMes = pMes->uSequence;
          break;

      case 'R':
          if ( pMes->uSequence < ptrBaseProduct->uSeqFirstDeletedMes )
             ptrBaseProduct->uSeqFirstDeletedMes = pMes->uSequence;
          break;
   }

   unsigned int uCountProcessed = 0;
   ptrBaseProduct->ApplyRemoveMes( cActionCode, uCountProcessed );

   ( uCountProcessed > 1 ) ? bProcessedMore = True : bProcessedMore = False;

   // Initialize these values
   ptrBaseProduct->uSeqLastAppliedMes = 0;
   ptrBaseProduct->uSeqFirstDeletedMes = UINT_MAX;
};

//=======================================================================
int BackEnd::CloseFile()
{
   pMainWindow->pFile->CfrFileStream.close();
   return NO_ERROR;
};

//=======================================================================
int BackEnd::RenameFile()
{
   CloseFile();

   IString     sNewName = pMainWindow->strBrowsePath
                          + STR_PathDelimiter
                          + pMainWindow->strSaveName
                          + STR_ExtensionBIR;

   // Rename the file
   if ( rename( (char *) pMainWindow->pFile->sCfrFullFileName,
                (char *) sNewName ) != 0 )
      return IDS_ErrorFileRename_Text;

   // Open the file
   int iError = OpenFile( sNewName, BackEnd::READ, pMainWindow->pFile->CfrFileStream );
   if ( iError )
      return iError;

   return NO_ERROR;
};

//====================================================================
// Used by BackEnd::ParseCFREPORT() and BackEnd::ConvertPreviousCFREPORT().
//    1. Closes both input and output files
// If an error occurs, this function:
//    1. Removes the output file
//    2. Returns the error
// If no error, this function, renames the output file to input file
//====================================================================
int BackEnd::Close_Cfrs_And_Return  (  ifstream&   InputFileStream,
                                       ofstream&   OutputFileStream,
                                       int         iError,
                                       IString     sInputFileName,
                                       IString     sOutputFileName)
{


   InputFileStream.close();
   OutputFileStream.close();


   if( iError )
   {

   //D131060 Begin:08-10-2009 ELRR
   //remove( (char *) sOutputFileName );//edwtest
     remove( (char *) sOutputFileName );
   //D131060 End   08-10-2009 ELRR


   return iError;
   }

   // Rename the input file name so that the output file can be renamed to input file
   IString sTempFileName = GetTempFileName( sInputFileName );
   rename( (char *) sInputFileName, (char *) sTempFileName );


   if( rename( (char *) sOutputFileName, (char *) sInputFileName) != 0 )
       return IDS_ErrorFileRename_Text;

   else
   {
      remove( (char *) sTempFileName );
      return NO_ERROR;
   }

};

//====================================================================
// Function: BackEnd::WRITELINE()
//
// Writes a line to either ofstream or fstream.
// Returns:
//          NO_ERROR
//          IDS_ErrorFileWrite_Text
//====================================================================
int BackEnd::WRITELINE( ostream& myStream, IString& sLine )
{
   // Append new line char
   sLine += IString( "\n" );
   myStream.write( (char *) sLine, sLine.size() );

   if ( myStream.fail() )
      return IDS_ErrorFileWrite_Text;
   else
      return NO_ERROR;
};

//===================================================================
// Function: IString GetTempFileName( IString        sOriginalFileName )
//    Given a complete path of a file name, returns another a unique file name (full path).
//    For example, if the input file name is "BRIAN.CFR", the returned
//    file name might be "BRIAN.001".
//===================================================================

#include <sys\types.h>
#include <sys\stat.h>

IString BackEnd::GetTempFileName( IString        sOriginalFileName )
{
   // Extract the base of the file name.  For example,
   // if the file path is "C:\ORDAPPLY\BRIAN.BIR", extract "C:\ORDAPPLY\BRIAN."
   IString     sTempFileName = sOriginalFileName;
   IString     sTempFileBase = sTempFileName.remove( sOriginalFileName.indexOf('.') + 1 );

   // Logic:
   //    if the input file is "BRIAN.CFR", this function tries file names
   //    starting with "BRIAN.1", "BRIAN.2", and so on.  Returns the file
   //    name which does not exist.  Function stat() below is Standard C
   //    library function.

   unsigned    uCounter;
   for ( uCounter=0  ; uCounter<1000; uCounter++ )
   {
      struct stat buf;
      IString     sFileName = sTempFileBase + IString(uCounter);
      if ( 0 != stat((char *) sFileName, &buf) )
         return sFileName;
   }

   // Just in case above loop does not find a unique name
   for ( uCounter=0  ; uCounter<100; uCounter++ )
   {
      struct stat buf;
      IString     sFileName = sTempFileBase + IString("A") + IString(uCounter);
      if ( 0 != stat((char *) sFileName, &buf) )
         return sFileName;
   }

   return sTempFileBase + IString("tmp");
};


//=========================================================================
// Function: OpenFile()
// Opens a file in non-shared mode(ie, it disallows other processes to open
// the file).  Uses extended C library function _sopen().  Attaches
// the file handle to the FileStream.
// Parameters:
//   (Input)             sFullFileName            Full path and file name
//   (Input)             Open mode                could be READ, WRITE, ...
//   (Input/Output)      FileStream               If it is successful in opening
//                                                the file, the stream is attached
//                                                the file.
//
// Returns:
//          NO_ERROR                        File was successfully opened
//          IDS_ErrorInputFileOpen_Text     Error opening file (READ mode)
//          IDS_ErrorOutputFileOpen_Text    Error opening file (WRITE mode)
//=========================================================================

#include <fcntl.h>
#include <sys\stat.h>
#include <share.h>
#include <io.h>

int BackEnd::OpenFile( IString     sFullFileName,
                       const int   iOpenMode,
                       fstream&    FileStream )
{
  int    oflag;
  switch ( iOpenMode )
  {
    case BackEnd::READ:
                oflag = O_RDONLY;
                break;
    case BackEnd::WRITE:
                oflag = O_WRONLY;
                break;
  }

  // Open the file denying READ and WRITE access to others (SH_DENYRW)
  int iFileDesc = _sopen( (char *) sFullFileName, oflag, SH_DENYRW, S_IREAD|S_IWRITE );
  IBoolean bError = False;
  if ( iFileDesc < 0 )
     bError = True;
  else
  {
     FileStream.close();
     FileStream.clear();
     FileStream.attach( iFileDesc );
     if ( FileStream.fail() )
        bError = True;
  }

  if ( bError )
  {
     if ( iOpenMode == BackEnd::READ )
        return IDS_ErrorInputFileOpen_Text;
     else if ( iOpenMode == BackEnd::WRITE )
        return IDS_ErrorOutputFileOpen_Text;
  }
  return NO_ERROR;
};

