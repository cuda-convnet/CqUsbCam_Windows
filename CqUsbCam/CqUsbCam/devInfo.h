#ifndef __DEVINFO_H__
#define __DEVINFO_H__

/****************************************************************************************\
 * This is the header file for the Caiqi usb camera for Linux/Mac						*
 *                                                                                	*
 * Author              :        nxb     											* 												
 * License             :        GPL Ver 2.0                                       	*
 * Copyright           :        Caiqi Electronics Inc.								*
 * Date written        :        Jan 28, 2018                                    	*
 * Modification Notes  :                                                          	*
 *    1. nxb, Jan 28, 2018                                   						*
 *       Add documentation.       
 *
 \***************************************************************************************/
#include "Types.h"


#define DEV_INFOR_MANUFACTURE_NAME_LEN	4   //制造商
#define DEV_INFOR_INTERFACE_TYPE_LEN	2	//接口类型
#define DEV_INFOR_SENSOR_TYPE_LEN		4	//sensor型号
#define DEV_INFOR_SENSOR_CNT_LEN		2	//sensor个数
#define	DEV_INFOR_PIX_FORMAT_LEN		4	//像素类型
#define	DEV_INFOR_PIX_DEPTH_LEN			2	//像素宽度
#define	DEV_INFOR_RESERVED_LEN			6	//保留
#define	DEV_INFOR_DEV_VERSION_LEN		4	//设备版本
#define	DEV_INFOR_DEV_SN_LEN			4	//设备序列号

typedef struct 
{
	cq_int8_t manufactureName[DEV_INFOR_MANUFACTURE_NAME_LEN];
	cq_int8_t interfaceType[DEV_INFOR_INTERFACE_TYPE_LEN];
	cq_int8_t sensorType[DEV_INFOR_SENSOR_TYPE_LEN];
	cq_int8_t sensorCnt[DEV_INFOR_SENSOR_CNT_LEN];
	cq_int8_t pixFormat[DEV_INFOR_PIX_FORMAT_LEN];
	cq_int8_t pixDepth[DEV_INFOR_PIX_DEPTH_LEN];
	cq_int8_t reserved[DEV_INFOR_RESERVED_LEN];
	cq_int8_t devVersion[DEV_INFOR_DEV_VERSION_LEN];
	cq_uint32_t devSN;
} devInfo_t;

#endif // __DEVINFO_H__