#include <windows.h>

#pragma comment(lib, "user32.lib")

BOOL WINAPI DllMain(HINSTANCE h, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            MessageBox(NULL, TEXT("DLL Injection Successful!"), TEXT("Hello"), MB_OK);
            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}