#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>
#include <time.h> 
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <assert.h>

#include <switch.h>
#include "../UI.hpp"

#define MAX_SIZE 0xBA600000
#define BLOCK_SIZE 0x100000

using namespace std;

enum partitions {
    boot0 = 0,
    boot1 = 10,
    rawnand = 20,
    BCPKG21,
    BCPKG22,
    BCPKG23,
    BCPKG24,
    BCPKG25,
    BCPKG26,
    ProdInfo,
    ProdInfoF,
    SAFE,
    USER,
    SYSTEM1,
    SYSTEM2
};

namespace Tools {

    int create_combine_batch();
    u64 CheckFreeSpace();
    void calculate_time(u64 begin, u64 end);
    int DumpPartition(int part_number, string name);
}