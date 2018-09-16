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
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <regex>
#include <dirent.h>
#include <sys/stat.h>

#include <dirent.h>

#include <sys/stat.h>
#include <sys/statvfs.h>

using namespace std;

class ConfigManager;

typedef void (ConfigManager::*config_method)(vector<string> *);

extern string config_dir;
extern string config_path;

class ConfigManager {
	public:
		~ConfigManager();
		static ConfigManager * getInstance();
		
		int rawProgress = 0;
		int bootProgress = 0;
		bool nogc;
		string splashName;
	private:
		ConfigManager();
		
		void loadProgress(vector<string> * lines);
		void loadFlags(vector<string> * lines);
		void loadSettings(vector<string> * lines);
		
		static ConfigManager * mInstance;
		map<string, config_method> funcMap;
};
