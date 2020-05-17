/*
* ReiNX Toolkit
* Copyright (C) 2018  Reisyukaku
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

#include <SDL2/SDL_mixer.h>
#include <string>

class Audio {
public:
    Audio(u32 vol) {
        Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG);
        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
        Mix_VolumeMusic(vol);
    }
    
    ~Audio() {
        Mix_CloseAudio();
        Mix_Quit();
    }
    
    Mix_Music *Load(std::string file) {
        return Mix_LoadMUS(file.c_str());
    }
};