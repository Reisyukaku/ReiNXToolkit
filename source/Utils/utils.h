#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <switch.h>

#ifndef MAX_PATH
#define MAX_PATH 1024
#endif

void gfx_update(void);
void wait_for_button(u64 b);
u64 wait_for_input(void);

int direxists(const char *path);
char *dirname(const char *path);
int mkpath(const char *path, mode_t mode);
size_t fsize(FILE *f);
int fhash(const char *fname, u8 *out);
int fexists(const char *fname);

void bytes2hex(char *dest, u8 *src, u32 src_size);
void hex2bytes(u8 *dest, char *src);

char *strreplace(const char *str, const char *from, const char *to);