/****************************************************************************************\
 * This is the file for the Caiqi usb camera for Linux/Mac						*
 *                                                                                	*
 * Author              :        nxb     											* 												
 * License             :        GPL Ver 2.0                                       	*
 * Copyright           :        Caiqi Electronics Inc.								*
 * Date written        :        nov 05, 2017                                    	*
 * Modification Notes  :                                                          	*
 *    1. nxb, nov 05, 2017                                  						*
 *       Add documentation.                                          				*
 *                      															*
 *                                                                                	*
 \***************************************************************************************/
#include "../stdafx.h"

#include <string.h>

#include "MT9M001.h"
#include "../SensorCapbablity.h"

#include "../CyAPI.h"

//#include "StdAfx.h"
static cq_int32_t SendOrder(CCyUSBDevice *pUsbHandle, PUSB_ORDER pOrder )
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

static cq_int32_t MT9M001_WrSensorReg(CCyUSBDevice *pUsbHandle, const cq_uint32_t iAddr, const cq_uint32_t iValue)
{
	USB_ORDER     m_sUsbOrder;
	cq_uint8_t  m_byData[64];

	m_sUsbOrder.pData=m_byData;
	m_sUsbOrder.ReqCode = 0xF1;
	m_sUsbOrder.DataBytes = 2;
	m_sUsbOrder.Direction = 0;
	m_sUsbOrder.Index = iAddr&0xff;
	m_sUsbOrder.Value = iValue&0xffff;

	return SendOrder(pUsbHandle, &m_sUsbOrder);

    //return cyusb_control_write(pUsbHandle, 0x40, 0xf1, tempValue, tempAddr, data, 2, 100);
}


static cq_int32_t MT9M001_RdSensorReg(CCyUSBDevice *pUsbHandle, const cq_uint32_t iAddr, cq_uint32_t &iValue)
{
	cq_uint8_t tempAddr= iAddr&0xff;
	cq_uint8_t tempValue[2]={'0'};
	cq_int32_t r=0;
	//cq_int32_t r=cyusb_control_read(pUsbHandle, 0x40, 0xf2,0x0, tempAddr, tempValue, 2, 100);
	iValue = tempValue[0] << 8;
	iValue += tempValue[1];
	return r;
}

static cq_int32_t MT9M001_WrEeprom(CCyUSBDevice *pUsbHandle, const cq_uint32_t iAddr, const cq_uint8_t iValue)
{
	cq_int32_t len=2;
	cq_uint8_t data[10]={'0'};//no use, just to make firmware happy
	cq_uint16_t tempAddr= iAddr&0xffff;
	cq_uint16_t tempValue= iValue&0xff;
	//cyusb_control_write(pUsbHandle,0x40,0xf5,tempValue,tempAddr,data,len,100);
	return len;
}
static cq_int32_t MT9M001_RdEeprom(CCyUSBDevice *pUsbHandle, const cq_uint32_t iAddr, cq_uint8_t * buffer, cq_int32_t &length)
{
	cq_int32_t transferred=length;
	cq_uint16_t tempAddr= iAddr&0xffff;
	cq_int32_t r=0;
    //cq_int32_t r=cyusb_control_read(pUsbHandle,0x40,0xf6,0x0, tempAddr, buffer, transferred,100);
	return r;
	
}
static cq_int32_t MT9M001_WrDevID(CCyUSBDevice *pUsbHandle, cq_uint8_t* chIdBuf, cq_uint32_t &length )
{
	int len=1;
	cq_int32_t r=0;
    //int r= cyusb_control_write(pUsbHandle,0x40,0xd0,0x0,0x0, chIdBuf,len,100);
	length=len;
	return r;
}
static cq_int32_t MT9M001_RdDevID(CCyUSBDevice *pUsbHandle, cq_uint8_t *chIdBuf, cq_uint32_t &length)
{
	int len=1;
	unsigned char buf[10]={'0'};
	cq_int32_t r=0;
    //int r= cyusb_control_read(pUsbHandle,0x40,0xd1,0x0,0x0, buf, len,100);
	memcpy(chIdBuf, buf, len);
	length=len;
	return r;
}

static cq_int32_t MT9M001_WrDevSN(CCyUSBDevice *pUsbHandle, cq_uint8_t* chSnBuf, cq_uint32_t &length )
{
	int len=4;
	cq_int32_t r=0;
    //int r= cyusb_control_write(pUsbHandle,0x40,0xd2,0x0,0x0, chSnBuf,len,100);
	length=len;
	return r;
}
static cq_int32_t MT9M001_RdDevSN(CCyUSBDevice *pUsbHandle, cq_uint8_t *chSnBuf, cq_uint32_t &length)
{
	int len=4;
	unsigned char buf[10]={'0'};
	cq_int32_t r=0;
    //int r= cyusb_control_read(pUsbHandle,0x40,0xd3,0x0,0x0, buf, len,100);
	memcpy(chSnBuf, buf, len);
	length=len;
	return r;
}
static cq_int32_t MT9M001_WrFpgaReg(CCyUSBDevice *pUsbHandle, const cq_uint32_t iAddr, const cq_uint32_t iValue)
{
    cq_uint8_t data[10]={'0'};//no use, just to make firmware happy
	cq_uint8_t tempAddr= iAddr&0xff;
	cq_uint8_t tempValue= iValue&0xff;
	cq_int32_t r=0;
    //return cyusb_control_write(pUsbHandle, 0x40, 0xf3, tempValue, tempAddr, data, 1, 100);
	return r;
}


static cq_int32_t MT9M001_RdFpgaReg(CCyUSBDevice *pUsbHandle, const cq_uint32_t iAddr, cq_uint32_t &iValue)
{
	cq_uint8_t tempAddr= iAddr&0xff;
	cq_uint8_t tempValue[1]={'0'};
	cq_int32_t r=0;
    //cq_int32_t r=cyusb_control_read(pUsbHandle, 0x40, 0xf4, 0x0, tempAddr, tempValue, 1, 100);
	iValue=tempValue[0];
	return r;
}
/*
static cq_int32_t MT9M001_InitSensor(CCyUSBDevice *pUsbHandle)
{    
 
    
    return 0;
}

*/
/*
static cq_int32_t MT9M001_SetAnalogGain(CCyUSBDevice *pUsbHandle, const cq_uint32_t chTrigType, const cq_uint32_t chGainType)
{
	cq_int32_t r=0;
	switch(chTrigType) 
	{   
		case TRIGMODE_AUTO:
		{
			switch(chGainType)
			{
				case ANALOGGAIN_1X:
					r=MT9M001_WrSensorReg(pUsbHandle, 0x30B0,0x0080);
					break;
				case ANALOGGAIN_2X:
					r=MT9M001_WrSensorReg(pUsbHandle, 0x30B0,0x0090);
					break;
				case ANALOGGAIN_4X:
					r=MT9M001_WrSensorReg(pUsbHandle, 0x30B0,0x00A0);
					break;
				case ANALOGGAIN_8X:
					r=MT9M001_WrSensorReg(pUsbHandle, 0x30B0,0x00B0);
					break;
				default:
					r=-99;
					break;
			}
			break;	
		}
		case TRIGMODE_FPGA:
		{
			switch(chGainType)
			{
				case ANALOGGAIN_1X:
					r=MT9M001_WrSensorReg(pUsbHandle, 0x30B0,0x0480);
					break;
				case ANALOGGAIN_2X:
					r=MT9M001_WrSensorReg(pUsbHandle, 0x30B0,0x0490);
					break;
				case ANALOGGAIN_4X:
					r=MT9M001_WrSensorReg(pUsbHandle, 0x30B0,0x04A0);
					break;
				case ANALOGGAIN_8X:
					r=MT9M001_WrSensorReg(pUsbHandle, 0x30B0,0x04B0);
					break;
				default:
					r=-99;
					break;	
			}
			break;
		}	
		default:
			r=-99;
			break;	
	}
	return r;
}
*/

static cq_int32_t MT9M001_SetFpgaTrigFreq(CCyUSBDevice *pUsbHandle, const cq_uint32_t iFreqVal)
{
     cq_int32_t r=MT9M001_WrFpgaReg(pUsbHandle, 0x05, iFreqVal&0xff);
     return r;
}

static cq_int32_t MT9M001_SetTrigMode(CCyUSBDevice *pUsbHandle, const cq_uint32_t chTrigType)
{
	switch(chTrigType)
	{
		case TRIGMODE_AUTO:
			MT9M001_WrSensorReg(pUsbHandle, 0x07, 0x0388);
			MT9M001_WrSensorReg(pUsbHandle, 0x20, 0x01C1);
			MT9M001_WrFpgaReg(pUsbHandle, 0x00, 0x00);
			break;
		case TRIGMODE_FPGA:
			MT9M001_WrSensorReg(pUsbHandle, 0x07, 0x0398);
			MT9M001_WrSensorReg(pUsbHandle, 0x20, 0x03D5);
			MT9M001_WrFpgaReg(pUsbHandle, 0x00, 0x01);
			MT9M001_WrFpgaReg(pUsbHandle, 0x05, 0x01);// 0x01 by default
			break;
		case TRIGMODE_SOFT:
			MT9M001_WrSensorReg(pUsbHandle, 0x07, 0x0398);
			MT9M001_WrSensorReg(pUsbHandle, 0x20, 0x03D5);
			MT9M001_WrFpgaReg(pUsbHandle, 0x00, 0x02);
			break;
		case TRIGMODE_SIGNAL:
			MT9M001_WrSensorReg(pUsbHandle, 0x07, 0x0398);
			MT9M001_WrSensorReg(pUsbHandle, 0x20, 0x03D5);
			MT9M001_WrFpgaReg(pUsbHandle, 0x00, 0x03);
		default:
			break;
	}
	return 0;

}
static cq_int32_t MT9M001_SoftTrig(CCyUSBDevice* pUsbHandle)
{	 
	  MT9M001_WrFpgaReg(pUsbHandle, 0x0a, 0x00);
	  MT9M001_WrFpgaReg(pUsbHandle, 0x0a, 0x01);
	  return 0;

}

static cq_int32_t MT9M001_SetExpoValue(CCyUSBDevice *pUsbHandle, const cq_uint32_t iExpoVal)
{
	cq_int32_t r=MT9M001_WrSensorReg(pUsbHandle, 0x0b, iExpoVal&0xffff);
	return r;

}

static cq_int32_t MT9M001_SetGainValue(CCyUSBDevice *pUsbHandle, const cq_uint32_t iGainVal)
{
    cq_int32_t r=MT9M001_WrSensorReg(pUsbHandle, 0x35, iGainVal&0xffff);
    return r;
}
#if 0 //no such function
static cq_int32_t MT9M001_SetAutoGainExpo(CCyUSBDevice *pUsbHandle, const cq_bool_t bIsAutoGain, const cq_bool_t bIsAutoExpo)
{
    if((bIsAutoGain==true)&&(bIsAutoExpo==true))
       return MT9M001_WrSensorReg(pUsbHandle, 0xAF, 0x0003);

    if ((bIsAutoGain==true)&&(bIsAutoExpo==false))
       return MT9M001_WrSensorReg(pUsbHandle, 0xAF, 0x0002);

    if ((bIsAutoGain==false)&&(bIsAutoExpo==true))
       return MT9M001_WrSensorReg(pUsbHandle, 0xAF, 0x0001);

    if ((bIsAutoGain==false)&&(bIsAutoExpo==false))
       return MT9M001_WrSensorReg(pUsbHandle, 0xAF, 0x0000);
	return -99;//should never reach here
}

#endif

static cq_int32_t MT9M001_SetResolution(CCyUSBDevice *pUsbHandle, const cq_uint32_t chResoluType)
{
	switch(chResoluType)
	{
		case RESOLU_752_480:
			MT9M001_WrSensorReg(pUsbHandle,0x01, 0x0001);
			MT9M001_WrSensorReg(pUsbHandle,0x02, 0x0004);
			MT9M001_WrSensorReg(pUsbHandle,0x03, 0x01E0);
			MT9M001_WrSensorReg(pUsbHandle,0x04, 0x02F0);

			MT9M001_WrFpgaReg(pUsbHandle,0x01, 0x02);
			MT9M001_WrFpgaReg(pUsbHandle,0x02, 0xF0);
			MT9M001_WrFpgaReg(pUsbHandle,0x03, 0x01);
			MT9M001_WrFpgaReg(pUsbHandle,0x04, 0xE0);   
			break;
		case RESOLU_640_480:	
			MT9M001_WrSensorReg(pUsbHandle,0x01, 0x0039);
			MT9M001_WrSensorReg(pUsbHandle,0x02, 0x0004);
			MT9M001_WrSensorReg(pUsbHandle,0x03, 0x01E0);
			MT9M001_WrSensorReg(pUsbHandle,0x04, 0x0280);

			MT9M001_WrFpgaReg(pUsbHandle,0x01, 0x02);
			MT9M001_WrFpgaReg(pUsbHandle,0x02, 0x80);
			MT9M001_WrFpgaReg(pUsbHandle,0x03, 0x01);
			MT9M001_WrFpgaReg(pUsbHandle,0x04, 0xE0);  					
			break;
		default:
			break;
	}
	return 0;
		

}


static cq_int32_t MT9M001_SetMirrorType(CCyUSBDevice *pUsbHandle, const cq_uint32_t chMirrorType)
{
	if(MIRROR_NORMAL==chMirrorType)	//normal
		return MT9M001_WrSensorReg(pUsbHandle, 0x0D, 0x0300);

	if(MIRROR_X==chMirrorType)//X
		return MT9M001_WrSensorReg(pUsbHandle, 0x0D, 0x0320);

	if(MIRROR_Y==chMirrorType)//Y
		return MT9M001_WrSensorReg(pUsbHandle, 0x0D, 0x0310);

	if(MIRROR_XY==chMirrorType)//XY
		return MT9M001_WrSensorReg(pUsbHandle, 0x0D, 0x0330);

	return -99;//should never reach here

}

/*
static cq_int32_t MT9M001_SetBitDepth(CCyUSBDevice *pUsbHandle, const cq_uint32_t chBitDepthType)
{
	if(BITDEPTH_8==chBitDepthType)	//8
		return MT9M001_WrFpgaReg(pUsbHandle, 0x7, 0x0);

	if(BITDEPTH_16==chBitDepthType)//16
		return MT9M001_WrFpgaReg(pUsbHandle, 0x7, 0x1);

	if(BITDEPTH_16_2==chBitDepthType)//16
		return MT9M001_WrFpgaReg(pUsbHandle, 0x7, 0x2);

	return -99;//should never reach here

}

*/
static cq_int32_t MT9M001_StartCap(CCyUSBDevice *pUsbHandle)
{
	MT9M001_WrFpgaReg(pUsbHandle, 0x09, 0x00);
	MT9M001_WrFpgaReg(pUsbHandle, 0x09, 0x01);
	return 0;	
}

static cq_int32_t MT9M001_StopCap(CCyUSBDevice *pUsbHandle)
{
	MT9M001_WrFpgaReg(pUsbHandle, 0x09, 0x00);
	return 0;
}

static cq_int32_t MT9M001_SendUsbSpeed2Fpga(CCyUSBDevice *pUsbHandle, const cq_uint32_t chSpeedType)
{
	MT9M001_WrFpgaReg(pUsbHandle, 0x08, chSpeedType);
	return 0;
}


/*
static tagSensor sensor_MT9M001=
{
	.name="MT9M001",
	.iicAddr=0xf0,
	.regAddrLen=1,
	.regValueLen=2,

	.InitSensor		=	NULL,
	.WrSensorReg	=	MT9M001_WrSensorReg,
	.RdSensorReg	=	MT9M001_RdSensorReg,
	.WrFpgaReg		=	MT9M001_WrFpgaReg,
	.RdFpgaReg		=	MT9M001_RdFpgaReg,
	.SetAnalogGain	=	NULL,
	.SetFpgaTrigFreq=	MT9M001_SetFpgaTrigFreq,
	.SetTrigMode	=	MT9M001_SetTrigMode,
	.SetExpoValue	=	MT9M001_SetExpoValue,
	.SetGainValue	=	MT9M001_SetGainValue,
	.SetAutoGainExpo=	MT9M001_SetAutoGainExpo,
	.SetResolution	=	MT9M001_SetResolution,
	.SetMirrorType	=	MT9M001_SetMirrorType,
	.SetBitDepth	=	NULL,
	.StartCap 		=	MT9M001_StartCap,
	.StopCap 		=	MT9M001_StopCap,
	.SendUsbSpeed2Fpga=	MT9M001_SendUsbSpeed2Fpga,

	.WrEeprom		=	MT9M001_WrEeprom,
	.RdEeprom		=	MT9M001_RdEeprom,
	.WrDevID		=	MT9M001_WrDevID,
	.RdDevID		=	MT9M001_RdDevID,
	.WrDevSN		=	MT9M001_WrDevSN,
	.RdDevSN		=	MT9M001_RdDevSN,
	.SoftTrig		=	MT9M001_SoftTrig
};
*/
static tagSensor sensor_MT9M001=
{
	"MT9MV034",
	0xf0,
	1,
	2,

	NULL,
	MT9M001_WrSensorReg,
	MT9M001_RdSensorReg,
	MT9M001_WrFpgaReg,
	MT9M001_RdFpgaReg,
	NULL,
	MT9M001_SetFpgaTrigFreq,
	MT9M001_SetTrigMode,
	MT9M001_SetExpoValue,
	MT9M001_SetGainValue,
	NULL,
	MT9M001_SetResolution,
	MT9M001_SetMirrorType,
	NULL,
	MT9M001_StartCap,
	MT9M001_StopCap,
	MT9M001_SendUsbSpeed2Fpga,

	MT9M001_WrEeprom,
	MT9M001_RdEeprom,
	MT9M001_WrDevID,
	MT9M001_RdDevID,
	MT9M001_WrDevSN,
	MT9M001_RdDevSN,
	MT9M001_SoftTrig
};
void RegisterSensor_MT9V034(list<tagSensor>& sensorList)
{
	sensorList.push_back(sensor_MT9M001);

}

