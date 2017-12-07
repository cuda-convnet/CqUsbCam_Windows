
// usbCamConsoleDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "usbCamConsole.h"
#include "usbCamConsoleDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

cq_uint32_t     g_iWidth;
cq_uint32_t     g_iHeight;
cq_uint8_t		g_byteBitDepthNo;
cq_uint8_t		g_byteResolutionType;

HANDLE g_mutexDisp;
HANDLE g_mutexTimer;


void  Disp(LPVOID lpParam)
{
	int i = 0, j = 0, k = 0;
	cq_uint8_t *pDataBuffer = (cq_uint8_t*)lpParam;
	cv::Mat frame(g_iHeight, g_iWidth, (g_byteBitDepthNo==1? CV_8UC1: CV_16UC1) ,pDataBuffer);
	
	//WaitForSingleObject(g_mutexDisp, INFINITE); 
	cv::imshow("disp",frame);
	//ReleaseMutex(g_mutexDisp);
}
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CusbCamConsoleDlg �Ի���




CusbCamConsoleDlg::CusbCamConsoleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CusbCamConsoleDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


	m_bUsbOpen=FALSE;
	m_bIsCapturing = false;

	m_lBytePerSecond=0;

	m_bIsCamSelected = false;

	m_sensorInUse = new CCqUsbCam(this);

	assert(m_sensorInUse);

	g_mutexDisp= CreateMutex(NULL, FALSE, NULL);
	g_mutexTimer = CreateMutex(NULL, FALSE, NULL);
}

void CusbCamConsoleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SENSOR_SUPPORTED, m_comboSensor);
	DDX_Control(pDX, IDC_COMBO_RESOLUTION, m_comboResolution);
}

BEGIN_MESSAGE_MAP(CusbCamConsoleDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO_SENSOR_SUPPORTED, &CusbCamConsoleDlg::OnCbnSelchangeComboSensorSupported)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_USB, &CusbCamConsoleDlg::OnBnClickedButtonOpenUsb)
	ON_BN_CLICKED(IDC_BUTTON_INIT_SENSOR, &CusbCamConsoleDlg::OnBnClickedButtonInitSensor)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_USB, &CusbCamConsoleDlg::OnBnClickedButtonCloseUsb)
	ON_BN_CLICKED(IDC_BUTTON_STOP_CAP, &CusbCamConsoleDlg::OnBnClickedButtonStopCap)
	ON_BN_CLICKED(IDC_BUTTON_VEDIO_CAP, &CusbCamConsoleDlg::OnBnClickedButtonVedioCap)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_RESOLUTION, &CusbCamConsoleDlg::OnCbnSelchangeComboResolution)
END_MESSAGE_MAP()


// CusbCamConsoleDlg ��Ϣ�������

BOOL CusbCamConsoleDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	m_comboSensor.InsertString(0,"AR013x");
	m_comboSensor.InsertString(1,"MT9V034");


	m_comboResolution.InsertString(0,"1280x720");
	m_comboResolution.InsertString(1,"1280x960");
	m_comboResolution.InsertString(2,"640x480BIN");
	m_comboResolution.InsertString(3,"640x480SKIP");
	m_comboResolution.InsertString(4,"752x480");

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CusbCamConsoleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CusbCamConsoleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CusbCamConsoleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CusbCamConsoleDlg::OnCbnSelchangeComboSensorSupported()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//sensorInUse->SelectSensor();
	CString str;
	m_comboSensor.GetLBText(m_comboSensor.GetCurSel(),str);
	std::string sensorName=str.GetBuffer(0);
	m_sensorInUse->SelectSensor(&sensorName);
}


void CusbCamConsoleDlg::OnBnClickedButtonOpenUsb()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_sensorInUse->OpenUSB(0)<0)
	{
		SetDlgItemText(IDC_STATIC_STATUS, "��USBʧ�ܡ�");
		return;
	}
	SetDlgItemText(IDC_STATIC_STATUS, "��USB�ɹ���");
	m_bUsbOpen = true;
}


void CusbCamConsoleDlg::OnBnClickedButtonInitSensor()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, "USBδ�򿪡�");
		return;
	}
	m_sensorInUse->InitSensor();
	SetDlgItemText(IDC_STATIC_STATUS, "��ʼ��sensor�ɹ���");
}


void CusbCamConsoleDlg::OnBnClickedButtonCloseUsb()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, "USBδ�򿪻��Ѿ��رա�");
		return;
	}
	OnBnClickedButtonStopCap();
	m_sensorInUse->CloseUSB();
	m_bUsbOpen = false;
}


void CusbCamConsoleDlg::OnBnClickedButtonStopCap()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CusbCamConsoleDlg::OnBnClickedButtonVedioCap()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
		/************************************************************/
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, "USBδ�򿪡�");
		return;
	}
	if (true == m_bIsCapturing)
		return;
#if 0
	GetDlgItem(IDC_RADIO1)->EnableWindow(false);
	GetDlgItem(IDC_RADIO2)->EnableWindow(false);
	GetDlgItem(IDC_RADIO3)->EnableWindow(false);
	GetDlgItem(IDC_RADIO4)->EnableWindow(false);

	GetDlgItem(IDC_RADIO_8BIT_DEPTH)->EnableWindow(false);
	GetDlgItem(IDC_RADIO_16BIT_DEPTH)->EnableWindow(false);
	GetDlgItem(IDC_RADIO5)->EnableWindow(false);

	GetDlgItem(IDC_CHECK_SAVE_VEDIO)->EnableWindow(false);
#endif

	cv::namedWindow("disp");
	HWND hWnd = (HWND)cvGetWindowHandle("disp");//��ȡ�Ӵ��ڵ�HWND
	HWND hParentWnd = ::GetParent(hWnd);//��ȡ������HWND��������������Ҫ�õ�
	 //���ش��ڱ����� 
	long style = GetWindowLong(hParentWnd, GWL_STYLE);
	style &= ~(WS_SYSMENU);
	SetWindowLong(hParentWnd, GWL_STYLE, style);


	if(m_sensorInUse->StartCap(g_iHeight, (g_byteBitDepthNo == 1 ? g_iWidth : g_iWidth * 2), Disp)<0)
	{
		SetDlgItemText(IDC_STATIC_STATUS, "USB�豸��ʧ�ܣ�");
#if 0
		GetDlgItem(IDC_RADIO1)->EnableWindow(true);
		GetDlgItem(IDC_RADIO2)->EnableWindow(true);
		GetDlgItem(IDC_RADIO3)->EnableWindow(true);
		GetDlgItem(IDC_RADIO4)->EnableWindow(true);

		GetDlgItem(IDC_RADIO_8BIT_DEPTH)->EnableWindow(true);
		GetDlgItem(IDC_RADIO_16BIT_DEPTH)->EnableWindow(true);
		GetDlgItem(IDC_RADIO5)->EnableWindow(true);

		GetDlgItem(IDC_CHECK_SAVE_VEDIO)->EnableWindow(true);
#endif
		return;
	}
	/************************************************************/

	SetTimer(1, 1000, NULL);

	m_bIsCapturing = true;
	SetDlgItemText(IDC_STATIC_STATUS, "�ɼ���...");
}


void CusbCamConsoleDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	unsigned long iFrameCntPerSec =0;
	unsigned long iByteCntPerSec = 0;

	int i = 0, j = 0;

	CString str;
	switch(nIDEvent)
	{
		case 1:
			{
				WaitForSingleObject(g_mutexTimer, INFINITE);
				m_sensorInUse->GetRecvByteCnt(iByteCntPerSec);
				m_sensorInUse->ClearRecvByteCnt();
				m_sensorInUse->GetRecvFrameCnt(iFrameCntPerSec);
				m_sensorInUse->ClearRecvFrameCnt();
				str.Format("%d Fps     %0.4f MBs", iFrameCntPerSec,float(iByteCntPerSec)/1024.0/1024.0);		 
				HWND hWnd = (HWND)cvGetWindowHandle("disp");//��ȡ�Ӵ��ڵ�HWND
				HWND hParentWnd = ::GetParent(hWnd);//��ȡ������HWND��������������Ҫ�õ�			
				if(hParentWnd !=NULL)
				{
					::SetWindowText(hParentWnd,str);
				}			
				break;
				ReleaseMutex(g_mutexTimer);
			}
		default:
			break;
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CusbCamConsoleDlg::OnCbnSelchangeComboResolution()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!m_bUsbOpen)
	{
		SetDlgItemText(IDC_STATIC_STATUS, "USBδ�򿪡�");
		return;
	}
	//CString str;
	//m_comboSensor.GetLBText(m_comboSensor.GetCurSel(),str);
	int index=m_comboResolution.GetCurSel();
	switch(index)
	{
	case 0:
		if (m_bIsCamSelected)
		{
			m_sensorInUse->SetResolution(RESOLU_1280_720);
			g_byteResolutionType = RESOLU_1280_720;
			g_iWidth=1280;
			g_iHeight=720;
			SetDlgItemText(IDC_STATIC_STATUS, "�ֱ���1280*720��");
		}	
		break;
	case 1:
		if (m_bIsCamSelected)
		{
			m_sensorInUse->SetResolution(RESOLU_1280_960);
			g_byteResolutionType = RESOLU_1280_960;
			g_iWidth=1280;
			g_iHeight=960;
			SetDlgItemText(IDC_STATIC_STATUS, "�ֱ���1280*960��");
		}
		break;
	case 2:
		if (m_bIsCamSelected)
		{
			m_sensorInUse->SetResolution(RESOLU_640_480_BIN);
			g_byteResolutionType = RESOLU_640_480_BIN;
			g_iWidth=640;
			g_iHeight=480;
			SetDlgItemText(IDC_STATIC_STATUS, "�ֱ���640*480��");
		}
		break;
	case 3:
		if (m_bIsCamSelected)
		{
			m_sensorInUse->SetResolution(RESOLU_640_480_SKIP);
			g_byteResolutionType = RESOLU_640_480_SKIP;
			g_iWidth=640;
			g_iHeight=480;
			SetDlgItemText(IDC_STATIC_STATUS, "�ֱ���640*480��");
		}
		break;
	case 4:
		if (m_bIsCamSelected)
		{
			m_sensorInUse->SetResolution(RESOLU_752_480);
			g_byteResolutionType = RESOLU_752_480;
			g_iWidth=752;
			g_iHeight=480;
			SetDlgItemText(IDC_STATIC_STATUS, "�ֱ���752*480��");
		}
		break;
	default:
		break;
	}

}
