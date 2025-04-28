// Standard Libraries
#include <stdint.h>

// Window Libraries
#include <Windows.h>
#include "win32_platform.h"
#include <string>

//Utilities
#include "../utils/logger.h"

LRESULT CALLBACK platform_window_callback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        /*
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            bool isDown = msg == WM_KEYDOWN ? true : false;
            uint32_t keyID = INVALID_IDX;

            switch((int)wParam)
            {
                case 'A':
                    keyID = KEY_A;
                    break;
                case 'S':
                    keyID = KEY_S;
                    break;
                case 'D':
                    keyID = KEY_D;
                    break;
                case 'W':
                    keyID = KEY_W;
                    break;
                case VK_ESCAPE:
                    keyID = KEY_ESC;
                break;
            }

            if(keyID < KEY_COUNT)
            {
                //if(input.keys[keyID].isDown != isDown)
                {
                    input.keys[keyID].isDown = isDown;
                    input.keys[keyID].halfTransitionCount++;
                }

            }
            break;
        }
        case WM_MOUSEMOVE:
        {
            input.prevMousePos = input.mousePos;
            input.mousePos = {(float)GET_X_LPARAM(lParam),
                              (float)GET_Y_LPARAM(lParam)};
        }
        break;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
        {
            bool isDown = msg == WM_LBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_RBUTTONDOWN ? true : false;
            KeyID keyID = (msg==WM_LBUTTONDOWN || msg == WM_LBUTTONUP) ? KEY_LEFT_MOUSE :
                          (msg==WM_MBUTTONDOWN || msg == WM_MBUTTONUP) ? KEY_MIDDLE_MOUSE :
                           KEY_RIGHT_MOUSE;

                           input.keys[keyID].halfTransitionCount++;
                           input.keys[keyID].isDown = isDown;

                           input.mouseClickPos = {(float)GET_X_LPARAM(lParam),
                                             (float)GET_Y_LPARAM(lParam)};
        }

        break; */
        case WM_CLOSE:
            platform_cleanup();
            break;
   
    }

    return DefWindowProc(window, msg, wParam, lParam);
};

bool platform_create_window(HWND window, int windowWidth, int windowHeight)
{
    HINSTANCE instance = GetModuleHandle(0);

    WNDCLASS wc = {};
    wc.lpfnWndProc = platform_window_callback;
    wc.hInstance = instance;
    wc.lpszClassName = L"vulkan_engine_class";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc))
    {
        MessageBoxEx(0, L"Failed to register window class", (LPCWSTR)"Error", MB_ICONEXCLAMATION | MB_OK, 0);
        return false;
    }

    uint32_t windowStyle = WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_OVERLAPPED;

    // Obtain the size of the border
    {
        RECT borderRect = {};
        AdjustWindowRectEx(&borderRect, windowStyle, 0, WS_EX_APPWINDOW);
        windowWidth += borderRect.right - borderRect.left;
        windowHeight += borderRect.bottom - borderRect.top;
    }

    window = CreateWindowEx(
        WS_EX_APPWINDOW,
        L"vulkan_engine_class",
        L"Chip8Emulator",
        windowStyle, 100, 100, windowWidth, windowHeight, 0, 0, instance, 0);

    if (window == 0)
    {
        MessageBoxEx(0, L"Failed to create Window", L"Error", MB_ICONEXCLAMATION | MB_OK, 0);
        return false;
    }

    ShowWindow(window, SW_SHOW);
    return true;
};

void platform_update_window(HWND window)
{
    MSG msg;

    while (PeekMessage(&msg, window, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
};

void platform_log(const wchar_t* msg, TextColor color)
{
    AllocConsole();
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    uint32_t colorBits = 0;

    switch (color)
    {
    case TEXT_COLOR_WHITE:
        colorBits = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
        break;

    case TEXT_COLOR_GREEN:
        colorBits = FOREGROUND_GREEN;
        break;

    case TEXT_COLOR_YELLOW:
        colorBits = FOREGROUND_BLUE | FOREGROUND_RED;
        break;

    case TEXT_COLOR_RED:
        colorBits = FOREGROUND_RED;
        break;
    
    case TEXT_COLOR_LIGHT_RED:
        colorBits = FOREGROUND_INTENSITY | FOREGROUND_RED;
        break;
    }

    SetConsoleTextAttribute(consoleHandle, colorBits);

    
    OutputDebugString(msg);
    

    WriteConsole(consoleHandle, msg, (DWORD)wcslen(msg), 0, 0);

}

void platform_cleanup()
{
 running = false;
};