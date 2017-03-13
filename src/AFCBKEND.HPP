#include "AFCINCLS.H"
#include "AFCCFRST.HPP"
#include "AFCCPULS.HPP"
#include "AFCPRODS.HPP"

class MainWindow;


//=================================================================================
#ifndef OA_CLASS_BACKEND
   #define OA_CLASS_BACKEND

class BackEnd
{
  public:
    BackEnd (MainWindow *ptrMainWin);
    int         LoadCpuList();
    int         ListFiles();
    int         LoadCFREPORT( IBoolean );
    void        ApplyRemoveMes( char cActionCode, IBoolean& bProcessedMore );
    void        ApplyRemoveMes( char cActionCode, Mes *pMes, IBoolean& bProcessedMore );
    void        Check05reclen ( IString strRET );  // ZOR 09032004 93732 method in AFCGENCF.CPP

    // D95670: New function in AFCGENCF.CPP.   HECC 10/29/2004
    void        Write05Recs ( ofstream& sOutputCfr, IString strRT05, long int& iRecCnt );

    int         SaveCFREPORT();
    int         GenerateCFREPORT();
    int         CloseFile();
    int         RenameFile();

    int         ConvertPreviousCFREPORT( CfReportFileStats& );
    int         ParseCFREPORT( CfReportFileStats& );
    static int  WRITELINE( ostream& stream, IString& sLine );
    static int  OpenFile( IString     sFullFileName,
                          const int   iOpenMode,
                          fstream&    FileStream );
    enum        { READ, WRITE } eOpenMode;
    CpuList&    getCpuList(){ return myCpuList; }                 //edw02/11/99
    ISortedSet<IString> Feature54TableSet1;                       //53915-MAMT-09/01/2000
    ISortedSet<IString> Feature54TableSet2;                       //53915-MAMT-09/01/2000
    IString     strTempName;
    Boolean     bfind95Rec;
  private:

    // Various utility functions
    static int  GetFileNames( IString, ISet<IString>& );
    static IString     GetTempFileName( IString sOriginalFileName );
    static int  Close_Cfrs_And_Return(  ifstream&   InputFileStream,
                                        ofstream&   OutputFileStream,
                                        int         iError,
                                        IString     sInputFileName,
                                        IString     sOutputFileName);

    CpuList     myCpuList;
    MainWindow  * pMainWindow;
    unsigned long uOffsetToInfoRecs;                              //edw 1/3/2000
    unsigned long uOffsetTo53Recs;                                //95887  added to keep the 53 record position
    ISortedSet<IString> placementRecSet;                          //edw 5/5/98
    ISortedSet<IString> connectionRecSet;                         //edw 5/5/98
};
#endif
