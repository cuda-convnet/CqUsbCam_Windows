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
#include "../SendOrder.h"
//#include "StdAfx.h"



static cq_int32_t MT9V034_WrSensorReg(CCyUSBDevice *pUsbHandle, const cq_uint32_t iAddr, const cq_uint32_t iValue)
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
}


static cq_int32_t MT9V034_RdSensorReg(CCyUSBDevice *pUsbHandle, const cq_uint32_t iAddr, cq_uint32_t &iValue)
{
	USB_ORDER     sUsbOrder;
	cq_uint8_t  chData[64];

	chData[0] = '0';
	chData[1] = '0';

	sUsbOrder.pData=chData;
	sUsbOrder.ReqCode = 0xF2;
	sUsbOrder.DataBytes = 2;
	sUsbOrder.Direction = ORDER_IN;
	sUsbOrder.Index = iAddr&0xff;

	cq_int32_t r=SendOrder(pUsbHandle, &sUsbOrder);

	cq_uint8_t rxval[2];
	memcpy(rxval, chData, 2);
	cq_uint16_t irxval = rxval[0] << 8;
	irxval += rxval[1];

	iValue=irxval;
	return r;
}

#if 0
static cq_int32_t MT9V034_WrEeprom(CCyUSBDevice *pUsbHandle, const cq_uint32_t iAddr, const cq_uint8_t iValue)
{

}
static cq_int32_t MT9V034_RdEeprom(CCyUSBDevice *pUsbHandle, const cq_uint32_t iAddr, cq_uint8_t * buffer, cq_int32_t &length)
{

}
static cq_int32_t MT9V034_WrDevID(CCyUSBDevice *pUsbHandle, cq_uint8_t* chIdBuf, cq_uint32_t &length )
{

}
static cq_int32_t MT9V034_RdDevID(CCyUSBDevice *pUsbHandle, cq_uint8_t *chIdBuf, cq_uint32_t &length)
{

}

static cq_int32_t MT9V034_WrDevSN(CCyUSBDevice *pUsbHandle, cq_uint8_t* chSnBuf, cq_uint32_t &length )
{

}
static cq_int32_t MT9V034_RdDevSN(CCyUSBDevice *pUsbHandle, cq_uint8_t *chSnBuf, cq_uint32_t &length)
{

}
#endif

static cq_int32_t MT9V034_WrFpgaReg(CCyUSBDevice *pUsbHandle, const cq_uint32_t iAddr, const cq_uint32_t iValue)
{
	USB_ORDER     sUsbOrder;
	cq_uint8_t  chData[64];

	sUsbOrder.pData=chData;
	sUsbOrder.ReqCode = 0xF3; 
	sUsbOrder.DataBytes = 1;
	sUsbOrder.Direction = ORDER_OUT;
	sUsbOrder.Index = iAddr&0xff;
	sUsbOrder.Value = iValue&0xff;

	return SendOrder(pUsbHandle, &sUsbOrder);
}


static cq_int32_t MT9V034_RdFpgaReg(CCyUSBDevice *pUsbHandle, const cq_uint32_t iAddr, cq_uint32_t &iValue)
{
	USB_ORDER     sUsbOrder;
	cq_uint8_t  chData[64];

	chData[0] = '0';

	sUsbOrder.pData=chData;
	sUsbOrder.ReqCode = 0xF4;
	sUsbOrder.DataBytes = 1;
	sUsbOrder.Direction = ORDER_IN;
	sUsbOrder.Index = iAddr&0xff;

	SendOrder(pUsbHandle, &sUsbOrder);

	cq_uint8_t rxval[1];
	memcpy(rxval, chData, 1);
	cq_uint8_t irxval = rxval[0];
	iValue=irxval;
	return irxval;
}
/*
static cq_int32_t MT9V034_InitSensor(CCyUSBDevice *pUsbHandle)
{    
 
    
    return 0;
}

*/
/*
static cq_int32_t MT9V034_SetAnalogGain(CCyUSBDevice *pUsbHandle, const cq_uint32_t chTrigType, const cq_uint32_t chGainType)
{
	cq_int32_t r=0;
	switch(chTrigType) 
	{   
		case TRIGMODE_AUTO:
		{
			switch(chGainType)
			{
				case ANALOGGAIN_1X:
					r=MT9V034_WrSensorReg(pUsbHandle, 0x30B0,0x0080);
					break;
				case ANALOGGAIN_2X:
					r=MT9V034_WrSensorReg(pUsbHandle, 0x30B0,0x0090);
					break;
				case ANALOGGAIN_4X:
					r=MT9V034_WrSensorReg(pUsbHandle, 0x30B0,0x00A0);
					break;
				case ANALOGGAIN_8X:
					r=MT9V034_WrSensorReg(pUsbHandle, 0x30B0,0x00B0);
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
					r=MT9V034_WrSensorReg(pUsbHandle, 0x30B0,0x0480);
					break;
				case ANALOGGAIN_2X:
					r=MT9V034_WrSensorReg(pUsbHandle, 0x30B0,0x0490);
					break;
				case ANALOGGAIN_4X:
					r=MT9V034_WrSensorReg(pUsbHandle, 0x30B0,0x04A0);
					break;
				case ANALOGGAIN_8X:
					r=MT9V034_WrSensorReg(pUsbHandle, 0x30B0,0x04B0);
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

static cq_int32_t MT9V034_SetFpgaTrigFreq(CCyUSBDevice *pUsbHandle, const cq_uint32_t iFreqVal)
{
     cq_int32_t r=MT9V034_WrFpgaReg(pUsbHandle, 0x05, iFreqVal&0xff);
     return r;
}

static cq_int32_t MT9V034_SetTrigMode(CCyUSBDevice *pUsbHandle, const cq_uint32_t chTrigType)
{
	switch(chTrigType)
	{
		case TRIGMODE_AUTO:
			MT9V034_WrSensorReg(pUsbHandle, 0x07, 0x0388);
			MT9V034_WrSensorReg(pUsbHandle, 0x20, 0x01C1);
			MT9V034_WrFpgaReg(pUsbHandle, 0x00, 0x00);
			break;
		case TRIGMODE_FPGA:
			MT9V034_WrSensorReg(pUsbHandle, 0x07, 0x0398);
			MT9V034_WrSensorReg(pUsbHandle, 0x20, 0x03D5);
			MT9V034_WrFpgaReg(pUsbHandle, 0x00, 0x01);
			MT9V034_WrFpgaReg(pUsbHandle, 0x05, 0x01);// 0x01 by default
			break;
		case TRIGMODE_SOFT:
			MT9V034_WrSensorReg(pUsbHandle, 0x07, 0x0398);
			MT9V034_WrSensorReg(pUsbHandle, 0x20, 0x03D5);
			MT9V034_WrFpgaReg(pUsbHandle, 0x00, 0x02);
			break;
		case TRIGMODE_SIGNAL:
			MT9V034_WrSensorReg(pUsbHandle, 0x07, 0x0398);
			MT9V034_WrSensorReg(pUsbHandle, 0x20, 0x03D5);
			MT9V034_WrFpgaReg(pUsbHandle, 0x00, 0x03);
		default:
			break;
	}
	return 0;

}
static cq_int32_t MT9V034_SoftTrig(CCyUSBDevice* pUsbHandle)
{	 
	  MT9V034_WrFpgaReg(pUsbHandle, 0x0a, 0x00);
	  MT9V034_WrFpgaReg(pUsbHandle, 0x0a, 0x01);
	  return 0;

}

static cq_int32_t MT9V034_SetExpoValue(CCyUSBDevice *pUsbHandle, const cq_uint32_t iExpoVal)
{
	cq_int32_t r=MT9V034_WrSensorReg(pUsbHandle, 0x0b, iExpoVal&0xffff);
	return r;
}

static cq_int32_t MT9V034_SetGainValue(CCyUSBDevice *pUsbHandle, const cq_uint32_t iGainVal)
{
    cq_int32_t r=MT9V034_WrSensorReg(pUsbHandle, 0x35, iGainVal&0xffff);
    return r;
}

static cq_int32_t MT9V034_SetAutoGainExpo(CCyUSBDevice *pUsbHandle, const cq_bool_t bIsAutoGain, const cq_bool_t bIsAutoExpo)
{
    if((bIsAutoGain==true)&&(bIsAutoExpo==true))
       return MT9V034_WrSensorReg(pUsbHandle, 0xAF, 0x0003);

    if ((bIsAutoGain==true)&&(bIsAutoExpo==false))
       return MT9V034_WrSensorReg(pUsbHandle, 0xAF, 0x0002);

    if ((bIsAutoGain==false)&&(bIsAutoExpo==true))
       return MT9V034_WrSensorReg(pUsbHandle, 0xAF, 0x0001);

    if ((bIsAutoGain==false)&&(bIsAutoExpo==false))
       return MT9V034_WrSensorReg(pUsbHandle, 0xAF, 0x0000);
	return -99;//should never reach here
}

static cq_int32_t MT9V034_SetResolution(CCyUSBDevice *pUsbHandle, const cq_uint32_t chResoluType)
{
	switch(chResoluType)
	{
		case RESOLU_752_480:
			MT9V034_WrSensorReg(pUsbHandle,0x01, 0x0001);
			MT9V034_WrSensorReg(pUsbHandle,0x02, 0x0004);
			MT9V034_WrSensorReg(pUsbHandle,0x03, 0x01E0);
			MT9V034_WrSensorReg(pUsbHandle,0x04, 0x02F0);

			MT9V034_WrFpgaReg(pUsbHandle,0x01, 0x02);
			MT9V034_WrFpgaReg(pUsbHandle,0x02, 0xF0);
			MT9V034_WrFpgaReg(pUsbHandle,0x03, 0x01);
			MT9V034_WrFpgaReg(pUsbHandle,0x04, 0xE0);   
			break;
		case RESOLU_640_480:	
			MT9V034_WrSensorReg(pUsbHandle,0x01, 0x0039);
			MT9V034_WrSensorReg(pUsbHandle,0x02, 0x0004);
			MT9V034_WrSensorReg(pUsbHandle,0x03, 0x01E0);
			MT9V034_WrSensorReg(pUsbHandle,0x04, 0x0280);

			MT9V034_WrFpgaReg(pUsbHandle,0x01, 0x02);
			MT9V034_WrFpgaReg(pUsbHandle,0x02, 0x80);
			MT9V034_WrFpgaReg(pUsbHandle,0x03, 0x01);
			MT9V034_WrFpgaReg(pUsbHandle,0x04, 0xE0);  					
			break;
		default:
			break;
	}
	return 0;
		

}


static cq_int32_t MT9V034_SetMirrorType(CCyUSBDevice *pUsbHandle, const cq_uint32_t chMirrorType)
{
	if(MIRROR_NORMAL==chMirrorType)	//normal
		return MT9V034_WrSensorReg(pUsbHandle, 0x0D, 0x0300);

	if(MIRROR_X==chMirrorType)//X
		return MT9V034_WrSensorReg(pUsbHandle, 0x0D, 0x0320);

	if(MIRROR_Y==chMirrorType)//Y
		return MT9V034_WrSensorReg(pUsbHandle, 0x0D, 0x0310);

	if(MIRROR_XY==chMirrorType)//XY
		return MT9V034_WrSensorReg(pUsbHandle, 0x0D, 0x0330);

	return -99;//should never reach here

}

/*
static cq_int32_t MT9V034_SetBitDepth(CCyUSBDevice *pUsbHandle, const cq_uint32_t chBitDepthType)
{
	if(BITDEPTH_8==chBitDepthType)	//8
		return MT9V034_WrFpgaReg(pUsbHandle, 0x7, 0x0);

	if(BITDEPTH_16==chBitDepthType)//16
		return MT9V034_WrFpgaReg(pUsbHandle, 0x7, 0x1);

	if(BITDEPTH_16_2==chBitDepthType)//16
		return MT9V034_WrFpgaReg(pUsbHandle, 0x7, 0x2);

	return -99;//should never reach here

}

*/
static cq_int32_t MT9V034_StartCap(CCyUSBDevice *pUsbHandle)
{
	MT9V034_WrFpgaReg(pUsbHandle, 0x09, 0x00);
	MT9V034_WrFpgaReg(pUsbHandle, 0x09, 0x01);
	return 0;	
}

static cq_int32_t MT9V034_StopCap(CCyUSBDevice *pUsbHandle)
{
	MT9V034_WrFpgaReg(pUsbHandle, 0x09, 0x00);
	return 0;
}

static cq_int32_t MT9V034_SendUsbSpeed2Fpga(CCyUSBDevice *pUsbHandle, const cq_uint32_t chSpeedType)
{
	MT9V034_WrFpgaReg(pUsbHandle, 0x08, chSpeedType);
	return 0;
}


/*
static tagSensor sensor_MT9V034=
{
	.name="MT9V034",
	.iicAddr=0xf0,
	.regAddrLen=1,
	.regValueLen=2,

	.InitSensor		=	NULL,
	.WrSensorReg	=	MT9V034_WrSensorReg,
	.RdSensorReg	=	MT9V034_RdSensorReg,
	.WrFpgaReg		=	MT9V034_WrFpgaReg,
	.RdFpgaReg		=	MT9V034_RdFpgaReg,
	.SetAnalogGain	=	NULL,
	.SetFpgaTrigFreq=	MT9V034_SetFpgaTrigFreq,
	.SetTrigMode	=	MT9V034_SetTrigMode,
	.SetExpoValue	=	MT9V034_SetExpoValue,
	.SetGainValue	=	MT9V034_SetGainValue,
	.SetAutoGainExpo=	MT9V034_SetAutoGainExpo,
	.SetResolution	=	MT9V034_SetResolution,
	.SetMirrorType	=	MT9V034_SetMirrorType,
	.SetBitDepth	=	NULL,
	.StartCap 		=	MT9V034_StartCap,
	.StopCap 		=	MT9V034_StopCap,
	.SendUsbSpeed2Fpga=	MT9V034_SendUsbSpeed2Fpga,

	.WrEeprom		=	MT9V034_WrEeprom,
	.RdEeprom		=	MT9V034_RdEeprom,
	.WrDevID		=	MT9V034_WrDevID,
	.RdDevID		=	MT9V034_RdDevID,
	.WrDevSN		=	MT9V034_WrDevSN,
	.RdDevSN		=	MT9V034_RdDevSN,
	.SoftTrig		=	MT9V034_SoftTrig
};
*/
static tagSensor sensor_MT9V034=
{
	"MT9V034",
	0xf0,
	1,
	2,

	NULL,
	MT9V034_WrSensorReg,
	MT9V034_RdSensorReg,
	MT9V034_WrFpgaReg,
	MT9V034_RdFpgaReg,
	NULL,
	MT9V034_SetFpgaTrigFreq,
	MT9V034_SetTrigMode,
	MT9V034_SetExpoValue,
	MT9V034_SetGainValue,
	MT9V034_SetAutoGainExpo,
	MT9V034_SetResolution,
	MT9V034_SetMirrorType,
	NULL,
	MT9V034_StartCap,
	MT9V034_StopCap,
	MT9V034_SendUsbSpeed2Fpga,

	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,

	MT9V034_SoftTrig
};
void RegisterSensor_MT9V034(list<tagSensor>& sensorList)
{
	sensorList.push_back(sensor_MT9V034);

}

