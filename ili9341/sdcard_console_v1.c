#include <stdio.h>
#include "pico/stdlib.h"
#include "include/FatFs_SPI/sd_driver/hw_config.h"
#include "include/FatFs_SPI/ff15/source/diskio.h"
#include "include/FatFs_SPI/ff15/source/ff.h"

spi_t *spi;
sd_card_t *sd_card;

void list_files(const TCHAR *path, FIL *output_file)
{
    FRESULT fr;
    DIR dir;
    FILINFO fno;

    fr = f_opendir(&dir, path);
    if (fr != FR_OK)
    {
        f_printf(output_file, "Failed to open directory: %s\n", path);
        return;
    }

    f_printf(output_file, "Listing files and directories in %s:\n", path);
    while (1)
    {
        fr = f_readdir(&dir, &fno);
        if (fr != FR_OK || fno.fname[0] == 0)
        {
            break;
        }

        if (fno.fattrib & AM_DIR)
        {
            f_printf(output_file, "  [DIR] %s\n", fno.fname);
        }
        else
        {
            f_printf(output_file, "  [FILE] %s (size: %lu)\n", fno.fname, fno.fsize);
        }
    }

    f_closedir(&dir);
}


void list_filesPage(const TCHAR *path, FIL *output_file)
{
    FRESULT fr;
    DIR dir;
    FILINFO fno;

    fr = f_opendir(&dir, path);
    if (fr != FR_OK)
    {
        f_printf(output_file, "Failed to open directory: %s\n", path);
        return;
    }

    f_printf(output_file, "Listing files and directories in %s:\n", path);
    while (1)
    {
        fr = f_readdir(&dir, &fno);
        if (fr != FR_OK || fno.fname[0] == 0)
        {
            break;
        }

        if (fno.fattrib & AM_DIR)
        {
            f_printf(output_file, "  [DIR] %s\n", fno.fname);
        }
        else
        {
            f_printf(output_file, "  [FILE] %s (size: %lu)\n", fno.fname, fno.fsize);
        }
    }

    f_closedir(&dir);
}

void my_files()
{
    stdio_init_all();

    // Initialize the pointers
    spi = spi_get_by_num(0);
    sd_card = sd_get_by_num(0);

    FRESULT fr;
    FATFS fs;

    printf("Mounting SD card...\n");
    fr = f_mount(&fs, sd_card->pcName, 1);
    if (fr != FR_OK)
    {
        printf("Failed to mount SD card: %d\n", fr);
        return 1;
    }
    printf("SD card mounted\n");

    FIL output_file;
    const TCHAR *output_file_path = "file_list.txt";
    if (f_open(&output_file, output_file_path, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
    {
        printf("Failed to open output file\n");
        return 1;
    }

    list_files("/", &output_file); // List files and directories at the root of the SD card

    f_close(&output_file);
    printf("File list written to %s\n", output_file_path);

    printf("Unmounting SD card...\n");
    f_mount(NULL, sd_card->pcName, 0);
    printf("SD card unmounted\n");
}
