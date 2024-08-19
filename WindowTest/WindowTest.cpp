#include <iostream>
#include <Windows.h>
#include <d2d1.h>
#include <d2d1_1.h>

#pragma comment(lib, "d2d1.lib")
//Zde zkusime udelat testovaci overlay pomoci Direct2D, minula je GDI
/*

Step 1: Include Direct2D Header
Step 2: Create an ID2D1Factory
Step 3: Create an ID2D1HwndRenderTarget
Step 4: Create a Brush
Step 5: Draw the Rectangle
Step 6: Release Resources

*/

template <class T> void SafeRelease(T** ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

ID2D1Factory* pFactory = NULL;
ID2D1HwndRenderTarget* pRenderTarget = NULL;
ID2D1SolidColorBrush* pBrush = NULL;

void Paint(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);

    pFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(
            hwnd,
            D2D1::SizeU(
                rc.right - rc.left,
                rc.bottom - rc.top)
        ),
        &pRenderTarget
    );

    pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Red),
        &pBrush
    );

    PAINTSTRUCT ps;
    BeginPaint(hwnd, &ps);

    pRenderTarget->BeginDraw();
    pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

    D2D1_RECT_F rectangle1 = D2D1::RectF(
        static_cast<FLOAT>(GetSystemMetrics(SM_CXFULLSCREEN) / 2 - rand()%50),
        static_cast<FLOAT>(GetSystemMetrics(SM_CYFULLSCREEN) / 2 - 10),
        static_cast<FLOAT>(GetSystemMetrics(SM_CXFULLSCREEN) / 2 + 10),
        static_cast<FLOAT>(GetSystemMetrics(SM_CYFULLSCREEN) / 2 + 10)
    );

    pRenderTarget->DrawRectangle(&rectangle1, pBrush);

    HRESULT hr = pRenderTarget->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET)
    {
        SafeRelease(&pRenderTarget);
        SafeRelease(&pBrush);
    }

    EndPaint(hwnd, &ps);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {

    case WM_DESTROY:
    {
        PostQuitMessage(0);
        SafeRelease(&pFactory);
        SafeRelease(&pRenderTarget);
        SafeRelease(&pBrush);
        std::cout << "Zaviram okno" << std::endl;
        return 0;
    }
    case WM_PAINT:
    {
        Paint(hwnd);
        return 0;
    }
    break;
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam); //default akce pokud se mi nic nechce delat
}

int main()
{
    std::cout << "Hello World!\n";

    //Registrace okna, nevim proc, pry je potreba
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc; 
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "MojeTrida"; //To co je videt v process hackeru, musi sedet s CreateWindowEx

    RegisterClass(&wc);

    //topmost layered transparent window
    HWND windowHandle = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT, //https://learn.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
        L"MojeTrida",
        L"EspOverlay",
        WS_VISIBLE | WS_POPUP,  //https://learn.microsoft.com/en-us/windows/win32/winmsg/window-styles
        0, 0,
        GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN),
        NULL,
        NULL,
        NULL,
        NULL
    );

    //check
    if (!windowHandle) {
        std::cout << "Error: " + GetLastError() << std::endl; //https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes
        return 0;
    }

    std::cout << windowHandle << std::endl;

    SetLayeredWindowAttributes(windowHandle, RGB(0, 0, 0), NULL, LWA_COLORKEY);

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        Paint(windowHandle); //Je tohle dobry napad na kresleni esp?
        Sleep(500);
    }
}