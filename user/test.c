#include "types.h"
#include "stat.h"
#include "user.h"

int main() {
    char *addr = sbrk(4096);  // Reservar una página de memoria
    if (addr == (char *)-1) {
        printf("Error al reservar memoria\n");
        exit(1);
    }
    // Imprimir dirección de la memoria reservada
    printf("Memoria reservada en la dirección: %p\n", addr);

    // Proteger la página de memoria para lectura solamente
    if (mprotect(addr, 1) == -1) {
        printf("mprotect falló\n");
        exit(1);
    } else {
        printf("mprotect exitoso\n");
    }

    // ############################################### //
    // COMENTAR PARA QUE CORRA LA SEGUNDA FUNCION //

    // Intentar escribir en la página protegida
    char *ptr = addr; //se deja para probar las dos fucniones
    //*ptr = 'A';  // Esto debería fallar si la protección es exitosa
    //printf("Valor en la dirección después de escribir: %c\n", *ptr);  // Verificar el valor

    // // Intentar leer el valor
    // if (*ptr == 'A') {
    //     printf("La escritura fue exitosa, protección no está funcionando.\n");
    // } else {
    //     printf("Protección funcionando, no se pudo escribir en la página.\n");
    // }

    // ############################################### //

    // Restaurar la protección de escritura con munprotect
    if (munprotect(addr, 1) == -1) {
        printf("munprotect falló\n");
        exit(1);
    } else {
        printf("munprotect exitoso\n");
    }

    // Intentar escribir en la página nuevamente (debería ser exitoso)
    printf("Valor inicial en la dirección: %d\n", *ptr);
    *ptr = 'B';  // Esto debería ser exitoso después de la llamada a munprotect
    printf("Valor en la dirección después de escribir: %d\n", *ptr);
    exit(0);  // Salir correctamente
}
