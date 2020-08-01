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
	//����������Դ
	bool CreateTexture(const wchar_t* file, int id);
	//���id�Ƿ����
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
	//����id��ȡvector˳��
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
	//����idɾ����Դ
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
	//����vector˳���ȡ��Դstruct
	TextureResourceStruct* GetTRS(int order)
	{
		if ((size_t)order < TextureResource.size())
			return &TextureResource[order];

		return nullptr;
	}
	//��ȡ�ı���ʽ
	IDWriteTextFormat* GetTextFormat() { return textFormat; }
	ResourceManager() {};
	~ResourceManager();
private:
	IDWriteTextFormat* textFormat = nullptr;
	GDevices* devices = nullptr;
	vector<TextureResourceStruct> TextureResource;
};

