#include "vault.h"

#include <array>     // std::array
#include <cstdio>    // std::remove
#include <fstream>   // std::ifstream, std::ofstream
#include <iterator>  // std::istreambuf_iterator
#include <utility>   // std::move

// File layout (all integers little-endian):
//
//   offset  size  content
//   0       4     magic "SAF1"
//   4       2     N = payload length
//   6       N     obfuscated payload
//   6+N     4     FNV-1a checksum of the plain payload
//
// Plain payload:
//   [1 byte language][1 byte code length][code][1 byte phrase length][phrase]

namespace {

const char kMagic[4] = {'S', 'A', 'F', '1'};

void putU16(std::string& s, std::uint16_t v) {
    s += static_cast<char>(v & 0xFF);
    s += static_cast<char>((v >> 8) & 0xFF);
}
void putU32(std::string& s, std::uint32_t v) {
    for (int i = 0; i < 4; ++i) s += static_cast<char>((v >> (8 * i)) & 0xFF);
}
std::uint32_t getU(const std::string& s, std::size_t pos, int bytes) {
    std::uint32_t v = 0;
    for (int i = 0; i < bytes; ++i)
        v |= static_cast<std::uint32_t>(static_cast<unsigned char>(s[pos + i])) << (8 * i);
    return v;
}

bool readAll(const std::string& path, std::string& out) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return false;
    out.assign(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
    return true;
}

bool isFourDigits(const std::string& code) {
    if (code.size() != 4) return false;
    for (char c : code)
        if (c < '0' || c > '9') return false;
    return true;
}

}  // namespace

Vault::Vault(std::string path) : path_(std::move(path)) {}

bool Vault::exists() const {
    std::ifstream f(path_, std::ios::binary);
    return f.good();
}

bool Vault::save(const VaultData& data) const {
    if (!isFourDigits(data.code) || data.phrase.size() > 255) return false;

    std::string plain;
    plain += static_cast<char>(data.lang);
    plain += static_cast<char>(data.code.size());
    plain += data.code;
    plain += static_cast<char>(data.phrase.size());
    plain += data.phrase;

    std::string file(kMagic, 4);
    putU16(file, static_cast<std::uint16_t>(plain.size()));
    file += obfuscate(plain);
    putU32(file, checksum(plain));

    {
        std::ofstream f(path_, std::ios::binary | std::ios::trunc);
        if (!f) return false;
        f.write(file.data(), static_cast<std::streamsize>(file.size()));
        if (!f) return false;
    }  // closes the file (RAII) before reading it back

    VaultData check;
    return load(check) && check.code == data.code && check.phrase == data.phrase &&
           check.lang == data.lang;
}

bool Vault::load(VaultData& out) const {
    std::string file;
    if (!readAll(path_, file)) return false;
    if (file.size() < 10 || file.compare(0, 4, kMagic, 4) != 0) return false;

    const std::size_t n = getU(file, 4, 2);
    if (file.size() != 4 + 2 + n + 4) return false;

    const std::string plain = obfuscate(file.substr(6, n));
    if (checksum(plain) != getU(file, 6 + n, 4)) return false;

    // Parse with bounds checks at every step.
    std::size_t p = 0;
    if (p + 2 > plain.size()) return false;
    const auto lang = static_cast<unsigned char>(plain[p++]);
    const std::size_t codeLen = static_cast<unsigned char>(plain[p++]);
    if (p + codeLen + 1 > plain.size()) return false;
    std::string code = plain.substr(p, codeLen);
    p += codeLen;
    const std::size_t phraseLen = static_cast<unsigned char>(plain[p++]);
    if (p + phraseLen != plain.size()) return false;

    if (!isFourDigits(code) || lang > 1) return false;
    out.code = std::move(code);
    out.phrase = plain.substr(p, phraseLen);
    out.lang = static_cast<i18n::Lang>(lang);
    return true;
}

void Vault::remove() const { std::remove(path_.c_str()); }

bool Vault::loadLegacy(const std::string& path, std::string& code) {
    std::string file;
    if (!readAll(path, file) || file.size() != 8) return false;
    code = obfuscate(file.substr(0, 4));
    return isFourDigits(code) && checksum(code) == getU(file, 4, 4);
}

std::string Vault::obfuscate(const std::string& data) {
    // Same key and scheme as the original version, so old files stay readable.
    static const std::array<std::uint8_t, 8> kKey = {0x5A, 0xC3, 0x17, 0x9E,
                                                     0x44, 0xB1, 0x2D, 0xF8};
    std::string s = data;
    for (std::size_t i = 0; i < s.size(); ++i)
        s[i] = static_cast<char>(s[i] ^ kKey[i % kKey.size()] ^ static_cast<std::uint8_t>(i * 31));
    return s;
}

std::uint32_t Vault::checksum(const std::string& data) {
    std::uint32_t h = 2166136261u;
    for (unsigned char c : data) {
        h ^= c;
        h *= 16777619u;
    }
    return h;
}
