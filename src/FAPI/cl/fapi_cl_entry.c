/** @file fapi_cl_entry.c
 *
 *
**/

#include <stdint.h>
#include <string.h>

#include "resultcodes.h"
#include "basetypes.h"
#include "typedef.h"
#include "appprintf.h"
#include "supervisor.h"
#include "heaps.h"
#include "larmmsgid.h"
#include "diagnostics.h"
#include "svsrlogger.h"
#include "mlog.h"

#include "lte_entry.h"

#include <LtePhyL2Api.h>

#include <l1_types.h>
#include <l1_tlv.h>
#include <l1_l2_iface.h>
#include <l1_msg_alloc.h>

#include "fapi_cl.h"
#include "fapi_cl_cphy_sap.h"
#include "fapi_cl_nmm_sap.h"
#include "fapi_cl_phy_iface.h"
#include "fapi_cl_mac_iface.h"

#include "fapi_cl_entry.h"
/*
 * 4GMX Thread Scheduler
 */
//#define MAC_PROCESSING_THREAD_ARM_ID ((MxGetARMID () == 0) ? 3 : 1)

UINT32 FapiClConsoleHandler(CMgrCmdCtx *pCtx, CMgrCmdParamCtx *pParams);

__align(4) const char FapiCLCommand[] = "fapi";


/*
 * Global Variables
 */
MXRC (*LteMacAppInitCallback)(void) = FapiClInitHandler;

FapiClAppCtx *pFapiClAppCtx =  NULL;

/**
 * This function processes queued the NMM FAPI mesages from MAC and PHY
 *
 *  @param   pParam  Pointer to FAPI PHY API Context
 *  @return  MX_OK if success
 */
UINT32 FapiNmmApiThread(LPVOID pParam)
{
    UINT32 status = 0;
// (A) Conver API to MSG
// (B) Send to PHY
// (C) Free N-1 subframe MAC SDU's

    return status;
}



//-------------------------------------------------------------------------------------------
/** @brief This function initializes the variables needed for processing the Phy APIs (from CL)
 *
 *  @param none
 *
 *  @return MX_OK if success
 *
 *  \ingroup group_lte_phy_control_rt_mac_phy
 *
**/
//-------------------------------------------------------------------------------------------
MXRC FapiPhyInitApiThread()
{
    MXRC rc = MX_OK;

    _ASSERT_PTR(pFapiClAppCtx);

    if (HANDLE_NOT_NULL(pFapiClAppCtx->hLteNmmThread))
        return MX_OK;

    /* rc |= MxCreateThreadEx(3 /\*FIXME: CL THREAD*\/, FapiNmmApiThread, */
    /*                        NULL, */
    /*                        pFapiClAppCtx, */
    /*                        0, */
    /*                        &pFapiClAppCtx->hLteNmmThread); */

    if (FAILED(rc))
    {
        MxRemoveThread(pFapiClAppCtx->hLteNmmThread);
        pFapiClAppCtx->hLteNmmThread = HNULL;
    }

    return rc;
}

/** Performs CL instance registration on Supervisor level
 *  Application callback
 *
 * @param p Unused
 * @return 4GMX result code
 */
MXRC FapiClRegMsgThread(void * p)
{
    MXRC rc = SUCCESS;

    rc = SvsrRegMsgHandler(IID_LTE_MAC, FapiPHYMsgHandler, pFapiClAppCtx);
    if (SUCCESSFUL(rc))
        uart_printf("FAPI CL PHY Handler Registered [ID=%d]\r\n", IID_LTE_MAC);
    else
        uart_printf("ERROR: Unable to register FAPI CL hnd [%d]\r\n", rc);


    rc = SvsrRegMsgHandler(IID_LTE_MAC_EX, FapiPHYListMsgHandler, pFapiClAppCtx);
    if (SUCCESSFUL(rc))
        uart_printf("FAPI CL PHY List Handler Registered [ID=%d]\r\n", IID_LTE_MAC_EX);
    else
        uart_printf("ERROR: Unable to register FAPI CL list hnd [%d]\r\n", rc);

    MacCpuID = MxGetCpuID();

    uart_printf("CL Registered on CPU %d\r\n", MacCpuID);
        
    rc |= FapiPhyInitApiThread();

    SetMacInstanceId(IID_LTE_MAC);

    // Initialize CL
    cl_init ();

    // Initialize CPHY-SAP
    lte_cphy_sap_init ();

    // Initialize NMM-SAP FIXME:

    pFapiClAppCtx->RegComplete = 1;
    return rc;
}

/** @brief DOX_INTERNAL This function is called by the function of the L-ARM application init.
           (the function AppInitCpu0 of appinit.c file).

    @return [MXRC] the error code of initialization (0 (or MX_OK) - OK)
*/
MXRC FapiClInitHandler(void)
{
    MXRC rc = MX_OK;


    pFapiClAppCtx = (FapiClAppCtx *) MxHeapAlloc(AppHeap, sizeof(FapiClAppCtx));

    if (pFapiClAppCtx == NULL)
    {
        uart_printf("ERROR: Unable to allocate APP heap\r\n");
        return RC_LTE_ALLOC_ERROR;
    }
    memset(pFapiClAppCtx, 0, sizeof(FapiClAppCtx));

    // Register LTE API handler on the supervisor layer
    rc = SvsrRegClient(IID_LTE_MAC, FapiClApiHandler, pFapiClAppCtx);
    if (SUCCESSFUL(rc))
    {
        uart_printf("FAPI CL application started [ID=%d]\r\n", IID_LTE_MAC);
    }
    else
    {
        uart_printf("ERROR: Unable to register LTE FAPI CL application [%d]\r\n", rc);
    }

    // Register LTE API handler on the supervisor layer
    rc = SvsrRegClient(IID_WIMAX, FapiCPHYApiHandler, pFapiClAppCtx);
    if (SUCCESSFUL(rc))
    {
        uart_printf("CPHY-SAP application started [ID=%d]\r\n", IID_WIMAX);
    }
    else
    {
        uart_printf("ERROR: Unable to register LTE CPHY-SAP application [%d]\r\n", rc);
    }

    // Register LTE console handler
    if (SUCCESSFUL(rc))
        CMgrRegisterHandler(FapiCLCommand, FapiClConsoleHandler);

    return rc;
}


/** Handle 'createmac' command from console input
 *
 * @param pCtx Pointer to console manager context
 * @param pParams Pointer to command parameters list
 * @return 4GMX result code
 */
UINT32 FapiClConsoleHandler(CMgrCmdCtx *pCtx, CMgrCmdParamCtx *pParams)
{
    UINT32 configId = 0, rc = 0;
    UINT32 opt = 0;

    if (pParams->ParamCount < 1)
    {
        return RC_CM_PARAM_ERROR;
    }

    uart_printf("Done [rc = %d], [configId %d] [opt %d]\n", rc, configId, opt);

    return 0;
}
