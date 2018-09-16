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