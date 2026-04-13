# Text Expansion Keyboard

A Windows-native TSF (Text Services Framework) text input service that automatically expands shortcut abbreviations into full phrases as you type.

## How It Works

Type a short abbreviation followed by the trigger sequence `$#$`, and it is automatically replaced with the full phrase:

| You Type | Result |
|----------|--------|
| `gom$#$` | Good morning |
| `ty$#$` | Thank you |
| `brg$#$` | Best regards |
| `tnx$#$` | Thank you for contacting support |
| `bp$#$` | Blood pressure |

The system uses a rolling buffer to track the last typed characters and detects the `$#$` trigger sequence. When triggered, it looks up the shortcut (up to 3 characters before the trigger) in the expansion dictionary and replaces the entire input with the expanded phrase.

## Project Structure

```
├── src/                          # Source code
│   ├── InputBuffer.h/cpp         # Rolling character buffer and trigger detection
│   ├── ExpansionDictionary.h/cpp # Shortcut-to-phrase dictionary
│   ├── TextService.h/cpp         # TSF text input processor implementation
│   ├── ClassFactory.h/cpp        # COM class factory
│   ├── Register.h/cpp            # DLL and TSF profile registration
│   ├── Globals.h/cpp             # Global definitions and GUIDs
│   ├── dllmain.cpp               # DLL entry point and exports
│   └── TextExpansionKeyboard.def # Module definition file
├── config/
│   └── dictionary.json           # Default expansion dictionary
├── tests/
│   ├── test_main.cpp             # Unit tests for core logic
│   └── CMakeLists.txt            # CMake build for tests
├── docs/
│   ├── deployment_guide.md       # Installation and deployment guide
│   └── user_manual.md            # End-user manual
├── TextExpansionKeyboard.sln     # Visual Studio solution
└── TextExpansionKeyboard.vcxproj # Visual Studio project
```

## Building

### Prerequisites

- Windows 10 or later
- Visual Studio 2022 with **Desktop development with C++** workload

### Build with Visual Studio

1. Open `TextExpansionKeyboard.sln` in Visual Studio 2022
2. Select **Release | x64** configuration
3. Build the solution (**Ctrl+Shift+B**)
4. The output DLL is in `x64\Release\TextExpansionKeyboard.dll`

### Running Tests

The core logic (InputBuffer and ExpansionDictionary) is platform-independent and can be tested on any OS with a C++17 compiler:

```bash
# Linux / macOS
g++ -std=c++17 -o run_tests tests/test_main.cpp src/InputBuffer.cpp src/ExpansionDictionary.cpp
./run_tests

# Windows (MSVC Developer Command Prompt)
cl /EHsc /std:c++17 tests\test_main.cpp src\InputBuffer.cpp src\ExpansionDictionary.cpp /Fe:run_tests.exe
run_tests.exe
```

Or use CMake:

```bash
cd tests
cmake -B build .
cmake --build build
./build/run_tests
```

## Installation

Register the DLL as a TSF text service (requires Administrator privileges):

```cmd
regsvr32 TextExpansionKeyboard.dll
```

To unregister:

```cmd
regsvr32 /u TextExpansionKeyboard.dll
```

After registration, the text service appears in **Settings → Time & Language → Language & Region** under your keyboard list.

## Architecture

```
Physical Keyboard
     ↓
Windows Text Input Pipeline
     ↓
TSF Text Service (TextExpansionKeyboard.dll)
     ↓
Input Buffer Manager (rolling 3-char buffer)
     ↓
Trigger Detector (detects "$#$")
     ↓
Expansion Dictionary (shortcut → phrase lookup)
     ↓
Text Replacement Engine (TSF APIs)
     ↓
Target Application
```

### Core Components

- **InputBuffer**: Fixed rolling buffer that stores the last typed characters and detects the `$#$` trigger sequence
- **ExpansionDictionary**: Maps shortcuts to full phrases with case-insensitive matching
- **TextService**: TSF text input processor that integrates with Windows input pipeline
- **ClassFactory**: COM class factory for TSF registration
- **Register**: Handles COM server and TSF profile registration

### Key TSF APIs Used

- `ITfTextInputProcessorEx` — Core text service interface
- `ITfKeyEventSink` — Keyboard event handling
- `ITfThreadMgrEventSink` — Thread manager events
- `ITfInsertAtSelection` — Text insertion and replacement
- `ITfRange` — Text range manipulation
- `ITfInputProcessorProfileMgr` — Profile registration

## Documentation

- [Deployment Guide](docs/deployment_guide.md) — Build, register, and deploy
- [User Manual](docs/user_manual.md) — End-user usage instructions