
// usbCamConsoleDlg.h : 头文件
//

#pragma once

#include "CqUsbCam.h"
#include "SensorCapbablity.h"
#include <cv.hpp>
#include <opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "afxwin.h"

// CusbCamConsoleDlg 对话框
class CusbCamConsoleDlg : public CDialogEx
{
private:
	cq_bool_t		m_bUsbOpen;
	cq_bool_t		m_bIsCapturing;
	cq_bool_t		m_bIsCamSelected;
	cq_int64_t      m_lBytePerSecond;
	CCqUsbCam*		m_sensorInUse;
	cq_uint32_t		m_iDevCnt;


// 构造
public:
	CusbCamConsoleDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CusbCamConsoleDlg();

// 对话框数据
	enum { IDD = IDD_USBCAMCONSOLE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

private:

public:
	afx_msg void OnBnClickedButtonOpenUsb();
	afx_msg void OnBnClickedButtonInitSensor();
	afx_msg void OnBnClickedButtonCloseUsb();
	afx_msg void OnBnClickedButtonStopCap();
	afx_msg void OnBnClickedButtonVedioCap();


	afx_msg void OnTimer(UINT_PTR nIDEvent);
private:

public:

	afx_msg void OnBnClickedRadioResolu12801024();
	afx_msg void OnBnClickedRadioTrigmodeAuto();
	afx_msg void OnBnClickedRadioTrigmodeFpga();
	afx_msg void OnBnClickedRadioTrigmodeSoft();
	afx_msg void OnBnClickedRadioTrigmodeExt();
	afx_msg void OnBnClickedButtonWrSensor();
	afx_msg void OnBnClickedButtonRdSensor();
	afx_msg void OnBnClickedButtonWrFpga();
	afx_msg void OnBnClickedButtonRdFpga();
	int str2hex(CString str);
	afx_msg void OnEnChangeEditFpgatrigFreq();
	afx_msg void OnBnClickedButtonSoftTrig();
	afx_msg void OnBnClickedRadioMirrorNormal();
	afx_msg void OnBnClickedRadioMirrorX();
	afx_msg void OnBnClickedRadioMirrorY();
	afx_msg void OnBnClickedRadioMirrorXy();
	afx_msg void setAutoGainExpo();
	afx_msg void OnEnChangeEditGainValue();
	afx_msg void OnEnChangeEditExpoValue();
private:
	CButton cbAutoGain;
public:
	CButton cbAutoExpo;
	afx_msg void OnBnClickedButtonCheckSpeed();
	afx_msg void OnBnClickedButtonWrEeprom();
	afx_msg void OnBnClickedButtonRdEeprom();
	afx_msg void OnBnClickedButtonRdDevId();
	afx_msg void OnBnClickedButtonWrDevId();
	afx_msg void OnBnClickedButtonRdDevSn();
	afx_msg void OnBnClickedButtonWrDevSn();
	afx_msg void OnBnClickedRadioResolu640480();
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD dwData );
	afx_msg void OnClose();
};
