#include "tdfm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

API_FILE_IN_TABLE** getCurrentDirs(val* curDir, API_FILE_IN_TABLE*** files, API** api, val* readed_files) {
    if (!*files) printf("nie znalezc globalnego plika\n");

    val dirs = 0;
    for (int i = 0; i < (*api)->table_all_files; i++) {
        if ((*files)[i]->dir_id == *curDir) dirs++;
    }
    *readed_files = dirs;

    API_FILE_IN_TABLE** output = (API_FILE_IN_TABLE**)malloc(sizeof(API_FILE_IN_TABLE*) * dirs);
    if (!output) return NULL;

    val c = 0;
    for (int i = 0; i < (*api)->table_all_files; i++) {
        if ((*files)[i]->dir_id == *curDir) {
            output[c] = (*files)[i];
            c++;
        }
    }

    return output;
}

int INIT(API** api, API_FILE_IN_TABLE*** files, char* filename) {
    *api = FS_OpenAPI(filename);
    if (!*api) {
        printf("blad przy czytanie plika\n");
        return 1;
    }
    if (FS_InitAPI(*api) != 0) {
        printf("nie udalo sie znalezc api\n");
        return 1;
    }

    FS_InitTable(*api);

    *files = FS_GetAllFiles(*api);

    return 0;
}

void CLOSE(API** api, API_FILE_IN_TABLE*** files) {
    FS_ClearALLFilesStruct(*files, *api);
    FS_CloseAPI(*api);
}

void readCmds(char* bfr, val* curDir, API** api, API_FILE_IN_TABLE*** files) {
    if (memcmp(bfr, "ls", 2) == 0) {
        int readed_files;
        API_FILE_IN_TABLE** f = getCurrentDirs(curDir, files, api, &readed_files);
        if (!f) {
            printf("blad przy czytanie plików\n");
            return;
        }
        for (int i = 0; i < readed_files; i++) {
            char* filname = FS_ReadFileName(*api, f[i]);
            if (!filname) {
                printf("blad przy czytanie tytula plika\n");
                return;
            }
            printf("%s\n", filname);
            FS_Free(filname);
        }
        free(f);
    } 
    else if (memcmp(bfr, "mkdir", 5) == 0) {
        char* filename = "TEST_PLIK.BIN";
        int id = 0;
        int* addresses = NULL;
        if ((*api)->table_all_files >= 1) {
            addresses = (int*)malloc(sizeof(int) * (*api)->table_all_files);
            memset(addresses, 0, sizeof(int) * (*api)->table_all_files);
            for (int i = 0; i < (*api)->table_all_files; i++) {
                addresses[i] = (*files)[i]->dir_id;
            }

            id = IDK_SORT(addresses, (*api)->table_all_files);
        }

        if (FS_CREATEFile(*api, filename, strlen(filename)+1, id, 0, strlen(filename)+2, *files) == 1) {
            printf("blad przy stworzeniu plika\n");
            if ((*api)->table_all_files >= 1) {
                free(addresses);
            }
            return;
        }
        if ((*api)->table_all_files >= 1) free(addresses);


        if ((*api)->table_all_files >= 1)
            FS_ClearALLFilesStruct(*files, *api);

        FS_InitTable(*api);

        *files = FS_GetAllFiles(*api);
        if (!(*files)) {
            printf("wydarzyl sie nieznany blad\n");
            return;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("napisz chocby 1 plik\n");
        return 0;
    }

    API* api = NULL;
    API_FILE_IN_TABLE** files = NULL;
    if (INIT(&api, &files, argv[1]) != 0) return 1;

    char cmd_bfr[256];
    int run = 1;
    val curDir = 0;

    while (run) {
        memset(cmd_bfr, 0, sizeof(cmd_bfr));
        printf("-/%d > ", curDir);
        fgets(cmd_bfr, sizeof(cmd_bfr), stdin);

        readCmds(cmd_bfr, &curDir, &api, &files);
    }

    CLOSE(&api, &files);
    return 0;
}
