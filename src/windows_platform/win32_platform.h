#ifndef WIN_32_PLATFORM_H
#define WIN_32_PLATFORM_H

// Standard Libraries
#include <stdint.h>

// Windows Libraries
#include <windows.h>

// Forward decleration

enum TextColor;
extern bool running;
extern bool framebufferResized;

LRESULT CALLBACK platform_window_callback(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);
bool platform_create_window(HWND* window, int windowWidth, int windowHeight);
void platform_log(const wchar_t *msg, TextColor color);
void platform_update_window(HWND window);
void platform_cleanup();
void platform_get_window_size(HWND window, uint32_t* width, uint32_t* height);

#endif