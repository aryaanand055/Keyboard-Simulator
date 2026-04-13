#include "ClassFactory.h"
#include "TextService.h"

ClassFactory::ClassFactory()
    : m_refCount(1)
{
    DllAddRef();
}

ClassFactory::~ClassFactory()
{
    DllRelease();
}

// ---------------------------------------------------------------------------
// IUnknown
// ---------------------------------------------------------------------------

STDMETHODIMP ClassFactory::QueryInterface(REFIID riid, void** ppvObj)
{
    if (ppvObj == nullptr)
        return E_INVALIDARG;

    *ppvObj = nullptr;

    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory)) {
        *ppvObj = static_cast<IClassFactory*>(this);
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) ClassFactory::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

STDMETHODIMP_(ULONG) ClassFactory::Release()
{
    LONG count = InterlockedDecrement(&m_refCount);
    if (count == 0) {
        delete this;
    }
    return count;
}

// ---------------------------------------------------------------------------
// IClassFactory
// ---------------------------------------------------------------------------

STDMETHODIMP ClassFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObj)
{
    if (ppvObj == nullptr)
        return E_INVALIDARG;

    *ppvObj = nullptr;

    // Aggregation is not supported
    if (pUnkOuter != nullptr)
        return CLASS_E_NOAGGREGATION;

    TextService* pTextService = new (std::nothrow) TextService();
    if (pTextService == nullptr)
        return E_OUTOFMEMORY;

    HRESULT hr = pTextService->QueryInterface(riid, ppvObj);
    pTextService->Release();
    return hr;
}

STDMETHODIMP ClassFactory::LockServer(BOOL fLock)
{
    if (fLock) {
        DllAddRef();
    } else {
        DllRelease();
    }
    return S_OK;
}
