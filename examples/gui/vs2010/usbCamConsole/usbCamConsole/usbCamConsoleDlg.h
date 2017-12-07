
// usbCamConsoleDlg.h : ͷ�ļ�
//

#pragma once

#include "CqUsbCam.h"
#include "SensorCapbablity.h"
#include <cv.hpp>
#include <opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "afxwin.h"

// CusbCamConsoleDlg �Ի���
class CusbCamConsoleDlg : public CDialogEx
{
private:
	cq_bool_t		m_bUsbOpen;
	cq_bool_t		m_bIsCapturing;
	cq_bool_t		m_bIsCamSelected;
	cq_int64_t      m_lBytePerSecond;
	CCqUsbCam*		m_sensorInUse;


// ����
public:
	CusbCamConsoleDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_USBCAMCONSOLE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
