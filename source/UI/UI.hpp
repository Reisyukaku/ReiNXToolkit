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
#include "types.hpp"
#include "Themes.hpp"

class UI
{
    public:
        static Themes *mThemes;
        static void Init(Themes *theme);
        static bool MessageBox(string header, string message, MessageType type);
        static void CreateProgressBar(ProgBar *prog, string header);
        static void IncrementProgressBar(ProgBar *prog);        
        static void CreatePopupBox(u32 x, u32 y, u32 w, u32 h, string header);
};