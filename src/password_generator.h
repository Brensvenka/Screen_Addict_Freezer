// password_generator.h - Random 4-digit Screen Time passcodes.
#pragma once
#include <random>
#include <string>

class PasswordGenerator {
public:
    PasswordGenerator();

    // Returns a random 4-digit code that is not weak and differs from `avoid`.
    std::string generate(const std::string& avoid = "");

    // Weak = any digit repeats (1171, 3393, 5505) or the digits run
    // ascending (1234) / descending (4321). Repeated digits form a rhythm
    // that sticks in memory, which defeats showing the code digit by digit.
    static bool isWeak(const std::string& code);

private:
    std::mt19937 engine_;  // seeded once from std::random_device
};
