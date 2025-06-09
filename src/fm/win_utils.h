#ifndef _WIN_UTILS_H_
#define _WIN_UTILS_H_

#include <windows.h>
#include "tdfm.h"

typedef struct {
    HBITMAP hbitmap;
    BITMAPINFO* bminfo;
    byte* bmpdata;
} IMAGE;

IMAGE* IMAGE_INIT(const char* texture_name, API** api, API_FILE_IN_TABLE*** files);

void DRAW_IMAGE(IMAGE* img, HDC hdc, int x, int y, int width, int height);

void DRAW_FSELEMENT(HDC hdc, IMAGE* folder_img, IMAGE* file_img, const char* filename, 
    POINT curpos, RECT win, int file_type, int x, int y);

wchar_t *convertCharArrayToLPCWSTR(const char* charArray);

#endif
