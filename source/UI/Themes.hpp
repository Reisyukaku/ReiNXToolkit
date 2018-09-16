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
#include "Render.hpp"

class Themes
{    
	public:
		static Themes * mInstance;
        string ttf;
        TTF_Font *fntMedium;
        TTF_Font *fntLarge;
        TTF_Font *fntSmall;
        SDL_Color txtcolor;
        SDL_Color selcolor;
        SDL_Color butCol;
        SDL_Color butBorderCol;
        SDL_Color bgCol;
        SDL_Color popCol1;
        SDL_Color popCol2;
        SDL_Surface *bgs;
        SDL_Texture *bgt;
		
		static Themes *instance();
        void Init(Render rend);
        Theme HorizonLight();
        Theme HorizonDark();
};