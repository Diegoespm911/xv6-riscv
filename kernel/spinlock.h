// spinlock.h
#ifndef SPINLOCK_H
#define SPINLOCK_H

struct spinlock {
    uint locked;       // Is the lock held?
    char *name;        // Name of lock.
    struct cpu *cpu;   // The CPU holding the lock.
};

#endif // SPINLOCK_H
