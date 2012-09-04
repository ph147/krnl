#include <stdio.h>
#include <inttypes.h>

uint32_t start_of_fat;
uint32_t start_of_data;

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
    uint8_t file_attributes;
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

void fat_print_file(file_t *file)
{
    /*
     * insgesamt 1,0K
     * -rwxr-xr-x 1 root root 34 2012-09-04 14:53 datei1
     * -rwxr-xr-x 1 root root 37 2012-09-04 14:53 datei2
     */
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

void fat_ls_root(FILE *fp)
{
    int i;
    file_t file;

    fseek(fp, start_of_data + 0x20, SEEK_SET);
    for (;;)
    {
        fread(&file, sizeof(file_t), 1, fp);
        if (file.short_file_name[0] == 0x00)
            return;
        fat_print_file(&file);
        fseek(fp, 0x20, SEEK_CUR);
    }
}

FILE *initfat()
{
    bootsector_t boot;
    uint16_t bytes_per_sector;
    uint16_t reserved_sectors;
    uint32_t sectors_per_fat;

    FILE *fp = fopen("fs.fat", "r");
    fread(&boot, sizeof(bootsector_t), 1, fp);

    bytes_per_sector = boot.bios_param.bytes_per_sector;
    reserved_sectors = boot.bios_param.reserved_sectors;
    sectors_per_fat = boot.bios_param.sectors_per_fat;

    start_of_fat = boot.bios_param.bytes_per_sector * reserved_sectors;
    start_of_data = start_of_fat + bytes_per_sector * sectors_per_fat * 2;

    return fp;
}

int main()
{
    FILE *fp = initfat();
    fat_ls_root(fp);
    fclose(fp);
}
