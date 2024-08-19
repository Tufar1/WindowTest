#include <iostream>
#include <Windows.h>

/*

HWND CreateWindowExW(
  [in]           DWORD     dwExStyle,
  [in, optional] LPCWSTR   lpClassName,
  [in, optional] LPCWSTR   lpWindowName,
  [in]           DWORD     dwStyle,
  [in]           int       X,
  [in]           int       Y,
  [in]           int       nWidth,
  [in]           int       nHeight,
  [in, optional] HWND      hWndParent,
  [in, optional] HMENU     hMenu,
  [in, optional] HINSTANCE hInstance,
  [in, optional] LPVOID    lpParam
);

*/

//https://learn.microsoft.com/en-us/windows/win32/learnwin32/your-first-windows-program

// X-TOP | Y-TOP | X-BOT | Y-BOT
RECT rct_a = { 20, 20, 40, 40 };
RECT rct_b = { 40, 20, 60, 40 };

//Timhle ovladam zpravy jako resize, docela cool
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        /*
        case WM_CLOSE: //tohle se aktivuje na altf4 nebo klik krizku, DefWndProc automaticky vola destroywindow
            if (MessageBox(hwnd, "Really quit?", "My application", MB_OKCANCEL) == IDOK)
            {
                DestroyWindow(hwnd);
            }
            // Else: User canceled. Do nothing.
            return 0;
            */
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            std::cout << "Zaviram okno" << std::endl;
            return 0;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.

            FillRect(hdc, &ps.rcPaint, CreateSolidBrush(RGB(255, 0, 0))); //https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-createsolidbrush
            FrameRect(hdc, &rct_a, CreateSolidBrush(RGB(0, 0, 255)));
            FrameRect(hdc, &rct_b, CreateSolidBrush(RGB(0, 0, 255)));

            EndPaint(hwnd, &ps);
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
    wc.lpszClassName = "ClassName"; //To co je videt v process hackeru, musi sedet s CreateWindowEx

    RegisterClass(&wc);

    /*
    //https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
    HWND windowHandle = CreateWindowExW(
        0, //https://learn.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
        L"ClassName", //To co je videt v process hackeru, musi sedet s registrovanou classou
        L"WindowName", //Jmeno okna
        WS_VISIBLE | WS_SYSMENU,  //https://learn.microsoft.com/en-us/windows/win32/winmsg/window-styles
        100, 100, //Odkud zacina okno
        500, 100, //Velikost okna
        NULL,
        NULL,
        NULL,
        NULL
    );
    */


    //topmost layered transparent window
    HWND windowHandle = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT, //https://learn.microsoft.com/en-us/windows/win32/winmsg/extended-window-styles
        L"ClassName", //To co je videt v process hackeru, musi sedet s registrovanou classou
        L"WindowName", //Jmeno okna
        WS_VISIBLE | WS_POPUP,  //https://learn.microsoft.com/en-us/windows/win32/winmsg/window-styles
        0, 0, //Odkud zacina okno
        500, 100, //Velikost okna
        NULL,
        NULL,
        NULL,
        NULL
    );

    //SetLayeredWindowAttributes(windowHandle, RGB(0,0,0), 180, LWA_ALPHA);
    //s nastavenim lwa_colorkey nastavim, ktera barva bude pruhledna
    SetLayeredWindowAttributes(windowHandle, RGB(255,0,0), NULL, LWA_COLORKEY);

    //check
    if(!windowHandle){
        std::cout << "Error: " + GetLastError() << std::endl; //https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes
        return 0;
    }
    std::cout << windowHandle << std::endl;

    //neni potreba, pokud je v creatu zaple WS_VISIBLE
    //ShowWindow(windowHandle, SW_SHOW); //https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindow

    //Basic loop pro preklad zprav, prej

    /* WARNING: Don't actually write your loop this way.
    MSG msg = { };
    while (1)
    {
        GetMessage(&msg, NULL, 0, 0);
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }*/

    // Correct.

    //Jak mam odejit z loopu, aby to bylo korektni? Break nefeeluju
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
}