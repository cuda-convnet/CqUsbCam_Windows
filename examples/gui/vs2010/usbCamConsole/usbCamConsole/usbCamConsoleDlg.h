
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


// 构造
public:
	CusbCamConsoleDlg(CWnd* pParent = NULL);	// 标准构造函数

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
	afx_msg void OnCbnSelchangeComboSensorSupported();
private:
	CComboBox m_comboSensor;
public:
	afx_msg void OnBnClickedButtonOpenUsb();
	afx_msg void OnBnClickedButtonInitSensor();
	afx_msg void OnBnClickedButtonCloseUsb();
	afx_msg void OnBnClickedButtonStopCap();
	afx_msg void OnBnClickedButtonVedioCap();


	afx_msg void OnTimer(UINT_PTR nIDEvent);
private:
	CComboBox m_comboResolution;
public:
	afx_msg void OnCbnSelchangeComboResolution();
};
