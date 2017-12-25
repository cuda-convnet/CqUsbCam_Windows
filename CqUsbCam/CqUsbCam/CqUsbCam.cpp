/****************************************************************************************\
 * This is the main file for the Caiqi usb camera for Linux/Mac						*
 *                                                                                	*
 * Author              :        nxb     											* 												
 * License             :        GPL Ver 2.0                                       	*
 * Copyright           :        Caiqi Electronics Inc.								*
 * Date written        :        Oct 12, 2017                                    	*
 * Modification Notes  :                                                          	*
 *    1. nxb, Oct 12, 2017                                  						*
 *       Add documentation.                                          				*
 *                      															*
 *                                                                                	*
 \***************************************************************************************/

// CqUsbCam.cpp : 定义 DLL 应用程序的导出函数。
//

#include "StdAfx.h"

#include "CqUsbCam.h"
#include "Types.h"
#include "./sensors/AR0135.h"
#include "./sensors/MT9V034.h"
#include "./sensors/MT9M001.h"
#include <assert.h>



#define ERR_ITF_NOT_CLAIMED 		-0x80
#define ERR_ITF_IS_ALREADY_CLAIMED	-0x81
#define ERR_NULL_FUNC_POINTER		-0x82

#define ERR_IS_CAPTURING		-0x83
#define ERR_IS_NOT_CAPTURING	-0x84

#define TEST_SelectSensor

CCqUsbCam::CCqUsbCam(HANDLE h)
{
	m_pUsbHandle=NULL;

 	m_pImgQueue=NULL;
	m_pDataCap=NULL;
	m_pDataProc=NULL;

	m_bIsInterfaceClaimed=false; 		
	m_bIsCapturing=false;

	RegisterSensor_AR0135(m_sensorList);
	RegisterSensor_MT9V034(m_sensorList);
	RegisterSensor_MT9M001(m_sensorList);

	m_pUsbHandle=new CCyUSBDevice(h);
	assert(NULL!=m_pUsbHandle);

#ifdef TEST_SelectSensor
#else
	int size=m_sensorList.size();
	m_sensorInUse=*m_sensorList.begin();
#endif
}

CCqUsbCam::~CCqUsbCam()
{
	delete m_pUsbHandle;
}

cq_bool_t CCqUsbCam::Reset()
{
	m_pUsbHandle->ControlEndPt->Reset();
	m_pUsbHandle->BulkInEndPt->Reset();
	m_pUsbHandle->BulkOutEndPt->Reset();
	//m_pUsbHandle->IsocInEndPt->Reset();
	//m_pUsbHandle->IsocOutEndPt->Reset();
	//m_pUsbHandle->Reset();
	return 0;
}

cq_int32_t  CCqUsbCam::SelectSensor(string* pStrSensorName)
{
	assert(NULL!=pStrSensorName);

#ifdef TEST_SelectSensor
	list<tagSensor>::iterator i;      

	if(0==m_sensorList.size())
		return -1;

    for (i = m_sensorList.begin(); i != m_sensorList.end(); ++i)   
    {
		if((*pStrSensorName)==(*i).name)
		{
			m_sensorInUse=(*i);
			return 0;
		}
	}
	return -2;
#else
	m_sensorInUse=m_sensorList.front();
	return 0;
#endif
}

cq_int32_t CCqUsbCam::OpenUSB(cq_uint32_t usbNum)
{
	m_pUsbHandle->Open(usbNum);
	if(m_pUsbHandle->IsOpen())
	{
		m_pUsbHandle->ControlEndPt->Target = TGT_DEVICE;
		m_pUsbHandle->ControlEndPt->ReqType = REQ_VENDOR;
		m_pUsbHandle->ControlEndPt->Value = 0;
		m_pUsbHandle->ControlEndPt->Index = 0;

		m_bIsInterfaceClaimed=true;

		return m_pUsbHandle->VendorID;
	}
	return -1;

}

cq_int32_t CCqUsbCam::CloseUSB()
{
    if(m_pUsbHandle->IsOpen())
	{
		m_pUsbHandle->Close();
		m_bIsInterfaceClaimed=false;
		return 0;
	}
	return -1;
}



cq_int32_t  CCqUsbCam::InitSensor()
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.InitSensor)
		return ERR_NULL_FUNC_POINTER;

	return m_sensorInUse.InitSensor(m_pUsbHandle);
}


cq_int32_t  CCqUsbCam::StartCap(const cq_uint32_t iHeight, const cq_uint32_t iWidth, callback_t CallBackFunc)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

	if(true ==m_bIsCapturing)
		return ERR_IS_CAPTURING;
	 
	if(NULL==m_sensorInUse.StartCap)
		return ERR_NULL_FUNC_POINTER;

	m_pImgQueue=new wqueue<CImgFrame*>;
	m_pDataCap=new CDataCapture(iWidth, iHeight);
	m_pDataProc=new CDataProcess();

	assert(NULL!=m_pImgQueue);
	assert(NULL!=m_pDataCap);
	assert(NULL!=m_pDataProc);


    m_pDataCap->SetUsbHandle(m_pUsbHandle);
    m_pDataCap->SetImgQueue(m_pImgQueue);

	m_pDataProc->SetCallBackFunc(CallBackFunc);
    m_pDataProc->SetImgQueue(m_pImgQueue);


    m_pDataCap->Open();
	m_pDataProc->Open();

	m_sensorInUse.StartCap(m_pUsbHandle);

	m_bIsCapturing=true;
	return 0;
}


cq_int32_t  CCqUsbCam::StopCap()
{
	if(true!=m_bIsCapturing)
		return ERR_IS_NOT_CAPTURING;

	if(NULL==m_sensorInUse.StopCap)
		return ERR_NULL_FUNC_POINTER;

	m_sensorInUse.StopCap(m_pUsbHandle);

	m_pDataProc->Close();//必须保证在停止采集之前停掉处理线程，否则会造成死锁
    m_pDataCap->Close();
	

	if(NULL!=m_pImgQueue)	delete m_pImgQueue;
	if(NULL!=m_pDataCap)	delete m_pDataCap;
	if(NULL!=m_pDataProc)	delete m_pDataProc;


	
	
	m_bIsCapturing=false;
	return 0;
}

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

cq_int32_t CCqUsbCam::WrDevID(cq_uint8_t* chIdBuf, cq_uint32_t &length )
{
	USB_ORDER		sUsbOrder;

	sUsbOrder.pData=chIdBuf;
	sUsbOrder.ReqCode = 0xD0;
	sUsbOrder.DataBytes = length;
	sUsbOrder.Direction = 0;
	sUsbOrder.Index = 0;
	sUsbOrder.Value = 0;

	cq_int32_t ret = SendOrder(m_pUsbHandle, &sUsbOrder);
	length = sUsbOrder.DataBytes;
	return ret;
}
cq_int32_t CCqUsbCam::RdDevID(cq_uint8_t *chIdBuf, cq_uint32_t &length)
{
	USB_ORDER		sUsbOrder;

	sUsbOrder.pData=chIdBuf;
	sUsbOrder.ReqCode = 0xD1;
	sUsbOrder.DataBytes = length;
	sUsbOrder.Direction = 1;
	sUsbOrder.Index = 0;
	sUsbOrder.Value = 0;

	cq_int32_t ret = SendOrder(m_pUsbHandle, &sUsbOrder);
	length=sUsbOrder.DataBytes;
	return ret;
}

cq_int32_t CCqUsbCam::WrDevSN(cq_uint8_t* chSnBuf, cq_uint32_t &length )
{
	USB_ORDER		sUsbOrder;

	sUsbOrder.pData=chSnBuf;
	sUsbOrder.ReqCode = 0xD2;
	sUsbOrder.DataBytes = length;
	sUsbOrder.Direction = 0;
	sUsbOrder.Index = 0;
	sUsbOrder.Value = 0;

	cq_int32_t ret = SendOrder(m_pUsbHandle, &sUsbOrder);
	length = sUsbOrder.DataBytes;
	return ret;
}
cq_int32_t CCqUsbCam::RdDevSN(cq_uint8_t *chSnBuf, cq_uint32_t &length)
{
	USB_ORDER		sUsbOrder;

	sUsbOrder.pData=chSnBuf;
	sUsbOrder.ReqCode = 0xD3;
	sUsbOrder.DataBytes = length;
	sUsbOrder.Direction = 1;
	sUsbOrder.Index = 0;
	sUsbOrder.Value = 0;

	cq_int32_t ret = SendOrder(m_pUsbHandle, &sUsbOrder);
	length=sUsbOrder.DataBytes;
	return ret;
}
cq_int32_t CCqUsbCam::WrEeprom(const cq_uint32_t iAddr, const cq_uint8_t iValue)
{
	USB_ORDER		sUsbOrder;
	cq_uint8_t		chData[64];

	sUsbOrder.pData=chData;
	sUsbOrder.ReqCode = 0xF5;
	sUsbOrder.DataBytes = 2;
	sUsbOrder.Direction = 0;
	sUsbOrder.Index = iAddr&0xffff;
	sUsbOrder.Value = iValue&0xffff;

	return SendOrder(m_pUsbHandle, &sUsbOrder);
}
cq_int32_t CCqUsbCam::RrEeprom(const cq_uint32_t iAddr, cq_uint8_t * buffer, cq_uint32_t &length)
{
	USB_ORDER		sUsbOrder;

	sUsbOrder.pData = buffer;
	sUsbOrder.ReqCode = 0xF6;
	sUsbOrder.DataBytes = length;
	sUsbOrder.Direction = 1;
	sUsbOrder.Index = iAddr&0xffff;
	sUsbOrder.Value = 0;

	cq_int32_t ret = SendOrder(m_pUsbHandle, &sUsbOrder);
	length=sUsbOrder.DataBytes;
	return ret;
}

cq_int32_t CCqUsbCam::GetUsbSpeed(cq_uint32_t &chSpeedType)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

	if (m_pUsbHandle->bHighSpeed == true)
		chSpeedType=USB_SPEED_HIGH;
	else
		chSpeedType=USB_SPEED_SUPER;
	return 0;
}


cq_int32_t CCqUsbCam::SetAnalogGain(const cq_uint32_t chTrigType, const cq_uint32_t chGainType)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SetAnalogGain)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SetAnalogGain(m_pUsbHandle, chTrigType, chGainType);
}

cq_int32_t CCqUsbCam::SetFpgaTrigFreq(const cq_uint32_t iFreqVal)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SetFpgaTrigFreq)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SetFpgaTrigFreq(m_pUsbHandle, iFreqVal);
}

cq_int32_t CCqUsbCam::SetTrigMode(const cq_uint32_t chTrigType)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SetTrigMode)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SetTrigMode(m_pUsbHandle, chTrigType);
}

cq_int32_t CCqUsbCam::SetExpoValue(const cq_uint32_t iExpoVal)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SetExpoValue)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SetExpoValue(m_pUsbHandle, iExpoVal);
}

cq_int32_t CCqUsbCam::SetGainValue(const cq_uint32_t iGainVal)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SetGainValue)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SetGainValue(m_pUsbHandle, iGainVal);
}

cq_int32_t CCqUsbCam::SetAutoGainExpo(const cq_bool_t bIsAutoGain, const cq_bool_t bIsAutoExpo)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SetAutoGainExpo)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SetAutoGainExpo(m_pUsbHandle, bIsAutoGain, bIsAutoExpo);
}

cq_int32_t CCqUsbCam::SetResolution(const cq_uint32_t chResoluType)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SetResolution)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SetResolution(m_pUsbHandle, chResoluType);
}

cq_int32_t CCqUsbCam::SetMirrorType(const cq_uint32_t chMirrorType)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SetMirrorType)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SetMirrorType(m_pUsbHandle, chMirrorType);
}

cq_int32_t CCqUsbCam::SetBitDepth(const cq_uint32_t chBitDepthType)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SetBitDepth)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SetBitDepth(m_pUsbHandle, chBitDepthType);
}

cq_int32_t CCqUsbCam::SendUsbSpeed2Fpga(const cq_uint32_t chSpeedType)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SendUsbSpeed2Fpga)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SendUsbSpeed2Fpga(m_pUsbHandle, chSpeedType);
}
cq_int32_t CCqUsbCam::SoftTrig()
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.SoftTrig)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.SoftTrig(m_pUsbHandle);
}
cq_int32_t CCqUsbCam::WrSensorReg(const cq_uint32_t iAddr, const cq_uint32_t iValue)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.WrSensorReg)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.WrSensorReg(m_pUsbHandle, iAddr, iValue);
}

cq_int32_t CCqUsbCam::RdSensorReg(const cq_uint32_t iAddr, cq_uint32_t &iValue)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.RdSensorReg)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.RdSensorReg(m_pUsbHandle, iAddr, iValue);
}

cq_int32_t CCqUsbCam::WrFpgaReg(const cq_uint32_t 	iAddr, const cq_uint32_t iValue)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.WrFpgaReg)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.WrFpgaReg(m_pUsbHandle, iAddr, iValue);
}

cq_int32_t CCqUsbCam::RdFpgaReg(const cq_uint32_t 	iAddr, cq_uint32_t &iValue)
{
	if(false == m_bIsInterfaceClaimed)
		return ERR_ITF_NOT_CLAIMED;

    if(NULL==m_sensorInUse.RdFpgaReg)
		return ERR_NULL_FUNC_POINTER;
	return m_sensorInUse.RdFpgaReg(m_pUsbHandle, iAddr, iValue);
}


void CCqUsbCam::GetRecvByteCnt(cq_uint64_t& byteCnt)
{
	assert(NULL!=m_pDataCap);
	byteCnt=m_pDataCap->m_lRecvByteCnt;
}
void CCqUsbCam::ClearRecvByteCnt()
{
	assert(NULL!=m_pDataCap);
	m_pDataCap->m_lRecvByteCnt=0;
}
void CCqUsbCam::GetRecvFrameCnt(cq_uint64_t& frameCnt)
{
	assert(NULL!=m_pDataCap);
	frameCnt=m_pDataCap->m_lRecvFrameCnt;
}
void CCqUsbCam::ClearRecvFrameCnt()
{
	assert(NULL!=m_pDataCap);
	m_pDataCap->m_lRecvFrameCnt=0;
}




