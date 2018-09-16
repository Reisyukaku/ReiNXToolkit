#include <stdio.h>
#include <algorithm>
#include <switch.h>
#include "../FS.hpp"

namespace kip {
    std::string Name(std::string path);
    void LoadKips(std::vector<std::string> *kips);
    int Toggle(std::string path);
}