#include "password_generator.h"

PasswordGenerator::PasswordGenerator() : engine_(std::random_device{}()) {}

std::string PasswordGenerator::generate(const std::string& avoid) {
    std::uniform_int_distribution<int> digit(0, 9);
    std::string code;
    do {
        code.clear();
        for (int i = 0; i < 4; ++i) code += static_cast<char>('0' + digit(engine_));
    } while (isWeak(code) || code == avoid);
    return code;
}

bool PasswordGenerator::isWeak(const std::string& code) {
    bool same = true, ascending = true, descending = true;
    for (std::size_t i = 1; i < code.size(); ++i) {
        if (code[i] != code[0]) same = false;
        if (code[i] != code[i - 1] + 1) ascending = false;
        if (code[i] != code[i - 1] - 1) descending = false;
    }
    return same || ascending || descending;
}
