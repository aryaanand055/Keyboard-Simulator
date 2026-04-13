#include "Globals.h"

HINSTANCE g_hInstance = nullptr;
LONG g_dllRefCount = 0;

LONG DllAddRef()
{
    return InterlockedIncrement(&g_dllRefCount);
}

LONG DllRelease()
{
    return InterlockedDecrement(&g_dllRefCount);
}
