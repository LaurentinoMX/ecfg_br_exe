 /*--------------------------------------------------------------------------*\
 |    Filename - PRF.CPP                                                      |
 |      contains source for the following EUOP class implementation:          |
 |                                                                            |
 |      EUOPProfile - to provide the EUOP application with                    |
 |                    user-preferred and EUOP-related data                    |
 |                    to be retained between program invocations              |
 |                    for which the OS2 and EUOP INI file are used.           |
 |                                                                            |
 |                    In addtiton, it offers a method to load the             |
 |                    application's Resource DLL and determine                |
 |                    the Help File Name.                                     |
 |                                                                            |
 |        The class maintains EUOP data in 2 states:                          |
 |                  1.  Default Values                                        |
 |                  2.  Profile Values (INI file data)                        |
 |         and, depending on its instantiation, a possible third:             |
 |                  3.  Current Values                                        |
 |         If Ctor is called with a 'true' parameter (the default             |
 |         being false), the class is in synchronous mode:                    |
 |               a. Current Value == Profile Value                            |
 |               b. Every 'Set' call will immediately update                  |
 |                  the INI file.                                             |
 |               c. There's no need to call 'WriteProfile' to                 |
 |                  update the INI file with the Current Values.              |
 |         In asynchronous mode:                                              |
 |               a. Current Value != Profile Value                            |
 |               b. Every 'Set' call does NOT update                          |
 |                  the INI file.                                             |
 |               c. The 'WriteProfile' call is required to                    |
 |                  update the INI file with the Current Values.              |
 |                                                                            |
 |         Notes:                                                             |
 |                Profile contains default values until                       |
 |                   'ReadProfile' is called.                                 |
 |                'WriteProfile' only updates those keys                      |
 |                    which were altered                                      |
 \*--------------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------*\
    |                               PM Includes                               |
    \*-----------------------------------------------------------------------*/
//jt extern "C"
//jt   {
   #define  INCL_PM
   #define  INCL_DOSNLS
   #define  INCL_WIN
   #define  INCL_WINSHELLDATA
   #include <os2.h>
   #include <direct.h>
//jt  }

#define INCL_IFRAMEWINDOW
#include "uicl.hpp"

    /*-----------------------------------------------------------------------*\
    |                            IBMClass includes                            |
    \*-----------------------------------------------------------------------*/
#include <iapp.hpp>
#include <ihelp.hpp>
#include <istattxt.hpp>
#include <ithread.hpp>
#include <ireslib.hpp>
#include <istring.hpp>

    /*-----------------------------------------------------------------------*\
    |                         Class-specific includes                         |
    \*-----------------------------------------------------------------------*/
#include "prf.h"                                 // this class' resource Ids
#include "prf.hpp"                               // this class' include

    /*-----------------------------------------------------------------------*\
    |                                 Macros                                  |
    \*-----------------------------------------------------------------------*/
#define ALMAX(a,b)        (((a) > (b)) ? (a) : (b))
#define ALMIN(a,b)        (((a) < (b)) ? (a) : (b))
#define DEFAULTMSGFILE        "RM2ENU.MSG"
    /*-----------------------------------------------------------------------*\
    |                                 Statics                                 |
    \*-----------------------------------------------------------------------*/
                  /*---------------------------------------------------------*\
                  |    String Constants                                       |
                  |     Multi-used strings ensuring non-duplicate             |
                  |     copies in the Data Segment.                           |
                  |     Includes EUOP.INI key strings                         |
                  \*---------------------------------------------------------*/
//jt
   typedef struct _COUNTRYINFO   /* ctryi */
   {
      ULONG       country;
      ULONG       codepage;
      ULONG       fsDateFmt;
      CHAR        szCurrency[5];
      CHAR        szThousandsSeparator[2];
      CHAR        szDecimal[2];
      CHAR        szDateSeparator[2];
      CHAR        szTimeSeparator[2];
      UCHAR       fsCurrencyFmt;
      UCHAR       cDecimalPlace;
      UCHAR       fsTimeFmt;
      USHORT      abReserved1[2];
      CHAR        szDataSeparator[2];
      USHORT      abReserved2[5];
   } COUNTRYINFO, *PCOUNTRYINFO;

COUNTRYINFO  CntryInfo;

//char *pPRF_NAME                  = "RM2.INI";
char *pPRF_NAME_OLD              = "BF9RM2";                       /* D53335 edw20000912 */  // F62721 EDW 01/21/2002
char *pPRF_NAME                  = "IBM\\econfig_profile\\BF9RM2"; /* D53335 edw20000912 */  // F62721 EDW 01/21/2002
char *pPRF_APP                   = "RM2Profile";
char *pPRF_USER_NAME             = "User_Name";
char *pPRF_USER_TL               = "User_TieLine";
char *pPRF_USER_VM_ID            = "User_VM_Id";
char *pPRF_USER_VM_NODE          = "User_VM_Node";
char *pPRF_DSPLYALL              = "DsplyAll";
char *pPRF_DSPLYDSCRP            = "DsplyDscrp";
char *pPRF_PRCSSGENRPO           = "PrcssGenRPO";
char *pPRF_PRCSSGENRPOLIST       = "PrcssGenRPOList";
char *pPRF_PRCSSGENRPOAUTO       = "PrcssGenRPOAuto";
char *pPRF_PRCSSAPPLYRSLVDFEAT   = "PrcssApplyRslvdFeat";
char *pPRF_PRCSSDSPLYEDITWND     = "PrcssDsplyEditWnd";
char *pPRF_VIEWDSCRPFEAT         = "ViewDscrpFeat";
char *pPRF_VIEWDSCRPFEATINFO     = "ViewDscrpFeatInfo";
char *pPRF_VIEWRSLVDFEAT         = "ViewRslvdFeat";
char *pPRF_VIEWRSLVDFEATINFO     = "ViewRslvdFeatInfo";
char *pPRF_VIEWGENRPO            = "ViewGenRPO";
char *pPRF_VIEWGENRPOINFO        = "ViewGenRPOInfo";
char *pPRF_VIEWEDITWNDINFO       = "ViewEditWndInfo";
char *pPRF_VIEWPRFLWNDINFO       = "ViewPRFLWndInfo";
char *pPRF_NONE                  = "none";
char *pPRF_PM_NTL                = "PM_National";
char *pPRF_EUOP                  = "RM2";
char *pPRF_HLP                   = ".HLP";
char *pPRF_MSG                   = ".MSG";
char *pPRF_WORKDIR               = "WorkDir";
char *pPRF_PKFILE                = "PKFile";

IString EUOPProfile::strExecPath;

                  /*---------------------------------------------------------*\
                  |        Language Table.                                    |
                  |         Used to resolve NLS related possibilities         |
                  |         by cross-referencing:                             |
                  |           ISO Language Code                               |
                  |           IBM Country Code                                |
                  |           Our Resource Designation                        |
                  |           Required ASCI Code Page                         |
                  |                (0 = supported by 437 & 850)               |
                  |           Required EBCDIC Code Page                       |
                  |                (0 = supported by 37 & 500)                |
                  \*---------------------------------------------------------*/
 static struct LangStrc { char *Lang;
                          long  lCC;
                          char *Res;
                          long  lReqASCICp;
                          long  lEBCDICCp;
                        } LangTbl[]=
             {
                 {"AFR",   -1,"ENU",   0,   0},  // Afrikaans
//               {"ARA",   -1,"ARA",   0,   0}, //Arabic
//               {"BGR",   -1,"BGR",   0,   0}, //Bulgarian
                 {"CAT",   -1,"ENU",   0,   0},  // Catalan
                 {"CHT",   88,"ENU", 948, 937},  // Traditional Chinese
//               {"CHS",   88,""   , 938,   0}, //Simplified Chinese
                 {"CSY",   42,"ENU", 852, 870},  // Czech
                 {"DAN",   45,"ENU", 865, 277},  // Danish
                 {"DEU",   49,"ENU",   0, 273},  // German
                 {"DES",   41,"ENU",   0, 273},  // Swiss German
                 {"ELL",   -1,"ENU", 869, 875},  // Greek
                 {"ENG",   44,"ENU",   0,   0},  // UK English
                 {"ENU",    1,"ENU",   0,   0},  // US English
                 {"ESP",   34,"ENU",   0,   0},  // Spanish
                 {"FIN",  358,"ENU",   0, 278},  // Finnish
                 {"FRA",   33,"ENU",   0, 297},  // French
                 {"FRB",   -1,"ENU",   0, 297},  // Belgian French
                 {"FRC",    2,"ENU", 863, 297},  // Canadian French
                 {"FRS",   -1,"ENU",   0, 297},  // Swiss French
                 {"HEB",   -1,"ENU", 862,   0},  // Hebrew
                 {"HUN",   36,"ENU", 852, 870},  // Hungarian
                 {"ISL",  354,"ENU", 861, 871},  // Icelandic
                 {"ITA",   39,"ENU",   0,   0},  // Italian
                 {"ITS",   -1,"ENU",   0,   0},  // Swiss Italian
                 {"JPN",   81,"ENU", 942, 939},  // Japanese
                 {"KOR",   82,"ENU", 944, 933},  // Korean
                 {"NLD",   31,"NLD",   0,   0},  // Dutch
                 {"NLB",   -1,"NLD",   0,   0},  // Belgian Dutch
                 {"NON",   -1,"ENU", 865, 277},  // Norwegian Nynorsk
                 {"NOR",   47,"ENU", 865, 277},  // Norwegian Bokmal
                 {"PLK",   48,"ENU", 852, 870},  // Polish
                 {"PTG",  351,"ENU", 860,  37},  // Portuguese
                 {"PTB",   -1,"ENU", 860,  37},  // Brazilian Portuguese
                 {"RMS",   -1,"ENU",   0,   0},  // Rhaeto-Romanic
//               {"ROM",   -1,"ROM",   0,   0}, //Romanian
//               {"RUS",   -1,"RUS",   0,   0}, //Russian
//               {"SHC",   -1,"SHC",   0,   0}, //Serbo_Croation (Cyrillic)
//               {"SHL",   -1,"SHL",   0,   0}, //Serbo_Croation (Latin 2)
//               {"SKY",   -1,"SKY",   0,   0}, //Slovakian
//               {"SQI",   -1,"SQI",   0,   0}, //Albanian
                 {"SVE",   46,"ENU",   0,   0},  // Swedish
//               {"THA",   -1,"THA",   0,   0}, //Thai
                 {"TRK",   90,"ENU", 857,1026},  // Turkish
//               {"URD",   -1,"URD",   0,   0}, //Urdu
                 {"",0,"",0,0}
             };

static char TempBuf[256];                        // work buffer

    /*-------------------------------------------------------*\
    |              Private straight C Functions               |
    \*-------------------------------------------------------*/
           /*----------------------------------------------------------------*\
           |    Boolean ResourceNameFromCntryCode(long, char *)               |
           |      Store our Resource Code for the given Country               |
           |      Code in the given buffer.                                   |
           |    Returns false/true: (un)successful                            |
           \*----------------------------------------------------------------*/
static Boolean ResourceNameFromCntryCode(long lCntryCode, char *szTrgt)
{
 struct LangStrc *pLS;
 for (*szTrgt=0,pLS=LangTbl; pLS->lCC && pLS->lCC != lCntryCode;pLS++);
 if ( pLS->lCC == lCntryCode) strcpy(szTrgt,pLS->Res);
 return (*szTrgt != 0);
}


           /*----------------------------------------------------------------*\
           |    Boolean CodepageSupportsLanguage(char *szLang,long lCurrCp)   |
           |      Return whether or not the given Language Code               |
           |      is supported by the given Code Page (true/false)            |
           \*----------------------------------------------------------------*/
static Boolean CodepageSupportsLanguage(char *szLang,long lCurrCp)
{
 struct LangStrc *pLS;
 for (pLS=LangTbl; *(pLS->Lang) && stricmp(szLang,pLS->Lang);pLS++);
 if  ( *(pLS->Lang) &&
       ( (pLS->lReqASCICp && lCurrCp == pLS->lReqASCICp) ||
         (!(pLS->lReqASCICp) && ( lCurrCp == 437 || lCurrCp == 850))) )
      return true;
 return false;
}

           /*----------------------------------------------------------------*\
           |   Boolean CheckResourceLanguage(char *szLang)                    |
           |      Return whether or not the given string is a valid           |
           |      Resource Designation (true/false)                           |
           \*----------------------------------------------------------------*/
static Boolean CheckResourceLanguage(char *szLang)
{
 struct LangStrc *pLS;
 for (pLS=LangTbl; *(pLS->Res) && stricmp(szLang,pLS->Res);pLS++);
 return (*(pLS->Res) != '\0');
}

    /*-----------------------------------------------------------------------*\
    |           Ctor                                                          |
    |            Set the write-mode as given.                                 |
    |            Initialize as follows:                                       |
    |               Set the Default Values                                    |
    |               Set the Profile Values to 'not there/read'                |
    |               Set the Current Values to their defaults                  |
    |               Instantiate a Profile Class                               |
    \*-----------------------------------------------------------------------*/
EUOPProfile :: EUOPProfile(Boolean fSync)
             : pProfile(NULL)
              ,fDDsplyAll(true)              ,fPDsplyAll(2)
              ,fDDsplyDscrp(false)           ,fPDsplyDscrp(2)
              ,fDPrcssGenRPO(true)           ,fPPrcssGenRPO(2)
              ,fDPrcssGenRPOList(true)       ,fPPrcssGenRPOList(2)
              ,fDPrcssGenRPOAuto(false)      ,fPPrcssGenRPOAuto(2)
              ,fDPrcssApplyRslvdFeat(true)   ,fPPrcssApplyRslvdFeat(2)
              ,fDPrcssDsplyEditWnd(true)     ,fPPrcssDsplyEditWnd(2)
              ,fDViewDscrpFeat(true)         ,fPViewDscrpFeat(2)
              ,fDViewDscrpFeatInfo(false)    ,fPViewDscrpFeatInfo(2)
              ,fDViewRslvdFeat(true)        ,fPViewRslvdFeat(2)
              ,fDViewRslvdFeatInfo(false)    ,fPViewRslvdFeatInfo(2)
              ,fDViewEditWndInfo(false)      ,fPViewEditWndInfo(2)
              ,fDViewPrflWndInfo(false)      ,fPViewPrflWndInfo(2)
              ,fDViewGenRPO(false)           ,fPViewGenRPO(2)
              ,fDViewGenRPOInfo(false)       ,fPViewGenRPOInfo(2)
              ,fSyncWrite(fSync)
              ,fProfileRead(false)
              ,strNone(pPRF_NONE)
              ,strWorkDir(".")
{
// char * pc = _getdcwd(_getdrive(),NULL,0);
// if (pc)
//   { strDefaultWorkDir = strWorkDir = IString(pc);
//     free(pc);
//   }
// strDefaultWorkDir = IApplication::current().argv(0);
// strDefaultWorkDir.remove(strDefaultWorkDir.lastIndexOf('\\'));

//edw10/21/98-Set application execution path attribute

 IString strAppName = IApplication::current().argv(0);
 ULONG ulOffset = strAppName.lastIndexOf(STR_PathDelimiter);
 if ( ulOffset )
    strAppName = strAppName.subString( ulOffset + 1 );
 ulOffset = strAppName.lastIndexOf(".");
 if ( !ulOffset )
    strAppName = strAppName.insert( STR_ExtensionEXE, strAppName.length() );

 char strE[100], strE1[100];
 LPTSTR *strE2 = (LPSTR*)strE1;
 SearchPath(NULL, strAppName, NULL, 99, strE, strE2);
 strExecPath = strE;

 ulOffset = strExecPath.lastIndexOf(STR_PathDelimiter);
 if ( ulOffset )
    strExecPath.remove(ulOffset);
 else
    strExecPath = "";

 strDefaultWorkDir = strExecPath;

 strWorkDir = strDefaultWorkDir;

 fDsplyAll            = fDDsplyAll;
 fDsplyDscrp          = fDDsplyDscrp;
 fPrcssGenRPO         = fDPrcssGenRPO;
 fPrcssGenRPOList     = fDPrcssGenRPOList;
 fPrcssGenRPOAuto     = fDPrcssGenRPOAuto;
 fPrcssApplyRslvdFeat = fDPrcssApplyRslvdFeat;
 fPrcssDsplyEditWnd   = fDPrcssDsplyEditWnd;
 fViewDscrpFeat       = fDViewDscrpFeat;
 fViewDscrpFeatInfo   = fDViewDscrpFeatInfo;
 fViewRslvdFeat       = fDViewRslvdFeat;
 fViewRslvdFeatInfo   = fDViewRslvdFeatInfo;
 fViewEditWndInfo     = fDViewEditWndInfo;
 fViewPrflWndInfo     = fDViewPrflWndInfo;
 fViewGenRPO          = fDViewGenRPO;
 fViewGenRPOInfo      = fDViewGenRPOInfo;
 //pProfile = new IProfile(pPRF_NAME);
 pProfile = new IProfile( IProfile::userProfile().name() +
                              STR_PathDelimiter + pPRF_NAME ); /* D53335 edw20000912 */

 pProfile->setDefaultApplicationName(pPRF_APP);
 memset(&CntryInfo,0,sizeof(COUNTRYINFO));       // null-out PM structures
 memset(CPList,0,sizeof(CPList));
}

    /*-----------------------------------------------------------------------*\
    |           Dtor                                                          |
    |            Release obtained resources.                                  |
    \*-----------------------------------------------------------------------*/
EUOPProfile :: ~EUOPProfile()
{
 if (pProfile) delete pProfile;
}

         /*------------------------------------------------------------------*\
         |     Boolean SetResources( ICurrentApplication & App                |
         |                            ,unsigned short usArg)                  |
         |        Method loads the Resource DLL and determines the            |
         |        proper Help File Name for the application, informing        |
         |        the user when unsuccessful:                                 |
         |          If the application was started with arguments:            |
         |            Check the arguments or a specific argument              |
         |              if its number is given, for one of our Resource       |
         |              Designations (upper or lower case).                   |
         |            For a valid designation, ensure it's supported          |
         |              by the current CodePage and if so, assume the         |
         |              corresponding Help File would be desired.             |
         |          If a Resource Designation was not obtained via            |
         |              argement(s), use the default-one as per the           |
         |              current Country Info.                                 |
         |          Load the Resource DLL and tell the user when              |
         |          unsuccessful, showing the searched for filename           |
         |          since it's most likely its absence that caused            |
         |          the error.                                                |
         |                                                                    |
         |    Note:                                                           |
         |        This method is intended to be called before the             |
         |          application is started and any DLL resources are          |
         |          to be used, since it's unlikely that the logic            |
         |          would reach this far when resources are unavaillable.     |
         |          Therefore, it uses 'ICurrentApplication::run' to be       |
         |          able show it's private Error Message window.              |
         |                                                                    |
         |        Returns: false/true: (un)successful                         |
         \*------------------------------------------------------------------*/
Boolean EUOPProfile :: SetResources( ICurrentApplication & App
                                     ,unsigned short usArg)
{
return true;
//edw disabled 04 09 97
// IString strDllName,strErrorMsg;
// char * pch = 0;
// if (App.argc() > 1)
//   { if (usArg)
//       { if (CheckResourceLanguage((char *)App.argv(usArg)))
//          pch = (char *)App.argv(usArg);
//       }
//     else for ( usArg = App.argc() - 1; usArg && !pch; usArg--)
//            if (CheckResourceLanguage((char *)App.argv(usArg)))
//              pch = (char *)App.argv(usArg);
//     if (pch)
//       { int iRC = SpecificResourceFile(&strDllName,pch);
//         if (!iRC)
//           { IString Name;
//             SpecificHelpFile(&Name,pch);
//             if (Name.size())
//                SetCurrentHelpFile((char *)Name);
//             SpecificMessageFile(&Name,pch);
//             if (Name.size())
//                SetCurrentMessageFile((char *)Name);
//           }
//         else { strErrorMsg = (iRC==EUOPProfile::NotSupported)?
//                        "The application does not recognize language id : " :
//                        "The current codepage does not support language id : ";
//                strErrorMsg += pch;
//              }
//       }
//   }
// if (!pch)
//   strDllName = DefaultResourceFile();
// if (!strErrorMsg.size())
//   { try { App.setUserResourceLibrary(strDllName);
//         }
//     catch (IException& exc)
//       { strErrorMsg = "Unable to load Resource File : '";
//         strErrorMsg += strDllName;
//         strErrorMsg += IString(".DLL'!");
//       }
//   }
// if (strErrorMsg.size())
//   { DosBeep(750,1000) ;
//     IFrameWindow * msgWindow=new IFrameWindow(0x1000);
//     IStaticText * msg=new IStaticText( 0x1010, msgWindow, msgWindow);
//     msg->setText((char *)strErrorMsg);
//     msg->setAlignment( IStaticText::centerCenter);
//     msgWindow->sizeTo(ISize(500,300));
//     msgWindow->setClient(msg);
//     msgWindow->setFocus();
//     msgWindow->show();
//     App.run();
//   }
// return (strErrorMsg.size() == 0);
}

         /*------------------------------------------------------------------*\
         |     Boolean ReadProfile()                                          |
         |        Set both the Profile and Current Values of the              |
         |        EUOP-related profile items.                                 |
         |        Returns: false/true: profile (not) read                     |
         \*------------------------------------------------------------------*/
Boolean EUOPProfile :: ReadProfile()
{
 fProfileRead = true;
 if (pProfile->containsApplication(pPRF_APP))
   {
     if (pProfile->containsKeyName(pPRF_DSPLYALL))
       fDsplyAll=fPDsplyAll=(Boolean)pProfile->integerWithKey(pPRF_DSPLYALL);
     if (pProfile->containsKeyName(pPRF_DSPLYDSCRP))
       fDsplyDscrp = fPDsplyDscrp =
                      (Boolean)pProfile->integerWithKey( pPRF_DSPLYDSCRP );
     if (pProfile->containsKeyName(pPRF_PRCSSGENRPO))
       fPrcssGenRPO=fPPrcssGenRPO=pProfile->integerWithKey( pPRF_PRCSSGENRPO );
     if (pProfile->containsKeyName(pPRF_PRCSSGENRPOLIST))
       fPrcssGenRPOList = fPPrcssGenRPOList =
                              pProfile->integerWithKey( pPRF_PRCSSGENRPOLIST );
     if (pProfile->containsKeyName(pPRF_PRCSSGENRPOAUTO))
       fPrcssGenRPOAuto = fPPrcssGenRPOAuto =
                              pProfile->integerWithKey( pPRF_PRCSSGENRPOAUTO );
     if (pProfile->containsKeyName(pPRF_PRCSSAPPLYRSLVDFEAT))
       fPrcssApplyRslvdFeat = fPPrcssApplyRslvdFeat =
                            pProfile->integerWithKey(pPRF_PRCSSAPPLYRSLVDFEAT);
     if (pProfile->containsKeyName(pPRF_PRCSSDSPLYEDITWND))
       fPrcssDsplyEditWnd = fPPrcssDsplyEditWnd =
                            pProfile->integerWithKey(pPRF_PRCSSDSPLYEDITWND);

     if (pProfile->containsKeyName(pPRF_VIEWDSCRPFEATINFO))
       fViewDscrpFeatInfo = fPViewDscrpFeatInfo =
                            pProfile->integerWithKey( pPRF_VIEWDSCRPFEATINFO );

     if (pProfile->containsKeyName(pPRF_VIEWRSLVDFEAT))
       fViewRslvdFeat = fPViewRslvdFeat =
                              pProfile->integerWithKey( pPRF_VIEWRSLVDFEAT );
     if (pProfile->containsKeyName(pPRF_VIEWRSLVDFEATINFO))
       fViewRslvdFeatInfo = fPViewRslvdFeatInfo =
                          pProfile->integerWithKey( pPRF_VIEWRSLVDFEATINFO );
     if (pProfile->containsKeyName(pPRF_VIEWEDITWNDINFO))
       fViewEditWndInfo = fPViewEditWndInfo =
                          pProfile->integerWithKey( pPRF_VIEWEDITWNDINFO );
     if (pProfile->containsKeyName(pPRF_VIEWPRFLWNDINFO))
       fViewPrflWndInfo = fPViewPrflWndInfo =
                          pProfile->integerWithKey( pPRF_VIEWPRFLWNDINFO );
     if (pProfile->containsKeyName(pPRF_VIEWGENRPO))
       fViewGenRPO=fPViewGenRPO=pProfile->integerWithKey( pPRF_VIEWGENRPO );
     if (pProfile->containsKeyName(pPRF_VIEWGENRPOINFO))
       fViewGenRPOInfo = fPViewGenRPOInfo =
                               pProfile->integerWithKey( pPRF_VIEWGENRPOINFO );
     if (pProfile->containsKeyName(pPRF_WORKDIR))
        strWorkDir = pProfile->elementWithKey( pPRF_WORKDIR );

     if (pProfile->containsKeyName(pPRF_USER_NAME))
       strUserName = pProfile->elementWithKey( pPRF_USER_NAME);
     if (pProfile->containsKeyName(pPRF_USER_TL))
       strUserTieLine = pProfile->elementWithKey( pPRF_USER_TL);
     if (pProfile->containsKeyName(pPRF_USER_VM_ID))
       strUserVM_Id = pProfile->elementWithKey( pPRF_USER_VM_ID);
     if (pProfile->containsKeyName(pPRF_USER_VM_NODE))
       strUserVM_Node = pProfile->elementWithKey( pPRF_USER_VM_NODE);
     if (pProfile->containsKeyName(pPRF_PKFILE))
       strPKFile = pProfile->elementWithKey( pPRF_PKFILE);
     return true;
    }
 return false;
}

         /*------------------------------------------------------------------*\
         |     Boolean GetBooleanKey(BooleanKey Key,Boolean fDefault)         |
         |        Return the Current or Default Value for the                 |
         |        given Key                                                   |
         \*------------------------------------------------------------------*/
Boolean EUOPProfile :: GetBooleanKey(BooleanKey Key,Boolean fDefault)
{
 if (!fProfileRead) ReadProfile();
 switch(Key)
  { case DsplyAll:            return fDefault? fDDsplyAll            : fDsplyAll           ;
    case DsplyDscrp:          return fDefault? fDDsplyDscrp          : fDsplyDscrp         ;
    case PrcssGenRPO:         return fDefault? fDPrcssGenRPO         : fPrcssGenRPO        ;
    case PrcssGenRPOList:     return fDefault? fDPrcssGenRPOList     : fPrcssGenRPOList    ;
    case PrcssGenRPOAuto:     return fDefault? fDPrcssGenRPOAuto     : fPrcssGenRPOAuto    ;
    case PrcssApplyRslvdFeat: return fDefault? fDPrcssApplyRslvdFeat : fPrcssApplyRslvdFeat;
    case PrcssDsplyEditWnd:   return fDefault? fDPrcssDsplyEditWnd   : fPrcssDsplyEditWnd  ;
    case ViewDscrpFeat:       return fDefault? fDViewDscrpFeat       : fViewDscrpFeat     ;
    case ViewDscrpFeatInfo:   return fDefault? fDViewDscrpFeatInfo   : fViewDscrpFeatInfo ;
    case ViewRslvdFeat:       return fDefault? fDViewRslvdFeat       : fViewRslvdFeat     ;
    case ViewRslvdFeatInfo:   return fDefault? fDViewRslvdFeatInfo   : fViewRslvdFeatInfo ;
    case ViewEditWndInfo:     return fDefault? fDViewEditWndInfo     : fViewEditWndInfo   ;
    case ViewPrflWndInfo:     return fDefault? fDViewPrflWndInfo     : fViewPrflWndInfo   ;
    case ViewGenRPO:          return fDefault? fDViewGenRPO          : fViewGenRPO        ;
    case ViewGenRPOInfo:      return fDefault? fDViewGenRPOInfo      : fViewGenRPOInfo    ;
    default: return false;
  }
}

         /*------------------------------------------------------------------*\
         |     IString & GetStringKey(StringKey Key)                          |
         |        Return the string for the given Profile string key,         |
         |        obtaining Country Info if required and not yet done.        |
         |      Returns: string ( "none" if not found.)                       |
         \*------------------------------------------------------------------*/
IString & EUOPProfile :: GetStringKey(StringKey Key,Boolean fDefault)
{
 switch(Key)                                     // get data on demand
   { case DateSeparator:
        return GetCountryInfo()? strNone : strDateSeparator;
     case TimeSeparator:
        return GetCountryInfo()? strNone : strTimeSeparator;
     case TimeAM:
        return GetCountryInfo()? strNone : strTimeAM;
     case TimePM:
        return GetCountryInfo()? strNone : strTimePM;
     case WorkDir:
        return fDefault? strDefaultWorkDir : strWorkDir ;
     case UserName:
       return strUserName;
     case UserTieLine:
       return strUserTieLine;
     case UserVM_Id:
       return strUserVM_Id;
     case UserVM_Node:
       return strUserVM_Node;
     case ProductKnowledgeFile:
       return strPKFile;

     case CurrDsplyDrvr:
//      strCurrDsplyDrvr=pProfile->userProfile().elementWithKey(
//                                         "PM_DISPLAYDRIVERS","CURRENTDRIVER");
/*jt       if (!strCurrDsplyDrvr.size())
//jt          { PrfQueryProfileString(HINI_PROFILE,
//jt                                  "PM_DISPLAYDRIVERS", "CURRENTDRIVER",
//jt                                  (char *)strNone, TempBuf,255);
            strCurrDsplyDrvr=TempBuf;
          }*/
       return  strCurrDsplyDrvr;
   }
 return strNone;
}

         /*------------------------------------------------------------------*\
         |     int GetIntKey(IntKey Key)                                      |
         |        Return the int for the given key,                           |
         |        obtaining data if not already retrieved.                    |
         |      Returns: int (EUOPProfile::NotThere if not found.)            |
         \*------------------------------------------------------------------*/
int EUOPProfile :: GetIntKey(IntKey Key)
{
 switch(Key)
   { case CntryCode:
       return GetCountryInfo()? NotThere : CntryInfo.country;
     case DateFormat:
       return GetCountryInfo()? NotThere : CntryInfo.fsDateFmt;
     case TimeFormat:
       return GetCountryInfo()? NotThere :
                     ((CntryInfo.fsTimeFmt == '0')? Time12Hours : Time24Hours);
     case CodePageCurrent:
       return GetCodePages()? NotThere : CPList[0];
     case CodePagePrime:
       return GetCodePages()? NotThere : CPList[1];
     case CodePage2nd:
       return GetCodePages()? NotThere : CPList[2];
   }
 return NotThere;
}


         /*------------------------------------------------------------------*\
         |     int SpecificResourceFile(IString *pstr,char *szLang)           |
         |        Return the EUOP Resource filename for the                   |
         |        given language in the given string.                         |
         |      Returns: int :                                                |
         |              0          : filename in string.                      |
         |              EUOPProfile::NotCodePageSupported;                    |
         |              EUOPProfile::NotSupported (unknown Language Code      |
         \*------------------------------------------------------------------*/
int EUOPProfile :: SpecificResourceFile(IString *pstr,char *szLang)
{
 if (CheckResourceLanguage(szLang))
   if (!GetCodePages() && CodepageSupportsLanguage(szLang,CPList[0]))
      { *pstr = pPRF_EUOP;
//        *pstr += (char *)strupr(szLang);
        *pstr += (char *)szLang;
        pstr->upperCase();
        return 0;
      }
   else return NotCodePageSupported;
 else return NotSupported ;
}

         /*------------------------------------------------------------------*\
         |     IString & DefaultResourceFile()                                |
         |        Return the EUOP Default Resource filename                   |
         |        as per the System's Country Setting.                        |
         |      Returns: string ( "none" if unobtainable)                     |
         \*------------------------------------------------------------------*/
IString & EUOPProfile :: DefaultResourceFile()
{
 if  (!strDefaultResource.size())
   if (!GetCountryInfo() &&
       ResourceNameFromCntryCode(CntryInfo.country,TempBuf))
         { strDefaultResource = pPRF_EUOP;
           strDefaultResource += (char *)TempBuf;
         }
   else strDefaultResource = strNone;
 return strDefaultResource;
}

         /*------------------------------------------------------------------*\
         |     IString & DefaultHelpFile()                                    |
         |        Return the EUOP Help filename                               |
         |        as per the System's Country Setting.                        |
         |      Returns: string ( "none" if unobtainable)                     |
         \*------------------------------------------------------------------*/
IString & EUOPProfile :: DefaultHelpFile()
{
 if  (!strDefaultHelp.size())
   if (!GetCountryInfo() &&
       ResourceNameFromCntryCode(CntryInfo.country,TempBuf))
         { strDefaultHelp = pPRF_EUOP;
           strDefaultHelp += (char *)TempBuf;
           strDefaultHelp += pPRF_HLP;
         }
   else strDefaultHelp = strNone;
 return strDefaultHelp;
}

         /*------------------------------------------------------------------*\
         |     IString & DefaultMessageFile()                                 |
         |        Return the EUOP Message filename                            |
         |        as per the System's Country Setting.                        |
         |      Returns: string ( "none" if unobtainable)                     |
         \*------------------------------------------------------------------*/
IString & EUOPProfile :: DefaultMessageFile()
{
 if  (!strDefaultMsgF.size())
   if (!GetCountryInfo() &&
       ResourceNameFromCntryCode(CntryInfo.country,TempBuf))
         { strDefaultMsgF = pPRF_EUOP;
           strDefaultMsgF += (char *)TempBuf;
           strDefaultMsgF += pPRF_MSG;
           strDefaultMsgF = strWorkDir+"\\"+strDefaultMsgF;          //edw07/01/1998
         }
   else strDefaultMsgF = strWorkDir+"\\"+IString(DEFAULTMSGFILE);
 return strDefaultMsgF;
}


         /*------------------------------------------------------------------*\
         |     int SpecificHelpFile(IString *pstr,char *szLang)               |
         |        Return the EUOP Help filename for the                       |
         |        given language in the given string.                         |
         |      Returns: int :                                                |
         |              0          : filename in string.                      |
         |              EUOPProfile::NotCodePageSupported;                    |
         |              EUOPProfile::NotSupported                             |
         \*------------------------------------------------------------------*/
int EUOPProfile :: SpecificHelpFile(IString *pstr,char *szLang)
{
 if (CheckResourceLanguage(szLang))
   if (!GetCodePages() && CodepageSupportsLanguage(szLang,CPList[0]))
      { *pstr = pPRF_EUOP;
//        *pstr += (char *)strupr(szLang);
        *pstr += (char *)szLang;
        *pstr += pPRF_HLP;
        pstr->upperCase();
        return 0;
      }
   else return NotCodePageSupported;
 else return NotSupported ;
}

         /*------------------------------------------------------------------*\
         |     int SpecificMessageFile(IString *pstr,char *szLang)            |
         |        Return the EUOP Message filename for the                    |
         |        given language in the given string.                         |
         |      Returns: int :                                                |
         |              0          : filename in string.                      |
         |              EUOPProfile::NotCodePageSupported;                    |
         |              EUOPProfile::NotSupported                             |
         \*------------------------------------------------------------------*/
int EUOPProfile :: SpecificMessageFile(IString *pstr,char *szLang)
{
 if (CheckResourceLanguage(szLang))
   if (!GetCodePages() && CodepageSupportsLanguage(szLang,CPList[0]))
      { *pstr = pPRF_EUOP;
//        *pstr += (char *)strupr(szLang);
        *pstr += (char *)szLang;
        *pstr += pPRF_MSG;
        pstr->upperCase();
        return 0;
      }
   else return NotCodePageSupported;
 else return NotSupported ;
}


         /*------------------------------------------------------------------*\
         |     unsigned short GetCodePages()                                  |
         |        Get Code Pages (primary, secondary etc.)                    |
         |        in the CPList array if that's not already.                  |
         |        done                                                        |
         |      Returns: 0 : already done/'DosQueryCp's good return code      |
         |              !=0 : DosQueryCp's (bad) return code                  |
         \*------------------------------------------------------------------*/
unsigned short EUOPProfile :: GetCodePages()
{ return 0;//jt (!CPList[0])? DosQueryCp(sizeof(CPList),CPList,&CPList[3]) : false;
}

         /*-------------------------------------------------------------------*\
         |     unsigned short  GetCountryInfo()                                |
         |        Get Country-related Information from the                     |
         |        System and OS2 INI file if not already done.                 |
         |      Returns: 0 : already done/'DosQueryCtryInfo's good return code |
         |              !=0 : DosQueryCtryInfo's (bad) return code             |
         \*-------------------------------------------------------------------*/
unsigned short EUOPProfile :: GetCountryInfo()
{         /*
 if (!CntryInfo.country)
   {
     COUNTRYCODE CntryCode;
     ULONG  ulLen;
     CntryCode.country = CntryCode.codepage = 0L;
     unsigned short rc = DosQueryCtryInfo(sizeof(CntryInfo),&CntryCode,
                                          &CntryInfo,&ulLen);
     if (!rc)
       { CntryInfo.country = (ULONG)PrfQueryProfileInt(HINI_PROFILE,pPRF_PM_NTL,
                                                       "iCountry",
                                                       (long)CntryInfo.country);
         CntryInfo.fsDateFmt = (ULONG)PrfQueryProfileInt(HINI_PROFILE,
                                                         pPRF_PM_NTL, "iDate",
                                                     (long)CntryInfo.fsDateFmt);
         PrfQueryProfileString(HINI_PROFILE,pPRF_PM_NTL,"sDate",
                               CntryInfo.szDateSeparator, TempBuf,255L);
         strDateSeparator = TempBuf;
         PrfQueryProfileString(HINI_PROFILE,pPRF_PM_NTL,"sTime",
                               CntryInfo.szTimeSeparator, TempBuf,255L);
         strTimeSeparator = TempBuf;
         PrfQueryProfileString(HINI_PROFILE,pPRF_PM_NTL,"iTime","1",TempBuf,255L);
         CntryInfo.fsTimeFmt = *TempBuf;
         if ( CntryInfo.fsTimeFmt == '0')        // 12-hours format
           { PrfQueryProfileString(HINI_PROFILE,pPRF_PM_NTL,"s1159","AM.",
                                   TempBuf,255L);
             strTimeAM = TempBuf;
             PrfQueryProfileString(HINI_PROFILE,pPRF_PM_NTL,"s2359","PM.",
                                   TempBuf,255L);
             strTimePM = TempBuf;
           }
       }
     return rc;
   }*/
 return 0;
}

         /*------------------------------------------------------------------*\
         |     char ** EUOPProfile :: AllocateMonthArray()                    |
         |        Allocate storage for, and build a 12 pointer array          |
         |        and the null-terminated strings they point to from our      |
         |        own array of the NLS-sensitive months names,                |
         |        which will be obtained if still empty.                      |
         |     Returns: NULL/pointer : (un)successful.                        |
         |                                                                    |
         |     Note: It's up to the caller to release obtained storage        |
         |          (that's why it's all wrapped up into one chunk)           |
         \*------------------------------------------------------------------*/
char ** EUOPProfile :: AllocateMonthArray()
{
 if (!(AstrMonths[0].size())) LoadMonths();
 char **ppch = NULL, *pch;
 unsigned i,len = LongestMonthString()+1;
 if ( len > 1)
   { pch = new char[12 * (sizeof(ppch) + len)];
     for (i=0,ppch=(char **)pch,pch+=(12 * sizeof(ppch));i<12;i++,pch+=len)
       strcpy( ppch[i] = pch,(char *)AstrMonths[i]);
   }
 return ppch;
}

         /*------------------------------------------------------------------*\
         |     unsigned LongestMonthString()                                  |
         |        Return the length of the longest month name                 |
         \*------------------------------------------------------------------*/
unsigned EUOPProfile :: LongestMonthString()
{
 unsigned len, i;
 for (len = i = 0; i < 12 ; i++)
   len = ALMAX(len,AstrMonths[i].size());
 return len;
}

         /*------------------------------------------------------------------*\
         |     void LoadMonths()                                              |
         |        Method loads the array of month names from the              |
         |        Resource which it assumes has beeen loaded at               |
         |        the time of call.                                           |
         |     Returns: nothing                                               |
         |     Note: Month names Ids to be 12 consecutive numbers             |
         \*------------------------------------------------------------------*/
void EUOPProfile :: LoadMonths()
{
 int i,id;
 for (i = 0, id = PRFL_STR_MNTH_JAN ; i < 12 ; i++ , id++)
   AstrMonths[i] = IApplication::current().userResourceLibrary().loadString(id);
}

         /*------------------------------------------------------------------*\
         |     IString &  MonthString(unsigned short us)                      |
         |        Returns the string corresponding to the                     |
         |        given month (number).                                       |
         \*------------------------------------------------------------------*/
IString & EUOPProfile :: MonthString(unsigned short us)
{
 if (!(AstrMonths[0].size())) LoadMonths();
 us = ALMIN(ALMAX(us,1),12);
 return AstrMonths[us-1];
}

         /*------------------------------------------------------------------*\
         |     unsigned short GetMonthNmbr(IString & )                        |
         |        Returns the  month (number) corresponding to the            |
         |        given month name.                                           |
         \*------------------------------------------------------------------*/
unsigned short EUOPProfile :: GetMonthNmbr(IString & strMonth)
{
 if (!(AstrMonths[0].size()))
   LoadMonths();
 unsigned short usMonth;
 for (usMonth = 0; usMonth < 12 ; usMonth++)
   if  (strMonth == AstrMonths[usMonth])
     return (++usMonth);
 return 0;
}

         /*------------------------------------------------------------------*\
         |     Boolean SetBooleanKey(BooleanKey Key,Boolean value)            |
         |        Method sets the given profile key to the                    |
         |        given Boolean value:                                        |
         |          1. Set the Current key value                              |
         |          2. If we're in synchronous mode and the                   |
         |             value differs from that in the INI file:               |
         |               i.  Set the Profile key Value                        |
         |               ii. Write the value to the application's             |
         |                   INI file.                                        |
         |        Returns: false/true: (un)successful                         |
         \*------------------------------------------------------------------*/
Boolean EUOPProfile :: SetBooleanKey(BooleanKey Key,Boolean flag)
{
 Boolean fOK = true;
 const char *pch = NULL;
 switch(Key)
   {
     case DsplyAll:
         fDsplyAll = flag;
         if (fSyncWrite && fDsplyAll != fPDsplyAll)
            { fPDsplyAll = fDsplyAll;
              pch = pPRF_DSPLYALL;
            }
         break;
      case DsplyDscrp:
         fDsplyDscrp = flag;
         if (fSyncWrite && fDsplyDscrp != fPDsplyDscrp)
            { fPDsplyDscrp = fDsplyDscrp;
              pch = pPRF_DSPLYDSCRP;
            }
         break;
      case PrcssGenRPO:
         fPrcssGenRPO = flag;
         if (fSyncWrite && fPrcssGenRPO != fPPrcssGenRPO )
            { fPPrcssGenRPO = fPrcssGenRPO;
              pch = pPRF_PRCSSGENRPO;
            }
         break;
      case PrcssGenRPOList:
         fPrcssGenRPOList = flag;
         if (fSyncWrite && fPrcssGenRPOList != fPPrcssGenRPOList )
            { fPPrcssGenRPOList = fPrcssGenRPOList;
              pch = pPRF_PRCSSGENRPOLIST;
            }
         break;
      case PrcssGenRPOAuto:
         fPrcssGenRPOAuto = flag;
         if (fSyncWrite && fPrcssGenRPOAuto != fPPrcssGenRPOAuto )
            { fPPrcssGenRPOAuto = fPrcssGenRPOAuto;
              pch = pPRF_PRCSSGENRPOAUTO;
            }
         break;
      case PrcssApplyRslvdFeat:
         fPrcssApplyRslvdFeat = flag;
         if (fSyncWrite && fPrcssApplyRslvdFeat != fPPrcssApplyRslvdFeat )
            { fPPrcssApplyRslvdFeat = fPrcssApplyRslvdFeat;
              pch = pPRF_PRCSSAPPLYRSLVDFEAT;
            }
         break;
      case PrcssDsplyEditWnd:
         fPrcssDsplyEditWnd = flag;
         if (fSyncWrite && fPrcssDsplyEditWnd != fPPrcssDsplyEditWnd )
            { fPPrcssDsplyEditWnd = fPrcssDsplyEditWnd;
              pch = pPRF_PRCSSDSPLYEDITWND;
            }
         break;
#if 0
      case ViewDscrpFeat:
         fViewDscrpFeat = flag;
         if (fSyncWrite && fViewDscrpFeat != fPViewDscrpFeat )
            { fPViewDscrpFeat = fViewDscrpFeat ;
              pch = pPRF_VIEWDSCRPFEAT;
            }
         break;
#endif
      case ViewDscrpFeatInfo:
         fViewDscrpFeatInfo = flag;
         if (fSyncWrite && fViewDscrpFeatInfo != fPViewDscrpFeatInfo )
            { fPViewDscrpFeatInfo = fViewDscrpFeatInfo ;
              pch = pPRF_VIEWDSCRPFEATINFO;
            }
         break;
      case ViewRslvdFeat:
         fViewRslvdFeat = flag;
         if (fSyncWrite && fViewRslvdFeat != fPViewRslvdFeat )
            { fPViewRslvdFeat = fViewRslvdFeat ;
              pch = pPRF_VIEWRSLVDFEAT;
            }
         break;
      case ViewRslvdFeatInfo:
         fViewRslvdFeatInfo = flag;
         if (fSyncWrite && fViewRslvdFeatInfo != fPViewRslvdFeatInfo )
            { fPViewRslvdFeatInfo = fViewRslvdFeatInfo ;
              pch = pPRF_VIEWRSLVDFEATINFO;
            }
         break;
      case ViewEditWndInfo:
         fViewEditWndInfo = flag;
         if (fSyncWrite && fViewEditWndInfo != fPViewEditWndInfo )
            { fPViewEditWndInfo = fViewEditWndInfo ;
              pch = pPRF_VIEWEDITWNDINFO;
            }
         break;
      case ViewPrflWndInfo:
         fViewPrflWndInfo = flag;
         if (fSyncWrite && fViewPrflWndInfo != fPViewPrflWndInfo )
            { fPViewPrflWndInfo = fViewPrflWndInfo ;
              pch = pPRF_VIEWPRFLWNDINFO;
            }
         break;
      case ViewGenRPO:
         fViewGenRPO = flag;
         if (fSyncWrite && fViewGenRPO != fPViewGenRPO )
            { fPViewGenRPO = fViewGenRPO ;
              pch = pPRF_VIEWGENRPO;
            }
         break;
      case ViewGenRPOInfo:
         fViewGenRPOInfo = flag;
         if (fSyncWrite && fViewGenRPOInfo != fPViewGenRPOInfo )
            { fPViewGenRPOInfo = fViewGenRPOInfo ;
              pch = pPRF_VIEWGENRPOINFO;
            }
         break;
      default: fOK = false;
    }
 if (pch)
   { try
       { pProfile->addOrReplaceElementWithKey(pch,flag);}
     catch (IException& exc)
       { fOK = false;}
   }
 return fOK;
}

Boolean EUOPProfile :: SetStringKey(StringKey Key, const IString& str)
{
 Boolean fOK = true;
 const char *pch = NULL;
 switch(Key)
   {
     case WorkDir:
         strWorkDir = str;
         if (fSyncWrite)
           pch = pPRF_WORKDIR;
         break;
     case ProductKnowledgeFile:
         strPKFile = str;
//         if (fSyncWrite) // not part of profile window - write to ini file
           pch = pPRF_PKFILE;
         break;
     case UserName:
         strUserName = str;
//         if (fSyncWrite) // not part of profile window - write to ini file
           pch = pPRF_USER_NAME;
         break;
     case UserTieLine:
         strUserTieLine = str;
//         if (fSyncWrite) // not part of profile window - write to ini file
           pch = pPRF_USER_TL;
         break;
     case UserVM_Id:
         strUserVM_Id = str;
//         if (fSyncWrite) // not part of profile window - write to ini file
           pch = pPRF_USER_VM_ID;
         break;
     case UserVM_Node:
         strUserVM_Node = str;
//         if (fSyncWrite) // not part of profile window - write to ini file
           pch = pPRF_USER_VM_NODE;
         break;
      default: fOK = false;
    }
 if (pch)
   { try
       { pProfile->addOrReplaceElementWithKey(pch,str);}
     catch (IException& exc)
       { fOK = false;}
   }
 return fOK;
}

         /*------------------------------------------------------------------*\
         |     Boolean WriteProfile()                                         |
         |        Method writes the application keys to its                   |
         |        INI file:                                                   |
         |          Switch temporarily to synchronous mode and                |
         |             use 'SetBooleanKey' for every key.                     |
         |      Returns: false/true: (un)successful                           |
         \*------------------------------------------------------------------*/
Boolean EUOPProfile :: WriteProfile()
{
  Boolean fTemp = fSyncWrite;
  fSyncWrite = true;
  Boolean fRC = SetBooleanKey(DsplyAll      ,fDsplyAll           );
  if (fRC) SetBooleanKey(DsplyDscrp         ,fDsplyDscrp         );
  if (fRC) SetBooleanKey(PrcssGenRPO        ,fPrcssGenRPO        );
  if (fRC) SetBooleanKey(PrcssGenRPOList    ,fPrcssGenRPOList    );
  if (fRC) SetBooleanKey(PrcssGenRPOAuto    ,fPrcssGenRPOAuto    );
  if (fRC) SetBooleanKey(PrcssApplyRslvdFeat,fPrcssApplyRslvdFeat);
  if (fRC) SetBooleanKey(PrcssDsplyEditWnd  ,fPrcssDsplyEditWnd  );
  if (fRC) SetBooleanKey(ViewDscrpFeat      ,fViewDscrpFeat     );
  if (fRC) SetBooleanKey(ViewDscrpFeatInfo  ,fViewDscrpFeatInfo );
  if (fRC) SetBooleanKey(ViewRslvdFeat      ,fViewRslvdFeat     );
  if (fRC) SetBooleanKey(ViewRslvdFeatInfo  ,fViewRslvdFeatInfo );
  if (fRC) SetBooleanKey(ViewEditWndInfo    ,fViewEditWndInfo   );
  if (fRC) SetBooleanKey(ViewPrflWndInfo    ,fViewPrflWndInfo   );
  if (fRC) SetBooleanKey(ViewGenRPO         ,fViewGenRPO        );
  if (fRC) SetBooleanKey(ViewGenRPOInfo     ,fViewGenRPOInfo    );
  SetStringKey(WorkDir,strWorkDir);
  SetStringKey(UserName,strUserName);
  SetStringKey(ProductKnowledgeFile,strPKFile);
  SetStringKey(UserTieLine,strUserTieLine);
  SetStringKey(UserVM_Id,strUserVM_Id);
  SetStringKey(UserVM_Node,strUserVM_Node);

  fSyncWrite = fTemp;
  return fRC;
}

         /*------------------------------------------------------------------*\
         |    IString & FormatNLSDateString( unsigned short usM               |
         |                                  ,unsigned short usD               |
         |                                  ,unsigned short usY)              |
         |        Method returns the given month, day and year                |
         |        in a user-preferred format using the System's               |
         |        Country Setup date format, seperator and our                |
         |        work IString.                                               |
         \*------------------------------------------------------------------*/
IString & EUOPProfile :: FormatNLSDateString( unsigned short usM
                                             ,unsigned short usD
                                             ,unsigned short usY)
{
 IString str2,str3;
 switch(GetIntKey(DateFormat))
   { case DateMMDDYY: strBuff=usM; str2=usD; str3=usY; break;
     case DateDDMMYY: strBuff=usD; str2=usM; str3=usY; break;
     case DateYYMMDD: strBuff=usY; str2=usM; str3=usD; break;
   }
 strBuff += strDateSeparator + str2 + strDateSeparator + str3;
 return strBuff;
}

IString & EUOPProfile :: strftimeNLSFormattingString(char cY)
{
 IString str2,str3,strY = IString('%') + IString(cY);
 switch(GetIntKey(DateFormat))
   { case DateMMDDYY: strBuff="%m"; str2="%d"; str3=strY; break;
     case DateDDMMYY: strBuff="%d"; str2="%m"; str3=strY; break;
     case DateYYMMDD: strBuff=strY; str2="%m"; str3="%d"; break;
   }
 strBuff += strDateSeparator + str2 + strDateSeparator + str3;
 return strBuff;
}

         /*------------------------------------------------------------------*\
         |    IString & TodayNLSDateString()                                  |
         |        Method returns today's date in a                            |
         |        user-preferred format using the System's                    |
         |        Country Setup date format, seperator                        |
         |        and our work IString.                                       |
         \*------------------------------------------------------------------*/
IString & EUOPProfile :: TodayNLSDateString()
{  /*jt
 DATETIME  DT;
 DosGetDateTime(&DT);
 FormatNLSDateString(DT.month,DT.day,DT.year); jt*/
 return strBuff;
}

         /*------------------------------------------------------------------*\
         |     IString & FormatNLSTimeString( unsigned short usH              |
         |                                   ,unsigned short usM              |
         |                                   ,unsigned short usS)             |
         |        Method returns the given hours, minutes and                 |
         |        seconds in a user-preferred format using the                |
         |        System's Country Setup time format, seperator               |
         |        and our work IString.                                       |
         \*------------------------------------------------------------------*/
IString & EUOPProfile :: FormatNLSTimeString( unsigned short usH
                                             ,unsigned short usM
                                             ,unsigned short usS)
{
 Boolean f12Hours = (GetIntKey(TimeFormat) == Time12Hours);
 strBuff=IString((usH > 12 && f12Hours)? usH-12: usH);
 strBuff += strTimeSeparator + IString(usM) + strTimeSeparator + IString(usS);
 if (f12Hours) strBuff += " " + ((usH > 12)? strTimePM : strTimeAM);
 return strBuff;
}

         /*------------------------------------------------------------------*\
         |    IString & NowNLSTimeString()                                    |
         |        Method returns the current time in a                        |
         |        user-preferred format using the System's                    |
         |        Country Setup time format, seperator                        |
         |        and our work IString.                                       |
         \*------------------------------------------------------------------*/
IString & EUOPProfile :: NowNLSTimeString()
{                  /*jt
 DATETIME  DT;
 DosGetDateTime(&DT);
 FormatNLSTimeString(DT.hours,DT.minutes,DT.seconds);   jt*/
 return strBuff;
}

IString & EUOPProfile :: HelpFile()
{
 return (CurrentHelpFile().size()? CurrentHelpFile() : DefaultHelpFile());
}

IString & EUOPProfile :: MessageFile()
{
 return (CurrentMessageFile().size()?
                       CurrentMessageFile() : DefaultMessageFile());
}

IString & EUOPProfile :: ResourceFile()
{
 return (CurrentResourceFile().size()?
                CurrentResourceFile() : DefaultResourceFile());
}




