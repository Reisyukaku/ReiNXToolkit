#include <switch.h>
#include <stdio.h>
#include <stdint.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#include "utils.h"
#include "sha256.h"

void gfx_update(void) {
}

void wait_for_button(u64 b) {
  while (appletMainLoop()) {
    hidScanInput();
    u64 kdown = hidKeysDown(CONTROLLER_P1_AUTO);
    if (kdown & b) return;
    gfx_update();
  }
}

u64 wait_for_input(void) {
  while (appletMainLoop()) {
    hidScanInput();
    u64 kdown = hidKeysDown(CONTROLLER_P1_AUTO);
    if (kdown) return kdown;
    gfx_update();
  }
  return 0;
}

int direxists(const char *path) {
  DIR* dir = opendir(path);
  if (!dir) return 0;
  closedir(dir);
  return 1;
}

char *dirname(const char *path) {
  static char outbuf[2][MAX_PATH + 1];
  static int idx = 1;
  idx = !idx;
  strncpy(outbuf[idx], path, MAX_PATH);
  char *slash = strrchr(outbuf[idx], '/');
  if (!slash) return ".";
  *slash = '\0';
  return outbuf[idx];
}

// mkpath() code stolen from J. Leffler

static int do_mkdir(const char *path, mode_t mode) {
  struct stat st;
  int status = 0;

  if (stat(path, &st) != 0) {
    /* Directory does not exist. EEXIST for race condition */
    if (mkdir(path, mode) != 0 && errno != EEXIST)
      status = -1;
  } else if (!S_ISDIR(st.st_mode)) {
    errno = ENOTDIR;
    status = -1;
  }

  return status;
}

/**
** mkpath - ensure all directories in path exist
** Algorithm takes the pessimistic view and works top-down to ensure
** each directory in path exists, rather than optimistically creating
** the last element and working backwards.
*/
int mkpath(const char *path, mode_t mode) {
  static char copypath[MAX_PATH];
  strncpy(copypath, path, MAX_PATH);

  int status = 0;
  char *pp = copypath;
  char *sp;
  while (status == 0 && (sp = strchr(pp, '/')) != 0) {
    if (sp != pp) {
      /* Neither root nor double slash in path */
      *sp = '\0';
      status = do_mkdir(copypath, mode);
      *sp = '/';
    }
    pp = sp + 1;
  }

  if (status == 0)
    status = do_mkdir(path, mode);

  return status;
}

size_t fsize(FILE *f) {
  size_t old = ftell(f);
  fseek(f, 0, SEEK_END);
  size_t sz = ftell(f);
  fseek(f, old, SEEK_SET);
  return sz;
}

int fhash(const char *fname, u8 *out) {
  SHA256_CTX sha;

  FILE *f = fopen(fname, "rb");
  if (!f) return -1;

  size_t sz = fsize(f);
  if (!sz) { fclose(f); return -2; }

  u8 *buf = malloc(sz);
  if (!buf) { fclose(f); return -3; }

  // fuck it, load the whole file in one go
  fread(buf, sz, 1, f);
  fclose(f);

  sha256_init(&sha);
  sha256_update(&sha, buf, sz);
  sha256_final(&sha, out);

  free(buf);

  return 0;
}

int fexists(const char *fname) {
  FILE *f = fopen(fname, "rb");
  int exists = !!f;
  if (f) fclose(f);
  return exists;
}

static const char hex[] = "0123456789abcdef";

void bytes2hex(char *dest, u8 *src, u32 src_size) {
  for (u8 *p = src; p < src + src_size; ++p) {
    *(dest++) = hex[*p >> 4];
    *(dest++) = hex[*p & 0x0F];
  }
  *dest = '\0';
}

static inline u8 hexord(char x) {
  if (x >= '0' && x <= '9')
    return x - '0';
  return (0x0A + (x - 'a')) & 0x0F;
}

void hex2bytes(u8 *dest, char *src) {
  if (!src) return;
  u32 src_size = strlen(src);
  if (!src_size || (src_size & 1)) return;
  u32 dest_size = src_size >> 1;

  for (char *p = src; p < src + src_size; p += 2) {
    u8 hi = hexord(p[0]);
    u8 lo = hexord(p[1]);
    *(dest++) = (hi << 4) | lo;
  }
}

// from creativeandcritical.net
char *strreplace(const char *str, const char *from, const char *to) {
  /* Adjust each of the below values to suit your needs. */

  /* Increment positions cache size initially by this number. */
  size_t cache_sz_inc = 16;
  /* Thereafter, each time capacity needs to be increased,
   * multiply the increment by this factor. */
  const size_t cache_sz_inc_factor = 3;
  /* But never increment capacity by more than this number. */
  const size_t cache_sz_inc_max = 1048576;

  char *pret, *ret = NULL;
  const char *pstr2, *pstr = str;
  size_t i, count = 0;
  #if (__STDC_VERSION__ >= 199901L)
  uintptr_t *pos_cache_tmp, *pos_cache = NULL;
  #else
  ptrdiff_t *pos_cache_tmp, *pos_cache = NULL;
  #endif
  size_t cache_sz = 0;
  size_t cpylen, orglen, retlen, tolen, fromlen = strlen(from);

  /* Find all matches and cache their positions. */
  while ((pstr2 = strstr(pstr, from)) != NULL) {
    count++;

    /* Increase the cache size when necessary. */
    if (cache_sz < count) {
      cache_sz += cache_sz_inc;
      pos_cache_tmp = realloc(pos_cache, sizeof(*pos_cache) * cache_sz);
      if (pos_cache_tmp == NULL) {
        goto end_repl_str;
      } else pos_cache = pos_cache_tmp;
      cache_sz_inc *= cache_sz_inc_factor;
      if (cache_sz_inc > cache_sz_inc_max) {
        cache_sz_inc = cache_sz_inc_max;
      }
    }

    pos_cache[count-1] = pstr2 - str;
    pstr = pstr2 + fromlen;
  }

  orglen = pstr - str + strlen(pstr);

  /* Allocate memory for the post-replacement string. */
  if (count > 0) {
    tolen = strlen(to);
    retlen = orglen + (tolen - fromlen) * count;
  } else  retlen = orglen;
  ret = malloc(retlen + 1);
  if (ret == NULL) {
    goto end_repl_str;
  }

  if (count == 0) {
    /* If no matches, then just duplicate the string. */
    strcpy(ret, str);
  } else {
    /* Otherwise, duplicate the string whilst performing
     * the replacements using the position cache. */
    pret = ret;
    memcpy(pret, str, pos_cache[0]);
    pret += pos_cache[0];
    for (i = 0; i < count; i++) {
      memcpy(pret, to, tolen);
      pret += tolen;
      pstr = str + pos_cache[i] + fromlen;
      cpylen = (i == count-1 ? orglen : pos_cache[i+1]) - pos_cache[i] - fromlen;
      memcpy(pret, pstr, cpylen);
      pret += cpylen;
    }
    ret[retlen] = '\0';
  }

end_repl_str:
  /* Free the cache and return the post-replacement string,
   * which will be NULL in the event of an error. */
  free(pos_cache);
  return ret;
}
