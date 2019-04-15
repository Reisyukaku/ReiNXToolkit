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

#include "Graphics.hpp"

SDL_Renderer *Graphics::Rend;
SDL_Window *Graphics::Window;

void Graphics::Init(std::string name, u32 width, u32 height) {
    //Basic SDL init
    SDL_Init(SDL_INIT_EVERYTHING);
    Window = SDL_CreateWindow(name.c_str(), 0, 0, width, height, 0);
    Rend = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(Rend, SDL_BLENDMODE_BLEND);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    TTF_Init();
    SDL_SetRenderDrawColor(Rend, 0xFF, 0xFF, 0xFF, 0xFF);
}

void Graphics::Exit() {
    TTF_Quit();
    IMG_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_DestroyRenderer(Rend);
    SDL_DestroyWindow(Window);
    SDL_Quit();
}

void Graphics::ClearScreen() {
	Rectangle(0, 0, 1280, 720, 0xFFFFFFFF);
}

void Graphics::Rectangle(u32 x, u32 y, u32 w, u32 h, u32 scolor) {
    SDL_Rect pos;
    pos.x = x;
    pos.y = y;
    pos.w = w;
    pos.h = h;
    
    Rectangle(pos, scolor);
}

void Graphics::Rectangle(SDL_Rect pos, u32 scolor) {
    SDL_SetRenderDrawColor(Rend, ((scolor >> 24) & 0xFF), ((scolor >> 16) & 0xFF), ((scolor >> 8) & 0xFF), (scolor & 0xFF));
    SDL_RenderFillRect(Rend, &pos);
}

void Graphics::DrawTexture(std::string tex, u32 x, u32 y) {
    SDL_Surface *bgs = IMG_Load(tex.c_str()); //Maybe cache images in the future so not to constantly read SD?
    if(!bgs) return;
    SDL_Rect pos;
    pos.x = x;
    pos.y = y;
    pos.w = bgs->w;
    pos.h = bgs->h;
    SDL_Texture *t = SDL_CreateTextureFromSurface(Rend, bgs);
	if(t) {
		RenderTexture(t, pos);
		SDL_FreeSurface(bgs);
	}
    SDL_DestroyTexture(t);
}

SDL_Texture *Graphics::CreateTexFromSurf(SDL_Surface *surf) {
    return SDL_CreateTextureFromSurface(Rend, surf);
}

void Graphics::RenderTexture(SDL_Texture *tex, SDL_Rect pos) {
	if(!tex) return;
    SDL_RenderCopy(Rend, tex, NULL, &pos);
}

void Graphics::DrawText(TTF_Font *font, u32 x, u32 y, std::string str) {
    DrawText(font, x, y, str, 0xFFFFFFFF);
}

void Graphics::DrawText(TTF_Font *font, u32 x, u32 y, std::string str, u32 col) {
	if(!font) return;
    SDL_Color scolor;
    scolor.r = (col >> 24)&0xFF; scolor.g = (col >> 16)&0xFF; scolor.b = (col >> 8)&0xFF; scolor.a = col&0xFF;
    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(font, str.c_str(), scolor, 1280);
    if (!surface) return;

    SDL_SetSurfaceAlphaMod(surface, 0xFF);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(Rend, surface);

    if (texture) {
        SDL_Rect pos;
        pos.x = x;
        pos.y = y;
        pos.w = surface->w;
        pos.h = surface->h;
        SDL_RenderCopy(Rend, texture, NULL, &pos);
        SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(surface);
}


void Graphics::Render() {
    SDL_RenderPresent(Rend);
}

void Graphics::Clear() {
    SDL_RenderClear(Rend);
}