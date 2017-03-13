# include "parr.hpp"

//*****************************************************************************
//    Reentrant
//   Parses a given string as per given character extracting the parsed
//   field into a given buffer, thereby leaving the string intact
//   returns
//       char * the location of the byte after the extracted field in the string
//       NULL   end of the string was reached.
//*****************************************************************************
char *MystrtokExtract(char *Buf, char *String, char *Tokens)
{
 while (*String && !strchr(Tokens,*String))
    *Buf++ = *String++;
 *Buf = 0;
 return  *String ? String + 1 : 0;
}

//*****************************************************************************
//       Reentrant
//   Parses a given string as per given character altering the string by
//   null terminating the token in the given string. Assigns the location
//   of the byte following the parsed substring if the end of string has not
//   yet been reached, else assigns NULL
//   returns
//       char * location of the string to be parsed.
//*****************************************************************************
char *Mystrtok(char **ppszNext, char *String, char *Tokens)
{
 char *pch;
 if (!String)
   return *ppszNext = 0;
 for (pch = String; *pch && !strchr(Tokens,*pch); pch++);
 *ppszNext = *pch?  pch+1 : 0;
 if (*pch)
    *pch = 0;
 return  String;
}

/////////////////////////////////////////////////////

PtrStrArr::PtrStrArr( unsigned   Initial_Size
                     ,unsigned   Automatic_Expansion
                     ,comp_fn_tp Compare_Function )
        : PtrArray(Initial_Size,Automatic_Expansion,Compare_Function)
{ }


PtrStrArr::PtrStrArr(const PtrStrArr& other)
{
  P= 0;
  Clear();
 *(PtrArray *)this = *(PtrArray *)&other;
}

PtrStrArr & PtrStrArr :: operator=(const PtrStrArr & other)
{
 *(PtrArray *)this = *(PtrArray *)&other;
 return (*this);
}

PtrStrArr & PtrStrArr :: operator+=(const PtrStrArr & other)
{
 RC = AddArray(other);
 return *this;
}

PtrStrArr & PtrStrArr :: operator+(void *p)
{
 RC = Add(p);
 return *this;
}

PtrStrArr & PtrStrArr :: operator-(void *p)
{
 RC = Delete(p);
 return *this;
}

int PtrStrArr::AddArray(const PtrStrArr& other)
{
  return PtrArray::AddArray(*(PtrArray *)&other);
}

int PtrStrArr::Delete(const void *p,comp_fn_tp comp_fn_in)
{
 return PtrArray::Delete(p,comp_fn_in);
}

int PtrStrArr::DeletePtr(void *p)
{
 return PtrArray::DeletePtr(p,0);
}

int PtrStrArr::DeleteIndex(int index)
{
 return PtrArray::DeleteIndex(index);
}
/*
Function name: WriteToFile

Write the strings addressed by the string pointers in the array to an asciiz
file
                     Note:
   *************************************************************************
   *The counterpart to this function ('LoadFile') uses 'fgets' to load an  *
   *array from an ASCII file. 'fgets' reads a file up to the new line      *
   *character '\n' which it includes in the string before adding a null    *
   *terminator. All lines in a file end with a linefeed-carriage return    *
   *combo. Therefore, if the last character in the string is not a line-   *
   *feed, the null terminator is temporarily replaced by a linefeed (since *
   *'fwrite' changes linefeeds with a LF-CR combo) and the number of bytes *
   *to be written out is extended by one to include the linefeed. After    *
   *that, the null terminator to the string is restored.                   *
   *If the last character in the string is already a linefeed (for example *
   *the strings could have originated from a file created by this function)*
   *the string is written out as is for a length such that the linefeed    *
   *is the last byte.                                                      *
   *************************************************************************

*/
int PtrStrArr :: WriteToFile(char *filename)
{
 FILE *f;
 int i,j;
 char *pc;
 if (!(f = fopen(filename,"a")))
   return PARR_NO_FILE;
 for ( i=0 ; i < N ; i++)
   { j = strlen(pc=(char *)P[i]);
     if (pc[j-1] != 10)
      { pc[j] = 10;
        fwrite(pc,j + 1,1,f);
        pc[j]=0;
      }
    else fwrite(pc,j,1,f);
   }
 fclose(f);
 return PARR_OK;
}

int PtrStrArr::Replace(void *p, void *pRep, unsigned *pCount,
                       comp_fn_tp comp_fn_in)
{
 return MyReplace(p,pRep,comp_fn_in,pCount);
}

int PtrStrArr::RemoveDupes(comp_fn_tp comp_fn_in)
{
 return PtrArray::RemoveDupes(comp_fn_in,0);
}
#if 0
int PtrStrArr::Tokenize( char *String, char *Tokenchars, unsigned *pCount)
{
 char *s1;
 unsigned n = 0;
 int rc = PARR_OK;
 for (s1 = strtok(String,Tokenchars); s1 && rc == PARR_OK;)
   if ( (rc = Add(s1)) == PARR_OK)
     { n++;
       s1 = strtok(0,Tokenchars);
     }
 if (pCount) *pCount = n;
 return rc;
}
#endif

int PtrStrArr::Tokenize( char *String, char *Tokenchars, unsigned *pCount)
{
 char *s1;
 unsigned n = 0;
 int rc = PARR_OK;
 while ( (s1 = Mystrtok(&String,String,Tokenchars))  && (rc == PARR_OK))
   if ( (rc = Add(s1)) == PARR_OK)
     n++;
 if (pCount) *pCount = n;
 return rc;
}

int  PtrStrArr::AppendString(char *p,int fDupe)
{
 if (!fDupe)
    P[N++] = p;
 else if (!(P[N++] = strdup(p)))
             { N--;
               return  PARR_NO_MEM;
             }
 return PARR_OK;
}

int PtrStrArr::MyParse( char *String, char *Delim,unsigned *pCount, int fDupe)
{
 char *p1,*p2 = String;                    /* working pointers             */
 unsigned count;
 for ( p1 = String,count = 0; p1 ; count++)   /* count the number of elements */
   if (p1=strpbrk(p1,Delim))
      p1++;
 int  rc = AccomodateNPtrs(count);
 for (count = N; rc == PARR_OK && (p1 = strpbrk(p2,Delim));) // save current count
   { *p1 = 0;                     /* make delimeter a null terminator    */
     rc = AppendString(p2,fDupe);
     p2 = p1+1;
   }
 if (rc == PARR_OK)                         // do last token
    rc = AppendString(p2,fDupe);
 if (rc == PARR_OK)
   QSort();
 if (pCount)
   *pCount = (N-count);
 return rc;
}

int PtrStrArr::Parse( char *String, char *Delim,unsigned *pCount)
{
 return MyParse(String,Delim,pCount,0);
}

////////////////////////////////////////////////////////

PtrStrArrM::PtrStrArrM( unsigned   Initial_Size
                     ,unsigned   Automatic_Expansion
                     ,comp_fn_tp Compare_Function )
        : PtrStrArr(Initial_Size,Automatic_Expansion,Compare_Function)
{ }


PtrStrArrM::PtrStrArrM(const PtrStrArrM& other)
{
  P= 0;
 *(PtrArray *)this = *(PtrArray *)&other;
}

PtrStrArrM & PtrStrArrM :: operator=(const PtrStrArrM & other)
{
 *(PtrArray *)this = *(PtrArray *)&other;
 return (*this);
}

PtrStrArrM & PtrStrArrM :: operator+=(const PtrStrArrM & other)
{
 RC = AddArray(other);
 return *this;
}

int PtrStrArrM::AppendData(void **Array, unsigned count)
{
 int rc = PARR_OK;
 for (int i = 0 ; rc == PARR_OK && i < count ; i++)
   rc = AppendString((char *)Array[i],1);
 return rc;
}

PtrStrArrM ::~PtrStrArrM()
{ Free(); }

int PtrStrArrM::AddArray(const PtrStrArrM& other)
{
 int rc = AccomodateNPtrs(other.N);
 if (rc == PARR_OK && (rc = AppendData(other.P,other.N)) == PARR_OK)
   QSort();
 return rc;
}

int PtrStrArrM::Add(void * string)
{
 char *temp = strdup((const char *)string);
 return !temp? PARR_NO_MEM : PtrArray::Add(temp);
}

int PtrStrArrM::DeletePtr(void *p)
{
 int index = PtrX(p);
 if (index != PARR_NOT_FOUND )
   { PtrArray::DeleteIndex(index,1);
     return 1;
   }
 return 0;
}

int PtrStrArrM::DeleteIndex(int index)
{ return PtrArray::DeleteIndex(index,1); }

int PtrStrArrM::AddasLast(void * string)
{
 char *temp = strdup((const char *)string);
 return !temp? PARR_NO_MEM : PtrArray::AddasLast(temp);
}

int PtrStrArrM::Insert(void * string,unsigned index)
{
 char *temp = strdup((const char *)string);
 return !temp? PARR_NO_MEM : MyInsert(temp,index,1);
}

int PtrStrArrM::AddasFirst(void * string)
{
 char *temp = strdup((const char *)string);
 return !temp? PARR_NO_MEM : PtrArray::AddasFirst(temp);
}

int PtrStrArrM::Delete(const void *p,comp_fn_tp comp_fn_in)
{
 return PtrArray::Delete(p,comp_fn_in,1);
}

int PtrStrArrM :: LoadFile(char *FileName)
{
 Free();
 return AddFile(FileName);
}

#define PARR_LINE_MAX   257
int PtrStrArrM::AddFile(char *FileName)
{
 FILE *f;
 char *B, *temp;
 unsigned n = 0;
 int rc = PARR_OK;

 if (!(f=fopen(FileName,"rb")))
   return PARR_NO_FILE;
 if (!(B=(char *)malloc(PARR_LINE_MAX)))
   return PARR_NO_MEM;
 while (fgets(B,PARR_LINE_MAX,f))
    n++;
 rc = AccomodateNPtrs(n);
 fseek(f,0,SEEK_SET);
 while ( fgets(B, PARR_LINE_MAX,f) && rc == PARR_OK)
   { B[strlen(B)-1]=0;       // eliminate the line feed included by fgets
     if (!(temp=strdup(B)))
       rc = PARR_NO_MEM;               /* out of memory */
     else P[N++] = temp;
   }
 if (rc == PARR_OK)
    QSort();
 fclose(f);
 free(B);
 return rc;
}

int PtrStrArrM::Replace(void *p, void *pRep,unsigned *pCount,
                        comp_fn_tp comp_fn_in)
{
 return MyReplace(p,pRep,comp_fn_in,pCount,strlen((const char *)pRep) + 1);
}

int PtrStrArrM::RemoveDupes(comp_fn_tp comp_fn_in)
{
 return PtrArray::RemoveDupes(comp_fn_in,1);
}

int PtrStrArrM::Tokenize( char *String, char *Tokenchars, unsigned *pCount)
{
 char *dupe;
 int rc = (dupe = strdup(String)) ? PARR_OK : PARR_NO_MEM;
 if (rc == PARR_OK)
   { rc = PtrStrArr::Tokenize(dupe,Tokenchars,pCount);
     free(dupe);
   }
 return rc;
}


int PtrStrArrM::Parse( char *String, char *Delim,unsigned *pCount)
{
 char *p;
 int rc = (p = strdup(String))? PARR_OK : PARR_NO_MEM; /* make copy */
 if (rc == PARR_OK)
   { rc = MyParse(p,Delim,pCount,1);
     free(p);
   }
 return rc;
}

/***************************************************************************/

////////////////////////
#ifdef UNITTEST

int parrstr_comp(const void *p1,const void * p2)
{
 return strcmp((char *)p1,(char *)p2);
}

int parrstr_comp_desc(const void *p1,const void * p2)
{
 return strcmp((char *)p2,(char *)p1);
}

int  parrstr_walk(void * p)
{
 printf("->%s<-\n",(char *)p);
 return (0);
}

void main(void)
{
 int   rc,i;
 if (1)
   {
     PtrStrArrM ArrM;
     if (rc=ArrM.LoadFile("PARRLOAF.TST"))
        printf("LoadFile trouble - rc : %d\n",rc);
     else { ArrM.CWalk(parrstr_walk);
            ArrM.QSort(parrstr_comp);
            ArrM.CWalk(parrstr_walk);
          }
     ArrM.Free();
     ArrM.Tokenize("This is a little test for finding the tokens"," ");
     ArrM.CWalk(parrstr_walk);
     ArrM.AddasLast("AAAA This should cancel the sort");
     ArrM.CWalk(parrstr_walk);

     ArrM.Replace("AAAA This should cancel the sort","No Replacement");
     ArrM.CWalk(parrstr_walk);

     ArrM.Replace("AAAA This should cancel the sort","Replacement",0,parrstr_comp);
     ArrM.CWalk(parrstr_walk);

     ArrM.Free();

//     ArrM.StopOrdering();
     ArrM.Parse("These,are,placement,parms,to,be,parsed,,including,empty,,placements,,,,,ones",",");
     ArrM.CWalk(parrstr_walk);
     ArrM.Free();
    }
#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif
}

#endif
#if 0
int PtrStrArr::Replace(void *p, void *pRep, unsigned *pCount,
                       comp_fn_tp comp_fn_in)
{
 int index,n = 0;
 char * pTemp;
 int rc = PARR_OK;
 while (rc == PARR_OK && (index = FindX(p,comp_fn_in)) != PARR_NOT_FOUND)
   { if (!comp_fn)    // no order on the elements
       P[index] = pRep;
     else if ((rc=PtrArray::DeleteIndex(index,0)) == PARR_OK)
            rc = Add(pRep);
     if (rc == PARR_OK)
        n++;
   }
 if (pCount) *pCount = n;
 if (rc == PARR_OK && !n)
   return PARR_NOT_FOUND;
 return rc;
}

int PtrStrArrM::Replace(void *p, void *pRep,unsigned *pCount,
                        comp_fn_tp comp_fn_in)
{
 int index,n = 0;
 char * pTemp;
 int rc = PARR_OK;
 while (rc == PARR_OK && (index = FindX(p,comp_fn_in)) != PARR_NOT_FOUND)
   { if (!(pTemp=strdup((char *)pRep)))
       rc = PARR_NO_MEM;               /* out of memory */
     else { free(P[index]);
            if (!comp_fn)    // no order on the elements
              P[index] = pTemp;
            else if ((rc=PtrArray::DeleteIndex(index,0)) == PARR_OK)
                   rc = Add(pRep);
            if (rc == PARR_OK)
               n++;
          }
   }
 if (pCount) *pCount = n;
 if (rc == PARR_OK && !n)
   return PARR_NOT_FOUND;
 return rc;
}
#if 0
int PtrStrArrM::Tokenize( char *String, char *Tokenchars, unsigned *pCount)
{
 char *s1,*s2;                    /* working pointers             */
 unsigned n = 0;
 int rc = (s1 = strdup(String)) ? PARR_OK : PARR_NO_MEM;
 if (rc == PARR_OK)
   { s2 = strtok(s1,Tokenchars);
     while (rc == PARR_OK && s2)
       { if ( (rc = Add(s2)) == PARR_OK)
           n++;
         s2 = strtok(0,Tokenchars);
       }
     free(s1);
   }
 if (pCount) *pCount = n;
 return rc;
}
#endif

int PtrStrArrM::Tokenize( char *String, char *Tokenchars, unsigned *pCount)
{
 char *s1,*s2,s3;                    /* working pointers             */
 unsigned n = 0;
 int rc = (s1 = strdup(String)) ? PARR_OK : PARR_NO_MEM;
 if (rc == PARR_OK)
   {  for (s3 = s1; rc == PARR_OK && (s2 = Mystrtok(&s3,s3,Tokens)))
        if ( (rc = Add(s2)) == PARR_OK)
           n++;
     free(s1);
   }
 if (pCount) *pCount = n;
 return rc;
}


int PtrStrArrM::Parse( char *String, char *Delim,unsigned *pCount)
{
 char *p1,*p2,*pS;                    /* working pointers             */
 unsigned n = 0;
 int rc = (p1 = p2 = pS = strdup(String))? PARR_OK : PARR_NO_MEM; /* make copy */
 if (rc == PARR_OK)
   { for ( ; p1 ; n++)                   /* count the number of elements */
       if (p1=strpbrk(p1,Delim))
         p1++;
     rc = AccomodateNPtrs(n);
     for (n = 0; rc == PARR_OK && (p1 = strpbrk(p2,Delim));)
       { *p1 = 0;                     /* make delimeter a null terminator    */
         if (P[N++] = strdup(p2))      /* put copy address in the array  */
           n++;
         else rc == PARR_NO_MEM;
         p2 = p1+1;
       }
     if (rc == PARR_OK)
       if (P[N++] = strdup(p2))      /* do last token  */
         { n++;
           QSort();
         }
       else rc == PARR_NO_MEM;
     free(pS);
   }
 if (pCount) *pCount = n;
 return rc;
}

#endif
