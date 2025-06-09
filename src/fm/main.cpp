#include "conf.h"
#include <windows.h>
#include "msg.h"
#include "win_utils.h"

#include <cstdlib>

IMAGE* folder_img;
IMAGE* file_img;

API* disk;
API_FILE_IN_TABLE** disk_files;

char disk_name_bfr[512];

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);

    if (argc < 2) {
        return 0;
    }

    API* api = FS_OpenAPI("fm.pak");
    if (!api) {
        return 1;
    }
    char*msgg;
    if (FS_InitAPI(api) != 0) {
        FS_CloseAPI(api);
        return 1;
    }

    FS_InitTable(api);

    API_FILE_IN_TABLE** api_files = FS_GetAllFiles(api);

    const wchar_t CLASS_NAME[] = L"CRELGD_TDFMFM";

    WNDCLASS wc = { };

    wc.lpfnWndProc = TDFMWindow;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.style = CS_DBLCLKS;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"tdfm", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        500, 600, NULL, NULL, hInstance, NULL);

    if (!hwnd) {
        MessageBox(NULL, L"Window isn't started", L"err", 0);
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);

    file_img = IMAGE_INIT("file_min.bmp", &api, &api_files);
    if (!file_img) {
        FS_ClearALLFilesStruct(api_files, api);
        FS_CloseAPI(api);
        return 1;
    }

    folder_img = IMAGE_INIT("folder_min.bmp", &api, &api_files);
    if (!folder_img) {
        FS_Free(file_img->bmpdata);
        delete file_img;
        FS_ClearALLFilesStruct(api_files, api);
        FS_CloseAPI(api);
        return 1;
    }

    // load disk image
    wcstombs(disk_name_bfr, argv[1], 512);

    disk = FS_OpenAPI(disk_name_bfr);
    if (!disk) {
        FS_Free(file_img->bmpdata);
        delete file_img;
        FS_Free(folder_img->bmpdata);
        delete folder_img;
        FS_ClearALLFilesStruct(api_files, api);
        FS_CloseAPI(api);
        return 1;
    }

    if (FS_InitAPI(disk) != 0) {
        FS_CloseAPI(disk);
        FS_Free(file_img->bmpdata);
        delete file_img;
        FS_Free(folder_img->bmpdata);
        delete folder_img;
        FS_ClearALLFilesStruct(api_files, api);
        FS_CloseAPI(api);
        return 1;
    };

    FS_InitTable(disk);

    disk_files = FS_GetAllFiles(disk);

    // win open
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    FS_ClearALLFilesStruct(disk_files, disk);
    FS_CloseAPI(disk);
    FS_Free(folder_img->bmpdata);
    delete folder_img;
    FS_Free(file_img->bmpdata);
    delete file_img;
    FS_ClearALLFilesStruct(api_files, api);
    FS_CloseAPI(api);
    LocalFree(argv);
    return 0;
}