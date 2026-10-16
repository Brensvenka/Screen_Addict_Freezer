// gui_app.h - The windowed front end (Win32 API, Windows only).
//
// One small fixed-size window. Each step of a flow is a "page": the old
// controls are destroyed and the new ones are laid out top to bottom.
// Flows are chained with callbacks: "ask the phrase, THEN show the digits,
// THEN say done". The logic (Vault, PasswordGenerator, phrase) is the same
// code the console version uses.
#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "../password_generator.h"
#include "../vault.h"

class GuiApp {
public:
    explicit GuiApp(HINSTANCE instance);
    ~GuiApp();
    int run(int showCommand);

private:
    using Action = std::function<void()>;

    // --- Window plumbing ---------------------------------------------------
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
    LRESULT handleMessage(UINT msg, WPARAM wp, LPARAM lp);
    int scale(int px) const;  // 96-dpi pixels -> real pixels
    void createFonts();

    HINSTANCE instance_;
    HWND window_ = nullptr;
    int dpi_ = 96;
    HFONT fontBody_ = nullptr, fontTitle_ = nullptr, fontDigit_ = nullptr, fontSmall_ = nullptr;

    // --- Page building -----------------------------------------------------
    void beginPage(const std::string& heading);
    HWND addText(const std::string& text, HFONT font, bool quiet = false, bool center = false);
    HWND addEdit();
    void addGap(int px);
    void addButton(const std::string& label, Action action, bool primary = false, bool enabled = true);
    void addButtonRow(const std::string& primaryLabel, Action primary,
                      const std::string& secondaryLabel, Action secondary);
    void finishPage();

    std::vector<HWND> controls_;
    std::vector<HWND> quietLabels_;
    std::map<int, Action> actions_;  // control id -> what a click does
    int nextId_ = 100;
    int cursorY_ = 0;
    HWND focus_ = nullptr;
    HWND edit1_ = nullptr, edit2_ = nullptr;

    // --- Pages -------------------------------------------------------------
    void showHome();
    void showMessage(const std::string& heading, const std::string& text);
    void showInfo(const std::string& heading, const std::string& text, Action next,
                  Action onCancel = nullptr);
    void askPhrase(const std::string& stored, Action onMatch, const std::string& error = "");
    void askNewPhrase(std::function<void(const std::string&)> onDone, const std::string& error = "");
    void showDigits(const std::string& title, const std::string& code, Action onDone,
                    std::size_t index = 0);
    void askYesNo(const std::string& heading, const std::string& question, Action yes, Action no);
    void chooseLanguage(Action next);

    // --- Flows (same logic as the console App) ----------------------------
    void unlock();
    void firstSetup();
    void rotate();
    void changePhrase();
    void changeLanguage();
    void startRotation();
    void resolveInterruptedRotation();
    void resolveUnfinishedSetup();
    void confirmAndCommit(const VaultData& next);

    Vault vault_;
    Vault pending_;
    PasswordGenerator generator_;
    VaultData data_;
    bool loaded_ = false;
    bool positioned_ = false;  // centred on screen once, on the first page
};
