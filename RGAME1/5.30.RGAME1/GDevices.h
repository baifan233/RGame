#pragma once
#include"framework.h"
#include"Resource.h"
#include"RObject.h"
#include"Player.h"
static IDWriteFactory* dWriteFactory = NULL;  //dwrite工厂
static ID2D1Factory4* d2dFactory = NULL;  //d2d工厂
static IWICImagingFactory2* g_image_factory = NULL;
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

	void UpdateMousePoint(int x, int y) { MouseX = x; MouseY = y; }

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
		wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RGAME1));
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_RGAME1);
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

	ID2D1Device3* d2dDevice = 0;  //d2d设备
	ID2D1DeviceContext3* d2dContext = 0;   //d2d设备上下文
	IDXGISwapChain1* SwapChain = 0;  //交换链
	ID2D1Bitmap1* d2dTargetBitmap = 0;   
	D3D_FEATURE_LEVEL featurelevel;   //特征级
	DXGI_PRESENT_PARAMETERS m_parameters;  //手动交换链

	ID3D11Device* d3dDevice = 0;  //d3d设备
	ID3D11DeviceContext* d3dDeviceContext = 0;  //d3d设备上下文
	
	
	IDWriteTextFormat* m_pText = NULL;
	ID2D1SolidColorBrush* p_BlackBrush = 0;
	ID2D1SolidColorBrush* p_bControlsBrush = 0;

	

	ID2D1Bitmap* g_bitmap = 0;
	ID2D1Bitmap* bitmap2 = 0;
	ID2D1BitmapBrush* bitmapBrush = 0;

	//ID2D1Layer* g_bgLayer = 0;
	DWORD frameRate = 1000 / 60;
	BYTE frame = 0;
	ULONGLONG lastSec = 0;
	ULONGLONG NowSec = 0;
	D2D1_RECT_F cameraRect = { 0.0f,0.0f,1024.0f,768.0f };
	float scaleSize = 1.0f;
public:
	bool bBuildMode=false;
	bool bGravity = true;
	void CreateWindowSizeResources();
	void SetCamera(float x, float y)
	{
		/*x = x/scaleSize;
		y = y/scaleSize;*/
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
	

	//ID2D1Factory* GetD2DFactory() { return d2dFactory; };
	ID2D1RenderTarget* GetD2DRen()
	{
		if (d2dContext)return d2dContext;
	};
	IWICImagingFactory* GetimageFacotry() { return g_image_factory; };
	//IDWriteTextFormat* GetTextFormat() { return m_pText; };
	bool InitD2D();
	void D2DRender(bool Present);
	void D2DPresent()
	{
		if (d2dContext)d2dContext->EndDraw();
	};
private:
	
public:

	void CreateItem(int x, int y, int type);
};

