/***********************************************************************
*                                                                      *
* Define message constants.                                            *
*                                                                      *
* F86752 ZOR  march-2004 a new id message is added                     *
***********************************************************************/

#define EUO_MSG_CONQUAN                   1
#define EUO_MSG_AAS_Y_MRDB_N              2
#define EUO_MSG_MRDB_Y_AAS_N              3
#define EUO_MSG_RES_MRDB_QUAN             4
#define EUO_MSG_RES_AAS_QUAN              5
#define EUO_MSG_RES_AAS_QUAN0             6
#define EUO_MSG_RES_W_AAS_FEATCODE        7
#define EUO_MSG_MACH_READ_NOT_IDENT       8
#define EUO_MSG_NOT_MACH_READ             9
#define EUO_MSG_NOT_INSTALLED            10
#define EUO_MSG_MMDDYY                   11
#define EUO_MSG_DDMMYY                   12
#define EUO_MSG_YYMMDD                   13
#define EUO_MSG_NOEDIT                   14
#define EUO_MSG_EDIT_CRAD                18
#define EUO_MSG_EDIT_DATE                19
//#define EUO_MSG_INVALID_CFR               17
#define EUO_MSG_RES_COMBO                15
#define EUO_MSG_RES_GENERIC_MACH         16
#define EUO_MSG_INTERNAL_ERROR           23
#define EUO_MSG_DUMMY                    24
#define EUO_MSG_ORPHAN_RECORD            25
#define EUO_MSG_NON_BLANK_1ST_54         26
#define EUO_MSG_DUPE_MACH                27
#define EUO_MSG_NO_54                    28
#define EUO_MSG_NO_98                    29
#define EUO_MSG_WRONG_VERSION            30
#define EUO_MSG_25_MACH_QTY              31
#define EUO_MSG_25_ADD_QTY               32
#define EUO_MSG_25_NOTES                 33
#define EUO_MSG_25_MODEL_QTY             34
#define EUO_MSG_NO_FILE                  35
#define EUO_MSG_UNKNOWN_REC_TYPE         36
#define EUO_MSG_54_DIFF_25_QTY           37
#define EUO_MSG_54_NO_25_QTY             38
#define EUO_MSG_REBUILD                  39
#define EUO_MSG_SAVING                   40
#define EUO_MSG_READING_AAS              41
#define EUO_MSG_READING_MR               42
#define EUO_MSG_RESOLVING                43
#define EUO_MSG_BLDG_PK                  44
#define EUO_MSG_WRITING                  45
#define EUO_MSG_SPOOLED                  46
#define EUO_MSG_NOFEATMACH_MRDB_Y_AAS_N  47
#define EUO_MSG_NOFEATMACH_AAS_Y_MRDB_N  48
#define EUO_MSG_MACH_AAS_Y_MRDB_N        49
#define EUO_MSG_MACH_MRDB_Y_AAS_N        50
#define EUO_MSG_FEAT_SERNO_DIFFERS       51
#define EUO_MSG_MODELS_DIFFER_PICK       52
#define EUO_MSG_MODELS_DIFFER_QUIT       53
#define EUO_MSG_MODELS_DIFFER_PROCEED    54
#define EUO_MSG_ERR_OPEN_FILE            55
#define EUO_MSG_OLD_MRDB_PICK            56
#define EUO_MSG_OLD_MRDB_QUIT            57
#define EUO_MSG_ERR_CONVERSION_START     58
#define EUO_MSG_ERR_CONVERSION_WAIT      59
#define EUO_MSG_ERR_CONVERSION           60
#define EUO_MSG_NO_95                    61
#define EUO_MSG_ERR_BAD_CHECKSUM         62
#define EUO_MSG_ERR_CHECKSUM_RUN         63
#define EUO_MSG_ERR_READING_PKFILE       64
#define EUO_MSG_ERR_READING_PKFILES      65
#define EUO_MSG_MODELS_DIFFER_PICKS390   66  //53915 MAMT 28/02/2001

//F72273  Strings added to describe how the iSeries SW records were resolved
//*HECC: 07/04/2002
#define EUO_MSG_ISERIES_MRDB_Y_AAS_Y_R1        67  /* F73732 EDW 07/25/02 */
#define EUO_MSG_ISERIES_MRDB_Y_AAS_N_R1        68  /* F73732 EDW 07/25/02 */
#define EUO_MSG_ISERIES_MRDB_N_AAS_Y_R1        69  /* F73732 EDW 07/25/02 */
#define EUO_MSG_ISERIES_MRDB_Y_AAS_N_R2        70  /* F73732 EDW 07/25/02 */
#define EUO_MSG_ISERIES_MRDB_N_AAS_Y_TBL_Y_R4  71  /* F73732 EDW 07/25/02 */

//F79548  Strings added to explain that the Feature.$$F file was not found in the
//appropiate Product Fmaily directory
//*HECC: 05/09/2003
#define EUO_MSG_FEATURE_FILE_NOTFOUND          72
#define EUO_MSG_RESOLVED_IS			     73 /*86752*/



