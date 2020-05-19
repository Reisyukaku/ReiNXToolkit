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

#include "Graphics.hpp"

SDL_Renderer *Graphics::Rend = nullptr;
SDL_Window *Graphics::Window = nullptr;
u32 Graphics::selectCol;
u32 Graphics::msgCol;
u32 Graphics::butCol;
u32 Graphics::headerCol;
Rect Graphics::Screen;
Context Graphics::ctxt;

u32 Graphics::glowInt = 0;

void Graphics::Init(Rect pos) {
    printf("[Graphics]: Initializing\n");
    //Basic SDL init
    Screen = pos;
    SDL_Init(SDL_INIT_VIDEO);
    Window = SDL_CreateWindow("ReiNXToolkit", Screen.x, Screen.y, Screen.w, Screen.h, 0);
    if(!Window) {
        printf("[Error]: Window null (%s)\n", SDL_GetError());
        return;
    }
    Rend = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
    if(!Rend) {
        printf("[Error]: Renderer null (%s)\n", SDL_GetError());
        return;
    }
    SDL_SetRenderDrawBlendMode(Rend, SDL_BLENDMODE_BLEND);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    TTF_Init();
    SDL_SetRenderDrawColor(Rend, 0xFF, 0xFF, 0xFF, 0xFF);
}

void Graphics::Exit() {
    printf("[Graphics]: Exiting\n");
    TTF_Quit();
    IMG_Quit();
    SDL_DestroyRenderer(Rend);
    SDL_DestroyWindow(Window);
    SDL_Quit();
}

void Graphics::ClearScreen() {
	Rectangle(Screen, 0xFFFFFFFF);
}

void Graphics::BorderedRectangle(Rect pos, u32 foreCol, u32 bordCol, u8 bordThick) {
    Rect p = pos;
    p.x -= bordThick;
    p.y -= bordThick;
    p.w += (bordThick*2);
    p.h += (bordThick*2);
    Rectangle(p, bordCol);
    Rectangle(pos, foreCol);
}

void Graphics::Rectangle(Rect pos, u32 scolor) {
    SDL_SetRenderDrawColor(Rend, ((scolor >> 24) & 0xFF), ((scolor >> 16) & 0xFF), ((scolor >> 8) & 0xFF), (scolor & 0xFF));
    SDL_RenderFillRect(Rend, (SDL_Rect*)&pos);
}

SDL_Texture *Graphics::CreateTexFromString(std::string file, Rect *pos) {
    printf("[Graphics]: Creating texture from image %s\n", file.c_str());
    SDL_Surface *surf = IMG_Load(file.c_str());
    if(!surf) {
        printf("[Error]: Failed to create surface from image %s\n", file.c_str());
        return nullptr;
    }
    SDL_Texture *tex = CreateTexFromSurf(surf);
    if(pos != nullptr) 
        SDL_QueryTexture(tex, NULL, NULL, &pos->w, &pos->h);
    
    return tex;
}

SDL_Texture *Graphics::CreateTexFromBuffer(void *buffer, size_t size) {
    SDL_Texture *tex = nullptr;
    SDL_Surface *img = IMG_Load_RW(SDL_RWFromMem(buffer, size), 1);
    if(img) 
        tex = Graphics::CreateTexFromSurf(img);
    
    return tex;
}

SDL_Texture *Graphics::CreateTexFromSurf(SDL_Surface *surf) {
    SDL_Texture *tex = SDL_CreateTextureFromSurface(Rend, surf);
    if(!tex) printf("[Error]: Failed to create texture from image\n");
    SDL_FreeSurface(surf);
    return tex;
}

void Graphics::RenderTexture(SDL_Texture *tex, Rect pos, Rect *clip) {
	if(!tex) return;
    if( clip != NULL ) {
        pos.w = clip->w;
        pos.h = clip->h;
    }
    SDL_RenderCopy(Rend, tex, (SDL_Rect*)clip, (SDL_Rect*)&pos);
}

void Graphics::DrawImageElem(SDL_Texture *tex, Rect pos, ElementType type) {
    if(type & ELEM_Selected){
        Rect p = pos;
        p.x -= 5; p.y -= 5;
        p.w += 10; p.h += 10;
        Rectangle(p, GetSelectCol());
    }
    RenderTexture(tex, pos);
}

void Graphics::DrawText(u8 fntsize, s32 x, s32 y, std::string str, u32 col, u32 wrap) {
    ctxt.color = col;
    ctxt.sizeIndex = fntsize;
    ctxt.renderer = Rend;
    fonts().draw(x, y, str, ctxt);
}

void Graphics::DrawButton(Rect Pos, std::string Text, ElementType butType) {
    int tHight = Fonts::FONT_SIZE_SMALL;
    if(butType & ELEM_DrawOutline) {
        Rect pos; 
        pos.x = Pos.x-5; pos.y = Pos.y-5;
        pos.w = Pos.w+10; pos.h = Pos.h+10;
        Graphics::Rectangle(pos, (butType & ELEM_Selected) ? GetSelectCol() : GetMessageCol());
        Graphics::Rectangle(Pos, GetButtonCol());
    }
    Graphics::DrawText(Fonts::FONT_SMALL, Pos.x + 12, Pos.y + (Pos.h/2) - (tHight/2), Text, (butType & ELEM_Selected) ? GetSelectCol() : GetMessageCol());
}

void Graphics::DrawOption(Rect Pos, std::string Text, std::string OptionText, ElementType type) {
    int tHight = Fonts::FONT_SIZE_SMALL;
    if(type & ELEM_Selected) {
        Rect pos; 
        pos.x = Pos.x-5; pos.y = Pos.y-5;
        pos.w = Pos.w+10; pos.h = Pos.h+10;
        Graphics::Rectangle(pos, GetSelectCol());
        Graphics::Rectangle(Pos, GetButtonCol());
        Graphics::DrawText(Fonts::FONT_SMALL, Pos.x + 10, Pos.y + (Pos.h/2) - (tHight/2), Text);
        Graphics::DrawText(Fonts::FONT_SMALL, Pos.x + ((Text != "") ? 200 : 40), Pos.y + (Pos.h/2) - (tHight/2), OptionText, GetSelectCol());
    }
    else {
        Graphics::DrawText(Fonts::FONT_SMALL, Pos.x + 10, Pos.y + (Pos.h/2) - (tHight/2), Text);
        Graphics::DrawText(Fonts::FONT_SMALL, Pos.x + ((Text != "") ? 200 : 40), Pos.y + (Pos.h/2) - (tHight/2), OptionText);
    }
}

SDL_Texture *Graphics::BlitSurfacesFromString(std::vector<std::string> layers, Rect pos) {
    SDL_Texture *tex;
    SDL_Surface *wall = SDL_CreateRGBSurface(0,pos.w,pos.h,32,0,0,0,0);
	for(auto &l: layers) {
		SDL_Surface *lay = IMG_Load(l.c_str());
		SDL_BlitSurface(lay, (SDL_Rect*)&pos, wall, NULL);
		SDL_FreeSurface(lay);
	}
	tex = SDL_CreateTextureFromSurface(Rend, wall);
    SDL_FreeSurface(wall);
    return tex;
}

void Graphics::DestroyTex(SDL_Texture *tex) {
    SDL_DestroyTexture(tex);
}

void Graphics::Render() {
    SDL_RenderPresent(Rend);
}

void Graphics::Clear() {
    SDL_RenderClear(Rend);
}
