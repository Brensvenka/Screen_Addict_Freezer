#include "i18n.h"

namespace i18n {
namespace {
Lang g_lang = Lang::English;  // file-local: only reachable through set()/current()
}

void set(Lang lang) { g_lang = lang; }
Lang current() { return g_lang; }
const char* t(const char* en, const char* tr) { return g_lang == Lang::Turkish ? tr : en; }

}  // namespace i18n
