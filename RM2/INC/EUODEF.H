#ifndef _AB_EUODEF_H_
#define _AB_EUODEF_H_

#define ISDIGIT(c)   ((c) >= '0' && (c) <= '9')
#define DIGIT(c)     unsigned ((c) - '0')

#define SIPO_TYPE           "5755"  // F73732 EDW 07/25/02
#define SIPO_MODEL          "AS5"   // F73732 EDW 07/25/02

#define BUFFER_SIZE          1024

#define RC_OK                 0
#define RC_ABORT              1
#define RC_SKIP               2

#define RELFEAT_TYPE_SIZE     5
#define RELFEAT_CODE_SIZE    13
#define RELFEAT_DESC_SIZE    17

#define FEAT_NMBR_CFR_LISTS   3
#define FEAT_AAS_CFR_LIST     0
#define FEAT_MRDB_CFR_LIST    1
#define FEAT_BOTH_CFR_LIST    2
#define FEAT_DESC_SIZE       60


#define MAX_FILENAME_SIZE     12
#define FEAT_SIZE             7
#define QTY_SIZE              8
#define MACH_TYPE_SIZE        4
#define MACH_MODEL_SIZE       7
#define MACH_SERIALNO_SIZE    9
#define MACH_NMBR_CFR_LISTS   6
#define MACH_AAS_INFO_REC_LIST    0
#define MACH_MRDB_INFO_REC_LIST   1
#define MACH_AAS_54_REC_LIST      2
#define MACH_MRDB_54_REC_LIST     3
#define MACH_AAS_OTHER_REC_LIST   4
#define MACH_MRDB_OTHER_REC_LIST  5
#define SYSTEM_NAME_SIZE      40
#define SYSTEM_NMBR_CFR_LISTS  2
#define SYSTEM_TITLE_SIZE     40
#define SYSTEM_USERID_SIZE    40
#define SYSTEM_NUMBER_SIZE     7
#define CUSTOM_NUMBER_SIZE     7
#define SYSTEM_DATE_SIZE       8
#define CFREC_TYPE_SIZE        2
#define CFREC_INSTID_SIZE      6

#define GENCODE_SIZE           21
#define SPECCODE_SIZE          13
#define SPECDESC_SIZE          17

//#define FEAT_REC_SIZE        301
#define MAX_CFREC_SIZE       300

#define PL_SIGNATURE          51148
#define PL_SYSDATA            1  // items need to be consecutive and start at 1
#define PL_MACHINES           2  // order unimportant
#define PL_MFEATS             3
#define PL_CFRECS             4
#define PL_XPLNS              5
#define PL_RPO2S              6
#define PL_FEATS              7
#define PL_MDBFEATDESC        8
#define PL_L55                9
#define PL_L56               10
#define PL_SWDISCREPS        11
#define PL_L10_AAS           12
#define PL_L10_MRDB          13
#define PL_L05_AAS           14
#define PL_L05_MRDB          15
#define PL_L52_AAS           16
#define PL_L52_MRDB          17
//#define PL_NONMRFEATS         8


#define PL_PKFEATS            1     // product knowledge
#define PL_PKFEATSDESC        2
#define PL_PKNONMRFEATS       3




#define RBS_ERR_NO_FILE        1
#define RBS_ERR_NO_EUOP_FILE   2
#define RBS_ERR_READ_FILE      3
#define RBS_ERR_DAMAGED_FILE   4
#define RBS_ERR_UNKNOWN_TYPE   5
#define RBS_ERR_INTERNAL       6
#define RBS_ERR_PARR_ERROR     100
#define RBS_ERR_GOTEM_ALL      200


struct PackList{
                unsigned   Type;
                unsigned   LRecl;
                unsigned   Count;
                unsigned   Offset;
               };

struct Record_00 {
                  char       Type[2];                    //       0
                  char       Type2[2];                   //       2
                  char       Filler1[14];                //       4
                  char       Name[SYSTEM_NAME_SIZE];     //  20  18
                  char       Title[SYSTEM_TITLE_SIZE];   //  40  38
                  char       Filler2[158];               //      78
                  char       UserId[SYSTEM_USERID_SIZE]; //  40 236
                 };

struct CFR_00 {
               char       Type[2];                    //       0
               char       Version[2];                 //       2
               char       Date[6];                    //       4
               char       aidName[8];                 //      10
               char       System[20];                 //      18
               char       Title[41];                  //      38
               char       filler1[158];               //      78
               char       UserId[40];                 //     236
               char       filler2[44];                //     276
              };                                      //     320

struct Record_05 {
               char       Type         [ 2];          //       0
               char       Filler1      [ 9];          //       2
               char       TypePrint;                  //      11
               char       Filler2      [ 1];          //      12
               char       RefNotesNmbr [ 3];          //      13
               char       Filler3      [60];          //      16
              };                                      //      76

struct Record_07 {
                  char       Type[2];                    //       0
                  char       RefNotesNmbr[3];            //       2
                  char       Description[40];            //       5
                  char       Filler[40];                  //      45
                 };

struct CFR_00V31 {
               char       Type        [  2];             //       0
               char       Version     [  2];             //       2
               char       Date        [  8];             //       4
               char       Filler      [ 29];             //      12
               char       Description [ 40];             //      41
               char       Filler2     [166];             //      81
              };                                         //     247


struct Feat78{
              char   Feature [7];        // 7  0
              char   Qty     [8];        // 8  7
             };


struct Record_08 {
                  char       Type[2];                    //       0
                  char       Machine[MACH_TYPE_SIZE];    //  4    2
                  char       MachineQty[QTY_SIZE];       //  8    6
                  char       cAddQty;                    //  1   14
                  char       cNotes;                     //  1   15
                  char       cIPProd;                    //  1   16
                  char       Filler1[2];                 //      17
                  char       Model[MACH_MODEL_SIZE];     //  7   19
                  char       ModelQty[QTY_SIZE];         //  8   26
                  Feat78     Feats[1];                   // 15   34
                 };

//NK
struct Feat75{
              char   Feature[ 7];
              char   Qty    [ 5];
             };
//NK
struct Record_08V31 {                                    //  Offset
                     char       Type      [ 2];          //      0
                     char       Machine   [ 4];          //      2
                     char       cCFOutFlag;              //      6
                     char       Model     [ 3];          //      7
                     char       ProductQty[ 5];          //     10
                     Feat75     Feats[1];                //     15
                    };

//* F86750  Begin logic:  Added code of the RT 38 record layout

struct FeatR38{
              char   Feature[ 7];
              char   Qty    [ 5];
              char   Action;
             };

struct Record_38_V31 {                                    //  Offset
                      char       Type       [ 2];         //      0
                      char       Machine    [ 4];         //      2
                      char       Model      [ 3];         //      6
                      char       ActionCode [ 1];         //      9
                      char       ProductQty [ 5];         //     10
                      char       Filler     [20];         //     15
                      FeatR38    Feats[1];                //     35
                     };
//* F86750 End logic: HECC: 01/13/2004

struct Record_10 {
                  char       Type[2];                    //       0
                  char       RefNotesNmbr[3];            //       2
                  char       Description[40];            //       5
                  char       Filler[40];                  //      45
                 };


struct FeatR19{
                char   Feature[FEAT_SIZE];  // 7   0
                char   Qty[4];              // 4   7
                char   Action;              // 1  11
                char   Filler[36];          // 35 12  L 47
               };

struct Record_19 {
                  char       Type[2];                    //       0
                  char       Machine[MACH_TYPE_SIZE];    //  4    2
                  char       Filler1   [3];              //  3    6
                  char       FromModel [3];              //  3    9
                  char       toMachine[MACH_TYPE_SIZE];  //  4   12
                  char       ToModel  [3];               //  3   16
                  char       Qty      [4];               //  4   19
                  char       Action;                     //  1   23
                  char       Filler2  [3];              //  3    24
                  char       ProdSerial[9];             //  9    27
                  char       Filler3  [35];             // 35    36
                  FeatR19    Feats[1];                   // 15   71
                 };


struct Feat751{
               char   Feature [7];         //    0
               char   Qty     [5];         //    7
               char   Action;              //    12
              };                           //    13
struct Record_19V31 {
                  char       Type            [ 2];       //       0
                  char       ProductType     [ 4];       //       2
                  char       ProductModel    [ 3];       //       6
                  char       ToProductType   [ 4];       //       9
                  char       ToProductModel  [ 3];       //      13
                  char       Action;                     //      16
                  Feat751    Feats[1];                   //      17
                 };
#if 0
struct R25_Feat{
                char   Feature[FEAT_SIZE];  // 7  0
                char   Qty[QTY_SIZE];       // 8  7
               };

struct Record_25 {
                  char       Type[2];                    //       0
                  char       Machine[MACH_TYPE_SIZE];    //  4    2
                  char       MachineQty[QTY_SIZE];       //  8    6
                  char       cAddQty;                    //  1   14
                  char       cNotes;                     //  1   15
                  char       cIPProd;                    //  1   16
                  char       Filler1[2];                 //      17
                  char       Model[MACH_MODEL_SIZE];     //  7   19
                  char       ModelQty[QTY_SIZE];         //  8   26
                  R25_Feat   Feats[1];                   // 15   34
                 };
#endif

struct FeatR50{
                char   Feature[FEAT_SIZE];  // 7   0
                char   Qty[4];              // 4   7
                char   Action;              // 1  11
                char   Filler[31];          // 31 12  L 42
               };

struct Record_50 {
                  char       Type[2];                    //       0
                  char       Machine[MACH_TYPE_SIZE];    //  4    2
                  char       Model     [3];              //  3    6
                  char       Qty       [4];              //  4    9
                  char       Action;                     //  1   13
                  char       Filler1  [43];              // 43   14
                  FeatR50    Feats[1];                   // 15   57
                 };

struct Record_50V31 {
                  char       Type            [ 2];       //       0
                  char       ProgramType     [ 4];       //       2
                  char       ProgramModel    [ 3];       //       6
                  char       Action;                     //       9
                  Feat751    Feats[1];                   //      10
                 };


struct Record_53 {
                  char       Type[2];                    //       0
                  char       Filler1[7];                 //       2
                  char       Number[SYSTEM_NUMBER_SIZE]; //   7   9
                  char       Filler2[5];                 //      16
                  char       DateSent[SYSTEM_DATE_SIZE]; //   8  21
                  char       Filler3[10];                //      29
                  char       Date[SYSTEM_DATE_SIZE];     //   8  39
                 };

struct Record_96 {                                   // offset
                  char       RecType          [ 2];  //  0
                  char       Feat_RPQ_PartNmbr[ 7];  //  2
                  char       RefNotesNmbr     [ 3];  //  9
                  char       VRM_Text         [ 8];  // 12
                  char       Filler1          [ 4];  // 20
                  char       Release          [ 2];  // 24
                  char       Mod              [ 2];  // 26
                  char       PriceFlag        [ 1];  // 28
                  char       Filler2          [21];  // 29
                  char       Description      [60];  // 50   (SW->60 HW->44)
                 };                                  //110

struct Record_95 {                                   // offset
                  char       RecType              [ 2];  //  0
                  char       ProductType          [ 4];  //  2
                  char       ProductModel         [ 3];  //  6
                  char       Filler1              [10];  //  9
                  char       SerialNo             [ 9];  // 19
                  char       OrderNo              [ 9];  // 28
                  char       Filler2              [11];  // 37
                  char       PiceFlag             [ 1];  // 48
                  char       SystemNoFlag         [ 1];  // 49
                  char       SystemType           [ 4];  // 50
                  char       SystemNo             [ 7];  // 54
                  char       SIUFlag              [ 1];  // 61
                  char       DesignatedCPUType    [ 4];  // 62
                  char       DesignatedCPUSer_Ord [ 9];  // 66
                  char       CustomerNo           [ 7];  // 75
                  char       CradDate             [ 8];  // 82
                  char       InstallDate          [ 8];  // 90
                  char       ProductStatus        [ 1];  // 98
                  char       SequenceField        [ 2];  // 99
                  char       StepSeqFlagForSW     [ 1];  //101
                  char       RefNotesNmbr         [ 3];  //102
                  char       SourceFlag           [ 1];  //105
                  char       ModelSequenceCode    [ 1];  //106
                  char       Filler3              [35];  //107
                  char       Description          [60];  //143   (SW->60 HW->44)
                 };                                      // 203


struct Record_54 {                  // Type 54 CF Report record     offset
  char rectype [ 2];                // Record type                      0
  char InstId  [CFREC_INSTID_SIZE]; // Instance id               6      2
  char InstType;                    // Instance type                    8
  char MachType[ 4];                // Machine type                     9
  char MachMod [ 7];                // Machine model                   13
  char Feature [ 7];                // Feature                         20
  char SerialNo[ 9];                // Serial number                   27
  char contract[ 7];                // Contract number                 36
  char custnum [CUSTOM_NUMBER_SIZE];  // Customer number         7     43
  char modcode [ 3];                // Modification code               50
  char moddate [ 8];                // Modification date yyyymmdd      53
  char refmach [ 4];                // Ref. machine type               61
  char euoprso [ 9];                // EUOP ref serial/order           65
  char refinsid[ 6];                // Ref. instance id                74
  char CradDate[ 6];                // Crad date mmddyy                80
  char swflag  [ 1];                // S/W flag                        86
  char ordersys[ 7];                // Ordering system                 87
  char ordernum[ 9];                // Order number                    94
  char CsdDate [ 8];                // CSD date                       103

  char Sequence[ 2];                // Sequence field
  char InstDate[ 6];                // Install date mmddyy
  char custname[15];                // Customer name
  char numusers[ 4];                // Number of users
  char vrmtext [12];                // VRM = version/release/mod level
  char stsqflag[ 1];                // Step seq flag
  char lictlflg[ 1];                // License control flag
  char resism  [ 7];                // Reserved for ISM
  char resorder[13];                // Reserved for ordering
  char refnonum[ 3];                // Reference notes num
  char release [ 2];                // Release
  char modlevel[ 2];                // Modification level
  char origser [ 9];                // Original serial number
  char origord [ 9];                // Original order number
  char SorcFlag;                    // Source flag:  I = AAS inventory
                                    //               B = AAS backlog
                                    //               M = MRDB
  char QualText[FEAT_DESC_SIZE];                // Quality text
                                    //  (maximum 60 characters)
};                                  // Type 54 CF Report record

struct CFR_55 {
               char Type [ 2];
               char InstId [CFREC_INSTID_SIZE];
               char Filler [75];
              };

struct CFR_Inst {
               char Type [ 2];
               char InstId [CFREC_INSTID_SIZE];
               char Remainder [1];
              };


struct Feat44{
              char   Feature[ 4];
              char   Qty    [ 4];
             };

struct Record_17 {
               char      RecType [ 2];                    //   0
               char      Type    [ 4];                    //   2
               char      Filler  [ 3];                    //   6
               char      Model   [ 3];                    //   9
               char      Qty     [ 4];                    //  12
               Feat44    Feats   [ 1];                    //  16
              };

struct Record_35 {
               char      RecType [ 2];                    //   0
               char      Type    [ 4];                    //   2
               char      Model   [ 3];                    //   6
               char      Qty     [ 4];                    //   9
               Feat44    Feats   [ 1];                    //  13
              };


struct Feat74{
              char   Feature[ 7];
              char   Qty    [ 4];
             };

struct Record_47 {
               char      RecType[ 2];                    //   0
               char      Type   [ 4];                    //   2
               char      Filler [ 3];                    //   6
               char      Model  [ 3];                    //   9
               char      Qty    [ 4];                    //  12
               Feat74    Feats  [ 1];                    //  16
              };


struct Record_49 {
               char      RecType[ 2];                    //   0
               char      Type   [ 4];                    //   2
               char      Model  [ 3];                    //   6
               char      Qty    [ 4];                    //   9
               Feat74    Feats  [ 1];                    //  13
              };

struct Record_SW_V31 {       // records 47,48 & 49
               char      RecType[ 2];                    //   0
               char      Type   [ 4];                    //   2
               char      Model  [ 3];                    //   6
               char      Qty    [ 5];                    //   9
               Feat75    Feats  [ 1];                    //  14
              };

struct Record_99 {
               char      RecType[ 2];                    //   0
               char      CheckSumValue[10];              //   2
              };                                         //  12

#endif









