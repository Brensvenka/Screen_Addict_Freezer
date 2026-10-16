#include "app.h"

#include <iostream>

#include "console.h"
#include "core.h"
#include "i18n.h"
#include "phrase.h"

using i18n::t;


App::App() : vault_(core::kVaultFile), pending_(core::kPendingFile) {}

// ---------------------------------------------------------------------------
int App::run() {
    migrateLegacy();

    loaded_ = vault_.load(data_);
    VaultData pendingData;
    if (loaded_) {
        i18n::set(data_.lang);
    } else if (!vault_.exists() && pending_.load(pendingData)) {
        i18n::set(pendingData.lang);  // unfinished setup: reuse the language chosen then
        data_.lang = pendingData.lang;
    } else {
        chooseLanguage();
        if (vault_.exists())
            std::cout << t("\nWARNING: vault.dat is corrupted. Restore it from your backup,\n"
                           "or reset the Screen Time passcode with your recovery Apple Account.\n",
                           "\nUYARI: vault.dat bozuk. Yedekten geri yukle ya da Ekran Suresi\n"
                           "sifresini kurtarma Apple hesabinla sifirla.\n");
    }
    if (!vault_.exists() && pending_.exists()) resolveUnfinishedSetup();

    while (true) {
        std::cout << "\n=== SCREEN ADDICT FREEZER ===\n"
                  << t("1) Unlock (show code)\n", "1) Kilidi ac (sifreyi goster)\n")
                  << t("2) First setup\n", "2) Ilk kurulum\n")
                  << t("3) Rotate code\n", "3) Sifreyi yenile\n")
                  << t("4) Change confirmation phrase\n", "4) Onay cumlesini degistir\n")
                  << t("5) Language / Dil\n", "5) Dil / Language\n")
                  << t("0) Exit\n", "0) Cikis\n")
                  << t("Choice: ", "Secim: ");
        const std::string c = console::readLine();
        if (c == "1") unlock();
        else if (c == "2") firstSetup();
        else if (c == "3") rotate();
        else if (c == "4") changePhrase();
        else if (c == "5") changeLanguage();
        else if (c == "0") return 0;
        else std::cout << t("Invalid choice.\n", "Gecersiz secim.\n");
    }
}

// ---------------------------------------------------------------------------
// Startup
// ---------------------------------------------------------------------------
void App::migrateLegacy() {
    if (!core::migrateLegacy(vault_)) return;
    std::cout << "kasa.dat -> vault.dat\n"
              << "Eski kasa tasindi. vault.dat'i yedekledikten sonra kasa.dat'i silebilirsin.\n"
              << "Onay cumlesi: \"" << core::kLegacyPhrase << "\" (4 ile degistirebilirsin)\n";
}

void App::chooseLanguage() {
    while (true) {
        std::cout << "\n1) English\n2) Turkce\n> ";
        const std::string c = console::readLine();
        if (c == "1") { i18n::set(i18n::Lang::English); break; }
        if (c == "2") { i18n::set(i18n::Lang::Turkish); break; }
    }
    data_.lang = i18n::current();
}

// The program stopped during first setup, after the new code was shown.
void App::resolveUnfinishedSetup() {
    VaultData next;
    if (!pending_.load(next)) {
        pending_.remove();
        return;
    }
    std::cout << t("\nAn unfinished setup was found.\n", "\nYarida kalmis bir kurulum bulundu.\n");
    if (!askPhrase(next.phrase)) return;
    console::waitEnter(t("Try this code on the phone. Press Enter...",
                         "Bu sifreyi telefonda dene. Enter'a bas..."));
    console::showDigitByDigit(t("CODE TO TRY", "DENENECEK SIFRE"), next.code);
    confirmAndCommit(next);
}

// ---------------------------------------------------------------------------
// Menu actions
// ---------------------------------------------------------------------------
void App::unlock() {
    if (!requireVault()) return;
    if (!askPhrase(data_.phrase)) return;
    if (pending_.exists()) {
        resolveInterruptedRotation();
        return;
    }
    console::waitEnter(t("\nThe code will be shown digit by digit. Press Enter...",
                         "\nSifre rakam rakam gosterilecek. Enter'a bas..."));
    console::showDigitByDigit(t("CURRENT CODE", "GECERLI SIFRE"), data_.code);
    std::cout << t("The code was not changed. Use 3) Rotate code whenever you like.\n",
                   "Sifre degismedi. Istedigin zaman 3) Sifreyi yenile ile degistirebilirsin.\n");
}

void App::firstSetup() {
    if (vault_.exists()) {
        std::cout << t("\nThe vault is already set up. Use 3) Rotate code to change the code.\n",
                       "\nKasa zaten kurulu. Sifreyi degistirmek icin 3) Sifreyi yenile.\n");
        return;
    }
    std::cout << t(
        "\nFIRST SETUP\n"
        "1. On the iPhone: Settings > Screen Time > Lock Screen Time Settings.\n"
        "   Pick a TEMPORARY passcode that you know (e.g. 1357).\n"
        "   For recovery, enter your SECOND Apple Account.\n"
        "2. Do all the settings with that temporary code\n"
        "   (Downtime, restrictions, web limits - see docs/iphone-setup.md).\n"
        "3. Come back here when everything is done.\n",
        "\nILK KURULUM\n"
        "1. iPhone'da: Ayarlar > Ekran Suresi > Ekran Suresi Ayarlarini Kilitle.\n"
        "   BILDIGIN GECICI bir sifre sec (ornegin 1357).\n"
        "   Kurtarma icin IKINCI Apple hesabini gir.\n"
        "2. Tum ayarlari bu gecici sifreyle yap\n"
        "   (Kapali Kalma Suresi, kisitlamalar, web - bkz. docs/iphone-kurulum.md).\n"
        "3. Hepsi bitince buraya don.\n");
    console::waitEnter(t("Press Enter when the phone is ready...",
                         "Telefon hazir olunca Enter'a bas..."));

    VaultData next;
    next.lang = i18n::current();
    next.phrase = chooseNewPhrase();
    next.code = generator_.generate();
    if (!pending_.save(next)) {
        std::cout << t("ERROR: could not write vault_pending.dat. Nothing changed.\n",
                       "HATA: vault_pending.dat yazilamadi. Hicbir sey degismedi.\n");
        return;
    }
    std::cout << t("\nOn the phone: Settings > Screen Time > Change Screen Time Passcode.\n"
                   "First type your TEMPORARY code yourself.\n"
                   "Then the new code will be shown digit by digit, twice.\n",
                   "\nTelefonda: Ayarlar > Ekran Suresi > Ekran Suresi Sifresini Degistir.\n"
                   "Once GECICI sifreni kendin gir.\n"
                   "Sonra yeni sifre rakam rakam, iki kez gosterilecek.\n");
    console::waitEnter(t("Press Enter when ready...", "Hazir olunca Enter'a bas..."));
    console::showDigitByDigit(t("NEW CODE (1st entry)", "YENI SIFRE (1. giris)"), next.code);
    console::showDigitByDigit(t("NEW CODE (2nd entry - confirm)", "YENI SIFRE (2. giris - onay)"),
                              next.code);
    confirmAndCommit(next);
}

void App::rotate() {
    if (!requireVault()) return;
    if (!askPhrase(data_.phrase)) return;  // the OLD code will be shown, so ask here too
    if (pending_.exists()) resolveInterruptedRotation();
    else startRotation();
}

void App::changePhrase() {
    if (!requireVault()) return;
    std::cout << t("\nTo change the phrase, first type the current one.",
                   "\nCumleyi degistirmek icin once mevcut cumleyi yaz.");
    if (!askPhrase(data_.phrase)) return;
    VaultData next = data_;
    next.phrase = chooseNewPhrase();
    if (vault_.save(next)) {
        data_ = next;
        std::cout << t("Phrase changed. Update your backup of vault.dat.\n",
                       "Cumle degisti. vault.dat yedegini guncelle.\n");
    } else {
        std::cout << t("ERROR: could not save. The old phrase is still valid.\n",
                       "HATA: kaydedilemedi. Eski cumle gecerli.\n");
    }
}

void App::changeLanguage() {
    chooseLanguage();
    if (!loaded_) return;  // no vault yet: the choice is saved during first setup
    VaultData next = data_;
    next.lang = i18n::current();
    if (vault_.save(next)) data_ = next;
}

// ---------------------------------------------------------------------------
// Building blocks
// ---------------------------------------------------------------------------
bool App::requireVault() {
    if (loaded_) return true;
    std::cout << (vault_.exists()
                      ? t("\nvault.dat is corrupted. Restore your backup or use the recovery Apple Account.\n",
                          "\nvault.dat bozuk. Yedegi geri yukle ya da kurtarma Apple hesabini kullan.\n")
                      : t("\nNo vault yet. Run 2) First setup.\n",
                          "\nHenuz kasa yok. 2) Ilk kurulum'u calistir.\n"));
    return false;
}

bool App::askPhrase(const std::string& stored) {
    std::cout << t("\nType this sentence:\n", "\nSu cumleyi yaz:\n") << "  \"" << stored << "\"\n> ";
    if (phrase::matches(console::readLine(), stored)) return true;
    std::cout << t("The sentence did not match.\n", "Cumle tutmadi.\n");
    return false;
}

std::string App::chooseNewPhrase() {
    while (true) {
        std::cout << t("\nChoose your confirmation phrase. You will type it every time you want\n"
                       "to see the code, so make it honest. (min 10 characters)\n> ",
                       "\nOnay cumleni sec. Sifreyi her gormek istediginde bunu yazacaksin,\n"
                       "durust bir cumle olsun. (en az 10 karakter)\n> ");
        const std::string candidate = phrase::normalize(console::readLine());
        if (!phrase::isValid(candidate)) {
            std::cout << t("Must be 10-200 characters (letters, digits, spaces, punctuation).\n",
                           "10-200 karakter olmali (harf, rakam, bosluk, noktalama).\n");
            continue;
        }
        std::cout << t("Type it again: ", "Tekrar yaz: ");
        if (phrase::matches(console::readLine(), candidate)) {
            std::cout << t("Saved as: \"", "Kaydedilen hali: \"") << candidate << "\"\n";
            return candidate;
        }
        std::cout << t("The two entries differ, try again.\n", "Iki giris farkli, tekrar dene.\n");
    }
}

// Two-phase change: the new code goes to vault_pending.dat first and only
// replaces vault.dat after the user confirms the phone accepted it. If the
// program dies in between, both codes are still on disk.
void App::startRotation() {
    VaultData next = data_;
    next.code = generator_.generate(data_.code);
    if (!pending_.save(next)) {
        std::cout << t("ERROR: could not write vault_pending.dat. Do not change anything.\n",
                       "HATA: vault_pending.dat yazilamadi. Hicbir seyi degistirme.\n");
        return;
    }
    std::cout << t("\nOn the phone: Settings > Screen Time > Change Screen Time Passcode.\n"
                   "It asks for the OLD code first, then the NEW code twice.\n",
                   "\nTelefonda: Ayarlar > Ekran Suresi > Ekran Suresi Sifresini Degistir.\n"
                   "Once ESKI sifre, sonra YENI sifre iki kez sorulacak.\n");
    console::waitEnter(t("Press Enter when ready...", "Hazir olunca Enter'a bas..."));
    console::showDigitByDigit(t("OLD CODE", "ESKI SIFRE"), data_.code);
    console::showDigitByDigit(t("NEW CODE (1st entry)", "YENI SIFRE (1. giris)"), next.code);
    console::showDigitByDigit(t("NEW CODE (2nd entry - confirm)", "YENI SIFRE (2. giris - onay)"),
                              next.code);
    confirmAndCommit(next);
}

// A previous rotation stopped halfway: we don't know which code the phone has.
void App::resolveInterruptedRotation() {
    VaultData next;
    if (!pending_.load(next)) {
        pending_.remove();
        std::cout << t("A corrupted vault_pending.dat was removed. Try again.\n",
                       "Bozuk vault_pending.dat silindi. Tekrar dene.\n");
        return;
    }
    std::cout << t("\nA previous code change was interrupted. Try the OLD code on the phone first.\n",
                   "\nOnceki sifre degisikligi yarida kalmis. Telefonda once ESKI sifreyi dene.\n");
    console::waitEnter(t("Press Enter...", "Enter'a bas..."));
    console::showDigitByDigit(t("OLD CODE (try)", "ESKI SIFRE (dene)"), data_.code);
    if (console::askYesNo(t("Did the old code work?", "Eski sifre calisti mi?"))) {
        pending_.remove();
        std::cout << t("OK, the old code is valid. Starting a clean rotation.\n",
                       "Tamam, eski sifre gecerli. Temiz bir yenileme baslatiliyor.\n");
        startRotation();
    } else {
        console::showDigitByDigit(t("NEW CODE (try)", "YENI SIFRE (dene)"), next.code);
        confirmAndCommit(next);
    }
}

void App::confirmAndCommit(const VaultData& next) {
    if (console::askYesNo(t("Did the phone accept the new code?", "Telefon yeni sifreyi kabul etti mi?"))) {
        if (!vault_.save(next)) {
            std::cout << t("ERROR: could not write vault.dat! Do NOT delete vault_pending.dat.\n",
                           "HATA: vault.dat yazilamadi! vault_pending.dat'i SILME.\n");
            return;
        }
        pending_.remove();
        data_ = next;
        loaded_ = true;
        std::cout << t("Done. Back up vault.dat (e.g. to a USB stick).\n",
                       "Tamam. vault.dat'i yedekle (ornegin USB bellege).\n");
    } else {
        pending_.remove();
        std::cout << t("Cancelled. The phone still uses its previous code.\n",
                       "Iptal edildi. Telefonda onceki sifre gecerli.\n");
    }
}
