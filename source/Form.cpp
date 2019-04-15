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
#include "Tools/reinxconfig.hpp"
#include "Tools/nandDump.hpp"
#include "Utils/unzip_utils.hpp"
#include "FS.hpp"
#include "Form.hpp"
#include "Power.hpp"

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720

static u32 currSel = 0;
static u32 currSubSel = 0;
static u32 titleX = 60;
static u32 titleY = 30;
static u32 menuX = 55, menuY = 115;
static u32 subX = 411, subY = 88;

// Input vars, updated every iteration of the loop, proper way to access input using CustomForm
static u64 HeldInput = 0;
static u64 PressedInput = 0;
static u64 ReleasedInput = 0;

static string title;
static string version;

vector<MenuOption*> mainMenu;
vector<SDL_Surface*> images;

Mix_Music *menuSel;
Mix_Music *menuConfirm;
Mix_Music *menuBack;

/* ---------------------------------------------------------------------------------------
* Menu functions
*/
void Form::optReiUpdate() {
    ProgBar prog;
    prog.max = 4;
    prog.step = 1;
    string url = "http://builds.reinx.guide/nightly/ReiNX-latest.zip";
    UI::CreateProgressBar(&prog, "Updating ReiNX...");
    
    Net net = Net();
    hidScanInput();
    if(hidKeysDown(CONTROLLER_P1_AUTO) & KEY_L) {
        if(UI::MessageBox("Warning!", "You are about to do a clean install.\nThis deletes the ReiNX folder!", TYPE_YES_NO)) {
            FS::DeleteDirRecursive("./ReiNX");
        }
    }
    bool res = net.Download(url, "/ReiNX.zip");
    UI::IncrementProgressBar(&prog);
    if(!res){
        appletBeginBlockingHomeButton(0);
        unzFile zip = Utils::zip_open("/ReiNX.zip"); UI::IncrementProgressBar(&prog);
        Utils::zip_extract_all(zip, "/"); UI::IncrementProgressBar(&prog);
        Utils::zip_close(zip); UI::IncrementProgressBar(&prog);
        remove("/ReiNX.zip");
        appletEndBlockingHomeButton();
        UI::MessageBox("Update", "Update has downloaded successfully!", TYPE_OK);
    }else{
        prog.curr = 4;
        UI::MessageBox("Update", "Update unsuccessful!", TYPE_OK);
    }
}

void Form::optAutoRCM() {
    bool res = UI::MessageBox("Warning!", "THIS WRITES TO NAND.\nDo you want to continue?", TYPE_YES_NO);
    if(res) {
        appletBeginBlockingHomeButton(0);
        int ret = Tools::toggle_rcm();
        appletEndBlockingHomeButton();
        UI::MessageBox("AutoRCM", ret ? "AutoRCM enabled" : "AutoRCM disabled", TYPE_OK);
    }
}

void Form::optDumpCal0() {
    if(Tools::CheckFreeSpace() >= CAL0_BLOCK_SIZE) {
        appletBeginBlockingHomeButton(0);
        Tools::DumpPartition(ProdInfo, "cal0.bin");
        appletEndBlockingHomeButton();
    } else {
        UI::MessageBox("Warning!", "Not enough space on the SD card to write to!", TYPE_OK);
    }
}

void Form::optDumpBoots() {
    if(Tools::CheckFreeSpace() >= BOOT_BLOCK_SIZE*2) {
        appletBeginBlockingHomeButton(0);
        Tools::DumpPartition(boot0, "boot0.bin");
        Tools::DumpPartition(boot1, "boot1.bin");
        appletEndBlockingHomeButton();
    } else {
        UI::MessageBox("Warning!", "Not enough space on the SD card to write to!", TYPE_OK);
    }
}

void Form::optDumpNand() {
    if(Tools::CheckFreeSpace() >= (u64)MAX_SIZE) {
        appletBeginBlockingHomeButton(0);
        Tools::DumpPartition(rawnand, "nand.bin");
        appletEndBlockingHomeButton();
    } else {
        UI::MessageBox("Warning!", "Not enough space on the SD card to write to!", TYPE_OK);
    }
}

/*
* SubMenus
*/
//Change splash
void Form::optImage(u32 ind) {
    SDL_Surface *converted_surface = SDL_ConvertSurfaceFormat(images[ind], SDL_PIXELFORMAT_ABGR8888, 0);
    if(converted_surface->w != WIN_WIDTH || converted_surface->h != WIN_HEIGHT) {
        UI::MessageBox("Warning!", "Image needs to be 1280 x 720!", TYPE_OK);
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
    UI::MessageBox("Conversion", "Image successfully converted!", TYPE_OK);
}

void Form::optToggleKip(string path) {
    int ret = kip::Toggle(path);
    if (ret != 0)
        UI::MessageBox("Failed to toggle KIP", "Error code " + to_string(ret), TYPE_OK);
    Form::drawKipman();
}

void Form::drawKipman() {
    mainMenu[2]->subMenu.clear();
    vector<string> kips;
    kip::LoadKips(&kips);

    for(unsigned int i=0;i<kips.size();i++) {
        mainMenu[2]->subMenu.push_back(new MenuOption(kip::Name(kips[i]), "", bind(&Form::optToggleKip, this, kips[i])));
    }
}

void Form::optCfwCfg(string file) {
    int ret = cfg::Toggle(file);
    if (ret != 0)
        UI::MessageBox("Failed to toggle config file", "Error code " + to_string(ret), TYPE_OK);
    Form::drawCfwman();
}

void Form::drawCfwman() {
    mainMenu[3]->subMenu.clear();
    vector<string> flags;
    
    flags.push_back("nogc");
    flags.push_back("debug");

    for(unsigned int i=0;i<flags.size();i++) {
        mainMenu[3]->subMenu.push_back(new MenuOption(cfg::Name(flags[i]), "", bind(&Form::optCfwCfg, this, flags[i])));
    }
}

//Power
void Form::optReboot() {
    Form::deinit();
    Power::Reboot();
}
void Form::optShutdown() {
    Form::deinit();
    Power::Shutdown();
}

//Help
void Form::optAbout() {
    UI::MessageBox(
        "About", 
        "Version: " + version + 
        "\n\n" +
        "Main developers:\n" +
        "Rei - Concept/Form\n" +
        "Calypso - Nand functions\n\n" +
        "Contributers:\n" +
        "fgsfds - Utils code\n" +
        "Crusatyr - Testing and Nand functions\n" +
        "neonsea - KIP Manager", 
    TYPE_OK);
}

void Form::optUpdateHB() {
    ProgBar prog;
    prog.max = 1;
    prog.step = 1;
    string url = "http://builds.reinx.guide/toolkit/ReiNXToolkit-latest.nro";

    if (!UI::MessageBox("Update", 
      "This will attempt to update the Toolbox.\nAfter updating, the app will exit.\n\nContinue?", 
      TYPE_YES_NO))
        return;

    appletBeginBlockingHomeButton(0);
    UI::CreateProgressBar(&prog, "Updating Toolkit...");
    
    Net net = Net();
    if (net.Download(url, "/switch/ReiNXToolkit_new.nro")){
        prog.curr = 1;
        appletEndBlockingHomeButton();
        UI::MessageBox("Update", "Update unsuccessful!", TYPE_OK);
        return;
    }

    UI::IncrementProgressBar(&prog);
    romfsExit();
    remove("/switch/ReiNXToolkit.nro");
    rename("/switch/ReiNXToolkit_new.nro", "/switch/ReiNXToolkit.nro");
    fsdevCommitDevice("sdmc");
    exitApp();
}

/* ---------------------------------------------------------------------------------------
* Form class functions
*/
Form::Form(string Title, string Version) {
    romfsInit();
    fsdevMountSdmc();
    socketInitializeDefault();
#ifdef DEBUG
    nxlinkStdio();
    printf("printf output now goes to nxlink server\n");
#endif
    
    WindowPos.x = WindowPos.y = 0;
    WindowPos.w = WIN_WIDTH;
    WindowPos.h = WIN_HEIGHT;
    title = Title;
    version = Version;
    Graphics::Init(Title, WindowPos.w, WindowPos.h);
    Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
    Mix_VolumeMusic(64);
}

Form::~Form() {
    
}

void Form::Init() {
    mThemes = new Themes();
    UI::Init(mThemes);
    inSubMenu = false;

    menuSel = Mix_LoadMUS("romfs:/Sounds/menu_select.mp3");
    menuConfirm = Mix_LoadMUS("romfs:/Sounds/menu_confirm.mp3");
    menuBack = Mix_LoadMUS("romfs:/Sounds/menu_back.mp3");
    
    //Main pages
    mainMenu.push_back(new MenuOption("ReiNX Updates", "Update ReiNX CFW.", nullptr));
    mainMenu.push_back(new MenuOption("Change splash", "Change ReiNX splash.bin image.", nullptr));
    mainMenu.push_back(new MenuOption("KIP Manager", "Enable or disable KIPs. Requires a reboot to take effect.", nullptr));
    mainMenu.push_back(new MenuOption("CFW Manager", "Enable or disable cfw options. Requires a reboot to take effect.", nullptr));
    mainMenu.push_back(new MenuOption("Toggle AutoRCM", "Toggles AutoRCM by writing to NAND.",  bind(&Form::optAutoRCM, this)));
    mainMenu.push_back(new MenuOption("Backup tool", "Backup various partitions from NAND.", nullptr));
    mainMenu.push_back(new MenuOption("Power", "Power options.", nullptr));
    mainMenu.push_back(new MenuOption("Help", "Information in regards to this software.", nullptr));
    
    //Subpages
    mainMenu[0]->subMenu.push_back(new MenuOption("Update ReiNX", "", bind(&Form::optReiUpdate, this)));
    mainMenu[0]->subMenu.push_back(new MenuOption("Update Toolkit", "", bind(&Form::optUpdateHB, this)));

    vector<string> paths = FS::EnumDir("/Toolkit/splashes");
    for(unsigned int i=0;i<paths.size();i++) {
        mainMenu[1]->subMenu.push_back(new MenuOption(paths[i], "", bind(&Form::optImage, this, i)));
        images.push_back(IMG_Load(("/Toolkit/splashes/"+paths[i]).c_str()));
    }

    Form::drawKipman();
    Form::drawCfwman();

    mainMenu[5]->subMenu.push_back(new MenuOption("Backup Cal0", "", bind(&Form::optDumpCal0, this)));
    mainMenu[5]->subMenu.push_back(new MenuOption("Backup Boot0/1", "", bind(&Form::optDumpBoots, this)));
    mainMenu[5]->subMenu.push_back(new MenuOption("Backup NAND", "", bind(&Form::optDumpNand, this)));
    
    mainMenu[6]->subMenu.push_back(new MenuOption("Reboot", "", bind(&Form::optReboot, this)));
    mainMenu[6]->subMenu.push_back(new MenuOption("Shutdown", "", bind(&Form::optShutdown, this)));
    
    mainMenu[7]->subMenu.push_back(new MenuOption("About", "", bind(&Form::optAbout, this)));
    
    //Make dirs
    if(!FS::DirExists("/Toolkit"))  FS::MakeDir("/Toolkit", 0777);
    if(!FS::DirExists("/Toolkit/splashes"))  FS::MakeDir("/Toolkit/splashes", 0777);
}

void Form::deinit() {
    mainMenu.clear();
    Graphics::Exit();
    romfsExit();
    socketExit();
    fsdevUnmountAll();
}

void Form::exitApp() {
    deinit();
    appletEndBlockingHomeButton(); // make sure we don't screw up hbmenu
    ::exit(0);
}

/*
* Render function
*/
void Form::Update() {
    Graphics::Clear();
    
    Graphics::RenderTexture(mThemes->bgt, WindowPos);
    //Mainmenu  text
    Graphics::DrawText(mThemes->fntLarge, titleX, titleY, title, mThemes->txtcolor);
    int oy = menuY;
    if(!mainMenu.empty()) for(unsigned int i = 0; i < mainMenu.size(); i++){
        //Mainmenu buttons
        if(i == currSel && !inSubMenu) {
            Graphics::Rectangle(menuX-5, oy-5, 210+30, 25+30, mThemes->butBorderCol);
            Graphics::Rectangle(menuX-10, oy-10, 210+20, 25+20, mThemes->butCol);
            Graphics::DrawText(mThemes->fntMedium, menuX, oy, mainMenu[i]->getName(), mThemes->selcolor);
        } else {
            Graphics::DrawText(mThemes->fntMedium, menuX, oy, mainMenu[i]->getName(), mThemes->txtcolor);
        }
        
        if(i == currSel) {
            //Submenu text
            Graphics::DrawText(mThemes->fntSmall, subX + 30, subY + 30, mainMenu[i]->getDesc(), mThemes->txtcolor);
            if(!mainMenu[i]->subMenu.empty()) for(unsigned int j = 0; j < mainMenu[i]->subMenu.size(); j++){
                //Submenu buttons
                if(j == currSubSel && inSubMenu) {
                    Graphics::Rectangle(subX + 10-3, subY + 20-3 + ((j+1)*50), 200+26, 46, mThemes->butBorderCol);
                    Graphics::Rectangle(subX + 10, subY + 20 + ((j+1)*50), 200+20, 40, mThemes->butCol);
                    Graphics::DrawText(mThemes->fntMedium, subX + 30, subY + 30 + ((j+1)*50), mainMenu[i]->subMenu[j]->getName(), mThemes->selcolor);
                }else{
                    Graphics::DrawText(mThemes->fntMedium, subX + 30, subY + 30 + ((j+1)*50), mainMenu[i]->subMenu[j]->getName(), mThemes->txtcolor);
                }
                if(j == currSubSel && currSel == 1) {
                    SDL_Texture* tex = Graphics::CreateTexFromSurf(images[currSubSel]);
                    SDL_Rect imgPos;
                    imgPos.x = 710; imgPos.y = 120;
                    imgPos.w = images[currSubSel]->w/3;
                    imgPos.h = images[currSubSel]->h/3;
                    Graphics::RenderTexture(tex, imgPos);
                }
            }
        }
        oy += 50;
    }
    Graphics::Render();
}

/*
* Menu nav functions
*/
void Form::MenuUp() {
    Mix_PlayMusic(menuSel, 1);
    if(currSel > 0) currSel--;
    else currSel = mainMenu.size() - 1;
    currSubSel = 0;
}

void Form::MenuDown() {
    Mix_PlayMusic(menuSel, 1);
    if((unsigned int) currSel < mainMenu.size() - 1) currSel++;
    else currSel = 0;
    currSubSel = 0;
}

void Form::SubMenuUp() {
    u32 subSize = mainMenu[currSel]->subMenu.size();
    if(subSize > 1) Mix_PlayMusic(menuSel, 1);
    if(currSubSel > 0) currSubSel--;
    else currSubSel = subSize - 1;
}

void Form::SubMenuDown() {
    u32 subSize = mainMenu[currSel]->subMenu.size();
    if(subSize > 1) 
        Mix_PlayMusic(menuSel, 1);
    if((unsigned int) currSubSel < subSize - 1) 
        currSubSel++;
    else currSubSel = 0;
}

void Form::MenuSel() {
    Mix_PlayMusic(menuConfirm, 1);
    if(!inSubMenu){
        if(mainMenu[currSel]->subMenu.empty()) 
            mainMenu[currSel]->callFunc();
        else 
            inSubMenu = true;
    }else{
        mainMenu[currSel]->subMenu[currSubSel]->callFunc();
    }
}

void Form::MenuBack() {
    Mix_PlayMusic(menuBack, 1);
    if(inSubMenu)
        inSubMenu = false;
    else
        exitApp();
}
