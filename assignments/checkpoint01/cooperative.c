#include <8051.h>

#include "cooperative.h"

/* Fixed internal-RAM layout shared with the context-switch assembly. */
__data __at(0x20) ThreadID curThd;
__data __at(0x22) unsigned char savedSP[MAXTHREADS];
__data __at(0x26) _Bool bitmap[4];
__data __at(0x2A) unsigned char thdCnt;

/* Save registers in stack order, then record this thread's SP. */
// clang-format off
#define SAVESTATE                                               \
    {                                                           \
        __asm                                                   \
            push ACC                                            \
            push B                                              \
            push DPL                                            \
            push DPH                                            \
            push PSW                                            \
        __endasm;                                               \
        savedSP[curThd] = SP;                                   \
    }
// clang-format on

/* Restore SP first, then pop registers in reverse order. */
// clang-format off
#define RESTORESTATE                                            \
    {                                                           \
        SP = savedSP[curThd];                                   \
        __asm                                                   \
            pop PSW                                             \
            pop DPH                                             \
            pop DPL                                             \
            pop B                                               \
            pop ACC                                             \
        __endasm;                                               \
    }
// clang-format on

/*
 * we declare main() as an extern so we can reference its symbol
 * when creating a thread for it.
 */
extern void main(void);

/*
 * Bootstrap is jumped to by the startup code to make the thread for
 * main, and restore its context so the thread can run.
 */
void Bootstrap(void) {
    for (unsigned int i = 0; i < MAXTHREADS; i++)
        bitmap[i] = 0;
    thdCnt = 0;

    ThreadCreate(main);
    curThd = 0;
    RESTORESTATE;
}

/*
 * ThreadCreate() creates a thread data structure so it is ready
 * to be restored (context switched in).
 * The function pointer itself should take no argument and should
 * return no argument.
 */
ThreadID ThreadCreate(FunctionPtr fp) {
    if (thdCnt >= MAXTHREADS)
        return -1;

    __data __at(0x2C) ThreadID id = 0;
    for (unsigned int i = 0; i < MAXTHREADS; i++)
        if (!bitmap[i]) {
            id = i;
            bitmap[i] = 1;
            break;
        }
    thdCnt++;

    /* Each thread receives a 16-byte stack region. */
    savedSP[id] = 0x3F + 16 * id;

    __data __at(0x2D) unsigned int tempSP = SP;
    /* Build the initial frame on the new stack; fp arrives in DPTR. */
    SP = savedSP[id];

    // clang-format off
    __asm
        push DPL
        push DPH
        mov r7, #0x00
        push ar7
        push ar7
        push ar7
        push ar7
    __endasm;
    // clang-format on

    /* Initial PSW selects register bank 0, 1, 2, or 3 for this thread. */
    switch (id) {
    case 0:
        // clang-format off
        __asm
            mov r7, #0x00
            push ar7
        __endasm;
        // clang-format on
        break;

    case 1:
        // clang-format off
        __asm
            mov r7, #0x08
            push ar7
        __endasm;
        // clang-format on
        break;

    case 2:
        // clang-format off
        __asm
            mov r7, #0x10
            push ar7
        __endasm;
        // clang-format on
        break;

    case 3:
        // clang-format off
        __asm
            mov r7, #0x18
            push ar7
        __endasm;
        // clang-format on
        break;

    default:
        break;
    }

    savedSP[id] = SP;

    /* Resume construction on the caller's stack. */
    SP = tempSP;

    return id;
}

/*
 * this is called by a running thread to yield control to another
 * thread.  ThreadYield() saves the context of the current
 * running thread, picks another thread (and set the current thread
 * ID to it), if any, and then restores its state.
 */
void ThreadYield(void) {
    SAVESTATE;
    do {
        curThd++;
        if (curThd == MAXTHREADS)
            curThd = 0;
        if (bitmap[curThd])
            break;
    } while (1);
    RESTORESTATE;
}

/* Preserved submission behavior: restores context without releasing the slot. */
void ThreadExit(void) {
    RESTORESTATE;
}
