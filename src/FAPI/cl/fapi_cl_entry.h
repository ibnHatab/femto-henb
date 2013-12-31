#ifndef _FAPI_CL_ENTRY_H_
#define _FAPI_CL_ENTRY_H_

#include "4gmx.h"
#include "appids.h"
#include "cmgr.h"

/*
 * Application context
 */
typedef struct
{
    UINT32 RegComplete;
    HANDLE hLteNmmThread;
    UINT32 nMacUserInstanceId;
    UINT32 nPhyUserInstanceId;
} FapiClAppCtx;

/*
 * Global Application Callback
 */
extern MXRC (*LteMacAppInitCallback)(void);

/** This function is called by the function of the L-ARM application init.
           (the function AppInitCpu0 of appinit.c file).

    @return [MXRC] the initialization error code (0 (or MX_OK) - OK)
*/
MXRC FapiClInitHandler(void);

/** This function will be called by the console manager in case of
            running <lte> console command.

    @NOTE!!!  This function is specially designed for debug purpose.

    @param pCtx     [in] - the console manager context (it can be skipped )
    @param pParams  [in] - the command parameters specified for console command
                           for example: "lte myparam1 myparam2 ... mypaamN"

    @return [UINT32] it should return 0 (or) MX_OK

 */
UINT32 FapiClConsoleHandler(CMgrCmdCtx *pCtx, CMgrCmdParamCtx *pParams);


#endif /* _FAPI_CL_ENTRY_H_ */
