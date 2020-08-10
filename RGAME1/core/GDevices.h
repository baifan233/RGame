#pragma once
#include"framework.h"
#include"Resource.h"
#include"GameLogRecorder.h"
struct RMouseptPointer { int* x; int* y; };
class GDevices
{
	//Input System
private:
	LPDIRECTINPUT8 inp_dinputDeivce = NULL;   //dinput8设备
	LPDIRECTINPUTDEVICE8 inp_KeyBoard = NULL;   //键盘设备
	LPDIRECTINPUTDEVICE8 inp_Mouse = NULL;    //鼠标设备
	LPDIRECTINPUTDEVICE8 inp_Joy = NULL;   //手柄设备
	DIJOYSTATE2 inp_JoyState = { 0 };   //手柄输入状态
	DIDEVICEOBJECTDATA inp_keyBoardDidod[8] = { 0 };   //键盘输入状态数据
	DIMOUSESTATE inp_mouseState = { 0 };   //鼠标输入状态
	DIMOUSESTATE inp_mouseStateAfter = { 0 };////鼠标输入状态(经处置)  调用请调用这个
	RSKEYSTATE KeyState[256] = { 0 };   //键盘输入状态

	bool inp_bMouseReady = false;    //如果鼠标设备启用 则为true
	bool inp_bJoyReady = false;   //如果手柄设备启用 则为true
	bool inp_bKeyBoardReady = false; //如果键盘设备启用 则为true
	int MouseY = 0;    //鼠标在窗口内X坐标
	int MouseX = 0; //鼠标在窗口内Y坐标
	vector<WPARAM>* charwParam;     //指向窗口过程中输入字符储存的vetor
	//窗口内XY指针结构体
	RMouseptPointer mpt = { &MouseX,&MouseY };
public:
	vector<WPARAM>* GetCharParam() { return charwParam; }
	void SetCharParam(vector<WPARAM>* wparam) { charwParam = wparam; };
	void ClearInputData()    //清除所有输入设备的输入状态
	{
		memset(&inp_mouseState, 0, sizeof(DIMOUSESTATE));
		memset(&inp_mouseStateAfter, 0, sizeof(DIMOUSESTATE));
		for (size_t i = 0; i < 8; i++)
		{
			memset(&inp_keyBoardDidod[i], 0, sizeof(DIDEVICEOBJECTDATA));
		}
		memset(&KeyState, 0, sizeof(RSKEYSTATE));
	}
	//检查输入设备   如果设备还没初始化  就初始化
	void CheckInputDevice(DeviceType deviceType);
	//获取键盘输入状态
	RSKEYSTATE* GetKeyState() { return KeyState; };
	//获取手柄输入状态
	DIJOYSTATE2  GetJoyState() { return inp_JoyState; }
	//初始化输入设备
	int inp_InitinputDevice(HINSTANCE hInstance, DeviceType deviceType);
	//获取设备的运行状态(是否工作)
	bool inp_IsDeviceReady(DeviceType deviceType);

	void inp_DealInput(DeviceType deviceType);
	//获取设备数据(设备是否有信息)
	bool UpdateDeviceData(DeviceType deviceType);
	void inp_ShutDown(DeviceType deviceType);

	DIMOUSESTATE GetMouseState() { return inp_mouseStateAfter; };
	RMouseptPointer* GetMousePoint()
	{
		return &mpt;
	}
	void UpdateMousePoint(int x, int y) { MouseX = x; MouseY = y; }

	//Window
private:

	HWND mainWindow = 0;    //主窗口句柄
	wchar_t* ClassName = 0;   //窗口类名
	int width = 0, height = 0;   //宽度  高度
	HINSTANCE hIns = 0;   //实例句柄

public:
	RECT GetWindowRect() { RECT rect = { 0 }; rect.right = width; rect.bottom = height; return rect; };
	bool InitMainWindow(HINSTANCE hInstance, WNDPROC WndProc, const wchar_t* szWindowClass, int width, int height,bool border)
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
		DWORD ws = WS_OVERLAPPEDWINDOW;
		if (border == true)ws = WS_POPUP;
		mainWindow = CreateWindowW(szWindowClass, 0, ws,
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

	//Graphic	
protected:
	//D2D
	IDWriteFactory* g_dWriteFactory = NULL;  //dwrite工厂
	ID2D1Factory4* g_d2dFactory = NULL;  //d2d工厂
	IWICImagingFactory2* g_image_factory = NULL;

	ID2D1Device3* g_d2dDevice = 0;  //d2d设备
	ID2D1DeviceContext3* g_d2dContext = 0;   //d2d设备上下文
	IDXGISwapChain1* g_SwapChain = 0;  //交换链
	ID2D1Bitmap1* g_d2dTargetBitmap = 0;
	D3D_FEATURE_LEVEL g_featurelevel;   //特征级
	DXGI_PRESENT_PARAMETERS g_m_parameters;  //手动交换链

	ID3D11Device* g_d3dDevice = 0;  //d3d设备
	ID3D11DeviceContext* g_d3dDeviceContext = 0;  //d3d设备上下文

	//OpenGL
	HGLRC g_glrc=0;
	HDC g_hdc = 0;
	sp::Scene* scene=nullptr;
	sp::Renderer r=sp::Renderer();
	D2D1_RECT_F cameraRect = { 0.0f,0.0f,1024.0f,768.0f };

public:

	//获取交换链
	IDXGISwapChain1* g_GetSwapChain() { return g_SwapChain; }
	IDWriteFactory* g_GetDwriteFactory() { return g_dWriteFactory; }
	ID2D1Factory4* g_GetFactory() { return g_d2dFactory; }
	
	ID2D1DeviceContext3* g_GetD2DRen()
	{
		return g_d2dContext;
	};
	IWICImagingFactory* g_GetimageFacotry() { return g_image_factory; };
	void g_D2DPresent()
	{
		if (g_d2dContext)g_d2dContext->EndDraw();
	};

	bool g_InitGraphic(HWND hWnd);		
	void g_CreateWindowSizeResources();	
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
		/*if (cameraRect.left + x >= 0)
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
		}*/
		cameraRect.left += x;
		cameraRect.top += y;
		cameraRect.right = cameraRect.left + width;
		cameraRect.bottom = cameraRect.top + height;
	}
	float GetCameraX() { return cameraRect.left; };
	float GetCameraY() { return cameraRect.top; };
	D2D1_RECT_F GetCameraRect() { return cameraRect; };

	sp::Scene* g_GetScene() { return scene; }
	sp::Renderer* g_GetRenderer() { return &r; }
	void g_OpenGLPresent() { SwapBuffers(g_hdc); }
	HDC g_GetHDC() { return g_hdc; }
	//Recorder
protected:
	GameLogRecorder Recorder = GameLogRecorder();
public:
	GameLogRecorder* GetRecorder() { return &Recorder; }
};

