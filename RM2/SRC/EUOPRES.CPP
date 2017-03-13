 /*--------------------------------------------------------------------------*\
 |    Filename - EUOPRES.CPP                                                  |
 |      contains source for the following RM/2 class implementations:         |
 |                                                                            |
 |   EUOPResolve - allows for the retention of the system name                |
 |                 to be Reconciled by the application with                   |
 |                 methods to                                                 |
 |                      Open                                                  |
 |                      Resolve                                               |
 |                      Save                                                  |
 |                      Write                                                 |
 |                    the System on which it is based.                        |
 \*--------------------------------------------------------------------------*/

        /*---------------------------------------------------*\
        |                  ICLUI's Includes                   |
        \*---------------------------------------------------*/
#include <isynonym.hpp>                          // Definition of Boolean etc

        /*---------------------------------------------------*\
        |               RM/2 specific Includes                |
        \*---------------------------------------------------*/
#include "euopres.hpp"                           // this file's declarations
#include "euoutil.hpp"                           // utilities
#include "euomsg.h"                              // Message String Ids
#include "mainw.h"                               // mainw window's Ids

        /*---------------------------------------------------*\
        |                   System Includes                   |
        \*---------------------------------------------------*/
#define INCL_DOSPROCESS                          // dossleep
#define INCL_WIN
#include <os2.h>

#ifndef UNITTEST

#include "mainw.hpp"                             // application's mainw window



    /*-----------------------------------------------------------------------*\
    |    Boolean resolve()                                                    |
    |       Resolve the System indicated by its given AAS File,               |
    |       MRDB File and System-Specific Description File and                |
    |       indicate (non)success.                                            |
    |       (The additional Message String File allows for                    |
    |        System-Specific Messages or those for another language).         |
    |                                                                         |
    |       Retain the System's name as the MRDB filename.                    |
    |       Have the Sytem re-set itself for a new reconciliation             |
    |       job and have the application's main window start and show its     |
    |         timer with a message stating that the AAS                       |
    |         File is being read.                                             |
    |       If the AAS file is successfully read:                             |
    |          Have the main window show the file's particulars and           |
    |             alter its message to reflect that the System's              |
    |             Product Knowledge is being established.                     |
    |          If the Product Knowledge is successfully obtained:             |
    |             Have the main window alter its message to say               |
    |                that the MRDB file is being read.                        |
    |             If the MRDB file is successfully read and                   |
    |              the user did not cancel on account of the MRDB             |
    |              being old (about which the System object communicates      |
    |              directly with the user via the mainw window):              |
    |                Have the main window show the file's particulars         |
    |                  and alter its message to say that the System           |
    |                  is being resolved.                                     |
    |                Apply the Product Knowledge to the System.               |
    |                If the user did not cancel on account of                 |
    |                  the presence of identical machines having              |
    |                  different model numbers ( about which the              |
    |                  System object communicates directly with               |
    |                  the user via the mainw window):                        |
    |                    Reconcile the system by resolving the follwoing      |
    |                       type of machines or features in order:            |
    |                                                                         |
    |                      Combos : Within a machine, AAS features            |
    |                               for which there is an equivalent          |
    |                               combination of features in the            |
    |                               MRDB file.                                |
    |                                                                         |
    |                      Generics: Generic features in the MRDB file        |
    |                                for which there is an equivalent         |
    |                                specific feature or machine in the       |
    |                                AAS file.                                |
    |                                                                         |
    |                      Matches : Features that appear in both files.      |
    |                                                                         |
    |                      Featureless Machines:                              |
    |                                Those appearing in only the AAS or       |
    |                                MRDB file (assumed to be resolved)       |
    |                                                                         |
    |                      NonEquals : Features appearing in both file        |
    |                                  but with different quantities.         |
    |                                                                         |
    |                      Others    : Currently handling the following:      |
    |                                     machines from the AAS CFReport      |
    |                                     to be passed on to the new Report.  |
    |                                                                         |
    |                    Float the first-read MRDB machine to the top         |
    |                      of the alphabetically sorted System Lists.         |
    |                    Generate the Feature and Backlog Collections         |
    |                      for the EUI.                                       |
    |                    Sort the RPO Collection generated during             |
    |                      the reconciliation.                                |
    |                    Save the Product Knowledge in its compiled           |
    |                      version to a file if such a file was not           |
    |                      present when the Product Knowledge was obtained.   |
    |                    Consider the resolution only succesfull if           |
    |                      this point in the logic is reached.                |
    |       Have the main window eliminate its timer and message.             |
    |       Return indicating success/failure.                                |
    |                                                                         |
    |       Note: If the method fails, it will have been                      |
    |             due to a System method returning false, in which case       |
    |             the System will have set an error text.                     |
    |             The main window, which will receive our unsuccessful        |
    |             return, will obtain that text directly from the             |
    |             System and show it to the user.                             |
    \*-----------------------------------------------------------------------*/
Boolean EUOPResolve::resolve( const char * AASFile
                             ,const char * MRDBFile
                             ,const char * MRDBDescFile
                             ,const char * MsgStringsFile)
{
 //This method replaced with EUOPResolve::resolveX below ...
 Boolean fOK = false; //jt Boolean fOK = false,
 return fOK;
}

Boolean EUOPResolve::resolveX( const char * AASFile
                              ,const char * MRDBFile
                              ,const char * MRDBDescFile
                              ,const char * MsgStringsFile
                              ,ISortedSet<IString>* pNoCompTable
                              ,ISortedSet<IString>* pQuickFixTable    //49222-MAMT-06/27/2000
                              ,ISortedSet<IString>* pFeature54Table   //53915-MAMT-08/31/2000
                              ,ISortedSet<IString>* pISeriesSWTable   //F72273 -HECC-06/28/2002
                              ,char * szCompiledFeatFile
                              ,Boolean fForcePKRead
                              ,Boolean bS390
                              ,Boolean bAS4 )  //Include bAS4 flag to reconcile iSeries SW.  HECC 72273 06/27/2002
{
 IString strConvertedFile;
 FileNameOnly(MRDBFile,CurrentSystem);

 Boolean fContinue = false; //* D81708: To flag that reconciliation continues. HECC 05/26/2003
 IString strPKFilePath = szCompiledFeatFile; //* F81710 To check if we need to load the feature.$$F file.  HECC 05/26/2003

 Boolean fOK = false,
         fConvert = getenv("RM2CONVERT31")? true:false;
//       fConvert = true;
 pW->WWShowTimer();
 if (re_Set(AASFile,MRDBFile,MRDBDescFile,MsgStringsFile)) {
   pW->WWRe_StartTimer((char *)getString(MAINW_MSG_READING_AAS,
                                         findFileName(AASFile)));
   int Version = filesVersion(AASFile);
   if ((Version == 31) && fConvert)
     if (!convert(pW,AASFile,false,strConvertedFile,Version = 30)) {
       pW->WWHideTimer();
       return false;
       }
   if (((Version==31) && readCFRFileV31(pW,false,bAS4)) || //F73732 EDW 07/25/02
        ((Version!=31) && readCFRFile(pW,false,bAS4))) {   //Include bAS4 flag to reconcile iSeries SW.  HECC 72273 06/27/2002
     if (strConvertedFile.length())              // converted
       remove((char *)strConvertedFile);
     pW->WWShowDetails(false);
     pW->WWSetTimerText((char *)getString(MAINW_MSG_BLDG_PK));

     IString strMRDBDesc = MRDBDescFile;               //edw06/10/1998
     strMRDBDesc.strip();                              //edw06/10/1998
     Boolean bGetDescriptions;                         //edw06/10/1998
     if ( strMRDBDesc.length() )                       //edw06/10/1998
         bGetDescriptions = true;                      //edw06/10/1998
     else                                              //edw06/10/1998
         bGetDescriptions = false;                     //edw06/10/1998

     //* F79548 Begin New Logic: When doing a SSD lookup, none of the models will
     //* be selected in the profile. This will cause the feature.$$f file not be
     //* found in any of the Product Family directories. In this case, logic must
     //* send an error message to the user and continue.
     //* if (getPKX(bGetDescriptions,fForcePKRead,szCompiledFeatFile)) {

     //* F81710 Begin new logic: If strPKFilePath contains a valid PK file path,
     //* then a valid server model was passed as a parameter or the Product Family
     //* preference was set to AS400, RS6000 or S390.
     //* Note: strPKFilePath will be empty if the parsed parameter from eConfig is not a
     //* server model or if the profile product family is not S390, AS400 or RS6000

     //* if (!getPKX(bGetDescriptions,fForcePKRead,szCompiledFeatFile))
     if ( (!getPKX(bGetDescriptions,fForcePKRead,szCompiledFeatFile)) &&
          (strPKFilePath != "")                                           )
     //* F81710 End new logic:   HECC - 06/03/2003

        {
         IString strText = LOADSTRING(EUO_MSG_FEATURE_FILE_NOTFOUND);
         strText.change(STR_Substitute0, szCompiledFeatFile);

         //* D81708: Changes the style of the error message to a Yes/No styled dialog.
         //* HECC - 05/28/2003
         //* IMessageBox::Style style = IMessageBox::warningIcon;
         IMessageBox::Style style = IMessageBox::yesNoButton |
                                    IMessageBox::warningIcon;

         IMessageBox msg(IWindow::desktopWindow());
         msg.setTitle(IResourceId(IDS_ErrorBackEnd_Title));

         //* D81708: Adds the "reply" variable to receive the choice of the user if he
         //* wishes to continue or not without the .$$F file.    HECC - 05/28/2003
         //* msg.show(strText, style);
         IMessageBox::Response reply = msg.show(strText, style, IDH_ErrorBackEnd);

         //* D81708: In case that response from user be "Yes" set the fContinue flag to
         //* "true". This flag will let the logic to continue with reconciliation even
         //* without the feature.$$f data. HECC - 05/28/2003
         if (reply == IMessageBox::yes)
            fContinue = true;

        }
     //* D81708: If the feature.$$f file was loaded successfully, set the fContinue
     //* flag to "true", to enable the logic to continue with reconciliation.
     else
        fContinue = true;

       //* F79548 End New Logic:  HECC - 05/08/2003

       //* D81708: Continue reconciliation if:
       //* a)User selected to continue even without the .$$f file found,
       //* b)Specified .$$f file was loaded successfully.
       if (fContinue)
          {
       //* D81708: End New Logic:  HECC - 05/08/2003

       pW->WWSetTimerText((char *)getString(MAINW_MSG_READING_MRDB,
                                            findFileName(MRDBFile)));
       Version = filesVersion(MRDBFile);
       if ((Version == 31) && fConvert)
         if (!convert(pW,MRDBFile,true,strConvertedFile,Version = 30)) {
           pW->WWHideTimer();
           return false;
           }
       if (((Version==31) && readCFRFileV31(pW,true,bAS4)) ||  //F73732 EDW 07/25/02
           ((Version!=31) && readCFRFile(pW,true,bAS4))) {     //Include bAS4 flag to reconcile iSeries SW.  HECC 72273 06/27/2002
         if (strConvertedFile.length())          // converted
           remove((char *)strConvertedFile);
         pW->WWShowDetails(true);
         pW->WWSetTimerText((char *)getString(MAINW_MSG_RESOLVING));
         applyPK();
         //53915 MAMT 28/02/2001 include bS390 in check4DifferentModelMachines
         if (check4DifferentModelMachines(pW , bS390)) {
            if ( bS390 )
               {
               resolveMatches();
               resolveLoneMachines();
               resolveUsingNoCompareTable( pNoCompTable, pQuickFixTable, pFeature54Table );
                                          // 49222 pQuickFixTable, 53915 Feature54TableSet

               //F72273  Begin:  Include bAS4 flag and iSeries SW features table
               //to reconcile iSeries SW.     HECC  07/02/2002
               //resolveOthers();
               resolveOthers( bAS4, pISeriesSWTable );
               //F72273   End.

               floatFirstMRDBMachine();

               //F72273  Begin:  Include bAS4 flag
               //to reconcile iSeries SW.     HECC  07/10/2002
               //generateFeatList( );
               generateFeatList( bAS4 );

               generateBacklogList();
               rpos().SortUp();
               if (!usingCompiledPK())
                 savePK();
               fOK = true;
               }
            else
               {
               resolveMRCombos();
               resolveGenerics();
               resolveMatches();
               resolveLoneMachines();
               resolveNonEquals();

               //F72273  Begin:  Include bAS4 flag and iSeries SW features table
               //to reconcile iSeries SW.     HECC  07/02/2002
               //resolveOthers();
               resolveOthers( bAS4, pISeriesSWTable );
               //F72273  End.

               floatFirstMRDBMachine();

               //F72273  Begin:  Include bAS4 flag
               //to reconcile iSeries SW.     HECC  07/10/2002
               //generateFeatList( );
               generateFeatList( bAS4 );

               generateBacklogList();
               rpos().SortUp();
               if (!usingCompiledPK())
                 savePK();
               fOK = true;
               }
            }
         else
            fOK = false; // 53915 MAMT
         }
       }
     }
   }
 pW->WWHideTimer();
 return fOK;
}

Boolean EUOPResolve::convert(XWindow *pW, const char * szFile,Boolean fMRDB,
                             IString & strConvertedFile,int version)
{
 strConvertedFile = IString(szFile);
 strConvertedFile.remove(strConvertedFile.lastIndexOf('.'));
 strConvertedFile += IString(fMRDB? ".M" : ".A");
 strConvertedFile += IString((char *)IString(version),2);
 if (fMRDB)
   setFileNames(0, (const char *)strConvertedFile);
 else setFileNames((const char *)strConvertedFile);
 return runConversion(pW,szFile,(char *)strConvertedFile,version);
}

    /*-----------------------------------------------------------------------*\
    |    Boolean reBuild( const char * szFileName)                            |
    |       Rebuild the System from the previously saved                      |
    |       version in the given filename and                                 |
    |       indicate (non)success.                                            |
    |                                                                         |
    |       Retain the System's name as the file's name.                      |
    |       Have the main window show its timer with a message                |
    |       stating that the System is being rebuild.                         |
    |       If the System is successfully built:                              |
    |          Have the main window show the System's particulars.            |
    |          Consider the method's successful if there's no problem         |
    |            establishing the System's Product Knowledge.                 |
    |       Have the main window eliminate its timer and message.             |
    |       Return indicating success/failure.                                |
    \*-----------------------------------------------------------------------*/
Boolean EUOPResolve::reBuild( const char * szFileName)
{
 FileNameOnly(szFileName,CurrentSystem);
 Boolean fOK = false;
 pW->WWShowTimer();
 pW->WWRe_StartTimer(message(EUO_MSG_REBUILD,findFileName(szFileName)));
 if (reBuildState(szFileName)) {
   pW->WWShowDetails(true);
   if (fOK = getPKX())
//   if (fOK = getPK())
     floatFirstMRDBMachine();
   }
 pW->WWHideTimer();
 return fOK;
// return (reBuildState(szFileName) && getPK());
}

    /*-----------------------------------------------------------------------*\
    |    Boolean writeReport(const char * szFileName,Boolean fAppendRPOS)     |
    |       Write the current System to a new CFReport of the                 |
    |       given name, optionally appending generated RPOs and               |
    |       indicate (non)success.                                            |
    |                                                                         |
    |       Have the main window show its timer with a message                |
    |          stating that the System is being written.                      |
    |       Retain the return from the System's method.                       |
    |       Have the main window eliminate its timer and message.             |
    |       Return the return.                                                |
    \*-----------------------------------------------------------------------*/
Boolean EUOPResolve::writeReport(const char * szFileName,Boolean fAppendRPOS,Boolean bSetLockingFlag)
{
 pW->WWShowTimer();
 pW->WWRe_StartTimer(message(EUO_MSG_WRITING,findFileName(szFileName)));
 //edw
 //Boolean fMRDB = (getenv("RM2WRITEAAS")? false : true);
 Boolean fMRDB = (getenv("RM2WRITEAAS")? true : false);
 Boolean fOK = fWriteCFR(szFileName,fMRDB,fAppendRPOS,bSetLockingFlag);
// DosSleep(1000);
 pW->WWHideTimer();
 return fOK;
}

    /*-----------------------------------------------------------------------*\
    |    Boolean save(const char * szFileName)                                |
    |       Save the current System to a file with the given name and         |
    |       indicate (non)success.                                            |
    |                                                                         |
    |       Enforce a "RM2" extension on the given filename.                  |
    |       Have the main window show its timer with a message                |
    |         saying that the System is saved to that file.                   |
    |       Retain the return from the System's method.                       |
    |       Have the main window eliminate its timer and message.             |
    |       Return the return.                                                |
    \*-----------------------------------------------------------------------*/
Boolean EUOPResolve::save(const char * szFileName)
{
 char Buf[15];
 pW->WWShowTimer();
 pW->WWRe_StartTimer(message(EUO_MSG_SAVING,
                             MakeFileName(Buf,findFileName(szFileName),"RM2")));
 IString str(szFileName);
 int i = str.lastIndexOf('.');
 if (i)
   str.remove(i);
 str += IString(".RM2");
 Boolean fOK = saveState((char *)str);
 //jt DosSleep(1000);
 pW->WWHideTimer();
 return fOK;
}


#else

 /*--------------------------------------------------------------------------*\
 |                         Non-Compiled testing code                          |
 \*--------------------------------------------------------------------------*/

///////////////////////////
Boolean EUOPResolve::resolve( const char * AASFile
                             ,const char * MRDBFile
                             ,const char * MRDBDescFile
                             ,const char * MsgStringsFile)
{
 FileNameOnly((const char *)MRDBFile,CurrentSystem);
 if (re_Set(AASFile,MRDBFile,MRDBDescFile,MsgStringsFile))
  if (getPK(true))
   if (readCFRFile(false))
    if (readCFRFile(true))
       { applyPK();
         resolveNoFeatMachines();
         resolveGenerics();
         resolveMatches();
         resolveMRCombos();
//       resolveGenerics();
         resolveNonEquals();
         resolveOthers();
         generateFeatList();
         generateBacklogList();

         if (!usingCompiledPK())
            savePK();
         return true;
       }
 return false;
}

Boolean EUOPResolve::reBuild( const char * szFileName)
{
 FileNameOnly(szFileName,CurrentSystem);
 return (reBuildState(szFileName) && getPK());
}

Boolean EUOPResolve::writeReport(const char * szFileName,Boolean fAppendRPOs)
{
 return fWriteCFR(szFileName,true,true,fAppendRPOs);
}

Boolean EUOPResolve::save(const char * szFileName)
{
 return saveState(szFileName);
}

////////////////////////////////////////////////////////////

#include <iostream.h>

int main(int argc,char **argv)
{
 char Buffer[20];
 if (argc < 3)
  { cout << "Usage : EUOPRES AASFile MDBFile [MDBDescription File]";
    exit(1);
  }
 if (1)
   {
     Boolean fOK;
     EUOPResolve R;
     mystrncpy(Buffer,(argc > 3)? argv[3] : "TST13_17.DSC",15);
     if (R.resolve(argv[1],argv[2],Buffer,"EUOENU.MSG"))
       {
         R.SystemOut();
         R.writeReport((const char *)MakeFileName(Buffer,argv[1],"NW1"));
         R.save(argv[1]);
         if (R.reBuild(argv[1]))
           { R.writeReport((const char *)MakeFileName(Buffer,argv[1],"NW2"));
             R.SystemOut();
           }
       }
#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif
   }

#ifdef __DEBUG_ALLOC__
   _dump_allocated(16);
#endif
  exit(0);
}

#endif
