#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Función que implementa la llamada al sistema getppid().
// Devuelve el ID del proceso padre del proceso actual.
uint64
sys_getppid(void)
{
    // Obtiene el proceso actual usando myproc().
    struct proc *p = myproc();
    
    // Verifica si el proceso tiene un padre.
    if (p->parent)
        // Si existe un padre, devuelve el ID del padre.
        return p->parent->pid;
    
    // Si no existe un padre, devuelve -1 como código de error.
    return -1;
}

// Función que implementa la llamada al sistema getancestor(int).
// Devuelve el ID del ancestro especificado en el parámetro.
uint64
sys_getancestor(void)
{
    // Obtiene el proceso actual usando myproc().
    struct proc *p = myproc();
    
    // Declara una variable para almacenar el argumento.
    int n;
    
    // Obtiene el valor del argumento de la llamada al sistema.
    // `argint(0, &n)` lee el primer argumento entero.
    argint(0, &n);

    // Verifica si el argumento es negativo.
    if (n < 0)
        // Si el argumento es negativo, devuelve -1 como código de error.
        return -1;

    // Imprime un mensaje de depuración que indica que se está obteniendo el ancestro
    // para el proceso con el PID actual.
    printf("Obteniendo ancestro %d para PID %d\n", n, p->pid);

    // Recorre la jerarquía de procesos hasta el ancestro solicitado.
    for (int i = 0; i < n; i++) {
        // Verifica si el proceso tiene un padre.
        if (p->parent) {
            // Si existe un padre, actualiza el puntero al proceso padre.
            p = p->parent;
            // Imprime un mensaje de depuración con el PID del ancestro actual.
            printf("Ancestro %d: PID %d\n", i, p->pid);
        } else {
            // Si no existe más ancestros, imprime un mensaje y devuelve -1.
            printf("No hay más ancestros.\n");
            return -1;
        }
    }
    
    // Devuelve el PID del ancestro encontrado.
    return p->pid;
}
