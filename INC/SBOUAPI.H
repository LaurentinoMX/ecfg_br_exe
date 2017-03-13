#ifndef __SBOUAPI_H
#define __SBOUAPI_H

/***************************************************************************

    Contains:      User API functions for SB for OS/2
    Creation Date: 07/06/96

***************************************************************************/


#include <trilapi.h>
//#include <idmgr/idmtypes.h>
#include <engtypes.h>

#if defined(__cplusplus)
extern "C" {
#endif

/***** Data Types *****/
typedef long SBOID;                    /* SB OS/2 ID */
typedef long SBOcfgobjID;
typedef IteratorID SBOConfigObjectIteratorID;

long FARPASCAL sbo_GetHwnd(SBOID sboid, HWND *hwnd);
#define sb_getHwnd(S, H) sbo_getHwnd((S), (H))

/* config object */

long FARPASCAL sbo_OpenConfigObject(char *filename, SBOcfgobjID *cfgobjID);
long FARPASCAL sbo_CloseConfigObject(SBOcfgobjID cfgobjID);
long FARPASCAL sbo_AddConfigObjectFile(SBOcfgobjID cfgobjID,
                                       const char *key, const char *filename);
long FARPASCAL sbo_ExtractConfigObjectFile(SBOcfgobjID cfgobjID,
                                           const char *key, const char *filename);
long FARPASCAL sbo_DestroyConfigObjectFile(SBOcfgobjID cfgobjID,
                                           const char *key);
long FARPASCAL sbo_ContainsConfigObjectFile(SBOcfgobjID cfgobjID,
                                            const char *key,
                                            int *yes_or_no);
long FARPASCAL sbo_GetConfigObjectList(SBOcfgobjID cfgobjID,
                                       SBOConfigObjectIteratorID *coit);

long FARPASCAL sbo_SetConfigObjectName(SBOID sboid, char *name);

/* Config object callbacks - called by custom menu options */
long FARPASCAL sbo_GetConfigObjectName(SBOID sboid, char *name, int nameLen);
#define sb_getConfigObjectName(S, N, L) sbo_getConfigObjectName((S), (N), (L))


/***** Iterator functions *****/

long FARPASCAL sbo_Next(IteratorID it, long* id);
long FARPASCAL sbo_NextString(IteratorID it, char **ps);
long FARPASCAL sbo_Count(IteratorID it, int* cnt);
long FARPASCAL sbo_FreeIterator(IteratorID it);

#if defined(__cplusplus)
}
#endif

#endif
