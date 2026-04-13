# Deployment Guide

## Text Expansion Keyboard — Deployment Guide

### Prerequisites

- Windows 10 or later (64-bit recommended)
- Visual Studio 2022 with C++ Desktop Development workload
- Administrator privileges for registration

### Building the Project

1. Open `TextExpansionKeyboard.sln` in Visual Studio 2022
2. Select the desired configuration:
   - **Debug|x64** for 64-bit debug build
   - **Release|x64** for 64-bit release build
   - **Debug|Win32** or **Release|Win32** for 32-bit builds
3. Build the solution (Ctrl+Shift+B)
4. The output DLL will be in `x64\Release\` or `x64\Debug\`

### Registration

The text service must be registered as a COM server and TSF profile before use.

#### Register the DLL

Open an **Administrator Command Prompt** and run:

```cmd
regsvr32 TextExpansionKeyboard.dll
```

This will:
- Register the COM server in the Windows registry
- Register the TSF input processor profile
- Register the keyboard category

#### Unregister the DLL

```cmd
regsvr32 /u TextExpansionKeyboard.dll
```

### Verifying Installation

1. Open **Settings → Time & Language → Language & Region**
2. Click on your language (e.g., English (United States))
3. Click **Language options**
4. Under **Keyboards**, you should see **Text Expansion Keyboard**

### Configuration

The default dictionary is built into the DLL. Custom dictionaries can be configured via the `config/dictionary.json` file.

### Troubleshooting

| Issue | Solution |
|-------|----------|
| DLL registration fails | Run command prompt as Administrator |
| Text service not showing | Restart the computer after registration |
| Expansions not working in some apps | Ensure the app supports TSF input |
| 32-bit app on 64-bit Windows | Register the 32-bit DLL as well |

### Uninstallation

1. Unregister the DLL: `regsvr32 /u TextExpansionKeyboard.dll`
2. Delete the DLL file
3. Restart the computer
