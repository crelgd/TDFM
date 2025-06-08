#ifndef TDFM_FM_ARCHIVE
#define TDFM_FM_ARCHIVE

#include "tdfm.h"

byte* getFileData(const char* filename, API_FILE_IN_TABLE*** files, API** api, int* filesize);

API_FILE_IN_TABLE** getCurrentDirs(val* curDir, API_FILE_IN_TABLE*** files, API** api, val* readed_files);

#endif