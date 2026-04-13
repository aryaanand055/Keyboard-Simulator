#pragma once

#include <string>

// InputBuffer manages a rolling character buffer and detects the trigger sequence.
// When the trigger "$#$" is detected, it extracts the shortcut from the buffer.
// The shortcut portion is limited to MAX_SHORTCUT_LEN characters (default: 3).
class InputBuffer {
public:
    static const size_t MAX_SHORTCUT_LEN = 3;
    static const size_t TRIGGER_LEN = 3;

    InputBuffer();

    // Add a character to the buffer.
    void addChar(wchar_t ch);

    // Check if the trigger sequence "$#$" has been detected.
    // If triggered, extracts the shortcut into 'shortcut' and resets the buffer.
    // Returns true if a trigger was detected, false otherwise.
    bool checkTrigger(std::wstring& shortcut);

    // Reset the buffer to empty state.
    void reset();

    // Get the current buffer contents (for testing/debugging).
    std::wstring getBuffer() const;

    // Get the trigger sequence string.
    static std::wstring getTrigger();

private:
    std::wstring m_buffer;
    static const size_t MAX_BUFFER_LEN = MAX_SHORTCUT_LEN + TRIGGER_LEN;
};
