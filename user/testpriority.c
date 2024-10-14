#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void) {
    int num_processes = 20; // Crear 20 procesos
    int i;

    for (i = 0; i < num_processes; i++) {
        int pid = fork();

        if (pid < 0) {
            printf("Error al crear proceso\n");
            exit(1);
        } else if (pid == 0) {
            // Código que ejecuta cada proceso hijo
            printf("Ejecutando proceso %d con PID %d\n", i, getpid());
            sleep(5);  // Esperar unos segundos para simular la ejecución del proceso
            exit(0);    // Terminar el proceso hijo
        }
        sleep(1); // Añadir una pausa aquí para evitar el entrelazado de salida
    }

    // Esperar a que todos los procesos hijos terminen
    for (i = 0; i < num_processes; i++) {
        wait(0);
    }

    exit(0);
}
