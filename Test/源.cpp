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
//	if (!d2dHwndRenderTarget)//如果还没初始化   (防止重复初始化)
//	{
//		if (mainWindow)//如果窗口句柄不为空就创建窗口句柄的
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
//    // 主消息循环:
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
//    //文件句柄  
//    long   hFile = 0;
//    //文件信息  
//    struct _finddata_t fileinfo;
//    string p;
//    if ((hFile = _findfirst(p.assign(path).append("\\*.uc").c_str(), &fileinfo)) != -1)
//    {
//        do
//        {
//            //如果是目录,迭代之  
//            //如果不是,加入列表  
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
//    char input[260] = "\0";  //接受用户输入的字符串
//    if (inputfr == nullptr) return -1;
//    fread(input,1,260,inputfr);
//
//    FILE* fr = nullptr;//用于读取文件结构体
//    FILE* fr2 = nullptr;//用于写入文件结构体
//    
//    
//    
//    vector<string> mfiles;   //这个相当于动态的  string数组   
//    getFiles(input,mfiles);//获取目录下所有文件名字
//    string temp = "";  //中间变量
//    int succeedNum = 0;//成功的文件数
//    for (size_t i = 0; i < mfiles.size(); i++)  //循环每个.uc为后缀的文件
//    {       
//        temp = mfiles[i];   //中间变量接受
//        fopen_s(&fr, temp.c_str(), "rb+");    //打开要读取的缓存文件
//        temp = mfiles[i];
//        temp.replace(temp.end()-3,temp.end(),".mp3");   //把.uc 替换成 .mp3  然后打开(创建)将要写入的文件
//        fopen_s(&fr2, temp.c_str(), "wb+");    //创建MP3文件
//        
//        if (fr == nullptr || fr2 == nullptr) //如果打开文件失败则尝试下一个文件
//        {
//            ++i;
//            continue;
//        }
//        fseek(fr, 0, SEEK_END);   //把文件指针跳到尾端
//        int  sizei = ftell(fr);   //获取文件大小
//        fseek(fr, 0, SEEK_SET);   //文件操作指针跳到首端
//        char* bff = new char[sizei];   //申请
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

//#include<Windows.h>
//#include<iostream>
//#include<vector>
//#include<map>
//using namespace std;
//map<int, string> m1;
//struct s1
//{
//	int i;
//	string s;
//};
//vector<s1> v1;
//void TestMap()
//{
//	map<int, string>::const_iterator f1 = m1.find(m1.size()-1);
//	printf("map  :%d %s\n",f1->first,f1->second.c_str());
//}
//void TestVect()
//{
//	for (size_t i = 0; i < v1.size(); i++)
//	{
//		if (v1[i].i == v1.size()-1)
//			printf("vector:  %d  %s\n",v1[i].i,v1[i].s.c_str());
//	}
//}
//int main()
//{
//	char temp[24];
//	for (size_t i = 0; i < 1000000; i++)
//	{
//		sprintf_s(temp,"num%d",i);
//		m1.insert({ (int)i,temp});
//		v1.push_back({(int)i,temp});
//	}
//	DWORD start = GetTickCount64();
//	TestMap();
//	DWORD end = GetTickCount64();
//	printf("map test time: %d\n",end-start);
//	start = GetTickCount64();
//	TestVect();
//	end = GetTickCount64();
//	printf("vect test time: %d\n",end-start);
//	
//	return 0;
//}

//#include<windows.h>
//#include<stdio.h>
//#include<string>
//#include<vector>
//#include<d2d1.h>
//#include<map>
//#include<TlHelp32.h>
//using namespace std;
//struct ms
//{
//	string id;
//	int value;
//};
//vector<ms>  ms1;
//map<string, int> map1;
//DWORD GetProcessIdFromName(const wchar_t* name)
//{
//	HANDLE  hsnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//	if (hsnapshot == INVALID_HANDLE_VALUE)
//	{
//		//printf("CreateToolhelp32Snapshot Error!\n");
//		return 0;
//	}
//
//	PROCESSENTRY32 pe;
//	pe.dwSize = sizeof(PROCESSENTRY32);
//
//	int flag = Process32First(hsnapshot, &pe);
//
//	while (flag != 0)
//	{
//		if (lstrcmpW(pe.szExeFile, name) == 0)
//		{
//			return pe.th32ProcessID;
//		}
//		flag = Process32Next(hsnapshot, &pe);
//	}
//
//	CloseHandle(hsnapshot);
//
//	return 0;
//}
//int main()
//{
//	
//	//char temp[20] = "";
//	//for (size_t i = 0; i < 10000; i++)
//	//{
//	//	sprintf_s(temp, "id%i\0", i);
//	//	ms1.push_back({ temp,(int)i + 1 });
//	//	map1.insert({ temp,(int)i + 1 });
//	//}
//	//DWORD t1 = GetTickCount64();
//	//string ftemp = "id9999";
//	//for (size_t i = 0; i < 10000; i++)
//	//{
//	//	for (size_t j = 0; j < ms1.size(); j++)
//	//	{
//	//		if (0 == ms1[j].id.compare(ftemp))
//	//		{
//	//			//printf("%d\n", ms1[j].value);
//	//		}
//	//	}
//	//}
//	//DWORD t2 = GetTickCount64();
//	//printf("  %d ms\n", t2 - t1);
//	//t1 = GetTickCount64();
//	//for (size_t j = 0; j < 10000; j++)
//	//{
//	//	for (map<string, int>::iterator i = map1.begin(); i != map1.end(); i++)
//	//	{
//	//		if (0==i->first.compare(ftemp))
//	//		{
//	//			//printf("%d\n", i->second);
//	//		}
//	//	}
//	//}
//	//t2 = GetTickCount64();
//	//printf(" %d ms\n",t2-t1);
//	//system("pause");
//	STARTUPINFO si;
//	PROCESS_INFORMATION pi;
//	ZeroMemory(&si, sizeof(si));
//	ZeroMemory(&pi, sizeof(pi));
//	WCHAR topen[260] = TEXT(".\\nodejs\\node.exe .\\UnblockNeteaseMusic-master\\app.js");
//	printf("请进入网易云音乐的 设置->工具里面设置代理  服务器设置为 127.0.0.1  端口为 8080  后任意键继续\n");
//	//TerminateProcess();
//	WinExec("taskkill /f /im node.exe", SW_HIDE);
//	system("pause");
//	if (CreateProcess(0, topen, 0, 0, false, CREATE_NO_WINDOW, 0, 0, &si, &pi) != 0)
//	{
//		printf("运行成功\n");
//		system("pause");
//	}
//	else
//	{
//		printf("失败!\n");
//		system("pause");
//	}
//
//
//	return 0;
//}
#include"标头.h"
#include"FrameBufferObject.h"
#include"Lighting.h"
#include"Polygon.h"
using namespace std;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static HWND hWnd = 0;
static HDC g_hdc = 0;
static HGLRC g_glrc = 0;

std::vector<m_Polygon*> polygons;
std::vector<Light*> lights;
FrameBufferObject* fbo;
bool InitWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wnd = { 0 };
	wnd.cbClsExtra = 0; wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.style = CS_VREDRAW | CS_HREDRAW;
	wnd.lpfnWndProc = WndProc;
	wnd.cbWndExtra = 0;
	wnd.hInstance = hInstance;
	wnd.hIcon = 0; wnd.hCursor = LoadCursor(0, IDC_ARROW);
	wnd.hbrBackground = 0;
	wnd.lpszMenuName = 0;
	wnd.lpszClassName = L"mogl";
	if (!RegisterClassExW(&wnd))
		return false;
	hWnd = CreateWindow(L"mogl", L"", WS_POPUP, 0, 0, 1024, 768, 0, 0, hInstance, 0);
	if (hWnd == nullptr)
		return false;

	return true;
}

void InitGL()
{
	g_hdc = GetDC(hWnd);
	PIXELFORMATDESCRIPTOR pfd;
	int  iFormat;

	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	iFormat = ChoosePixelFormat(g_hdc, &pfd);
	SetPixelFormat(g_hdc, iFormat, &pfd);
	g_glrc = wglCreateContext(g_hdc);
	wglMakeCurrent(g_hdc, g_glrc);
	glewInit();
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//resizeWindow
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glViewport(0,0,1024,768);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,1024,768,0,-1.0f,1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glDisable(GL_CULL_FACE);

	m_Polygon* poly = new m_Polygon(5);
	
	poly = new m_Polygon(3);
	poly->setVertex(0, vector2f(0, 768));
	poly->setVertex(1, vector2f(1024,0 ));
	poly->setVertex(2, vector2f(0,0));
	//poly->setVertex(3, vector2f(450, 350));
	polygons.push_back(poly);

	//Create the lighting alpha texture
	//http://www.opengl.org/wiki/Framebuffer_Object_Examples

	fbo = new FrameBufferObject(1024, 768, 2);

	Light* l = new Light(vector2f(800, 600), 400, .6f);
	lights.push_back(l);

}
void Clean()
{
	delete fbo;
	wglMakeCurrent(0, 0);
	wglDeleteContext(g_glrc);
	ReleaseDC(hWnd, g_hdc);
}

void drawLighting() {
	//Set up the main fbo
	fbo->bindFrameBuffer(GL_FRAMEBUFFER_EXT);
	glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);

	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, fbo->width, fbo->height);

	//Draw each light to a secondary texture, then draw that one to the primary
	for (int i = 0; i < lights.size(); i++) {
		//Draw to secondary texture
		fbo->setRenderToTexture(1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ZERO);

		//Draw the light alpha
		glColorMask(false, false, false, true);
		lights[i]->drawAlpha(&polygons);

		//Draw the light color
		glColorMask(true, true, true, false);
		lights[i]->draw(&polygons);

		glColorMask(true, true, true, true);

		//Draw second texture to the first one
		fbo->setRenderToTexture(0);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		fbo->draw(1);
	}
	fbo->unsetRenderToTexture();
	fbo->unbindFrameBuffer(GL_FRAMEBUFFER_EXT);
	glPopAttrib();
}

void draw() {
	drawLighting();

	//Draw the scene objects
	fbo->bindFrameBuffer(GL_FRAMEBUFFER_EXT);
	fbo->setRenderToTexture(0);
	glPushAttrib(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_DST_COLOR, GL_DST_ALPHA); //Blends the scene objects very nicely with the color of the light

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	for (int i = 0; i < polygons.size(); i++) {
		float cols[] = { 1.0f, 0.0f, .5f, 1.0f,
						 1.0f, 1.0f, .5f, 1.0f,
						 0.0f, 1.0f, 1.0f, 1.0f,
						 1.0f, 0.0f, .5f, 1.0f,
						 1.0f, 0.0f, .5f, 1.0f };
		polygons[i]->draw(cols);
	}

	glPopAttrib();
	fbo->unbindFrameBuffer(GL_FRAMEBUFFER_EXT);

	fbo->draw(0);
}
void Render()
{
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	draw();
	/*glBegin(GL_TRIANGLES);
	glVertex2f(0.0f,1.0f);
	glColor4f(1.0f,0.0f,0.0f,1.0f);
	glVertex2f(1.0,0.0);
	glColor4f(0.0,1.0,0.0,1.0);
	glVertex2f(-1,0.0);
	glColor4f(0.0,0.0,1.0,1.0);
	glEnd();*/
	SwapBuffers(g_hdc);
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE PrehInstance, _In_ LPWSTR nCmdLine, _In_ int nCmdShow)
{	
	MSG msg = { 0 };
	if (!InitWindow(hInstance))
		return -1;
	ShowWindow(hWnd, SW_SHOWNORMAL);
	
	InitGL();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Render();
			Sleep(16);
		}
	}
	Clean();
	return 0;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	m_Polygon* poly=nullptr;
	switch (message)
	{
	case WM_LBUTTONDOWN:
		lights.push_back(new Light({(float)LOWORD(lParam),(float)HIWORD(lParam)},500.0f,0.4f));
		break;
	case WM_RBUTTONDOWN:
		poly = new m_Polygon(4);
		poly->setVertex(0,vector2f((float)LOWORD(lParam), (float)HIWORD(lParam)));
		poly->setVertex(1,vector2f((float)LOWORD(lParam)+20.0f, (float)HIWORD(lParam)));
		poly->setVertex(2,vector2f((float)LOWORD(lParam), (float)HIWORD(lParam)+20.0f));
		poly->setVertex(3,vector2f((float)LOWORD(lParam)+20.0f, (float)HIWORD(lParam)+20.0f));
		
		polygons.push_back(poly);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProcW(hwnd, message, wParam, lParam);
	}
}