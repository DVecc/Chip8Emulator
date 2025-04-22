#include <windows.h>
#include "defines.h"
#include "platform.h"
#include "Game/game.cpp"
#include "assets/assets.cpp"
#include "renderer/vk_renderer.cpp"
#include "input.cpp"


global_variable bool running = TRUE;
global_variable HWND window;
global_variable InputState input;

LRESULT CALLBACK platform_window_callback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {

        case WM_DROPFILES:
        {
            HDROP drop = (HDROP)wParam;
            uint32_t fileCount = DragQueryFileA(drop, INVALID_IDX, 0, 0);

            for(uint32_t i = 0; i < fileCount; i++)
            {
                uint32_t fileNameLength = DragQueryFileA(drop, i, 0, 0);
                char filePath[500] = {};

                DragQueryFileA(drop, i, filePath, fileNameLength + 1);

                char command[300] = {};
                sprintf(command, "lib\\texconv.exe -y -m 1 -f R8G8B8A8_UNORM \"%s\" -o assets/textures ", filePath);
                ASSERT(!system(command), "Failed to import Asset: %s", filePath);
            }
        }
        break;

        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            bool isDown = msg == WM_KEYDOWN ? true : false;
            uint32_t keyID = INVALID_IDX;

            switch((int)wParam)
            {
                case 'A':
                    keyID = A_KEY;
                    break;
                case 'S':
                    keyID = S_KEY;
                    break;
                case 'D':
                    keyID = D_KEY;
                    break;
                case 'W':
                    keyID = W_KEY;
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
        case WM_CLOSE:
            running = FALSE;
    }

    return DefWindowProcA(window, msg, wParam, lParam);
}

bool platform_create_window(Vec2 screenSize)
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

    uint32_t windowStyle =  WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_OVERLAPPED;

    // Obtain the size of the border
    {
        RECT borderRect = {};
        AdjustWindowRectEx(&borderRect, windowStyle, 0, WS_EX_APPWINDOW);
        screenSize.x += borderRect.right - borderRect.left;
        screenSize.y += borderRect.bottom - borderRect.top;
    }

    window = CreateWindowExA(
        WS_EX_APPWINDOW,
        "vulkan_engine_class",
        "Chip8Emulator",
       windowStyle, 100, 100, screenSize.x, screenSize.y, 0, 0, instance, 0);

    if (window == 0)
    {
        MessageBoxA(0, "Failed to create Window", "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    #ifdef DEBUG
    DragAcceptFiles(window, true);
    #endif

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

    input.screenSize = {1000, 700};
    VkContext vkcontext = {};
    GameState gameState = {};
    if (!platform_create_window(input.screenSize))
    {
        LOG_FATAL("Failed to open a window");
        return -1;
    }

    if (!vk_init(&vkcontext, window))
    {
        LOG_FATAL("Failed to initialize Vulkan");
        return -1;
    }

    if(!init_game(&gameState,&input))
    {
        LOG_FATAL("Failed to initialize Game");
        return -1;
    
    }

    LARGE_INTEGER ticksPerSecond, lastTickCount, currentTickCount;
    QueryPerformanceFrequency(&ticksPerSecond);
    QueryPerformanceCounter(&lastTickCount);
    float dt;

    while (running)
    {
        // Clear out key transition count
        {
            for (uint32_t i = 0; i < KEY_COUNT; i++)
            {
                input.keys[i].halfTransitionCount = 0;
            }

        }
        platform_update_window(window);

        // Evaluate Delta Time
        {
            QueryPerformanceCounter(&currentTickCount);
            uint64_t elapsedTicks = currentTickCount.QuadPart - lastTickCount. QuadPart;

            // convert to Microseconds to not lose percision
            uint64_t elapsedTimeInMicroseconds = (elapsedTicks * 1000000) / ticksPerSecond.QuadPart;

            lastTickCount = currentTickCount;

            // Time in milliseconds
            dt = (float)elapsedTimeInMicroseconds / 1000.0f;

            // time in seconds 
            dt /=1000.0f;
        }

        update_game(&gameState, &input, dt);
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