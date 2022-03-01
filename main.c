#include <windows.h>

extern __declspec(dllimport) void hello(char *name);

int main(int argc,char **argv) {
    char *name = "towa0131";

    while (1) {
        hello(name);
        Sleep(1000);
    }
}