// header.h: 标准系统包含文件的包含文件，
// 或特定于项目的包含文件
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头文件中排除极少使用的内容
#define DIRECTINPUT_VERSION 0x800
// Windows 头文件
#include <windows.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include<d2d1.h>
#include<vector>
#include<process.h>
#include<string>
#include<dinput.h>
#include<wincodec.h>
#include<dwrite.h>
//#include"DTSOUND.h"
//#pragma comment(lib,"DTSOUND.lib")
//#include"include/box2d/box2d.h"
#include"chipmunk/chipmunk.h"

using namespace std;
//#pragma comment(lib,"box2d.lib")
#pragma comment(lib,"chipmunk.lib")
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")//使用dinput要链接dxguid.lib


typedef int DeviceType;
typedef wchar_t RSKEYSTATE;

#define RSKeyDown 128
#define RSKey0 0
#define RSKeyUp 129
#define DeviceType_Joy 1
#define DeviceType_KeyBoard 2
#define DeviceType_Mouse 3
#define DeviceType_All 0
#define PI 3.14159

#define ReleaseCom(X){if(X)X->Release();X=0;}
#define HR(X) {if(FAILED(X)) {MessageBox(NULL,0,0,0|MB_ICONERROR); return false;}}

static  float RadToDeg(float rad) { return rad * 180.0f / (float)PI; }

static ID2D1Bitmap* D2DCreateBitmap(ID2D1RenderTarget* d2dRenderTarget, IWICImagingFactory* g_image_factory, const wchar_t* FilePath)
{
	ID2D1Bitmap* g_bitmap = 0;
	if (!g_image_factory)
	{
		if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, NULL
			, CLSCTX_INPROC_SERVER, __uuidof(g_image_factory)
			, (LPVOID*)&g_image_factory)))
			return nullptr;
	}

	IWICBitmapDecoder* bitmapdecoder = NULL;

	if (FAILED(g_image_factory->CreateDecoderFromFilename(FilePath, NULL
		, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &bitmapdecoder)))
	{
		MessageBox(0, L"CreateBitmap Failed! Check the bitmap file Or Code", L"Error", MB_OK | MB_ICONERROR);
		return 0;
	}
	IWICBitmapFrameDecode* pframe = NULL;
	bitmapdecoder->GetFrame(0, &pframe);
	IWICFormatConverter* fmtcovter = NULL;	g_image_factory->CreateFormatConverter(&fmtcovter);

	fmtcovter->Initialize(pframe, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone
		, NULL, 0.0f, WICBitmapPaletteTypeCustom);
	d2dRenderTarget->CreateBitmapFromWicBitmap(fmtcovter, NULL, &g_bitmap);

	g_image_factory->Release();
	fmtcovter->Release();
	pframe->Release();
	bitmapdecoder->Release();
	return g_bitmap;
}



static DWORD GetCurrentActiveWindowsProcessId()
{
	HWND hWnd = GetActiveWindow();
	DWORD processId = 0;
	GetWindowThreadProcessId(hWnd, &processId);
	return processId;
}

static BOOL HasFocus()
{
	DWORD cp = GetCurrentProcessId();
	DWORD cawp = GetCurrentActiveWindowsProcessId();
	return cp == cawp;
}
