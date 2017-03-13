#ifndef EUOGMAIN_H
#define EUOGMAIN_H

#define WND_DISC         0x1000
#define WND_RESO         0x2000
#define WND_GEN          0x3000
#define WND_GEN_SPB_MCC  0x3010
#define WND_GEN_SPB      0x3011
#define WND_GEN_SPB_TXT  0x3012
#define WND_BUTTONS      0x2020

#define CNR_DISC         0x1010
#define CNR_RESO         0x1020
#define CNR_GEN          0x1030

#define MI_POPMENU_DISC  0x1200
#define MI_POPMENU_RESO  0x2200
#define MI_POPMENU_MACH  0x3200
#define MI_POPMENU_GEN   0x4200
//#define MI_EDITBASE    0x1220
#define MI_EDISCBASE     0x1220
//#define MI_EDISCCRAD     0x1221
//#define MI_EDISCDATE     0x1223
#define MI_ERESOBASE     0x1225
//#define MI_ERESOCRAD     0x1226
//#define MI_ERESODATE     0x1228


#define MI_SELECTED_DISC   3001
#define MI_SELECTED_RESO   3002
#define MI_EXPL            3003
#define MI_EDIT_DISC       3004
#define MI_EDIT_RESO       3005
#define MI_DISC_APPLY      3006
#define MI_GEN_APPLY       3007
#define MI_MIS_FILE        3008    //45449 Mismatch File
#define MI_MIS_SAVE        3009    //45449 Mismatch Save
#define MI_MIS_PRINT       3010    //45449 Mismatch Print
#define IDS_MMDIALOGTITLE  3011    //45449
#define IDS_ERR_NOPRINTERS 3012    //45449

//#define MI_DYNINFO         3011
#define MI_HELP            3014
#define MI_GENHELP         3015
//#define MI_USINGHELP       3016
//#define MI_TUTORIAL        3017
#define MI_HELPINDEX       3018
#define MI_CANCEL          3024
#define RPO_CL_MSG         9876
#define XPLNW_CL_MSG_DISC  9877
#define XPLNW_CL_MSG_RESO  9878

#define STR_INFO         0x1231
#define STR_HTITLE       0x1255

#define WND_DISC_STR         "Mismatches"
#define WND_RESO_STR         "Matches"
#define WND_GEN_STR          "Generic Features"
//#define STR_INFO_STR         "SAVE: Save changes, EXIT: Save and Exit, QUIT: Exit without saving"
#define MI_SELECTED_DISC_STR "For selected feature, display additional information, apply to base or edit specific fields."
#define MI_SELECTED_RESO_STR "For selected feature, display additional information or edit specific fields."
#define MI_EXPL_STR          "Display the rules that were used to automatically resolve the feature."
#define MI_EDIT_DISC_STR     "Apply option and list of fields that can be edited."
#define MI_EDIT_RESO_STR     "List of fields that can be edited."
#define MI_APPLY_STR         "Apply the row to the Resolution Window."
//#define MI_DYNINFO_STR       "Enable or disable the display of the dynamic information line on this window."
#define MI_HELP_STR          "Help options that are available."
#define MI_HELPINDEX_STR     "Option unavailable."
#define MI_GENHELP_STR       "Display the help text for this window."
//#define MI_USINGHELP_STR     "Option unavailable."
//#define MI_TUTORIAL_STR      "Option unavailable."
#define MI_EDBASE_STR        "To edit Base"
//#define MI_EDCRAD_STR        "To edit CRAD"
//#define MI_EDDATE_STR        "To edit Installed date"
#define STR_HTITLE_STR       "Help Window"
#define STR_PB_HIDE          "Hide"

#define STR_MSGTTL_DATE_ENTRY     "Date Entry Error"
#define STR_MSGTTL_NOEDIT         "Restricted edit message"
#define STR_MSGTTL_INVOPT         "Invalid Option"
#define STR_MSGTTL_INVALID_CFR    "Invalid CF Report"

// Strings used in euogmain.cpp
#define MACH_STR         "Machine/Model"
#define BASE_STR         "Base"
#define QTY_STR          "Quantity"           //edw12/16/98-SR6551
#define MRDB_STR         "MRDB"
#define AAS_STR          "AAS"
#define FEATURE_STR      "Feature"
#define DESC_STR         "Description"
#define CRAD_STR         "CRAD"
#define IDATE_STR        "Install Date"
#define SNUM_STR         "Serial Number"
#define OK_BTN_STR       "  &Ok  "
#define HELP_BTN_STR     " Help "
#define CANCEL_BTN_STR   "Cancel"

//F73252 Strings used in euogmain.cpp to display the two new parameters
//in Matches & Mismatches windows:  Sensed and Receive at IBM dates.
//Begin:  F73252 07/18/2002 HECC
#define ARIAL_FONT_STR   "Arial"
#define RECEIVE_DATE_STR "Date Received at IBM:  "
#define SENSED_DATE_STR  "Date Sensed:  "
#define NEW_LINE_CHAR    "\n"
//End:  F73252 07/18/2002

//F104628 Strings used in euogmain.cpp to display the "Mismatch Type"
//column in the "HW Mismatches" dialog
//Begin: 01/30/2006 RERC
#define MACHTYPE_STR     "Mismatch Type"
//#define ACCEPTED_STR     "ACCEPTABLE" //F107280 RERC 04/24/2006 Change "ACCEPTABLE" to "Acceptable"
#define ACCEPTED_STR     "Acceptable"
//#define UNACCEPTED_STR   "UNACCEPTABLE" //F107280 RERC 04/24/2006 Change "UNACCEPTABLE" to "Critical"
#define UNACCEPTED_STR   "Critical"
#define ACCEPTED_CHR     'A'
#define UNACCEPTED_CHR   'U'
//End:  01/30/2006 RERC
//F124059 flag for exclude flag on nocomp.tbl Mismatch type
#define EXCLUDED_CHR     'X' // 06/12/2008 SAQP
#define EXCLUDED_STR     "Excluded"


#define BLANK            " "
#define STAR             "*"
#define FPLUS            "[+]"
#define FMINUS           "[-]"
#define FBLANK           "[ ]"
#define SLASH            "/"
#define DASH             "-"
#define SERIAL_NUM       "Serial# "
#define STR_OEM          "OEM"

#define RPO_CODE_DELETE 'D'
#define RPO_CODE_ADD    'A'

// Text for Menu Items
#define  MENU_TXT_SELECTED  "&Selected"
#define  MENU_TXT_EXPL      "E&xplain...\tF2"
#define  MENU_TXT_EDIT      "&Edit"
#define  MENU_TXT_APPLY     "&Apply...\tCtrl+A"
#define  MENU_TXT_ED_BASE   "Edit &Base..."
#define  MENU_TXT_ED_QTY    "Edit &Quantity..."    //edw12/16/98-SR6551
#define  MENU_TXT_FILE      "&File"                //45449
#define  MENU_TXT_PRT       "Print"                //45449
#define  MENU_TXT_SAVE      "Save"                 //45449

#define  MENU_TXT_HELP      "&Help"
#define  MENU_TXT_HINDEX    "Help &index..."
#define  MENU_TXT_HGEN      "&General Help..."
//#define  MENU_TXT_HUSE      "&Using Help..."
//#define  MENU_TXT_HTUT      "&Tutorial..."

#define  MENU_TXT_SA        "a"
#define  MENU_TXT_SR        "r"
#define  MENU_TXT_SI        "i"
#define  MENU_TXT_SP        "p"
#define  MENU_TXT_CA        "A"
#define  MENU_TXT_CR        "R"
#define  MENU_TXT_CI        "I"
#define  MENU_TXT_CP        "P"

#define  EUOP_BACKLOG             'B'
#define  EUOP_INVENTORY           'I'
#define  EUOP_MINVENTORY          'M'
#define  EUOP_MSG_FILE_NAME       "EUOPENU.MSG"
#define  EUOP_ALLOW_EDIT          1
#define  EUOP_CFR_BLANK_DATE      "      "
#define  EUOP_ERR_FILE            "ERRMSG.TXT"
//****************************************************************************** */
// icon ids
//****************************************************************************** */

#define ICO_4005  4005
#define ICO_4006  4006
#define ICO_4007  4007
#define ICO_4008  4008
#define ICO_4009  4009
#define ICO_4010  4010
#define ICO_4011  4011
#define ICO_4012  4012

//* F73252  New icons Ids to show how features were resolved. HECC: 07/16/2002
#define ICO_4013  4013
#define ICO_4014  4014
#define ICO_4015  4015
//* F73252  HECC: 07/16/2002

#endif
