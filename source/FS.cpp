#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "FS.hpp"

vector<string> FS::EnumDir(string path) {
    DIR *dp;
    vector<string> ret;
    struct dirent *ep;     
    dp = opendir (path.c_str());
    if (dp != NULL){
        while (ep = readdir(dp))
            ret.push_back(ep->d_name);
        closedir(dp);
    }
    return ret;
}

bool FS::DirExists(string dirstr) {
    bool exist = false;
    DIR * dir = opendir(dirstr.c_str());
    if(dir) {
        closedir(dir);
        exist = true;
    }
    return exist;
}

unsigned FS::DeleteDirRecursive(string path) {
	int ret = -1;
	DIR * d = opendir(path.c_str());
	size_t path_len = strlen(path.c_str());
	if(d!=NULL) {
		struct dirent * de;
		ret = 0;
		
		while(!ret && (de=readdir(d))) {
			int status = -1;
			char * buf;
			size_t len;
			
			//skip over . and ..
			if(!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")) {
				continue;
			}
			
			len = path_len + strlen(de->d_name);
			buf = (char *)malloc(len +2);
			
			if(buf!=NULL) {
				struct stat stator;
				snprintf(buf, len, "%s/%s", path.c_str(), de->d_name);
				
				if(!stat(buf, &stator)) {
					if(S_ISDIR(stator.st_mode)) {
						status = DeleteDirRecursive(buf);
					}
					else {
					status = unlink(buf);
					}
				}
				free(buf);
			}
			ret = status;
		}
		closedir(d);
	}
	
	if(!ret) {
		ret = rmdir(path.c_str());
	}
	
	return 0;
}

unsigned FS::MakeDir(string file, unsigned perm) {
    return mkdir(file.c_str(), perm);
}