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

#include "kipmanager.hpp"

std::string smdDir ("/ReiNX/sysmodules");
std::string disDir (smdDir + ".dis");

int enabledStatus(std::string path) {
    if (path == "nogc") {
        FILE *f;
        if (f = fopen("/ReiNX/nogc", "rb")) {
            fclose(f);
            return 1;
        }
        return 0;
    }
    if (path.compare(0, disDir.length(), disDir) == 0) {
        return 0;
    } else if (path.compare(0, smdDir.length(), smdDir) == 0) {
        return 1;
    }
    return -1; // this should never happen
}

int toggleNogc() {
    int status = enabledStatus("nogc");

    if (status == 0) {
        FILE *f = fopen("/ReiNX/nogc", "wb");
        if (f != NULL) {
            fclose(f);
            return 0;
        }
        return -1;
    }
    return remove("/ReiNX/nogc");
}

std::string getName(std::string path) {
    std::size_t pos = path.rfind("/"); // last slash
    if (pos == 0)
        return path;
    else
        return path.substr(pos+1);
}

bool compKip(std::string &path1, std::string &path2) {
    return getName(path1) > getName(path2);
}

void kip::LoadKips(std::vector<std::string> *kips){
    if(!FS::DirExists(smdDir)) FS::MakeDir(smdDir, 0777);
    if(!FS::DirExists(disDir)) FS::MakeDir(disDir, 0777);

    std::vector<std::string> en_name  = FS::EnumDir(smdDir);
    std::vector<std::string> dis_name = FS::EnumDir(disDir);

    for(unsigned int i=0;i<en_name.size();i++) {
        (*kips).push_back(smdDir + "/" + en_name[i]);
    }

    for(unsigned int i=0;i<dis_name.size();i++) {
        (*kips).push_back(disDir + "/" + dis_name[i]);
    }

    (*kips).push_back("nogc"); // add nogc toggle

    std::sort((*kips).rbegin(), (*kips).rend(), compKip);
}

std::string kip::Name(std::string path) {
    std::string name;
    int status = enabledStatus(path);

    // add icon
    if (status == 0) {
        name.append("\uE14C ");
    } else if (status == 1) {
        name.append("\uE14B ");
    } else {
        // how even
        name.append("\uE152 ");
    }

    name.append(getName(path)); // name to icon

    return name;
}

int kip::Toggle(std::string path) {
    if (path == "nogc")
        return toggleNogc();

    int status = enabledStatus(path);
    std::string name = getName(path);

    if (status == 0) {
        std::string new_path (smdDir + "/" + name);
        return rename(path.c_str(), new_path.c_str());
    } else if (status == 1) {
        std::string new_path (disDir + "/" + name);
        return rename(path.c_str(), new_path.c_str());
    }

    return -1; // wut
}