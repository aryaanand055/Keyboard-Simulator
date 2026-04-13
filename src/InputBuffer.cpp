#include "InputBuffer.h"

static const std::wstring TRIGGER_SEQ = L"$#$";

InputBuffer::InputBuffer()
{
}

void InputBuffer::addChar(wchar_t ch)
{
    m_buffer += ch;

    // Keep the buffer trimmed to a reasonable size.
    // We only need: MAX_SHORTCUT_LEN chars for the shortcut + TRIGGER_LEN for the trigger.
    if (m_buffer.size() > MAX_BUFFER_LEN) {
        m_buffer = m_buffer.substr(m_buffer.size() - MAX_BUFFER_LEN);
    }
}

bool InputBuffer::checkTrigger(std::wstring& shortcut)
{
    if (m_buffer.size() < TRIGGER_LEN) {
        return false;
    }

    // Check if the buffer ends with the trigger sequence "$#$"
    size_t bufLen = m_buffer.size();
    std::wstring tail = m_buffer.substr(bufLen - TRIGGER_LEN);

    if (tail != TRIGGER_SEQ) {
        return false;
    }

    // Extract the shortcut: characters before the trigger, up to MAX_SHORTCUT_LEN
    size_t available = bufLen - TRIGGER_LEN;
    size_t shortcutLen = (available < MAX_SHORTCUT_LEN) ? available : MAX_SHORTCUT_LEN;

    if (shortcutLen == 0) {
        // Trigger typed without any shortcut
        reset();
        return false;
    }

    shortcut = m_buffer.substr(available - shortcutLen, shortcutLen);
    reset();
    return true;
}

void InputBuffer::reset()
{
    m_buffer.clear();
}

std::wstring InputBuffer::getBuffer() const
{
    return m_buffer;
}

std::wstring InputBuffer::getTrigger()
{
    return TRIGGER_SEQ;
}
