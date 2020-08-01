

#include "RGAME1.h"
#include"GDevices.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
GDevices g_Devices;


LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    // 初始化全局字符串
    LoadStringW(hInstance, IDC_RGAME1, szWindowClass, MAX_LOADSTRING);


    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RGAME1));



    //初始化设备

    g_Devices.InitMainWindow(hInstance, WndProc, L"RGame", width,height);
    g_Devices.window_Show(true);
    g_Devices.inp_InitinputDevice(hInstance, DeviceType_All);

    g_Devices.InitD2D();//初始化D2D绘图设备   

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
            g_Devices.D2DRender(true);
            g_Devices.GetDeviceData(DeviceType_KeyBoard);
            g_Devices.inp_DealInput(DeviceType_KeyBoard);
            g_Devices.GetDeviceData(DeviceType_Mouse);
            g_Devices.inp_DealInput(DeviceType_Mouse);
            g_Devices.GetDeviceData(DeviceType_Joy);
            g_Devices.inp_DealInput(DeviceType_Joy);
        }
    }
    //g_Devices.inp_ShutDown();
    return (int)msg.wParam;
}

bool cleared = false;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    
   /* if (!HasFocus())
    {
        if (!cleared)
            g_Devices.ClearInputData();
        cleared = true;
    }
    else
    {
        cleared = false;
    }*/
    switch (message)
    {
    case WM_MOUSEMOVE:
        //给device类传入鼠标坐标
        g_Devices.UpdateMousePoint(LOWORD(lParam),HIWORD(lParam));
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
