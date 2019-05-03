#pragma once
#include <dirent.h>
#include <errno.h>

#include <switch.h>
#include "unzip.h"

#define ZIP_READBUF_SIZE 16384

namespace Utils {
    
unzFile zip_open(const char *zname);
void zip_close(unzFile zip);

int zip_select_file(unzFile zip, const char *zfname);
int zip_open_selected(unzFile zip);
s64 zip_read_selected(unzFile zip, void *buf, u64 bufsize);
u64 zip_size_selected(unzFile zip);
void zip_close_selected(unzFile zip);

int zip_extract_all(unzFile zip, const char *outdir);
int zip_extract_dir(unzFile zip, const char *zdir, const char *outdir);

};