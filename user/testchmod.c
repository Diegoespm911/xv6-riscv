#include "types.h"
#include "stat.h"
#include "fcntl.h"  // Incluye las macros O_CREATE y O_RDWR
#include "user.h"

#define FILENAME "testfile"

// Función para imprimir mensaje de error
void error(char *msg) {
    printf(msg);
    exit(1);
}

int main(void) {
    int fd;

    // Crear un archivo con permisos de lectura y escritura (O_CREATE | O_RDWR)
    fd = open(FILENAME, O_CREATE | O_RDWR);
    if (fd < 0) {
        error("No se pudo crear el archivo con permisos de lectura y escritura.\n");
    }
    printf("Archivo creado con permisos de lectura y escritura.\n");

    // Escribir datos en el archivo para asegurar que la creación y escritura funcionan
    write(fd, "Este es un archivo de prueba.\n", 28);
    close(fd);
    printf("Escritura inicial exitosa.\n");

    // Cambiar los permisos a solo lectura (chmod(filename, 1))
    if (chmod(FILENAME, 1) < 0) {
        error("Error al cambiar permisos a solo lectura.\n");
    }
    printf("Permisos cambiados a solo lectura.\n");

    // Intentar escribir en el archivo (debería fallar)
    fd = open(FILENAME, O_WRONLY);
    if (fd >= 0) {
        error("Se pudo escribir en un archivo de solo lectura (esto no debería ocurrir).\n");
    }
    printf("Intento de escritura fallido correctamente en archivo de solo lectura.\n");

    // Cambiar los permisos a lectura/escritura (chmod(filename, 3))
    if (chmod(FILENAME, 3) < 0) {
        error("Error al cambiar permisos a lectura/escritura.\n");
    }
    printf("Permisos cambiados a lectura/escritura.\n");

    // Intentar escribir nuevamente (debería funcionar)
    fd = open(FILENAME, O_WRONLY);
    if (fd < 0) {
        error("No se pudo abrir el archivo para escritura.\n");
    }
    write(fd, "Escritura después de cambio de permisos.\n", 37);
    close(fd);
    printf("Escritura final exitosa.\n");

    // Cambiar los permisos a inmutable (chmod(filename, 5))
    if (chmod(FILENAME, 5) < 0) {
        error("Error al cambiar permisos a inmutable.\n");
    }
    printf("Permisos cambiados a inmutable.\n");

    // Intentar escribir en el archivo (debería fallar debido al permiso inmutable)
    fd = open(FILENAME, O_WRONLY);
    if (fd >= 0) {
        error("Se pudo escribir en un archivo inmutable (esto no debería ocurrir).\n");
    }
    printf("Intento de escritura fallido correctamente en archivo inmutable.\n");

    // Intentar cambiar los permisos a lectura/escritura (debería fallar debido al permiso inmutable)
    if (chmod(FILENAME, 3) == 0) {
        error("Se pudo cambiar los permisos de un archivo inmutable (esto no debería ocurrir).\n");
    }
    printf("Intento de cambio de permisos fallido correctamente en archivo inmutable.\n");

    // Terminar el programa
    printf("Pruebas completadas con éxito.\n");
    exit(0);
}
