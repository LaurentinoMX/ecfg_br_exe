
 /*-----------------------------------------------------------------------------------*\
 |   Change Log:                                                                       |
 |                                                                                     |
 |   HECC               08/28/2002  Added three new methods to process the RPO List:   |
 |                                  a) writeListToCFReport.  This method writes a new  |
 |                                     CFR containing a Base Section and an Order      |
 |                                     section with the RPO information.               |
 |                                  b) sendRpoCfrToOrderForward.  This method starts   |
 |                                     the Order Forward execution, passing the new    |
 |                                     CFR file that has been created, in in case it   |
 |                                     exists.                                         |
 |                                  c) saveRpoCfrToDisk.  In case that Order Forward   |
 |                                     is not available, this new method displays a    |
 |                                     Save As... dialog to enable user to manage the  |
 |                                     new CFR on his own.                             |
 |                                                                                     |
 |   HECC      D83392   07/23/2003  Added new logic to force the Locking Flag off, in  |
 |                                  the header of the input CFR, position 39 of the    |
 |                                  RT 00                                              |
 |                                                                                     |
 |   HECC      D83402   07/28/2003  Logic to that creates the RT 95 associated to the  |
 |                                  RT 19 was changed to add four new parameters       |
 |                                  in the RPO2 class: Customer number, System number  |
 |                                  flag, System type and System number.               |
 |                                                                                     |
 |                                                                                     |
 \*-----------------------------------------------------------------------------------*/


      /*--------------------------------------------------------------*\
      |                  Standard Library includes                     |
      \*--------------------------------------------------------------*/

extern "C"
  {
   #include <io.h>                    //  _filelength
   #include <stdlib.h>
   #include <process.h>               // _spawnlp   F73686  HECC: 09/09/2002
   #include <windows.h>               //* 73686
  }

        /*---------------------------------------------------*\
        |                   Class Includes                    |
        \*---------------------------------------------------*/
#include <idate.hpp>                 //* 73686
#include <itime.hpp>                  //* 73686
#include <ifiledlg.hpp>               //* 73686
#include <imsgbox.hpp>                //* 73686

#include "rpo2l.hpp"
#include "euoutil.hpp"
#include "af9apply.h"                 //* 73686

RPO2ListDisplayText :: RPO2ListDisplayText()
{ init(); }

void RPO2ListDisplayText :: init()
{
 Buff = new char[ BUFF_SIZE +1];
 Buff[0] = 0;
}

RPO2ListDisplayText :: RPO2ListDisplayText(char * text)
{
 init();
 mystrncpy(Buff,text,BUFF_SIZE);
}

RPO2ListDisplayText :: RPO2ListDisplayText(RPO2ListDisplayText & other)
{
 init();
 mystrncpy(Buff,other.Buff,BUFF_SIZE);
}

RPO2ListDisplayText :: ~RPO2ListDisplayText()
{
// DosBeep(262,100);
 delete[] Buff;
}

RPO2ListDisplayText & RPO2ListDisplayText :: operator=(RPO2ListDisplayText & other)
{
  strcpy(Buff,other.Buffer());
  return (*this);
}

char * RPO2ListDisplayText :: format(RPO2&  rpo,char * Buffer,unsigned BuffSize )
{
  if (!Buffer) {
    Buffer = Buff;
    BuffSize = BUFF_SIZE;
    }
  return rpo.formatText(Buffer,BuffSize);
}

/***********************************************************************/
/* External functions                                                  */
/*                                                                     */
/* F73686 HECC: 09/05/2002                                             */
/** Begin New Logic: Includes to process the checksum in the RT 99 *****/

#define  PSZ                  LPSTR
#define  WAPIRET              unsigned long
#define  WAPIENTRY            EXPENTRY

WAPIRET WAPIENTRY cfrcksum(PSZ pszFileName,PSZ pszCookie);

/** End New Logic ******************************************************/


 /*--------------------------------------------------------------------------*\
 |                               RPOList Class                                |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |             In-Line RPO Compare methods             |
        \*---------------------------------------------------*/
inline long RPO2CompareUp(const RPO2& rpo1,const RPO2& rpo2)
     { return (( rpo1 == rpo2)? 0 : ((rpo1 < rpo2)? -1 : 1));}

inline long RPO2CompareDown(const RPO2& rpo1,const RPO2& rpo2)
            { return (-1*RPO2CompareUp(rpo1,rpo2)); }


          /*-----------------------------------------------------------------*\
          |   Ctors                                                           |
          |      Other constructor(s) and destructor in-lined in RPOLST.HPP   |
          \*-----------------------------------------------------------------*/
RPO2List :: RPO2List(RPO2List & other)
          : RPO2ListDisplayText(other.Buffer())
{ addAllFrom(other); }

RPO2List & RPO2List :: operator=(RPO2List & other)
{
 removeAll();
 addAllFrom(other);
 return (*this);
}

          /*-----------------------------------------------------------------*\
          |   int  longestDisplayText()                                       |
          |      Method returns number of bytes of the longest                |
          |      columnized display text in this list of RPOs.                |
          \*-----------------------------------------------------------------*/
unsigned RPO2List :: longestDisplayText()
{
  Cursor crsr(*this);
  unsigned ilength, ilongest = 0;
  forCursor(crsr)
    if ((ilength=  strlen(format(elementAt(crsr)))) > ilongest)
      ilongest = ilength;
  return ilongest;
}

          /*-----------------------------------------------------------------*\
          |   char * DisplayText( RPOList::Cursor & crsr)                     |
          |      Method returns the pointer to the buffer which contains      |
          |      the columnized display string for the RPO indicated by the   |
          |      given cursor.                                                |
          |      Buffer will be empty if unsuccessful                         |
          \*-----------------------------------------------------------------*/
char *RPO2List :: DisplayText( RPO2List::Cursor & crsr)
{ return (crsr.isValid()? RPO2ListDisplayText::format(elementAt(crsr))
                          : NullBuffer());
}

          /*-----------------------------------------------------------------*\
          |   char * DisplayText(RPO & rpo)                                   |
          |      Method returns the pointer to the buffer which contains      |
          |      the columnized display string for the given RPO.             |
          \*-----------------------------------------------------------------*/
char *RPO2List :: DisplayText( RPO2 & rpo)
{ return format(rpo); }

          /*-----------------------------------------------------------------*\
          |   void SortUp()                                                   |
          |      Method sorts the RPO List in ascending order                 |
          \*-----------------------------------------------------------------*/
void RPO2List :: SortUp()
{ this->sort(RPO2CompareUp); }

          /*-----------------------------------------------------------------*\
          |   void SortDown()                                                 |
          |      Method sorts the RPO List in descending order                |
          \*-----------------------------------------------------------------*/
void RPO2List :: SortDown()
{ this->sort(RPO2CompareDown); }

RPO2List & RPO2List :: Sort()
{
 if (SortedUp())            // default when 1 element
   SortUp();
 else SortDown();
 return *this;
}

Boolean RPO2List :: removeElement(RPO2 const& rpo)
{
 long l = locate(rpo);
 if (l) this->removeAtPosition(IPosition(l));
 return (l != 0);
}

          /*-----------------------------------------------------------------*\
          |   long Locate(RPO const& rpo)                                     |
          |      Method returns the 1-based index of the given RPO            |
          |      in the list.                                                 |
          |      Returns: 0/n where 0 ==  unsuccessful                        |
          |                         n ==  the 1-based index                   |
          \*-----------------------------------------------------------------*/
long RPO2List :: locate(RPO2 const& rpo)
{
 Cursor crsr(*this);
 long l;
 for (crsr.setToFirst(),l=1;crsr.isValid();crsr.setToNext(),l++)
   if (rpo == elementAt(crsr))
     return l;
 return 0;
}

long RPO2List :: locate(const char * szType, const char * szModel,
                        const char * szSerialNo, const char * szFeat)
{
 Cursor crsr(*this);
 long l;
 for (crsr.setToFirst(),l=1;crsr.isValid();crsr.setToNext(),l++)
   if ( elementAt(crsr).equals(szType,szModel,szSerialNo,szFeat))
     return l;
 return 0;
}

          /*-----------------------------------------------------------------*\
          |   Boolean Sorted(Boolean fUp)                                     |
          |      Method indicates whether the list is sorted in the given     |
          |      order ( true == ascending - false == descending)             |
          |      Returns: false/true  == (not)sorted                          |
          \*-----------------------------------------------------------------*/
Boolean RPO2List :: Sorted(Boolean fUp)
{
 Boolean fOK=true;
 if ( numberOfElements() > 1) {
   RPO2 *p1 = new RPO2, *p2 = new RPO2, *pt;
   Cursor crsr(*this);
   for (crsr.setToFirst();crsr.isValid() && fOK ;crsr.setToNext()) {
     *p1 = elementAt(crsr);
     pt=p1;p1=p2;p2=pt;                      // swap pointers
     if (!isFirst(crsr))
       fOK = fUp? ((*p1 < *p2)? true:false):((*p1 > *p2)? true:false);
     }
   delete p1;
   delete p2;
   }
 return fOK;
}

          /*-----------------------------------------------------------------*\
          |   Boolean SortedDown()                                            |
          |      Method indicates whether the list is sorted in               |
          |      descending order.                                            |
          |      Returns: false/true  == (not)sorted                          |
          \*-----------------------------------------------------------------*/
Boolean RPO2List :: SortedDown()
{ return Sorted(false);}

          /*-----------------------------------------------------------------*\
          |   Boolean SortedUp()                                              |
          |      Method indicates whether the list is sorted in               |
          |      ascending order.                                             |
          |      Returns: false/true  == (not)sorted                          |
          \*-----------------------------------------------------------------*/
Boolean RPO2List :: SortedUp()
{ return Sorted();}

unsigned RPO2List :: writeLength()
{
 return (numberOfElements() * sizeof(RPO2));
}

Boolean RPO2List :: writeToFile(FILE *f, unsigned * pBytes)
{
 unsigned len = sizeof(RPO2), lenT = 0;
 Boolean fOK = true;
 Cursor crsr(*this);
 for (crsr.setToFirst(); fOK && crsr.isValid();crsr.setToNext())
   if ( fOK = (fwrite(&elementAt(crsr),len,1,f) == 1) )
     lenT += len;
 if (pBytes)
   *pBytes = lenT;
 return fOK;
}

Boolean RPO2List :: writeToFile(char *filename, unsigned * pBytes)
{
 FILE *f = fopen(filename,"wb");
 Boolean fOK;
 if (pBytes)
   *pBytes = 0;
 if (fOK = ( f != 0)) {
   fOK = writeToFile(f,pBytes);
   fclose(f);
   }
 return fOK;
}

Boolean RPO2List :: loadFile(FILE *f , unsigned count, unsigned * pRead)
{
 int i;
 char *Buff = new char[sizeof(RPO2)];
 Boolean fOK = (Buff != 0);
 if (fOK) {
   for (i = 0; fOK && i < count; i++)
     if (fOK = (fread(Buff,sizeof(RPO2),1,f) == 1))
       this->add(*(RPO2 *)Buff);
   delete[] Buff;
   }
 if (pRead)
   *pRead = i;
 return fOK;
}

Boolean RPO2List :: loadFile(char * szFileName)
{
 FILE *f;
 unsigned i;
 Boolean fOK = false;
 if ( f = fopen(szFileName,"rb") ) {
   i = _filelength(_fileno(f));
   if (!( i % sizeof(RPO2)))
     fOK = loadFile(f,i/sizeof(RPO2));
   fclose(f);
   }
 return fOK;
}

          /*-----------------------------------------------------------------*\
          |                                                                   |
          |   void  RPO2List :: writeListToCFReport(IString strAASFile,       |
          |                                         IString &strCFRFile)      |
          |       (NEW)                                                       |
          |       Processes the RPO list.  This logic builds a new CFR        |
          |       containing the Base Section using the Base section of the   |
          |       input CFR and an Order Section containning a RT 19 for each |
          |       product that contains RPO's in the List                     |
          |       HECC: 73686  10/17/2002                                     |
          |       Returns: nothing                                            |
          \*-----------------------------------------------------------------*/

void  RPO2List :: writeListToCFReport(IString strAASFile,
                                      IString &strCFRFile)
{

   Cursor RPOCrsr(*this);
   RPO2 rpo;

   IString sInputRec, sOutputRec;             //* Variables to read input files
   IString strRecType, strSectionType;        //* To read type of records and the 01 section
   IString strTrailerRecord, strChksumRecord; //* To write the RT 98 (Trailer) and 99 (Checksum)
   IString strRecCnt, strChkSum;              //* Records count and file checksum
   IString strDate, strTime;                  //* To define the time and date based CFR name
   IString strInitType, strInitMod;           //* Saves the Type-Mod product of the first RPO
   IString str19RecType, str95RecType;        //* To create and write the RT 19, 95 & 96
   IString str96RecType;

   IString strRPOFeatList;                    //* To store the read Features from the RPO list

   Boolean bHeaderRec   = true;               //* Checks when the input file header records have been read
   Boolean bBaseSection = false;              //* Checks when the Base Section is being read
   Boolean bNewTypeModel = true;              //* Checks when a new Type-Model product has been read
   Boolean bValidProd    = false;             //* Checks if the read product in the RPO's list is HW

   int iRecCnt = 0;                           //* To count the number of written records to the output file

   ULONG ulOffset = 0;
   unsigned long ulChecksum;                  //* Calculated checksum of cfreport

   ulOffset = strAASFile.lastIndexOf(STR_PathDelimiter);
   strCFRFile = strAASFile;

   if ( ulOffset )
      strCFRFile.remove(ulOffset+1);

   //* Creates the unique filename for the CFR that is going to be built with the
   //* form:  RPO-yymmdd-hhmmss.CFR
   strDate = IDate().asString().subString(4,2) +
             IDate().asString().subString(1,2) +
             IDate().asString().subString(7,2);
   strTime = ITime().asString("3/8/17M%S");

   strCFRFile = strCFRFile + "RPO-" + strDate + STR_Hyphen +
                                      strTime + STR_ExtensionCFR;

   //* Opens the input CFR used to extract the Base Section.  In case of any failure,
   //* returns an error message.
   ofstream ofs(strCFRFile);
   ifstream ifs(strAASFile);

   if ( ifs.fail() )
      {
      IMessageBox msg(IWindow::desktopWindow());
      IMessageBox::Style styleError = IMessageBox::okButton |
                                      IMessageBox::errorIcon |
                                      IMessageBox::moveable;
      msg.setTitle(LOADSTRING(IDS_ErrorLoadAASFile_Title));
      IString sMsg = LOADSTRING(IDS_ErrorLoadAASFile_Text);
      sMsg.change(STR_Substitute0, strAASFile);
      msg.show(sMsg, styleError);
      return;
      }

   //* Copies the header and the Base section of the input CFR file to the
   //* new CFR file.
   while ( !ifs.eof() )
         {
         sInputRec = IString::lineFrom(ifs,'\n');
         strRecType = sInputRec.subString(1, 2);
         strSectionType = sInputRec.subString(1, 3);

         //* D83392  HECC - Begin new logic: The header, RT00, from the retrieve tool has the locking flag
         //* on by default. It is corrected to force the Locking Flag off, setting this flag to zero.
         if (strRecType == STR_00RecType)
            {
             IString strFirstSec = sInputRec.subString( 1, 38);
             IString strRemainSec= sInputRec.subString(40);
             sInputRec = strFirstSec + "0" + strRemainSec;
            }
         //* D83392 Ends New logic  HECC: 07/23/2003

         if ( (strRecType == STR_98RecType) || (strRecType == " ") )
            {
             bHeaderRec = false;
             bBaseSection = false;
            }

         if (strRecType == STR_01RecType)
            if (strSectionType == STR_Rec01BaseSection)
               {
                bBaseSection = true;
                sOutputRec = sInputRec;
                ofs << sOutputRec <<endl;
                iRecCnt++;
               }
            else
               {
                bHeaderRec   = false;
                bBaseSection = false;
               }
         else
            if (bHeaderRec || bBaseSection)
               {
                sOutputRec = sInputRec;
                ofs << sOutputRec <<endl;
                iRecCnt++;
               }
         }

   //* Creates the Order Section identifier: 014
   sOutputRec = STR_Rec01OrderSection;
   ofs << sOutputRec <<endl;
   iRecCnt++;

   //* Copies the information contained in the RPO list to the
   //* new CFR file.
   RPOCrsr.setToFirst();
   while (RPOCrsr.isValid())
       {
       strRPOFeatList = "";
       rpo = this->elementAt(RPOCrsr);
       strInitType     = rpo.machineType();
       strInitMod      = rpo.machineModel();
       bNewTypeModel   = true;
       bValidProd      = false;


       //* First, checks that the read RPO feature is not SIPO. This means that it
       //* is a SW feature wich category is HW, so we need to ensure that this type
       //* of features doesn't be included as they are really SW.
       boolean isSIPOMach = !strcmp(rpo.machineType(), SIPO_TYPE) &&
                            !strcmp(rpo.machineModel(), SIPO_MODEL);

       //* Now, verifies that the read RPO record from the list be Hardware, and
       //* not a SIPO feature, to flag it as valid.
       if ( rpo.productCat() == 'H' & !isSIPOMach)
            bValidProd = true;


       //* If the read RPO was a valid HW feature, then writes a RT 19 for the
       //* product that it belongs to, a RT 95 for the identifier product data
       //* and the necessary RT 96 for all the HW RPO's that belong to this product.
       if (bValidProd)
          {
          str19RecType = STR_19RecType +
                         rpo.machineType() + rpo.machineModel().leftJustify( 10 ) + "C";

          //* Reads all the features for the same Type-Model product
          while ( bNewTypeModel )
                {
                 str19RecType = str19RecType + rpo.featureCode().leftJustify( 7 ) +
                                IString(rpo.qty()).rightJustify( 5 ) +
                                IString(rpo.featureAct());

                 //* Stores the RT 96 from the feature that has been read from the
                 //* RPO list into one single variable.
                 strRPOFeatList = strRPOFeatList + STR_96RecType +
                                  rpo.featureCode().leftJustify( 48 ) +
                                  rpo.description() + "\n";
                 iRecCnt++;

                 RPOCrsr.setToNext();
                 if ( RPOCrsr.isValid() )
                    {
                    rpo = this->elementAt(RPOCrsr);
                    if ( ( strInitType != rpo.machineType()  ) ||
                         ( strInitMod  != rpo.machineModel() )   )
                       {
                        bNewTypeModel = false;
                        RPOCrsr.setToPrevious();
                        rpo = this->elementAt(RPOCrsr);
                        RPOCrsr.setToNext();
                       }
                    }
                 else
                    bNewTypeModel = false;
                }

          //* Writes the RT 19 that has been created
          ofs << str19RecType <<endl;
          iRecCnt++;

          //* Creates and writes the RT 95 associated with the product record 19.
          //* str95RecType = STR_95RecType +
          //*                rpo.machineType() + rpo.machineModel().leftJustify( 13 ) +
          //*                rpo.serialNum().leftJustify( 123 ) +
          //*                rpo.machineDescr();

          str95RecType = STR_95RecType     +
                         rpo.machineType() +
                         rpo.machineModel().leftJustify( 13 ) +
                         rpo.serialNum().leftJustify( 30 )    +
                         rpo.systNumberFlag()  +
                         rpo.systemType()      +
                         rpo.systemNum().leftJustify( 21 )    +
                         rpo.customerNumber().leftJustify( 67 )   +
                         rpo.machineDescr();

          ofs << str95RecType <<endl;
          iRecCnt++;

          //* Writes all the RT 96 that were created from the RPO list for the
          //* same Type-Model product
          strRPOFeatList = strRPOFeatList.subString(1, strRPOFeatList.length() - 1);
          str96RecType = strRPOFeatList;
          ofs << str96RecType <<endl;

          }  //* ends  if bValidProd

       //* If the read RPO is not from a valid HW product, retrieve the next
       //* feature from the RPO's list.
       else
          RPOCrsr.setToNext();

       }  //* ends  while  RPOCrsr.isValid()


   //* Writes the RT 98 with the number of records written to the output file
   iRecCnt++;
   strRecCnt = IString(iRecCnt);
   strTrailerRecord = STR_98RecType + strRecCnt.rightJustify( 5 );
   ofs << strTrailerRecord <<endl;

   //* Closes the input and output files first before the final Checksum be calculated
   ifs.close();
   ofs.close();

   //* Calculates the accurate checksum for the RT 99 of the new CFR file
   ulChecksum = cfrcksum(strCFRFile,"82151");
   strChkSum = IString(ulChecksum);
   strChksumRecord = STR_99RecType + strChkSum;

   //* Copies the file to the right one and writes the RT 99 with the checksum and
   //* closes the new file.

   ofstream ofcfr(strCFRFile, ios::app);
   if ( ofcfr.fail() );
   else
      {
      ofcfr << strChksumRecord <<endl;
      ofcfr.close();
      }

}
          /*--------------------------------------------------------------------*\
          |                                                                      |
          |   LONG RPO2List :: sendRpoCfrToOrderForward(IString strOrFwdPath,    |
          |                                             IString strCFRFile)      |
          |                                                                      |
          |       Sends the resulting CFR with the RPO list information to       |
          |       Order Forward                                                  |
          |       HECC: 73686  10/17/2002                                        |
          |       Returns: LONG lRC. Return Code of the Order Forward execution  |
          \*--------------------------------------------------------------------*/

LONG  RPO2List :: sendRpoCfrToOrderForward(IString strOrFwdPath,
                                           IString strCFRFile)
{

   LONG lRC = 0;

   //* Starts execution of Order Forward
   IString strPara = IString("\"")+ strOrFwdPath +IString("\"");
   lRC = _spawnl(P_NOWAIT, strOrFwdPath, strOrFwdPath, strCFRFile, NULL);

   return lRC;

}


          /*-----------------------------------------------------------------*\
          |                                                                   |
          |    void  RPO2List :: saveRpoCfrToDisk(IString strCFRFile)         |
          |                                                                   |
          |      If Order Forward is not available, display a dialog to       |
          |      the user to select the new name and location for the CFR     |
          |      that was created.                                            |
          |                                                                   |
          |          HECC: 73686  10/17/2002                                  |
          |          Returns: nothing                                         |
          \*-----------------------------------------------------------------*/

void  RPO2List :: saveRpoCfrToDisk(IString strCFRFile)

{
      ULONG ulDlgTitle = 0;
      IString strAltCFRName;
      ulDlgTitle = IDS_NOTAVAILABLEDLGTITLE;

      strAltCFRName = strCFRFile + STR_ExtensionCFT;
      rename(strCFRFile, strAltCFRName);

      IFileDialog::Settings settings;
      settings.setSaveAsDialog();
      settings.setTitle(LOADSTRING(ulDlgTitle));
      settings.setFileName( strCFRFile );
      IFileDialog::Style fileStyle = IFileDialog::defaultStyle() |
                                     IFileDialog::explorer;

      IFileDialog fd(IWindow::desktopWindow(), IWindow::desktopWindow(), settings, fileStyle);
      IString strNewCFRName = fd.fileName();
      Boolean bOK = fd.pressedOK();

      if (bOK)
         MoveFile(strAltCFRName, strNewCFRName);
      else
         remove(strAltCFRName);
}



////////////////////////////////////////////////////////////
#ifdef UNITTEST

#include <iostream.h>


void main(void)
{
 char Buff[200];
 char c;
 int Id;

 if (1)
   {
    RPO2 r1(  "Typ1","Mod1","Serial1","Feat2","Description 1",1,'A');
    RPO2 r2(  "Typ2","Mod2","Serial2","Feat3","Description 2",2,'B');
    RPO2 r3(  "Typ3","Mod3","Serial3","Feat4","Description 3",3,'C');
    RPO2 r4(  "Typ4","Mod4","Serial4","Feat5","Description 4",4,'D');
    RPO2 r5(  "Typ5","Mod5","Serial5","Feat6","Description 5",5,'E');
    RPO2 r6(  "Typ6","Mod6","Serial6","Feat7","Description 6",6,'F');
    RPO2 r7(  "Typ7","Mod7","Serial7","Feat8","Description 7",7,'G');
    RPO2List L;
    L.add(r5);
    L.add(r7);
    L.add(r3);
    L.add(r2);
    L.add(r4);
    L.add(r1);
    L.add(r6);

    unsigned u = L.writeLength();
    cout << " writelength :" << u << endl;
    Boolean f = L.writeToFile("RPO2L.TST",&u);
    cout << " written length :" << u << endl;
    cout << L;
    L.SortUp();
    cout << L;
    L.SortDown();
    cout << L << endl;

    strcpy(Buff,L.DisplayText(r4));
    cout << "r4's display text : '" << Buff << "'" << endl;

//    cout << "removing r4 via it's display text (rc): "
//         << (int)L.removeViaDisplayText(Buff) << endl;
    cout << L;



    RPO2List L2;
    u = L2.loadFile("RPO2L.TST");
    cout << " loadFile rc :" << u << endl;
    cout << L2;

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif

   }

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif
}

#endif

#if 0
          /*-----------------------------------------------------------------*\
          |   Boolean  RemoveViaDisplayText(char * txt)                       |
          |      Method removes the RPO corresponding to the given            |
          |      columnized display text from the list.                       |
          |      Returns: false/true == un/successful                         |
          \*-----------------------------------------------------------------*/
Boolean RPO2List :: removeViaDisplayText(char * txt)
{
 int Id;
 RPO2::setFromFormattedText(txt,&Id);
 return removeElement(Id);
}

          /*-----------------------------------------------------------------*\
          |   Boolean RemoveElement(RPO const& rpo)                           |
          |      Method removes the given RPO from the list.                  |
          |      Returns: false/true == un/successful                         |
          \*-----------------------------------------------------------------*/
Boolean RPO2List :: removeElement(int Id)
{
 long l = locate(Id);
 if (l) this->removeAtPosition(IPosition(l));
 return (l != 0);
}

long RPO2List :: locate(int Id)
{
 Cursor crsr(*this);
 long l;
 for (crsr.setToFirst(),l=1;crsr.isValid();crsr.setToNext(),l++)
   if (Id == elementAt(crsr).id())
     return l;
 return 0;
}
#endif
