#include <stdio.h>
#include "tdfm.h"

int main(int argc, char* argv[]) {
    if (argc < 2) return 0;

    API* api = FS_OpenAPI(argv[1]);
    if (!api) return 1;

    FS_InitAPI(api);

    int size = api->sectors+1;

    printf("%d\n",size);

    for (int i = 0; i < size; i++) {
        printf("|SEC: %d - %d, ", i, api->sectors_address[i]);
    }
    printf("\n");

    FS_CloseAPI(api);
    return 0;
}
