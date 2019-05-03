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
#include "Themes.hpp"
#include "Render.hpp"

Themes * Themes::mInstance = 0;

Theme Themes::HorizonLight() {
	Theme th;
	th.FontPath = "romfs:/Fonts/NintendoStandard.ttf";
	th.BackgroundPath= "romfs:/Graphics/ForReiNXLight.png";
	th.TextColor  = BLACK;
	th.SelectedTextColor = BLUE;
    th.ButtonColor = WHITE;
    th.ButtonBorderColor = TEAL;
    th.BGColor = GREY;
    th.PopCol1 = 0xD0D0D0FF;
    th.PopCol2 = 0xF0F0F0FF;
    return th;
}

Theme Themes::HorizonDark() {
	Theme th;
	th.FontPath = "romfs:/Fonts/NintendoStandard.ttf";
	th.BackgroundPath= "romfs:/Graphics/ForReiNXDark.png";
	th.TextColor  = WHITE;
	th.SelectedTextColor = TEAL;
    th.ButtonColor = GREY;
    th.ButtonBorderColor = LIGHTBLUE;
    th.BGColor = GREY;
    th.PopCol1 = 0x202020FF;
    th.PopCol2 = 0x303030FF;
    return th;
}

SDL_Color convertColor(unsigned col) {
	SDL_Color c;
	c.r = ((col >> 24) & 0xFF);
	c.g = ((col >> 16) & 0xFF);
	c.b = ((col >> 8) & 0xFF);
	c.a = (col & 0xFF);
	return c;
}

void Themes::Init(Render rend) {
    //Get current theme set
    ColorSetId id;
    setsysInitialize();
    setsysGetColorSetId(&id);
    setsysExit();
    
    Theme theme = id ? HorizonDark() : HorizonLight();
    ttf = theme.FontPath;
    fntLarge = TTF_OpenFont(theme.FontPath.c_str(), 25);
    fntMedium = TTF_OpenFont(theme.FontPath.c_str(), 20);
    fntSmall = TTF_OpenFont(theme.FontPath.c_str(), 14);
    bgs = IMG_Load(theme.BackgroundPath.c_str());
    if (bgs){
        Uint32 colorkey = SDL_MapRGB(bgs->format, 0, 0, 0);
        SDL_SetColorKey(bgs, SDL_TRUE, colorkey);
    }
    bgt = SDL_CreateTextureFromSurface(rend._renderer, bgs);
    txtcolor = convertColor(theme.TextColor);
    selcolor = convertColor(theme.SelectedTextColor);
    butCol = convertColor(theme.ButtonColor);
    butBorderCol = convertColor(theme.ButtonBorderColor);
    bgCol = convertColor(theme.BGColor);
    popCol1 = convertColor(theme.PopCol1);
    popCol2 = convertColor(theme.PopCol2);
}

Themes * Themes::instance() {
	if(!mInstance)
		mInstance = new Themes;
	return mInstance;
}