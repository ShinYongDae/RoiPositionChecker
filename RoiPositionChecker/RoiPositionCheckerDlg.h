
// RoiPositionCheckerDlg.h : ��� ����
//

#pragma once


#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>

#pragma pack(push, 1) // ����ü�� �е��� ���ֱ� ���� ����մϴ�.

#include ".\IppImage\IppDib.h"

// BMP ���� ��� ����ü
struct BMPHeader {
	uint16_t signature;
	uint32_t fileSize;
	uint32_t reserved;
	uint32_t dataOffset;
};

// BMP �̹��� ���� ��� ����ü
struct BMPInfoHeader {
	uint32_t headerSize;
	int32_t width;
	int32_t height;
	uint16_t planes;
	uint16_t bitCount;
	uint32_t compression;
	uint32_t imageSize;
	int32_t xPixelsPerMeter;
	int32_t yPixelsPerMeter;
	uint32_t colorsUsed;
	uint32_t colorsImportant;
};

// TIFF �̹��� ���� ��� ����ü
struct TIFFInfoHeader {
	//uint32_t headerSize;
	int32_t width;
	int32_t height;
	uint16_t planes;
	uint16_t bitCount;
	uint32_t compression;
	uint32_t imageSize;
	int32_t xPixelsPerMeter;
	int32_t yPixelsPerMeter;
	uint32_t colorsUsed;
	uint32_t colorsImportant;
};
#pragma pack(pop) // ������ ����� ��ŷ�� �����մϴ�.


#define CropWidth		32; // ���� �̹������� �߶� ������ �ʺ�
#define CropHeight		32; // ���� �̹������� ������ ����
#define ModelWidth		16; // ���ϸ� ������ �ʺ�
#define ModelHeight		16; // ���ϸ� ������ ����

#define BmpCropModelName	_T("Model.bmp"); // ���ϸ� ���ϸ�
#define BmpCropRoiName		_T("Roi.bmp"); // ���ϸ� ���ϸ�
#define TifCropModelName	_T("Model.tif"); // ���ϸ� ���ϸ�
#define TifCropRoiName		_T("Roi.tif"); // ���ϸ� ���ϸ�


struct ImageRange {
	int nOrgX;
	int nOrgY;
	int nWidth;
	int nHeight;
};

// CRoiPositionCheckerDlg ��ȭ ����
class CRoiPositionCheckerDlg : public CDialogEx
{
	// ��Ʈ�� ��ü
	IppDib m_dibTmpl; // Device Independent Bitmap

	CString m_sPathCurrBmp;
	CString m_sPathPrevBmpFolder, m_sNamePrevBmpFile;
	CString m_sPathCurrTif;
	CString m_sPathPrevTifFolder, m_sNamePrevTifFile;
	ImageRange stRoi, stModel;

	void TestBmpCopy(CString sPath);
	void Test(CString sPath);

	CString FileBrowse();
	uint8_t* LoadBmp(CString sPath, BMPHeader* header = NULL, BMPInfoHeader* infoHeader = NULL);
	uint8_t* CropBmp(CString sPath);

	char* TCHARToChar(TCHAR *tszStr);
	TCHAR* CStringToTCHAR(CString *pStr);
	char* CStringToChar(CString *pStr);

	void StartBmp();
	BOOL StartBmp(CString sPathCurr);
	uint8_t* CropBmp(CString sPath, ImageRange stRng, CString sTarget);
	BOOL GetBMPInfoHeader(CString sPath, BMPInfoHeader& infoHeader);
	BOOL SetBmpImageRange(BMPInfoHeader bmpInfoHeader);

	void StartTif();
	BOOL StartTif(CString sPathCurr);
	uint8_t* CropTif(CString sPath, ImageRange stRng, CString sTarget);
	BOOL GetTifInfoHeader(CString sPath, TIFFInfoHeader& infoHeader);
	BOOL SetTifImageRange(TIFFInfoHeader bmpInfoHeader);

	BOOL TemplateMatching(CString sModelName, CString sRoiName, CPoint& pnt);
	BOOL TemplateMatchingTif(CString sModelName, CString sRoiName, CPoint& pnt);

// �����Դϴ�.
public:
	CRoiPositionCheckerDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ROIPOSITIONCHECKER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// ��Ʈ�� ��ü
	IppDib m_Dib; // Device Independent Bitmap
	//IppDib m_dibTmpl; // Device Independent Bitmap

	// ROI(OrgX)
	CString m_sOrgX;
	// ROI(OrgY)
	CString m_sOrgY;
	// ROI(Width)
	CString m_sWidth;
	// ROI(Height)
	CString m_sHeight;


	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnOpen();
	CString m_sMaxPixel;
};
