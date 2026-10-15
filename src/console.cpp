#include "console.h"

#include <cstdlib>   // std::system, std::exit
#include <iostream>  // std::cout, std::cin, std::getline

#include "i18n.h"

namespace console {

void clear() {
#ifdef _WIN32
    std::system("cls");
#else
    std::cout << "\033[2J\033[H" << std::flush;  // ANSI: clear screen, cursor home
#endif
}

std::string readLine() {
    std::string line;
    if (!std::getline(std::cin, line)) std::exit(0);
    if (!line.empty() && line.back() == '\r') line.pop_back();  // tolerate CRLF input
    return line;
}

void waitEnter(const std::string& message) {
    std::cout << message;
    readLine();
}

bool askYesNo(const std::string& question) {
    while (true) {
        std::cout << question << i18n::t(" (Y/N): ", " (E/H): ");
        const std::string a = readLine();
        if (a == "Y" || a == "y" || a == "E" || a == "e") return true;
        if (a == "N" || a == "n" || a == "H" || a == "h") return false;
    }
}

void showDigitByDigit(const std::string& title, const std::string& code) {
    for (std::size_t i = 0; i < code.size(); ++i) {
        clear();
        std::cout << title << "\n\n"
                  << "   " << i18n::t("Digit ", "") << (i + 1) << i18n::t(":", ". rakam:")
                  << "   [ " << code[i] << " ]\n\n";
        waitEnter(i18n::t("Type it on the phone, then press Enter...",
                          "Telefona yaz, sonra Enter'a bas..."));
    }
    clear();
}

}  // namespace console
