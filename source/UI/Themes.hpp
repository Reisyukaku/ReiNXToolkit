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
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "types.hpp"
#include "Graphics.hpp"

class Themes
{    
	public:
        Themes();
        ~Themes();
        string ttf;
        TTF_Font *fntMedium;
        TTF_Font *fntLarge;
        TTF_Font *fntSmall;
        u32 txtcolor;
        u32 selcolor;
        u32 butCol;
        u32 butBorderCol;
        u32 bgCol;
        u32 popCol1;
        u32 popCol2;
        SDL_Surface *bgs;
        SDL_Texture *bgt;
        
        Theme HorizonLight();
        Theme HorizonDark();
};