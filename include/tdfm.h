// Copyright (c) 2025 crelgd

#ifndef _TDFM_H_
#define _TDFM_H_

#ifdef _TDFM_EXPORTS
#define _TDFM_API __declspec(dllexport)
#else
#define _TDFM_API __declspec(dllimport)
#endif

#include <stdio.h>
#include "base_op.h"

#define SECTOR_SIZE 512
#define TYPE_FILE   0xFF
#define TYPE_DIR    0x00

typedef unsigned char byte;
typedef unsigned int val;

typedef struct FSSTRUCTURE {
    byte* file_mem;
    int sectors;
    int* sectors_address;
    int filesize;

    FILE* file;

    val table_global_size;

    val table_all_sectors;
    val table_all_files;
    val table_sector_end;
    val table_address_end;

    val cur_pos;
} API;

typedef struct {
    val dir_id; // 4
    byte filename_size; // 1
    byte file_type; // 1
    val number_sector; // 4
    val sector_address_start; // 4
    val sector_number_end; // 4
    val sector_address_end; //4
} API_FILE_IN_TABLE;

#ifdef __cplusplus
extern "C" {
#endif

// Открывает файл и заполняет структуру
_TDFM_API API* FS_OpenAPI(const char* filename);
// Очищает все ресурсы у структуры
_TDFM_API void FS_CloseAPI(API* api);

// Выставляет адреса у секторов
_TDFM_API int FS_InitAPI(API* api);

// выдает массив с содержимым сектора указаным в функции
_TDFM_API byte* FS_GetSectorData(API* api, int sector);
// записывает данные из массива в уазаный сектор со смещением не больше 512 байт
_TDFM_API int FS_ToSector(API* api, int sector, int offset, byte* data, int data_array_count);

// просто выполняет Си функцию free()
_TDFM_API void FS_Free(void* obj);

// czyta ilość plików w tabele
_TDFM_API void FS_InitTable(API* api);

// otrzymanie wszystkich plików w tabele
_TDFM_API API_FILE_IN_TABLE** FS_GetAllFiles(API* api);
// wyczyszczenie pamięci od tabelu
_TDFM_API void FS_ClearALLFilesStruct(API_FILE_IN_TABLE** files, API* api);

// czyta tytuł plików
_TDFM_API char* FS_ReadFileName(API* api, API_FILE_IN_TABLE* file);

// czyta dane plików
_TDFM_API byte* FS_ReadFile(API* api, API_FILE_IN_TABLE* file, int* size);

// stworzenie tytuła pliku
_TDFM_API int FS_CREATEFile(API* api, char* filename, byte filename_size, val dir_id, byte file_type, val file_size,
    API_FILE_IN_TABLE** all_files);
// stworzy dane o pliku w tabelu
_TDFM_API int FS_LoadFileTitle(API* api, val dir_id, byte filename_size, byte file_type, val number_sector,
    val sector_address_start, val sector_number_end, val sector_address_end);

// zapisuje dane z zewnętrznego pliku
_TDFM_API int FS_LoadFileToTitle(API* api, API_FILE_IN_TABLE* file, byte* file_data, val data_size_bytes);

// tworzy pusty dysk
_TDFM_API int FS_CreateDISK(char* filename, val filesize_in_bytes);

#ifdef __cplusplus
}
#endif

#endif