# Tarea 3 Diego Espinoza

## Funcionamiento y lógica de la protección de memoria.

Para entender la lógica de la protección de memoria, se explica este código en C del archivo `test.c`que demuestra cómo reservar, proteger y desproteger una página de memoria en un entorno UNIX, probando así el uso de las funciones `mprotect` y ` munprotect` para manipular permisos de acceso a la memoria. Su estructura principal está diseñada para gestionar los permisos de una región de memoria y verificar si las restricciones aplicadas funcionan como se espera.

Primero, el programa reserva una página de 4096 bytes con `sbrk(4096)`, lo que expande el segmento de datos del proceso en una página. La dirección de inicio de esta memoria recién asignada se almacena en el puntero `addr`. Si la reserva falla, el programa imprime un mensaje de error y termina, asegurando que no haya intentos de manipulación en una dirección no válida.

A continuación, el programa llama a` mprotect(addr, 1)` para establecer permisos de solo lectura en la página de memoria reservada. Si la operación tiene éxito, imprime "mprotect exitoso"; de lo contrario, muestra un mensaje de error y finaliza. Si se desea verificar la protección, el bloque de código comentado intenta escribir un valor en addr. Esto debería fallar en un sistema compatible, ya que la página ha sido configurada para ser solo de lectura. Si esta escritura es exitosa, significa que la protección no está funcionando como se espera.

Luego, con la llamada a `munprotect(addr, 1)`, el programa restaura los permisos de escritura en la misma página, permitiendo la modificación de su contenido. Si la operación es exitosa, imprime "munprotect exitoso"; si falla, muestra un mensaje de error y termina. Finalmente, se intenta escribir en la página de memoria otra vez; en este punto, la escritura debería funcionar sin problemas, demostrando que los permisos de escritura han sido restaurados correctamente. Acá el codigo: 

```bash
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

```
### Importante
Un punto importante es lo que se va a mostrar en las fotos, cómo se activa la protección y se intenta acceder a dicha memoria para comprobar si se puede escribir se genera un error que se maneja cómo una excepción en el archiv `trap.c` por lo que se aprecia el mensaje de vilacion a memoria por lo que se comprueba el funcionamiento de la función, ahora, cómo el sistema define el proceso cómo muerto, se  termina la depuración lo que no permite comprobar la siguiente función por lo que se tiene que comentar dicho proceso de comprobación para que se aplique la siguiente función y realice los cambios de lectura a escritura, el otro punto que es importante mencionar es el resultado que arroja el sprintf, surgió el problema de que por terminal estaba arrojando `%c` y no la letra en cuestión (“B”) por lo que se decide colocar  `%d`, esto imprime el valor entero de `*ptr`, que es el código ASCII de la letra que se está almacenado en la memoria (en este caso, el valor ASCII de 'B', que es 66). Por lo que se comprueba que la función si está funcionando.

![alt text](<f1 t3.png>)
Acá se aprecia cómo se muestra la función 1 con lo comentado anteriormente

![alt text](<f2 t3.png>)
En esta imagen se aprecia lo que arroja por terminal la función 2, dejando en evidencia que ambas funcionan según lo redactado antes.

## Explicación de las modificaciones realizadas.

Las modificaciones realizadas se parten con las definiciones a las llamadas en los archivos correspondientes:

## `user.h`

```bash
int mprotect(void *addr, int len); //Nuevo agregado
int munprotect(void *addr, int len); //Nuevo agregado
```

## `usys.pl`

```bash
entry("mprotect");
entry("munprotect");
```

## `syscall.h`

```bash
#define SYS_mprotect 24 // Nuevo
#define SYS_munprotect 25 //Nuevo
```
## `syscall.c`

```bash
extern uint64 sys_mprotect(void); //Nuevo agregado
extern uint64 sys_munprotect(void); //Nuevo agregado
```

## `Makefile`

```bash
    $U/_test
```

## `spinlock.h`

```bash
#ifndef SPINLOCK_H
#define SPINLOCK_H

struct spinlock {
    uint locked;       // Is the lock held?
    char *name;        // Name of lock.
    struct cpu *cpu;   // The CPU holding the lock.
};

#endif // SPINLOCK_H
```

## `sysproc.c`

```bash
// Declaraciones de las funciones
int mprotect(void *addr, int len);
int munprotect(void *addr, int len);

uint64
sys_mprotect(void) {
    void *addr;
    int len;
    argaddr(0, (uint64 *)&addr);  // Usa directamente argaddr sin chequeo de retorno
    argint(1, &len);              // Usa directamente argint sin chequeo de retorno
    return mprotect(addr, len);
}


uint64
sys_munprotect(void) {
    void *addr;
    int len;
    argaddr(0, (uint64 *)&addr);
    argint(1, &len);
    return munprotect(addr, len);
}
```

## `trap.c`

```bash
void
usertrap(void)
{
  int which_dev = 0;
  uint64 scause = r_scause();  // Captura el valor de scause
  uint64 sepc = r_sepc();      // Captura el valor de sepc
  uint64 stval = r_stval();    // Captura el valor de stval (dirección de la violación)


  if((r_sstatus() & SSTATUS_SPP) != 0)
    panic("usertrap: not from user mode");


  // Send interrupts and exceptions to kerneltrap(), since we're now in the kernel.
  w_stvec((uint64)kernelvec);


  struct proc *p = myproc();
 
  // Save the user program counter.
  p->trapframe->epc = r_sepc();


  // Manejo de las excepciones
  if(scause == 8) {
    // system call
    if(killed(p))
      exit(-1);


    // sepc points to the ecall instruction,
    // but we want to return to the next instruction.
    p->trapframe->epc += 4;


    // Enable interrupts after handling the trap
    intr_on();
    syscall();
  } else if(scause == 0xf) {
    // Violación de acceso a memoria (scause == 0xf)
    // Aquí capturamos el error de acceso a memoria
    printf("usertrap: Violación de acceso a memoria (se comprueba)\n");
    printf("scause=0x%lx sepc=0x%lx stval=0x%lx\n", scause, sepc, stval);


    // Verificar si la violación ocurrió en una dirección protegida
    if (stval == (uint64)argaddr) {
        // Esto indica que la violación ocurrió en una dirección que intentamos proteger con mprotect
        printf("Violación de acceso en una dirección protegida\n");
        // Restaurar la ejecución, permitiendo al proceso continuar si lo deseamos
        p->trapframe->epc = sepc;  // Restaurar el valor de `sepc` para continuar la ejecución
    } else {
        // Si la violación no es esperada, terminamos el proceso
        setkilled(p);
    }
  } else if((which_dev = devintr()) != 0) {
    // Interrupción de un dispositivo manejada correctamente
  } else {
    // Ocurrió una excepción no esperada
    printf("usertrap(): unexpected scause 0x%lx pid=%d\n", scause, p->pid);
    printf("            sepc=0x%lx stval=0x%lx\n", sepc, stval);
    setkilled(p);
  }


  // Si el proceso fue marcado como matado, lo terminamos
  if(killed(p)) {
    exit(-1);
  }


  // Give up the CPU if this is a timer interrupt.
  if(which_dev == 2)
    yield();


  usertrapret();
}
```

Acá se manejó la violacion de acceso mencionada anteriormente para dejar en evidencia el funcionamiento del código.

## `vm.c`

```bash
int mprotect(void *addr, int len);
int munprotect(void *addr, int len);


// Función mprotect: Marca una región de memoria como solo lectura.
int mprotect(void *addr, int len) {
    if (len <= 0 || addr == 0 || ((uintptr_t)addr % PGSIZE) != 0) {
        return -1; // Dirección inválida o longitud negativa
    }


    uintptr_t start = (uintptr_t)addr;
    uintptr_t end = start + len * PGSIZE;


    // Recorre las páginas involucradas
    for (uintptr_t va = start; va < end; va += PGSIZE) {
        pte_t *pte = walk(myproc()->pagetable, va, 0);


        if (pte == 0) {
            return -1; // No se encontró una entrada de página para esta dirección
        }


        // Imprimir el valor de PTE antes de la modificación
        // printf("mprotect: pte antes de modificación: %p\n", pte);
        // printf("PTE antes de modificación: %lx\n", *pte); // Mostrar el valor de pte antes de modificarlo
        // printf("PTE_R: %lx, PTE_W: %lx\n", (*pte & PTE_R), (*pte & PTE_W)); // Mostrar bits PTE_R y PTE_W


        // Deshabilitar el permiso de escritura (PTE_W)
        *pte &= ~PTE_W;  // Poner a 0 el bit W
        *pte |= PTE_R;   // Asegurarse de que el bit R esté habilitado
       
        // Imprimir el valor después de la modificación
        // printf("mprotect: pte después de modificación: %p\n", pte);
        // printf("PTE después de modificación: %lx\n", *pte);  // Mostrar el valor de pte después de modificarlo
        // printf("PTE_R: %lx, PTE_W: %lx\n", (*pte & PTE_R), (*pte & PTE_W)); // Mostrar bits PTE_R y PTE_W después de la modificación
    }


    return 0; // Éxito
}

// Función munprotect: Restaura el permiso de escritura en una región de memoria.
int munprotect(void *addr, int len) {
    if (len <= 0 || addr == 0 || ((uintptr_t)addr % PGSIZE) != 0) {
        return -1; // Dirección inválida o longitud negativa
    }


    uintptr_t start = (uintptr_t)addr;
    uintptr_t end = start + len * PGSIZE;


    // Recorre las páginas involucradas
    for (uintptr_t va = start; va < end; va += PGSIZE) {
        pte_t *pte = walk(myproc()->pagetable, va, 0);


        if (pte == 0) {
            return -1; // No se encontró una entrada de página para esta dirección
        }


        // Imprimir el valor de PTE antes de la modificación
        // printf("munprotect: pte antes de modificación: %p\n", pte);
        // printf("PTE antes de modificación: %lx\n", *pte); // Mostrar el valor de pte antes de modificarlo
        // printf("PTE_R: %lx, PTE_W: %lx\n", (*pte & PTE_R), (*pte & PTE_W)); // Mostrar bits PTE_R y PTE_W


        // Restaurar el permiso de escritura (habilitar PTE_W)
        *pte |= PTE_W;  // Poner a 1 el bit W para habilitar escritura
       
        // Imprimir el valor después de la modificación
        // printf("munprotect: pte después de modificación: %p\n", pte);
        // printf("PTE después de modificación: %lx\n", *pte);  // Mostrar el valor de pte después de modificarlo
        // printf("PTE_R: %lx, PTE_W: %lx\n", (*pte & PTE_R), (*pte & PTE_W)); // Mostrar bits PTE_R y PTE_W después de la modificación
    }


    return 0; // Éxito
}
```

Este código implementa dos funciones, `mprotect` y `munprotect` en el archivo `vm.c`. La función `mprotect` recibe una dirección de memoria (`addr`) y un tamaño en páginas (`len`) y desactiva el permiso de escritura en esa región, marcándose como de solo lectura. Primero verifica que los parámetros sean válidos, luego recorre cada página en el rango especificado, ajustando su entrada en la tabla de páginas para deshabilitar el bit de escritura (`PTE_W`) y habilitar el de lectura (`PTE_R`). Si la modificación es exitosa, retorna `0`; si hay algún error, retorna `-1`.
Por otro lado, `munprotect` restaura el permiso de escritura en esa misma región, utilizando la misma lógica de validación y recorrido que `mprotect`, pero habilitando el bit de escritura (`PTE_W`) en lugar de deshabilitar. Ambas funciones incluyen mensajes para depurar, mostrando el estado de la entrada de la tabla de páginas antes y después de cada modificación. En conjunto, permiten proteger o restaurar permisos de escritura a nivel de página en la memoria de un proceso.

## Dificultades encontradas y soluciones implementadas.

En relación a los problemas encontrados, tenemos que la implementación de las funciones en relación a las llamadas tuvieron problemas debido a la falta de no agregarla en el archivo `usys.pl` por lo que arrojaba errores.

Por otro lado, al momento de entender que ela archivo `vm.c` era el encargado de manejar la memoria, tuvimos problemas con cierta lógica en la función `walk` ya que no estaba funcionando, pero después de investigar la documentación de xv6 se pudo solucionar.

Un punto muy complejo fue la violacion a la memoria que se hace para comprobar la función mprotect al inicio, no se entendía el porqué de dicho error pero después de analizarlo en profundidad se entendió que era porque la función estaba cumpliendo lo requerido pero empezar a manejar las excepciones de los errores podría generar problemas futuros por lo que se decir manejar la lógica comentando ciertos fragmentos del código cómo se explica en un punto anterior a la documentación, espero que dicha explicación se entienda y no genere confusión al momento de evaluar la implementación de las funciones.
