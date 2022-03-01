#include <windows.h>

void inject(DWORD pid, char *path) {
    HANDLE hProcess = NULL, hThread = NULL;
    PWSTR processMemory = NULL;

    PTHREAD_START_ROUTINE remote_start;

    __try {
        SetLastError(NO_ERROR);

        printf("Opening Process...\n");

        hProcess = OpenProcess(
            PROCESS_CREATE_THREAD |
            PROCESS_VM_OPERATION |
            PROCESS_VM_WRITE,
            FALSE,
            pid
        );

        if (hProcess == NULL) {
            printf("ERROR : Cannot open process (%d)\n", pid);
            __leave;
        }

        printf("Allocating memory...\n");

        processMemory = (PWSTR) VirtualAllocEx(hProcess, NULL, strlen(path) + 1, MEM_COMMIT, PAGE_READWRITE);

        if (processMemory == NULL) {
            printf("ERROR [%d] : VirtualAllocEx failed\n", GetLastError());
            __leave;
        }

        printf("Writing...\n");

        if (!WriteProcessMemory(hProcess, processMemory, path, strlen(path) + 1, NULL)) {
            printf("ERROR [%d] : WriteProcessMemory failed\n", GetLastError());
            __leave;
        }

        hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE) LoadLibraryA, processMemory, 0, NULL);

        if (hThread == NULL) {
            printf("ERROR [%d] : CreateRemoteThread failed\n", GetLastError());
            __leave;
        }

        printf("Waiting...\n");
        WaitForSingleObject(hThread, INFINITE);
        printf("Completed!\n");
    }
    __finally {
        if (processMemory) VirtualFreeEx(hProcess, processMemory, 0, MEM_RELEASE);
        if (hProcess) CloseHandle(hProcess);
        if (hThread) CloseHandle(hThread);
    }
}

int main(int argc, char **argv) {
    int pid;

    if (argc != 3) {
        printf("Usage : %s [ProcessId] [DLL]\n", argv[0]);
        return 1;
    }

    pid = atoi(argv[1]);

    inject(pid, argv[2]);

    return 0;
}