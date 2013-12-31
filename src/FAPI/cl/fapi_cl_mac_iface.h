#ifndef _FAPI_CL_MAC_IFACE_H_
#define _FAPI_CL_MAC_IFACE_H_

/**
 * Allocate API message
 *
 * @param buff - raw data buffer
 * @param iID - from whom this message
 * @return allocated API message
 */
ApiParam * FapiAllocMacMsg (ApiHeader *api, UINT32 iID);

/**
 * Send API message to the MAC using extended interface
 *
 * @param pApi - messge to send
 * @param nMsgSize Message size
 * @return 4GMX return code
 */
MXRC FapiSendMsgToMac(ApiHeader * pApi, UINT16 msgType, UINT32 nMsgSize);

/**
 * Send API message LIS to the MAC using extended interface
 *
 * @param pApi - messge to send
 * @return 4GMX return code
 */
MXRC FapiSendListToMac(ApiHeader * pApi);

/** This function will be called by the supervisor in case of receiving
    API command with the LTE instance ID

    @param pClientCtx [in] - pointer to the LTE context (LTEAppCtx *, please see LTEInit)
    @param pApi       [in] - pointer to the received API command
    @param pResp      [out]- pointer to the response for the API command

    @return [APIHEC] the control code of response control
*/
APIHEC FapiClApiHandler(LPVOID pClientCtx, ApiHeader *pApi, ApiHeader *pResp);

/**  This function will be called by the supervisor in case of receiving
     CPHY SAP API command with the WIMAX (reuse) instance ID

    @param pClientCtx [in] - pointer to the LTE context (LTEAppCtx *, please see LTEInit)
    @param pApi       [in] - pointer to the received API command
    @param pResp      [out]- pointer to the response for the API command

    @return [APIHEC] the control code of response control
*/
APIHEC FapiCPHYApiHandler(LPVOID pClientCtx, ApiHeader *pApi, ApiHeader *pResp);



#endif /* _FAPI_CL_MAC_IFACE_H_ */
