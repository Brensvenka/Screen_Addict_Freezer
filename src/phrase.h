// phrase.h - The confirmation phrase the user must type before seeing the code.
//
// The phrase is not a secret. It is a speed bump: typing a deliberate sentence
// in a weak moment makes you notice what you are about to do.
#pragma once
#include <string>

namespace phrase {

// Minimum length of a new phrase (after normalization).
constexpr std::size_t kMinLength = 10;
constexpr std::size_t kMaxLength = 200;

// Makes text comparable regardless of how it was typed:
//   - upper case -> lower case
//   - Turkish letters -> ASCII (i/I/dotless i/dotted I -> i, s-cedilla -> s, ...),
//     both in UTF-8 and (on Windows) in the console's code page 857
//   - runs of whitespace -> one space, leading/trailing spaces removed
std::string normalize(const std::string& input);

// A new phrase is valid if, after normalization, it has kMinLength..kMaxLength
// characters and only printable ASCII. Expects an already-normalized string.
bool isValid(const std::string& normalized);

// True if `typed` (raw user input) matches the stored (normalized) phrase.
bool matches(const std::string& typed, const std::string& stored);

}  // namespace phrase
