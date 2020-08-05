#include "ResourceManager.h"

void ResourceManager::Init(GDevices* devices)
{
	this->devices = devices;
	IDWriteFactory* g_dWriteFactory = devices->g_GetDwriteFactory();
	//如果Dwrite工厂为null的话就返回false
	if (g_dWriteFactory == nullptr) 
	{
		devices->GetRecorder()->LogRecord(LogType::Error,L"Init ResourceManager Failed ,the dwriteFactory is nullptr");
		return; 
	}
	
	if (FAILED(g_dWriteFactory->CreateTextFormat(L"微软雅黑", 0, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_EXTRA_CONDENSED, 30, L"", &textFormat)))  
	{
		devices->GetRecorder()->LogRecord(LogType::Error, L"Init ResourceManager Failed ,Create TextFormat Failed");
		return;
	}
	textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);  //居中对齐（垂直）
	textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER); //居中对齐（垂直）
	return;
	
}

bool ResourceManager::CreateTexture(const wchar_t* file, string id)
{	
	ID2D1Bitmap* bitmap = D2DCreateBitmap(devices->g_GetD2DRen(), devices->g_GetimageFacotry(), file);
	if (bitmap == nullptr)
	{
		this->devices->GetRecorder()->LogRecord(LogType::Error,L"Load Texture Failed!");
		return false;
	}
	ID2D1BitmapBrush* bitmapbrush;
	devices->g_GetD2DRen()->CreateBitmapBrush(bitmap, &bitmapbrush);
	TextureResource.push_back({ bitmap,bitmapbrush,id });
	return true;
}

ResourceManager::~ResourceManager()
{
	ReleaseCom(this->textFormat);
	this->TextureResource.~vector();
}
