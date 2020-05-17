/*
* ReiNX Toolkit
* Copyright (C) 2018  Reisyukaku
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
#include <switch.h>
#include <stdio.h>
#include <stdlib.h>

#define BCT_KEY_OFF 0x210
#define BCT_SZ 0x4000
#define NUM_KBS 32
#define KB_SZ 0xB0

#define RCM_XOR 0x77

namespace Tools {
	int toggle_rcm();
}