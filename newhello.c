#include <windows.h>
#include <imagehlp.h>

#pragma comment(lib, "imagehlp.lib")

#define MODULE_NAME "main.exe"

__declspec(dllexport) void newhello(char *name) {
    printf("Bonjour %s!\n", name);
}

BOOL updateIAT(PIMAGE_IMPORT_DESCRIPTOR ipd, HMODULE hm, char *dll, char *proc, PVOID newproc) {
  PIMAGE_THUNK_DATA th, name;
  PIMAGE_IMPORT_BY_NAME ibn;
  DWORD old;

  while (ipd->Name) {
      if (strcmp((PBYTE) hm + ipd->Name, dll) != 0) {
          ipd++;
          continue;
      }

      th = (PIMAGE_THUNK_DATA) ((PBYTE) hm + ipd->FirstThunk);
      name = (PIMAGE_THUNK_DATA) ((PBYTE) hm + ipd->OriginalFirstThunk);

        while (th->u1.Function) {
            if (name->u1.AddressOfData & 0x80000000) {
                name++;
                th++;
                continue;
            }

            ibn = (PIMAGE_IMPORT_BY_NAME) ((PBYTE) hm + name->u1.AddressOfData);

            if (strcmp((char *) (ibn->Name), proc) == 0) {
                VirtualProtect(&th->u1.Function, sizeof(th->u1.Function), PAGE_WRITECOPY, &old);
                CopyMemory(&th->u1.Function, &newproc, sizeof(th->u1.Function));
                VirtualProtect(&th->u1.Function, sizeof(th->u1.Function), old, &old);
                
                return TRUE;
            }

            name++;
            th++;
        }

      ipd++;
  }

  return FALSE;
}

BOOL WINAPI DllMain(HINSTANCE h, DWORD reason, PVOID imp) {
    BOOL ret;
    PIMAGE_IMPORT_DESCRIPTOR ipd;
    HMODULE hm;
    ULONG s;
    char *str;

    if (reason == DLL_PROCESS_ATTACH) {
        hm = GetModuleHandle(MODULE_NAME);

        if (hm == NULL) {
            return FALSE;
        }

        ipd = (PIMAGE_IMPORT_DESCRIPTOR) ImageDirectoryEntryToDataEx(
            hm, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &s, NULL
        );

        if (ipd == NULL) {
            return FALSE;
        }

        ret = updateIAT(ipd, hm, "hello.dll", "hello", newhello);
    }

    return TRUE;
}