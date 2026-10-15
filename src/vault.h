// vault.h - Stores the Screen Time code, the confirmation phrase and the
// language on disk, in an obfuscated file.
//
// IMPORTANT: obfuscation is NOT encryption. The key is in this source code.
// The goal is only that the code cannot be read by opening the file in a text
// editor. This tool is a barrier against yourself, not against an attacker.
#pragma once
#include <cstdint>
#include <string>

#include "i18n.h"

struct VaultData {
    std::string code;     // 4 digits
    std::string phrase;   // normalized confirmation phrase
    i18n::Lang lang = i18n::Lang::English;
};

class Vault {
public:
    explicit Vault(std::string path);

    bool exists() const;

    // Reads and verifies the file. Returns false if missing or corrupted.
    bool load(VaultData& out) const;

    // Writes the file, then reads it back to verify. Returns false on any error.
    // Never show a code to the user before this returned true: a code that is on
    // the phone but not safely on disk would lock you out.
    bool save(const VaultData& data) const;

    void remove() const;
    const std::string& path() const { return path_; }

    // Reads the 8-byte file of the original single-file version (kasa.dat),
    // which held only the code. Used once to migrate old installations.
    static bool loadLegacy(const std::string& path, std::string& code);

private:
    std::string path_;

    static std::string obfuscate(const std::string& data);  // XOR; applying twice restores
    static std::uint32_t checksum(const std::string& data);  // FNV-1a 32-bit
};
