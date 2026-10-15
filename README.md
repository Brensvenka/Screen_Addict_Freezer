# Screen Addict Freezer

**English** | [Türkçe](README.tr.md)

When you set your own Screen Time passcode, getting out of that loop is way too easy; you can change it or turn it off whenever you like. This little program holds the passcode for you: the phone stays locked, the code lives on your PC, and you don't know what it is.

## What it does

- Picks a random 4-digit Screen Time passcode. All four digits are different, because codes like 1171 stick in your head right away.
- Shows the code **one digit at a time**. You never see all four together.
- Before it shows anything, you sit down at your PC and type a sentence you chose yourself. Something like *"yes I am weak and I need my phone"*. Typing that in the moment tends to make you think twice.
- If a code ever sticks in your memory, you rotate it with one menu option and the one you know stops working.

No Mac, no paid Apple Developer account, no app on the phone. Just the iPhone's own Screen Time settings and a small C++ program on your computer.

## How it works

```
 iPhone                                   PC
┌───────────────────────────┐      ┌──────────────────────────────┐
│ Screen Time               │      │ screen_addict_freezer.exe     │
│  - Downtime / App Limits  │      │                              │
│  - app install/delete off │◄─────│ vault.dat  ──► code 4-8-1-6  │
│  - web limits             │ code │ shown digit by digit, only    │
│  - passcode: ????         │      │ after your sentence           │
└───────────────────────────┘      └──────────────────────────────┘
 Recovery: a SECOND Apple Account that you don't use day to day
```

## Let's be honest

This isn't a security tool. It's an annoying, discouraging obstacle you put in your own way.

- The file that holds the code is only obfuscated (XOR), not encrypted. The key is right here in this repo. Anyone with the file and the code can recover the passcode.
- The recovery Apple Account can reset the Screen Time passcode.
- If you really want to get around it, you will. The point is that in a weak moment the road is long enough: turn on the PC, run the program, type the sentence, collect the digits one by one. Most of the time that's enough to make you give up.

## What you need

- An iPhone with Screen Time (iOS 12 or later)
- A second Apple Account, used only to recover the Screen Time passcode
- A Windows PC. Linux and macOS work too if you build from source.

## Install

### Windows with MinGW

1. Install a C++ compiler, e.g. [MSYS2](https://www.msys2.org/) or [WinLibs](https://winlibs.com/) MinGW-w64.
2. Download this repo (green **Code** button → *Download ZIP*) and unzip it.
3. Double-click `build.bat`, or run it from a terminal in the folder.
4. Copy `screen_addict_freezer.exe` to the folder where you want to keep your passcode. Not inside the repo folder.

### CMake (any platform)

```bash
cmake -B build
cmake --build build --config Release
```

## Using it

The full step-by-step guide is in [docs/iphone-setup.md](docs/iphone-setup.md). The short version:

1. On the iPhone, lock Screen Time with a **temporary passcode you pick**. Use your second Apple Account for recovery.
2. Do all the settings with that temporary code. First pick the mode that fits your life:
   - **Hard mode (Downtime):** outside your chosen hours almost everything internet-based on the phone shuts down, and your access to websites goes with it.
   - **Balanced mode (App Limits), recommended:** only the social media apps you pick get a daily time budget. Safari and everything else keep working.
3. Run the program → **2) First setup** → choose your confirmation sentence.
4. On the iPhone, go to *Change Screen Time Passcode*. Type your temporary code, then the new code the program shows you digit by digit.
5. Back up `vault.dat` to a USB stick.

From now on, you don't know the passcode on your phone.

| Menu | What it does |
|---|---|
| 1) Unlock | Type your sentence and the code is shown digit by digit. The code stays the same. |
| 2) First setup | Only works when there's no vault yet. |
| 3) Rotate code | Type your sentence, see the old and new code, and give the phone a new code you don't know. |
| 4) Change confirmation phrase | You type the current sentence first. |
| 5) Language | English / Türkçe |

## Files

| File | What's in it |
|---|---|
| `vault.dat` | The current code, your sentence and your language. **Back it up. Never commit it.** |
| `vault_pending.dat` | Only exists while a code change is in progress. If something gets interrupted, the program sorts it out next time it starts. |
| `kasa.dat` | The file from the very first version. The program moves it into `vault.dat` on its own. |

The program writes these files to **whatever folder you run it from**.

## If something goes wrong

- **`vault.dat` is lost or corrupted:** Put your backup back. No backup? iPhone → Settings → Screen Time → Change Screen Time Passcode → *Forgot Passcode?* → sign in with the recovery Apple Account → set a new code → run *First setup* again.
- **You forgot the recovery account's password:** Reset it at [iforgot.apple.com](https://iforgot.apple.com) from another device.
- **Emergency:** Downtime never blocks phone calls. Emergency calls always work.

## Inside the code

```
src/
  main.cpp                  entry point
  app.h/.cpp                menu and all the flows
  vault.h/.cpp              the file that stores the code (obfuscation + checksum)
  password_generator.h/.cpp 4-digit codes with all-different digits
  phrase.h/.cpp             sentence matching (case, Turkish letters, spaces)
  console.h/.cpp            console helpers, digit-by-digit display
  i18n.h/.cpp               English / Turkish texts
docs/                       iPhone setup guides (EN / TR)
```

## License

[MIT](LICENSE). Use it, change it, share it however you like.
