#pragma once

#include"GDevices.h"
#include"World.h"
#define buttonFlag_Normal 0
#define buttonFlag_OnClick 1
#define buttonFlag_MouseOn 2
enum class RButtonState
{
	Normal = 0,
	OnClick = 1,
	MouseOn = 2,
};
enum class RControlType
{
	Button = 0,
	Slip = 1,
	Edit = 2,
};
struct ControlDescStruct
{
	BYTE type;
	BYTE page;
	D2D1_COLOR_F NormalColor;
	D2D1_COLOR_F MouseOnColor;
	D2D1_COLOR_F OnClickColor;
	ID2D1Bitmap* NormalBitmap;
	ID2D1Bitmap* MouseOnBitmap;
	ID2D1Bitmap* OnClickBitmap;
	int transformTime;

};
struct ButtonStruct
{
	float bitmapOpacity = 1.0f;
	ID2D1Bitmap* bitmap;
	D2D1_COLOR_F color = { 0 };
	D2D1_COLOR_F MouseOncolor = { 0 };
	D2D1_COLOR_F OnClickcolor = { 0 };
	D2D1_COLOR_F Textcolor = { 0 };
	D2D1_RECT_F rect = { 0 };
	size_t id = 0;
	void(*oc)(void*, void*, void*) = 0;
	const wchar_t* text = NULL;
	RButtonState RbuttonState;
};
struct SlipStruct
{
	D2D1_RECT_F rect = { 0 };
	D2D1_RECT_F rect2 = { 0 };//拖动小滑块的rect
	D2D1_COLOR_F color = { 0 };
	int value = 0;
	const wchar_t* text = 0;
	//bool vertical;//竖着？不然就横着
	size_t id = 0;
	int Width = 0;//小滑块宽度
	bool MouseDown = false;
};
struct EditStruct
{
	float bitmapOpacity = 1.0f;
	ID2D1Bitmap* bitmap;
	size_t id = 0;
	D2D1_COLOR_F color;
	D2D1_RECT_F rect;
	int cursorPos;
	int selectNum;
	wstring text;
	bool focus;
	bool selectMode;
	IDWriteTextLayout* textLayout;
};
struct Page
{
	float bitmapOpacity = 1.0f;
	ID2D1Bitmap* bitmap;
	vector<ButtonStruct*> buttonsAddr;
	vector<SlipStruct*> slipsAddr;
	vector<EditStruct*> editsAddr;
	D2D1_COLOR_F backGroundColor{ 1.0f,1.0f,1.0f,1.0f };
};
struct RID
{
	size_t id;
	RControlType conTrol_type;
	size_t pointId;
};
struct UIInputStruct
{
	RMouseptPointer* mpt = 0;
	vector<WPARAM>* wParam = 0;
	RSKEYSTATE* rsKeyState = 0;
};
struct TextStruct
{
	D2D1_RECT_F rect = { 0.0f };
	D2D1_COLOR_F color = { 0.0f };
	const wchar_t* text = NULL;
};
class RControls
{
public:
	RControls() {}
protected:
	float opacity = 0.0f;
	D2D1_RECT_F rect = { 0.0f };
	vector<ID2D1Bitmap**> bitmaps;
	vector<TextStruct>texts;
	vector<D2D1_COLOR_F> colors;
};
class RButton :public RControls
{
public:
	RButton() {}
protected:
	RButtonState rButtonState = RButtonState::Normal;
	D2D1_COLOR_F ButtonColor = { 0.0f };
	int transformTime = 0;
};
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
struct UIPAGE
{
	vector<RControls*> controls;
};
static UIPAGE uiReadPage(string s)
{
	int f1 = -1;
	int f2 = -1;
	char temp[24];
	string temp2 = "";
	vector<string>datas;
	float* ftemp = new float[4];
	for (size_t i = 0; i < 5; i++)
	{
		sprintf_s(temp, "Color%d\0", i);
		f1 = s.find(temp);
		if (f1 != -1)
		{
			f2 = s.find("}", f1);
			temp2 = s.substr(f1 + strlen(temp) + 2, f2 - f1 - strlen(temp) - 2);
			datas = split(temp2, ",");
			for (size_t i = 0; i < 4; i++)
			{
				ftemp[i] = (float)atof(datas[i].c_str());
			}

		}
	}

}
class UI
{
protected:
	vector<UIPAGE> uipage;
public:
	void LoadFromFile()
	{
		FILE* ifile = nullptr;
		fopen_s(&ifile, "C:\\rb1.txt", "r");
		if (ifile == nullptr) return;
		char* buffer = new char[2048];
		fread_s(buffer, 2048, 1, 2048, ifile);
		fclose(ifile);
		string b2 = buffer;
		delete[] buffer; buffer = nullptr;
		b2 = DeleteNote(b2);
		b2 = DeleteUnnecessaryChar(b2);

		uipage.push_back(uiReadPage(b2));
		
		return;
	}
	UI();
	World* world;
	bool Init(GDevices* gdevices, World* world);

	HWND hwnd = 0;
	int screenWidth = 0;
	int screenHeight = 0;
	//private:
	float dpi = 0;
	GDevices* devices = nullptr;
	//Button
public:
	void Draw();
	IDWriteTextFormat* m_pText = NULL;

	bool SetButtonText(const wchar_t* text, size_t id);
	size_t CreateButton(D2D1_COLOR_F color,
		D2D1_COLOR_F Textcolor, const wchar_t* text, D2D1_RECT_F rect, void(*oc)(void*, void*, void*));
	bool LoadBitmapForControl(size_t id, const wchar_t* FilePath);

	size_t CreateSlip(D2D1_COLOR_F color, D2D1_RECT_F rect, const wchar_t* text/*, bool vertical*/);
	int* GetSlipValueAddress(size_t id);

	UIInputStruct UIS = { 0 };
	void SetUIS(UIInputStruct uis) { UIS = uis; }
	//void Draw(DIMOUSESTATE inp_mouseState, POINT pt, RSKEYSTATE keyState);
	//画当前页  当前页可由按钮控件更改

	bool SetPageForControl(size_t id, size_t pageId);
	void SetCurrentPage(size_t pageId) { this->pageCurrent = pageId; };
	void SetPageBackGounrdColor(D2D1_COLOR_F color, size_t pageId)
	{
		if (pageId < 20)
		{
			pages[pageId].backGroundColor = color;
		}
	};
	void LoadBitmapForPageBackgournd(size_t Pageid, const wchar_t* FilePath);
	~UI();
	//private:
	int frame = 0;

	size_t pageCurrent = 0;//当前页
	ID2D1SolidColorBrush* p_bControlsBrush = nullptr;
	IDWriteTextLayout* textLayout = nullptr;
	IDWriteTextFormat* editTextFormat = nullptr;
	vector<ButtonStruct> buttons{ 0 };
	vector<SlipStruct> slips{ 0 };
	vector<Page> pages{ 0 };

	//Edit
	vector<WPARAM> editInputChar{ 0 };
	vector<EditStruct> edits{ 0 };
	wstring GetEditText(size_t id)
	{
		for (size_t i = 0; i < RIds.size(); i++)
		{
			if (RIds[i].id == id)
			{
				return  edits[RIds[i].pointId].text;
			}
		}
	}
	void SetEditText(size_t id, wstring text);
	//ID manager
	vector<RID> RIds{ 0 };
public:
	bool LoadControlFromFile(const wchar_t* filePath);
	void DeleteControl(size_t id);
	size_t CreateEdit(D2D1_RECT_F rect, D2D1_COLOR_F color);
};

