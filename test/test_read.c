#include <stdio.h>
#include "tdfm.h"

int main(int argc, char* argv[]) {
    API* api  = FS_OpenAPI(argv[1]);
    if (!api) {
        printf("open file error\n");
        return 1;
    }

    if (FS_InitAPI(api) == 1) {
        printf("get sectors addresses error\n");
        FS_CloseAPI(api);
        return 1;
    }

    FS_InitTable(api);

    API_FILE_IN_TABLE** files = FS_GetAllFiles(api);
    if (!files) {
        printf("get files error\n");
        FS_CloseAPI(api);
        return 1;
    }
 
    printf("TABLE\n\
        table all sectors: %d\n\
        table all files: %d\n\
        table sector end: %d\n\
        table address end: %d\n", api->table_all_sectors,
                                api->table_all_files,
                                api->table_sector_end,
                                api->table_address_end);


    int filesize = 0;

    for (int i = 0; i < api->table_all_files; i++) {
        API_FILE_IN_TABLE* file = files[i];
        printf("FILE\n\
            dir od: %d\n\
            filename size: %d\n\
            file type: %d\n\
            number_sectors: %d\n\
            sector_address_start: %d\n\
            sector_number_end: %d\n\
            sector_address_end: %d\n", file->dir_id,
                                    file->filename_size,
                                    file->file_type,
                                    file->number_sector,
                                    file->sector_address_start,
                                    file->sector_number_end,
                                    file->sector_address_end);
        char* filename = FS_ReadFileName(api, file);
        printf("Filename: %s\n", filename);
        FS_Free(filename);

        byte* data = FS_ReadFile(api, file, &filesize);
        if (!data) {i++; continue;}
        for (int j = 0; j < filesize; j++) {
            printf("%02X ", data[j]);
        }
        printf("\n%d\n", filesize);
        FS_Free(data);
    }

    FS_ClearALLFilesStruct(files, api);
    FS_CloseAPI(api);

    return 0;
}
