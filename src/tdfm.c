#include "tdfm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

API* FS_OpenAPI(const char* filename) {
    API* api = (API*)malloc(sizeof(API));
    if (!api) return NULL;

    if (filename) {
        api->file = fopen(filename, "rb+");
        if (!api->file) return NULL;

        // выставление курсора, для получения количеста байтов
        fseek(api->file, 0, SEEK_END);
        int filesize = ftell(api->file);
        fseek(api->file, 0, SEEK_SET);

        // сохранение размера файла в глобальную переменную
        api->filesize = filesize;

        // получение количеста секторов и сохранение в глобальную переменную
        int* fdata = convert_address_from_number(filesize, SECTOR_SIZE);
        api->sectors = fdata[0];
        free(fdata);

        api->file_mem = (byte*)malloc(filesize * sizeof(byte));
        if (!api->file_mem) {
            fclose(api->file);
            free(api);
            return NULL;
        }

        if (fread(api->file_mem, sizeof(byte), filesize, api->file) != filesize) {
            fclose(api->file);
            free(api->file_mem);
            free(api);
            return NULL;
        }
    }

    fseek(api->file, 0, SEEK_SET);

    api->sectors_address = NULL;
    return api;
}

void FS_CloseAPI(API* api) {
    if (api->file) fclose(api->file);
    if (api->sectors_address)
        free(api->sectors_address);
    if (api->file_mem)
        free(api->file_mem);
    free(api);
}

int FS_InitAPI(API* api) {
    if (!api) return 1;

    int sectors = api->sectors+1;

    api->sectors_address = (int*)malloc(sectors * sizeof(int));
    if (!api->sectors_address) return 1;

    api->sectors_address[0] = 0x00;

    int pos_in_sector = 0;
    // 1 - потому что нулевой сектор уже указан
    int sector_count = 1;

    for (int i = 0; i < api->filesize; i++) {
        // если подсчитаных байтов равно "SECTOR_SIZE"
        if (pos_in_sector >= SECTOR_SIZE) {
            // запись текущей позиции в массив
            api->sectors_address[sector_count] = i;
            sector_count++;
            // сброс подсчитаных байтов
            pos_in_sector = 0;
        }

        pos_in_sector++;
    }

    return 0;
}

byte* FS_GetSectorData(API* api, int sector) {
    if (!api) return NULL;
    // если указаный сектор больше максимального значения сектора
    if (sector > (api->sectors+1)) return NULL;

    byte* sector_data = (byte*)malloc(SECTOR_SIZE * sizeof(byte));
    if (!sector_data) return NULL;

    // получение адреса сектора для глобального массива
    int address = api->sectors_address[sector];

    for (int i = 0; i < SECTOR_SIZE; i++) {
        sector_data[i] = api->file_mem[address+i];
    }

    return sector_data;
}

int FS_ToSector(API* api, int sector, int offset, byte* data, int data_array_count) {
    if (!api) return 1;
    if (!data) return 1;
    // если смещение больше чем размер сектора
    if (offset >= SECTOR_SIZE) return 1;
    // если данные со смещением больше чем размер сектора
    if (offset+data_array_count >= SECTOR_SIZE) return 1;

    // получение адреса сектора со смещением
    int address = api->sectors_address[sector] + offset;
    // создание переменной для цикла
    int counter = offset;
    int data_pos = 0;

    for (counter < SECTOR_SIZE; counter++;) {
        if (data_pos >= data_array_count)
            break;

        // записывание в массив по адрессу (адрес + позиция)
        api->file_mem[address+data_pos] = data[data_pos];
        
        data_pos++;
    }

    return 0;
}

void FS_Free(void* obj) {
    free(obj);
}

val read4bytes(byte array[4], byte* array_global, val* cur_pos) {
    for (int i = 0; i < 4; i++) {
        array[i] = array_global[*cur_pos];
        (*cur_pos)++;
    }

    return (array[0] << 24) | (array[1] << 16) | (array[2] << 8) | array[3];
}

void FS_InitTable(API* api) {
    byte array_for_bytes[4] = {0};

    val cur_data = SECTOR_SIZE;

    // czytanie tabely dla otrzymania adresów

    val* targets[] = {
        &api->table_all_sectors,
        &api->table_all_files,
        &api->table_sector_end,
        &api->table_address_end
    };

    for (int i = 0; i < 4; i++) {
        *targets[i] = read4bytes(array_for_bytes, api->file_mem, &cur_data);
    }

    int timely_data[2] = {api->table_sector_end, api->table_address_end};
    val end_table = convert_address_to_number(timely_data, SECTOR_SIZE)-SECTOR_SIZE; 
    api->table_global_size = end_table;

    api->cur_pos = cur_data;
}

void YYReadFile(API* api, API_FILE_IN_TABLE* data) {
    if (api->cur_pos >= api->filesize) {
        return;
    }
    byte array[4];
    data->dir_id = read4bytes(array, api->file_mem, &api->cur_pos);
    data->filename_size = api->file_mem[api->cur_pos++];
    data->file_type = api->file_mem[api->cur_pos++];
    data->number_sector = read4bytes(array, api->file_mem, &api->cur_pos);
    data->sector_address_start = read4bytes(array, api->file_mem, &api->cur_pos);
    data->sector_number_end = read4bytes(array, api->file_mem, &api->cur_pos);
    data->sector_address_end = read4bytes(array, api->file_mem, &api->cur_pos);
}

API_FILE_IN_TABLE** FS_GetAllFiles(API* api) {
    if (api->table_all_files == 0) {
        return NULL;
    }

    API_FILE_IN_TABLE** _File = (API_FILE_IN_TABLE**)malloc(api->table_all_files * sizeof(API_FILE_IN_TABLE*));
    if (!_File) return NULL;

    for (int i = 0; i < api->table_all_files; i++) {
        _File[i] = (API_FILE_IN_TABLE*)malloc(sizeof(API_FILE_IN_TABLE));
        if (!_File[i]) {
            continue;
        }

        YYReadFile(api, _File[i]);
    }

    return _File;
}

void FS_ClearALLFilesStruct(API_FILE_IN_TABLE** files, API* api) {
    for (int i = 0; i < api->table_all_files; i++) {
        FS_Free(files[i]);
    }
    FS_Free(files);
}

char* FS_ReadFileName(API* api, API_FILE_IN_TABLE* file) {
    int data_to_convert[] = {file->number_sector, file->sector_address_start};
    int converted = convert_address_to_number(data_to_convert, SECTOR_SIZE);
    char* filename = (char*)malloc(sizeof(char) * file->filename_size);
    for (int i = 0; i < file->filename_size; i++) {
        filename[i] = api->file_mem[converted+i];
    }

    return filename;
}

byte* FS_ReadFile(API* api, API_FILE_IN_TABLE* file, int* size) {
    if (file->file_type == TYPE_DIR) return NULL;

    int d[2] = {file->number_sector, file->sector_address_start};
    int data_start = convert_address_to_number(d, SECTOR_SIZE);

    val data_adr = data_start + file->filename_size;

    d[0] = file->sector_number_end; d[1] = file->sector_address_end;
    int data_end = convert_address_to_number(d, SECTOR_SIZE);

    val data_size = data_end - data_adr;
    byte* output_array = (byte*)malloc(sizeof(byte) * data_size);
    if (!output_array) return NULL;

    if (size) *size = data_size;

    memcpy(output_array, &api->file_mem[data_adr], sizeof(byte) * data_size);

    return output_array;
}

void write4bytes(val value, byte* array_global, val* cur_pos) {
    array_global[*cur_pos + 0] = (value >> 24) & 0xFF;
    array_global[*cur_pos + 1] = (value >> 16) & 0xFF;
    array_global[*cur_pos + 2] = (value >> 8)  & 0xFF;
    array_global[*cur_pos + 3] = value & 0xFF;

    *cur_pos += 4;
}

int check_table_addresses(API* api, API_FILE_IN_TABLE** all_files, int end_table) {
    int d = end_table;

    if (api->table_all_files >= 1) {
        int* addresses = (int*)malloc(sizeof(int) * api->table_all_files);
        if (!addresses) return -1;
        int data[2] = {0};

        for (int i = 0; i < api->table_all_files; i++) {
            data[0] = all_files[i]->sector_number_end;
            data[1] = all_files[i]->sector_address_end;

            addresses[i] = convert_address_to_number(data, SECTOR_SIZE);
        }

        d = IDK_SORT(addresses, api->table_all_files);

        free(addresses);
    }

    return d;
}

int is_empty_file(API_FILE_IN_TABLE* f) {
    return f->dir_id == 0 &&
           f->filename_size == 0 &&
           f->file_type == 0 &&
           f->number_sector == 0 &&
           f->sector_address_start == 0 &&
           f->sector_number_end == 0 &&
           f->sector_address_end == 0;
}

int FS_CREATEFile(API* api, char* filename, byte filename_size, val dir_id, byte file_type, val file_size,
    API_FILE_IN_TABLE** all_files)
{
    API_FILE_IN_TABLE file = {-1};

    val idk = api->table_global_size / 22;
    api->cur_pos = SECTOR_SIZE+16;

    byte cmpdata[22] = { 0 };

    int FLAG = 0;

    if (api->table_all_files >= 1) {
        for (int i = 0; i < idk; i++) {
            YYReadFile(api, &file);
            if (is_empty_file(&file)) {
                FLAG = 1;
                break;
            }
        }
        api->cur_pos -= 22;
    } else FLAG = 1;

    if (FLAG) {
        int data[2] = {api->table_sector_end, api->table_address_end};
        int end_table = convert_address_to_number(data, SECTOR_SIZE);
        int second_start_data = check_table_addresses(api, all_files, end_table);
 
        int start_data = (second_start_data < end_table) ? end_table+1 :
                                                           second_start_data+1;


        int* start_data_totable = convert_address_from_number(start_data, SECTOR_SIZE);
        int* end_data_totable = convert_address_from_number(start_data+file_size+filename_size, SECTOR_SIZE);

        FS_LoadFileTitle(api, dir_id, filename_size, file_type, start_data_totable[0], start_data_totable[1], end_data_totable[0], 
            end_data_totable[1]);

        free(start_data_totable);
        free(end_data_totable);

        memcpy(api->file_mem+start_data, filename, filename_size);

        int kostyl = api->table_all_files+1;
        val pos = SECTOR_SIZE+4;
        write4bytes(kostyl, api->file_mem, &pos);

        fseek(api->file, 0, SEEK_SET);
        fwrite(api->file_mem, sizeof(byte), api->filesize, api->file);
        fseek(api->file, 0, SEEK_SET);
    }

    if (!FLAG) return 1;

    return 0;
}

int FS_LoadFileTitle(API* api, val dir_id, byte filename_size, byte file_type, val number_sector,
    val sector_address_start, val sector_number_end, val sector_address_end) 
{
    write4bytes(dir_id, api->file_mem, &api->cur_pos);
    api->file_mem[api->cur_pos++] = filename_size;
    api->file_mem[api->cur_pos++] = file_type;
    write4bytes(number_sector, api->file_mem, &api->cur_pos);
    write4bytes(sector_address_start, api->file_mem, &api->cur_pos);
    write4bytes(sector_number_end, api->file_mem, &api->cur_pos);
    write4bytes(sector_address_end, api->file_mem, &api->cur_pos);

    return 0;
}

int FS_LoadFileToTitle(API* api, API_FILE_IN_TABLE* file, byte* file_data, val data_size_bytes) {
    if (!file) return 1;
    if (!file_data) return 1;
    if (data_size_bytes < 0) return 1;

    // czytanie początku pliku
    int timely[2] = {file->number_sector, file->sector_address_start};
    int start_data = convert_address_to_number(timely, SECTOR_SIZE)+file->filename_size;
    // czytanie końca pliku
    timely[0] = file->sector_number_end; timely[1] = file->sector_address_end;
    int end_data = convert_address_to_number(timely, SECTOR_SIZE);

    if ((start_data+data_size_bytes) > api->filesize) return 1;

    val file_size = end_data - start_data;
    if (data_size_bytes < file_size) return 1;

    memcpy(api->file_mem+start_data, file_data, data_size_bytes);
    fwrite(api->file_mem, sizeof(byte), api->filesize, api->file);

    return 0;
}

int FS_CreateDISK(char* filename, val filesize_in_bytes) {
    if (filesize_in_bytes < 1024) return 1;
    FILE* file = fopen(filename, "wb");
    if (!file) return 1;

    byte buffer[1024] = {0};

    int idk = filesize_in_bytes/1024;
    for (int i = 0; i < idk; i++) {
        fwrite(buffer, sizeof(byte), 1024, file);
    }

    int table_size = table_calculation(filesize_in_bytes, SECTOR_SIZE);
    if (table_size == -1) {
        fclose(file);
        return 1;
    }

    int* data = convert_address_from_number(table_size, SECTOR_SIZE);
    if (!data) return 1;

    byte table_array[16] = {0};

    val pos = 0;

    write4bytes(data[0], table_array, &pos);
    write4bytes(0, table_array, &pos);
    write4bytes(data[0], table_array, &pos);
    write4bytes(data[1], table_array, &pos);

    free(data);

    fseek(file, SECTOR_SIZE, SEEK_SET);
    fwrite(table_array, sizeof(byte), 16, file);

    fclose(file);

    return 0;
}
