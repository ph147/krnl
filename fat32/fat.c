#include <stdio.h>
#include <inttypes.h>

#define LOWERCASE(c) (((c)>='A' && (c)<='Z') ? ((c)-'A'+'a') : (c))

#define FAT_READONLY 0x01
#define FAT_HIDDEN 0x02
#define FAT_SYSTEM 0x04
#define FAT_SPECIAL 0x08
#define FAT_SUBDIR 0x10
#define FAT_ARCHIVE 0x20

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
    char current_directory[9];
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
    if (tmp == 0xfffffff)
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
    int i;
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

int fat_chdir(mountpoint_t *mount, file_t *file)
{
}

void fat_ls_file(mountpoint_t *mount, file_t *file)
{
    int hours, minutes, seconds;
    int day, month, year;

    if (file->short_file_name[0] == 0xe5)
        return;

    seconds = (file->created_time & 0x1f) * 2;
    minutes = (file->created_time >> 5) & 0x3f;
    hours = (file->created_time >> 11) & 0x1f;

    day = file->created_date & 0x1f;
    month = (file->created_date >> 5) & 0xf;
    year = 1980 + ((file->created_date >> 9) & 0x7f);

    printf("%c ", (file->attributes & FAT_SUBDIR) ? 'D' : ' ');

    printf("%d-%02d-%02d ", year, month, day);
    printf("%02d:%02d:%02d ", hours, minutes, seconds);
    printf("%d ", file->file_size);
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
    fseek(mount->fp, mount->current_directory_addr + 0x20, SEEK_SET);
    for (;;)
    {
        fread(file, sizeof(file_t), 1, mount->fp);
        if (file->short_file_name[0] == 0x00)
        {
            printf("%s: Datei nicht gefunden.\n", filename);
            return 0;
        }
        if (fat_strcmp(file->short_file_name, filename))
        {
            return 1;
        }
        fseek(mount->fp, 0x20, SEEK_CUR);
    }
}

void fat_cat(mountpoint_t *mount, char *filename)
{
    file_t file;

    if (fat_search_file(mount, filename, &file))
        print_file(mount, &file);
}

void fat_ls_dir(mountpoint_t *mount)
{
    file_t file;

    printf("Contents of %s\n\n", mount->current_directory);
    fseek(mount->fp, mount->current_directory_addr + 0x20, SEEK_SET);
    for (;;)
    {
        fread(&file, sizeof(file_t), 1, mount->fp);
        if (file.short_file_name[0] == 0x00)
            return;
        fat_ls_file(mount, &file);
        fseek(mount->fp, 0x20, SEEK_CUR);
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
}

void umount(mountpoint_t *mount)
{
    fclose(mount->fp);
}

int main()
{
    mountpoint_t fat32;
    mountpoint_t *mount = &fat32;

    mount_fat32(mount, FILENAME);
    fat_ls_dir(mount);
    fat_cat(mount, "long");
    umount(mount);
}
