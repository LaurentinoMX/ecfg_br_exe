#ifndef _AB_PCOL6_HPP_
#define _AB_PCOL6_HPP_

#include <fstream.h>
#include <stdio.h>
#include <istring.hpp>
#include <idate.hpp>
#include "euoutil.hpp"
#include "parr.hpp"

#pragma pack(4)

class Column
{
 public:

   enum Justify
     {
       Left = 0
      ,Center
      ,Right
      ,Same
     };

    Column( const char * heading1
           ,unsigned width   = 0
           ,Justify  jHeader = Center
           ,Justify  jText   = Same
           ,const char * heading2 = 0)
               { init(IString(heading1),IString(heading2),width,jHeader,jText);}
    Column( const IString & heading1
           ,unsigned width = 0
           ,Justify  jHeader = Center
           ,Justify  jText   = Same
           ,const IString & heading2 = IString())
           { init(heading1,heading2,width,jHeader,jText);}
    Column(const Column & other) { *this = other; }
    ~Column(){}
    Column & operator=(Column const& other);
    Column & operator+=(Column const& other);
    unsigned          width() { return Width;}
    Justify           headerJustified() { return JustH;}
    Justify           textJustified()   { return JustT;}
    int               compare(Column const& c) const;
    const IString &   strHeading(unsigned index)
                          { return Hdr[ALMIN(1,index)];}
    const char *      heading(unsigned index)
                          { return (const char *)strHeading(index);}
    void              setText( const IString & s1
                              ,const IString & s2 = IString());
    void              setText( const char * T1
                              ,const char * T2 = 0);
    void              setHeaderText( const IString & s1
                                    ,const IString & s2 = IString());
    void              setHeaderText( const char * T1
                                    ,const char * T2 = 0);
    const IString &   strText(unsigned index)
                          { return Txt[ALMIN(1,index)];}
    const char *      text(unsigned index)
                          { return (const char *)strText(index);}


    friend ostream& operator<< (ostream& os, Column & c)
        {
          return os << "Heading1: '"  << c.heading(0) << "'"
                    << "Heading2: '"  << c.heading(1) << "'"
                    << "Text1: '"  << c.text(0) << "'"
                    << "Text2: '"  << c.text(1) << "'"
                    << "Width  : " << c.width()
                    << "headerJustified() : "
                    << ((c.headerJustified()==Column::Left)? "Left" :
                         ((c.headerJustified()==Column::Right)?
                           "Right" : "Center"))
                    << endl;
        }

 private:

   IString   Hdr[2];
   IString   Txt[2];
   Justify   JustH
           , JustT;
   unsigned  Width;
   void      init(const IString &,const IString &,unsigned,Justify,Justify);

};

class Columns : public PtrArray
{
 protected:
  unsigned Columns::write(Boolean fText,char * Buffer,unsigned * pLines,FILE *f,
                        char cLeft,char cSep,char cRight);
  unsigned Columns::write(Boolean fText,char ** ppBuf,unsigned * pLines,FILE *f,
                          char cLeft,char cSep,char cRight);
 public:
   enum FrameType
     {
       Top = 0
      ,TopRight
      ,TopLeft
      ,Middle
      ,Bottom
      ,BotRight
      ,Line
      ,Across
     };

   enum FrameStyle
     {
       Basic = 0
      ,Solid
      ,Double
      ,Double_Solid
     };

  Columns(FrameStyle s = Basic);
  virtual    ~Columns();
  Columns(const Columns & other) { *this = other; }

  Columns&  operator=(const Columns&);
  Columns&  operator+=(const Columns&);

  FrameStyle  frameStyle();
  Columns &   setFrameStyle(FrameStyle);
  int         add(const Column & c);
  Column *    element(unsigned index) const;
  void        write(FILE *f);
  void        remove(const Column & c);
  void        setHeaderTexts( const char * sz0 = 0
                             ,const char * sz1 = 0
                             ,const char * sz2 = 0
                             ,const char * sz3 = 0
                             ,const char * sz4 = 0
                             ,const char * sz5 = 0
                             ,const char * sz6 = 0
                             ,const char * sz7 = 0
                             ,const char * sz8 = 0
                             ,const char * sz9 = 0
                            );

  void        setTexts( const char * sz0 = 0
                       ,const char * sz1 = 0
                       ,const char * sz2 = 0
                       ,const char * sz3 = 0
                       ,const char * sz4 = 0
                       ,const char * sz5 = 0
                       ,const char * sz6 = 0
                       ,const char * sz7 = 0
                       ,const char * sz8 = 0
                       ,const char * sz9 = 0
                      );
//  unsigned    writeHeadings(FILE *f);
  unsigned    charType(unsigned index);

  unsigned    writeHeaderText(char * Buffer,unsigned * pLines,FILE *f,
                              char cLeft,char cSep,char cRight);
  unsigned    writeHeaderText(char ** pBuffer,unsigned * pLines,FILE *f,
                              char cLeft,char cSep,char cRight);
  unsigned    writeHeaderText(char * Buffer,unsigned * pLines,FILE *f = 0);
  unsigned    writeHeaderText(char ** pBuffer,unsigned * pLines,FILE *f = 0);
  unsigned    writeText(char * Buffer,unsigned * pLines,FILE *f,
                        char cLeft,char cSep,char cRight);
  unsigned    writeText(char ** pBuffer,unsigned * pLines,FILE *f,
                        char cLeft,char cSep,char cRight);
  unsigned    writeText(char * Buffer,unsigned * pLines,FILE *f = 0);
  unsigned    writeText(char ** pBuffer,unsigned * pLines,FILE *f=0);
  unsigned    closeOff(char ** pBuffer,unsigned * pLines,FILE *f = 0);
  unsigned    closeOff(char * Buffer,unsigned * pLines,FILE *f = 0);
  unsigned    writeMergingFrameLine(Columns & other,Boolean fLookUp,
                                     char ** pBuf, FILE * f = 0);
  unsigned    writeMergingFrameLine(Columns & other,Boolean fLookUp,
                                         char * Buf, FILE * f = 0);


  virtual unsigned    writeHeader(char * Buf,unsigned * pLines,FILE *f= 0);
  virtual unsigned    writeHeader(char ** pBuf,unsigned * pLines,FILE *f= 0);
  unsigned    writeToplessHeader(char * Buf,unsigned * pLines,FILE *f = 0);
  unsigned    writeToplessHeader(char ** pBuf,unsigned * pLines,FILE *f = 0);
  unsigned    writeAppendHeader(char * Buf,unsigned * pLines,FILE *f = 0);
  unsigned    writeAppendHeader(char ** pBuf,unsigned * pLines,FILE *f = 0);
  virtual unsigned writeFrameLine(FrameType type,char *Buffer,FILE *f = 0);
  virtual unsigned writeFrameLine(FrameType type,char ** pBuffer,FILE *f = 0);
  unsigned    writeFrameLine( char Left, char Middle, char Sep, char Right,
                              char * Buffer, FILE * f = 0);
  unsigned    writeFrameLine( char Left, char Middle, char Sep, char Right,
                              char ** pBuffer, FILE * f = 0);
  unsigned    totalWidth();
  unsigned    headerRows();
  unsigned    textRows();

  friend ostream& operator<< (ostream& os, Columns & list)
                {
                 for (int i = 0, N = list.NumberElements() ; i < N ; i++ )
                   os <<  *list.element(i) << endl;
                 return os;
                }
 private:
  FrameStyle style;

  Columns( unsigned, unsigned, comp_fn_tp){} //hide
  char *    GetBigEnoughBuffer();

};

class PageHdr : public Columns
{
 unsigned PageNo
         ,Width;

 public:

  PageHdr(Columns::FrameStyle fs = Columns::Basic)
         : Columns(fs), PageNo(1) , Width(80){};
  PageHdr(unsigned width, const char * txt,
          Columns::FrameStyle fs = Columns::Basic,
          const char * txtPage = 0, unsigned number = 1);
  ~PageHdr() {};
  PageHdr(const PageHdr & other) { *this = other; }
  PageHdr&  operator=(const PageHdr&);
  unsigned    pageNo() { return PageNo; }
  unsigned    write(char * Buffer,unsigned * pLines,FILE *f = 0);
  unsigned    write(char * Buffer,unsigned * pLines,Boolean fPageBreak,FILE *f = 0);
  unsigned    write(char * Buffer,unsigned * pLines,char * szPageBreak,FILE *f = 0);
  unsigned    writeJustText(const char *Text,char * Buffer,
                            unsigned * pLines, Column::Justify j, FILE *f);
  unsigned    writeSubHeader(const char *Text,char * Buffer,unsigned * pLines,
                             Column::Justify j = Column::Center,FILE *f = 0);
  static IString  displayPageBreak(unsigned pageWidth);
  static const IString  strPageBreak;

};

class ItemHdr : public Columns
{
 public:

  ItemHdr(FrameStyle fs = Columns::Basic) : Columns(fs) {};
  ~ItemHdr() {};
  ItemHdr(const ItemHdr & other) { *this = other; }
  ItemHdr&  operator=(const ItemHdr&);
  unsigned    WriteHdrOnly(char * Buffer,unsigned * pLines = 0,
                           const char * ContinueText = 0,FILE *f = 0);
  unsigned    Write(char * Buffer,unsigned * pLines = 0,
                    const char * ContinueText = 0,FILE *f = 0);
  unsigned    Write( char * Buffer,Columns & subhdr ,unsigned * pLines = 0,
                     const char * ContinueText = 0 ,FILE *f = 0);
  unsigned    Write( char * Buffer,ItemHdr & subhdr ,unsigned * pLines = 0,
                     const char * ContinueText = 0 ,FILE *f = 0);
  unsigned    rows();

  friend ostream& operator<< (ostream& os, ItemHdr & hdr)
                { return os << *(Columns *)&hdr; }
};

#pragma pack()
#endif

