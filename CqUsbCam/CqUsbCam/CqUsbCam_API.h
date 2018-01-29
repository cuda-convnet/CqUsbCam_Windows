#ifndef _CQUSBCAM_API_H_
#define _CQUSBCAM_API_H_

/****************************************************************************************\
 * This is the main header file for the Caiqi usb camera for Linux/Mac						*
 *                                                                                	*
 * Author              :        nxb     											* 												
 * License             :        GPL Ver 2.0                                       	*
 * Copyright           :        Caiqi Electronics Inc.								*
 * Date written        :        Jan 28, 2017                                    	*
 * Modification Notes  :                                                          	*
 *    1. nxb, Jan 28, 2017                                  						*
 *       Add documentation.															*        															*
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


#include "CqUsbCam.h"


CQUSBCAM_API cq_int32_t OpenUSB(cq_uint32_t usbNum);




#endif //_CQUSBCAM_API_H_