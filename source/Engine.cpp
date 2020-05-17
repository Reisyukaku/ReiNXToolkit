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

#include "Engine.hpp"

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720

UI::MessageBox* UI::MessageBox::instance = nullptr;
Themes* Themes::instance = nullptr;

Engine::Engine(std::string Title, std::string Version) {
    printf("[Engine]: Initializing engine\n");
    Running = true;

    Rect win = {0, 0, WIN_WIDTH, WIN_HEIGHT};
    Graphics::Init(win);
    
    Graphics::SetSelectCol(0x00FFCEFF);
    Graphics::SetMessageCol(0xFFFFFFFF);
    Graphics::SetButtonCol(0x4C4C4CFF);
    Graphics::SetHeaderCol(0xFFFFFFFF);
    
    bgImage = Graphics::CreateTexFromString("romfs:/Graphics/defaultDark.png");
    
    msgBox = UI::MessageBox::getInstance();
    msgBox->SetTexture(Graphics::CreateTexFromString("romfs:/Graphics/Forms/PopupMessage.png"));
    
    audio = new Audio(64);
    inSubMenu = false;
    currIndex = 0;
    
    menuSel = audio->Load("romfs:/Sounds/menu_select.mp3");
    menuConfirm = audio->Load("romfs:/Sounds/menu_confirm.mp3");
    menuBack = audio->Load("romfs:/Sounds/menu_back.mp3");
    
    //Main pages
    mainMenu.push_back(new Button("ReiNX Updates", nullptr));
    mainMenu.push_back(new Button("Change splash", nullptr));
    mainMenu.push_back(new Button("KIP Manager", nullptr));
    mainMenu.push_back(new Button("CFW Manager", nullptr));
    mainMenu.push_back(new Button("Toggle AutoRCM", nullptr));
    mainMenu.push_back(new Button("Backup tool", nullptr));
    mainMenu.push_back(new Button("Power", nullptr));
    mainMenu.push_back(new Button("Help", [&]()->Result{ msgBox->Show("Help", "Made by Reisyukaku"); return 0; }));
    
    //Subpages
    /*mainMenu[0].subMenu.push_back(MenuOption("Update ReiNX", "", nullptr));
    mainMenu[0].subMenu.push_back(MenuOption("Update Toolkit", "", nullptr));

    vector<string> paths = FS::EnumDir("/Toolkit/splashes");
    for(unsigned int i=0;i<paths.size();i++) {
        mainMenu[1].subMenu.push_back(MenuOption(paths[i], "", nullptr));
        images.push_back(IMG_Load(("/Toolkit/splashes/"+paths[i]).c_str()));
    }

    UI::drawKipman();
    UI::drawCfwman();

    mainMenu[5].subMenu.push_back(MenuOption("Backup Cal0", "", nullptr));
    mainMenu[5].subMenu.push_back(MenuOption("Backup Boot0/1", "", nullptr));
    mainMenu[5].subMenu.push_back(MenuOption("Backup NAND", "", nullptr));
    
    mainMenu[6].subMenu.push_back(MenuOption("Reboot", "", nullptr));
    mainMenu[6].subMenu.push_back(MenuOption("Shutdown", "", nullptr));
    
    mainMenu[7].subMenu.push_back(MenuOption("About", "", nullptr));*/
    
    //Make dirs
    if(!FS::DirExists("/Toolkit"))  FS::MakeDir("/Toolkit", 0777);
    if(!FS::DirExists("/Toolkit/splashes"))  FS::MakeDir("/Toolkit/splashes", 0777);
}

Engine::~Engine() {
    printf("[Engine]: Exiting\n");
    delete audio;
    Graphics::DestroyTex(bgImage);
    for(auto &e : mainMenu) delete e;
    mainMenu.clear();
    Graphics::Exit();
    appletEndBlockingHomeButton();
}

void Engine::Update() {
    //HID
    Hid::KeyProcess();
    if((Hid::Input & KEY_LSTICK_UP)||(Hid::Input & KEY_DUP)) {
        if(currIndex > 0) currIndex--;
    }
    else if((Hid::Input & KEY_LSTICK_DOWN)||(Hid::Input & KEY_DDOWN)) {
        if(currIndex < mainMenu.size()-1) currIndex++;
    }
    if(Hid::Input & KEY_A) {
        if(mainMenu[currIndex]->HasFunc()) mainMenu[currIndex]->Run();
    }
    if(Hid::Input & KEY_B) {
        Running = false;
    }
    
    //Draw BG
    Graphics::RenderTexture(bgImage, {0, 0, Graphics::GetWinWidth(), Graphics::GetWinHeight()});
    
    //Draw Buttons
    int space = 50;
    for(int i = 0; i < mainMenu.size(); i++) {
        mainMenu[i]->Draw((i == currIndex) ? ELEM_Selected : ELEM_Unselected, {25, space+=(MENU_BUTTON_HEIGHT + 15), MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT});
    }
}

void Engine::Render() {
    Graphics::Render();
}

void Engine::Clear() {
    Graphics::Clear();
}
