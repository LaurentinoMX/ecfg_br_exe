/*
/------+-----+-------------\
| Type |Model|Serial Number|
|------+-----+-------------|
| TTTT | MMM |  12-34567   |
|-----+-------+------------------------------+---------\
| Qty |Feature| Description                  | Install |
|     |       |                              |  Date   |
|-----+-------+------------------------------+---------|
|  2  |F223456| This is feature F223456      |12-14-94 |
|  2  |F223456| This is feature F223456      |12-14-94 |
\-----+-------+------------------------------+---------/


/------+-----+-------------+-------------\
| Type |Model|Serial Number| Continued.. |
|------+-----+-------------+-------------/
| TTTT | MMM |  12-34567   |
|-----+-------+------------------------------+---------\
| Qty |Feature| Description                  | Install |
|     |       |                              |  Date   |
|-----+-------+------------------------------+---------|
|  2  |F223456| This is feature F223456      |12-14-94 |
|  2  |F223456| This is feature F223456      |12-14-94 |
\-----+-------+------------------------------+---------/


/------+-----+-------------\
| Type |Model|Serial Number|
|------+-----+-------------|


 System : S123456   sensed : 12-8-94                                 Page  1
------------------------------------------------------------------------------


 System : S123456   sensed : 12-8-94                                 Page  2
------------------------------------------------------------------------------
*/

#include <isynonym.hpp> // Definition of Boolean etc
#include <string.h>
#include <stddef.h>
#include <istring.hpp>
#include "pcol6.hpp"

//const IString PageHdr::strPageBreak("");
const IString PageHdr::strPageBreak("\x0D\x0C");

int Column_comp(const void *p1,const void * p2)
{
 return ((Column *)p1)->compare(*(Column *)p2);
}

Column & Column::operator=(Column const& other)
{
 if (&other != this)
   init(other.Hdr[0],other.Hdr[1],other.Width,other.JustH,other.JustT);
 setText(other.Txt[0],other.Txt[1]);
 return *this;
}

void Column::init(const IString & hdr1,const IString & hdr2
                 ,unsigned w, Justify jH,Justify jT)
{
 Hdr[0] = hdr1;
 Hdr[1] = hdr2;
 JustH = (jH == Same)? ((jT != Same)? jT : Center) : jH;
 JustT = (jT == Same)? JustH : jT;
 Width = w;
// unsigned hdrlen = ALMAX(Hdr[0].length(),Hdr[1].length()) + 1;
 unsigned hdrlen = ALMAX(Hdr[0].length(),Hdr[1].length());
 if (!Width)
   Width = hdrlen;
 else if ( hdrlen > Width) {
        Hdr[0] = IString((char *)hdr1,hdrlen,0);
        Hdr[1] = IString((char *)hdr2,hdrlen,0);
        }
 Txt[0] = IString();
 Txt[1] = IString();
}

void Column::setText(const IString & t1, const IString &t2)
{
 Txt[0] = IString((char *)t1,Width,0);
 if (t2 != IString())
   Txt[1] = IString((char *)t2,Width,0);
}

void Column::setText(const char * t1, const char * t2)
{
 Txt[0] = IString(t1,Width,0);
 if (t2)
   Txt[1] = IString(t2,Width,0);
}

void Column::setHeaderText(const IString & t1, const IString &t2)
{
 Hdr[0] = IString((char *)t1,Width,0);
 if (t2 != IString())
   Hdr[1] = IString((char *)t2,Width,0);
}

void Column::setHeaderText(const char * t1, const char * t2)
{
 Hdr[0] = IString(t1,Width,0);
 if (t2)
    Hdr[1] = IString(t2,Width,0);
}

int Column::compare(Column const& other) const
{
 int i = (Width - other.Width);
 if ( !(i = strcmp((char *)Hdr[0],(char *)other.Hdr[0]))  &&
      !(i = strcmp((char *)Hdr[1],(char *)other.Hdr[1])))
   i = (JustH - other.JustH);
  return i;
}

/////////////////////////////////////////////////////////////


#define COLS_CT_NONE    0
#define COLS_CT_LSEP    1
#define COLS_CT_CHAR    2
#define COLS_CT_SEP     3
#define COLS_CT_RSEP    4

#define COLS_FT_TL       0
#define COLS_FT_TH       1
#define COLS_FT_TV       2
#define COLS_FT_TR       3
#define COLS_FT_RV       4
#define COLS_FT_RH       5
#define COLS_FT_BR       6
#define COLS_FT_BV       7
#define COLS_FT_BH       8
#define COLS_FT_BL       9
#define COLS_FT_LV      10
#define COLS_FT_LH      11
#define COLS_FT_IH      12
#define COLS_FT_IV      13
#define COLS_FT_IVH     14
#define COLS_FT_IBV     15
#define COLS_FT_ITV     16

char FrameChar[17][4] = {
                         {  '/' , 'Ú' , 'É' , 'É' },
                         {  '-' , 'Ä' , 'Í' , 'Í' },
                         {  '+' , 'Â' , 'Ë' , 'Ñ' },
                         { '\\' , '¿' , '»' , '»' },
                         {  '|' , '³' , 'º' , 'º' },
                         {  '+' , '´' , '¹' , '¶' },
                         {  '/' , 'Ù' , '¼' , '¼' },
                         {  '+' , 'Á' , 'Ê' , 'Ï' },
                         {  '-' , 'Ä' , 'Í' , 'Í' },
                         { '\\' , 'À' , 'È' , 'È' },
                         {  '|' , '³' , 'º' , 'º' },
                         {  '+' , 'Ã' , 'Ì' , 'Ç' },
                         {  '-' , 'Ä' , 'Í' , 'Ä' },
                         {  '|' , '³' , 'º' , '³' },
                         {  '+' , 'Å' , 'Î' , 'Å' },
                         {  '+' , 'Á' , 'Ê' , 'Á' },
                         {  '+' , 'Â' , 'Ë' , 'Â' }
                        };

Columns::Columns(FrameStyle s)
         : PtrArray(sizeof(Column))
          ,style(s)
{}

int Columns::add(const Column & c)
{
 return PtrArray::AddasLast(new Column(c));
}

Columns & Columns :: operator=(const Columns & other)
{
 if (this != &other) {
   for (unsigned n = 0, N = NumberElements() ; n < N ; n++ )
     delete element(n);
   *(PtrArray *)this = *(PtrArray *)&other;
   Clear();
   style = other.style;
   for (n = 0, N = other.NumberElements() ; n < N ; n++ )
     this->add(*(other.element(n)));
   }
 return (*this);
}

Columns & Columns :: operator+=(const Columns & other)
{
 for (unsigned n = 0, N = other.NumberElements() ; n < N ; n++ )
   this->add(*(other.element(n)));
 return (*this);
}

Column * Columns::element(unsigned i)const
{
 return (Column *)(PtrArray::Element(i));
}

void Columns::remove(const Column & c)
{
 Column * p = (Column *)(PtrArray::Find(&c,Column_comp));
 if (p) {
   PtrArray::DeletePtr(p);
   delete p;
   }
}

Columns ::~Columns()
{
 for (unsigned n = 0, N = NumberElements() ; n < N ; n++ )
   delete element(n);
}

Columns::FrameStyle Columns :: frameStyle()
{ return style;}

Columns & Columns :: setFrameStyle(FrameStyle s)
{
 style = s;
 return *this;
}

unsigned Columns::totalWidth()
{
 Column *pC;
 for (unsigned n = 0, w = 0, N = NumberElements() ; n < N ; n++ )
   if (pC = element(n))
     w += pC->width();
 return N? w + N + 1 + 1 : 2 + 1;
}

unsigned Columns::headerRows()
{
 Column *pC;
 for (unsigned n = 0, u = 0, N = NumberElements() ; (n < N) && (u < 2); n++ )
   if (pC = element(n))
     if ( pC->strHeading(1).length())
       u = 2;
     else if ( pC->strHeading(0).length())
            u = 1;
 return u;
}

unsigned Columns::textRows()
{
 Column *pC;
 for (unsigned n = 0, u = 0, N = NumberElements() ; (n < N) && (u < 2); n++ )
   if (pC = element(n))
//     if ( pC->strText(1).length())
     if (strlen(pC->text(1)))
       u = 2;
     else if (strlen(pC->text(0)))
            u = 1;
 return u;
}

unsigned Columns::writeFrameLine(FrameType type,char *Buffer,FILE *f)
{
 return writeFrameLine(type,Buffer? &Buffer : 0,f);
}

unsigned Columns::writeFrameLine(FrameType type,char **Buffer,FILE *f)
{
 switch(type) {
   case Top:      return writeFrameLine( FrameChar[COLS_FT_TL][style]
                                        ,FrameChar[COLS_FT_TH][style]
                                        ,FrameChar[COLS_FT_TV][style]
                                        ,FrameChar[COLS_FT_TR][style]
                                        ,Buffer,f );
   case Middle:   return writeFrameLine( FrameChar[COLS_FT_LH][style]
                                        ,FrameChar[COLS_FT_IH][style]
                                        ,FrameChar[COLS_FT_IVH][style]
                                        ,FrameChar[COLS_FT_RH][style]
                                        ,Buffer,f );
   case Bottom:   return writeFrameLine( FrameChar[COLS_FT_BL][style]
                                        ,FrameChar[COLS_FT_BH][style]
                                        ,FrameChar[COLS_FT_BV][style]
                                        ,FrameChar[COLS_FT_BR][style]
                                        ,Buffer,f );
   case Line:     return writeFrameLine( FrameChar[COLS_FT_IH][style == Basic? Basic : Solid]
                                        ,FrameChar[COLS_FT_IH][style == Basic? Basic : Solid]
                                        ,FrameChar[COLS_FT_IH][style == Basic? Basic : Solid]
                                        ,FrameChar[COLS_FT_IH][style == Basic? Basic : Solid]
                                        ,Buffer,f );
   case Across:   return writeFrameLine( FrameChar[COLS_FT_TH][style]
                                        ,FrameChar[COLS_FT_TH][style]
                                        ,FrameChar[COLS_FT_TH][style]
                                        ,FrameChar[COLS_FT_TH][style]
                                        ,Buffer,f );
   case TopRight: return writeFrameLine( FrameChar[COLS_FT_LV][style]
                                        ,FrameChar[COLS_FT_IH][style]
                                        ,FrameChar[COLS_FT_IVH][style]
                                        ,FrameChar[COLS_FT_TR][style]
                                        ,Buffer,f );
   case BotRight: return writeFrameLine( FrameChar[COLS_FT_LH][style]
                                        ,FrameChar[COLS_FT_IH][style]
                                        ,FrameChar[COLS_FT_IVH][style]
                                        ,FrameChar[COLS_FT_BR][style]
                                        ,Buffer,f );
   case TopLeft:  return writeFrameLine( FrameChar[COLS_FT_TL][style]
                                        ,FrameChar[COLS_FT_IH][style]
                                        ,FrameChar[COLS_FT_IVH][style]
                                        ,FrameChar[COLS_FT_RH][style]
                                        ,Buffer,f );
   }
 return false;
}

unsigned Columns::writeFrameLine( char Left, char Middle, char Sep, char Right
                                 ,char **ppBuf, FILE * f)
{
 Column * pC;
 unsigned n = 0, k = 0, N = NumberElements();
 char * Buffer;
 Boolean fOK = (N != 0);
 if (fOK) {
   Boolean fMine = (ppBuf == 0);
   if (fMine)
     if (!f)
       return 0;
     else Buffer = new char[totalWidth() + 5];
   else Buffer = *ppBuf;
   char *pB = Buffer;
   *pB++ = Left;
   for ( n = 0, k = 0 ; n < N ; n++ )
     if ((pC = element(n)) && (k = pC->width())) {
       while (k--)
         *pB++ = Middle;
       *pB++ = (n == (N-1))? Right : Sep;
       }
   *pB++ = '\x0D',*pB++ = '\x0A';
   if (f) {
     *pB++ = '\0';
     fOK = (fputs(Buffer,f) != EOF);
     }
   n = fOK? pB - Buffer : 0;
   if (fMine)
     delete[]  Buffer;
   else *ppBuf += n;
   return n;
   }
 return 0;
}

unsigned Columns::writeFrameLine( char Left, char Middle, char Sep, char Right
                                 ,char * Buffer, FILE * f)
{
 return writeFrameLine(Left,Middle,Sep,Right,Buffer? &Buffer : 0,f);
}


unsigned Columns::charType(unsigned index)
{
 Column * pC;
 unsigned n,i,k,N = NumberElements(), type = COLS_CT_NONE;
 if (N)
   if (index == 0)
     type = COLS_CT_LSEP;
   else if (index < (totalWidth()-1))
          if (index == (totalWidth()-2))
            type = COLS_CT_RSEP;
          else for ( n = 0, i = 0 ; (type == COLS_CT_NONE) && (n < N) ; n++ )
                 if ((pC = element(n)) && (k = pC->width())) {
                   i += (k+1);
                   if (index <= i)
                     type = (index == i)? COLS_CT_SEP : COLS_CT_CHAR;
                   }
 return type;
}

unsigned Columns::write(Boolean fText,char * Buffer,unsigned * pLines,FILE *f,
                        char cLeft,char cSep,char cRight)
{
 return write(fText,Buffer? &Buffer : 0,pLines,f,cLeft,cSep,cRight);
}

unsigned Columns::write(Boolean fText,char ** ppBuf,unsigned * pLines,FILE *f,
                        char cLeft,char cSep,char cRight)
{
 Column * pC;
 Boolean fMine = (ppBuf == 0)
        ,fOK = true;
 unsigned  lines = 0
          ,byteswritten = 0
          ,J = (fText? textRows() : headerRows())
          ,j,n, N, k;
 const char *pc;
 char * Buffer;
 int i1,i2,i3;
 if (fMine)
   if (!f)
     return 0;
   else Buffer = new char[totalWidth() + 5];
 else Buffer = *ppBuf;
 char *pB = Buffer;
 for (j = 0 ; fOK && j < J ; j++) {
   *pB++ = cLeft;
   for (n = 0 ,N = NumberElements() ; n < N ; n++ ) {
     if ((pC = element(n)) && (k = pC->width())) {
//       i2 = fText?  pC->strText(j).length() : pC->strHeading(j).length();
       i2 = fText? strlen(pC->text(j)) : strlen(pC->heading(j));
       switch (fText? pC->textJustified() : pC->headerJustified() ) {
         case Column::Left:
           i1 = 0;
           i3 = ALMAX(pC->width() - i2,0);
           if (i3 > 1 )
             i1++,i3--;
           break;
         case Column::Center:
           i1 = ALMAX(((int)(pC->width()) - i2)/2,0);
           i3 = ALMAX((int)(pC->width()) - (i1+i2),0);
           break;
         case Column::Right:
           i3 = 0;
           i1 = ALMAX((int)(pC->width()) - i2,0);
           if (i1 > 1 )
             i3++,i1--;
           break;
         }
       while (i1--)
         *pB++ = ' ';
       for (pc = fText? pC->text(j) : pC->heading(j) ; i2 ; i2--)
         *pB++ = *pc++;
       while (i3--)
         *pB++ = ' ';
       }
     *pB++ = (n == (N-1))? cRight : cSep;
     }
   *pB++ = '\x0D',*pB++ = '\x0A';
   if (f) {
     *pB = '\0';
     fOK = (fputs(Buffer,f) != EOF);
     }
   if (f || fMine) {
     byteswritten += (pB - Buffer);
     pB = Buffer;
     }
   if (fOK)
     lines++;
   }
 if (!(f || fMine))
     byteswritten = (pB - Buffer);
 if (fMine)
   delete[]  Buffer;
 else if (fOK)
        *ppBuf += byteswritten;
 if (pLines)
   *pLines += lines;
 return (fOK? byteswritten : 0);
}


void Columns :: setTexts( const char * sz0
                         ,const char * sz1
                         ,const char * sz2
                         ,const char * sz3
                         ,const char * sz4
                         ,const char * sz5
                         ,const char * sz6
                         ,const char * sz7
                         ,const char * sz8
                         ,const char * sz9 )
{
#if 0
// this code abends in IString's DLL when doing the second string if
// compiled with optimization tags /O /Oc
// I don't understand why yet (just the second string?) The appended code
// doesn't abend but puts out garbage for the second string.
// Answer: parameters are not stored in ascending order on the stack.
// In fact, the second parm is not stored on the stack at all
// ( it goes via a register - _Optlink convention)  .
 Column * pC;
 const char ** ppc = &sz0;
 for (unsigned n = 0 ,N = NumberElements() ; n < N && n <= 9 && *ppc; n++,ppc++ )
   if (pC = element(n))
     pC->setText(*ppc);
#endif

 Column * pC;
 const char * pc = 0;
 for (unsigned n = 0 ,N = NumberElements() ; n < N && n <= 9 ; n++) {
   switch (n) {
     case 0: pc = sz0; break;
     case 1: pc = sz1; break;
     case 2: pc = sz2; break;
     case 3: pc = sz3; break;
     case 4: pc = sz4; break;
     case 5: pc = sz5; break;
     case 6: pc = sz6; break;
     case 7: pc = sz7; break;
     case 8: pc = sz8; break;
     case 9: pc = sz9; break;
     }
   if (pc && (pC = element(n)))
     pC->setText(pc);
   }
}

void Columns :: setHeaderTexts( const char * sz0
                               ,const char * sz1
                               ,const char * sz2
                               ,const char * sz3
                               ,const char * sz4
                               ,const char * sz5
                               ,const char * sz6
                               ,const char * sz7
                               ,const char * sz8
                               ,const char * sz9 )
{
 Column * pC;
 const char * pc = 0;
 for (unsigned n = 0 ,N = NumberElements() ; n < N && n <= 9 ; n++) {
   switch (n) {
     case 0: pc = sz0; break;
     case 1: pc = sz1; break;
     case 2: pc = sz2; break;
     case 3: pc = sz3; break;
     case 4: pc = sz4; break;
     case 5: pc = sz5; break;
     case 6: pc = sz6; break;
     case 7: pc = sz7; break;
     case 8: pc = sz8; break;
     case 9: pc = sz9; break;
     }
   if (pc && (pC = element(n)))
     pC->setHeaderText(pc);
   }
}


unsigned Columns::writeHeaderText(char * Buffer,unsigned * pLines,FILE *f,
                                  char cLeft,char cSep,char cRight)
{ return write(false,Buffer,pLines,f,cLeft,cSep,cRight); }

unsigned Columns::writeHeaderText(char ** pBuffer,unsigned * pLines,FILE *f,
                                  char cLeft,char cSep,char cRight)
{ return write(false,pBuffer,pLines,f,cLeft,cSep,cRight); }

unsigned Columns::writeHeaderText(char * Buffer,unsigned * pLines,FILE *f)
{
  return write(false,Buffer,pLines,f,FrameChar[COLS_FT_LV][style],
                                     FrameChar[COLS_FT_IV][style],
                                     FrameChar[COLS_FT_RV][style]);
}

unsigned Columns::writeHeaderText(char ** pBuffer,unsigned * pLines,FILE *f)
{
 return write(false,pBuffer,pLines,f,FrameChar[COLS_FT_LV][style],
                                     FrameChar[COLS_FT_IV][style],
                                     FrameChar[COLS_FT_RV][style]);
}

unsigned Columns::writeText(char * Buffer,unsigned * pLines,FILE *f,
                            char cLeft,char cSep,char cRight)
{ return write(true,Buffer,pLines,f,cLeft,cSep,cRight); }

unsigned Columns::writeText(char ** pBuffer,unsigned * pLines,FILE *f,
                            char cLeft,char cSep,char cRight)
{ return write(true,pBuffer,pLines,f,cLeft,cSep,cRight); }

unsigned Columns::writeText(char * Buffer,unsigned * pLines,FILE *f)
{
 return write(true,Buffer,pLines,f,FrameChar[COLS_FT_LV][style],
                                   FrameChar[COLS_FT_IV][style],
                                   FrameChar[COLS_FT_RV][style]);
}

unsigned Columns::writeText(char ** pBuffer,unsigned * pLines,FILE *f)
{
 return write(true,pBuffer,pLines,f,FrameChar[COLS_FT_LV][style],
                                    FrameChar[COLS_FT_IV][style],
                                    FrameChar[COLS_FT_RV][style]);
}

unsigned Columns::writeHeader(char * Buf,unsigned * pLines,FILE *f)
{
 return writeHeader(Buf? &Buf: 0,pLines,f);
}

unsigned Columns::writeHeader(char ** pBuffer,unsigned * pLines,FILE *f)
{
 unsigned u , uTot = 0, lines = 0;
 if ( (uTot = writeFrameLine(Columns::Top,pBuffer,f)) && ++lines)
  if (u = writeHeaderText(pBuffer,&lines,f)) {
    uTot += u;
    if (u = writeFrameLine( Columns::Middle,pBuffer,f)) {
      uTot += u;
      lines++;
      }
    }
 if (pLines)
   *pLines += lines;
 return uTot;
}

unsigned Columns::writeAppendHeader(char * Buf,unsigned * pLines,FILE *f)
{
 return writeAppendHeader(Buf? &Buf : 0,pLines,f);
}

unsigned Columns::writeAppendHeader(char ** pBuffer,unsigned * pLines,FILE *f)
{
 unsigned u , uTot = 0, lines = 0;
 if ( (uTot = writeFrameLine(Columns::TopRight,pBuffer,f)) && ++lines)
  if (u = writeHeaderText(pBuffer,&lines,f)) {
    uTot += u;
    if (u = writeFrameLine(Columns::Middle,pBuffer,f)) {
      uTot += u;
      lines++;
      }
    }
 if (pLines)
   *pLines += lines;
 return uTot;
}

unsigned Columns::writeToplessHeader(char * Buf,unsigned * pLines,FILE *f)
{
 return writeToplessHeader(Buf? &Buf : 0,pLines,f);
}

unsigned Columns::writeToplessHeader(char ** pBuffer,unsigned * pLines,FILE *f)
{
 unsigned u , uTot = 0, lines = 0;
 if (uTot = writeHeaderText(pBuffer,&lines,f))
   if (u = writeFrameLine(Columns::Middle,pBuffer,f)) {
     uTot += u;
     lines++;
     }
 if (pLines)
   *pLines += lines;
 return uTot;
}

unsigned Columns::writeMergingFrameLine(Columns & other,Boolean fLookUp,
                                        char ** pBuf, FILE * f)
{
 Column * pC;
 unsigned i,n,k,N = NumberElements();
 char * Buffer;
 Boolean fOK = (N != 0);
 if (fOK) {
   Boolean fMine = (pBuf == 0);
   if (fMine)
     if (!f)
       return 0;
     else Buffer = new char[totalWidth() + 5];
   else Buffer = *pBuf;
   char *pB = Buffer;
   *pB++ = FrameChar[COLS_FT_LH][style];
   for ( n = 0, k = 0, i = 1 ; n < N ; n++ )
     if ((pC = element(n)) && (k = pC->width())) {
       while (k--) {
         switch(other.charType(i)) {
           case COLS_CT_NONE: *pB++ = FrameChar[COLS_FT_TH][style]; break;
           case COLS_CT_CHAR: *pB++ = FrameChar[COLS_FT_IH][style]; break;
           case COLS_CT_RSEP:
             if (style==Double_Solid)
               *pB++ = FrameChar[fLookUp? COLS_FT_BL : COLS_FT_TR ][style];
             else *pB++ = FrameChar[fLookUp? COLS_FT_BV : COLS_FT_TV ][style];
             break;
           case COLS_CT_SEP :
             *pB++ = FrameChar[fLookUp? COLS_FT_IBV : COLS_FT_ITV ][style];
           }
         i++;
         }
       if (n == (N-1))                  //right border
         switch(other.charType(i)) {
           case COLS_CT_NONE:
             *pB++ = FrameChar[fLookUp? COLS_FT_TR : COLS_FT_BR ][style];
             break;
           case COLS_CT_CHAR:
             *pB++ = FrameChar[fLookUp? COLS_FT_TV : COLS_FT_BV ][style];
             break;
           case COLS_CT_RSEP: *pB++ = FrameChar[COLS_FT_RH][style]; break;
           case COLS_CT_SEP :
             *pB++ = FrameChar[fLookUp? COLS_FT_RH : COLS_FT_IVH ][style];
           }
       else switch(other.charType(i)) {  // separator
              case COLS_CT_NONE:
                *pB++ = FrameChar[fLookUp? COLS_FT_TV : COLS_FT_BV ][style];
                break;
              case COLS_CT_CHAR:
                *pB++ = FrameChar[fLookUp? COLS_FT_ITV : COLS_FT_IBV ][style];
                break;
              case COLS_CT_RSEP:
                if (style == Double_Solid)
                  *pB++ = FrameChar[fLookUp? COLS_FT_LH : COLS_FT_TL ][style];
                else *pB++ = FrameChar[COLS_FT_IVH][style];
                break;
              case COLS_CT_SEP :
                *pB++ = FrameChar[COLS_FT_IVH][style];
              }
       i++;
       }
   *pB++ = '\x0D',*pB++ = '\x0A';
   if (f) {
     *pB++ = '\0';
     fOK = (fputs(Buffer,f) != EOF);
     }
   n = fOK? pB - Buffer : 0;
   if (fMine)
     delete[]  Buffer;
   else *pBuf += n;
   return n;
   }
 return 0;
}

unsigned Columns::writeMergingFrameLine(Columns & other,Boolean fLookUp,
                                         char * Buf, FILE * f)
{
 return writeMergingFrameLine(other,fLookUp, Buf? & Buf : 0,f);
}

unsigned Columns::closeOff(char ** pBuffer,unsigned * pLines,FILE *f)
{
 unsigned u = writeFrameLine(Bottom,pBuffer,f);
 if (u && pLines)
   *pLines += 1;
 return u;
}

unsigned Columns::closeOff(char * Buffer,unsigned * pLines,FILE *f)
{ return closeOff(Buffer ? &Buffer : 0,pLines,f); }

////////////////////////////////////////

PageHdr::PageHdr(unsigned width, const char * txt, Columns::FrameStyle fs,
                 const char * txtPage, unsigned number)
             : Columns(fs)
              ,PageNo(number)
              ,Width(ALMAX(width,8))
{
 unsigned lenP, lenT;
 IString strText(txt)
        ,strPText(txtPage)
        ,strNmbr(999);
 Column colText(strText);
 Column colPageText(strPText);
 Columns temp;
 temp.add(colText);
 temp.add(colPageText);
 temp.add(Column(strNmbr));
 lenT = temp.totalWidth();
 lenP = colPageText.width();
 if (lenT <= width) {            // texts fit within the given width
   lenP += (width - lenT);  // give extra space to the page column
   lenT = colText.width();
   }
 else {    // texts don't fit. Take excess away from the text
   lenT = colText.width() - (lenT - width);
   lenP = colPageText.width();
   }
 Columns::add(Column(strText,lenT,Column::Left));
 Columns::add(Column(strPText,lenP,Column::Right));
 Columns::add(Column(IString(number),4));
}

PageHdr & PageHdr :: operator=(const PageHdr & other)
{
 if (this != &other) {
   PageNo = other.PageNo;
   Width  = other.Width;
   *(Columns *)this = other;
   }
 return *this;
}

IString PageHdr :: displayPageBreak(unsigned pageWidth)
{
// return IString(IString("\x0D\x0A") + IString("",pageWidth,'-') +
//                IString("\x0D\x0A\x0D\x0A"));
// return IString(IString(0,pageWidth,'-') + IString("\x0D\x0A"));
  return IString("\x0D\x0A\x0D\x0A",4,
                 (char *)IString(0,pageWidth,'-'),pageWidth,
                 "\x0D\x0A\x0D\x0A",4);
}

unsigned PageHdr :: write(char * Buffer,unsigned * pLines,FILE *f)
{
 Columns::element(2)->setHeaderText(IString(PageNo++));
 unsigned bytes2,
          bytes1 = writeHeaderText(Buffer,pLines,f,' ',' ',' ');
 if ( bytes1 &&
     (bytes2 = writeFrameLine(Columns::Across, Buffer? Buffer+bytes1 : 0,f))) {
   bytes1 += bytes2;
   if (pLines)
     (*pLines)++;
   }
 return bytes1;
}

//char *pPB_Stub  = "\x0D\x0A>>Pagebreak<<\x0D\x0A\x0D\x0A";
//extern char *pPB_Stub;

unsigned PageHdr :: write(char * Buffer,unsigned * pLines,
                          Boolean fPageBreak,FILE *f)
{
 unsigned bytes0 = 0, bytes1;
 if (pLines)
   *pLines = 0;
 if (!Buffer && !f)
   return 0;
 if (fPageBreak) {
   IString str(strPageBreak);
   str += IString("\x0D\x0A");
   bytes0 = str.length();
   if (f) {
     if (fputs((char *)str,f) == EOF)
       return 0;
     }
   else memcpy(Buffer,(char *)str,bytes0);
   }
 if (bytes1 = write(Buffer,pLines,f))
   bytes0 += bytes1;
 return bytes0;
}

unsigned PageHdr :: write(char * Buffer,unsigned * pLines,
                          char * PageBreak,FILE *f)
{
 unsigned bytes0 = 0, bytes1;
 if (pLines)
   *pLines = 0;
 if (!Buffer && !f)
   return 0;
 if (PageBreak) {
   IString PB(PageBreak);
   unsigned l = PB.length();
   if ( (l < 2) || (PageBreak[l-2] != '\x0D') || (PageBreak[l-1] != '\x0A') )
     PB +=  "\x0D\x0A";
   if (f) {
     if (fputs((char *)PB,f) == EOF)
       return 0;
     }
   else memcpy(Buffer,(char *)PB,PB.length());
   bytes0 = PB.length();
   }
 if (bytes1 = write(Buffer,pLines,f))
   bytes0 += bytes1;
 return bytes0;
}

unsigned PageHdr :: writeSubHeader(const char *Text,char * Buffer,
                                    unsigned * pLines,
                                    Column::Justify j, FILE *f)
{
 Columns C(frameStyle());
 C.add(Column(Text,Width-2,j));
 unsigned bytes2
         ,bytes1 = C.writeHeaderText(Buffer,pLines,f,' ',' ',' ');
 if ( bytes1 &&
     (bytes2 = C.writeFrameLine(Columns::Across, Buffer ? Buffer+bytes1 : 0,f)))
   { bytes1 += bytes2;
     if (pLines)
       (*pLines)++;
   }
 return bytes1;
}

unsigned PageHdr :: writeJustText(const char *Text,char * Buffer,
                                  unsigned * pLines,
                                  Column::Justify j, FILE *f)
{
 Columns C(frameStyle());
 C.add(Column(Text,Width-2,j));
 return C.writeHeaderText(Buffer,pLines,f,' ',' ',' ');
}

////////////////////////////////////////////

ItemHdr & ItemHdr :: operator=(const ItemHdr & other)
{
 if (this != &other)
   *(Columns *)this = other;
 return *this;
}

unsigned ItemHdr :: rows()
{
 return (headerRows() + textRows() + 2);
}

unsigned ItemHdr::WriteHdrOnly(char * Buf,unsigned * pLines,
                               const char * ContinueText,FILE *f)
{
 unsigned u , uTot = 0, lines = 0;
 if (ContinueText) {
   ItemHdr temp(*this);
   Column col(ContinueText);
   add(col);
   if ( (uTot = writeFrameLine(Columns::Top,Buf,f)) && ++lines)
     if (u = writeHeaderText(Buf? Buf+uTot : 0,&lines,f)) {
       uTot += u;
       if (u = writeMergingFrameLine(*(Columns *)&temp,false,
                                     Buf? Buf+uTot : 0,f)) {
         uTot += u;
         lines++;
         }
       }
   remove(col);
   }
 else if ( (uTot = writeFrameLine(Columns::Top,Buf,f)) && ++lines)
        if (u = writeHeaderText(Buf? Buf+uTot : 0,&lines,f)) {
          uTot += u;
          if (u = writeFrameLine(Columns::Middle, Buf? Buf+uTot : 0,f)) {
            uTot += u;
            lines++;
            }
          }
 if (pLines)
   *pLines += lines;
 return uTot;
}

unsigned ItemHdr::Write(char * Buf,unsigned * pLines,
                        const char * ContinueText,FILE *f)
{
 unsigned u , uTot = 0, lines = 0;
 if ( uTot = WriteHdrOnly(Buf,&lines,ContinueText,f))
   if (u = writeText(Buf? Buf+uTot : 0,&lines,f))
     uTot += u;
 if (pLines)
   *pLines += lines;
 return uTot;
}

unsigned ItemHdr :: Write( char * Buf ,ItemHdr & subhdr ,unsigned * pLines
                          ,const char * ContinueText ,FILE *f)
{ return Write(Buf,*(Columns *)&subhdr,pLines,ContinueText,f);}

unsigned ItemHdr :: Write( char * Buf ,Columns & subhdr ,unsigned * pLines
                          ,const char * ContinueText ,FILE *f)
{
 unsigned bytesT = Write(Buf,pLines,ContinueText,f)
        , bytes2 = 0;
 if (bytesT) {
   bytes2 = (totalWidth() > subhdr.totalWidth())?
                  writeMergingFrameLine(subhdr,false,Buf? Buf+bytesT : 0,f)
                : subhdr.writeMergingFrameLine(*(Columns *)this,true,
                                                Buf? Buf+bytesT : 0,f);
   if (bytes2) {
     bytesT += bytes2;
     if (pLines)
       (*pLines)++;
     bytesT += subhdr.writeToplessHeader(Buf? Buf+bytesT : 0,pLines,f);
     }
   }
 return bytesT;
}

////////////////////////////////////////

#ifdef UNITTEST

//#include "euoutil.cpp"
#include <stdlib.h>
#include <iostream.h>


void main(void)
{
 if (1)
   {
#if 0
    FILE *f;
    Columns cols;
    cols.add(Column("Header 1"));
    cols.add(Column("Header 2",20));
    cols.add(Column("Header 3 with a",30,Column::Center,"subheader"));
    cols.add(Column("Header 3",20,Column::Right));
    cols.element(0)->setText("text 1");
    cols.element(1)->setText("text 2");
    cols.element(2)->setText("text 3","(subtext)");
//    cols.element(2)->setText("text 3");
    cols.element(3)->setText("text 4");
    cout << cols << endl;
    if (f = fopen("PCOL.TST","wb"))
      { cols.writeHeader(f);
        cols.writeText(f);
        cols.writeFrame(Columns::Bottom,0,f);
        fclose(f);
      }
#endif

    FILE *f;
    Columns colsM, colsF,colsMTemp;
//    ColumnsF colsfM, colsfF,colsfMTemp;
    colsM.add(Column("Type",6));
    colsM.add(Column("Model"));
    colsM.add(Column("Serial Number"));

    colsF.add(Column("Qty",5));
    colsF.add(Column("Feature"));
    colsF.add(Column("Description",30,Column::Left));
    colsF.add(Column("Install",9,Column::Center,Column::Left,"Date"));


    Column ContinueCol("Continued..",13);
    colsM.element(0)->setText("TTTT");
    colsM.element(1)->setText("MMM");
    colsM.element(2)->setText("12-34567");

    colsF.element(0)->setText("1");
    colsF.element(1)->setText("F123456");
    colsF.element(2)->setText("This is feature F123456");
    colsF.element(3)->setText("12-13-94");

    if (f = fopen("PCOL.TST","wb"))
      {

       colsM.writeFrameLine(Columns::Top,(char *)0,f);
       colsM.writeHeaderText((char *)0,0,f);
       colsM.writeFrameLine(Columns::Middle,(char *)0,f);
       colsM.writeText((char *)0,0,f);
       colsF.writeFrameLine(Columns::TopRight,(char *)0,f);
       colsF.writeHeaderText((char *)0,0,f);
       colsF.writeFrameLine(Columns::Middle,(char *)0,f);

       colsF.writeText((char *)0,0,f);

       colsF.element(0)->setText("2");
       colsF.element(1)->setText("F223456");
       colsF.element(2)->setText("This is feature F223456");
       colsF.element(3)->setText("12-14-94");
       colsF.writeText((char *)0,0,f);

       colsF.writeFrameLine(Columns::Bottom,(char *)0,f);

       fclose(f);
      }

    char * Buffer = new char[10000];
    if (Buffer)
      {
        char * pB = Buffer;
        unsigned u, uLines ;

/*
/------+-----+-------------\
| Type |Model|Serial Number|
|------+-----+-------------|
| TTTT | MMM |  12-34567   |
|-----+-------+------------------------------+---------\
| Qty |Feature| Description                  | Install |
|     |       |                              |  Date   |
|-----+-------+------------------------------+---------|
|  2  |F223456| This is feature F223456      |12-14-94 |
|  2  |F223456| This is feature F223456      |12-14-94 |
\-----+-------+------------------------------+---------/
*/
//////////////////////////////////////////////////////////////////


        pB += (u = colsM.writeFrameLine(Columns::Top,pB));
        pB += (u = colsM.writeHeaderText(pB,&uLines));
        pB += (u = colsM.writeFrameLine(Columns::Middle,pB));
        pB += (u = colsM.writeText(pB,&uLines));
        pB += (u = colsF.writeFrameLine(Columns::TopRight,pB));
        pB += (u = colsF.writeHeaderText(pB,&uLines));
        pB += (u = colsF.writeFrameLine(Columns::Middle,pB));

        pB += (u = colsF.writeText(pB,&uLines));

        colsF.element(0)->setText("2");
        colsF.element(1)->setText("F223456");
        colsF.element(2)->setText("This is feature F223456");
        colsF.element(3)->setText("12-14-94");
        pB += (u = colsF.writeText(pB,&uLines));

        pB += (u = colsF.writeFrameLine(Columns::Bottom,pB));
//        *pB++ = '\n';
        *pB++ = '\x0D',*pB++ = '\x0A';
        *pB++ = '\x0D',*pB++ = '\x0A';
// double solid

        colsM.setFrameStyle(Columns::Double_Solid);
        colsF.setFrameStyle(Columns::Double_Solid);
        pB += (u = colsM.writeFrameLine(Columns::Top,pB));
        pB += (u = colsM.writeHeaderText(pB,&uLines));
        pB += (u = colsM.writeFrameLine(Columns::Middle,pB));
        pB += (u = colsM.writeText(pB,&uLines));
        pB += (u = colsF.writeMergingFrameLine(colsM,true,pB));
        pB += (u = colsF.writeHeaderText(pB,&uLines));
        pB += (u = colsF.writeFrameLine(Columns::Middle,pB));

        pB += (u = colsF.writeText(pB,&uLines));

        colsF.element(0)->setText("2");
        colsF.element(1)->setText("F223456");
        colsF.element(2)->setText("This is feature F223456");
        colsF.element(3)->setText("12-14-94");
        pB += (u = colsF.writeText(pB,&uLines));

        pB += (u = colsF.writeFrameLine(Columns::Bottom,pB));
//        *pB++ = '\n';
        *pB++ = '\x0D',*pB++ = '\x0A';
        *pB++ = '\x0D',*pB++ = '\x0A';

//solid



        colsM.setFrameStyle(Columns::Solid);
        colsF.setFrameStyle(Columns::Solid);
        pB += (u = colsM.writeFrameLine(Columns::Top,pB));
        pB += (u = colsM.writeHeaderText(pB,&uLines));
        pB += (u = colsM.writeFrameLine(Columns::Middle,pB));
        pB += (u = colsM.writeText(pB,&uLines));
        pB += (u = colsF.writeMergingFrameLine(colsM,true,pB));
        pB += (u = colsF.writeHeaderText(pB,&uLines));
        pB += (u = colsF.writeFrameLine(Columns::Middle,pB));

        pB += (u = colsF.writeText(pB,&uLines));

        colsF.element(0)->setText("2");
        colsF.element(1)->setText("F223456");
        colsF.element(2)->setText("This is feature F223456");
        colsF.element(3)->setText("12-14-94");
        pB += (u = colsF.writeText(pB,&uLines));

        pB += (u = colsF.writeFrameLine(Columns::Bottom,pB));
//        *pB++ = '\n';
        *pB++ = '\x0D',*pB++ = '\x0A';
        *pB++ = '\x0D',*pB++ = '\x0A';

//end solid



/*
/------+-----+-------------+-------------\
| Type |Model|Serial Number| Continued.. |
|------+-----+-------------+-------------/
| TTTT | MMM |  12-34567   |
|-----+-------+------------------------------+---------\
| Qty |Feature| Description                  | Install |
|     |       |                              |  Date   |
|-----+-------+------------------------------+---------|
|  2  |F223456| This is feature F223456      |12-14-94 |
|  2  |F223456| This is feature F223456      |12-14-94 |
\-----+-------+------------------------------+---------/
*/
//solid
        colsMTemp = colsM;
        colsM.add(ContinueCol);
        pB += (u = colsM.writeFrameLine(Columns::Top,pB));
        pB += (u = colsM.writeHeaderText(pB,&uLines));
        pB += (u = colsM.writeMergingFrameLine(colsMTemp,false,pB));
        colsM.remove(ContinueCol);
        pB += (u = colsM.writeText(pB,&uLines));
        pB += (u = colsF.writeMergingFrameLine(colsM,true,pB));
        pB += (u = colsF.writeHeaderText(pB,&uLines));
        pB += (u = colsF.writeFrameLine(Columns::Middle,pB));

        pB += (u = colsF.writeText(pB,&uLines));

        colsF.element(0)->setText("2");
        colsF.element(1)->setText("F223456");
        colsF.element(2)->setText("This is feature F223456");
        colsF.element(3)->setText("12-14-94");
        pB += (u = colsF.writeText(pB,&uLines));

        pB += (u = colsF.writeFrameLine(Columns::Bottom,pB));
        *pB++ = '\x0D',*pB++ = '\x0A';
        *pB++ = '\x0D',*pB++ = '\x0A';
//double_solid
        colsM.setFrameStyle(Columns::Double_Solid);
        colsF.setFrameStyle(Columns::Double_Solid);
        colsMTemp = colsM;
        colsM.add(ContinueCol);
        pB += (u = colsM.writeFrameLine(Columns::Top,pB));
        pB += (u = colsM.writeHeaderText(pB,&uLines));
        pB += (u = colsM.writeMergingFrameLine(colsMTemp,false,pB));
        colsM.remove(ContinueCol);
        pB += (u = colsM.writeText(pB,&uLines));
        pB += (u = colsF.writeMergingFrameLine(colsM,true,pB));
        pB += (u = colsF.writeHeaderText(pB,&uLines));
        pB += (u = colsF.writeFrameLine(Columns::Middle,pB));

        pB += (u = colsF.writeText(pB,&uLines));

        colsF.element(0)->setText("2");
        colsF.element(1)->setText("F223456");
        colsF.element(2)->setText("This is feature F223456");
        colsF.element(3)->setText("12-14-94");
        pB += (u = colsF.writeText(pB,&uLines));

        pB += (u = colsF.writeFrameLine(Columns::Bottom,pB));
        *pB++ = '\x0D',*pB++ = '\x0A';
        *pB++ = '\x0D',*pB++ = '\x0A';

///////////////
        pB += (u = colsM.writeFrameLine(Columns::Top,pB));
        pB += (u = colsM.writeHeaderText(pB,&uLines));
        pB += (u = colsM.writeFrameLine(Columns::Middle,pB));

        *pB++ = '\x0D',*pB++ = '\x0A';
        *pB++ = '\x0D',*pB++ = '\x0A';


        Columns colsH(Columns::Double);
        colsH.add(Column("System :"));
        colsH.add(Column("S123456 "));
        colsH.add(Column(" sensed :"));
        colsH.add(Column("12-8-94"));
        colsH.add(Column(" Page  1",40,Column::Right));
        pB += (u = colsH.writeHeaderText(pB,&uLines,0,' ',' ',' '));
        pB += (u = colsH.writeFrameLine(Columns::Across,pB));

        *pB++ = '\x0D',*pB++ = '\x0A';
        *pB++ = '\x0D',*pB++ = '\x0A';

        colsH.element(4)->setHeaderText(" Page  2");
        pB += (u = colsH.writeHeaderText(pB,&uLines,0,' ',' ',' '));
        pB += (u = colsH.writeFrameLine(Columns::Line,pB));

        *pB++ = '\x0D',*pB++ = '\x0A';
        *pB++ = '\x0D',*pB++ = '\x0A';

        *pB = '\0';


       if (f = fopen("PCOL.TSB","wb"))
         {
           fputs(Buffer,f);
          fclose(f);
        }



        delete[] Buffer;
      }

   }

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif

}

#endif

#if 0
/////////////////////////////////////////////////////////////////////////
// this code puts out garbage for the second string if
// compiled with optimization tags /O /Oc
// I don't understand why yet (just the second string?) Posed this test case
// on the VACPP Forum on 10-4-95
// file x.hpp
// compile with
// icc /B"/L /map" /Tdp /Ti+ /Tm+ /O /Oc -Gm+ %1.cpp
// or
// icc /B"/L /map" /Tdp /Ti+ /Tm+ -Gm+ %1.cpp
// to see the difference
/////////////
class X
{
 unsigned count;
 IString  *pStr;
 public:
  X();
  void        setTexts( const char * sz0 = 0
                       ,const char * sz1 = 0
                       ,const char * sz2 = 0
                       ,const char * sz3 = 0
                      );
  void      showTexts();
};
///////////////////////////////////////////////////////
#include <errno.h>
#include <stdlib.h>
#include <iostream.h>
#include <istring.hpp>
//JT #include "x2.hpp"

X::X()
{ pStr = new IString[4]; }

void X :: setTexts( const char * sz0
                   ,const char * sz1
                   ,const char * sz2
                   ,const char * sz3 )
{
 const char ** ppc = &sz0;
 for (count = 0; count < 4 && *ppc; count++,ppc++ )
   *(pStr + count) = IString(*ppc);
}

void X::showTexts()
{
 for (int i = 0; i < count; i++)
   cout << *(pStr + i) << endl;
}

void main(void)
{
 X x;
 x.setTexts("one","two","three");
 x.showTexts();
}
#endif
