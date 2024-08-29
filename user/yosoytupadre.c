// yosoytupadre.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    // Probar sys_getppid()
    int ppid = getppid();
    printf("Parent PID: %d\n", ppid);

    // Probar sys_getancestor() con diferentes valores
    for (int i = 0; i < 5; i++) {
        int ancestor_pid = getancestor(i);
        printf("Ancestor %d PID: %d\n", i, ancestor_pid);
    }

    // Finalizar el programa
    exit(0);
}
