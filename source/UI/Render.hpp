#pragma once

class Render{
    public:
        SDL_Window *_window;
        SDL_Renderer *_renderer;
        SDL_Surface *_surface;
		Render() {
			_window = NULL;
			_renderer = NULL;
			_surface = NULL;
		}
};