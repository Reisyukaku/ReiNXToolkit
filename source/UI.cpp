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

#include <switch.h>
#include "Net/Request.hpp"
#include "Tools/autorcm.hpp"
#include "Tools/kipmanager.hpp"
#include "Utils/unzip_utils.hpp"
#include "FS.hpp"
#include "UI.hpp"
#include "Power.hpp"

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720

static u32 currSel = 0;
static u32 currSubSel = 0;
static u32 vol = 64;
static u32 titleX = 60;
static u32 titleY = 30;
static u32 menuX = 55, menuY = 115;
static u32 subX = 411, subY = 88;

// Input vars, updated every iteration of the loop, proper way to access input using CustomUI
static u64 HeldInput = 0;
static u64 PressedInput = 0;
static u64 ReleasedInput = 0;

static string title;
static string version;

u32 clippy = 0;

vector<MenuOption> mainMenu;
vector<SDL_Surface*> images;

Mix_Music *menuSel;
Mix_Music *menuConfirm;
Mix_Music *menuBack;

UI * UI::mInstance = 0;

/* ---------------------------------------------------------------------------------------
* Menu functions
*/
void UI::optReiUpdate() {
    ProgBar prog;
    prog.max = 4;
    prog.step = 1;
    string url = "http://builds.reinx.guide/nightly/ReiNX-latest.zip";
    CreateProgressBar(&prog, "Updating ReiNX...");
    
    Net net = Net();
    hidScanInput();
    if(hidKeysDown(CONTROLLER_P1_AUTO) & KEY_L) {
        if(MessageBox("Warning!", "You are about to do a clean install.\nThis deletes the ReiNX folder!", TYPE_YES_NO)) {
            FS::DeleteDirRecursive("./ReiNX");
        }
    }
    bool res = net.Download(url, "/ReiNX.zip");
    IncrementProgressBar(&prog);
    if(!res){
        appletBeginBlockingHomeButton(0);
        unzFile zip = Utils::zip_open("/ReiNX.zip"); IncrementProgressBar(&prog);
        Utils::zip_extract_all(zip, "/"); IncrementProgressBar(&prog);
        Utils::zip_close(zip); IncrementProgressBar(&prog);
        remove("/ReiNX.zip");
        appletEndBlockingHomeButton();
        MessageBox("Update", "Update has downloaded successfully!", TYPE_OK);
    }else{
        prog.curr = 4;
        MessageBox("Update", "Update unsuccessful!", TYPE_OK);
    }
}

void UI::optAutoRCM() {
    bool res = MessageBox("Warning!", "THIS WRITES TO NAND.\nDo you want to continue?", TYPE_YES_NO);
    if(res) {
        appletBeginBlockingHomeButton(0);
        Tools::toggle_rcm();
        appletEndBlockingHomeButton();
        MessageBox("AutoRCM", "AutoRCM Toggled!", TYPE_OK);
    }
    
    //Easter egg ;^)
    if(!res) clippy++; else clippy = 0;
    if (clippy == 5) {
        SDL_Surface* image = IMG_Load("romfs:/Graphics/clippy.png");
        SDL_Texture* tex = SDL_CreateTextureFromSurface(mRender._renderer, image);
        drawBackXY(image, tex, 50, 350);
        SDL_RenderPresent(mRender._renderer);
        SDL_Delay(5000);
        SDL_DestroyTexture(tex);
        SDL_FreeSurface(image);
    }
}

void UI::optDumpCal0() {
    if(Tools::CheckFreeSpace() >= (u64)BLOCK_SIZE*4) {
        appletBeginBlockingHomeButton(0);
        Tools::DumpPartition(27, "cal0.bin");
        appletEndBlockingHomeButton();
    } else {
        MessageBox("Warning!", "Not enough space on the SD card to write to!", TYPE_OK);
    }
}

void UI::optDumpBoots() {
    if(Tools::CheckFreeSpace() >= ((u64)BLOCK_SIZE*8)) {
        appletBeginBlockingHomeButton(0);
        Tools::DumpPartition(0, "boot0.bin");
        Tools::DumpPartition(10, "boot1.bin");
        appletEndBlockingHomeButton();
    } else {
        MessageBox("Warning!", "Not enough space on the SD card to write to!", TYPE_OK);
    }
}

void UI::optDumpNand() {
    if(Tools::CheckFreeSpace() >= (u64)MAX_SIZE) {
        appletBeginBlockingHomeButton(0);
        Tools::DumpPartition(20, "nand.bin");
        appletEndBlockingHomeButton();
    } else {
        MessageBox("Warning!", "Not enough space on the SD card to write to!", TYPE_OK);
    }
}

void UI::optToggleKip(string path) {
    int ret = kip::Toggle(path);
    if (ret != 0)
        MessageBox("Failed to toggle KIP", "Error code " + to_string(ret), TYPE_OK);
    UI::drawKipman();
}

void UI::drawKipman() {
    mainMenu[2].subMenu.clear();
    vector<string> kips;
    kip::LoadKips(&kips);

    for(unsigned int i=0;i<kips.size();i++) {
        mainMenu[2].subMenu.push_back(MenuOption(kip::Name(kips[i]), "", 
          bind(&UI::optToggleKip, this, kips[i])));
    }
}

/*
* SubMenus
*/
//Change splash
void UI::optImage(u32 ind) {
    SDL_Surface *converted_surface = SDL_ConvertSurfaceFormat(images[ind], SDL_PIXELFORMAT_ABGR8888, 0);
    if(converted_surface->w != WIN_WIDTH || converted_surface->h != WIN_HEIGHT) {
        MessageBox("Warning!", "Image needs to be 1280 x 720!", TYPE_OK);
        return;
    }
    size_t size = 768 * 1280 * 4;
    u8* out_pixels = (u8*)malloc(size);
    u8* in_pixels = static_cast<u8*> (converted_surface->pixels);
    
    int i = 0;
    for(int y=0; y<768; y++)
        for(int x=1280; x>0; x--)
            memcpy(&out_pixels[(x*768+(x==1280?0:y)) * 4], &in_pixels[(i++) * 4], 4);

    FILE *fp = fopen("/ReiNX/splash.bin", "wb");
    fwrite(out_pixels, size, 1, fp);
    fclose(fp);
    free(out_pixels);
    
    SDL_FreeSurface(converted_surface);
    MessageBox("Conversion", "Image successfully converted!", TYPE_OK);
}

//Power
void UI::optReboot() {
    UI::deinit();
    Power::Reboot();
}
void UI::optShutdown() {
    UI::deinit();
    Power::Shutdown();
}

//Help
void UI::optAbout() {
    MessageBox(
        "About", 
        "Version: " + version + 
        "\n\n" +
        "Main developers:\n" +
        "Rei - Concept/UI\n" +
        "Calypso - Nand functions\n\n" +
        "Contributers:\n" +
        "fgsfds - Utils code\n" +
        "Crusatyr - Testing and Nand functions\n" +
        "neonsea - KIP Manager", 
    TYPE_OK);
}

void UI::optUpdateHB() {
    ProgBar prog;
    prog.max = 1;
    prog.step = 1;
    string url = "http://builds.reinx.guide/toolkit/ReiNX_Toolkit-latest.nro";

    if (!MessageBox("Update", 
      "This will attempt to update the Toolbox.\nAfter updating, the app will exit.\n\nContinue?", 
      TYPE_YES_NO))
        return;

    appletBeginBlockingHomeButton(0);
    CreateProgressBar(&prog, "Updating Toolkit...");
    
    Net net = Net();
    if (net.Download(url, "/switch/ReiNXToolkit_new.nro")){
        prog.curr = 1;
        appletEndBlockingHomeButton();
        MessageBox("Update", "Update unsuccessful!", TYPE_OK);
        return;
    }

    IncrementProgressBar(&prog);
    romfsExit();
    remove("/switch/ReiNXToolkit.nro");
    rename("/switch/ReiNXToolkit_new.nro", "/switch/ReiNXToolkit.nro");
    fsdevCommitDevice("sdmc");
    exitApp();
}

/* ---------------------------------------------------------------------------------------
* UI class functions
*/
UI::UI(string Title, string Version) {
    romfsInit();
    fsdevMountSdmc();
    socketInitializeDefault();
#ifdef DEBUG
    nxlinkStdio();
    printf("printf output now goes to nxlink server\n");
#endif
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(WIN_WIDTH, WIN_HEIGHT, 0, &mRender._window, &mRender._renderer);
    mRender._surface = SDL_GetWindowSurface(mRender._window);
    SDL_SetRenderDrawBlendMode(mRender._renderer, SDL_BLENDMODE_BLEND);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    TTF_Init();
    SDL_SetRenderDrawColor(mRender._renderer, 255, 255, 255, 255);
    Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
    Mix_VolumeMusic(vol);
    mThemes = Themes::instance();
    mThemes->Init(mRender);
    inSubMenu = false;

    title = Title;
    version = Version;
    
    menuSel = Mix_LoadMUS("romfs:/Sounds/menu_select.mp3");
    menuConfirm = Mix_LoadMUS("romfs:/Sounds/menu_confirm.mp3");
    menuBack = Mix_LoadMUS("romfs:/Sounds/menu_back.mp3");
    
    //Main pages
    mainMenu.push_back(MenuOption("ReiNX Updates", "Update ReiNX CFW.", nullptr));
    mainMenu.push_back(MenuOption("Change splash", "Change ReiNX splash.bin image.", nullptr));
    mainMenu.push_back(MenuOption("KIP Manager", "Enable or disable KIPs. Requires a reboot to take effect.", nullptr));
    mainMenu.push_back(MenuOption("Toggle AutoRCM", "Toggles AutoRCM by writing to NAND.",  bind(&UI::optAutoRCM, this)));
    mainMenu.push_back(MenuOption("Backup tool", "Backup various partitions from NAND.", nullptr));
    mainMenu.push_back(MenuOption("Power", "Power options.", nullptr));
    mainMenu.push_back(MenuOption("Help", "Information in regards to this software.", nullptr));
    
    //Subpages
    mainMenu[0].subMenu.push_back(MenuOption("Update ReiNX", "", bind(&UI::optReiUpdate, this)));
    mainMenu[0].subMenu.push_back(MenuOption("Update Toolkit", "", bind(&UI::optUpdateHB, this)));

    vector<string> paths = FS::EnumDir("/Toolkit/splashes");
    for(unsigned int i=0;i<paths.size();i++) {
        mainMenu[1].subMenu.push_back(MenuOption(paths[i], "", bind(&UI::optImage, this, i)));
        images.push_back(IMG_Load(("/Toolkit/splashes/"+paths[i]).c_str()));
    }

    UI::drawKipman();

    mainMenu[4].subMenu.push_back(MenuOption("Backup Cal0", "", bind(&UI::optDumpCal0, this)));
    mainMenu[4].subMenu.push_back(MenuOption("Backup Boot0/1", "", bind(&UI::optDumpBoots, this)));
    mainMenu[4].subMenu.push_back(MenuOption("Backup NAND", "", bind(&UI::optDumpNand, this)));
    
    mainMenu[5].subMenu.push_back(MenuOption("Reboot", "", bind(&UI::optReboot, this)));
    mainMenu[5].subMenu.push_back(MenuOption("Shutdown", "", bind(&UI::optShutdown, this)));
    
    mainMenu[6].subMenu.push_back(MenuOption("About", "", bind(&UI::optAbout, this)));
    
    //Make dirs
    if(!FS::DirExists("/Toolkit"))  FS::MakeDir("/Toolkit", 0777);
    if(!FS::DirExists("/Toolkit/splashes"))  FS::MakeDir("/Toolkit/splashes", 0777);
}

void UI::setInstance(UI ui) {
    mInstance = &ui;
}

UI *UI::getInstance() {
    if(!mInstance)
        return NULL;
    return mInstance;
}

void UI::deinit() {
    TTF_Quit();
    IMG_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_DestroyRenderer(mRender._renderer);
    SDL_FreeSurface(mRender._surface);
    SDL_DestroyWindow(mRender._window);
    SDL_Quit();
    romfsExit();
    socketExit();
    fsdevUnmountAll();
}

void UI::exitApp() {
    deinit();
    appletEndBlockingHomeButton(); // make sure we don't screw up hbmenu
    ::exit(0);
}

/*
* UI draw functions
*/
void UI::drawText(int x, int y, SDL_Color scolor, string text, TTF_Font *font) {
    SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(font, text.c_str(), scolor, 1280);
    SDL_SetSurfaceAlphaMod(surface, 255);
    SDL_Rect position = { x, y, surface->w, surface->h };
    SDL_BlitSurface(surface, NULL, mRender._surface, &position);
    SDL_FreeSurface(surface);
}

void UI::drawRect(int x, int y, int w, int h, SDL_Color scolor, unsigned border, SDL_Color bcolor) {
    drawRect(x-border, y-border, w+(2*border), h+(2*border), bcolor);
    drawRect(x, y, w, h, scolor);
}

void UI::drawRect(int x, int y, int w, int h, SDL_Color scolor) {
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_SetRenderDrawColor(mRender._renderer, scolor.r, scolor.g, scolor.b, scolor.a);
    SDL_RenderFillRect(mRender._renderer, &rect);
}

void UI::drawBackXY(SDL_Surface *surf, SDL_Texture *tex, int x, int y) {
    SDL_Rect position;
    position.x = x;
    position.y = y;
    position.w = surf->w;
    position.h = surf->h;
    SDL_RenderCopy(mRender._renderer, tex, NULL, &position);
}

void UI::drawScaled(SDL_Surface *surf, SDL_Texture *tex, int x, int y, u32 w, u32 h) {
    SDL_Rect position;
    position.x = x;
    position.y = y;
    position.w = w;
    position.h = h;
    SDL_RenderCopy(mRender._renderer, tex, NULL, &position);
}

/*
* UI Pop-up stuff
*/
void UI::CreatePopupBox(u32 x, u32 y, u32 w, u32 h, string header) {
    drawRect(x-5, y-5, w+10, h+70, {0, 0, 0, 0xFF}); //BG box
    drawRect(x, y, w, 57, mThemes->popCol1); //Header
    drawRect(x, y+60, w, h, mThemes->popCol2); //Message
    drawText(x+15, y+15, mThemes->txtcolor, header, mThemes->fntMedium);
}

void UI::CreateProgressBar(ProgBar *prog, string header) {
    u32 poph = 100, popw = 300;
    u32 barh = 40, barw = 200;
    u32 startx = (WIN_WIDTH/2)-(popw/2), starty = (WIN_HEIGHT/2)-(poph/2);
    
    if(prog->max <= 0) prog->max = 100;
    if(prog->step <= 0) prog->step = 1;
    prog->curr = 0;
    prog->posx = startx;
    prog->posy = starty;
    CreatePopupBox(prog->posx, prog->posy, popw, poph, header);
    drawRect(prog->posx+(popw/2)-(barw/2), prog->posy+60+((poph-60)/2), barw, barh, {0, 0, 0, 0xFF}); //empty bar
    drawText(prog->posx+(popw/2), prog->posy+(poph/2)-(barh/2)+60, {0xFF,0xFF,0xFF,0xFF}, to_string((unsigned)((prog->curr/prog->max)*100)) + "%", mThemes->fntSmall);
    SDL_RenderPresent(mRender._renderer);
}

void UI::IncrementProgressBar(ProgBar *prog) {
    u32 poph = 100+2, popw = 300+2;
    u32 barh = 40-4, barw = 200-4;
    drawRect(prog->posx+(popw/2)-(barw/2), prog->posy+60+((poph-60)/2), barw, barh, {0, 0, 0, 0xFF}); //empty bar
    drawRect(prog->posx+(popw/2)-(barw/2), prog->posy+60+((poph-60)/2), (unsigned)(barw*(prog->curr/prog->max)), barh, {0, 0xFF, 0, 0xFF}); //green bar
    drawText(prog->posx+(popw/2), prog->posy+(poph/2)-(barh/2)+60, {0xFF,0xFF,0xFF,0xFF}, to_string((unsigned)((prog->curr/prog->max)*100)) + "%", mThemes->fntSmall);
    prog->curr += prog->step;
    SDL_RenderPresent(mRender._renderer);
}

bool UI::MessageBox(string header, string message, MessageType type) {
    bool ret = false;
    SDL_Rect rect;
    u32 poph = 300, popw = 450;
    u32 buth = 50, butw = 100;
    u32 startx = (WIN_WIDTH/2)-(popw/2), starty = (WIN_HEIGHT/2)-(poph/2);
    
    CreatePopupBox(startx, starty, popw, poph, header);
    drawText(startx+15, starty+75, mThemes->txtcolor, message, mThemes->fntMedium);
    
    switch(type){
        case TYPE_YES_NO:
            drawRect(startx+popw-butw-10, starty+poph, butw, buth, mThemes->popCol1, 3, {0, 0, 0, 0xFF}); //YES
            drawText(startx+popw-butw+12, starty+poph+12, mThemes->txtcolor, "\ue0e0 Yes", mThemes->fntMedium);
            drawRect(startx+popw-(2*(butw+5))-10, starty+poph, butw, buth, mThemes->popCol1, 3, {0, 0, 0, 0xFF}); //NO
            drawText(startx+popw-(2*(butw+5))+12, starty+poph+12, mThemes->txtcolor, "\ue0e1 No", mThemes->fntMedium);
            break;
        default:
        case TYPE_OK:
            drawRect(startx+popw-butw-10, starty+poph, butw, buth, mThemes->popCol1, 3, {0, 0, 0, 0xFF}); //OK
            drawText(startx+popw-butw+12, starty+poph+12, mThemes->txtcolor, "\ue0e0 OK", mThemes->fntMedium);
            break;
    }
    SDL_RenderPresent(mRender._renderer);
    while(1){
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        if(kDown & KEY_A) {
            ret = true;
            Mix_PlayMusic(menuConfirm, 1);
            break;
        }
        else if(kDown & KEY_B) {
            ret = (type == TYPE_OK);
            Mix_PlayMusic(menuBack, 1);
            break;
        }
    }
    return ret;
}

/*
* Render function
*/
void UI::renderMenu() {
    SDL_RenderClear(mRender._renderer);
    drawBackXY(mThemes->bgs, mThemes->bgt, 0, 0);
    //Mainmenu  text
    drawText(titleX, titleY, mThemes->txtcolor, title, mThemes->fntLarge);
    int oy = menuY;
    if(!mainMenu.empty()) for(unsigned int i = 0; i < mainMenu.size(); i++){
        //Mainmenu buttons
        if(i == currSel && !inSubMenu) {
            drawRect(menuX-10, oy-10, 210+20, 25+20, mThemes->butCol, 5, mThemes->butBorderCol);
            drawText(menuX, oy, mThemes->selcolor, mainMenu[i].getName(), mThemes->fntMedium);
        } else {
            drawText(menuX, oy, mThemes->txtcolor, mainMenu[i].getName(), mThemes->fntMedium);
        }
        
        if(i == currSel) {
            //Submenu text
            drawText(subX + 30, subY + 30, mThemes->txtcolor, mainMenu[i].getDesc(), mThemes->fntSmall);
            if(!mainMenu[i].subMenu.empty()) for(unsigned int j = 0; j < mainMenu[i].subMenu.size(); j++){
                //Submenu buttons
                if(j == currSubSel && inSubMenu) {
                    drawRect(subX + 10, subY + 20 + ((j+1)*50), 200+20, 40, mThemes->butCol, 3, mThemes->butBorderCol);
                    drawText(subX + 30, subY + 30 + ((j+1)*50), mThemes->selcolor, mainMenu[i].subMenu[j].getName(), mThemes->fntMedium);
                }else{
                    drawText(subX + 30, subY + 30 + ((j+1)*50), mThemes->txtcolor, mainMenu[i].subMenu[j].getName(), mThemes->fntMedium);
                }
                if(j == currSubSel && currSel == 1) {
                    SDL_Texture* tex = SDL_CreateTextureFromSurface(mRender._renderer, images[currSubSel]);
                    drawScaled(images[currSubSel], tex, 710, 120, images[currSubSel]->w/3, images[currSubSel]->h/3);
                }
            }
        }
        oy += 50;
    }
    SDL_RenderPresent(mRender._renderer);
}

/*
* Menu nav functions
*/
void UI::MenuUp() {
    Mix_PlayMusic(menuSel, 1);
    if(currSel > 0) currSel--;
    else currSel = mainMenu.size() - 1;
    currSubSel = 0;
}

void UI::MenuDown() {
    Mix_PlayMusic(menuSel, 1);
    if((unsigned int) currSel < mainMenu.size() - 1) currSel++;
    else currSel = 0;
    currSubSel = 0;
}

void UI::SubMenuUp() {
    u32 subSize = mainMenu[currSel].subMenu.size();
    if(subSize > 1) Mix_PlayMusic(menuSel, 1);
    if(currSubSel > 0) currSubSel--;
    else currSubSel = subSize - 1;
}

void UI::SubMenuDown() {
    u32 subSize = mainMenu[currSel].subMenu.size();
    if(subSize > 1) 
        Mix_PlayMusic(menuSel, 1);
    if((unsigned int) currSubSel < subSize - 1) 
        currSubSel++;
    else currSubSel = 0;
}

void UI::MenuSel() {
    Mix_PlayMusic(menuConfirm, 1);
    if(!inSubMenu){
        if(mainMenu[currSel].subMenu.empty()) 
            mainMenu[currSel].callFunc();
        else 
            inSubMenu = true;
    }else{
        mainMenu[currSel].subMenu[currSubSel].callFunc();
    }
}

void UI::MenuBack() {
    Mix_PlayMusic(menuBack, 1);
    if(inSubMenu)
        inSubMenu = false;
    else
        exitApp();
}