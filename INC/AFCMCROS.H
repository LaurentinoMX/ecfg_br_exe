#ifdef GARGJ_TEST
   #undef  LOADSTRING
   #define LOADSTRING(string)    NULL
#endif

#define  OPEN_INPUT_OUTPUT_CFRS(INPUT_STREAM, OUTPUT_STREAM, INPUT_FILENAME)   \
   ifstream       INPUT_STREAM( INPUT_FILENAME );                              \
   if ( INPUT_STREAM.fail() )                                                  \
      return IDS_ErrorInputFileOpen_Text;                                      \
   IString        sOutputFileName = GetTempFileName( INPUT_FILENAME );         \
   ofstream       OUTPUT_STREAM( sOutputFileName );                            \
   if ( OUTPUT_STREAM.fail() )                                                 \
      return IDS_ErrorOutputFileOpen_Text;

#define  CLOSE_INPUT_OUTPUT_CFRS(INPUT_STREAM, OUTPUT_STREAM, ERRCODE, INPUT_FILENAME) \
{                                                                                      \
   INPUT_STREAM.close();                                                               \
   OUTPUT_STREAM.close();                                                              \
   int   iCloseError = ERRCODE;                                                        \
   if( iCloseError )                                                                   \
   {                                                                                   \
      remove( (char *) sOutputFileName );                                              \
      return ERRCODE;                                                                  \
   }                                                                                   \
   IString        sTempFileName = GetTempFileName( INPUT_FILENAME );                   \
   rename( (char *) INPUT_FILENAME, (char *) sTempFileName );                          \
   if( rename((char *) sOutputFileName, (char *) INPUT_FILENAME) != 0 )                \
      return IDS_ErrorFileRename_Text;                                                 \
   else                                                                                \
   {                                                                                   \
      remove( (char *) sTempFileName );                                                \
      return NO_ERROR;                                                                 \
   }                                                                                   \
}

#define  CLOSE_INPUT_CFR(INPUT_STREAM, ERRCODE)                                        \
{                                                                                      \
   INPUT_STREAM.close();                                                               \
   int   iCloseError = ERRCODE;                                                        \
   if ( iCloseError )                                                                  \
     return ERRCODE;                                                                   \
   else                                                                                \
     return NO_ERROR;                                                                  \
}

#define  CLOSE_OUTPUT_CFR(OUTPUT_STREAM, ERRCODE)                                      \
{                                                                                      \
   OUTPUT_STREAM.close();                                                              \
   int   iCloseError = ERRCODE;                                                        \
   if ( iCloseError )                                                                  \
     return ERRCODE;                                                                   \
   else                                                                                \
     return NO_ERROR;                                                                  \
}
