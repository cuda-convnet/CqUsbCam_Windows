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


#include "CqUsbCam.h"


CQUSBCAM_API cq_int32_t OpenUSB(cq_uint32_t usbNum);




#endif //_CQUSBCAM_API_H_