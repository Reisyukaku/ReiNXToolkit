/*
* ReiNX Toolkit
* Copyright (C) 2018-2020  Reisyukaku
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

#include <switch.h>
#include "Engine.hpp"

#ifdef __SWITCH__
extern "C"{    
    void userAppInit(void);
    void userAppExit(void);
}

void userAppInit(void) {
    plInitialize(PlServiceType_User);
    romfsInit();
    fsdevMountSdmc();
    socketInitializeDefault();
}

void userAppExit(void) {
    plExit();
    romfsExit();
    fsdevUnmountAll();
    socketExit();
}
#endif

int main(int argc, char **argv) {
    nxlinkStdio();
    printf("Initializing engine...\n");
	Engine eng(TITLE, VERSION);
    while(appletMainLoop() && eng.Running) {
        eng.Clear();
        eng.Update();
        eng.Render();
    }
}