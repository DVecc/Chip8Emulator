#include <windows.h>
#include "defines.h"
#include "platform.h"
#include "Game/game.cpp"
#include "renderer/vk_renderer.cpp"


global_variable bool running = TRUE;
global_variable HWND window;

LRESULT CALLBACK platform_window_callback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        running = FALSE;
    }

    return DefWindowProcA(window, msg, wParam, lParam);
}

bool platform_create_window()
{
    HINSTANCE instance = GetModuleHandleA(0);

    WNDCLASSA wc = {};
    wc.lpfnWndProc = platform_window_callback;
    wc.hInstance = instance;
    wc.lpszClassName = "vulkan_engine_class";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassA(&wc))
    {
        MessageBoxA(0, "Failed to register window class", "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    window = CreateWindowExA(
        WS_EX_APPWINDOW,
        "vulkan_engine_class",
        "Chip8Emulator",
        WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_OVERLAPPED, 100, 100, 1600, 720, 0, 0, instance, 0);

    if (window == 0)
    {
        MessageBoxA(0, "Failed to create Window", "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    ShowWindow(window, SW_SHOW);
    return true;
}

void platform_update_window(HWND window)
{
    MSG msg;

    while (PeekMessageA(&msg, window, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

int main()
{
    VkContext vkcontext = {};
    GameState gameState = {};
    if (!platform_create_window())
    {
        LOG_FATAL("Failed to open a window");
        return -1;
    }

    if (!vk_init(&vkcontext, window))
    {
        LOG_FATAL("Failed to initialize Vulkan");
        return -1;
    }

    if(!init_game(&gameState))
    {
        LOG_FATAL("Failed to initialize Game");
        return -1;
    
    }

    while (running)
    {
        platform_update_window(window);
        update_game(&gameState);
       if(!vk_render(&vkcontext, &gameState))
       {
            LOG_FATAL("Failed to render Vulkan");
            return -1;
       }
    }

    return 0;
}

void platform_get_window_size(uint32_t *width, uint32_t *height)
{
    RECT rect;
    GetClientRect(window, &rect);

    *width = rect.right - rect.left;
    *height = rect.bottom - rect.top;
}

char *platform_read_file(char *path, uint32_t *length)
{
    char *result = 0;

    HANDLE file = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (file != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER size;
        if (GetFileSizeEx(file, &size))
        {
            *length = (uint32_t)size.QuadPart;
            result = new char[*length];

            DWORD bytesRead;
            if (ReadFile(file, result, *length, &bytesRead, 0))
            {
            }
            else
            {
               
                ASSERT(0, "Failed to read file: %s", path);
                LOG_ERROR("Failed to read file: %s", path);
            }
        }
        else
        {
                ASSERT(0, "Failed to get file size: %s", path);
                LOG_ERROR("Failed to get file size: %s", path);
        }

        CloseHandle(file);
    }
    else
    {
        ASSERT(0, "Failed to open file: %s", path);
        LOG_ERROR("Failed to open file: %s", path);
    }

    return result;
}

void platform_log(const char* msg, TextColor color)
{
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

    
    OutputDebugStringA(msg);
    

    WriteConsoleA(consoleHandle, msg, strlen(msg), 0, 0);

}