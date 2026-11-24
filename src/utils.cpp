#include "utils.hpp"

int Utils::Mod(int a, int b) {
    int remainder = a % b;
    if (remainder < 0)
        remainder += b < 0 ? -b : b;
    return remainder;
}

std::vector<std::string> Utils::Split(std::string string, std::string delimiter) {
    std::vector<std::string> substrings;
    size_t pos = 0;
    while ((pos = string.find(delimiter)) != std::string::npos) {
        std::string substring = string.substr(0, pos);
        substrings.push_back(substring);
        string.erase(0, pos + delimiter.length());
    }
    substrings.push_back(string);
    return substrings;
}
