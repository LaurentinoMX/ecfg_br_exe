#ifndef _AB_RPO2LST_HPP
#define _AB_RPO2LST_HPP
 /*--------------------------------------------------------------------------*\
 |    File Name  : RPOLST.HPP                                                 |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
 |                                                                            |
 |      RPOListDisplayText - Buffer to (un)format the columnized display      |
 |                           text for a RPO.                                  |
 |      RPOList            - List of RPOs                                     |
 |      RPOpList           - List of RPO pointers                             |
 |                           (Use the pointer list when manipulating          |
 |                           multiple lists whose elements will not           |
 |                           change)                                          |
 |                                                                            |
 |         Lists are based on: ISequence Collection Class                     |
 |                             RPOListDisplayText  Class                      |
 \*--------------------------------------------------------------------------*/

                                                 // align Collection classes
                                                 // structures on 4 byte
#pragma pack(4)                                  // boundries
#include <iseq.h>
#pragma pack()

        /*---------------------------------------------------*\
        |               Class specific includes               |
        \*---------------------------------------------------*/
#include <iostream.h>
#include <stdio.h>
#include "rpo2.hpp"

#define BUFF_SIZE 257

                                                 // align Collection classes
                                                 // structures on 4 byte
#pragma pack(4)                                  // boundries
class RPO2ListDisplayText
{
 char             * Buff;

    void            init();

 protected:
    char *   Buffer(){return Buff;}
    char *   NullBuffer(){*Buff = 0; return Buff;};
    char *   FormatDisplayText(RPO2 &);

  public:
    RPO2ListDisplayText();
    RPO2ListDisplayText(char * text);
    RPO2ListDisplayText(RPO2ListDisplayText &);
   ~RPO2ListDisplayText();
    RPO2ListDisplayText &  operator=(RPO2ListDisplayText &);
    char *   format(RPO2 &,char * Buff = 0,unsigned BuffSize = 0 );
};

class RPO2List :  public ISequence<RPO2>
                ,public RPO2ListDisplayText
{
  protected:
    virtual Boolean        Sorted(Boolean fUp = true);

  public:
    RPO2List() : RPO2ListDisplayText(){};
    RPO2List(RPO2List &);
    RPO2List &  operator=(RPO2List &);
   ~RPO2List(){};
    unsigned longestDisplayText();
    void     SortUp();
    void     SortDown();
    long     locate(RPO2 const&);
    long     locate(const char * szType, const char * szModel,
                    const char * szSerialNo, const char * szFeat);
//    Boolean  removeViaDisplayText(char *);
    Boolean  removeElement(RPO2 const& );
    Boolean  writeToFile(FILE *, unsigned * pBytesWritten = 0);
    Boolean  writeToFile(char *filename, unsigned * pBytesWritten = 0);
    unsigned   writeLength();
    Boolean    loadFile(char * szFilename);
    Boolean    loadFile(FILE *f ,unsigned count,unsigned * pRead = 0);
    Boolean  SortedUp();
    Boolean  SortedDown();
    RPO2List & Sort();
    Boolean  isSorted() { return (SortedUp() || SortedDown());}
    char   * DisplayText(RPO2List::Cursor &);
    char   * DisplayText(RPO2 &);

    void     writeListToCFReport( IString strAASFile, IString &strCFRFile );       //* 73686 HECC: 08/29/2002
    long     sendRpoCfrToOrderForward( IString strOrFwdPath, IString strCFRFile ); //* 73686 HECC: 08/29/2002
    void     saveRpoCfrToDisk(IString strCFRFile);                                 //* 73686 HECC: 08/29/2002


    friend ostream& operator<< (ostream& os, RPO2List & L)
                 {
                  os   << "          ----> RPO List ( "
                       << L.numberOfElements()
                       << " elements) <---- " << endl;
                  Cursor crsr(L);
                  forCursor(crsr)
                    os << L.elementAt(crsr) << endl;
                  return os;
                 }

};
#pragma pack()
//typedef ISortedSet<RPO>::Cursor  RPOListCursor;
#endif
