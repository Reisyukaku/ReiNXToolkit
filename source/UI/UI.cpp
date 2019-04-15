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
#include "UI.hpp"

Themes *UI::mThemes;

/* ---------------------------------------------------------------------------------------
* UI class functions
*/
void UI::Init(Themes *theme) {
    mThemes = theme;
}

/*
* UI Pop-up stuff
*/
void UI::CreatePopupBox(u32 x, u32 y, u32 w, u32 h, string header) {
    Graphics::Rectangle(x-5, y-5, w+10, h+70, 0xFF); //BG box
    Graphics::Rectangle(x, y, w, 57, mThemes->popCol1); //Header
    Graphics::Rectangle(x, y+60, w, h, mThemes->popCol2); //Message
    Graphics::DrawText(mThemes->fntMedium, x+15, y+15, header, mThemes->txtcolor);
}

void UI::CreateProgressBar(ProgBar *prog, string header) {
    u32 poph = 100, popw = 300;
    u32 barh = 40, barw = 200;
    u32 startx = (WIN_WIDTH/2)-(popw/2), starty = (WIN_HEIGHT/2)-(poph/2);
    
    if(prog->max <= 0) prog->max = 100;
    if(prog->step <= 0) prog->step = 1;
    prog->curr = 0;
    prog->posx = startx;
    prog->posy = starty;
    CreatePopupBox(prog->posx, prog->posy, popw, poph, header);
    Graphics::Rectangle(prog->posx+(popw/2)-(barw/2), prog->posy+60+((poph-60)/2), barw, barh, 0xFF); //empty bar
    Graphics::DrawText(mThemes->fntSmall, prog->posx+(popw/2), prog->posy+(poph/2)-(barh/2)+60, to_string((unsigned)((prog->curr/prog->max)*100)) + "%", 0xFFFFFFFF);
    Graphics::Render();
}

void UI::IncrementProgressBar(ProgBar *prog) {
    u32 poph = 100+2, popw = 300+2;
    u32 barh = 40-4, barw = 200-4;
    Graphics::Rectangle(prog->posx+(popw/2)-(barw/2), prog->posy+60+((poph-60)/2), barw, barh, 0xFF); //empty bar
    Graphics::Rectangle(prog->posx+(popw/2)-(barw/2), prog->posy+60+((poph-60)/2), (unsigned)(barw*(prog->curr/prog->max)), barh, 0xFF00FF);
    Graphics::DrawText(mThemes->fntSmall, prog->posx+(popw/2), prog->posy+(poph/2)-(barh/2)+60, to_string((unsigned)((prog->curr/prog->max)*100)) + "%", 0xFFFFFFFF);
    prog->curr += prog->step;
    Graphics::Render();
}

bool UI::MessageBox(string header, string message, MessageType type) {
    bool ret = false;
    SDL_Rect rect;
    u32 poph = 300, popw = 450;
    u32 buth = 50, butw = 100;
    u32 startx = (WIN_WIDTH/2)-(popw/2), starty = (WIN_HEIGHT/2)-(poph/2);
    
    CreatePopupBox(startx, starty, popw, poph, header);
    Graphics::DrawText(mThemes->fntMedium, startx+15, starty+75, message, mThemes->txtcolor);
    
    switch(type){
        case TYPE_YES_NO:
            Graphics::Rectangle(startx+popw-butw-10, starty+poph, butw, buth, 0xFF);//YES
            Graphics::DrawText(mThemes->fntMedium, startx+popw-butw+12, starty+poph+12, "\ue0e0 Yes", mThemes->txtcolor);
            Graphics::Rectangle(startx+popw-(2*(butw+5))-10, starty+poph, butw, buth, 0xFF);//NO
            Graphics::DrawText(mThemes->fntMedium, startx+popw-(2*(butw+5))+12, starty+poph+12, "\ue0e1 No", mThemes->txtcolor);
            break;
        default:
        case TYPE_OK:
            Graphics::Rectangle(startx+popw-butw-10, starty+poph, butw, buth, mThemes->popCol1);
            Graphics::DrawText(mThemes->fntMedium, startx+popw-butw+12, starty+poph+12, "\ue0e0 OK", mThemes->txtcolor);
            break;
    }
    Graphics::Render();
    while(1){
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        if(kDown & KEY_A) {
            ret = true;
            Mix_PlayMusic(menuConfirm, 1);
            break;
        }
        else if(kDown & KEY_B) {
            ret = (type == TYPE_OK);
            Mix_PlayMusic(menuBack, 1);
            break;
        }
    }
    return ret;
}