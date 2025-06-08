#ifndef _TDFM_MSG_
#define _TDFM_MSG_

#include <windows.h>
#include "win_utils.h"

LRESULT __stdcall TDFMWindow(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

extern IMAGE* folder_img;
extern IMAGE* file_img;

extern API* disk;
extern API_FILE_IN_TABLE** disk_files;

extern char disk_name_bfr[512];

#endif