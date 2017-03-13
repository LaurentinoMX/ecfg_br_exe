#ifndef __PRINTER_H
#define __PRINTER_H

/* constant to convert millimeters to inch  */
const float MM_TO_INCH = 0.03937007;
const float CM_TO_INCH = 0.3937007;
const float INCH_TO_MM = 25.4;
const float INCH_TO_CM = 2.54;

#define PRINTER_SELECT       5000
#define WND_CANVAS           5100
#define WND_BUTTONS          5110
#define LB_PRINTERS          5200


#endif

/*****************************************************************************
    This is what the PRQINFO3 structure looks like                  

typedef struct _PRQINFO3 {
PSZ          pszName;         Queue name  
USHORT       uPriority;       Queue priority  
USHORT       uStartTime;      Minutes after midnight when queue becomes active  
USHORT       uUntilTime;      Minutes after midnight when queue ceases to be active  
USHORT       fsType;          Queue type  
PSZ          pszSepFile;      Separator-page file  
PSZ          pszPrProc;       Default queue-processor  
PSZ          pszParms;        Queue parameters  
PSZ          pszComment;      Queue description  
USHORT       fsStatus;        Queue status  
USHORT       cJobs;           Number of jobs in queue  
PSZ          pszPrinters;     Print devices connected to queue  
PSZ          pszDriverName;   Default device driver  
PDRIVDATA    pDriverData;     Default queue job properties  
 } PRQINFO3;
******************************************************************************/
