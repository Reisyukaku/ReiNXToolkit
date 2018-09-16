#include <stdlib.h>
#include <stdio.h>
#include <switch.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

extern "C" {
#include "utils.h"
#include "unzip.h"
}
#include "unzip_utils.hpp"

unzFile  Utils::zip_open(const char *zname) {
  return (unzFile*)unzOpen(zname);
}

void Utils::zip_close(unzFile zip) {
  if (zip) unzClose(zip);
}

int Utils::zip_select_file(unzFile zip, const char *zfname) {
  return (unzLocateFile(zip, zfname, 0) != UNZ_END_OF_LIST_OF_FILE);
}

int Utils::zip_open_selected(unzFile zip) {
  return (unzOpenCurrentFile(zip) == UNZ_OK);
}

s64 Utils::zip_read_selected(unzFile zip, void *buf, u64 bufsize) {
  return unzReadCurrentFile(zip, buf, bufsize);
}

u64 Utils::zip_size_selected(unzFile zip) {
  unz_file_info finfo;
  if (unzGetCurrentFileInfo(zip, &finfo, NULL, 0, NULL, 0, NULL, 0) != UNZ_OK)
    return 0;
  return finfo.uncompressed_size;
}

void Utils::zip_close_selected(unzFile zip) {
  unzCloseCurrentFile(zip);
}

int Utils::zip_extract_all(unzFile zip, const char *outdir) {
  return zip_extract_dir(zip, NULL, outdir);
}

int Utils::zip_extract_dir(unzFile zip, const char *zdir, const char *outdir) {
  char fullpath[MAX_PATH + 1];
  char filepath[MAX_PATH + 1];

  if (!direxists(outdir))
    return -1;

  char *filebuf = (char *)malloc(ZIP_READBUF_SIZE);
  if (!filebuf)
    return -2;

  int ret = unzGoToFirstFile(zip);
  int zdir_len = zdir ? strlen(zdir) : 0;
  unz_file_info finfo;
  while (ret != UNZ_END_OF_LIST_OF_FILE) {
    unzGetCurrentFileInfo(zip, &finfo, filepath, MAX_PATH, NULL, 0, NULL, 0);

    // very crappy way to filter by directory, but works in our case
    if (zdir && strncmp(zdir, dirname(filepath), zdir_len)) {
      ret = unzGoToNextFile(zip);
      continue;
    }

    // remove the first directory in filepath
    char *fpath = filepath + zdir_len;

    snprintf(fullpath, MAX_PATH, "%s/%s", outdir, fpath);
    mkpath(dirname(fullpath), 0777);

    if (!zip_open_selected(zip)) {
      ret = unzGoToNextFile(zip);
      continue;
    }

    int fno = open(fullpath, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    if (fno >= 0) {
      s64 rx = 0, wx = 0;
	  u64 wx_total = 0;
      while (wx_total < finfo.uncompressed_size) {
        rx = zip_read_selected(zip, filebuf, ZIP_READBUF_SIZE);
        if (rx <= 0) break;
        wx = write(fno, filebuf, rx);
        if (wx <= 0) break;
        if (wx != rx) break;
        wx_total += wx;
      }
      close(fno);
    }

    zip_close_selected(zip);
    ret = unzGoToNextFile(zip);
  }

  free(filebuf);

  return 0;
}
