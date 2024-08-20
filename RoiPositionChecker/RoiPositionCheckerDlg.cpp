
// RoiPositionCheckerDlg.cpp : ���� ����
//

#include "stdafx.h"

#include "RoiPositionChecker.h"
#include "RoiPositionCheckerDlg.h"
#include "afxdialogex.h"

#include <math.h>

#include "IppImage/IppImage.h"
#include "IppImage/IppConvert.h"
#include "IppImage/IppEnhance.h"
#include "IppImage/IppFilter.h"
#include "IppImage/IppGeometry.h"
#include "IppImage/IppFourier.h"
#include "IppImage/IppFeature.h"
#include "IppImage/IppColor.h"
#include "IppImage/IppSegment.h"

#include "lib/tiffio.h"
#pragma comment (lib, "lib/libtiff.lib")
//#pragma comment (lib, "C:\\Workspaces\\VisualStudio\\RoiPositionChecker\\RoiPositionChecker\\lib\\libtiff.lib")


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



// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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


// CRoiPositionCheckerDlg ��ȭ ����



CRoiPositionCheckerDlg::CRoiPositionCheckerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ROIPOSITIONCHECKER_DIALOG, pParent)
	, m_sOrgX(_T("8155"))
	, m_sOrgY(_T("290"))
	, m_sWidth(_T("18"))
	, m_sHeight(_T("18"))
	, m_sMaxPixel(_T("10"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_sPathCurrTif = _T("");
	m_sPathCurrTif = _T("");
	m_sPathPrevTifFolder = _T("");
	m_sNamePrevBmpFile = _T("");
	m_sPathCurrBmp = _T("");
	m_sPathPrevBmpFolder = _T("");
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


// CRoiPositionCheckerDlg �޽��� ó����

BOOL CRoiPositionCheckerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CRoiPositionCheckerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
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
	static TCHAR BASED_CODE szFilter[] = _T("TIF Files (*.tif)|*.tif|BMP Files (*.bmp)|*.bmp|Files (*.*)|*.*||");

	// CFileDialog 
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL);

	// Win2k Style FileDialog Box
	dlg.m_ofn.lStructSize = sizeof(OPENFILENAME) + 12; // +12�� Win2k Style�� ���̾�α� �ڽ��� Open��.

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

		CString sFileName, sExp, sExpLower;
		int nPos0 = FilePath.ReverseFind('\\');
		sFileName = FilePath.Right(FilePath.GetLength() - nPos0 - 1);

		int nPos = sFileName.ReverseFind('.');
		sExp = sFileName.Right(sFileName.GetLength() - nPos - 1);
		sExpLower = sExp.MakeLower();
		if (sExpLower == _T("tif"))
		{
			m_sPathCurrTif = sPath;
			m_sNamePrevTifFile = sFileName;
			m_sPathPrevTifFolder = FilePath.Left(nPos0);

			m_sPathCurrBmp = _T("");
			m_sNamePrevBmpFile = _T("");
			m_sPathPrevBmpFolder = _T("");
		}
		else if (sExpLower == _T("bmp"))
		{
			m_sPathCurrBmp = sPath;
			m_sNamePrevBmpFile = sFileName;
			m_sPathPrevBmpFolder = FilePath.Left(nPos0);

			m_sPathCurrTif = _T("");
			m_sNamePrevTifFile = _T("");
			m_sPathPrevTifFolder = _T("");
		}
		else
		{
			AfxMessageBox(_T("TIF or BMP ������ �����ϼ���."));
			sPath = _T("");
		}
		return sPath;
	}

	return sPath;
}

void CRoiPositionCheckerDlg::OnBnClickedBtnOpen()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString sPath = FileBrowse();

	if (sPath.IsEmpty())
		return;

	GetDlgItem(IDC_STC_PATH)->SetWindowText(sPath);
	//m_sPathCurrBmp = sPath;
	GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
}

void CRoiPositionCheckerDlg::OnBnClickedBtnStart()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (!m_sPathCurrBmp.IsEmpty())
	{
		StartBmp();
	}
	else if (!m_sPathCurrTif.IsEmpty())
	{
		StartTif();
	}
	else
	{
		AfxMessageBox(_T("TIF or BMP ������ �����ϼ���."));
	}


	//TestBmpCopy(m_sPathCurrBmp);
	//Test(m_sPathCurrBmp);

/*
	// �̹��� �����͸� ������ ���� ����
	uint8_t* imageData;
	imageData = CropBmp(m_sPathCurrBmp);
	// �޸� ����
	delete[] imageData;
*/
}

void CRoiPositionCheckerDlg::StartBmp()
{
	GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);

	BMPInfoHeader infoHeader;

	if (!GetBMPInfoHeader(m_sPathCurrBmp, infoHeader))
	{
		GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
		return;
	}

	if (!SetBmpImageRange(infoHeader))
	{
		GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
		return;
	}

	CString sModelName = BmpCropModelName;
	CString sRoiName = BmpCropRoiName;

	uint8_t* ModelData = CropBmp(m_sPathCurrBmp, stModel, sModelName);
	//uint8_t* RoiData = Crop(m_sPathCurrBmp, stRoi);

	StartBmp(m_sPathCurrBmp);

	// �޸� ����
	//delete[] RoiData;
	delete[] ModelData;

	GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
}

BOOL CRoiPositionCheckerDlg::StartBmp(CString sPathCurr)
{
	CString sModelName = BmpCropModelName;
	CString sRoiName = BmpCropRoiName;

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
		return FALSE; // bmp������ �������� ����.
	}

	CString sFileName, sRst;
	CString sFilePath, sDisp=_T("");
	int nTot = 0;

	GetDlgItem(IDC_EDIT_DISPLAY)->SetWindowText(sDisp);

	//IppDib dibTmpl;
	if (!m_dibTmpl.Load(CT2A(sModelName)))
	{
		AfxMessageBox(_T("Model ������ �ҷ����� ���߽��ϴ�."));
		return FALSE;
	}


	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (cFile.IsDirectory()) continue;

		sFileName = cFile.GetFileName();
		sFilePath = cFile.GetFilePath();
		uint8_t* RoiData = CropBmp(sFilePath, stRoi, sRoiName);
		// �޸� ����
		delete[] RoiData;
		Sleep(10);

		// ���ø� ��Ī
		if (!TemplateMatching(sModelName, sRoiName, ptResult))
			return FALSE;

		int OffsetX = ptResult.x - ptCenter.x;
		int OffsetY = ptResult.y - ptCenter.y;
		sRst.Format(_T("[���ø���Ī] �Է¿���: %s, ��鸰 �����ǥ: (%d, %d)\r\n"), sFileName, OffsetX, OffsetY);
		sDisp += sRst;

		nTot++;
	}

	GetDlgItem(IDC_EDIT_DISPLAY)->SetWindowText(sDisp);
	return TRUE;
}

BOOL CRoiPositionCheckerDlg::SetBmpImageRange(BMPInfoHeader bmpInfoHeader)
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
		TRACE(_T("Model �̹����� Y���� ũ�Ⱑ ���� �̹����� ũ�⺸�� Ů�ϴ�."));
		sMsg.Format(_T("Model �̹����� Y���� ũ�Ⱑ ���� �̹����� ũ�⺸�� Ů�ϴ�."));
		AfxMessageBox(sMsg);
		return FALSE;
	}

	if (stModel.nOrgX + stModel.nWidth >= nWidth)
	{
		TRACE(_T("Model �̹����� X���� ũ�Ⱑ ���� �̹����� ũ�⺸�� Ů�ϴ�."));
		sMsg.Format(_T("Model �̹����� X���� ũ�Ⱑ ���� �̹����� ũ�⺸�� Ů�ϴ�."));
		AfxMessageBox(sMsg);
		return FALSE;
	}

	if (stRoi.nOrgY + stRoi.nHeight >= nHeight)
	{
		TRACE(_T("ROI�� �ִ붳�� �̹����� Y���� ũ�Ⱑ ���� �̹����� ũ�⺸�� Ů�ϴ�."));
		sMsg.Format(_T("ROI�� �ִ붳�� �̹����� Y���� ũ�Ⱑ ���� �̹����� ũ�⺸�� Ů�ϴ�."));
		AfxMessageBox(sMsg);
		return FALSE;
	}

	if (stRoi.nOrgX + stRoi.nWidth >= nWidth)
	{
		TRACE(_T("ROI�� �ִ붳�� �̹����� X���� ũ�Ⱑ ���� �̹����� ũ�⺸�� Ů�ϴ�."));
		sMsg.Format(_T("ROI�� �ִ붳�� �̹����� X���� ũ�Ⱑ ���� �̹����� ũ�⺸�� Ů�ϴ�."));
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
	const int cropX = 100; // �߶� ������ ���� X ��ǥ
	const int cropY = 100; // �߶� ������ ���� Y ��ǥ
	const int cropWidth = 64; // �߶� ������ �ʺ�
	const int cropHeight = 64; // �߶� ������ ����

								// �Է� ���� ����
	std::ifstream inputFile(inputFilename, std::ios::binary);
	if (!inputFile.is_open()) {
		//std::cerr << "�Է� ������ �� �� �����ϴ�." << std::endl;
		TRACE(_T("�Է� ������ �� �� �����ϴ�."));
		return;
	}

	// BMP ���� ��� �б�
	BMPHeader header;
	inputFile.read(reinterpret_cast<char*>(&header), sizeof(header));

	// BMP �̹��� ���� ��� �б�
	BMPInfoHeader infoHeader;
	inputFile.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

	// 8��Ʈ �÷��� ����ϴ��� Ȯ��
	if (infoHeader.bitCount != 8) {
		std::cerr << "�� �ڵ�� 8��Ʈ �÷��� ����ϴ� BMP ���Ͽ��� ����˴ϴ�." << std::endl;
		TRACE(_T("�� �ڵ�� 8��Ʈ �÷��� ����ϴ� BMP ���Ͽ��� ����˴ϴ�."));
		return;
	}

	// ���� ���̺� �б�
	std::vector<uint32_t> colorTable;
	if (infoHeader.colorsUsed == 0) {
		infoHeader.colorsUsed = 256; // ���� ���̺��� ���� ��� �⺻������ ����
	}
	for (uint32_t i = 0; i < infoHeader.colorsUsed; ++i) {
		uint32_t color;
		inputFile.read(reinterpret_cast<char*>(&color), sizeof(color));
		colorTable.push_back(color);
	}

	// �Է� ���Ͽ��� �̹��� �����͸� �о�ɴϴ�.
	int rowSize = ((infoHeader.bitCount * infoHeader.width + 31) / 32) * 4; // ��� ����Ʈ ��
	std::vector<uint8_t> imageData(rowSize * infoHeader.height);
	inputFile.seekg(header.dataOffset, std::ios::beg);
	inputFile.read(reinterpret_cast<char*>(imageData.data()), imageData.size());
	inputFile.close();

	// ��� ���� ����
	std::ofstream outputFile(outputFilename, std::ios::binary);
	if (!outputFile.is_open()) {
		//std::cerr << "��� ������ �� �� �����ϴ�." << std::endl;
		TRACE(_T("��� ������ �� �� �����ϴ�."));
		return;
	}

	// ���� �̹��� ũ��
	uint32_t imageSizeOrg = infoHeader.width * infoHeader.height * (infoHeader.bitCount / 8);

	// ���ο� BMP ���� ��� �ۼ�
	BMPHeader newHeader = header;
	newHeader.fileSize = sizeof(newHeader) + sizeof(infoHeader) + cropWidth * cropHeight * (infoHeader.bitCount / 8);
	newHeader.dataOffset = sizeof(newHeader) + sizeof(infoHeader);
	outputFile.write(reinterpret_cast<char*>(&newHeader), sizeof(newHeader));

	// ���ο� BMP �̹��� ���� ��� �ۼ�
	BMPInfoHeader newInfoHeader = infoHeader;
	newInfoHeader.width = cropWidth;
	newInfoHeader.height = cropHeight;
	newInfoHeader.imageSize = cropWidth * cropHeight * (infoHeader.bitCount / 8);
	outputFile.write(reinterpret_cast<char*>(&newInfoHeader), sizeof(newInfoHeader));

	// �Է� ���Ͽ��� ���� ���̺��� ��� ���Ϸ� ����
	for (const auto& color : colorTable) {
		outputFile.write(reinterpret_cast<const char*>(&color), sizeof(color));
	}

	// �̹��� �����Ϳ��� �߶� ������ �����Ͽ� ��� ���Ͽ� ����
	for (int y = cropY; y < cropY + cropHeight; ++y) {
		//int offset = (infoHeader.height - 1 - y) * rowSize + cropX;
		int offset = y * rowSize + cropX;
		outputFile.write(reinterpret_cast<char*>(imageData.data() + offset), cropWidth);
	}

	outputFile.close();
	std::cout << "������ ���������� crop�Ǿ����ϴ�." << std::endl;
	TRACE(_T("������ ���������� crop�Ǿ����ϴ�."));
}

void CRoiPositionCheckerDlg::TestBmpCopy(CString sPath)
{
	const char* inputFilename = CStringToChar(&sPath);//"input.bmp";
	const char* outputFilename = "output.bmp";

	// �Է� ���� ����
	std::ifstream inputFile(inputFilename, std::ios::binary);
	delete inputFilename;

	if (!inputFile.is_open()) 
	{
		//std::cerr << "�Է� ������ �� �� �����ϴ�." << std::endl;
		TRACE(_T("�Է� ������ �� �� �����ϴ�."));
		return;
	}

	// �Է� ������ ũ�� Ȯ��
	inputFile.seekg(0, std::ios::end);
	std::streampos fileSize = inputFile.tellg();
	inputFile.seekg(0, std::ios::beg);

	// �Է� ���� ������ ������ ���� ����
	std::vector<char> fileData(fileSize);

	// ���� ���� �б�
	inputFile.read(fileData.data(), fileSize);

	// �Է� ���� �ݱ�
	inputFile.close();

	// ��� ���� ����
	std::ofstream outputFile(outputFilename, std::ios::binary);
	if (!outputFile.is_open()) 
	{
		//std::cerr << "��� ������ �� �� �����ϴ�." << std::endl;
		TRACE(_T("��� ������ �� �� �����ϴ�."));
		return;
	}

	// ���� ���� ����
	outputFile.write(fileData.data(), fileSize);

	// ��� ���� �ݱ�
	outputFile.close();

	//std::cout << "������ ���������� ����Ǿ����ϴ�." << std::endl;
	TRACE(_T("������ ���������� ����Ǿ����ϴ�."));
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
		//std::cerr << "������ �� �� �����ϴ�: " << filename << std::endl;
		TRACE(_T("������ �� �� �����ϴ�: %s", sPath));
		sMsg.Format(_T("������ �� �� �����ϴ�: %s"), sPath);
		AfxMessageBox(sMsg);
		return NULL;
	}

	// BMP ���� ��� �б�
	//BMPHeader header;
	file.read(reinterpret_cast<char*>(header), sizeof(*header));

	// BMP �̹��� ���� ��� �б�
	//BMPInfoHeader infoHeader;
	file.read(reinterpret_cast<char*>(infoHeader), sizeof(*infoHeader));

	// �̹��� �����͸� ������ ���� ����
	uint8_t* imageData = new uint8_t[infoHeader->imageSize];

	// �̹��� ������ �б�
	file.seekg(header->dataOffset, std::ios::beg);
	file.read(reinterpret_cast<char*>(imageData), infoHeader->imageSize);

	// �̹��� ���� ���
	TRACE(_T("�̹��� ��: %d px", infoHeader->width));
	TRACE(_T("�̹��� ����: %d px", infoHeader->height));
	TRACE(_T("��Ʈ ��: %d", infoHeader->bitCount));

	// ���⿡�� �̹����� ó���ϰų� ǥ���� �� �ֽ��ϴ�.

	// �޸� ����
	//delete[] imageData;

	return imageData;
}

uint8_t* CRoiPositionCheckerDlg::CropBmp(CString sPath)
{
	CString sMsg;

	const char* inputFilename = CStringToChar(&sPath);
	const char* outputFilename = "CropBmp.bmp";

	const int cropWidth = CropWidth; // �߶� ������ �ʺ�
	const int cropHeight = CropHeight; // �߶� ������ ����

	std::ifstream inputFile(inputFilename, std::ios::binary);
	std::ofstream outputFile(outputFilename, std::ios::binary);
	delete inputFilename;
	delete outputFilename;

	if (!inputFile.is_open() || !outputFile.is_open()) {
		//std::cerr << "������ �� �� �����ϴ�." << std::endl;
		TRACE(_T("������ �� �� �����ϴ�."));
		sMsg.Format(_T("������ �� �� �����ϴ�: %s"), sPath);
		AfxMessageBox(sMsg);
		return NULL;
	}

	// BMP ���� ��� �б�
	BMPHeader header;
	inputFile.read(reinterpret_cast<char*>(&header), sizeof(header));

	// BMP �̹��� ���� ��� �б�
	BMPInfoHeader infoHeader;
	inputFile.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));


	// 8��Ʈ �÷��� ����ϴ��� Ȯ��
	std::vector<uint32_t> colorTable;
	if (infoHeader.bitCount == 8) 
	{
		//std::cerr << "�� �ڵ�� 8��Ʈ �÷��� ����ϴ� BMP ���Ͽ��� ����˴ϴ�." << std::endl;
		TRACE(_T("8��Ʈ �÷��� ����ϴ� BMP �����Դϴ�."));

		// ���� ���̺� �б�
		if (infoHeader.colorsUsed == 0) 
		{
			infoHeader.colorsUsed = 256; // ���� ���̺��� ���� ��� �⺻������ ����
		}
		for (uint32_t i = 0; i < infoHeader.colorsUsed; ++i) 
		{
			uint32_t color;
			inputFile.read(reinterpret_cast<char*>(&color), sizeof(color));
			colorTable.push_back(color);
		}
	}


	// ���� �̹��� ũ��
	uint32_t imageSizeOrg = infoHeader.width * infoHeader.height * (infoHeader.bitCount / 8);

	// ���ο� �̹��� ���� ��� ����
	BMPInfoHeader newInfoHeader = infoHeader;
	newInfoHeader.width = cropWidth;
	newInfoHeader.height = cropHeight;
	newInfoHeader.imageSize = cropWidth * cropHeight * (infoHeader.bitCount / 8);

	// ���ο� ���� ��� �� �̹��� ���� ��� ����
	outputFile.write(reinterpret_cast<char*>(&header), sizeof(header));
	outputFile.write(reinterpret_cast<char*>(&newInfoHeader), sizeof(newInfoHeader));

	// �Է� ���Ͽ��� ���� ���̺��� ��� ���Ϸ� ����
	for (const auto& color : colorTable) 
	{
		outputFile.write(reinterpret_cast<const char*>(&color), sizeof(color));
	}

	// �̹��� �����͸� ������ ���� ����
	uint8_t* imageData = new uint8_t[imageSizeOrg + 1];
	inputFile.seekg(header.dataOffset, std::ios::beg);
	inputFile.read(reinterpret_cast<char*>(imageData), imageSizeOrg);

	// �߶� �̹��� �����͸� ������ ���� ����
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

	// �߶� �̹��� ������ ����
	outputFile.write(reinterpret_cast<char*>(croppedImageData), newInfoHeader.imageSize);


	// �޸� ����
	delete[] imageData;
	//delete[] croppedImageData;

	//std::cout << "�̹����� ���������� crop�Ǿ����ϴ�." << std::endl;
	TRACE(_T("�̹����� ���������� crop�Ǿ����ϴ�."));
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
		//std::cerr << "������ �� �� �����ϴ�." << std::endl;
		TRACE(_T("������ �� �� �����ϴ�."));
		sMsg.Format(_T("������ �� �� �����ϴ�: %s"), sPath);
		AfxMessageBox(sMsg);
		return FALSE;
	}

	// BMP ���� ��� �б�
	BMPHeader header;
	inputFile.read(reinterpret_cast<char*>(&header), sizeof(header));

	// BMP �̹��� ���� ��� �б�
	inputFile.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

	return TRUE;
}

uint8_t* CRoiPositionCheckerDlg::CropBmp(CString sPath, ImageRange stRng, CString sTarget)
{
	CString sMsg;

	const char* inputFilename = CStringToChar(&sPath);
	const char* outputFilename = CStringToChar(&sTarget);

	const int cropOrgX = stRng.nOrgX; // �߶� ������ ������ġ X
	const int cropOrgY = stRng.nOrgY; // �߶� ������ ������ġ Y
	const int cropWidth = stRng.nWidth; // �߶� ������ �ʺ�
	const int cropHeight = stRng.nHeight; // �߶� ������ ����

	std::ifstream inputFile(inputFilename, std::ios::binary);
	std::ofstream outputFile(outputFilename, std::ios::binary);
	delete inputFilename;
	delete outputFilename;

	if (!inputFile.is_open() || !outputFile.is_open()) {
		//std::cerr << "������ �� �� �����ϴ�." << std::endl;
		TRACE(_T("������ �� �� �����ϴ�."));
		sMsg.Format(_T("������ �� �� �����ϴ�: %s"), sPath);
		AfxMessageBox(sMsg);
		return NULL;
	}

	// BMP ���� ��� �б�
	BMPHeader header;
	inputFile.read(reinterpret_cast<char*>(&header), sizeof(header));

	// BMP �̹��� ���� ��� �б�
	BMPInfoHeader infoHeader;
	inputFile.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));


	// 8��Ʈ �÷��� ����ϴ��� Ȯ��
	std::vector<uint32_t> colorTable;
	if (infoHeader.bitCount == 8)
	{
		//std::cerr << "�� �ڵ�� 8��Ʈ �÷��� ����ϴ� BMP ���Ͽ��� ����˴ϴ�." << std::endl;
		TRACE(_T("8��Ʈ �÷��� ����ϴ� BMP �����Դϴ�."));

		// ���� ���̺� �б�
		if (infoHeader.colorsUsed == 0)
		{
			infoHeader.colorsUsed = 256; // ���� ���̺��� ���� ��� �⺻������ ����
		}
		for (uint32_t i = 0; i < infoHeader.colorsUsed; ++i)
		{
			uint32_t color;
			inputFile.read(reinterpret_cast<char*>(&color), sizeof(color));
			colorTable.push_back(color);
		}
	}

	// ���� �̹��� ũ��
	uint32_t imageSizeOrg = infoHeader.width * infoHeader.height * (infoHeader.bitCount / 8);

	// ���ο� �̹��� ���� ��� ����
	BMPInfoHeader newInfoHeader = infoHeader;
	newInfoHeader.width = cropWidth;
	newInfoHeader.height = cropHeight;
	newInfoHeader.imageSize = cropWidth * cropHeight * (infoHeader.bitCount / 8);

	// ���ο� ���� ��� �� �̹��� ���� ��� ����
	outputFile.write(reinterpret_cast<char*>(&header), sizeof(header));
	outputFile.write(reinterpret_cast<char*>(&newInfoHeader), sizeof(newInfoHeader));

	// �Է� ���Ͽ��� ���� ���̺��� ��� ���Ϸ� ����
	for (const auto& color : colorTable)
	{
		outputFile.write(reinterpret_cast<const char*>(&color), sizeof(color));
	}

	// �̹��� �����͸� ������ ���� ����
	uint8_t* imageData = new uint8_t[imageSizeOrg + 1];
	inputFile.seekg(header.dataOffset, std::ios::beg);
	inputFile.read(reinterpret_cast<char*>(imageData), imageSizeOrg);

	// �߶� �̹��� �����͸� ������ ���� ����
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

	// �߶� �̹��� ������ ����
	outputFile.write(reinterpret_cast<char*>(croppedImageData), newInfoHeader.imageSize);


	// �޸� ����
	delete[] imageData;
	//delete[] croppedImageData;

	//std::cout << "�̹����� ���������� crop�Ǿ����ϴ�." << std::endl;
	TRACE(_T("�̹����� ���������� crop�Ǿ����ϴ�."));
	return croppedImageData;
}

BOOL CRoiPositionCheckerDlg::TemplateMatching(CString sModelName, CString sRoiName, CPoint& pnt)
{
	CString sMsg;

	//IppDib dibTmpl;
	//if (!dibTmpl.Load(CT2A(sModelName)))
	//{
	//	AfxMessageBox(_T("Model ������ �ҷ����� ���߽��ϴ�."));
	//	return FALSE;
	//}

	//IppDib dibRoi;
	if (!m_Dib.Load(CT2A(sRoiName)))
	{
		AfxMessageBox(_T("Roi ������ �ҷ����� ���߽��ϴ�."));
		return FALSE;
	}
	//AfxNewBitmap(dibTmpl);

	if (m_Dib.GetWidth() < m_dibTmpl.GetWidth() || m_Dib.GetHeight() < m_dibTmpl.GetHeight())
	{
		AfxMessageBox(_T("���ø� ������ ũ�Ⱑ �Է� ���󺸴� Ů�ϴ�."));
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
	{ // �Է� ���� BOX �׸���
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

	AfxPrintInfo(_T("[���ø� ��Ī] �Է� ����: %s, ���ø� ����: %s, ���� ��ǥ: (%d, %d)"),
		GetTitle(), dlg.GetFileName(), dp.x, dp.y);
	AfxNewBitmap(dib);
#endif

	sMsg.Format(_T("[���ø� ��Ī] �Է� ����: %s, ���ø� ����: %s, ���� ��ǥ: (%d, %d)"), sRoiName, sModelName, dp.x, dp.y);
	TRACE(sMsg);
	return TRUE;
}


void CRoiPositionCheckerDlg::StartTif()
{
	GetDlgItem(IDC_BTN_START)->EnableWindow(FALSE);

	TIFFInfoHeader infoHeader;

	if (!GetTifInfoHeader(m_sPathCurrTif, infoHeader))
	{
		GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
		return;
	}

	if (!SetTifImageRange(infoHeader))
	{
		GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
		return;
	}

	CString sModelName = TifCropModelName;
	CString sRoiName = TifCropRoiName;

	uint8_t* ModelData = CropTif(m_sPathCurrTif, stModel, sModelName);
	//uint8_t* RoiData = Crop(m_sPathCurrBmp, stRoi);

	StartTif(m_sPathCurrTif);

	// �޸� ����
	//delete[] RoiData;
	delete[] ModelData;

	GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
}

BOOL CRoiPositionCheckerDlg::StartTif(CString sPathCurr)
{
	CString sModelName = TifCropModelName;
	CString sRoiName = TifCropRoiName;

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
	BOOL bExist = cFile.FindFile(sPathFolder + _T("\\*.tif"));
	if (!bExist)
	{
		return FALSE; // tif������ �������� ����.
	}

	CString sFileName, sRst;
	CString sFilePath, sDisp = _T("");
	int nTot = 0;

	GetDlgItem(IDC_EDIT_DISPLAY)->SetWindowText(sDisp);

	////IppDib dibTmpl;
	//if (!m_dibTmpl.Load(CT2A(sModelName)))
	//{
	//	AfxMessageBox(_T("Model ������ �ҷ����� ���߽��ϴ�."));
	//	return FALSE;
	//}


	while (bExist)
	{
		bExist = cFile.FindNextFile();
		if (cFile.IsDots()) continue;
		if (cFile.IsDirectory()) continue;

		sFileName = cFile.GetFileName();
		sFilePath = cFile.GetFilePath();
		uint8_t* RoiData = CropTif(sFilePath, stRoi, sRoiName);
		// �޸� ����
		delete[] RoiData;
		Sleep(10);

		// ���ø� ��Ī
		if (!TemplateMatchingTif(sModelName, sRoiName, ptResult))
			return FALSE;

		int OffsetX = ptResult.x - ptCenter.x;
		int OffsetY = ptResult.y - ptCenter.y;
		sRst.Format(_T("[���ø���Ī] �Է¿���: %s, ��鸰 �����ǥ: (%d, %d)\r\n"), sFileName, OffsetX, OffsetY);
		sDisp += sRst;

		nTot++;
	}

	GetDlgItem(IDC_EDIT_DISPLAY)->SetWindowText(sDisp);
	return TRUE;
}

uint8_t* CRoiPositionCheckerDlg::CropTif(CString sPath, ImageRange stRng, CString sTarget)
{
	CString sMsg;

	const char* inputFilename = CStringToChar(&sPath);
	const char* outputFilename = CStringToChar(&sTarget);

	const int cropOrgX = stRng.nOrgX; // �߶� ������ ������ġ X
	const int cropOrgY = stRng.nOrgY; // �߶� ������ ������ġ Y
	const int cropWidth = stRng.nWidth; // �߶� ������ �ʺ�
	const int cropHeight = stRng.nHeight; // �߶� ������ ����

	// TIFF ���� ����
	TIFF* tif_i = TIFFOpen(inputFilename, "r");
	delete inputFilename;

	if (!tif_i)
	{
		TRACE(_T("Source ������ �� �� �����ϴ�."));
		sMsg.Format(_T("Source ������ �� �� �����ϴ�: %s"), sPath);
		AfxMessageBox(sMsg);
		return NULL;
	}

	// TIFF ���
	// �̹��� ũ�� �� �Ӽ� ���� (Default Values)
	uint32_t width = 640;								// �̹����� �ʺ�
	uint32_t height = 480;								// �̹����� ����
	uint16_t bitsPerSample = 8;							// �� �ȼ��� ��Ʈ ��
	uint16_t samplesPerPixel = 1;						// ȸ���� �̹���
	uint16_t photoMetric = PHOTOMETRIC_MINISBLACK;		// ȸ���� �̹���
	uint16_t planarConfig = PLANARCONFIG_CONTIG;		// �ȼ� �����Ͱ� ���������� ����

	TIFFGetField(tif_i, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(tif_i, TIFFTAG_IMAGELENGTH, &height);
	TIFFGetField(tif_i, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
	TIFFGetField(tif_i, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
	TIFFGetField(tif_i, TIFFTAG_PHOTOMETRIC, &photoMetric);
	TIFFGetField(tif_i, TIFFTAG_PLANARCONFIG, &planarConfig);

	// ���� �̹��� ũ��
	uint32_t imageSizeOrg = width * height * sizeof(uint8_t);

	uint8_t* imageData_i = new uint8_t[imageSizeOrg];
	for (uint32_t row = 0; row < height; ++row) 
	{
		TIFFReadScanline(tif_i, &imageData_i[row * width], row, 0);
	}

	// TIFF ���� �ݱ�
	TIFFClose(tif_i);


	TIFF* tif_o = TIFFOpen(outputFilename, "w");
	delete outputFilename;

	if (!tif_o)
	{
		TRACE(_T("Target ������ �� �� �����ϴ�."));
		sMsg.Format(_T("Target ������ �� �� �����ϴ�: %s"), sTarget);
		AfxMessageBox(sMsg);
		return NULL;
	}

	// ���ο� �̹��� ���� ��� ����
	TIFFInfoHeader newInfoHeader;
	newInfoHeader.width = cropWidth;
	newInfoHeader.height = cropHeight;
	newInfoHeader.imageSize = cropWidth * cropHeight * sizeof(uint8_t);
	newInfoHeader.bitCount = bitsPerSample;

	// ���ο� ���� ��� �� �̹��� ���� ��� ����
	TIFFSetField(tif_o, TIFFTAG_IMAGEWIDTH, cropWidth);
	TIFFSetField(tif_o, TIFFTAG_IMAGELENGTH, cropHeight);
	TIFFSetField(tif_o, TIFFTAG_BITSPERSAMPLE, bitsPerSample);
	TIFFSetField(tif_o, TIFFTAG_SAMPLESPERPIXEL, samplesPerPixel);
	TIFFSetField(tif_o, TIFFTAG_PHOTOMETRIC, photoMetric);
	TIFFSetField(tif_o, TIFFTAG_PLANARCONFIG, planarConfig);

	// �̹��� �����͸� ������ ���� ����
	uint8_t* imageData_o = new uint8_t[cropWidth * cropHeight * samplesPerPixel];

	// �̹��� ������ �ۼ� (������ ȸ���� ������ ����)
	for (uint32_t y = 0; y < cropHeight; ++y)
	{
		for (uint32_t x = 0; x < cropWidth; ++x)
		{
			int sourceIndex = ((cropOrgY + y) * width + cropOrgX + x);
			int destIndex = (y * cropWidth + x);
			//int sourceIndex = ((height - cropOrgY - 1 - y) * width + cropOrgX + x);
			//int destIndex = ((cropHeight - 1 - y) * cropWidth + x);
			imageData_o[destIndex] = imageData_i[sourceIndex];
		}
	}

	// �߶� �̹��� ������ ����
	for (uint32_t row = 0; row < cropHeight; ++row) 
	{
		TIFFWriteScanline(tif_o, &imageData_o[row * cropWidth], row, 0);
	}

	// TIFF ���� �ݱ�
	TIFFClose(tif_o);

	// �޸� ����
	delete[] imageData_i;
	//delete[] imageData_o;

	//std::cout << "�̹����� ���������� crop�Ǿ����ϴ�." << std::endl;
	TRACE(_T("�̹����� ���������� crop�Ǿ����ϴ�."));
	return imageData_o;
}

BOOL CRoiPositionCheckerDlg::GetTifInfoHeader(CString sPath, TIFFInfoHeader& infoHeader)
{
	CString sMsg;
	const char* inputFilename = CStringToChar(&sPath);

	// TIFF ���� ����
	TIFF* tif_i = TIFFOpen(inputFilename, "r");
	delete inputFilename;

	if (!tif_i)
	{
		TRACE(_T("Source ������ �� �� �����ϴ�."));
		sMsg.Format(_T("Source ������ �� �� �����ϴ�: %s"), sPath);
		AfxMessageBox(sMsg);
		return NULL;
	}

	// TIFF ���
	// �̹��� ũ�� �� �Ӽ� ���� (Default Values)
	uint32_t width = 640;								// �̹����� �ʺ�
	uint32_t height = 480;								// �̹����� ����
	uint16_t bitsPerSample = 8;							// �� �ȼ��� ��Ʈ ��
	uint16_t samplesPerPixel = 1;						// ȸ���� �̹���
	uint16_t photoMetric = PHOTOMETRIC_MINISBLACK;		// ȸ���� �̹���
	uint16_t planarConfig = PLANARCONFIG_CONTIG;		// �ȼ� �����Ͱ� ���������� ����

	TIFFGetField(tif_i, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(tif_i, TIFFTAG_IMAGELENGTH, &height);
	TIFFGetField(tif_i, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
	TIFFGetField(tif_i, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
	TIFFGetField(tif_i, TIFFTAG_PHOTOMETRIC, &photoMetric);
	TIFFGetField(tif_i, TIFFTAG_PLANARCONFIG, &planarConfig);


	// ���ο� �̹��� ���� ��� ����
	infoHeader.width = width;
	infoHeader.height = height;
	infoHeader.imageSize = width * height * sizeof(uint8_t);
	infoHeader.bitCount = bitsPerSample;

	// TIFF ���� �ݱ�
	TIFFClose(tif_i);

	return TRUE;
}

BOOL CRoiPositionCheckerDlg::SetTifImageRange(TIFFInfoHeader bmpInfoHeader)
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
		TRACE(_T("Model �̹����� Y���� ũ�Ⱑ ���� �̹����� ũ�⺸�� Ů�ϴ�."));
		sMsg.Format(_T("Model �̹����� Y���� ũ�Ⱑ ���� �̹����� ũ�⺸�� Ů�ϴ�."));
		AfxMessageBox(sMsg);
		return FALSE;
	}

	if (stModel.nOrgX + stModel.nWidth >= nWidth)
	{
		TRACE(_T("Model �̹����� X���� ũ�Ⱑ ���� �̹����� ũ�⺸�� Ů�ϴ�."));
		sMsg.Format(_T("Model �̹����� X���� ũ�Ⱑ ���� �̹����� ũ�⺸�� Ů�ϴ�."));
		AfxMessageBox(sMsg);
		return FALSE;
	}

	if (stRoi.nOrgY + stRoi.nHeight >= nHeight)
	{
		TRACE(_T("ROI�� �ִ붳�� �̹����� Y���� ũ�Ⱑ ���� �̹����� ũ�⺸�� Ů�ϴ�."));
		sMsg.Format(_T("ROI�� �ִ붳�� �̹����� Y���� ũ�Ⱑ ���� �̹����� ũ�⺸�� Ů�ϴ�."));
		AfxMessageBox(sMsg);
		return FALSE;
	}

	if (stRoi.nOrgX + stRoi.nWidth >= nWidth)
	{
		TRACE(_T("ROI�� �ִ붳�� �̹����� X���� ũ�Ⱑ ���� �̹����� ũ�⺸�� Ů�ϴ�."));
		sMsg.Format(_T("ROI�� �ִ붳�� �̹����� X���� ũ�Ⱑ ���� �̹����� ũ�⺸�� Ů�ϴ�."));
		AfxMessageBox(sMsg);
		return FALSE;
	}

	return TRUE;
}

BOOL CRoiPositionCheckerDlg::TemplateMatchingTif(CString sModelName, CString sRoiName, CPoint& pnt)
{
	CString sMsg;

	//IppDib dibTmpl;
	//if (!dibTmpl.Load(CT2A(sModelName)))
	//{
	//	AfxMessageBox(_T("Model ������ �ҷ����� ���߽��ϴ�."));
	//	return FALSE;
	//}

	//IppDib dibRoi;
	//if (!m_Dib.Load(CT2A(sRoiName)))
	//{
	//	AfxMessageBox(_T("Roi ������ �ҷ����� ���߽��ϴ�."));
	//	return FALSE;
	//}
	//AfxNewBitmap(dibTmpl);


	// TIFF ���� ����
	const char* modelFilename = CStringToChar(&sModelName);
	TIFF* tif_m = TIFFOpen(modelFilename, "r");
	delete modelFilename;

	if (!tif_m)
	{
		TRACE(_T("Model ������ �� �� �����ϴ�."));
		sMsg.Format(_T("Model ������ �� �� �����ϴ�: %s"), sModelName);
		AfxMessageBox(sMsg);
		return NULL;
	}

	// TIFF ���
	// �̹��� ũ�� �� �Ӽ� ���� (Default Values)
	uint32_t width_m = 640;									// �̹����� �ʺ�
	uint32_t height_m = 480;								// �̹����� ����
	uint16_t bitsPerSample_m = 8;							// �� �ȼ��� ��Ʈ ��
	uint16_t samplesPerPixel_m = 1;							// ȸ���� �̹���
	uint16_t photoMetric_m = PHOTOMETRIC_MINISBLACK;		// ȸ���� �̹���
	uint16_t planarConfig_m = PLANARCONFIG_CONTIG;			// �ȼ� �����Ͱ� ���������� ����

	TIFFGetField(tif_m, TIFFTAG_IMAGEWIDTH, &width_m);
	TIFFGetField(tif_m, TIFFTAG_IMAGELENGTH, &height_m);
	TIFFGetField(tif_m, TIFFTAG_BITSPERSAMPLE, &bitsPerSample_m);
	TIFFGetField(tif_m, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel_m);
	TIFFGetField(tif_m, TIFFTAG_PHOTOMETRIC, &photoMetric_m);
	TIFFGetField(tif_m, TIFFTAG_PLANARCONFIG, &planarConfig_m);

	// model �̹��� ũ��
	uint32_t imageSizeModel = width_m * height_m * sizeof(uint8_t);

	uint8_t* imageData_m = new uint8_t[imageSizeModel];
	for (uint32_t row_m = 0; row_m < height_m; ++row_m)
	{
		TIFFReadScanline(tif_m, &imageData_m[row_m * width_m], row_m, 0);
	}
	
	// TIFF ���� �ݱ�
	TIFFClose(tif_m);


	// TIFF ���� ����
	const char* roiFilename = CStringToChar(&sRoiName);
	TIFF* tif_r = TIFFOpen(roiFilename, "r");
	delete roiFilename;

	if (!tif_r)
	{
		TRACE(_T("ROI ������ �� �� �����ϴ�."));
		sMsg.Format(_T("ROI ������ �� �� �����ϴ�: %s"), sRoiName);
		AfxMessageBox(sMsg);
		return NULL;
	}

	// TIFF ���
	// �̹��� ũ�� �� �Ӽ� ���� (Default Values)
	uint32_t width_r = 640;									// �̹����� �ʺ�
	uint32_t height_r = 480;								// �̹����� ����
	uint16_t bitsPerSample_r = 8;							// �� �ȼ��� ��Ʈ ��
	uint16_t samplesPerPixel_r = 1;							// ȸ���� �̹���
	uint16_t photoMetric_r = PHOTOMETRIC_MINISBLACK;		// ȸ���� �̹���
	uint16_t planarConfig_r = PLANARCONFIG_CONTIG;			// �ȼ� �����Ͱ� ���������� ����

	TIFFGetField(tif_r, TIFFTAG_IMAGEWIDTH, &width_r);
	TIFFGetField(tif_r, TIFFTAG_IMAGELENGTH, &height_r);
	TIFFGetField(tif_r, TIFFTAG_BITSPERSAMPLE, &bitsPerSample_r);
	TIFFGetField(tif_r, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel_r);
	TIFFGetField(tif_r, TIFFTAG_PHOTOMETRIC, &photoMetric_r);
	TIFFGetField(tif_r, TIFFTAG_PLANARCONFIG, &planarConfig_r);

	// model �̹��� ũ��
	uint32_t roiSizeModel = width_r * height_r * sizeof(uint8_t);

	uint8_t* imageData_r = new uint8_t[roiSizeModel];
	for (uint32_t row_r = 0; row_r < height_r; ++row_r)
	{
		TIFFReadScanline(tif_r, &imageData_r[row_r * width_r], row_r, 0);
	}

	// TIFF ���� �ݱ�
	TIFFClose(tif_r);


	if (width_r < width_m || height_r < height_m)
	{
		AfxMessageBox(_T("���ø� ������ ũ�Ⱑ �Է� ���󺸴� Ů�ϴ�."));
		return FALSE;
	}

	IppByteImage img, imgTmpl;

	{
		int w = width_m;
		int h = height_m;
		int ws = (w + 3) & ~3;
		BYTE* pDIBits = (BYTE*)imageData_m;

		imgTmpl.CreateImage(w, h);
		BYTE** pixels = imgTmpl.GetPixels2D();

		for (int i = 0; i < h; i++)
		{
			memcpy(pixels[i], &pDIBits[(h - 1 - i) * ws], w);
		}
	}

	{
		int w = width_r;
		int h = height_r;
		int ws = (w + 3) & ~3;
		BYTE* pDIBits = (BYTE*)imageData_r;

		img.CreateImage(w, h);
		BYTE** pixels = img.GetPixels2D();

		for (int i = 0; i < h; i++)
		{
			memcpy(pixels[i], &pDIBits[(h - 1 - i) * ws], w);
		}
	}

	//CONVERT_DIB_TO_BYTEIMAGE(m_Dib, img);
	//CONVERT_DIB_TO_BYTEIMAGE(m_dibTmpl, imgTmpl);
	IppIntImage imgMap;
	IppPoint dp = IppTemplateMatching(img, imgTmpl, imgMap); // (IppByteImage& imgSrc, IppByteImage& imgTmpl, IppIntImage& imgMap)
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
	{ // �Է� ���� BOX �׸���
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

	AfxPrintInfo(_T("[���ø� ��Ī] �Է� ����: %s, ���ø� ����: %s, ���� ��ǥ: (%d, %d)"),
		GetTitle(), dlg.GetFileName(), dp.x, dp.y);
	AfxNewBitmap(dib);
#endif

	sMsg.Format(_T("[���ø� ��Ī] �Է� ����: %s, ���ø� ����: %s, ���� ��ǥ: (%d, %d)"), sRoiName, sModelName, dp.x, dp.y);
	TRACE(sMsg);

	// �޸� ����
	delete[] imageData_m;	// Model
	delete[] imageData_r;	// ROI

	return TRUE;
}
