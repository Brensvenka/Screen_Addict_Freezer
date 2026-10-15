// console.h - Small helpers around the text console.
#pragma once
#include <string>

namespace console {

// Clears the console window.
void clear();

// Reads one full line (spaces included). Exits the program on end of input
// (e.g. the window was closed), so callers never loop forever on a dead stream.
std::string readLine();

// Prints `message` and waits for Enter.
void waitEnter(const std::string& message);

// Asks a yes/no question until a valid answer is given.
// Accepts Y/N (English) and E/H (Turkish) in any case, regardless of language.
bool askYesNo(const std::string& question);

// Shows `code` one digit per screen. The full code is never visible at once,
// which makes it much harder to memorize by accident.
void showDigitByDigit(const std::string& title, const std::string& code);

}  // namespace console
