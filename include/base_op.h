#ifndef _BASE_OP_H_
#define _BASE_OP_H_

// {sector, address in sector}
int* convert_address_from_number(long address, int sector_size);
int convert_address_to_number(int* data, int sector_size);

// сохраняет данные для таблицы файлов
int table_calculation(int disk_size_bytes, int sector_size);

// wydaje największy rezultat
int IDK_SORT(int *array, unsigned long array_size);

#endif
