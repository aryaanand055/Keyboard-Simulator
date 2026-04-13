#include "TextService.h"

// ===========================================================================
// EditSession Implementation
// ===========================================================================

EditSession::EditSession(ITfContext* pContext, TextService* pTextService,
                         const std::wstring& shortcut, const std::wstring& expansion)
    : m_refCount(1)
    , m_pContext(pContext)
    , m_pTextService(pTextService)
    , m_shortcut(shortcut)
    , m_expansion(expansion)
{
    if (m_pContext)
        m_pContext->AddRef();
}

EditSession::~EditSession()
{
    if (m_pContext)
        m_pContext->Release();
}

STDMETHODIMP EditSession::QueryInterface(REFIID riid, void** ppvObj)
{
    if (ppvObj == nullptr)
        return E_INVALIDARG;

    *ppvObj = nullptr;

    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_ITfEditSession)) {
        *ppvObj = static_cast<ITfEditSession*>(this);
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) EditSession::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

STDMETHODIMP_(ULONG) EditSession::Release()
{
    LONG count = InterlockedDecrement(&m_refCount);
    if (count == 0)
        delete this;
    return count;
}

STDMETHODIMP EditSession::DoEditSession(TfEditCookie ec)
{
    if (m_pContext == nullptr)
        return E_FAIL;

    // Total characters to delete: shortcut length + trigger length ("$#$" = 3)
    LONG charsToDelete = static_cast<LONG>(m_shortcut.size() + InputBuffer::TRIGGER_LEN);

    // Use ITfInsertAtSelection to get the current selection/insertion point
    ITfInsertAtSelection* pInsert = nullptr;
    HRESULT hr = m_pContext->QueryInterface(IID_ITfInsertAtSelection,
                                            reinterpret_cast<void**>(&pInsert));
    if (FAILED(hr) || pInsert == nullptr)
        return E_FAIL;

    // Get the current insertion range (query only, no modification yet)
    ITfRange* pRange = nullptr;
    hr = pInsert->InsertTextAtSelection(ec, TF_IAS_QUERYONLY, nullptr, 0, &pRange);

    if (SUCCEEDED(hr) && pRange != nullptr) {
        // Move the start of the range back by charsToDelete to cover the shortcut + trigger
        LONG shifted = 0;
        pRange->ShiftStart(ec, -charsToDelete, &shifted, nullptr);

        // Replace the selected range with the expansion text
        pRange->SetText(ec, 0, m_expansion.c_str(), static_cast<LONG>(m_expansion.size()));

        pRange->Release();
    }

    pInsert->Release();
    return S_OK;
}

// ===========================================================================
// TextService Implementation
// ===========================================================================

TextService::TextService()
    : m_refCount(1)
    , m_pThreadMgr(nullptr)
    , m_tfClientId(TF_CLIENTID_NULL)
    , m_dwThreadMgrEventSinkCookie(TF_INVALID_COOKIE)
{
    m_dictionary.loadDefaults();
    DllAddRef();
}

TextService::~TextService()
{
    DllRelease();
}

// ---------------------------------------------------------------------------
// IUnknown
// ---------------------------------------------------------------------------

STDMETHODIMP TextService::QueryInterface(REFIID riid, void** ppvObj)
{
    if (ppvObj == nullptr)
        return E_INVALIDARG;

    *ppvObj = nullptr;

    if (IsEqualIID(riid, IID_IUnknown) ||
        IsEqualIID(riid, IID_ITfTextInputProcessor) ||
        IsEqualIID(riid, IID_ITfTextInputProcessorEx))
    {
        *ppvObj = static_cast<ITfTextInputProcessorEx*>(this);
    }
    else if (IsEqualIID(riid, IID_ITfKeyEventSink))
    {
        *ppvObj = static_cast<ITfKeyEventSink*>(this);
    }
    else if (IsEqualIID(riid, IID_ITfThreadMgrEventSink))
    {
        *ppvObj = static_cast<ITfThreadMgrEventSink*>(this);
    }
    else
    {
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

STDMETHODIMP_(ULONG) TextService::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

STDMETHODIMP_(ULONG) TextService::Release()
{
    LONG count = InterlockedDecrement(&m_refCount);
    if (count == 0) {
        delete this;
    }
    return count;
}

// ---------------------------------------------------------------------------
// ITfTextInputProcessor / ITfTextInputProcessorEx
// ---------------------------------------------------------------------------

STDMETHODIMP TextService::Activate(ITfThreadMgr* pThreadMgr, TfClientId tfClientId)
{
    return ActivateEx(pThreadMgr, tfClientId, 0);
}

STDMETHODIMP TextService::ActivateEx(ITfThreadMgr* pThreadMgr, TfClientId tfClientId, DWORD /*dwFlags*/)
{
    m_pThreadMgr = pThreadMgr;
    m_pThreadMgr->AddRef();
    m_tfClientId = tfClientId;

    if (!InitKeyEventSink()) {
        Deactivate();
        return E_FAIL;
    }

    if (!InitThreadMgrEventSink()) {
        Deactivate();
        return E_FAIL;
    }

    m_inputBuffer.reset();
    return S_OK;
}

STDMETHODIMP TextService::Deactivate()
{
    UninitKeyEventSink();
    UninitThreadMgrEventSink();

    if (m_pThreadMgr) {
        m_pThreadMgr->Release();
        m_pThreadMgr = nullptr;
    }

    m_tfClientId = TF_CLIENTID_NULL;
    m_inputBuffer.reset();
    return S_OK;
}

// ---------------------------------------------------------------------------
// ITfKeyEventSink
// ---------------------------------------------------------------------------

STDMETHODIMP TextService::OnSetFocus(BOOL /*fForeground*/)
{
    return S_OK;
}

STDMETHODIMP TextService::OnTestKeyDown(ITfContext* /*pContext*/, WPARAM /*wParam*/,
                                         LPARAM /*lParam*/, BOOL* pfEaten)
{
    // We do not consume keys during test phase
    if (pfEaten)
        *pfEaten = FALSE;
    return S_OK;
}

STDMETHODIMP TextService::OnTestKeyUp(ITfContext* /*pContext*/, WPARAM /*wParam*/,
                                       LPARAM /*lParam*/, BOOL* pfEaten)
{
    if (pfEaten)
        *pfEaten = FALSE;
    return S_OK;
}

STDMETHODIMP TextService::OnKeyDown(ITfContext* pContext, WPARAM wParam,
                                     LPARAM /*lParam*/, BOOL* pfEaten)
{
    if (pfEaten)
        *pfEaten = FALSE;

    // Convert virtual key to character
    BYTE keyboardState[256];
    if (!GetKeyboardState(keyboardState))
        return S_OK;

    wchar_t chars[5] = { 0 };
    int result = ToUnicode(static_cast<UINT>(wParam),
                           MapVirtualKey(static_cast<UINT>(wParam), MAPVK_VK_TO_VSC),
                           keyboardState, chars, 4, 0);

    if (result <= 0 || chars[0] == L'\0')
        return S_OK;

    // Feed the character into the input buffer
    m_inputBuffer.addChar(chars[0]);

    // Check for trigger
    std::wstring shortcut;
    if (m_inputBuffer.checkTrigger(shortcut)) {
        std::wstring expansion;
        if (m_dictionary.lookup(shortcut, expansion)) {
            // Calculate the total length to delete: shortcut + trigger "$#$"
            RequestExpansion(pContext, shortcut, expansion);
            if (pfEaten)
                *pfEaten = TRUE;
        }
    }

    return S_OK;
}

STDMETHODIMP TextService::OnKeyUp(ITfContext* /*pContext*/, WPARAM /*wParam*/,
                                   LPARAM /*lParam*/, BOOL* pfEaten)
{
    if (pfEaten)
        *pfEaten = FALSE;
    return S_OK;
}

STDMETHODIMP TextService::OnPreservedKey(ITfContext* /*pContext*/, REFGUID /*rguid*/, BOOL* pfEaten)
{
    if (pfEaten)
        *pfEaten = FALSE;
    return S_OK;
}

// ---------------------------------------------------------------------------
// ITfThreadMgrEventSink
// ---------------------------------------------------------------------------

STDMETHODIMP TextService::OnInitDocumentMgr(ITfDocumentMgr* /*pDocMgr*/)
{
    return S_OK;
}

STDMETHODIMP TextService::OnUninitDocumentMgr(ITfDocumentMgr* /*pDocMgr*/)
{
    return S_OK;
}

STDMETHODIMP TextService::OnSetFocus(ITfDocumentMgr* /*pDocMgrFocus*/,
                                      ITfDocumentMgr* /*pDocMgrPrevFocus*/)
{
    // Reset the input buffer when focus changes
    m_inputBuffer.reset();
    return S_OK;
}

STDMETHODIMP TextService::OnPushContext(ITfContext* /*pContext*/)
{
    return S_OK;
}

STDMETHODIMP TextService::OnPopContext(ITfContext* /*pContext*/)
{
    return S_OK;
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

BOOL TextService::InitKeyEventSink()
{
    if (m_pThreadMgr == nullptr)
        return FALSE;

    ITfKeystrokeMgr* pKeystrokeMgr = nullptr;
    HRESULT hr = m_pThreadMgr->QueryInterface(IID_ITfKeystrokeMgr,
                                               reinterpret_cast<void**>(&pKeystrokeMgr));
    if (FAILED(hr) || pKeystrokeMgr == nullptr)
        return FALSE;

    hr = pKeystrokeMgr->AdviseKeyEventSink(m_tfClientId,
                                            static_cast<ITfKeyEventSink*>(this),
                                            TRUE);
    pKeystrokeMgr->Release();
    return SUCCEEDED(hr);
}

void TextService::UninitKeyEventSink()
{
    if (m_pThreadMgr == nullptr)
        return;

    ITfKeystrokeMgr* pKeystrokeMgr = nullptr;
    if (SUCCEEDED(m_pThreadMgr->QueryInterface(IID_ITfKeystrokeMgr,
                                                reinterpret_cast<void**>(&pKeystrokeMgr)))) {
        pKeystrokeMgr->UnadviseKeyEventSink(m_tfClientId);
        pKeystrokeMgr->Release();
    }
}

BOOL TextService::InitThreadMgrEventSink()
{
    if (m_pThreadMgr == nullptr)
        return FALSE;

    ITfSource* pSource = nullptr;
    HRESULT hr = m_pThreadMgr->QueryInterface(IID_ITfSource,
                                               reinterpret_cast<void**>(&pSource));
    if (FAILED(hr) || pSource == nullptr)
        return FALSE;

    hr = pSource->AdviseSink(IID_ITfThreadMgrEventSink,
                             static_cast<ITfThreadMgrEventSink*>(this),
                             &m_dwThreadMgrEventSinkCookie);
    pSource->Release();
    return SUCCEEDED(hr);
}

void TextService::UninitThreadMgrEventSink()
{
    if (m_pThreadMgr == nullptr || m_dwThreadMgrEventSinkCookie == TF_INVALID_COOKIE)
        return;

    ITfSource* pSource = nullptr;
    if (SUCCEEDED(m_pThreadMgr->QueryInterface(IID_ITfSource,
                                                reinterpret_cast<void**>(&pSource)))) {
        pSource->UnadviseSink(m_dwThreadMgrEventSinkCookie);
        pSource->Release();
    }
    m_dwThreadMgrEventSinkCookie = TF_INVALID_COOKIE;
}

void TextService::RequestExpansion(ITfContext* pContext, const std::wstring& shortcut,
                                   const std::wstring& expansion)
{
    if (pContext == nullptr)
        return;

    // Create an edit session to perform the text replacement with a valid edit cookie
    EditSession* pEditSession = new (std::nothrow) EditSession(pContext, this, shortcut, expansion);
    if (pEditSession == nullptr)
        return;

    HRESULT hr;
    pContext->RequestEditSession(m_tfClientId, pEditSession, TF_ES_READWRITE | TF_ES_ASYNC, &hr);
    pEditSession->Release();
}
