/*--------------------------------------------------------------------------*/
/* EUOPHLP.H                                                                */
/*                                                                          */
/* RM/2 help panel ids.                                                     */
/*                                                                          */
/* Purpose: this filel contains the panel ids of all of the help panels     */
/*   used by RM/2                                                           */
/*                                                                          */
/* Change Log:                                                              */
/*                                                                          */
/* 01-20-95 Updated to remove old ids, and include sections for all windows */
/*          in RM/2 [djl]                                                   */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* Windows covered in this file:                                            */
/*                                                                          */
/* RM/2 Main Window                                                         */
/* Discrepancies Window                                                     */
/* Resolutions Window                                                       */
/* Software Discrepancies Window                                            */
/* Generic Window                                                           */
/* RPO Window                                                               */
/* Explain Window                                                           */
/* Print Preview Window                                                     */
/* Profile Window                                                           */
/* RPO Information Window                                                   */
/* Upload Window (used when selecting host session to upload to)            */
/*                                                                          */
/*--------------------------------------------------------------------------*/


#ifndef EUOPHLP_H
#define EUOPHLP_H

#define  EUOP_HLP_TABLE                       5000

/*--------------------------------------------*/
/* RM/2 Main Window                           */
/*--------------------------------------------*/
#define  IDH_TBL_MAINW                        5100
#define  IDH_PNL_MAINW                        5101

#define IDH_PNL_MAINW_MI_FILE                 5102 // file menu
#define   IDH_PNL_MAINW_MI_NEW                5103
#define   IDH_PNL_MAINW_MI_OPEN               5104
#define   IDH_PNL_MAINW_MI_RESOLVE            5105
#define   IDH_PNL_MAINW_MI_SAVE               5106
#define   IDH_PNL_MAINW_MI_SAVEAS             5107
#define   IDH_PNL_MAINW_MI_PRINT              5108
#define   IDH_PNL_MAINW_MI_PPRVW              5109
#define   IDH_PNL_MAINW_MI_GENCFR             5110
#define   IDH_PNL_MAINW_MI_PROFILE            5111 // ???link???
#define   IDH_PNL_MAINW_MI_EXIT               5112
#define IDH_PNL_MAINW_MI_VIEW                 5113 // view menu
#define   IDH_PNL_MAINW_MI_DISCW              5114
#define   IDH_PNL_MAINW_MI_RESW               5115
#define   IDH_PNL_MAINW_MI_RPOW               5116
#define   IDH_PNL_MAINW_MI_SW                 5117
#define   IDH_PNL_MAINW_MI_BLW                5118
#define   IDH_PNL_MAINW_MI_PKFILE             5119

#define  IDH_TBL_FD                           5150
#define  IDH_PNL_FD                           5151
#define  IDH_PNL_FD_OPEN                      5152
#define  IDH_PNL_FD_SAVEAS                    5153
#define  IDH_PNL_FD_WRITE_CFR                 5154
#define  IDH_PNL_FD_EXPORT                    5155
#define  IDH_PNL_FD_RESOLVE                   5156



#define  IDH_PNL_PKFILEWND                    5300
#define  IDH_TBL_PKFILEWND                    5301
/*--------------------------------------------*/
/* Discrepancies Window                       */
/*--------------------------------------------*/
#define  IDH_TBL_DISCWND                      5400
#define  IDH_PNL_DISCWND                      5401

#define  IDH_PNL_DISCWND_CL_BASE              5402
#define  IDH_PNL_DISCWND_CL_MRDB              5403
#define  IDH_PNL_DISCWND_CL_AAS               5404
#define  IDH_PNL_DISCWND_CL_MODTYPE           5405
#define  IDH_PNL_DISCWND_CL_DESCRIP           5468
#define  IDH_PNL_DISCWND_CL_CRAD              5470
#define  IDH_PNL_DISCWND_CL_INSTDT            5472
/*#define  IDH_PNL_DISCWND_CL_SLNUM             5474 */

#define  IDH_PNL_DISCWND_MENU_FILE            5475
#define  IDH_PNL_DISCWND_MENU_SELECTED        5480
#define    IDH_PNL_DISCWND_SMENU_EXPLAIN      5482
#define    IDH_PNL_DISCWND_SMENU_APPLY        5486
#define    IDH_PNL_DISCWND_SMENU_EDBASE       5490

/*--------------------------------------------*/
/* Resolutions Window                         */
/*--------------------------------------------*/
#define  IDH_TBL_RESLWND                      5600
#define  IDH_PNL_RESLWND                      5601

#define  IDH_PNL_RESLWND_CL_BASE              5656
#define  IDH_PNL_RESLWND_CL_MRDB              5658
#define  IDH_PNL_RESLWND_CL_AAS               5660
#define  IDH_PNL_RESLWND_CL_MODTYPE           5662
#define  IDH_PNL_RESLWND_CL_DESCRIP           5664
#define  IDH_PNL_RESLWND_CL_CRAD              5666
#define  IDH_PNL_RESLWND_CL_INSTDT            5668
/* #define  IDH_PNL_RESLWND_CL_SLNUM             5670  */

#define  IDH_PNL_RESLWND_MENU_SELECTED        5676

/*--------------------------------------------*/
/* Software Discrepancies Window              */
/*--------------------------------------------*/
#define  IDH_TBL_SWDWND                      6550
#define  IDH_PNL_SWDWND                      6551
#define  IDH_PNL_SWDWND_CNR                  6552

/*--------------------------------------------*/
/* Generic Window                             */
/*--------------------------------------------*/
#define  IDH_TBL_GENRWND                      5800
#define  IDH_PNL_GENRWND                      5801

/*--------------------------------------------*/
/* RPO Window                                 */
/*--------------------------------------------*/
#define  IDH_TBL_RPOSWND                      6400
#define  IDH_PNL_RPOSWND                      6401


#define  IDH_PNL_RPOSWND_LB_RPOS              6418
#define  IDH_PNL_RPOSWND_CNR_RPOS             6419
#define  IDH_PNL_RPOSWND_PB_CANCEL            6420
#define  IDH_PNL_RPOSWND_PB_DELETE            6422
#define  IDH_PNL_RPOSWND_PB_HELP              6424
#define  IDH_PNL_RPOSWND_PB_SAVE              6426
#define  IDH_PNL_RPOSWND_POP_DELETE           6428
#define  IDH_PNL_RPOSWND_POP_SAVE             6430
#define  IDH_PNL_RPOSWND_POP_RESTORE          6432
#define  IDH_PNL_RPOSWND_POP_SORT             6434
#define  IDH_PNL_RPOSWND_POP_SORT_UP          6436
#define  IDH_PNL_RPOSWND_POP_SORT_DOWN        6438

/*--------------------------------------------*/
/* Explain Window                             */
/*--------------------------------------------*/
#define  IDH_TBL_XPLNWND                      6000
#define  IDH_PNL_XPLNWND                      6001

/*--------------------------------------------*/
/* Print Preview Window                       */
/*--------------------------------------------*/
#define  IDH_TBL_PPRVW                       6540
#define  IDH_PNL_PPRVW                       6541

/*--------------------------------------------*/
/* Profile Window                             */
/*--------------------------------------------*/
#define  IDH_TBL_PROFWND                      6600
#define  IDH_PNL_PROFWND                      6601

#define IDH_PNL_PROFWND_GB_DSPLY              6602 // display options
#define   IDH_PNL_PROFWND_RB_DSPLYALL         6607 // display all resolutions
#define   IDH_PNL_PROFWND_RB_DSPLYDSCRP       6608 // display user-applied discrepancies only
#define IDH_PNL_PROFWND_PB_SAVE               6603
#define IDH_PNL_PROFWND_PB_CANCEL             6604
#define IDH_PNL_PROFWND_PB_DEFAULT            6605
#define IDH_PNL_PROFWND_PB_RESET              6606
#define IDH_PNL_PROFWND_EF_WORKDIR            6609 // working directory
#define IDH_PNL_PROFWND_CB_GENRPO             6610 // Generate RPOs

/*--------------------------------------------*/
/* RPO Information Window                     */
/*--------------------------------------------*/
#define IDH_TBL_RPOINFOWND                           6700
#define IDH_PNL_RPOINFOWND                           6701

#define IDH_PNL_GRP_USER_INFORMATION                 6702
#define   IDH_PNL_EF_USER_NAME                       6703
#define   IDH_PNL_EF_TIE_LINE                        6704
#define   IDH_PNL_EF_VM_USER_ID                      6705
#define   IDH_PNL_EF_VM_NODE_ID                      6706
#define IDH_PNL_GRP_FEDERAL_CUSTOMER                 6707
#define   IDH_PNL_RB_FEDCUST_YES                     6708
#define   IDH_PNL_RB_FEDCUST_NO                      6709
#define IDH_PNL_EF_SPECIAL_CONTRACT_NUMBER           6710
#define IDH_PNL_GRP_EDUCATIONAL_ALLOWANCE_APPLIES    6711
#define   IDH_PNL_RB_EDALLOW_YES                     6712
#define   IDH_PNL_RB_EDALLOW_NO                      6713
#define IDH_PNL_GRP_CUSTOMER_STATUS                  6714
#define   IDH_PNL_RB_CUSTSTATUS_OWNING               6715
#define   IDH_PNL_RB_CUSTSTATUS_USING                6716
#define IDH_PNL_GRP_MACHINE_STATUS                   6717
#define   IDH_PNL_RB_MACSTATUS_LEASE                 6718
#define   IDH_PNL_RB_MACSTATUS_PURCHASEMAINTENANCE   6719
#define   IDH_PNL_RB_MACSTATUS_PURCHASEPER_CALL      6720

/*- RPO Submit information window ------------*/
/*- F73686  HECC: 10/05/2002      ------------*/
#define IDH_PNL_RPOSWND_PB_SUBMIT                    6721

/*--------------------------------------------*/
/* Upload Window                              */
/*--------------------------------------------*/
#define IDH_TBL_UPL_DLG                              6800 // use one panel for
#define IDH_PNL_UPL_DLG                              6801 // this whole window

#define IDH_PNL_UPL_MSGBOX                           6850 // forward RPOs?
#define IDH_PNL_MES_RPO                              6860 // Help 44853 erg 113099
#define  IDH_TBL_MMWND                        6900
#define  IDH_PNL_MMWND                        6901


#define  IDH_PNL_MSGBOX_OLD_MRDB              6910

/*--------------------------------------------*/
/* Miscellaneous                              */
/*--------------------------------------------*/
#define  IDH_PNL_KEYS                         6960

/*--------------------------------------------*/
/* Mismatches Type Acceptable/Unacceptable    */
/* F106814   HECC - 03/10/2006                */
/*--------------------------------------------*/

#define  IDH_TBL_ACCEPTABLE                   6971
#define  IDH_PNL_ACCEPTABLE                   6972
#define  IDH_TBL_UNACCEPTABLE                 6973
#define  IDH_PNL_UNACCEPTABLE                 6974

/*--------------------------------------------*/
/* Backlog Window                             */
/*--------------------------------------------*/
#define  IDH_TBL_BKLGWND                      7100
#define  IDH_PNL_BKLGWND                      7101
#define  IDH_PNL_BKLGWND_CNR                  7102

#endif
/***************Current above *********************/
/***************Old     below *********************/
/*------------------------------------------------*/
/* These old help ids are being left here as a    */
/* guide to determine what needs to be removed    */
/* from the old help file. Any reference in the   */
/* help file to one of the ids here should be     */
/* removed from the help file.                    */
/*------------------------------------------------*/
#if 0 // old help ids

#define  IDH_TBL_INITWND                      5200
#define  IDH_PNL_INITWND                      5201

#define  INITWND_PB_CANCEL                    5202
#define  INITWND_PB_HELP                      5204
#define  IDH_PNL_INITWND_PB_CANCEL            5206
#define  IDH_PNL_INITWND_PB_HELP              5208

#define  DISCWND_CL_BASE                      5402
#define  DISCWND_CL_MRDB                      5404
#define  DISCWND_CL_AAS                       5406
#define  DISCWND_CL_MODTYPE                   5408
#define  DISCWND_CL_DESCRIP                   5410
#define  DISCWND_CL_CRAD                      5412
#define  DISCWND_CL_INSTDT                    5414
#define  DISCWND_CL_SLNUM                     5416

#define  DISCWND_MENU_SELECTED                5422
#define  DISCWND_SMENU_EXPLAIN                5424
#define  DISCWND_SMENU_APPLY                  5428
#define  DISCWND_SMENU_EDBASE                 5432

#define  DISCWND_MENU_FILE                    5417
#define  DISCWND_SMENU_SAVE                   5418
#define  DISCWND_SMENU_EXIT                   5419
#define  DISCWND_SMENU_QUIT                   5420
#define  DISCWND_SMENU_PROFILE                5421

#define  DISCWND_SMENU_EDIT                   5426
#define  DISCWND_SMENU_IGNORE                 5430
#define  DISCWND_SMENU_EDCRAD                 5434
#define  DISCWND_SMENU_EDINSTDT               5436
#define  DISCWND_SMENU_EDSLNUM                5438
#define  DISCWND_MENU_VIEW                    5440
#define  DISCWND_SMENU_VDISC                  5442
#define  DISCWND_SMENU_VRESOL                 5444
#define  DISCWND_SMENU_VDYNLK                 5446
#define  DISCWND_SMENU_VRPOS                  5448
#define  DISCWND_PB_SAVE                      5450
#define  DISCWND_PB_EXIT                      5452
#define  DISCWND_PB_QUIT                      5454
#define  DISCWND_PB_HELP                      5456

#define  IDH_PNL_DISCWND_SMENU_EDIT           5484
#define  IDH_PNL_DISCWND_SMENU_IGNORE         5488
#define  IDH_PNL_DISCWND_SMENU_EDCRAD         5492
#define  IDH_PNL_DISCWND_SMENU_EDINSTDT       5494
#define  IDH_PNL_DISCWND_SMENU_EDSLNUM        5496

#define  IDH_PNL_DISCWND_SMENU_SAVE           5476
#define  IDH_PNL_DISCWND_SMENU_EXIT           5477
#define  IDH_PNL_DISCWND_SMENU_QUIT           5478
#define  IDH_PNL_DISCWND_SMENU_PROFILE        5479

#define  IDH_PNL_DISCWND_MENU_VIEW            5498
#define  IDH_PNL_DISCWND_SMENU_VDISC          5500
#define  IDH_PNL_DISCWND_SMENU_VRESOL         5502
#define  IDH_PNL_DISCWND_SMENU_VDYNLK         5504
#define  IDH_PNL_DISCWND_SMENU_VRPOS          5506

#define  IDH_PNL_DISCWND_PB_SAVE              5508
#define  IDH_PNL_DISCWND_PB_EXIT              5510
#define  IDH_PNL_DISCWND_PB_QUIT              5512
#define  IDH_PNL_DISCWND_PB_HELP              5514

#define  RESLWND_MENU_FILE                    5617
#define  RESLWND_SMENU_SAVE                   5618
#define  RESLWND_SMENU_EXIT                   5619
#define  RESLWND_SMENU_QUIT                   5620
#define  RESLWND_SMENU_PROFILE                5621
#define  RESLWND_SMENU_EDIT                   5626
#define  RESLWND_SMENU_IGNORE                 5628
#define  RESLWND_SMENU_EDCRAD                 5632
#define  RESLWND_SMENU_EDINSTDT               5634
#define  RESLWND_SMENU_EDSLNUM                5636

#define  RESLWND_MENU_VIEW                    5638
#define  RESLWND_SMENU_VDISC                  5640
#define  RESLWND_SMENU_VRESOL                 5642
#define  RESLWND_SMENU_VDYNLK                 5644
#define  RESLWND_SMENU_VRPOS                  5646

#define  RESLWND_PB_SAVE                      5648
#define  RESLWND_PB_EXIT                      5650
#define  RESLWND_PB_QUIT                      5652
#define  RESLWND_PB_HELP                      5654

#define  RESLWND_CL_BASE                      5602
#define  RESLWND_CL_MRDB                      5604
#define  RESLWND_CL_AAS                       5606
#define  RESLWND_CL_MODTYPE                   5608
#define  RESLWND_CL_DESCRIP                   5610
#define  RESLWND_CL_CRAD                      5612
#define  RESLWND_CL_INSTDT                    5614
#define  RESLWND_CL_SLNUM                     5616

#define  RESLWND_MENU_SELECTED                5622
#define  RESLWND_SMENU_EXPLAIN                5624
#define  RESLWND_SMENU_EDBASE                 5630

#define  IDH_PNL_RESLWND_MENU_FILE            5671
#define  IDH_PNL_RESLWND_SMENU_SAVE           5672
#define  IDH_PNL_RESLWND_SMENU_EXIT           5673
#define  IDH_PNL_RESLWND_SMENU_QUIT           5674
#define  IDH_PNL_RESLWND_SMENU_PROFILE        5675
#define   IDH_PNL_RESLWND_SMENU_EXPLAIN        5678
#define  IDH_PNL_RESLWND_SMENU_EDIT           5680
#define  IDH_PNL_RESLWND_SMENU_IGNORE         5682
#define   IDH_PNL_RESLWND_SMENU_EDBASE         5684
#define  IDH_PNL_RESLWND_SMENU_EDCRAD         5686
#define  IDH_PNL_RESLWND_SMENU_EDINSTDT       5688
#define  IDH_PNL_RESLWND_SMENU_EDSLNUM        5690

#define  IDH_PNL_RESLWND_MENU_VIEW            5692
#define  IDH_PNL_RESLWND_SMENU_VDISC          5694
#define  IDH_PNL_RESLWND_SMENU_VRESOL         5696
#define  IDH_PNL_RESLWND_SMENU_VDYNLK         5698
#define  IDH_PNL_RESLWND_SMENU_VRPOS          5700

#define  IDH_PNL_RESLWND_PB_SAVE              5702
#define  IDH_PNL_RESLWND_PB_EXIT              5704
#define  IDH_PNL_RESLWND_PB_QUIT              5706
#define  IDH_PNL_RESLWND_PB_HELP              5708

#define  IDH_TBL_EDTWND                       6200
#define  IDH_PNL_EDTWND                       6201


#define  IDH_PNL_EDTWND_PB_ADD                6205
#define  IDH_PNL_EDTWND_PB_CANCEL             6210
#define  IDH_PNL_EDTWND_PB_HELP               6215
#define  IDH_PNL_EDTWND_PB_SHOW_HIDE          6220
#define  IDH_PNL_EDTWND_EF_AAS                6225
#define  IDH_PNL_EDTWND_EF_BASE               6230
#define  IDH_PNL_EDTWND_EF_DATE               6235
#define  IDH_PNL_EDTWND_EF_MRDB               6240
#define  IDH_PNL_EDTWND_EF_NMBR               6245
#define  IDH_PNL_EDTWND_EF_TYPE               6250
#define  IDH_PNL_EDTWND_MLE                   6255

#define  IDH_TBL_MMWND                        6900
#define  IDH_PNL_MMWND                        6901
#define  IDH_PNL_MMWND_MLE                    6905
#define  IDH_PNL_MMWND_RB1                    6906

#define  GENRWND_PB_CANCEL                    5822
#define  GENRWND_PB_HELP                      5828


#define  IDH_PNL_GENRWND_PB_CANCEL            5850
#define  IDH_PNL_GENRWND_PB_HELP              5856

#define  IDH_PNL_XPLNWND_PB_CANCEL            6006
#define  IDH_PNL_XPLNWND_PB_HELP              6008


#define  IDH_PNL_PROFWND_RB_DISPOPT           6656
#define  IDH_PNL_PROFWND_RB_DISP_ALL          6658
#define  IDH_PNL_PROFWND_RB_DISP_DISC         6660

#define  IDH_PNL_PROFWND_RB_PROCOPT           6662
#define  IDH_PNL_PROFWND_RB_PROC_GRPOS        6664
#define  IDH_PNL_PROFWND_RB_PROC_GRPOSY       6666
#define  IDH_PNL_PROFWND_RB_PROC_GRPOSYL      6668
#define  IDH_PNL_PROFWND_RB_PROC_GRPOSYA      6670
#define  IDH_PNL_PROFWND_RB_PROC_GRPOSN       6672
#define  IDH_PNL_PROFWND_RB_PROC_APPLY        6674
#define  IDH_PNL_PROFWND_RB_PROC_APPLYY       6676
#define  IDH_PNL_PROFWND_RB_PROC_APPLYN       6678
#define  IDH_PNL_PROFWND_RB_PROC_DISPAE       6680
#define  IDH_PNL_PROFWND_RB_PROC_DISPAEY      6682
#define  IDH_PNL_PROFWND_RB_PROC_DISPAEN      6684

#define  IDH_PNL_PROFWND_CB_VIEWOPT           6686
#define  IDH_PNL_PROFWND_CB_VIEW_DISC         6688
#define  IDH_PNL_PROFWND_CB_VIEW_PROF         6690
#define  IDH_PNL_PROFWND_CB_VIEW_EDT          6692
#define  IDH_PNL_PROFWND_CB_VIEW_RESOL        6694
#define  IDH_PNL_PROFWND_CB_VIEW_GRPO         6696
#define  IDH_PNL_PROFWND_CB_VIEW_DYNIL        6698

#define  IDH_PNL_PROFWND_PB_APPLY             6700
#define  IDH_PNL_PROFWND_PB_DEFAULT           6702
#define  IDH_PNL_PROFWND_PB_RESET             6704
#define  IDH_PNL_PROFWND_PB_CANCEL            6706
#define  IDH_PNL_PROFWND_PB_HELP              6708

#define  IDH_PNL_CNTXT_KEYS                   6961
#define  IDH_PNL_CNTXT_ABOUT                  6962
#endif // old help ids
