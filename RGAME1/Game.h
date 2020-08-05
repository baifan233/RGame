#pragma once
#include"GDevices.h"
#include"World.h"
#include"ResourceManager.h"
#include"UI.h"
#include"GameLogRecorder.h"
class Game
{
public:
	void Init(GDevices* gDevices);
	void Work();
	void DealInput(RSKEYSTATE* KeyState, DIJOYSTATE2);
private:
	UIPAGE ui1;
	GameLogRecorder* gRecorder=nullptr;
	UI ui=UI();
	DWORD frameRate = 1000 / 60;
	BYTE frame = 0;
	ULONGLONG lastSec = 0;
	ULONGLONG NowSec = 0;
	float scaleSize = 2.0f;
	GDevices* devices=nullptr;
	void Draw();
	World world=World();
	ResourceManager resourceManager=ResourceManager();
};

