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
#include <switch.h>
#include <string>
#include <vector>
#include <functional>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h> 
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "MenuObj.hpp"
#include "Render.hpp"
#include "Themes.hpp"

using namespace std;

class MenuOption : public MenuObj
{
    public:
        MenuOption(string Name, string Desc, function<void()> Call);
        string getDesc();
        void callFunc();
        vector<MenuOption> subMenu;

    protected:
        string desc;
        function<void()> callback;
};