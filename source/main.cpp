#include <switch.h>
#include "UI.hpp"


int main() {    
	UI ui(TITLE, VERSION);
	UI::setInstance(ui);
    ui.renderMenu();
    
    while(appletMainLoop()) {        
        hidScanInput();
        u64 PressedInput = hidKeysDown(CONTROLLER_P1_AUTO);
        if((PressedInput & KEY_LSTICK_UP)||(PressedInput & KEY_DUP)) {
            ui.inSubMenu ? ui.SubMenuUp() : ui.MenuUp();
            ui.renderMenu();
        }
        else if((PressedInput & KEY_LSTICK_DOWN)||(PressedInput & KEY_DDOWN)) {
            ui.inSubMenu ? ui.SubMenuDown() : ui.MenuDown();
            ui.renderMenu();
        }
        if(PressedInput & KEY_A) {
            ui.MenuSel();
            ui.renderMenu();
        }
        if(PressedInput & KEY_B) {
            ui.MenuBack();
            ui.renderMenu();
        }
    }
}