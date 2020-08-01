#pragma once
#include"GDevices.h"
struct TextureResourceStruct
{
	ID2D1Bitmap* bitmap;
	ID2D1BitmapBrush* brush;
	int id;
};
class ResourceManager
{
public:
	void Init(GDevices* devices);
	//创建纹理资源
	bool CreateTexture(const wchar_t* file, int id);
	//检查id是否可用
	bool IsIdUsed(int id)
	{
		bool b = false;
		for (size_t i = 0; i < TextureResource.size(); i++)
		{
			if (TextureResource[i].id == id)
				b = true;
		}
		return b;
	}
	//根据id获取vector顺序
	size_t GetVectOrderById(int id)
	{
		for (size_t i = 0; i < TextureResource.size(); i++)
		{
			if (TextureResource[i].id == id)
			{
				return i;
			}
		}
		return -1;
	}
	//根据id删除资源
	void DeleteResource(int id)
	{
		for (size_t i = 0; i < TextureResource.size(); i++)
		{
			if (TextureResource[i].id == id)
			{
				ReleaseCom(TextureResource[i].bitmap);
				TextureResource.erase(TextureResource.begin() + i);
			}
		}
	}
	//根据vector顺序获取资源struct
	TextureResourceStruct* GetTRS(int order)
	{
		if ((size_t)order < TextureResource.size())
			return &TextureResource[order];

		return nullptr;
	}
	//获取文本格式
	IDWriteTextFormat* GetTextFormat() { return textFormat; }
	ResourceManager() {};
	~ResourceManager();
private:
	IDWriteTextFormat* textFormat = nullptr;
	GDevices* devices = nullptr;
	vector<TextureResourceStruct> TextureResource;
};

