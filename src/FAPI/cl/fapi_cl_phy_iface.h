#ifndef _FAPI_CL_PHY_IFACE_H_
#define _FAPI_CL_PHY_IFACE_H_

#include "4gmx.h"

/**
 * Send API message (84xxx-SWG-009-A) to the PHY using extended interface
 *
 * @param MessageType  PHY API message ID (LtePhyL2Api.h)
 * @param pData Pointer to the message buffer
 * @param nMsgSize Message size
 * @return 4GMX return code
 */
MXRC FapiSendMsgToPhy (U8 MessageType, LPVOID pData, UINT32 nMsgSize);

/**
 * MAC message handler registered at Supervisor level for the MAC instance ID
 *
 * @param pClientCtx
 * @param pMsg Pointer to the message to process (MSA-E-81479)
 * @return 4GMX result code
 */
MXRC FapiPHYMsgHandler(LPVOID pClientCtx, LPVOID pMsg);

/**
 * MAC message handler registered at Supervisor level for the MAC instance ID
 *
 * @param pClientCtx
 * @param pMsg Pointer to the message to process (MSA-E-81479)
 * @return 4GMX result code
 */
MXRC FapiPHYListMsgHandler(LPVOID pClientCtx, LPVOID pMsg);


#endif /* _FAPI_CL_PHY_IFACE_H_ */
