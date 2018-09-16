#include "Services/bpc.h"
#include "Power.hpp"

void Power::Reboot() {
    bpcInitialize();
    bpcRebootSystem();
    bpcExit();
}

void Power::Shutdown() {
	bpcInitialize();
    bpcShutdownSystem();
    bpcExit();
}
