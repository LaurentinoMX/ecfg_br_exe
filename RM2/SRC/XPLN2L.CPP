 /*--------------------------------------------------------------------------*\
 |    Filename - XPLN2L.CPP                                                   |
 |      contains source for the following RM/2 class implementations:         |
 |                                                                            |
 |     XPLN2List    - A sorted collection of XPLN2 objects                    |
 |                    (messages generated as a result of the resolution       |
 |                    process available for the user's viewing),              |
 |                    as described in XPLN2.HPP.                              |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                  ICLUI's Includes                   |
        \*---------------------------------------------------*/
#include <isynonym.hpp>                          // Definition of Booleans

        /*---------------------------------------------------*\
        |              Standard Library Includes              |
        \*---------------------------------------------------*/
#include <string.h>
#include <io.h>
#include <stdio.h>

        /*---------------------------------------------------*\
        |               RM/2 specific Includes                |
        \*---------------------------------------------------*/
#include "euoutil.hpp"                           // utilities
#include "xpln2l.hpp"                            // this file's declarations

        /*---------------------------------------------------*\
        |                       defines                       |
        \*---------------------------------------------------*/
#define XPLNL_SIGNATURE  81148

    /*-----------------------------------------------------------------------*\
    |   IString & displayMLEText(XPLN2 & x)                                   |
    |      Set the display text of the given explanation by appending         |
    |      the text on a new line to the object's key and return              |
    |      the string's reference.                                            |
    \*-----------------------------------------------------------------------*/
IString & XPLN2List :: displayMLEText(XPLN2 & x)
{
 IString strKey;
 strMLEDisplay = keyString(x,strKey) + "\n"+ x.textString();
 return strMLEDisplay;
}

    /*-----------------------------------------------------------------------*\
    |   IString & keyString(XPLN2 & x,IString & str)                          |
    |      Set the given string to the given explanation by                   |
    |      bracketing its type, model, serial # and feature.                  |
    \*-----------------------------------------------------------------------*/
IString & XPLN2List :: keyString(XPLN2 & x,IString & str)
{
 str = IString("[") + IString((x.machineString().size())? x.machine() : (const char *)("-")) + "/";
 str += ( IString((x.modelString().size())? x.model() : (const char *)("-")) + "/");
 str += (IString((x.serialNoString().size())? x.serialNo() : (const char *)("-")) + "/");
 str += (IString((x.featureString().size())? x.feature() : (const char *)("-")) + "]");
 return str;
}

ostream& XPLN2List::out(ostream& os)
{
 Cursor crsr(*this);
 forCursor(crsr) {
   XPLN2 & x = elementAt(crsr);
   os << "Machine : '" << elementAt(crsr).machine()
      << "'  Model : '" << elementAt(crsr).model()
      << "'  SerialNo : '" << elementAt(crsr).serialNo()
      << "'  Feature : '"  << elementAt(crsr).feature()
      << "'  Text : '" << elementAt(crsr).text() << endl;
   os << "DisplayText : '" << displayMLEText(x) << "'" << endl;
   }
 return os;
}

    /*-----------------------------------------------------------------------*\
    |    unsigned fWriteBLength()                                             |
    |       Return the number of bytes the records in the collection          |
    |       would occupy if it was to be written to a binary file             |
    |       by method  'fWriteB'.                                             |
    |         For every element in the collection, every IString's buffer     |
    |         will be  preceded with its length, while the collection itself  |
    |         will be preceded by a signature and the number of elements.     |
    \*-----------------------------------------------------------------------*/
unsigned XPLN2List::fWriteBLength()
{
 unsigned Len = 2 * sizeof(unsigned);
 Cursor crsr(*this);
 forCursor(crsr)
   Len += ( ( 5 *  sizeof(unsigned) )                       +
            (elementAt(crsr).machineString().length() + 1)  +
            (elementAt(crsr).modelString().length() + 1)    +
            (elementAt(crsr).serialNoString().length() + 1) +
            (elementAt(crsr).featureString().length() + 1)  +
            (elementAt(crsr).textString().length() + 1) );
 return Len;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean fWriteB(FILE * f, unsigned * pu)                             |
    |       Write the collection to the given filename,                       |
    |       set the number of bytes written and indicate                      |
    |       success.                                                          |
    \*-----------------------------------------------------------------------*/
Boolean XPLN2List::fWriteB(char * szFileName, unsigned * pu)
{
 FILE * f;
 Boolean fOK = ((f = fopen(szFileName,"wb")) != 0);
 if (fOK) {
   fOK = fWriteB(f,pu);
   fclose(f);
   }
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean fWriteB(FILE * f, unsigned * pu)                             |
    |       Write the collection at the current position of                   |
    |       the given file and set the number of bytes written.               |
    |         For every element in the collection, every IString's buffer     |
    |         will be  preceded with its length, while the collection itself  |
    |         will be preceded by a signature and the number of elements.     |
    \*-----------------------------------------------------------------------*/
Boolean XPLN2List::fWriteB(FILE * f, unsigned * pu)
{
 unsigned U2[2];
 Boolean fOK;
 U2[0] = XPLNL_SIGNATURE;
 U2[1] = numberOfElements();
 if (fOK = (fwrite(U2,sizeof(U2),1,f) == 1)) {
                                                 // to contain the accumulated 
                                                 // total length so set to how 
   U2[0] = sizeof(U2);                           // much has been written sofar
   Cursor crsr(*this);
   for (crsr.setToFirst(); fOK && crsr.isValid();crsr.setToNext())
     fOK = ( fWriteBIString(f,elementAt(crsr).machineString(),U2)  &&
             fWriteBIString(f,elementAt(crsr).modelString(),U2)    &&
             fWriteBIString(f,elementAt(crsr).serialNoString(),U2) &&
             fWriteBIString(f,elementAt(crsr).featureString(),U2)  &&
             fWriteBIString(f,elementAt(crsr).textString(),U2));
   }
 if (pu)
   *pu = U2[0];
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    unsigned fReadB(char * szFileName)                                   |
    |       Add records from the given file                                   |
    |       which is to have been created by the 'fWriteB' method,            |
    |       and return result as defined in XPLN2L.HPP.                       |
    \*-----------------------------------------------------------------------*/
unsigned XPLN2List::fReadB(char * szFileName)
{
 FILE * f;
 int rc = 0;
 if (f = fopen(szFileName,"rb")) {
   if ( _filelength(_fileno(f) >= (2 * sizeof(unsigned))))
     rc = fReadB(f);
   else rc = XPLNL_ERR_NO_XPLNL_FILE;
   fclose(f);
   }
 else rc = XPLNL_ERR_NO_FILE;
 return rc;
}

    /*-----------------------------------------------------------------------*\
    |    unsigned fReadB(FILE * f)                                            |
    |       Add records at the current position of a given                    |
    |       file which is to have been created by the 'fWriteB' method,       |
    |       and return result as defined in XPLN2L.HPP.                       |
    \*-----------------------------------------------------------------------*/
unsigned XPLN2List::fReadB(FILE * f)
{
 unsigned U2[2];
 IString machStr, modStr, serialStr, featStr, textStr;
 int rc = 0;
 if (fread(U2,sizeof(U2),1,f) == 1)
   if ( XPLNL_SIGNATURE == U2[0])
     for ( int i = 0 ,I = U2[1]; !rc && (i < I) ; i++)
       if (fReadBIString(f,machStr)      &&
           fReadBIString(f,modStr)       &&
           fReadBIString(f,serialStr)    &&
           fReadBIString(f,featStr)      &&
           fReadBIString(f,textStr))
             this->add(XPLN2(machStr,modStr,serialStr,featStr,textStr));
       else rc = XPLNL_ERR_READ_FILE;
   else rc = XPLNL_ERR_NO_XPLNL_FILE;
 else rc = XPLNL_ERR_READ_FILE;
 return rc;
}

    /*-----------------------------------------------------------------------*\
    |    XPLN2List & allElementsFor()                                         |
    |       Add all explanations for the given criteria to the                |
    |       given collection.                                                 |
    |       Note: If a string is not given or is empty,                       |
    |             it is not considered and all entries conforming             |
    |             to the remaining parameters are added.                      |
    \*-----------------------------------------------------------------------*/
XPLN2List & XPLN2List::allElementsFor( XPLN2List & List
                                      ,char * szMachine
                                      ,char * szModel
                                      ,char * szSerialNo
                                      ,char * szFeature
                                      ,char * szText)
{
 XPLN2 x(szMachine,szModel,szSerialNo,szFeature,szText);
 Cursor crsr(*this);
 forCursor(crsr)
   if ( elementAt(crsr) == x )
     List.add(elementAt(crsr));
 return List;
}

    /*-----------------------------------------------------------------------*\
    |    XPLN2List & allElementsFor()                                         |
    |       Add all explanations for the given criteria to the                |
    |       given collection.                                                 |
    |       Note: Default strings are not considered and all                  |
    |       entries conforming to the remaining parameters are added.         |
    \*-----------------------------------------------------------------------*/
XPLN2List & XPLN2List::allElementsFor( XPLN2List & List
                                      ,const IString & Ma
                                      ,const IString & Mo
                                      ,const IString & S
                                      ,const IString & F
                                      ,const IString & T)
{
 return allElementsFor(List,(char *)Ma,(char *)Mo,(char *)S,(char *)F,(char *)T);
}

 /*--------------------------------------------------------------------------*\
 |                         Non-Compiled testing code                          |
 \*--------------------------------------------------------------------------*/
#ifdef UNITTEST

#include <iostream.h>

void main(void)
{

///////////////////////////////////////////

 if (1)
   {
    XPLN2 x1("Mach 1","Mod 1","Ser 1","Feat 1","Text 1");
    XPLN2 x2("Mach 2","Mod 2","Ser 2","Feat 2","Text 2");
    XPLN2 x3("Mach 3","Mod 1","Ser 1","Feat 3","Text 1");
    XPLN2 x4("Mach 2","Mod 2","Ser 2","Feat 4","Text 2");
    XPLN2 x5("Mach 1","Mod 1","Ser 1","Feat 5","Text 1");
    XPLN2 x6("Mach 2","Mod 2","Ser 2","Feat 6","Text 2");
    XPLN2 x7("Mach 3","Mod 1","Ser 1","Feat 7","Text 1");
    XPLN2 x8("Mach 2","Mod 2","Ser 2","Feat 8","Text 2");
    XPLN2 x9("Mach 1",0);
    XPLN2List L1;
    L1.add(x8);
    L1.add(x7);
    L1.add(x6);
    L1.add(x5);
    L1.add(x4);
    L1.add(x3);
    L1.add(x2);
    L1.add(x1);
    L1.add(x9);
    unsigned u = L1.writeLength();
    cout << " writelength :" << u << endl;
    Boolean f = L1.writeToFile("XPLN2LST.TST",&u);
    cout << " written length :" << u << endl;
    cout << L1;

    XPLN2List L2;
    u = L2.loadFile("XPLN2LST.TST");

    cout << " loadFile rc :" << u << endl;

    cout << L2;

    XPLN2List L3;
    cout << " All for 'Mach 2' :" << endl << L1.allElementsFor(L3,"Mach 2");

    XPLN2List L4;
    cout << " All for 'Mach 2' Mod 1 :" << endl << L1.allElementsFor(L4,"Mach 2","Mod 1");
    XPLN2List L5;
    cout << " All for 'Mach 2' Mod 2 :" << endl << L1.allElementsFor(L5,"Mach 2","Mod 2");

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif
   }
#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif

/////////////////////////////////////////////

}

#endif

#if 0
//         forCursor(crsr)
Boolean XPLNList::WriteToFile(char *filename, unsigned *pu)
{
 FILE *f;
 unsigned L[2];
 Boolean fOK = ((f = fopen(filename,"wb")) != 0);
 if (fOK)
   { L[0] = XPLNL_SIGNATURE;
     L[1] = numberOFElements();
     if (fwrite(L,sizeof(L),1,f) == 1)
       { L[1] = 0;
         Cursor crsr(*this);
         for (crsr.setToFirst(); fOK && crsr.isValid();crsr.setToNext())
           {
             L[0] = elementAt(crsr).nmbrString().length()+1;
             if ( fOK = ((fwrite(L,sizeof(L[0]),1,f) == 1)           &&
                         (fwrite(elementAt(crsr).text()L,L[0],1,f) == 1)) )
              {
                L[1] += ( L[0] + sizeof(L[0]));
                L[0] = elementAt(crsr).textString().length()+1;
                if ( fOK = ((fwrite(L,sizeof(L[0]),1,f) == 1)           &&
                            (fwrite(elementAt(crsr).text()L,L[0],1,f) == 1)) )
                  L[1] += ( L[0] + sizeof(L[0]));
              }
           }
       }
    fclose(f);
   }
 if (pu) *pu = L[1];
 return fOK;
}

#if 0
Boolean XPLN2List::writeToFile(FILE * f, unsigned * pu)
{
 unsigned U2[2];
 Boolean fOK;
 U2[0] = XPLN2L_SIGNATURE;
 U2[1] = numberOfElements();
 if (fOK = (fwrite(U2,sizeof(U2),1,f) == 1))
   { U2[1] = sizeof(U2);
     Cursor crsr(*this);
     for (crsr.setToFirst(); fOK && crsr.isValid();crsr.setToNext())
       { U2[0] = elementAt(crsr).nmbrString().length()+1;
         if ( fOK = ((fwrite(U2,sizeof(U2[0]),1,f) == 1)           &&
                     (fwrite(elementAt(crsr).nmbr(),U2[0],1,f) == 1)) )
           { U2[1] += ( U2[0] + sizeof(U2[0]));
             U2[0] = elementAt(crsr).textString().length()+1;
             if ( fOK = ((fwrite(U2,sizeof(U2[0]),1,f) == 1)           &&
                         (fwrite(elementAt(crsr).text(),U2[0],1,f) == 1)) )
               U2[1] += ( U2[0] + sizeof(U2[0]));
           }
       }
   }
 if (pu) *pu = U2[1];
 return fOK;
}
#endif

Boolean XPLN2List::writeToFile(char * szFileName, unsigned * pu)
{
 FILE * f;
 Boolean fOK = ((f = fopen(szFileName,"wb")) != 0);
 if (fOK)
   { fOK = writeToFile(f,pu);
     fclose(f);
   }
 return fOK;
}

Boolean  XPLN2List::writeIString(FILE *f,const IString & str, unsigned * pu)
{
 Boolean fOK = false;
 unsigned u = 0, len = str.length()+1;
 if (fwrite(&len,sizeof(len),1,f) == 1)
   { u = sizeof(len);
     if ( fOK = (fwrite((char *)str,len,1,f) == 1) )
       u += len;
   }
 if (*pu) *pu += u;
 return fOK;
}

Boolean XPLN2List::writeToFile(FILE * f, unsigned * pu)
{
 unsigned U2[2];
 Boolean fOK;
 U2[0] = XPLNL_SIGNATURE;
 U2[1] = numberOfElements();
 if (fOK = (fwrite(U2,sizeof(U2),1,f) == 1))
   { U2[0] = sizeof(U2);
     Cursor crsr(*this);
     for (crsr.setToFirst(); fOK && crsr.isValid();crsr.setToNext())
        fOK = ( writeIString(f,elementAt(crsr).machineString(),U2)  &&
                writeIString(f,elementAt(crsr).modelString(),U2)    &&
                writeIString(f,elementAt(crsr).serialNoString(),U2) &&
                writeIString(f,elementAt(crsr).featureString(),U2)  &&
                writeIString(f,elementAt(crsr).textString(),U2));
   }
 if (pu) *pu = U2[0];
 return fOK;
}

Boolean  XPLN2List::loadIString(FILE *f,IString & istring)
{
 unsigned len;
 char * Buff;
 Boolean fOK = (fread(&len,sizeof(len),1,f) == 1);
 if (fOK)
   if (len)
     { if (fOK = ((Buff = new char[len+1]) != 0))
         { if (fOK = (fread(Buff,len,1,f) == 1))
             istring = IString(Buff);
           delete[] Buff;
         }
     }
   else istring = IString("");
 return fOK;
}

unsigned XPLN2List::loadFile(char * szFileName)
{
 FILE * f;
 int rc = 0;
 if (f = fopen(szFileName,"rb"))
   { if ( _filelength(_fileno(f) >= (2 * sizeof(unsigned))))
       rc = loadFile(f);
     else rc = XPLNL_ERR_NO_XPLNL_FILE;
     fclose(f);
   }
 else rc = XPLNL_ERR_NO_FILE;
 return rc;
}

unsigned XPLN2List::loadFile(FILE * f)
{
 unsigned U2[2];
 IString machStr, modStr, serialStr, featStr, textStr;
 int rc = 0;
 if (fread(U2,sizeof(U2),1,f) == 1)
   if ( XPLNL_SIGNATURE == U2[0])
     for ( int i = 0 ,I = U2[1]; !rc && (i < I) ; i++)
       if (loadIString(f,machStr)      &&
           loadIString(f,modStr)       &&
           loadIString(f,serialStr)    &&
           loadIString(f,featStr)      &&
           loadIString(f,textStr))
         { XPLN2 x(machStr,modStr,serialStr,featStr,textStr);
           this->add(x);
         }
       else rc = XPLNL_ERR_READ_FILE;
   else rc = XPLNL_ERR_NO_XPLNL_FILE;
 else rc = XPLNL_ERR_READ_FILE;
 return rc;
}

#endif
