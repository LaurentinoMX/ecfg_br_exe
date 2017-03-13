/* --------------------------------------------------------------------
                         Controls Header File
                              Chapter 10

                    Real World Programming for OS/2
             Copyright (c) 1993 Blain, Delimon, and English
-------------------------------------------------------------------- */

/*************************************************
               GENERAL USE FUNCTIONS
 ************************************************/

VOID EXPENTRY CenterWindow (HWND);
VOID EXPENTRY ForceLink(VOID);


/*************************************************
                 BEVEL CONTROL
 ************************************************/

#define BEVELCLASS       "BEVEL"

/* Use low word of style for private style flags */
#define BVS_BEVELIN     0x00000000
#define BVS_BEVELOUT    0x00000001
#define BVS_RECTANGLE   0x00000000
#define BVS_LINE        0x00000002
#define BVS_FILL        0x00000004
#define BVS_PREVID      0x00000008
#define BVS_PREVWINDOW  0x00000010

#define BVS_IDMASK      0x00008000

/* Bevel control messages */
#define BVM_SETCOLOR     WM_USER+1
#define BVM_SETTHICKNESS WM_USER+2
#define BVM_RESIZE       WM_USER+3


/*************************************************
              THERMOMETER CONTROL
 ************************************************/

#define THERMOMETERCLASS "THERMOMETER"
#define PROGRESSBARCLASS "PROGRESSBAR"
#define MINSECTIMERCLASS "MINSECTIMER"
#define COUNTERCLASS     "COUNTER"
#define SPINDATECLASS    "SPINDATE"

/* Use low word of style for private style flags */
#define ALCTLS_NOPERCENT       0x00000001
#define ALCTLS_VERTICAL        0x00000002
#define ALCTLS_BORDER_SYSTEM   0x00000008
#define ALCTLS_BORDER_FURROW   0x00000010
#define ALCTLS_BORDER_INNI     0x00000020
#define ALCTLS_BORDER_OUTI     0x00000040
#define ALCTLS_BORDER_STANDARD 0x00000080
#define ALCTLS_BORDER_MASK     0x000000F0
#define ALCTLS_DT_FORMAT_MDY   0x00000100
#define ALCTLS_DT_FORMAT_DMY   0x00000200
#define ALCTLS_DT_FORMAT_YMD   0x00000400
#define ALCTLS_DT_FORMAT_MASK  0x00000700
#define ALCTLS_DT_FORMAT_PRF   0x00000000
#define ALCTLS_DT_TEXT_MONTHS  0x00000800
#define ALCTLS_1_DIGIT         0x00000100
#define ALCTLS_2_DIGIT         0x00000200
#define ALCTLS_3_DIGIT         0x00000000  //default
#define ALCTLS_4_DIGIT         0x00000400
#define ALCTLS_5_DIGIT         0x00000500
#define ALCTLS_6_DIGIT         0x00000600
#define ALCTLS_7_DIGIT         0x00000700
#define ALCTLS_8_DIGIT         0x00000800
#define ALCTLS_9_DIGIT         0x00000900
#define ALCTLS_10_DIGIT        0x00000A00

/* Thermometer control messages */
#define ALCTLM_SETRANGE                 WM_USER+1
#define ALCTLM_DATE_SET_LIMIT           WM_USER+1
#define ALCTLM_SETVALUE                 WM_USER+2
#define ALCTLM_DATE_SET                 WM_USER+2
#define ALCTLM_TIME_SET                 WM_USER+2
#define ALCTLM_SETCOLORS                WM_USER+3
#define ALCTLM_SETDELTA                 WM_USER+4
#define ALCTLM_DATE_ADD                 WM_USER+4
#define ALCTLM_INCREMENT                WM_USER+5
#define ALCTLM_DECREMENT                WM_USER+6
#define ALCTLM_GETRANGE                 WM_USER+7
#define ALCTLM_DATE_SUBTRACT            WM_USER+7
#define ALCTLM_GETVALUE                 WM_USER+8
#define ALCTLM_DATE_GET                 WM_USER+8
#define ALCTLM_GETCOLORS                WM_USER+9
#define ALCTLM_GETDELTA                 WM_USER+10
#define ALCTLM_DATE_SET_TODAY           WM_USER+10
#define ALCTLM_RESET                    WM_USER+11
#define ALCTLM_MINIMUM_SIZE             WM_USER+12
#define ALCTLM_START_COUNTING_UP        WM_USER+13
#define ALCTLM_DATE_SET_MONTH_NAMES     WM_USER+13
#define ALCTLM_START_COUNTING_DOWN      WM_USER+14
#define ALCTLM_STOP_COUNTING            WM_USER+15
#define ALCTLM_LEAVE_WAKE_UP_CALL       WM_USER+16
#define ALCTLM_TAB_OUT_RIGHT            WM_USER+17
#define ALCTLM_TAB_OUT_LEFT             WM_USER+18
#define ALCTLM_SET_FOCUS                WM_USER+19
#define ALCTLM_DELAY_HIDE               WM_USER+20
