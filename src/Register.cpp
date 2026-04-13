#include "Register.h"
#include <strsafe.h>

static const wchar_t* const TEXTSERVICE_DESC = TEXT_SERVICE_DISPLAY_NAME;

// ---------------------------------------------------------------------------
// COM Server Registration
// ---------------------------------------------------------------------------

BOOL RegisterServer()
{
    wchar_t modulePath[MAX_PATH];
    if (GetModuleFileName(g_hInstance, modulePath, MAX_PATH) == 0)
        return FALSE;

    // Create the CLSID registry key
    wchar_t clsidKey[256];
    StringCchPrintf(clsidKey, ARRAYSIZE(clsidKey),
                    L"CLSID\\{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
                    CLSID_TextExpansionKeyboard.Data1,
                    CLSID_TextExpansionKeyboard.Data2,
                    CLSID_TextExpansionKeyboard.Data3,
                    CLSID_TextExpansionKeyboard.Data4[0],
                    CLSID_TextExpansionKeyboard.Data4[1],
                    CLSID_TextExpansionKeyboard.Data4[2],
                    CLSID_TextExpansionKeyboard.Data4[3],
                    CLSID_TextExpansionKeyboard.Data4[4],
                    CLSID_TextExpansionKeyboard.Data4[5],
                    CLSID_TextExpansionKeyboard.Data4[6],
                    CLSID_TextExpansionKeyboard.Data4[7]);

    HKEY hKey = nullptr;
    HKEY hSubKey = nullptr;
    BOOL result = FALSE;

    // Create CLSID key
    if (RegCreateKeyEx(HKEY_CLASSES_ROOT, clsidKey, 0, nullptr, REG_OPTION_NON_VOLATILE,
                       KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        RegSetValueEx(hKey, nullptr, 0, REG_SZ,
                      reinterpret_cast<const BYTE*>(TEXTSERVICE_DESC),
                      static_cast<DWORD>((wcslen(TEXTSERVICE_DESC) + 1) * sizeof(wchar_t)));

        // Create InprocServer32 subkey
        if (RegCreateKeyEx(hKey, L"InprocServer32", 0, nullptr, REG_OPTION_NON_VOLATILE,
                           KEY_WRITE, nullptr, &hSubKey, nullptr) == ERROR_SUCCESS) {
            RegSetValueEx(hSubKey, nullptr, 0, REG_SZ,
                          reinterpret_cast<const BYTE*>(modulePath),
                          static_cast<DWORD>((wcslen(modulePath) + 1) * sizeof(wchar_t)));

            const wchar_t* threadingModel = L"Apartment";
            RegSetValueEx(hSubKey, L"ThreadingModel", 0, REG_SZ,
                          reinterpret_cast<const BYTE*>(threadingModel),
                          static_cast<DWORD>((wcslen(threadingModel) + 1) * sizeof(wchar_t)));

            RegCloseKey(hSubKey);
            result = TRUE;
        }
        RegCloseKey(hKey);
    }

    return result;
}

void UnregisterServer()
{
    wchar_t clsidKey[256];
    StringCchPrintf(clsidKey, ARRAYSIZE(clsidKey),
                    L"CLSID\\{%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
                    CLSID_TextExpansionKeyboard.Data1,
                    CLSID_TextExpansionKeyboard.Data2,
                    CLSID_TextExpansionKeyboard.Data3,
                    CLSID_TextExpansionKeyboard.Data4[0],
                    CLSID_TextExpansionKeyboard.Data4[1],
                    CLSID_TextExpansionKeyboard.Data4[2],
                    CLSID_TextExpansionKeyboard.Data4[3],
                    CLSID_TextExpansionKeyboard.Data4[4],
                    CLSID_TextExpansionKeyboard.Data4[5],
                    CLSID_TextExpansionKeyboard.Data4[6],
                    CLSID_TextExpansionKeyboard.Data4[7]);

    // Delete InprocServer32 subkey first
    wchar_t inprocKey[512];
    StringCchPrintf(inprocKey, ARRAYSIZE(inprocKey), L"%s\\InprocServer32", clsidKey);
    RegDeleteKey(HKEY_CLASSES_ROOT, inprocKey);

    // Delete CLSID key
    RegDeleteKey(HKEY_CLASSES_ROOT, clsidKey);
}

// ---------------------------------------------------------------------------
// TSF Profile Registration
// ---------------------------------------------------------------------------

BOOL RegisterProfiles()
{
    ITfInputProcessorProfileMgr* pProfileMgr = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_TF_InputProcessorProfiles, nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  IID_ITfInputProcessorProfileMgr,
                                  reinterpret_cast<void**>(&pProfileMgr));

    if (FAILED(hr) || pProfileMgr == nullptr)
        return FALSE;

    wchar_t modulePath[MAX_PATH];
    GetModuleFileName(g_hInstance, modulePath, MAX_PATH);

    hr = pProfileMgr->RegisterProfile(
        CLSID_TextExpansionKeyboard,
        TEXT_SERVICE_LANGID,
        GUID_TextExpansionProfile,
        TEXTSERVICE_DESC,
        static_cast<ULONG>(wcslen(TEXTSERVICE_DESC)),
        modulePath,
        static_cast<ULONG>(wcslen(modulePath)),
        0,  // icon index
        0,  // hkl substitute
        0,
        TRUE,  // enable by default
        0);

    pProfileMgr->Release();
    return SUCCEEDED(hr);
}

void UnregisterProfiles()
{
    ITfInputProcessorProfileMgr* pProfileMgr = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_TF_InputProcessorProfiles, nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  IID_ITfInputProcessorProfileMgr,
                                  reinterpret_cast<void**>(&pProfileMgr));

    if (SUCCEEDED(hr) && pProfileMgr != nullptr) {
        pProfileMgr->UnregisterProfile(
            CLSID_TextExpansionKeyboard,
            TEXT_SERVICE_LANGID,
            GUID_TextExpansionProfile,
            0);
        pProfileMgr->Release();
    }
}

// ---------------------------------------------------------------------------
// TSF Category Registration
// ---------------------------------------------------------------------------

BOOL RegisterCategories()
{
    ITfCategoryMgr* pCategoryMgr = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_TF_CategoryMgr, nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  IID_ITfCategoryMgr,
                                  reinterpret_cast<void**>(&pCategoryMgr));

    if (FAILED(hr) || pCategoryMgr == nullptr)
        return FALSE;

    // Register as a TIP (Text Input Processor)
    hr = pCategoryMgr->RegisterCategory(
        CLSID_TextExpansionKeyboard,
        GUID_TFCAT_TIP_KEYBOARD,
        CLSID_TextExpansionKeyboard);

    pCategoryMgr->Release();
    return SUCCEEDED(hr);
}

void UnregisterCategories()
{
    ITfCategoryMgr* pCategoryMgr = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_TF_CategoryMgr, nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  IID_ITfCategoryMgr,
                                  reinterpret_cast<void**>(&pCategoryMgr));

    if (SUCCEEDED(hr) && pCategoryMgr != nullptr) {
        pCategoryMgr->UnregisterCategory(
            CLSID_TextExpansionKeyboard,
            GUID_TFCAT_TIP_KEYBOARD,
            CLSID_TextExpansionKeyboard);
        pCategoryMgr->Release();
    }
}
