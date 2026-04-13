# User Manual

## Text Expansion Keyboard — User Manual

### What is Text Expansion Keyboard?

Text Expansion Keyboard is a Windows-native smart typing tool that automatically expands short abbreviations into full phrases. It works system-wide in any application that supports Windows text input (TSF), including browsers, text editors, word processors, and more.

### How It Works

1. Type a **shortcut** (1–3 characters)
2. Type the **trigger sequence**: `$#$`
3. The shortcut and trigger are automatically replaced with the full phrase

#### Example

| You Type | Result |
|----------|--------|
| `gom$#$` | Good morning |
| `ty$#$` | Thank you |
| `brg$#$` | Best regards |
| `tnx$#$` | Thank you for contacting support |
| `bp$#$` | Blood pressure |

### Default Shortcuts

| Shortcut | Expansion |
|----------|-----------|
| `gom` | Good morning |
| `gm` | Good morning |
| `ge` | Good evening |
| `gn` | Good night |
| `ty` | Thank you |
| `thx` | Thanks |
| `tnx` | Thank you for contacting support |
| `brg` | Best regards |
| `br` | Best regards |
| `yw` | You're welcome |
| `pls` | Please |
| `omw` | On my way |
| `idk` | I don't know |
| `np` | No problem |
| `ttl` | Talk to you later |
| `fyi` | For your information |
| `eta` | Estimated time of arrival |
| `bp` | Blood pressure |
| `sig` | Sincerely, (signature) |

### Trigger Sequence

The trigger sequence is **`$#$`** (dollar, hash, dollar). This was chosen because:

- It does not conflict with normal typing
- It is easy to type on any keyboard
- It works even when browser shortcuts are blocked
- It avoids accidental triggering

### Supported Applications

Text Expansion Keyboard works in:

- **Notepad** and other Windows built-in editors
- **Microsoft Word**, Excel, and other Office apps
- **Web browsers** (Chrome, Edge, Firefox) — text fields on web pages
- **Visual Studio Code** and other code editors
- **Chat applications** and messaging tools
- **CRM software** and form input areas

### Tips

- Shortcuts are **case-insensitive**: `GOM$#$` works the same as `gom$#$`
- The trigger only activates after the complete `$#$` sequence
- If a shortcut is not found in the dictionary, no replacement occurs
- The input buffer resets when you switch between applications

### Troubleshooting

| Problem | Solution |
|---------|----------|
| Nothing happens when I type the trigger | Ensure the text service is registered and selected as the active input method |
| Expansion works in some apps but not others | Some applications may not support TSF input. Try a different app to verify |
| Wrong expansion appears | Check the shortcut dictionary for conflicts |

### How to Select the Input Method

1. Click the language indicator in the taskbar (bottom-right)
2. Select **Text Expansion Keyboard** from the input method list
3. Alternatively, press **Win + Space** to cycle through input methods
