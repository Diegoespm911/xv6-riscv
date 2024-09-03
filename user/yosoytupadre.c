#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Función para probar getppid
void test_getppid() {
    int pid, ppid;

    // Crear un nuevo proceso
    pid = fork();
    
    if (pid == 0) { // En el proceso hijo
        ppid = getppid();
        printf("En proceso hijo (PID: %d), PID del padre: %d\n", getpid(), ppid);
        exit(0);
    } else if (pid > 0) { // En el proceso padre
        wait(0); // Espera a que el proceso hijo termine
        printf("En proceso padre (PID: %d), PID del hijo: %d\n", getpid(), pid);
    } else {
        // Manejo de error si falla fork
        printf("Fallo en fork\n");
        exit(1);
    }
}

// Función para probar getancestor
void test_getancestor(int levels) {
    for (int i = 0; i <= levels; i++) {
        int ancestor_pid = getancestor(i);
        if (ancestor_pid != -1) {
            printf("Getancestor %d: PID %d\n", i, ancestor_pid);
        } else {
            printf("Getancestor %d: PID -1 No se encuentra ancestor\n", i);
            break;  // Detener la búsqueda cuando no hay más ancestros
        }
    }
}

int main() {
    int ancestor_levels = 50;
    int pid;

    // Proceso para la prueba de getppid
    pid = fork();
    if (pid == 0) {
        // Proceso hijo para la prueba de getppid
        printf("Ejecutando prueba de getppid...\n");
        test_getppid();
        exit(0);
    } else if (pid > 0) {
        // Proceso principal espera a que termine la prueba de getppid
        wait(0);
    } else {
        printf("Fallo en fork para getppid\n");
        exit(1);
    }

    // Proceso para la prueba de getancestor
    pid = fork();
    if (pid == 0) {
        // Crear la jerarquía de procesos para la prueba de getancestor
        for (int i = 0; i <= 10; i++) {
            pid = fork();
            if (pid > 0) {
                // Proceso padre espera a que el hijo termine
                wait(0);
                exit(0); // Salir una vez que el hijo termine para mantener la jerarquía
            } else if (pid == 0) {
                // Proceso hijo continúa el bucle
                continue;
            } else {
                // Manejo de error si falla fork
                printf("Fallo en fork durante getancestor\n");
                exit(1);
            }
        }

        // En el último proceso hijo, prueba getancestor
        if (pid == 0) {
            printf("Ejecutando prueba de getancestor...\n");
            printf("Actual PID: %d\n", getpid());
            test_getancestor(ancestor_levels);
            exit(0);
        }
    } else if (pid > 0) {
        // Proceso principal espera a que termine la prueba de getancestor
        wait(0);
    } else {
        printf("Fallo en fork para getancestor\n");
        exit(1);
    }

    return 0;
}
