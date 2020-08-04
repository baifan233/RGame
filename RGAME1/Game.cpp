#include "Game.h"

void changegravity(World* world, GDevices* devices, ButtonStruct* bs)
{
	if (world->IsGravityOn())
	{
		world->SetGravity(0.0f, 0.0f);
		bs->text = L"重力:关";
	}
	else
	{
		world->SetGravity(0.0f, 1000.0f);
		bs->text = L"重力:开";
	}

}
void changebuildmode(World* world, GDevices* devices, ButtonStruct* bs)
{

	world->SetBuildMode(!world->bBuildMode());
	if (world->bBuildMode())
	{
		bs->text = L"建造模式:开";
	}
	else
	{
		bs->text = L"建造模式:关";
	}
}
void build1(World* world, GDevices* devices, ButtonStruct* bs)
{
	cpVect pos = world->GetPlayer().GetPosition();
	for (size_t i = 0; i < 100; i++)
	{
		RObject obj;

		obj.AddShapeAsBox(pos.x - (double)20 * (double)i, pos.y - 200, 20, 20, world->GetSpace(), 0.7f, world->GetBodySize(), RObjectMaterial::WOOD);
		obj.CreateGeometry(obj.GetShape(), devices->g_GetFactory());
		world->AddObject(obj);
	}
}
void Game::Init(GDevices* gDevices)
{
	if (gDevices == nullptr)
		return;	
	this->devices = gDevices;

	gRecorder = gDevices->GetRecorder();
	if (gRecorder == nullptr)  return;

	resourceManager.Init(gDevices);
	world.Init(gDevices, &resourceManager);
		

	resourceManager.CreateTexture(L"bitmap2.png", 0);
	ui.Init(gDevices, &world);
	size_t b1 = ui.CreateButton({ 0.176f,0.176f,0.188f,1.0f }, { 1.0f,1.0f,1.0f,1.0f }, L"重力:开", { 100.0f,100.0f,200.0f,170.0f }, (void(*)(void*, void*, void*))changegravity);
	size_t b2 = ui.CreateButton({ 0.176f,0.176f,0.188f,1.0f }, { 1.0f,1.0f,1.0f,1.0f }, L"建造模式:关", { 100.0f,200.0f,200.0f,270.0f }, (void(*)(void*, void*, void*))changebuildmode);	

	size_t b3 = ui.CreateButton({ 0.176f,0.176f,0.188f,1.0f }, { 1.0f,1.0f,1.0f,1.0f }, L"生成100个方块", { 100.0f,300.0f,280.0f,370.0f }, (void(*)(void*, void*, void*))build1);
	ui.SetPageBackGounrdColor({ 0.176f,0.176f,0.188f,0.8f }, 0);
	ui.SetPageForControl(b1, 0);
	ui.SetCurrentPage(0);
	ui.LoadBitmapForControl(b1, L"bitmap2.png");

	ui.SetPageForControl(b2, 0);
	ui.SetCurrentPage(0);
	ui.LoadBitmapForControl(b2, L"bitmap2.png");


	ui.SetPageForControl(b3, 0);
	ui.SetCurrentPage(0);
	ui.LoadBitmapForControl(b3, L"bitmap2.png");
	UIInputStruct uis;
	uis.mpt = devices->GetMousePoint();
	uis.rsKeyState = devices->GetKeyState();
	uis.wParam = devices->GetCharParam();
	ui.SetUIS(uis);

	ui.LoadFromFile();
	//ui.CreateRenderThread();
}
static bool bdraw = true;
static bool bdrawui = false;
void Game::DealInput(RSKEYSTATE* KeyState, DIJOYSTATE2 inp_JoyState)
{
	if (KeyState[DIK_TAB] == RSKeyUp)
	{
		bdrawui = !bdrawui;
	}
	if (KeyState[DIK_Z] == RSKeyUp)
	{
		bdraw = !bdraw;
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
		if (scaleSize > 1.0f)scaleSize -= 0.01f;
	}
	if (KeyState[DIK_E] == RSKeyDown)
	{
		if (scaleSize < 3.5f)scaleSize += 0.01f;
	}
	if (KeyState[DIK_LSHIFT] == RSKeyUp)
	{
		cpVect speed = cpBodyGetVelocity(world.GetPlayer().GetBody());
		speed.x *= 10.0f;
		speed.y *= 10.0f;
		cpBodySetVelocity(world.GetPlayer().GetBody(), speed);
	}
	//else if (KeyState[DIK_G] == RSKeyUp)
	//{
	//	world.ClearSmall();
	//}
	else if (KeyState[DIK_R] == RSKeyUp)
	{
		world.SetTemp(!world.btemp);
	}
	else if (KeyState[DIK_LEFT] == RSKeyDown)
	{
		devices->MoveCamera(-1.0f, 0.0f);
	}
	else if (KeyState[DIK_RIGHT] == RSKeyDown)
	{
		devices->MoveCamera(1.0f, 0.0f);
	}
	else if (KeyState[DIK_UP] == RSKeyDown)
	{
		devices->MoveCamera(0.0f, -1.0f);
	}
	else if (KeyState[DIK_DOWN] == RSKeyDown)
	{
		devices->MoveCamera(0.0f, 1.0f);
	}


	if (inp_JoyState.lX)
	{
		world.GetPlayer().MoveX((float)inp_JoyState.lX / (float)5, 500.0f);
	}
	if (inp_JoyState.rgbButtons[0] == RSKeyUp)
	{
		//world.GetPlayer().MoveY(-50, 500.0f);
	}
	if (inp_JoyState.rgbButtons[1] == RSKeyUp)
	{
		//world.GetPlayer().MoveY(-50, 500.0f);
	}
	if (inp_JoyState.rgbButtons[2] == RSKeyUp)
	{
		//world.GetPlayer().MoveY(-50, 500.0f);
	}
	if (inp_JoyState.rgbButtons[3] == RSKeyUp)
	{
		//world.GetPlayer().MoveY(-50, 500.0f);
	}


}
void Game::Work()
{
	ULONGLONG ts = GetTickCount64();

	cpVect pos = world.GetPlayer().GetPosition();
	//根据玩家的位置 求出使玩家位于屏幕中间的 摄像机x坐标
	float cx = (float)pos.x - (float)screenWidth / 2.0f / scaleSize;
	//根据玩家的位置 求出使玩家位于屏幕中间的 摄像机y坐标
	float cy = (float)pos.y - (float)screenHeight / 2.0f / scaleSize;

	world.GetDevice()->SetCamera(cx, cy);

	world.PhyStep(1.0f / 60.0f);

	if (bdraw)
	{
		Draw();
	}

	if (!devices->UpdateDeviceData(DeviceType_Joy))
		gRecorder->LogRecord(LogType::Warning, L"Update Joy Device Failed!");	  //如果更新设备数据失败则写入警告日志
	if (!devices->UpdateDeviceData(DeviceType_KeyBoard))
		gRecorder->LogRecord(LogType::Warning, L"Update KeyBoard Device Failed!");  //如果更新设备数据失败则写入警告日志
	if (!devices->UpdateDeviceData(DeviceType_Mouse))
		gRecorder->LogRecord(LogType::Warning, L"Update Mouse Device Failed!");  //如果更新设备数据失败则写入警告日志

	devices->inp_DealInput(DeviceType_All);
	DealInput(devices->GetKeyState(), devices->GetJoyState());
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
		SetWindowText(devices->GetMainWindow(), btemp3.c_str());
		frame = 0;
	}
	else
		frame++;
}
void Game::Draw()
{

	ID2D1DeviceContext3* d2dContext = nullptr;
	d2dContext = devices->g_GetD2DRen();

	if (d2dContext)
	{
		//开始绘制
		d2dContext->BeginDraw();

		D2D1_RECT_F rect = { 0,0,(float)screenWidth,(float)screenHeight };
		d2dContext->Clear({ 1.0f,1.0f,1.0f,0.0f });

		DIMOUSESTATE mouseState = devices->GetMouseState();
		RMouseptPointer* mpt = devices->GetMousePoint();
		int MouseX = *mpt->x;
		int MouseY = *mpt->y;

		if (world.bBuildMode())
		{
			cpVect post = ScreenToSpace(MouseX, MouseY, devices->GetCameraRect(), 1.0f);  //这个样本的不要 缩放
			D2D1_RECT_F rect = SpaceToScreen((float)post.x, (float)post.y, 50, 50, devices->GetCameraRect());


			d2dContext->FillRectangle(rect, resourceManager.GetTRS(0)->brush);

			if (mouseState.rgbButtons[0] == RSKeyUp)
			{
				cpVect pos2 = ScreenToSpace(MouseX, MouseY, devices->GetCameraRect(), scaleSize);
				RObject obj; obj.AddShapeAsBox(pos2.x, pos2.y, 40, 40, world.GetSpace(), 0.7f, world.GetBodySize(), RObjectMaterial::WOOD);
				world.AddObject(obj);
			}
			else if (mouseState.rgbButtons[1] == RSKeyUp)
			{
				cpPointQueryInfo info;
				if (cpSpacePointQueryNearest(world.GetSpace(), ScreenToSpace((double)MouseX, (double)MouseY, devices->GetCameraRect(), scaleSize), 0, { CP_NO_GROUP,(cpBitmask)(1 << 31), (cpBitmask)(1 << 31) }, &info)) {
					cpBB bb = cpShapeGetBB(info.shape);
					//float mass = world.RGetObject((int)cpBodyGetUserData(cpShapeGetBody(info.shape))).GetMass();
					cpFloat cell_size = cpfmax(bb.r - bb.l, bb.t - bb.b) / 5.0f;   //    除以10.0f将会让cell更细
					if (cell_size > 5.0f) {
						ShatterShape(world.GetSpace(), (cpShape*)info.shape, cell_size, ScreenToSpace((double)MouseX, (double)MouseY, devices->GetCameraRect(), scaleSize), &world);
					}
					else {
						//				printf("Too small to splinter %f\n", cell_size);
					}
				}
			}

		}

		world.Draw(RObjectType::All, scaleSize);

		cpVect pos = world.GetPlayer().GetPosition();
		wchar_t wtemp[128] = L"";
		cpVect speed = cpBodyGetVelocity(world.GetPlayer().GetBody());
		swprintf_s(wtemp, L"x:%d \n  y:%d \n  v:%d \n objsize:%d ", (int)pos.x, (int)pos.y, (int)speed.x, (int)world.GetBodySize());
		wstring text = wtemp;
		d2dContext->DrawText(text.c_str(), text.size(), resourceManager.GetTextFormat(), { 800, 100, 900,200 }, resourceManager.GetTRS(0)->brush);

		text.clear();
		if (!world.IsGravityOn())
		{
			swprintf_s(wtemp, L"失重:打开");
			text = wtemp;
			d2dContext->DrawText(text.c_str(), text.size(), resourceManager.GetTextFormat(), { 200, 200, 400,300 }, resourceManager.GetTRS(0)->brush);
		}
		swprintf_s(wtemp, L"WASD移动 Q缩小 E放大  TAB设置");
		text = wtemp;
		d2dContext->DrawText(text.c_str(), text.size(), resourceManager.GetTextFormat(), { 0,0, 400,300 }, resourceManager.GetTRS(0)->brush);

		if (bdrawui)
		{
			ui.Draw();
		}

		d2dContext->EndDraw();
		devices->g_GetSwapChain()->Present(0, 0);


	}

}
