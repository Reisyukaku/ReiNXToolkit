#include "configmanager.hpp"

ConfigManager * ConfigManager::mInstance = 0;

string config_dir = "sdmc:/ReiNX";
string config_path = config_dir + "/config.ini";

void directory_check(){ // Added to keep user's sdroot clean
	DIR * dir = opendir(config_dir.c_str());
	if (dir)
	{
		closedir(dir); // Upon a sucessful open, close the directory
	}
	else if (ENOENT == errno)
	{
		mkdir(config_dir.c_str(), 0777); //If directory cannot be opened because it doesn't exist create directory
	}
	else
	{
		printf("Unidentified error\n"); // opening directory failed for a different reason other than it not existing, mabe not having read access IDK
	}

}

void ConfigManager::loadProgress(vector<string> * lines) {
	cout << "Progress func" <<endl;
	if(lines == NULL) return;
	if(lines->size() == 0) return;
	
	regex r("(.*)=(.*)");
	for(string line : *lines) {
		smatch m;
		if(regex_match(line, m, r)) {
			if(m[1] == "rawnand") {
				rawProgress = stoi(m[2]);
				cout << rawProgress <<endl;
			}
			else if(m[1] == "boot0") {
				bootProgress = stoi(m[2]);
				cout << bootProgress << endl;
			}
		}
	}
}
void ConfigManager::loadFlags(vector<string> * lines) {
	cout << "Flags func" <<endl;
}
void ConfigManager::loadSettings(vector<string> * lines) {
	cout << "Settings func" <<endl;
}


ConfigManager::ConfigManager() {
	funcMap["[Progress]"] = &ConfigManager::loadProgress;
	funcMap["[Flags]"] = &ConfigManager::loadFlags;
	funcMap["[Settings]"] = &ConfigManager::loadSettings;
	
	ifstream configfile;
	directory_check();
	configfile.open(config_path);
	int linecount = 0;
	regex r("\\[(.*)\\]");
	
	vector<string> lines;
	map<string, vector<string> *> headerLines;
	vector<int> headers;
	vector<string> * tmpLines = new vector<string>();
	string oldline = "";
	for(string line; getline(configfile, line); ) {
		if(line == "")
			continue;
		lines.push_back(line);
		smatch m;
		if(regex_match(line, m, r)) {
			headers.push_back(linecount);
			if(oldline == "") {
				delete tmpLines;
			}
			else {
				headerLines[oldline] = tmpLines;
			}
			oldline = line;
			tmpLines = new vector<string>();
		}
		else tmpLines->push_back(line);
		linecount++;
	}
	headerLines[oldline] = tmpLines;
	for(const auto &p: headerLines) {
		auto iter = funcMap.find(p.first);
		if(iter == funcMap.end()) {
			cout << "Header:"<< p.first << " not found" <<endl;
		}
		else {
			(this->*iter->second)(p.second);
		}
	}
} 

ConfigManager::~ConfigManager() {
	directory_check();
	ofstream configfile;
	configfile.open(config_path);
	configfile << "[Progress]" << endl;
	configfile << "rawnand=" << rawProgress << endl;
	configfile << "boot0=" << bootProgress << endl;
	configfile << "[Flags]" << endl;
	configfile << "[Settings]" << endl;
	configfile.close();
}

ConfigManager * ConfigManager::getInstance() {
	if(mInstance==0)
		mInstance=new ConfigManager();
	return mInstance;
}