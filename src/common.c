#include "common.h"
#include "cmd.h"
#include "input.h"
#include "io.h"
#include "len.h"
#include "platform.h"
#include "window.h"
#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef UNIX
#include <dlfcn.h>
#include <unistd.h>
#elif defined(WIN32)
#include <winbase.h>
#endif

#define MAX_PRINT_MSG 256

static char errorMsgBuffer[MAX_PRINT_MSG];

#define HEADER "HELL: "

void
hell_Print(const char* fmt, ...)
{
    va_list argptr;
    char    msg[MAX_PRINT_MSG];
    int64_t c = 0;
    va_start(argptr, fmt);
    c += vsnprintf(msg + c, sizeof(msg), fmt, argptr);
    //c += sprintf(msg + c, "\n");
    va_end(argptr);
    fputs(msg, stdout);
#if WIN32
    OutputDebugString(msg);
#endif
    hell_WriteToLog(msg);
}

void
hell_print(const char* fmt, ...)
{
    va_list argptr;
    char    msg[MAX_PRINT_MSG];
    int64_t c = 0;
    va_start(argptr, fmt);
    c += vsnprintf(msg + c, sizeof(msg), fmt, argptr);
    assert((char*)c - msg < MAX_PRINT_MSG - 2);
    c += sprintf(msg + c, "\n");
    va_end(argptr);
    fputs(msg, stdout);
#if WIN32
    OutputDebugString(msg);
#endif
    hell_WriteToLog(msg);
}

void
hell_Announce(const char* fmt, ...)
{
    va_list argptr;
    char    msg[MAX_PRINT_MSG];
    int     l = 0;
    l += sprintf(msg + l, HEADER);
    va_start(argptr, fmt);
    vsnprintf(msg + l, sizeof(msg) - l, fmt, argptr);
    va_end(argptr);
    fputs(msg, stdout);
#if WIN32
    OutputDebugString(msg);
#endif
    hell_WriteToLog(msg);
}

void
hell_Abort(void)
{
    abort();
}

void
hell_Error(Hell_ErrorCode errorCode, const char* fmt, ...)
{
    va_list       argptr;
    const int64_t len = sizeof(errorMsgBuffer);
    int64_t       c   = 0;
    c += sprintf(errorMsgBuffer + c, "***ERROR: ***\n");
    c += sprintf(errorMsgBuffer + c, "HELL: ");
    va_start(argptr, fmt);
    c += vsnprintf(errorMsgBuffer + c, len - c, fmt, argptr);
    va_end(argptr);
    c += snprintf(errorMsgBuffer + c, len - c, "Errno %d\n", errno);
    fputs(errorMsgBuffer, stderr);
#if WIN32
    OutputDebugString(errorMsgBuffer);
#endif
    hell_WriteToLog(errorMsgBuffer);
    if (errorCode == HELL_ERR_FATAL)
        hell_Abort();
}

void
hell_ErrorFatalImpl(const char* fmt, ...)
{
    va_list       argptr;
    const int64_t len = sizeof(errorMsgBuffer);
    int64_t       c   = 0;
    c += sprintf(errorMsgBuffer + c, "*** ERROR ***\n");
    va_start(argptr, fmt);
    c += vsnprintf(errorMsgBuffer + c, len - c, fmt, argptr);
    va_end(argptr);
    c += snprintf(errorMsgBuffer + c, len - c, "Errno %d\n", errno);
    fputs(errorMsgBuffer, stderr);
#if WIN32
    OutputDebugString(errorMsgBuffer);
#endif
    hell_WriteToLog(errorMsgBuffer);
    hell_Abort();
}

uint64_t
hell_Align(const uint64_t quantity, const uint32_t alignment)
{
    assert(alignment != 0);
    if (quantity % alignment != 0) // not aligned
        return (quantity / alignment + 1) * alignment;
    else
        return quantity;
}

void
hell_BitPrint(const void* const thing, const uint32_t bitcount)
{
    int mask;
    for (int i = bitcount - 1; i >= 0; i--)
    {
        mask = 1 << i;
        if (mask & *(int*)thing)
            putchar('1');
        else
            putchar('0');
    }
    putchar('\n');
}

void
hell_BytePrint(const void* const thing, const uint32_t byteCount)
{
    int            mask;
    const uint8_t* base = (uint8_t*)thing;
    for (int i = byteCount - 1; i >= 0; i--)
    {
        for (int j = 8 - 1; j >= 0; j--)
        {
            mask = 1 << j;
            if (mask & *(base + i))
                putchar('1');
            else
                putchar('0');
        }
    }
    putchar('\n');
}

void
hell_Print_Vec3(const float v[3])
{
    hell_Print("[ %f, %f, %f]", v[0], v[1], v[2]);
}

void
hell_Print_Mat4(const float m[4][4])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%f ", m[i][j]);
        }
        printf("\n");
    }
}

void*
hell_LoadLibrary(const char* name)
{
#ifdef UNIX
    return dlopen(name, RTLD_LAZY);
#elif defined(WIN32)
    return LoadLibrary(name);
#endif
}

void*
hell_LoadSymbol(void* module, const char* symname)
{
#ifdef UNIX
    return dlsym(module, symname);
#elif defined(WIN32)
    return GetProcAddress(module, symname);
#endif
}

bool
hell_FileExists(const char* path)
{
#ifdef UNIX
    return (access(path, F_OK) == 0);
#elif defined(WIN32)
    WIN32_FIND_DATA findData;
    HANDLE          handle = FindFirstFile(path, &findData);
    bool            found  = handle != INVALID_HANDLE_VALUE;
    if (found)
        FindClose(handle);
    return found;
#endif
}

void
hell_Sleep(double s)
{
    uint64_t us = (s * 1000000);
#ifdef UNIX
    usleep(us);
#elif defined(WIN32)
    Sleep(us / 1000);
#endif
}

void hell_MicroSleep(uint64_t us)
{
#ifdef UNIX
    usleep(us);
#elif defined(WIN32)
    Sleep(us / 1000);
#endif
}
