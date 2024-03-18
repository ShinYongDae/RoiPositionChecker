
// RoiPositionCheckerDlg.cpp : 구현 파일
//

#include "stdafx.h"

#include "RoiPositionChecker.h"
#include "RoiPositionCheckerDlg.h"
#include "afxdialogex.h"

#include <math.h>

#include "IppImage\IppImage.h"
#include "IppImage\IppConvert.h"
#include "IppImage\IppEnhance.h"
#include "IppImage\IppFilter.h"
#include "IppImage\IppGeometry.h"
#include "IppImage\IppFourier.h"
#include "IppImage\IppFeature.h"
#include "IppImage\IppColor.h"
#include "IppImage\IppSegment.h"


#define CONVERT_DIB_TO_BYTEIMAGE(m_Dib, img) \
	IppByteImage img; \
	IppDibToImage(m_Dib, img);

#define CONVERT_DIB_TO_RGBIMAGE(m_Dib, img) \
	IppRgbImage img; \
	IppDibToImage(m_Dib, img);

#define CONVERT_IMAGE_TO_DIB(img, dib) \
	IppDib dib; \
	IppImageToDib(img, dib);


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CRoiPositionCheckerDlg 대화 상자



CRoiPositionCheckerDlg::CRoiPositionCheckerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ROIPOSITIONCHECKER_DIALOG, pParent)
	, m_sOrgX(_T("8351"))
	, m_sOrgY(_T("92"))
	, m_sWidth(_T("7"))
	, m_sHeight(_T("10"))
	, m_sMaxPixel(_T("10"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_sPathCurrBmp = _T("");
	m_sPathPrevBmpFolder = _T("");
	m_sNamePrevBmpFile = _T("");
}

void CRoiPositionCheckerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_sOrgX);
	DDX_Text(pDX, IDC_EDIT2, m_sOrgY);
	DDX_Text(pDX, IDC_EDIT3, m_sWidth);
	DDX_Text(pDX, IDC_EDIT4, m_sHeight);
	DDX_Text(pDX, IDC_EDIT5, m_sMaxPixel);
}

BEGIN_MESSAGE_MAP(CRoiPositionCheckerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_START, &CRoiPositionCheckerDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BTN_OPEN, &CRoiPositionCheckerDlg::OnBnClickedBtnOpen)
END_MESSAGE_MAP()


// CRoiPositionCheckerDlg 메시지 처리기

BOOL CRoiPositionCheckerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CRoiPositionCheckerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CRoiPositionCheckerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CRoiPositionCheckerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


CString CRoiPositionCheckerDlg::FileBrowse()
{
	/* Load from file */
	CString FilePath;
	CFileFind finder;
	CString SrchPath, filePos, strTitleMsg, strErrMsg;

	CWaitCursor mCursor;

	CString DirPath[10];
	CString strWorkDir;
	BOOL bResult;

	CString strMcNum, sPath = _T("");
	int nAoiMachineNum = 0;

	// File Open Filter 
	static TCHAR BASED_CODE szFilter[] = _T("BMP Files (*.bmp)|*.bmp;ll Files (*.*)|*.*||");

	// CFileDialog 
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL);

	// Win2k Style FileDialog Box
	dlg.m_ofn.lStructSize = sizeof(OPENFILENAME) + 12; // +12를 Win2k Style로 다이얼로그 박스가 Open됨.

													   // Open Directory
	TCHAR strPrevDir[MAX_PATH];
	DWORD dwLength = GetCurrentDirectory(MAX_PATH, strPrevDir);

	if (!m_sPathPrevBmpFolder.IsEmpty())
	{
		filePos = m_sPathPrevBmpFolder + _T("\\*.*");
		strWorkDir = filePos;
	}
	else
	{
		strWorkDir = strPrevDir;
	}
	dlg.m_ofn.lpstrInitialDir = strWorkDir;

	bResult = 0;

	// Dialog Open
	if (dlg.DoModal() == IDOK)
	{
		sPath = FilePath = dlg.GetPathName();

		CString sFileName;
		int nPos = FilePath.ReverseFind('\\');
		sFileName = FilePath.Right(FilePath.GetLength() - nPos - 1);
		m_sNamePrevBmpFile = sFileName;
		m_sPathPrevBmpFolder = FilePath.Left(nPos);
		return sPath;
	}

	return sPath;
}

void CRoiPositionCheckerDlg::OnBnClickedBtnOpen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString sPath = FileBrowse();

	if (sPath.IsEmpty())
		return;

	GetDlgItem(IDC_STC_PATH)->SetWindowText(sPath);
	m_sPathCurrBmp = sPath;
	GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
}

void CRoiPositionCheckerDlg::OnBnClickedBtnStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//TestBmpCopy(m_sPathCurrBmp);
	//Test(m_sPathCurrBmp);

/*	
	// 이미지 데이터를 저장할 버퍼 생성
	uint8_t* imageData;
	imageData = CropBmp(m_sPathCurrBmp);
	// 메모리 해제
	delete[] imageData;
*/

	GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);

	BMPInfoHeader infoHeader;

	if (!GetBMPInfoHeader(m_sPathCurrBmp, infoHeader))
	{
		GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
		return;
	}

	if (!SetImageRange(infoHeader))
	{
		GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
		return;
	}

	CString sModelName = CropModelName;
	CString sRoiName = CropRoiName;

	uint8_t* ModelData = Crop(m_sPathCurrBmp, stModel, sModelName);
	//uint8_t* RoiData = Crop(m_sPathCurrBmp, stRoi);

	Start(m_sPathCurrBmp);

	// 메모리 해제
	//delete[] RoiData;
	delete[] ModelData;

	GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
}

BOOL CRoiPositionCheckerDlg::Start(CString sPathCurr)
{
	CString sModelName = CropModelName;
	CString sRoiName = CropRoiName;

	CPoint ptResult, ptCenter;
	ptCenter.x = stRoi.nWidth / 2;
	ptCenter.y = stRoi.nHeight / 2;

	CString sFile = _T(""), sPathFolder = _T("");
	int nPos = -1;

	nPos = sPathCurr.ReverseFind(_T('\\'));
	sFile = sPathCurr.Right(sPathCurr.GetLength() - nPos - 1);
	sFile.Trim();
	sPathFolder = sPathCurr.Left(nPos);


	CFileFind cFile;
	BOOL bExist = cFile.FindFile(sPathFolder + _T("\\*.bmp"));
	if (!bExist)
	{
		return FALSE; // bmp파일이 존재하지 않음.
	}

	CString sFileName, sRst;
	CString sFilePath, sDisp=_T("");
	int nTot = 0;

	GetDlgItem(IDC_EDIT_DISPLAY)->SetWindowText(sDisp);

	//IppDib dibTmpl;
	if (!m_dibTmpl.Load(CT2A(sModelName)))
	{
		AfxMessageBox(_T("Model 파일을 불러오지 못했습니다."));
		return FALSE;
	}


	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (cFile.IsDirectory()) continue;

		sFileName = cFile.GetFileName();
		sFilePath = cFile.GetFilePath();
		uint8_t* RoiData = Crop(sFilePath, stRoi, sRoiName);
		// 메모리 해제
		delete[] RoiData;
		Sleep(10);

		// 템플릿 매칭
		if (!TemplateMatching(sModelName, sRoiName, ptResult))
			return FALSE;

/*
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		CPoint pnt;
		CString sMsg;

		////IppDib dibTmpl;
		//if (!m_dibTmpl.Load(CT2A(sModelName)))
		//{
		//	AfxMessageBox(_T("Model 파일을 불러오지 못했습니다."));
		//	return FALSE;
		//}

		//IppDib dibRoi;
		if (!m_Dib.Load(CT2A(sRoiName)))
		{
			AfxMessageBox(_T("Roi 파일을 불러오지 못했습니다."));
			return FALSE;
		}
		//AfxNewBitmap(dibTmpl);

		if (m_Dib.GetWidth() < m_dibTmpl.GetWidth() || m_Dib.GetHeight() < m_dibTmpl.GetHeight())
		{
			AfxMessageBox(_T("템플릿 영상의 크기가 입력 영상보다 큽니다."));
			return FALSE;
		}

		CONVERT_DIB_TO_BYTEIMAGE(m_Dib, img);
		CONVERT_DIB_TO_BYTEIMAGE(m_dibTmpl, imgTmpl);

		IppIntImage imgMap;
		IppPoint dp = IppTemplateMatching(img, imgTmpl, imgMap);
		pnt.x = dp.x;
		pnt.y = dp.y;

#if 0
		{
			IppByteImage imgCvt(img.GetWidth(), img.GetHeight());
			BYTE* pCvt = imgCvt.GetPixels();
			int* pMap = imgMap.GetPixels();

			int max_value = 0;
			for (int i = 0; i < img.GetSize(); i++)
			{
				if (pMap[i] > max_value) max_value = pMap[i];
			}

			if (max_value != 0)
			{
				for (int i = 0; i < img.GetSize(); i++)
				{
					pCvt[i] = pMap[i] * 255 / max_value;
				}
			}

			CONVERT_IMAGE_TO_DIB(imgCvt, dibMap)
				AfxNewBitmap(dibMap);
		}
#endif

#if 0
		{ // 입력 영상에 BOX 그리기
			int tw2 = imgTmpl.GetWidth() / 2;
			int th2 = imgTmpl.GetHeight() / 2;

			int minx = dp.x - tw2;
			int maxx = dp.x + tw2;
			int miny = dp.y - th2;
			int maxy = dp.y + th2;

			BYTE** ptr = img.GetPixels2D();

			for (int j = miny; j < maxy; j++)
				ptr[j][minx] = ptr[j][maxx] = 255;

			for (int i = minx; i < maxx; i++)
				ptr[miny][i] = ptr[maxy][i] = 255;
		}

		CONVERT_IMAGE_TO_DIB(img, dib);

		AfxPrintInfo(_T("[템플릿 매칭] 입력 영상: %s, 템플릿 영상: %s, 검출 좌표: (%d, %d)"),
			GetTitle(), dlg.GetFileName(), dp.x, dp.y);
		AfxNewBitmap(dib);
#endif

		ptResult = pnt;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

		int OffsetX = ptResult.x - ptCenter.x;
		int OffsetY = ptResult.y - ptCenter.y;
		sRst.Format(_T("[템플릿매칭] 입력영상: %s, 흔들린 상대좌표: (%d, %d)\r\n"), sFileName, OffsetX, OffsetY);
		sDisp += sRst;

		nTot++;
	}

	GetDlgItem(IDC_EDIT_DISPLAY)->SetWindowText(sDisp);
	return TRUE;
}

BOOL CRoiPositionCheckerDlg::SetImageRange(BMPInfoHeader bmpInfoHeader)
{
	UpdateData(TRUE);

	int nWidth = bmpInfoHeader.width;
	int nHeight = bmpInfoHeader.height;

	int nOrgX_m = _ttoi(m_sOrgX);
	int nOrgY_m = _ttoi(m_sOrgY);
	int nWidth_m = _ttoi(m_sWidth);
	int nHeight_m = _ttoi(m_sHeight);

	int nOrgX_r = _ttoi(m_sOrgX) - _ttoi(m_sMaxPixel);
	int nOrgY_r = _ttoi(m_sOrgY) - _ttoi(m_sMaxPixel);
	int nWidth_r = _ttoi(m_sWidth) + _ttoi(m_sMaxPixel) * 2;
	int nHeight_r = _ttoi(m_sHeight) + _ttoi(m_sMaxPixel) * 2;

	int nWidth_mLog = int(log2(nWidth_m));
	if (log2(nWidth_m) > nWidth_mLog) nWidth_mLog++;

	int nHeight_mLog = int(log2(nHeight_m));
	if (log2(nHeight_m) > nHeight_mLog) nHeight_mLog++;

	int nWidth_rLog = int(log2(nWidth_r));
	if (log2(nWidth_r) > nWidth_rLog) nWidth_rLog++;

	int nHeight_rLog = int(log2(nHeight_r));
	if (log2(nHeight_r) > nHeight_rLog) nHeight_rLog++;

	int nWidth_mV = pow(2, nWidth_mLog);
	int nHeight_mV = pow(2, nHeight_mLog);

	int nWidth_rV = pow(2, nWidth_rLog);
	int nHeight_rV = pow(2, nHeight_rLog);

	int nWidth_mD = nWidth_mV - nWidth_m;
	int nHeight_mD = nHeight_mV - nHeight_m;

	int nWidth_rD = nWidth_rV - nWidth_r;
	int nHeight_rD = nHeight_rV - nHeight_r;

	stModel.nOrgX = nOrgX_m - nWidth_mD / 2;
	stModel.nOrgY = nOrgY_m - nHeight_mD / 2;
	stModel.nWidth = nWidth_mV;
	stModel.nHeight = nHeight_mV;

	stRoi.nOrgX = nOrgX_r - nWidth_rD / 2;
	stRoi.nOrgY = nOrgY_r - nHeight_rD / 2;
	stRoi.nWidth = nWidth_rV;
	stRoi.nHeight = nHeight_rV;

	CString sMsg;

	if (stModel.nOrgY + stModel.nHeight >= nHeight)
	{
		TRACE(_T("Model 이미지의 Y방향 크기가 원본 이미지의 크기보다 큽니다."));
		sMsg.Format(_T("Model 이미지의 Y방향 크기가 원본 이미지의 크기보다 큽니다."));
		AfxMessageBox(sMsg);
		return FALSE;
	}

	if (stModel.nOrgX + stModel.nWidth >= nWidth)
	{
		TRACE(_T("Model 이미지의 X방향 크기가 원본 이미지의 크기보다 큽니다."));
		sMsg.Format(_T("Model 이미지의 X방향 크기가 원본 이미지의 크기보다 큽니다."));
		AfxMessageBox(sMsg);
		return FALSE;
	}

	if (stRoi.nOrgY + stRoi.nHeight >= nHeight)
	{
		TRACE(_T("ROI의 최대떨림 이미지의 Y방향 크기가 원본 이미지의 크기보다 큽니다."));
		sMsg.Format(_T("ROI의 최대떨림 이미지의 Y방향 크기가 원본 이미지의 크기보다 큽니다."));
		AfxMessageBox(sMsg);
		return FALSE;
	}

	if (stRoi.nOrgX + stRoi.nWidth >= nWidth)
	{
		TRACE(_T("ROI의 최대떨림 이미지의 X방향 크기가 원본 이미지의 크기보다 큽니다."));
		sMsg.Format(_T("ROI의 최대떨림 이미지의 X방향 크기가 원본 이미지의 크기보다 큽니다."));
		AfxMessageBox(sMsg);
		return FALSE;
	}

	return TRUE;

	//stRoi.nOrgX = stRoi.nOrgX - 1;
	//stRoi.nOrgY = stRoi.nOrgY - 2;


	//stModel.nOrgX = _ttoi(m_sOrgX);
	//stModel.nOrgY = _ttoi(m_sOrgY);
	//stModel.nWidth = _ttoi(m_sWidth);
	//stModel.nHeight = _ttoi(m_sHeight);

	//stRoi.nOrgX = _ttoi(m_sOrgX) - _ttoi(m_sMaxPixel);
	//stRoi.nOrgY = _ttoi(m_sOrgY); -_ttoi(m_sMaxPixel);
	//stRoi.nWidth = _ttoi(m_sWidth) + _ttoi(m_sMaxPixel) * 2;
	//stRoi.nHeight = _ttoi(m_sHeight) + _ttoi(m_sMaxPixel) * 2;
}

void CRoiPositionCheckerDlg::Test(CString sPath)
{
	const char* inputFilename = "input.bmp";
	const char* outputFilename = "output.bmp";
	const int cropX = 100; // 잘라낼 영역의 시작 X 좌표
	const int cropY = 100; // 잘라낼 영역의 시작 Y 좌표
	const int cropWidth = 64; // 잘라낼 영역의 너비
	const int cropHeight = 64; // 잘라낼 영역의 높이

								// 입력 파일 열기
	std::ifstream inputFile(inputFilename, std::ios::binary);
	if (!inputFile.is_open()) {
		//std::cerr << "입력 파일을 열 수 없습니다." << std::endl;
		TRACE(_T("입력 파일을 열 수 없습니다."));
		return;
	}

	// BMP 파일 헤더 읽기
	BMPHeader header;
	inputFile.read(reinterpret_cast<char*>(&header), sizeof(header));

	// BMP 이미지 정보 헤더 읽기
	BMPInfoHeader infoHeader;
	inputFile.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

	// 8비트 컬러를 사용하는지 확인
	if (infoHeader.bitCount != 8) {
		std::cerr << "이 코드는 8비트 컬러를 사용하는 BMP 파일에만 적용됩니다." << std::endl;
		TRACE(_T("이 코드는 8비트 컬러를 사용하는 BMP 파일에만 적용됩니다."));
		return;
	}

	// 색상 테이블 읽기
	std::vector<uint32_t> colorTable;
	if (infoHeader.colorsUsed == 0) {
		infoHeader.colorsUsed = 256; // 색상 테이블이 없는 경우 기본값으로 설정
	}
	for (uint32_t i = 0; i < infoHeader.colorsUsed; ++i) {
		uint32_t color;
		inputFile.read(reinterpret_cast<char*>(&color), sizeof(color));
		colorTable.push_back(color);
	}

	// 입력 파일에서 이미지 데이터를 읽어옵니다.
	int rowSize = ((infoHeader.bitCount * infoHeader.width + 31) / 32) * 4; // 행당 바이트 수
	std::vector<uint8_t> imageData(rowSize * infoHeader.height);
	inputFile.seekg(header.dataOffset, std::ios::beg);
	inputFile.read(reinterpret_cast<char*>(imageData.data()), imageData.size());
	inputFile.close();

	// 출력 파일 열기
	std::ofstream outputFile(outputFilename, std::ios::binary);
	if (!outputFile.is_open()) {
		//std::cerr << "출력 파일을 열 수 없습니다." << std::endl;
		TRACE(_T("출력 파일을 열 수 없습니다."));
		return;
	}

	// 새로운 BMP 파일 헤더 작성
	BMPHeader newHeader = header;
	newHeader.fileSize = sizeof(newHeader) + sizeof(infoHeader) + cropWidth * cropHeight * (infoHeader.bitCount / 8);
	newHeader.dataOffset = sizeof(newHeader) + sizeof(infoHeader);
	outputFile.write(reinterpret_cast<char*>(&newHeader), sizeof(newHeader));

	// 새로운 BMP 이미지 정보 헤더 작성
	BMPInfoHeader newInfoHeader = infoHeader;
	newInfoHeader.width = cropWidth;
	newInfoHeader.height = cropHeight;
	newInfoHeader.imageSize = cropWidth * cropHeight * (infoHeader.bitCount / 8);
	outputFile.write(reinterpret_cast<char*>(&newInfoHeader), sizeof(newInfoHeader));

	// 입력 파일에서 색상 테이블을 출력 파일로 복사
	for (const auto& color : colorTable) {
		outputFile.write(reinterpret_cast<const char*>(&color), sizeof(color));
	}

	// 이미지 데이터에서 잘라낼 영역만 추출하여 출력 파일에 쓰기
	for (int y = cropY; y < cropY + cropHeight; ++y) {
		//int offset = (infoHeader.height - 1 - y) * rowSize + cropX;
		int offset = y * rowSize + cropX;
		outputFile.write(reinterpret_cast<char*>(imageData.data() + offset), cropWidth);
	}

	outputFile.close();
	std::cout << "파일이 성공적으로 crop되었습니다." << std::endl;
	TRACE(_T("파일이 성공적으로 crop되었습니다."));
}

//void CRoiPositionCheckerDlg::Test(CString sPath)
//{
//	CString sMsg;
//
//	const char* inputFilename = "input.bmp";
//	const char* outputFilename = "output.bmp";
//	const int cropX = 100; // 잘라낼 영역의 시작 X 좌표
//	const int cropY = 100; // 잘라낼 영역의 시작 Y 좌표
//	const int cropWidth = 64; // 잘라낼 영역의 너비
//	const int cropHeight = 64; // 잘라낼 영역의 높이
//
//								// 입력 파일 열기
//	std::ifstream inputFile(inputFilename, std::ios::binary);
//	if (!inputFile.is_open()) 
//	{
//		//std::cerr << "입력 파일을 열 수 없습니다." << std::endl;
//		TRACE(_T("입력 파일을 열 수 없습니다."));
//		return;
//	}
//
//	// BMP 파일 헤더 읽기
//	BMPHeader header;
//	inputFile.read(reinterpret_cast<char*>(&header), sizeof(header));
//
//	// BMP 이미지 정보 헤더 읽기
//	BMPInfoHeader infoHeader;
//	inputFile.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
//
//	// 8비트 컬러를 사용하는지 확인
//	// 색상 테이블 읽기
//	std::vector<uint32_t> colorTable;
//
//	if (infoHeader.bitCount == 8) 
//	{
//		if (infoHeader.colorsUsed == 0) 
//		{
//			infoHeader.colorsUsed = 256; // 색상 테이블이 없는 경우 기본값으로 설정
//		}
//
//		for (uint32_t i = 0; i < infoHeader.colorsUsed; ++i) 
//		{
//			uint32_t color;
//			inputFile.read(reinterpret_cast<char*>(&color), sizeof(color));
//			colorTable.push_back(color);
//		}
//	}
//
//	// 입력 파일에서 이미지 데이터를 읽어옵니다.
//	int rowSize = ((infoHeader.bitCount * infoHeader.width + 31) / 32) * 4; // 행당 바이트 수
//	std::vector<char> imageData(rowSize * infoHeader.height);
//	inputFile.seekg(header.dataOffset, std::ios::beg);
//	inputFile.read(imageData.data(), imageData.size());
//	inputFile.close();
//
//	// 출력 파일 열기
//	std::ofstream outputFile(outputFilename, std::ios::binary);
//	if (!outputFile.is_open()) 
//	{
//		//std::cerr << "출력 파일을 열 수 없습니다." << std::endl;
//		TRACE(_T("출력 파일을 열 수 없습니다."));
//		return;
//	}
//
//	// 새로운 BMP 파일 헤더 작성
//	BMPHeader newHeader = header;
//	newHeader.fileSize = sizeof(newHeader) + sizeof(infoHeader) + cropWidth * cropHeight * (infoHeader.bitCount / 8);
//	newHeader.dataOffset = sizeof(newHeader) + sizeof(infoHeader);
//	//outputFile.write(reinterpret_cast<char*>(&newHeader), sizeof(newHeader));
//	outputFile.write(reinterpret_cast<char*>(&header), sizeof(header));
//
//	// 새로운 BMP 이미지 정보 헤더 작성
//	BMPInfoHeader newInfoHeader = infoHeader;
//	newInfoHeader.width = cropWidth;
//	newInfoHeader.height = cropHeight;
//	newInfoHeader.imageSize = cropWidth * cropHeight * (infoHeader.bitCount / 8);
//	//outputFile.write(reinterpret_cast<char*>(&newInfoHeader), sizeof(newInfoHeader));
//	outputFile.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
//
//	// 8비트 컬러를 사용하는지 확인
//	if (infoHeader.bitCount == 8)
//	{
//		// 색상 테이블 출력
//		//std::cout << "색상 테이블:" << std::endl;
//		TRACE(_T("색상 테이블:"));
//
//		for (const auto& color : colorTable) 
//		{
//			uint8_t blue = color & 0xFF;
//			uint8_t green = (color >> 8) & 0xFF;
//			uint8_t red = (color >> 16) & 0xFF;
//			//std::cout << "R: " << static_cast<int>(red) << ", G: " << static_cast<int>(green) << ", B: " << static_cast<int>(blue) << std::endl;
//			sMsg.Format(_T("R: %d, G: %d, B: %d"), static_cast<int>(red), static_cast<int>(green), static_cast<int>(blue));
//			TRACE(sMsg);
//		}
//	}
//
//	// 이미지 데이터에서 잘라낼 영역만 추출하여 출력 파일에 쓰기
///*	for (int y = cropY; y < cropY + cropHeight; ++y) 
//	{
//		int offset = (infoHeader.height - 1 - y) * rowSize + cropX * (infoHeader.bitCount / 8);
//		outputFile.write(imageData.data() + offset, cropWidth * (infoHeader.bitCount / 8));
//	}*/
//	outputFile.write(imageData.data(), imageData.size());
//
//	outputFile.close();
//	//std::cout << "파일이 성공적으로 crop되었습니다." << std::endl;
//	TRACE(_T("파일이 성공적으로 crop되었습니다."));
//}

void CRoiPositionCheckerDlg::TestBmpCopy(CString sPath)
{
	const char* inputFilename = CStringToChar(&sPath);//"input.bmp";
	const char* outputFilename = "output.bmp";

	// 입력 파일 열기
	std::ifstream inputFile(inputFilename, std::ios::binary);
	delete inputFilename;

	if (!inputFile.is_open()) 
	{
		//std::cerr << "입력 파일을 열 수 없습니다." << std::endl;
		TRACE(_T("입력 파일을 열 수 없습니다."));
		return;
	}

	// 입력 파일의 크기 확인
	inputFile.seekg(0, std::ios::end);
	std::streampos fileSize = inputFile.tellg();
	inputFile.seekg(0, std::ios::beg);

	// 입력 파일 내용을 저장할 벡터 생성
	std::vector<char> fileData(fileSize);

	// 파일 내용 읽기
	inputFile.read(fileData.data(), fileSize);

	// 입력 파일 닫기
	inputFile.close();

	// 출력 파일 열기
	std::ofstream outputFile(outputFilename, std::ios::binary);
	if (!outputFile.is_open()) 
	{
		//std::cerr << "출력 파일을 열 수 없습니다." << std::endl;
		TRACE(_T("출력 파일을 열 수 없습니다."));
		return;
	}

	// 파일 내용 쓰기
	outputFile.write(fileData.data(), fileSize);

	// 출력 파일 닫기
	outputFile.close();

	//std::cout << "파일이 성공적으로 복사되었습니다." << std::endl;
	TRACE(_T("파일이 성공적으로 복사되었습니다."));
}

char* CRoiPositionCheckerDlg::TCHARToChar(TCHAR *tszStr) // TCHAR* returned must be deleted... 
{
	char *szStr = NULL;
	int nLength;

	USES_CONVERSION;
	LPWSTR lpWideCharStr = T2W(tszStr);

	nLength = WideCharToMultiByte(CP_ACP, 0, lpWideCharStr, -1, NULL, NULL, NULL, NULL);
	szStr = new char[nLength + 1];
	WideCharToMultiByte(CP_ACP, 0, lpWideCharStr, -1, szStr, nLength, NULL, NULL);

	return szStr;
}

TCHAR* CRoiPositionCheckerDlg::CStringToTCHAR(CString *pStr)
{
	int nLen = pStr->GetLength();
	TCHAR* cSend = new TCHAR[nLen + 1];
	wsprintf(cSend, TEXT("%s"), *pStr);
	return cSend;
}

char* CRoiPositionCheckerDlg::CStringToChar(CString *pStr)
{
	TCHAR* cSend = CStringToTCHAR(pStr);
	char* cSend2 = TCHARToChar(cSend);
	delete cSend;

	return cSend2;
}

uint8_t* CRoiPositionCheckerDlg::LoadBmp(CString sPath, BMPHeader* header, BMPInfoHeader* infoHeader)
{
	CString sMsg;

	const char* filename = CStringToChar(&sPath);

	std::ifstream file(filename, std::ios::binary);
	delete filename;

	if (!file) {
		//std::cerr << "파일을 열 수 없습니다: " << filename << std::endl;
		TRACE(_T("파일을 열 수 없습니다: %s", sPath));
		sMsg.Format(_T("파일을 열 수 없습니다: %s"), sPath);
		AfxMessageBox(sMsg);
		return NULL;
	}

	// BMP 파일 헤더 읽기
	//BMPHeader header;
	file.read(reinterpret_cast<char*>(header), sizeof(*header));

	// BMP 이미지 정보 헤더 읽기
	//BMPInfoHeader infoHeader;
	file.read(reinterpret_cast<char*>(infoHeader), sizeof(*infoHeader));

	// 이미지 데이터를 저장할 버퍼 생성
	uint8_t* imageData = new uint8_t[infoHeader->imageSize];

	// 이미지 데이터 읽기
	file.seekg(header->dataOffset, std::ios::beg);
	file.read(reinterpret_cast<char*>(imageData), infoHeader->imageSize);

	// 이미지 정보 출력
	//std::cout << "이미지 폭: " << infoHeader.width << "px" << std::endl;
	//std::cout << "이미지 높이: " << infoHeader.height << "px" << std::endl;
	//std::cout << "비트 수: " << infoHeader.bitCount << std::endl;
	TRACE(_T("이미지 폭: %d px", infoHeader->width));
	TRACE(_T("이미지 높이: %d px", infoHeader->height));
	TRACE(_T("비트 수: %d", infoHeader->bitCount));

	// 여기에서 이미지를 처리하거나 표시할 수 있습니다.

	// 메모리 해제
	//delete[] imageData;

	return imageData;
}

uint8_t* CRoiPositionCheckerDlg::CropBmp(CString sPath)
{
	CString sMsg;

	const char* inputFilename = CStringToChar(&sPath);
	const char* outputFilename = "CropBmp.bmp";

	const int cropWidth = CropWidth; // 잘라낼 영역의 너비
	const int cropHeight = CropHeight; // 잘라낼 영역의 높이

	std::ifstream inputFile(inputFilename, std::ios::binary);
	std::ofstream outputFile(outputFilename, std::ios::binary);
	delete inputFilename;

	if (!inputFile.is_open() || !outputFile.is_open()) {
		//std::cerr << "파일을 열 수 없습니다." << std::endl;
		TRACE(_T("파일을 열 수 없습니다."));
		sMsg.Format(_T("파일을 열 수 없습니다: %s"), sPath);
		AfxMessageBox(sMsg);
		return NULL;
	}

	// BMP 파일 헤더 읽기
	BMPHeader header;
	inputFile.read(reinterpret_cast<char*>(&header), sizeof(header));

	// BMP 이미지 정보 헤더 읽기
	BMPInfoHeader infoHeader;
	inputFile.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));


	// 8비트 컬러를 사용하는지 확인
	std::vector<uint32_t> colorTable;
	if (infoHeader.bitCount == 8) 
	{
		//std::cerr << "이 코드는 8비트 컬러를 사용하는 BMP 파일에만 적용됩니다." << std::endl;
		TRACE(_T("8비트 컬러를 사용하는 BMP 파일입니다."));

		// 색상 테이블 읽기
		if (infoHeader.colorsUsed == 0) 
		{
			infoHeader.colorsUsed = 256; // 색상 테이블이 없는 경우 기본값으로 설정
		}
		for (uint32_t i = 0; i < infoHeader.colorsUsed; ++i) 
		{
			uint32_t color;
			inputFile.read(reinterpret_cast<char*>(&color), sizeof(color));
			colorTable.push_back(color);
		}
	}


	// 새로운 이미지 정보 헤더 설정
	BMPInfoHeader newInfoHeader = infoHeader;
	newInfoHeader.width = cropWidth;
	newInfoHeader.height = cropHeight;
	newInfoHeader.imageSize = cropWidth * cropHeight * (infoHeader.bitCount / 8);

	// 새로운 파일 헤더 및 이미지 정보 헤더 쓰기
	outputFile.write(reinterpret_cast<char*>(&header), sizeof(header));
	outputFile.write(reinterpret_cast<char*>(&newInfoHeader), sizeof(newInfoHeader));

	// 입력 파일에서 색상 테이블을 출력 파일로 복사
	for (const auto& color : colorTable) 
	{
		outputFile.write(reinterpret_cast<const char*>(&color), sizeof(color));
	}

	// 이미지 데이터를 저장할 버퍼 생성
	uint8_t* imageData = new uint8_t[infoHeader.imageSize + 1];
	inputFile.seekg(header.dataOffset, std::ios::beg);
	inputFile.read(reinterpret_cast<char*>(imageData), infoHeader.imageSize);

	// 잘라낸 이미지 데이터를 저장할 버퍼 생성
	uint8_t* croppedImageData = new uint8_t[newInfoHeader.imageSize + 1];
	for (int y = 0; y < cropHeight; ++y) 
	{
		for (int x = 0; x < cropWidth; ++x)
		{
			//int sourceIndex = ((_ttoi(m_sOrgY) + y) * infoHeader.width + _ttoi(m_sOrgX) + x) * (infoHeader.bitCount / 8); // Y Mirrorring
			int sourceIndex = ((infoHeader.height - _ttoi(m_sOrgY) - 1 - y) * infoHeader.width + _ttoi(m_sOrgX) + x) * (infoHeader.bitCount / 8);
			//int destIndex = (y * cropWidth + x) * (infoHeader.bitCount / 8); // Y Mirrorring
			int destIndex = ((newInfoHeader.height - 1 - y) * cropWidth + x) * (infoHeader.bitCount / 8);
			for (int i = 0; i < infoHeader.bitCount / 8; ++i)
			{
				croppedImageData[destIndex + i] = imageData[sourceIndex + i];
			}
		}
	}

	// 잘라낸 이미지 데이터 쓰기
	outputFile.write(reinterpret_cast<char*>(croppedImageData), newInfoHeader.imageSize);


	// 메모리 해제
	delete[] imageData;
	//delete[] croppedImageData;

	//std::cout << "이미지가 성공적으로 crop되었습니다." << std::endl;
	TRACE(_T("이미지가 성공적으로 crop되었습니다."));
	return croppedImageData;
}

BOOL CRoiPositionCheckerDlg::GetBMPInfoHeader(CString sPath, BMPInfoHeader& infoHeader)
{
	CString sMsg;

	const char* inputFilename = CStringToChar(&sPath);
	std::ifstream inputFile(inputFilename, std::ios::binary);
	delete inputFilename;

	if (!inputFile.is_open())
	{
		//std::cerr << "파일을 열 수 없습니다." << std::endl;
		TRACE(_T("파일을 열 수 없습니다."));
		sMsg.Format(_T("파일을 열 수 없습니다: %s"), sPath);
		AfxMessageBox(sMsg);
		return FALSE;
	}

	// BMP 파일 헤더 읽기
	BMPHeader header;
	inputFile.read(reinterpret_cast<char*>(&header), sizeof(header));

	// BMP 이미지 정보 헤더 읽기
	inputFile.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

	return TRUE;
}

uint8_t* CRoiPositionCheckerDlg::Crop(CString sPath, ImageRange stRng, CString sTarget)
{
	CString sMsg;

	const char* inputFilename = CStringToChar(&sPath);
	const char* outputFilename = CStringToChar(&sTarget);

	const int cropOrgX = stRng.nOrgX; // 잘라낼 영역의 시작위치 X
	const int cropOrgY = stRng.nOrgY; // 잘라낼 영역의 시작위치 Y
	const int cropWidth = stRng.nWidth; // 잘라낼 영역의 너비
	const int cropHeight = stRng.nHeight; // 잘라낼 영역의 높이

	std::ifstream inputFile(inputFilename, std::ios::binary);
	std::ofstream outputFile(outputFilename, std::ios::binary);
	delete inputFilename;
	delete outputFilename;

	if (!inputFile.is_open() || !outputFile.is_open()) {
		//std::cerr << "파일을 열 수 없습니다." << std::endl;
		TRACE(_T("파일을 열 수 없습니다."));
		sMsg.Format(_T("파일을 열 수 없습니다: %s"), sPath);
		AfxMessageBox(sMsg);
		return NULL;
	}

	// BMP 파일 헤더 읽기
	BMPHeader header;
	inputFile.read(reinterpret_cast<char*>(&header), sizeof(header));

	// BMP 이미지 정보 헤더 읽기
	BMPInfoHeader infoHeader;
	inputFile.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));


	// 8비트 컬러를 사용하는지 확인
	std::vector<uint32_t> colorTable;
	if (infoHeader.bitCount == 8)
	{
		//std::cerr << "이 코드는 8비트 컬러를 사용하는 BMP 파일에만 적용됩니다." << std::endl;
		TRACE(_T("8비트 컬러를 사용하는 BMP 파일입니다."));

		// 색상 테이블 읽기
		if (infoHeader.colorsUsed == 0)
		{
			infoHeader.colorsUsed = 256; // 색상 테이블이 없는 경우 기본값으로 설정
		}
		for (uint32_t i = 0; i < infoHeader.colorsUsed; ++i)
		{
			uint32_t color;
			inputFile.read(reinterpret_cast<char*>(&color), sizeof(color));
			colorTable.push_back(color);
		}
	}


	// 새로운 이미지 정보 헤더 설정
	BMPInfoHeader newInfoHeader = infoHeader;
	newInfoHeader.width = cropWidth;
	newInfoHeader.height = cropHeight;
	newInfoHeader.imageSize = cropWidth * cropHeight * (infoHeader.bitCount / 8);

	// 새로운 파일 헤더 및 이미지 정보 헤더 쓰기
	outputFile.write(reinterpret_cast<char*>(&header), sizeof(header));
	outputFile.write(reinterpret_cast<char*>(&newInfoHeader), sizeof(newInfoHeader));

	// 입력 파일에서 색상 테이블을 출력 파일로 복사
	for (const auto& color : colorTable)
	{
		outputFile.write(reinterpret_cast<const char*>(&color), sizeof(color));
	}

	// 이미지 데이터를 저장할 버퍼 생성
	uint8_t* imageData = new uint8_t[infoHeader.imageSize + 1];
	inputFile.seekg(header.dataOffset, std::ios::beg);
	inputFile.read(reinterpret_cast<char*>(imageData), infoHeader.imageSize);

	// 잘라낸 이미지 데이터를 저장할 버퍼 생성
	uint8_t* croppedImageData = new uint8_t[newInfoHeader.imageSize + 1];
	for (int y = 0; y < cropHeight; ++y)
	{
		for (int x = 0; x < cropWidth; ++x)
		{
			int sourceIndex = ((infoHeader.height - cropOrgY - 1 - y) * infoHeader.width + cropOrgX + x) * (infoHeader.bitCount / 8);
			int destIndex = ((newInfoHeader.height - 1 - y) * newInfoHeader.width + x) * (infoHeader.bitCount / 8);
			for (int i = 0; i < infoHeader.bitCount / 8; ++i)
			{
				croppedImageData[destIndex + i] = imageData[sourceIndex + i];
			}
		}
	}

	// 잘라낸 이미지 데이터 쓰기
	outputFile.write(reinterpret_cast<char*>(croppedImageData), newInfoHeader.imageSize);


	// 메모리 해제
	delete[] imageData;
	//delete[] croppedImageData;

	//std::cout << "이미지가 성공적으로 crop되었습니다." << std::endl;
	TRACE(_T("이미지가 성공적으로 crop되었습니다."));
	return croppedImageData;
}

BOOL CRoiPositionCheckerDlg::TemplateMatching(CString sModelName, CString sRoiName, CPoint& pnt)
{
	CString sMsg;

	//IppDib dibTmpl;
	//if (!dibTmpl.Load(CT2A(sModelName)))
	//{
	//	AfxMessageBox(_T("Model 파일을 불러오지 못했습니다."));
	//	return FALSE;
	//}

	//IppDib dibRoi;
	if (!m_Dib.Load(CT2A(sRoiName)))
	{
		AfxMessageBox(_T("Roi 파일을 불러오지 못했습니다."));
		return FALSE;
	}
	//AfxNewBitmap(dibTmpl);

	if (m_Dib.GetWidth() < m_dibTmpl.GetWidth() || m_Dib.GetHeight() < m_dibTmpl.GetHeight())
	{
		AfxMessageBox(_T("템플릿 영상의 크기가 입력 영상보다 큽니다."));
		return FALSE;
	}

	CONVERT_DIB_TO_BYTEIMAGE(m_Dib, img);
	CONVERT_DIB_TO_BYTEIMAGE(m_dibTmpl, imgTmpl);
	IppIntImage imgMap;
	IppPoint dp = IppTemplateMatching(img, imgTmpl, imgMap);
	pnt.x = dp.x;
	pnt.y = dp.y;

#if 0
	{
		IppByteImage imgCvt(img.GetWidth(), img.GetHeight());
		BYTE* pCvt = imgCvt.GetPixels();
		int* pMap = imgMap.GetPixels();

		int max_value = 0;
		for (int i = 0; i < img.GetSize(); i++)
		{
			if (pMap[i] > max_value) max_value = pMap[i];
		}

		if (max_value != 0)
		{
			for (int i = 0; i < img.GetSize(); i++)
			{
				pCvt[i] = pMap[i] * 255 / max_value;
			}
		}

		CONVERT_IMAGE_TO_DIB(imgCvt, dibMap)
			AfxNewBitmap(dibMap);
	}
#endif

#if 0
	{ // 입력 영상에 BOX 그리기
		int tw2 = imgTmpl.GetWidth() / 2;
		int th2 = imgTmpl.GetHeight() / 2;

		int minx = dp.x - tw2;
		int maxx = dp.x + tw2;
		int miny = dp.y - th2;
		int maxy = dp.y + th2;

		BYTE** ptr = img.GetPixels2D();

		for (int j = miny; j < maxy; j++)
			ptr[j][minx] = ptr[j][maxx] = 255;

		for (int i = minx; i < maxx; i++)
			ptr[miny][i] = ptr[maxy][i] = 255;
	}

	CONVERT_IMAGE_TO_DIB(img, dib);

	AfxPrintInfo(_T("[템플릿 매칭] 입력 영상: %s, 템플릿 영상: %s, 검출 좌표: (%d, %d)"),
		GetTitle(), dlg.GetFileName(), dp.x, dp.y);
	AfxNewBitmap(dib);
#endif

	sMsg.Format(_T("[템플릿 매칭] 입력 영상: %s, 템플릿 영상: %s, 검출 좌표: (%d, %d)"), sRoiName, sModelName, dp.x, dp.y);
	TRACE(sMsg);
	return TRUE;
}