# Screen Addict Freezer

**English** | [Türkçe](README.tr.md)

Lock your iPhone's social media behind a passcode that **you don't know** — your PC knows it.

iPhone Screen Time can block apps, but if you set the passcode yourself, you can also unblock them in two seconds. Screen Addict Freezer moves the passcode off your head and onto your computer:

- The program generates a random 4-digit Screen Time passcode and keeps it in an obfuscated file.
- It shows the code **one digit at a time**, so you never see all four digits together.
- To see the code, you have to sit at your PC and type a sentence you chose yourself, e.g. *"yes I am weak and I need my phone"*.
- You can **rotate** the code at any time, which makes any code you happened to remember useless.

No Mac, no paid Apple Developer account, no cloud service, no app on the phone. Just iPhone's own Screen Time and a small C++ console program.

## How it works

```
 iPhone                                   PC
┌───────────────────────────┐      ┌──────────────────────────────┐
│ Screen Time               │      │ screen_addict_freezer.exe     │
│  - Downtime (e.g. 21-20)  │      │                              │
│  - app install/delete off │◄─────│ vault.dat  ──► code 4-8-1-6  │
│  - web limits             │ code │ shown digit by digit, only    │
│  - passcode: ????         │      │ after your sentence           │
└───────────────────────────┘      └──────────────────────────────┘
 Recovery: a SECOND Apple Account that you don't use day to day
```

## Honest limits

This is a **speed bump against yourself**, not a security product.

- The vault file is obfuscated (XOR), **not encrypted**. The key is in this repository. Anyone with the file and the code can recover the passcode, and that includes you on a bad day.
- The recovery Apple Account can reset the Screen Time passcode. Keep its password somewhere inconvenient (a sealed envelope, a family member).
- If you really want to break out, you can. The goal is that in a weak moment the path is long enough (PC → program → sentence → digits) that you give up.

## Requirements

- An iPhone with Screen Time (iOS 12 or later)
- A second Apple Account, used only as the Screen Time recovery account
- A Windows PC (Linux and macOS also work if you build from source)

## Install

### Option A: build with MinGW (Windows)

1. Install a C++ compiler, e.g. [MSYS2](https://www.msys2.org/) or [WinLibs](https://winlibs.com/) MinGW-w64.
2. Download this repository (green **Code** button → *Download ZIP*) and unzip it.
3. Double-click `build.bat`, or run it from a terminal in the folder.
4. Copy `screen_addict_freezer.exe` to the folder where you want to keep your vault. Not inside the repository folder.

### Option B: CMake (any platform)

```bash
cmake -B build
cmake --build build --config Release
```

## Usage

Follow **[docs/iphone-setup.md](docs/iphone-setup.md)** for the full, step-by-step guide. In short:

1. On the iPhone, lock Screen Time with a **temporary passcode you choose**, and set the second Apple Account as recovery.
2. Do all the settings with the temporary passcode: Downtime, content restrictions, web limits.
3. Run the program → **2) First setup** → choose your confirmation sentence.
4. On the iPhone, *Change Screen Time Passcode*: type your temporary code, then the new code the program shows you digit by digit.
5. Back up `vault.dat` (e.g. on a USB stick).

| Menu | What it does |
|---|---|
| 1) Unlock | Type your sentence → the code is shown digit by digit. The code does not change. |
| 2) First setup | Only when there is no vault yet. |
| 3) Rotate code | Type your sentence → old code + new code are shown → the phone gets a new code you don't know. |
| 4) Change confirmation phrase | Requires the current sentence. |
| 5) Language | English / Türkçe |

## Files

| File | Content |
|---|---|
| `vault.dat` | Current code, your sentence and language. **Back it up. Never commit it.** |
| `vault_pending.dat` | Exists only while a code change is in progress. The program resolves it on the next run. |
| `kasa.dat` | Vault of the very first version. Migrated automatically to `vault.dat`. |

The program creates these files in the **current working directory**, i.e. the folder you run it from.

## If something goes wrong

- **vault.dat lost or corrupted:** restore your backup. If you have none: iPhone → Settings → Screen Time → Change Screen Time Passcode → *Forgot Passcode?* → sign in with the recovery Apple Account → set a new code → run *First setup* again.
- **Forgot the recovery account's password:** reset it at [iforgot.apple.com](https://iforgot.apple.com) from another device.
- **Emergency:** Downtime never blocks phone calls. Emergency calls always work.

## Project structure

```
src/
  main.cpp                  entry point
  app.h/.cpp                menu and all user flows
  vault.h/.cpp              obfuscated file storage with checksum
  password_generator.h/.cpp random, non-trivial 4-digit codes
  phrase.h/.cpp             sentence normalization (case, Turkish letters, spaces)
  console.h/.cpp            console helpers, digit-by-digit display
  i18n.h/.cpp               English / Turkish texts
docs/                       iPhone setup guides (EN / TR)
```

## License

[MIT](LICENSE)
