#ifndef LOGGER_H
#define LOGGER_H

// Standard Library
#include <stdio.h>
#include <iostream>
// Platform Libraries
#ifdef WINDOWS_BUILD
#include "../windows_platform/win32_platform.h"
#endif

enum TextColor
{
    TEXT_COLOR_WHITE,
    TEXT_COLOR_GREEN,
    TEXT_COLOR_YELLOW,
    TEXT_COLOR_RED,
    TEXT_COLOR_LIGHT_RED,
    TEXT_COLOR_BLUE
};

template <typename... Args>
void _log(wchar_t* prefix, TextColor color, const wchar_t* msg, Args... args)
{
    wchar_t fmtBuffer[32000] = {};
    wchar_t msgBuffer[32000] = {};

    wsprintf(fmtBuffer, L"%ls: %ls\n", prefix, msg);
    wsprintf(msgBuffer, fmtBuffer, args...);

    platform_log(msgBuffer, color);
    std::wcerr << msgBuffer;

    if (prefix == L"FATAL")
    {
        throw std::runtime_error("");
    }
}

#define LOG_TRACE(msg, ...) _log(L"TRACE", TEXT_COLOR_GREEN, msg, __VA_ARGS__)
#define LOG_WARN(msg, ...) _log(L"WARN", TEXT_COLOR_YELLOW, msg, __VA_ARGS__)
#define LOG_ERROR(msg, ...) _log(L"ERROR", TEXT_COLOR_RED, msg, __VA_ARGS__)
#define LOG_FATAL(msg, ...) _log(L"FATAL", TEXT_COLOR_LIGHT_RED, msg, __VA_ARGS__)
#define LOG_VK(msg, ...) _log(L"VULKAN VALIDATION", TEXT_COLOR_BLUE, msg, __VA_ARGS__)

#define ASSERT(x, msg, ...)              \
    {                                    \
        if (!(x))                        \
        {                                \
            LOG_ERROR(msg, __VA_ARGS__); \
        }                                \
    }

#endif
