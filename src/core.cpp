#include "core.h"

namespace core {

const char* const kVaultFile = "vault.dat";
const char* const kPendingFile = "vault_pending.dat";
const char* const kLegacyFile = "kasa.dat";
const char* const kLegacyPhrase = "evet ben iradesizim ve sifreye muhtacim";

bool migrateLegacy(const Vault& vault) {
    if (vault.exists()) return false;
    std::string code;
    if (!Vault::loadLegacy(kLegacyFile, code)) return false;
    const VaultData migrated{code, kLegacyPhrase, i18n::Lang::Turkish};
    return vault.save(migrated);
}

}  // namespace core
