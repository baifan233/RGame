#include"GDevices.h"

#include"World.h"


World world;

#define MAX_VERTEXES_PER_VORONOI 16

struct WorleyContex {
	uint32_t seed;
	cpFloat cellSize;
	int width, height;
	cpBB bb;
	cpVect focus;
};

static inline cpVect
HashVect(uint32_t x, uint32_t y, uint32_t seed)
{
	//	cpFloat border = 0.21f;
	cpFloat border = 0.05f;
	uint32_t h = (x * 1640531513 ^ y * 2654435789) + seed;

	return cpv(
		cpflerp(border, 1.0f - border, (cpFloat)(h & 0xFFFF) / (cpFloat)0xFFFF),
		cpflerp(border, 1.0f - border, (cpFloat)((h >> 16) & 0xFFFF) / (cpFloat)0xFFFF)
	);
}

static cpVect
WorleyPoint(int i, int j, struct WorleyContex* context)
{
	cpFloat size = context->cellSize;
	int width = context->width;
	int height = context->height;
	cpBB bb = context->bb;

	//	cpVect fv = cpv(0.5, 0.5);
	cpVect fv = HashVect(i, j, context->seed);

	return cpv(
		cpflerp(bb.l, bb.r, 0.5f) + size * (i + fv.x - width * 0.5f),
		cpflerp(bb.b, bb.t, 0.5f) + size * (j + fv.y - height * 0.5f)
	);
}

static int
ClipCell(cpShape* shape, cpVect center, int i, int j, struct WorleyContex* context, cpVect* verts, cpVect* clipped, int count)
{
	cpVect other = WorleyPoint(i, j, context);
	//	printf("  other %dx%d: (% 5.2f, % 5.2f) ", i, j, other.x, other.y);
	if (cpShapePointQuery(shape, other, NULL) > 0.0f) {
		//		printf("excluded\n");
		memcpy(clipped, verts, count * sizeof(cpVect));
		return count;
	}
	else {
		//		printf("clipped\n");
	}

	cpVect n = cpvsub(other, center);
	cpFloat dist = cpvdot(n, cpvlerp(center, other, 0.5f));

	int clipped_count = 0;
	for (int j = 0, i = count - 1; j < count; i = j, j++) {
		cpVect a = verts[i];
		cpFloat a_dist = cpvdot(a, n) - dist;

		if (a_dist <= 0.0) {
			clipped[clipped_count] = a;
			clipped_count++;
		}

		cpVect b = verts[j];
		cpFloat b_dist = cpvdot(b, n) - dist;

		if (a_dist * b_dist < 0.0f) {
			cpFloat t = cpfabs(a_dist) / (cpfabs(a_dist) + cpfabs(b_dist));

			clipped[clipped_count] = cpvlerp(a, b, t);
			clipped_count++;
		}
	}

	return clipped_count;
}

static void
ShatterCell(cpSpace* space, cpShape* shape, cpVect cell, int cell_i, int cell_j, struct WorleyContex* context, RObjectMaterial material)
{
	//	printf("cell %dx%d: (% 5.2f, % 5.2f)\n", cell_i, cell_j, cell.x, cell.y);

	cpBody* body = cpShapeGetBody(shape);

	cpVect* ping = (cpVect*)alloca(MAX_VERTEXES_PER_VORONOI * sizeof(cpVect));
	cpVect* pong = (cpVect*)alloca(MAX_VERTEXES_PER_VORONOI * sizeof(cpVect));

	int count = cpPolyShapeGetCount(shape);
	count = (count > MAX_VERTEXES_PER_VORONOI ? MAX_VERTEXES_PER_VORONOI : count);

	for (int i = 0; i < count; i++) {
		ping[i] = cpBodyLocalToWorld(body, cpPolyShapeGetVert(shape, i));
	}

	for (int i = 0; i < context->width; i++) {
		for (int j = 0; j < context->height; j++) {
			if (
				!(i == cell_i && j == cell_j) &&
				cpShapePointQuery(shape, cell, NULL) < 0.0f
				) {
				count = ClipCell(shape, cell, i, j, context, ping, pong, count);
				memcpy(ping, pong, count * sizeof(cpVect));
			}
		}
	}

	cpVect centroid = cpCentroidForPoly(count, ping);
	cpFloat mass = cpAreaForPoly(count, ping, 0.0f) * RObjectMaterialGetMass(material);
	cpFloat moment = cpMomentForPoly(mass, count, ping, cpvneg(centroid), 0.0f);

	cpBody* new_body = cpSpaceAddBody(space, cpBodyNew(mass, moment));
	cpBodySetPosition(new_body, centroid);
	cpBodySetVelocity(new_body, cpBodyGetVelocityAtWorldPoint(body, centroid));
	cpBodySetAngularVelocity(new_body, cpBodyGetAngularVelocity(body));

	cpTransform transform = cpTransformTranslate(cpvneg(centroid));
	cpShape* new_shape = cpSpaceAddShape(space, cpPolyShapeNew(new_body, count, ping, transform, 0.0));
	// Copy whatever properties you have set on the original shape that are important
	cpShapeSetFriction(new_shape, cpShapeGetFriction(shape));
	RObject obj;
	obj.AddObj(new_body, new_shape);
	obj.SetMass(mass);
	obj.SetMaterial(material);
	obj.SetId(world.GetUsableId());
	obj.SetType(RObjectType_DynamicsBox);
	obj.CreateGeometry(obj.GetShape(),d2dFactory);
	world.AddObj(obj);
}

static void
ShatterShape(cpSpace* space, cpShape* shape, cpFloat cellSize, cpVect focus)
{
	int id = (int)cpBodyGetUserData(cpShapeGetBody(shape));
	RObject obj = world.RGetObject(id);
	RObjectMaterial material = obj.GetMaterial();
	world.RemoveObj(id);
	world.Delete(id);

	cpBB bb = cpShapeGetBB(shape);
	int width = (int)((bb.r - bb.l) / cellSize) + 1;
	int height = (int)((bb.t - bb.b) / cellSize) + 1;
	//	printf("Splitting as %dx%d\n", width, height);
	struct WorleyContex context = { rand(), cellSize, width, height, bb, focus };

	for (int i = 0; i < context.width; i++) {
		for (int j = 0; j < context.height; j++) {
			cpVect cell = WorleyPoint(i, j, &context);
			if (cpShapePointQuery(shape, cell, NULL) < 0.0f) {
				ShatterCell(space, shape, cell, i, j, &context, material);
			}
		}
	}
	//world.Free(id);


	//cpBodyFree(cpShapeGetBody(shape));
	//cpShapeFree(shape);
}

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

		if (FAILED(inp_KeyBoard->SetCooperativeLevel(mainWindow, DISCL_EXCLUSIVE | DISCL_FOREGROUND)))
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
		/*GetCursorPos(&pt);
		ClientToScreen(mainWindow,&pt);
		MouseX = pt.x;
		MouseY = pt.y;*/
		return true;
		break;
	default:
		break;
	}
	return false;
}
void GDevices::inp_DealInput(DeviceType deviceType)
{
	for (int i = 0; i < 256; i++)//���ǵ��ϴε�KeyUp��Ϣ
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


		if (KeyState[DIK_V] == RSKeyUp)
		{
			cpVect pos = world.GetPlayer().GetPosition();
			for (size_t i = 0; i < 100; i++)
			{
				RObject obj;

				obj.AddShapeAsBox(pos.x - 200*i, pos.y - 2000, 200, 200, world.GetSpace(), 0.7, world.GetBodySize(), RObjectMaterial_WOOD);
				obj.CreateGeometry(obj.GetShape(),d2dFactory);
				world.AddObject(obj);
			}



		}
		if (KeyState[DIK_W] == RSKeyDown)
		{
			world.GetPlayer().MoveY(-50, 500.0f);
		}
		if (KeyState[DIK_S] == RSKeyDown)
		{
			world.GetPlayer().MoveY(50.0f, 500.0f);
		}
		if (KeyState[DIK_A] == RSKeyDown)
		{
			world.GetPlayer().MoveX(-50.0f, 1000.0f);
		}
		if (KeyState[DIK_D] == RSKeyDown)
		{
			world.GetPlayer().MoveX(50.0f, 1000.0f);
		}
		if (KeyState[DIK_Q] == RSKeyDown)
		{
			scaleSize -= 0.01f;
		}
		if (KeyState[DIK_E] == RSKeyDown)
		{
			scaleSize += 0.01f;
		}
		if (KeyState[DIK_LSHIFT] == RSKeyUp)
		{
			cpVect speed = cpBodyGetVelocity(world.GetPlayer().GetBody());
			speed.x *= 10.0f;
			speed.y *= 10.0f;
			cpBodySetVelocity(world.GetPlayer().GetBody(), speed);
		}
		if (KeyState[DIK_F] == RSKeyUp)
		{
			bBuildMode = !bBuildMode;
		}
		else if (KeyState[DIK_G] == RSKeyUp)
		{
			world.ClearSmall();
		}
		else if (KeyState[DIK_H] == RSKeyUp)
		{
			bGravity = !bGravity;
			if (!bGravity)
			{
				world.SetGravity(0, 0);
			}
			else
			{
				world.SetGravity(0, 1000);
			}
		}
		else if (KeyState[DIK_R] == RSKeyUp)
		{
			world.SetTemp(!world.btemp);
		}
		/*else if (KeyState[DIK_LEFT] == RSKeyDown)
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
		}*/


	}
	else if (deviceType == DeviceType_Joy)
	{


		/*if (inp_JoyState.lX)
		{
			world.GetPlayer().MoveX(inp_JoyState.lX / 5);
		}
		if (inp_JoyState.rgbButtons[0] == RSKeyUp)
		{
			world.GetPlayer().MoveY(-500);
		}
		if (inp_JoyState.rgbButtons[1] == RSKeyUp)
		{
			world.GetPlayer().MoveY(500);
		}
		if (inp_JoyState.rgbButtons[2] == RSKeyUp)
		{
			world.GetPlayer().MoveX(4000);
		}
		if (inp_JoyState.rgbButtons[3] == RSKeyUp)
		{
			world.GetPlayer().MoveX(-4000);
		}*/


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
			if (RSKeyUp == inp_mouseStateAfter.rgbButtons[i])//�����ϴε�KeyUp��Ϣ ��ΪKey0
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

void GDevices::CreateWindowSizeResources()
{
	// DXGI ������
	IDXGIAdapter* pDxgiAdapter = nullptr;
	// DXGI ����
	IDXGIFactory2* pDxgiFactory = nullptr;
	// DXGI Surface ��̨����
	IDXGISurface* pDxgiBackBuffer = nullptr;
	// DXGI �豸
	IDXGIDevice1* pDxgiDevice = nullptr;

	HRESULT hr = S_OK;

	// ���֮ǰ���ڵĳ���������豸
	d2dContext->SetTarget(nullptr);
	ReleaseCom(d2dTargetBitmap);
	d3dDeviceContext->Flush();

	RECT rect = { 0 }; GetClientRect(mainWindow, &rect);

	if (SwapChain != nullptr)
	{
		// ����������Ѿ������������軺����
		hr = SwapChain->ResizeBuffers(
			2, // Double-buffered swap chain.
			lround(rect.right - rect.left),
			lround(rect.bottom - rect.top),
			DXGI_FORMAT_B8G8R8A8_UNORM,
			0);

		assert(hr == S_OK);
	}
	else
	{
		// �������Ѵ��ڵ�D3D�豸����һ���µĽ�����
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		swapChainDesc.Width = lround(rect.right - rect.left);
		swapChainDesc.Height = lround(rect.bottom - rect.top);
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.Flags = 0;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

		// ��ȡ IDXGIDevice
		if (SUCCEEDED(hr))
		{
			hr = d3dDevice->QueryInterface(IID_PPV_ARGS(&pDxgiDevice));
		}
		// ��ȡDxgi������ ���Ի�ȡ����������Ϣ
		if (SUCCEEDED(hr))
		{
			hr = pDxgiDevice->GetAdapter(&pDxgiAdapter);
		}
		// ��ȡDxgi����
		if (SUCCEEDED(hr))
		{
			hr = pDxgiAdapter->GetParent(IID_PPV_ARGS(&pDxgiFactory));
		}
		// ����������
		if (SUCCEEDED(hr))
		{
			hr = pDxgiFactory->CreateSwapChainForHwnd(
				d3dDevice,
				mainWindow,
				&swapChainDesc,
				nullptr,
				nullptr,
				&SwapChain);
		}
		// ȷ��DXGI������߲��ᳬ��һ֡
		if (SUCCEEDED(hr))
		{
			hr = pDxgiDevice->SetMaximumFrameLatency(1);
		}
	}

	// ������Ļ����
	if (SUCCEEDED(hr))
	{
		hr = SwapChain->SetRotation(DXGI_MODE_ROTATION_IDENTITY);
	}
	// ���ý�������ȡDxgi����
	if (SUCCEEDED(hr))
	{
		hr = SwapChain->GetBuffer(0, IID_PPV_ARGS(&pDxgiBackBuffer));
	}
	// ����Dxgi���洴��λͼ
	if (SUCCEEDED(hr))
	{
		D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
			96.0f,
			96.0f);
		hr = d2dContext->CreateBitmapFromDxgiSurface(
			pDxgiBackBuffer,
			&bitmapProperties,
			&d2dTargetBitmap);
	}
	// ����
	if (SUCCEEDED(hr))
	{
		// ���� Direct2D ��ȾĿ��
		d2dContext->SetTarget(d2dTargetBitmap);
	}

	ReleaseCom(pDxgiDevice);
	ReleaseCom(pDxgiAdapter);
	ReleaseCom(pDxgiFactory);
	ReleaseCom(pDxgiBackBuffer);
}

//D2D

bool GDevices::InitD2D()
{

	if (mainWindow == NULL)
		return false;
	if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory)))
		return false;

	if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(dWriteFactory),
		reinterpret_cast<IUnknown**>(&dWriteFactory))))

		return false;


	if (FAILED(dWriteFactory->CreateTextFormat(L"΢���ź�", 0, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_EXTRA_CONDENSED, 30, L"", &m_pText)))
		return false;
	m_pText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_pText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	//DXGI����
	IDXGIFactory2* dxgiFactory = 0;
	//DXGI�豸
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
		nullptr,					// ��Ϊ��ָ��ѡ��Ĭ���豸
		D3D_DRIVER_TYPE_HARDWARE,	// ǿ��ָ��Ӳ����Ⱦ
		nullptr,					// ǿ��ָ��WARP��Ⱦ D3D_DRIVER_TYPE_WARP û������ӿ�
		creationFlags,				// ����flag
		featureLevels,				// ��ʹ�õ����Եȼ��б�
		ARRAYSIZE(featureLevels),	// ���Եȼ��б���
		D3D11_SDK_VERSION,			// SDK �汾
		&d3dDevice,  //���ص�D3D11�豸ָ��
		&featurelevel,//���ص�������
		&d3dDeviceContext   //���ص�D3D11�豸������ָ��
	);
	//����IDXGIDevice
	if (SUCCEEDED(hr))
		hr = d3dDevice->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
	//����D2D�豸
	if (SUCCEEDED(hr))
		hr = d2dFactory->CreateDevice(dxgiDevice, &d2dDevice);
	//����D2D�豸������
	if (SUCCEEDED(hr))
		hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dContext);
	ReleaseCom(dxgiDevice);
	ReleaseCom(dxgiFactory);

	CreateWindowSizeResources();
	world.Init(60000, 4000);

	bitmap2 = D2DCreateBitmap(d2dContext, g_image_factory, L"bitmap2.png");
	d2dContext->CreateBitmapBrush(bitmap2, &bitmapBrush);

	d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &p_bControlsBrush);

	d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &p_BlackBrush);

	g_bitmap = D2DCreateBitmap(d2dContext, g_image_factory, L"tex.jpg");
	if (!g_bitmap)return false;
	d2dContext->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);//�����
	bitmapBrush->SetExtendModeX(D2D1_EXTEND_MODE_MIRROR);
	bitmapBrush->SetExtendModeY(D2D1_EXTEND_MODE_MIRROR);

	if (hr == E_FAIL)
		return false;
	else return true;

}

void GDevices::D2DRender(bool Present)
{
	ULONGLONG ts = GetTickCount64();


	if (d2dContext)
	{
		vector<RObject> bodys = world.GetBodys();
		cpSpace* space = world.GetSpace();
		//debug�õ�  ������������  �Ҽ����Բ��

		//��ʼ����
		d2dContext->BeginDraw();

		//������ģʽ  ��������Ļ���������
		d2dContext->PushAxisAlignedClip({ 0.0f,0.0f,(float)width,(float)height }, D2D1_ANTIALIAS_MODE_ALIASED);
		D2D1_RECT_F rect = { 0,0,(float)width,(float)height };
		d2dContext->Clear({ 1.0f,1.0f,1.0f,1.0f });
		d2dContext->DrawBitmap(g_bitmap, rect);
		if (bBuildMode)
		{
			cpVect post = ScreenToSpace(MouseX, MouseY, cameraRect, 1.0f);  //��������Ĳ�Ҫ ����
			D2D1_RECT_F rect = SpaceToScreen(post.x, post.y, 50, 50, cameraRect);


			d2dContext->FillRectangle(rect, bitmapBrush);

			if (inp_mouseStateAfter.rgbButtons[0] == RSKeyUp)
			{
				cpVect pos2 = ScreenToSpace(MouseX, MouseY, cameraRect, scaleSize);
				RObject obj; obj.AddShapeAsBox(pos2.x, pos2.y, 200, 200, space, 0.7, world.GetBodySize(), RObjectMaterial_WOOD);
				world.AddObject(obj);
			}
			else if (inp_mouseStateAfter.rgbButtons[1] == RSKeyUp)
			{
				cpPointQueryInfo info;
				if (cpSpacePointQueryNearest(space, ScreenToSpace((double)MouseX, (double)MouseY, cameraRect, scaleSize), 0, { CP_NO_GROUP,(cpBitmask)(1 << 31), (cpBitmask)(1 << 31) }, &info)) {
					cpBB bb = cpShapeGetBB(info.shape);
					//float mass = world.RGetObject((int)cpBodyGetUserData(cpShapeGetBody(info.shape))).GetMass();
					cpFloat cell_size = cpfmax(bb.r - bb.l, bb.t - bb.b) / 5.0f;
					if (cell_size > 5.0f) {
						ShatterShape(space, (cpShape*)info.shape, cell_size, ScreenToSpace((double)MouseX, (double)MouseY, cameraRect, scaleSize));
					}
					else {
						//				printf("Too small to splinter %f\n", cell_size);
					}
				}
			}

		}
		//���������˶�		
		world.PhyStep(1.0f / 60.0f);

		cpVect pos = world.GetPlayer().GetPosition();
		//������ҵ�λ�� ���ʹ���λ����Ļ�м�� �����x����
		float cx = pos.x - (float)width / 2.0f / scaleSize;
		//������ҵ�λ�� ���ʹ���λ����Ļ�м�� �����y����
		float cy = pos.y - (float)height / 2.0f / scaleSize;

		SetCamera(cx, cy);

		world.SetScaleSize(scaleSize);
		world.Draw(d2dContext, d2dFactory, RObjectType_All, bitmapBrush, cameraRect, bitmap2);

		wchar_t wtemp[128] = L"";
		cpVect speed = cpBodyGetVelocity(world.GetPlayer().GetBody());
		swprintf_s(wtemp, L"x:%d  y:%d  v:%d", (int)pos.x, (int)pos.y,(int)speed.x);
		wstring text = wtemp;
		d2dContext->DrawText(text.c_str(), text.size(), m_pText, { 800, 100, 900,200 }, p_BlackBrush);

		if (!bGravity)
		{
			swprintf_s(wtemp, L"ʧ��:��");
			text = wtemp;
			d2dContext->DrawText(text.c_str(), text.size(), m_pText, { 200, 200, 400,300 }, p_BlackBrush);
		}
		swprintf_s(wtemp, L"F�� �򿪹رս���ģʽ\n ��������� �Ҽ�����  Gɾ������ H��������  WASD�ƶ� Q��С E�Ŵ�");
		text = wtemp;
		d2dContext->DrawText(text.c_str(), text.size(), m_pText, { 0,0, 400,300 }, p_BlackBrush);

		d2dContext->PopAxisAlignedClip();
		if (Present)
		{
			d2dContext->EndDraw();
			SwapChain->Present(0, 0);
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