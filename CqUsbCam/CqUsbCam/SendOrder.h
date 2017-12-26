#ifndef _SENDORDER_H_
#define _SENDORDER_H_

/****************************************************************************************\
 * This is the header file for the Caiqi usb camera for Linux/Mac						*
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

#include "Types.h"
#include "CyAPI.h"

cq_int32_t SendOrder(CCyUSBDevice *pUsbHandle, PUSB_ORDER pOrder );


#endif //_SENDORDER_H_