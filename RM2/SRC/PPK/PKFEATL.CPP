 /*--------------------------------------------------------------------------*\
 |    Filename - PKFEATL.CPP                                                  |
 |      contains source for the following RM/2 class implementations:         |
 |                                                                            |
 |     PKArr    - Product Knowledge Array.                                    |
 |                An array of pointers to fixed length records                |
 |                (PtrRecArr) with the added ability to have the              |
 |                array point to records in a contiguous block of             |
 |                storage. This ability is utilized when the                  |
 |                Product Knowledge is retrieved from its 'compiled'          |
 |                version file, to cut down on processing time.               |
 |                This object is the base of some of the following:           |
 |                                                                            |
 |   PKFeatList - An array of PKFeat pointers as described in PKFEAT.HPP      |
 |                sorted in a special way such that the features              |
 |                having Type and Model fields containing wildcard            |
 |                characters, are ranked behind specific-ones.                |
 |                Thus, using a linear search, the more specific entries      |
 |                are found first.                                            |
 |                                                                            |
 |   PKFeatDescList - An array of PKFeatDesc pointers (features with their    |
 |                    machine type and model and their descriptions, as       |
 |                    described in PKFEAT.HPP) sorted like 'PKFeatList'       |
 |                                                                            |
 |   PKFeatSS       - A regularly sorted collection of PKFeatDesc objects.    |
 |                                                                            |
 |   PKNonMRList    - An array of PKBase object pointers representing a list  |
 |                    of Non-machinereadable features.                        |
 |                                                                            |
 |   SpecificFeat   - An object to manipulate a feature and its description   |
 |                                                                            |
 |   SpecificFeatList- A collection of SpecificFeat objects for a             |
 |                     particular generic feature (type-model-feature)        |
 |                                                                            |
 |   ProductKnowledge - An object to retain and provide the System's          |
 |                      knowledge, containing:                                |
 |                       Product Knowledge Feature List                       |
 |                          currently parsed from FEATURE.TXT                 |
 |                       Product Knowledge Generic Feature List               |
 |                          currently parsed from GENERIC.TXT                 |
 |                       System's Machine Readable Feature Description List   |
 |                          this is different for every system being          |
 |                          resolved and is parsed (in SYSTEM.CPP) from       |
 |                          the "System".DSC file downloaded with the         |
 |                          System's AAS and MRDB files.                      |
 |                       Non-Machinereadable Feature List                     |
 |                          a byproduct from generating the                   |
 |                          PK Feature List.                                  |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                  ICLUI's Includes                   |
        \*---------------------------------------------------*/
#include <isynonym.hpp>                          // Definition of Booleans
#include <istring.hpp>

        /*---------------------------------------------------*\
        |               RM/2 specific Includes                |
        \*---------------------------------------------------*/
#include "euoutil.hpp"                           // utilities
#include "pkfeatl.hpp"                           // this file's declarations
#include "euomsg.h"                              // message ids

//#define INCL_WIN
//#include <os2.h>

        /*---------------------------------------------------*\
        |                   static strings                    |
        \*---------------------------------------------------*/
char *pNoDesc = "[Description Unavailable]";
char *pkNull = "NULL";

        /*---------------------------------------------------*\
        |                       defines                       |
        \*---------------------------------------------------*/
#define MAX_LINE_SIZE 257

        /*---------------------------------------------------*\
        | 'C' compare functions called by the array objects   |
        \*---------------------------------------------------*/
int PKFeat_comp(const void *p1,const void * p2)
{
 return ((const PKFeat *)p1)->compare(*(const PKFeat *)p2);
}
    /*-----------------------------------------------------------------------*\
    |   int PKFeat_compSort()                                                 |
    |      Call PKFeat's compare method which ranks the                       |
    |       specific Models before those containing wildcards                 |
    \*-----------------------------------------------------------------------*/
int PKFeat_compSort(const void *p1,const void * p2)
{
 return ((const PKFeat *)p1)->compareSort(*(const PKFeat *)p2);
}

    /*-----------------------------------------------------------------------*\
    |   int PKFeatDesc_comp()                                                 |
    |      Call PKFeatDesc's compare method which orders                      |
    |       by machine-model-feature                                          |
    \*-----------------------------------------------------------------------*/
int PKFeatDesc_comp(const void *p1,const void * p2)
{
 return ((const PKFeatDesc *)p1)->compare(*(const PKFeatDesc *)p2);
}

int PKBase_comp(const void *p1,const void * p2)
{
 return ((const PKBase *)p1)->compBase(*(const PKBase *)p2);
}

    /*-----------------------------------------------------------------------*\
    |   int PKFeat_compSort()                                                 |
    |      Call PKBase's compare method which ranks the                       |
    |       specific Models before those containing wildcards                 |
    \*-----------------------------------------------------------------------*/
int PKBase_compSort(const void *p1,const void * p2)
{
 return ((const PKBase *)p1)->compBaseSort(*(const PKBase *)p2);
}

int PKBase_compFind(const void *p1,const void * p2)
{
 return ((const PKBase *)p1)->compBaseFind(*(const PKBase *)p2);
}


 /*--------------------------------------------------------------------------*\
 |                                PKArr Class                                 |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |                        Ctor                         |
        \*---------------------------------------------------*/
PKArr :: PKArr( unsigned lrecl, unsigned init, unsigned exp,comp_fn_tp fn)
       : PtrRecArr(lrecl,init,exp,fn)
        ,PointBuffer(0)
{}

        /*---------------------------------------------------*\
        |                        Dtor                         |
        \*---------------------------------------------------*/
PKArr :: ~PKArr()
{
 releaseBuffer();
}

    /*-----------------------------------------------------------------------*\
    |   Boolean PKArr::releaseBuffer()                                        |
    |       If the array was loaded via 'pointToMem()', free the              |
    |       contiguous block of storage and idicate whether that              |
    |       was the case.                                                     |
    \*-----------------------------------------------------------------------*/
Boolean PKArr::releaseBuffer()
{
 if (PointBuffer) {
   delete[] PointBuffer;
   PointBuffer = 0;
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean  pointToMem( char *Storage, unsigned count)                   |
    |       Load the array with the given number of addresses                 |
    |       pointing to the fixed length records contained in                 |
    |       the given contiguous block of memory                              |
    \*-----------------------------------------------------------------------*/
Boolean  PKArr::pointToMem( char *Storage, unsigned count)
{
                                                 // release possible previous
 releaseBuffer();                                // block
 PointBuffer = Storage;
 return (Point(Storage,count) == PARR_OK);
}

 /*--------------------------------------------------------------------------*\
 |                              PKFeatList Class                              |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |                        Ctor                         |
        \*---------------------------------------------------*/
PKFeatList :: PKFeatList()
            : PKArr(sizeof(PKFeat),1000,100)
{}

PKFeatList ::~PKFeatList()
{
// release();
}

    /*-----------------------------------------------------------------------*\
    |    PKFeat * element(unsigned i)                                         |
    |       Return the i-th ( 0 based ) array element.                        |
    \*-----------------------------------------------------------------------*/
PKFeat * PKFeatList::element(unsigned i)
{
 return (PKFeat *)(PtrArray::Element(i));
}

    /*-----------------------------------------------------------------------*\
    |    void release()                                                       |
    |       free the obtained storage associated with the                     |
    |       elements of the array:                                            |
    |         Free the contiguous storage if the array was loaded             |
    |         via 'pointToMem()' otherwise, delete the individual             |
    |         addresses.                                                      |
    \*-----------------------------------------------------------------------*/
void PKFeatList::release()
{
 PKFeat * p;
 if (!releaseBuffer())                           // not a contiguous block
   for (int i = 0 , N = NumberElements(); i < N ;i++ ) {
     p = element(i);
     delete p;
//     delete element(i);
     }
 EmptyOut();
}

    /*-----------------------------------------------------------------------*\
    |   Boolean quantityWord(const IString & word, unsigned *pQty)            |
    |       Set the quantity at the given address from a string               |
    |       surrouneded by a left and right banana and indicate               |
    |       success.                                                          |
    \*-----------------------------------------------------------------------*/
Boolean PKFeatList :: quantityWord(const IString & word, unsigned *pQty)
{
 char Buf[FEAT_SIZE + 1];
 mystrncpy(Buf,(char *)word,sizeof(Buf)-1);
 if ((*Buf == '(') && (Buf[strlen(Buf)-1] == ')')) {
   Buf[strlen(Buf)-1] = 0;
   *pQty = IString(Buf+1).asInt();
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean readFile(char * File, PKNonMRList * pNonMRList)               |
    |       Build the array from the given Feature Product Knowledge          |
    |       ASCII file and optionally add the Non-Machinereadable             |
    |       features to the given list, as follows:                           |
    |                                                                         |
    |          Strip the comments                                             |
    |          Separate the quantities                                        |
    |            (i.e. some entries come in like '1203(2)'                    |
    |             which IString's 'word()' considers one word)                |
    |          Translate the Product Knowledge file's idea of wild            |
    |            character '_' to our (and IString's) '?'                     |
    |            (their '*' is the same)                                      |
    |          Process the file entries according to the number of            |
    |            words they contain:                                          |
    |             2:  Don't know what this means. A featureless               |
    |                 feature? Just add.                                      |
    |             3:  Add to the list and since there is no fourth            |
    |                 field (Generic Name), add it to the                     |
    |                 Non-Machinereadable Feature List as well,               |
    |                 which will be the source to determine whether           |
    |                 a feature is 'generic' since we assume that             |
    |                 that list will be shorter than picking out              |
    |                 the generics in the list we're building.                |
    |             4:  Add this generic feature with the assumed               |
    |                 quantity of 1.                                          |
    |             5:  Depending on the contents of the 5th field,             |
    |                 add this entry as either one with                       |
    |                     2 generic features and their assumed                |
    |                          quantities of 1 -OR-                           |
    |                     a generic feature and its quantity                  |
    |             6:  If the 5th field is a quantity field                    |
    |                    add the entry with the first generic and             |
    |                    its quantity and the second generic with             |
    |                    an assumed quantity of 1.                            |
    |                 Otherwise,                                              |
    |                    If the 6th field is a quantity field                 |
    |                        add the entry with the first generic having      |
    |                        the assumed quantity of 1 and                    |
    |                        the second generic with its quantity.            |
    |                    Otherwise, skip the entry.                           |
    |             7:  Only add an entry with 2 generic features and           |
    |                    their quantities if the fields 5 and 7 are           |
    |                    quantity fields, otherwise, skip                     |
    |          Sort the array (since there is no order on                     |
    |            them) using the compare functions that rank                  |
    |            specific Models before those containing wildcards.           |
    |                                                                         |
    |          Note: a typical entry:                                         |
    |          9406 *   6237    6130 6031(2)   # 9406 *** ACTION #103         |
    |           translation: for type Machine 9406, any model,                |
    |                        feature 62379 equates to a combination           |
    |                        of 1 sensable feature 6130 and                   |
    |                           2 sensable features 6031                      |
    \*-----------------------------------------------------------------------*/
Boolean PKFeatList :: readFile(char * File, PKNonMRList * pNonMRList)
{
 FILE *f;
 PKFeat * pF;
 unsigned qty,qty2;
 Boolean fOK = false;
 char *Buff = new char[MAX_LINE_SIZE],           // minimize stack size
      *pc;
 if (Buff) {
   if (fOK = ( (f = fopen(File,"r")) != 0)) {
     while ( fgets(Buff, MAX_LINE_SIZE ,f)) {
//         mystrrearstrip(Buff);
       if (pc = strchr(Buff,'#'))                // strip comments
         *pc = '\0';
       stringreplace(Buff,"("," (",MAX_LINE_SIZE);
       stringreplace(Buff,")",") ",MAX_LINE_SIZE);
       IString str(Buff);
       pF = 0;
       IString w1 = str.word(1).translate("_","?");
       IString w2 = str.word(2).translate("_","?");
       switch ( str.numWords()) {
         case 2:
           pF = new PKFeat((char *)w1,(char *)w2);
           break;
         case 3:
           pF = new PKFeat((char *)w1,(char *)w2,(char *)str.word(3));
           if (pNonMRList)
             pNonMRList->Add(pF);
           break;
         case 4:
           pF = new PKFeat((char *)w1,(char *)w2,
                           (char *)str.word(3),(char *)str.word(4),1);
           break;
         case 5:
           if (quantityWord(str.word(5),&qty))
               pF = new PKFeat((char *)w1,(char *)w2,
                               (char *)str.word(3),(char *)str.word(4),qty);
           else pF = new PKFeat((char *)w1,(char *)w2,
                                (char *)str.word(3),(char *)str.word(4),
                                1,(char*)str.word(5),1);
           break;
         case 6:
           if (quantityWord(IString(str.word(5)),&qty))
               pF = new PKFeat((char *)w1,(char *)w2,
                               (char *)str.word(3),(char *)str.word(4), qty,
                               str.word(6),1);
           else if (quantityWord(IString(str.word(6)),&qty))
                    pF = new PKFeat((char *)w1,(char *)w2,
                                    (char *)str.word(3),(char *)str.word(4)
                                   ,1, str.word(5),qty);
           break;
         case 7:
           if (quantityWord(IString(str.word(5)),&qty) &&
               quantityWord(IString(str.word(7)),&qty2))
              pF = new PKFeat((char *)w1,(char *)w2,
                              (char *)str.word(3),(char *)str.word(4), qty,
                              str.word(6),qty2);
           break;
         }
       if (pF)
         AddasLast(pF);
       }
     fclose(f);
     }
   delete[] Buff;
   QSort(PKFeat_compSort);
   if (pNonMRList)
     pNonMRList->QSort(PKBase_compSort);
   }
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean readFile( char * File, Tellem * pTellem ,                     |
    |                     PKNonMRList * pNonMRList)                           |
    |                                                                         |
    |   Same method as above with the added capability of reading             |
    |       nested files (those '# included')                                 |
    |       The 'pTellem' parm, if given,  is used to report on               |
    |       '# included' files which can't be read.                           |
    |       Not being able to read the given file is considered an error      |
    |         while the absence or other inability to read the                |
    |         '#included' file(s) is merely reported.                         |
    |                                                                         |
    |       The method retains a list of files to be read to which            |
    |       '#include' files are added as they are encountered.               |
    \*-----------------------------------------------------------------------*/
Boolean PKFeatList :: readFile( char * File, Tellem * pTellem ,
                                PKNonMRList * pNonMRList)
{
 FILE *f;
 PKFeat * pF;
 unsigned qty,qty2;
 Boolean fFirstRead = false;
 char *Buff = new char[MAX_LINE_SIZE], *pc;
 IString strFilesToBeRead(File),
         strPathName(PathNameOnly(File)),
         strThisFile;
 if (Buff) {
   if (strPathName.length())
     strPathName += IString('\\');
   while (strFilesToBeRead.numWords()) {
     strThisFile = strFilesToBeRead.word(1);
     strFilesToBeRead.removeWords(1,1);
     if ( (f = fopen((char *)strThisFile,"r")) != 0) {
       if (!fFirstRead)
         fFirstRead = true;
       while ( fgets(Buff, MAX_LINE_SIZE ,f)) {
         if (pc = strchr(Buff,'#'))
           *pc = '\0';
         stringreplace(Buff,"("," (",MAX_LINE_SIZE);
         stringreplace(Buff,")",") ",MAX_LINE_SIZE);
         IString str(Buff);
         IString w1 = str.word(1);
         IString w2 = str.word(2);
         if (w1.upperCase() == IString("!INCLUDE"))
           strFilesToBeRead += IString(' ') + strPathName + w2.strip('"');
         else {
           pF = 0;
           w1.translate("_","?");
           w2.translate("_","?");
           switch ( str.numWords()) {
             case 2:
               pF = new PKFeat((char *)w1,(char *)w2);
               break;
             case 3:
               pF = new PKFeat((char *)w1,(char *)w2,(char *)str.word(3));
               if (pNonMRList)
                 pNonMRList->Add(pF);
               break;
             case 4:
               pF = new PKFeat((char *)w1,(char *)w2,
                               (char *)str.word(3),(char *)str.word(4),1);
               break;
             case 5:
               if (quantityWord(str.word(5),&qty))
                  pF = new PKFeat((char *)w1,(char *)w2,
                                  (char *)str.word(3),(char *)str.word(4),qty);
               else pF = new PKFeat((char *)w1,(char *)w2,
                                    (char *)str.word(3),(char *)str.word(4),
                                    1,(char*)str.word(5),1);
               break;
             case 6:
               if (quantityWord(IString(str.word(5)),&qty))
                  pF = new PKFeat((char *)w1,(char *)w2,
                                  (char *)str.word(3),(char *)str.word(4), qty,
                                   str.word(6),1);
               else if (quantityWord(IString(str.word(6)),&qty))
                       pF = new PKFeat((char *)w1,(char *)w2,
                                       (char *)str.word(3),(char *)str.word(4)
                                       ,1, str.word(5),qty);
               break;
             case 7:
               if (quantityWord(IString(str.word(5)),&qty) &&
                   quantityWord(IString(str.word(7)),&qty2))
                  pF = new PKFeat((char *)w1,(char *)w2,
                                  (char *)str.word(3),(char *)str.word(4), qty,
                                  str.word(6),qty2);
               break;
             }
           if (pF)
             AddasLast(pF);
           }
         }
        fclose(f);
       }
     else {
       if (!fFirstRead)
         strFilesToBeRead = IString();           // get out of loop
       else if (pTellem)
              pTellem->say(false,EUO_MSG_NO_FILE,strThisFile);
            else cout << "Unable to read file '" << strThisFile << "'" << endl;
       }
     }
   delete[] Buff;
   QSort(PKFeat_compSort);
//   WinAlarm(HWND_DESKTOP, WA_NOTE);
   if (pNonMRList)
     pNonMRList->QSort(PKBase_compSort);
   }
 return fFirstRead;
}


    /*-----------------------------------------------------------------------*\
    |   Boolean fReadB(FILE *f, unsigned count)                               |
    |       Re-load the with array given number of elements from the current  |
    |       position of the given binary file by allocating                   |
    |       a contiguous block of storage, reading the records                |
    |       into it and having the addresses in the array point to            |
    |       the individual records.                                           |
    \*-----------------------------------------------------------------------*/
Boolean PKFeatList :: fReadB(FILE *f, unsigned count)
{
 unsigned len = (count *LRecl());
 char * Buf = new char[len];
 if (Buf)
   if (fread(Buf,len,1,f) == 1) {
     release();
     pointToMem(Buf,count);
     return true;
     }
   else delete[] Buf;
 return false;
}

 /*--------------------------------------------------------------------------*\
 |                            PKFeatDescList Class                            |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |                        Ctor                         |
        \*---------------------------------------------------*/
PKFeatDescList :: PKFeatDescList()
                : PKArr(sizeof(PKFeatDesc),200,50,PKFeatDesc_comp)
{}

PKFeatDescList ::~PKFeatDescList()
{
// release();
}

    /*-----------------------------------------------------------------------*\
    |    PKFeatDesc * element(unsigned i)                                     |
    |       Return the i-th ( 0 based ) array element.                        |
    \*-----------------------------------------------------------------------*/
PKFeatDesc * PKFeatDescList::element(unsigned i)
{
 return (PKFeatDesc *)(PtrArray::Element(i));
}

    /*-----------------------------------------------------------------------*\
    |    void release()                                                       |
    |       free the obtained storage associated with the                     |
    |       elements of the array:                                            |
    |         Free the contiguous storage if the array was loaded             |
    |         via 'pointToMem()' otherwise, delete the individual             |
    |         addresses.                                                      |
    \*-----------------------------------------------------------------------*/
void PKFeatDescList::release()
{
 PKFeatDesc *p;
 if (!releaseBuffer())
   for (int i = 0 , N = NumberElements(); i < N ;i++ ) {
     p = element(i);
     delete p;
//      delete element(i);
     }
 EmptyOut();
}

    /*-----------------------------------------------------------------------*\
    |   Boolean readFile(char * File)                                         |
    |       Build the array from the given Product Knowledge                  |
    |       Feature Description ASCII file, as follows:                       |
    |                                                                         |
    |          Strip the line's comments and blanks.                          |
    |          Consider only lines with 4 or more words as follows:           |
    |            word 1 :  Type (machine)                                     |
    |                 2 :  Model                                              |
    |                 3 :  Feature                                            |
    |                 4 and up: Description                                   |
    |          Sort the array in machine-model-feature order.                 |
    |                                                                         |
    |          Note: a typical entry:                                         |
    |           7016 *   2790   'LIC' PWR GT4X 8-BIT FEAT                     |
    \*-----------------------------------------------------------------------*/
Boolean PKFeatDescList :: readFile(char * File)
{
 FILE *f;
 PKFeatDesc * pFD;
 int i;
 char *Buff = new char[MAX_LINE_SIZE], *pc;
 Boolean fOK = false;
 if (Buff) {
   if (fOK = ((f = fopen(File,"r")) != 0)) {
     while ( fgets(Buff, MAX_LINE_SIZE,f)) {
       if (pc = strchr(Buff,'#'))                // strip comments
         *pc = '\0';
       mystrrearstrip(Buff);
       IString str(Buff);
       if (( ( i = str.numWords()) >= 4)         &&
           ( pFD = new PKFeatDesc( (char *)str.word(1)
                                  ,(char *)str.word(2)
                                  ,(char *)str.word(3)
                                  ,(char *)str.words(4,i-3))))
         Add(pFD);
       }
     fclose(f);
     }
   delete[] Buff;
   QSort(PKFeatDesc_comp);                       // think it's unnecessary
   }
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean readFile(char * File, Tellem * pTellem )                      |
    |                                                                         |
    |   Same method as above with the added capability of reading             |
    |       nested files (those '# included')                                 |
    |       The 'pTellem' parm, if given,  is used to report on               |
    |       '# included' files which can't be read.                           |
    |       Not being able to read the given file is considered an error      |
    |         while the absence or other inability to read the                |
    |         '#included' file(s) is merely reported.                         |
    |                                                                         |
    |       The method retains a list of files to be read to which            |
    |       '#include' files are added as they are encountered.               |
    \*-----------------------------------------------------------------------*/
Boolean PKFeatDescList :: readFile(char * File, Tellem * pTellem )
{
 FILE *f;
 PKFeatDesc * pFD;
 int i;
 Boolean fFirstRead = false;
 char *Buff = new char[MAX_LINE_SIZE], *pc;
 IString strFilesToBeRead(File),
         strPathName(PathNameOnly(File)),
         strThisFile;
 if (Buff) {
   if (strPathName.length())
     strPathName += IString('\\');
   while (strFilesToBeRead.numWords()) {
     strThisFile = strFilesToBeRead.word(1);
     strFilesToBeRead.removeWords(1,1);
     if ( (f = fopen((char *)strThisFile,"r")) != 0) {
       if (!fFirstRead)
         fFirstRead = true;
       while ( fgets(Buff, MAX_LINE_SIZE ,f)) {
         mystrrearstrip(Buff);
         if (pc = strchr(Buff,'#'))
            *pc = '\0';
         IString str(Buff);
         if (str.word(1).upperCase() == IString("!INCLUDE"))
           strFilesToBeRead += IString(' ') + strPathName + str.word(2).strip('"');
         else if (( ( i = str.numWords()) >= 4)         &&
                  ( pFD = new PKFeatDesc( (char *)str.word(1)
                                         ,(char *)str.word(2)
                                         ,(char *)str.word(3)
                                        ,(char *)str.words(4,i-3))))
                Add(pFD);
         }
        fclose(f);
       }
     else {
       if (!fFirstRead)
         strFilesToBeRead = IString();           // get out of loop
       else if (pTellem)
              pTellem->say(false,EUO_MSG_NO_FILE,strThisFile);
            else cout << "Unable to read file '" << strThisFile << "'" << endl;
       }
     }
   delete[] Buff;
   }
 return fFirstRead;
}


    /*-----------------------------------------------------------------------*\
    |   Boolean fReadB(FILE *f, unsigned count)                               |
    |       Re-load the array with given number of elements from the current  |
    |       position of the given binary file by allocating                   |
    |       a contiguous block of storage, reading the records                |
    |       into it and having the addresses in the array point to            |
    |       the individual records.                                           |
    \*-----------------------------------------------------------------------*/
Boolean PKFeatDescList :: fReadB(FILE *f, unsigned count)
{
 unsigned len = (count *LRecl());
 char * Buf = new char[len];
 if (Buf)
   if (fread(Buf,len,1,f) == 1) {
     release();
     pointToMem(Buf,count);
     return true;
     }
   else delete[] Buf;
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   const PKFeatDesc * findLike( )                                        |
    |       Linearly search the array allowing for wildcard                   |
    |       characters for Type and Model, returning                          |
    |       the pointer when found                                            |
    \*-----------------------------------------------------------------------*/
const PKFeatDesc * PKFeatDescList::findLike( const char * type
                                            ,const char * model
                                            ,const char * feature)
{
 PKFeatDesc * p;
 for (int n = 0, N = NumberElements() ; n < N ; n++)
   if ((p = element(n)) && p->isLike(type,model,feature))
      return p;
 return 0;
}


    /*-----------------------------------------------------------------------*\
    |   const char * description( )                                           |
    |       Binary search to return the description.                          |
    |       Handles special case of substituting 'NULL'                       |
    |       for non-given features (feature-less machines).                   |
    |       Uses the array's Find() method which uses                         |
    |       the compare function stored with the array                        |
    |       which compares straight (no wildcard chars)                       |
    |       type-model-feature's                                              |
    |       or '[Description Unavailable]' when not found.                    |
    \*-----------------------------------------------------------------------*/
const char * PKFeatDescList::description( const char * type
                                         ,const char * model
                                         ,const char * feature)
{
 if (!feature || !IString::stripBlanks(IString(feature)).length())
   feature = pkNull;
 PKFeatDesc d(type,model,feature,0);
 void * pv = Find(&d);
 if (pv)
   return ((PKFeatDesc *)pv)->description();
// return 0;
 return pNoDesc;
}

    /*-----------------------------------------------------------------------*\
    |   const char * descriptionLike( )                                       |
    |       Return the description for the given feature                      |
    |       allowing for wildcard characters for model and type.              |
    \*-----------------------------------------------------------------------*/
const char * PKFeatDescList::descriptionLike( const char * type
                                             ,const char * model
                                             ,const char * feature)
{
 const PKFeatDesc *p = findLike(type,model,feature);
 if (p)
   return (p->description());
 return 0;
}

 /*--------------------------------------------------------------------------*\
 |                             PKNonMRList Class                              |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |                        Ctor                         |
        \*---------------------------------------------------*/
PKNonMRList :: PKNonMRList()
             : PKArr(sizeof(PKBase),100,25)
{}

    /*-----------------------------------------------------------------------*\
    |    PKBase * element(unsigned i)                                         |
    |       Return the i-th ( 0 based ) array element.                        |
    \*-----------------------------------------------------------------------*/
PKBase * PKNonMRList::element(unsigned i)
{
 return (PKBase *)(PtrArray::Element(i));
}

    /*-----------------------------------------------------------------------*\
    |    Boolean contains( )                                                  |
    |       Indicates whether the given feature is in the                     |
    |       list allowing for wildcard characters in the                      |
    |       list elements                                                     |
    \*-----------------------------------------------------------------------*/
Boolean PKNonMRList::contains( const char * type
                              ,const char * model
                              ,const char * feature)
{
 for (int n = 0, N = NumberElements() ; n < N ; n++)
   if (element(n)->isLike(type,model,feature))
     return true;
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean fReadB(FILE *f, unsigned count)                               |
    |       Re-load the array with given number of elements from the current  |
    |       position of the given binary file by allocating                   |
    |       a contiguous block of storage, reading the records                |
    |       into it and having the addresses in the array point to            |
    |       the individual records.                                           |
    \*-----------------------------------------------------------------------*/
Boolean PKNonMRList :: fReadB(FILE *f, unsigned count)
{
 unsigned len = (count *LRecl());
 char * Buf = new char[len];
 if (Buf)
   if (fread(Buf,len,1,f) == 1) {
     pointToMem(Buf,count);
     return true;
     }
   else delete[] Buf;
 return false;
}


 /*--------------------------------------------------------------------------*\
 |                             SpecificFeat Class                             |
 \*--------------------------------------------------------------------------*/
    /*------------------------------------------------------*\
    |                       Assignment                       |
    \*------------------------------------------------------*/
SpecificFeat & SpecificFeat::operator=(SpecificFeat const& other)
{
 if (this != &other) {
    Code  = other.Code;
    Desc = other.Desc;
    }
 return *this;
}

 /*--------------------------------------------------------------------------*\
 |                           ProductKnowledge Class                           |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |                        Ctor                         |
        \*---------------------------------------------------*/
ProductKnowledge :: ProductKnowledge()
                  : pMdbFDL(0)
                   ,fUsingCompiledPK(false)
{
 Buff = new char[128];
}

        /*---------------------------------------------------*\
        |                        Dtor                         |
        \*---------------------------------------------------*/
ProductKnowledge :: ~ProductKnowledge()
{
 PKFL.release();                                 // free obtained storage
 PKGenFDL.release();
 delete[] Buff;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean build(char * szPKFeatFile ,char *szPKGenDescFile)             |
    |       Build the object from the given Product Knowledge Feature and     |
    |       Generic-Description ACII files if restoring the object            |
    |       from a file containing the stored version is unsuccessful.        |
    |       (This compiled version is looked for in a file with the           |
    |        'szPKFeatFile' name and a '$$F' extension.                       |
    \*-----------------------------------------------------------------------*/
Boolean ProductKnowledge::build(char * szPKFeatFile ,char *szPKGenDescFile)
{
 FeatFile = IString(szPKFeatFile);
 if (reBuild(MakeFileName(Buff,(char *)szPKFeatFile,"$$F")))
   if (szPKGenDescFile)
     return ( PKFL.readFile(szPKFeatFile,&PKNMRL) &&
              PKGenFDL.readFile(szPKGenDescFile));
   else return false;
 else return true;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean build(Tellem * pTellem, char * szPKFeatFile ,                 |
    |                 char *szPKGenDescFile)                                  |
    |                                                                         |
    |   Same method as above with the added capability of reading             |
    |       nested files (those '# included')                                 |
    |       The 'pTellem' parm, if given,  is used to report on               |
    |       '# included' files which can't be read.                           |
    \*-----------------------------------------------------------------------*/
Boolean ProductKnowledge::build(Tellem * pTellem, char * szPKFeatFile ,
                                char *szPKGenDescFile)
{
 FeatFile = IString(szPKFeatFile);
 if (reBuild(MakeFileName(Buff,(char *)szPKFeatFile,"$$F")))
   if (szPKGenDescFile)
     return ( PKFL.readFile(szPKFeatFile,pTellem,&PKNMRL) &&
              PKGenFDL.readFile(szPKGenDescFile,pTellem));
   else return false;
 else return true;
}

Boolean ProductKnowledge::buildX(Tellem * pTellem,
                                char * szCompiledFeatFile ,
                                char * szPKFeatFile ,
                                char *szPKGenDescFile)
{
 int rc = 0;
 if (szCompiledFeatFile && strlen(szCompiledFeatFile))
   if (rc = reBuild(szCompiledFeatFile))
     pTellem->say(false,EUO_MSG_ERR_READING_PKFILE,szCompiledFeatFile,
                  (char *) IString(rc));
   else {
   FeatFile = IString(szCompiledFeatFile);
   return true;
   }
          // try reading the compiled file delivered with the app which has the
          // name of the uncompiled Feature File and an '$$F' extension
 IString strFile(MakeFileName(Buff,(char *)szPKFeatFile,"$$F"));
 if (rc = reBuild((char *)strFile)) {
   if (rc !=  RBS_ERR_NO_FILE)
     pTellem->say(false,EUO_MSG_ERR_READING_PKFILE,(char *)strFile,
                  (char *) IString(rc));
   }
 else {
   FeatFile = strFile;
   return true;
   }
    // try reading the compiled file delivered with the app
    // in the current directory
 char * pFeatFile = (char *)findFileName((char *)strFile);
 if (pFeatFile > (char *)strFile)
   if (rc = reBuild(pFeatFile)) {
     if (rc !=  RBS_ERR_NO_FILE)
       pTellem->say(false,EUO_MSG_ERR_READING_PKFILE,pFeatFile,
                    (char *) IString(rc));
     }
   else {
     FeatFile = IString(pFeatFile);
     return true;
     }
    // try building the Product Knowledge from the uncompiled feature and
    // generic source files
 if (szPKFeatFile && szPKGenDescFile) {
   if ( PKFL.readFile(szPKFeatFile,pTellem,&PKNMRL) &&
        PKGenFDL.readFile(szPKGenDescFile,pTellem)) {
     FeatFile = IString(szPKFeatFile);
     return true;
     }
    // try building the Product Knowledge from the uncompiled feature and
    // generic source files in the current directory
   pFeatFile = (char *)findFileName(szPKFeatFile);
   if (pFeatFile > szPKFeatFile) {
     if ( PKFL.readFile(pFeatFile,pTellem,&PKNMRL) &&
          PKGenFDL.readFile((char *)findFileName(szPKGenDescFile),pTellem)) {
       FeatFile = IString(pFeatFile);
       return true;
       }
     }
   }
 return false;
}

Boolean ProductKnowledge::buildNew(char * szPKFeatFile,char *szPKGenDescFile)
{
 return ( PKFL.readFile(szPKFeatFile,0,&PKNMRL) &&
          PKGenFDL.readFile(szPKGenDescFile,0));
}


    /*-----------------------------------------------------------------------*\
    |   Boolean save( char * szFileName)                                      |
    |       Save the Product Knowledge Record Lists to the given file         |
    |       in the following format:                                          |
    |       Information about the file and every section of records           |
    |       it is to contain, is collected in an array of 'packing lists'     |
    |         as defined in EUODEF.H :                                        |
    |           struct PackList{ unsigned   Type;   // signature              |
    |                            unsigned   LRecl;  // record length          |
    |                            unsigned   Count;  // record count           |
    |                            unsigned   Offset; // file offset            |
    |                          };                                             |
    |       This array is written out at the top of the file with its         |
    |       first element containing the file's data as follows:              |
    |                        Count :  packing list array count                |
    |                        Offset:  file size                               |
    |                        LRecl :  packing list length                     |
    |       The record lists are then appended.                               |
    \*-----------------------------------------------------------------------*/
Boolean ProductKnowledge::save( char * szFileName)
{
 int i,j;
 FILE *f;
 Boolean fOK = false;
 PackList *pPL1, *pPL2;
 pPL1 = pPL2 = (PackList *)Buff;                 // room for 7 !

                                                 // set 1st (file's) packing
 pPL2->Offset = 0;                               // list data
 pPL2->Type = PL_SIGNATURE;
 pPL2->LRecl = sizeof(*pPL1);
                                                 // set the other packing
   {                                             // lists' data
                                                 // one for every set of
     pPL2++;                                     // records
     pPL2->Type = PL_PKFEATS;
     pPL2->LRecl = PKFL.LRecl();
     pPL2->Count = PKFL.NumberElements();

     pPL2++;
     pPL2->Type = PL_PKFEATSDESC;
     pPL2->LRecl = PKGenFDL.LRecl();
     pPL2->Count = PKGenFDL.NumberElements();

     pPL2++;
     pPL2->Type = PL_PKNONMRFEATS;
     pPL2->LRecl = PKNMRL.LRecl();
     pPL2->Count = PKNMRL.NumberElements();

   }
 pPL1->Count = pPL2 - pPL1 + 1;                  // number of packlists
 for (i = 1, pPL2 = pPL1+1 , j = pPL1->Count; i < j ; i++,pPL1++,pPL2++)
                                                 // set the offsets
   pPL2->Offset = pPL1->Offset + (pPL1->Count * pPL1->LRecl);
 pPL2 = (PackList *)Buff;                        // go back to 1st elment
                                                 // set filesize
 pPL2->Offset = pPL1->Offset + (pPL1->Count * pPL1->LRecl);

 if (!szFileName)
   MakeFileName(szFileName = (Buff + 112),(char *)FeatFile,"$$F");
 FeatFile = IString(szFileName);
 if (f=fopen(szFileName,"wb")) {
   fwrite(Buff,(++pPL2)->Offset,1,f);            // write packing list array
      {                                          // and the record lists
        fOK = (PKFL.WriteToFile(f)         &&
               PKGenFDL.WriteToFile(f)     &&
               PKNMRL.WriteToFile(f)
              );
      }
   fclose(f);
   }
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |   int reBuild(char * szFileName)                                        |
    |       Restore the Product Knowledge's Lists from the                    |
    |       given file to have been written by the 'save()' method.           |
    |                                                                         |
    |       Information about the file and every section of records           |
    |       it contains, is stored in an array of 'packing lists'             |
    |         as defined in EUODEF.H :                                        |
    |           struct PackList{ unsigned   Type;   // signature              |
    |                            unsigned   LRecl;  // record length          |
    |                            unsigned   Count;  // record count           |
    |                            unsigned   Offset; // file offset            |
    |                          };                                             |
    |       This array is written out at the top of the file with its         |
    |       first element containing the file's data as follows:              |
    |                        Count :  packing list array count                |
    |                        Offset:  file size                               |
    |                        LRecl :  packing list length                     |
    |                                                                         |
    |       Read the packing list array, edit it and with the data            |
    |       for the file's record segments which starts with the              |
    |       second array element, call the lists' file read methods.          |
    |       Retain the fact that the object's loaded from a compiled          |
    |       PK file, if successfull and indicate success.                     |
    \*-----------------------------------------------------------------------*/
int ProductKnowledge::reBuild(char * szFileName)
{
 FILE *f;
 PackList *pPL = (PackList *)Buff;
 int rc = 0,i;
 if (!szFileName)
   MakeFileName(szFileName = Buff,(char *)FeatFile,"$$F");
 else FeatFile = IString(szFileName);
 if (f = fopen(szFileName,"rb")) {
   i = _filelength(_fileno(f));
   if ( i >= sizeof(*pPL))                       // at least a packing list
     if (fread(pPL,sizeof(*pPL),1,f) == 1)       // get first packing list
       if ( PL_SIGNATURE == pPL->Type)           // check signature
         if ( i == pPL->Offset) {                // check files size
           fseek(f,0,SEEK_SET);                  // to top of file
                                                 // read array
           if (fread(pPL,pPL->Count * pPL->LRecl,1,f) == 1) {
             for (i = 1 ; i < pPL->Count; i++)   // for all stored segments
               switch ( (pPL+i)->Type ) {
                 case PL_PKFEATS:
                   if (!PKFL.fReadB(f,(pPL+i)->Count))
                     rc = RBS_ERR_READ_FILE;
                   else  (pPL+i)->Type = 0;      // check this one off
                   break;
                 case PL_PKFEATSDESC:
                   if (!PKGenFDL.fReadB(f,(pPL+i)->Count))
                     rc = RBS_ERR_READ_FILE;
                   else  (pPL+i)->Type = 0;
                   break;
                 case PL_PKNONMRFEATS:
                   if (!PKNMRL.fReadB(f,(pPL+i)->Count))
                     rc = RBS_ERR_READ_FILE;
                   else  {
                     (pPL+i)->Type = 0;
                                                 // just wanna make sure
                     PKNMRL.QSort(PKBase_compSort);
                     }
                   break;
                 default : rc = RBS_ERR_UNKNOWN_TYPE;
                 }
             for (i = 1 ; i < pPL->Count; i++)   // got-em all?
               if ((pPL+i)->Type)
                 rc = RBS_ERR_GOTEM_ALL;
             }
           else rc = RBS_ERR_READ_FILE;
           }
         else rc =  RBS_ERR_DAMAGED_FILE;
       else rc = RBS_ERR_NO_EUOP_FILE;
     else rc = RBS_ERR_READ_FILE;
   else rc = RBS_ERR_NO_EUOP_FILE;
   fclose(f);
   }
 else rc = RBS_ERR_NO_FILE;
 fUsingCompiledPK = (rc == 0);
 return rc;
}

int ProductKnowledge :: check4CompiledPKFile(const char * szFileName)
{
 FILE *f;
 PackList PL;
 int rc = 0,i;
 if (szFileName && (f = fopen(szFileName,"rb"))) {
   i = _filelength(_fileno(f));
   if ( i >= sizeof(PackList))                       // at least a packing list
     if (fread(&PL,sizeof(PackList),1,f) == 1)       // get first packing list
       if ( PL_SIGNATURE == PL.Type) {               // check signature
         if ( i != PL.Offset)                      // check files size
           rc =  RBS_ERR_DAMAGED_FILE;
         }
       else rc = RBS_ERR_NO_EUOP_FILE;
     else rc = RBS_ERR_READ_FILE;
   else rc = RBS_ERR_NO_EUOP_FILE;
   fclose(f);
   }
 else rc = RBS_ERR_NO_FILE;
 return rc;
}

    /*-----------------------------------------------------------------------*\
    |   unsigned specificFeatures()                                           |
    |       Load the given array with a list of specific features             |
    |       equivalnet to the given generic feature and return                |
    |       the number of elements in the list.                               |
    \*-----------------------------------------------------------------------*/
unsigned ProductKnowledge::specificFeatures(
                                            const char * type
                                           ,const char * model
                                           ,const char * MRFeature
                                           ,PtrStrArr & List)
{
 PKFeat * p;
 IString strType(type);
 IString strMod(model);
 List.EmptyOut();
 for (int n = 0, N = PKFL.NumberElements() ; n < N ; n++)
   if (p = PKFL.element(n))
     if (strType.isLike(p->type()))
       if  (strMod.isLike(p->model()))
         if (!strcmp(MRFeature,p->featureMR1()))
           if (strlen(p->feature()))
             List.Add((char *)(p->feature()));
 return List.NumberElements();
}

    /*-----------------------------------------------------------------------*\
    |   unsigned specificFeatures()                                           |
    |       Look for the given AAS feature's equivalent                       |
    |       combination and set the fields if found, indicating               |
    |       success.                                                          |
    |       Note: a combination consists of multiple features                 |
    |             i.e. either a multiple of the same feature                  |
    |                  or a set of different feature(s).                      |
    \*-----------------------------------------------------------------------*/
Boolean  ProductKnowledge :: comboMRFeatFromAASFeat( const char * type
                                                    ,const char * model
                                                    ,const char * AASfeat
                                                    ,IString & MRFeat1
                                                    ,unsigned *pQty1
                                                    ,IString & MRFeat2
                                                    ,unsigned *pQty2)
{
 PKFeat * p;
 IString strType(type);
 IString strMod(model);
 for (int n = 0, N = PKFL.NumberElements() ; n < N ; n++)
   if (p = PKFL.element(n))
     if (strType.isLike(p->type()))
       if  (strMod.isLike(p->model()))
         if (!strcmp(AASfeat,p->feature()))
           if ( (p->qtyFeatureMR1() > 1 ) || p->qtyFeatureMR2() ) {
             MRFeat1 = IString(p->featureMR1());
             MRFeat2 = IString(p->featureMR2());
             *pQty1 = p->qtyFeatureMR1();
             *pQty2 = p->qtyFeatureMR2();
             return true;
             }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean machineFromMRFeature( const char * MRFeature                  |
    |       Look for the AAS machine that's equivalent                        |
    |       to the given machine-read feature and set its                     |
    |       type and model if found, indicating                               |
    |       success.                                                          |
    |       Note: the machine needs to be feature-less represented            |
    |             by 'NULL' as a feature in the PRoduct Knowledge             |
    |             Feature File and is not have wildcard characters            |
    |             in its type and model names.                                |
    \*-----------------------------------------------------------------------*/
Boolean ProductKnowledge :: machineFromMRFeature( const char * MRFeature
                                                 ,IString * pStrT
                                                 ,IString * pStrM)
{
 PKFeat * p;
 for (int n = 0, N = PKFL.NumberElements() ; n < N ; n++)
  if (p = PKFL.element(n))
   if (!strcmp(MRFeature,p->featureMR1()))
                                                 // AAS feature needs to be
    if (!strcmp(pkNull,p->feature()))            // 'NULL'
     if (!(IString(p->type()).includes("*") ||
         IString(p->type()).includes("?")   ))
      if (!(IString(p->model()).includes("*") ||
          IString(p->model()).includes("?")   )) {
       if (pStrT)
         *pStrT  = IString(p->type());
       if (pStrM)
         *pStrM = IString(p->model());
       return true;
       }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean ProductKnowledge :: isGenericMachine()                        |
    |       Determine if for the given feature-less machine                   |
    |       there is a generic feature equivalent,                            |
    |       optionally set its name and indicate success.                     |
    \*-----------------------------------------------------------------------*/
Boolean ProductKnowledge :: isGenericMachine( const char * szType
                                             ,const char *szModel
                                             ,IString * pStrGeneric)
{
 PKFeat * p;
 for (int n = 0, N = PKFL.NumberElements() ; n < N ; n++)
   if (p = PKFL.element(n))
     if (!strcmp(pkNull,p->feature()))
       if (!strcmp(szType,p->type()))
         if  (IString(szModel).isLike(p->model()))
//         if (!strcmp(szModel,p->model()))
           {  if (pStrGeneric)
                *pStrGeneric  = IString(p->featureMR1());
              return true;
           }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   unsigned genericMachines()                                            |
    |       Load the given array with a list of                               |
    |       generic features equivalent to the given                          |
    |       machine data which can contain wildcard characters                |
    |       and return the number of features loaded.                         |
    \*-----------------------------------------------------------------------*/
unsigned ProductKnowledge::genericMachines( const char * type
                                           ,const char * model
                                           ,PtrStrArr & List)
{
 PKFeat * p;
 IString strType(type);
 IString strMod(model);
 List.EmptyOut();
 for (int n = 0, N = PKFL.NumberElements() ; n < N ; n++)
   if (p = PKFL.element(n))
     if (!strcmp(pkNull,p->feature()))
       if (strType.isLike(p->type()))
         if  (strMod.isLike(p->model()))
           if (strlen(p->featureMR1()))
             List.Add((char *)(p->featureMR1()));
 return List.NumberElements();
}


    /*-----------------------------------------------------------------------*\
    |   Boolean isGenericFeature( const char * type                           |
    |       Indicate whether the given feature is generic.                    |
    |       Note: if the feature equates to a featureless                     |
    |             machine ( the PK feature field is 'NULL'                    |
    |             it's also generic and we don't even consider                |
    |             the given machine type and model.                           |
    \*-----------------------------------------------------------------------*/
Boolean ProductKnowledge::isGenericFeature( const char * type
                                           ,const char * model
                                           ,const char * feature)
{
 PKFeat * p;
 IString strType(type);
 IString strMod(model);
 for (int n = 0, N = PKFL.NumberElements() ; n < N ; n++)
   if (p = PKFL.element(n))
     if (strlen(p->featureMR1()))
       if (!strcmp(p->featureMR1(),feature))
         if (!strcmp(pkNull,p->feature()) ||
             (strType.isLike(p->type()) && strMod.isLike(p->model())))
           return true;
 return false;
}


    /*-----------------------------------------------------------------------*\
    |   Boolean strGenericDescription()                                       |
    |       Set the description for the given feature from the                |
    |       Generic Description file and indicate success.                    |
    \*-----------------------------------------------------------------------*/
Boolean   ProductKnowledge :: strGenericDescription( const char * type
                                                    ,const char * model
                                                    ,const char * feature
                                                    ,IString & str)
{
 PKFeatDesc *p;
 IString strType(type);
 IString strMod(model);
 for (int n = 0, N = PKGenFDL.NumberElements() ; n < N ; n++)
   if (p = PKGenFDL.element(n))
     if (strType.isLike(p->type()))
       if  (strMod.isLike(p->model()))
         if (!strcmp(feature,p->feature()))
           { str = IString(p->description());
             return true;
           }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |   Boolean strGenericDescription()                                       |
    |       Return the description for the given feature from the             |
    |       Generic Description file                                          |
    |       or '[Description Unavailable]' when not found.                    |
    \*-----------------------------------------------------------------------*/
const char *  ProductKnowledge :: genericDescription( const char * type
                                                     ,const char * model
                                                     ,const char * feature)
{
 PKFeatDesc *p;
 IString strType(type);
 IString strMod(model);
 for (int n = 0, N = PKGenFDL.NumberElements() ; n < N ; n++)
   if (p = PKGenFDL.element(n))
     if (strType.isLike(p->type()))
       if  (strMod.isLike(p->model()))
         if (!strcmp(feature,p->feature()))
           return (const char *)(p->description());
 return pNoDesc;
}

    /*-----------------------------------------------------------------------*\
    |   const char *  description()                                           |
    |       Return the description for the given feature from the             |
    |       current system's Machine Readable Feature Description List        |
    |       or '[Description Unavailable]' when not found.                    |
    \*-----------------------------------------------------------------------*/
const char *  ProductKnowledge :: description( const char * type
                                              ,const char * model
                                              ,const char * feature)
{
 return (pMdbFDL? pMdbFDL->description(type,model,feature) : pNoDesc);
}

    /*-----------------------------------------------------------------------*\
    |   const char *  description()                                           |
    |       Set the description for the given feature from the                |
    |       current system's Machine Readable Feature Description List        |
    |       or set to '[Description Unavailable]' when not found              |
    |       and indicate success.                                             |
    \*-----------------------------------------------------------------------*/
Boolean   ProductKnowledge :: strDescription( const char * type
                                             ,const char * model
                                             ,const char * feature
                                             ,IString & str)
{
 str = IString(description(type,model,feature));
 return (str != IString(pNoDesc));
}

    /*-----------------------------------------------------------------------*\
    |   unsigned machinesMapping2GenericFeature()                             |
    |       Load the given list with the features and                         |
    |       their descriptions which represent the equivalent                 |
    |       machines to the given feature                                     |
    |       and return the number of elements loaded.                         |
    |       Note: the returned features will always be 'NULL'.                |
    \*-----------------------------------------------------------------------*/
unsigned ProductKnowledge::machinesMapping2GenericFeature( const char * feat,
                                                       SpecificFeatList & List)
{
 PKFeat * p;
 List.removeAll();
 for (int n = 0, N = PKFL.NumberElements() ; feat && (n < N) ; n++)
   if (p = PKFL.element(n))
    if (strlen(p->featureMR1()))
     if (!strcmp(p->featureMR1(),feat))
      if (!strcmp(pkNull,p->feature()))
        List.add(SpecificFeat(p->feature(),genericDescription(p->type(),
                                                              p->model(),
                                                              p->feature())));
 return List.numberOfElements();
}

    /*-----------------------------------------------------------------------*\
    |   unsigned machinesMapping2GenericFeature()                             |
    |       Load the given list with the features ( and their                 |
    |       machine type and model) and                                       |
    |       their descriptions which represent the equivalent                 |
    |       machines to the given feature                                     |
    |       and return the number of elements loaded.                         |
    \*-----------------------------------------------------------------------*/
unsigned ProductKnowledge::machinesMapping2GenericFeature( const char * feat,
                                                           PKFeatSS & List)
{
 PKFeat * p;
 List.removeAll();
 for (int n = 0, N = PKFL.NumberElements() ; feat && (n < N) ; n++)
   if (p = PKFL.element(n))
    if (strlen(p->featureMR1()))
     if (!strcmp(p->featureMR1(),feat))
      if (!strcmp(pkNull,p->feature()))
        List.add(PKFeatDesc(p->type(),p->model(),p->feature(),
                            genericDescription(p->type(),p->model(),
                                               p->feature())));
 return List.numberOfElements();
}

    /*-----------------------------------------------------------------------*\
    |   unsigned specificFeat_Desc( SpecificFeatList & List)                  |
    |       Load the given list with the specific features and their          |
    |       descriptions for the generic contained within the object.         |
    \*-----------------------------------------------------------------------*/
unsigned ProductKnowledge::specificFeat_Desc( SpecificFeatList & List)
{
 PKFeat * p;
 List.removeAll();
 for (int n = 0, N = PKFL.NumberElements() ; n < N ; n++)
   if (p = PKFL.element(n))
     if (!List.strMachType().length() || List.strMachType().isLike(p->type()))
       if  (!List.strMachModel().length() || List.strMachModel().isLike(p->model()))
         if (!List.strGeneric().length() || !strcmp(List.strGeneric(),p->featureMR1()))
           if (strlen(p->feature()) && strcmp(p->feature(),"BASE"))
             List.add(SpecificFeat(p->feature(),
                                   genericDescription(p->type(),
                                                      p->model(),
                                                      p->feature())));
 return List.numberOfElements();
}

    /*-----------------------------------------------------------------------*\
    |   unsigned specificFeat_Desc()                                          |
    |       Load the given list with the specific features and their          |
    |       descriptions for the given generic.                               |
    \*-----------------------------------------------------------------------*/
unsigned ProductKnowledge::specificFeat_Desc(
                                            SpecificFeatList & List
                                           ,const IString & Generic
                                           ,const IString & Type
                                           ,const IString & Model)
{
 List.setGeneric(Generic);
 List.setMachType(Type);
 List.setMachModel(Model);
 return specificFeat_Desc(List);
}
unsigned ProductKnowledge::specificFeat_Desc(
                                            SpecificFeatList & List
                                           ,const char * Generic
                                           ,const char * Type
                                           ,const char * Model)
{
 List.setGeneric(Generic);
 List.setMachType(Type);
 List.setMachModel(Model);
 return specificFeat_Desc(List);
}


    /*-----------------------------------------------------------------------*\
    |   Boolean isMachineReadable()                                           |
    |       Indicate whether the given feature is machine                     |
    |       readable as determined by the feature's absence                   |
    |       in the Non-Machinereadable List                                   |
    \*-----------------------------------------------------------------------*/
Boolean ProductKnowledge::isMachineReadable(
                                             const char * type
                                            ,const char * model
                                            ,const char * feature)
{
 return !PKNMRL.contains(type,model,feature);
}

ostream& ProductKnowledge::PKOut(ostream& os)
{
 os   << "                 ----> Product Knowledge <---- " << endl
      << "Using Compiled data : "  << (fUsingCompiledPK? "Y" : "N") << endl
      << "PKFL :" << endl << PKFL << endl
      << "PKGenFDL :" << endl << PKGenFDL << endl
      << "*pMdbFDL :" << endl << *pMdbFDL << endl
      << "PKNMRL :" << endl << PKNMRL << endl;
 return os;
}


///////////////////////////////////////////////////////////
#ifdef UNITTEST

#include <iostream.h>

void main(void)
{
#if 0
//////////////////////////////
 if (1)
   {
    PKFeatList L;
//    L.readFile("PKFTL.FIN");
    L.readFile("FEATURE.TXT");
    cout << L;
    PtrStrArr GenL;
    IString MRF1,MRF2;
    unsigned qty1, qty2;
    if (L.specificFeat("7012","223","HDISK",GenL))
      {
        cout << " specificFeat('7012','223','HDISK') " << endl;
        for (int i = 0, I = GenL.NumberElements() ; i < I; i++)
         cout << GenL.element(i) << endl;
      }
//    if (L.comboMRFeatFromAASFeat("7010","MODA","A534",MRF1,&qty1,MRF2,&qty2))
    if (L.comboMRFeatFromAASFeat("9406","MODA","6421",MRF1,&qty1,MRF2,&qty2))
      {
        cout << " comboMRFeatFromAASFeat('9406','MODA','6421') :" << endl;
         cout << "MRF1 : '" << MRF1
              << "' qty : " << qty1
              << " MRF2 : '" << MRF2
              << "' qty : " << qty2 << endl;
      }

    PKFeatDescList LD;
//  LD.readFile("PKFDL.FIN");
    LD.readFile("GENERIC.TXT");
    cout << LD << endl;

//  cout << LD.description("T702", "M502" ,"F125");
//  cout << LD.description("7013", "M502" ,"ACPA");
    const char *p =  LD.description("7013", "M502" ,"ACPA");
    cout << p;
    IString str;
    if ( LD.Description("7013", "M502" ,"ACPA",str))
      cout << str;

//#ifdef __DEBUG_ALLOC__
//   _dump_allocated(16);
//#endif
    L.Release();
    LD.Release();
   }
#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif

/////////////////////////////////////////////
#endif

//////////////////////////////
 if (1)
   {
    ProductKnowledge PK;
//    L.readFile("PKFTL.FIN");
    PK.build("MDBDESC.FIN","FEATURE.TXT","GENERIC.TXT");

    cout << "-----------PKFeatList-------------" << endl;
    cout << PK.pkfl() << endl;

    cout << "-----------PKFeatListDecriptionList-------------" << endl;
    cout << PK.pkgenfdl() << endl;
    PtrStrArr GenL;
    IString MRF1,MRF2;
    unsigned qty1, qty2;
    if (PK.specificFeatures("7012","223","HDISK",GenL))
      {
        cout << " specificFeatures('7012','223','HDISK') " << endl;
        for (int i = 0, I = GenL.NumberElements() ; i < I; i++)
         cout << GenL.element(i) << endl;
      }
    PK.save();

    ProductKnowledge PK2;
    PK2.build("MDBDESC.FIN","FEATURE.TXT");

    if (PK2.comboMRFeatFromAASFeat("9406","MODA","6421",MRF1,&qty1,MRF2,&qty2))
      {
        cout << " PK2.comboMRFeatFromAASFeat('9406','MODA','6421') :" << endl;
         cout << "MRF1 : '" << MRF1
              << "' qty : " << qty1
              << " MRF2 : '" << MRF2
              << "' qty : " << qty2 << endl;
      }

    GenL.EmptyOut();
    if (PK2.specificFeatures("7012","223","HDISK",GenL))
      {
        cout << " PK2.specificFeatures('7012','223','HDISK') " << endl;
        for (int i = 0, I = GenL.NumberElements() ; i < I; i++)
         cout << GenL.element(i) << endl;
      }

    cout << endl;
    cout <<  "PK2.genericDescription('7013', 'M502' ,'ACPA') : " << endl;
    cout << endl;
    cout << PK2.genericDescription("7013", "M502" ,"ACPA");

    IString str;
    cout << endl;
    if ( PK2.strGenericDescription("7013", "M502" ,"ACPA",str))
      { cout <<  "PK2.strGenericDescription('7013', 'M502' ,'ACPA') : " << endl;
        cout << str;
        cout << endl;
      }
//    ProductKnowledge PK3(PK2);  // hiding works

     SpecificFeatList SFL;
     if (PK2.specificFeat_Desc(SFL,"HDISK","7012","M502"))
       {
         cout << " PK2.specificFeat_Desc('HDISK','7012','M502') " << endl;
         cout << SFL;
       }

//#ifdef __DEBUG_ALLOC__
//   _dump_allocated(16);
//#endif
   }
#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif

/////////////////////////////////////////////

}

#endif


