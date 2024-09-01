# Tarea 1 Diego Espinoza

## Funcionamiento de las llamadas al sistema.
Las llamadas al sistema getppid y getancestor se implementan en sysproc.c y funcionan de la siguiente manera:

### sys_getppid()

```bash
uint64 sys_getppid(void)
{
    struct proc *p = myproc();  // Obtiene el proceso actual
    if (p->parent)              // Verifica si el proceso tiene un padre
        return p->parent->pid;  // Devuelve el ID del padre si existe
    return -1;                  // Devuelve -1 si no hay un padre
}
```
La función obtiene el proceso actual usando myproc() y, si tiene un proceso padre (p->parent), devuelve el ID de ese padre. Si no hay un padre (por ejemplo, si el proceso es el inicial o se ha quedado sin padre), devuelve -1.

### sys_getancestor(int)

```bash
uint64 sys_getancestor(void)
{
    struct proc *p = myproc();  // Obtiene el proceso actual
    int n;
    argint(0, &n);              // Obtiene el nivel del ancestro

    if (n < 0)
        return -1;              // Si el nivel es negativo, devuelve -1

    while (n > 0) {
        if (p->parent) {        // Si hay un proceso padre
            p = p->parent;      // Avanza al proceso padre
            n--;                // Decrementa el nivel deseado
        } else {
            return -1;          // Devuelve -1 si no hay más ancestros
        }
    }

    return p->pid;              // Devuelve el PID del ancestro encontrado
}
```
La función getancestor(int n) recorre los procesos padres n veces para encontrar el ancestro en el nivel especificado. Si no hay suficientes niveles o n es negativo, devuelve -1.

### Funcionamiento de yosoytupadre.c
El archivo yosoytupadre.c prueba las llamadas al sistema getppid y getancestor mediante la creación de procesos con fork().

test_getppid(): Crea un proceso hijo, y en el hijo llama a getppid para imprimir el PID del padre. El padre espera que el hijo termine.
test_getancestor(int levels): Recorre los niveles de ancestros hasta el especificado y llama a getancestor para imprimir cada PID ancestro.

```bash
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
            printf("Ancestor level %d: PID %d\n", i, ancestor_pid);
        } else {
            printf("Ancestor level %d: No se encuentra ancestor\n", i);
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
        for (int i = 0; i <= 20; i++) {
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
```

## Explicación de las modificaciones realizadas.

Definir las funciones en sysproc.c:

```bash
sys_getppid() y sys_getancestor() implementan las llamadas al sistema.
```
Agregar llamadas en syscall.c:

```bash
extern uint64 sys_getppid(void);    // Se define la llamada a getppid
extern uint64 sys_getancestor(void); // Se define la llamada a getancestor
```

Agregar en syscall.h:

```bash
#define SYS_getppid 22       // Asigna un número de llamada al sistema
#define SYS_getancestor 23   // Asigna un número de llamada al sistema
```

Agregar en user.h:

```bash
int getppid(void);           // Declara la función de usuario
int getancestor(int n);      // Declara la función de usuario
```

Agregar en usys.pl:

```bash
entry("getppid");            // Agrega entrada para generar en usys.S
entry("getancestor");        // Agrega entrada para generar en usys.S
```
Crear archivo yosoytupadre.c: ubicado en la carpeta user. Este archivo se utiliza para probar las llamadas al sistema implementadas, para que funcione se añade al Makefile lo siguiente:

```bash
$U/_yosoytupadre\
```

## Dificultades encontradas y cómo se resolvieron.

### Error al modificar usys.S manualmente:
Inicialmente, se añadieron las llamadas al sistema directamente en usys.S, pero al ejecutar xv6, estas se eliminaban. Se resolvió al agregar las llamadas en usys.pl, que automáticamente genera usys.S.

### Saltos en los PID utilizados en getancestor:
Se observó que los procesos utilizados en getppid no aparecían en la lista de ancestros durante la prueba de getancestor. Esto se debe a que esos procesos terminan y liberan su PID antes de la llamada a getancestor. La funcionalidad sigue siendo correcta ya que cumple con los requisitos de encontrar ancestros existentes.


## Funcionamiento de las llamadas al sistema (imagen)
![alt text](<Tarea 1 Diego Espinoza.png>)