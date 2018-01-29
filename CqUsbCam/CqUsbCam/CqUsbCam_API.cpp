#if 0
#include "stdafx.h"
#include "CqUsbCam_API.h"
 #include <vector>

using namespace std;

vector<CCqUsbCam*> g_vecDev;
	
	;

CQUSBCAM_API cq_int32_t OpenUSB(HANDLE h, cq_uint32_t devNum)
{
	if (devNum > g_vecDev.size() | devNum < 0| devNum < g_vecDev.size())
		return -1;

	CCqUsbCam* dev = new CCqUsbCam(h);	
	g_vecDev.push_back(dev);

	if (g_vecDev[devNum]->m_bIsInterfaceClaimed == true)
		return -2;

	if(g_vecDev[devNum]->OpenUSB(devNum) != 0)// 打开USB失败
	{
		g_vecDev[devNum]->CloseUSB();
		delete dev;
		vector<CCqUsbCam*>::iterator k = g_vecDev.begin() + devNum;
		g_vecDev.erase(k);
		return -3;
	}
	else// 打开USB成功
	{
		devInfo_t devInfo;
		g_vecDev[devNum]->GetDevInfo(devInfo);
		//cq_int8_t sensorType[DEV_INFOR_SENSOR_TYPE_LEN];
		if(strcmp(devInfo.sensorType, "0134")||strcmp(devInfo.sensorType, "0135"))
			g_vecDev[devNum]->SelectSensor((string*)"AR0135");
		else if(strcmp(devInfo.sensorType, "V034"))
			g_vecDev[devNum]->SelectSensor((string*)"MT9V034");
		else if(strcmp(devInfo.sensorType, "M001"))
			g_vecDev[devNum]->SelectSensor((string*)"MT9M001");
		else if(strcmp(devInfo.sensorType, "P001"))
			g_vecDev[devNum]->SelectSensor((string*)"MT9P001");
		else
			return -4;//sensor type错误

		return 0;
	}
}
#endif
