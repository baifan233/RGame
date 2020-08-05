#pragma once

#include"GDevices.h"
#include"World.h"
#include"ResourceManager.h"
enum class RButtonState
{
	Normal = 0,
	OnClick = 1,
	MouseOn = 2,
	ClickUp = 3
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
	string id;
	wstring text = NULL;
};

struct ActionStruct
{
	int funcNum = 0;
	vector<void*> param;
};
struct FLOAT4Struct
{
	D2D1_COLOR_F f4s;
	string id;
};

class RControls;
struct RControl
{
	RControls* control = nullptr;
	string id = 0;
};
struct UIPAGE
{
	vector<RControl> controls;
};
class UI;
static UIPAGE uiReadPage(string s, UI* ui);

class UI
{

public:	
	UIPAGE LoadFromFile()
	{
		FILE* ifile = nullptr;
		UIPAGE uipg;
		fopen_s(&ifile, "rb1.txt", "r");
		if (ifile == nullptr) return uipg;
		char* buffer = new char[4096];
		fread_s(buffer, 4096, 1, 4096, ifile);
		fclose(ifile);
		string b2 = buffer;
		delete[] buffer; buffer = nullptr;
		b2 = DeleteNote(b2);
		b2 = DeleteUnnecessaryChar(b2);		
		uipg = uiReadPage(b2, this);
		return uipg;
	}


	UIPAGE* mcurrentPage=nullptr;
	void SetUIPAGE(UIPAGE* upg) { mcurrentPage = upg; }
	UI();
	World* world=nullptr;
	ResourceManager* resourceManager;
	bool Init(GDevices* gdevices, World* world,ResourceManager* res);

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
class RControls
{
public:
	RControls() {}
	void Init(UI* ui, float roundX, float roundY, int t)
	{
		this->transformTime = t;
		this->rect.radiusX = roundX; this->rect.radiusY = roundY;
		ui->devices->g_GetFactory()->CreateRoundedRectangleGeometry(rect, &geo);
	}
	void AddFloat4(D2D1_COLOR_F float4, string id) { float4s.push_back({ float4 ,id }); }
	void AddmText(TextStruct mt) { mtexts.push_back(mt); }

	void SetBitmap(ID2D1Bitmap* bmp) { this->bitmap = bmp; }
	void SetOpacity(float o) { this->opacity = o; }
	void SetRect(D2D1_RECT_F rect) { this->rect.rect = rect; }
	void SetControlColor(D2D1_COLOR_F color) { controlColor = color; }
	void SetText(wstring text) { this->text = text; }
	void SetTextColor(D2D1_COLOR_F color) { textColor = color; }

	void MouseOnAddActionStruct(ActionStruct as) { mouseOn.push_back(as); }
	void OnClickOnAddActionStruct(ActionStruct as) { onClick.push_back(as); }
	void ClickUpAddActionStruct(ActionStruct as) { clickUp.push_back(as); }
	void NormalAddActionStruct(ActionStruct as) { normal.push_back(as); }

	vector<FLOAT4Struct>* GetFloat4() { return &float4s; }
	D2D1_COLOR_F GetControlColor() { return controlColor; }
	D2D1_COLOR_F GetTextColor() { return textColor; }

	virtual void Draw(UI* ui) {}
protected:
	ID2D1Bitmap* bitmap=nullptr;
	D2D1_COLOR_F controlColor = { 0.0f };
	D2D1_COLOR_F textColor = { 1.0f };
	wstring text = L"";
	ID2D1RoundedRectangleGeometry* geo=nullptr;
	vector<ActionStruct> mouseOn;
	vector<ActionStruct> onClick;
	vector<ActionStruct> normal;
	vector<ActionStruct> clickUp;
	vector<FLOAT4Struct> float4s;
	int transformTime = 0;
	float opacity = 0.0f;
	D2D1_ROUNDED_RECT rect = { 0 };
	vector<TextStruct>mtexts;

};
static bool PtInGeometry(ID2D1Geometry* pGeometry, D2D1_MATRIX_3X2_F& transMatrix, D2D1_POINT_2F pt)
{
	BOOL contain = FALSE;
	HRESULT hr = S_OK;

	hr = pGeometry->FillContainsPoint(pt, &transMatrix, &contain);
	if (SUCCEEDED(hr) && contain)
	{
		return true;
	}

	return false;
}

class RButton :public RControls
{
public:
	RButton() {}
	virtual void Draw(UI* ui)
	{
		if (ui->mcurrentPage == nullptr)return;
		ID2D1DeviceContext3* context = ui->devices->g_GetD2DRen();
		POINT mpoint = { *ui->UIS.mpt->x,*ui->UIS.mpt->y };
		DIMOUSESTATE mouseState = ui->devices->GetMouseState();
		bool result = false;

		if (mpoint.x == -1 || mpoint.y == -1)//如果鼠标不在窗口内 则提交 鼠标不在矩形内的result且不再计算是否在矩形内
		{
			result = false;
		}
		else  //如果鼠标在窗口内 判断指针是否在控件矩形内
		{
			D2D1_MATRIX_3X2_F matrix;
			ui->devices->g_GetD2DRen()->GetTransform(&matrix);
			if (geo)
			{
				result = PtInGeometry(geo, matrix, { (float)mpoint.x,(float)mpoint.y });
			}
		}
		if (!result)
		{
			rButtonState = RButtonState::Normal;
		}
		else
		{
			if (mouseState.rgbButtons[0] != RSKeyDown && mouseState.rgbButtons[0] != RSKeyUp)//如果鼠标在矩形内但是没按下
			{
				rButtonState = RButtonState::MouseOn;
			}
			else if (mouseState.rgbButtons[0] == RSKeyDown)
			{
				rButtonState = RButtonState::OnClick;
			}
			else
			{
				rButtonState = RButtonState::ClickUp;
			}

		}

		if (rButtonState == RButtonState::Normal)
		{
			DoFunc(normal, ui);
		}
		else if (rButtonState == RButtonState::MouseOn)
		{
			DoFunc(mouseOn, ui);
		}
		else if (rButtonState == RButtonState::OnClick)
		{
			DoFunc(onClick, ui);
		}
		else if (rButtonState == RButtonState::ClickUp)
		{
			DoFunc(clickUp, ui);
		}
		ui->devices->g_GetD2DRen()->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

		ui->p_bControlsBrush->SetColor(this->controlColor);//切换画刷颜色至按钮颜色
		ui->p_bControlsBrush->SetOpacity(opacity);//切换画刷颜色至按钮颜色
		context->FillRoundedRectangle(rect, ui->p_bControlsBrush);//画矩形		
		ui->p_bControlsBrush->SetColor({ 1.0f,1.0f,1.0f,1.0f });//切换画刷颜色至按钮颜色
		context->DrawRoundedRectangle(rect, ui->p_bControlsBrush);//画矩形		
		ID2D1Layer* layer;
		context->CreateLayer(&layer);
		context->PushLayer(D2D1::LayerParameters(rect.rect,geo),layer);
		context->DrawBitmap(bitmap,rect.rect);
		context->PopLayer();
		layer->Release();
		ui->p_bControlsBrush->SetColor(textColor);//
		context->DrawText(text.c_str(), text.size(), ui->m_pText, rect.rect, ui->p_bControlsBrush);

	}

	void DoFunc(vector<ActionStruct> as, UI* ui)
	{
		string stemp = "";
		float ftemp = 0.0f;
		float ftemp2 = 0.0f;
		D2D1_COLOR_F ctemp = { 0.0f };
		D2D1_COLOR_F controlColor = { 0.0f };
		for (size_t i = 0; i < as.size(); i++)
		{
			switch (as[i].funcNum)
			{
			case 0:  //ColorTurnTo
				stemp = *(string*)as[i].param[1];
				for (size_t j = 0; j < ui->mcurrentPage->controls.size(); j++)
				{
					if (0 == stemp.compare(ui->mcurrentPage->controls[j].id))
					{
						ctemp = *(D2D1_COLOR_F*)as[i].param[0];
						ftemp2 = (float)transformTime / 1000.0f;    ftemp2 = 1.0f / ftemp2;

						controlColor = ui->mcurrentPage->controls[j].control->GetControlColor();
						ftemp = ctemp.r - controlColor.r;
						ftemp = ftemp / 60.0f * ftemp2;

						controlColor.r += ftemp;

						ftemp = ctemp.g - controlColor.g;
						ftemp = ftemp / 60.0f * ftemp2;
						controlColor.g += ftemp;

						ftemp = ctemp.b - controlColor.b;
						ftemp = ftemp / 60.0f * ftemp2;
						controlColor.b += ftemp;

						ftemp = ctemp.a - controlColor.a;
						ftemp = ftemp / 60.0f * ftemp2;
						controlColor.a += ftemp;
						ui->mcurrentPage->controls[j].control->SetControlColor(controlColor);
					}
				}
				break;
			case 1:
				stemp = *(string*)as[i].param[1];
				for (size_t j = 0; j < ui->mcurrentPage->controls.size(); j++)
				{
					if (0 == stemp.compare(ui->mcurrentPage->controls[j].id))
					{
						ctemp = *(D2D1_COLOR_F*)as[i].param[0];
						
						ui->mcurrentPage->controls[j].control->SetControlColor(ctemp);
					}
				}
				break;
			case 2:
				for (size_t j = 0; j < ui->mcurrentPage->controls.size(); j++)
				{
					if (0 == stemp.compare(ui->mcurrentPage->controls[j].id))
					{
						ctemp = *(D2D1_COLOR_F*)as[i].param[0];
						ftemp2 = (float)transformTime / 1000.0f;    ftemp2 = 1.0f / ftemp2;

						controlColor = ui->mcurrentPage->controls[j].control->GetTextColor();
						ftemp = ctemp.r - controlColor.r;
						ftemp = ftemp / 60.0f * ftemp2;

						controlColor.r += ftemp;

						ftemp = ctemp.g - controlColor.g;
						ftemp = ftemp / 60.0f * ftemp2;
						controlColor.g += ftemp;

						ftemp = ctemp.b - controlColor.b;
						ftemp = ftemp / 60.0f * ftemp2;
						controlColor.b += ftemp;

						ftemp = ctemp.a - controlColor.a;
						ftemp = ftemp / 60.0f * ftemp2;
						controlColor.a += ftemp;
						ui->mcurrentPage->controls[j].control->SetTextColor(controlColor);
					}
				}
				break;
			default:
				break;
			}
		}



	}

protected:

	RButtonState rButtonState = RButtonState::Normal;

};
static UIPAGE uiReadPage(string s, UI* ui)
{
	int f1 = -1;
	int f2 = -1;
	int f3 = -1;
	UIPAGE uipg;

	string stemp = "";
	string id = "";

	char temp[24];
	string temp2 = "";
	vector<string>datas;
	vector<string>datas2;
	float* ftemp = new float[4];
	RButton* rb = nullptr;
	int ff1 = 0;
	int ff2 = 0;
	int transFormTime = -1;
	float roundX = -1.0f;
	float roundY = -1.0f;
	do
	{
		f1 = s.find("RButton");
		if (f1 != -1)
		{
			ff1 = f1;
			f2 = s.find("{", f1);
			f3 = s.find("}#endRButton");
			ff2 = f3 + 12;
			id = s.substr(f1 + 7, f2 - f1 - 7);
			stemp = s.substr(f2 + 1, f3 - f2 - 1);
			rb = new RButton();
			datas = split(stemp, ";");
			for (size_t i = 0; i < datas.size(); i++)
			{
				if (transFormTime == -1)
				{
					f1 = datas[i].find("transformTime");
					if (f1 != -1)
					{
						f2 = datas[i].find("=", f1);
						transFormTime = atoi(datas[i].substr(f2 + 1, f2 - f1 - 1).c_str());
					}
				}

				f1 = datas[i].find("rect");
				if (f1 != -1)
				{
					f1 = datas[i].find("{", f1);
					f2 = datas[i].find("}", f1);
					datas2 = split(datas[i].substr(f1, f2 - f1), ",");
					for (size_t j = 0; j < 4; j++)
					{
						ftemp[j] = (float)atof(datas2[j].c_str());
					}
					rb->SetRect({ ftemp[0],ftemp[1] ,ftemp[2] ,ftemp[3] });
					datas2.~vector();
				}
				if (roundX == -1.0f)
				{
					f1 = datas[i].find("roundX");
					if (f1 != -1)
					{
						f2 = datas[i].find("=", f1);
						roundX = atof(datas[i].substr(f2 + 1, f2 - f1 - 1).c_str());
					}
				}
				if (roundY == -1.0f)
				{
					f1 = datas[i].find("roundY");
					if (f1 != -1)
					{
						f2 = datas[i].find("=", f1);
						roundY = atof(datas[i].substr(f2 + 1, f2 - f1 - 1).c_str());
					}
				}
				f1 = datas[i].find("opacity");
				if (f1 != -1)
				{
					f2 = datas[i].find("=", f1);
					rb->SetOpacity(atof(datas[i].substr(f2 + 1, f2 - f1 - 1).c_str()));
				}
				f1 = datas[i].find("textColor");
				if (f1 != -1)
				{
					f1 = datas[i].find("{", f1);
					f2 = datas[i].find("}", f1);
					datas2 = split(datas[i].substr(f1 + 1, f2 - f1 - 1), ",");
					for (size_t j = 0; j < 4; j++)
					{
						ftemp[j] = (float)atof(datas2[j].c_str());
					}
					rb->SetTextColor({ ftemp[0],ftemp[1] ,ftemp[2] ,ftemp[3] });
					datas2.~vector();
				}
				else    //找不到textcolor再找text  以免 textcolor的东西把text替换了
				{
					f1 = datas[i].find("text");
					if (f1 != -1)
					{
						f1 = datas[i].find("\"", f1);
						f2 = datas[i].find("\"", f1 + 1);
						string stemp2 = datas[i].substr(f1 + 1, f2 - f1 - 1);
						wchar_t wtemp[64] = L"\0";
						swprintf_s(wtemp, 64, L"%hs\0", stemp2.c_str());
						rb->SetText(wtemp);
					}
				}


				f1 = datas[i].find("float4");
				f2 = datas[i].find("=");
				if (f1 != -1)
				{
					f3 = datas[i].find("{", f2);
					if (f3 != -1)
					{
						f2 = datas[i].find("}", f3);
						temp2 = datas[i].substr(f3 + 1, f2 - f3 - 1);
						datas2 = split(temp2, ",");
						for (size_t j = 0; j < 4; j++)
						{
							ftemp[j] = (float)atof(datas2[j].c_str());
						}
						f2 = datas[i].find("=");;
						rb->AddFloat4({ ftemp[0],ftemp[1] ,ftemp[2] ,ftemp[3] }, datas[i].substr(f1 + 6, f2 - f1 - 6));
						datas2.~vector();

					}
				}

				f1 = datas[i].find("bitmap");
				
				if (f1 != -1)
				{					
					f2 = datas[i].find("=");
					if (f2 != -1)
					{						
						temp2 = datas[i].substr(f2 +1, datas[i].size()-f2-1);																	
						rb->SetBitmap(ui->resourceManager->GetTexture(temp2).bitmap);
					}
				}

				f1 = datas[i].find("RText");
				f2 = datas[i].find("=");
				if (f1 != -1)
				{
					f3 = datas[i].find("\"", f2);
					if (f3 != -1)
					{
						f2 = datas[i].find("\"", f3 + 1);
						temp2 = datas[i].substr(f3 + 1, f2 - f3 - 1);
						wchar_t wtemp2[128];
						swprintf_s(wtemp2, 128, L"%hs\0", temp2.c_str());
						f2 = datas[i].find("=");;
						//rb->AddFloat4({ ftemp[0],ftemp[1] ,ftemp[2] ,ftemp[3] }, datas[i].substr(f1 + 6, f2 - f1 - 6));
						rb->AddmText({ datas[i].substr(f1 + 5,f2 - f1 - 5),wtemp2 });
						datas2.~vector();

					}
				}

			}
			datas.~vector();
			rb->Init(ui, roundX, roundY, transFormTime);
			roundX = -1.0f; roundY = -1.0f;
			transFormTime = -1;
			uipg.controls.push_back({ rb,id });
			s.erase(ff1, ff2 - ff1);
		}
		f1 = s.find("RButton");
	} while (f1 != -1);

	delete[] ftemp; ftemp = nullptr;

	id = "";

	datas.~vector();
	datas2.~vector();
	vector<string>ToSearch =
	{
		"MouseOn",
		"Normal",
		"OnClick",
		"ClickUp"
	};
	vector<string> definedFuncNames =
	{
		"ColorTurnTo",
		"ColorTurnToAtOnce",
		"TextColorTurnTo",
		"SetText"
	};
	do
	{
		for (size_t i1 = 0; i1 < ToSearch.size(); i1++)
		{
			f1 = s.find(ToSearch[i1]);
			if (f1 != -1)
			{
				f2 = s.rfind("#", f1);
				id = s.substr(f2 + 1, f1 - f2 - 2);
				for (size_t i = 0; i < uipg.controls.size(); i++)  //I为控件id
				{
					if (id == uipg.controls[i].id)
					{
						f1 = s.find("{", f1);
						if (f1 != -1)
						{
							f2 = s.find("}", f1);
							stemp = s.substr(f1 + 1, f2 - f1 - 1);
							datas = split(stemp, ";");
							for (size_t j = 0; j < datas.size(); j++)  //j为{}内每行的代码
							{
								f1 = datas[j].find(".");
								stemp = datas[j].substr(0, f1);
								if (f1 != -1)
								{
									for (size_t k = 0; k < uipg.controls.size(); k++)//k为控件 id
									{
										if (0 == stemp.compare(uipg.controls[k].id))
										{
											f2 = datas[j].find("(", f1);
											if (f2 != -1)
											{
												string funcName = datas[j].substr(f1 + 1, f2 - f1 - 1);
												for (size_t l = 0; l < definedFuncNames.size(); l++)  //l为函数id
												{
													if (0 == funcName.compare(definedFuncNames[l]))
													{
														f3 = datas[j].find(")");
														stemp = datas[j].substr(f2 + 1, f3 - f2 - 1);
														vector<FLOAT4Struct>* f4stemp = uipg.controls[k].control->GetFloat4();
														for (size_t m = 0; m < f4stemp->size(); m++)//m为colorid
														{
															if (0 == stemp.compare(f4stemp[0][m].id))
															{
																vector<void*> paramTemp;
																switch (l)
																{
																case 0:  //ColorTurnTo
																	paramTemp.push_back(&f4stemp[0][m].f4s);
																	paramTemp.push_back(&uipg.controls[k].id);
																	break;
																case 1:   //ColorTurnToAtOnce
																	paramTemp.push_back(&f4stemp[0][m].f4s);
																	paramTemp.push_back(&uipg.controls[k].id);
																	break;
																case 2:  //TextColorTurnTo
																	paramTemp.push_back(&f4stemp[0][m].f4s);
																	paramTemp.push_back(&uipg.controls[k].id);
																	break;
																case 3:   //SetText

																	break;
																default:
																	break;
																}
																switch (i1)
																{
																case 0:
																	uipg.controls[i].control->MouseOnAddActionStruct({ (int)l,paramTemp });
																	break;
																case 1:
																	uipg.controls[i].control->NormalAddActionStruct({ (int)l,paramTemp });
																	break;
																case 2:
																	uipg.controls[i].control->OnClickOnAddActionStruct({ (int)l,paramTemp });
																	break;
																case 3:
																	uipg.controls[i].control->ClickUpAddActionStruct({ (int)l,paramTemp });
																	break;
																default:
																	break;
																}

																paramTemp.~vector();
															}
														}

													}

												}
											}
											else
											{
												f2 = datas[i].find("=", f1);
												//if)_
											}

										}
									}
								}

							}
						}
					}
				}
			}
		}
	} while (f1 != -1);

	return uipg;
}

