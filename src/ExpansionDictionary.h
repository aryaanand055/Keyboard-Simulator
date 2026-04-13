#pragma once

#include <string>
#include <unordered_map>
#include <vector>

// ExpansionDictionary maps shortcut strings to their expanded phrases.
// Shortcuts are case-insensitive for lookup.
class ExpansionDictionary {
public:
    ExpansionDictionary();

    // Add a shortcut-expansion pair.
    void addEntry(const std::wstring& shortcut, const std::wstring& expansion);

    // Remove a shortcut entry. Returns true if the entry existed.
    bool removeEntry(const std::wstring& shortcut);

    // Look up a shortcut. Returns true if found, with the expansion in 'result'.
    bool lookup(const std::wstring& shortcut, std::wstring& result) const;

    // Load the default built-in dictionary entries.
    void loadDefaults();

    // Get the number of entries in the dictionary.
    size_t size() const;

    // Clear all entries.
    void clear();

    // Get all entries as a vector of pairs (for enumeration).
    std::vector<std::pair<std::wstring, std::wstring>> getAllEntries() const;

private:
    std::unordered_map<std::wstring, std::wstring> m_entries;

    // Convert a string to lowercase for case-insensitive matching.
    static std::wstring toLower(const std::wstring& str);
};
