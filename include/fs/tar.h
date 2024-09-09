#include <stdint.h>
#include <vfs.h>

#pragma once

typedef struct {
    char filename[100];
    char filemode[8];
    char ouid[8];
    char ogid[8];
    char size[12];
    char l_mod[12];
    char chk[8];
    char type;
    char l_file[100];
    char ustar_i[6];
    char ustar_v[2];
    char own_uname[32];
    char own_group[32];
    char d_major[8];
    char d_minor[8];
    char f_prefix[155];
} tar_header_t;

typedef struct {
    uint8_t* ptr;
} tar_archive_t;

vfs_node_t* init_tar(uint8_t* ptr);