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
    // 1) Every digit must be different: no 1171, no 3393, no 5505.
    bool seen[10] = {};
    for (char c : code) {
        const int d = c - '0';
        if (seen[d]) return true;
        seen[d] = true;
    }

    // 2) No straight runs like 1234 or 9876.
    bool ascending = true, descending = true;
    for (std::size_t i = 1; i < code.size(); ++i) {
        if (code[i] != code[i - 1] + 1) ascending = false;
        if (code[i] != code[i - 1] - 1) descending = false;
    }
    return ascending || descending;
}
