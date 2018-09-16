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
