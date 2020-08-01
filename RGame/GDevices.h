#pragma once
#include"framework.h"
#include"Resource.h"
#include"RObject.h"
#include"Player.h"
class GDevices
{
public:
	HWND GetWindowHwnd();

	//Input System
private:
	LPDIRECTINPUT8 inp_dinputDeivce = NULL;
	LPDIRECTINPUTDEVICE8 inp_KeyBoard = NULL;
	LPDIRECTINPUTDEVICE8 inp_Mouse = NULL;
	LPDIRECTINPUTDEVICE8 inp_Joy = NULL;
	DIJOYSTATE2 inp_JoyState = { 0 };
	DIDEVICEOBJECTDATA inp_keyBoardDidod[8] = { 0 };
	DIMOUSESTATE inp_mouseState = { 0 };
	DIMOUSESTATE inp_mouseStateAfter = { 0 };
	RSKEYSTATE KeyState[256] = { 0 };

	bool inp_bMouseReady = false;
	bool inp_bJoyReady = false;
	bool inp_bKeyBoardReady = false;
	int MouseY = 0;
	int MouseX = 0;
public:
	void ClearInputData()
	{
		memset(&inp_mouseState, 0, sizeof(DIMOUSESTATE));
		memset(&inp_mouseStateAfter, 0, sizeof(DIMOUSESTATE));
		for (size_t i = 0; i < 8; i++)
		{
			memset(&inp_keyBoardDidod[i], 0, sizeof(DIDEVICEOBJECTDATA));
		}
		memset(&KeyState, 0, sizeof(RSKEYSTATE));
	}
	void CheckInputDevice(DeviceType deviceType);
	RSKEYSTATE* GetKeyState() { return this->KeyState; };
	int inp_InitinputDevice(HINSTANCE hInstance, DeviceType deviceType);
	//获取设备状态(是否启用)
	bool inp_GetDeviceState(DeviceType deviceType);

	void inp_DealInput(DeviceType deviceType);
	//获取设备数据(设备是否有信息)
	bool GetDeviceData(DeviceType deviceType);
	void inp_ShutDown(DeviceType deviceType);

	DIMOUSESTATE* GetMouseState() { return &inp_mouseStateAfter; };
	POINT GetMousePoint() { POINT pt = { MouseX,MouseY }; return pt; };


	//Window
private:

	HWND mainWindow = 0;
	wchar_t* ClassName = 0;
	int width = 0, height = 0;
	HINSTANCE hIns = 0;
	HWND menu;
public:
	bool InitMainWindow(HINSTANCE hInstance, WNDPROC WndProc, const wchar_t* szWindowClass, int width, int height)
	{
		WNDCLASSEXW wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RGAME));
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_RGAME);
		wcex.lpszClassName = szWindowClass;
		wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

		if (!RegisterClassExW(&wcex))
			return false;
		this->width = width;
		this->height = height;
		mainWindow = CreateWindowW(szWindowClass, 0, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, width, height, nullptr, nullptr, hInstance, nullptr);

		if (!mainWindow)
		{
			return FALSE;
		}

		return TRUE;
	}
	bool window_Show(bool show);

	HWND GetMainWindow() { return mainWindow; };
	~GDevices();
	GDevices() {};

	//D2D
private:
	ID2D1Factory* d2dFactory = NULL;

	IDWriteFactory* dWriteFactory = NULL;
	IDWriteTextFormat* m_pText = NULL;
	ID2D1SolidColorBrush* p_BlackBrush = 0;
	ID2D1SolidColorBrush* p_bControlsBrush = 0;

	IWICImagingFactory* g_image_factory = NULL;
	ID2D1HwndRenderTarget* d2dHwndRenderTarget = NULL;

	ID2D1Bitmap* g_bitmap = 0;
	ID2D1Bitmap* bitmap2 = 0;
	ID2D1BitmapBrush* bitmapBrush = 0;

	//ID2D1Layer* g_bgLayer = 0;
	DWORD frameRate = 1000 / 60;
	BYTE frame = 0;
	ULONGLONG lastSec = 0;
	ULONGLONG NowSec = 0;
	D2D1_RECT_F cameraRect = { 0.0f,0.0f,1024.0f,768.0f };
public:
	void SetCamera(float x, float y)
	{
		cameraRect.left = x;
		cameraRect.top = y;
		cameraRect.right = cameraRect.left + width;
		cameraRect.bottom = cameraRect.top + height;
	}
	void MoveCamera(float x, float y)
	{
		if (cameraRect.left + x >= 0)
		{
			cameraRect.left += x;
		}
		else
		{
			cameraRect.left = 0.0f;
		}
		if (cameraRect.top + y >= 0)
		{
			cameraRect.top += y;
		}
		else
		{
			cameraRect.top = 0.0f;
		}
		cameraRect.right = cameraRect.left + width;
		cameraRect.bottom = cameraRect.top + height;
	}
	float GetCameraX() { return cameraRect.left; };
	float GetCameraY() { return cameraRect.top; };
	cpVect ScreenToSpace(double x, double y)
	{
		return { x + cameraRect.left,y + cameraRect.top };
	}
	D2D1_RECT_F SpaceToScreen(D2D1_RECT_F rect)
	{		
		float rwidth = rect.right-rect.left;
		float rheight = rect.bottom-rect.top;
		rect.left = cameraRect.right - rect.left;
		rect.left = this->width - rect.left - rwidth / 2;
		rect.right = rect.left + rwidth;
		rect.top = cameraRect.bottom - rect.top;
		rect.top = this->height - rect.top - rheight / 2;
		rect.bottom = rect.top + rheight;
		return rect;
	}
	D2D1_RECT_F SpaceToScreen(float x, float y, float rwidth, float rheight)
	{
		D2D1_RECT_F rect = { 0 };
		rect.left = cameraRect.right - x;
		rect.left = this->width - rect.left - rwidth / 2;
		rect.right = rect.left + rwidth;
		rect.top = cameraRect.bottom - y;
		rect.top = this->height - rect.top - rheight / 2;
		rect.bottom = rect.top + rheight;
		return rect;
	}
	D2D1_POINT_2F SpaceToScreen(float x, float y)
	{
		D2D1_POINT_2F point;
		point.x = (float)width - (cameraRect.right - x);
		point.y = (float)height - (cameraRect.bottom - y);
		return point;
	}

	//ID2D1Factory* GetD2DFactory() { return d2dFactory; };
	ID2D1RenderTarget* GetD2DRen()
	{
		if (d2dHwndRenderTarget)return d2dHwndRenderTarget;
	};
	IWICImagingFactory* GetimageFacotry() { return g_image_factory; };
	//IDWriteTextFormat* GetTextFormat() { return m_pText; };
	bool InitD2D();
	void D2DRender(bool Present);
	void D2DPresent()
	{
		if (d2dHwndRenderTarget)d2dHwndRenderTarget->EndDraw();
	};
private:
	Player player;
public:

	void CreateItem(int x, int y, int type);
};

