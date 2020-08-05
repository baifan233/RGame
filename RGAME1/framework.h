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
#include<d2d1_3.h>
#include<d2d1_3helper.h>
#include<vector>
#include<process.h>
#include<string>
#include<dinput.h>
#include<wincodec.h>
#include<dwrite_2.h>
#include<dxgi1_4.h>
#include<d3d11.h>
#include<assert.h>
#include<crtdbg.h>

//#include"DTSOUND.h"
//#pragma comment(lib,"DTSOUND.lib")
//#include"include/box2d/box2d.h"

#include"chipmunk/chipmunk.h"

using namespace std;
//#pragma comment(lib,"box2d.lib")

// 32位编译
#ifdef _M_IX86
#pragma comment(lib,"chipmunk.lib")
#endif // _M_IX86
// 64位编译
#ifdef _M_X64
#pragma comment(lib,"chipmunk64.lib")
#endif // _M_X64


#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")//使用dinput要链接dxguid.lib

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwrite.lib" )
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "windowscodecs.lib" )


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

static int screenWidth = 1024;
static int screenHeight = 768;


static ID2D1Bitmap* D2DCreateBitmap(ID2D1RenderTarget* d2dRenderTarget, IWICImagingFactory* g_image_factory, const wchar_t* FilePath)
{
	ID2D1Bitmap* g_bitmap = 0;
	if (!g_image_factory)
	{
		if (FAILED(CoCreateInstance(CLSID_WICImagingFactory2, NULL
			, CLSCTX_INPROC_SERVER, __uuidof(g_image_factory)
			, (LPVOID*)&g_image_factory)))
			return nullptr;
	}

	IWICBitmapDecoder* bitmapdecoder = NULL;

	if (FAILED(g_image_factory->CreateDecoderFromFilename(FilePath, NULL
		, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &bitmapdecoder)))
	{
		
		return nullptr;
	}
	IWICBitmapFrameDecode* pframe = NULL;
	bitmapdecoder->GetFrame(0, &pframe);
	IWICFormatConverter* fmtcovter = NULL;	g_image_factory->CreateFormatConverter(&fmtcovter);

	fmtcovter->Initialize(pframe, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone
		, NULL, 0.0f, WICBitmapPaletteTypeCustom);
	d2dRenderTarget->CreateBitmapFromWicBitmap(fmtcovter, NULL, &g_bitmap);

	//g_image_factory->Release();
	fmtcovter->Release();
	pframe->Release();
	bitmapdecoder->Release();
	return g_bitmap;
}


static string DeleteUnnecessaryChar(string b2)
{
	for (size_t i = 0; i < b2.size(); i++)
	{
		if (b2[i] == '\n' || b2[i] == ' ' || b2[i] < 0 || b2[i] == '\t')
		{
			b2.erase(b2.begin() + i);
			if (i >= 1)i--;
			continue;
		}
	}
	return b2;
}
static string DeleteNote(string b2)
{
	int f1 = b2.find("/*");
	int f2 = b2.find("*/");
	while (f1 != -1)
	{
		if (f1 != -1 && f1 < f2)
		{
			b2.erase(b2.begin() + f1, b2.begin() + f2 + 2);
		}
		f1 = b2.find("/*");
		f2 = b2.find("*/");
	}
	f1 = b2.find("//");

	while (f1 != -1)
	{
		if (f1 != -1)
		{
			f2 = b2.find("\n", f1);
			b2.erase(b2.begin() + f1, b2.begin() + f2 + 1);
		}
		f1 = b2.find("//");
		if (f1 != -1)
		{
			f2 = b2.find("\n", f1);
		}
	}
	return b2;
}

static DWORD GetCurrentActiveWindowsProcessId()
{
	HWND hWnd = GetActiveWindow();
	DWORD processId = 0;
	GetWindowThreadProcessId(hWnd, &processId);
	return processId;
}
static vector<string> split(string str, string pattern)
{
	string::size_type pos;
	vector<string> result;

	str += pattern;
	int size = str.size();

	for (int i = 0; i < size; i++) {
		pos = str.find(pattern, i);
		if (pos < size) {
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}
static BOOL HasFocus()
{
	DWORD cp = GetCurrentProcessId();
	DWORD cawp = GetCurrentActiveWindowsProcessId();
	return cp == cawp;
}


static bool PtIncamera(D2D1_RECT_F cameraRect,cpVect pos)
{

}

// 创建路径几何图形
static ID2D1PathGeometry* GetPathGeometry(ID2D1Factory4* pD2DFactory, vector<D2D1_POINT_2F> points, UINT pointsCount, cpVect Pos)
{
	ID2D1PathGeometry* geometry = NULL;
	HRESULT hr = pD2DFactory->CreatePathGeometry(&geometry);
	if (SUCCEEDED(hr))
	{
		ID2D1GeometrySink* pSink = NULL;
		hr = geometry->Open(&pSink); // 获取Sink对象

		if (SUCCEEDED(hr))
		{
			for (size_t i = 0; i < points.size(); i++)
			{
				points[i].x += (float)Pos.x;
				points[i].y += (float)Pos.y;
			}
			D2D1_POINT_2F* pointst = new D2D1_POINT_2F[points.size()]{0};
			memcpy(pointst,&points[0],sizeof(D2D1_POINT_2F)*points.size());
			pSink->BeginFigure(pointst[0], D2D1_FIGURE_BEGIN_FILLED);

			pSink->AddLines(pointst, pointsCount);
			pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
			delete[] pointst;
		}
		
		pSink->Close(); // 关闭Sink对象
		pSink->Release();		
		return geometry;
	}

	return NULL;
}

static cpVect ScreenToSpace(double x, double y, D2D1_RECT_F cameraRect,float scaleSize)
{
	return { x /scaleSize+ cameraRect.left,y/scaleSize + cameraRect.top };	
}
static D2D1_RECT_F SpaceToScreen(D2D1_RECT_F rect, D2D1_RECT_F cameraRect)
{
	float rwidth = rect.right - rect.left;
	float rheight = rect.bottom - rect.top;
	rect.left = cameraRect.right - rect.left;
	rect.left = screenWidth - rect.left - rwidth / 2;
	rect.right = rect.left + rwidth;
	rect.top = cameraRect.bottom - rect.top;
	rect.top = screenHeight - rect.top - rheight / 2;
	rect.bottom = rect.top + rheight;
	return rect;
}
static D2D1_RECT_F SpaceToScreen(float x, float y, float rwidth, float rheight, D2D1_RECT_F cameraRect)
{
	D2D1_RECT_F rect = { 0 };
	rect.left = cameraRect.right - x;
	rect.left = screenWidth - rect.left - rwidth / 2;
	rect.right = rect.left + rwidth;
	rect.top = cameraRect.bottom - y;
	rect.top = screenHeight - rect.top - rheight / 2;
	rect.bottom = rect.top + rheight;
	return rect;
}
static D2D1_POINT_2F SpaceToScreen(cpVect pointt,float rwidth,float rheight,D2D1_RECT_F cameraRect)
{
	D2D1_POINT_2F point;
	point.x = cameraRect.right - (float)pointt.x;
	point.x = (float)screenWidth -point.x-rwidth/2;
	point.y = (float)screenHeight - (cameraRect.bottom - (float)pointt.y)-rheight/2;
	return point;
}

static D2D1_POINT_2F SpaceToScreen(float x,float y, D2D1_RECT_F cameraRect)
{
	D2D1_POINT_2F point;
	
	point.x = (float)screenWidth - (cameraRect.right - x);
	point.y = (float)screenHeight - (cameraRect.bottom - y);
	return point;
}