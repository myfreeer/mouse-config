#pragma once
#ifndef _MOUSE_CONFIG_H
#define _MOUSE_CONFIG_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef __GNUC__
#define UNUSED
#else // __GNUC__
#define UNUSED __attribute__((unused))
#endif // __GNUC__

#include <windows.h>

#define printError(...)
#define COM_CALL(hr, pointer, function, ...)                                   \
    hr = (pointer)->lpVtbl->function((pointer), ##__VA_ARGS__);                \
    EXIT_ON_ERROR(hr, function)

#define EXIT_ON_ERROR(hr, description)                                         \
    if (FAILED(hr)) {                                                          \
        printError("%s failed: error %d occurred\n", #description, -(hr));     \
        goto Exit;                                                             \
    }
#endif //_MOUSE_CONFIG_H
