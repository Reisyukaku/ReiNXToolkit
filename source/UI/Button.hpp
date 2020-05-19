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
#include <functional>
#include <string>
#include "UIElement.hpp"
#include "../Core/Graphics.hpp"

#define MENU_BUTTON_WIDTH 280
#define MENU_BUTTON_HEIGHT 50

class Button: public UIElement
{
    public:        
        Button(std::string text, std::function<Result()> callback = nullptr) : UIElement() {
            printf("[UIElement]: Creating button %s\n", text.c_str());
            Callback = callback;
            Properties = ELEM_Button;
            Color = Graphics::GetSelectCol();
            Name = text;
        }
        
        Button(std::string text, UIElement *sub) : UIElement() {
            printf("[UIElement]: Creating sub button %s\n", text.c_str());
            Callback = nullptr;
            Properties = ELEM_Button;
            Color = Graphics::GetSelectCol();
            Name = text;
        }
        
        ~Button() {
            //
        }
        
        void Draw(ElementType butType, Rect pos) override{
            Graphics::DrawButton(pos, Name, butType);
        }
        
        Result Run() override {
            Result res = 0;
            if(Callback != nullptr) res = Callback();
            return res;
        }
        
        //vars
        u32 Color;
};