#include "ExpansionDictionary.h"
#include <algorithm>
#include <cctype>
#include <cwctype>

ExpansionDictionary::ExpansionDictionary()
{
}

void ExpansionDictionary::addEntry(const std::wstring& shortcut, const std::wstring& expansion)
{
    m_entries[toLower(shortcut)] = expansion;
}

bool ExpansionDictionary::removeEntry(const std::wstring& shortcut)
{
    return m_entries.erase(toLower(shortcut)) > 0;
}

bool ExpansionDictionary::lookup(const std::wstring& shortcut, std::wstring& result) const
{
    auto it = m_entries.find(toLower(shortcut));
    if (it != m_entries.end()) {
        result = it->second;
        return true;
    }
    return false;
}

void ExpansionDictionary::loadDefaults()
{
    // Default expansion entries
    addEntry(L"gom", L"Good morning");
    addEntry(L"gm",  L"Good morning");
    addEntry(L"ge",  L"Good evening");
    addEntry(L"gn",  L"Good night");
    addEntry(L"ty",  L"Thank you");
    addEntry(L"thx", L"Thanks");
    addEntry(L"tnx", L"Thank you for contacting support");
    addEntry(L"brg", L"Best regards");
    addEntry(L"br",  L"Best regards");
    addEntry(L"yw",  L"You're welcome");
    addEntry(L"pls", L"Please");
    addEntry(L"omw", L"On my way");
    addEntry(L"idk", L"I don't know");
    addEntry(L"np",  L"No problem");
    addEntry(L"ttl", L"Talk to you later");
    addEntry(L"fyi", L"For your information");
    addEntry(L"eta", L"Estimated time of arrival");
    addEntry(L"bp",  L"Blood pressure");
    addEntry(L"sig", L"Sincerely,\r\nText Expansion Keyboard User");
}

size_t ExpansionDictionary::size() const
{
    return m_entries.size();
}

void ExpansionDictionary::clear()
{
    m_entries.clear();
}

std::vector<std::pair<std::wstring, std::wstring>> ExpansionDictionary::getAllEntries() const
{
    std::vector<std::pair<std::wstring, std::wstring>> entries;
    entries.reserve(m_entries.size());
    for (const auto& pair : m_entries) {
        entries.push_back(pair);
    }
    return entries;
}

std::wstring ExpansionDictionary::toLower(const std::wstring& str)
{
    std::wstring lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](wchar_t c) { return static_cast<wchar_t>(std::towlower(c)); });
    return lower;
}
