 /*--------------------------------------------------------------------------*\
 |    Filename - FEATL.CPP                                                    |
 |      contains source for the following RM/2 class implementations:         |
 |                                                                            |
 |     Feat     - Feature object wrapper, merely containing the pointers      |
 |                to the Machine and MFeat objects.                           |
 |                Methods to manipulate the underlying CFReport machine       |
 |                and feature objects, do so directly. There is no copy.      |
 |                Intended for EUI for which also can use the ID data         |
 |                (easier when working with containers)                       |
 |                The storage numbers int the object are used to establish    |
 |                a connection with the Machine and MFeat object during       |
 |                file IO.                                                    |
 |                Object declared in FEAT.HPP.                                |
 |                                                                            |
 |     FeatList - A collection of 'Feat' objects                              |
 |  ZOR march-2004 F86752 add tag Resolve_IS in the status                    |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |              Standard Library Includes              |
        \*---------------------------------------------------*/
#include <io.h>
#include <stdio.h>
#include <limits.h>

        /*---------------------------------------------------*\
        |                   ICLUI Includes                    |
        \*---------------------------------------------------*/
#include <isynonym.hpp>                          // Definition of Boolean etc


        /*---------------------------------------------------*\
        |               RM/2-specific includes                |
        \*---------------------------------------------------*/
#include "featl.hpp"                             // this file's declarations

 /*--------------------------------------------------------------------------*\
 |                                 Feat Class                                 |
 \*--------------------------------------------------------------------------*/
        /*---------------------------------------------------*\
        |                     Assignment                      |
        \*---------------------------------------------------*/
Feat & Feat:: operator=(Feat const& other)
{
 if (&other != this)
   memcpy(this,&other,sizeof(*this));
 return *this;
}

    /*-----------------------------------------------------------------------*\
    |    int compare(Feat const& other) const                                 |
    |       Ranks the features of System's first machine in the MRDB file     |
    |       alphabetically before those of subsequent machines.               |
    \*-----------------------------------------------------------------------*/
int Feat :: compare(Feat const& other) const
{
// return (pM->isFirstMRDB()?
//              (other.pM->isFirstMRDB()? compareMachFeat(other) : -1)
//            : (other.pM->isFirstMRDB()? 1 : compareMachFeat(other)));
  int i = pM->compareBySubsystem(*(other.pM));
  return i? i : compareMachFeat(other);
}

    /*-----------------------------------------------------------------------*\
    |    int compareMachFeat(Feat const& other) const                         |
    |       Alphabetically compares the object's machine before its feature.  |
    \*-----------------------------------------------------------------------*/
int Feat :: compareMachFeat(Feat const& other) const
{
 int i = pM->compare(*(other.pM));
 if (!i && pF)
   return pF->compare(*(other.pF));
 return i;
}

    /*-----------------------------------------------------------------------*\
    |    Feat & setStatus(Status s)                                           |
    |       Set the status by converting it to the MFeat's status and         |
    |       storing the information in that object.                           |
    \*-----------------------------------------------------------------------*/
Feat & Feat :: setStatus(Status s)
{
 if (pF)
   switch(s) {
     case Resolved:      pF->setTag(MFeat::Resolved); break;
     case Resolved_EUOP: pF->setTag(MFeat::Resolved_EUOP);break;
     case Resolved_User: pF->setTag(MFeat::Resolved_User);break;

     case ISeriesSWTblResolved: pF->setTag(MFeat::ISeriesSWTblResolved);break;   /* F72273 HECC: 07/03/2002 */
     case Resolved_IS:   pF->setTag(MFeat::Resolved_IS);break;  //ZOR F86752 1373

     case Unresolved:    pF->setTag(MFeat::Unresolved);
     }
 return *this;
}

    /*-----------------------------------------------------------------------*\
    |    Status status()                                                      |
    |       Return the status by translating the MFeat's status.              |
    |       For featureless machines determine the status here                |
    |       since the Machine object has no status:                           |
    |         Featureless Machines are considered automatically               |
    |         resolved (in System's 'resolveLoneMachines()' method).          |
    |         We're merely checking the presence of a CFReport                |
    |         Machine Record (since that's the one to be written out          |
    |         to the 'resolved' CFReport) as an additional safeguard.         |
    \*-----------------------------------------------------------------------*/
Feat::Status Feat :: status()
{
 if (pF)
   switch(pF->tag()) {
     case MFeat::Resolved     : return Resolved;
     case MFeat::Resolved_EUOP: return Resolved_EUOP;

     case MFeat::ISeriesSWTblResolved: return ISeriesSWTblResolved;   /* F72273 HECC: 07/03/2002 */
     case MFeat::Resolved_IS  : return Resolved_IS;  //ZOR F86752 1373

     case MFeat::Resolved_User: return Resolved_User;
     default                  : return Unresolved;
     }
 else return ( (pM->hasAASData() || pM->hasMRDBData())?
                Resolved_EUOP : Unresolved);
}

    /*-----------------------------------------------------------------------*\
    |    Boolean machineDescription(IString & str)                            |
    |       Set the machine's description in the given string                 |
    |       and indicate success                                              |
    \*-----------------------------------------------------------------------*/
Boolean Feat :: machineDescription(IString & str)
{
 if (pM) {
   str = pM->description();
   return true;
   }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    void setStorageNumbers()                                             |
    |       Get and store the Machine and MFeat's storage numbers             |
    |       This connection is made before the object is saved to a file.     |
    |       The machine's number became necessary upon the introduction       |
    |       of featureless machines.                                          |
    \*-----------------------------------------------------------------------*/
void Feat :: setStorageNumbers()
{
 Nmbr = pF? pF->number(): 0;
 if (pM)
   MachNmbr = pM->number();
}

    /*-----------------------------------------------------------------------*\
    |    Boolean description(IString & str,Boolean fMRDB = false)             |
    |       Set the AAS or MRDB description in the given string               |
    |       and indicate success                                              |
    \*-----------------------------------------------------------------------*/
Boolean Feat :: description(IString & str,Boolean fMRDB)
{
 if (pF)
   return pF->description(str,fMRDB);
 else str = IString();
 return false;
}

ostream & Feat :: out(ostream& os)
{
 IString str;
 description(str);
  os  << "Machine "
      << " -> Type: '" << machine()
      << "' Model: '" << model()
      << "' SerialNo : '" << serialNo()
      << "' Feature : '"  << feature()
      << "' installDateYMD : '" << installDateYMD()
      << "' cradDateYMD : '"  << cradDateYMD()
      << "' isGeneric : " << (isGeneric()? "Y" : "N")
      << " description : '"  << str
      << "'";
  return os;
}

 /*--------------------------------------------------------------------------*\
 |                               FeatList Class                               |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |        FeatList's inlined 'C' sort function         |
        \*---------------------------------------------------*/
inline long FeatCompareUp(const Feat& f1,const Feat& f2)
{ return f1.compare(f2);}


void FeatList :: SortUp()
{ this->sort(FeatCompareUp); }

    /*-----------------------------------------------------------------------*\
    |    void FeatList :: SetStorageNumbers()                                 |
    |       Have all Feat in the list set their storage numbers.              |
    \*-----------------------------------------------------------------------*/
void FeatList :: SetStorageNumbers()
{
 Cursor crsr(*this);
 forCursor(crsr)
   elementAt(crsr).setStorageNumbers();
}

    /*-----------------------------------------------------------------------*\
    |    Boolean removeViaID(int id)                                          |
    |       Remove the elements with the given ID from the array.             |
    \*-----------------------------------------------------------------------*/
Boolean FeatList :: removeViaID(int id)
{
 Cursor crsr(*this);
 forCursor(crsr)
   if (elementAt(crsr).id() == id) {
     removeAt(crsr);
     return true;
     }
 return false;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean removeViaID(int id)                                          |
    |       Return the greatest ID from the elements in the list.             |
    |       This is used to determine the ID of a new element to              |
    |       be added to the list.                                             |
    \*-----------------------------------------------------------------------*/
int FeatList :: greatestID()
{
 int greatest = INT_MIN;
 Cursor crsr(*this);
 forCursor(crsr)
   if (elementAt(crsr).id() > greatest)
     greatest = elementAt(crsr).id();
 return greatest;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean writeToFile(FILE *f, unsigned * pBytes)                      |
    |       Write the list to the current position of the given               |
    |       binary file, set the number of bytes written and                  |
    |       indicate success.                                                 |
    \*-----------------------------------------------------------------------*/
Boolean FeatList :: writeToFile(FILE *f, unsigned * pBytes)
{
 unsigned len = sizeof(Feat), lenT = 0;
 Boolean fOK = true;
 Cursor crsr(*this);
 for (crsr.setToFirst(); fOK && crsr.isValid();crsr.setToNext())
   if ( fOK = (fwrite(&elementAt(crsr),len,1,f) == 1) )
     lenT += len;
 if (pBytes)
   *pBytes = lenT;
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean writeToFile(FILE *f, unsigned * pBytes)                      |
    |       Write the list to the given file name, set the number             |
    |       of bytes written and indicate success.                            |
    \*-----------------------------------------------------------------------*/
Boolean FeatList :: writeToFile(char *filename, unsigned * pBytes)
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

    /*-----------------------------------------------------------------------*\
    |    Boolean loadFile()                                                   |
    |       Add the given number of Feat objects form the current position    |
    |       of the given file to the given storage allocating array.          |
    |       Set the actual number of objects read in indicate success.        |
    \*-----------------------------------------------------------------------*/
Boolean FeatList::loadFile(FILE *f , FeatListMem & FLM, unsigned count,
                           unsigned * pRead)
{
 int i;
 char Buff[sizeof(Feat)+1];
 Boolean fOK = true;
 for (i = 0; fOK && i < count; i++)
   if (fOK = (fread(Buff,sizeof(Feat),1,f) == 1))
     FLM.Add(Buff);
 if (pRead)
   *pRead = i;
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean loadFile()                                                   |
    |       Add the given number of Feat objects form the current position    |
    |       of the given file to the collection.                              |
    |       Set the actual number of objects read in indicate success.        |
    \*-----------------------------------------------------------------------*/
Boolean FeatList :: loadFile(FILE *f , unsigned count, unsigned * pRead)
{
 int i;
 char Buff[sizeof(Feat)+1];
 Boolean fOK = true;
 for (i = 0; fOK && i < count; i++)
   if (fOK = (fread(Buff,sizeof(Feat),1,f) == 1))
     this->add(*(Feat *)Buff);
 if (pRead)
   *pRead = i;
 return fOK;
}


    /*-----------------------------------------------------------------------*\
    |    Boolean loadFile()                                                   |
    |       Add the Feat objects contained in the given filename to the       |
    |       collection  and indicate success.                                 |
    \*-----------------------------------------------------------------------*/
Boolean FeatList :: loadFile(char * szFileName)
{
 FILE *f;
 unsigned i;
 Boolean fOK = false;
 if ( f = fopen(szFileName,"rb") ) {
   i = _filelength(_fileno(f));
   if (!( i % sizeof(Feat)))
     fOK = loadFile(f,i/sizeof(Feat));
   fclose(f);
   }
 return fOK;
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

