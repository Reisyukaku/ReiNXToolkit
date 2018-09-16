#pragma once
#include <iostream>
#include <string>
#include <cstring>
#include <vector>

using namespace std;

namespace FS {
    vector<string> EnumDir(string);
    bool DirExists(string dirstr);
    unsigned DeleteDirRecursive(string path);
    unsigned MakeDir(string file, unsigned perm);
};