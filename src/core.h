// core.h - Things both front ends (console and window) share:
// file names and the one-time migration from the very first version.
#pragma once

#include "vault.h"

namespace core {

extern const char* const kVaultFile;    // "vault.dat"
extern const char* const kPendingFile;  // "vault_pending.dat"
extern const char* const kLegacyFile;   // "kasa.dat" from the single-file version
extern const char* const kLegacyPhrase; // phrase the single-file version used

// If there is no vault yet but an old kasa.dat exists, copy its code into a
// new vault with the old phrase and Turkish as language. Returns true only
// when a migration actually happened.
bool migrateLegacy(const Vault& vault);

}  // namespace core
