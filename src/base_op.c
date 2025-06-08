#include "base_op.h"
#include <stdlib.h>

int* convert_address_from_number(long address, int sector_size) {
    int* data = (int*)malloc(2 * sizeof(int));
    if (!data) return NULL;

    data[0] = address / sector_size;
    data[1] = address % sector_size;
    
    return data;
}

int convert_address_to_number(int* data, int sector_size) {
    return data[0] * sector_size + data[1];
}

int table_calculation(int disk_size_bytes, int sector_size) {
    if (disk_size_bytes < 1024) return -1;
    if (disk_size_bytes-sector_size < 0) return -1;

    int data = disk_size_bytes-sector_size;

    return data / 5;
}

int IDK_SORT(int *array, unsigned long array_size) {
    int wieksza_cyfra = array[0];

    for (unsigned long i = 0; i < array_size; i++) {
        if (wieksza_cyfra < array[i]) {
            wieksza_cyfra = array[i];
        }
    }

    return wieksza_cyfra;
}
