
// MFC_DeltaFinalDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MFC_DeltaFinal.h"
#include "MFC_DeltaFinalDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

Mat CMFC_DeltaFinalDlg::Img_Depth_source;
vector<vector<Point>> contours;
bool CMFC_DeltaFinalDlg::ObjectToWork = false;
int CMFC_DeltaFinalDlg::ObjectCoordinate[4] = { 0 };
Net  CMFC_DeltaFinalDlg::_net;
VideoCapture CMFC_DeltaFinalDlg::cap;
Mat CMFC_DeltaFinalDlg::frame_WebCam;
KinectCapture CMFC_DeltaFinalDlg::kinect;
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

// 程式碼實作
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFC_DeltaFinalDlg 對話方塊



CMFC_DeltaFinalDlg::CMFC_DeltaFinalDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFC_DELTAFINAL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFC_DeltaFinalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ImageDepth, m_ImageDepth);
	DDX_Control(pDX, IDC_Image_WebCam, m_Image_WebCam);
	DDX_Control(pDX, IDC_LIST_SCARA, m_LIST_SCARA);
	DDX_Control(pDX, IDC_CHECK_SuperUSER, m_SuperUSER);
}

BEGIN_MESSAGE_MAP(CMFC_DeltaFinalDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(Btn_KinectOpn, &CMFC_DeltaFinalDlg::OnBnClickedKinectopn)
	ON_BN_CLICKED(Btn_WebOpn, &CMFC_DeltaFinalDlg::OnBnClickedWebopn)
	ON_BN_CLICKED(Btn_coordinateCalibration, &CMFC_DeltaFinalDlg::OnBnClickedcoordinatecalibration)
	ON_BN_CLICKED(Btn_CarConnect, &CMFC_DeltaFinalDlg::OnBnClickedCarconnect)
	ON_BN_CLICKED(Btn_SCARAConnect, &CMFC_DeltaFinalDlg::OnBnClickedScaraconnect)
	ON_BN_CLICKED(IDC_Btn_AllRun, &CMFC_DeltaFinalDlg::OnBnClickedBtnAllrun)
	ON_BN_CLICKED(IDC_Btn_CARGO, &CMFC_DeltaFinalDlg::OnBnClickedBtnCargo)
	ON_BN_CLICKED(IDC_BtnStartRun, &CMFC_DeltaFinalDlg::OnBnClickedBtnstartrun)
END_MESSAGE_MAP()


// CMFC_DeltaFinalDlg 訊息處理常式

BOOL CMFC_DeltaFinalDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 將 [關於...] 功能表加入系統功能表。

	// IDM_ABOUTBOX 必須在系統命令範圍之中。
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

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

	// TODO: 在此加入額外的初始設定
	SetTimer(1,500, NULL);
	m_ImageDepth.SetWindowPos(NULL, 10, 20, 512, 424, SWP_SHOWWINDOW);
	m_Image_WebCam.SetWindowPos(NULL, 730, 20, 640, 480, SWP_SHOWWINDOW);
	LoadSet();
	WORD dwStyle = m_LIST_SCARA.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_LIST_SCARA.SetExtendedStyle(dwStyle);
	m_LIST_SCARA.ModifyStyle(0, LVS_REPORT);
	m_LIST_SCARA.SetExtendedStyle(LVS_EX_GRIDLINES);
	//CRect rec;
	//m_LIST_SCARA.GetWindowRect(rec);
	m_LIST_SCARA.InsertColumn(0, _T("To Work"), LVCFMT_LEFT, 80);
	m_LIST_SCARA.InsertColumn(1, _T("Done"), LVCFMT_LEFT, 80);
	m_LIST_SCARA.InsertColumn(2, _T("X"), LVCFMT_LEFT, 100);
	m_LIST_SCARA.InsertColumn(3, _T("Y"), LVCFMT_LEFT, 100);
	m_LIST_SCARA.InsertColumn(4, _T("Z"), LVCFMT_LEFT, 100);
	m_LIST_SCARA.InsertColumn(5, _T("Theta"), LVCFMT_LEFT, 100);


	mb = modbus_new_tcp("192.168.1.1", 502);

	String modelTxt1 = "tomato_train_val.prototxt";	//記得改成deploy模式，頭前面去掉data層，後面去掉loss和accuracy層  // deep learning	// lenet_pa_OK
	String modelBin1 = "HSI_Tomato_Model_iter_5000.caffemodel";	// trained model	
	_net = dnn::readNetFromCaffe(modelTxt1, modelBin1);

	ToWork = 0;
	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

void CMFC_DeltaFinalDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，
// 以便繪製圖示。對於使用文件/檢視模式的 MFC 應用程式，
// 框架會自動完成此作業。

void CMFC_DeltaFinalDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 當使用者拖曳最小化視窗時，
// 系統呼叫這個功能取得游標顯示。
HCURSOR CMFC_DeltaFinalDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFC_DeltaFinalDlg::OnBnClickedKinectopn()
{
	m_threadPara.m_case = 1;
	m_threadPara.hWnd = m_hWnd;
	m_lpThread = AfxBeginThread(&CMFC_DeltaFinalDlg::threadFun, (LPVOID)&m_threadPara);
}
void CMFC_DeltaFinalDlg::ShowImage(cv::Mat Image, CWnd* pWnd)
{
	//Windows中顯示圖像存在一個4位元組對齊的問題, 也就是每一行的位元組數必須是4的倍數.
	//而Mat的資料是連續存儲的.一般Mat的資料格式為BGR, 也就是一個圖元3個位元組, 假設我的圖片一行有5個圖元, 那一行就是15個位元組, 這不符合MFC的資料對齊方式,
	//如果我們直接把Mat的data加個資料頭再顯示出來就可能會出錯.
	//手動4位元組對齊, 就是計算每行的位元組是不是4的倍數, 不是的話, 在後面補0
	//但是我們把圖片轉成RGBA之後, 一個圖元就是4個位元組, 不管你一行幾個圖元, 一直都是對齊的.

	cv::Mat imgTmp;
	CRect rect;
	pWnd->GetClientRect(&rect);
	cv::resize(Image, imgTmp, cv::Size(rect.Width(), rect.Height()));

	switch (imgTmp.channels())
	{
	case 1:
		cv::cvtColor(imgTmp, imgTmp, CV_GRAY2BGRA);
		break;
	case 3:
		cv::cvtColor(imgTmp, imgTmp, CV_BGR2BGRA);
		break;
	default:
		break;
	}
	int pixelBytes = imgTmp.channels()*(imgTmp.depth() + 1);
	BITMAPINFO bitInfo;
	bitInfo.bmiHeader.biBitCount = 8 * pixelBytes;
	bitInfo.bmiHeader.biWidth = imgTmp.cols;
	bitInfo.bmiHeader.biHeight = -imgTmp.rows;
	bitInfo.bmiHeader.biPlanes = 1;
	bitInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo.bmiHeader.biCompression = BI_RGB;
	bitInfo.bmiHeader.biClrImportant = 0;
	bitInfo.bmiHeader.biClrUsed = 0;
	bitInfo.bmiHeader.biSizeImage = 0;
	bitInfo.bmiHeader.biXPelsPerMeter = 0;
	bitInfo.bmiHeader.biYPelsPerMeter = 0;

	CDC *pDC = pWnd->GetDC();
	::StretchDIBits(
		pDC->GetSafeHdc(),
		0, 0, rect.Width(), rect.Height(),
		0, 0, rect.Width(), rect.Height(),
		imgTmp.data,
		&bitInfo,
		DIB_RGB_COLORS,
		SRCCOPY
	);
	ReleaseDC(pDC);
}
UINT CMFC_DeltaFinalDlg::threadFun(LPVOID LParam)
{
	CthreadParam* para = (CthreadParam*)LParam;
	CMFC_DeltaFinalDlg* lpview = (CMFC_DeltaFinalDlg*)(para->m_lpPara);
	para->m_blthreading = TRUE;

	switch (para->m_case)
	{
	case 0:
		lpview->Thread_Image_Cam(LParam);
		break;
	case 1:
		lpview->Thread_Image_Depth(LParam);
		break;
	default:
		break;
	}

	para->m_blthreading = FALSE;
	para->m_case = 0xFF;
	return 0;

}

void CMFC_DeltaFinalDlg::OnBnClickedWebopn()
{
	cap.open(0);
	m_threadPara.m_case = 0;
	m_threadPara.hWnd = m_hWnd;
	m_lpThread = AfxBeginThread(&CMFC_DeltaFinalDlg::threadFun, (LPVOID)&m_threadPara);
}


void CMFC_DeltaFinalDlg::OnBnClickedcoordinatecalibration()
{
	Mat Img_ROI;

	Img_Depth_source.copyTo(Img_ROI);
	//	SetROI(Img_ROI, Rect(50, 100, 395, 125));
	 /*********/
	Mat tme = Img_ROI(rec);
	int thre_value = cv::threshold(tme, tme, 240, 255, CV_THRESH_OTSU);

	Mat Img_desk_;
	cv::threshold(Img_ROI, Img_ROI, thre_value, 255, CV_THRESH_BINARY_INV);
	SetROI(Img_ROI, rec);
	cv::threshold(Img_Depth_source, Img_desk_, thre_value, 255, CV_THRESH_BINARY);
	SetROI(Img_desk_, rec);
	bitwise_and(Img_desk_, Img_Depth_source, Img_desk_);
	vector<Point>deskDepth;
	vector<double>depth_value;
	double minv = 0.0, maxv = 0.0;
	double* minp = &minv;
	double* maxp = &maxv;
	minMaxIdx(Img_desk_, minp, maxp);
	DeskThreshold = *maxp;
	cv::findNonZero(Img_desk_, deskDepth);
	for (uint i = 0; i < deskDepth.size(); i++)
	{
		Point3d p;
		kinect.Depth2CameraSpace(deskDepth[i], &p);
		depth_value.push_back(p.z*1000);
	}
	sort(depth_value.begin(), depth_value.end());
	//cv::erode(Img_ROI, Img_ROI, Mat());

	vector<vector<cv::Point>> contours_calibrate; // Vector for storing contour
	vector<Vec4i>hierarchy;
	cv::findContours(Img_ROI, contours_calibrate, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image
	for (int i = 0; i < contours_calibrate.size(); i++) // Iterate through each contour
	{
		double Area = contourArea(contours_calibrate[i], false);
		if (Area > 250)
		{
			Mat temp = Mat::zeros(Img_ROI.size(), CV_8UC1);
			drawContours(temp, contours_calibrate, i, Scalar(255), CV_FILLED, 8, hierarchy);
			cv::Moments m = cv::moments(temp, false);
			cv::Point center = cv::Point(m.m10 / m.m00, m.m01 / m.m00);
			cv::Point3d CameraSpace;
			kinect.Depth2CameraSpace(center, &CameraSpace);
			coord_bias.x = ArmXcorrect + CameraSpace.x * 1000;
			coord_bias.y = ArmYcorrect - 1000 * CameraSpace.y;
		}
	}
	if (depth_value.size() % 2 == 0)
		coord_bias.z = 0.5*(depth_value[depth_value.size() / 2] + depth_value[depth_value.size() / 2 - 1]);
	else
		coord_bias.z = depth_value[depth_value.size() / 2];
	fstream fp;
	fp.open("Bias.txt", ios::out);
	fp << coord_bias.x << " " << coord_bias.y << " " << coord_bias.z<<" "<<DeskThreshold;
	fp.close();
}
void CMFC_DeltaFinalDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	switch (nIDEvent)
	{
	case 0:
	{
		uint16_t tab_reg[1] = { 0 };
		CString str;
		if (ObjectToWork)//有東西夾
		{
			//ToWork = 1;
			//modbus_write_register(mb, ArmToWorkAddr, ToWork);
			if (modbus_read_registers(mb, ArmWorkDoneAddr, 1, tab_reg))//讀取是否成功
			{
				str.Format(_T("%d"), ToWork);
				m_LIST_SCARA.InsertItem(0, str);
				str.Format(_T("%d"), tab_reg[0]);
				m_LIST_SCARA.SetItemText(0, 1, str);
				m_LIST_SCARA.DeleteItem(3);

				switch (tab_reg[0])
				{
				case 1:
				{
					Mat Img_Depth_temp;
					Img_Depth_source.copyTo(Img_Depth_temp);

					Camera2SCARA(Img_Depth_temp);
					Img_Depth_temp.release();
					str.Format(_T("%d"), ObjectCoordinate[0]);
					m_LIST_SCARA.SetItemText(0, 2, str);
					str.Format(_T("%d"), ObjectCoordinate[1]);
					m_LIST_SCARA.SetItemText(0, 3, str);
					str.Format(_T("%d"), ObjectCoordinate[2]);
					m_LIST_SCARA.SetItemText(0, 4, str);
					str.Format(_T("%d"), ObjectCoordinate[3]);
					m_LIST_SCARA.SetItemText(0, 5, str);
					if (ObjectToWork)
					{
						modbus_write_register(mb, ArmXAddr, ObjectCoordinate[0]);
						modbus_write_register(mb, ArmYAddr, ObjectCoordinate[1]);
						modbus_write_register(mb, ArmZAddr, ObjectCoordinate[2]);
						modbus_write_register(mb, ArmThetaAddr, ObjectCoordinate[3]);
						modbus_write_register(mb, ArmSendDoneAddr, 1);
					}
				}
				break;
				case 2:
				{
					Mat frame;
					frame_WebCam.copyTo(frame);
					calcCircles(frame);
					Mat Hsv;
					cvtColor(frame, Hsv, CV_BGR2HSV);
					int ans = Caffe(Hsv, _net);
					modbus_write_register(mb, ArmSendDoneAddr, ans);
					frame.release();
					modbus_write_register(mb, ArmSendDoneAddr, 2);

				}
				break;
				default:
					break;
				}

			}
		}
		else
		{
			ToWork = 0;
			modbus_write_register(mb, ArmToWorkAddr, ToWork);
			char buffer[10] = { 0 };
			buffer[0] = 0x30;
			buffer[1] = 0x41;
			buffer[2] = 0x98;
			Serial_Car_RF.SendData(buffer, sizeof(buffer));

			CString str;
			str.Format(_T("Send buffer %d", buffer[2]));
			SetDlgItemText(IDC_CAR_Statement, str);
			
		}
	}
	case 1:
	{
		
		
		if (m_SuperUSER.GetCheck())
		{
			GetDlgItem(Btn_coordinateCalibration)->ShowWindow(SW_RESTORE);
			GetDlgItem(IDC_Btn_CARGO)->ShowWindow(SW_RESTORE);
		}
		else
		{
			GetDlgItem(Btn_coordinateCalibration)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_Btn_CARGO)->ShowWindow(SW_HIDE);
		}
	}
	break;
	break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CMFC_DeltaFinalDlg::OnBnClickedCarconnect()
{
	Serial_Car_RF.Open(3, 57600);
}
void CMFC_DeltaFinalDlg::Thread_Image_Depth(LPVOID lParam)
{
	CthreadParam * Thread_Info = (CthreadParam *)lParam;
	CMFC_DeltaFinalDlg * hWnd = (CMFC_DeltaFinalDlg *)CWnd::FromHandle((HWND)Thread_Info->hWnd);
	kinect.Open(0, 1, 0);

	while (1)
	{
		kinect.DepthImage(Img_Depth_source);

		cv::Point3d temp;
		if (!Img_Depth_source.empty())
		{
			hWnd->ShowImage(Img_Depth_source, hWnd->GetDlgItem(IDC_ImageDepth));
		}
	}

}
void CMFC_DeltaFinalDlg::Thread_Image_Cam(LPVOID lParam)
{
	CthreadParam * Thread_Info = (CthreadParam *)lParam;
	CMFC_DeltaFinalDlg * hWnd = (CMFC_DeltaFinalDlg *)CWnd::FromHandle((HWND)Thread_Info->hWnd);
	Mutex mtx;
	mtx.lock();
	while (1)
	{
		Mat frame_buffer;
		cap >> frame_buffer;
		if (!frame_buffer.empty())
		{
			frame_buffer.copyTo(frame_WebCam);
			hWnd->ShowImage(frame_WebCam, hWnd->GetDlgItem(IDC_Image_WebCam));
		}

	}
	mtx.unlock();
}
void CMFC_DeltaFinalDlg::Camera2SCARA(Mat DepthImage)
{
	//SetROI(DepthImage, Rect(50, 100, 395, 125));

	Mat temp= DepthImage(rec);

	threshold(DepthImage, DepthImage,(DeskThreshold-5), 255, CV_THRESH_BINARY_INV);
	SetROI(DepthImage, rec);
	cv::erode(DepthImage, DepthImage, Mat());
	vector<vector<cv::Point>> contours_findobject; // Vector for storing contour
	vector<Vec4i>hierarchy;
	cv::Point center;
	findContours(DepthImage, contours_findobject, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image
	if (contours_findobject.size() == 0)
	{
		ObjectToWork = false;
		return;
	}
	for (int i = 0; i < contours_findobject.size(); i++) // Iterate through each contour
	{
		double Area = contourArea(contours_findobject[i], false);
		if (Area > 250)/*如果面積是250以下判斷為路邊雜訊*/
		{
			Mat temp = Mat::zeros(DepthImage.size(), CV_8UC1);
			drawContours(temp, contours_findobject, i, Scalar(255), CV_FILLED, 8, hierarchy);
			cv::Moments m = cv::moments(temp, false);
			center = cv::Point(m.m10 / m.m00, m.m01 / m.m00);
			break;
		}
	}

	cv::Point3d CameraSpace;
	kinect.Depth2CameraSpace(center, &CameraSpace);


	ObjectCoordinate[0] = -1000 * CameraSpace.x + coord_bias.x;
	ObjectCoordinate[1] = 1000 * CameraSpace.y + coord_bias.y;
	ObjectCoordinate[2] = -197 + coord_bias.z - CameraSpace.z*1000;
	ObjectCoordinate[3] = 0;
}

int CMFC_DeltaFinalDlg::Caffe(Mat & input, Net & net1)
{
	int a = 0;
	Rect bounding_rect;
	for (int i = 0; i < contours.size(); i++) // Iterate through each contour
	{
		bounding_rect = boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
	}

	Mat img = input(bounding_rect);
	cvtColor(img, img, CV_BGR2HSV);

	Mat img_r;
	resize(img, img_r, cv::Size(32, 32));
	Mat inputBlob = blobFromImage(img_r, 1, Size(32, 32),
		Scalar(33, 148, 160), false);
	Mat prob;
	cv::TickMeter t;
	for (int i = 0; i < 1; i++)
	{
		CV_TRACE_REGION("forward");
		net1.setInput(inputBlob, "data");        //set the network input
		t.start();
		prob = net1.forward("prob");                          //compute output
		t.stop();
	}
	print(prob);
	int classId;
	double classProb;
	getMaxClass(prob, &classId, &classProb);//find the best class
	return classId;

	a++;
	img.release();
}

void CMFC_DeltaFinalDlg::getMaxClass(const Mat &probBlob, int *classId, double *classProb)
{
	Mat probMat = probBlob.reshape(1, 1); //reshape the blob to 1x1000 matrix
	Point classNumber;
	minMaxLoc(probMat, NULL, classProb, NULL, &classNumber);
	*classId = classNumber.x;
}
void CMFC_DeltaFinalDlg::SetROI(Mat &Img_src, Rect rect_t)
{
	for (uint j = 0; j < Img_src.rows; j++)

	{
		uchar* temp = Img_src.ptr<uchar>(j);

		for (uint i = 0; i < Img_src.cols; i++)
		{
			if (i < rect_t.x || (i > (rect_t.x + rect_t.width)) || j<rect_t.y || j>(rect_t.y + rect_t.height))
				temp[i] = 0;
		}
	}
}
void CMFC_DeltaFinalDlg::calcCircles(const Mat &input)
{
	Mat gray(480, 640, CV_8U);
	cvtColor(input, gray, cv::COLOR_RGB2GRAY);/*轉灰階*/
	SetROI(gray, Rect(171, 19, 340, 216));
	GaussianBlur(gray, gray, cv::Size(3, 3), 0, 0);
	Mat _Canny;
	Canny(gray, _Canny, 50, 100);
	cv::line(_Canny, cv::Point(171, 19), cv::Point(171, 235), cv::Scalar(0));
	cv::line(_Canny, cv::Point(170, 19), cv::Point(170, 235), cv::Scalar(0));
	cv::line(_Canny, cv::Point(172, 19), cv::Point(172, 235), cv::Scalar(0));
	cv::line(_Canny, cv::Point(173, 19), cv::Point(173, 235), cv::Scalar(0));
	cv::line(_Canny, cv::Point(169, 19), cv::Point(169, 235), cv::Scalar(0));

	cv::line(_Canny, cv::Point(511, 19), cv::Point(511, 235), cv::Scalar(0));
	cv::line(_Canny, cv::Point(510, 19), cv::Point(510, 235), cv::Scalar(0));
	cv::line(_Canny, cv::Point(509, 19), cv::Point(509, 235), cv::Scalar(0));
	cv::line(_Canny, cv::Point(512, 19), cv::Point(512, 235), cv::Scalar(0));
	cv::line(_Canny, cv::Point(513, 19), cv::Point(513, 235), cv::Scalar(0));

	cv::line(_Canny, cv::Point(171, 19), cv::Point(511, 19), cv::Scalar(0));
	cv::line(_Canny, cv::Point(171, 18), cv::Point(511, 18), cv::Scalar(0));
	cv::line(_Canny, cv::Point(171, 17), cv::Point(511, 17), cv::Scalar(0));
	cv::line(_Canny, cv::Point(171, 20), cv::Point(511, 20), cv::Scalar(0));
	cv::line(_Canny, cv::Point(171, 21), cv::Point(511, 21), cv::Scalar(0));

	cv::line(_Canny, cv::Point(171, 235), cv::Point(511, 235), cv::Scalar(0));
	cv::line(_Canny, cv::Point(171, 236), cv::Point(511, 236), cv::Scalar(0));
	cv::line(_Canny, cv::Point(171, 237), cv::Point(511, 237), cv::Scalar(0));
	cv::line(_Canny, cv::Point(171, 234), cv::Point(511, 234), cv::Scalar(0));
	cv::line(_Canny, cv::Point(171, 233), cv::Point(511, 233), cv::Scalar(0));

	cv::dilate(_Canny, _Canny, Mat());
	erode(_Canny, _Canny, Mat());

	Mat Boundary = Mat::zeros(_Canny.size(), CV_8UC1);

	vector<Vec4i> hierarchy;

	findContours(_Canny, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	for (int i = 0; i < contours.size(); i++) {
		drawContours(Boundary, contours, i, cv::Scalar(255), 2, 8, hierarchy);
	}

}
void CMFC_DeltaFinalDlg::LoadSet()
{
	fstream fp;
	fp.open("Bias.txt", ios::in);
	fp >> coord_bias.x;
	fp >> coord_bias.y;
	fp >> coord_bias.z;
	fp >> DeskThreshold;
	fp.close();
}

void CMFC_DeltaFinalDlg::OnBnClickedScaraconnect()
{
	int res = modbus_connect(mb);
	modbus_set_slave(mb, 2);

	if (res < 0)
	{
		MessageBox(L"Connect Fail");
		SetDlgItemText(IDC_SCARA_Statement, _T("Connect Fail"));
	}
	else
	{
		SetDlgItemText(IDC_SCARA_Statement, _T("Connect"));
	}
}


void CMFC_DeltaFinalDlg::OnBnClickedBtnAllrun()
{
	CButton* m_button = (CButton*)GetDlgItem(Btn_KinectOpn);
	SendMessage(WM_COMMAND, MAKEWPARAM(Btn_KinectOpn, BN_CLICKED), (LPARAM)m_button->m_hWnd);
	Sleep(50);
    m_button = (CButton*)GetDlgItem(Btn_WebOpn);
	SendMessage(WM_COMMAND, MAKEWPARAM(Btn_WebOpn, BN_CLICKED), (LPARAM)m_button->m_hWnd);
	Sleep(50);
	 m_button = (CButton*)GetDlgItem(Btn_SCARAConnect);
	SendMessage(WM_COMMAND, MAKEWPARAM(Btn_SCARAConnect, BN_CLICKED), (LPARAM)m_button->m_hWnd);
	Sleep(50);
	 m_button = (CButton*)GetDlgItem(Btn_CarConnect);
	SendMessage(WM_COMMAND, MAKEWPARAM(Btn_CarConnect, BN_CLICKED), (LPARAM)m_button->m_hWnd);
	Sleep(50);
	

}




void CMFC_DeltaFinalDlg::OnBnClickedBtnCargo()
{
	/*char buffer[10] = { 0 };
	buffer[0] = 0x30;
	buffer[1] = 0x41;
	buffer[2] = 0x00;
	Serial_Car_RF.SendData(buffer, sizeof(buffer));
	CString str;
	str.Format(_T("Send buffer %d", buffer[2]));
	SetDlgItemText(IDC_CAR_Statement, str);*/
	Mat Img_Depth_temp;
	Img_Depth_source.copyTo(Img_Depth_temp);
	Camera2SCARA(Img_Depth_temp);
	int a = 99;
}


void CMFC_DeltaFinalDlg::OnBnClickedBtnstartrun()
{
	char buffer[10] = { 0 };
	buffer[0] = 0x30;
	buffer[1] = 0x41;
	buffer[2] = 0x62;
	Serial_Car_RF.SendData(buffer, sizeof(buffer));
	CString str;
	str.Format(_T("Send buffer %d", buffer[2]));
	SetDlgItemText(IDC_CAR_Statement, str);
	while (1)
	{
		Serial_Car_RF.ReadData(buffer, 10);
		if (buffer[2] == 0x65)
		{
			str.Format(_T("Get buffer %d",buffer[2]));
			SetDlgItemText(IDC_CAR_Statement, str);
			break;
		}
	}
	ObjectToWork = true;
	ToWork = 1;
	modbus_write_register(mb, ArmToWorkAddr, ToWork);
	SetTimer(0, 1000, NULL);
}
