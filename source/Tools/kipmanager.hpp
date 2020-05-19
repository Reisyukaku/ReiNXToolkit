/*
* ReiNX Toolkit
* Copyright (C) 2018  Reisyukaku, neonsea
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

#include <stdio.h>
#include <algorithm>
#include <switch.h>
#include "../FS.hpp"

class KipManager {
public:
    KipManager(std::string path) {
        sysmodDir = path;
        disDir = sysmodDir + ".dis";
    }
    ~KipManager() {
        //
    }
    
    std::string Name(std::string path) {
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
    
    void LoadKips(std::vector<std::string> *kips) {
        if(!FS::DirExists(sysmodDir)) 
            FS::MakeDir(sysmodDir, 0777);
        
        if(!FS::DirExists(disDir)) 
            FS::MakeDir(disDir, 0777);

        std::vector<std::string> en_name  = FS::EnumDir(sysmodDir);
        std::vector<std::string> dis_name = FS::EnumDir(disDir);

        for(unsigned int i=0;i<en_name.size();i++) {
            (*kips).push_back(sysmodDir + "/" + en_name[i]);
        }

        for(unsigned int i=0;i<dis_name.size();i++) {
            (*kips).push_back(disDir + "/" + dis_name[i]);
        }

        std::sort((*kips).rbegin(), (*kips).rend(), []( const std::string lhs, const std::string rhs )
        {
           return lhs < rhs;
        });
    }
    
    int Toggle(std::string path) {
        int status = enabledStatus(path);
        std::string name = getName(path);

        if (status == 0) {
            std::string new_path (sysmodDir + "/" + name);
            return rename(path.c_str(), new_path.c_str());
        } else if (status == 1) {
            std::string new_path (disDir + "/" + name);
            return rename(path.c_str(), new_path.c_str());
        }

        return -1; // wut
    }
    
private:
    int enabledStatus(std::string path) {
        if (path.compare(0, disDir.length(), disDir) == 0) 
            return 0;
        else if (path.compare(0, sysmodDir.length(), sysmodDir) == 0) 
            return 1;
        else 
            return -1;
    }

    std::string getName(std::string path) {
        std::size_t pos = path.rfind("/"); // last slash
        if (pos == 0)
            return path;
        else
            return path.substr(pos+1);
    }
    std::string sysmodDir, disDir;
};