#include"GDevices.h"

//inp

void GDevices::CheckInputDevice(DeviceType deviceType)
{
	inp_InitinputDevice(0, deviceType);
	if (inp_Joy == nullptr)
		inp_bJoyReady = false;
	if (inp_KeyBoard == nullptr)
		inp_bKeyBoardReady = false;
	if (inp_Mouse == nullptr)
		inp_bMouseReady = false;
}

int GDevices::inp_InitinputDevice(HINSTANCE hInstance, DeviceType deviceType)
{
	//初始化dinput设备
	if (!inp_dinputDeivce)
	{
		if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&inp_dinputDeivce, 0)))
		{
			//MessageBox(0, L"Init Dinput Failed!", L"error", 0);
			return -1;
		}
	}
	//如果deviceType是键盘类型或者是All  则初始化键盘
	if ((deviceType == DeviceType_KeyBoard || deviceType == DeviceType_All) && !inp_KeyBoard)
	{
#pragma region keyboardInit
		if (FAILED(inp_dinputDeivce->CreateDevice(GUID_SysKeyboard, &inp_KeyBoard, 0)))   //创建键盘设备
		{
			return 0;
		}

		if (FAILED(inp_KeyBoard->SetDataFormat(&c_dfDIKeyboard)))
		{
			return 0;
		}

		if (FAILED(inp_KeyBoard->SetCooperativeLevel(mainWindow, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))//设置为非独占且前台工作
		{
			return 0;
		}
		DIPROPDWORD dipdw;
		dipdw.diph.dwSize = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		dipdw.diph.dwObj = 0;
		dipdw.diph.dwHow = DIPH_DEVICE;
		dipdw.dwData = 8; // Arbitary buffer size
		if (FAILED(inp_KeyBoard->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph)))
		{
			return 0;
		}
		inp_bKeyBoardReady = true;
		inp_KeyBoard->Acquire();
#pragma endregion


	}
	//如果deviceType是鼠标类型或者是All  则初始化鼠标
	if ((deviceType == DeviceType_Mouse || deviceType == DeviceType_All) && !inp_Mouse)
	{
		if (FAILED(inp_dinputDeivce->CreateDevice(GUID_SysMouse, &inp_Mouse, 0)))
		{
			return 0;
		}

		if (FAILED(inp_Mouse->SetDataFormat(&c_dfDIMouse)))
		{
			return 0;
		}

		if (FAILED(inp_Mouse->SetCooperativeLevel(mainWindow, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
		{
			return 0;
		}
		inp_bMouseReady = true;
	}
	//如果deviceType是手柄类型或者是All  则初始化手柄
	if ((deviceType == DeviceType_Joy || deviceType == DeviceType_All) && !inp_Joy)
	{
#pragma region JoyInit
		UINT choose = 0;
		if (FAILED(inp_dinputDeivce->CreateDevice(GUID_Joystick, &inp_Joy, 0)))
		{
			return 0;
		}

		if (FAILED(inp_Joy->SetDataFormat(&c_dfDIJoystick2)))
		{
			return 0;
		}

		if (FAILED(inp_Joy->SetCooperativeLevel(mainWindow, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
		{
			return 0;
		}
		DIPROPRANGE diprg;
		diprg.diph.dwSize = sizeof(DIPROPRANGE);
		diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		diprg.diph.dwHow = DIPH_DEVICE;
		diprg.diph.dwObj = 0; // Specify the enumerated axis
		diprg.lMin = -1000;
		diprg.lMax = +1000;
		if (FAILED(inp_Joy->SetProperty(DIPROP_RANGE, &diprg.diph)))
		{
			return 0;
		}
		inp_Joy->Acquire();

#pragma endregion

	}

	return 1;
}
bool GDevices::inp_IsDeviceReady(DeviceType deviceType)
{
	if (inp_Joy == nullptr)
		inp_bJoyReady = false;
	if (inp_KeyBoard == nullptr)
		inp_bKeyBoardReady = false;
	if (inp_Mouse == nullptr)
		inp_bMouseReady = false;
	switch (deviceType)
	{
	case DeviceType_Joy:
		return inp_bJoyReady;
	case DeviceType_KeyBoard:
		return inp_bKeyBoardReady;
	case DeviceType_Mouse:
		return inp_bMouseReady;
	default:
		break;
	}
	return false;
}
void GDevices::inp_ShutDown(DeviceType deviceType)
{
	switch (deviceType)
	{
	case 0:
		ReleaseCom(inp_Joy);
		ReleaseCom(inp_KeyBoard);
		ReleaseCom(inp_Mouse);
		break;
	case DeviceType_Joy:
		//inp_Joy->Unacquire();
		ReleaseCom(inp_Joy);

		break;
	case DeviceType_KeyBoard:
		//inp_KeyBoard->Unacquire();		
		ReleaseCom(inp_KeyBoard);

		break;
	case DeviceType_Mouse:
		//inp_Mouse->Unacquire();
		ReleaseCom(inp_Mouse);

		break;
	default:
		break;
	}
}
bool GDevices::UpdateDeviceData(DeviceType deviceType)
{
	DWORD dwElements = 8;
	POINT pt = { 0 };
	DIJOYSTATE2 diJoyStateTemp = { 0 };
	if (deviceType == DeviceType_KeyBoard)
	{
		if (this->inp_KeyBoard == NULL)
			return false;
		ZeroMemory(inp_keyBoardDidod, sizeof(DIDEVICEOBJECTDATA) * 8);
		//g_KeyBoard->Poll();
		if (DI_OK != inp_KeyBoard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), inp_keyBoardDidod, &dwElements, 0))
			inp_KeyBoard->Acquire();
		return true;
	}
	else if (deviceType == DeviceType_Joy)
	{
		if (this->inp_Joy == NULL)
			return false;
		for (size_t i = 0; i < 128; i++)
		{
			if (inp_JoyState.rgbButtons[i] == RSKeyUp)
			{
				inp_JoyState.rgbButtons[i] = RSKey0;
			}
		}
		diJoyStateTemp = inp_JoyState;
		inp_Joy->Poll();
		if (DI_OK != inp_Joy->GetDeviceState(sizeof(DIJOYSTATE2), &inp_JoyState))
			inp_Joy->Acquire();
		for (size_t i = 0; i < 128; i++)
		{
			if (inp_JoyState.rgbButtons[i] == 0 && diJoyStateTemp.rgbButtons[i] == RSKeyDown)
			{
				inp_JoyState.rgbButtons[i] = RSKeyUp;
			}
		}
		return true;
	}
	else if (deviceType == DeviceType_Mouse)
	{
		if (this->inp_Mouse == NULL)
			return false;
		inp_Mouse->Poll();
		if (DI_OK != inp_Mouse->GetDeviceState(sizeof(DIMOUSESTATE), &inp_mouseState))
			inp_Mouse->Acquire();
		return true;

	}
	return false;
}
void GDevices::inp_DealInput(DeviceType deviceType)
{

	if (deviceType == DeviceType_KeyBoard || deviceType == DeviceType_All)
	{
		for (int i = 0; i < 256; i++)//覆盖掉上次的KeyUp消息
		{
			if (KeyState[i] == RSKeyUp)
				KeyState[i] = RSKey0;
		}
		for (int i = 0; i < 8; i++)
		{
			if (inp_keyBoardDidod[i].dwOfs != 0)
			{
				if (inp_keyBoardDidod[i].dwData == 128)
					KeyState[inp_keyBoardDidod[i].dwOfs] = RSKeyDown;
				else if (inp_keyBoardDidod[i].dwData == 0)
				{
					if (KeyState[inp_keyBoardDidod[i].dwOfs] == RSKeyDown)
						KeyState[inp_keyBoardDidod[i].dwOfs] = RSKeyUp;
				}

			}
		}

	}
	if (deviceType == DeviceType_Joy || deviceType == DeviceType_All)
	{

	}
	if (deviceType == DeviceType_Mouse || deviceType == DeviceType_All)
	{
		inp_mouseStateAfter.lX = inp_mouseState.lX;
		inp_mouseStateAfter.lY = inp_mouseState.lY;
		inp_mouseStateAfter.lZ = inp_mouseState.lZ;
		for (int i = 0; i < 3; i++)
		{
			if (RSKeyUp == inp_mouseStateAfter.rgbButtons[i])//处理上次的KeyUp消息 改为Key0
			{
				inp_mouseStateAfter.rgbButtons[i] = RSKey0;
			}
			if (inp_mouseState.rgbButtons[i] == RSKeyDown)
			{
				inp_mouseStateAfter.rgbButtons[i] = inp_mouseState.rgbButtons[i];
			}
			else if (inp_mouseState.rgbButtons[i] == RSKey0 && inp_mouseStateAfter.rgbButtons[i] == RSKeyDown)
			{
				inp_mouseStateAfter.rgbButtons[i] = RSKeyUp;
			}
		}
	}

	return;
}


//window
bool GDevices::window_Show(bool show)
{
	int s = 0;
	if (show)
	{
		s = SW_NORMAL;
	}
	else
		s = SW_HIDE;
	return ShowWindow(mainWindow, s);
}


GDevices::~GDevices()
{
	//g_d3dDeviceContext->

	//d2d
	ReleaseCom(g_d2dContext);
	ReleaseCom(g_d2dDevice);
	ReleaseCom(g_d2dTargetBitmap);
	ReleaseCom(g_d3dDevice);
	ReleaseCom(g_d3dDeviceContext);
	ReleaseCom(g_image_factory);
	ReleaseCom(g_d2dFactory);
	ReleaseCom(g_dWriteFactory);
	ReleaseCom(g_SwapChain);
	
	//OpenGL
	delete scene;	
	wglMakeCurrent(0, 0);
	wglDeleteContext(g_glrc);
	ReleaseDC(mainWindow, g_hdc);
	SDL_Quit();

	//input
	inp_ShutDown(DeviceType_All);
	ReleaseCom(inp_dinputDeivce);

	//window
	DestroyWindow(mainWindow);
	UnregisterClass(ClassName, hIns);
}

void GDevices::g_CreateWindowSizeResources()
{
	// DXGI 适配器
	IDXGIAdapter* pDxgiAdapter = nullptr;
	// DXGI 工厂
	IDXGIFactory2* pDxgiFactory = nullptr;
	// DXGI Surface 后台缓冲
	IDXGISurface* pDxgiBackBuffer = nullptr;
	// DXGI 设备
	IDXGIDevice1* pDxgiDevice = nullptr;

	HRESULT hr = S_OK;

	// 清除之前窗口的呈现器相关设备
	g_d2dContext->SetTarget(nullptr);
	ReleaseCom(g_d2dTargetBitmap);
	g_d3dDeviceContext->Flush();

	RECT rect = { 0 }; GetClientRect(mainWindow, &rect);

	if (g_SwapChain != nullptr)
	{
		// 如果交换链已经创建，则重设缓冲区
		hr = g_SwapChain->ResizeBuffers(
			2, // Double-buffered swap chain.
			lround((double)rect.right - (double)rect.left),
			lround((double)rect.bottom - (double)rect.top),
			DXGI_FORMAT_B8G8R8A8_UNORM,
			0);

		assert(hr == S_OK);
	}
	else
	{
		// 否则用已存在的D3D设备创建一个新的交换链
		DXGI_SWAP_CHAIN_DESC1 g_SwapChainDesc = { 0 };
		g_SwapChainDesc.Width = lround((double)rect.right - (double)rect.left);
		g_SwapChainDesc.Height = lround((double)rect.bottom - (double)rect.top);
		g_SwapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		g_SwapChainDesc.Stereo = false;
		g_SwapChainDesc.SampleDesc.Count = 1;
		g_SwapChainDesc.SampleDesc.Quality = 0;
		g_SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		g_SwapChainDesc.BufferCount = 2;
		g_SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		g_SwapChainDesc.Flags = 0;
		g_SwapChainDesc.Scaling = DXGI_SCALING_NONE;
		g_SwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		// 获取 IDXGIDevice
		if (SUCCEEDED(hr))
		{
			hr = g_d3dDevice->QueryInterface(IID_PPV_ARGS(&pDxgiDevice));
		}
		// 获取Dxgi适配器 可以获取该适配器信息
		if (SUCCEEDED(hr))
		{
			hr = pDxgiDevice->GetAdapter(&pDxgiAdapter);
		}
		// 获取Dxgi工厂
		if (SUCCEEDED(hr))
		{
			hr = pDxgiAdapter->GetParent(IID_PPV_ARGS(&pDxgiFactory));
		}
		// 创建交换链
		if (SUCCEEDED(hr))
		{
			hr = pDxgiFactory->CreateSwapChainForHwnd(
				g_d3dDevice,
				mainWindow,
				&g_SwapChainDesc,
				nullptr,
				nullptr,
				&g_SwapChain);
		}
		// 确保DXGI队列里边不会超过一帧
		if (SUCCEEDED(hr))
		{
			hr = pDxgiDevice->SetMaximumFrameLatency(1);
		}
	}

	// 设置屏幕方向
	if (SUCCEEDED(hr))
	{
		hr = g_SwapChain->SetRotation(DXGI_MODE_ROTATION_IDENTITY);
	}
	// 利用交换链获取Dxgi表面
	if (SUCCEEDED(hr))
	{
		hr = g_SwapChain->GetBuffer(0, IID_PPV_ARGS(&pDxgiBackBuffer));
	}
	// 利用Dxgi表面创建位图
	if (SUCCEEDED(hr))
	{
		D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
			96.0f,
			96.0f);
		hr = g_d2dContext->CreateBitmapFromDxgiSurface(
			pDxgiBackBuffer,
			&bitmapProperties,
			&g_d2dTargetBitmap);
	}
	// 设置
	if (SUCCEEDED(hr))
	{
		// 设置 Direct2D 渲染目标
		g_d2dContext->SetTarget(g_d2dTargetBitmap);
	}

	ReleaseCom(pDxgiDevice);
	ReleaseCom(pDxgiAdapter);
	ReleaseCom(pDxgiFactory);
	ReleaseCom(pDxgiBackBuffer);
}


//Graphic
bool GDevices::g_InitGraphic(HWND hWnd)
{

	if (mainWindow == NULL)
		return false;
	if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_d2dFactory)))
		return false;

	if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(g_dWriteFactory),
		reinterpret_cast<IUnknown**>(&g_dWriteFactory))))

		return false;
	//DXGI工厂
	IDXGIFactory2* dxgiFactory = 0;
	//DXGI设备
	IDXGIDevice1* dxgiDevice = 0;

	HRESULT hr = S_OK;
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL featureLevels[] =
	{
	D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
	};

	hr = D3D11CreateDevice(
		nullptr,					// 设为空指针选择默认设备
		D3D_DRIVER_TYPE_HARDWARE,	// 强行指定硬件渲染
		nullptr,					// 强行指定WARP渲染 D3D_DRIVER_TYPE_WARP 没有软件接口
		creationFlags,				// 创建flag
		featureLevels,				// 欲使用的特性等级列表
		ARRAYSIZE(featureLevels),	// 特性等级列表长度
		D3D11_SDK_VERSION,			// SDK 版本
		&g_d3dDevice,  //返回的D3D11设备指针
		&g_featurelevel,//返回的特征级
		&g_d3dDeviceContext   //返回的D3D11设备上下文指针
	);
	//创建IDXGIDevice
	if (SUCCEEDED(hr))
		hr = g_d3dDevice->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
	//创建D2D设备
	if (SUCCEEDED(hr))
		hr = g_d2dFactory->CreateDevice(dxgiDevice, &g_d2dDevice);
	//创建D2D设备上下文
	if (SUCCEEDED(hr))
		hr = g_d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &g_d2dContext);
	ReleaseCom(dxgiDevice);
	ReleaseCom(dxgiFactory);

	g_CreateWindowSizeResources();

	g_d2dContext->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);//抗锯齿

	if (hr == E_FAIL)
		return false;

	//InitOpenGL



	scene = new sp::DemoScene();	
	r.scene(scene);
	r.Initialize();
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);

	g_hdc = GetDC(mainWindow);
	PIXELFORMATDESCRIPTOR pfd;
	int  iFormat;

	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	iFormat = ChoosePixelFormat(g_hdc, &pfd);
	SetPixelFormat(g_hdc, iFormat, &pfd);
	g_glrc = wglCreateContext(g_hdc);
	wglMakeCurrent(g_hdc, g_glrc);
	glewExperimental = GL_TRUE;
	//SDL_GL_SetSwapInterval(1);
	glewInit();

	r.Load();
	//g_hdc = GetDC(hWnd);
	//PIXELFORMATDESCRIPTOR pfd;
	//int  iFormat;

	//ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	//pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	//pfd.nVersion = 1;
	//pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	//pfd.iPixelType = PFD_TYPE_RGBA;
	//pfd.cColorBits = 24;
	//pfd.cDepthBits = 16;
	//pfd.iLayerType = PFD_MAIN_PLANE;

	//iFormat = ChoosePixelFormat(g_hdc, &pfd);
	//SetPixelFormat(g_hdc, iFormat, &pfd);
	//g_glrc = wglCreateContext(g_hdc);
	//if (g_glrc == nullptr)return false;
	//wglMakeCurrent(g_hdc, g_glrc);
	//glewInit();

	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);
	////resizeWindow
	//glEnable(GL_TEXTURE_2D);
	//glEnable(GL_BLEND);

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glViewport(0, 0, 1024, 768);

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glOrtho(0, 1024, 768, 0, -1.0f, 1.0f);

	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	//glMatrixMode(GL_TEXTURE);
	//glLoadIdentity();

	//glDisable(GL_CULL_FACE);
	//fbo = new FrameBufferObject(width,height,2);
	return true;

}