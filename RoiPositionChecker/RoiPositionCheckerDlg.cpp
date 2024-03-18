
// RoiPositionCheckerDlg.cpp : ���� ����
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
	static TCHAR BASED_CODE szFilter[] = _T("BMP Files (*.bmp)|*.bmp;ll Files (*.*)|*.*||");

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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString sPath = FileBrowse();

	if (sPath.IsEmpty())
		return;

	GetDlgItem(IDC_STC_PATH)->SetWindowText(sPath);
	m_sPathCurrBmp = sPath;
	GetDlgItem(IDC_BTN_START)->EnableWindow(TRUE);
}

void CRoiPositionCheckerDlg::OnBnClickedBtnStart()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//TestBmpCopy(m_sPathCurrBmp);
	//Test(m_sPathCurrBmp);

/*	
	// �̹��� �����͸� ������ ���� ����
	uint8_t* imageData;
	imageData = CropBmp(m_sPathCurrBmp);
	// �޸� ����
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

	// �޸� ����
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
		uint8_t* RoiData = Crop(sFilePath, stRoi, sRoiName);
		// �޸� ����
		delete[] RoiData;
		Sleep(10);

		// ���ø� ��Ī
		if (!TemplateMatching(sModelName, sRoiName, ptResult))
			return FALSE;

/*
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		CPoint pnt;
		CString sMsg;

		////IppDib dibTmpl;
		//if (!m_dibTmpl.Load(CT2A(sModelName)))
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

		ptResult = pnt;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

		int OffsetX = ptResult.x - ptCenter.x;
		int OffsetY = ptResult.y - ptCenter.y;
		sRst.Format(_T("[���ø���Ī] �Է¿���: %s, ��鸰 �����ǥ: (%d, %d)\r\n"), sFileName, OffsetX, OffsetY);
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

//void CRoiPositionCheckerDlg::Test(CString sPath)
//{
//	CString sMsg;
//
//	const char* inputFilename = "input.bmp";
//	const char* outputFilename = "output.bmp";
//	const int cropX = 100; // �߶� ������ ���� X ��ǥ
//	const int cropY = 100; // �߶� ������ ���� Y ��ǥ
//	const int cropWidth = 64; // �߶� ������ �ʺ�
//	const int cropHeight = 64; // �߶� ������ ����
//
//								// �Է� ���� ����
//	std::ifstream inputFile(inputFilename, std::ios::binary);
//	if (!inputFile.is_open()) 
//	{
//		//std::cerr << "�Է� ������ �� �� �����ϴ�." << std::endl;
//		TRACE(_T("�Է� ������ �� �� �����ϴ�."));
//		return;
//	}
//
//	// BMP ���� ��� �б�
//	BMPHeader header;
//	inputFile.read(reinterpret_cast<char*>(&header), sizeof(header));
//
//	// BMP �̹��� ���� ��� �б�
//	BMPInfoHeader infoHeader;
//	inputFile.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
//
//	// 8��Ʈ �÷��� ����ϴ��� Ȯ��
//	// ���� ���̺� �б�
//	std::vector<uint32_t> colorTable;
//
//	if (infoHeader.bitCount == 8) 
//	{
//		if (infoHeader.colorsUsed == 0) 
//		{
//			infoHeader.colorsUsed = 256; // ���� ���̺��� ���� ��� �⺻������ ����
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
//	// �Է� ���Ͽ��� �̹��� �����͸� �о�ɴϴ�.
//	int rowSize = ((infoHeader.bitCount * infoHeader.width + 31) / 32) * 4; // ��� ����Ʈ ��
//	std::vector<char> imageData(rowSize * infoHeader.height);
//	inputFile.seekg(header.dataOffset, std::ios::beg);
//	inputFile.read(imageData.data(), imageData.size());
//	inputFile.close();
//
//	// ��� ���� ����
//	std::ofstream outputFile(outputFilename, std::ios::binary);
//	if (!outputFile.is_open()) 
//	{
//		//std::cerr << "��� ������ �� �� �����ϴ�." << std::endl;
//		TRACE(_T("��� ������ �� �� �����ϴ�."));
//		return;
//	}
//
//	// ���ο� BMP ���� ��� �ۼ�
//	BMPHeader newHeader = header;
//	newHeader.fileSize = sizeof(newHeader) + sizeof(infoHeader) + cropWidth * cropHeight * (infoHeader.bitCount / 8);
//	newHeader.dataOffset = sizeof(newHeader) + sizeof(infoHeader);
//	//outputFile.write(reinterpret_cast<char*>(&newHeader), sizeof(newHeader));
//	outputFile.write(reinterpret_cast<char*>(&header), sizeof(header));
//
//	// ���ο� BMP �̹��� ���� ��� �ۼ�
//	BMPInfoHeader newInfoHeader = infoHeader;
//	newInfoHeader.width = cropWidth;
//	newInfoHeader.height = cropHeight;
//	newInfoHeader.imageSize = cropWidth * cropHeight * (infoHeader.bitCount / 8);
//	//outputFile.write(reinterpret_cast<char*>(&newInfoHeader), sizeof(newInfoHeader));
//	outputFile.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
//
//	// 8��Ʈ �÷��� ����ϴ��� Ȯ��
//	if (infoHeader.bitCount == 8)
//	{
//		// ���� ���̺� ���
//		//std::cout << "���� ���̺�:" << std::endl;
//		TRACE(_T("���� ���̺�:"));
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
//	// �̹��� �����Ϳ��� �߶� ������ �����Ͽ� ��� ���Ͽ� ����
///*	for (int y = cropY; y < cropY + cropHeight; ++y) 
//	{
//		int offset = (infoHeader.height - 1 - y) * rowSize + cropX * (infoHeader.bitCount / 8);
//		outputFile.write(imageData.data() + offset, cropWidth * (infoHeader.bitCount / 8));
//	}*/
//	outputFile.write(imageData.data(), imageData.size());
//
//	outputFile.close();
//	//std::cout << "������ ���������� crop�Ǿ����ϴ�." << std::endl;
//	TRACE(_T("������ ���������� crop�Ǿ����ϴ�."));
//}

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
	//std::cout << "�̹��� ��: " << infoHeader.width << "px" << std::endl;
	//std::cout << "�̹��� ����: " << infoHeader.height << "px" << std::endl;
	//std::cout << "��Ʈ ��: " << infoHeader.bitCount << std::endl;
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
	uint8_t* imageData = new uint8_t[infoHeader.imageSize + 1];
	inputFile.seekg(header.dataOffset, std::ios::beg);
	inputFile.read(reinterpret_cast<char*>(imageData), infoHeader.imageSize);

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

uint8_t* CRoiPositionCheckerDlg::Crop(CString sPath, ImageRange stRng, CString sTarget)
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
	uint8_t* imageData = new uint8_t[infoHeader.imageSize + 1];
	inputFile.seekg(header.dataOffset, std::ios::beg);
	inputFile.read(reinterpret_cast<char*>(imageData), infoHeader.imageSize);

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