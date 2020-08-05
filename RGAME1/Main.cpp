

#include"Resource.h"
#include"GDevices.h"
#include"Game.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szWindowClass[MAX_LOADSTRING];            
GDevices g_Devices;


LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

vector<WPARAM> charwParam;//用于储存窗口过程输入字符

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)       //WinMain  程序主入口
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // 初始化全局字符串
    LoadStringW(hInstance, IDC_RGAME1, szWindowClass, MAX_LOADSTRING);
    

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RGAME1));
    

    //初始化设备
                            //实例句柄  窗口过程  窗口类名   窗口宽度       窗口高度
    g_Devices.InitMainWindow(hInstance, WndProc, L"RGame", screenWidth, screenHeight);   //初始化窗口

    g_Devices.window_Show(true);//显示窗口

    g_Devices.inp_InitinputDevice(hInstance, DeviceType_All);//初始化全部输入设备(包括鼠标 键盘  手柄(如果有的话))

    g_Devices.g_InitD2D();//初始化D2D绘图设备   
    
    g_Devices.SetCharParam(&charwParam);//给设备类传进窗口输入的字符(用于文本框字符输入)

    
    Game mGame;//声明游戏类
    
    mGame.Init(&g_Devices);//初始化游戏

    
    MSG msg = { 0 };

    // 主消息循环:
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            //游戏运行
            mGame.Work();
          
        }
    }    
    return (int)msg.wParam;
}

bool cleared = false;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{   
    TRACKMOUSEEVENT tme = { 0 };
    switch (message)
    {
    case WM_MOUSEMOVE:
        //给device类传入鼠标坐标
        g_Devices.UpdateMousePoint(LOWORD(lParam),HIWORD(lParam));        
        
        tme.cbSize = sizeof(tme);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = hWnd;
        tme.dwHoverTime = HOVER_DEFAULT;

        if (!TrackMouseEvent(&tme))
            return FALSE;
        break;
    case WM_MOUSELEAVE:
        g_Devices.UpdateMousePoint(-1,-1);
        g_Devices.ClearInputData();
        break;
    case WM_CHAR:
        charwParam.push_back(wParam);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
