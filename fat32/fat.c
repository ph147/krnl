#include <stdio.h>
#include <inttypes.h>
#include <string.h>

// TODO touch

#define LOWERCASE(c) (((c)>='A' && (c)<='Z') ? ((c)-'A'+'a') : (c))

#define FAT_READONLY 0x01
#define FAT_HIDDEN 0x02
#define FAT_SYSTEM 0x04
#define FAT_SPECIAL 0x08
#define FAT_SUBDIR 0x10
#define FAT_ARCHIVE 0x20

#define FAT_DELETED 0xe5
#define FAT_LAST_CLUSTER 0xfffffff

#define FILENAME "/home/test/fs.fat"

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
    uint32_t start_of_fat;
    uint32_t start_of_data;
    uint32_t current_directory_addr;
    uint16_t current_directory_cluster;
    char current_directory[128];
    uint32_t bytes_per_cluster;
    bootsector_t boot;
} mountpoint_t;

static int isempty(char *s, size_t n)
{
    size_t i;
    for (i = 0; i < n; ++i)
    {
        if (*s != ' ')
            return 0;
        ++s;
    }
    return 1;
}

static void fat_put_str(char *s)
{
    while (*s != ' ')
    {
        putchar(*s);
        ++s;
    }
}

/* returns 0 on EOF */
static uint32_t next_cluster(mountpoint_t *mount, uint32_t cluster)
{
    uint32_t tmp;
    fseek(mount->fp, mount->start_of_fat + 4*cluster, SEEK_SET);
    fread(&tmp, sizeof(uint32_t), 1, mount->fp);
    if (tmp == FAT_LAST_CLUSTER)
        return 0;
    return tmp;
}

static uint32_t cluster_to_addr(mountpoint_t *mount, uint32_t cluster)
{
    uint32_t offset = (cluster-2) * mount->bytes_per_cluster;
    return mount->start_of_data + offset;
}

static void print_file(mountpoint_t *mount, file_t *file)
{
    int count = 0;
    uint32_t cluster = file->cluster;
    uint32_t last_pos;
    char buf[mount->bytes_per_cluster+1];

    last_pos = cluster_to_addr(mount, cluster);

    while (count < file->file_size)
    {
        fseek(mount->fp, last_pos, SEEK_SET);
        count += fread(buf, sizeof(char),
                mount->bytes_per_cluster, mount->fp);
        buf[mount->bytes_per_cluster] = 0;
        printf("%s", buf);
        cluster = next_cluster(mount, cluster);
        last_pos = cluster_to_addr(mount, cluster);
    }
}

void parent_dir(mountpoint_t *mount)
{
    char *s;
    s = mount->current_directory;
    while (*s)
        ++s;
    s -= 2;
    while (*s != '/')
        --s;
    *(s+1) = '\0';
}

int fat_chdir(mountpoint_t *mount, char *dirname)
{
    file_t dir;

    if (fat_search_file(mount, dirname, &dir))
    {
        if (!(dir.attributes & FAT_SUBDIR))
        {
            printf("%s: Not a directory.\n", dirname);
            return 0;
        }
        if (!dir.cluster)
            dir.cluster = 2;
        mount->current_directory_cluster = dir.cluster;
        mount->current_directory_addr = cluster_to_addr(mount, dir.cluster);
        if (dirname[0] == '.')
        {
            if (dirname[1] != '.')
                return 1;
            parent_dir(mount);
        }
        else
        {
            strcat(mount->current_directory, dirname);
            strcat(mount->current_directory, "/");
        }
        return 1;
    }
}

int valid_file(mountpoint_t *mount, file_t *file)
{
    if ((file->attributes & FAT_READONLY) &&
            (file->attributes & FAT_HIDDEN) &&
            (file->attributes & FAT_SYSTEM) &&
            (file->attributes & FAT_SPECIAL))
        return 0;
    return 1;
}

void fat_ls_file(mountpoint_t *mount, file_t *file)
{
    int hours, minutes, seconds;
    int day, month, year;

    if (file->short_file_name[0] == FAT_DELETED)
        return;
    if (!valid_file(mount, file))
        return;

    seconds = (file->created_time & 0x1f) * 2;
    minutes = (file->created_time >> 5) & 0x3f;
    hours = (file->created_time >> 11) & 0x1f;

    day = file->created_date & 0x1f;
    month = (file->created_date >> 5) & 0xf;
    year = 1980 + ((file->created_date >> 9) & 0x7f);

    printf("%c", (file->attributes & FAT_SUBDIR) ? 'd' : '-');
    printf("r");
    printf("%c", (file->attributes & FAT_READONLY) ? '-' : 'w');
    printf("xr-xr-x ");
    printf("root root ");

    printf("%d-%02d-%02d ", year, month, day);
    printf("%02d:%02d:%02d ", hours, minutes, seconds);
    printf("%3d ", file->file_size);
    fat_put_str(file->short_file_name);
    if (!isempty(file->short_file_extension, 3))
    {
        putchar('.');
        fat_put_str(file->short_file_extension);
    }
    putchar('\n');
}

static int fat_cmp(char c, char d)
{
    return LOWERCASE(c) == LOWERCASE(d);
}

static int fat_strcmp(char *fs_file, char *cmp_file)
{
    while (fat_cmp(*cmp_file, *fs_file))
    {
        ++cmp_file;
        ++fs_file;
    }
    if (*cmp_file == '\0' && *fs_file == ' ')
        return 1;
    return 0;
}

int fat_search_file(mountpoint_t *mount, char *filename, file_t *file)
{
    char *fname;
    char *fext;

    if (filename[0] == '.')
        fname = filename;
    else
    {
        fname = strtok(filename, ".");
        fext = strtok(NULL, ".");
    }
    fseek(mount->fp, mount->current_directory_addr, SEEK_SET);
    for (;;)
    {
        fread(file, sizeof(file_t), 1, mount->fp);
        if (!valid_file(mount, file))
            continue;
        if (file->short_file_name[0] == 0x00)
        {
            printf("%s: Datei nicht gefunden.\n", filename);
            return 0;
        }
        if (fat_strcmp(file->short_file_name, fname))
        {
            if (fext == NULL && file->short_file_extension[0] == ' ')
                return 1;
            if (fat_strcmp(file->short_file_extension, fext))
                return 1;
            return 0;
        }
    }
}

void fat_cat(mountpoint_t *mount, char *filename)
{
    file_t file;

    if (fat_search_file(mount, filename, &file))
    {
        if (file.attributes & FAT_SUBDIR)
            printf("%s/: Is a directory.\n", filename);
        else
            print_file(mount, &file);
    }
}

void fat_ls_dir(mountpoint_t *mount)
{
    int i;
    uint16_t cluster;
    file_t file;

    printf("Contents of %s\n", mount->current_directory);
    for (;;)
    {
        fseek(mount->fp, mount->current_directory_addr, SEEK_SET);
        for (i = 0; i < 16; ++i)
        {
            fread(&file, sizeof(file_t), 1, mount->fp);
            if (file.short_file_name[0] == 0x00)
                continue;
            fat_ls_file(mount, &file);
        }
        cluster = next_cluster(mount, mount->current_directory_cluster);
        if (!cluster)
            break;
        mount->current_directory_cluster = cluster;
        mount->current_directory_addr = cluster_to_addr(mount, cluster);
    }
}

void mount_fat32(mountpoint_t *mount, char *filename)
{
    uint16_t bytes_per_sector;
    uint16_t reserved_sectors;
    uint32_t sectors_per_fat;
    uint32_t sectors_per_cluster;

    FILE *fp = fopen(filename, "r");
    fread(&(mount->boot), sizeof(bootsector_t), 1, fp);

    mount->fp = fp;

    bytes_per_sector = mount->boot.bios_param.bytes_per_sector;
    reserved_sectors = mount->boot.bios_param.reserved_sectors;
    sectors_per_fat = mount->boot.bios_param.sectors_per_fat;
    sectors_per_cluster = mount->boot.bios_param.sectors_per_cluster;

    mount->bytes_per_cluster = bytes_per_sector * sectors_per_cluster;
    mount->start_of_fat = bytes_per_sector * reserved_sectors;
    mount->start_of_data = mount->start_of_fat +
        bytes_per_sector * sectors_per_fat * 2;
    mount->current_directory_addr = mount->start_of_data;
    mount->current_directory[0] = '/';
    mount->current_directory[1] = '\0';
    mount->current_directory_cluster = 2;
}

void umount(mountpoint_t *mount)
{
    fclose(mount->fp);
}

void commandline(mountpoint_t *mount)
{
    char buf[128];
    char *args[2];
    for (;;)
    {
        printf("sh:%s$ ", mount->current_directory);
        fgets(buf, 127, stdin);
        args[0] = strtok(buf, " \n");
        args[1] = strtok(NULL, " \n");

        if (!strcmp(buf, "ls") || !strcmp(buf, "ll"))
            fat_ls_dir(mount);
        else
        {
            if (!args[1])
            {
                printf("%s: Command not found.\n", args[0]);
                continue;
            }
            if (!strcmp(args[0], "cd"))
                fat_chdir(mount, args[1]);
            else if (!strcmp(args[0], "cat"))
                fat_cat(mount, args[1]);
        }
    }
}

int main()
{
    mountpoint_t fat32;
    mountpoint_t *mount = &fat32;

    mount_fat32(mount, FILENAME);

    commandline(mount);

    fat_ls_dir(mount);
    fat_cat(mount, "long");
    fat_chdir(mount, "ordner");
    fat_ls_dir(mount);
    fat_chdir(mount, "..");
    fat_ls_dir(mount);
    umount(mount);
}
