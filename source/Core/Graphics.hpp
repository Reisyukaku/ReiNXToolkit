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
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <switch.h>
#include <stdint.h>
#include <cmath>
#include <string>
#include "Font.hpp"
#include "Font.hpp"

typedef enum {
    ELEM_Unselected = BIT(0),
    ELEM_Selected = BIT(1),
    ELEM_DrawOutline = BIT(2)
} ElementType;

typedef struct Rect {
    int x;
    int y;
    int w;
    int h;
} Rect;

class Graphics
{
    public:
        static SDL_Renderer *Rend;
        static SDL_Window *Window;
        
        static void Init(Rect pos);
        static void Exit();
		static void ClearScreen();
        static void BorderedRectangle(Rect pos, u32 foreCol, u32 bordCol, u8 bordThick);
        static void Rectangle(Rect pos, u32 scolor);
        static SDL_Texture *CreateTexFromString(std::string file, Rect *pos = nullptr);
        static SDL_Texture *CreateTexFromBuffer(void *buffer, size_t size);
        static SDL_Texture *CreateTexFromSurf(SDL_Surface *surf);
        static void DrawText(u8 fntsize, s32 x, s32 y, std::string str, u32 col = msgCol, u32 wrap = Screen.w);
        static void RenderTexture(SDL_Texture *tex, Rect pos, Rect *clip = NULL);
        static void DrawImageElem(SDL_Texture *tex, Rect pos, ElementType type);
        static void DrawButton(Rect Pos, std::string Text, ElementType butType);
        static void DrawOption(Rect Pos, std::string Text, std::string OptionText, ElementType type);
        static SDL_Texture *BlitSurfacesFromString(std::vector<std::string> layers, Rect pos);
        static void DestroyTex(SDL_Texture *tex);
        static Result ImageToSplash(SDL_Surface *img);
        static u32 GetWinWidth() { return Screen.w; }
        static u32 GetWinHeight() { return Screen.h; }
        static void Render();
        static void Clear();
        
        //colors
        static u32 GetSelectCol() { return selectCol; }
        static void SetSelectCol(u32 col) { selectCol = col; }
        static u32 GetButtonCol() { return butCol; }
        static void SetButtonCol(u32 col) { butCol = col; }
        static u32 GetMessageCol() { return msgCol; }
        static void SetMessageCol(u32 col) { msgCol = col; }
        static u32 GetHeaderCol() { return headerCol; }
        static void SetHeaderCol(u32 col) { headerCol = col; }
        
    private:
        static u32 selectCol, msgCol, butCol, headerCol;
        static Context ctxt;
        static Rect Screen;
        static u32 glowInt;
};
