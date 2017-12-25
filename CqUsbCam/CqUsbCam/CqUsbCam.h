
#ifndef _CQUSBCAM_H_
#define _CQUSBCAM_H_

/****************************************************************************************\
 * This is the main header file for the Caiqi usb camera for Linux/Mac						*
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


// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� CQUSBCAM_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// CQUSBCAM_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�

#ifdef CQUSBCAM_EXPORTS
#define CQUSBCAM_API __declspec(dllexport)
#else
#define CQUSBCAM_API __declspec(dllimport)
#endif


#include <stdio.h>
#include <string>
#include <list>

//#include "cyusb.h"
#include "CyAPI.h"

#include "Types.h"
#include "ImgFrame.h"
#include "wqueue.h"
#include "DataCapture.h"
#include "DataProcess.h"
#include "tagSensor.h"


#define USB_SPEED_SUPER		0x00
#define USB_SPEED_HIGH		0x01

// �����Ǵ� CqUsbCam.dll ������
class CQUSBCAM_API CCqUsbCam {
private:
 	//CCyUSBDevice *m_pUsbHandle;

	CCyUSBDevice *m_pUsbHandle;
 	wqueue<CImgFrame*>  *m_pImgQueue;	//
	CDataCapture *m_pDataCap;
	CDataProcess *m_pDataProc;

	list<tagSensor> m_sensorList;
 	tagSensor m_sensorInUse;		
	
	cq_bool_t m_bIsInterfaceClaimed; 		
	cq_bool_t m_bIsCapturing;
	


 public:
 	CCqUsbCam(HANDLE h);
	~CCqUsbCam();

	cq_bool_t Reset();
	cq_int32_t SelectSensor(string* pStrSensorName);

	cq_int32_t WrSensorReg(const cq_uint32_t iAddr, const cq_uint32_t iValue);
	cq_int32_t RdSensorReg(const cq_uint32_t iAddr, cq_uint32_t & iValue);
	cq_int32_t WrFpgaReg(const cq_uint32_t 	iAddr, const cq_uint32_t iValue);
	cq_int32_t RdFpgaReg(const cq_uint32_t 	iAddr, cq_uint32_t& iValue);

	cq_int32_t OpenUSB(cq_uint32_t usbNum);
	cq_int32_t CloseUSB();	

	cq_int32_t WrEeprom(const cq_uint32_t iAddr, const cq_uint8_t iValue);
	cq_int32_t RrEeprom(const cq_uint32_t iAddr, cq_uint8_t * buffer, cq_uint32_t &length);
	cq_int32_t WrDevID(cq_uint8_t* chIdBuf, cq_uint32_t &length );
	cq_int32_t RdDevID(cq_uint8_t *chIdBuf, cq_uint32_t &length);
	cq_int32_t WrDevSN(cq_uint8_t* chSnBuf, cq_uint32_t &length );
	cq_int32_t RdDevSN(cq_uint8_t *chSnBuf, cq_uint32_t &length);

	cq_int32_t GetUsbSpeed(cq_uint32_t &chSpeedType);
	cq_int32_t InitSensor();


	cq_int32_t  StartCap(const cq_uint32_t iHeight, const cq_uint32_t iWidth, callback_t CallBackFunc);
	cq_int32_t  StopCap();

	
	cq_int32_t SetAnalogGain(const cq_uint32_t chTrigType, const cq_uint32_t chGainType);
	cq_int32_t SetFpgaTrigFreq(const cq_uint32_t iFreqVal);	
	cq_int32_t SetTrigMode(const cq_uint32_t chTrigType);

	
	cq_int32_t SetExpoValue(const cq_uint32_t iExpoVal);
	cq_int32_t SetGainValue(const cq_uint32_t iGainVal);
	cq_int32_t SetAutoGainExpo(const cq_bool_t bIsAutoGain, const cq_bool_t bIsAutoExpo);
	
	
	cq_int32_t SetResolution(const cq_uint32_t chResoluType);
	
	
	cq_int32_t SetMirrorType(const cq_uint32_t chMirrorType);
	cq_int32_t SetBitDepth(const cq_uint32_t chBitDepthType);

	cq_int32_t SendUsbSpeed2Fpga(const cq_uint32_t chSpeedType);

	cq_int32_t SoftTrig();

	void GetRecvByteCnt(cq_uint64_t& byteCnt);
	void ClearRecvByteCnt();
	void GetRecvFrameCnt(cq_uint64_t& frameCnt);
	void ClearRecvFrameCnt();
};

#endif //  _CQUSBCAM_H_