//#include<vector>
//#include<Windows.h>
//#include<stdio.h>
//#include<string>
//#include<wincodec.h>
//#include<d2d1.h>
//#include<dwrite.h>
//#pragma comment(lib,"d2d1.lib")
//#pragma comment(lib, "windowscodecs.lib" )
//#pragma comment(lib,"dwrite.lib")
//
//static IWICImagingFactory2* g_image_factory = NULL;
//ID2D1Bitmap* D2DCreateBitmap(ID2D1RenderTarget* d2dRenderTarget, const wchar_t* FilePath)
//{
//	ID2D1Bitmap* g_bitmap = 0;
//	if (!g_image_factory)
//	{
//		if (FAILED(CoCreateInstance(CLSID_WICImagingFactory2, NULL
//			, CLSCTX_INPROC_SERVER, __uuidof(g_image_factory)
//			, (LPVOID*)&g_image_factory)))
//			return nullptr;
//	}
//
//	IWICBitmapDecoder* bitmapdecoder = NULL;
//
//	if (FAILED(g_image_factory->CreateDecoderFromFilename(FilePath, NULL
//		, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &bitmapdecoder)))
//	{
//		MessageBox(0, L"CreateBitmap Failed! Check the bitmap file Or Code", L"Error", MB_OK | MB_ICONERROR);
//		return 0;
//	}
//	IWICBitmapFrameDecode* pframe = NULL;
//	bitmapdecoder->GetFrame(0, &pframe);
//	IWICFormatConverter* fmtcovter = NULL;	g_image_factory->CreateFormatConverter(&fmtcovter);
//
//	fmtcovter->Initialize(pframe, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone
//		, NULL, 0.0f, WICBitmapPaletteTypeCustom);
//	d2dRenderTarget->CreateBitmapFromWicBitmap(fmtcovter, NULL, &g_bitmap);
//
//	g_image_factory->Release();
//	fmtcovter->Release();
//	pframe->Release();
//	bitmapdecoder->Release();
//	return g_bitmap;
//}
//using namespace std;
//ID2D1Factory* d2dFactory = NULL;
//
//IDWriteFactory* dWriteFactory = NULL;
//
//
//ID2D1HwndRenderTarget* d2dHwndRenderTarget = NULL;
//
//ID2D1Bitmap* g_bitmap = 0;
//HWND mainWindow = 0;
//#define ReleaseCom(X){if(X)X->Release();X=0;}
//
//HRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	switch (message)
//	{
//	case WM_DESTROY:
//		PostQuitMessage(0);
//		break;
//	default:
//		return DefWindowProc(hwnd,message,wParam,lParam);
//			
//	}
//	return 0;
//}
//void Quit()
//{
//	ReleaseCom(dWriteFactory);
//	ReleaseCom(d2dHwndRenderTarget);
//	ReleaseCom(d2dFactory);
//	ReleaseCom(g_image_factory);
//	ReleaseCom(g_bitmap);
//
//}
//bool initwindow(HINSTANCE hInstance)
//{
//	WNDCLASSEX wnd = {0};
//	wnd.cbClsExtra = 0;
//	wnd.cbSize = sizeof(wnd);
//	wnd.cbWndExtra = 0;
//	wnd.hInstance = hInstance;
//	wnd.hIcon = 0;
//	wnd.hIconSm = 0;
//	wnd.hbrBackground = 0;
//	wnd.hCursor = LoadCursor(0,IDC_ARROW);
//	wnd.lpszClassName = L"ggao";
//	wnd.style = CS_HREDRAW|CS_VREDRAW;
//	wnd.lpfnWndProc = WndProc;
//	if (!RegisterClassEx(&wnd))
//		return false;
//
//	mainWindow = CreateWindow(L"ggao",0,WS_OVERLAPPEDWINDOW,0,0,1024,768,0,0,hInstance,0);
//	if (mainWindow)
//		return true;
//
//	return false;
//}
//bool initd2d()
//{
//	if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory)))
//		return false;
//
//	if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(dWriteFactory),
//		reinterpret_cast<IUnknown**>(&dWriteFactory))))
//		return false;
//
//
//	if (!d2dHwndRenderTarget)//�����û��ʼ��   (��ֹ�ظ���ʼ��)
//	{
//		if (mainWindow)//������ھ����Ϊ�վʹ������ھ����
//		{
//
//			//d2dFactory->CreateDevice
//			if (FAILED(d2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
//				D2D1::HwndRenderTargetProperties(mainWindow, D2D1::SizeU(1024,768)), &d2dHwndRenderTarget)))
//				return false;
//
//			g_bitmap = D2DCreateBitmap(d2dHwndRenderTarget, L"C:\\Program Files\\RS\\tex.jpg");
//
//			d2dHwndRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
//			if (!g_bitmap)return false;
//
//			return true;
//		}
//		else
//			return false;
//	}
//	else
//		return true;
//}
//
//void Render()
//{
//	d2dHwndRenderTarget->BeginDraw();
//	d2dHwndRenderTarget->DrawBitmap(g_bitmap);
//	d2dHwndRenderTarget->EndDraw();
//}
//
//int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
//    _In_opt_ HINSTANCE hPrevInstance,
//    _In_ LPWSTR    lpCmdLine,
//    _In_ int       nCmdShow)
//{
//    UNREFERENCED_PARAMETER(hPrevInstance);
//    UNREFERENCED_PARAMETER(lpCmdLine);
//
//	initwindow(hInstance);
//	initd2d();
//    MSG msg = { 0 };
//
//    // ����Ϣѭ��:
//    while (msg.message != WM_QUIT)
//    {
//        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
//        {
//            TranslateMessage(&msg);
//            DispatchMessage(&msg);
//        }
//        else
//        {
//			Render();
//			Sleep(32);
//        }
//    }
//    
//	Quit();
//    return (int)msg.wParam;
//}
//


//
//#include<Windows.h>
//#include<stdio.h>
//#include<fstream>
//#include<iostream>
//#include<io.h>
//#include<vector>
//using namespace std;
//void getFiles(string path, vector<string>& files)
//{
//    //�ļ����  
//    long   hFile = 0;
//    //�ļ���Ϣ  
//    struct _finddata_t fileinfo;
//    string p;
//    if ((hFile = _findfirst(p.assign(path).append("\\*.uc").c_str(), &fileinfo)) != -1)
//    {
//        do
//        {
//            //�����Ŀ¼,����֮  
//            //�������,�����б�  
//            if ((fileinfo.attrib & _A_SUBDIR))
//            {
//                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
//                    getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
//            }
//            else
//            {
//                files.push_back(p.assign(path).append("\\").append(fileinfo.name));
//            }
//        } while (_findnext(hFile, &fileinfo) == 0);
//        _findclose(hFile);
//    }
//}
//int main()
//{
//    FILE* inputfr = nullptr;
//    fopen_s(&inputfr,"a.txt", "rb+");    
//    char input[260] = "\0";  //�����û�������ַ���
//    if (inputfr == nullptr) return -1;
//    fread(input,1,260,inputfr);
//
//    FILE* fr = nullptr;//���ڶ�ȡ�ļ��ṹ��
//    FILE* fr2 = nullptr;//����д���ļ��ṹ��
//    
//    
//    
//    vector<string> mfiles;   //����൱�ڶ�̬��  string����   
//    getFiles(input,mfiles);//��ȡĿ¼�������ļ�����
//    string temp = "";  //�м����
//    int succeedNum = 0;//�ɹ����ļ���
//    for (size_t i = 0; i < mfiles.size(); i++)  //ѭ��ÿ��.ucΪ��׺���ļ�
//    {       
//        temp = mfiles[i];   //�м��������
//        fopen_s(&fr, temp.c_str(), "rb+");    //��Ҫ��ȡ�Ļ����ļ�
//        temp = mfiles[i];
//        temp.replace(temp.end()-3,temp.end(),".mp3");   //��.uc �滻�� .mp3  Ȼ���(����)��Ҫд����ļ�
//        fopen_s(&fr2, temp.c_str(), "wb+");    //����MP3�ļ�
//        
//        if (fr == nullptr || fr2 == nullptr) //������ļ�ʧ��������һ���ļ�
//        {
//            ++i;
//            continue;
//        }
//        fseek(fr, 0, SEEK_END);   //���ļ�ָ������β��
//        int  sizei = ftell(fr);   //��ȡ�ļ���С
//        fseek(fr, 0, SEEK_SET);   //�ļ�����ָ�������׶�
//        char* bff = new char[sizei];   //����
//        fread(bff, 1, sizei, fr);
//        for (int i = 0; i < sizei; i++)
//        {
//            fputc(bff[i] ^ -93, fr2);
//        }
//        delete[] bff; bff = nullptr;
//
//        fclose(fr);
//        fclose(fr2);
//        succeedNum++;
//    }
//    printf("succeed:%d   failed:%d   total:%d",succeedNum,mfiles.size()-succeedNum, mfiles.size());
//    mfiles.clear();
//	system("pause");
//	return 0;
//}

//
//#include<Windows.h>
//#include<iostream>
//using namespace std;
//class Base
//{
//public:
//	Base() {};
//	virtual void Test() { printf("Base class here\n"); };
//	void work() { Test(); }
//};
//class child:public Base
//{
//public:
//	virtual void Test()
//	{
//		printf("child class here\n");
//	}
//};
//class child2:public Base
//{
//public:
//	virtual void Test()
//	{
//		printf("child2 class here\n");
//	}
//};
//int main()
//{
//
//	char a = 12700000;
//	int b = sizeof(a);
//	Base *c1 = new child();
//	c1->work();
//	
//	Base* c2 = new child2();
//	c2->work();
//	return 0;
//}

#include<Windows.h>
#include<iostream>
#include<vector>
#include<map>
using namespace std;
map<int, string> m1;
struct s1
{
	int i;
	string s;
};
vector<s1> v1;
void TestMap()
{
	map<int, string>::const_iterator f1 = m1.find(m1.size()-1);
	printf("map  :%d %s\n",f1->first,f1->second.c_str());
}
void TestVect()
{
	for (size_t i = 0; i < v1.size(); i++)
	{
		if (v1[i].i == v1.size()-1)
			printf("vector:  %d  %s\n",v1[i].i,v1[i].s.c_str());
	}
}
int main()
{
	char temp[24];
	for (size_t i = 0; i < 1000000; i++)
	{
		sprintf_s(temp,"num%d",i);
		m1.insert({ (int)i,temp});
		v1.push_back({(int)i,temp});
	}
	DWORD start = GetTickCount64();
	TestMap();
	DWORD end = GetTickCount64();
	printf("map test time: %d\n",end-start);
	start = GetTickCount64();
	TestVect();
	end = GetTickCount64();
	printf("vect test time: %d\n",end-start);
	
	return 0;
}