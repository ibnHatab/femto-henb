#include <stdint.h>


#include "resultcodes.h"
#include "basetypes.h"
#include "typedef.h"
#include "appprintf.h"
#include "supervisor.h"
#include "intercpu.h"
#include "mlog.h"
#include "tcb.h"

#include "lte_entry.h"

#include <l1_l2_iface.h>

#include "fapi_cl.h"
#include "fapi_cl_entry.h"
#include "fapi_cl_mac_iface.h"
#include "fapi_cl_cphy_sap.h"

extern int fapi_cl_parse_fapi_tx_data (LPVOID pClientCtx, ApiHeader *pApi);

extern FapiClAppCtx *pFapiClAppCtx;

ApiParam * FapiAllocMacMsg (ApiHeader *api, UINT32 iID)
{
    UINT32 dstIID = 0;

    switch (iID) {
    case IID_LTE_MAC: // CL PHY-SAP
        dstIID = pFapiClAppCtx->nMacUserInstanceId; break;
    case IID_WIMAX: // CPHY-SAP
        dstIID = pFapiClAppCtx->nPhyUserInstanceId; break;
    default:
        uart_printf("ERROR: CL MacIface unknown source [%x]\n", iID);
        _ASSERT_PTR(0);
    }

    api = ApiCreate(api, API_ENCAP_FM, iID, dstIID, API_TYPE_IND, PHY_LTE_MSG);

    return (ApiParam *) &api[1];
}

MXRC FapiSendMsgToMac(ApiHeader * pApi, UINT16 paramID, UINT32 nMsgSize)
{
    ApiParam *pParam = (ApiParam *) &pApi[1];
    MXRC rc = SUCCESS;
    UINT32 start_tick = MxGetTicks();
    
    _ASSERT_PTR(pApi);

    pParam->ParamID =  paramID;
    pParam->ParamLength = nMsgSize;

    pApi->Length += GetParamTotalSize(pParam);

    CLDBG("msg(id=0x%x, param=0x%x) : cl -> mac", pApi->MessageID, paramID);
    rc = SvsrSendAPI/*noAlloc*/ (pApi); //??

    MLogTask(PID_CL_MAC_MSG_SND, RESOURCE_LARM, start_tick, MxGetTicks());
    return rc;
}

MXRC FapiSendListToMac(ApiHeader * pApi)
{
    MXRC rc = SUCCESS;
    UINT32 start_tick = MxGetTicks();
    
    _ASSERT_PTR(pApi);

    CLDBG("list 0x%x of %d : CL -> MAC", pApi->MessageID, pApi->Length);

    rc = SvsrSendAPI (pApi);
    MLogTask(PID_CL_MAC_MSG_SND, RESOURCE_LARM, start_tick, MxGetTicks());
    return rc;
}


APIHEC FapiClApiHandler(LPVOID pClientCtx, ApiHeader *pApi, ApiHeader *pResp)
{
    int rc = SUCCESS;
    FapiClAppCtx *pCtx = (FapiClAppCtx *) pClientCtx;
    UINT32 start_tick = MxGetTicks();
    
    pCtx->nMacUserInstanceId = pApi->SrcInstID;

    if (pCtx == NULL)
    {
        rc = RC_PHY_CONTEXT_ALLOC_ERROR;
        ApiAddErrorCode(pResp, rc);
        return APIHEC_OK;
    }

    CLDBG("cl handle MAC msg(id=0d, type=%x, len=%d)", pApi->MessageID, pApi->Type, pApi->Length);

    if (pApi->Type == API_TYPE_DATA)
    {
        switch (pApi->MessageID)
        {
        case MESSAGE_ID(PHY_MSG_OFFS, LTE_MODULE, 7) /* PHY_LTE_MSG_EX */: {
            rc = fapi_cl_parse_fapi_tx_data (pCtx, pApi);
        } break;
        default:
            rc = RC_LTE_UNKNOWN_COMMAND;
        }
    } else {
        rc = RC_LTE_UNKNOWN_COMMAND;
    }

    // Signal core-3 to read message queue
    //FIXME: MxScheduleThread (pCtx->hLteNmmThread);

    MLogTask(PID_CL_LTE_API_REQ, RESOURCE_LARM, start_tick, MxGetTicks());
    return APIHEC_DROP_RESP;
}

APIHEC FapiCPHYApiHandler(LPVOID pClientCtx, ApiHeader *pApi, ApiHeader *pResp)
{
    int rc = SUCCESS;
    APIHEC apiHec = APIHEC_OK;
    UINT32 start_tick = MxGetTicks();

    FapiClAppCtx *pCtx = (FapiClAppCtx *) pClientCtx;
    pCtx->nPhyUserInstanceId = pApi->SrcInstID;

    CLDBG("rcv ICPU msg 0x%x", pApi->MessageID);

    if (pCtx == NULL)
    {
        rc = RC_PHY_CONTEXT_ALLOC_ERROR;
        ApiAddErrorCode (pResp, rc);
        return APIHEC_OK;
    }

    if (pApi->Type == API_TYPE_CFG_PARAM &&
        pApi->MessageID == MESSAGE_ID(PHY_MSG_OFFS, LTE_MODULE, 7) /* PHY_LTE_MSG_EX */)
    {
        ApiParam *requestParam = ApiGetNextParam (pApi, NULL);

        switch (requestParam->ParamID)
        {
            /**
               Routing Configuration Procedures on CPHY SAP:
               - Initialization
               - Termination
               - Restart
               - Reset
               - Error notification
            */
        case FAPI_L1_PARAM_REQ:
        case FAPI_L1_CONFIG_REQ:
        case FAPI_L1_START_REQ:
        case FAPI_L1_STOP_REQ: {
            ApiParam *rspParam = (ApiParam *) &pResp[1];
            fapi_l1_msg_hdr_t
                *request  = (fapi_l1_msg_hdr_t *) requestParam->ParamData,
                *response  = (fapi_l1_msg_hdr_t *) rspParam->ParamData;

            response->msgType = FAPI_L1_MSG_INVALID;
            response->length = 0;

            CLDBG("handle FAPI_L1_[%d]", request->msgType);

            if (lte_cphy_request (request, response) < 0)
                rc = RC_LTE_ALLOC_ERROR;

            if (response->msgType == FAPI_L1_MSG_INVALID) {
                // Drop parm
                rspParam->ParamLength = 0;
                rspParam->ParamID = PARAM_ID_INVALID;               
                pResp->Length -= GetParamTotalSize(rspParam);
            } else {
                // Keep param
                CLDBG("reply(type=%d, rc=%d, cpu=%d) : cl -> mac", 
                      response->msgType, rc, MxGetCpuID());

                rspParam->ParamID =  response->msgType;
                rspParam->ParamLength = response->length;
                
                pResp->Length += GetParamTotalSize(rspParam);
            }

            // Use asynchronous protocol between MAC and CPHY-SAP
            if (rc == MX_OK && response->msgType == FAPI_L1_MSG_INVALID)
                apiHec = APIHEC_DROP_RESP;

        } break;
        default:
            rc = RC_PHY_UNKNOWN_PARAM;
        }
    } else {
        rc = RC_LTE_UNKNOWN_COMMAND;
    }


    if (! rc == MX_OK) 
        ApiAddErrorCode(pResp, rc);
    
    MLogTask(PID_CL_CPHY_SAP_API_REQ, RESOURCE_LARM, start_tick, MxGetTicks());
    
    return apiHec;
}
