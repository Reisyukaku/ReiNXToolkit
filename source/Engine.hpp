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

#pragma once
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <iostream>
#include <string>
#include <vector>
#include <switch.h>
#include "Net/Request.hpp"
#include "Tools/autorcm.hpp"
#include "Tools/kipmanager.hpp"
#include "Tools/reinxconfig.hpp"
#include "Utils/unzip_utils.hpp"
#include "FS.hpp"
#include "Power.hpp"
#include "UI/types.hpp"
#include "UI/UIElement.hpp"
#include "UI/Button.hpp"
#include "UI/MessageBox.hpp"
#include "Core/Graphics.hpp"
#include "Core/Audio.hpp"
#include "Core/Hid.hpp"
#include "UI/Themes.hpp"

class Engine
{
    public:
        bool inSubMenu;
        Engine(string title, string footer);
        ~Engine();
        void Render();
        void Clear();
        void Update();
        
        void renderMenu();
        void MenuUp();
        void MenuDown();
        void MenuSel();
        void MenuBack();
        void SubMenuUp();
        void SubMenuDown();
        
        bool Running;
    private:
        //MainMenu
        void optAutoRCM();
        void optReiUpdate();
        void optDumpCal0();
        void optDumpBoots();
        void optToggleKip(string path);
        void drawKipman();
        void drawCfwman();
        
        //SubMenus
        void optImage(u32 ind);
        void optCfwCfg(string file);
        void optShutdown();
        void optReboot();
        void optAbout();
        void optUpdateHB();
        
        Mix_Music *menuSel;
        Mix_Music *menuConfirm;
        Mix_Music *menuBack;
        
        UI::MessageBox *msgBox;
        
        SDL_Texture *bgImage;
        u32 currIndex;

        std::vector<UIElement*> mainMenu;
        Audio *audio;
};
