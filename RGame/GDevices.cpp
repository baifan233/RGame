#include"GDevices.h"
//inp

void GDevices::CheckInputDevice(DeviceType deviceType)
{
	inp_InitinputDevice(0, deviceType);
}

int GDevices::inp_InitinputDevice(HINSTANCE hInstance, DeviceType deviceType)
{
	if (!inp_dinputDeivce)
	{
		if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&inp_dinputDeivce, 0)))
		{
			//MessageBox(0, L"Init Dinput Failed!", L"error", 0);
			return -1;
		}
	}

	if ((deviceType == DeviceType_KeyBoard || deviceType == DeviceType_All) && !inp_KeyBoard)
	{
#pragma region keyboardInit
		if (FAILED(inp_dinputDeivce->CreateDevice(GUID_SysKeyboard, &inp_KeyBoard, 0)))
		{
			//MessageBox(0, L"Init Dinput Failed!", L"error", 0);
			return 0;
		}

		if (FAILED(inp_KeyBoard->SetDataFormat(&c_dfDIKeyboard)))
		{
			//MessageBox(0, L"Init Dinput Failed!", L"error", 0);
			return 0;
		}

		if (FAILED(inp_KeyBoard->SetCooperativeLevel(mainWindow, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
		{
			//MessageBox(0, L"Init Dinput Failed!", L"error", 0);
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
			//MessageBox(0, L"Init Dinput Failed!", L"error", 0);
			return 0;
		}
		inp_KeyBoard->Acquire();
#pragma endregion


	}
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
	if ((deviceType == DeviceType_Mouse || deviceType == DeviceType_All) && !inp_Mouse)
	{
#pragma region mouseInit
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

#pragma endregion


	}
	return 1;
}
bool GDevices::inp_GetDeviceState(DeviceType deviceType)
{
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
		if (inp_Joy)
		{
			//inp_Joy->Unacquire();
			inp_Joy->Release();
		}
		if (inp_KeyBoard)
		{
			//inp_KeyBoard->Unacquire();
			inp_KeyBoard->Release();
		}
		if (inp_Mouse)
		{
			//inp_Mouse->Unacquire();
			inp_Mouse->Release();
		}
		break;
	case DeviceType_Joy:
		if (inp_Joy)
		{
			//inp_Joy->Unacquire();
			inp_Joy->Release();
		}
		break;
	case DeviceType_KeyBoard:
		if (inp_KeyBoard)
		{
			//inp_KeyBoard->Unacquire();
			inp_KeyBoard->Release();
		}
		break;
	case DeviceType_Mouse:
		if (inp_Mouse)
		{
			//inp_Mouse->Unacquire();
			inp_Mouse->Release();
		}
		break;
	default:
		break;
	}
}
bool GDevices::GetDeviceData(DeviceType deviceType)
{
	DWORD dwElements = 8;
	POINT pt = { 0 };
	DIJOYSTATE2 diJoyStateTemp = { 0 };
	switch (deviceType)
	{
	case DeviceType_KeyBoard:
		if (this->inp_KeyBoard == NULL)
			return false;
		ZeroMemory(inp_keyBoardDidod, sizeof(DIDEVICEOBJECTDATA) * 8);
		//g_KeyBoard->Poll();
		if (DI_OK != inp_KeyBoard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), inp_keyBoardDidod, &dwElements, 0))
			inp_KeyBoard->Acquire();
		return true;
		break;
	case DeviceType_Joy:
		if (this->inp_Joy == NULL)
			return false;

		//g_KeyBoard->Poll();
		/*if (DI_OK != inp_Joy->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), inp_joyDidod, &dwElements, 0))
			inp_Joy->Acquire();*/
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
		break;
	case DeviceType_Mouse:
		if (this->inp_Mouse == NULL)
			return false;

		inp_Mouse->Poll();
		if (DI_OK != inp_Mouse->GetDeviceState(sizeof(DIMOUSESTATE), &inp_mouseState))
			inp_Mouse->Acquire();		
		GetCursorPos(&pt);
		MouseX = pt.x;
		MouseY = pt.y;
		return true;
		break;
	default:
		break;
	}
	return false;
}
void GDevices::inp_DealInput(DeviceType deviceType)
{
	for (int i = 0; i < 256; i++)//覆盖掉上次的KeyUp消息
	{
		if (KeyState[i] == RSKeyUp)
			KeyState[i] = RSKey0;
	}

	if (deviceType == DeviceType_KeyBoard)
	{
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
		if (KeyState[DIK_W] == RSKeyUp)
		{
			/*for (size_t i = 0; i < bodys.size(); i++)
			{
				cpBodyDestroy(bodys[i]);
			}
			bodys.~vector();*/
		}
		else if (KeyState[DIK_LEFT] == RSKeyDown)
		{
			this->MoveCamera(-1.0f, 0.0f);
		}
		else if (KeyState[DIK_RIGHT] == RSKeyDown)
		{
			this->MoveCamera(1.0f, 0.0f);
		}
		else if (KeyState[DIK_UP] == RSKeyDown)
		{
			this->MoveCamera(0.0f, -1.0f);
		}
		else if (KeyState[DIK_DOWN] == RSKeyDown)
		{
			this->MoveCamera(0.0f, 1.0f);
		}


	}
	else if (deviceType == DeviceType_Joy)
	{		

		if (inp_JoyState.lX)
		{
			player.MoveX(inp_JoyState.lX / 5);
		}
		if (inp_JoyState.rgbButtons[0] == RSKeyDown)
		{
			player.MoveY(-500);
		}
		if (inp_JoyState.rgbButtons[1] == RSKeyDown)
		{
			player.MoveY(500);
		}
		if (inp_JoyState.rgbButtons[2] == RSKeyDown)
		{
			player.MoveX(2000);
		}
		if (inp_JoyState.rgbButtons[3] == RSKeyDown)
		{
			player.MoveX(-2000);
		}
		/*if (inp_JoyState.lRx != 0 || inp_JoyState.lRy != 0)
		{
			int lry = inp_JoyState.lRy / 20;
			mouse_event(MOUSEEVENTF_WHEEL, 0, 0, -lry, 0);
		}
		if (inp_JoyState.rgbButtons[0] == RSKeyDown)
		{
			if (!simMouseButtonStateA[0])
			{
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				simMouseButtonStateA[0] = true;
			}
		}
		else if (inp_JoyState.rgbButtons[0] == RSKeyUp)
		{
			mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			simMouseButtonStateA[0] = false;
		}*/


	}
	else if (deviceType == DeviceType_Mouse)
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
HWND GDevices::GetWindowHwnd()
{
	return mainWindow;
}


GDevices::~GDevices()
{
	//if (g_bitmap)g_bitmap->Release();	
	ReleaseCom(g_image_factory);
	ReleaseCom(d2dFactory);
	ReleaseCom(dWriteFactory);
	ReleaseCom(p_BlackBrush);
	ReleaseCom(p_bControlsBrush);

	//input
	if (inp_bJoyReady)
	{
		inp_ShutDown(DeviceType_Joy);
	}
	if (inp_bKeyBoardReady)
	{
		inp_ShutDown(DeviceType_KeyBoard);
	}
	if (inp_bMouseReady)
	{
		inp_ShutDown(DeviceType_Mouse);
	}
	if (inp_dinputDeivce)ReleaseCom(inp_dinputDeivce);

	DestroyWindow(mainWindow);
	UnregisterClass(ClassName, hIns);
}

//D2D

#include"World.h"
World world;
bool GDevices::InitD2D()
{

	if (mainWindow == NULL)
		return false;
	if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory)))
		return false;

	if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(dWriteFactory),
		reinterpret_cast<IUnknown**>(&dWriteFactory))))

		return false;


	if (FAILED(dWriteFactory->CreateTextFormat(L"微软雅黑", 0, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_EXTRA_CONDENSED, 30, L"", &m_pText)))
		return false;
	m_pText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_pText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);







	//surface.Reset();


	if (!this->d2dHwndRenderTarget)//如果还没初始化   (防止重复初始化)
	{
		if (mainWindow)//如果窗口句柄不为空就创建窗口句柄的
		{
			if (FAILED(d2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(mainWindow, D2D1::SizeU(width, height)), &d2dHwndRenderTarget)))
				return false;

			if (FAILED(d2dHwndRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &p_bControlsBrush)))
				return false;
			if (FAILED(d2dHwndRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &p_BlackBrush)))
				return false;
			g_bitmap = D2DCreateBitmap(d2dHwndRenderTarget, g_image_factory, L"C:\\Program Files\\RS\\tex.jpg");

			//d2dHwndRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);  
			if (!g_bitmap)return false;



			d2dHwndRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);//抗锯齿

			bitmap2 = D2DCreateBitmap(d2dHwndRenderTarget, g_image_factory, L"C:\\bitmap2.png");
			d2dHwndRenderTarget->CreateBitmapBrush(bitmap2, &bitmapBrush);
			bitmapBrush->SetExtendModeX(D2D1_EXTEND_MODE_MIRROR);
			bitmapBrush->SetExtendModeY(D2D1_EXTEND_MODE_MIRROR);

			d2dHwndRenderTarget->BeginDraw();
			d2dHwndRenderTarget->DrawBitmap(g_bitmap, { 0.0f,0.0f,1024.0f,768.0f });
			d2dHwndRenderTarget->EndDraw();
						
			world.Init(60000,5200);
			player.AddShapeAsBox(0,0,30,30,1,world.GetSpace(),0.7);
			return true;
		}
		else
			return false;
	}
	else
		return true;
}
void GDevices::D2DRender(bool Present)
{
	ULONGLONG ts = GetTickCount64();
	if (d2dHwndRenderTarget)
	{
		vector<RObject> bodys = world.GetBodys();
		cpSpace* space = world.GetSpace();
		//debug用的  左键添加正方形  右键添加圆形
		if (inp_mouseStateAfter.rgbButtons[0] == RSKeyDown)
		{
			cpVect pos2 = ScreenToSpace(MouseX, MouseY);
			RObject obj; obj.AddShapeAsBox(pos2.x, pos2.y, 50, 50, 1, space, 0.7);
			bodys.push_back(obj);
		}
		else if (inp_mouseStateAfter.rgbButtons[1] == RSKeyDown)
		{
			cpVect pos2 = ScreenToSpace(MouseX, MouseY);
			RObject obj; obj.AddShapeAsCircle(pos2.x, pos2.y,50, 1, space, 0.7);
			bodys.push_back(obj);
		}
		//开始绘制
		d2dHwndRenderTarget->BeginDraw();

		//开剪切模式  不绘制屏幕外面的物体
		d2dHwndRenderTarget->PushAxisAlignedClip({ 0.0f,0.0f,(float)width,(float)height }, D2D1_ANTIALIAS_MODE_ALIASED);
		D2D1_RECT_F rect = { 0,0,(float)width,(float)height };
		d2dHwndRenderTarget->Clear({ 1.0f,1.0f,1.0f,1.0f });
		
		//物理世界运动
		world.PhyStep(1.0f/60.0f);

		cpVect pos = { 0 };
		float angle = 0.0f;
		rect = { 0 };

		//获取玩家的位置
		pos = player.GetPosition();
		//根据玩家的位置 求出使玩家位于屏幕中间的 摄像机x坐标
		float cx = pos.x - (float)width / (float)2;	
		//根据玩家的位置 求出使玩家位于屏幕中间的 摄像机y坐标
		float cy = pos.y - (float)height / (float)2;
		//设置摄像机位置
		SetCamera(cx, cy);		
		//获取玩家位置以及角度
		pos = cpBodyGetPosition(player.GetBody());
		angle = cpBodyGetAngle(player.GetBody());

		//空间坐标转屏幕坐标  设置矩阵  绘制玩家
		rect = SpaceToScreen(pos.x,pos.y,player.GetWidth(),player.GetHeight());
		d2dHwndRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(RadToDeg(angle),
			{ rect.left + 15 ,rect.top + 15 }));
		d2dHwndRenderTarget->FillRectangle(rect, bitmapBrush);
		d2dHwndRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(0.0f));
		p_BlackBrush->SetColor({ 1.0f,0.0f,0.0f,1.0f });

		
		for (size_t i = 0; i < bodys.size(); i++)   //
		{

			if (RObjectType_DynamicsBox == bodys[i].GetType())
			{
				angle = cpBodyGetAngle(bodys[i].GetBody());
				pos = bodys[i].GetPosition();
		
				rect = SpaceToScreen(pos.x,pos.y,bodys[i].GetWidth(),bodys[i].GetHeight());
				d2dHwndRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(RadToDeg(angle),
					{ rect.left + 25 ,rect.top + 25 }));

				if (cpBodyIsSleeping( bodys[i].GetBody()))
				{
					d2dHwndRenderTarget->FillRectangle(rect, p_BlackBrush);
				}
				else
				{
					d2dHwndRenderTarget->FillRectangle(rect, bitmapBrush);
				}

				d2dHwndRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(0.0f));
			}

			else if (bodys[i].GetType() == RObjectType_DynamicsCircle)
			{
				pos = cpBodyGetPosition(bodys[i].GetBody());
				D2D1_ELLIPSE ellip = { 0 };
				ellip.point = SpaceToScreen(pos.x,pos.y);
				ellip.radiusX = 50.0f;
				ellip.radiusY = 50.0f;

				d2dHwndRenderTarget->FillEllipse(ellip, bitmapBrush);
			}
			else if (bodys[i].GetType() == RObjectType_StaticBox)
			{				
				
				pos = bodys[i].GetPosition();
				rect = SpaceToScreen(pos.x+bodys[i].GetWidth()/2,pos.y+bodys[i].GetHeight()/2,bodys[i].GetWidth(),bodys[i].GetHeight());
				d2dHwndRenderTarget->DrawRectangle(rect, p_BlackBrush);
			}

		}
		pos = player.GetPosition();


		wchar_t wtemp[128] = L"";
		swprintf_s(wtemp, L"x:%d  y:%d", (int)pos.x, (int)pos.y);
		wstring text = wtemp;

		d2dHwndRenderTarget->DrawText(text.c_str(), text.size(), m_pText, { 100, 100, 200,200 }, p_BlackBrush);

		d2dHwndRenderTarget->PopAxisAlignedClip();
		if (Present)
		{
			d2dHwndRenderTarget->EndDraw();
		}

	}
	ULONGLONG te = GetTickCount64();
	DWORD t2 = (DWORD)(te - ts);
	if (t2 < frameRate)
	{
		Sleep((DWORD)(frameRate - t2));
	}
	NowSec = GetTickCount64();
	if (NowSec - lastSec >= 1000)
	{
		lastSec = NowSec;
		wchar_t btemp2[128] = L"";
		swprintf_s(btemp2, L"FPS:%d", frame);
		wstring btemp3 = btemp2;
		SetWindowText(mainWindow, btemp3.c_str());
		frame = 0;		
	}
	else
		frame++;
	
}
