// usbCamConsole.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include<windows.h>  
#include <Mmsystem.h>  
#pragma comment(lib, "winmm.lib")  


#include "CqUsbCam.h"
#include "SensorCapbablity.h"

#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

#define MAIN_RESOLU_SELECT	 		'a'
#define MAIN_RESOLU_752_480 		'0'
#define MAIN_RESOLU_640_480 		'1'
#define MAIN_RESOLU_640_480SKIP 	'2'
#define MAIN_RESOLU_640_480BIN		'3'
#define MAIN_RESOLU_1280_1024		'4'
#define MAIN_RESOLU_1280_720		'5'
#define MAIN_RESOLU_1280_960		'6'

#define MAIN_BITDEPTH_SELECT 		'b'
#define MAIN_BITDEPTH_8		 		'0'
#define MAIN_BITDEPTH_16		 	'1'
#define MAIN_BITDEPTH_L8		 	'2'

#define MAIN_PROCTYPE_SELECT 		'c'
#define MAIN_PROCTYPE_N		 		'0'
#define MAIN_PROCTYPE_X		 		'1'
#define MAIN_PROCTYPE_Y		 		'2'
#define MAIN_PROCTYPE_XY		 	'3'


#define MAIN_CHECK_SPEED 			'd'

#define MAIN_TRIGMODE_SELECT		'e'
#define MAIN_TRIGMODE_AUTO 			'0'
#define MAIN_TRIGMODE_FPGA 			'1'
#define MAIN_TRIGMODE_SOFT 			'2'
#define MAIN_TRIGMODE_SIGNAL		'3'

#define MAIN_FPGA_TRIG_FREQ_SELECT	'g'
#define MAIN_EXPO_VALUE_SELECT		'h'
#define MAIN_GAIN_VALUE_SELECT		'i'
#define MAIN_AUTO_GAIN_EXPO_SELECT	'j'
#define MAIN_ROI					'k'
#define MAIN_CAPTURE				'l'
#define MAIN_ROI_BOX				'm'
#define MAIN_ANALOG_GAIN_AUTO_TRIG	'n'
#define MAIN_ANALOG_GAIN_FPGA_TRIG	'o'
#define MAIN_ANALOG_GAIN_1X			'1'
#define MAIN_ANALOG_GAIN_2X			'2'
#define MAIN_ANALOG_GAIN_4X			'4'
#define MAIN_ANALOG_GAIN_8X			'8'
#define MAIN_ROI_X1			'p'
#define MAIN_ROI_X2			'q'
#define MAIN_ROI_X3			'r'
#define MAIN_ROI_X4			's'
#define MAIN_ROI_X5			't'
#define MAIN_ROI_Y1			'u'
#define MAIN_ROI_Y2			'v'
#define MAIN_ROI_Y3			'w'
#define MAIN_ROI_Y4			'x'
#define MAIN_ROI_Y5			'y'
#define MAIN_ROI_WIDTH		'A'
#define MAIN_ROI_HEIGHT		'B'
#define MAIN_SAVE_VEDIO		'D'
#define MAIN_SAVE_ALL		'E'
#define MAIN_WRITE_SENSOR_REG	'F'
#define MAIN_READ_SENSOR_REG	'G'
#define MAIN_WRITE_FPGA_REG		'H'
#define MAIN_READ_FPGA_REG		'I'
#define MAIN_SELECT_SENSOR		'J'

#define MAIN_EXIT_NORMAL		'z'
 
cq_uint32_t g_iWidth=1280;
cq_uint32_t g_iHeight=720;
cq_uint8_t	g_byteBitDepthNo=1;
cq_uint8_t	g_byteResolutionType;

cq_bool_t g_bCap=false;

HANDLE g_mutexDisp;
HANDLE g_mutexTimer;
HANDLE g_mutexThread;

Mat frame;
CCqUsbCam *pCamInUse=new CCqUsbCam(NULL);

unsigned short hex2dec(char *hex)

{
	unsigned short  number=0;
	char *p=hex;
	for(p=hex;*p;++p)
	{
		if((hex[p-hex]<='z')&&(hex[p-hex]>='a'))
			hex[p-hex]=hex[p-hex]-32;
 		number=number*16+(hex[p-hex]>='A'?hex[p-hex]-'A'+10:hex[p-hex]-'0');
	}
	return number;
}


void checkspeed()
{
	unsigned int speed = 0;
	pCamInUse->GetUsbSpeed(speed);
	if (speed == USB_SPEED_SUPER)
	{
		printf("USB 3.0 device found");
		pCamInUse->SendUsbSpeed2Fpga(USB_SPEED_SUPER);
	}
	else if (speed == USB_SPEED_HIGH)
	{
		printf("USB 2.0 device found");
		pCamInUse->SendUsbSpeed2Fpga(USB_SPEED_HIGH);
	}
	else
		printf("Unknown USB speed");
}


void CALLBACK TimeProc(UINT uID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2)  
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	unsigned long iFrameCntPerSec =0;
	unsigned long iByteCntPerSec = 0;

	int i = 0, j = 0;

	WaitForSingleObject(g_mutexTimer, INFINITE);
	pCamInUse->GetRecvByteCnt(iByteCntPerSec);
	pCamInUse->ClearRecvByteCnt();
	pCamInUse->GetRecvFrameCnt(iFrameCntPerSec);
	pCamInUse->ClearRecvFrameCnt();
	printf("%d Fps     %0.4f MBs\n", iFrameCntPerSec,float(iByteCntPerSec)/1024.0/1024.0);	
	ReleaseMutex(g_mutexTimer);
}



void  Disp(LPVOID lpParam)
{
	WaitForSingleObject(g_mutexDisp, INFINITE); 
	frame.data=(cq_uint8_t*)lpParam;
	ReleaseMutex(g_mutexDisp);
}

unsigned int __stdcall ThreadEntry(void* __this)
{
	cv::namedWindow("disp",CV_WINDOW_AUTOSIZE | CV_GUI_NORMAL);
	HWND hWnd = (HWND)cvGetWindowHandle("disp");//��ȡ�Ӵ��ڵ�HWND
	HWND hParentWnd = ::GetParent(hWnd);//��ȡ������HWND��������������Ҫ�õ�
	long style = GetWindowLong(hParentWnd, GWL_STYLE);
	style &= ~(WS_SYSMENU);
	SetWindowLong(hParentWnd, GWL_STYLE, style);


	while(g_bCap)	
	{
		WaitForSingleObject(g_mutexDisp, INFINITE); 
		imshow("disp", frame);
		waitKey(1);
		ReleaseMutex(g_mutexDisp);
	}

	cv::destroyWindow("disp");
	cv::waitKey(1);
	cv::waitKey(1);
	cv::waitKey(1);
	cv::waitKey(1);

	printf("destroy window\n");

	return NULL;
}


int _tmain(int argc, _TCHAR* argv[])
{

	std::string sensorName = "AR0135";
	pCamInUse->SelectSensor(&sensorName);

	g_mutexDisp= CreateMutex(NULL, FALSE, NULL);
	g_mutexTimer = CreateMutex(NULL, FALSE, NULL);
	//g_mutexThread = CreateMutex(NULL, FALSE, NULL);

	if (pCamInUse->OpenUSB(0)<0)
	{
		printf("open usb failed, exiting ...\n");
		return -1;
	}
	printf("open usb succeed\n");

	pCamInUse->InitSensor();
	printf("init sensor done\n");

	checkspeed();

	while(1)
	{
		printf("Please input your choice ...\n");
		printf("\
				\'a\':	Select resolution\n\
				\'b\':	Select bit depth\n\
				\'c\':	Select proc type\n\
				\'d\':	Check speed\n\
				\'e\':	Set auto trig mode\n\
				\'f\':	Set fpga trig mode\n\
				\'g\':	Set fpga trig frequency\n\
				\'h\':	Set expo value\n\
				\'i\':	Set gain value\n\
				\'j\':	Set auto-gain-expo function\n\
				\n\
				\'l\':	Begin to capture\n\
				\n\
				\'n\':	Set analog gain while in auto trig mode\n\
				\'0\':	Set analog gain while in FPGA trig mode\n\
				\'F\':	Write sensor\n\
				\'G\':	Read sensor\n\
				\'H\':	Write FPGA\n\
				\'I\':	Read FPGA\n\
				\n\
				\'z\':	Exit\n"\
		);

		char ch=getchar();
		getchar();
		printf("Your choice is %c\n", ch);
		switch(ch)
		{
			case MAIN_RESOLU_SELECT:
			{
				printf("Please input your choice ...\n");
				printf("\
						\'5\':	1280x720\n\
						\'6\':	1280x960\n\
						\'2\':	640x480 in skip mode\n\
						\'3\':	640x480 in binning mode\n"\
				);
				char ch=getchar();
				getchar();	
				printf("Your choice is %c\n", ch);
				switch(ch)
				{
					case MAIN_RESOLU_1280_720:
						pCamInUse->SetResolution(RESOLU_1280_720);
						g_iWidth=1280;
						g_iHeight=720;
						break;
					case MAIN_RESOLU_1280_960:
						pCamInUse->SetResolution(RESOLU_1280_960);
						g_iWidth=1280;
						g_iHeight=960;
						break;
					case MAIN_RESOLU_640_480SKIP:
						pCamInUse->SetResolution(RESOLU_640_480_SKIP);
						g_iWidth=640;
						g_iHeight=480;
						break;
					case MAIN_RESOLU_640_480BIN:
						pCamInUse->SetResolution(RESOLU_640_480_BIN);
						g_iWidth=640;
						g_iHeight=480;
						break;
					default:
						printf("Bad inut ...");
				}
				break;
			}
			case MAIN_BITDEPTH_SELECT:
			{
				printf("Please input your choice ...\n");
				printf("\
						\'0\':	8 bit depth\n\
						\'1\':	16 bit depth\n\
						\'2\':	L8 bit depth\n"\
				);
				char ch=getchar();
				getchar();
				printf("Your choice is %c\n", ch);
				switch(ch)
				{
					case MAIN_BITDEPTH_8:
						pCamInUse->SetBitDepth(BITDEPTH_8);
						g_byteBitDepthNo=1;
						printf("g_byteBitDepthNo is %d\n", g_byteBitDepthNo);
						break;
					case MAIN_BITDEPTH_16:
						pCamInUse->SetBitDepth(BITDEPTH_16);
						g_byteBitDepthNo=2;
						printf("g_byteBitDepthNo is %d\n", g_byteBitDepthNo);
						break;
					case MAIN_BITDEPTH_L8:
						pCamInUse->SetBitDepth(BITDEPTH_16_2);
						g_byteBitDepthNo=2;
						printf("g_byteBitDepthNo is %d\n", g_byteBitDepthNo);
						break;
					default:
						printf("Bad inut ...\n");
				}
				break;			
			}
			case MAIN_PROCTYPE_SELECT:
			{
				printf("Please input your choice ...\n");
				printf("\
						\'0\':	Normal\n\
						\'1\':	X mirror\n\
						\'2\':	Y mirror\n\
						\'3\':	XY mirror\n"\
				);

				char ch=getchar();
				getchar();
				printf("Your choice is %c", ch);
				switch(ch)
				{
					case MAIN_PROCTYPE_N:
						pCamInUse->SetMirrorType(MIRROR_NORMAL);
						break;
					case MAIN_PROCTYPE_X:
						pCamInUse->SetMirrorType(MIRROR_X);
						break;
					case MAIN_PROCTYPE_Y:
						pCamInUse->SetMirrorType(MIRROR_Y);
						break;
					case MAIN_PROCTYPE_XY:
						pCamInUse->SetMirrorType(MIRROR_XY);
						break;
					default:
						printf("Bad inut ...\n");
				}
				break;
			}
			case MAIN_CHECK_SPEED:
			{
				checkspeed();
				break;
			}
			case MAIN_TRIGMODE_AUTO:
				pCamInUse->SetTrigMode(TRIGMODE_AUTO);
				break;
			case MAIN_TRIGMODE_FPGA:
				pCamInUse->SetTrigMode(TRIGMODE_FPGA);
				break;
			case MAIN_FPGA_TRIG_FREQ_SELECT:
			{
				printf("Please input the trig frequency (Dec, 0~45), make sure the device is in FPGA trig mode\n");
				char str[10];
				memset(str,0,sizeof(str));
				gets(str);
				unsigned int freq=atoi(str);
				printf("Your input is %d \n", freq);
				pCamInUse->SetFpgaTrigFreq(freq);	
				break;				
			}
			case MAIN_AUTO_GAIN_EXPO_SELECT:
			{
				printf("Please input your choice ...\n");
				printf("\
						\'0\':	Manual 	gain, Manual 	expo\n\
						\'1\':	Manual 	gain, Auto 	expo\n\
						\'2\':	Auto	gain, Manual 	expo\n\
						\'3\':	Auto 	gain, Auto	expo\n"\
				);
				char ch=getchar();
				getchar();
				printf("Your choice is %c\n", ch);
				switch(ch)
				{
					case '0':
						pCamInUse->SetAutoGainExpo(false, false);
						break;
					case '1':
						pCamInUse->SetAutoGainExpo(false, true);
						break;
					case '2':
						pCamInUse->SetAutoGainExpo(true, false);
						break;
					case '3':
						pCamInUse->SetAutoGainExpo(true, true);
						break;
					default:
						printf("Bad inut ...\n");					
				}
				break;
			}

			case MAIN_EXPO_VALUE_SELECT:
			{
				printf("Please input the expo value (Dec, 0~65536)\n");
				char str[10];
				memset(str,0,sizeof(str));
				gets(str);
				unsigned int expoValue=atoi(str);
				printf("Your input is %d \n", expoValue);
				pCamInUse->SetExpoValue(expoValue);
				break;
			}
			case MAIN_GAIN_VALUE_SELECT:
			{
				printf("Please input the gain value (Dec, 0~64)\n");
				char str[10];
				memset(str,0,sizeof(str));
				gets(str);
				unsigned int gainValue=atoi(str);
				printf("Your input is %d \n", gainValue);
				pCamInUse->SetGainValue(gainValue);
				break;
			}		
			case MAIN_CAPTURE:
			{
				Mat tempFrame(g_iHeight, g_iWidth, CV_8UC1);
				frame=tempFrame;
				pCamInUse->StartCap(g_iHeight, (g_byteBitDepthNo == 1 ? g_iWidth : g_iWidth * 2), Disp);
				g_bCap=true;
				Sleep(10);

				HANDLE hThread;
				hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadEntry, NULL, 0, NULL);
				
				int timerID=timeSetEvent( 1000, 0, TimeProc, 0, (UINT)TIME_PERIODIC);  

				printf("Press any key to stop capturing\n");		
				getchar();
				g_bCap=false;
				timeKillEvent(timerID);

				pCamInUse->StopCap();
				printf("stop cap\n");
				break;
			}
			case MAIN_ANALOG_GAIN_AUTO_TRIG:
			{
				printf("Please input your choice ...\n");
				printf("\
						\'1\':	1x\n\
						\'2\':	2x\n\
						\'4\':	4x\n\
						\'8\':	8x\n"\
				);
				char ch=getchar();
				getchar();
				printf("Your choice is %c\n", ch);
				switch(ch)
				{
					case MAIN_ANALOG_GAIN_1X:
						pCamInUse->SetAnalogGain(TRIGMODE_AUTO, ANALOGGAIN_1X);
						break;
					case MAIN_ANALOG_GAIN_2X:
						pCamInUse->SetAnalogGain(TRIGMODE_AUTO, ANALOGGAIN_2X);
						break;
					case MAIN_ANALOG_GAIN_4X:
						pCamInUse->SetAnalogGain(TRIGMODE_AUTO, ANALOGGAIN_4X);
						break;
					case MAIN_ANALOG_GAIN_8X:
						pCamInUse->SetAnalogGain(TRIGMODE_AUTO, ANALOGGAIN_8X);
						break;
					default:
						printf("Bad inut ...");
				}
				break;
			}
			case MAIN_ANALOG_GAIN_FPGA_TRIG:
			{
				printf("Please input your choice ...\n");
				printf("\
						\'1\':	1x\n\
						\'2\':	2x\n\
						\'4\':	4x\n\
						\'8\':	8x\n"\
				);
				char ch=getchar();
				getchar();
				printf("Your choice is %c\n", ch);
				switch(ch)
				{
					case MAIN_ANALOG_GAIN_1X:
						pCamInUse->SetAnalogGain(TRIGMODE_FPGA, ANALOGGAIN_1X);
						break;
					case MAIN_ANALOG_GAIN_2X:
						pCamInUse->SetAnalogGain(TRIGMODE_FPGA, ANALOGGAIN_2X);
						break;
					case MAIN_ANALOG_GAIN_4X:
						pCamInUse->SetAnalogGain(TRIGMODE_FPGA, ANALOGGAIN_4X);
						break;
					case MAIN_ANALOG_GAIN_8X:
						pCamInUse->SetAnalogGain(TRIGMODE_FPGA, ANALOGGAIN_8X);
						break;
					default:
						printf("Bad inut ...");
				}
				break;
			}
			case MAIN_WRITE_SENSOR_REG:
			{
				printf("Please input the reg address (Hex. Just number, no \'0x\' needed))\n");
				char str[10];
				memset(str,0,sizeof(str));
				gets(str);
				unsigned int iDecRegAddr=hex2dec(str);
				printf("Your input is %x \n", iDecRegAddr);
				printf("Please input the reg value (Hex. Just number, no \'0x\' needed))\n");
				memset(str,0,sizeof(str));
				gets(str);
				unsigned int iDecRegValue=hex2dec(str);
				printf("Your input is %x \n", iDecRegValue);
				pCamInUse->WrSensorReg(iDecRegAddr, iDecRegValue);
				break;
			}
			case MAIN_READ_SENSOR_REG:
			{
				printf("Please input the reg address (Hex. Just number, no \'0x\' needed))\n");
				char str[10];
				memset(str,0,sizeof(str));
				gets(str);
				unsigned int iDecRegAddr=hex2dec(str);
				printf("Your input is %x \n", iDecRegAddr);
				unsigned int iValue=0;
				pCamInUse->RdSensorReg(iDecRegAddr, iValue);
				printf("reg value is %04x\n", iValue&0xffff);
				break;
			}
			case MAIN_WRITE_FPGA_REG:
			{
				printf("Please input the reg address (Hex. Just number, no \'0x\' needed))\n");
				char str[10];
				memset(str,0,sizeof(str));
				gets(str);
				unsigned char iDecRegAddr=hex2dec(str);
				printf("Your input is %x \n", iDecRegAddr);
				printf("Please input the reg value (Hex. Just number, no \'0x\' needed))\n");
				memset(str,0,sizeof(str));
				gets(str);
				unsigned char iDecRegValue=hex2dec(str);
				printf("Your input is %x \n", iDecRegValue);
				pCamInUse->WrFpgaReg(iDecRegAddr, iDecRegValue);
				break;
			}
			case MAIN_READ_FPGA_REG:
			{
				printf("Please input the reg address (Hex. Just number, no \'0x\' needed))\n");
				char str[10];
				memset(str,0,sizeof(str));
				gets(str);
				unsigned int iDecRegAddr=hex2dec(str);
				printf("Your input is %x \n", iDecRegAddr);
				unsigned int iValue=0;
				pCamInUse->RdFpgaReg(iDecRegAddr, iValue);
				printf("reg value is %02x\n", iValue&0xff);
				break;
			}
			case MAIN_EXIT_NORMAL:
				pCamInUse->CloseUSB();
				printf("Exiting ...\n");
				exit(0);
				break;
			default:
				printf("Bad inut ...\n");
		}

	}
	return 0;
}


