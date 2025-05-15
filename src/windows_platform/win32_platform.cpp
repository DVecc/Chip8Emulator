// Standard Libraries
#include <stdint.h>

// Window Libraries
#include <Windows.h>
#include "win32_platform.h"
#include <string>

//Utilities
#include "../utils/logger.h"



static Input* input = nullptr;

void platform_setInput(Input* inputPointer)
{
    input = inputPointer;
}

int MapVKToChip8Key(int vk) {
    switch (vk) {
        case '1': return 0x1;
        case '2': return 0x2;
        case '3': return 0x3;
        case '4': return 0xC;
        case 'Q': return 0x4;
        case 'W': return 0x5;
        case 'E': return 0x6;
        case 'R': return 0xD;
        case 'A': return 0x7;
        case 'S': return 0x8;
        case 'D': return 0x9;
        case 'F': return 0xE;
        case 'Z': return 0xA;
        case 'X': return 0x0;
        case 'C': return 0xB;
        case 'V': return 0xF;
        default: return -1;
    }
}

LRESULT CALLBACK platform_window_callback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        
        case WM_KEYDOWN:
        {

        
            int vk = (int)wParam;

            // Map VK to CHIP-8 key (you'll need to define this function)
            int chip8Key = MapVKToChip8Key(vk);
            LOG_TRACE(L"VK: %d down", chip8Key);
            if (chip8Key != -1 && input)
                input->onKeyDown((uint8_t)chip8Key);

            break;
        }
        case WM_KEYUP:
        {
            int vk = (int)wParam;
            int chip8Key = MapVKToChip8Key(vk);

            if (chip8Key != -1 && input)
                input->onKeyUp((uint8_t)chip8Key);

            else if (vk == VK_ESCAPE)
            {
                running = false;
            }
            break;
        }
        
        case WM_CLOSE:
            platform_cleanup();
            break;

        case WM_SIZE:
            framebufferResized = true;
            break;
   
    }

    return DefWindowProc(window, msg, wParam, lParam);
};

bool platform_create_window(HWND* window, int windowWidth, int windowHeight)
{
    HINSTANCE instance = GetModuleHandle(nullptr);

    WNDCLASS wc = {};
    wc.lpfnWndProc = platform_window_callback;
    wc.hInstance = instance;
    wc.lpszClassName = L"vulkan_engine_class";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc))
    {
        MessageBoxEx(0, L"Failed to register window class", L"Error", MB_ICONEXCLAMATION | MB_OK, 0);
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

    *window = CreateWindowEx(
        WS_EX_APPWINDOW,
        L"vulkan_engine_class",
        L"Chip8Emulator",
        windowStyle, 100, 100, windowWidth, windowHeight, 0, 0, instance, 0);

    if (window == 0)
    {
        MessageBoxEx(0, L"Failed to create Window", L"Error", MB_ICONEXCLAMATION | MB_OK, 0);
        return false;
    }

    ShowWindow(*window, SW_SHOW);
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

    case TEXT_COLOR_BLUE:
        colorBits = FOREGROUND_BLUE;
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

void platform_get_window_size(HWND window, uint32_t *width, uint32_t *height)
{
    RECT rect;
    GetClientRect(window, &rect);

    *width = rect.right - rect.left;
    *height = rect.bottom - rect.top;
}
