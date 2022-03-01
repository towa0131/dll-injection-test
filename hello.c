#include <windows.h>

__declspec(dllexport) void hello(char *name) {
	printf("Hello %s!\n", name);
}

BOOL WINAPI DllMain(HINSTANCE h, DWORD reason, PVOID imp) {
    return TRUE;
}