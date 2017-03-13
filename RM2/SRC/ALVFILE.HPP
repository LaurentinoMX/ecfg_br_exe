#ifndef _ALVFILE_HPP_
#define _ALVFILE_HPP_

#pragma pack(4)
class VFile
{
  public:
    enum Direction
      {
        up = 0
       ,down
      };
    enum FileMode
      {
        VF_ReadWriteApp = 0            // Creates-Appends
       ,VF_ReadOnly
       ,VF_ReadWriteOver               // Creates-Truncates existing file
       ,VF_ReadWriteNew                // Creates-Only if new
       ,VF_Temp                        // Temporary File
      };

//             VFile(unsigned iCacheSize = 20480);
               VFile(unsigned iCacheSize =  1024);
  virtual      ~VFile();
  int          searchResultPtr(){ return lSearchCachePtr;}
  int          insert(const void * Buf, unsigned len, long lFileOffset);
//  virtual int  Append(const void * Buf,unsigned len);
  int          append(const void * Buf,unsigned len);
  int          removeVF(long len, long lFileOffset);
  int          replace(long FileOffset,long len,const void * BufRep,long lenRep);
  int          replaceAll(const void * Buf,long BufLen, const void * RepBuf,
                      long RepBufLen,long startOffset = 0);
  int          saveAs(char * newFile);

  virtual int  openVF(char * name,FileMode m = VF_ReadWriteApp);
//  char * getfn();
//  char * getext();
  int          readVF(void * buf,long FileOffset,unsigned len,Direction dir=up);
  virtual int  writeVF(const void * buf, unsigned len, long FileOffset = -1,
                     Direction dir = up);
  int          changeFileSize (long toSize);
  int          flushVF();
  virtual int  closeVF ();
// virtual int vEOF();
  int          search(const void * Object,unsigned ObjectLen,long lFileOffset,
                      Direction dir, int skip,int fCountCRLFs = 1);
  virtual int  szSearch(char * sz, long FileOffset = -1,Direction dir = up,
                         int skip = 0);
  virtual unsigned lineCount();
  int          CRLFEOF();
 private:
  unsigned   cachesize;
  FileMode   Mode;
  int        fEOF;
  int        fChanged;
  int        fFileLengthChanged;
  int        lCachePtr;
  int        lCacheLen;
  int        lCacheOffset;
  int        fRestoreEOF;
  char    *  pCache;
  char    *  filename;
  int        fractureFile(long FileOffset,int len);
  int        fractureCache(unsigned Offset,int len);
  void       adjustOffset(long *);
  int        tempFile();
  int        setFileName(char *);

  protected:
  int        hFile;
  int        lSearchCachePtr;
  int        iFileSize;
  int        cCRLFs;         // number of crlf's passed during search action
  int        setCache(long idx,Direction dir, long len, int fForce = 0);
  int        writeCache();
};

class VFLine : public VFile
{
  unsigned     iLine;
  long         lFileOffset;
  int          iLen;
  int          iBufLen;
  char *       Buf;
  void         init();
  int          bufCopy(long lOffset,long Len);
  int          CRLFEnded(const void * pv, unsigned uLen);
  const void * dupe_CRLF(const void * pv, unsigned uLen);
  int          insert(const void *,unsigned,long){return 0;}

 public:

  VFLine(char * fn= 0, FileMode m = VF_ReadWriteApp,unsigned CacheSize = 20000);
  virtual      ~VFLine();
  int            openVF(char * name,FileMode m = VF_ReadWriteApp);
  int            move(int iDelta);
  int            readVF(unsigned iNmbr);
  int            search(char * sz,unsigned iStartLine = 0,Direction dir = up,
                        int skip= 0);
  int            deleteVF(unsigned Line = 0);
  int            rewrite(const void * Buf, unsigned ByteCount,unsigned Line = 0);
  int            insertLine(const void * Buf, unsigned uByteCount,unsigned Line = 0);
  int            appendString(const char * sz);
  int            appendLine(const void * pv, unsigned uByteCount);
  int            insertString(const char * sz,unsigned Line = 0);
  int            copy(void * Buf,unsigned Line = 0);
  int            strCopy(char * Buf,unsigned Line = 0);
  int            byteCount(unsigned Line = 0);
  int            last();
  int            isLast();
  const char *   stringData(unsigned Line = 0);
  const void *   data(unsigned Line = 0);
  unsigned       lineCount();
};
//#if 0
class VFRec : public VFile
{
  unsigned     uRec;
  unsigned     uLen;
  char *       Buf;
  int          insert(const void *,unsigned,long){return 0;}
  int          append(const void * Buf,unsigned len){return 0;}

 public:

  VFRec(unsigned RecLength,char * fn= 0, FileMode m = VF_ReadWriteApp,unsigned CacheSize = 20000);
  virtual      ~VFRec();
  int            openVF(char * name,FileMode m = VF_ReadWriteApp);
  int            move(int Delta);
  int            readVF(unsigned RecNumber);
  int            search(const void * Bytes,unsigned ByteCount,
                        unsigned StartRec = 0, Direction dir = up, int skip= 0);
  int            deleteVF(unsigned Rec = 0);
  int            replace(const void * Buf,unsigned Rec = 0);
  int            insert(const void * Buf,unsigned Rec = 0);
  int            append(const void * Buf);
  int            copy(void * Buf,unsigned Rec = 0);
  int            last();
  int            isLast();
  const void *   data(unsigned Line = 0);
  unsigned       count();
};
//#endif
#pragma pack()
#endif


