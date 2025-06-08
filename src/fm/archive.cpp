#include "archive.h"
#include <string>

int searchFile(const char* filename, API_FILE_IN_TABLE*** files, API** api) {
    if (!*files || !*api) return -1;
    int output = -1;
    for (int i = 0; i < (*api)->table_all_files; i++) {
        char* filename_in_file = FS_ReadFileName(*api, (*files)[i]);
        if (!filename_in_file) {
            output = -1;
            break;
        }

        if (strcmp(filename, filename_in_file) == 0) {
            output = i;
            FS_Free(filename_in_file);
            break;
        }

        FS_Free(filename_in_file);
    }
    return output;
}

byte* getFileData(const char* filename, API_FILE_IN_TABLE*** files, API** api, int* filesize) {
    if (!*files) return NULL;
    if (!*api) return NULL;

    int index = searchFile(filename, files, api);
    if (index == -1) {
        return NULL;
    }

    byte* filedata = FS_ReadFile(*api, (*files)[index], filesize);
    if (!filedata) {
        return NULL;
    }

    return filedata;
}

API_FILE_IN_TABLE** getCurrentDirs(val* curDir, API_FILE_IN_TABLE*** files, API** api, val* readed_files) {
    if (!*files) return NULL;

    val dirs = 0;
    for (int i = 0; i < (*api)->table_all_files; i++) {
        if ((*files)[i]->dir_id == *curDir) dirs++;
    }
    *readed_files = dirs;

    API_FILE_IN_TABLE** output = new API_FILE_IN_TABLE*[dirs];
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
