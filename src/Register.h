#pragma once

#include "Globals.h"

// Register/unregister the text service DLL and TSF profile.
BOOL RegisterServer();
void UnregisterServer();

BOOL RegisterProfiles();
void UnregisterProfiles();

BOOL RegisterCategories();
void UnregisterCategories();
