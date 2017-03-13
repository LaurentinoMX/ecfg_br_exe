/* --------------------------------------------------------------------
                             Controls DLL
                              Chapter 10

                    Real World Programming for OS/2
             Copyright (c) 1993 Blain, Delimon, and English
-------------------------------------------------------------------- */

/* Bevel Control
     Class Name  - BEVELCLASS
     Styles
        BVS_BEVELIN     - Draw bevel inwards
        BVS_BEVELOUT    - Draw bevel outwards
        BVS_RECTANGLE   - Draw a beveled rectangle
        BVS_LINE        - Draw a beveled trough or fence
        BVS_FILL        - Fill interior
        BVS_PREVID      - Size bevel to encompass all previous sibling
                          windows until window with specified ID is reached.
                          The windows ID is specified as the ID of the
                          window to stop at or'd with BVS_IDMASK
                          x Position is horizontal gap between bevel and windows
                          y Position is vertical gap between bevel and windows
                          cx is border width (0 is nominal width border)
        BVS_PREVWINDOW  - Size bevel to encompass the previous sibling window
                          x Position is horizontal gap between bevel and windows
                          y Position is vertical gap between bevel and windows
                          cx is border width (0 is nominal width border)
     Messages
        BVM_SETCOLOR     - mp1 MAKELONG(bordercolorindex, interiorcolorindex)
                           mp2 0L
        BVM_SETTHICKNESS - mp1 MPFROMSHORT (bevelthickness)
        BVM_RESIZE       - mp1 MAKELONG(horizontalgap, verticalgap)

   Thermometer control
     Class Name  - THERMOMETER CLASS
     Messages
        THM_SETRANGE    - mp1 Low value of range
                          mp2 High value of range
        THM_SETVALUE    - mp1 Current value
        THM_SETCOLOR    - mp1 MAKELONG(fillcolorindex, backgroundcolorindex)
*/

#define INCL_DOS
#define INCL_DOSERRORS
#define  INCL_WINSHELLDATA
#define INCL_DOSDATETIME
#define INCL_DOSINFOSEG
#define INCL_WIN
#define INCL_GPI
#define INCL_GPIPRIMITIVES
#define INCL_GPIPATHS

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stddef.h>
#include <time.h>

#include "pmwin9.h"
#include "alctls.h"

/* Window functions */

MRESULT EXPENTRY BevelWndProc (HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY ThermometerWndProc (HWND,ULONG,MPARAM,MPARAM);
MRESULT EXPENTRY ProgressBarWndProc (HWND,ULONG,MPARAM,MPARAM);
MRESULT EXPENTRY TimerWndProc (HWND,ULONG,MPARAM,MPARAM);
MRESULT EXPENTRY CounterWndProc (HWND,ULONG,MPARAM,MPARAM);
MRESULT EXPENTRY SpinDateWndProc (HWND,ULONG,MPARAM,MPARAM);

/* Undocumented WinDrawBorder flags */

#define DB_RAISED       0x0400
#define DB_DEPRESSED    0x0800
#define DB_TROUGH       0x1000
#define DB_FENCE        0x2000   // great looking - but equeals standard in 2.1
#define DB_FIELD        0x4000
#define DB_CORNERBORDER 0x8000

/* Shared data placed into initialized variables data segment */

HMODULE hModDll = 0;

/* Bevel window extra bytes */

#define QWL_COLORS          0
#define QWS_THICKNESS       QWL_COLORS    + sizeof(ULONG)
#define BEVELEXTRA          QWS_THICKNESS + sizeof(USHORT)

/* Thermometer window extra bytes */

#define QWL_FIRST           0
#define QWL_SECOND          QWL_FIRST       + sizeof(ULONG)
#define QWL_THIRD           QWL_SECOND      + sizeof(ULONG)
#define QWL_FOURTH          QWL_THIRD       + sizeof(ULONG)
#define QWL_FIFTH           QWL_FOURTH      + sizeof(ULONG)
#define QWL_SIXTH           QWL_FIFTH       + sizeof(ULONG)
#define QWL_SEVENTH         QWL_SIXTH       + sizeof(ULONG)
#define QWL_EIGHTH          QWL_SEVENTH     + sizeof(ULONG)
#define QWL_NINETH          QWL_EIGHTH      + sizeof(ULONG)
#define QWL_TENTH           QWL_NINETH      + sizeof(ULONG)
#define QWL_ELEVENTH        QWL_TENTH       + sizeof(ULONG)


#define QWL_LOWER           QWL_FIRST
#define QWL_UPPER           QWL_SECOND
#define QWL_VALUE           QWL_THIRD
#define QWL_COLOR_FORE      QWL_FOURTH
#define QWL_COLOR_AFT       QWL_FIFTH
#define QWL_DELTA           QWL_SIXTH
#define QWL_BORDERSTYLE     QWL_SEVENTH
#define QWL_CALL_BACK_HNDL  QWL_EIGHTH
#define QWL_CALL_BACK_MSG   QWL_NINETH
#define QWL_MIN_SIZE        QWL_TENTH
#define PROGRESSEXTRA       QWL_ELEVENTH

#define QWL_MINUTES         QWL_LOWER
#define QWL_SECONDS         QWL_UPPER

#define QWL_DATE_OBJ        QWL_LOWER


#define QWL_DIGIT_COUNT     QWL_LOWER


#define TIMERMARGIN 1L
#define TIMER_UP    1L
#define TIMER_DOWN  2L
#define TIMER_DELAY 3L

#define COUNTERMARGIN 1L

char *pPRF_PM_NTL                = "PM_National";

#define ALMAX(a,b)        (((a) > (b)) ? (a) : (b))
#define ALMIN(a,b)        (((a) < (b)) ? (a) : (b))
#define ALLEAP(A) ( (((A)%4) || (!((A)%400)) )? 0: 1 )


typedef struct tagSPB_OBJ
{
 HWND    hwnd;
 HWND    hwndEF;
 LONG    cx;
 ULONG   id;
 ULONG   Upper;
 ULONG   Lower;
} SPB_OBJ;
typedef SPB_OBJ *PSPB_OBJ;

typedef struct tagDATE_OBJ
{
  ULONG        ulM;
  ULONG        ulD;
  ULONG        ulY;
  ULONG        id;
  HWND         hwnd;
  HWND         hwndOwner;
  RECTL        rctReq;
  RECTL        rctBorder;
  LONG         cy;
  SPB_OBJ      SPB[3];
  BOOL         fText;
  CHAR       * Months[12];
  CHAR         Names[12][101];
} DATE_OBJ;
typedef DATE_OBJ *PDATE_OBJ;

static int DaysMonth[2][12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
                                31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static  CHAR *DefaultArr[12]= {
                               "January",
                               "February",
                               "March",
                               "April",
                               "May",
                               "June",
                               "July",
                               "August",
                               "September",
                               "October",
                               "November",
                               "December"
                             };


#ifdef __IBMC__
ULONG _System _DLL_InitTerm (HMODULE hModule, ULONG ulTerminating)
#else
ULONG _stdcall InitializeControlDll (HMODULE hModule, ULONG ulTerminating)
#endif
{
    BOOL bResult;

    if (ulTerminating & 0x01)
        return (FALSE);
    else
    {
        /* Register the window control classes */
        bResult =
            WinRegisterClass (
                (HAB)0, BEVELCLASS, BevelWndProc, CS_HITTEST | CS_SIZEREDRAW, BEVELEXTRA) &&
            WinRegisterClass (
                (HAB)0, THERMOMETERCLASS, ThermometerWndProc, CS_HITTEST | CS_SIZEREDRAW, PROGRESSEXTRA) &&
            WinRegisterClass (
                (HAB)0, MINSECTIMERCLASS, TimerWndProc,CS_HITTEST |  CS_SIZEREDRAW, PROGRESSEXTRA) &&
            WinRegisterClass (
                (HAB)0, COUNTERCLASS, CounterWndProc,CS_HITTEST |  CS_SIZEREDRAW, PROGRESSEXTRA) &&
            WinRegisterClass (
                (HAB)0, SPINDATECLASS, SpinDateWndProc, CS_SIZEREDRAW, PROGRESSEXTRA) &&
            WinRegisterClass (
                (HAB)0, PROGRESSBARCLASS, ProgressBarWndProc,CS_HITTEST |  CS_SIZEREDRAW, PROGRESSEXTRA);
    }

    /* Save the module handle when we get loaded.  The module handle
       will be the same for all instances of the DLL */

    hModDll = hModule;

    return ((ULONG)bResult);
}


/* --- lGetPresParam ---------------------------------- [ Private } --- */
/*                                                                      */
/*     This function is used to retrieve a presentation parameter       */
/*     that may be present.  If the presentation parameter is not,      */
/*     the default colour passed to the function will be used.          */
/*                                                                      */
/*     Upon Entry:                                                      */
/*                                                                      */
/*     HWND  hWnd;     = Window Handle                                  */
/*     ULONG ulID1;    = Presentation Parameter 1 ID                    */
/*     ULONG ulID2;    = Presentation Parameter 2 ID                    */
/*     LONG  lDefault; = Default Colour                                 */
/*                                                                      */
/*     Upon Exit:                                                       */
/*                                                                      */
/*     lGetPresParam = Colour to Use                                    */
/*                                                                      */
/* -------------------------------------------------------------------- */

static LONG lGetPresParam(HWND hWnd, ULONG ulID1, ULONG ulID2, LONG lDefault)

{
HPS   hPS;                         /* Presentation Space Handle         */
LONG  lClr;                        /* Presentation Parameter Colour     */
ULONG ulID;                        /* Presentation Parameter ID         */

if ( WinQueryPresParam(hWnd, ulID1, ulID2, &ulID, 4, (PVOID)&lClr,
                       QPF_NOINHERIT | QPF_ID1COLORINDEX | QPF_ID2COLORINDEX | QPF_PURERGBCOLOR) )
   return(lClr);
else
   if ( (lDefault >= SYSCLR_SHADOWHILITEBGND) &&
        (lDefault <= SYSCLR_HELPHILITE) )
       return(WinQuerySysColor(HWND_DESKTOP, lDefault, 0L));
   else
       if ( (lClr = GpiQueryRGBColor(hPS = WinGetPS(hWnd),
                                     LCOLOPT_REALIZED, lDefault)) == GPI_ALTERROR )
           {
           WinReleasePS(hPS);
           return(lDefault);
           }
       else
           {
           WinReleasePS(hPS);
           return(lClr);
           }
}


ULONG MyULPower(ULONG ul, ULONG to)
{
 ULONG result = 1L;
 while (to--)
   result *= ul;
 return result;
}
PSZ MyULONGToSZLeading(ULONG ul, PSZ psz, ULONG Digits)
{
 PCHAR pc;
 ULONG ul10s,ul2;
 for (Digits = ALMIN(Digits,10); ul/MyULPower(10,Digits); ul/=10);
 for (pc = psz; Digits ; Digits--, pc++)
   { ul10s = MyULPower(10,Digits-1);
     ul2 = ul/ul10s ;
     *pc = '0' + ul2;
     ul -= (ul2*ul10s);
   }
 *pc = '\0';
 return psz;
}

PSZ MyULONGToSZ(ULONG ul, PSZ psz)
{
 PCHAR pc;
 ULONG Digits,ul10s,ul2;
 for (Digits = 10; ul/MyULPower(10,Digits); ul/=10);
 for (pc = psz; Digits ; Digits--)
   { ul10s = MyULPower(10,Digits-1);
     if ( (ul2 = ul/ul10s) || (pc != psz))
        *pc++ = '0' + ul2;
     ul -= (ul2*ul10s);
   }
 if (pc==psz)
   *pc++ = '0';
 *pc = '\0';
 return psz;
}

PSZ Mystrncpy(PCHAR Buf, PSZ psz,ULONG ulLen)
{
 PCHAR pc;
 if (Buf && psz && ulLen)
   { for (pc = Buf;ulLen && *psz;ulLen--)
       *pc++ = *psz++;
     *pc = '\0';
   }
 return Buf;
}

ULONG Mystrlen(PSZ psz)
{
 ULONG ul;
 for (ul = 0L; *psz++;)
   ul++;
 return ul;
}

PSZ  EXPENTRY GetLastPMErrData(hab, szErrorData, usStringLen)
HAB    hab;             /* The Anchor Block for Obtaining the last error */
PSZ    szErrorData;          /* storage location to pu the error data string */
USHORT usStringLen;        /* length of available storage for error data    */
                           /* string should be at least 200 bytes           */
{
  PERRINFO             perriBlk;
  PSZ                  pszErrMsg = NULL;
  USHORT               *pusTemp;
  ERRORID              Last;
  USHORT               Severity;
  USHORT               Error;

  szErrorData[0] = '\0';
  if (usStringLen < 200 )
    return(szErrorData);

  Last = WinGetLastError(hab);

  Error    = ERRORIDERROR(Last);
  Severity = ERRORIDSEV(Last);

  Mystrncpy(szErrorData,"Error ID: ",12);
  MyULONGToSZ((ULONG)Error,szErrorData + Mystrlen(szErrorData));
  Mystrncpy(szErrorData + Mystrlen(szErrorData),". Severity: ",20);
  switch(Severity)
     {
      case  SEVERITY_WARNING:
            Mystrncpy(szErrorData + Mystrlen(szErrorData)," WARNING",20);
            break;
      case  SEVERITY_ERROR:
            Mystrncpy(szErrorData + Mystrlen(szErrorData)," ERROR",20);
            break;
      case  SEVERITY_SEVERE:
            Mystrncpy(szErrorData + Mystrlen(szErrorData)," SEVERE",20);
            break;
      case  SEVERITY_UNRECOVERABLE:
            Mystrncpy(szErrorData + Mystrlen(szErrorData)," UNRECOVERABLE",20);
            break;
         case SEVERITY_NOERROR:
            Mystrncpy(szErrorData + Mystrlen(szErrorData)," NONE No Error Text",30);
            return(szErrorData);
            break;
      default:
            Mystrncpy(szErrorData + Mystrlen(szErrorData)," NONE SPECIFIED",30);
            break;

     }

  Mystrncpy(szErrorData + Mystrlen(szErrorData),". Text: ",30);

  perriBlk = WinGetErrorInfo(hab);        /* Get pointer to error structure */

  if (perriBlk && (Severity != SEVERITY_UNRECOVERABLE))
     {
          // perriBlk->offaoffszMsg is the offset to a pointer to a short which
          // is the offset to the string
      pszErrMsg = MAKEP(SELECTOROF(perriBlk),perriBlk->offaoffszMsg);
      Mystrncpy(szErrorData + Mystrlen(szErrorData),pszErrMsg,100);
     }

  if (perriBlk)
      WinFreeErrorInfo(perriBlk);  /* Free the Error structure's storage */

  Last = WinGetLastError(hab);  // for codeview to see if error condition  is
  return(szErrorData);           // reset to 'errorfree'
}


VOID EXPENTRY CenterWindow (HWND hWnd)
{
    ULONG ulScrWidth, ulScrHeight;
    RECTL Rectl;

    ulScrWidth  = WinQuerySysValue (HWND_DESKTOP, SV_CXSCREEN);
    ulScrHeight = WinQuerySysValue (HWND_DESKTOP, SV_CYSCREEN);
    WinQueryWindowRect (hWnd, &Rectl);
    WinSetWindowPos (hWnd, HWND_TOP, (ulScrWidth-Rectl.xRight)/2,
        (ulScrHeight-Rectl.yTop)/2, 0, 0, SWP_MOVE | SWP_ACTIVATE);
    return;
}

VOID EXPENTRY ForceLink(VOID)
{
 return;
}

ULONG PixInString(HPS hPS,PSZ szString)
{
 POINTL ptl = {0,0};                //postition of a point in long coordinates
 PPOINTL pptl;              // (far) pointer to a POINTL stgructure
 ULONG Length,
       Pixels;

 ptl.x = ptl.y = 0;              // init the starting position
 Length = Mystrlen(szString);
 if (DosAllocMem ((PPVOID)&pptl,sizeof(POINTL)*(Length + 1),fALLOC))
    return 0L;
 GpiQueryCharStringPosAt(hPS,
                         (PPOINTL)&ptl,
                         0L,
                         (LONG)Length,
                         szString,
                         0L,
                         pptl);

  Pixels = pptl[Length].x - pptl[0].x;
  DosFreeMem (pptl);
  free(pptl);
  return(Pixels);
}

VOID TextBox(HPS hps,PSZ psz, PLONG plWidth,PLONG plHeight)
{
 POINTL      aptl[TXTBOX_COUNT];
 GpiQueryTextBox (hps,Mystrlen(psz),psz,TXTBOX_COUNT,aptl);
 if (plWidth)
    *plWidth = aptl[TXTBOX_TOPRIGHT].x - aptl[TXTBOX_TOPLEFT].x ;
 if (plHeight)
    *plHeight = aptl[TXTBOX_TOPLEFT].y - aptl[TXTBOX_BOTTOMLEFT].y ;
}

void MyBorder(HPS hps, PRECTL prctl, LONG width, BOOL fDraw)
{
// prctl->xRight --;
// prctl->yTop --;
 if (fDraw)
   { LONG light = width/2;
     LONG dark = width-light;
     RECTL rctl = *prctl;
     if (light)
       { rctl.yTop -= light ;
         rctl.xLeft += light;
         WinDrawBorder(hps,&rctl,light,light,SYSCLR_BUTTONLIGHT,
                       SYSCLR_BUTTONDARK, DB_PATCOPY);
         rctl.yTop += light;
         rctl.yBottom += light;
         rctl.xRight -= light ;
         rctl.xLeft -=  light;
       }
     WinDrawBorder(hps,&rctl,dark,dark,SYSCLR_BUTTONDARK,SYSCLR_BUTTONLIGHT,
                   DB_PATCOPY);
  }
 WinInflateRect((HAB)0L, prctl, -(width+1),-(width+1));
// prctl->xRight ++;
// prctl->xLeft--;
// prctl->xLeft--;
// prctl->yBottom++;
// prctl->yTop ++;
}

VOID My3DBorder(HPS hps, PRECTL prctl,LONG width, BOOL fDraw, BOOL fInni)
{
 if (width < 3)
  { MyBorder(hps,prctl,width,fDraw);
    return;
  }
 prctl->xRight--;
 prctl->yTop--;
 if (fDraw)
   {
     POINTL    ptl;
     RECTL  rctl = *prctl
          , rctltemp = rctl;
     LONG   lColorNW = fInni? SYSCLR_BUTTONDARK: SYSCLR_BUTTONLIGHT
           ,lColorSE = fInni? SYSCLR_BUTTONLIGHT: SYSCLR_BUTTONDARK
           ,size = width - 1;

      WinDrawBorder (hps, prctl, 2L, 2L,SYSCLR_BUTTONLIGHT,SYSCLR_BUTTONLIGHT,
                     DB_PATCOPY);

      GpiSetColor(hps, SYSCLR_BUTTONDARK);
    //GpiSetColor(hps, CLR_DARKGRAY);
    //GpiSetColor(hps, CLR_BLACK);



      ptl.x = rctl.xLeft;
      ptl.y = rctl.yBottom;
      GpiMove(hps, &ptl);
      ptl.x = rctl.xRight;
      ptl.y = rctl.yTop;
      GpiBox(hps, DRO_OUTLINE, &ptl, 0L, 0L);
      WinInflateRect((HAB)0L, &rctltemp, -1,-1);
      rctl = rctltemp;
      rctl.xLeft++;

     rctl.xRight = rctl.xLeft + size;
     WinFillRect(hps, &rctl, lColorNW);
     rctl.yBottom = rctl.yTop - size;
     rctl.xRight = rctltemp.xRight;
     WinFillRect(hps, &rctl, lColorNW );

     rctl = rctltemp;
     ptl.x = rctl.xLeft;
     ptl.y = rctl.yBottom;
     GpiSetColor(hps, lColorSE);
     GpiBeginPath(hps, 1);
     GpiMove(hps, &ptl);
     ptl.x += size;
     ptl.y += size;
     GpiLine(hps, &ptl);                   //NE
     ptl.x = rctl.xRight - size;
     GpiLine(hps, &ptl);                   //E
     ptl.y = rctl.yTop - size;
     GpiLine(hps, &ptl);                   //N
     ptl.x = rctl.xRight;
     ptl.y = rctl.yTop;
     GpiLine(hps, &ptl);                   //NE
     ptl.y = rctl.yBottom;
     GpiLine(hps, &ptl);                   //S
     ptl.x = rctl.xLeft;
     GpiLine(hps, &ptl);                   //W
     GpiEndPath(hps);
     GpiFillPath(hps, 1, FPATH_ALTERNATE);

     GpiSetColor(hps, SYSCLR_BUTTONMIDDLE);
     ptl.x = rctl.xLeft + size;
     ptl.y = rctl.yBottom + size;
     GpiMove(hps, &ptl);
     ptl.x = rctl.xRight - size;
     ptl.y = rctl.yTop - size;
     GpiBox(hps, DRO_OUTLINE, &ptl, 0L, 0L);

     GpiSetColor(hps, SYSCLR_BUTTONMIDDLE);
     if (fInni)
       { ptl.y = rctl.yBottom + size;
         GpiMove(hps, &ptl);
         ptl.x += size;
         ptl.y -= size;
         GpiLine(hps, &ptl);                   //SE
       }
     else { ptl.x = rctl.xLeft + size;
            GpiMove(hps, &ptl);
            ptl.x -= size;
            ptl.y += size;
            GpiLine(hps, &ptl);                //NW
          }
   }
// prctl->xLeft++;
 WinInflateRect((HAB)0L, prctl, -(width+1),-(width+1));
//kludge

// prctl->yBottom++;
  prctl->yTop++;
  prctl->xRight++;
//  prctl->xLeft--;
}






VOID QueryBorderRect (HWND hWnd, ULONG ulStyle, PRECTL pRectl)
{
    HWND   hWndPrev;
    SWP    Swp;
    USHORT usID;
    RECTL  RectlPrev;

    /* If style is BVS_PREVID then size the bevel to encompass
       all previous sibling windows until a window with the
       specifed ID is located.

       If style is BVS_PREVWINDOW then size the bevel to encompass
       the previous sibling window. */

    WinSetRect ((HAB)0L, pRectl, 0L, 0L, 0L, 0L);

    if (ulStyle & BVS_PREVWINDOW)
    {
        if ((hWndPrev = WinQueryWindow (hWnd, QW_PREV)) != 0)
        {
            WinQueryWindowPos (hWndPrev, &Swp);
            WinSetRect ((HAB)0L, pRectl, Swp.x, Swp.y,
                Swp.x+Swp.cx, Swp.y+Swp.cy);
        }
    }
    else
    {
        hWndPrev = hWnd;
        usID     = WinQueryWindowUShort (hWnd, QWS_ID);
        usID    &= ~BVS_IDMASK;

        /* Union the bounding rectangle of all sibling windows
           until window with ID usID is reached */

        while ((hWndPrev = WinQueryWindow (hWndPrev, QW_PREV)) != 0)
        {
            WinQueryWindowPos (hWndPrev, &Swp);
            WinSetRect ((HAB)0L, &RectlPrev, Swp.x, Swp.y, Swp.x+Swp.cx, Swp.y+Swp.cy);
            WinUnionRect ((HAB)0L, pRectl, pRectl, &RectlPrev);
            if (WinQueryWindowUShort (hWndPrev, QWS_ID) == usID)
                break;
        }
    }
    return;
}

MRESULT EXPENTRY BevelWndProc (HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    BOOL    bHandled = TRUE;
    MRESULT mReturn  = 0;
    RECTL   Rectl;
    HPS     hps;
    ULONG   ulStyle;
    ULONG   ulDrawStyle;
    ULONG   ulColors;
    ULONG   ulThickness;

    switch (msg)
    {
        case WM_CREATE:
            /* Initialize border and interior colors */
            WinSetWindowULong (hWnd, QWL_COLORS,
                MAKELONG (SYSCLR_BUTTONDARK, SYSCLR_BUTTONMIDDLE));

            ulStyle = WinQueryWindowULong (hWnd, QWL_STYLE);
            if (ulStyle & (BVS_PREVID | BVS_PREVWINDOW))
            {
                SWP    SwpBevel;

                /* Query bevel window parameters
                   x Position specifies the horizontal gap between
                     bevel and sibling windows
                   y Position specifies the vertical gap between
                     bevel and sibling windows
                   cx specifies the width of the bevel */

                WinQueryWindowPos (hWnd, &SwpBevel);
                QueryBorderRect (hWnd, ulStyle, &Rectl);

                /* Position the bevel window */
                WinSetWindowPos (hWnd, 0,
                    Rectl.xLeft   - SwpBevel.x,
                    Rectl.yBottom - SwpBevel.y,
                    Rectl.xRight  - Rectl.xLeft   + (SwpBevel.x << 1),
                    Rectl.yTop    - Rectl.yBottom + (SwpBevel.y << 1),
                    SWP_MOVE | SWP_SIZE);

                /* If width is zero use nominal width bevel */
                if (SwpBevel.cx == 0)
                    SwpBevel.cx = 1;
                WinSetWindowUShort (hWnd, QWS_THICKNESS, (USHORT)SwpBevel.cx);
            }
            else
                WinSetWindowUShort (hWnd, QWS_THICKNESS, 1);
            break;

        case BVM_RESIZE:
            ulStyle = WinQueryWindowULong (hWnd, QWL_STYLE);
            if (ulStyle & (BVS_PREVID | BVS_PREVWINDOW))
            {
                RECTL Rectl;

                QueryBorderRect (hWnd, ulStyle, &Rectl);

                /* Position the bevel window */
                WinSetWindowPos (hWnd, 0,
                    Rectl.xLeft   - SHORT1FROMMP(mp1),
                   Rectl.yBottom - SHORT2FROMMP(mp1),
                    Rectl.xRight  - Rectl.xLeft   + (SHORT1FROMMP(mp1) << 1),
                    Rectl.yTop    - Rectl.yBottom + (SHORT2FROMMP(mp1) << 1),
                    SWP_MOVE | SWP_SIZE);
            }
            break;

        case BVM_SETCOLOR:
            /* LOUSHORT(mp1) == Border color
               HIUSHORT(mp1) == Interior color */
            WinSetWindowULong (hWnd, QWL_COLORS, (ULONG)mp1);

            /* Repaint bevel with new color */
            WinInvalidateRect (hWnd, NULL, FALSE);
            break;

        case BVM_SETTHICKNESS:
            WinSetWindowUShort (hWnd, QWS_THICKNESS, SHORT1FROMMP(mp1));
            break;

        case WM_PAINT:
            hps         = WinBeginPaint (hWnd,0,0);
            ulColors    = WinQueryWindowULong (hWnd, QWL_COLORS);
            ulThickness = (ULONG)WinQueryWindowUShort (hWnd, QWS_THICKNESS);
            ulStyle     = WinQueryWindowULong (hWnd, QWL_STYLE) & 0x0000FFFF;

            ulDrawStyle = 0L;
            if (ulStyle & BVS_BEVELOUT)
                if (ulStyle & BVS_LINE)
                    ulDrawStyle |= DB_FENCE;
                else
                    ulDrawStyle |= DB_RAISED;
            else if (ulStyle & BVS_LINE)
                ulDrawStyle |= DB_TROUGH;
            else
                ulDrawStyle |= DB_DEPRESSED;
            if (ulStyle & BVS_FILL)
                ulDrawStyle |= DB_INTERIOR;

            WinQueryWindowRect (hWnd, &Rectl);
            WinDrawBorder (hps, &Rectl, ulThickness, ulThickness,
                LOUSHORT(ulColors), HIUSHORT(ulColors), ulDrawStyle);

            WinEndPaint (hps);
            mReturn = 0L;
            break;

        case WM_HITTEST:
            /* Allow mouse message to pass through this window */
            mReturn = (MRESULT)HT_TRANSPARENT;
            break;


        default:
            bHandled = FALSE;
            break;
    }

    if (!bHandled)
        mReturn = WinDefWindowProc (hWnd,msg,mp1,mp2);

    return (mReturn);
}

ULONG CalculatePercentage (HWND hWnd)
{
    LONG lLowerRange;
    LONG lUpperRange;
    LONG lValue;

    lLowerRange = WinQueryWindowULong (hWnd, QWL_LOWER);
    lUpperRange = WinQueryWindowULong (hWnd, QWL_UPPER);
    lValue      = WinQueryWindowULong (hWnd, QWL_VALUE);
    if (lUpperRange <= lLowerRange)
        return (0L);
    else
        return ( ((lValue - lLowerRange) * 100) / (lUpperRange - lLowerRange) );
}

VOID DrawPercentages (HPS hps, PRECTL pRectl,BOOL fBorder)
{
    ULONG  ulTop;
    ULONG  ulBottom;
    POINTL Ptl;
    RECTL  Rectl;
    ULONG  ulHeight;
//    CHAR szErr[201];
    LONG rc;

//  Rectl.xLeft  = pRectl->xRight * 6 / 10;
    Rectl.xLeft  = pRectl->xRight * 50 / 100;
//  Rectl.xRight = pRectl->xRight * 3 / 4 - 2;
//  Ptl.x        = pRectl->xRight * 3 / 4;
    Rectl.xRight = pRectl->xRight * 3 / 5 - 2;
    Ptl.x        = pRectl->xRight * 3 / 5;
//  Rectl.xRight = pRectl->xRight * 1 / 2 - 2;
//  Ptl.x        = pRectl->xRight * 1 / 2;

    ulTop        = pRectl->yTop * 95 / 100;
//    ulBottom     = pRectl->yTop / 4 + 5;
    ulBottom     = pRectl->yTop / 4 + (fBorder? 8:4);
    ulHeight     = ulTop - ulBottom;

    Rectl.yBottom = ulTop;
    Rectl.yTop    = Rectl.yBottom + 2;
    WinDrawBorder (hps, &Rectl, 1L, 1L, CLR_BLACK, CLR_WHITE, DB_TROUGH);
    Ptl.y         = Rectl.yBottom - 2;
//    GpiSetColor (hps, CLR_BLACK);
    GpiSetColor (hps, CLR_NEUTRAL);
    rc = GpiCharStringAt (hps, &Ptl, 4, "100%");
//    GetLastPMErrData(NULL, szErr, 201);

    Rectl.yBottom = ulBottom + (ulHeight * 9) / 10;
    Rectl.yTop    = Rectl.yBottom + 2;
    WinDrawBorder (hps, &Rectl, 1L, 1L, CLR_BLACK, CLR_WHITE, DB_TROUGH);

    Rectl.yBottom = ulBottom + (ulHeight * 8) / 10;
    Rectl.yTop    = Rectl.yBottom + 2;
//  WinDrawBorder (hps, &Rectl, 1L, 1L, CLR_BLACK, CLR_WHITE, DB_FENCE);
    WinDrawBorder (hps, &Rectl, 1L, 1L, CLR_BLACK, CLR_WHITE, DB_TROUGH );
//   GpiSetColor(hps, SYSCLR_BUTTONMIDDLE);
    Ptl.y         = Rectl.yBottom - 2;
    rc = GpiCharStringAt (hps, &Ptl, 3, "80%");

    Rectl.yBottom = ulBottom + (ulHeight * 7) / 10;
    Rectl.yTop    = Rectl.yBottom + 2;
    WinDrawBorder (hps, &Rectl, 1L, 1L, CLR_BLACK, CLR_WHITE, DB_TROUGH);

    Rectl.yBottom = ulBottom + (ulHeight * 6) / 10;
    Rectl.yTop    = Rectl.yBottom + 2;
    WinDrawBorder (hps, &Rectl, 1L, 1L, CLR_BLACK, CLR_WHITE, DB_TROUGH);
    Ptl.y         = Rectl.yBottom - 2;
    rc = GpiCharStringAt (hps, &Ptl, 3, "60%");

    Rectl.yBottom = ulBottom + (ulHeight * 5) / 10;
    Rectl.yTop    = Rectl.yBottom + 2;
    WinDrawBorder (hps, &Rectl, 1L, 1L, CLR_BLACK, CLR_WHITE, DB_TROUGH);

    Rectl.yBottom = ulBottom + (ulHeight * 4) / 10;
    Rectl.yTop    = Rectl.yBottom + 2;
    WinDrawBorder (hps, &Rectl, 1L, 1L, CLR_BLACK, CLR_WHITE, DB_TROUGH);
    Ptl.y         = Rectl.yBottom - 2;
    rc = GpiCharStringAt (hps, &Ptl, 3, "40%");

    Rectl.yBottom = ulBottom + (ulHeight * 3) / 10;
    Rectl.yTop    = Rectl.yBottom + 2;
    WinDrawBorder (hps, &Rectl, 1L, 1L, CLR_BLACK, CLR_WHITE, DB_TROUGH);

    Rectl.yBottom = ulBottom + (ulHeight * 2) / 10;
    Rectl.yTop    = Rectl.yBottom + 2;
    WinDrawBorder (hps, &Rectl, 1L, 1L, CLR_BLACK, CLR_WHITE, DB_TROUGH);
    Ptl.y         = Rectl.yBottom - 2;
    rc = GpiCharStringAt (hps, &Ptl, 3, "20%");

    Rectl.yBottom = ulBottom + (ulHeight * 1) / 10;
    Rectl.yTop    = Rectl.yBottom + 2;
    WinDrawBorder (hps, &Rectl, 1L, 1L, CLR_BLACK, CLR_WHITE, DB_TROUGH);

    Rectl.yBottom = ulBottom;
    Rectl.yTop    = Rectl.yBottom + 2;
    WinDrawBorder (hps, &Rectl, 1L, 1L, CLR_BLACK, CLR_WHITE, DB_TROUGH);
    Ptl.y         = Rectl.yBottom - 2;
    rc = GpiCharStringAt (hps, &Ptl, 2, "0%");

    return;
}

BOOL APIENTRY DrawTemperature (HPS hPS,PRECTL pRectl,LONG lColor,LONG lBackColor,
                               LONG lPercent)
/*-----------------------------------------------------------------*\

   This function will fill draw the vertical bar portion of the
   thermometer. lColor is used to fill lPercent of the bar starting
   at the top of the bulb. lBackColor is used to fill the remainder
   of the bar. The rectangle identified by pRectl should be identical
   to that supplied to DrawThermometer.

   hPS        Presentation space to render in.
   pRectl     Bounding box of the thermometer
   lColor     Color index used to fill bulb and stem of thermometer.
   lBackColor Color to fill the remainder of the stem (100 - lPercent)
   lPercent   Amount to fill the thermometer. Valid range is 0 to 100.


\*-----------------------------------------------------------------*/
{
    AREABUNDLE ab;
    RECTL      Rectl;
    int        iWidth  = (pRectl->xRight - pRectl->xLeft) / 3;
    int        iBoxHeight = pRectl->yTop - pRectl->yBottom;
    int        iBarHeight = (iBoxHeight * 70) / 100;

    /* Range check the percent */
    if (lPercent > 100)
        lPercent = 100;
    else if (lPercent < 0)
        lPercent = 0;

    Rectl.xLeft = pRectl->xLeft + iWidth;
    Rectl.yTop  = pRectl->yTop - (iBoxHeight * 5) / 100;
    if (lPercent)
        Rectl.yBottom = Rectl.yTop - ((iBarHeight * (100 - lPercent)) / 100);
    else
        Rectl.yBottom = Rectl.yTop - iBarHeight;
    Rectl.xRight  = pRectl->xRight - iWidth - 1;
    Rectl.xLeft++;
//    Rectl.xRight  = pRectl->xRight - iWidth;

    ab.lColor = lBackColor;
    GpiSetAttrs(hPS,PRIM_AREA,ABB_COLOR,0,(PBUNDLE)&ab);
    GpiMove (hPS,(PPOINTL)&Rectl);
    GpiBox (hPS,DRO_FILL,(PPOINTL)&Rectl.xRight,0,0);

    Rectl.yTop = Rectl.yBottom - 1;
    Rectl.yBottom = pRectl->yBottom + (iBoxHeight * 20) / 100;
    ab.lColor = lColor;
    GpiSetAttrs(hPS,PRIM_AREA,ABB_COLOR,0,(PBUNDLE)&ab);
    GpiMove (hPS,(PPOINTL)&Rectl);
    GpiBox (hPS,DRO_FILL,(PPOINTL)&Rectl.xRight,0,0);
    return (TRUE);
}


BOOL APIENTRY DrawThermometer (HPS hPS,PRECTL pRectl,LONG lStyle,LONG lColor,
      LONG lBackColor, LONG lPercent)
/*-----------------------------------------------------------------*\

   This function will draw the outine of a thermometer scaled to
   the supplied rectangle in the give presentation space . The bulb
   of the rectangle is filled with lColor and the remainder is drawn
   by the DrawTemperature function.

   hPS        Presentation space to render in.
   pRectl     Bounding box of the thermometer
   lStyle     Currently undefined
   lColor     Color index used to fill bulb and stem of thermometer.
   lBackColor Color to fill the remainder of the stem (100 - lPercent)
   lPercent   Amount to fill the thermometer. Valid range is 0 to 100.

\*-----------------------------------------------------------------*/
{
    LINEBUNDLE lb;
    AREABUNDLE ab;
    ARCPARAMS  arcparms;
    POINTL     lPtCenter;
    POINTL     lStartPt;
    POINTL     lEndPt;
    POINTL     lPt;
    FIXED      fxStartAngle,fxSweepAngle;
    RECTL      BulbRectl;
    int        iBoxHeight = pRectl->yTop - pRectl->yBottom;

    /* Calculate the height of the bulb to be 25% of the total height */
    BulbRectl = *pRectl;
    BulbRectl.yTop = BulbRectl.yBottom + (iBoxHeight * 25) / 100;

    /* define the bounding box for the arc */
    arcparms.lP = (BulbRectl.xRight - BulbRectl.xLeft) / 2L;
    arcparms.lQ = (BulbRectl.yTop - BulbRectl.yBottom) / 2L;
    arcparms.lR = 0L;
    arcparms.lS = 0L;
    GpiSetArcParams(hPS, (PARCPARAMS)&arcparms);

    /* Calculate the center point of bulb */
    lPtCenter.x = BulbRectl.xLeft + arcparms.lP;
    lPtCenter.y = BulbRectl.yBottom + arcparms.lQ + 1;

    /*-------------------------------------------------------------------*\
                           Draw the bulb
    \*-------------------------------------------------------------------*/
    /* Draw an invisble arc from the current location to the start angle */
    fxStartAngle = MAKEFIXED(120,0);
    fxSweepAngle = MAKEFIXED(90,0);
    lb.usType = LINETYPE_INVISIBLE;
    GpiSetAttrs (hPS,PRIM_LINE,LBB_TYPE,0,(PBUNDLE)&lb);
    GpiPartialArc (hPS,&lPtCenter,MAKEFIXED(1,0),fxStartAngle,MAKEFIXED(0,0));
    GpiQueryCurrentPosition(hPS,(PPOINTL)&lStartPt);

    /* Draw the left highlighted outline of the bulb */
    lb.lColor = CLR_WHITE;
    lb.usType = LINETYPE_SOLID;
    GpiSetAttrs (hPS,PRIM_LINE,LBB_COLOR | LBB_TYPE,0,(PBUNDLE)&lb);
    GpiPartialArc (hPS,&lPtCenter,MAKEFIXED(1,0),fxStartAngle,fxSweepAngle);

    /* Draw the right shadowed outline of the bulb */
    fxStartAngle = MAKEFIXED(210,0);
    fxSweepAngle = MAKEFIXED(210,0);
    lb.lColor = CLR_DARKGRAY;
    GpiSetAttrs (hPS,PRIM_LINE,LBB_COLOR,0,(PBUNDLE)&lb);
    GpiPartialArc (hPS,&lPtCenter,MAKEFIXED(1,0),fxStartAngle,fxSweepAngle);
    GpiQueryCurrentPosition(hPS,(PPOINTL)&lEndPt);

    /* Fill the bulb of the thermometer with the supplied color */
    arcparms.lP -= 4;
    arcparms.lQ -= 4;
    GpiSetArcParams(hPS, (PARCPARAMS)&arcparms);
    ab.lColor = lColor;
    GpiSetAttrs(hPS,PRIM_AREA,ABB_COLOR,0,(PBUNDLE)&ab);
    GpiMove (hPS,&lPtCenter);
    GpiFullArc(hPS,DRO_FILL,MAKEFIXED(1,0));

    /*-------------------------------------------------------------------*\
                  Draw the reflection on the bulb
    \*-------------------------------------------------------------------*/
    arcparms.lP = (arcparms.lP * 3) / 4;
    arcparms.lQ = (arcparms.lQ * 3) / 4;
    GpiSetArcParams(hPS, (PARCPARAMS)&arcparms);
    fxStartAngle = MAKEFIXED(120,0);
    fxSweepAngle = MAKEFIXED(90,0);
    lb.usType = LINETYPE_INVISIBLE;
    GpiSetAttrs (hPS,PRIM_LINE,LBB_TYPE,0,(PBUNDLE)&lb);
    GpiPartialArc (hPS,&lPtCenter,MAKEFIXED(1,0),fxStartAngle,MAKEFIXED(0,0));

    lb.lColor = CLR_WHITE;
    lb.usType = LINETYPE_SOLID;
    GpiSetAttrs (hPS,PRIM_LINE,LBB_COLOR | LBB_TYPE,0,(PBUNDLE)&lb);
    GpiPartialArc (hPS,&lPtCenter,MAKEFIXED(1,0),fxStartAngle,fxSweepAngle);

    arcparms.lP = (arcparms.lP * 2) / 3;
    arcparms.lQ = (arcparms.lQ * 2) / 3;
    GpiSetArcParams(hPS, (PARCPARAMS)&arcparms);
    fxStartAngle = MAKEFIXED(120,0);
    fxSweepAngle = MAKEFIXED(90,0);
    lb.usType = LINETYPE_INVISIBLE;
    GpiSetAttrs (hPS,PRIM_LINE,LBB_TYPE,0,(PBUNDLE)&lb);
    GpiPartialArc (hPS,&lPtCenter,MAKEFIXED(1,0),fxStartAngle,MAKEFIXED(0,0));

    lb.lColor = CLR_WHITE;
    lb.usType = LINETYPE_SOLID;
    GpiSetAttrs (hPS,PRIM_LINE,LBB_COLOR | LBB_TYPE,0,(PBUNDLE)&lb);
    GpiPartialArc (hPS,&lPtCenter,MAKEFIXED(1,0),fxStartAngle,fxSweepAngle);

    /*-------------------------------------------------------------------*\
                           Draw the sides
    \*-------------------------------------------------------------------*/
    /* Draw the right vertical side */
    lb.lColor = CLR_DARKGRAY;
    GpiSetAttrs (hPS,PRIM_LINE,LBB_COLOR,0,(PBUNDLE)&lb);
    GpiMove(hPS,(PPOINTL)&lEndPt);
    lPt.x = lEndPt.x;
    lPt.y = pRectl->yBottom + ((iBoxHeight * 95) / 100);
    GpiLine (hPS,&lPt);

    /* Draw the left vertical side */
    lb.lColor = CLR_WHITE;
    GpiSetAttrs (hPS,PRIM_LINE,LBB_COLOR,0,(PBUNDLE)&lb);
    lPt.x = lStartPt.x;
    GpiMove(hPS,(PPOINTL)&lPt);
    GpiLine (hPS,&lStartPt);

    /*-------------------------------------------------------------------*\
                           Draw the rounded top
    \*-------------------------------------------------------------------*/
    /* Reset the rectangle for drawing the rounded top */
    BulbRectl.xLeft   = lStartPt.x;
    BulbRectl.yBottom = pRectl->yBottom + (iBoxHeight * 90) / 100;
    BulbRectl.xRight  = lEndPt.x;
    BulbRectl.yTop    = pRectl->yTop;

    arcparms.lP = (BulbRectl.xRight - BulbRectl.xLeft) / 2L;
    arcparms.lQ = (BulbRectl.yTop - BulbRectl.yBottom) / 2L;
    GpiSetArcParams(hPS, (PARCPARAMS)&arcparms);

    /* Calculate the center point for the rounded top arc */
    lPtCenter.x = BulbRectl.xLeft + arcparms.lP;
    lPtCenter.y = BulbRectl.yBottom + arcparms.lQ + 1;

    /* Draw an invisble arc from the current location to the start angle */
    fxStartAngle = MAKEFIXED(0,0);
    fxSweepAngle = MAKEFIXED(120,0);
    lb.usType = LINETYPE_INVISIBLE;
    GpiSetAttrs (hPS,PRIM_LINE,LBB_TYPE,0,(PBUNDLE)&lb);
    GpiPartialArc (hPS,&lPtCenter,MAKEFIXED(1,0),fxStartAngle,MAKEFIXED(0,0));

    /* Draw the shaded side */
    lb.lColor = CLR_DARKGRAY;
    lb.usType = LINETYPE_SOLID;
    GpiSetAttrs (hPS,PRIM_LINE,LBB_COLOR | LBB_TYPE,0,(PBUNDLE)&lb);
    GpiPartialArc (hPS,&lPtCenter,MAKEFIXED(1,0),fxStartAngle,fxSweepAngle);

    /* Draw the highlighed side */
    fxStartAngle = MAKEFIXED(120,0);
    fxSweepAngle = MAKEFIXED(60,0);
    lb.lColor = CLR_WHITE;
    GpiSetAttrs (hPS,PRIM_LINE,LBB_COLOR,0,(PBUNDLE)&lb);
    GpiPartialArc (hPS,&lPtCenter,MAKEFIXED(1,0),fxStartAngle,fxSweepAngle);

    /* Draw the reflection */
    arcparms.lP = (arcparms.lP * 3) / 4;
    arcparms.lQ = (arcparms.lQ * 3) / 4;
    GpiSetArcParams(hPS, (PARCPARAMS)&arcparms);
    fxStartAngle = MAKEFIXED(90,0);
    fxSweepAngle = MAKEFIXED(90,0);
    lb.usType = LINETYPE_INVISIBLE;
    GpiSetAttrs (hPS,PRIM_LINE,LBB_TYPE,0,(PBUNDLE)&lb);
    GpiPartialArc (hPS,&lPtCenter,MAKEFIXED(1,0),fxStartAngle,MAKEFIXED(0,0));

    lb.lColor = CLR_WHITE;
    lb.usType = LINETYPE_SOLID;
    GpiSetAttrs (hPS,PRIM_LINE,LBB_COLOR | LBB_TYPE,0,(PBUNDLE)&lb);
    GpiPartialArc (hPS,&lPtCenter,MAKEFIXED(1,0),fxStartAngle,fxSweepAngle);

    DrawTemperature(hPS,pRectl,lColor,lBackColor,lPercent);
    return (TRUE);
}

BOOL ProgressDefault(HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 switch (msg)
   {
     case WM_DESTROY:
//     WinAlarm(HWND_DESKTOP,WA_NOTE);
       break;
     case WM_CREATE:
       {  /* Initialize the window extra bytes */
         ULONG ulStyle = WinQueryWindowULong (hWnd, QWL_STYLE);
         ULONG ulBorderStyle = 0L;
         if (ulStyle & ALCTLS_BORDER_MASK)
           { if (ulStyle & ALCTLS_BORDER_INNI)
                ulBorderStyle |= DB_DEPRESSED;
             else if (ulStyle & ALCTLS_BORDER_OUTI)
                     ulBorderStyle |= DB_RAISED;
                  else if (ulStyle & ALCTLS_BORDER_STANDARD)
                          ulBorderStyle |= DB_STANDARD;
                       else if (ulStyle & ALCTLS_BORDER_FURROW)
                                ulBorderStyle |= DB_TROUGH;
             ulBorderStyle |= DB_INTERIOR;
           }
         WinSetWindowULong (hWnd, QWL_BORDERSTYLE,ulBorderStyle);
         WinSetWindowULong (hWnd, QWL_LOWER, 0L);
         WinSetWindowULong (hWnd, QWL_UPPER, 0L);
         WinSetWindowULong (hWnd, QWL_VALUE, 0L);
         WinSetWindowULong (hWnd, QWL_DELTA, 1L);
         WinSetWindowULong (hWnd, QWL_CALL_BACK_HNDL, 0L);
         WinSetWindowULong (hWnd, QWL_CALL_BACK_MSG, 0L);
         WinSetWindowULong (hWnd, QWL_MIN_SIZE, 0L);
         WinSetWindowULong(hWnd,QWL_COLOR_FORE,(ULONG)SYSCLR_ACTIVETITLETEXT);
         WinSetWindowULong(hWnd,QWL_COLOR_AFT,(ULONG)SYSCLR_ACTIVETITLETEXTBGND);
         break;
      }
     case ALCTLM_DECREMENT:
     case ALCTLM_INCREMENT:
         { LONG l = WinQueryWindowULong (hWnd, QWL_VALUE);
           if (msg==ALCTLM_INCREMENT)
              l += WinQueryWindowULong (hWnd, QWL_DELTA);
           else l -= WinQueryWindowULong (hWnd, QWL_DELTA);
           if ((l >= WinQueryWindowULong (hWnd, QWL_LOWER)) &&
               (l <= WinQueryWindowULong (hWnd, QWL_UPPER)))
              WinSendMsg(hWnd,ALCTLM_SETVALUE,(MPARAM)l, (MPARAM)0L);
           break;
         }

     case ALCTLM_SETRANGE:
         WinSetWindowULong (hWnd, QWL_LOWER, (ULONG)mp1);
         WinSetWindowULong (hWnd, QWL_UPPER, (ULONG)mp2);
         break;

     case ALCTLM_SETDELTA:
         WinSetWindowULong (hWnd, QWL_DELTA, (ULONG)mp1);
         break;

     case ALCTLM_GETVALUE:
         *((PULONG)mp1) = WinQueryWindowULong (hWnd, QWL_VALUE);
         break;

     case ALCTLM_GETDELTA:
         *((PULONG)mp1) = WinQueryWindowULong (hWnd, QWL_DELTA);
         break;

     case ALCTLM_GETRANGE:
         *((PULONG)mp1) = WinQueryWindowULong (hWnd, QWL_LOWER);
         *((PULONG)mp2) = WinQueryWindowULong (hWnd, QWL_UPPER);
         break;

     case ALCTLM_GETCOLORS:
         *((PULONG)mp1) = WinQueryWindowULong (hWnd, QWL_COLOR_FORE);
         *((PULONG)mp2) = WinQueryWindowULong (hWnd, QWL_COLOR_AFT);
         break;

     case ALCTLM_RESET:
         WinSendMsg(hWnd,ALCTLM_SETVALUE,
                    (MPARAM)WinQueryWindowULong(hWnd,QWL_LOWER),
                    (MPARAM)0L);
         break;

     case ALCTLM_DELAY_HIDE:
       WinStartTimer((HAB)0,hWnd,TIMER_DELAY,(ULONG)mp1);
       break;

     case WM_TIMER:
       if ((ULONG)mp1 == TIMER_DELAY)
         { WinStopTimer((HAB)0,hWnd,TIMER_DELAY);
           WinShowWindow(hWnd,FALSE);
         }
       break;

     case ALCTLM_SETCOLORS:
         WinSetWindowULong (hWnd, QWL_COLOR_FORE, (ULONG)mp1);
         WinSetWindowULong (hWnd, QWL_COLOR_AFT, (ULONG)mp2);
         WinInvalidateRect (hWnd, NULL, FALSE);
         break;
     default:
         return FALSE;
   }
 return TRUE;
}

BOOL Draw_AdjustBorder(HWND hWnd,HPS hps, PRECTL  prectl,BOOL fDraw)
{
 ULONG ulThickness;
 ULONG ulStyle = WinQueryWindowULong (hWnd, QWL_BORDERSTYLE);
 if (ulStyle)
    if (WinQueryWindowULong (hWnd, QWL_STYLE) & ALCTLS_BORDER_SYSTEM)
       { ulThickness = ((ulStyle & ALCTLS_BORDER_STANDARD)? 1 : 2 );
         if (fDraw)
           WinDrawBorder (hps, prectl, ulThickness, ulThickness,
                           (LONG)SYSCLR_BUTTONDARK,
                           WinQueryWindowULong(hWnd, QWL_COLOR_AFT),
                           ulStyle);
         WinInflateRect((HAB)0,prectl,-(1+ulThickness),-(2+ulThickness));
//       WinInflateRect((HAB)0,prectl,-(2*ulThickness),-(2*ulThickness));
//         prectl->yBottom--;
       }
    else {
           if (ulStyle & DB_DEPRESSED)
             My3DBorder(hps,prectl,5L,fDraw,TRUE);
           else if (ulStyle & DB_RAISED)
                  My3DBorder(hps,prectl,5L,fDraw,FALSE);
           else if (ulStyle & DB_STANDARD)
                  MyBorder(hps,prectl,1L,fDraw);
                else if (ulStyle & DB_TROUGH)
                        MyBorder(hps,prectl,2L,fDraw);
           if (fDraw)
             {  RECTL rctl = *prectl;
                WinInflateRect((HAB)0,&rctl,1L,1L);
                WinFillRect(hps,prectl,WinQueryWindowULong(hWnd,QWL_COLOR_AFT));
//             WinFillRect(hps,prectl,WinQueryWindowULong(hWnd,SYSCLR_WINDOW));
             }
         }
 else if (fDraw)
         WinFillRect (hps, prectl, WinQueryWindowULong(hWnd, QWL_COLOR_AFT));
 return (ulStyle != 0L);
}

VOID SetMinThermoSize(HWND hWnd,ULONG ulW, ULONG ulH)
{
 RECTL rctl = {0L,0L,100L,100L};
 HPS hps = WinGetPS (hWnd);
 ULONG ul;
 if (!ulW || !ulH)
   { ul = WinQueryWindowULong (hWnd, QWL_MIN_SIZE);
     ulW = (ULONG)(LOUSHORT(ul));
     ulH = (ULONG)(HIUSHORT(ul));
   }
 Draw_AdjustBorder(hWnd,hps,&rctl,FALSE);
 TextBox(hps,"100%",(PLONG)&ul,NULL);
 if (!(WinQueryWindowULong (hWnd, QWL_STYLE) & ALCTLS_NOPERCENT))
    ul *= 2;
 ulW = ALMAX(ulW,ul);
 ulH = ALMAX(ulH,(ul*3)/2);
 ulW += (rctl.xLeft + (100L-rctl.xRight)+1);
 ulH += (rctl.yBottom + (100L-rctl.yTop)+1);
 WinSetWindowULong(hWnd,QWL_MIN_SIZE,MAKELONG((USHORT)ulW,(USHORT)ulH));
 WinReleasePS (hps);
}

MRESULT EXPENTRY ThermometerWndProc (HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
 BOOL    bHandled = TRUE;
 MRESULT mReturn  = 0;
 RECTL   Rectl;
 HPS     hps;
 ULONG   ul1,ul2;

 switch (msg)
   {
    case WM_CREATE:
      {
       PCREATESTRUCT pcrst = (PCREATESTRUCT)PVOIDFROMMP(mp2);
       ProgressDefault(hWnd,msg,mp1,mp2);
       SetMinThermoSize(hWnd,pcrst->cx,pcrst->cy);
       WinSetWindowULong (hWnd, QWL_COLOR_AFT, SYSCLR_DIALOGBACKGROUND);
       WinSetWindowULong(hWnd,QWL_COLOR_FORE,(ULONG)SYSCLR_ACTIVETITLETEXTBGND);
       break;
      }
    case ALCTLM_MINIMUM_SIZE:
       ul1 = WinQueryWindowULong (hWnd, QWL_MIN_SIZE);
       *((PULONG)mp1) = (ULONG)(LOUSHORT(ul1)+1);
       *((PULONG)mp2) = (ULONG)(HIUSHORT(ul1));
       break;

    case WM_PRESPARAMCHANGED:
       SetMinThermoSize(hWnd,0L,0L);
       WinInvalidateRect (hWnd, NULL, FALSE);
       break;

     case ALCTLM_SETVALUE:
         /* Set the new value */
         WinSetWindowULong (hWnd, QWL_VALUE, (ULONG)mp1);
         WinQueryWindowRect (hWnd, &Rectl);
         hps = WinGetPS (hWnd);
         if (Draw_AdjustBorder(hWnd,hps,&Rectl,FALSE))
             WinInflateRect((HAB)0,&Rectl,-0L,-6L);
         else WinInflateRect((HAB)0,&Rectl,-0L,-4L);
         Rectl.xRight -= 6;
         if (!(WinQueryWindowULong (hWnd, QWL_STYLE) & ALCTLS_NOPERCENT))
         {
             Rectl.xRight  = (Rectl.xRight * 6) / 10;
             Rectl.xLeft  += (Rectl.xRight / 5);
         }
         else Rectl.xLeft += 6;
         Rectl.yTop--;

         /* Update the thermometer temperature */
         DrawTemperature (hps, &Rectl,
                          (LONG)WinQueryWindowULong(hWnd, QWL_COLOR_FORE),
                          (LONG)WinQueryWindowULong(hWnd, QWL_COLOR_AFT),
                          CalculatePercentage(hWnd));
         WinReleasePS (hps);
         break;

     case WM_PAINT:
       { BOOL fBorder;
         hps = WinBeginPaint (hWnd,0,0);
         WinQueryWindowRect (hWnd, &Rectl);
         Rectl.xRight--;
         fBorder = Draw_AdjustBorder(hWnd,hps,&Rectl,TRUE);
         WinInflateRect((HAB)0,&Rectl,-0L,(fBorder? -6L: -4L));
         Rectl.xRight -= 6;
         if (!(WinQueryWindowULong (hWnd, QWL_STYLE) & ALCTLS_NOPERCENT))
            { DrawPercentages (hps, &Rectl,fBorder);
              Rectl.xRight  = (Rectl.xRight * 6) / 10;
              Rectl.xLeft  += (Rectl.xRight / 5);
            }
         else Rectl.xLeft += 6;
         Rectl.yTop--;

         DrawThermometer (hps, &Rectl,0L,
                          WinQueryWindowULong(hWnd, QWL_COLOR_FORE),
                          WinQueryWindowULong(hWnd, QWL_COLOR_AFT),
                          CalculatePercentage(hWnd));

         WinEndPaint (hps);
         break;
       }
     case WM_HITTEST:
       return(MRFROMLONG(HT_TRANSPARENT));

     default:
         bHandled = ProgressDefault(hWnd,msg,mp1,mp2);
         break;
   }

 if (!bHandled)
     mReturn = WinDefWindowProc (hWnd,msg,mp1,mp2);

 return (mReturn);
}

VOID DrawBar(HWND hWnd,HPS hps,PRECTL prct,LONG lPercent)
{
 AREABUNDLE ab;
 RECTL      rct = *prct;
//  WinFillRect(hps,prct,WinQueryWindowULong(hWnd,QWL_COLOR_AFT));
// rct.xRight--;

 if (WinQueryWindowULong(hWnd,QWL_STYLE) & ALCTLS_VERTICAL)
   rct.xRight--;

 if (lPercent = ALMAX(ALMIN(100,lPercent),0))
   {if (WinQueryWindowULong(hWnd,QWL_STYLE) & ALCTLS_VERTICAL)
       rct.yTop = rct.yBottom + (((rct.yTop-rct.yBottom)*lPercent)/100)-1;
    else { rct.xRight = rct.xLeft + (((rct.xRight-rct.xLeft)*lPercent)/100)-1;
//         rct.yTop--;
         }
    ab.lColor = WinQueryWindowULong(hWnd,QWL_COLOR_FORE);
    GpiSetAttrs(hps,PRIM_AREA,ABB_COLOR,0,(PBUNDLE)&ab);
    GpiMove (hps,(PPOINTL)&rct);
    GpiBox (hps,DRO_FILL,(PPOINTL)&rct.xRight,0,0);
    if (WinQueryWindowULong(hWnd,QWL_STYLE) & ALCTLS_VERTICAL)
      { rct.yBottom = rct.yTop + 1;
        rct.yTop = prct->yTop;
      }
    else { rct.xLeft = rct.xRight+1;
           rct.xRight = prct->xRight;
         }
  }
 if (lPercent < 100)
   { ab.lColor = WinQueryWindowULong(hWnd,QWL_COLOR_AFT);
     GpiSetAttrs(hps,PRIM_AREA,ABB_COLOR,0,(PBUNDLE)&ab);
     GpiMove (hps,(PPOINTL)&rct);
     GpiBox (hps,DRO_FILL,(PPOINTL)&rct.xRight,0,0);
  }

}

VOID DrawPercentageBar(HWND hWnd,HPS hps,PRECTL prectl,ULONG ulPercent)
{
 RECTL       rct = *prectl,rct1;
 ULONG cx
      , cy
      , x
      ,ul;
// FONTMETRICS FM;
 CHAR        ach[10];
// POINTL      aptl[TXTBOX_COUNT];
 POINTL      ptl;

// GpiQueryFontMetrics(hps,sizeof(FONTMETRICS),(PFONTMETRICS)&FM); //expensive call

 rct.xRight -=1;                      // cosmetic adjustments to have the
 rct.xLeft  -=1;                      // bar fit evenly in its frame
// rct.yTop   -=1;
                  /* establish two bar rectangle structures
                   * one for the left (growing) portion (rct1)
                   * the other for the right (shrinking) part (rct)
                   */
 rct1 = rct;

 cx = rct.xRight - rct.xLeft + 1;
 cy = rct.yTop - rct.yBottom;
// x  = ((ulPercent * cx)/100 + 1) + rct.xLeft;
 x  = ((ulPercent * cx)/100 + 0) + rct.xLeft;
// sprintf (ach,"%3d%%",(USHORT)ulPercent); // too stack hungry - can blow
// ltoa(ulPercent,ach,10);
 MyULONGToSZ(ulPercent,ach);
 ul = Mystrlen(ach);
 ach[ul] = '%';
 ach[ul+1] = '\0';
 TextBox(hps,ach,&ptl.x,&ptl.y);
// GpiQueryTextBox (hps,4L,ach,TXTBOX_COUNT,aptl);
// ptl.x = aptl[TXTBOX_TOPRIGHT].x - aptl[TXTBOX_TOPLEFT].x ;
// ptl.y = aptl[TXTBOX_TOPLEFT].y - aptl[TXTBOX_BOTTOMLEFT].y ;

 ptl.x = rct1.xLeft + (cx - ptl.x)/2L;
// ptl.y = rct1.yBottom + ((cy - ptl.y)/2L) + FM.lMaxDescender+2;     // vertically too
 ptl.y = rct1.yBottom + ((cy - ptl.y)/2L) + ptl.y/4;     // vertically too

                      /* break the rectangle of the whole bar up
                       * into two horizontal regions: all coordinates
                       * the same except the right (shrinking) part
                       * starts where the left (growing) part stops
                       */
 rct1.xRight = rct.xLeft  = x;
                 /* draw the left bar portion by writing the percent
                  * string (which will go smack in the middle) within
                  * the left clipping rectangular region. The effect
                  * will be a proper contrasting percent string text
                  * even when the two parts straddle the string
                  */
 GpiSetColor ( hps , WinQueryWindowULong(hWnd,QWL_COLOR_AFT) );                  // set fore and background
 GpiSetBackColor(hps,WinQueryWindowULong (hWnd,QWL_COLOR_FORE) );

               // now draw the string cliiping it to the left rectangle
 GpiCharStringPosAt(hps,&ptl,&rct1, CHS_OPAQUE | CHS_CLIP, 4L,ach,NULL);
 GpiCharStringPosAt(hps,&ptl,&rct1, CHS_OPAQUE | CHS_CLIP,Mystrlen(ach),ach,NULL);

 GpiSetBackColor (hps,WinQueryWindowULong(hWnd,QWL_COLOR_AFT));      // re-set the fore and background              // for example: blue
 GpiSetColor(hps,WinQueryWindowULong(hWnd,QWL_COLOR_FORE));
              // now draw the right bar by drawing the string clippping
              // it to the right rectangle
 GpiCharStringPosAt(hps,&ptl,&rct, CHS_OPAQUE | CHS_CLIP, 4L,ach,NULL);
 GpiCharStringPosAt(hps,&ptl,&rct, CHS_OPAQUE | CHS_CLIP,Mystrlen(ach),ach,NULL);
}

VOID DrawMeABar(HWND hWnd,HPS hps,PRECTL prectl)
{
 if (WinQueryWindowULong (hWnd, QWL_STYLE) & ALCTLS_NOPERCENT)
  DrawBar(hWnd,hps,prectl,CalculatePercentage(hWnd));
 else DrawPercentageBar(hWnd,hps,prectl,CalculatePercentage(hWnd));
}
#if 0
ULONG  MinimumBarSize(HWND hWnd)
{
 ULONG ulW
      ,ulH
      , ulStyle = WinQueryWindowULong (hWnd, QWL_STYLE);
 HPS hps = WinGetPS (hWnd);
 if (ulStyle & ALCTLS_NOPERCENT)
    { if (ulStyle & ALCTLS_BORDER_MASK)
        { RECTL rctl = {0L,0L,100L,100L};
          Draw_AdjustBorder(hWnd,hps,&rctl,FALSE);
          ulW = rctl.xLeft + (100L-rctl.xRight);
          ulH = rctl.yBottom + (100L-rctl.yTop);
        }
      else ulW = ulH = 0L;
      ulW += ((ulStyle & ALCTLS_VERTICAL)? 2 : 8);
      ulH += ((ulStyle & ALCTLS_VERTICAL)? 8 : 2);
    }
 else { TextBox(hps,"100%",(PLONG)&ulW,(PLONG)&ulH);
         ulW *= 2;
//       ulH = ((ulH*3)/2);//+8;
         ulH += (ulH*8)/10;
//       ulH *= 2;
      }
 WinReleasePS (hps);
 return (MAKELONG((USHORT)ulW,(USHORT)ulH));
}
#endif
VOID  SetMinimumBarSize(HWND hWnd,ULONG ulWReq,ULONG ulHReq)
{
 HPS hps = WinGetPS (hWnd);
 ULONG ulStyle, ulW, ulH;
 if (!ulWReq || !ulHReq)     // Requested dimensions
   { ulStyle = WinQueryWindowULong (hWnd, QWL_MIN_SIZE);//borrow variable
     ulWReq = (ULONG)(LOUSHORT(ulStyle));
     ulHReq = (ULONG)(HIUSHORT(ulStyle));
   }
 ulStyle = WinQueryWindowULong (hWnd, QWL_STYLE);
 if (ulStyle & ALCTLS_NOPERCENT)
    { if (ulStyle & ALCTLS_BORDER_MASK)
        { RECTL rctl = {0L,0L,100L,100L};
          Draw_AdjustBorder(hWnd,hps,&rctl,FALSE);
          ulW = rctl.xLeft + (100L-rctl.xRight);
          ulH = rctl.yBottom + (100L-rctl.yTop);
        }
      else ulW = ulH = 0L;
      ulW += ((ulStyle & ALCTLS_VERTICAL)? 2 : 8);
      ulH += ((ulStyle & ALCTLS_VERTICAL)? 8 : 2);
    }
 else { TextBox(hps,"100%",(PLONG)&ulW,(PLONG)&ulH);
         ulW *= 2;
//       ulH = ((ulH*3)/2);//+8;
         ulH += (ulH*8)/10;
//       ulH *= 2;
      }
 ulW = ALMAX(ulW,ulWReq);
 ulH = ALMAX(ulH,ulHReq);
 WinSetWindowULong(hWnd,QWL_MIN_SIZE,MAKELONG((USHORT)ulW,(USHORT)ulH));
 WinReleasePS (hps);
}

MRESULT EXPENTRY ProgressBarWndProc (HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    BOOL    bHandled = TRUE;
    MRESULT mReturn  = 0;
    RECTL   Rectl;
    HPS     hps;
    ULONG ul1, ul2,ulStyle;

 switch (msg)
  {
    case WM_CREATE:
       { PCREATESTRUCT pcrst = (PCREATESTRUCT)PVOIDFROMMP(mp2);
         ProgressDefault(hWnd,msg,mp1,mp2);
         SetMinimumBarSize(hWnd,pcrst->cx,pcrst->cy);
          break;
       }
    case WM_PRESPARAMCHANGED:
       SetMinimumBarSize(hWnd,0L,0L);
       WinInvalidateRect (hWnd, NULL, FALSE);
       break;

    case ALCTLM_MINIMUM_SIZE:  // for ibmclass
       ul1 = WinQueryWindowULong (hWnd, QWL_MIN_SIZE);
       *((PULONG)mp1) = (ULONG)(LOUSHORT(ul1)+1);
       *((PULONG)mp2) = (ULONG)(HIUSHORT(ul1));
       break;

    case ALCTLM_SETVALUE:
       hps = WinGetPS (hWnd);
       WinSetWindowULong (hWnd, QWL_VALUE, (ULONG)mp1);
       WinQueryWindowRect (hWnd, &Rectl);
       Rectl.xRight--;
       Draw_AdjustBorder(hWnd,hps,&Rectl,FALSE);
       DrawMeABar(hWnd,hps,&Rectl);
       WinReleasePS (hps);
       break;

    case WM_PAINT:
       hps = WinBeginPaint (hWnd,0,0);
       WinQueryWindowRect (hWnd, &Rectl);
       Rectl.xRight--; // for ibmclass canvases columns who will take right most pixel
       Draw_AdjustBorder(hWnd,hps,&Rectl,TRUE);
       DrawMeABar(hWnd,hps,&Rectl);
       WinEndPaint (hps);
       break;

    case WM_HITTEST:
      return(MRFROMLONG(HT_TRANSPARENT));

    default:
       bHandled = ProgressDefault(hWnd,msg,mp1,mp2);
       break;
  }

 if (!bHandled)
    mReturn = WinDefWindowProc (hWnd,msg,mp1,mp2);
 return (mReturn);
}

PSZ FormatLess100(PSZ pszBuff, ULONG ul)
{
 PCHAR pch = pszBuff;
 if (ul < 100)
   { if (ul < 10)
      *pch++ = '0';
     ltoa(ul,pch,10);
   }
 return pszBuff;
}

VOID DigitSize(HPS hps,PULONG pulW, PULONG pulH)
{
 TextBox(hps,"0 ",(PLONG)pulW,(PLONG)pulH);
// *pulW *= 90;
// *pulW /= 100;
// *pulH *= 90;
// *pulH /= 100;
}

VOID DrawNDigits(HPS hps,PRECTL prctl,ULONG ulOld, ULONG ulNew,
                 ULONG ulColorFore,ULONG ulColorAft,ULONG N,BOOL fSelective)
{
 CHAR szOld[11];
 CHAR szNew[11];
 POINTL ptl;
 RECTL rctl = *prctl;
 ULONG   ulH
        ,ulW
        ,n
        ,lColorSave = GpiQueryColor(hps);;
  N = ALMIN(10,N);
  MyULONGToSZLeading(ulNew,szNew,N);
  GpiSetColor(hps, ulColorFore);
  DigitSize(hps,&ulW,&ulH);
  if (!fSelective)
//  WinDrawBorder (hps, &rctl, COUNTERMARGIN+2L,COUNTERMARGIN+2L,
//                 ulColorFore,ulColorAft, DB_INTERIOR);
  WinFillRect(hps,prctl,ulColorFore);
  else  MyULONGToSZLeading(ulOld,szOld,N);

  WinInflateRect((HAB)0L, &rctl, -(COUNTERMARGIN+1L),-(COUNTERMARGIN+1L));
  rctl.yBottom++;
  if (!fSelective)
    { rctl.xRight--;
      WinFillRect(hps,&rctl,ulColorAft);
    }
  for (n = 0 ; n < N ; n++)
   { rctl.xRight = rctl.xLeft + ulW;
     if (!fSelective || ( szOld[n] != szNew[n]))
       { WinDrawText(hps,1,szNew+n,&rctl, ulColorFore,ulColorAft,
                     DT_CENTER | DT_VCENTER | DT_ERASERECT);
         if (n < (N-1))
          { ptl.y = rctl.yBottom;
            ptl.x = (rctl.xRight+1);
            GpiMove(hps, &ptl);
            ptl.y = rctl.yTop;
          //     GpiLine(hps, &ptl);
            GpiBox(hps, DRO_OUTLINE, &ptl, 0L, 0L);
          }
       }
    rctl.xLeft = rctl.xRight+2;
  }
 GpiSetColor(hps, lColorSave);
}

VOID SizeMeNDigits(HPS hps,PULONG pulW,PULONG pulH,ULONG N)
{
 DigitSize(hps,pulW,pulH);

// *pulW =  (N+1) + (N*(*pulW));
// *pulW =   N*(*pulW+2) -2;
 *pulW =   N*(*pulW+2);
 *pulH +=  2;
}

VOID TimerSize(HWND hWnd,HPS hps,PULONG pulW, PULONG pulH)
{
 RECTL rctl = {0L,0L,100L,100L};
 ULONG ulW, ulH;
 Draw_AdjustBorder(hWnd,hps,&rctl,FALSE);
 SizeMeNDigits(hps,&ulW,&ulH,2);
 *pulW = 4*TIMERMARGIN + 2*ulW + (rctl.xLeft + (100L-rctl.xRight)+1)+5;
 *pulH = 2*TIMERMARGIN + ulH +  (rctl.yBottom + (100L-rctl.yTop)+1);
}

VOID DrawMeATimer(HWND hWnd,HPS hps,PRECTL prctl,ULONG ulMinOld, ULONG ulMin,
                  ULONG ulSecOld,ULONG ulSec,BOOL fSelective)
{
  RECTL rct = *prctl;
  ULONG ulColorFore =  WinQueryWindowULong(hWnd,QWL_COLOR_FORE);
  ULONG ulColorAft =  WinQueryWindowULong(hWnd,QWL_COLOR_AFT);
  ULONG ulUnitW, ulUnitH;

  SizeMeNDigits(hps,&ulUnitW,&ulUnitH,2L);

  if (!fSelective)
    WinFillRect(hps,prctl,ulColorFore);
  rct.xRight = rct.xLeft + ulUnitW;
  if (!fSelective || (ulMinOld != ulMin))
    DrawNDigits(hps,&rct,ulMinOld,ulMin,ulColorFore,ulColorAft,2L,fSelective);

  rct.xLeft = rct.xRight+2;
  if (!fSelective)
    { rct.xRight = rct.xLeft + 5;
      WinDrawText(hps,1,":",&rct, ulColorAft,ulColorFore,
                  DT_CENTER | DT_VCENTER | DT_ERASERECT);
    }
  rct.xLeft = rct.xLeft +4;
  rct.xRight = rct.xLeft + ulUnitW;
  DrawNDigits(hps,&rct,ulSecOld,ulSec,ulColorFore,ulColorAft,2L,fSelective);
}

VOID TickUp(HWND hWnd,HPS hps,PRECTL prctl,ULONG ulTicks)
{
 ULONG ulSecs = WinQueryWindowULong(hWnd,QWL_SECONDS)
     , ulMins = WinQueryWindowULong(hWnd,QWL_MINUTES)
     , ul;
 BOOL fDrawMins;
 if (ulTicks == 1)
   { if (fDrawMins = !(ulSecs = (++ulSecs%60)))
       ulMins++;
   }
 else { ulSecs += ulTicks;
        ul = ulSecs / 60;
        ulSecs = ulSecs%60;
        ulMins += ul;
        fDrawMins = ul? TRUE : FALSE;
      }
 if (ulMins > 59L ) ulMins = 0L;
 DrawMeATimer(hWnd,hps,prctl,WinQueryWindowULong(hWnd,QWL_MINUTES),
              ulMins,WinQueryWindowULong(hWnd,QWL_SECONDS),ulSecs, TRUE);
 WinSetWindowULong (hWnd, QWL_MINUTES,ulMins);
 WinSetWindowULong (hWnd, QWL_SECONDS,ulSecs);
}

VOID TickDown(HWND hWnd,HPS hps,PRECTL prctl,ULONG ulTicks)
{
 ULONG ulSecs = WinQueryWindowULong(hWnd,QWL_SECONDS)
     , ulMins = WinQueryWindowULong(hWnd,QWL_MINUTES)
     , ulTotal
     , ulOldMins;
 BOOL fDrawMins;
 if ( ulMins || ulSecs)
   { ulTotal = (ulMins*60 + ulSecs) - ulTicks;
     if (ulTotal > 0 )
       { ulOldMins = ulMins;
         ulMins = ulTotal/60;
         ulSecs = ulTotal%60;
         fDrawMins = (ulOldMins != ulMins)? TRUE : FALSE;
       }
     else { ulMins = ulSecs = 0L;
            WinStopTimer((HAB)0,hWnd,TIMER_DOWN);
            if ( WinQueryWindowULong(hWnd,QWL_CALL_BACK_HNDL) )
              { WinPostMsg( (HWND)WinQueryWindowULong(hWnd,QWL_CALL_BACK_HNDL)
                           , WinQueryWindowULong(hWnd,QWL_CALL_BACK_MSG)
                           ,(MPARAM)0L ,(MPARAM)0L);
                WinSetWindowULong (hWnd, QWL_CALL_BACK_MSG,0L);
              }
          }
   }
 else return;
 DrawMeATimer(hWnd,hps,prctl,WinQueryWindowULong(hWnd,QWL_MINUTES),
              ulMins,WinQueryWindowULong(hWnd,QWL_SECONDS),ulSecs,TRUE);
 WinSetWindowULong (hWnd, QWL_MINUTES,ulMins);
 WinSetWindowULong (hWnd, QWL_SECONDS,ulSecs);
}

VOID PositionTimer(HWND hWnd,HPS hps,PRECTL prctl)
{
 ULONG ulW, ulH;
 TimerSize(hWnd,hps,&ulW,&ulH);
 prctl->xRight = prctl->xLeft + ulW;
 if ((prctl->yTop - prctl->yBottom) >  ulH)
    prctl->yBottom += ((prctl->yTop - prctl->yBottom) - ulH)/2;
 prctl->yTop = prctl->yBottom + ulH;
}

MRESULT EXPENTRY TimerWndProc (HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    BOOL    bHandled = TRUE
          , f;
    MRESULT mReturn  = 0;
    RECTL   Rectl;
    HPS     hps;

 switch (msg)
  {
    case ALCTLM_MINIMUM_SIZE:
       hps = WinGetPS (hWnd);
       TimerSize(hWnd,hps,(PULONG)mp1,(PULONG)mp2);
       *((PLONG)mp1) += 1; //ibmclass' canvas columns chopping of one unit
//       *((PLONG)mp2) += 1;
       WinReleasePS (hps);
       break;

    case ALCTLM_TIME_SET:
       hps = WinGetPS (hWnd);
       WinQueryWindowRect (hWnd, &Rectl);
       PositionTimer(hWnd,hps,&Rectl);
       Draw_AdjustBorder(hWnd,hps,&Rectl,FALSE);
       DrawMeATimer(hWnd,hps,&Rectl,
                    WinQueryWindowULong(hWnd,QWL_MINUTES), (ULONG)mp1,
                    WinQueryWindowULong(hWnd,QWL_SECONDS), (ULONG)mp2,TRUE);
       WinSetWindowULong (hWnd, QWL_MINUTES, (ULONG)mp1);
       WinSetWindowULong (hWnd, QWL_SECONDS, (ULONG)mp2);
       WinReleasePS (hps);
       break;

    case ALCTLM_RESET:
       WinPostMsg(hWnd,ALCTLM_TIME_SET, (MPARAM)0L, (MPARAM)0L);
       break;

    case ALCTLM_LEAVE_WAKE_UP_CALL:
       WinSetWindowULong (hWnd, QWL_CALL_BACK_HNDL, (ULONG)mp1);
       WinSetWindowULong (hWnd, QWL_CALL_BACK_MSG, (ULONG)mp2);
       break;

    case ALCTLM_START_COUNTING_UP:
    case ALCTLM_START_COUNTING_DOWN:
            // VALUE's available for use to figure elapsed seconds
       WinSetWindowULong (hWnd, QWL_VALUE,(ULONG)(clock()/CLOCKS_PER_SEC));
       WinStartTimer((HAB)0,hWnd,
                     (msg == ALCTLM_START_COUNTING_UP? TIMER_UP : TIMER_DOWN),
                      1000*WinQueryWindowULong(hWnd,QWL_DELTA));
       break;

    case ALCTLM_STOP_COUNTING:
       WinStopTimer((HAB)0,hWnd,TIMER_UP);
       WinStopTimer((HAB)0,hWnd,TIMER_DOWN);
       break;

    case WM_TIMER:
       switch((ULONG)mp1)
         { case TIMER_UP:
              WinPostMsg(hWnd,ALCTLM_INCREMENT,(MPARAM)0L,(MPARAM)0L);
              break;
           case TIMER_DOWN:
              WinPostMsg(hWnd,ALCTLM_DECREMENT,(MPARAM)0L,(MPARAM)0L);
              break;
           default: bHandled = ProgressDefault(hWnd,msg,mp1,mp2);//4 delay timer
         }
       break;

    case ALCTLM_DECREMENT:
    case ALCTLM_INCREMENT:
     {
       ULONG ulNow = (ULONG)(clock()/CLOCKS_PER_SEC);
//       ULONG ulWas =  WinQueryWindowULong(hWnd,QWL_VALUE);
       ULONG ulElapsed = ulNow - WinQueryWindowULong(hWnd,QWL_VALUE);
       WinSetWindowULong (hWnd, QWL_VALUE,ulNow);
       hps = WinGetPS (hWnd);
       WinQueryWindowRect (hWnd, &Rectl);
       PositionTimer(hWnd,hps,&Rectl);
       Draw_AdjustBorder(hWnd,hps,&Rectl,FALSE);
       if (msg == ALCTLM_DECREMENT)
#if 0
         TickDown(hWnd,hps,&Rectl,ulElapsed);
       else TickUp(hWnd,hps,&Rectl,ulElapsed);
#endif
         TickDown(hWnd,hps,&Rectl,ulElapsed/WinQueryWindowULong(hWnd,QWL_DELTA));
       else TickUp(hWnd,hps,&Rectl,ulElapsed/WinQueryWindowULong(hWnd,QWL_DELTA));
       WinReleasePS (hps);
       break;
    }
    case WM_PAINT:
       hps = WinBeginPaint (hWnd,0,0);
       WinQueryWindowRect (hWnd, &Rectl);
       PositionTimer(hWnd,hps,&Rectl);
       Draw_AdjustBorder(hWnd,hps,&Rectl,TRUE);
       DrawMeATimer(hWnd,hps,&Rectl,
                    0L,WinQueryWindowULong(hWnd,QWL_MINUTES),
                    0L,WinQueryWindowULong(hWnd,QWL_SECONDS),FALSE);
       WinEndPaint (hps);
       break;

    case WM_HITTEST:
      return(MRFROMLONG(HT_TRANSPARENT));

    default:
       bHandled = ProgressDefault(hWnd,msg,mp1,mp2);
       break;
  }

 if (!bHandled)
    mReturn = WinDefWindowProc (hWnd,msg,mp1,mp2);
 return (mReturn);
}

VOID CounterSize(HWND hWnd,HPS hps,ULONG ulDigits,PULONG pulW, PULONG pulH)
{
 RECTL rctl = {0L,0L,100L,100L};
 ULONG ulW, ulH;
 Draw_AdjustBorder(hWnd,hps,&rctl,FALSE);
 SizeMeNDigits(hps,&ulW,&ulH,ulDigits);
 *pulW = 2*COUNTERMARGIN + ulW + (rctl.xLeft + (100L-rctl.xRight)+2);
 *pulH = 2*COUNTERMARGIN + ulH +  (rctl.yBottom + (100L-rctl.yTop)+2);
}

VOID PositionCounter(HWND hWnd,HPS hps,PRECTL prctl,ULONG ulDigits)
{
 ULONG ulW, ulH;
 CounterSize(hWnd,hps,ulDigits,&ulW,&ulH);         // leaving left-bottom as is
 prctl->xRight = prctl->xLeft + ulW;
 prctl->yTop = prctl->yBottom + ulH;
}

VOID SetDigitCount(HWND hWnd)
{
 ULONG ulCount = ( WinQueryWindowULong (hWnd, QWL_STYLE) & 0x00000F00);
 ulCount = (ulCount >> 8 );
 if (!ulCount)
    ulCount = 3;
 WinSetWindowULong (hWnd,QWL_DIGIT_COUNT,ulCount);
}


MRESULT EXPENTRY CounterWndProc (HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    BOOL    bHandled = TRUE
          , f;
    MRESULT mReturn  = 0;
    RECTL   Rectl;
    HPS     hps;
    ULONG   ul1,ul2;

 switch (msg)
  {
    case WM_CREATE:
       ProgressDefault(hWnd,msg,mp1,mp2);
       SetDigitCount(hWnd);
       break;

    case ALCTLM_MINIMUM_SIZE:
       hps = WinGetPS (hWnd);
       CounterSize( hWnd,hps
                   ,WinQueryWindowULong(hWnd,QWL_DIGIT_COUNT)
                   ,(PULONG)mp1,(PULONG)mp2);
       *((PLONG)mp1) += 1; //ibmclass' multicell canvas is chopping of one unit
       *((PLONG)mp2) += 1;
       WinReleasePS (hps);
       break;

    case ALCTLM_SETVALUE:
       hps = WinGetPS (hWnd);
       WinQueryWindowRect (hWnd, &Rectl);
       ul1 = WinQueryWindowULong(hWnd,QWL_DIGIT_COUNT);
       PositionCounter(hWnd,hps,&Rectl,ul1);
       Draw_AdjustBorder(hWnd,hps,&Rectl,FALSE);
       ul2 = WinQueryWindowULong(hWnd, QWL_VALUE);
       DrawNDigits( hps,&Rectl,WinQueryWindowULong(hWnd,QWL_VALUE)
                   ,(ULONG)mp1
                   ,WinQueryWindowULong(hWnd,QWL_COLOR_FORE)
                   ,WinQueryWindowULong(hWnd,QWL_COLOR_AFT)
                   ,ul1,TRUE);
       WinSetWindowULong(hWnd,QWL_VALUE,(ULONG)mp1);
       WinReleasePS (hps);
       break;

    case ALCTLM_RESET:
       WinPostMsg(hWnd,ALCTLM_SETVALUE, (MPARAM)0L, (MPARAM)0L);
       break;

    case ALCTLM_LEAVE_WAKE_UP_CALL:
       WinSetWindowULong (hWnd, QWL_CALL_BACK_HNDL, (ULONG)mp1);
       WinSetWindowULong (hWnd, QWL_CALL_BACK_MSG, (ULONG)mp2);
       break;
    case ALCTLM_START_COUNTING_UP:
       WinStartTimer((HAB)0,hWnd,TIMER_UP,
                      1000*WinQueryWindowULong(hWnd,QWL_DELTA));
       break;

    case ALCTLM_START_COUNTING_DOWN:
       WinStartTimer((HAB)0,hWnd,TIMER_DOWN,
                      1000*WinQueryWindowULong(hWnd,QWL_DELTA));
       break;

    case ALCTLM_STOP_COUNTING:
       WinStopTimer((HAB)0,hWnd,TIMER_UP);
       WinStopTimer((HAB)0,hWnd,TIMER_DOWN);
       break;

    case WM_TIMER:
       switch((ULONG)mp1)
         { case TIMER_UP:
              WinPostMsg(hWnd,ALCTLM_INCREMENT,(MPARAM)0L,(MPARAM)0L);
              break;
           case TIMER_DOWN:
              WinPostMsg(hWnd,ALCTLM_DECREMENT,(MPARAM)0L,(MPARAM)0L);
              break;
           default: bHandled = ProgressDefault(hWnd,msg,mp1,mp2);//4 delay timer
         }
       break;

    case ALCTLM_DECREMENT:
    case ALCTLM_INCREMENT:
       hps = WinGetPS (hWnd);
       WinQueryWindowRect (hWnd, &Rectl);
       ul1 = WinQueryWindowULong(hWnd,QWL_DIGIT_COUNT);
       PositionCounter(hWnd,hps,&Rectl,ul1);
       Draw_AdjustBorder(hWnd,hps,&Rectl,FALSE);
       ul2 = WinQueryWindowULong(hWnd, QWL_VALUE);
       if (msg == ALCTLM_INCREMENT)
         ul2 += WinQueryWindowULong(hWnd,QWL_DELTA);
       else ul2 = (ul2 <= WinQueryWindowULong(hWnd,QWL_DELTA))?
                          0L : (ul2 - WinQueryWindowULong(hWnd,QWL_DELTA));
       DrawNDigits( hps,&Rectl,WinQueryWindowULong(hWnd,QWL_VALUE),ul2
                   ,WinQueryWindowULong(hWnd,QWL_COLOR_FORE)
                   ,WinQueryWindowULong(hWnd,QWL_COLOR_AFT)
                   ,ul1,TRUE);
       WinSetWindowULong(hWnd,QWL_VALUE,ul2);
       WinReleasePS (hps);
       break;

    case WM_PAINT:
       hps = WinBeginPaint (hWnd,0,0);
       WinQueryWindowRect (hWnd, &Rectl);
       ul1 = WinQueryWindowULong(hWnd,QWL_DIGIT_COUNT);
       PositionCounter(hWnd,hps,&Rectl,ul1);
       Draw_AdjustBorder(hWnd,hps,&Rectl,TRUE);
//   Rectl.xRight++;
       DrawNDigits( hps,&Rectl,0L,WinQueryWindowULong(hWnd,QWL_VALUE)
                   ,WinQueryWindowULong(hWnd,QWL_COLOR_FORE)
                   ,WinQueryWindowULong(hWnd,QWL_COLOR_AFT)
                   ,ul1,FALSE);
       WinEndPaint (hps);
       break;

    case WM_HITTEST:
      return(MRFROMLONG(HT_TRANSPARENT));

    default:
       bHandled = ProgressDefault(hWnd,msg,mp1,mp2);
       break;
  }

 if (!bHandled)
    mReturn = WinDefWindowProc (hWnd,msg,mp1,mp2);
 return (mReturn);
}















//#if 0

HWND FindKid(HWND hWnd,ULONG id)
{
 HWND  hWndChild;
 HENUM  hEnum = WinBeginEnumWindows (hWnd);
 while ((hWndChild = WinGetNextWindow (hEnum)) != 0)
   if (WinQueryWindowUShort(hWndChild,QWS_ID) == id)
      break;
 return hWndChild;
}

ULONG DWPLongestMonth(PDATE_OBJ pDO)
{
 ULONG ulLongest = 0L, ul = 12L;
   while (ul--)
     ulLongest = ALMAX(ulLongest,Mystrlen(pDO->Months[ul]));
 return ulLongest;
}

ULONG DWPMonthNumber(PDATE_OBJ pDO,PSZ szMonth)
{
 ULONG ul = 12;
 if (pDO->fText && szMonth)
   while(ul--)
     if (!strcmp(pDO->Months[ul],szMonth))
        return (++ul);
 return ul;
}

LONG DWPGetValue(HWND hwndSPB)
{
 LONG l = 0L;
 WinSendMsg(hwndSPB,SPBM_QUERYVALUE,MPFROMP(&l),
            MPFROM2SHORT(0,SPBQ_DONOTUPDATE));
 return l;
}

ULONG  DWPDaysInMonth(ULONG ulM, ULONG ulY)
{ return  ((ulM > 0 && ulM < 13)? DaysMonth[ALLEAP(ulY)][ulM-1] : 0 ); }

BOOL DWPValidDate(ULONG ulM, ULONG ulD, ULONG ulY)
{ return (ulD && ulD <= DWPDaysInMonth(ulM,ulY)); }

VOID DWPHandleSpinEnded(PDATE_OBJ pDO)
#if 0
{
 ULONG ulY = DWPGetValue(pDO->SPB[pDO->ulY].hwnd);
 ULONG ulM = DWPGetValue(pDO->SPB[pDO->ulM].hwnd);
 ULONG ulD ;
 if (pDO->fText) ulM++;
 ulD = DWPDaysInMonth(ulM,ulY);
 WinSendMsg(pDO->SPB[pDO->ulD].hwnd, SPBM_SETLIMITS,
            MPFROMLONG(ulD), MPFROMLONG(1L) );
}
#endif
{
 ULONG ulDUpper = 0L
     , ulDLower = 1L
     , ulMUpper = 12
     , ulMLower = 1
     , ulY = DWPGetValue(pDO->SPB[pDO->ulY].hwnd)
     , ulM = DWPGetValue(pDO->SPB[pDO->ulM].hwnd);
 if (pDO->fText) ulM++;
 if (ulY == pDO->SPB[pDO->ulY].Upper)
   { ulMUpper = pDO->SPB[pDO->ulM].Upper;
     if (ulM == ulMUpper)
        ulDUpper = pDO->SPB[pDO->ulD].Upper;
   }
 if (ulY == pDO->SPB[pDO->ulY].Lower)
   { ulMLower = pDO->SPB[pDO->ulM].Lower;
     if (ulM == ulMLower)
        ulDLower = pDO->SPB[pDO->ulD].Lower;
   }
 if (!ulDUpper)
    ulDUpper = DWPDaysInMonth(ulM,ulY);
 WinSendMsg(pDO->SPB[pDO->ulD].hwnd, SPBM_SETLIMITS,
            MPFROMLONG(ulDUpper), MPFROMLONG(ulDLower) );
 if (pDO->fText)
   WinSendMsg(pDO->SPB[pDO->ulM].hwnd, SPBM_SETARRAY,
              MPFROMP(&(pDO->Months[ulMLower-1])),
              MPFROMLONG(ulMUpper - ulMLower + 1));
 else WinSendMsg(pDO->SPB[pDO->ulM].hwnd, SPBM_SETLIMITS,
                 MPFROMLONG(ulMUpper),MPFROMLONG(ulMLower) );
}

VOID  DWPSetDate(PDATE_OBJ pDO,ULONG ulM,ULONG ulD,ULONG ulY)
{
 if (pDO->fText)
    ulM--;
 WinSendMsg(pDO->SPB[pDO->ulM].hwnd,SPBM_SETCURRENTVALUE,MPFROMLONG(ulM),0);
 WinSendMsg(pDO->SPB[pDO->ulD].hwnd,SPBM_SETCURRENTVALUE,MPFROMLONG(ulD),0);
 WinSendMsg(pDO->SPB[pDO->ulY].hwnd,SPBM_SETCURRENTVALUE,MPFROMLONG(ulY),0);
}

VOID  DWPSetDateEasy(PDATE_OBJ pDO,ULONG ulM,ULONG ulD,ULONG ulY)
{
 WinSendMsg(pDO->SPB[pDO->ulY].hwnd,SPBM_SETCURRENTVALUE,MPFROMLONG(ulY),0);
 DWPHandleSpinEnded(pDO);
 WinSendMsg(pDO->SPB[pDO->ulM].hwnd,SPBM_SETCURRENTVALUE,
                     MPFROMLONG(pDO->fText? --ulM : ulM),0);
 DWPHandleSpinEnded(pDO);
 WinSendMsg(pDO->SPB[pDO->ulD].hwnd,SPBM_SETCURRENTVALUE,MPFROMLONG(ulD),0);
}

VOID DWPCreateSpinButtons(PDATE_OBJ pDO,ULONG ulStyle)
{
 ULONG ulBaseSpinFlags = SPBS_READONLY | SPBS_JUSTCENTER | SPBS_FASTSPIN;
 ULONG ulMonthSpinFlags = pDO->fText? SPBS_ALLCHARACTERS : SPBS_NUMERICONLY;
 ULONG ul;
 if (!(ulStyle & ALCTLS_DT_FORMAT_MASK))
   switch(PrfQueryProfileInt(HINI_PROFILE, pPRF_PM_NTL, "iDate",1L))
    {  case 0L: ulStyle |= ALCTLS_DT_FORMAT_MDY; break;
       case 1L: ulStyle |= ALCTLS_DT_FORMAT_DMY; break;
       case 2L: ulStyle |= ALCTLS_DT_FORMAT_YMD; break;
    }
 if (ulStyle & ALCTLS_DT_FORMAT_YMD)
   { pDO->ulY = 0;
     pDO->ulM = 1;
     pDO->ulD = 2;
   }
 else if (ulStyle & ALCTLS_DT_FORMAT_DMY)
        { pDO->ulD = 0;
          pDO->ulM = 1;
          pDO->ulY = 2;
        }
      else { pDO->ulM = 0;
             pDO->ulD = 1;
             pDO->ulY = 2;
           }

 pDO->SPB[pDO->ulM].hwnd = WinCreateWindow( pDO->hwnd, WC_SPINBUTTON,NULL,
                              ulBaseSpinFlags | ulMonthSpinFlags |
                              ((pDO->ulM == 2)? SPBS_MASTER : SPBS_SERVANT),
                              0L,0L,0L,0L,pDO->hwnd,HWND_TOP,
                              (pDO->SPB[pDO->ulM].id = pDO->id + 1 + pDO->ulM),
                              NULL,NULL);

 pDO->SPB[pDO->ulD].hwnd = WinCreateWindow( pDO->hwnd, WC_SPINBUTTON,NULL,
                            ulBaseSpinFlags | SPBS_NUMERICONLY |
                              ((pDO->ulD == 2)? SPBS_MASTER : SPBS_SERVANT),
                            0L,0L,0L,0L,pDO->hwnd,HWND_TOP,
                            (pDO->SPB[pDO->ulD].id = pDO->id + 1 + pDO->ulD),
                            NULL,NULL);

 pDO->SPB[pDO->ulY].hwnd = WinCreateWindow( pDO->hwnd, WC_SPINBUTTON,NULL,
                             ulBaseSpinFlags | SPBS_NUMERICONLY |
                              ((pDO->ulY == 2)? SPBS_MASTER : SPBS_SERVANT),
                             0L,0L,0L,0L,pDO->hwnd,HWND_TOP,
                             (pDO->SPB[pDO->ulY].id = pDO->id + 1 + pDO->ulY),
                             NULL,NULL);

 WinSendMsg(pDO->SPB[0].hwnd,SPBM_SETMASTER,
            (MPARAM)pDO->SPB[2].hwnd,(MPARAM)0L);
 WinSendMsg(pDO->SPB[1].hwnd,SPBM_SETMASTER,
            (MPARAM)pDO->SPB[2].hwnd,(MPARAM)0L);

 for (ul=0;ul<3;ul++)
   pDO->SPB[ul].hwndEF = FindKid(pDO->SPB[ul].hwnd,pDO->SPB[ul].id);

 WinSendMsg(pDO->SPB[pDO->ulD].hwnd, SPBM_SETLIMITS,
            MPFROMLONG(31L), MPFROMLONG(1L) );
 WinSendMsg(pDO->SPB[pDO->ulD].hwnd, SPBM_SETTEXTLIMIT,
            MPFROMSHORT(2),(MPARAM)0L);
 WinSendMsg(pDO->SPB[pDO->ulY].hwnd, SPBM_SETLIMITS,
            MPFROMLONG(2050L), MPFROMLONG(1950L) );
 WinSendMsg(pDO->SPB[pDO->ulD].hwnd, SPBM_SETTEXTLIMIT,
            MPFROMSHORT(4),(MPARAM)0L);
 if (pDO->fText)
   { WinSendMsg(pDO->SPB[pDO->ulM].hwnd, SPBM_SETARRAY,
                MPFROMP(pDO->Months), MPFROMLONG(12));
     WinSendMsg(pDO->SPB[pDO->ulM].hwnd, SPBM_SETTEXTLIMIT,
                MPFROMSHORT(DWPLongestMonth(pDO)),(MPARAM)0L);
   }
 else { WinSendMsg(pDO->SPB[pDO->ulM].hwnd, SPBM_SETLIMITS,
                 MPFROMLONG(12L), MPFROMLONG(1L) );
        WinSendMsg(pDO->SPB[pDO->ulM].hwnd, SPBM_SETTEXTLIMIT,
                   MPFROMSHORT(2),(MPARAM)0L);
      }


}

VOID DWPSetMinSize(PDATE_OBJ pDO)
{
 ULONG ulW = pDO->rctBorder.xLeft + pDO->SPB[0].cx +
             pDO->SPB[1].cx + pDO->SPB[2].cx + pDO->rctBorder.xRight;
 ULONG ulH = pDO->rctBorder.yBottom + pDO->cy + pDO->rctBorder.yTop;
 WinSetWindowULong(pDO->hwnd,QWL_MIN_SIZE,MAKELONG((USHORT)ulW,(USHORT)ulH));
}

VOID DWPShrinkToMinimumSize(HWND hWnd, PRECTL prct)
{
 ULONG ul = WinQueryWindowULong (hWnd, QWL_MIN_SIZE);
 if ((prct->xRight - prct->xLeft) > (ULONG)LOUSHORT(ul))
   prct->xRight = prct->xLeft + (ULONG)LOUSHORT(ul);
 if ((prct->yTop - prct->yBottom) > (ULONG)HIUSHORT(ul))
   prct->yTop = prct->yBottom + (ULONG)HIUSHORT(ul);
}

VOID DWPSetSizes(HPS hps,PDATE_OBJ pDO)
{
 ULONG ul,ul2;
 RECTL rct;
 WinQueryWindowRect (pDO->SPB[0].hwnd, &rct);
// TextBox(hps,"000",&(pDO->SPB[pDO->ulD].cx),&(pDO->cy));
 TextBox(hps,"000",&(pDO->SPB[pDO->ulD].cx),NULL);
 pDO->cy = (rct.yTop-rct.yBottom);
// pDO->cy += 8;
 TextBox(hps,"000000",&(pDO->SPB[pDO->ulY].cx),NULL);
 if (pDO->fText)
   { for (pDO->SPB[pDO->ulM].cx = ul = 0L ; ul < 12L ; ul++)
       { TextBox(hps,pDO->Months[ul],(PLONG)&ul2,NULL);
         if (ul2 > pDO->SPB[pDO->ulM].cx)
            pDO->SPB[pDO->ulM].cx = ul2;
       }
     pDO->SPB[pDO->ulM].cx += 8;
   }
 else pDO->SPB[pDO->ulM].cx = pDO->SPB[pDO->ulD].cx;
 pDO->SPB[2].cx += WinQuerySysValue(HWND_DESKTOP,SV_CXVSCROLL);
// pDO->SPB[2].cx += (rct.xRight-rct.xLeft);
 DWPSetMinSize(pDO);
}


VOID DWPLoadMonths(PDATE_OBJ pDO, PCHAR * months)
{
 ULONG ul;
 if (!months)
    months = DefaultArr;
 for (ul = 0; ul < 12 ; ul++)
   Mystrncpy(pDO->Months[ul],*(months+ul),100);
}

VOID DWPPosition(PDATE_OBJ pDO)
{
 ULONG ulLeft,ul;
 SWP aSwp[3];
 for (ulLeft = pDO->rctBorder.xLeft, ul = 0 ; ul < 3 ;ul++)
   { aSwp[ul].fl   = SWP_MOVE | SWP_SIZE;
     aSwp[ul].hwnd = pDO->SPB[ul].hwnd;
     aSwp[ul].x    = ulLeft;
     aSwp[ul].y    = pDO->rctBorder.yBottom;
     aSwp[ul].cx   = pDO->SPB[ul].cx;
     aSwp[ul].cy   = pDO->cy;
     ulLeft += pDO->SPB[ul].cx;
   }
 WinSetMultWindowPos((HAB)0L,aSwp,ul);
}

#if 0
VOID DWPPosition(PDATE_OBJ pDO)
{
 ULONG ulLeft,ul;
 for (ulLeft = pDO->rctBorder.xLeft, ul = 0 ; ul < 3 ;ul++)
   { WinSetWindowPos (pDO->SPB[ul].hwnd, 0,
                      ulLeft,pDO->rctBorder.yBottom, pDO->SPB[ul].cx,pDO->cy,
                      SWP_MOVE | SWP_SIZE);
     ulLeft += pDO->SPB[ul].cx;
   }
}
#endif


MRESULT DWPHandleGroupTab(PDATE_OBJ pDO,SHORT sDirection)
{
 SHORT s;
 HWND hwndFocus = WinQueryFocus(HWND_DESKTOP);
 for (s=0;s<3;s++)
   if (pDO->SPB[s].hwndEF == hwndFocus)
     break;
 if (s < 3)
   { s += ((sDirection == VK_RIGHT)? 1 : -1);
     s = (s < 0)? 2 : ( (s == 3)? 0 : s);
     WinSetFocus(HWND_DESKTOP,pDO->SPB[s].hwnd);
   }
 return (MRESULT) TRUE;
}

BOOL DWPHandleTab(PDATE_OBJ pDO,SHORT sTabType)
{
 SHORT s;
 HWND hwndFocus = WinQueryFocus(HWND_DESKTOP);
 for (s=0;s<3;s++)
   if (pDO->SPB[s].hwndEF == hwndFocus)
     break;
 if (s < 3)
   { s += ((sTabType == VK_TAB)? 1 : -1);
     if (s >= 0 && s < 3)
        WinSetFocus(HWND_DESKTOP,pDO->SPB[s].hwnd);
     else {
//          WinSetFocus(HWND_DESKTOP,pDO->hwnd);
//          return FALSE;
            WinPostMsg(pDO->hwnd,(s>=3)?
              ALCTLM_TAB_OUT_RIGHT:ALCTLM_TAB_OUT_LEFT,(MPARAM)0L,(MPARAM)0L);
          }
   }
 else WinSetFocus(HWND_DESKTOP,pDO->SPB[0].hwnd); // getting focus
 return TRUE;
}
#if 0
VOID DWPSetLimits(PDATE_OBJ pDO,ULONG ulM,ULONG ulD,ULONG ulY,BOOL fUpper)
{
 ulM = ALMIN(ALMAX(1,ulM),12);
 ulD = ALMIN(ALMAX(1,ulD),DWPDaysInMonth(ulM,ulY));
 if (fUpper)
   { pDO->SPB[pDO->ulD].Upper = ulD;
     pDO->SPB[pDO->ulM].Upper = ulM;
     pDO->SPB[pDO->ulY].Upper = ulY;
   }
 else
   { pDO->SPB[pDO->ulD].Lower = ulD;
     pDO->SPB[pDO->ulM].Lower = ulM;
     pDO->SPB[pDO->ulY].Lower = ulY;
   }
 if (pDO->SPB[pDO->ulY].Lower || pDO->SPB[pDO->ulY].Upper)
   { if (!pDO->SPB[pDO->ulY].Upper)
       pDO->SPB[pDO->ulY].Upper = 2050;
     if (pDO->SPB[pDO->ulY].Upper < pDO->SPB[pDO->ulY].Lower)
        pDO->SPB[pDO->ulY].Upper = pDO->SPB[pDO->ulY].Lower;
     WinSendMsg(pDO->SPB[pDO->ulY].hwnd, SPBM_SETLIMITS,
                MPFROMLONG(pDO->SPB[pDO->ulY].Upper),
                MPFROMLONG(pDO->SPB[pDO->ulY].Lower) );
     if (pDO->SPB[pDO->ulY].Upper == pDO->SPB[pDO->ulY].Lower)
       { if (pDO->SPB[pDO->ulM].Upper < pDO->SPB[pDO->ulM].Lower)
         pDO->SPB[pDO->ulM].Upper = pDO->SPB[pDO->ulM].Lower;
         if (pDO->SPB[pDO->ulM].Upper == pDO->SPB[pDO->ulM].Lower)
           { if (pDO->fText)
             WinSendMsg(pDO->SPB[pDO->ulM].hwnd, SPBM_SETARRAY,
                        MPFROMP(&(pDO->Months[pDO->SPB[pDO->ulM].Lower-1])),
                        MPFROMLONG(pDO->SPB[pDO->ulM].Upper -
                                   pDO->SPB[pDO->ulM].Lower + 1));
             else WinSendMsg(pDO->SPB[pDO->ulM].hwnd, SPBM_SETLIMITS,
                             MPFROMLONG(pDO->SPB[pDO->ulM].Upper),
                             MPFROMLONG(pDO->SPB[pDO->ulM].Lower) );
             if (pDO->SPB[pDO->ulD].Upper < pDO->SPB[pDO->ulD].Lower)
               pDO->SPB[pDO->ulD].Upper = pDO->SPB[pDO->ulD].Lower;
             WinSendMsg(pDO->SPB[pDO->ulD].hwnd, SPBM_SETLIMITS,
                        MPFROMLONG(pDO->SPB[pDO->ulD].Upper),
                        MPFROMLONG(pDO->SPB[pDO->ulD].Lower) );
           }
       }
   }
}
VOID DWPSetLimits(PDATE_OBJ pDO,ULONG ulM,ULONG ulD,ULONG ulY,BOOL fUpper)
{
 ulM = ALMIN(ALMAX(1,ulM),12);
 ulD = ALMIN(ALMAX(1,ulD),DWPDaysInMonth(ulM,ulY));
 if (fUpper)
   { pDO->SPB[pDO->ulD].Upper = ulD;
     pDO->SPB[pDO->ulM].Upper = ulM;
     pDO->SPB[pDO->ulY].Upper = ulY;
   }
 else
   { pDO->SPB[pDO->ulD].Lower = ulD;
     pDO->SPB[pDO->ulM].Lower = ulM;
     pDO->SPB[pDO->ulY].Lower = ulY;
   }
 if (pDO->SPB[pDO->ulY].Lower || pDO->SPB[pDO->ulY].Upper)
   { if (!pDO->SPB[pDO->ulY].Upper)
       pDO->SPB[pDO->ulY].Upper = 2050;
     if (pDO->SPB[pDO->ulY].Upper < pDO->SPB[pDO->ulY].Lower)
        pDO->SPB[pDO->ulY].Upper = pDO->SPB[pDO->ulY].Lower;
     WinSendMsg(pDO->SPB[pDO->ulY].hwnd, SPBM_SETLIMITS,
                MPFROMLONG(pDO->SPB[pDO->ulY].Upper),
                MPFROMLONG(pDO->SPB[pDO->ulY].Lower) );
     if (pDO->SPB[pDO->ulY].Upper == pDO->SPB[pDO->ulY].Lower)
       { if (pDO->SPB[pDO->ulM].Upper < pDO->SPB[pDO->ulM].Lower)
         pDO->SPB[pDO->ulM].Upper = pDO->SPB[pDO->ulM].Lower;
         if (pDO->SPB[pDO->ulM].Upper == pDO->SPB[pDO->ulM].Lower)
           { if (pDO->fText)
             WinSendMsg(pDO->SPB[pDO->ulM].hwnd, SPBM_SETARRAY,
                        MPFROMP(&(pDO->Months[pDO->SPB[pDO->ulM].Lower-1])),
                        MPFROMLONG(pDO->SPB[pDO->ulM].Upper -
                                   pDO->SPB[pDO->ulM].Lower + 1));
             else WinSendMsg(pDO->SPB[pDO->ulM].hwnd, SPBM_SETLIMITS,
                             MPFROMLONG(pDO->SPB[pDO->ulM].Upper),
                             MPFROMLONG(pDO->SPB[pDO->ulM].Lower) );
             if (pDO->SPB[pDO->ulD].Upper < pDO->SPB[pDO->ulD].Lower)
               pDO->SPB[pDO->ulD].Upper = pDO->SPB[pDO->ulD].Lower;
             WinSendMsg(pDO->SPB[pDO->ulD].hwnd, SPBM_SETLIMITS,
                        MPFROMLONG(pDO->SPB[pDO->ulD].Upper),
                        MPFROMLONG(pDO->SPB[pDO->ulD].Lower) );
           }
       }
   }
}
#endif

VOID DWPSetLimits(PDATE_OBJ pDO,ULONG ulM,ULONG ulD,ULONG ulY,BOOL fUpper)
{
 ulM = ALMIN(ALMAX(1,ulM),12);
 ulD = ALMIN(ALMAX(1,ulD),DWPDaysInMonth(ulM,ulY));
 if (fUpper)
   { pDO->SPB[pDO->ulD].Upper = ulD;
     pDO->SPB[pDO->ulM].Upper = ulM;
     pDO->SPB[pDO->ulY].Upper = ulY;
   }
 else
   { pDO->SPB[pDO->ulD].Lower = ulD;
     pDO->SPB[pDO->ulM].Lower = ulM;
     pDO->SPB[pDO->ulY].Lower = ulY;
   }
 if (pDO->SPB[pDO->ulY].Lower || pDO->SPB[pDO->ulY].Upper)
   { if (!pDO->SPB[pDO->ulY].Upper)
       pDO->SPB[pDO->ulY].Upper = 2050;
     if (pDO->SPB[pDO->ulY].Upper < pDO->SPB[pDO->ulY].Lower)
        pDO->SPB[pDO->ulY].Upper = pDO->SPB[pDO->ulY].Lower;
     WinSendMsg(pDO->SPB[pDO->ulY].hwnd, SPBM_SETLIMITS,
                MPFROMLONG(pDO->SPB[pDO->ulY].Upper),
                MPFROMLONG(pDO->SPB[pDO->ulY].Lower) );
   }
}

BOOL DWPFitLimits(PDATE_OBJ pDO,ULONG ulM,ULONG ulD,ULONG ulY,BOOL fMakeItFit)
{
 if ( pDO->SPB[pDO->ulM].Upper                              &&
      ( (ulY > pDO->SPB[pDO->ulY].Upper)                  ||
        ( (ulY == pDO->SPB[pDO->ulY].Upper)             &&
            ( ( ulM > pDO->SPB[pDO->ulM].Upper)      ||
              ( ( ulM == pDO->SPB[pDO->ulM].Upper) &&
                ( ulD > pDO->SPB[pDO->ulD].Upper) ) ) ) ) )
    if (fMakeItFit)
       DWPSetLimits(pDO,ulM,ulD,ulY,TRUE);
    else return FALSE;
 if ( pDO->SPB[pDO->ulM].Lower                              &&
       ( (ulY < pDO->SPB[pDO->ulY].Lower)                 ||
         ( (ulY == pDO->SPB[pDO->ulY].Lower)            &&
             ( ( ulM < pDO->SPB[pDO->ulM].Lower)      ||
               ( ( ulM == pDO->SPB[pDO->ulM].Lower) &&
                 ( ulD < pDO->SPB[pDO->ulD].Lower) ) ) ) ) )
    if (fMakeItFit)
       DWPSetLimits(pDO,ulM,ulD,ulY,FALSE);
    else return FALSE;
 return TRUE;
}

BOOL  DWPFitLimit_SetDate(PDATE_OBJ pDO,ULONG ulM,ULONG ulD,ULONG ulY)
{
 if (DWPValidDate(ulM,ulD,ulY))
   { DWPFitLimits(pDO,ulM,ulD,ulY,TRUE);
     DWPSetDateEasy(pDO,ulM,ulD,ulY);
     return TRUE;
   }
 return FALSE;
}

VOID  DWPSetToToday( PDATE_OBJ pDO )
{
 DATETIME  DT;
 DosGetDateTime(&DT);
 DWPFitLimit_SetDate(pDO,(ULONG)DT.month,(ULONG)DT.day,(ULONG)DT.year);
}

BOOL  DWPDayCrement(PDATE_OBJ pDO,BOOL fIn)
{
 ULONG ulM = DWPGetValue(pDO->SPB[pDO->ulM].hwnd)
     , ulD = DWPGetValue(pDO->SPB[pDO->ulD].hwnd)
     , ulY = DWPGetValue(pDO->SPB[pDO->ulY].hwnd);
 if (pDO->fText) ulM++;
 if (fIn)
   { if ( ++ulD > DWPDaysInMonth(ulM,ulY))
       { ulD = 1;
         if (++ulM == 13L)
           { ulY++;
             ulM = 1L;
           }
       }
   }
 else if (!(--ulD))
        { if (!(--ulM))
            { ulY--;
              ulM = 12L;
            }
          ulD = DWPDaysInMonth(ulM,ulY);
        }
 if (DWPFitLimits(pDO,ulM,ulD,ulY,FALSE))
   { DWPSetDateEasy(pDO,ulM,ulD,ulY); //do all - spbs do not repaint already dislayed text
     return TRUE;
   }
 return FALSE;
}

BOOL DWPDelta(PDATE_OBJ pDO,ULONG ulMDelta, ULONG ulDDelta,ULONG ulYDelta,
              BOOL fAdd)
{
 BOOL fMonthEnd;
 ULONG ulM = DWPGetValue(pDO->SPB[pDO->ulM].hwnd)
     , ulD = DWPGetValue(pDO->SPB[pDO->ulD].hwnd)
     , ulY = DWPGetValue(pDO->SPB[pDO->ulY].hwnd);
 if (pDO->fText) ulM++;
 if (fAdd)
   {
    if (ulYDelta)
      { ulY += ulYDelta;              // add years and retain Feb's integrity
        if ( (ulD==29) && (ulM == 2) && !(ALLEAP(ulY)) )
          ulD--;
      }
    if (ulMDelta)
      { fMonthEnd = (ulD == DWPDaysInMonth(ulM,ulY));
        ulY += (ulMDelta/12);   // take the years out of the 2b-added months
        ulMDelta %= 12;
        ulY += (ulM + ulMDelta)/13;     // take the years out of the total months
        ulM = ((ulM + ulMDelta) > 12) ?  (ulM + ulMDelta) % 12 : ulM + ulMDelta;
        if (fMonthEnd)
           ulD = DWPDaysInMonth(ulM,ulY);      // retain month-end feature
      }
    ulD = ALMIN(DWPDaysInMonth(ulM,ulY),ulD); // retain days per month integrity
    ulD += ulDDelta;
    while (ulD > DWPDaysInMonth(ulM,ulY)) // distribute total days over months
      { ulD -= DWPDaysInMonth(ulM++,ulY);
        if (ulM > 12)
          ulM = 1, ulY++;
      }
   }
 else
   {
    if(ulYDelta)
      { ulY -= ulYDelta;
        if ( (ulD == 29) && (ulM == 2) && !(ALLEAP(ulY)) )
          ulD--;
      }
    if(ulMDelta)
      { fMonthEnd = ( ulD == DWPDaysInMonth(ulM,ulY));
        ulY -= (ulMDelta/12);      // 1st take care of the years in the months
        if (ulM > ulMDelta%12)
          ulM -= ulMDelta%12;    // enough months to subtract the remaining months
        else                   // not enough months to subtract the remaining months
           { ulM = ulM+12 - (ulMDelta%12);     // borrow from the years
             ulY -=1;
           }
       if (fMonthEnd)                        // if it was a month end retain it
         ulD = DWPDaysInMonth(ulM,ulY);      // retain month-end feature
      }
    while (ulDDelta >= ulD)
      { if (ulM-- == 1)
          { ulM = 12;
            ulY -=1;
          }
        ulD += DWPDaysInMonth(ulM,ulY);
      }
    ulD -= ulDDelta;
    ulD = ALMIN(DWPDaysInMonth(ulM,ulY),ulD);
   }
 if (DWPFitLimits(pDO,ulM,ulD,ulY,FALSE))
   { DWPSetDateEasy(pDO,ulM,ulD,ulY); //do all - spbs do not repaint already dislayed text
     return TRUE;
   }
 return FALSE;
}

#define  K_ENTER   0X0D

MRESULT EXPENTRY SpinDateWndProc (HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{

 BOOL    bHandled = TRUE, fToOwner = FALSE,f;
 MRESULT mReturn  = 0;
 RECTL   Rectl;
 HPS     hps;
 ULONG   ulStyle,ul;
 PCREATESTRUCT pcrst;
 PDATE_OBJ pDO;

 switch (msg)
  {
   case WM_CREATE:
       ProgressDefault(hWnd,msg,mp1,mp2);
//       WinSetWindowULong (hWnd, QWL_COLOR_AFT,(ULONG)SYSCLR_WINDOW);
       DosAllocMem((PPVOID)&pDO,sizeof(*pDO),PAG_READ | PAG_WRITE | PAG_COMMIT);
       WinSetWindowULong(hWnd,QWL_DATE_OBJ,(ULONG)pDO);
       pDO->hwnd = hWnd;
       pcrst = (PCREATESTRUCT)PVOIDFROMMP(mp2);
       pDO->rctReq.xLeft  = pDO->rctReq.yBottom = 0L;
       pDO->rctReq.xRight = pcrst->cx;
       pDO->rctReq.yTop   = pcrst->cy;
       pDO->id = pcrst->id;
       pDO->hwndOwner = pcrst->hwndOwner;
       ulStyle = WinQueryWindowULong (hWnd, QWL_STYLE);
       hps = WinGetPS (hWnd);
       pDO->rctBorder.xLeft = pDO->rctBorder.yBottom = 0L;
       pDO->rctBorder.xRight = pDO->rctBorder.yTop = 100L;
       Draw_AdjustBorder(hWnd,hps,&(pDO->rctBorder),FALSE);
       pDO->rctBorder.xRight = 100L - pDO->rctBorder.xRight;
       pDO->rctBorder.yTop = 100L - pDO->rctBorder.yTop;
//     WinSetWindowULong (hWnd, QWL_COLOR_AFT,
//                        lGetPresParam(pDO->hwndOwner, PP_BACKGROUNDCOLOR,
//                                      PP_BACKGROUNDCOLORINDEX,
//                                      SYSCLR_DIALOGBACKGROUND));
       WinSetWindowULong (hWnd, QWL_COLOR_AFT, SYSCLR_DIALOGBACKGROUND);

       if (pDO->fText = (ulStyle & ALCTLS_DT_TEXT_MONTHS))
         { for (ul = 0; ul < 12 ; ul++)
              pDO->Months[ul] = pDO->Names[ul];
           DWPLoadMonths(pDO,NULL);
         }
       DWPCreateSpinButtons(pDO,ulStyle);
       DWPSetSizes(hps,pDO);
       DWPPosition(pDO);
       DWPSetToToday(pDO);
       for (ul = 0; ul < 3 ; ul++)
         WinShowWindow(pDO->SPB[ul].hwnd,TRUE);
       WinReleasePS (hps);
       break;

    case WM_ERASEBACKGROUND :
       return(MRFROMLONG(TRUE));

    case WM_PRESPARAMCHANGED:
//       WinPostMsg(hWnd,ALCTLM_MINIMUM_SIZE,(MPARAM)0L, (MPARAM)0L);
       hps = WinGetPS (hWnd);
       DWPSetSizes(hps,(PDATE_OBJ)WinQueryWindowULong(hWnd,QWL_DATE_OBJ));
       WinReleasePS (hps);
       WinInvalidateRect (hWnd, NULL, TRUE);
       break;

    case ALCTLM_MINIMUM_SIZE:
       hps = WinGetPS (hWnd);
       DWPSetSizes(hps,(PDATE_OBJ)WinQueryWindowULong(hWnd,QWL_DATE_OBJ));
       WinReleasePS (hps);
       ul = WinQueryWindowULong (hWnd, QWL_MIN_SIZE);
       if (mp1)
          *((PULONG)mp1) = (ULONG)(LOUSHORT(ul)+1);
       if (mp2)
       *((PULONG)mp2) = (ULONG)(HIUSHORT(ul));
//       WinInvalidateRect (hWnd, NULL, TRUE);
       break;

    case WM_PAINT:
       hps = WinBeginPaint (hWnd,0,0);
       WinQueryWindowRect (hWnd, &Rectl);
       DWPShrinkToMinimumSize(hWnd,&Rectl);
//     pDO = (PDATE_OBJ)WinQueryWindowULong(hWnd,QWL_DATE_OBJ);
//     Rectl.yTop = Rectl.yBottom + pDO->rctBorder.yBottom + pDO->cy +
//                    pDO->rctBorder.yTop;
       Draw_AdjustBorder(hWnd,hps,&Rectl,TRUE);
       DWPPosition((PDATE_OBJ)WinQueryWindowULong(hWnd,QWL_DATE_OBJ));
       WinEndPaint (hps);
       break;

   case ALCTLM_DATE_SET_MONTH_NAMES:
       pDO = (PDATE_OBJ)WinQueryWindowULong(hWnd,QWL_DATE_OBJ);
       if (pDO->fText)
         { DWPLoadMonths(pDO,(PCHAR *)mp1);
           hps = WinGetPS (hWnd);
           DWPSetSizes(hps,pDO);
           WinReleasePS (hps);
           WinInvalidateRect (hWnd, NULL, TRUE);
         }
       return (MRESULT)(pDO->fText);

   case ALCTLM_SETCOLORS:
       pDO = (PDATE_OBJ)WinQueryWindowULong(hWnd,QWL_DATE_OBJ);
       for (ul = 0; ul < 3 ; ul++)
         { WinSetPresParam (pDO->SPB[ul].hwndEF, PP_FOREGROUNDCOLORINDEX,
                            4L, (PVOID)&mp1);
           WinSetPresParam (pDO->SPB[ul].hwndEF, PP_BACKGROUNDCOLORINDEX,
                            4L, (PVOID)&mp2);
         }
       WinInvalidateRect (hWnd, NULL, FALSE);
       break;

   case WM_CONTROL:
        if (HIUSHORT(mp1) == SPBN_ENDSPIN)
          { pDO = (PDATE_OBJ)WinQueryWindowULong(hWnd,QWL_DATE_OBJ);
            if ((SHORT1FROMMP(mp1) == (SHORT)pDO->SPB[pDO->ulY].id) ||
                (SHORT1FROMMP(mp1) == (SHORT)pDO->SPB[pDO->ulM].id))
              DWPHandleSpinEnded(pDO);
          }
       break;

   case WM_CHAR:
       if (SHORT1FROMMP(mp1) & KC_KEYUP)
        { pDO = (PDATE_OBJ)WinQueryWindowULong(hWnd,QWL_DATE_OBJ);
          if ( SHORT1FROMMP(mp1) & KC_VIRTUALKEY)
            { switch (SHORT2FROMMP(mp2))
               { case VK_F3:
                 case VK_ESC:
                   fToOwner = TRUE;
                   break;
                 case VK_BACKTAB:
                 case VK_TAB:
                   fToOwner = !DWPHandleTab(pDO,SHORT2FROMMP(mp2));
  //               bHandled = DWPHandleTab(pDO,SHORT2FROMMP(mp2));
                   break;
                 case VK_LEFT:
                 case VK_RIGHT: return DWPHandleGroupTab(pDO,SHORT2FROMMP(mp2));
               }
            }
         if ( SHORT1FROMMP(mp1) & KC_CHAR )
            { switch (SHORT1FROMMP(mp2))
               { case K_ENTER:
                   fToOwner = TRUE;
                   break;
               }
            }
        }
      break;

     case WM_SETFOCUS :  // trow away msg - depending on WM_CHAR-tab
//      if ((USHORT)mp2)
//         { pDO = (PDATE_OBJ)WinQueryWindowULong(hWnd,QWL_DATE_OBJ);
//           pDO->fAcceptCharMsg = FALSE;
//           WinPostMsg(hWnd,ALCTLM_SET_FOCUS,(MPARAM)0L,(MPARAM)0L);
//         }
      break;

   case ALCTLM_SET_FOCUS:
      WinSetFocus(HWND_DESKTOP,
              ((PDATE_OBJ)WinQueryWindowULong(hWnd,QWL_DATE_OBJ))->SPB[0].hwnd);
      break;

   case ALCTLM_DATE_SET_LIMIT:
       DWPSetLimits((PDATE_OBJ)WinQueryWindowULong(hWnd,QWL_DATE_OBJ),
                     (ULONG)SHORT1FROMMP(mp1),
                     (ULONG)SHORT2FROMMP(mp1),
                     (ULONG)SHORT1FROMMP(mp2),
                     (SHORT2FROMMP(mp2) != 0));
       break;

   case ALCTLM_DATE_SET:
       return (MRESULT)DWPFitLimit_SetDate(
                       (PDATE_OBJ)WinQueryWindowULong(hWnd,QWL_DATE_OBJ),
                       (ULONG)SHORT1FROMMP(mp1),(ULONG)SHORT2FROMMP(mp1),
                       (ULONG)SHORT1FROMMP(mp2));

   case ALCTLM_DATE_ADD:
   case ALCTLM_DATE_SUBTRACT:
       return (MRESULT)DWPDelta(
                          (PDATE_OBJ)WinQueryWindowULong(hWnd,QWL_DATE_OBJ),
                          (ULONG)SHORT1FROMMP(mp1),
                          (ULONG)SHORT2FROMMP(mp1),
                          (ULONG)SHORT1FROMMP(mp2),
                          (msg==ALCTLM_DATE_ADD));

   case ALCTLM_DATE_GET:
       pDO = (PDATE_OBJ)WinQueryWindowULong(hWnd,QWL_DATE_OBJ);
       ul = DWPGetValue(pDO->SPB[pDO->ulM].hwnd);
       if (pDO->fText) ul++;
       *((PULONG)mp1) = MAKEULONG(ul,DWPGetValue(pDO->SPB[pDO->ulD].hwnd));
       *((PULONG)mp2) = DWPGetValue(pDO->SPB[pDO->ulY].hwnd);
       break;

   case ALCTLM_INCREMENT:
   case ALCTLM_DECREMENT:
       return (MRESULT)DWPDayCrement(
                       (PDATE_OBJ)WinQueryWindowULong(hWnd,QWL_DATE_OBJ),
                       (msg == ALCTLM_INCREMENT));

   case ALCTLM_RESET:
   case ALCTLM_DATE_SET_TODAY:
       DWPSetToToday((PDATE_OBJ)WinQueryWindowULong(hWnd,QWL_DATE_OBJ));
       break;

   case WM_DESTROY :
       DosFreeMem((PVOID)WinQueryWindowPtr(hWnd,QWL_DATE_OBJ));
       break;

    default:
       bHandled = ProgressDefault(hWnd,msg,mp1,mp2);
       break;
  }

   if (fToOwner)
      return(MRFROMLONG(WinSendMsg(pDO->hwndOwner, msg, mp1, mp2)));

 if (!bHandled)
    mReturn = WinDefWindowProc (hWnd,msg,mp1,mp2);
 return (mReturn);
}
//#endif
#if 0

#endif











