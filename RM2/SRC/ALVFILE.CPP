// FILE VFILE.CPP
/**************************************************************************
 VBUFF.CPP is the generic unit to implement the virtual buffer class

 A virtual buffer is a buffer of any size. It owns a file in the background
 which is used to swap the memory as needed.

 The class can also be used for buffered files.

***************************************************************************/
/**************************************************************************

 definitions for os/2 or windows

 **************************************************************************/
//#include "def.h"
//#ifdef os2
//  #include <os2.h>
//#elif defined(win)
//  #include "win.h"
//#elif
//  #error You must define either "os2" or "win" in your compiler definitions
  //     define os2 if you are compiling under OS/2
  //     define win if you are compiling under DOS/Windows
  //     depending on this definition the appropriate includes will be fetched

//#endif
/**************************************************************************

 the win.h is locally hold and holds some os/2 specific statements

 **************************************************************************/
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <stdio.h>
#include <stdlib.h>

#include "alvfile.hpp"
//#include "error.h"

#define ALMAX(a,b)        (((a) > (b)) ? (a) : (b))
#define ALMIN(a,b)        (((a) < (b)) ? (a) : (b))
#define ALABS(A) ( ((A) < 0) ? -(A) : (A) )
#define BUFSIZE    1024         // look ahead for cache
static char *szCRLF =  "\r\n";

/**************************************************************************
Method: Exist
Returns false/true - whether a file exists.
***************************************************************************/
int Exist(char * fname)
{
 FILE  *file = fopen(fname,"r");
 if (file)
   fclose(file);
 return file? 1:0;
}

int VFile::tempFile()
{
 char FN[15];
 for (int i = 1; i <= 999; i++) {
   sprintf(FN,"VBUFFTMP.%03d",i);
   if (!Exist(FN))
     return openVF(FN,VF_Temp);
   }
 return 0;
}

/**************************************************************************
Function memchrd
memchrd is the same function as the C++ function memchr,but it searches
downwards starting at the offset.
***************************************************************************/
void *memchrd(const void *s, int c, size_t n)
{
 char *pc = (char *)s + n;
 while (n--)
   if (*pc-- == c)
     return ++pc;
 return 0;
}

VFile::VFile(unsigned iCacheSize)
      : hFile(-1)
       ,cachesize(ALMAX(4096,iCacheSize))
       ,fRestoreEOF(0)
       ,filename(0)
{
 pCache = new char[cachesize + 1];
}

VFile::~VFile()
{
 closeVF();
 if (pCache) delete[] pCache;
 if (filename) delete[] filename;
}

int VFile:: setFileName(char * name)
{
 if (filename) {
   delete[] filename;
   filename = 0;
   }
 if (name && (filename = new char[strlen(name) +1]))
   strcpy(filename,name);
 return (filename != 0);
}


/**************************************************************************
Method: openVF
Returns:  false/true  File (un)successfully opened
Open the given file in the given mode, closing the current file.
***************************************************************************/
int VFile::openVF(char * name, FileMode  mode)
{
 Mode = mode;
 int oflag = O_BINARY, pmode = S_IREAD;
 if ( Mode == VF_ReadOnly)
   oflag |= O_RDONLY;
 else {
   oflag |= O_RDWR | O_CREAT;
   pmode |= S_IWRITE;
   }
 closeVF();
 if ((Mode == VF_ReadWriteOver) || (Mode == VF_Temp) )
   oflag |= O_TRUNC;
 else if ((Mode == VF_ReadWriteNew) && Exist(filename) )
         return 0;
 if ((hFile = open(name,oflag,pmode)) != -1) {
   setFileName(name);
   iFileSize = filelength(hFile);
     if  ( fRestoreEOF = ((iFileSize)                &&
           (lseek(hFile,iFileSize-1,SEEK_SET) != -1) &&
           (read(hFile,pCache,1) == 1)               &&
           (*pCache == '\x1A')) )
       changeFileSize(iFileSize -1);

   *pCache = 0;
   lCachePtr = lCacheLen = lCacheOffset = lSearchCachePtr = 0;
   fChanged = fFileLengthChanged = 0;
   }
 return (hFile != -1);
}

/**************************************************************************
Method:  writeCache()
Returns: false/true  : (un)successful
Writes the cache to the file
***************************************************************************/
int VFile::writeCache()
{
 if ((hFile == -1) && !tempFile())
   return 0;
 int iWritten = 0;
 if (fFileLengthChanged)
   changeFileSize(iFileSize);
 if ( (lseek(hFile,lCachePtr,SEEK_SET) != -1) &&
      ((iWritten = _write(hFile,pCache,lCacheLen)) != -1) )
   fChanged = fFileLengthChanged = 0;
 return (lCacheLen == iWritten);
}

/**************************************************************************
Method:  fractureCache
Returns: false/true  : (un)successful
Shifts the Cache either to the left or the right at the given file offset,
if that offset, as well as the end of the file, are contained within the cache.
If len > 0: Opens a hole of len bytes at the offset
            (eg 123456789  -> 123....456789  for Shift(3,4)
If len < 0: Closes a section of len bytes at the offset
            (eg 123456789  -> 12389  for Shift(3,-4)
Returns success only if
  1 Cache points to the tailend of the file
  2 The requested length is such that the offset-wth-length is on the cache
***************************************************************************/
int VFile::fractureCache(unsigned uOffset,int len)
{
 char * pS, *pT;
 int CopyLen = 0;
             // ensure the stretch of bytes is in the cache and the cache
             // points to the end of the file
 int fShifted =  ( (uOffset >= lCachePtr)               &&
                   (uOffset <= (lCachePtr + lCacheLen)) &&
                   ((lCachePtr + lCacheLen) >= iFileSize));
 if (fShifted) {
   uOffset -= lCachePtr;            // determine cache offset
   if (len >= 0) {                   // open up a hole at offset of length len
     if (fShifted = ((lCacheLen + len ) <= cachesize) ) { //fits in cache
       pS = pCache + lCacheLen;
       pT = pS + len;
//       CopyLen = ALMAX(lCacheLen - uOffset,0);
       CopyLen = ALMAX(lCacheLen - uOffset + 1,0);
       while(CopyLen--)
         *pT-- = *pS--;
       }
     }
   else                                             // overlap to the left
     if ( fShifted = ( (pS = (pCache + uOffset - len)) >= pCache) ) {
       pT = pCache + uOffset;
       CopyLen = ALMAX(lCacheLen - (uOffset-len),0);
       while(CopyLen--)
         *pT++ = *pS++;
       }
   }
 if (fShifted) {
   iFileSize += len;
   fChanged = fFileLengthChanged = 1;
   lCacheLen += len;
   }
 return fShifted;
}

/**************************************************************************
Method:  fractureFile
Returns: false/true  : (un)successful
Shifts the File either to the left or the right at the given offset.
If len > 0: Opens a hole of len bytes at the offset
            (eg 123456789  -> 123....456789  for Shift(3,4)
If len < 0: Closes a section of len bytes at the offset
            (eg 123456789  -> 12389  for Shift(3,-4)
***************************************************************************/
int VFile::fractureFile(long lOffset,int len)
{
 lOffset = ALABS(lOffset);
 if (len && lOffset < iFileSize) {
   long lBytes = cachesize;
   if (len > 0) {                  // open up a hole
     long lEnd = iFileSize;
     changeFileSize(iFileSize + len);
              // start at the end and use the cache with its size to move
              // cache-fulls of bytes to the right
     while ((lEnd-lBytes) > lOffset) {
       lseek(hFile,lEnd - lBytes,SEEK_SET);
       lBytes = read(hFile,pCache,lBytes);
       lseek(hFile,lEnd - lBytes + len,SEEK_SET);
       lBytes = _write(hFile,pCache,lBytes);
       lEnd -= lBytes;
       }
             // now move from the requested offset, the remaining bytes in
             // the cache to the right
     lseek(hFile,lOffset,SEEK_SET);
     lBytes = lEnd - lOffset;
     lBytes = read(hFile,pCache,lBytes);
     lseek(hFile,lOffset+len,SEEK_SET);
     lBytes = _write(hFile,pCache,lBytes);
     }
   else {            // move len bytes left starting at the offset
     len = -len;
     len = ALMIN(len,iFileSize-lOffset);
     long lTarget = lOffset, lSource = lOffset + len;
     while ((lSource+lBytes) < iFileSize) {
       lseek(hFile,lSource,SEEK_SET);
       lBytes = read(hFile,pCache,lBytes);
       lseek(hFile,lTarget,SEEK_SET);
       lBytes = _write(hFile,pCache,lBytes);
       lTarget += lBytes;
       lSource += lBytes;
       }
     lseek(hFile,lSource,SEEK_SET);
     lBytes = iFileSize - lSource;
     lBytes = read(hFile,pCache,lBytes);
     lseek(hFile,lTarget,SEEK_SET);
     lBytes = _write(hFile,pCache,lBytes);
     changeFileSize(iFileSize - len);
     }
   setCache(lOffset,up,cachesize,1);
   return 1;
   }
 return 0;
}

/**************************************************************************
Method:  saveAs(char * newFile)
Method:  fractureFile
Returns: false/true  : (un)successful
Write the current state to a file of the given name.
If len > 0: Opens a hole of len bytes at the offset
            (eg 123456789  -> 123....456789  for Shift(3,4)
If len < 0: Closes a section of len bytes at the offset
            (eg 123456789  -> 12389  for Shift(3,-4)
***************************************************************************/
int VFile::saveAs(char * newFile)
{
 int  hNewFile, fOK = 0;
 char * TempBuf;
 if (fChanged)
   writeCache();   // store current state in file
                   // now copy from the file via a temporary buffer
 if (newFile && (!filename || strcmp(newFile,filename)))
   if ((hNewFile=open(newFile,O_BINARY | O_RDWR | O_CREAT,S_IWRITE)) != -1) {
     if (TempBuf = new char[BUFSIZE]) {
       int cBytes = 0;
       lseek(hFile,0,SEEK_SET);
       while ((cBytes + BUFSIZE) < iFileSize)
         if ((read(hFile,TempBuf,BUFSIZE) != BUFSIZE) ||
             (write(hNewFile,TempBuf,BUFSIZE) != BUFSIZE))
           break;
         else cBytes += BUFSIZE;
       if (!((cBytes + BUFSIZE) < iFileSize)) {  // check previous copy
         cBytes = iFileSize - cBytes;
         if (fOK = ((read(hFile,TempBuf,cBytes) == cBytes) &&
                    (write(hNewFile,TempBuf,cBytes) == cBytes)))
           if  (fRestoreEOF)
             write(hFile,"\x1A",1);
         }
       delete[] TempBuf;
       lseek(hFile,lCachePtr,SEEK_SET);     // reset to previous state
       }
     close(hNewFile);
     }
 return fOK;
}

/**************************************************************************
Method: setCache

Ensures that the requested offset and length (only if that's less than the
length of the cache) is in the cache, reading
the file if required, which also can be forced.
Returns: false/true: File was (not) read.
(lCacheOffset points to the data at the requested offset)
***************************************************************************/
int VFile::setCache(long lOffset,Direction dir, long len, int fForce)
{
 int fRead;
 if (fChanged)
   writeCache();
 if (fRead = (lOffset < lCachePtr))
   if (dir == up)
     lCachePtr = lOffset;
   else if (len > (cachesize - BUFSIZE))
          lCachePtr = lOffset;
        else lCachePtr = lOffset - cachesize + BUFSIZE;
 else if (fRead = (fForce || (lOffset >= (lCachePtr + lCacheLen))))
        if (dir == up)
          lCachePtr = lOffset - BUFSIZE;
        else if (len > (cachesize - BUFSIZE))
               lCachePtr = lOffset;
             else lCachePtr = lOffset - cachesize + BUFSIZE;
  lCachePtr = ALMAX(lCachePtr,0);
  lCacheOffset = lOffset - lCachePtr;
  if (fRead) {
    if ((hFile == -1) && !tempFile())
      return 0;
    lseek(hFile,lCachePtr,SEEK_SET);
    lCacheLen = read(hFile,pCache,cachesize); //  active cache size
    fEOF = ((dir == up) && (lCacheLen < cachesize));
    pCache[lCacheLen] = 0;
    }
  return (fRead);
}

/**************************************************************************
Method: readVF
Returns : the number of bytes read.
Read from the given offset the number of bytes into the given buffer via the
cache.
***************************************************************************/
int VFile::readVF(void * buf, long lOffset,unsigned uLen,Direction dir)
{
 char * Buf = (char *)buf;
 int iBytes = 0;
 long lGetLen, len = uLen;
 if (fEOF = ((lOffset + len) >= iFileSize))
   len = iFileSize - lOffset;
 while (len > 0) {
   setCache(lOffset,dir,len);
   lGetLen = ALMIN(lCacheLen - lCacheOffset,len);
   memcpy(Buf,&pCache[lCacheOffset],lGetLen);    // get the data
   Buf += lGetLen;                               // update buffer
   len -= lGetLen;
   lOffset += lGetLen;
   iBytes += lGetLen;
   }
 return iBytes;
}

/**************************************************************************
Method: adjustOffset
Adjust the offset such that it is within the filesize range, getting the
current file offset when negative.
***************************************************************************/
void VFile::adjustOffset(long * plOffset)
{
 if (*plOffset < 0)
    *plOffset = _tell(hFile);
 *plOffset = ALMIN(iFileSize,ALABS(*plOffset));
}

/**************************************************************************
Method: insert
Returns: false/true (un) successful
Insert the given bytes at the given offset.
***************************************************************************/
int VFile::insert(const void * Buf, unsigned uLen, long lOffset)
{
 if ((hFile != -1) || tempFile()) {
   adjustOffset(&lOffset);
   if (fractureCache(lOffset,uLen)) {
     memcpy(&pCache[lOffset - lCachePtr],Buf,uLen);
     return 1;
     }
   if ((iFileSize == lOffset) || fractureFile(lOffset,uLen))
     return writeVF(Buf,uLen,lOffset);
   }
 return 0;
}

/**************************************************************************
Method: append
Returns: false/true (un) successful
append the given bytes to the file.
***************************************************************************/
int VFile::append(const void * Buf,unsigned len)
{ return insert(Buf,len,iFileSize); }

/**************************************************************************
Method: replace
Returns: false/true (un) successful
Replace the given number of bytes at the given offset with the given
replacement bytes.
***************************************************************************/
int VFile::replace(long lOffset,long len,const void * Buf, long lenRep)
{
 adjustOffset(&lOffset);
 if ((len == lenRep) || fractureCache(lOffset,lenRep-len) ||
     fractureFile(lOffset,lenRep-len))
   return writeVF(Buf,lenRep,lOffset);
 return 0;
}

/**************************************************************************
Method: replaceAll
Returns: the number of replacements performed
Replace the given number of bytes at the given offset with the given
replacement bytes starting at the given offset fro the remainder of the file.
***************************************************************************/
int VFile::replaceAll(const void * Buf,long BufLen, const void * RepBuf,
                      long RepBufLen,long lOffset)
{
 int  qtyReplaced = 0;
 while ( search(Buf,BufLen,lOffset,up,0,0)) {
   replace(searchResultPtr(),BufLen,RepBuf,RepBufLen);
   lOffset += RepBufLen;
   qtyReplaced ++;
   }
 return qtyReplaced;
}

/**************************************************************************
Method: removeVF
Returns: false/true (un) successful
Remove the given number of bytes at the given file-offset.
***************************************************************************/
int VFile::removeVF(long len, long lOffset)
{
 adjustOffset(&lOffset);
 return (fractureCache(lOffset,-len) || fractureFile(lOffset,-len));
}

/**************************************************************************
Method: writeVF
Returns:   true - always successful
Write the given bytes at the given offset
***************************************************************************/
int VFile::writeVF(const void * Buf,unsigned uLen, long lOffset,Direction dir)
{
 const char *buf = (const char *)Buf;
 long lGetLen, len = uLen;
 if (lOffset < 0)
   lOffset = iFileSize;
 if ((lOffset > iFileSize) && !changeFileSize(lOffset))
   return 0;
 if (((lOffset + len) > iFileSize) &&! changeFileSize(lOffset + len))
   return 0;
 while (len > 0) {
   setCache(lOffset,dir,len);
   lGetLen = ALMIN(cachesize - lCacheOffset,len);
   if ((lCacheLen - lCacheOffset) < lGetLen)
     lCacheLen = lGetLen + lCacheOffset;
   memcpy(&pCache[lCacheOffset],buf,lGetLen);
   fChanged = 1;
   buf += lGetLen;
   len -= lGetLen;
   lOffset += lGetLen;
   }
 return 1;
}

/**************************************************************************
Method: changeFileSize
Returns:   false/true  - (un)successful
***************************************************************************/
int VFile::changeFileSize(long lSize)
{
 if (!chsize(hFile,lSize))
   iFileSize = lSize;
 return (iFileSize == lSize);
}

/**************************************************************************
Function: flushVF
Returns:   true/false  (un)sucessfully re-opened
Flushes and re-opens file to save it on  disk
***************************************************************************/
int VFile::flushVF()
{
 closeVF();
 return openVF(filename,Mode);
}

/**************************************************************************
Function closeVF
Returns: true/false: file (not) successfully closed
***************************************************************************/
int VFile::closeVF()
{
 int fRC = (hFile != -1);
 if (fRC) {
  if (fChanged)
    writeCache();
  if  (fRestoreEOF && changeFileSize(iFileSize + 1))
    if  (lseek(hFile,iFileSize-1,SEEK_SET) != -1)
      _write(hFile,"\x1A",1);
  fRC = !close(hFile);
  if (Mode == VF_Temp)
    fRC = remove(filename);
  hFile = -1;
  }
 return fRC;
}

/**************************************************************************
Method: szSearch
Returns: true/false (not) found
Find the given null-terminated string starting at the given offset in the
given direction.
***************************************************************************/
int VFile::szSearch (char * sz, long lOffset, Direction dir, int skip)
{ return search((const void *)sz,strlen(sz),lOffset,dir,skip); }

/**************************************************************************
Method: search
Returns: true/false (not) found
Find the given object starting at the given file-offset in the given direction,
optionally counting CRLF's.
***************************************************************************/
int VFile::search(const void * pv,unsigned iObjectLen,long lOffset,
                  Direction dir, int skip, int fCountCRLFs)
{
 char  * ptr,*ptrE,*ptrS,*pcIn = (char *)pv;
 long len;
 int fForce = 0, fFound = 0;
 cCRLFs = 0;
 int fCRLFSeek = (iObjectLen == 2 && *pcIn == '\r' && pcIn[1] == '\n');
 lOffset = (lOffset == -1)? _tell(hFile) : ALABS(lOffset);
 if (!(fEOF = (lOffset > iFileSize)))
   if (dir == up)
     while (!fFound && !fEOF) {
       setCache(lOffset,dir,iObjectLen,fForce);
       fForce = 0;
       ptrS = ptr = pCache + lCacheOffset;
       ptrE = pCache + lCacheLen;
       while (!fFound && ptr && (len = ptrE - ptr) >= iObjectLen) {
         if ( ptr = (char *)memchr(ptr,*pcIn,len))
           if (!memcmp(ptr,pcIn,iObjectLen))
             if (skip-- == 0)
               fFound = 1;
              else if (fCountCRLFs && fCRLFSeek)
                     cCRLFs++;
         if (ptr && !fFound)
           ptr++;          // continue ....
         }
       if (!fFound)
         if (fEOF)
           lSearchCachePtr = iFileSize;
         else {
           lOffset += (lCacheLen - lCacheOffset + 1 -iObjectLen); // get optimum buffer
           fForce = 1;
           }
       else  lSearchCachePtr = lOffset + (ptr - ptrS);
       if (fCountCRLFs && !fCRLFSeek)
         for (ptrE = fFound? ptr : ptrE; ptrS <= ptrE; ptrS++)
           if (*ptrS == '\n')
             cCRLFs++;
       }                                  // while not found ...
   else for (int fHitBottom = 0; !fFound && !fHitBottom ; ) {                               // if direction = DOWN ..... for (int fHitBottom = 0; !fFound && !fHitBottom;)
          setCache(lOffset,dir,iObjectLen,fForce);
          fForce = 0;
          if (!lOffset)            // search buffer down came to
            lCacheOffset = 0;          // bottom and actget was set to
          ptrS = ptr = pCache + lCacheOffset - 1;
          ptrS = ptr = ALMAX(ptr,pCache);
          while (!fFound && ptr && (len = ptr - pCache) >= iObjectLen)
            if (ptr = (char *)memchrd(pCache,*pcIn,len)) {
              if (!memcmp(ptr,pcIn,iObjectLen))
                if (skip-- == 0)
                  fFound = 1;
                else if (fCountCRLFs && fCRLFSeek)
                       cCRLFs--;
              if (!fFound)
                ptr--;        // continue ....
              }
          if (fCountCRLFs && !fCRLFSeek)
            for (ptrE = fFound? ptr : pCache; ptrS >= ptrE; ptrS--)
              if (*ptrS == '\n')
                cCRLFs--;
          if (!fFound)
            if (!lCachePtr) {             // top of file !
              lSearchCachePtr = 0;
              fHitBottom = 1;
              }
            else {
              lOffset = lCachePtr - 1;
              fForce = 1;
              }
          else  lSearchCachePtr = lOffset - (&pCache[lCacheOffset] - ptr);
          }
 return fFound;
}

unsigned VFile::lineCount()
{
 VFile::szSearch(szCRLF,0,up,iFileSize);
 return (CRLFEOF()? cCRLFs: cCRLFs+1);
}

/**************************************************************************
Method: CRLFEOF
Returns: true/false : File does (not) ends with CRLF ('0D0A')
***************************************************************************/
int VFile::CRLFEOF()
{
 char TempBuf[2];
 return ( (iFileSize > 1)                           &&
          fRestoreEOF                               &&
          (lseek(hFile,iFileSize-2,SEEK_SET) != -1) &&
          (read(hFile,TempBuf,2) == 2)              &&
          !memcmp(TempBuf,szCRLF,2));
}

////////////////////////////////////////////////////////
#define LINESIZE 1024

VFLine::VFLine(char * fn, FileMode  mode, unsigned CacheSize)
       :VFile(CacheSize)
{
 iBufLen = LINESIZE + 1;
 Buf = new char[iBufLen];
 init();
 if (fn)
   VFile::openVF(fn,mode);
}

VFLine::~VFLine()
{
 if (Buf) delete[] Buf;
}

int VFLine::openVF(char * fn, FileMode mode)
{
 init();
 return VFile::openVF(fn,mode);
}

void VFLine::init()
{
 iLine =
 lFileOffset =
 iLen = 0;
 *Buf = 0;
}

/**************************************************************************
Method: move
Returns: true/false : (un)successful
Move the Line relatively to the current-one.
***************************************************************************/
int VFLine::move(int iDelta)
{
 if (iDelta < 0)
   iDelta = ALMAX(0,iLine + iDelta);
 else { iDelta = ALMAX(1,iDelta);
        iDelta += iLine;
      }
 return readVF(iDelta);
}

/**************************************************************************
Method: bufCopy
Returns: true/false : (un)successful
Copy the requested bytes to the Buffer, realloc-ing as needed.
Null terminate for safety.
***************************************************************************/
int VFLine::bufCopy(long lOffset,long Len)
{
 if ((iLen = Len) > iBufLen) {
   delete[] Buf;
   Buf = new char[iBufLen = Len+1];
   }
 if (Buf && (VFile::readVF(Buf,lFileOffset = lOffset,Len) == iLen)) {
   Buf[iLen] = 0x00;
   return 1;
   }
 return 0;
}

/**************************************************************************
Method: readVF (the given Line number)
Returns: true/false : (un)successful
***************************************************************************/
int VFLine::readVF(unsigned iNmbr)
{
 int fUp, iSkips, fFound= 0;
 long lOffset;
 if (!iNmbr)
   init();
 else if (VFile::hFile != -1)
        { if (!iLine) iLine++;
          if (iNmbr < iLine)
             if (fUp = (iNmbr < iLine/2))
               { lOffset = 0;
                 iSkips = iNmbr - 1;
               }
             else { lOffset = lFileOffset;
                    iSkips = iLine - iNmbr;
                  }
          else { fUp = 1;
                 lOffset = lFileOffset;
                 iSkips = iNmbr - iLine;
               }
          if (fFound = VFile::szSearch(szCRLF,lOffset,fUp?up:down,iSkips))
             { iLine = iNmbr;
               if (fUp)
                 { lOffset = lSearchCachePtr + 2;
                   if (VFile::szSearch(szCRLF,lSearchCachePtr,down,0)) // down, 0 skip
                      lSearchCachePtr += 2;
                 }
               else { int iTemp = lSearchCachePtr + 2;
                      if (VFile::szSearch(szCRLF,iTemp,up,0)) // down, 0 skip
                         lSearchCachePtr += 2;
                      lOffset = lSearchCachePtr;
                      lSearchCachePtr = iTemp;
                    }
             }
          else if (fUp)
                 { iLine += cCRLFs;
                   int fCRLFEOF = CRLFEOF();
                   lOffset = lSearchCachePtr;
                   if (fCRLFEOF)
                      iLine--;
//                 else  fFound = (cCRLFs == iSkips);
                   if (VFile::szSearch(szCRLF,lOffset,down,fCRLFEOF))
                     lSearchCachePtr += 2;
                 }
               else iLine = 1;
          bufCopy(lSearchCachePtr,lOffset - lSearchCachePtr);
        }
 return fFound;
}

/**************************************************************************
Method search
Returns: the Line Number where found (which will then be the current Line)
         or 0 when not found.
Search for the given bytes, starting at the given Line in the given direction
and skipping the given number of occurrences.
***************************************************************************/
int VFLine::search(char * sz,unsigned iStartLine,Direction dir, int skip)
{
 int iLineSave = iLine;
 if (!iStartLine || iStartLine == iLine || this->readVF(iStartLine))
    if (VFile::szSearch(sz,this->lFileOffset,dir,skip))
      {
        iLine += cCRLFs;
        long lTemp = lSearchCachePtr;
        if (VFile::szSearch(szCRLF,lTemp,up,0))
          lSearchCachePtr += 2;
        long lOffset = lSearchCachePtr;
        if (VFile::szSearch(szCRLF,lTemp,down,0))
          lSearchCachePtr += 2;
        bufCopy(lSearchCachePtr,lOffset - lSearchCachePtr);
        return iLine;
      }
 this->readVF(iLineSave);
 return 0;
}

/**************************************************************************
Method: deleteVF (the given or current Line)
Returns: true/false : (un)successful
***************************************************************************/
int VFLine::deleteVF(unsigned uLineIn)
{
 if (!uLineIn || uLineIn == iLine || readVF(uLineIn))
   if (iLine)
     { int fLast = isLast();
       if (VFile::removeVF(iLen,lFileOffset))
         { readVF(fLast? iLine-1 : iLine);
           return 1;
         }
     }
 return 0;
}

/**************************************************************************
Method: isLast
Returns: true/false : The current Line is (not) the the last-one.
***************************************************************************/
int VFLine::isLast()
{
 return ((lFileOffset + iLen) == VFile::iFileSize);
}

/**************************************************************************
Method: insertLine
Returns: false/true : (un)successful
Insert the given data as the given or current Line.
The data will be appended with the delimeter if missing.
***************************************************************************/
int VFLine::insertLine(const void * pv, unsigned uByteCount,unsigned uLineIn)
{
 if (!uLineIn || uLineIn == iLine || this->readVF(uLineIn))
   if (iLine)
     { unsigned i = uByteCount;
       if (!isLast() && !CRLFEnded(pv,i))
         if (!(pv = dupe_CRLF(pv,i)))
           return 0;
         else i += 2;
       int RC = VFile::insert(pv,i,this->lFileOffset);
       if (i != uByteCount)
         delete[] (char *)pv;
       return RC;
     }
 return 0;
}

/**************************************************************************
Method: appendLine
Returns: false/true : (un)successful
Add the given data as the last Line.
The data will be appended with the delimeter if missing.
***************************************************************************/
int VFLine::appendLine(const void * pv, unsigned uByteCount)
{
 unsigned i = uByteCount;
 if (!CRLFEnded(pv,i))
   if (!(pv = dupe_CRLF(pv,i)))
     return 0;
   else i += 2;
 int RC = VFile::append(pv,i);
 if (i != uByteCount)
   delete[] (char *)pv;
 return RC;
}

/**************************************************************************
Method: appendString
Returns: false/true : (un)successful
Add the given null terminated string as the last Line.
The data will be appended with the delimeter if missing.
***************************************************************************/
int VFLine::appendString(const char * sz)
{
 return appendLine((const void *)sz,strlen(sz));
}

/**************************************************************************
Method: insertString
Returns: false/true : (un)successful
Insert the given null-terminated string as the given or current Line.
The data will be appended with the delimeter if missing.
***************************************************************************/
int VFLine::insertString(const char * sz,unsigned uLineIn)
{
 return insertLine((const void *)sz,strlen(sz),uLineIn);
}

/**************************************************************************
Method: CRLFEnded
Indicates whether the given data ends with the delimeter.
***************************************************************************/
int VFLine::CRLFEnded(const void * pv, unsigned uLen)
{
 if (uLen > 1)
   { char *pc = (char *)pv + uLen - 2;
     return (*pc++ == '\r' && *pc++ == '\n');
   }
 return 0;
}

/**************************************************************************
Method: dupe_CRLF
  Duplicate the given data and append a delimeter
Returns: Pointer to the copy or null if unsuccessful.
***************************************************************************/
const void * VFLine::dupe_CRLF(const void * pv, unsigned uLen)
{
 char *pc = new char[uLen+2];
 if (pc)
   { memcpy(pc,pv,uLen);
     memcpy(pc + uLen,szCRLF,2);
   }
 return (const *)pc;
}

/**************************************************************************
Method: rewrite
Returns: false/true : (un)successful
  Replace the given or current Line with the given data
The data will be appended with the delimeter if missing.
***************************************************************************/
int VFLine::rewrite(const void * pv, unsigned uByteCount,unsigned uLineIn)
{
 if (!uLineIn || uLineIn == iLine || this->readVF(uLineIn))
   if (iLine)
     { unsigned i = uByteCount;
       if (!isLast() && !CRLFEnded(pv,i))
         if (!(pv = dupe_CRLF(pv,i)))
           return 0;
         else i += 2;
       int RC = VFile::replace(lFileOffset,iLen,pv,i);
       if (i != uByteCount)
          delete[] (char *)pv;
       return RC;
     }
 return 0;
}

/**************************************************************************
Method: last
Returns: false/true : (un)successful
  Read the last Line.
***************************************************************************/
int VFLine::last()
{
 return readVF(VFile::lineCount());
}

/**************************************************************************
Method: copy
  Copies the given or current line to the given location.
  (Will make the requested Line the current-one, if different)
Returns: the number of bytes copied.
***************************************************************************/
int VFLine::copy(void * Buff,unsigned uLineIn)
{
 if (!uLineIn || uLineIn == iLine || readVF(uLineIn))
   if (iLine)
     { memcpy(Buff,Buf,iLen);
       return iLen;
     }
 return 0;
}

/**************************************************************************
Method: strCopy
  Copies and null-terminates the given or current line to the given location.
  (Will make the requested Line the current-one, if different)
Returns: the number of bytes copied.
***************************************************************************/
int VFLine::strCopy(char * Buff,unsigned uLineIn)
{
 int iBytes = this->copy(Buff,uLineIn);
 Buff[iBytes] = 0x00;
 return iBytes;
}

/**************************************************************************
Method: byteCount
  Returns the number of bytes of the given or current Line.
  (Will make the requested Line the current-one, if different)
***************************************************************************/
int VFLine::byteCount(unsigned uLineIn)
{
 if (!uLineIn || uLineIn == iLine || readVF(uLineIn))
   if (iLine)
     return iLen;
 return 0;
}

/**************************************************************************
Method: data
  Returns the pointer to the data of the given or current Line.
  (Will make the requested Line the current-one, if different)
***************************************************************************/
const void * VFLine::data(unsigned uLineIn)
{
 if (uLineIn && uLineIn != iLine)
    readVF(uLineIn);
 return Buf;
}

/**************************************************************************
Method: stringdata
  Returns the pointer to the null terminated data of the given or current Line.
  (Will make the requested Line the current-one, if different)
***************************************************************************/
const char * VFLine::stringData(unsigned uLineIn)
{
 return (const char *)(this->data(uLineIn));
}

/**************************************************************************
Method: lineCount
  Returns the number of Lines.
  (Needs to re-read the current Line, since the lineCount method might
   move the cache)
***************************************************************************/
unsigned VFLine::lineCount()
{
 unsigned i = VFile::lineCount();
 if (iLine)
    readVF(iLine);
 return i;
}

////////////////////////////////////////////////////////////
////////////////////////////////////
VFRec::VFRec(unsigned uRecLen,char * fn,FileMode  mode, unsigned CacheSize)
       :VFile(CacheSize)
{
 Buf = new char[uLen = uRecLen];
 if (fn)
   this->openVF(fn,mode);
 else uRec = 0;
}

VFRec::~VFRec()
{
 if (Buf) delete[] Buf;
}

/**************************************************************************
Method: openVF
Returns: true/false : (un)successful
Close the file if it contains a partial record.
***************************************************************************/
int VFRec::openVF(char * fn, FileMode mode)
{
 uRec = 0;
 if (VFile::openVF(fn,mode) && VFile::iFileSize % uLen)
   closeVF();
 return (VFile::hFile != -1);
}

/**************************************************************************
Method: move
Returns: true/false : (un)successful
Move the Record relatively to the current-one.
***************************************************************************/
int VFRec::move(int iDelta)
{
 if (iDelta < 0)
   iDelta = ALMAX(0,uRec + iDelta);
 else { iDelta = ALMAX(1,iDelta);
        iDelta += uRec;
      }
 return readVF(iDelta);
}

/**************************************************************************
Method: readVF (the given Record number)
Returns: true/false : (un)successful
***************************************************************************/
int VFRec::readVF(unsigned uRecNumber)
{
 if (uRecNumber && ((uRecNumber*uLen) <= VFile::iFileSize))
   { if (VFile::readVF(Buf,(uRecNumber-1) * uLen,uLen) == uLen)
       { uRec = uRecNumber;
         return 1;
       }
     else VFile::readVF(Buf,(uRec-1) * uLen,uLen);
   }
 return 0;
}

/**************************************************************************
Method search
Returns: the Record Number where found (which will then be the current-one)
         or 0 when not found.
Search for the given bytes, starting at the given Record in the given direction
and skipping the given number of occurrences.
***************************************************************************/
int VFRec::search(const void * Object,unsigned uObjectLen, unsigned uStartRec,
                  Direction dir, int skip)
{
 unsigned uRecSave = uRec;
 if (!uStartRec || uStartRec == uRec || this->readVF(uStartRec))
   if (VFile::search(Object,uObjectLen,(uStartRec-1)*uLen,dir,skip,0) &&
       this->readVF((lSearchCachePtr/uLen)+1))
     return uRec;
 this->readVF(uRecSave);
 return 0;
}

/**************************************************************************
Method: deleteVF (the given or current Record)
Returns: true/false : (un)successful
***************************************************************************/
int VFRec::deleteVF(unsigned uRecNumber)
{
 if (!uRecNumber || uRecNumber == uRec || readVF(uRecNumber))
   if (uRec)
     { int fLast = isLast();
       if (VFile::removeVF(uLen,(uRec-1) * uLen))
         { readVF(fLast? uRec-1 : uRec);
           return 1;
         }
     }
 return 0;
}

/**************************************************************************
Method: isLast
Returns: true/false : The current Rec is (not) the the last-one.
***************************************************************************/
int VFRec::isLast()
{
 return ((uRec * uLen) == VFile::iFileSize);
}

/**************************************************************************
Method: insert
Returns: false/true : (un)successful
Insert the given record as the given or current-one.
***************************************************************************/
int VFRec::insert(const void * pv,unsigned uRecNumber)
{
 if (!uRecNumber || uRecNumber == uRec || this->readVF(uRecNumber))
   if (uRec)
     return VFile::insert(pv,uLen,(uRec-1)*uLen);
 return 0;
}

/**************************************************************************
Method: Append
Returns: false/true : (un)successful
Add the given data as the last Record.
***************************************************************************/
int VFRec::append(const void * pv)
{
 return VFile::append(pv,uLen);
}

/**************************************************************************
Method: replace
Returns: false/true : (un)successful
  Replace the given or current Record with the given-one
***************************************************************************/
int VFRec::replace(const void * Rec,unsigned uRecNumber)
{
 if (!uRecNumber || uRecNumber == uRec || this->readVF(uRecNumber))
   if (uRec)
     return VFile::writeVF(Rec,uLen,(uRec-1)*uLen);
 return 0;
}

/**************************************************************************
Method: last
Returns: false/true : (un)successful
  Read the last Record.
***************************************************************************/
int VFRec::last()
{
 return this->readVF(count());
}

/**************************************************************************
Method: copy
  Copies the given or current Record to the given location.
  (Will make the requested Record the current-one, if different)
Returns: the number of bytes copied.
***************************************************************************/
int VFRec::copy(void * Buff,unsigned uRecNumber)
{
 if (!uRecNumber || uRecNumber == uRec || this->readVF(uRecNumber))
   if (uRec)
     { memcpy(Buff,Buf,uLen);
       return uLen;
     }
 return 0;
}

/**************************************************************************
Method: data
  Returns the pointer to the data of the given or current Record.
  (Will make the requested Line the current-one, if different)
***************************************************************************/
const void * VFRec::data(unsigned uRecNumber)
{
 if (uRecNumber && uRecNumber != uRec)
    this->readVF(uRecNumber);
 return Buf;
}


/**************************************************************************
Method: count
  Returns the number of Records.
  (Needs to re-read the current Line, since the lineCount method might
   move the cache)
***************************************************************************/
unsigned VFRec::count()
{
 return VFile::iFileSize/uLen;
}




#ifdef UNITTEST
///////////////////////////////////
///////////////////////////////////////////////////////////
/**************************************************************************
Test cases

remove the comment to activate the test cases and run vbuff.cpp as
stand-alone .EXE file

These test case help a.) to understand the module and b.) to allow
faster debugging in case of problems.

The test cases are at level of my private requirements. You would have
to change some filenames to adapt them to your reqs.

So, do not search in the package if you'll find the appropriate files
to run the cases, simply make your own environment. That is at least
what you should be able when you start testing this unit.

Have success !!!  Helmut Scherzer
***************************************************************************/
///* Test cases ...


static struct TestStruc {
                          char Name[25];
                          int  f;
                          int  i;
                        } TestTable[]={
                                       {"Record      1",0,  1},
                                       {"Record      2",1,  2},
                                       {"Record      3",0,  3},
                                       {"Record      4",1,  4},
                                       {"Record      5",0,  5},
                                       {"Record      6",1,  6},
                                       {"Record      7",0,  7},
                                       {"Record      8",1,  8},
                                       {"Record      9",0,  9},
                                       {"Record     10",1, 10},
                                       {"",            0,0}
                                      };







void main(int argc,char **argv)
{
 char * teststr;
 long actget;
 int fnd;
 VFile *vb;
 char *pc;
 unsigned size = (argc > 1)? atoi(argv[1]) : 0;
 teststr = (char *)malloc(2000);
 if (1)
   {
     vb = new VFile(size);
     vb->openVF("TEST.SCR"); // find help under ios

     fnd = vb->szSearch("inherited",0,VFile::up,0);

     fnd = vb->szSearch("the end()",0,VFile::up,0);
     vb->readVF(teststr,vb->searchResultPtr() ,20);


     vb->readVF(teststr,0,53);
     vb->readVF(teststr,200,53);
     vb->readVF(teststr,19996,100,VFile::up);
     vb->readVF(teststr,19996,100,VFile::down);
     vb->readVF(teststr,50540,100,VFile::down);

     vb->readVF(teststr,100,100);
     vb->writeVF("ANTON",5,102,VFile::up);          // store ANTON at index 100 with l=5
     vb->readVF(teststr,100,100);
     vb->readVF(teststr,102,10,VFile::down); // !! get from index 100
     vb->readVF(teststr,100,100);

     fnd = vb->szSearch("the end()",0,VFile::up,0);
     vb->readVF(teststr,vb->searchResultPtr(),20);

     fnd = vb->szSearch("ANTON",vb->searchResultPtr(),VFile::down,0);
     vb->readVF(teststr,vb->searchResultPtr(),20);
     vb->readVF(teststr,100,100);

     vb->insert("KAREL",5,107);
     vb->readVF(teststr,100,100);
     fnd = vb->szSearch("the end()",0,VFile::up,0);
     vb->readVF(teststr,vb->searchResultPtr(),20);

     fnd = vb->szSearch("ANTON",vb->searchResultPtr(),VFile::down,0);
     vb->readVF(teststr,vb->searchResultPtr(),20);

     vb->removeVF(5,102);
     vb->readVF(teststr,100,100);
     fnd = vb->szSearch("the end()",0,VFile::up,0);
     vb->readVF(teststr,vb->searchResultPtr(),20);

     fnd = vb->szSearch("KAREL",vb->searchResultPtr(),VFile::down,0);
     vb->readVF(teststr,vb->searchResultPtr(),20);

     vb->replace(vb->searchResultPtr(),5,"Karel Karelsen",14);
     vb->readVF(teststr,100,100);
     vb->readVF(teststr,vb->searchResultPtr(),20);
     fnd = vb->szSearch("the end()",0,VFile::up,0);
     fnd = vb->szSearch("Karel K",vb->searchResultPtr(),VFile::down,0);
     vb->replace(vb->searchResultPtr(),14,"Nelis",5);

     vb->readVF(teststr,100,100);
     vb->replaceAll("SET",3,"PETER",5,0);
     vb->closeVF();

     delete vb;
     VFLine line;
     line.openVF("TEST.SCR");
     line.readVF(2);
     line.readVF(4);
     line.move(7);
     line.move(-7);
     line.move(-4);
     line.readVF(20);
     line.readVF(21);
     line.readVF(18);
     line.readVF(22);
     line.readVF(462);
     line.readVF(463);
     line.readVF(18);
     line.readVF(1);
     unsigned c = line.lineCount();
     int i;
     for (i = 1, c = 0; i = line.search("REM",i) ; i++)
        c++;
   //  line.deleteVF();
//     line.append("The last line",strlen("The last line"));
     line.appendString("The last line");
     c = line.lineCount();
     line.readVF(1);
     line.last();

     VFLine lineT;
     lineT.saveAs("TEMP1.OUT");
     for (i = 1; line.readVF(i); i++)
       {
         sprintf(teststr,"T%04d%s",i,line.stringData());
         lineT.appendString(teststr);
       }
     lineT.saveAs("TEMP2.OUT");

     strcpy(teststr,"First line into the Temp File?");
     lineT.insertLine(teststr,strlen(teststr),1);
     lineT.saveAs("TEMP3.OUT");
     strcpy(teststr,"Last Line into the Temp File?");
     lineT.insertLine(teststr,strlen(teststr),100000); // offset too high - ignored
     lineT.appendString(teststr);
     lineT.saveAs("TEMP4.OUT");

     VFLine lineOut;
   //lineOut.openVF("TEST.OUT",VFLine::VF_ReadWriteNew);
     lineOut.openVF("TEST.OUT");
   #if 0
     for (i = 1; lineT.readVF(i); i++)
       {
         lineT.strCopy(teststr);
         lineOut.append(teststr,strlen(teststr));
       }
   #endif
     for (i = 1; lineT.readVF(i); i++)
       lineOut.appendString(lineT.stringData());

     strcpy(teststr,"Is this the first line in the OUT file?");
   //  lineOut.readVF(1);
     lineOut.insertLine(teststr,strlen(teststr),1);
     strcpy(teststr,"Is this the last line in the OUT File?");
     lineOut.appendString(teststr);


     lineT.closeVF();
     lineOut.closeVF();
   ///////////////////////////////////////
   #if 0
     TestStruc * pRec;
     VFRec vfrec(sizeof(*pRec));
     if (vfrec.openVF("RECTEST.WRT",VFLine::VF_ReadWriteOver))
       {
        for ( pRec = TestTable; *pRec->Name ; pRec++)
          vfrec.append(pRec);
        vfrec.closeVF();
        if (vfrec.openVF("RECTEST.WRT"))
          { pRec = (TestStruc *)teststr;
            for (i = 1; vfrec.readVF(i); i++)
               vfrec.copy(pRec);

            pRec = &TestTable[6];
            if (vfrec.search(pRec,sizeof(*pRec)))
              { vfrec.deleteVF();
                vfrec.append(pRec);
                vfrec.insert(pRec,2);
              }
            vfrec.closeVF();
          }
       }
   #endif

     TestStruc rec;
     TestStruc * pRec = &rec;
     rec.f = 0;
     VFRec vfrec(sizeof(*pRec));
     if (vfrec.openVF("RECTEST.WRT",VFLine::VF_ReadWriteOver))
       {
        for (i = 1; i <= 9999; i++)
          { sprintf(rec.Name,"Record  %05d",i);
            rec.i = i;
            vfrec.append(pRec);
          }
        vfrec.closeVF();
        if (vfrec.openVF("RECTEST.WRT"))
          {
            for (i = 1; vfrec.readVF(i); i++)
               vfrec.copy(pRec);
            i = 7;
            sprintf(rec.Name,"Record  %05d",i);
            rec.i = i;
            if (vfrec.search(pRec,sizeof(*pRec)))
              { vfrec.deleteVF();
                vfrec.append(pRec);
                vfrec.insert(pRec,2);
              }
            vfrec.closeVF();
          }
       }
   }
  free(teststr);
#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif

};

#endif
//*/

#if 0
#endif







