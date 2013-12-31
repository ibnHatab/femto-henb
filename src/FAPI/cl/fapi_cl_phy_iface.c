#include <stdint.h>

#include "appprintf.h"
#include "supervisor.h"
#include "intercpu.h"
#include "mlog.h"
#include "tcb.h"

#include <LtePhyL2Api.h>

#include <l1_l2_iface.h>
#include <l1_types.h>
#include <l1_tlv.h>

#include "fapi_cl.h"
#include "fapi_cl_phy_iface.h"

MXRC FapiSendMsgToPhy (U8 MessageType, LPVOID pData, UINT32 nMsgSize)
{
    PMAC2PHY_QUEUE_EL pCtrlBlock;
    MXRC rc;
    UINT32 start_tick = MxGetTicks();

    _ASSERT_PTR (pData);

    CLDBG("msg(type=0x%x, size=%d) : cl -> phy", MessageType, nMsgSize);

    pCtrlBlock = (PMAC2PHY_QUEUE_EL) SvsrAllocMsgEx(sizeof(MAC2PHY_QUEUE_EL));

    if (pCtrlBlock == NULL)
    {
        uart_printf("No memory to send I-CORE data, pData=%x, Size=%d\r\n", pData, nMsgSize);
        _ASSERT_PTR(pCtrlBlock);
        return 1;
    }

    pCtrlBlock->MessagePtr = (U8*) pData;
    pCtrlBlock->MessageLen = nMsgSize;
    pCtrlBlock->MessageType = MessageType;
    pCtrlBlock->Next = NULL;

    pCtrlBlock->frameNumber = 0;
    pCtrlBlock->subframeNumber = 0;

    rc = SvsrSendMsg(0, IID_LTE_EX, pCtrlBlock, SVSR_MSG_OPT_DEFAULT);
    _ASSERT_RC(rc);
    if (FAILED(rc))
    {
        uart_printf("phy_send_msg has failed, rc=%x\r\n", rc);
        return rc;
    }

    MLogTask(PID_CL_PHY_MSG_SND, RESOURCE_LARM, start_tick, MxGetTicks());
    
    return MX_OK;
}



/** Process API Msg received from PHY
 *
 * @param pClientCtx Not used
 * @param pMsg Pointer to the message buffer
 * @return Standard result code
 */
MXRC FapiPHYMsgHandler(LPVOID pClientCtx, LPVOID pMsg)
{
    MXRC rc = SUCCESS;
    PGENMSGDESC pMsgHdr = (PGENMSGDESC) pMsg;
    UINT32 start_tick = MxGetTicks();

    _ASSERT_PTR(pClientCtx);

    CLDBG("msg(type=PHY_[0x%x]) : phy -> cl", pMsgHdr->msgType);

    switch (pMsgHdr->msgType)
    {
    case PHY_STOP_IND:
    case PHY_START_CONF:
    case PHY_TXSTART_IND:
    case PHY_INIT_IND:
        rc = fapi_cl_handle_phy_command(pMsgHdr);
        break;

    default:
        uart_printf("Unknown: Fapi PHY msg: 0x%x(%d)\n\r",
                    pMsgHdr->msgType, pMsgHdr->msgType);
    }
    

    MLogTask(PID_CL_PHY_MSG_HND, RESOURCE_LARM, start_tick, MxGetTicks());
    return rc;
}

/** Process Rx Sdu lists received from PHY
 *
 * @param pClientCtx Not used
 * @param pMsg Pointer to the message buffer
 * @return Standard result code
 */
MXRC FapiPHYListMsgHandler(LPVOID pClientCtx, LPVOID pMsg)
{
    PMAC2PHY_QUEUE_EL Curr, Tail;
    MXRC rc = SUCCESS;
    PHY_MSG_Priority_e prio = PHY_MSG_DELAY;
    UINT32 start_tick = MxGetTicks();
        
    _ASSERT_PTR(pClientCtx);

    Curr = pMsg;
    CLDBG("list(type=PHY_RX[0x%x]) : phy -> cl", Curr->MessageType);
    
    while(Curr)
    {
        PGENMSGDESC pRxInd = (PGENMSGDESC) Curr->MessagePtr; // + :16 bits
        switch(Curr->MessageType)
        {
        case PHY_RXSDU_IND: {
            PRXSDUIND pRxSduInd = (PRXSDUIND) pRxInd;
            if (pRxSduInd->pucchDetected) 
                prio = PHY_MSG_DISPATCH;            
        } break; 
        case PHY_RXSTATUS_IND: break; // FIXME: receive status
        case PHY_RXEND_IND: break; // FIXME: proces end
        default:            
            uart_printf("Unknown: Fapi PHY msg in list: 0x%x(%d)\n\r",
                        Curr->MessageType, Curr->MessageType);
        }
        
        rc = fapi_cl_handle_phy_msg(pRxInd, prio);
        
        Tail = Curr->Next;
        SvsrFreeIMem(Curr);
        Curr = Tail;
    }

    MLogTask(PID_CL_PHY_MSG_LIST_HND, RESOURCE_LARM, start_tick, MxGetTicks());
       
    return rc;
}


