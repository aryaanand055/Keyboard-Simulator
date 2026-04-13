#pragma once

#include <windows.h>
#include <msctf.h>
#include <string>

// {A1B2C3D4-E5F6-7890-ABCD-EF1234567890}
// CLSID for the Text Expansion Keyboard text service
static const CLSID CLSID_TextExpansionKeyboard = {
    0xa1b2c3d4, 0xe5f6, 0x7890,
    { 0xab, 0xcd, 0xef, 0x12, 0x34, 0x56, 0x78, 0x90 }
};

// {B2C3D4E5-F6A7-8901-BCDE-F12345678901}
// GUID for the TSF profile
static const GUID GUID_TextExpansionProfile = {
    0xb2c3d4e5, 0xf6a7, 0x8901,
    { 0xbc, 0xde, 0xf1, 0x23, 0x45, 0x67, 0x89, 0x01 }
};

// Display name for the text service
#define TEXT_SERVICE_DISPLAY_NAME L"Text Expansion Keyboard"

// Language ID (English US)
#define TEXT_SERVICE_LANGID MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US)

// Global variables
extern HINSTANCE g_hInstance;
extern LONG g_dllRefCount;

// Helper functions
LONG DllAddRef();
LONG DllRelease();
