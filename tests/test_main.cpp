// Unit tests for InputBuffer and ExpansionDictionary.
// These core modules are platform-independent and can be compiled on any OS.
//
// Build (Linux/macOS):
//   g++ -std=c++17 -o run_tests tests/test_main.cpp src/InputBuffer.cpp src/ExpansionDictionary.cpp
//
// Build (Windows with MSVC):
//   cl /EHsc /std:c++17 tests\test_main.cpp src\InputBuffer.cpp src\ExpansionDictionary.cpp /Fe:run_tests.exe

#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <functional>

#include "../src/InputBuffer.h"
#include "../src/ExpansionDictionary.h"

static int g_testsPassed = 0;
static int g_testsFailed = 0;

#define TEST(name) \
    static void test_##name(); \
    static struct Register_##name { \
        Register_##name() { g_tests.push_back({#name, test_##name}); } \
    } s_register_##name; \
    static void test_##name()

#define ASSERT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "  FAIL: " << #expr << " (line " << __LINE__ << ")" << std::endl; \
            throw std::runtime_error("Assertion failed"); \
        } \
    } while (0)

#define ASSERT_FALSE(expr) ASSERT_TRUE(!(expr))

#define ASSERT_EQ(a, b) \
    do { \
        auto _a = (a); auto _b = (b); \
        if (_a != _b) { \
            std::wcerr << L"  FAIL: " << #a << L" == " << #b \
                       << L" (line " << __LINE__ << L")" << std::endl; \
            throw std::runtime_error("Assertion failed"); \
        } \
    } while (0)

struct TestEntry {
    std::string name;
    std::function<void()> func;
};
static std::vector<TestEntry> g_tests;

// =========================================================================
// InputBuffer Tests
// =========================================================================

TEST(buffer_starts_empty)
{
    InputBuffer buf;
    ASSERT_EQ(buf.getBuffer(), std::wstring(L""));
}

TEST(buffer_add_single_char)
{
    InputBuffer buf;
    buf.addChar(L'a');
    ASSERT_EQ(buf.getBuffer(), std::wstring(L"a"));
}

TEST(buffer_add_multiple_chars)
{
    InputBuffer buf;
    buf.addChar(L'a');
    buf.addChar(L'b');
    buf.addChar(L'c');
    ASSERT_EQ(buf.getBuffer(), std::wstring(L"abc"));
}

TEST(buffer_rolling_trims_to_max)
{
    InputBuffer buf;
    // Add more than MAX_BUFFER_LEN (6) characters
    std::wstring longInput = L"abcdefghij";
    for (wchar_t c : longInput) {
        buf.addChar(c);
    }
    // Buffer should be trimmed to last MAX_BUFFER_LEN characters
    std::wstring result = buf.getBuffer();
    ASSERT_TRUE(result.size() <= InputBuffer::MAX_SHORTCUT_LEN + InputBuffer::TRIGGER_LEN);
}

TEST(buffer_reset_clears)
{
    InputBuffer buf;
    buf.addChar(L'x');
    buf.addChar(L'y');
    buf.reset();
    ASSERT_EQ(buf.getBuffer(), std::wstring(L""));
}

TEST(trigger_not_detected_without_sequence)
{
    InputBuffer buf;
    std::wstring shortcut;
    buf.addChar(L'a');
    buf.addChar(L'b');
    buf.addChar(L'c');
    ASSERT_FALSE(buf.checkTrigger(shortcut));
}

TEST(trigger_detected_with_3char_shortcut)
{
    InputBuffer buf;
    std::wstring shortcut;
    // Type "gom$#$"
    for (wchar_t c : std::wstring(L"gom$#$")) {
        buf.addChar(c);
    }
    ASSERT_TRUE(buf.checkTrigger(shortcut));
    ASSERT_EQ(shortcut, std::wstring(L"gom"));
}

TEST(trigger_detected_with_2char_shortcut)
{
    InputBuffer buf;
    std::wstring shortcut;
    // Type "gm$#$"
    for (wchar_t c : std::wstring(L"gm$#$")) {
        buf.addChar(c);
    }
    ASSERT_TRUE(buf.checkTrigger(shortcut));
    ASSERT_EQ(shortcut, std::wstring(L"gm"));
}

TEST(trigger_detected_with_1char_shortcut)
{
    InputBuffer buf;
    std::wstring shortcut;
    // Type "x$#$"
    for (wchar_t c : std::wstring(L"x$#$")) {
        buf.addChar(c);
    }
    ASSERT_TRUE(buf.checkTrigger(shortcut));
    ASSERT_EQ(shortcut, std::wstring(L"x"));
}

TEST(trigger_resets_buffer_after_detection)
{
    InputBuffer buf;
    std::wstring shortcut;
    for (wchar_t c : std::wstring(L"abc$#$")) {
        buf.addChar(c);
    }
    ASSERT_TRUE(buf.checkTrigger(shortcut));
    // Buffer should be empty after trigger
    ASSERT_EQ(buf.getBuffer(), std::wstring(L""));
}

TEST(trigger_not_detected_for_partial_trigger)
{
    InputBuffer buf;
    std::wstring shortcut;
    // Type "$#" (incomplete trigger)
    for (wchar_t c : std::wstring(L"abc$#")) {
        buf.addChar(c);
    }
    ASSERT_FALSE(buf.checkTrigger(shortcut));
}

TEST(trigger_only_dollar_hash_dollar)
{
    InputBuffer buf;
    std::wstring shortcut;
    // "$#$" without any shortcut should not trigger
    for (wchar_t c : std::wstring(L"$#$")) {
        buf.addChar(c);
    }
    ASSERT_FALSE(buf.checkTrigger(shortcut));
}

TEST(trigger_after_long_input)
{
    InputBuffer buf;
    std::wstring shortcut;
    // Simulate a long typing session, then type shortcut + trigger
    std::wstring longInput = L"asdasfdgfsdgdfgfds";
    for (wchar_t c : longInput) {
        buf.addChar(c);
    }
    // Now type the shortcut and trigger
    for (wchar_t c : std::wstring(L"gom$#$")) {
        buf.addChar(c);
    }
    // Due to rolling buffer, only the last MAX_SHORTCUT_LEN + TRIGGER_LEN chars are kept
    // "gom$#$" = 6 chars, which fits in the buffer
    ASSERT_TRUE(buf.checkTrigger(shortcut));
    ASSERT_EQ(shortcut, std::wstring(L"gom"));
}

TEST(trigger_extracts_last_3_chars_as_shortcut)
{
    InputBuffer buf;
    std::wstring shortcut;
    // Type more than 3 chars before trigger, rolling buffer keeps last 3
    for (wchar_t c : std::wstring(L"xyzabc$#$")) {
        buf.addChar(c);
    }
    ASSERT_TRUE(buf.checkTrigger(shortcut));
    ASSERT_EQ(shortcut, std::wstring(L"abc"));
}

TEST(multiple_triggers_in_sequence)
{
    InputBuffer buf;
    std::wstring shortcut;

    // First expansion
    for (wchar_t c : std::wstring(L"gom$#$")) {
        buf.addChar(c);
    }
    ASSERT_TRUE(buf.checkTrigger(shortcut));
    ASSERT_EQ(shortcut, std::wstring(L"gom"));

    // Second expansion
    for (wchar_t c : std::wstring(L"ty$#$")) {
        buf.addChar(c);
    }
    ASSERT_TRUE(buf.checkTrigger(shortcut));
    ASSERT_EQ(shortcut, std::wstring(L"ty"));
}

TEST(get_trigger_returns_correct_string)
{
    ASSERT_EQ(InputBuffer::getTrigger(), std::wstring(L"$#$"));
}

// =========================================================================
// ExpansionDictionary Tests
// =========================================================================

TEST(dictionary_starts_empty)
{
    ExpansionDictionary dict;
    ASSERT_EQ(dict.size(), static_cast<size_t>(0));
}

TEST(dictionary_add_and_lookup)
{
    ExpansionDictionary dict;
    dict.addEntry(L"gom", L"Good morning");
    std::wstring result;
    ASSERT_TRUE(dict.lookup(L"gom", result));
    ASSERT_EQ(result, std::wstring(L"Good morning"));
}

TEST(dictionary_lookup_case_insensitive)
{
    ExpansionDictionary dict;
    dict.addEntry(L"GOM", L"Good morning");
    std::wstring result;
    ASSERT_TRUE(dict.lookup(L"gom", result));
    ASSERT_EQ(result, std::wstring(L"Good morning"));
}

TEST(dictionary_lookup_not_found)
{
    ExpansionDictionary dict;
    dict.addEntry(L"gom", L"Good morning");
    std::wstring result;
    ASSERT_FALSE(dict.lookup(L"xyz", result));
}

TEST(dictionary_remove_entry)
{
    ExpansionDictionary dict;
    dict.addEntry(L"gom", L"Good morning");
    ASSERT_TRUE(dict.removeEntry(L"gom"));
    std::wstring result;
    ASSERT_FALSE(dict.lookup(L"gom", result));
}

TEST(dictionary_remove_nonexistent)
{
    ExpansionDictionary dict;
    ASSERT_FALSE(dict.removeEntry(L"xyz"));
}

TEST(dictionary_overwrite_entry)
{
    ExpansionDictionary dict;
    dict.addEntry(L"gom", L"Good morning");
    dict.addEntry(L"gom", L"Great morning");
    std::wstring result;
    ASSERT_TRUE(dict.lookup(L"gom", result));
    ASSERT_EQ(result, std::wstring(L"Great morning"));
}

TEST(dictionary_clear)
{
    ExpansionDictionary dict;
    dict.addEntry(L"gom", L"Good morning");
    dict.addEntry(L"ty", L"Thank you");
    dict.clear();
    ASSERT_EQ(dict.size(), static_cast<size_t>(0));
}

TEST(dictionary_load_defaults)
{
    ExpansionDictionary dict;
    dict.loadDefaults();
    ASSERT_TRUE(dict.size() > 0);

    std::wstring result;
    ASSERT_TRUE(dict.lookup(L"gom", result));
    ASSERT_EQ(result, std::wstring(L"Good morning"));

    ASSERT_TRUE(dict.lookup(L"ty", result));
    ASSERT_EQ(result, std::wstring(L"Thank you"));

    ASSERT_TRUE(dict.lookup(L"brg", result));
    ASSERT_EQ(result, std::wstring(L"Best regards"));
}

TEST(dictionary_size)
{
    ExpansionDictionary dict;
    ASSERT_EQ(dict.size(), static_cast<size_t>(0));
    dict.addEntry(L"a", L"alpha");
    ASSERT_EQ(dict.size(), static_cast<size_t>(1));
    dict.addEntry(L"b", L"beta");
    ASSERT_EQ(dict.size(), static_cast<size_t>(2));
}

TEST(dictionary_get_all_entries)
{
    ExpansionDictionary dict;
    dict.addEntry(L"a", L"alpha");
    dict.addEntry(L"b", L"beta");
    auto entries = dict.getAllEntries();
    ASSERT_EQ(entries.size(), static_cast<size_t>(2));
}

// =========================================================================
// Integration Tests (InputBuffer + ExpansionDictionary)
// =========================================================================

TEST(integration_full_expansion_flow)
{
    InputBuffer buf;
    ExpansionDictionary dict;
    dict.loadDefaults();

    // Simulate typing "gom$#$"
    for (wchar_t c : std::wstring(L"gom$#$")) {
        buf.addChar(c);
    }

    std::wstring shortcut;
    ASSERT_TRUE(buf.checkTrigger(shortcut));

    std::wstring expansion;
    ASSERT_TRUE(dict.lookup(shortcut, expansion));
    ASSERT_EQ(expansion, std::wstring(L"Good morning"));
}

TEST(integration_unknown_shortcut)
{
    InputBuffer buf;
    ExpansionDictionary dict;
    dict.loadDefaults();

    // Simulate typing "zzz$#$" (not in dictionary)
    for (wchar_t c : std::wstring(L"zzz$#$")) {
        buf.addChar(c);
    }

    std::wstring shortcut;
    ASSERT_TRUE(buf.checkTrigger(shortcut));

    std::wstring expansion;
    ASSERT_FALSE(dict.lookup(shortcut, expansion));
}

TEST(integration_2char_shortcut_flow)
{
    InputBuffer buf;
    ExpansionDictionary dict;
    dict.loadDefaults();

    // Simulate typing "ty$#$"
    for (wchar_t c : std::wstring(L"ty$#$")) {
        buf.addChar(c);
    }

    std::wstring shortcut;
    ASSERT_TRUE(buf.checkTrigger(shortcut));

    std::wstring expansion;
    ASSERT_TRUE(dict.lookup(shortcut, expansion));
    ASSERT_EQ(expansion, std::wstring(L"Thank you"));
}

TEST(integration_long_typing_then_expand)
{
    InputBuffer buf;
    ExpansionDictionary dict;
    dict.loadDefaults();

    // Simulate long typing then shortcut
    std::wstring longText = L"The quick brown fox jumps over the lazy dog ";
    for (wchar_t c : longText) {
        buf.addChar(c);
    }
    for (wchar_t c : std::wstring(L"brg$#$")) {
        buf.addChar(c);
    }

    std::wstring shortcut;
    ASSERT_TRUE(buf.checkTrigger(shortcut));

    std::wstring expansion;
    ASSERT_TRUE(dict.lookup(shortcut, expansion));
    ASSERT_EQ(expansion, std::wstring(L"Best regards"));
}

// =========================================================================
// Test Runner
// =========================================================================

int main()
{
    std::cout << "Running Text Expansion Keyboard Tests" << std::endl;
    std::cout << "======================================" << std::endl;

    for (const auto& test : g_tests) {
        std::cout << "  " << test.name << "... ";
        try {
            test.func();
            std::cout << "PASS" << std::endl;
            g_testsPassed++;
        } catch (const std::exception& e) {
            std::cout << "FAIL (" << e.what() << ")" << std::endl;
            g_testsFailed++;
        }
    }

    std::cout << "======================================" << std::endl;
    std::cout << "Results: " << g_testsPassed << " passed, "
              << g_testsFailed << " failed, "
              << (g_testsPassed + g_testsFailed) << " total" << std::endl;

    return g_testsFailed > 0 ? 1 : 0;
}
