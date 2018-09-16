#pragma once
#include <switch.h>
#include <string>
#include <vector>
#include <functional>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h> 
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "MenuObj.hpp"
#include "Render.hpp"
#include "Themes.hpp"

using namespace std;

class MenuOption : public MenuObj
{
    public:
        MenuOption(string Name, string Desc, function<void()> Call);
        string getDesc();
        void callFunc();
        vector<MenuOption> subMenu;

    protected:
        string desc;
        function<void()> callback;
};