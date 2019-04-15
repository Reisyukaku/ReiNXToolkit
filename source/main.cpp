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

#include <switch.h>
#include "Form.hpp"

int main() {    
	Form frm(TITLE, VERSION);
    frm.Init();
    frm.Update();
    
    while(appletMainLoop()) {        
        hidScanInput();
        u64 PressedInput = hidKeysDown(CONTROLLER_P1_AUTO);
        if((PressedInput & KEY_LSTICK_UP)||(PressedInput & KEY_DUP)) {
            frm.inSubMenu ? frm.SubMenuUp() : frm.MenuUp();
            frm.Update();
        }
        else if((PressedInput & KEY_LSTICK_DOWN)||(PressedInput & KEY_DDOWN)) {
            frm.inSubMenu ? frm.SubMenuDown() : frm.MenuDown();
            frm.Update();
        }
        if(PressedInput & KEY_A) {
            frm.MenuSel();
            frm.Update();
        }
        if(PressedInput & KEY_B) {
            frm.MenuBack();
            frm.Update();
        }
    }
}