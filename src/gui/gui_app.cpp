#include "gui_app.h"

#include "../core.h"
#include "../i18n.h"
#include "../phrase.h"

using i18n::t;

namespace {

const wchar_t* kWindowClass = L"ScreenAddictFreezerWindow";
const int kClientWidth = 420;  // in 96-dpi pixels
const int kMargin = 28;

// Win32 "W" functions speak UTF-16; our strings are UTF-8.
std::wstring widen(const std::string& s) {
    if (s.empty()) return std::wstring();
    const int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), nullptr, 0);
    std::wstring w(static_cast<std::size_t>(n), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.data(), static_cast<int>(s.size()), &w[0], n);
    return w;
}

std::string narrow(const std::wstring& w) {
    if (w.empty()) return std::string();
    const int n = WideCharToMultiByte(CP_UTF8, 0, w.data(), static_cast<int>(w.size()), nullptr, 0,
                                      nullptr, nullptr);
    std::string s(static_cast<std::size_t>(n), '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.data(), static_cast<int>(w.size()), &s[0], n, nullptr, nullptr);
    return s;
}

std::string textOf(HWND control) {
    const int len = GetWindowTextLengthW(control);
    std::wstring w(static_cast<std::size_t>(len) + 1, L'\0');
    GetWindowTextW(control, &w[0], len + 1);
    w.resize(static_cast<std::size_t>(len));
    return narrow(w);
}

}  // namespace

// ===========================================================================
// Window plumbing
// ===========================================================================
GuiApp::GuiApp(HINSTANCE instance)
    : instance_(instance), vault_(core::kVaultFile), pending_(core::kPendingFile) {}

GuiApp::~GuiApp() {
    for (HFONT f : {fontBody_, fontTitle_, fontDigit_, fontSmall_})
        if (f) DeleteObject(f);
}

int GuiApp::scale(int px) const { return MulDiv(px, dpi_, 96); }

void GuiApp::createFonts() {
    auto make = [this](int points, int weight) {
        return CreateFontW(-MulDiv(points, dpi_, 72), 0, 0, 0, weight, FALSE, FALSE, FALSE,
                           DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                           CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    };
    fontBody_ = make(10, FW_NORMAL);
    fontSmall_ = make(9, FW_NORMAL);
    fontTitle_ = make(15, FW_SEMIBOLD);
    fontDigit_ = make(60, FW_BOLD);
}

int GuiApp::run(int showCommand) {
    HDC screen = GetDC(nullptr);
    dpi_ = GetDeviceCaps(screen, LOGPIXELSY);
    ReleaseDC(nullptr, screen);
    createFonts();

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = &GuiApp::windowProc;
    wc.hInstance = instance_;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = kWindowClass;
    RegisterClassExW(&wc);

    window_ = CreateWindowExW(WS_EX_CONTROLPARENT, kWindowClass, L"Screen Addict Freezer",
                              WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                              CW_USEDEFAULT, CW_USEDEFAULT, scale(kClientWidth), scale(400),
                              nullptr, nullptr, instance_, this);
    if (!window_) return 1;

    // Same start-up as the console version.
    const bool migrated = core::migrateLegacy(vault_);
    loaded_ = vault_.load(data_);
    VaultData pendingData;
    if (loaded_) {
        i18n::set(data_.lang);
        if (migrated)
            showMessage(t("Old vault moved", "Eski kasa taşındı"),
                        t("kasa.dat was copied into vault.dat. Back up vault.dat, then you can "
                          "delete kasa.dat.",
                          "kasa.dat, vault.dat'a taşındı. vault.dat'ı yedekledikten sonra "
                          "kasa.dat'ı silebilirsin."));
        else
            showHome();
    } else if (!vault_.exists() && pending_.load(pendingData)) {
        i18n::set(pendingData.lang);
        data_.lang = pendingData.lang;
        resolveUnfinishedSetup();
    } else {
        chooseLanguage([this] { showHome(); });
    }

    ShowWindow(window_, showCommand);
    UpdateWindow(window_);
    if (focus_) SetFocus(focus_);  // focus set before the window was visible may not stick

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        // IsDialogMessage gives us Tab between controls, Enter = IDOK, Esc = IDCANCEL.
        if (!IsDialogMessageW(window_, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK GuiApp::windowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    // The App pointer travels in CREATESTRUCT once, then lives in the window's user data.
    if (msg == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lp);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
    }
    auto* self = reinterpret_cast<GuiApp*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (self && self->window_ == hwnd) return self->handleMessage(msg, wp, lp);
    return DefWindowProcW(hwnd, msg, wp, lp);
}

LRESULT GuiApp::handleMessage(UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_COMMAND: {
            const int id = LOWORD(wp);
            if (HIWORD(wp) != BN_CLICKED) break;
            auto it = actions_.find(id);
            if (it == actions_.end()) break;
            // Copy first: the action usually builds a new page, which clears actions_.
            Action action = it->second;
            action();
            return 0;
        }
        case WM_CTLCOLORSTATIC: {
            HDC dc = reinterpret_cast<HDC>(wp);
            HWND control = reinterpret_cast<HWND>(lp);
            bool quiet = false;
            for (HWND q : quietLabels_)
                if (q == control) quiet = true;
            SetTextColor(dc, quiet ? RGB(105, 105, 105) : GetSysColor(COLOR_WINDOWTEXT));
            SetBkColor(dc, GetSysColor(COLOR_WINDOW));
            return reinterpret_cast<LRESULT>(GetSysColorBrush(COLOR_WINDOW));
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            break;
    }
    return DefWindowProcW(window_, msg, wp, lp);
}

// ===========================================================================
// Page building: controls are stacked top to bottom at cursorY_
// ===========================================================================
void GuiApp::beginPage(const std::string& heading) {
    for (HWND c : controls_) DestroyWindow(c);
    controls_.clear();
    quietLabels_.clear();
    actions_.clear();
    nextId_ = 100;
    focus_ = edit1_ = edit2_ = nullptr;
    cursorY_ = scale(18);
    addText("SCREEN ADDICT FREEZER", fontSmall_, true);
    addGap(2);
    addText(heading, fontTitle_);
    addGap(10);
}

HWND GuiApp::addText(const std::string& text, HFONT font, bool quiet, bool center) {
    const std::wstring w = widen(text);
    const int width = scale(kClientWidth - 2 * kMargin);

    // Measure how tall the wrapped text will be.
    HDC dc = GetDC(window_);
    HGDIOBJ old = SelectObject(dc, font);
    RECT r = {0, 0, width, 0};
    DrawTextW(dc, w.c_str(), -1, &r, DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX);
    SelectObject(dc, old);
    ReleaseDC(window_, dc);

    HWND h = CreateWindowExW(0, L"STATIC", w.c_str(),
                             WS_CHILD | WS_VISIBLE | SS_NOPREFIX | (center ? SS_CENTER : SS_LEFT),
                             scale(kMargin), cursorY_, width, r.bottom, window_, nullptr,
                             instance_, nullptr);
    SendMessageW(h, WM_SETFONT, reinterpret_cast<WPARAM>(font), TRUE);
    controls_.push_back(h);
    if (quiet) quietLabels_.push_back(h);
    cursorY_ += r.bottom + scale(6);
    return h;
}

HWND GuiApp::addEdit() {
    HWND h = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                             WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
                             scale(kMargin), cursorY_, scale(kClientWidth - 2 * kMargin), scale(30),
                             window_, nullptr, instance_, nullptr);
    SendMessageW(h, WM_SETFONT, reinterpret_cast<WPARAM>(fontBody_), TRUE);
    controls_.push_back(h);
    if (!focus_) focus_ = h;
    cursorY_ += scale(30 + 8);
    return h;
}

void GuiApp::addGap(int px) { cursorY_ += scale(px); }

void GuiApp::addButton(const std::string& label, Action action, bool primary, bool enabled) {
    const int id = primary ? IDOK : nextId_++;
    HWND h = CreateWindowExW(0, L"BUTTON", widen(label).c_str(),
                             WS_CHILD | WS_VISIBLE | WS_TABSTOP |
                                 (primary ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON),
                             scale(kMargin), cursorY_, scale(kClientWidth - 2 * kMargin), scale(36),
                             window_, reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)), instance_,
                             nullptr);
    SendMessageW(h, WM_SETFONT, reinterpret_cast<WPARAM>(fontBody_), TRUE);
    EnableWindow(h, enabled);
    controls_.push_back(h);
    actions_[id] = std::move(action);
    if (!focus_ && enabled) focus_ = h;
    cursorY_ += scale(36 + 8);
}

void GuiApp::addButtonRow(const std::string& primaryLabel, Action primary,
                          const std::string& secondaryLabel, Action secondary) {
    const int full = kClientWidth - 2 * kMargin, gap = 10, half = (full - gap) / 2;
    struct Spec { const std::string& label; int id; int x; DWORD style; };
    const Spec specs[] = {
        {secondaryLabel, IDCANCEL, kMargin, BS_PUSHBUTTON},
        {primaryLabel, IDOK, kMargin + half + gap, BS_DEFPUSHBUTTON},
    };
    for (const Spec& s : specs) {
        HWND h = CreateWindowExW(0, L"BUTTON", widen(s.label).c_str(),
                                 WS_CHILD | WS_VISIBLE | WS_TABSTOP | s.style, scale(s.x),
                                 cursorY_, scale(half), scale(36), window_,
                                 reinterpret_cast<HMENU>(static_cast<INT_PTR>(s.id)), instance_,
                                 nullptr);
        SendMessageW(h, WM_SETFONT, reinterpret_cast<WPARAM>(fontBody_), TRUE);
        controls_.push_back(h);
        if (s.id == IDOK && !focus_) focus_ = h;
    }
    actions_[IDOK] = std::move(primary);
    actions_[IDCANCEL] = std::move(secondary);
    cursorY_ += scale(36 + 8);
}

void GuiApp::finishPage() {
    // Grow or shrink the window to fit the page.
    RECT r = {0, 0, scale(kClientWidth), cursorY_ + scale(14)};
    const DWORD style = static_cast<DWORD>(GetWindowLongPtrW(window_, GWL_STYLE));
    const DWORD exStyle = static_cast<DWORD>(GetWindowLongPtrW(window_, GWL_EXSTYLE));
    AdjustWindowRectEx(&r, style, FALSE, exStyle);
    const int w = r.right - r.left, h = r.bottom - r.top;
    if (!positioned_) {
        RECT work;
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);
        SetWindowPos(window_, nullptr, work.left + (work.right - work.left - w) / 2,
                     work.top + (work.bottom - work.top - h) / 2, w, h, SWP_NOZORDER);
        positioned_ = true;
    } else {
        SetWindowPos(window_, nullptr, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER);
    }
    if (focus_) SetFocus(focus_);
}

// ===========================================================================
// Pages
// ===========================================================================
void GuiApp::showHome() {
    if (loaded_) {
        beginPage(t("Your code is locked away", "Şifren kilit altında"));
        addText(t("The Screen Time passcode lives on this PC. You don't know it, and that's the point.",
                  "Ekran Süresi şifresi bu bilgisayarda duruyor. Sen bilmiyorsun; olay da bu."),
                fontBody_, true);
    } else if (vault_.exists()) {
        beginPage(t("vault.dat is damaged", "vault.dat bozuk"));
        addText(t("Restore it from your backup, or reset the Screen Time passcode with your "
                  "recovery Apple Account and run First setup again.",
                  "Yedekten geri yükle ya da Ekran Süresi şifresini kurtarma Apple hesabınla "
                  "sıfırlayıp İlk kurulum'u yeniden yap."),
                fontBody_, true);
    } else {
        beginPage(t("No vault yet", "Henüz kasa yok"));
        addText(t("Start with First setup. It walks you through the phone side too.",
                  "İlk kurulum ile başla. Telefon tarafını da adım adım anlatıyor."),
                fontBody_, true);
    }
    addGap(6);
    addButton(t("Unlock (show code)", "Kilidi aç (şifreyi göster)"), [this] { unlock(); }, loaded_,
              loaded_);
    addButton(t("Rotate code", "Şifreyi yenile"), [this] { rotate(); }, false, loaded_);
    addButton(t("Change confirmation phrase", "Onay cümlesini değiştir"), [this] { changePhrase(); },
              false, loaded_);
    addButton(t("First setup", "İlk kurulum"), [this] { firstSetup(); }, !vault_.exists(),
              !vault_.exists());
    addButton("Türkçe / English", [this] { changeLanguage(); });
    addGap(4);
    addButton(t("Exit", "Çıkış"), [this] { DestroyWindow(window_); });
    finishPage();
}

void GuiApp::showMessage(const std::string& heading, const std::string& text) {
    beginPage(heading);
    addText(text, fontBody_);
    addGap(8);
    addButton(t("Back to menu", "Ana menü"), [this] { showHome(); }, true);
    finishPage();
}

void GuiApp::showInfo(const std::string& heading, const std::string& text, Action next,
                      Action onCancel) {
    beginPage(heading);
    addText(text, fontBody_);
    addGap(8);
    if (onCancel)
        addButtonRow(t("Continue", "Devam"), std::move(next), t("Cancel", "Vazgeç"),
                     std::move(onCancel));
    else
        addButton(t("Continue", "Devam"), std::move(next), true);
    finishPage();
}

void GuiApp::askPhrase(const std::string& stored, Action onMatch, const std::string& error) {
    beginPage(t("Type your sentence", "Cümleni yaz"));
    addText(t("To go on, type this sentence:", "Devam etmek için şu cümleyi yaz:"), fontBody_, true);
    addText("\xE2\x80\x9C" + stored + "\xE2\x80\x9D", fontBody_);  // “ ”
    edit1_ = addEdit();
    if (!error.empty()) addText(error, fontSmall_);
    addGap(4);
    addButtonRow(
        t("Continue", "Devam"),
        [this, stored, onMatch] {
            if (phrase::matches(textOf(edit1_), stored))
                onMatch();
            else
                askPhrase(stored, onMatch,
                          t("That didn't match. Case and Turkish letters don't matter, words do.",
                            "Cümle tutmadı. Büyük/küçük harf ve Türkçe harfler önemli değil, "
                            "kelimeler önemli."));
        },
        t("Cancel", "Vazgeç"), [this] { showHome(); });
    finishPage();
}

void GuiApp::askNewPhrase(std::function<void(const std::string&)> onDone,
                          const std::string& error) {
    beginPage(t("Choose your sentence", "Onay cümleni seç"));
    addText(t("You'll type it every time you want to see the code, so make it honest. "
              "At least 10 characters.",
              "Şifreyi her görmek istediğinde bunu yazacaksın; dürüst bir cümle olsun. "
              "En az 10 karakter."),
            fontBody_, true);
    addText(t("Sentence", "Cümle"), fontSmall_);
    edit1_ = addEdit();
    addText(t("Type it again", "Tekrar yaz"), fontSmall_);
    edit2_ = addEdit();
    if (!error.empty()) addText(error, fontSmall_);
    addGap(4);
    addButtonRow(
        t("Save", "Kaydet"),
        [this, onDone] {
            const std::string candidate = phrase::normalize(textOf(edit1_));
            if (!phrase::isValid(candidate)) {
                askNewPhrase(onDone, t("Use 10-200 characters: letters, digits, spaces, punctuation.",
                                       "10-200 karakter olmalı: harf, rakam, boşluk, noktalama."));
            } else if (!phrase::matches(textOf(edit2_), candidate)) {
                askNewPhrase(onDone, t("The two entries are different.", "İki giriş birbirini tutmuyor."));
            } else {
                onDone(candidate);
            }
        },
        t("Cancel", "Vazgeç"), [this] { showHome(); });
    finishPage();
}

void GuiApp::showDigits(const std::string& title, const std::string& code, Action onDone,
                        std::size_t index) {
    beginPage(title);
    addText(std::string(t("Digit ", "Rakam ")) + std::to_string(index + 1) + " / " +
                std::to_string(code.size()),
            fontBody_, true, true);
    addText(std::string(1, code.at(index)), fontDigit_, false, true);
    addText(t("Type it on the phone, then continue.", "Telefona yaz, sonra devam et."), fontSmall_,
            true, true);
    addGap(6);
    const bool last = index + 1 >= code.size();
    addButton(last ? t("Done", "Bitti") : t("Next digit", "Sonraki rakam"),
              [this, title, code, onDone, index, last] {
                  if (last)
                      onDone();
                  else
                      showDigits(title, code, onDone, index + 1);
              },
              true);
    finishPage();
}

void GuiApp::askYesNo(const std::string& heading, const std::string& question, Action yes,
                      Action no) {
    beginPage(heading);
    addText(question, fontBody_);
    addGap(8);
    addButtonRow(t("Yes", "Evet"), std::move(yes), t("No", "Hayır"), std::move(no));
    finishPage();
}

void GuiApp::chooseLanguage(Action next) {
    beginPage("Dil / Language");
    addButton("Türkçe", [this, next] { i18n::set(i18n::Lang::Turkish); next(); }, true);
    addButton("English", [this, next] { i18n::set(i18n::Lang::English); next(); });
    finishPage();
}

// ===========================================================================
// Flows
// ===========================================================================
void GuiApp::unlock() {
    askPhrase(data_.phrase, [this] {
        if (pending_.exists()) {
            resolveInterruptedRotation();
            return;
        }
        showDigits(t("Current code", "Geçerli şifre"), data_.code, [this] {
            showMessage(t("Done", "Tamam"),
                        t("The code did not change. Use Rotate code whenever you like.",
                          "Şifre değişmedi. İstediğin zaman Şifreyi yenile ile değiştirebilirsin."));
        });
    });
}

void GuiApp::rotate() {
    // The OLD code is shown during a rotation, so the sentence is asked here too.
    askPhrase(data_.phrase, [this] {
        if (pending_.exists())
            resolveInterruptedRotation();
        else
            startRotation();
    });
}

void GuiApp::changePhrase() {
    askPhrase(data_.phrase, [this] {
        askNewPhrase([this](const std::string& p) {
            VaultData next = data_;
            next.phrase = p;
            if (vault_.save(next)) {
                data_ = next;
                showMessage(t("Sentence changed", "Cümle değişti"),
                            t("Update your backup of vault.dat.", "vault.dat yedeğini güncelle."));
            } else {
                showMessage(t("Error", "Hata"),
                            t("Could not save. The old sentence is still valid.",
                              "Kaydedilemedi. Eski cümle geçerli."));
            }
        });
    });
}

void GuiApp::changeLanguage() {
    chooseLanguage([this] {
        if (loaded_) {
            VaultData next = data_;
            next.lang = i18n::current();
            if (vault_.save(next)) data_ = next;
        }
        showHome();
    });
}

void GuiApp::firstSetup() {
    showInfo(
        t("First setup", "İlk kurulum"),
        t("1. On the iPhone: Settings > Screen Time > Lock Screen Time Settings. Pick a TEMPORARY "
          "passcode you know (e.g. 1357). For recovery, enter your SECOND Apple Account.\n\n"
          "2. Do all the settings with that temporary code: Downtime or App Limits, restrictions, "
          "web limits (see docs/iphone-setup.md).\n\n"
          "3. When the phone is ready, continue here.",
          "1. iPhone'da: Ayarlar > Ekran Süresi > Ekran Süresi Ayarlarını Kilitle. BİLDİĞİN GEÇİCİ "
          "bir şifre seç (örneğin 1357). Kurtarma için İKİNCİ Apple hesabını gir.\n\n"
          "2. Tüm ayarları bu geçici şifreyle yap: Kapalı Kalma Süresi ya da Uygulama Sınırları, "
          "kısıtlamalar, web engeli (bkz. docs/iphone-kurulum.md).\n\n"
          "3. Telefon hazır olunca buradan devam et."),
        [this] {
            askNewPhrase([this](const std::string& p) {
                VaultData next;
                next.lang = i18n::current();
                next.phrase = p;
                next.code = generator_.generate();
                if (!pending_.save(next)) {
                    showMessage(t("Error", "Hata"),
                                t("Could not write vault_pending.dat. Nothing changed.",
                                  "vault_pending.dat yazılamadı. Hiçbir şey değişmedi."));
                    return;
                }
                showInfo(
                    t("Switch to the real code", "Gerçek şifreye geç"),
                    t("On the phone: Settings > Screen Time > Change Screen Time Passcode.\n\n"
                      "First type your TEMPORARY code yourself. Then the new code is shown here "
                      "digit by digit, twice.",
                      "Telefonda: Ayarlar > Ekran Süresi > Ekran Süresi Şifresini Değiştir.\n\n"
                      "Önce GEÇİCİ şifreni kendin yaz. Sonra yeni şifre burada rakam rakam, iki "
                      "kez gösterilecek."),
                    [this, next] {
                        showDigits(t("New code (1st entry)", "Yeni şifre (1. giriş)"), next.code,
                                   [this, next] {
                                       showDigits(t("New code (2nd entry)", "Yeni şifre (2. giriş)"),
                                                  next.code, [this, next] { confirmAndCommit(next); });
                                   });
                    },
                    [this] {
                        pending_.remove();
                        showHome();
                    });
            });
        },
        [this] { showHome(); });
}

// Two-phase change: the new code goes to vault_pending.dat first and only
// replaces vault.dat after the user confirms the phone accepted it.
void GuiApp::startRotation() {
    VaultData next = data_;
    next.code = generator_.generate(data_.code);
    if (!pending_.save(next)) {
        showMessage(t("Error", "Hata"),
                    t("Could not write vault_pending.dat. Don't change anything.",
                      "vault_pending.dat yazılamadı. Hiçbir şeyi değiştirme."));
        return;
    }
    showInfo(t("Change the code on the phone", "Telefonda şifreyi değiştir"),
             t("Settings > Screen Time > Change Screen Time Passcode.\n\n"
               "The phone asks for the OLD code first, then the NEW code twice. Each one is "
               "shown here digit by digit.",
               "Ayarlar > Ekran Süresi > Ekran Süresi Şifresini Değiştir.\n\n"
               "Telefon önce ESKİ şifreyi, sonra YENİ şifreyi iki kez soracak. Hepsi burada "
               "rakam rakam gösterilecek."),
             [this, next] {
                 showDigits(t("Old code", "Eski şifre"), data_.code, [this, next] {
                     showDigits(t("New code (1st entry)", "Yeni şifre (1. giriş)"), next.code,
                                [this, next] {
                                    showDigits(t("New code (2nd entry)", "Yeni şifre (2. giriş)"),
                                               next.code, [this, next] { confirmAndCommit(next); });
                                });
                 });
             },
             [this] {
                 pending_.remove();
                 showHome();
             });
}

// A previous rotation stopped halfway: we don't know which code the phone has.
void GuiApp::resolveInterruptedRotation() {
    VaultData next;
    if (!pending_.load(next)) {
        pending_.remove();
        showMessage(t("Cleaned up", "Temizlendi"),
                    t("A damaged vault_pending.dat was removed. Try again.",
                      "Bozuk vault_pending.dat silindi. Tekrar dene."));
        return;
    }
    showInfo(t("An earlier change was interrupted", "Yarıda kalmış bir değişiklik var"),
             t("We don't know which code the phone has now. Try the OLD code first.",
               "Telefonda hangi şifrenin geçerli olduğu belli değil. Önce ESKİ şifreyi dene."),
             [this, next] {
                 showDigits(t("Old code (try it)", "Eski şifre (dene)"), data_.code, [this, next] {
                     askYesNo(
                         t("Did it work?", "Çalıştı mı?"),
                         t("Did the phone accept the old code?", "Telefon eski şifreyi kabul etti mi?"),
                         [this] {
                             pending_.remove();
                             startRotation();
                         },
                         [this, next] {
                             showDigits(t("New code (try it)", "Yeni şifre (dene)"), next.code,
                                        [this, next] { confirmAndCommit(next); });
                         });
                 });
             });
}

// The program stopped during first setup, after the new code was shown.
void GuiApp::resolveUnfinishedSetup() {
    VaultData next;
    if (!pending_.load(next)) {
        pending_.remove();
        showHome();
        return;
    }
    askPhrase(next.phrase, [this, next] {
        showInfo(t("Unfinished setup", "Yarım kalmış kurulum"),
                 t("Try this code on the phone.", "Bu şifreyi telefonda dene."), [this, next] {
                     showDigits(t("Code to try", "Denenecek şifre"), next.code,
                                [this, next] { confirmAndCommit(next); });
                 });
    });
}

void GuiApp::confirmAndCommit(const VaultData& next) {
    askYesNo(
        t("Last check", "Son kontrol"),
        t("Did the phone accept the new code?", "Telefon yeni şifreyi kabul etti mi?"),
        [this, next] {
            if (!vault_.save(next)) {
                showMessage(t("Error", "Hata"),
                            t("Could not write vault.dat! Do NOT delete vault_pending.dat.",
                              "vault.dat yazılamadı! vault_pending.dat'ı SİLME."));
                return;
            }
            pending_.remove();
            data_ = next;
            loaded_ = true;
            showMessage(t("Done", "Tamam"),
                        t("The new code is saved. Back up vault.dat, e.g. to a USB stick.",
                          "Yeni şifre kaydedildi. vault.dat'ı yedekle, örneğin bir USB belleğe."));
        },
        [this] {
            pending_.remove();
            showMessage(t("Cancelled", "İptal edildi"),
                        t("The phone still uses its previous code.", "Telefonda önceki şifre geçerli."));
        });
}
