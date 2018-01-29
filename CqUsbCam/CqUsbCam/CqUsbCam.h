
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
 *       Add documentation.         
 *		2.   nxb, Dec 27, 2017                                    				*
 *       Add api description.                															*
 *                                                                                	*
 \***************************************************************************************/


// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 CQUSBCAM_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// CQUSBCAM_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。

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
#include "devInfo.h"


#define USB_SPEED_SUPER		0x00
#define USB_SPEED_HIGH		0x01

// 此类是从 CqUsbCam.dll 导出的
class CQUSBCAM_API CCqUsbCam {
private:
 	//CCyUSBDevice *m_pUsbHandle;

	CCyUSBDevice *m_pUsbHandle;
 	wqueue<CImgFrame*>  *m_pImgQueue;	
	CDataCapture *m_pDataCap;
	CDataProcess *m_pDataProc;

	list<tagSensor> m_sensorList;
 	tagSensor m_sensorInUse;		
	
	cq_bool_t m_bIsInterfaceClaimed; 		
	cq_bool_t m_bIsCapturing;
	
	/************************************************************************************************\
 	*	name:				SelectSensor
 	*	param:				pStrSensorName: 		input .  this param should be the sensor you are using. 
 	*																			currently, this dll supports the following sensors, 
 	*																			AR013x, MT9V034, MT9M001
 	*  return:				
 	*	description:		select sensor you want to use
 	\************************************************************************************************/	
	cq_int32_t SelectSensor(string* pStrSensorName);

 public:
 	/************************************************************************************************\
 	* 	name:				CCqUsbCam
 	*	param:				handle: 		input 
 	*  return:				
 	*	description:		constructor
 	\***********************************************************************************************/	
 	CCqUsbCam(HANDLE h);
 	
 	
 	/*************************************************************************************************\
 	*	name: 				~CCqUsbCam
 	*	param:
 	*  return:				
 	*	description:		deconstructor
 	\************************************************************************************************/	
	~CCqUsbCam();

	/*************************************************************************************************\
	*	name: 				Reset
 	*	param:
 	*  return:				
 	*	description:		reset usb endpoints
 	\************************************************************************************************/	
	cq_bool_t Reset();

	/************************************************************************************************\
 	*	name:				WrSensorReg
 	*	param:				iAddr: 		input .  address of the sensor reg 
 	*							iValue:		input .  value of the sensor reg 	
 	*  return:				0 on success, negative values on error
 	*	description:		write sensor register
 	\************************************************************************************************/	
	cq_int32_t WrSensorReg(const cq_uint32_t iAddr, const cq_uint32_t iValue);
	
	
	/************************************************************************************************\
 	*	name:				RdSensorReg
 	*	param:				iAddr: 		input .  address of the sensor reg 
 	*							iValue:		output .  value of the sensor reg 	
 	*  return:				0 on success, negative values on error
 	*	description:		read sensor register
 	\************************************************************************************************/	
	cq_int32_t RdSensorReg(const cq_uint32_t iAddr, cq_uint32_t & iValue);
	
	/************************************************************************************************\
 	*	name:				WrFpgaReg
 	*	param:				iAddr: 		input .  address of the fpga reg 
 	*							iValue:		input .  value of the fpga reg 	
 	*  return:				0 on success, negative values on error
 	*	description:		write fpga register
 	\************************************************************************************************/	
	cq_int32_t WrFpgaReg(const cq_uint32_t 	iAddr, const cq_uint32_t iValue);
	
	/************************************************************************************************\
 	*	name:				RdFpgaReg
 	*	param:				iAddr: 		input .  address of the fpga reg 
 	*							iValue:		output .  value of the fpga reg 	
 	*  return:				0 on success, negative values on error
 	*	description:		read fpga register
 	\************************************************************************************************/	
	cq_int32_t RdFpgaReg(const cq_uint32_t 	iAddr, cq_uint32_t& iValue);


	/************************************************************************************************\
 	*	name:				OpenUSB
 	*	param:				usbNum: 		input .  number of the usb device 
 	*  return:				VendorID of the usb device  on success, negative values on error
 	*	description:		open usb device
 	\************************************************************************************************/	
	cq_int32_t OpenUSB(cq_uint32_t usbNum);
	
	/************************************************************************************************\
 	*	name:				CloseUSB
 	*	param:				
 	*  return:				0 on success, negative values on error
 	*	description:		close usb device
 	\************************************************************************************************/	
	cq_int32_t CloseUSB();	
	
	/************************************************************************************************\
 	*	name:				GetDevCnt
 	*	param:				devCnt: output. count of usb devices attached on the bus
 	*  return:				0 
 	*	description:		get usb device count attached on the bus
 	\************************************************************************************************/
	cq_int32_t GetDevCnt(cq_uint32_t& devCnt);

	/************************************************************************************************\
 	*	name:				GetDevInfo
 	*	param:				devInfo: output. usb device info
 	*  return:				0 
 	*	description:		get usb device info attached on the bus
 	\************************************************************************************************/
	cq_int32_t GetDevInfo(devInfo_t& devInfo);


	/************************************************************************************************\
 	*	name:				WrEeprom
 	*	param:				iAddr: 		input .  address of the eeprom. note that address ranges lower 
 	*														than 0x10000 are reserved 
 	*							iValue:		input .  value of the eeprom 	
 	*  return:				0 on success, negative values on error
 	*	description:		write eeprom
 	\************************************************************************************************/
	cq_int32_t WrEeprom(const cq_uint32_t iAddr, const cq_uint8_t iValue);
	
	/************************************************************************************************\
 	*	name:				RrEeprom
 	*	param:				iAddr: 		input .  address of the eeprom. note that address ranges lower 
 	*														than 0x10000 are reserved 
 	*							buffer:		output .  value of the eeprom 	
 	*							length:		inout. length in byte you want to read.  the output value of 'length' 
 	*														 indicates the count of bytes acctually transmitted 
 	*  return:				0 on success, negative values on error
 	*	description:		read eeprom
 	\************************************************************************************************/
	cq_int32_t RrEeprom(const cq_uint32_t iAddr, cq_uint8_t * buffer, cq_uint32_t &length);
	
	/************************************************************************************************\
 	*	name:				WrDevID
 	*	param:				chIdBuf:	input.	buffer containing device id .  
 	*							length:		inout.	shold be 1
 	*  return:				0 on success, negative values on error
 	*	description:		write device id
 	\************************************************************************************************/
	cq_int32_t WrDevID(cq_uint8_t* chIdBuf, cq_uint32_t &length );
	
	/************************************************************************************************\
 	*	name:				RdDevID
 	*	param:				chIdBuf:	output.	buffer containing device id .  
 	*							length:		inout.	shold be 1
 	*  return:				0 on success, negative values on error
 	*	description:		read device id
 	\************************************************************************************************/
	cq_int32_t RdDevID(cq_uint8_t *chIdBuf, cq_uint32_t &length);
	
	/************************************************************************************************\
 	*	name:				WrDevSN
 	*	param:				chSnBuf:	input.	buffer containing device sn 
 	*							length:		inout.	shold be 4
 	*  return:				0 on success, negative values on error
 	*	description:		write device sn
 	\************************************************************************************************/
	cq_int32_t WrDevSN(cq_uint8_t* chSnBuf, cq_uint32_t &length );
	
	/************************************************************************************************\
 	*	name:				RdDevSN
 	*	param:				chSnBuf:	output.	buffer containing device sn .  
 	*							length:		inout.	shold be 4
 	*  return:				0 on success, negative values on error
 	*	description:		read device sn
 	\************************************************************************************************/
	cq_int32_t RdDevSN(cq_uint8_t *chSnBuf, cq_uint32_t &length);


	/************************************************************************************************\
 	*	name:				GetUsbSpeed
 	*	param:				chSpeedType:	output. can be either of the following two
 	*													USB_SPEED_HIGH: 		usb 2.0 
 	*													USB_SPEED_SUPER:	usb 3.0
 	*  return:				0 on success, negative values on error
 	*	description:		get usb speed 
 	\************************************************************************************************/
	cq_int32_t GetUsbSpeed(cq_uint32_t &chSpeedType);
	
	/************************************************************************************************\
 	*	name:				InitSensor
 	*	param:				
 	*  return:				0 on success, negative values on error
 	*	description:		init sensor
 	\************************************************************************************************/
	cq_int32_t InitSensor();

	/************************************************************************************************\
 	*	name:				StartCap
 	*	param:				iHeight:	input. 	height of the frame
 	*							iWidth:		input.	width of the frame
 	*							CallBackFunc:	input. function called when frame is ready
 	*  return:				0 on success, negative values on error
 	*	description:		start capture
 	\************************************************************************************************/
	cq_int32_t  StartCap(const cq_uint32_t iHeight, const cq_uint32_t iWidth, callback_t CallBackFunc);
	
	/************************************************************************************************\
 	*	name:				StopCap
 	*	param:				
 	*  return:				0 on success, negative values on error
 	*	description:		stop capture
 	\************************************************************************************************/
	cq_int32_t  StopCap();

	/************************************************************************************************\
 	*	name:				SetAnalogGain
 	*	param:				chTrigType:			input. 	can be either of the following
 	*															TRIGMODE_AUTO,	TRIGMODE_FPGA
 	*							chGainType:		input.	can be either of the following
 	*															ANALOGGAIN_1X,	 ANALOGGAIN_2X,	
 	*															ANALOGGAIN_4X,	ANALOGGAIN_8X
 	*  return:				0 on success, negative values on error
 	*	description:		set analog gain of the sensor
 	\************************************************************************************************/	
	cq_int32_t SetAnalogGain(const cq_uint32_t chTrigType, const cq_uint32_t chGainType);

	/************************************************************************************************\
 	*	name:				SetFpgaTrigFreq
 	*	param:				iFreqVal:			input. 	value of frequency
 	*  return:				0 on success, negative values on error
 	*	description:		set FPGA trig frequency
 	\************************************************************************************************/
	cq_int32_t SetFpgaTrigFreq(const cq_uint32_t iFreqVal);	
	
	/************************************************************************************************\
 	*	name:				SetTrigMode
 	*	param:				chTrigType:	input. 	trig mode, can be either of the following
 															TRIGMODE_AUTO,	TRIGMODE_FPGA
 															TRIGMODE_SOFT, TRIGMODE_SIGNAL
 	*  return:				0 on success, negative values on error
 	*	description:		set trig mode
 	\************************************************************************************************/
	cq_int32_t SetTrigMode(const cq_uint32_t chTrigType);

	/************************************************************************************************\
 	*	name:				SetExpoValue
 	*	param:				iExpoVal:		input. 	value of exposure
 	*  return:				0 on success, negative values on error
 	*	description:		set exposure value
 	\************************************************************************************************/
	cq_int32_t SetExpoValue(const cq_uint32_t iExpoVal);
	
	/************************************************************************************************\
 	*	name:				SetGainValue
 	*	param:				iGainVal:		input. 	value of gain
 	*  return:				0 on success, negative values on error
 	*	description:		set gain value
 	\************************************************************************************************/
	cq_int32_t SetGainValue(const cq_uint32_t iGainVal);
	
	/************************************************************************************************\
 	*	name:				SetAutoGainExpo
 	*	param:				bIsAutoGain:		input. whether auto gain mode is selsected
 	*							bIsAutoExpo:		input. 	whether auto exposure mode is selsected
 	*  return:				0 on success, negative values on error
 	*	description:		set gain mode and exposure mode
 	\************************************************************************************************/
	cq_int32_t SetAutoGainExpo(const cq_bool_t bIsAutoGain, const cq_bool_t bIsAutoExpo);
	
	/************************************************************************************************\
 	*	name:				SetResolution
 	*	param:				chResoluType:	input. 	resolution type, can be either of the following
	*																	RESOLU_1280_720,	RESOLU_1280_960
	*																	RESOLU_752_480,	RESOLU_640_480
	*																	RESOLU_640_480_BIN,	RESOLU_640_480_SKIP
	*																	RESOLU_1280_1024
 	*  return:				0 on success, negative values on error
 	*	description:		set sensor resolution
 	\************************************************************************************************/
	cq_int32_t SetResolution(const cq_uint32_t chResoluType);
	
	/************************************************************************************************\
 	*	name:				SetMirrorType
 	*	param:				chMirrorType:		input. 	mirror type, can be either of the following
	*																	MIRROR_NORMAL,		MIRROR_X
	*																	MIRROR_Y,					MIRROR_XY
 	*  return:				0 on success, negative values on error
 	*	description:		set mirror type
 	\************************************************************************************************/	
	cq_int32_t SetMirrorType(const cq_uint32_t chMirrorType);
	
	/************************************************************************************************\
 	*	name:				SetBitDepth
 	*	param:				chBitDepthType:		input. 	bit depth type, can be either of the following
	*																	BITDEPTH_8,		BITDEPTH_16
	*																	BITDEPTH_16_2
 	*  return:				0 on success, negative values on error
 	*	description:		set bit depth
 	\************************************************************************************************/	
	cq_int32_t SetBitDepth(const cq_uint32_t chBitDepthType);


	/************************************************************************************************\
 	*	name:				SendUsbSpeed2Fpga
 	*	param:				chSpeedType:		input. 	usb speed type, can be either of the following
	*																	USB_SPEED_HIGH,		USB_SPEED_SUPER
 	*  return:				0 on success, negative values on error
 	*	description:		send usb speed to fpga
 	\************************************************************************************************/
	cq_int32_t SendUsbSpeed2Fpga(const cq_uint32_t chSpeedType);

	/************************************************************************************************\
 	*	name:				SoftTrig
 	*	param:				
 	*  return:				0 
 	*	description:		use software to trig
 	\************************************************************************************************/
	cq_int32_t SoftTrig();

	/************************************************************************************************\
 	*	name:				GetRecvByteCnt
 	*	param:				byteCnt:	output. 	count of bytes received since ClearRecvByteCnt is called
 	*  return:				
 	*	description:		get the count of bytes received since ClearRecvByteCnt is called
 	\************************************************************************************************/
	void GetRecvByteCnt(cq_uint64_t& byteCnt);
	
	/************************************************************************************************\
 	*	name:				ClearRecvByteCnt
 	*	param:
 	*	return:				
 	*	description:		clear the count of bytes received
 	\************************************************************************************************/
	void ClearRecvByteCnt();
	
	/************************************************************************************************\
 	*	name:				GetRecvFrameCnt
 	*	param:				frameCnt:	output. 	count of frames received since ClearRecvFrameCnt is called
 	*  return:				
 	*	description:		get the count of frames received since ClearRecvFrameCnt is called
 	\************************************************************************************************/
	void GetRecvFrameCnt(cq_uint64_t& frameCnt);
	
	/************************************************************************************************\
 	*	name:				ClearRecvFrameCnt
 	*	param:				
 	*  return:				
 	*	description:		clear the count of frames received
 	\************************************************************************************************/
	void ClearRecvFrameCnt();
};

#endif //  _CQUSBCAM_H_