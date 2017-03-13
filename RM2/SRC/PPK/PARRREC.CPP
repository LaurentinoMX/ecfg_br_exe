# include "parr.hpp"

/////////////////////////////////////////////////////

PtrRecArr::PtrRecArr( unsigned    reclen
                     ,unsigned    Initial_Size
                     ,unsigned    Automatic_Expansion
                     ,comp_fn_tp  Compare_Function )
        : PtrArray(Initial_Size,Automatic_Expansion,Compare_Function)
        , lrecl(reclen)
{ }

PtrRecArr::PtrRecArr(const PtrRecArr& other)
{
  P= 0;
  Clear();
  lrecl = other.lrecl;
 *(PtrArray *)this = *(PtrArray *)&other;
}

PtrRecArr & PtrRecArr :: operator=(const PtrRecArr & other)
{
  lrecl = other.lrecl;
 *(PtrArray *)this = *(PtrArray *)&other;
 return (*this);
}

PtrRecArr & PtrRecArr :: operator+=(const PtrRecArr & other)
{
 *(PtrArray *)this += *(PtrArray *)&other;
 return (*this);
}

int PtrRecArr::AddArray(const PtrRecArr& other)
{
 if (lrecl != other.lrecl)
   return PARR_INCOMPATIBLE;
 return PtrArray::AddArray(*(PtrArray *)&other);
}
#if 0
int PtrRecArr::WriteToFile(FILE *f)
{
 for (int  i = 0 , iW = 1, len = LRecl() ; 1 == iW && i < N ; i++)
   iW = fwrite(P[i],len,1,f);
 return ( 1 == iW );
}

int PtrRecArr::WriteToFile(FILE *f)
{
 for (int  i = 0 ; i < N ; i++)
   fwrite(P[i],lrecl,1,f);
 return 1;
}
#endif

int PtrRecArr::WriteToFile(FILE *f)
{
 for (int  i = 0 , iW = 1 ; (1 == iW) && (i < N) ; i++)
   iW = fwrite(P[i],lrecl,1,f);
 return ( 1 == iW );
}

int PtrRecArr::WriteToFile(char *filename)
{
 FILE *f;
 if (!(f=fopen(filename,"wb")))
   return PARR_NO_FILE;
 for (int  i = 0 ; i < N ; i++)
   fwrite(P[i],lrecl,1,f);
 fclose(f);
 return PARR_OK;
}

int PtrRecArr::ReadFileM(FILE * f, unsigned count)
{
 void *pv;
 for (int i = 0, rc = PARR_OK ; rc == PARR_OK && i < count; i++)
   if (pv = malloc(lrecl))
     { if (fread(pv,lrecl,1,f) != 1)
          rc = PARR_READ;
        else Add(pv);
      }
    else rc = PARR_NO_MEM;
 return rc;
}

int PtrRecArr::ReadFileM(char *filename)
{
 FILE *f;
 if (!(f = fopen(filename,"rb")))
   return PARR_NO_FILE;
 int i = _filelength(_fileno(f));
 int rc = !(i%lrecl)?  ReadFileM(f, i/lrecl) : PARR_LRECL;
 fclose(f);
 return rc;
}

int PtrRecArr::Replace(void *p, void *pRep, unsigned *pCount,
                       comp_fn_tp comp_fn_in)
{
 return MyReplace(p,pRep,comp_fn_in,pCount);
}

int PtrRecArr::RemoveDupes(comp_fn_tp comp_fn_in)
{
 return PtrArray::RemoveDupes(comp_fn_in,0);
}

int PtrRecArr::Point( void *Storage, unsigned count)
{
 return PtrArray::Point(Storage,lrecl,count);
}

int PtrRecArr::Delete(void *p,comp_fn_tp comp_fn_in)
{
 return PtrArray::Delete(p,comp_fn_in,0);
}

int PtrRecArr::DeletePtr(void *p)
{
 return PtrArray::DeletePtr(p,0);
}

int PtrRecArr::DeleteIndex(int index)
{
 return PtrArray::DeleteIndex(index,0);
}


////////////////////////////////////////////

PtrRecArrM::PtrRecArrM( unsigned   reclen
                       ,unsigned   Initial_Size
                       ,unsigned   Automatic_Expansion
                       ,comp_fn_tp Compare_Function )
        : PtrRecArr(reclen,Initial_Size,Automatic_Expansion,Compare_Function)
{}

PtrRecArrM::PtrRecArrM(const PtrRecArrM& other)
{
  P= 0;
  Clear();
 *(PtrRecArr *)this = *(PtrRecArr *)&other;
}

PtrRecArrM & PtrRecArrM :: operator=(const PtrRecArrM & other)
{
 if (this != &other)
   { Free();
     *(PtrArray *)this = *(PtrArray *)&other;
   }
 return (*this);
}

int PtrRecArrM::AppendData(void **Array, unsigned count)
{
 for (int i = 0 ; i < count ; i++)
   if (P[N] = malloc(LRecl()))
     memcpy(P[N++],Array[i],LRecl());
   else return PARR_NO_MEM;
 return PARR_OK;
}

PtrRecArrM ::~PtrRecArrM()
{
 Free();
}

int PtrRecArrM::AddArray(const PtrRecArrM& other)
{
 if (LRecl() != other.LRecl())
   return PARR_INCOMPATIBLE;
 int rc = AccomodateNPtrs(other.N);
 if (rc == PARR_OK && (rc = AppendData(other.P,other.N)) == PARR_OK)
   QSort();
 return rc;
}

int PtrRecArrM::Add(void * p)
{
 void * pTemp = malloc(LRecl());
 if (pTemp)
   memcpy(pTemp,p,LRecl());
 return !pTemp? PARR_NO_MEM : PtrArray::Add(pTemp);
}

int PtrRecArrM :: LoadFile(char *FileName)
{
 Free();
 return AddFile(FileName);
}

int PtrRecArrM :: LoadFile(FILE *f,unsigned count)
{
 Free();
 return AddFromFile(f,count);
}

int PtrRecArrM::AddFile(char *filename)
{
 FILE *f;
 unsigned i;
 int rc = PARR_OK;

 if (!(f = fopen(filename,"rb")))
   return PARR_NO_FILE;
 i = _filelength(_fileno(f));
 if (!(i%LRecl()))
   rc = AddFromFile(f,i/LRecl());
 else rc = PARR_LRECL;  /*lrecl does not divide evenly into filesize*/
 fclose(f);
 return rc;
}

int PtrRecArrM::AddFromFile(FILE *f,unsigned count)
{
 int i, rc = AccomodateNPtrs(count);
 for (i = 0; rc == PARR_OK && i < count; i++)
   if (P[N] = malloc(LRecl()))
     { if (fread(P[N++],LRecl(),1,f) != 1)
         rc = PARR_READ;
     }
   else rc = PARR_NO_MEM;               /* out of memory */
 if (rc == PARR_OK)
   QSort();                     /* fnctn will sort when required */
 return rc;
}

int PtrRecArrM::Replace(void *p, void *pRep, unsigned *pCount,
                        comp_fn_tp comp_fn_in)
{
 return MyReplace(p,pRep,comp_fn_in,pCount,LRecl());
}

int PtrRecArrM::RemoveDupes(comp_fn_tp comp_fn_in)
{
 return PtrArray::RemoveDupes(comp_fn_in,1);
}

int PtrRecArrM::Point( void *Storage, unsigned count)
{
 Free();
 int rc = AccomodateNPtrs(count),len = LRecl();
 for (int i = 0; rc == PARR_OK && i < count; i++)
    rc = this->Add((char *)Storage + (i*len));
 return rc;
}

int PtrRecArrM::Delete(void *p,comp_fn_tp comp_fn_in)
{
 return PtrArray::Delete(p,comp_fn_in,1);
}

int PtrRecArrM::DeletePtr(void *p)
{
 return PtrArray::DeletePtr(p,1);
}

int PtrRecArrM::DeleteIndex(int index)
{
 return PtrArray::DeleteIndex(index,1);
}



/***************************************************************************/

////////////////////////
#ifdef UNITTEST

 static struct BindFileStruc {
                              char BindFile[25];
                              int  yesnobind;
                              int  bound;
                              } BindFileTable[]={
                                                 {"EFWPMDBI.BND",1,1},
                                                 {"EFWPMDBO.BND",1,2},
                                                 {"EFWVDOWN.BND",0,3},
                                                 {"EFWVDP2.BND", 1,4},
                                                 {"EFWVDREN.BND",1,5},
                                                 {"EFWVDRID.BND",0,6},
                                                 {"EFWVDRST.BND",0,7},
                                                 {"EFWVDSEL.BND",0,8},
                                                 {"EFWVDSLV.BND",1,9},
                                                 {"",            0,0}
                                                };

int parrrec_comp(const void *p1,const void * p2)
{
 return strcmp((char *)p1,(char *)p2);
}

int parrrec_comp_desc(const void *p1,const void * p2)
{
 return strcmp((char *)p2,(char *)p1);
}

int  parrrec_walk(void * p)
{
 printf("->%s<-\n",(char *)p);
 return (0);
}

void main(void)
{
 int   rc,i, nmbr = 30;
 if (1)
   { FILE *f;
     if (f=fopen("ARRLOBF.TST" , "wb"))
       { struct BindFileStruc *pRecl;
         int lrecl=sizeof(*pRecl);
         PtrRecArrM ArrM(lrecl);
         for ( pRecl = BindFileTable; *pRecl->BindFile ; pRecl++)
           fwrite(pRecl,lrecl, 1, f);
         fclose(f);
         if (rc=ArrM.LoadFile("ARRLOBF.TST"))
           printf("LoadFile trouble - rc : %d\n",rc);
         else {
               ArrM.QSort(parrrec_comp_desc);
               rc = ArrM.NumberElements();
               for (i=0;i < rc ;i++)
                 { pRecl = (struct BindFileStruc *)ArrM[i];
                   printf("Filename: %s int1: %d int2 %d\n",
                           pRecl->BindFile, pRecl->yesnobind, pRecl->bound);
                 }
               if (rc=ArrM.WriteToFile("ARRTOBF.TST"))
                 printf("WriteToFile trouble - rc : %d\n",rc);
               else {
                      PtrRecArrM ArrM2(lrecl);
                      if (rc=ArrM2.LoadFile("ARRTOBF.TST"))
                         printf("LoadFile trouble - rc : %d %s(%d)\n",rc,__FILE__,__LINE__);
                      else for (i=0;i < ArrM2.NumberElements() ;i++)
                              { pRecl = (struct BindFileStruc *)ArrM2[i];
                                printf("Filename: %s int1: %d int2 %d\n",
                                        pRecl->BindFile, pRecl->yesnobind, pRecl->bound);
                              }
                      ArrM2.Free();
                    }
              }
         ArrM.Free();
       }
   }
#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif

}

#endif
#if 0
int PtrRecArrM::AddFile(char *filename)
{
 FILE *f;
 unsigned count, i;
 int rc = PARR_OK;

 if (!(f = fopen(filename,"rb")))
   return PARR_NO_FILE;
 i = _filelength(_fileno(f));
 if (!(i%LRecl()))
   { count = i/LRecl();
     rc = AccomodateNPtrs(count);
     for (i = 0; rc == PARR_OK && i < count; i++)
       if (P[N] = malloc(LRecl()))
         { if (fread(P[N++],LRecl(),1,f) != 1)
             rc = PARR_READ;
         }
       else rc = PARR_NO_MEM;               /* out of memory */
     if (rc == PARR_OK)
       QSort();                     /* fnctn will sort when required */
   }
 else rc = PARR_LRECL;  /*lrecl does not divide evenly into filesize*/
 fclose(f);
 return rc;
}
#endif
