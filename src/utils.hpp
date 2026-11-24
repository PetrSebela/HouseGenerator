#pragma once
#include <string>
#include <vector>


namespace Utils {
    template <typename T> void LoopedErase(std::vector<T> &vector, int start, int count){
        if (count >= vector.size()) {
            vector.clear();
            return;
        }

        if (start + count > vector.size()) {
            int overflow = start + count - vector.size();
            vector.erase(vector.begin() + start, vector.end());
            vector.erase(vector.begin(), vector.begin() + overflow);
        }
        else {
            vector.erase(vector.begin() + start, vector.begin() + start + count);
        }
    }

    int Mod(int a, int b);

    std::vector<std::string> Split(std::string string, std::string delimiter);
}
