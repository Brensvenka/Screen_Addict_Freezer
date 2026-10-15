// i18n.h - Minimal two-language support (English / Turkish).
//
// Every user-facing text is written at the call site in both languages:
//     console << i18n::t("Hello", "Merhaba");
// This keeps the two versions side by side, which is easy to read and to
// keep in sync for a small program like this one.
//
// Turkish texts intentionally avoid Turkish letters (s instead of s-cedilla,
// etc.) because the Windows console does not reliably print UTF-8 by default.
#pragma once

namespace i18n {

enum class Lang : unsigned char { English = 0, Turkish = 1 };

void set(Lang lang);
Lang current();

// Returns `en` or `tr` depending on the current language.
const char* t(const char* en, const char* tr);

}  // namespace i18n
