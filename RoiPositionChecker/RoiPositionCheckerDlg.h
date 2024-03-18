
// RoiPositionCheckerDlg.h : 헤더 파일
//

#pragma once


#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>

#pragma pack(push, 1) // 구조체의 패딩을 없애기 위해 사용합니다.

#include ".\IppImage\IppDib.h"

// BMP 파일 헤더 구조체
struct BMPHeader {
	uint16_t signature;
	uint32_t fileSize;
	uint32_t reserved;
	uint32_t dataOffset;
};

// BMP 이미지 정보 헤더 구조체
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

#pragma pack(pop) // 이전에 사용한 패킹을 복원합니다.


#define CropWidth		32; // 원본 이미지에서 잘라낼 영역의 너비
#define CropHeight		32; // 원본 이미지에서 영역의 높이
#define ModelWidth		16; // 패턴모델 영역의 너비
#define ModelHeight		16; // 패턴모델 영역의 높이

#define CropModelName	_T("Model.bmp"); // 패턴모델 파일명
#define CropRoiName		_T("Roi.bmp"); // 패턴모델 파일명


struct ImageRange {
	int nOrgX;
	int nOrgY;
	int nWidth;
	int nHeight;
};

// CRoiPositionCheckerDlg 대화 상자
class CRoiPositionCheckerDlg : public CDialogEx
{
	// 비트맵 객체
	IppDib m_dibTmpl; // Device Independent Bitmap

	CString m_sPathCurrBmp;
	CString m_sPathPrevBmpFolder, m_sNamePrevBmpFile;
	ImageRange stRoi, stModel;

	void TestBmpCopy(CString sPath);
	void Test(CString sPath);

	CString FileBrowse();
	uint8_t* LoadBmp(CString sPath, BMPHeader* header = NULL, BMPInfoHeader* infoHeader = NULL);
	uint8_t* CropBmp(CString sPath);

	char* TCHARToChar(TCHAR *tszStr);
	TCHAR* CStringToTCHAR(CString *pStr);
	char* CStringToChar(CString *pStr);

	BOOL GetBMPInfoHeader(CString sPath, BMPInfoHeader& infoHeader);
	BOOL SetImageRange(BMPInfoHeader bmpInfoHeader);
	uint8_t* Crop(CString sPath, ImageRange stRng, CString sTarget);
	BOOL TemplateMatching(CString sModelName, CString sRoiName, CPoint& pnt);
	BOOL Start(CString sPathCurr);

// 생성입니다.
public:
	CRoiPositionCheckerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ROIPOSITIONCHECKER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// 비트맵 객체
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
