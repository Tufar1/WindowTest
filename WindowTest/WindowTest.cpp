#include <iostream>
#include <Windows.h>
#include <thread>
#include <print>
#include <vector>
#include <d2d1.h>
#include <d2d1_1.h>
#include <wil/com.h>

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

wil::com_ptr<ID2D1Factory> pFactory = nullptr;
wil::com_ptr<ID2D1HwndRenderTarget> pRenderTarget = nullptr;
wil::com_ptr<ID2D1SolidColorBrush> pBrush = nullptr;

void Paint(HWND hwnd) {

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

    pRenderTarget->DrawRectangle(&rectangle1, pBrush.get());

    pRenderTarget->EndDraw();

    EndPaint(hwnd, &ps);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {

    case WM_DESTROY:
    {
        PostQuitMessage(0);
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
    std::println("Hello World");
    //Registrace okna, nevim proc, pry je potreba
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc; 
    wc.hInstance = GetModuleHandle(nullptr);
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
        nullptr,
        nullptr,
        nullptr,
        nullptr
    );

    //check
    if (!windowHandle) {
        std::println("{}", GetLastError()); //https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes
        return 0;
    }

    std::println("{}", static_cast<PVOID>(windowHandle));

    SetLayeredWindowAttributes(windowHandle, RGB(0, 0, 0), NULL, LWA_COLORKEY);

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);

    RECT client_rect{};
    GetClientRect(windowHandle, &client_rect);

    pFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(
            windowHandle,
            D2D1::SizeU(
                client_rect.right - client_rect.left,
                client_rect.bottom - client_rect.top)
        ),
        &pRenderTarget
    );

    pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Red),
        &pBrush
    );

    MSG msg = { };

    while (true)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (msg.message == WM_QUIT)
        {
            break;
        }

        //překreslení, řekne že je okno poškozeno a vyvolá paint
        InvalidateRect(windowHandle, nullptr, TRUE);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
