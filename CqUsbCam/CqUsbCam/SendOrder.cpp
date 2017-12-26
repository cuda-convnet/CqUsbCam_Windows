/****************************************************************************************\
 * This is the cpp file for the Caiqi usb camera for Linux/Mac						*
 *                                                                                	*
 * Author              :        nxb     											* 												
 * License             :        GPL Ver 2.0                                       	*
 * Copyright           :        Caiqi Electronics Inc.								*
 * Date written        :        Dec 26, 2017                                    	*
 * Modification Notes  :                                                          	*
 *    1. nxb, Dec 26, 2017                                  						*
 *       Add documentation.                                          				*
 *                      															*
 *                                                                                	*
 \***************************************************************************************/
#include "stdafx.h"
#include "SendOrder.h"
cq_int32_t SendOrder(CCyUSBDevice *pUsbHandle, PUSB_ORDER pOrder )
{
	if(pUsbHandle!=NULL&&pUsbHandle->IsOpen())
	{
		pUsbHandle->ControlEndPt->Target=(CTL_XFER_TGT_TYPE)pOrder->Target;
		pUsbHandle->ControlEndPt->ReqType=(CTL_XFER_REQ_TYPE)pOrder->ReqType;
		pUsbHandle->ControlEndPt->Direction=(CTL_XFER_DIR_TYPE)pOrder->Direction;
		pUsbHandle->ControlEndPt->ReqCode=pOrder->ReqCode;
		pUsbHandle->ControlEndPt->Value=pOrder->Value;
		pUsbHandle->ControlEndPt->Index=pOrder->Index;

		cq_int64_t lBytes=0;
		lBytes=pOrder->DataBytes;
		if(pUsbHandle->ControlEndPt->XferData((cq_uint8_t*)(pOrder->pData),lBytes))
		{
			pOrder->DataBytes=lBytes;
			return 0;
		}
		pOrder->DataBytes=0;
	}
	return -1;
}