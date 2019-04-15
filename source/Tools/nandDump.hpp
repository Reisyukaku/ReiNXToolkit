/*
* ReiNX Toolkit
* Copyright (C) 2018  Team ReiSwitched
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
#include "../UI/UI.hpp"

#define MAX_SIZE 0xBA600000
#define NAND_BLOCK_SIZE 0x470000
#define CAL0_BLOCK_SIZE 0x3FBC00
#define BOOT_BLOCK_SIZE 0x400000
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
    ProdInfo = 27,
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