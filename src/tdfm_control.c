#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tdfm.h"

void alf(const char* filename) {
    API* api = FS_OpenAPI(filename);
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

    for (int i = 0; i < api->table_all_files; i++) {
        API_FILE_IN_TABLE* file = files[i];
        char* name = FS_ReadFileName(api, file);

        printf("%d | %s | type: %d | dir: %d\n", i, name, file->file_type, file->dir_id);
        FS_Free(name);
    }

    FS_ClearALLFilesStruct(files, api);
    FS_CloseAPI(api);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("create <disk name> <disk size (in bytes)>\n\
            alf <disk name>\n\
            copy <disk name> <copy file> <dir id> <file type>\n");
        return 0;
    }

    const char* cmd = argv[1];
    
    if (strcmp(cmd, "create") == 0) {
        int file = FS_CreateDISK(argv[2], atoi(argv[3]));
        if (file != 0) {
            printf("File hasnt created!\n");
            return 1;
        }
    } else if (strcmp(cmd, "alf") == 0) {
        alf(argv[2]);
    } else if (strcmp(cmd, "copy") == 0) {
        API* api = FS_OpenAPI(argv[2]);
        if (FS_InitAPI(api) == 1) {
            printf("get sectors addresses error\n");
            FS_CloseAPI(api);
            return 1;
        }
        FS_InitTable(api);

        API_FILE_IN_TABLE** files = NULL;

        if (api->table_all_files >= 1) {
            files = FS_GetAllFiles(api);
            if (!files) {
                printf("get files error\n");
                FS_CloseAPI(api);
                return 1;
            }
        }

        FILE* file = fopen(argv[3], "rb");
        if (!file) {
            printf("read file error\n");
            if (api->table_all_files >= 1)
                FS_ClearALLFilesStruct(files, api);
            FS_CloseAPI(api);
            return 1;
        }

        fseek(file, 0, SEEK_END);
        val filesize = ftell(file);
        fseek(file, 0, SEEK_SET);

        if (FS_CREATEFile(api, argv[3], strlen(argv[3])+1, atoi(argv[4]), atoi(argv[5]), filesize, 0, files) != 0) {
            printf("read file error\n");
            fclose(file);
            if (api->table_all_files >= 1)
                FS_ClearALLFilesStruct(files, api);
            FS_CloseAPI(api);
            return 1;
        }

        if (api->table_all_files >= 1) FS_ClearALLFilesStruct(files, api);
        FS_InitTable(api);
        files = FS_GetAllFiles(api);
        if (!files) {
            printf("get files error\n");
            fclose(file);
            FS_CloseAPI(api);
            return 1;
        }

        byte* content = (byte*)malloc(sizeof(byte) * filesize);
        if (!content) {
            printf("read file error\n");
            fclose(file);
            if (api->table_all_files >= 1)
                FS_ClearALLFilesStruct(files, api);
            FS_CloseAPI(api);
            return 1;
        }

        if (fread(content, sizeof(byte), filesize, file) != filesize) {
            printf("read file error\n");
            free(content);
            fclose(file);
            if (api->table_all_files >= 1)
                FS_ClearALLFilesStruct(files, api);
            FS_CloseAPI(api);
            return 1;
        }
        fclose(file);

        alf(argv[2]);

        char cmd_bfr[256] = {0};
        printf("napisz plik jaki chcesz zmienic: ");
        fgets(cmd_bfr, sizeof(cmd_bfr), stdin);

        int p = atoi(cmd_bfr);

        int readed = FS_LoadFileToTitle(api, files[p], content, filesize);
        if (readed != 0) {
            printf("UNKNOWN ERROR\n");
            free(content);
            if (api->table_all_files >= 1)
                FS_ClearALLFilesStruct(files, api);
            FS_CloseAPI(api);
        }

        free(content);
        if (api->table_all_files >= 1)
            FS_ClearALLFilesStruct(files, api);
        FS_CloseAPI(api);
    }

    return 0;
}
