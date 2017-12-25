/****************************************************************************************\
 * This is the implention file for the Caiqi usb camera for Linux/Mac						*
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
#include "StdAfx.h"
#include <process.h>
#include <assert.h>

#include "DataProcess.h"


//#define TEST

#ifdef TEST
#include <opencv/cv.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#endif

CDataProcess::CDataProcess()
{
    m_bProcess=false;

    //pthread_mutex_init(&m_mutexThread, NULL);
	m_mutexThread = CreateMutex(NULL, FALSE, NULL);
}

void CDataProcess::Open()
{
	//pthread_create( &m_pThread, NULL, ThreadAdapter, this );
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadAdapter, this, 0, NULL);
	m_bProcess=true;
}
//#define TEST_CLOSE
void CDataProcess::Close()
{
    m_bProcess=false;

    m_pImgQueue->reset();
#ifdef TEST_CLOSE
#else
    //pthread_mutex_lock(&m_mutexThread); 
	WaitForSingleObject(m_mutexThread, INFINITE); 
    //pthread_mutex_unlock(&m_mutexThread);
	ReleaseMutex(m_mutexThread);
#endif

}

void CDataProcess::SetImgQueue(wqueue<CImgFrame*>  *pImgQueue)
{
	assert(NULL!=pImgQueue);
	m_pImgQueue=pImgQueue;
}
void CDataProcess::SetCallBackFunc(callback_t CallBackFunc)
{
	assert(NULL!=CallBackFunc);
	m_CallBackFunc=CallBackFunc;
}

unsigned int CDataProcess::ThreadAdapter(void* __this)
{
	CDataProcess* _this=(CDataProcess*) __this;
	_this->ThreadFunc();	

	return NULL;
}
/*test*****************************************/
#ifdef TEST
int g_flag=0;
void Disp(void* frameData)
{
    //g_pData=(unsigned char*)frameData;
    if(g_flag==1)
        memset(frameData,0,1280*720);
    //frame.data=(unsigned char*)frameData;
    cv::Mat frame(720, 1280, CV_8UC1, (unsigned char*)frameData);


    cv::imshow("disp",frame);
    cv::waitKey(1);
    //cv::waitKey(1);
}
#endif
/*****************************************************/
int CDataProcess::ThreadFunc()
{

	//cv::namedWindow("disp",CV_WINDOW_AUTOSIZE | CV_GUI_NORMAL);

	CImgFrame* imgfrm;//be careful of this*********************
	
	//pthread_mutex_lock(&m_mutexThread);
	WaitForSingleObject(m_mutexThread, INFINITE);
	
	while(true==m_bProcess)
	{
		imgfrm=m_pImgQueue->remove();
        if(false==m_bProcess)
			break;

		//m_CallBackFunc(imgfrm->m_imgBuf);
#ifdef TEST
		Disp(imgfrm->m_imgBuf);
#else
        if(imgfrm!=NULL)
			m_CallBackFunc(imgfrm->m_imgBuf);
#endif

		Sleep(1);	
    }
    //pthread_mutex_unlock(&m_mutexThread);
	ReleaseMutex(m_mutexThread);
	return 0;

}
