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