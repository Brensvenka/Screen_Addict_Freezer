#include "phrase.h"

#include <cctype>   // std::isspace, std::tolower
#include <utility>  // std::pair

namespace phrase {
namespace {

// Turkish letters encoded as UTF-8 (two bytes each) and their ASCII stand-ins.
// Written as byte escapes so the result does not depend on the encoding the
// source file was saved with.
const std::pair<const char*, char> kUtf8Turkish[] = {
    {"\xC4\xB1", 'i'}, {"\xC4\xB0", 'i'},  // dotless i, dotted capital I
    {"\xC5\x9F", 's'}, {"\xC5\x9E", 's'},  // s-cedilla
    {"\xC3\xA7", 'c'}, {"\xC3\x87", 'c'},  // c-cedilla
    {"\xC4\x9F", 'g'}, {"\xC4\x9E", 'g'},  // g-breve
    {"\xC3\xBC", 'u'}, {"\xC3\x9C", 'u'},  // u-umlaut
    {"\xC3\xB6", 'o'}, {"\xC3\x96", 'o'},  // o-umlaut
};

// On Turkish Windows the console usually sends input in code page 857,
// where each Turkish letter is a single byte.
char fromCp857(unsigned char c) {
#if defined(_WIN32) || defined(SAF_TEST_CP857)
    switch (c) {
        case 0x8D: case 0x98: return 'i';  // dotless i, dotted capital I
        case 0x9F: case 0x9E: return 's';
        case 0x87: case 0x80: return 'c';
        case 0xA7: case 0xA6: return 'g';
        case 0x81: case 0x9A: return 'u';
        case 0x94: case 0x99: return 'o';
        default: break;
    }
#else
    (void)c;
#endif
    return 0;
}

std::size_t length(const char* s) {
    std::size_t n = 0;
    while (s[n] != '\0') ++n;
    return n;
}

}  // namespace

std::string normalize(const std::string& input) {
    std::string out;
    std::size_t i = 0;
    while (i < input.size()) {
        bool matched = false;
        for (const auto& entry : kUtf8Turkish) {
            const std::size_t n = length(entry.first);
            if (input.compare(i, n, entry.first) == 0) {
                out += entry.second;
                i += n;
                matched = true;
                break;
            }
        }
        if (matched) continue;

        const unsigned char c = static_cast<unsigned char>(input[i++]);
        if (const char ascii = fromCp857(c)) {
            out += ascii;
        } else if (std::isspace(c)) {
            if (!out.empty() && out.back() != ' ') out += ' ';
        } else {
            out += static_cast<char>(std::tolower(c));
        }
    }
    while (!out.empty() && out.back() == ' ') out.pop_back();
    return out;
}

bool isValid(const std::string& normalized) {
    if (normalized.size() < kMinLength || normalized.size() > kMaxLength) return false;
    for (const unsigned char c : normalized)
        if (c < 32 || c > 126) return false;  // printable ASCII only
    return true;
}

bool matches(const std::string& typed, const std::string& stored) {
    return normalize(typed) == stored;
}

}  // namespace phrase
