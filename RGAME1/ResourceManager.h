#pragma once
#include"GDevices.h"
struct TextureResourceStruct
{
	ID2D1Bitmap* bitmap;
	ID2D1BitmapBrush* brush;
	string id;
};
class ResourceManager
{
public:
	void LoadFormFile(const char* filePath)
	{
		FILE* ifile = nullptr;		
		fopen_s(&ifile, filePath, "r");
		if (ifile == nullptr) return;
		char* buffer = new char[4096];
		fread_s(buffer, 4096, 1, 4096, ifile);
		fclose(ifile);
		string b2 = buffer;
		delete[] buffer; buffer = nullptr;
		b2 = DeleteNote(b2);
		b2 = DeleteUnnecessaryChar(b2);
		int f1 = 0;
		int f2 = 0;
		string stemp = "";
		wchar_t wtemp[260];
		vector<string> datas;
		datas = split(b2,";");
		do 
		{
			for (size_t i = 0; i < datas.size(); i++)
			{
				f1 = datas[i].find("=LoadTexture(");  //=Load("
				if (f1 != -1)
				{
					f2 = datas[i].find(")");
					stemp = datas[i].substr(f1 + 14, f2 - f1 - 15);
					swprintf_s(wtemp, L"%hs\0", stemp.c_str());
					CreateTexture(wtemp, datas[i].substr(0, f1));
				}
			}
		} while (f1!=-1);

		return;
	}
	void Init(GDevices* devices);
	//创建纹理资源
	bool CreateTexture(const wchar_t* file, string id);	
	//根据id获取vector顺序
	size_t GetVectOrderById(string id)
	{
		for (size_t i = 0; i < TextureResource.size(); i++)
		{
			if (TextureResource[i].id.compare(id)==0)
			{
				return i;
			}
		}
		return -1;
	}
	TextureResourceStruct GetTexture(string id)
	{
		for (size_t i = 0; i < TextureResource.size(); i++)
		{
			if (TextureResource[i].id.compare(id) == 0)
			{
				return TextureResource[i];
			}
		}
	}
	//根据id删除资源
	void DeleteResource(string id)
	{		
		for (size_t i = 0; i < TextureResource.size(); i++)
		{
			if (TextureResource[i].id.compare(id) == 0)
			{
				ReleaseCom(TextureResource[i].bitmap);
				ReleaseCom(TextureResource[i].brush);
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

