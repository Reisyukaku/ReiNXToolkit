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

class Themes {
public:
    typedef enum ThemeType {
        THEME_DARK = 0,
        THEME_LIGHT
    };
    static Themes* getInstance() {
        if (instance == nullptr)
            instance = new Themes();
        return instance;
    };
    
    void SetTheme(ThemeType type) {
        switch(type){
            case THEME_DARK: {
                bg = "romfs:/Graphics/defaultDark.png";
                msgBox = "romfs:/Graphics/Forms/PopupMessage.png";
                break;
            }
            case THEME_LIGHT: {
                bg = "romfs:/Graphics/defaultLight.png";
                msgBox = "romfs:/Graphics/Forms/PopupMessage.png";
                break;
            }
        }
    }
    
    std::string GetBackgroundTex() { return bg; }
    std::string GetPopupMessageTex() { return msgBox; }

private:
    static Themes* instance;

    Themes() {
        //
    };
    
    Themes(const Themes&);
    Themes& operator=(const Themes&);
    std::string bg, msgBox;
};