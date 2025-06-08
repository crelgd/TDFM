#include "conf.h"
#include "win_utils.h"
#include "archive.h"

IMAGE* IMAGE_INIT(const char* texture_name, API** api, API_FILE_IN_TABLE*** files) {
    IMAGE* img = new IMAGE;
    if (!img) return NULL;

    int bmp_size;
    img->bmpdata = getFileData(texture_name, files, api, &bmp_size);
    if (!img->bmpdata) {
        delete img;
        return NULL;
    }

    BITMAPFILEHEADER* file_header = (BITMAPFILEHEADER*)img->bmpdata;
    img->bminfo = (BITMAPINFO*)(img->bmpdata+sizeof(BITMAPFILEHEADER));
    byte* pixelData = img->bmpdata+file_header->bfOffBits;

    HDC hdc = GetDC(NULL);

    img->hbitmap = CreateDIBitmap(hdc, &img->bminfo->bmiHeader, CBM_INIT, pixelData, img->bminfo, 
        DIB_RGB_COLORS);
    ReleaseDC(NULL, hdc);
    if (!img->hbitmap) {
        FS_Free(img->bmpdata);
        delete img;
        return NULL;
    }

    return img;
}

void DRAW_IMAGE(IMAGE* img, HDC hdc, int x, int y, int width, int height) {
    if (img->hbitmap) {
        HDC hdcimg = (HDC)CreateCompatibleDC(hdc);
        HGDIOBJ old = SelectObject(hdcimg, img->hbitmap);

        StretchBlt(hdc, x, y, width, height, hdcimg, 0, 0, img->bminfo->bmiHeader.biWidth, 
            img->bminfo->bmiHeader.biHeight, SRCCOPY);

        SelectObject(hdcimg, old);
        DeleteDC(hdcimg);
    }
}

wchar_t *convertCharArrayToLPCWSTR(const char* charArray)
{
    wchar_t* wString=new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
    return wString;
}

void DRAW_FSELEMENT(HDC hdc, IMAGE* folder_img, IMAGE* file_img, const char* filename, POINT curpos, RECT win, int file_type, int x, int y) {

    if (curpos.y < 34+y && curpos.y > 10+y) {
        HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

        SelectObject(hdc, pen);

        Rectangle(hdc, 10, y+7, win.right-10, y+37);

        DeleteObject(pen);
    }

    if (file_type == 0)
        DRAW_IMAGE(folder_img, hdc, 10+x, 10+y, 24, 24);
    else DRAW_IMAGE(file_img, hdc, 10+x, 10+y, 24, 24);

    HFONT hfont = CreateFont(0, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_CHARACTER_PRECIS, 
        CLIP_CHARACTER_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Yu Gothic Light");

    HFONT old = (HFONT)SelectObject(hdc, hfont);

    int c = strlen(filename);
    wchar_t* filename_converted = convertCharArrayToLPCWSTR(filename);

    TextOut(hdc, 40+x, 12+y, filename_converted, c);
    SelectObject(hdc, old);

    delete[] filename_converted;
    DeleteObject(hfont);
}
