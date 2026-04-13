#include "Globals.h"
#include "ClassFactory.h"
#include "Register.h"

// ---------------------------------------------------------------------------
// DLL Entry Point
// ---------------------------------------------------------------------------

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    switch (dwReason) {
    case DLL_PROCESS_ATTACH:
        g_hInstance = hInstance;
        DisableThreadLibraryCalls(hInstance);
        break;

    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}

// ---------------------------------------------------------------------------
// DLL Exports
// ---------------------------------------------------------------------------

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppvObj)
{
    if (ppvObj == nullptr)
        return E_INVALIDARG;

    *ppvObj = nullptr;

    if (!IsEqualCLSID(rclsid, CLSID_TextExpansionKeyboard))
        return CLASS_E_CLASSNOTAVAILABLE;

    ClassFactory* pFactory = new (std::nothrow) ClassFactory();
    if (pFactory == nullptr)
        return E_OUTOFMEMORY;

    HRESULT hr = pFactory->QueryInterface(riid, ppvObj);
    pFactory->Release();
    return hr;
}

STDAPI DllCanUnloadNow()
{
    return (g_dllRefCount <= 0) ? S_OK : S_FALSE;
}

STDAPI DllRegisterServer()
{
    if (!RegisterServer())
        return E_FAIL;

    if (!RegisterProfiles()) {
        UnregisterServer();
        return E_FAIL;
    }

    if (!RegisterCategories()) {
        UnregisterProfiles();
        UnregisterServer();
        return E_FAIL;
    }

    return S_OK;
}

STDAPI DllUnregisterServer()
{
    UnregisterCategories();
    UnregisterProfiles();
    UnregisterServer();
    return S_OK;
}
