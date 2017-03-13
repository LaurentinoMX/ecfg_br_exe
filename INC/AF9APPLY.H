/* =======================================================================
 * FILE NAME         AF9APPLY.H
 *
 * MODIFICATION LOG:
 * -----------------------------------------------------------------------
 * Date        Programmer        Description
 * =======================================================================
 * 2002-02-26  DCG    68568      Added definition of symbols:
 *                               IDS_ErrorRetrievalPath_Title,
 *                               IDS_ErrorRetrievalPath_Textwhich and
 *                               IDC_ProfileDialog_SelFirstPage.
 * 2002-03-08  DCG    68569      Defined two new commands for the
 *                               ProfileDialog command member in order to
 *                               pass focus to the window of that class
 * 2002-06-27  HECC   72273      Added definition of name of the new iSeries
 *                               SW feature table file (ISERIESW.TBL) and
 *                               symbol  IDS_ErrorLoadSWTableInput_Text
 *                               used to report any error when this table is
 *                               loaded.
 * 2002-08-28  HECC   73686      Added definition of the Order Forward executable.
 *                               Definitions of the base Section (012) and Order
 *                               sections (014) and RT 99 to create the new .CFR file
 *                               that will contain the valid items in the RPO list.
 *
 * 2003-04-11  HECC   79548      Added definition of name of the feature.$$f file
 *                               used to change the path of the directory where this
 *                               file is located as specified in the Product Family
 *                               profile preference.
 *                               Added new string ID's to display error messages in
 *                               case that a given Product Family directory where
 *                               the .TBL or .$$F files will be picked up, doesn't
 *                               exist.
 *
 * 2003-06-03  HECC   81710      Added definitions of strings of the parsed parameters
 *                               from econfig to Base Retrieval to indentify which of
 *                               these are model servers. (AS400, RS6K, S390 and S390
 *                               used parts)
 * 2003-10-07  PSC    D80040     New variables defined to handle RM2 Registry Entries.
 *
 * 2004-03-12  HECC   F87612     Added SQUADRON string identifier to support it in Base Ret
 * 2004-03-25  HECC   F86750     Added new definitions to validate the services RT 38 ids
 * 2004-04-02  HECC   F87612A    Change on the name of SQUADRON to be added to the
 *                               Product Family of Base Ret, from "PowerSeries Server" to
 *                               "POWER Systems"
 * 2004-09-21  HECC   D94556     Added Slash character to validate it when it is typed from
 *                               user.  Added also definition of symbols:
 *                                            IDS_ChangedRetrievalPath_Title
 *                                            IDS_ChangedRetrievalPath_Text
 * 2004-10-20  ZOR    D95887     add STR_53RecType
 * 2005-02-09  RERC   D97759     Added STR_RootDirectory_WOPD, STR_ThreeBackSlash characters
 *                               to validate them when someone is typed from user
 * 2005-06-30  HECC   F101422    Per customer requirement, implementation of Brand name
 *                               change for S390 to  Show System z9, zSeries and S/390
 *                               Servers mismatches.
 * 2005-07-26  HECC   F100160    Add the necessary IDs for the messages to be
 *                               displayed and the strings to be checked if the loaded
 *                               configuration belongs to Blue Horizon or xSeries family.
 * 2005-09-01  RERC   F102585    Change on the name of SQUADRON to be added to the
 *                               Product Family of Base Ret FROM "POWER Systems"
 *                               to "POWER5 based Systems"
 * 2006-01-30  RERC   F104628    Added STR_ACCEPTED and STR_UNACCEPTED to validate the
 *                               the Mismatch Type of the nocomp.tbl
 * 2006-02-21  RERC   D106441    Added strings to write text in .RET file about mismatches
 * 2006-04-24  RERC   F107280    Mismatch Output report and *.RET file change the text for
 *                               the "ACCEPTABLE" features.
 * 2006-05-29  HECC   F108502    Added logic to disable the "VM Host" (Ub9host1.exe) and
 *                               "Inventory Services" (Ub9host4.exe) options for EMEA
 *                               geography users.
 * 2006-06-05  RERC   F106765    Constants defined to disable the Ub9host1.exe option
 *                    Req1436    and write Ub9host4.exe option for "US" geography users.
 *
 * 2008-04-01  AMB    F122337    Change the name used for the zSeries model from "System z9,
 *                    Req1501    zSeries and S/390 Servers" to "System z (TM) and S/390 Servers"
 *                               in every location where it currently appears as "System z9,
 *                               zSeries and S/390 Servers".
 * 2008-07-16  SAQP   F124386    Change the name used for the POWER5 model from "POWER5 based
 *                               Systems" to "POWER5, POWER6 and BladeCenter JS2x" in every
 *                               location where it currently appears as "POWER5 based Systems"
 *
 * 2009-12-08  ELRR   F133122    Added definition of symbols for the BR support for Firefox:
 *                               RegKey_Firefox
 *                               STR_Profile_Firefox
 *                               BROWSER_Firefox
 *                               IDC_RB_Firefox
 *
 * 2010-04-09  ELRR   F134695    Change the legend used for the POWER models from "POWER5, POWER6 and BladeCenter JS2x"
 *                               to "POWER Systems (rack/tower and blades), and GTS / ISS " in every
 *                               location where it currently appears.
 *
 * 25/06/2014  GTL    Req1702    Made definitions to accommodate Chrome as a supported browser.
 *
 *
 *
 * =======================================================================
 * Copyright (c) International Business Machines Corp. 2002
 * All rights reserved
 * =======================================================================
 */


#ifndef AF9APPLY_H
#define DIALOG_1 1
#define AF9APPLY_H

#define LOADSTRING(string)              IApplication::current().userResourceLibrary().loadString(string)

#define STR_Version                     "2.01"
#define STR_ProfileVersion              "1.2"
#define STR_Prefix                      "BF9"
#define STR_LocaleSeparator             "_"
#define SEPARATOR_OFFSET                2
#define DLL_LENGTH                      8
#define STR_DummyFile                   "\\DUMMY###.XYZ"
#define STR_PathMRI                     "LOCALE\\"
#define CHR_PathTerminator              ';'
#define STR_PathDelimiter               "\\"
#define STR_ExtensionDLL                ".DLL"
#define STR_ExtensionHelp               ".HLP"
#define STR_ExtensionLocale             ".LCL"
#define STR_PrefixLocale                "\\IBM-"
#define STR_FormatDate                  "%x"
#define STR_FormatTime                  "%X"
#define STR_DefaultLocale               "C"
#define FORMAT_LENGTH                   32
#define WINDOWMAIN                      0
#define WINDOWORDERS                    1
#define WINDOWSYSTEMS                   2
#define WINDOWDETAIL                    3
#define WINDOWCOUNT                     WINDOWDETAIL + 1
#define ORDERSNEW                       0
#define ORDERSMES                       1
#define SYSTEMSUNASSIGNED               0
#define STR_ErrorLocaleTitle            "Error setting locale"
#define STR_ErrorLocaleText             "No locale files are available.  Base Retrieval cannot continue."
#define STR_ErrorMRITitle               "Error loading resources"
#define STR_ErrorMRIText                "The resource file %0 is unusable.  The Base Retrieval CPF cannot continue."
#define STR_OpenProfile                 "Load Profile"
#define STR_PathLabelText               "Retrieval Path:     "
#define STR_NoSensedDateRetrieved       "UNKNOWN"
#define STR_WildCard                    "*"
#define STR_Profile_INI                 "BaseRetrieval"
#define STR_Passport_Key                "Installation Directory"
#define STR_Profile_App                 "BaseRet"
#define STR_Profile_Key                 "Save"
#define STR_ProfileVersion_Key          "ProfileVersion"
#define STR_ExtensionCFR                ".CFR"
#define STR_ExtensionMRD                ".MRD"
#define STR_ExtensionMRR                ".MRR"               //42619 flw
#define STR_ExtensionADM                ".ADM"               //42619 flw
#define STR_PathDelimeter               ";"                  //42619 flw
#define STR_ExtensionDSC                ".DSC"
#define STR_ExtensionRET                ".RET"
#define STR_ExtensionBIR                ".BIR"
#define STR_ExtensionBYZ                ".BYZ"               //52012-MAMT-06/27/2000
#define STR_ExtensionCYZ                ".CYZ"               //52012-MAMT-06/27/2000
#define STR_ExtensionEXE                ".EXE"
#define STR_ExtensionBII                "bii"                 //edw12/16/97
#define STR_ExtensionCNV                "cnv"                 //edw12/16/97
#define STR_RetrievalFile               "LASTRET"
#define STR_CpuTable                    "cpulook.tbl"
#define STR_NoCompTable                 "NOCOMP.TBL"          //edw07/07/1998
#define STR_QuickFixTable               "QUICKFIX.TBL"        //49222-MAMT-06/27/2000
#define STR_ISeriesSWTable              "ISERIESW.TBL"        //HECC 27/06/2002   F72273
#define STR_PKTable                     "feature.$$f"         //HECC 04/11/2003   F79548

#define STR_Slash                       "/"                   //D94556 HECC - 09/21/2004
#define STR_RootDirectory               "C:\\"                //D94556 HECC - 09/21/2004

#define STR_RootDirectory_WOPD          "C:"                  //D97759 RERC - 02/09/2005
#define STR_ThreeBackSlash               "\\\\\\"             //D97759 RERC - 02/09/2005
#define MIN_DIR_LEN                     2                     //D97759 RERC - 02/09/2005
#define STR_Blank2                      ""                    //Usabilty
#define STR_Blank1                      " "
#define CHR_Blank                       ' '
#define CHR_Underscore                  '_'
#define STR_Hyphen                      "-"
#define STR_DescDelimiter               "\x0B"
#define STR_Substitute0                 "%0"
#define STR_Substitute1                 "%1"
#define STR_Substitute2                 "%2"
#define STR_GroupMES                    "%0(%1:%2)"
#define STR_ProfileDelimiter            "\n"
#define STR_FileUnusable                "???"
#define STR_TypeCFREPORT                "CFREPORT"
#define STR_TypeCFR                     "CFR"
#define STR_TypeMRD                     "MRD"
#define STR_TypeMRR                     "MRR"                 //flw 42619
#define STR_TypeDSC                     "DSC"
#define STR_TypeMSG                     "MSG"                 //53915 MAMT 02/02/2001
#define STR_TypeRET                     "RET"                 //53915 MAMT 02/02/2001
#define STR_ExtensionCFT                ".CFT"                //53915 MAMT 02/02/2001 temporal file
#define STR_ExtensionMRT                ".MRT"                //53915 MAMT 02/02/2001 temporal file
#define STR_CopyCommand                 "COPY"
#define STR_ConfigApp                   "SBOS2"
#define STR_ConfigKey                   "RetFile"
#define STR_VPDAidSystemName            "VPDTRAN CFSYSTEM"    //edw05/29/1998
#define STR_V20Header                   "0020"                //edw05/29/1998
#define STR_V20Rec53Header              "53"                  //edw05/29/1998
#define STR_Rec08Qtys                   "       0000  "       //edw05/29/1998
#define STR_NoMatches                   " NO AAS Matches"     //edw06/05/1998
#define STR_VPDTRAN                     "VPDTRAN"             //edw06/05/1998
#define STR_VMHost                      "UB9HOST1.EXE"        //edw09/02/1998
#define STR_HostInterfaces              "UB9HOST*.EXE"        //edw09/02/1998
#define STR_LocalFiles                  "Local Files"         //edw09/02/1998
#define STR_NoDescription               " <No Description> "  //edw09/02/1998
#define STR_HW                          "H"                   //edw09/02/1998
#define STR_SW                          "S"                   //edw09/02/1998
#define STR_HWSW                        "B"                   //edw09/02/1998
#define STR_OrderForward                "FF9FRWRD.EXE"        //F73686 HECC: 09/09/2002

#define ECONFIG                         "E-CONFIG"                           // Usability

//ELRR F133122-Beging   We disabled the netscape Regitry and define the registry for Firefox
//#define RegKey_Netscape                 "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\netscape.exe"
//#define RegKey_Netscape6                "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\netscp6.exe"     //D67437 iak
#define RegKey_Firefox                  "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\Firefox.exe"
//ELRR F133122-End

//GTL Req1702 BEGIN 
#define RegKey_Chrome                  "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\Chrome.exe"
//GTL Req1702 END

#define RegKey_IExplorer                "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\iexplore.exe"
#define RegKey_PCOMM                    "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\PCSWS.EXE"
#define RegKey_PCOMM_path               "Path"                                // ZOR 69791
#define RegKey_PCOMM_short              "PCSWS.EXE"                           // ZOR 69791
#define RegKey_Passport                 "SOFTWARE\\IBM Global Network\\Passport\\PasW95"
#define RegKey_Passport2000             "SOFTWARE\\AT&T Global Network\\Passport\\PasW95"
//51747 - MAMT 09/06/00
#define STR_Profile_RetSource           "RetSource"                         // Usability
#define STR_RetProd_HW                  "0"                                 // Usability
#define STR_RetProd_SW                  "1"                                 // Usability
#define STR_RetProd_Both                "2"                                 // Usability
#define STR_Profile_RetProd_Types       "RetProdTypes"                      // Usability
#define STR_Profile_FastPath            "Fastpath"                          // Usability
#define STR_FastPath_On                 "1"                                 // Usability
#define STR_FastPath_Off                "0"                                 // Usability
#define STR_ViewSort                    "Sort"                              // Usability  ZOR IC01552
#define STR_ViewSortName                "1"                                 // Usability  ZOR IC01552
#define STR_ViewSortDate                "0"                                 // Usability  ZOR IC01552
#define STR_ViewSortAscending           "Ascend"                            // Usability  ZOR IC01552
#define STR_ViewSortAscend              "1"                                 // Usability  ZOR IC01552
#define STR_ViewSortDescend             "0"                                 // Usability  ZOR IC01552
#define STR_Profile_Expanded            "Expanded"                          // Usability ZOR 48548
#define STR_Expanded_On                 "1"                                 // Usability ZOR 48548
#define STR_Expanded_Off                "0"                                 // Usability ZOR 48548
#define STR_Profile_Tile                "Tile"                              // Usability 47971 MAMT 28/08/00
#define STR_Tile_Hor                    "1"                                 // Usability 47971 MAMT 28/08/00
#define STR_Tile_Ver                    "0"                                 // Usability 47971 MAMT 28/08/00
#define STR_Placement_s390             "Placement390"                       // Usability 53915 MAMT 04/04/00
#define STR_Placement_Ys390             "1"                                 // Usability 53915 MAMT 04/04/00
#define STR_Placement_Ns390             "0"                                 // Usability 53915 MAMT 04/04/00
#define STR_MRPD_Yes                    "1"                                 // Usability
#define STR_MRPD_No                     "0"                                 // Usability
#define STR_Profile_MRPD                "MRPD"                              // Usability


//ELRR F133122-Begin: Firefox is defined with the value number "1" taking the Netscape value.

//#define STR_Profile_Netscape          "1"
#define STR_Profile_Firefox             "1"
//ELRR F133122-End

#define STR_Profile_IE                  "2"

// GTL Req1702 BEGIN
#define STR_Profile_Chrome              "3" 
// GTL Req1702 END

#define STR_Profile_NoBrowser           "0"

//ELRR F133122-Begin Firefox Browser value is defined as "0".
//#define BROWSER_NETSCAPE               0
#define BROWSER_Firefox                  0
//ELRR F133122-End

//GTL Req1702 BEGIN 
#define BROWSER_Chrome                   0
//GTL Req1702 END

#define BROWSER_IEXPLORER                1
#define STR_Profile_Browser             "RetBrowser"
#define STR_Profile_AutoRec             "AutoRec"                           //47698
#define STR_AutoRec_On                  "1"                                 //47698
#define STR_AutoRec_Off                 "0"                                 //47698
#define STR_Profile_ProcRPO             "ProcRPO"                           //47700
#define STR_ProcRPO_On                  "1"                                 //47700
#define STR_ProcRPO_Off                 "0"                                 //47700
#define STR_Profile_Family              "Family"                            //47701
#define STR_Key_RefreshList             "RefreshList"                       // F62721 EDW 01/21/2002
#define STR_Key_RefreshLocale           "RefreshLocale"                     // F62721 EDW 01/21/2002

//D106441 RERC 02/21/2006  BEGIN
#define LENGTH_START_TEXT               17      //Length to start the text of the RT05 to write in the RET file
#define LENGTH_TO_ALIGN                 16      //Length to align the text with the previous line in the RET file
#define STR_SpacesAlign                 "                                     " //Spaces to align the text

//Text for classification of the Mismatches Dialog for S390 product family
#define STR_REC05_TextUnacceptable      "These mismatches are critical and must be understood or resolved before proceeding."
//F107280 RERC 04/24/2006 BEGIN
//#define STR_REC05_TextAcceptable        "These mismatches are of less concern but should be understood before proceeding."
#define STR_REC05_TextAcceptable        "These mismatches are noncritical but should be understood before proceeding."
//F107280 RERC 04/24/2006 END

//Modify the text for the Reconcile data to one line
#define STR_REC05_MRDBQuit              "MRDB data greater than 30 days old - user chose to quit reconciliation"
#define STR_REC05_MismatchesContinue    "Mismatches exist - user chose to continue with default reconciliation values"
//D106441 RERC 02/21/2006 END

//edw12/29/99 Text of 05 records to indicate user's reconciliation decisions
//ZOR 07/19/00 IC65198 cut the comments until 55 characters long
#define STR_REC05_MRDBContinue          "MRDB data greater than 30 days old - user chose to"
#define STR_REC05_MRDBContinue2         "    continue with reconciliation"
//#define STR_REC05_MRDBQuit              "MRDB data greater than 30 days old - user chose to"
//#define STR_REC05_MRDBQuit2             "    quit reconciliation"
#define STR_REC05_ReconcileData         "AAS/MRDB reconciliation - user chose to reconcile"   //* hecc: 58629   04/04/2001
#define STR_REC05_ReconcileData2        "    the data"                                        //* hecc: 58629   04/04/2001
#define STR_REC05_ReconcileUseMRDB1     "AAS/MRDB reconciliation bypassed - user chose to use"
#define STR_REC05_ReconcileUseMRDB12    "    MRDB only"
#define STR_REC05_ReconcileUseAAS1      "AAS/MRDB reconciliation bypassed - user chose to use"
#define STR_REC05_ReconcileUseAAS12     "    AAS only"
#define STR_REC05_ReconcileUseMRDB2     "AAS/MRDB reconciliation started, but cancelled - user"
#define STR_REC05_ReconcileUseMRDB23    "    chose to use MRDB only"
#define STR_REC05_ReconcileUseAAS2      "AAS/MRDB reconciliation started, but cancelled - user"
#define STR_REC05_ReconcileUseAAS23     "    chose to use AAS only"
//#define STR_REC05_MismatchesContinue    "Mismatches exist - user chose to continue with default"
//#define STR_REC05_MismatchesContinue2   "    reconciliation values"
#define STR_REC05_MismatchesQuit        "Mismatches exist - user chose to quit reconciliation"

//#define NO_ERROR                        0
#define MAX_KEY_PATH                    512                                                      // F62721 EDW 01/21/2002
#define MAX_VALUE_NAME                  512                                                      // F62721 EDW 01/21/2002
#define STR_VALUE_DELETED               "1"                                                      // F62721 EDW 01/21/2002
#define STR_KEY_DELETED                 "Deleted"                                                // F62721 EDW 01/21/2002
#define REGPATH_OLD                     "SOFTWARE\\BASERETRIEVAL\\BASERET"                       // F62721 EDW 01/21/2002
#define REGPATH                         "SOFTWARE\\IBM\\econfig_profile\\BaseRetrieval\\BaseRet" // F62721 EDW 01/21/2002
#define STR_RegKey_RM2_OLD              "SOFTWARE\\BF9RM2"                                       // F62721 EDW 01/21/2002
#define STR_RegKey_RM2                  "SOFTWARE\\IBM\\econfig_profile\\BF9RM2"                 // F62721 EDW 01/21/2002
#define STR_RegKey_eConfig              "SOFTWARE\\IBM\\econfig_profile\\"                       // F62721 EDW 01/21/2002
#define STR_Key_ShowS390                "showS390"
// D80040 PSC 10/07/2003 start
// New variables are defined to handle the erroneous RM2 Registry Entries.
// This new variable contains the erreoneous path created at the time the IProfile class is instantiated
#define REGPATH_RM2_OLD                 "Software\\IBM/_econfig_profile/_BF9RM2"
// This one contains the right path where the RM2 Registry Entry must be created.
#define REGPATH_RM2_NEW                 "Software\\IBM\\econfig_profile\\BF9RM2"
// This one contains the final path where the RM2 Registry Entry is erroneously created.
#define REGPATH_RM2_DEL                 "Software\\IBM/_econfig_profile/_BF9RM2\\RM2Profile\\"
// All RM2 Keys
#define REGPATH_RM2_DEL_DSPLYALL               "DsplyAll"
#define REGPATH_RM2_DEL_DSPLYDSCRP             "DsplyDscrp"
#define REGPATH_RM2_DEL_PKFILE                 "PKFile"
#define REGPATH_RM2_DEL_PRCSSAPPLYRSLVDFEAT    "PrcssApplyRslvdFeat"
#define REGPATH_RM2_DEL_PRCSSDSPLYEDITWND      "PrcssDsplyEditWnd"
#define REGPATH_RM2_DEL_PRCSSGENRPO            "PrcssGenRPO"
#define REGPATH_RM2_DEL_PRCSSGENRPOAUTO        "PrcssGenRPOAuto"
#define REGPATH_RM2_DEL_PRCSSGENRPOLIST        "PrcssGenRPOList"
#define REGPATH_RM2_DEL_USERNAME               "User_Name"
#define REGPATH_RM2_DEL_USERTIELINE            "User_TieLine"
#define REGPATH_RM2_DEL_USERVMID               "User_VM_Id"
#define REGPATH_RM2_DEL_USERVMNMODE            "User_VM_Node"
#define REGPATH_RM2_DEL_VIEWDSCRPFEATINFO      "ViewDscrpFeatInfo"
#define REGPATH_RM2_DEL_VIEWEDITWNDINFO        "ViewEditWndInfo"
#define REGPATH_RM2_DEL_VIEWGENRPO             "ViewGenRPO"
#define REGPATH_RM2_DEL_VIEWGENRPOINFO         "ViewGenRPOInfo"
#define REGPATH_RM2_DEL_VIEWPRFLWNDINFO        "ViewPRFLWndInfo"
#define REGPATH_RM2_DEL_VIEWRSLVDFEAT          "ViewRslvdFeat"
#define REGPATH_RM2_DEL_VIEWRSLVDFEATINFO      "ViewRslvdFeatInfo"
#define REGPATH_RM2_DEL_WORKDIR                "WorkDir"
// D80040 PSC 10/07/2003 end

//* F108502 Begin logic, added constants to evaluate EMEA geography and their location in the registry
//* where "EM" is the expected registry string for all the EMEA's users
#define REGPATH_MAIN                    "SOFTWARE\\IBM\\econfig\\main"
#define STR_USERGEO                     "user_geography"
#define STR_EMEA                        "EM"
//* F108502 Ends logic,  HECC - 05/29/2006

#define STR_US                          "US"                            //REQ 1436 RERC - 05/12/2006
#define STR_NameVMHost                  "VM host"                       //REQ 1436 RERC - 05/12/2006
#define STR_IServices                   "UB9HOST4.EXE"                  //REQ 1436 RERC - 05/12/2006
#define STR_InventoryServices           "Inventory Services Host Interface" //REQ 1436 RERC - 05/12/2006

#define STR_SupportedAppVersion         "01.50"
#define STR_DummyRecType                "XX"
#define STR_00RecType                   "00"
#define STR_01RecType                   "01"
#define STR_03RecType                   "03"
#define STR_05RecType                   "05"
#define STR_07RecType                   "07"
#define STR_06RecType                   "06"    // ZOR  95997
#define STR_08RecType                   "08"
#define STR_19RecType                   "19"
#define STR_25RecType                   "25"
#define STR_38RecType                   "38"    //* F86750  HECC: 01/15/2004
#define STR_47RecType                   "47"
#define STR_49RecType                   "49"
#define STR_50RecType                   "50"
#define STR_53RecType                   "53"    // ZOR 95887
#define STR_54RecType                   "54"
#define STR_55RecType                   "55"
#define STR_56RecType                   "56"
#define STR_95RecType                   "95"
#define STR_96RecType                   "96"
#define STR_98RecType                   "98"
#define STR_99RecType                   "99"    //* F73686  HECC: 09/04/2002
#define STR_AaRecType                   "AA"
#define STR_AbRecType                   "AB"
#define STR_AcRecType                   "AC"
#define STR_01RecContent                "012"
#define STR_MesRecTypes                 "19 50"
#define STR_54SubTypes                  "55 56"
#define STR_54AndSubTypes               "54 55 56"
#define STR_CtlRecTypes                 "AA AB AC"
#define STR_OrderRecTypes               "08 47"

//* F86750 Begin logic: RT 38 added as a new valid product record id
//* #define STR_ProductRecTypes             "08 25 47 49"
#define STR_ProductRecTypes             "08 25 38 47 49"
//* F86750 End logic: HECC - 01/15/2004

#define STR_HWRecTypes                  "08 19 25 26"

//* F86750 Begin logic: RT 38 added as a new valid record id
//* #define STR_ValidRecIds                 "AA AB AC 00 01 03 05 07 08 19 25 47 49 50 54 55 56 95 96 98"
//* #define STR_ValidRecTypes               "AA AB AC 00 01 03 05 07 08 19 25 47 49 50 54 55 56 95 96 98"

// ZOR 95887
// #define STR_ValidRecIds                 "AA AB AC 00 01 03 05 07 08 19 25 38 47 49 50 54 55 56 95 96 98"
// #define STR_ValidRecTypes               "AA AB AC 00 01 03 05 07 08 19 25 38 47 49 50 54 55 56 95 96 98"
#define STR_ValidRecIds                 "AA AB AC 00 01 03 05 06 07 08 19 25 38 47 49 50 53 54 55 56 95 96 98"    // 95997
#define STR_ValidRecTypes               "AA AB AC 00 01 03 05 06 07 08 19 25 38 47 49 50 53 54 55 56 95 96 98"    // 95997
// ZOR 95887

//* F86750 End logic: HECC - 01/15/2004

#define STR_AllProductRecTypes          "08 19 25 26 47 48 49 50"
#define STR_VPDTranIds                  "A B C D E F G H I J K L M N O P Q R S T U V W X Y Z 1 2 3 4 5 6 7 8 9 0 a b c d e f g h i j k l m n o p q r s t u v w x y z"
#define STR_Rec01Text                   "012"
#define STR_Rec01BaseSection            "012"    //* F73686  HECC: 09/04/2002
#define STR_Rec01OrderSection           "014ORDER TRANSACTIONS"    //* F73686  HECC: 09/04/2002
#define STR_H                           "H"
//54035 MAMT 09/14/2000 will separate the variables that we use in the panel vs the
//                      variables that we use in the logic of the program
#define STR_S390                        "S390"
#define STR_AS400                       "AS400"
#define STR_RS6000                      "RS6000"
#define STR_RS6K                        "RS6K"   //HECC 05/08/2003   F79548

//F122337 Begin logic: Change name of zSeries model to System z (TM) and S/390 Servers
//F101422 Begin logic: Per customer requirement, implementation of Brand name change for S390
//* #define STR_S390N                       "zSeries and S/390"    //ZOR F 54792
//* #define STR_S390N                       "System z9, zSeries and S/390 Servers"
#define STR_S390N                       "System z (TM) and S/390 Servers"
//F101422 End logic:   HECC - 06/30/2005
//F122337 End logic:   AMB - 04/01/2008

#define STR_AS400N                      "iSeries and AS/400"   //ZOR F 54792
#define STR_RS6000N                     "pSeries and RS/6000"  //ZOR F 54792
#define STR_SQUAD                       "SQUAD"                //HECC F87612

//*F102585 Begin logic: Change on the name of SQUADRON to the Product Family ("POWER5 based Systems")
//* F87612A Begin logic:  Change on the name of SQUADRON to be added to the Product Family of Base Ret
//*#define STR_SQUADN                      "PowerSeries Server"   //HECC F87612
//#define STR_SQUADN                      "POWER Systems"        //HECC F87612A
//* F87612A End logic:  HECC - 04/02/2004
//#define STR_SQUADN                      "POWER5 based Systems"    //RERC F102585
//*F102585 End logic:   RERC - 09/05/2005

//F134395  Begin logic: ELRR - 04/09/2010
//#define STR_SQUADN                       "POWER5, POWER6 and BladeCenter JS2x " // SAQP F124386
//*F124386 End logic:   SAQP - 07/16/2008

#define STR_SQUADN                       "POWER Systems (rack/tower and blades), and GTS / ISS" // ELRR F134695
//*F134695 End logic:   ELRR - 04/09/2010

#define STR_UNKNOWN                     "UNKNOWN"
#define STR_CreateRPOReport_Text        "Creating RPO File to be Uploaded"
#define STR_Id                          "ID"

#define STR_AS400_Econfig               "AS400_SYSTEMS_VERSION_02"  //HECC 06/03/2003  F81710
#define STR_RS6000_Econfig              "RS6000_SYSTEMS_VERSION_02" //HECC 06/03/2003  F81710
#define STR_S390_Econfig                "S390_SYSTEMS_VERSION_02"   //HECC 06/03/2003  F81710
#define STR_S390IGF_Econfig             "S390_PPAC_SYSTEMS_VERSION_02"   //HECC 06/03/2003  F81710

#define STR_ACCEPTED                    "A" //RERC  30/01/2006  F104628
#define STR_UNACCEPTED                  "U" //RERC  30/01/2006  F104628

#define STR_BlueHorizon1                "BHWCFG"                    // F100160  HECC  07/26/2005
#define STR_BlueHorizon2                "BHNCFG"                    // F100160  HECC  07/26/2005
#define STR_XSeriesConf                 "ECMXS"                     // F100160  HECC  07/26/2005


#define UM_SelectFile                   WM_USER+1
#define UM_SaveFile                     WM_USER+2
#define UM_RenameFile                   WM_USER+3
#define UM_GenerateFile                 WM_USER+4
#define UM_SwitchLocale                 WM_USER+5
#define UM_StartUp1                     WM_USER+6
#define UM_StartUp2                     WM_USER+7
#define UM_SelectDate                   WM_USER+8
#define UM_ReturnRetrieval              WM_USER+9
#define UM_NameFile                     WM_USER+10            //edw 12/5/97
#define UM_NameFile1                    WM_USER+11            //52012 MAMT 28/07/2000


#define IDW_AFOAPPLY                    51
#define IDW_Orders                      52
#define IDW_Systems                     53
#define IDW_Detail                      54
#define WND_DETAIL                      55                       //47703 - flw

#define IDC_PB_OK                       61
#define IDC_PB_Cancel                   62
#define IDC_PB_Help                     63
#define IDC_PB_File_OK                  64
#define IDC_PB_File_Cancel              65
#define IDC_PB_Font_OK                  66
#define IDC_PB_Font_Cancel              67

#define IDI_Processor                   80
#define IDI_ApplyOrders                 81

#define IDM_Config                      100
#define IDMI_ConfigRetrieve             101
#define IDMI_ConfigBrowse               102
#define IDMI_ConfigISMProceed           103
#define IDMI_ConfigExit                 104                   //D50578 edw10/05/2000

#define IDM_Selected                    110
#define IDMI_SelectedOpen               111
#define IDMI_SelectedSave               112
#define IDMI_SelectedSaveAs             113
#define IDMI_SelectedGenerate           114
#define IDMI_SelectedRename             115
#define IDMI_SelectedDelete             116
#define IDMI_SelectedApply              117
#define IDMI_SelectedAssign             118
#define IDMI_SelectedRemove             119
#define IDMI_SelectedDetail             120
#define IDMI_SelectedOpen1              121 //52012 MAMT 08/01/2000
#define IDMI_CloseFile                  122 //52012 MAMT 08/01/2000

#define IDM_View                        130  //IC01552
#define IDMI_ViewSortName               131
#define IDMI_ViewSortNameA              132
#define IDMI_ViewSortNameD              133
#define IDMI_ViewSortProduct            134
#define IDMI_ViewSortBase               135
#define IDMI_ViewSortSerial             136
#define IDMI_ViewSortOrder              137
#define IDMI_ViewSortDate               138
#define IDMI_ViewSortDateA              139
#define IDMI_ViewSortDateD              183
#define IDMI_ViewSort                   184
#define IDMI_ViewSortA                  185
#define IDMI_ViewSortD                  186

#define IDM_Options                     140
#define IDMI_OptionsGenerate            141
#define IDMI_OptionsLocale              142
#define IDMI_OptionsRetrieval           143
#define IDMI_OptionsFastPath            144     //Usability
#define IDMI_OptionsFastPathOn          145
#define IDMI_OptionsFastPathOff         146
#define IDMI_OptionsView                147     // ZOR 48548
#define IDMI_Options_ExpandedOn         148     // ZOR 48548
#define IDMI_Options_ExpandedOff        149     // ZOR 48548
#define IDMI_OptionsTile                180     // 47971 MAMT 28/08/00
#define IDMI_Options_TileHor            181     // 47971 MAMT 28/08/00
#define IDMI_Options_TileVer            182     // 47971 MAMT 28/08/00
#define IDMI_OptionsPrefs               183     // F62721 EDW 01/21/2002

#define IDM_Edit                        150
#define IDMI_EditUndo                   151
#define IDMI_EditSelectDate             152
#define IDMI_EditSelectAll              153
#define IDMI_EditDeselectAll            154

#define IDM_System                      160
#define IDMI_SystemNew                  161
#define IDMI_SystemSettings             162
#define IDMI_SystemDelete               163

#define IDM_Help                        170
#define IDMI_HelpIndex                  171
#define IDMI_HelpGeneral                172
#define IDMI_HelpUsing                  173
#define IDMI_HelpProduct                174

#define IDM_Popup                       200
#define IDMI_PopupOpen                  201
#define IDMI_PopupSave                  202
#define IDMI_PopupSaveAs                203
#define IDMI_PopupGenerate              204
#define IDMI_PopupRename                205
#define IDMI_PopupDelete                206
#define IDMI_PopupApply                 207
#define IDMI_PopupAssign                208
#define IDMI_PopupRemove                209
#define IDMI_PopupDetail                210

#define IDC_CN_Files                    300
#define IDC_TX_Path                     301
#define IDC_CN_Detail                   302

#define IDC_NB_Orders                   310
#define IDC_NB_Systems                  311
#define IDC_CN_New                      312
#define IDC_CN_MES                      313
#define IDC_CN_Assign                   314

#define IDD_SystemEdit                  400
#define IDC_EF_SystemEdit_Name          401
#define IDC_LB_SystemEdit_Placement     402
#define IDC_RB_SystemEdit_Before        403
#define IDC_RB_SystemEdit_After         404
#define IDC_PB_SystemEdit_OK            405
#define IDC_PB_SystemEdit_Cancel        406

//edw 06/22/98
#define IDD_ReconcileData               410
#define IDC_RB_ReconcileData_Resolve    411
#define IDC_RB_ReconcileData_MRDB       412
#define IDC_RB_ReconcileData_AAS        413
#define IDC_PB_ReconcileData_OK         414
#define IDC_PB_ReconcileData_Cancel     415
#define IDS_ReconcileData_Title         416
#define IDS_ReconcileData_Text          417
#define IDS_LastSensed                  418

#define IDD_ReconcileNotCompl           420
#define IDC_RB_ReconcileNotCompl_Contd  421
#define IDC_RB_ReconcileNotCompl_MRDB   422
#define IDC_RB_ReconcileNotCompl_AAS    423
#define IDC_RB_ReconcileNotCompl_Quit   424
#define IDC_PB_ReconcileNotCompl_OK     425
#define IDC_PB_ReconcileNotCompl_Cancel 426

//edw 09/01/1998
#define IDC_LB_RetSource_ProdFam        434                //47701
#define IDD_RetrSource                  10435              //D68570 EDW 2/18/02
#define IDC_TX_RetrSource_Interfaces    436
#define IDC_LB_RetrSource_Interfaces    437
#define IDC_CB_RetrSource_HW            438
#define IDC_PB_RetrSource_OK            439
#define IDC_PB_RetrSource_Cancel        440
#define IDC_CB_RetrSource_SW            441
#define IDC_CB_RetrSource_Mismatches    442                   //edw10/18/99

//* Next line commented as the option Retrieve machine reported data is no
//* longer needed.  HECC: 57146  07/24/2001
//* #define IDC_CB_MRPD                     443

#define IDC_RB_IExplorer                444


//GTL Req1702 BEGIN
#define IDC_RB_Firefox                  443
//GTL Req1702 END

//ELRR F133122 Begin
#define IDC_RB_Netscape                 445
//ELRR F133122 End

#define IDC_GB_Browser                  446
#define IDC_CB_RetrSource_AutoRec       447
#define IDC_CB_RetrSource_ProcRPO       448

/* F62721 EDW 01/21/2002 begin */
#define IDC_GB_RetrSource_RetrPath      449
#define IDC_EF_RetrSource_RetrPath      450
#define IDC_PB_RetrSource_Browse        451
#define IDC_CB_RetrSource_FastPath      452
#define IDC_RB_RetrSource_SortName      453
#define IDC_RB_RetrSource_SortDate      454
#define IDC_RB_RetrSource_SortNameA     455
#define IDC_RB_RetrSource_SortNameD     456
#define IDC_RB_RetrSource_SortDateA     457
#define IDC_RB_RetrSource_SortDateD     458
#define IDC_GB_RetrSource_Sort          459

#define IDW_ProfileDialog               480
#define IDC_ProfileDialog_NB_Profile    481
#define IDC_ProfileDialog_MCC_Button    482
#define IDC_ProfileDialog_PB_OK         483
#define IDC_ProfileDialog_PB_Cancel     484
#define IDS_ProfileDialog_Retrieval     485
#define IDS_ProfileDialog_Reconcile     486
#define IDS_ProfileDialog_Language      487
#define IDS_ProfileDialog_ButtonOK      488
#define IDS_ProfileDialog_ButtonCancel  489
#define IDS_ProfileDialog_ButtonHelp    490
#define IDC_ProfileDialog_SelFirstPage  491                // DCG 02/25/2002 68568
#define IDS_ErrorRetrievalPath_Title    492                // DCG 02/25/2002 68568
#define IDS_ErrorRetrievalPath_Text     493                // DCG 02/25/2002 68568
#define IDS_ProfileDialog_TabSetFocus   494                // DCG 03/07/2002 68569
#define IDS_ProfileDialog_OKSetFocus    495                // DCG 03/07/2002 68569

#define IDD_RecProfile                  500
#define IDC_GB_RecProfile_View          501
#define IDC_RB_RecProfile_Expanded      502
#define IDC_RB_RecProfile_Collapsed     503
#define IDC_GB_RecProfile_Tile          504
#define IDC_RB_RecProfile_Horizontal    505
#define IDC_RB_RecProfile_Vertical      506
#define IDC_CB_RecProfile_ProcRPOs      507
#define IDC_CB_RecProfile_Mismatches    508
#define IDC_CB_RecProfile_AutoRec       509
/* F62721 EDW 01/21/2002 end */

#define IDD_SelectDate                  550
#define IDC_SB_SelectDate_Year          551
#define IDC_SB_SelectDate_Month         552
#define IDC_SB_SelectDate_Day           553
#define IDC_PB_SelectDate_OK            554
#define IDC_PB_SelectDate_Cancel        555

#define IDD_FileName                    600
#define IDC_EF_FileName_Name            601
#define IDC_PB_FileName_OK              602
#define IDC_PB_FileName_Cancel          603

#define IDD_Options                     700
#define IDC_LB_Options_Locale           701
#define IDC_TX_Options_Date             702
#define IDC_TX_Options_Time             703
#define IDC_PB_Options_OK               704
#define IDC_PB_Options_Cancel           705

//47973 - MAMT
#define IDD_SpecWindow                  800
#define IDC_PB_SpecWindow_OK            801
#define IDC_PB_SpecWindow_Cancel        802

#define MI_DETL_FILE                    900       //47703 Detail File
#define MI_DETL_SAVE                    901       //47703 Detailh Save
#define MI_DETL_PRINT                   902       //47703 Detail Print

#define IDD_Product                     1800
#define IDC_BMP_Product                 1801
#define IDC_TX_Product_FrontEnd         1802
#define IDC_TX_Product_BackEnd          1803
#define IDC_TX_Product_Pricing          1804
#define IDC_TX_Product_FrontEndAPI      1805
#define IDC_TX_Product_PricingAPI       1806
#define IDC_TX_Product_Extract          1807
#define IDC_TX_Product_Profile          1808
#define IDC_GP_Product                  1809
#define IDC_PB_Product_OK               1810

#define IDD_DirectoryDialog             1900

#define HELPTABLE_AFOAPPLY              5000
#define SUBTABLE_AFOAPPLY               5001
#define SUBTABLE_Orders                 5002
#define SUBTABLE_Systems                5003
#define SUBTABLE_Detail                 5004
#define SUBTABLE_SystemEdit             5005
#define SUBTABLE_SelectDate             5006
#define SUBTABLE_FileName               5007
#define SUBTABLE_Options                5008
#define SUBTABLE_Product                5009
#define SUBTABLE_FILE                   5010
#define SUBTABLE_Reconcile              5011
#define SUBTABLE_ReconcileNotCompl      5012
#define SUBTABLE_RetrSource             5013
#define SUBTABLE_SpecWindow             5014     //47973
#define SUBTABLE_ReturnRetrieval        5015     //53363
#define SUBTABLE_PROFILE                5016     //62721 EDW 01/21/2002
#define SUBTABLE_RecProfile             5017     //62721 EDW 01/21/2002

#define ACCELTABLE_AFOAPPLY             6000

#define IDH_ReplaceFile                 7001
#define IDH_SaveFile                    7002
#define IDH_DeleteFile                  7003
#define IDH_RetrieveData                7004
#define IDH_RetrieveFile                7005
#define IDH_Undo                        7006
#define IDH_AutoApply                   7007
#define IDH_ErrorWriteFile              7009
#define IDH_ErrorRetrieve               7010
#define IDH_ErrorRename                 7011
#define IDH_ErrorDelete                 7012
#define IDH_ErrorDeleteCPU              7013
#define IDH_ErrorUnusable               7014
#define IDH_ErrorBackEnd                7015
#define IDH_ReconcileData               7016
#define IDH_MAINW_MSG_NOTRESOLVED       7017
#define IDH_ConvertError                7018                  //edw12/17/97
#define IDH_NoMRDBRetrieved             7020                  //edw07/01/1998
#define IDH_NoAASRetrieved              7021                  //edw07/01/1998
#define IDH_MismatchWarning             7022                  //edw07/01/1998
#define IDH_ReconcileComplete           7023                  //edw07/01/1998
#define IDH_ReconcileResults            7024                  //edw10/19/1999
#define IDH_PlacementData               7025                  // 53915a - MAMT 02/01/2001

#define IDD_ReturnRetrieval             8000                  //ZOR 53363
#define IDC_PB_ReturnRetrieval_OK       8001
#define IDC_PB_ReturnRetrieval_Cancel   8002

#define IDS_Country                     10001
#define IDS_Language                    10002
#define IDS_CCSID                       10003
#define IDS_MainTitle                   10004
#define IDS_OrdersTitle                 10005
#define IDS_SystemsTitle                10006
#define IDS_DetailTitle                 10007
#define IDS_HelpTitle                   10008
#define IDS_Name1                       10009
#define IDS_Name2                       10010
#define IDS_DateRetrieval1              10011
#define IDS_DateRetrieval2              10012
#define IDS_Systems1                    10013
#define IDS_Systems2                    10014
#define IDS_Products1                   10015
#define IDS_Products2                   10016
#define IDS_Orders1                     10017
#define IDS_Orders2                     10018
#define IDS_NewOrders                   10019
#define IDS_MESOrders                   10020
#define IDS_Applied1                    10021
#define IDS_Applied2                    10022
#define IDS_Product1                    10023
#define IDS_Product2                    10024
#define IDS_Order1                      10025
#define IDS_Order2                      10026
#define IDS_DateCRAD1                   10027
#define IDS_DateCRAD2                   10028
#define IDS_Base1                       10029
#define IDS_Base2                       10030
#define IDS_Serial1                     10031
#define IDS_Serial2                     10032
#define IDS_Target1                     10033
#define IDS_Target2                     10034
#define IDS_Group1                      10035
#define IDS_Group2                      10036
#define IDS_Unassigned                  10037
#define IDS_Assigned                    10038
#define IDS_SerialOrder1                10039
#define IDS_SerialOrder2                10040
#define IDS_JoinHeadings                10041
#define IDS_SystemTab                   10042
#define IDS_MiscTab                     10043
#define IDS_ResourcesNotFound           10044
#define IDS_Applied                     10045
#define IDS_SystemEditNew               10046
#define IDS_SystemEditSettings          10047
#define IDS_DetailTypeModel             10048
#define IDS_DetailHwSw                  10049
#define IDS_DetailQuantity              10050
#define IDS_DetailSerial                10051
#define IDS_DetailTarget                10052
#define IDS_DetailOrder                 10053
#define IDS_DetailDate                  10054
#define IDS_DetailDescription           10055
#define IDS_DetailSystemTypeNumber      10056
#define IDS_DetailSystemTypeSerial      10057
#define IDS_DetailStatus                10058
#define IDS_DetailCustomerNumber        10059
#define IDS_DetailHardware              10060
#define IDS_DetailSoftware              10061
#define IDS_DetailFeatures              10062
#define IDS_LocaleList                  10063
#define IDS_FileNameSave                10064
#define IDS_FileNameGenerate            10065
#define IDS_FileNameRename              10066
#define IDS_FileNameName                10150                 //edw 12/5/97
#define IDS_ActionAdd                   10067
#define IDS_ActionRemove                10068
#define IDS_StatusInstalled             10069
#define IDS_StatusUninstalled           10070
#define IDS_StatusOnOrder               10071
#define IDS_StatusShipped               10072
#define IDS_StatusNewConfig             10073
#define IDS_AppliedCPU                  10074
#define IDS_Progress0                   10075
#define IDS_Progress1                   10076
#define IDS_Progress2                   10077
#define IDS_Progress3                   10078
#define IDS_NoHelp_Title                10079
#define IDS_NoHelp_Text                 10080
#define IDS_ReplaceFile_Title           10081
#define IDS_ReplaceFile_Text            10082
#define IDS_SaveFile_Title              10083
#define IDS_SaveFile_Text               10084
#define IDS_RetrieveFile_Title          10085
#define IDS_RetrieveFile_Text           10086
#define IDS_DeleteFile_Title            10087
#define IDS_DeleteFile_Text             10088
#define IDS_Undo_Title                  10091
#define IDS_Undo_Text                   10092
#define IDS_AutoApply_Title             10093
#define IDS_AutoApply_Text              10094
#define IDS_Product_Title               10095
#define IDS_Product_Text                10096
#define IDS_ReturnRetrieval_Title       10097
#define IDS_ReturnRetrieval_Text        10098
#define IDS_ErrorWriteFile_Title        10099
#define IDS_ErrorWriteFile_Text         10100
#define IDS_ErrorRetrieve_Title         10101
#define IDS_ErrorRetrieve_Text          10102
#define IDS_ErrorRename_Title           10103
#define IDS_ErrorRename_Text            10104
#define IDS_ErrorDelete_Title           10105
#define IDS_ErrorDelete_Text            10106
#define IDS_ErrorDeleteCPU_Title        10107
#define IDS_ErrorDeleteCPU_Text         10108
#define IDS_ErrorUnusable_Title         10109
#define IDS_ErrorBackEnd_Title          10110
#define IDS_NoMRDBRetrieved_Title       10113
#define IDS_NoMRDBRetrieved_Text        10114
#define IDS_NoMRDBS390_Text             10115
#define IDS_NoAASRetrieved_Title        10116
#define IDS_NoAASRetrieved_Text         10117
#define IDS_ForwardRPO_Text             10118
#define IDS_ReconcileComplete_Text      10119
#define IDS_MismatchWarning_Text        10120
#define IDS_SelectFileTitle             10121
#define IDS_SelectPathTitle             10122
#define IDS_NoFilesFound_Title          10123
#define IDS_ReconcileResults_Text       10124                 //edw10/19/99
#define IDS_MessageRPO_Text             10125                 //erg11/22/99
#define IDS_SAVEDTLDIALOGTITLE          10126                 //flw 47703
#define IDS_DTL_NOPRINTERS              10127                 //flw 47703
#define IDS_ErrorRenameRET_Text         10128                 //ZOR IC66972 062600
#define IDS_PlacementData_Text          10129                 // 53915a - MAMT 02/01/2001
#define IDS_NOTAVAILABLEDLGTITLE        10130                 //F73686   HECC: 09/11/2002

#define IDS_CFReportNotAccepted_Title        10131     // F100160  HECC  07/26/2005
#define IDS_BlueHorizonNotAccepted_Text      10132     // F100160  HECC  07/26/2005
#define IDS_XseriesProductsNotAccepted_Text  10133     // F100160  HECC  07/26/2005


#define IDS_ErrorGettingFileList_Text   11001
#define IDS_ErrorAllFilesUnusable_Text  11002
#define IDS_ErrorInvalidCfreport_Text   11003
#define IDS_ErrorCFRNotSupported_Text   11004
#define IDS_ErrorInputFileOpen_Text     11005
#define IDS_ErrorOutputFileOpen_Text    11006
#define IDS_ErrorFileRead_Text          11007
#define IDS_ErrorFileWrite_Text         11008
#define IDS_ErrorInputFileSeek_Text     11009
#define IDS_ErrorOutputFileSeek_Text    11010
#define IDS_ErrorUnexpectedEof_Text     11011
#define IDS_ErrorOaFileVersNotSup_Text  11012
#define IDS_ErrorNoProdsToGen_Text      11013
#define IDS_ErrorOpenCpuList_Text       11014
#define IDS_ErrorReadCpuList_Text       11015
#define IDS_ErrorFileRename_Text        11016
#define IDS_ErrorUnknownCfrRec_Text     11017
#define IDS_ErrorMesNotMatchProd_Text   11018
#define IDS_ErrorUnexpectedRecFnd_Text  11019
#define IDS_ErrorOutOfMem_Text          11020
#define IDS_ErrorFileNotFound_Text      11021
//edw 12/17/97-Error messages for converting MRDB to BIR
#define IDS_ErrorConvertInFileOpen_Text  11022
#define IDS_ErrorConvertOutFileOpen_Text 11023
#define IDS_ErrorFileType_Text           11024
#define IDS_ErrorAllocatingMemory        11025
#define IDS_ConvertError_Title           11026
#define IDS_ErrorLoadTableInput_Text     11027
#define IDS_ErrorLoadTableOutput_Text    11028
#define IDS_ErrorNoSerialNo_Text         11029   //ZOR IC65167

#define IDS_DTL_MSG_SPOOL_JOB            11030    //47701
#define IDS_DTL_PRNT_HDR                 11031
#define IDS_PlacementData_Title          11032   //53915a - MAMT 02/01/2001
#define IDS_ErrorLoadSWTableInput_Text   11033   //F72273 - HECC 06/27/2002
#define IDS_ErrorLoadAASFile_Title       11034   //F73686 - HECC 09/04/2002
#define IDS_ErrorLoadAASFile_Text        11035   //F73686 - HECC 09/04/2002
#define IDS_ErrorOpenDir_Text            11036   //F79548 - HECC 04/28/2003
#define IDS_ErrorLoadQuickInput_Text     11037   //F79548 - HECC 04/28/2003
#define IDS_ErrorDirCpuList_Text         11038   //F79548 - HECC 04/28/2003

#define IDS_ChangedRetrievalPath_Title   11039   //D94556 - HECC 09/21/2004
#define IDS_ChangedRetrievalPath_Text    11040   //D94556 - HECC 09/21/2004

#define afopd_desc_fd                   20100
#define afopd_type_model_fd             20110
#define afopd_serial_num_fd             20120
#define afopd_order_num_fd              20130
#define afopd_sys_type_num_fd           20140
#define afopd_status_fd                 20150
#define afopd_hw_sw_fd                  20160
#define afopd_cust_num_fd               20170
#define afopd_crad_date_fd              20180
#define afopd_mes_num_fd                20190
#define afopd_cpu_type_fd               20200
#define afopd_qty_feat_desc_fd          20210

#define afogen                          30000

#define afogen_config                   30010
#define afogen_cfr                      30020
#define afogen_apply                    30030
#define afogen_remove                   30040
#define afogen_locale                   30050

#define afomain_file                    40010
#define afomain_ret_date                40020
#define afomain_sys                     40030
#define afomain_unassi                  40040
#define afomain_unapp                   40050
#define afomain_asnote                  40060
#define afomain_unnote                  40070
#define afomain_rpos                    40080

#define afosel_open                     50010
#define afosel_save                     50020
#define afosel_delete                   50030
#define afoview_sort                    50040
#define afogen_host                     50060

#endif
