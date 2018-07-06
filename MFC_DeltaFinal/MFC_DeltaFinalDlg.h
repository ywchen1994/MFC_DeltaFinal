
// MFC_DeltaFinalDlg.h : 標頭檔
//

#pragma once
#include "afxwin.h"
#include"KinectCapture.h"
#include"modbus.h"
#include <fstream>
#include <algorithm>
#include "afxdialogex.h"
#include <afxsock.h>
#include "afxcmn.h"
#include"Serial.h"
#define ArmToWorkAddr 0x1010
#define ArmWorkDoneAddr 0x1020
#define ArmXAddr 0x1001
#define ArmYAddr 0x1002
#define ArmZAddr 0x1003
#define ArmThetaAddr 0x1004
#define ArmSendDoneAddr 0x1005
#define TomatoAddr 0x1006
#define ArmXcorrect 380
#define ArmYcorrect -250

using namespace cv;
using namespace cv::dnn;

struct CthreadParam
{
public:
	HWND hWnd;
	LPVOID m_lpPara;
	UINT   m_case;
	BOOL m_blthreading;
};

// CMFC_DeltaFinalDlg 對話方塊
class CMFC_DeltaFinalDlg : public CDialogEx
{
// 建構
public:
	CMFC_DeltaFinalDlg(CWnd* pParent = NULL);	// 標準建構函式

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_DELTAFINAL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CthreadParam m_threadPara;
	CWinThread*  m_lpThread;
	CStatic m_ImageDepth;
	afx_msg void OnBnClickedKinectopn();
	static UINT threadFun(LPVOID LParam);
	void ShowImage(cv::Mat Image, CWnd * pWnd);
	modbus_t *mb;
	static KinectCapture kinect;
	void OnTimer(UINT_PTR nIDEvent);
	CStatic m_Image_WebCam;
	afx_msg void OnBnClickedWebopn();
	afx_msg void OnBnClickedcoordinatecalibration();
	afx_msg void OnBnClickedCarconnect();
	void Thread_Image_Depth(LPVOID lParam);
	void Thread_Image_Cam(LPVOID lParam);
	void Camera2SCARA(Mat DepthImage);
	int Caffe(Mat & input, Net & net1);
	void getMaxClass(const Mat & probBlob, int * classId, double * classProb);
	void SetROI(Mat &Img_src, Rect rec);
	void calcCircles(const Mat & input);
	cv::Point3i coord_bias;
	static Mat Img_Depth_source;
	static bool ObjectToWork;
	static int ObjectCoordinate[4];
	static Net _net;
	static Mat frame_WebCam;
	int ToWork;
	CListCtrl m_LIST_SCARA;
	static VideoCapture cap;
	void LoadSet();
	CSerial Serial_Car_RF;
	afx_msg void OnBnClickedScaraconnect();
	afx_msg void OnBnClickedBtnAllrun();
	
	afx_msg void OnBnClickedBtnCargo();
	afx_msg void OnBnClickedBtnstartrun();
	CButton m_SuperUSER;
};
