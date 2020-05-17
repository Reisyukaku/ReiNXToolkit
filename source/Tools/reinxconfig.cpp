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

#include "reinxconfig.hpp"

int cfg::enabledStatus(std::string file) {
    std::string path = "/ReiNX/"+file;
    FILE *f;
    if (f = fopen(path.c_str(), "rb")) {
        fclose(f);
        return 1;
    }
    return 0;
}

std::string cfg::Name(std::string file) {
    std::string name;
    int status = enabledStatus(file);

    // add icon
    if (status == 0) {
        name.append("\uE14C ");
    } else if (status == 1) {
        name.append("\uE14B ");
    } else {
        // how even
        name.append("\uE152 ");
    }

    name.append(file); // name to icon

    return name;
}

int cfg::Toggle(std::string file) {
    int status = enabledStatus(file);
    std::string path = "/ReiNX/"+file;
    if (status == 0) {
        FILE *f = fopen(path.c_str(), "wb");
        if (f != NULL) {
            fclose(f);
            return 0;
        }
        return -1;
    }
    return remove(path.c_str());
}