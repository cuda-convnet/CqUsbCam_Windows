#ifndef _DATACAPTURE_H_
#define _DATACAPTURE_H_

/****************************************************************************************\
 * This is the header file for the Caiqi usb camera for Linux/Mac						*
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
#include "Types.h"
  
#include <string>
#include <stdint.h>
#include <string.h>
#include "ImgFrame.h"
#include "wqueue.h"
//#include "cyusb.h"
#include "CyAPI.h"

 	
typedef struct tagPoint
{
    cq_uint64_t x;
    cq_uint64_t y;
} MyPoint;

class CDataCapture
{

public:
	CDataCapture(const cq_uint32_t iWidth, const cq_uint32_t iHeight);
	
	cq_int32_t Open();
	cq_int32_t Close();
	cq_int32_t ReadData( cq_uint8_t* pbuff, cq_int64_t &lBytes );
	cq_int32_t Input(const cq_uint8_t* lpData, const cq_uint32_t dwSize );
	
	void SetImgQueue(wqueue<CImgFrame*>  *pImgQueue);
	void SetUsbHandle(CCyUSBDevice *m_pUsbHandle);
	
	//void ThreadFunc();
	//static void* ThreadAdapter(void* __this);
	static unsigned int __stdcall ThreadAdapter(void* __this);
	int ThreadFunc();

	//cq_int32_t ReadData(cq_uint8_t* pbuff, cq_int32_t &lBytes);
	
	
private:
	cq_uint32_t	m_iCount;				//数据计数器

	cq_uint32_t m_iWidth;
	cq_uint32_t m_iHeight;
	
	cq_uint8_t*	m_pInData;	
	cq_uint8_t*	m_pOutData;	
	cq_uint8_t*	m_pReadBuff;	
	
	std::string wname;



 	//CCyUSBDevice *m_pUsbHandle;
	CCyUSBDevice *m_pUsbHandle;

 	wqueue<CImgFrame*>  *m_pImgQueue;	//

	CImgFrame *m_pInputframe;


	cq_bool_t m_bCapture;

	//pthread_mutex_t m_mutexThread;
	HANDLE m_mutexThread;
	//pthread_t m_pThread; 
	HANDLE m_hThread;

public:
	cq_uint64_t m_lRecvByteCnt;
	cq_uint64_t m_lRecvFrameCnt;
};
 	
 	
 	
 	
 	
 	
 	
 

 
 
 
 
 
 
 
 
 #endif // _DATACAPTURE_H_
