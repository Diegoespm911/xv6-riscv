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
// sysproc.c
// Función que implementa la llamada al sistema getancestor(int).
// Devuelve el ID del ancestro especificado en el parámetro.
uint64
sys_getancestor(void)
{
    struct proc *p = myproc(); // Obtener el proceso actual
    int n;
    argint(0, &n); // Obtener el parámetro que indica el nivel del ancestro

    if (n < 0)
        return -1; // Si el nivel es negativo, devolver -1

    // Imprimir el PID actual y el nivel de ancestro que estamos buscando

    // Recorrer la cadena de padres hasta el nivel deseado
    while (n > 0) {
        if (p->parent) { // Si hay un proceso padre
            p = p->parent; // Ir al proceso padre
            n--; // Decrementar el nivel deseado
        } else {
            // Si no hay más ancestros, imprimir mensaje y devolver -1
            return -1;
        }
    }

    // Imprimir el PID del ancestro encontrado y devolverlo
    return p->pid; // Devolver el PID del ancestro encontrado
}
