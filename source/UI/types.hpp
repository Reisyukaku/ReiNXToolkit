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

#pragma once
#include <string>
#include <stdint.h>
typedef uint32_t in_addr_t;
typedef uint16_t in_port_t;
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/errno.h>

using namespace std;

#define WHITE 0xFFFFFFFF
#define BLACK 0xFF
#define GREY 0x4C4C4CFF
#define LIGHTGREY 0x8C8C8CFF
#define RED 0xFF0000FF
#define BLUE 0xFFFF
#define TEAL 0xFFCEFF
#define LIGHTBLUE 0x44C0DAFF
#define GREEN 0xFF00FF

enum MessageType {TYPE_OK, TYPE_YES_NO};

typedef struct
{
    string BackgroundPath;
    string FontPath;
    unsigned TextColor;
    unsigned SelectedTextColor;
    unsigned ButtonColor;
    unsigned ButtonBorderColor;
    unsigned BGColor;
    unsigned PopCol1;
    unsigned PopCol2;
} Theme;

typedef struct
{
    float max;
    float step;
    float curr;
    u32 posx;
    u32 posy;
} ProgBar;