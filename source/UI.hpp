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
#include <iostream>
#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h> 
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <switch.h>
using namespace std;

#include "UI/Render.hpp"
#include "UI/Themes.hpp"
#include "UI/types.hpp"
#include "UI/MenuOption.hpp"
#include "Tools/nandDump.hpp"

class UI
{
    public:
        static UI * mInstance;
        Render mRender;
        Themes *mThemes;
        bool inSubMenu;
        static UI * getInstance();
        static void setInstance(UI ui);
        UI(string title, string footer);
        void renderMenu();
        void MenuUp();
        void MenuDown();
        void MenuSel();
        void MenuBack();
        void SubMenuUp();
        void SubMenuDown();
        bool MessageBox(string header, string message, MessageType type);
        void CreateProgressBar(ProgBar *prog, string header);
        void IncrementProgressBar(ProgBar *prog);
        void drawScaled(SDL_Surface *surf, SDL_Texture *tex, int x, int y, u32 w, u32 h);
    private:
        //MainMenu
        void optAutoRCM();
        void optReiUpdate();
        void optDumpCal0();
        void optDumpBoots();
        void optDumpNand();
        void optToggleKip(string path);
        void drawKipman();
        
        //SubMenus
        void optImage(u32 ind);
        void optShutdown();
        void optReboot();
        void optAbout();
        void optUpdateHB();
        
        void CreatePopupBox(u32 x, u32 y, u32 w, u32 h, string header);
        void drawText(int x, int y, SDL_Color scolor, string text, TTF_Font *font);
        void drawRect(int x, int y, int w, int h, SDL_Color scolor);
        void drawRect(int x, int y, int w, int h, SDL_Color scolor, unsigned border, SDL_Color bcolor);
        void drawBackXY(SDL_Surface *surf, SDL_Texture *tex, int x, int y);
        void drawBack(SDL_Surface *surf, SDL_Texture *tex);
        
        void setTheme(Theme Theme);
        void deinit();
        void exitApp();
};