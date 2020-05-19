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
#include "../Core/Graphics.hpp"

enum Element {
    ELEM_None,
    ELEM_Image,
    ELEM_Button,
    ELEM_Option
};

class UIElement
{
    public:
        UIElement() {
            //
        }
        virtual ~UIElement() {
            Callback = nullptr;
        }
        
        virtual void Draw(ElementType type, Rect pos) {}
        virtual Result Run() { return 0; }
        bool HasFunc() { return Callback != nullptr; }
        
        //Setters for panel elements
        virtual void SetOption(std::string text, u32 ind = 0) {}
        virtual void SetOptions(std::vector<std::string> opts) {}
        virtual void IncrementOption(u32 cap = 0) {}
        virtual void SetImage(SDL_Texture *tex) {}
        virtual void SetNameText(std::string name) {}
        virtual void SetCallback(std::function<Result()> callback) { Callback = callback; }
        
        Element Properties;
        std::string Name;
    protected:
        std::vector<UIElement*> subs;
        std::function<Result()> Callback;
};