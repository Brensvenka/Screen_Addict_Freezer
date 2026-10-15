// app.h - The menu and all user flows. Composes Vault, PasswordGenerator,
// the phrase check and the console helpers.
#pragma once
#include <string>

#include "password_generator.h"
#include "vault.h"

class App {
public:
    App();
    int run();

private:
    Vault vault_;    // the current code: vault.dat
    Vault pending_;  // a new code waiting for confirmation: vault_pending.dat
    PasswordGenerator generator_;
    VaultData data_;
    bool loaded_ = false;

    // Startup
    void migrateLegacy();
    void chooseLanguage();
    void resolveUnfinishedSetup();

    // Menu actions
    void unlock();
    void firstSetup();
    void rotate();
    void changePhrase();
    void changeLanguage();

    // Building blocks
    bool requireVault();
    bool askPhrase(const std::string& stored);
    std::string chooseNewPhrase();
    void startRotation();
    void resolveInterruptedRotation();
    void confirmAndCommit(const VaultData& next);
};
