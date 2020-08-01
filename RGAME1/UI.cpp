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

bool PtInControl(D2D1_RECT_F rect, float dpi, POINT pt)
{
	RECT rect2 = { 0 };
	rect2.left = (LONG)(rect.left * dpi);
	rect2.right = (LONG)(rect.right * dpi);
	rect2.top = (LONG)(rect.top * dpi);
	rect2.bottom = (LONG)(rect.bottom * dpi);
	bool pirReuslt = PtInRect(&rect2, pt);
	if (!pirReuslt)return false;//如果不在矩形内
	else return true;
}


void UI::Draw()
{

	UI* ui = this;
	float opacityTemp = 0.0f;
	if (ui->pageCurrent < 20)
	{
		ID2D1DeviceContext3* context = nullptr;
		context = ui->devices->g_GetD2DRen();
		if (context)
		{
			p_bControlsBrush->SetColor(ui->pages[ui->pageCurrent].backGroundColor);
			context->FillRectangle({0.0f,0.0f,(float)ui->screenWidth,(float)ui->screenHeight},ui->p_bControlsBrush);

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
				/*if (inp_mouseState.rgbButtons[1] == RSKeyDown)
				{
					if (true)
					{

						widthtemp = pages[pageCurrent].buttonsAddr[i]->rect.right - pages[pageCurrent].buttonsAddr[i]->rect.left;
						pages[pageCurrent].buttonsAddr[i]->rect.left = pt.x;
						pages[pageCurrent].buttonsAddr[i]->rect.right = pt.x + widthtemp;
						widthtemp = pages[pageCurrent].buttonsAddr[i]->rect.bottom - pages[pageCurrent].buttonsAddr[i]->rect.top;
						pages[pageCurrent].buttonsAddr[i]->rect.top = pt.y;
						pages[pageCurrent].buttonsAddr[i]->rect.bottom = pt.y + widthtemp;

					}
				}*/

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
								ui->pages[ui->pageCurrent].buttonsAddr[i]->oc(ui->world, ui->devices,ui->pages[ui->pageCurrent].buttonsAddr[i]);
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



			D2D1_POINT_2F point0 = { 0 };
			D2D1_POINT_2F point1 = { 0 };
			int slipWidth = 0;
			int slipHeight = 0;
			float value = 0;
			int sliplong = 0;
			int stemp = 0;
			for (size_t i = 0; i < ui->pages[ui->pageCurrent].slipsAddr.size(); i++)
			{
				if (*ui->UIS.mpt->x == -1 || *ui->UIS.mpt->y == -1)
					ui->pages[ui->pageCurrent].slipsAddr[i]->MouseDown = false;
				slipHeight = (int)(ui->pages[ui->pageCurrent].slipsAddr[i]->rect.bottom - ui->pages[ui->pageCurrent].slipsAddr[i]->rect.top);
				slipWidth = ui->pages[ui->pageCurrent].slipsAddr[i]->Width;
				point0.x = ui->pages[ui->pageCurrent].slipsAddr[i]->rect.left;
				point0.y = ui->pages[ui->pageCurrent].slipsAddr[i]->rect.top + slipHeight / 2;

				point1.x = ui->pages[ui->pageCurrent].slipsAddr[i]->rect.right;
				point1.y = point0.y;

				ui->p_bControlsBrush->SetColor(ui->pages[ui->pageCurrent].slipsAddr[i]->color);
				context->DrawLine(point0, point1, ui->p_bControlsBrush);
				result = PtInControl(ui->pages[ui->pageCurrent].slipsAddr[i]->rect, ui->dpi, mpoint);

				if (!result && !ui->pages[ui->pageCurrent].slipsAddr[i]->MouseDown)
				{
					//如果鼠标不在slip内 不变色
				}
				else if (!result && ui->pages[ui->pageCurrent].slipsAddr[i]->MouseDown)
				{
					if (mouseState.rgbButtons[0] == RSKey0)
						ui->pages[ui->pageCurrent].slipsAddr[i]->MouseDown = false;
					if (*ui->UIS.mpt->x < ui->pages[ui->pageCurrent].slipsAddr[i]->rect.left)
					{
						ui->pages[ui->pageCurrent].slipsAddr[i]->value = 0;
						ui->pages[ui->pageCurrent].slipsAddr[i]->rect2.left = ui->pages[ui->pageCurrent].slipsAddr[i]->rect.left;
						ui->pages[ui->pageCurrent].slipsAddr[i]->rect2.right = ui->pages[ui->pageCurrent].slipsAddr[i]->Width + ui->pages[ui->pageCurrent].slipsAddr[i]->rect.left;
					}
					else if (*ui->UIS.mpt->x > ui->pages[ui->pageCurrent].slipsAddr[i]->rect.right)
					{
						ui->pages[ui->pageCurrent].slipsAddr[i]->value = 100;
						ui->pages[ui->pageCurrent].slipsAddr[i]->rect2.left = ui->pages[ui->pageCurrent].slipsAddr[i]->rect.right - ui->pages[ui->pageCurrent].slipsAddr[i]->Width;
						ui->pages[ui->pageCurrent].slipsAddr[i]->rect2.right = ui->pages[ui->pageCurrent].slipsAddr[i]->rect.right;
					}
				}
				else
				{
					if (mouseState.rgbButtons[0] != RSKeyDown)
					{
						ui->p_bControlsBrush->SetColor(TurnDarkOrShallow(ui->pages[ui->pageCurrent].slipsAddr[i]->color, false, 40));
						if (mouseState.rgbButtons[0] == RSKey0)
							ui->pages[ui->pageCurrent].slipsAddr[i]->MouseDown = false;
					}
					else
					{
						ui->pages[ui->pageCurrent].slipsAddr[i]->MouseDown = true;

						ui->p_bControlsBrush->SetColor(TurnDarkOrShallow(ui->pages[ui->pageCurrent].slipsAddr[i]->color, true, 40));

						ui->pages[ui->pageCurrent].slipsAddr[i]->rect2.left = *ui->UIS.mpt->x / ui->dpi;
						ui->pages[ui->pageCurrent].slipsAddr[i]->rect2.left = ui->pages[ui->pageCurrent].slipsAddr[i]->rect2.left - slipWidth / 2;
						ui->pages[ui->pageCurrent].slipsAddr[i]->rect2.right = ui->pages[ui->pageCurrent].slipsAddr[i]->rect2.left + slipWidth;

						sliplong = (int)(ui->pages[ui->pageCurrent].slipsAddr[i]->rect.right - ui->pages[ui->pageCurrent].slipsAddr[i]->rect.left);
						stemp = (int)(ui->pages[ui->pageCurrent].slipsAddr[i]->rect.right - ui->pages[ui->pageCurrent].slipsAddr[i]->rect2.left);
						stemp = sliplong - stemp;
						value = (float)stemp / (float)sliplong;
						value *= 100;
						value += 5;
						ui->pages[ui->pageCurrent].slipsAddr[i]->value = (int)value;

					}
				}
				context->FillRectangle(ui->pages[ui->pageCurrent].slipsAddr[i]->rect2, ui->p_bControlsBrush);
				//swprintf_s((wchar_t*)pages[pageCurrent].slipsAddr[i]->text,20, L"%d",value);
				//hwndren->DrawTextW(pages[pageCurrent].slipsAddr[i]->text,sizeof(pages[pageCurrent].slipsAddr[i]->text),m_pText,pages[pageCurrent].slipsAddr[i]->rect,p_bControlsBrush);
			}

			//

			D2D1_ROUNDED_RECT rrect = { 0 };
			bool b = false;
			for (size_t j = 0; j < ui->pages[ui->pageCurrent].editsAddr.size(); j++)
			{
				if (ui->pages[ui->pageCurrent].editsAddr[j]->focus)
				{
					b = true;
				}
			}
			if (!b)
			{
				ui->UIS.wParam->clear();
				ui->editInputChar.clear();
			}
			for (size_t j = 0; j < ui->pages[ui->pageCurrent].editsAddr.size(); j++)
			{

				rrect.rect = ui->pages[ui->pageCurrent].editsAddr[j]->rect;
				rrect.radiusX = (ui->pages[ui->pageCurrent].editsAddr[j]->rect.right - ui->pages[ui->pageCurrent].editsAddr[j]->rect.left) / 50;
				rrect.radiusY = rrect.radiusX;

				if (*ui->UIS.mpt->x == -1 || *ui->UIS.mpt->y == -1)
					result = false;
				else
					result = PtInControl(ui->pages[ui->pageCurrent].editsAddr[j]->rect, ui->dpi, mpoint);

				if (!result)
				{
					if (!ui->pages[ui->pageCurrent].editsAddr[j]->focus)
					{
						ui->p_bControlsBrush->SetColor(ui->pages[ui->pageCurrent].editsAddr[j]->color);
						ui->p_bControlsBrush->SetOpacity(1.0f);
					}
					else
					{
						ui->p_bControlsBrush->SetColor(ui->pages[ui->pageCurrent].editsAddr[j]->color);
						ui->p_bControlsBrush->SetOpacity(0.8f);
					}
					if (mouseState.rgbButtons[0] == RSKeyDown)
						ui->pages[ui->pageCurrent].editsAddr[j]->focus = false;
				}
				else
				{
					if (!ui->pages[ui->pageCurrent].editsAddr[j]->focus)
					{
						if (mouseState.rgbButtons[0] == RSKeyDown)
						{
							ui->pages[ui->pageCurrent].editsAddr[j]->focus = true;
						}
						ui->p_bControlsBrush->SetColor(ui->pages[ui->pageCurrent].editsAddr[j]->color);
						ui->p_bControlsBrush->SetOpacity(1.0f);
					}
					else
					{
						ui->p_bControlsBrush->SetColor(ui->pages[ui->pageCurrent].editsAddr[j]->color);
						ui->p_bControlsBrush->SetOpacity(0.7f);
					}
				}
				context->FillRoundedRectangle(&rrect, ui->p_bControlsBrush);

				DWRITE_TEXT_METRICS metrics = { 0 };

				ui->pages[ui->pageCurrent].editsAddr[j]->textLayout->GetMetrics(&metrics);

				float width = ui->pages[ui->pageCurrent].editsAddr[j]->rect.right - ui->pages[ui->pageCurrent].editsAddr[j]->rect.left;

				IDWriteFactory* dWriteFactory = ui->devices->g_GetDwriteFactory();
				if (ui->pages[ui->pageCurrent].editsAddr[j]->focus)
				{
					ui->editInputChar = *ui->UIS.wParam;
					if (0 != ui->editInputChar.size())
					{
						for (size_t i = 0; i < ui->editInputChar.size(); i++)
						{
							if (ui->editInputChar[i] == 8)
							{
								if (0 != ui->pages[ui->pageCurrent].editsAddr[j]->text.size())
								{
									ui->pages[ui->pageCurrent].editsAddr[j]->text.erase(ui->pages[ui->pageCurrent].editsAddr[j]->text.end() - 1);
								}
							}
							else if (ui->editInputChar[i] == 13)
							{
								ui->pages[ui->pageCurrent].editsAddr[j]->text += L"\r\n";
							}
							else
							{
								ui->pages[ui->pageCurrent].editsAddr[j]->text.push_back(ui->editInputChar[i]);
								//layoutLock.lock();

								if (ui->pages[ui->pageCurrent].editsAddr[j]->textLayout)
									ReleaseCom(ui->pages[ui->pageCurrent].editsAddr[j]->textLayout);

								dWriteFactory->CreateTextLayout(ui->pages[ui->pageCurrent].editsAddr[j]->text.c_str(), ui->pages[ui->pageCurrent].editsAddr[j]->text.size(), ui->editTextFormat, 0.5f, 0.5f, &ui->pages[ui->pageCurrent].editsAddr[j]->textLayout);
								ui->pages[ui->pageCurrent].editsAddr[j]->textLayout->GetMetrics(&metrics);
								//layoutLock.unlock();
								if ((metrics.width / width) >= 0.95)
								{
									ui->pages[ui->pageCurrent].editsAddr[j]->text.insert(ui->pages[ui->pageCurrent].editsAddr[j]->text.end() - 2, L'\n');
								}
							}
						}
						ui->UIS.wParam->clear();
						ui->editInputChar.clear();
					}
					if (ui->pages[ui->pageCurrent].editsAddr[j]->textLayout)
						ReleaseCom(ui->pages[ui->pageCurrent].editsAddr[j]->textLayout);
					dWriteFactory->CreateTextLayout(ui->pages[ui->pageCurrent].editsAddr[j]->text.c_str(), ui->pages[ui->pageCurrent].editsAddr[j]->text.size(), ui->editTextFormat, 0.5f, 0.5f, &ui->pages[ui->pageCurrent].editsAddr[j]->textLayout);
				}


				ui->p_bControlsBrush->SetColor({ 1.0f,1.0f,1.0f,1.0f });
				ui->p_bControlsBrush->SetOpacity(1.0f);
				//layoutLock.lock();

				//context->PushAxisAlignedClip(ui->pages[ui->pageCurrent].editsAddr[i]->rect , D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
				context->DrawTextLayout({ ui->pages[ui->pageCurrent].editsAddr[j]->rect.left ,ui->pages[ui->pageCurrent].editsAddr[j]->rect.top }
				, ui->pages[ui->pageCurrent].editsAddr[j]->textLayout, ui->p_bControlsBrush);
				//context->PopAxisAlignedClip();

				//layoutLock.unlock();
				DWRITE_HIT_TEST_METRICS dhtm = { 0 };
				BOOL inside = false;
				BOOL outside = false;
				ui->textLayout->HitTestPoint(*ui->UIS.mpt->x / ui->dpi, *ui->UIS.mpt->y / ui->dpi, &inside, &outside, &dhtm);
				ui->textLayout->SetUnderline(FALSE, { 0,MAXINT });
				ui->textLayout->SetUnderline(TRUE, { dhtm.textPosition,1 });
			}


		}
	}

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

