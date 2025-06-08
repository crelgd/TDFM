#include "conf.h"
#include "msg.h"
#include "archive.h"

val cur_dir = 0;
int cur_split = 0;
val readed_files;
int flag = 0;
POINT curpos;
int click_status = 0;

LRESULT __stdcall TDFMWindow(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    switch (Msg)
    {
    case WM_DESTROY: {
        DeleteObject(folder_img->hbitmap);
        DeleteObject(file_img->hbitmap);
        PostQuitMessage(0);
        return 0;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT win;
        int ww, wh;
        if (GetClientRect(hWnd, &win)) {
            ww = win.right - win.left;
            wh = win.bottom - win.top;
        }

        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBM = CreateCompatibleBitmap(hdc, win.right, win.bottom);
        HBITMAP oldBM = (HBITMAP)SelectObject(memDC, memBM);

        FillRect(memDC, &ps.rcPaint, (HBRUSH (RGB(247, 247, 247))));

        readed_files;
        API_FILE_IN_TABLE** cur_files = getCurrentDirs(&cur_dir, &disk_files, &disk, &readed_files);

        for (int i = 0; i < 15; i++) {
            if ((readed_files - i) == 0) break;
            if (cur_split+i > readed_files-1) break;
            char* filename = FS_ReadFileName(disk, cur_files[i+cur_split]);
            DRAW_FSELEMENT(memDC, folder_img, file_img, filename, curpos, win, cur_files[i+cur_split]->file_type, 20, 40*i+20);
            FS_Free(filename);
        }

        delete[] cur_files;

        RECT status_bar_bg;
        status_bar_bg.bottom = win.bottom;
        status_bar_bg.left = win.left;
        status_bar_bg.right = win.right;
        status_bar_bg.top = win.bottom-20;

        HBRUSH silver = CreateSolidBrush(RGB(237, 237, 237));
        //DRAW_TEXT(hdc, disk_name_bfr, 20, wh-20);

        int c = strlen(disk_name_bfr);
        wchar_t* filename_converted = convertCharArrayToLPCWSTR(disk_name_bfr);

        FillRect(memDC, &status_bar_bg, silver);

        DrawText(memDC, filename_converted, c, &status_bar_bg, DT_CENTER);

        delete[] filename_converted;
        DeleteObject(silver);

        BitBlt(hdc, 0, 0, win.right, win.bottom, memDC, 0, 0, SRCCOPY);

        SelectObject(memDC, oldBM);
        DeleteObject(memBM);
        DeleteDC(memDC);

        EndPaint(hWnd, &ps);
        flag = 1;
        return 0;
    }

    case WM_MOUSEWHEEL: {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (delta > 0)
            cur_split -= 1;
        else if (delta < 0)
            cur_split += 1;

        if (cur_split <= 0) cur_split = 0;
        if (cur_split >= readed_files) cur_split = readed_files-1;
        
        InvalidateRect(hWnd, NULL, TRUE);
        UpdateWindow(hWnd);
        return 0;
    }

    case WM_SIZE: {
        if (flag != 0) {
            InvalidateRect(hWnd, NULL, TRUE);
            UpdateWindow(hWnd);
        }
        return 0;
    }

    case WM_MOUSEMOVE: {
        GetCursorPos(&curpos);
        ScreenToClient(hWnd, &curpos);
        InvalidateRect(hWnd, NULL, FALSE);
        return 0;
    }

    case WM_LBUTTONDBLCLK: {
        click_status = 1;
        InvalidateRect(hWnd, NULL, FALSE);
        return 0;
    }
    }

    return DefWindowProc(hWnd, Msg, wParam, lParam);
}
