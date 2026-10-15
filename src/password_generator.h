// password_generator.h - Random 4-digit Screen Time passcodes.
#pragma once
#include <random>
#include <string>

class PasswordGenerator {
public:
    PasswordGenerator();

    // Returns a random 4-digit code that is not weak and differs from `avoid`.
    std::string generate(const std::string& avoid = "");

    // Weak = all digits equal (1111), ascending (1234) or descending (4321).
    static bool isWeak(const std::string& code);

private:
    std::mt19937 engine_;  // seeded once from std::random_device
};
