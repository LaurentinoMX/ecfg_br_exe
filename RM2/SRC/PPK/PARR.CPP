# include "parr.hpp"

static comp_fn_tp caller_comp_fn = 0;

#ifdef __cplusplus
  extern "C" {
#endif
int parr_comp_fn(const void *ptr1,const void *ptr2)
{
 return caller_comp_fn(*(const void **)ptr1,*(const void **)ptr2);
}
#ifdef __cplusplus
}
#endif

/***************************************************************************/

void ** PtrArray :: GetNPtrs(unsigned u)
{
 void ** pTemp;
 if (pTemp = (void **)(new char[u * sizeof(*pTemp)]))
   memset(pTemp,0,u * sizeof(*pTemp));
 return pTemp;
}

int PtrArray::OrderInsertPos(void *Addptr)
{
 if (comp_fn)
   { int    top = N-1, bot = 0, middle, result = 1;
     while (( top >= bot) && result)
       { middle= (top+bot)/2;
         if (result = comp_fn(P[middle],Addptr))
           if (result > 0)
             top = middle - 1;
         else bot = middle + 1;
       }
     return result < 0? middle + 1 : middle;
   }
 return N;      // else append
}

PtrArray::PtrArray( unsigned   Initial_Size
                   ,unsigned   Automatic_Expansion
                   ,comp_fn_tp Compare_Function)
        : M(Initial_Size)
         ,N(0)
         ,Exp(Automatic_Expansion)
         ,comp_fn(Compare_Function)
{
 RC = (P = GetNPtrs(M))? PARR_OK : PARR_NO_MEM;
}

PtrArray::PtrArray(const PtrArray& other)
        : P(0)
{
 Clear();
 *this=other;
}

PtrArray::~PtrArray()
{
 if (P) delete[] (char *)P;
}

void PtrArray::Clear(void)
{
 if (P)
   { delete[] (char *)P;
     P = 0;
   }
 M=N=0;
 comp_fn = 0;
 RC = PARR_OK;
}

void * PtrArray::operator[](int i)
{
 return this->Element(i);
}

PtrArray & PtrArray :: operator=(const PtrArray & other)
{
 if (this != &other)
   { Clear();
     if (P = GetNPtrs(other.M))
       { M = other.M;
         Exp = other.Exp;
         comp_fn = other.comp_fn;
         RC = AppendData(other.P,other.N);
       }
     else RC = PARR_NO_MEM;
   }
 return (*this);
}

int PtrArray::AppendData(void **Array, unsigned count)
{
 memcpy(&P[N],Array,count * sizeof(*P));
 N += count;
 return PARR_OK;
}

PtrArray & PtrArray :: operator+=(const PtrArray & other)
{
 RC = AddArray(other);
 return *this;
}

int PtrArray::AddArray(const PtrArray& other)
{
 int rc = AccomodateNPtrs(other.N);
 if (rc == PARR_OK)
   { AppendData(other.P,other.N);
     QSort();
   }
 return rc;
}

int PtrArray::Expand(unsigned n)
{
 void **pTemp = GetNPtrs(M+n);
 if (pTemp)
   { if (P)
       { memcpy(pTemp,P,M * sizeof(*P));
         delete (char*)P;
       }
     P = pTemp;
     M += n;
     return PARR_OK;
   }
 return PARR_NO_MEM;
}

int PtrArray::AccomodateNPtrs(unsigned n)
{
 if ( n > (M - N) )                   /* # array elements not yet used */
   { if ( !Exp )                    /* automatic expand number     */
       return PARR_FULL;
     int need = n - (M - N);
     need = ((need + (Exp-1))/Exp) * Exp;
     return Expand(need);
   }
 return PARR_OK;
}

#if 0
int PtrArray::AccomodateNPtrs(unsigned n)
{
 if ( n >= (M - N) )                   /* # array elements not yet used */
   { if ( !Exp )                    /* automatic expand number     */
       return PARR_FULL;
     return Expand(n - (M - N) + 1 );
   }
 return PARR_OK;
}
#endif

int PtrArray:: Add(void *AddPtr)
{
 int rc = AccomodateNPtrs(1);
 if (rc == PARR_OK)
   rc = MyInsert(AddPtr,(!comp_fn || !N)? N : OrderInsertPos(AddPtr));
 return rc;
}


void * PtrArray::Element(unsigned n)const
{
 if (P && n < N)
    return P[n];
 return 0;
}

int PtrArray::CheckOrderIndex(unsigned index)
{
 if (index < N )
    if ( comp_fn &&
        ( (index && comp_fn(P[index-1],P[index]) > 0) ||
          (index < (N-1) && comp_fn(P[index],P[index+1]) < 0 ) ) )
       return PARR_BAD;
    else return PARR_OK;
 return PARR_BAD_INDEX;
}

int PtrArray::Insert(void *p,unsigned index)
{
 return MyInsert(p,index,1);
}

int PtrArray::AddasFirst(void *p)
{
 int rc = AccomodateNPtrs(1);
   if (rc == PARR_OK)
     rc = MyInsert(p,0,1);
 return rc;
}

int PtrArray::AddasLast(void *p)
{
 int rc = AccomodateNPtrs(1);
   if (rc == PARR_OK)
     rc = MyInsert(p,N,1);
 return rc;
}

int PtrArray::MyInsert(void *p,unsigned insert,unsigned fEnforceOrder)
{
 if (insert < M )
   if ( N < M)
      { for (int i = N ; i > insert ; i--) /*memcpy does one at a time */
          P[i] = P[i-1];                /* copying the same again & again */
        P[insert]=p;
        N++;
        if (fEnforceOrder && (CheckOrderIndex(insert) == PARR_BAD) )
          comp_fn = 0;
        return PARR_OK;
      }
   else return PARR_FULL;
 else return PARR_BAD_INDEX;
}

int PtrArray::CWalk(walk_fn_tp fn)
{
 int rc = 0;
 for ( int i=0 ; fn && !rc && i < N  ; i++ )
    rc = fn(P[i]);
 return rc;
}

int PtrArray::VisitAll()
{
 int rc = 0;
 for ( int i=0 ;  i < N  && !rc ; i++ )
    rc = Visit(P[i]);
 return rc;
}

void * PtrArray::Pop(int fFree)
{
 void *p = 0;
 if (N)
   { p = P[0];
     DeleteIndex(0,fFree);
   }
 return p;
}

void * PtrArray::DeQue(int fFree)
{
 void *p = 0;
 if (N)
   { p = P[N-1];
     DeleteIndex(N-1,fFree);
   }
 return p;
}

int PtrArray::DeleteIndex(int index,int fFree)
{
 if ((0 <= index) && (index < N))
   { if (fFree) free(P[index]);
     memcpy(&P[index], &P[index+1],(N-index-1)*sizeof(*P));
     N--;
     return PARR_OK;
   }
 return PARR_BAD_INDEX;
}

int  PtrArray::Delete(const void *p,comp_fn_tp comp_fn_in,int fFree)
{
 int count = 0, index;
 while ((index = FindX(p,comp_fn_in)) != PARR_NOT_FOUND )
   { DeleteIndex(index,fFree);
     count++;
   }
 return count;
}

int PtrArray::PtrX(const void *p)
{
 int i;
 for ( i = 0 ; i < N && p != P[i] ; i++);
 return (i == N)? PARR_NOT_FOUND: i;
}

void * PtrArray::NextPtr(const void *p)
{
 if (N)
   { if (!p)
       return P[0];
     int i;
     for ( i = 0 ; i < (N - 1) && p != P[i] ; i++);
     if ( i < (N - 1))
       return P[++i];
   }
 return 0;
}

int PtrArray::DeletePtr(const void *p,int fFree)
{
 int count = 0, index;
 while ((index = PtrX(p)) != PARR_NOT_FOUND )
   { DeleteIndex(index,fFree);
     if (fFree) fFree = 0;
     count++;
   }
 return count;
}

int PtrArray::RemoveDupes(comp_fn_tp comp_fn_in,int fFree)
{
 int i, j, qty=0;
 if (comp_fn_in && (comp_fn_in != comp_fn))
   { for (i=0 ; i < N ;i++)
       for (j=0 ; j < N ;j++)
         if ((i != j) && !comp_fn_in(P[i],P[j]))
           { DeleteIndex(j,fFree);
             qty++;
           }
   }
 else if (comp_fn)
        for (i=0 ; i < N-1 ;i++)
           while ((i < N-1) && !comp_fn(P[i],P[i+1]) )
             { DeleteIndex(i+1,fFree);
               qty++;
             }
 return(qty);
}

void * PtrArray::LSearch(const void *p,comp_fn_tp comp_fn_in)
{
 int i = LSearchX(p,comp_fn_in);
 return (i < 0)? 0 : P[i];
}

int PtrArray::LSearchX(const void *p,comp_fn_tp comp_fn_in)const
{
 int i,not_found = 1;
 for (i = 0; i < N && not_found ;i++ )
    not_found = comp_fn_in(p,P[i]);
 return not_found? -1 : i-1;
}

void * PtrArray::BSearch(const void *p)
{
 int i = BSearchX(p);
 return (i < 0)? 0 : P[i];
}

int PtrArray::BSearchX(const void *p)const
{
// return bsearch(p,P,N,sizeof(P[0]),comp_fn);
 if (comp_fn)
   {
    if ( N == 1)
      return !comp_fn(P[0],p)? 0 : PARR_NOT_FOUND;
    else { int top = N - 1, bot = 0, not_found = 1, middle;
            while ((top >= bot) && not_found)
             { middle = (bot + top) / 2;
               if (!(not_found = comp_fn(P[middle],p)))
                  return middle;
               if (not_found > 0)
                  top = middle - 1;
               else bot = middle + 1;
             }
         }
   }
 return PARR_NOT_FOUND;
}

int PtrArray::FindX(const void *p,comp_fn_tp comp_fn_in)const
{
 if (comp_fn_in && comp_fn_in != comp_fn)
    return LSearchX(p,comp_fn_in);
 return BSearchX(p);
}

void *PtrArray::Find(const void *p,comp_fn_tp comp_fn_in)
{
 if (comp_fn_in && comp_fn_in != comp_fn)
    return LSearch(p,comp_fn_in);
 return BSearch(p);
}

void PtrArray::QSort(comp_fn_tp comp_fn_in)
{
 if ( (!(comp_fn_in)) || (comp_fn_in == comp_fn) )
    return;
 comp_fn = comp_fn_in;
 QSort();
}

void PtrArray::StopOrdering()
{
 comp_fn = 0;
}

void PtrArray::QSort()
{
 if (comp_fn)
   { caller_comp_fn = comp_fn;
     qsort(P,N,sizeof(P[0]),parr_comp_fn);
   }
}

unsigned PtrArray::NumberElements(const void *p,comp_fn_tp comp_fn_in)const
{
 unsigned n = 0;
 if (!p || (!comp_fn && !comp_fn_in))
   return N;
 while (FindX(p,comp_fn_in) != PARR_NOT_FOUND)
    n++;
 return n;
}

int  PtrArray::Point( void *Storage, unsigned lrecl, unsigned count)
{
 int rc = AccomodateNPtrs(count);
 if (rc == PARR_OK)
   { for (int i = 0; i < count; i++)
       P[N++] = (char *)Storage + (i*lrecl);
     QSort();
   }
 return rc;
}

void PtrArray::Free(void)
{
 for (int i=0; i < N;i++ )
   free (P[i]);
 N = 0;
}

//  I'm only writing replacement for one since multiples is too messy
//  with the freeing business and multiple replacement with the same pointer
// It's better to have it done one at a time when different replacement
// pointers can be given.
int PtrArray::Replace1(void *p, void *pRep,int fFree, comp_fn_tp comp_fn_in)
{
 int index = FindX(p,comp_fn_in);
 if (index != PARR_NOT_FOUND)
   {  if (fFree) free(P[index]);
      if (!comp_fn)    // no order on the elements
        P[index] = pRep;
      else { DeleteIndex(index,0);      // don't free
             Add(pRep);
           }
      return PARR_OK;
   }
 return PARR_NOT_FOUND;
}

int PtrArray::MyReplace( void *p, void *pRep, comp_fn_tp comp_fn_in,
                         unsigned *pCount, unsigned lrecl)
{
 int index,n = 0;
 void * pTemp;
 int rc = PARR_OK;
 while (rc == PARR_OK && (index = FindX(p,comp_fn_in)) != PARR_NOT_FOUND)
   { if (lrecl)            // copies
        { if (!(pTemp=malloc(lrecl)))
            rc = PARR_NO_MEM;               /* out of memory */
          else { free(P[index]);
                 memcpy(pTemp,pRep,lrecl);
                 pRep = pTemp;
               }
        }
     if ( rc == PARR_OK)
        { if (!comp_fn)    // no order on the elements
             P[index] = pRep;
          else if ((rc=PtrArray::DeleteIndex(index,0)) == PARR_OK)
                  rc = Add(pRep);
          if (rc == PARR_OK)
             n++;
          else if (lrecl)
                  free(pTemp);
        }
   }
 if (pCount) *pCount = n;
 if (rc == PARR_OK && !n)
   return PARR_NOT_FOUND;
 return rc;
}

/***************************************************************************/

////////////////////////
#ifdef UNITTEST

int parr_comp(const void *p1,const void * p2)
{
 return strcmp((char *)p1,(char *)p2);
}

int parr_comp_desc(const void *p1,const void * p2)
{
 return strcmp((char *)p2,(char *)p1);
}

int  parr_walk(void * p)
{
 printf("->%s<-\n",(char *)p);
 return (0);
}

void main(void)
{
 int   rc,i, nmbr = 30;
 char s1[25],
      *s2;
 if (1)
   {
      PtrArray Arr(10,3,parr_comp);

      for (rc=0;rc <= nmbr ;rc++)
          {
           s2 = (char *)malloc(25);
           sprintf(s2,"string **%03d**  ",rc);
           printf("Add's rc for call %d : %d \n",rc,Arr.Add(s2));
          }
      for (rc=0;rc <= nmbr ;rc++)
     //  printf("Array data %s\n",Arr.Element(rc));
         printf("Array data %s\n",Arr[rc]);

      Arr.CWalk(parr_walk);
      if (s2 = (char *)Arr.LSearch("string **123**  ",parr_comp))
         printf("LSearch Found this string %s\n",s2);

      if (s2 = (char *)Arr.LSearch("string **002**  ",parr_comp))
         printf("LSearch Found this string %s\n",s2);

      if (s2 = (char *)Arr.Find("string **002**  "))
         printf("Find found this string %s\n",s2);

      Arr.QSort(parr_comp_desc);
      Arr.CWalk(parr_walk);

      if (s2 = (char *)Arr.Find("string **002**  "))
         printf("Find found this string %s\n",s2);

      rc = Arr.Delete("string **002**  ",0,1);
      printf("Deleted %d times for 'string **002**  '\n",rc);

      rc = Arr.Delete("string **222**  ");
      printf("Deleted %d times for 'string **222**  '\n",rc);

      PtrArray Arr2(Arr);
      Arr2.CWalk(parr_walk);

      for (rc=0;rc <= 5 ;rc++)
          {
           s2 = (char *)malloc(25);
           strcpy(s2,"string **025**  ");
           Arr2.Add(s2);
          }
      Arr2.CWalk(parr_walk);
      rc = Arr2.RemoveDupes(0,1);
      printf("Deleted %d Dupes for 'string **025**  '\n",rc);
      Arr2.CWalk(parr_walk);

      for (char *pch = 0; pch = (char *)Arr2.NextPtr(pch);)
        printf("NexPtr:->%s<-\n",pch);

     // Arr.Free();
      Arr2.Free();
  }

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif

}

#endif



//////////////////////////////////////////////////
///////////////////////////////////////////////////
#if 0
int PtrArray::LoadAF(char *FileName)
{
 FILE *File;
 char s[PARR_LINE_MAX];
 char *temp;
 comp_fn_tp temp2;
 unsigned linecount;

 if (!(linecount = FileLines(FileName,PARR_LINE_MAX)))
   return PARR_NO_FILE;    /* could not obtain number of strings in file */

 if ( linecount > (M - N) )      /* # array elements not yet used */
   { if ( !Exp )                    /* automatic expand number     */
       return PARR_FULL;
     if (Expand(linecount - (M - N)))
       return PARR_NO_MEM;               /* out of memory */
   }
 if (!(File=fopen(FileName,"r")))
   return PARR_NO_FILE;
 while ( fgets(s, sizeof(s),File))
     { s[strlen(s)-1]=0;       // eliminate the line feed included by fgets
       if (!(temp=strdup(s)))
         return PARR_NO_MEM;               /* out of memory */
       P[N++] = temp;
     }
 if (comp_fn)
     { temp2 = comp_fn;
       comp_fn = 0; /* function won't sort if comp functions are same*/
       QSort(temp2);
     }
 fclose(File);
 return PARR_OK;
}
// This version allocates one contiguous piece of storage giving
// problems with subsequent 'free' addition methods.
int PtrArray::LoadBF(char *filename, unsigned LRecL)
{
 FILE *f;
 unsigned count, size, i;
 int rc = PARR_OK;
 void *Buffer;

 if (!(f = fopen(filename,"rb")))
   return PARR_NO_FILE;
 size = _filelength(_fileno(f));
 if (!(size%LRecL))
   { count = size/LRecL;
     if ( count > (M - N) )                 /* # array elements not yet used */
       { if ( !Exp )                         /* automatic expand number     */
           rc = PARR_FULL;
         if (Expand(count - (M - N)))
           rc = PARR_NO_MEM;               /* out of memory */
       }
     if (rc == PARR_OK)
       if (!(Buffer = calloc(LRecL,count)))
         rc = PARR_NO_MEM;               /* out of memory */
       else if (fread(Buffer,LRecL,count,f) != count)
               rc = PARR_READ;
   }
 else rc = PARR_LRECL;  /*lrecl does not divide evenly into filesize*/
 if (rc == PARR_OK)
   { for (i = N; i < N + count; i++)
       P[i]=&((char *)Buffer)[LRecL*(i-N)];
     N += count;
     QSort(comp_fn);                 /* fnctn will sort when required */
   }                                        /* Close the file, and return */
 fclose(f);
 return rc;
}

////////////////////////
/////// My own qsort which is giving me problems by eating up the stack
#define MAX_INSERT 50
void PtrArray::QSort(int start,int stop)
{
 if (-1 == stop || N == stop)   // default
    stop = N-1; // ensure stop at last index in the array

 if (MAX_INSERT < stop-start)
   {
    // break up array section into 2 completely
    // sortable partitions
    int partIdx = Partition( start, stop);

    // sort lower partition
    if (MAX_INSERT > partIdx - start) // if lower partition has less than 20 entries
       InsertSort(start,partIdx);
    else QSort(start,partIdx);

    // sort upper partition
    if (MAX_INSERT > (stop - partIdx+1))// if upper partition has less than 20 entries
       InsertSort(partIdx + 1,stop);
    else QSort(partIdx + 1,stop);

   }
 else if (start < stop) // else if array still has more than one element
        InsertSort( start, stop);
 return;
}

void PtrArray::Swap(int i, int j)
{
 void * pTemp = P[i];
 P[i] = P[j];
 P[j] = pTemp;
}

int  PtrArray::Partition(int start,int stop)
{
 int i,j, median;
 void * pivot;

 median = ((stop - start + 1)/2) + start; // calculate the middle element

 if (comp_fn(P[start],P[stop]) > 0)
   Swap(start,stop);
 if (comp_fn(P[median],P[stop]) > 0)
   Swap(median,stop);
 else if (comp_fn(P[start],P[median]) > 0)
        Swap(start,median);
 pivot = P[median]; // set pivot to the value in the middle (median)

 i = start - 1;
 j = stop + 1;

 for (;;)
   {  // infinite loop

    // advance backward from end of the array until either
    // the front of the array is passed or an element
    // is found that is less than or equal to the pivot
    do { j = j - 1; } while (j >= start && (comp_fn(P[j],pivot) > 0));

    // advance forward from beginning of the array until either
    // the end of the array is passed or an element
    // is found that is greater than the pivot
    do { i = i + 1; } while (i <= stop  && (comp_fn(P[i],pivot) <= 0));

    if (i<j) // if i stopped earlier in the array than where j stopped
      Swap(i,j);
    else if (j < start) // if j passed the front of the array
            return start;
         else return j;

   }
 return (-1); // garbage, should never be reached
}


void PtrArray::InsertSort(int start, int stop)
{
 int i,j;
 void *pTemp;
 if (-1 == stop || N == stop)
   stop = N-1;                    // ensure stop at last index in the array
 for (j = start + 1; j <= stop; j++)  // from the second to last elements
   { pTemp = P[j];
     i = j-1;                       // start i at element just before j
     while (i >= start && (comp_fn(P[i],pTemp) > 0))
       { P[i+1] = P[i];
         i--;
       }
     P[i+1] = pTemp;    // insert element into position of the last
                       // element moved up by one
   }
}
/////// END My own qsort which is giving me problems by eating up the stack
////////////////////////


#if 0
int PtrArray:: Add(void *AddPtr)
{
 int rc = AccomodateNPtrs(1);
// int rc = ((M - 1) == N)? ( (Exp)? Expand(Exp): PARR_FULL ) : PARR_OK;
#if 0
 int rc = PARR_OK;
 if ((M-1) == N)
   if (Exp)
      rc = Expand(Exp);
   else rc = PARR_FULL;
#endif
 if (rc == PARR_OK)
   { if (!comp_fn || !N)
       P[N]=AddPtr;
     else { int insertpstn = OrderInsertPos(AddPtr);
            for (int i = N; i > insertpstn ; i--) /*memcpy does one at a time */
              P[i] = P[i-1];                /* copying the same again & again */
            P[insertpstn]=AddPtr;
          }
     N++;
   }
 return rc;
}
#endif


#endif
