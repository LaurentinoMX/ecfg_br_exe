#ifndef _AB_PRF_HPP_
#define _AB_PRF_HPP_
 /*--------------------------------------------------------------------------*\
 |    File Name  : MYTRKHDR.HPP                                               |
 |                                                                            |
 |    Description:                                                            |
 |      This file contains the declaration(s) of the following                |
 |      class implementation(s):                                              |
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
 |                                                                            |
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

        /*---------------------------------------------------*\
        |                  IBMClass includes                  |
        \*---------------------------------------------------*/
#include <iexcbase.hpp>
#include <iprofile.hpp>
#include <iapp.hpp>

#pragma pack(4)
class EUOPProfile
{
    IProfile           *  pProfile;
    Boolean  fDDsplyAll,            fDsplyAll,            fPDsplyAll,
             fDDsplyDscrp,          fDsplyDscrp,          fPDsplyDscrp,
             fDPrcssGenRPO,         fPrcssGenRPO,         fPPrcssGenRPO,
             fDPrcssGenRPOList,     fPrcssGenRPOList,     fPPrcssGenRPOList,
             fDPrcssGenRPOAuto,     fPrcssGenRPOAuto,     fPPrcssGenRPOAuto,
             fDPrcssApplyRslvdFeat, fPrcssApplyRslvdFeat, fPPrcssApplyRslvdFeat,
             fDPrcssDsplyEditWnd,   fPrcssDsplyEditWnd,   fPPrcssDsplyEditWnd,
             fDViewDscrpFeat,       fViewDscrpFeat,       fPViewDscrpFeat,
             fDViewDscrpFeatInfo,   fViewDscrpFeatInfo,   fPViewDscrpFeatInfo,
             fDViewRslvdFeat,       fViewRslvdFeat,       fPViewRslvdFeat,
             fDViewRslvdFeatInfo,   fViewRslvdFeatInfo,   fPViewRslvdFeatInfo,
             fDViewGenRPO,          fViewGenRPO,          fPViewGenRPO,
             fDViewGenRPOInfo,      fViewGenRPOInfo,      fPViewGenRPOInfo,
             fDViewEditWndInfo,     fViewEditWndInfo,     fPViewEditWndInfo,
             fDViewPrflWndInfo,     fViewPrflWndInfo,     fPViewPrflWndInfo,
             fSyncWrite,
             fProfileRead;
    IString  strCurrDsplyDrvr;
    IString  strDateSeparator;
    IString  strTimeSeparator;
    IString  strTimeAM;
    IString  strTimePM;
    IString  strNone;
    IString  strDefaultResource;
    IString  strCurrentResource;
    IString  strDefaultHelp;
    IString  strCurrentHelp;
    IString  strDefaultMsgF;
    IString  strCurrentMsgF;
    IString  strWorkDir;
    IString  strDefaultWorkDir;
    IString  strUserName;
    IString  strUserTieLine;
    IString  strUserVM_Id;
    IString  strUserVM_Node;
    IString  strPKFile;
    IString  strBuff;
    unsigned long CPList[4];
    IString  AstrMonths[12];
    unsigned short GetCodePages();
    unsigned short GetCountryInfo();
    void LoadMonths();
    unsigned       LongestMonthString();
  public:
    EUOPProfile(Boolean fSync = false);
    ~EUOPProfile();
    static IString strExecPath;
    enum BooleanKey
      {
         DsplyAll
        ,DsplyDscrp
        ,PrcssGenRPO
        ,PrcssGenRPOList
        ,PrcssGenRPOAuto
        ,PrcssApplyRslvdFeat
        ,PrcssDsplyEditWnd
        ,ViewDscrpFeat
        ,ViewDscrpFeatInfo
        ,ViewRslvdFeat
        ,ViewRslvdFeatInfo
        ,ViewGenRPO
        ,ViewGenRPOInfo
        ,ViewEditWndInfo
        ,ViewPrflWndInfo
      };
    enum StringKey
      {
         CurrDsplyDrvr
        ,DateSeparator
        ,TimeSeparator
        ,TimeAM
        ,TimePM
        ,WorkDir
        ,UserName
        ,UserTieLine
        ,UserVM_Id
        ,UserVM_Node
        ,ProductKnowledgeFile
      };
    enum IntKey
      {
         CntryCode
        ,DateFormat
        ,TimeFormat
        ,CodePageCurrent
        ,CodePagePrime
        ,CodePage2nd
      };
    enum Constansts
      {
                                                 // these 3 must be first in
         DateMMDDYY                              // this order
        ,DateDDMMYY
        ,DateYYMMDD
        ,Time12Hours
        ,Time24Hours
        ,NotSupported = 99990
        ,NotCodePageSupported
        ,NotThere
      };
    Boolean   ReadProfile(),
              WriteProfile(),
              SetResources( ICurrentApplication & App
                            ,unsigned short usArg = 0),
              GetBooleanKey(BooleanKey Key, Boolean fDefault = false),
              SetBooleanKey(BooleanKey Key,Boolean),
              SetStringKey(StringKey Key, const IString& str);
    Boolean   SyncWrite(){return fSyncWrite;}
    IString & CurrentHelpFile() { return strCurrentHelp;}
    IString & CurrentResourceFile() { return strCurrentResource;}
    IString & CurrentMessageFile() { return strCurrentMsgF;}
    int       GetIntKey(IntKey),
              SpecificResourceFile(IString *,char *),
              SpecificHelpFile(IString *,char *);
              SpecificMessageFile(IString *,char *);
    void      SetCurrentResourceFile(char *psz) { strCurrentResource = psz;}
    void      SetCurrentHelpFile(char *psz)  { strCurrentHelp = psz;}
    void      SetCurrentMessageFile(char *psz) {strCurrentMsgF = psz;}

    //* 79548 Begins New logic:  Adds to the private member strPKFile wich contains
    //* the Path of the feature.$$f file, the Product File user preference, to load
    //* this file from the directory that specifies the Product File profile variable

    //* F81710 Begin new logic: Per the feature requirement next is the logic used
    //* to create the path of the feature.$$f file to load it:
    //* a) Use the parsed parameter from eConfig to set the directory into the PK file
    //*    path. If doesn't exist, use the product family profile preference.
    //* b) If strProdFamily receives any valid value, (from a parameter from eConfig
    //*    or from the profile) the Product Knowldge filepath will be changed
    //* c) If it is empty, the parsed parameter is not a server model, or the product
    //*    family preference doesn't exist, which will assign blanks to the PK file path,
    //*    and baseret will not do any specific product knowledge file-based processing.

    void      ChangePKFilePath(IString strExecPath, IString strProdFamily)
                              { //* strPKFile = strExecPath + strProdFamily +
                                //*            STR_PathDelimiter + STR_PKTable;
                                if (strProdFamily != "")
                                   strPKFile = strExecPath + strProdFamily +
                                               STR_PathDelimiter + STR_PKTable;
                                else
                                   strPKFile = "";

                              }
    //* F81710  Ends new logic:   HECC - 06/03/2003

    //* 79548 Ends new logic:   HECC - 04/11/2003

    IString & DefaultResourceFile(),
            & DefaultHelpFile(),
            & DefaultMessageFile(),
            & HelpFile(),
            & ResourceFile(),
            & MessageFile(),
            & MonthString(unsigned short),
            & TodayNLSDateString(),
            & strftimeNLSFormattingString(char cYear = 'Y'),
            & NowNLSTimeString(),
            & GetStringKey(StringKey, Boolean fDefault = false),
            & FormatNLSDateString( unsigned short usMonth
                                  ,unsigned short usDay
                                  ,unsigned short usYear),
            & FormatNLSTimeString( unsigned short usHours
                                  ,unsigned short usMinutes
                                  ,unsigned short usSeconds);
    unsigned short GetMonthNmbr(IString &);
    char  ** AllocateMonthArray();
};
#pragma pack()

#endif

