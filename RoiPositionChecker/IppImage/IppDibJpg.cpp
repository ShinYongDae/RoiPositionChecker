/******************************************************************************
*
* IppDibJpg.cpp
*
* Copyright (c) 2015~<current> by Sun-Kyoo Hwang <sunkyoo.ippbook@gmail.com>
*
* This source code is included in the book titled "Image Processing
* Programming By Visual C++ (2nd Edition)"
*
*****************************************************************************/

#pragma warning(disable:4099)

#include "stdafx.h"
#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include <setjmp.h>
#include <algorithm>

#include "IppDib.h"
#include "jpeglib.h"
//#pragma comment (lib, "IppImage/jpeg_32.lib")
#pragma comment (lib, "IppImage/jpeg.lib")

struct my_error_mgr {
	struct jpeg_error_mgr pub;	/* "public" fields */

	jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void)
my_error_exit(j_common_ptr cinfo)
{
	my_error_ptr myerr = (my_error_ptr)cinfo->err;
	(*cinfo->err->output_message) (cinfo);
	longjmp(myerr->setjmp_buffer, 1);
}

BOOL IppDib::LoadJPG(const char* filename)
{
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;

	FILE * infile;      // �Է� ����
	JSAMPARRAY buffer;  // ���� ���� ��� ����
	int row_stride;     // ��� ���� ũ��

	BOOL ret = FALSE;

	fopen_s(&infile, filename, "rb"); // �Է� ���� ����
	if (!infile)
		return FALSE;

	// Step 1: JPEG ���� ������ ���� ��ü ���� �� �ʱ�ȭ
	cinfo.err = jpeg_std_error(&jerr.pub); // JPEG ���̺귯�� ���� ó�� ����
	jerr.pub.error_exit = my_error_exit;

	if (setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return FALSE;
	}

	jpeg_create_decompress(&cinfo);

	// Step 2: �Է� ���� ����
	jpeg_stdio_src(&cinfo, infile);

	// Step 3: ���� ��� �б�
	(void)jpeg_read_header(&cinfo, TRUE);

	// Step 4: decompression �Ķ���� ����
	// �⺻ ���� �״�� ����� ���̹Ƿ� �߰� �ڵ� ����.

	// Step 5: JPEG ���� ���� ����
	(void)jpeg_start_decompress(&cinfo);

	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

	// Step 6: ���� ������ JPEG ������ �ȼ� �����͸� �о�ͼ� IppDib �ȼ� �� ����
	// �׷��̽����� ����� Ʈ���÷� ������ �����Ͽ� ó����.

	if (cinfo.out_color_space == JCS_GRAYSCALE)
	{
		// �׷��̽����� ����
		CreateGrayBitmap(cinfo.image_width, cinfo.image_height);
		BYTE* pixels = GetDIBitsAddr();
		int ws = (cinfo.image_width + 3) & ~3;

		while (cinfo.output_scanline < cinfo.output_height) {
			(void)jpeg_read_scanlines(&cinfo, buffer, 1);
			memcpy(&pixels[(cinfo.image_height - cinfo.output_scanline) * ws], buffer[0], row_stride);
		}
	}
	else if (cinfo.out_color_space == JCS_RGB)
	{
		// Ʈ���÷� ����
		CreateRgbBitmap(cinfo.image_width, cinfo.image_height);
		BYTE* pixels = GetDIBitsAddr();
		int ws = (cinfo.image_width * 3 + 3) & ~3;

		while (cinfo.output_scanline < cinfo.output_height) {
			(void)jpeg_read_scanlines(&cinfo, buffer, 1);

			// RGB -> BGR
			for (int i = 0; i < row_stride; i += 3) {
				std::swap(buffer[0][i], buffer[0][i + 2]);
			}

			memcpy(&pixels[(cinfo.image_height - cinfo.output_scanline) * ws], buffer[0], row_stride);
		}
	}

	// Step 7: JPEG ���� ���� ����
	(void)jpeg_finish_decompress(&cinfo);

	// Step 8: JPEG ���� ������ ���� ��ü ����
	jpeg_destroy_decompress(&cinfo);

	fclose(infile);

	return TRUE;
}

BOOL IppDib::SaveJPG(const char* filename)
{
	if (GetBitCount() == 8 || GetBitCount() == 24)
		return FALSE;

	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	FILE * outfile;             // ��� ����
	JSAMPROW row_pointer[1];    // ���� ���� ���� �ּ�
	int row_stride;             // ���� ũ��

								// ��Ʈ�� ��ü�� �׷��̽����� �Ǵ� Ʈ���÷� ������ �ƴϸ� ����
	if (GetBitCount() == 8 || GetBitCount() == 24)
		return FALSE;

	// Step 1: JPEG ������ ���� ��ü ���� �� �ʱ�ȭ
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	// Step 2: ��� ���� ����
	fopen_s(&outfile, filename, "wb");
	if (!outfile)
	{
		jpeg_destroy_compress(&cinfo);
		return FALSE;
	}

	jpeg_stdio_dest(&cinfo, outfile);

	// Step 3: ������ ���� �Ķ���� ����
	cinfo.image_width = GetWidth();
	cinfo.image_height = GetHeight();

	if (GetBitCount() == 8) // �׷��̽����� ����
	{
		cinfo.input_components = 1;
		cinfo.in_color_space = JCS_GRAYSCALE;
	}
	else  // Ʈ���÷� ����
	{
		cinfo.input_components = 3;
		cinfo.in_color_space = JCS_RGB;
	}

	int quality = 95;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

	// Step 4: ���� ����
	jpeg_start_compress(&cinfo, TRUE);

	// Step 5: ���� ������ DIB �ȼ� �����͸� �о�ͼ� JPEG ���Ϸ� ����
	// �׷��̽����� ����� Ʈ���÷� ������ �����Ͽ� ó����.

	if (GetBitCount() == 8) // �׷��̽����� ����
	{
		row_stride = cinfo.image_width;
		BYTE* pixels = GetDIBitsAddr();
		int ws = (cinfo.image_width + 3) & ~3;

		while (cinfo.next_scanline < cinfo.image_height) {
			row_pointer[0] = &pixels[(cinfo.image_height - cinfo.next_scanline - 1)*ws];
			(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
		}
	}
	else // Ʈ���÷� ����
	{
		row_stride = cinfo.image_width * 3;
		BYTE* pixels = GetDIBitsAddr();
		BYTE* buffer = new BYTE[row_stride];
		int ws = (cinfo.image_width * 3 + 3) & ~3;

		while (cinfo.next_scanline < cinfo.image_height) {
			memcpy(buffer, &pixels[(cinfo.image_height - cinfo.next_scanline - 1)*ws], row_stride);

			// BGR -> RGB
			for (int i = 0; i < row_stride; i += 3) {
				std::swap(buffer[i], buffer[i + 2]);
			}

			row_pointer[0] = buffer;
			(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
		}
		delete[] buffer;
	}

	// Step 6: JPEG ���� ����
	jpeg_finish_compress(&cinfo);
	fclose(outfile);

	// Step 7: JPEG ������ ���� ��ü ����
	jpeg_destroy_compress(&cinfo);

	return TRUE;
}
