#include "UI.h"
UI::UI()
{
	//初始化时设置最大20个控件页	
	Page p = { 0 };
	p.bitmapOpacity = 1.0f;
	for (int i = 0; i < 20; i++)
	{
		this->pages.push_back(p);
	}
}
float GetDPI(HWND hwnd)
{
	int zoom = GetDpiForWindow(hwnd);
	float dpi = 0;
	switch (zoom) {
	case 96:
		dpi = 1;
		break;
	case 120:
		dpi = 1.25;
		break;
	case 144:
		dpi = 1.5;
		break;
	case 192:
		dpi = 2;
		break;
	default:
		dpi = -1;
		break;
	}
	return dpi;
}

bool UI::Init(GDevices* gdevices,World* world)
{
	
	if (gdevices == nullptr|| world ==nullptr)
		return false;
	this->world = world;
	this->devices = gdevices;
	RECT rect = gdevices->GetWindowRect();
	this->screenWidth = rect.right;
	this->screenHeight = rect.bottom;
	gdevices->g_GetD2DRen()->CreateSolidColorBrush({ 0.0f,0.0f,0.0f,0.0f }, &p_bControlsBrush);
	gdevices->g_GetDwriteFactory()->CreateTextFormat(L"微软雅黑", 0, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_EXTRA_CONDENSED, 30, L"", &m_pText);

	gdevices->g_GetDwriteFactory()->CreateTextFormat(L"微软雅黑", 0, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_EXTRA_CONDENSED, 30, L"", &editTextFormat);

	dpi = GetDPI(gdevices->GetMainWindow());

	if (FAILED(gdevices->g_GetDwriteFactory()->CreateTextLayout(L"", lstrlenW(L""),
		editTextFormat, 1.0f, 1.0f, &textLayout)))
		return false;
	m_pText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_pText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	editTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	editTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
	editTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);



	/*textLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	textLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	textLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);*/
	return true;
}

bool UI::SetButtonText(const wchar_t* text, size_t id)
{
	for (size_t i = 0; i < RIds.size(); i++)
	{
		if (RIds[i].id == id)
		{

			buttons[RIds[i].pointId].text = text;

			return true;
		}
	}
	return false;
}

//true变深 false变浅 CNum变色程度
D2D1_COLOR_F TurnDarkOrShallow(D2D1_COLOR_F color, bool TurnDark, float Cnum)
{
	float Num = 0;
	if (TurnDark)
		Num = -Cnum;
	else
		Num = Cnum;
	Num /= 255;
	color.r += Num;
	if (color.r >= 1.0f)color.r = 1.0f;
	else if (color.r <= 0)color.r = 0;

	color.g += Num;
	if (color.g >= 1.0f)color.g = 1.0f;
	else if (color.g <= 0)color.g = 0;

	color.b += Num;
	if (color.b >= 1.0f)color.b = 1.0f;
	else if (color.b <= 0)color.b = 0;

	return color;
}






void UI::Draw()
{

	UI* ui = this;
	float opacityTemp = 0.0f;
	
	for (size_t i = 0; i < this->uipage[0].controls.size(); i++)
	{
		uipage[0].controls[i].control->Draw(ui);
	}
	
	/*
	if (ui->pageCurrent < 20)
	{
		ID2D1DeviceContext3* context = nullptr;
		context = ui->devices->g_GetD2DRen();
		if (context)
		{
			p_bControlsBrush->SetColor(ui->pages[ui->pageCurrent].backGroundColor);
			context->FillRectangle({ 0.0f,0.0f,(float)ui->screenWidth,(float)ui->screenHeight }, ui->p_bControlsBrush);

			if (ui->pages[ui->pageCurrent].bitmap)
			{
				context->DrawBitmap(ui->pages[ui->pageCurrent].bitmap, { 0,0,(float)ui->screenWidth,(float)ui->screenHeight }, ui->pages[ui->pageCurrent].bitmapOpacity);
			}

			bool result = false;
			int widthtemp = 0;

			//D2D1_COLOR_F colorTemp = { 0 };

			//按钮部分
			POINT mpoint = { *ui->UIS.mpt->x,*ui->UIS.mpt->y };
			DIMOUSESTATE mouseState = devices->GetMouseState();
			for (size_t i = 0; i < ui->pages[ui->pageCurrent].buttonsAddr.size(); i++)
			{				

				if (ui->pages[ui->pageCurrent].buttonsAddr[i] != nullptr)
				{
					if (*ui->UIS.mpt->x == -1 || *ui->UIS.mpt->y == -1)//如果鼠标不在窗口内 则提交 鼠标不在矩形内的result且不再计算是否在矩形内
					{
						result = false;
					}
					else  //如果鼠标在窗口内 判断指针是否在控件矩形内
					{
						result = PtInControl(ui->pages[ui->pageCurrent].buttonsAddr[i]->rect, ui->dpi, mpoint);
					}
					if (!result)
					{
						ui->pages[ui->pageCurrent].buttonsAddr[i]->RbuttonState = RButtonState::Normal;
					}
					else
					{
						if (mouseState.rgbButtons[0] != RSKeyDown && mouseState.rgbButtons[0] != RSKeyUp)//如果鼠标在矩形内但是没按下
						{
							ui->pages[ui->pageCurrent].buttonsAddr[i]->RbuttonState = RButtonState::MouseOn;
						}
						else
						{
							ui->pages[ui->pageCurrent].buttonsAddr[i]->RbuttonState = RButtonState::OnClick;
						}

					}
					opacityTemp = ui->pages[ui->pageCurrent].buttonsAddr[i]->bitmapOpacity;
					if (ui->pages[ui->pageCurrent].buttonsAddr[i]->RbuttonState == RButtonState::Normal)
					{
						ui->p_bControlsBrush->SetColor(ui->pages[ui->pageCurrent].buttonsAddr[i]->color);//切换画刷颜色至按钮颜色
						context->FillRectangle(ui->pages[ui->pageCurrent].buttonsAddr[i]->rect, ui->p_bControlsBrush);//画矩形		
						ui->p_bControlsBrush->SetColor(ui->pages[ui->pageCurrent].buttonsAddr[i]->Textcolor);//
						context->DrawText(ui->pages[ui->pageCurrent].buttonsAddr[i]->text, lstrlenW(ui->pages[ui->pageCurrent].buttonsAddr[i]->text), ui->m_pText, ui->pages[ui->pageCurrent].buttonsAddr[i]->rect, ui->p_bControlsBrush);
						if (ui->pages[ui->pageCurrent].buttonsAddr[i]->bitmap)//如果有图片则绘图
						{
							opacityTemp <= 0.0f ? opacityTemp = 0.0f : opacityTemp -= 0.08f;
							context->DrawBitmap(ui->pages[ui->pageCurrent].buttonsAddr[i]->bitmap, ui->pages[ui->pageCurrent].buttonsAddr[i]->rect, ui->pages[ui->pageCurrent].buttonsAddr[i]->bitmapOpacity);
						}
					}
					else if (ui->pages[ui->pageCurrent].buttonsAddr[i]->RbuttonState == RButtonState::MouseOn)
					{
						ui->p_bControlsBrush->SetColor(ui->pages[ui->pageCurrent].buttonsAddr[i]->MouseOncolor);
						context->FillRectangle(ui->pages[ui->pageCurrent].buttonsAddr[i]->rect, ui->p_bControlsBrush);//按钮颜色
						if (ui->pages[ui->pageCurrent].buttonsAddr[i]->bitmap)//如果有图片则绘图
						{
							opacityTemp >= 1.0f ? opacityTemp = 1.0f : opacityTemp += 0.08f;
							context->DrawBitmap(ui->pages[ui->pageCurrent].buttonsAddr[i]->bitmap, ui->pages[ui->pageCurrent].buttonsAddr[i]->rect, opacityTemp);
						}
						ui->p_bControlsBrush->SetColor(ui->pages[ui->pageCurrent].buttonsAddr[i]->Textcolor);//绘制文字
						context->DrawText(ui->pages[ui->pageCurrent].buttonsAddr[i]->text, lstrlenW(ui->pages[ui->pageCurrent].buttonsAddr[i]->text), ui->m_pText, ui->pages[ui->pageCurrent].buttonsAddr[i]->rect, ui->p_bControlsBrush);

					}
					else if (ui->pages[ui->pageCurrent].buttonsAddr[i]->RbuttonState == RButtonState::OnClick)
					{
						if (mouseState.rgbButtons[0] == RSKeyUp)
						{
							ui->pages[ui->pageCurrent].buttonsAddr[i]->RbuttonState = RButtonState::Normal;
							if (nullptr != ui->pages[ui->pageCurrent].buttonsAddr[i]->oc)
								ui->pages[ui->pageCurrent].buttonsAddr[i]->oc(ui->world, ui->devices, ui->pages[ui->pageCurrent].buttonsAddr[i]);
						}


						ui->p_bControlsBrush->SetColor(ui->pages[ui->pageCurrent].buttonsAddr[i]->OnClickcolor);
						context->FillRectangle(ui->pages[ui->pageCurrent].buttonsAddr[i]->rect, ui->p_bControlsBrush);
						if (ui->pages[ui->pageCurrent].buttonsAddr[i]->bitmap)//如果有图片则绘图
						{
							context->DrawBitmap(ui->pages[ui->pageCurrent].buttonsAddr[i]->bitmap, ui->pages[ui->pageCurrent].buttonsAddr[i]->rect, ui->pages[ui->pageCurrent].buttonsAddr[i]->bitmapOpacity);
						}
						ui->p_bControlsBrush->SetColor(ui->pages[ui->pageCurrent].buttonsAddr[i]->Textcolor);
						context->DrawText(ui->pages[ui->pageCurrent].buttonsAddr[i]->text, lstrlenW(ui->pages[ui->pageCurrent].buttonsAddr[i]->text), ui->m_pText, ui->pages[ui->pageCurrent].buttonsAddr[i]->rect, ui->p_bControlsBrush);
					}
					ui->pages[ui->pageCurrent].buttonsAddr[i]->bitmapOpacity = opacityTemp;

				}
			}



		}
	}
	*/
}



size_t UI::CreateButton(D2D1_COLOR_F color, D2D1_COLOR_F Textcolor, const wchar_t* text, D2D1_RECT_F rect, void(*oc)(void*,void*,void*))
{
	ButtonStruct bs = { 0 };
	RID rid = { 0 };
	rid.conTrol_type = RControlType::Button;
	rid.id = RIds.size();
	rid.pointId = buttons.size();
	bs.bitmapOpacity = 0.0f;
	bs.color = color;
	bs.Textcolor = Textcolor;
	bs.id = rid.pointId;
	bs.oc = oc;
	bs.rect = rect;
	bs.text = text;
	bs.MouseOncolor = TurnDarkOrShallow(color, false, 40);
	bs.OnClickcolor = TurnDarkOrShallow(color, true, 40);
	this->buttons.push_back(bs);
	this->RIds.push_back(rid);
	return rid.id;
}
bool UI::LoadBitmapForControl(size_t id, const wchar_t* FilePath)
{
	for (size_t i = 0; i < RIds.size(); i++)
	{
		if (RIds[i].id == id)
		{
			if (RIds[i].conTrol_type == RControlType::Button)
			{
				buttons[RIds[i].pointId].bitmap = D2DCreateBitmap(devices->g_GetD2DRen(), devices->g_GetimageFacotry(), FilePath);
				if (buttons[RIds[i].pointId].bitmap)
					return true;
			}

		}
	}
	return false;
	//return false;
}
size_t UI::CreateSlip(D2D1_COLOR_F color, D2D1_RECT_F rect, const wchar_t* text/*, bool vertical*/)
{
	SlipStruct ss = { 0 };
	RID rid = { 0 };
	ss.color = color;
	ss.rect = rect;
	ss.text = text;
	ss.id = slips.size();
	rid.conTrol_type = RControlType::Slip;
	rid.id = RIds.size();
	rid.pointId = ss.id;

	//ss.vertical = vertical;
	int width = (int)(ss.rect.right - ss.rect.left);
	width /= 10;
	ss.Width = width;
	ss.rect2.left = ss.rect.left;
	ss.rect2.right = ss.rect.left + width;
	ss.rect2.bottom = ss.rect.bottom;
	ss.rect2.top = ss.rect.top;
	slips.push_back(ss);
	RIds.push_back(rid);
	return rid.id;
}
void UI::SetEditText(size_t id, wstring text)
{
	IDWriteFactory* dWriteFactory = devices->g_GetDwriteFactory();
	for (size_t i = 0; i < RIds.size(); i++)
	{
		if (RIds[i].id == id)
		{
			edits[RIds[i].pointId].text = text;
			ReleaseCom(edits[RIds[i].pointId].textLayout);
			if (dWriteFactory)
			{
				dWriteFactory->CreateTextLayout(edits[RIds[i].pointId].text.c_str(), edits[RIds[i].pointId].text.size(), editTextFormat, 0.5f, 0.5f, &edits[RIds[i].pointId].textLayout);
			}
		}
	}
}
bool UI::LoadControlFromFile(const wchar_t* filePath)
{

	//ifstream ifile(filePath, std::ios::binary);
	//if (!ifile.is_open())
	//	return false;
	//string line = "";
	//vector<string> lines;
	//while (getline(ifile, line))
	//{
	//	lines.push_back(line);
	//}
	//vector<int> signStart;
	//vector<int> signEnd;
	//int numTemp = 0;

	//for (size_t i = 0; i < lines.size(); i++)//找出所有的初标志和末标志
	//{
	//	if (signStart.size() <= signEnd.size()) //如果尾标志的数量少于或者等于初标志的数量 那么就查找初标志
	//	{
	//		numTemp = lines[i].find("[RC]");
	//		if (-1 != numTemp)
	//		{
	//			signStart.push_back(i);
	//		}
	//	}
	//	else   //如果有初标志  那么寻找尾标志
	//	{
	//		numTemp = lines[i].find("[&RC]");
	//		if (-1 != numTemp)
	//		{
	//			signEnd.push_back(i);
	//		}
	//	}
	//}
	//if (signStart.size() == 0)//如果没有找到标志 则返回false
	//{
	//	return false;
	//}
	//if (signEnd.size() != signStart.size())//如果初标志和末标志数量不等  就报错 返回失败
	//{
	//	MessageBox(0, L"The number of the start and end signs is not the same!", L"Load Control Failed!", 0 | MB_ICONERROR);
	//	return false;
	//}


	//string varName = ""; //用来存储等号前面的值的类型
	//string var = "";  //用来存储=号后面的值
	//int search = 0;
	//for (size_t i = 0; i < lines.size(); i++)//除去特殊符
	//{
	//	search = 0;
	//	while (search != -1)
	//	{
	//		search = lines[i].find('\r');
	//		if (search != -1)lines[i].erase(lines[i].begin() + search);
	//		search = lines[i].find('\n');
	//		if (search != -1)lines[i].erase(lines[i].begin() + search);
	//		search = lines[i].find('\t');
	//		if (search != -1)lines[i].erase(lines[i].begin() + search);
	//	}
	//	
	//}
	//for (size_t i = 0; i < lines.size(); i++)  //除去空行
	//{
	//	if (lines[i] == "")
	//		lines.erase(lines.begin() + i);
	//}
	//search = -1;
	//for (size_t i = 0; i < signStart.size(); i++)//处理每一对标识符内容
	//{

	//	for (size_t j = signStart[i]; j < signEnd[i]; j++)
	//	{
	//		search = lines[j].find('=');
	//		if (-1 != search)
	//		{
	//			varName = lines[j].substr(0, search);//从头截取到等号之前
	//			var = lines[j].substr(search + 1, lines[j].size() - search); //从等号之后截取到字符串的尾部
	//			//varName.compare();
	//		}
	//	}
	//}
	return false;
}
void UI::DeleteControl(size_t id)
{

	for (size_t i = 0; i < RIds.size(); i++)
	{
		if (RIds[i].id == id)
		{

			switch (RIds[i].conTrol_type)
			{
			case RControlType::Button:
				if (RIds[i].pointId < buttons.size())
				{
					buttons.erase(buttons.begin() + RIds[i].pointId);
					for (size_t j = 0; j < pages.size(); j++)
					{
						for (size_t k = 0; k < pages[j].buttonsAddr.size(); k++)
						{
							if (pages[j].buttonsAddr[k]->id == RIds[i].pointId)
							{
								pages[j].buttonsAddr.erase(pages[j].buttonsAddr.begin() + k);
							}
						}
					}
				}
				return;
			case RControlType::Slip:
				if (RIds[i].pointId < slips.size())
				{
					slips.erase(slips.begin() + RIds[i].pointId);
					for (size_t j = 0; j < pages.size(); j++)
					{
						for (size_t k = 0; k < pages[j].slipsAddr.size(); k++)
						{
							if (pages[j].slipsAddr[k]->id == RIds[i].pointId)
							{
								pages[j].slipsAddr.erase(pages[j].slipsAddr.begin() + k);
							}
						}
					}
				}
				return;
			case RControlType::Edit:
				if (RIds[i].pointId < edits.size())
				{
					edits.erase(edits.begin() + RIds[i].pointId);
					for (size_t j = 0; j < pages.size(); j++)
					{
						for (size_t k = 0; k < pages[j].editsAddr.size(); k++)
						{
							if (pages[j].editsAddr[k]->id == RIds[i].pointId)
							{
								pages[j].editsAddr.erase(pages[j].editsAddr.begin() + k);
							}
						}
					}
				}
				return;
			}
		}
	}

}
size_t UI::CreateEdit(D2D1_RECT_F rect, D2D1_COLOR_F color)
{
	EditStruct es = { 0 };
	RID rid = { 0 };
	es.bitmapOpacity = 1.0f;
	es.cursorPos = 0;
	es.rect = rect;
	es.id = edits.size();
	rid.conTrol_type = RControlType::Edit;
	rid.pointId = es.id;
	rid.id = RIds.size();
	es.text = L"";
	es.color = color;
	devices->g_GetDwriteFactory()->CreateTextLayout(L"", lstrlenW(L""), editTextFormat, 0.5f, 0.5f, &es.textLayout);
	edits.push_back(es);
	RIds.push_back(rid);
	return rid.id;
}
int* UI::GetSlipValueAddress(size_t id)
{
	for (size_t i = 0; i < RIds.size(); i++)
	{
		if (RIds[i].id == id)
		{
			return &slips[RIds[i].pointId].value;
		}
	}
	return nullptr;
}

bool UI::SetPageForControl(size_t id, size_t pageid)
{
	if (pageid >= this->pages.size())
	{
		return false;
	}
	//order = this->GetOrderById(id, Control_Type);//根据id获取控件在vector组里的元素顺序  
	for (size_t i = 0; i < RIds.size(); i++)
	{
		if (RIds[i].id == id)
		{
			switch (RIds[i].conTrol_type)
			{
			case RControlType::Button:
				pages[pageid].buttonsAddr.push_back(&buttons[RIds[i].pointId]);  //把这个按钮的地址设置在page		
				return true;
			case  RControlType::Slip:
				pages[pageid].slipsAddr.push_back(&slips[RIds[i].pointId]);
				return true;
			case RControlType::Edit:
				pages[pageid].editsAddr.push_back(&edits[RIds[i].pointId]);
				return true;
			default:
				break;
			}
		}
	}

	return true;
}
void UI::LoadBitmapForPageBackgournd(size_t Pageid, const wchar_t* FilePath)
{
	if (Pageid >= 20)
		return;

	pages[Pageid].bitmap = D2DCreateBitmap(devices->g_GetD2DRen(), devices->g_GetimageFacotry(), FilePath);
}
UI::~UI()
{
	for (size_t i = 0; i < pages.size(); i++)
	{
		for (size_t j = 0; j < pages[i].editsAddr.size(); j++)
		{
			ReleaseCom(pages[i].editsAddr[j]->textLayout);
			pages[i].editsAddr.~vector();
		}
		for (size_t j = 0; j < pages[i].buttonsAddr.size(); j++)
		{
			pages[i].buttonsAddr.~vector();
		}
		for (size_t j = 0; j < pages[i].slipsAddr.size(); j++)
		{
			pages[i].slipsAddr.~vector();
		}
	}
	pages.~vector();
	this->editInputChar.clear();
	ReleaseCom(editTextFormat);
	ReleaseCom(this->m_pText);
	ReleaseCom(this->textLayout);


	ReleaseCom(this->p_bControlsBrush);
}

