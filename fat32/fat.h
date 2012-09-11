#ifndef _FAT_H
#define _FAT_H

#include <inttypes.h>

#define DEBUG printf

#define COLOR_DIR "\e[1;34m"
#define COLOR_FILE "\e[1;32m"
#define COLOR_NORMAL "\e[0m"

#define LOWERCASE(c) (((c)>='A' && (c)<='Z') ? ((c)-'A'+'a') : (c))
#define UPPERCASE(c) (((c)>='a' && (c)<='z') ? ((c)-'a'+'A') : (c))

#define FAT_READONLY 0x01
#define FAT_HIDDEN 0x02
#define FAT_SYSTEM 0x04
#define FAT_SPECIAL 0x08
#define FAT_SUBDIR 0x10
#define FAT_ARCHIVE 0x20

#define FAT_DELETED 0xe5
#define FAT_LAST_CLUSTER 0xfffffff
#define FAT_EMPTY 0x0

extern const char *fat_legal_chars;

typedef struct
{
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t number_of_fats;
    uint16_t NA_max_roots;
    uint16_t _NA_total_logical_sectors;
    uint8_t media_descriptor;
    uint16_t NA_logical_sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t heads;
    uint32_t hidden_sectors;
    uint32_t sectors_in_partition;
    uint32_t sectors_per_fat;
    uint16_t mirror_flags;
    uint16_t filesystem_version;
    uint32_t first_cluster_of_root;
    uint16_t fs_info_sector;
    uint16_t backup_boot_sector;
    uint8_t reserved[12];
    uint8_t logical_drive_number;
    uint8_t reserved_current_head;
    uint8_t extended_signature;
    uint32_t serial_number_of_partition;
    uint8_t volume_label[11];
    uint8_t fs_type[8];
} __attribute__ ((packed)) bios_param_t;

typedef struct
{
    uint16_t jmp_code;
    uint8_t nop;
    uint8_t oem_name[8];
    bios_param_t bios_param;
} __attribute__ ((packed)) bootsector_t;

typedef struct
{
    uint8_t short_file_name[8];
    uint8_t short_file_extension[3];
    uint8_t attributes;
    uint8_t nt;
    uint8_t millisecs;
    uint16_t created_time;
    uint16_t created_date;
    uint16_t last_accessed_date;
    uint16_t extended_attr;
    uint16_t time;
    uint16_t date;
    uint16_t cluster;
    uint32_t file_size;
} __attribute__ ((packed)) file_t;

typedef struct
{
    FILE *fp;
    uint32_t length_of_fat;
    uint32_t start_of_fat;
    uint32_t start_of_data;
    uint32_t current_directory_addr;
    uint16_t current_directory_cluster;
    char current_directory[128];
    uint32_t bytes_per_cluster;
    bootsector_t boot;
} mountpoint_t;

static int isempty(uint8_t *s, size_t n);
static void fat_put_str(uint8_t *s);
static uint32_t next_cluster(mountpoint_t *mount, uint32_t cluster);
static uint32_t is_last_cluster(uint32_t cluster);
static uint32_t cluster_to_addr(mountpoint_t *mount, uint32_t cluster);
static void print_file(mountpoint_t *mount, file_t *file);
static int valid_file(file_t *file);
static int valid_filename(char *s, int len, uint8_t att);
static int fat_cmp(char c, char d);
static int fat_strncmp(char *fs_file, char *cmp_file, int len, int max);
static void split_filename(char *filename, int *fname, int *fext);
static uint16_t next_free_cluster(mountpoint_t *mount, uint16_t last_cluster);
static void fat_write_to_fat(mountpoint_t *mount, uint16_t cluster, uint32_t value);
static int fat_create_file(mountpoint_t *mount, char *filename, uint8_t att, uint16_t cluster);
static void next_dir_entry(mountpoint_t *mount);
static void uppercase(char *s, int len);

void parent_dir(mountpoint_t *mount);
int fat_chdir(mountpoint_t *mount, char *dirname);
void fat_ls_file(file_t *file);
int fat_search_file(mountpoint_t *mount, char *filename, file_t *file);
void fat_cat(mountpoint_t *mount, char *filename);
void fat_rm(mountpoint_t *mount, char *filename);
// TODO void fat_mv(mountpoint_t *mount, char *source, char *destination);
int fat_mkdir(mountpoint_t *mount, char *filename);
void fat_touch(mountpoint_t *mount, char *filename);
void fat_ls_dir(mountpoint_t *mount);
void mount_fat32(mountpoint_t *mount, char *filename);
void umount(mountpoint_t *mount);
void commandline(mountpoint_t *mount);


#endif
