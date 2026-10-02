#include <8051.h>

#include "preemptive.h"

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
            PUSH ACC                                            \
            PUSH B                                              \
            PUSH DPL                                            \
            PUSH DPH                                            \
            PUSH PSW                                            \
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
            POP PSW                                             \
            POP DPH                                             \
            POP DPL                                             \
            POP B                                               \
            POP ACC                                             \
        __endasm;                                               \
    }
// clang-format on

extern void main(void);

void Bootstrap(void) {
    for (int i = 0; i < MAXTHREADS; i++)
        bitmap[i] = 0;
    thdCnt = 0;

    TMOD = 0;
    IE = 0x82;
    TR0 = 1;

    curThd = ThreadCreate(main);
    RESTORESTATE;
}

ThreadID ThreadCreate(FunctionPtr fp) {
    EA = 0;
    if (thdCnt >= MAXTHREADS) {
        EA = 1;
        return -1;
    }

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
        PUSH DPL
        PUSH DPH
        MOV r7, #0x00
        PUSH ar7
        PUSH ar7
        PUSH ar7
        PUSH ar7
    __endasm;
    // clang-format on

    /* Initial PSW selects register bank 0, 1, 2, or 3 for this thread. */
    switch (id) {
    case 0:
        // clang-format off
        __asm
            MOV r7, #0x00
            PUSH ar7
        __endasm;
        // clang-format on
        break;

    case 1:
        // clang-format off
        __asm
            MOV r7, #0x08
            PUSH ar7
        __endasm;
        // clang-format on
        break;

    case 2:
        // clang-format off
        __asm
            MOV r7, #0x10
            PUSH ar7
        __endasm;
        // clang-format on
        break;

    case 3:
        // clang-format off
        __asm
            MOV r7, #0x18
            PUSH ar7
        __endasm;
        // clang-format on
        break;

    default:
        break;
    }

    savedSP[id] = SP;

    /* Resume construction on the caller's stack. */
    SP = tempSP;

    EA = 1;
    return id;
}

void ThreadYield(void) {
    EA = 0;

    SAVESTATE;
    /* Round-robin selection skips unused thread slots. */
    do {
        curThd++;
        if (curThd == MAXTHREADS)
            curThd = 0;
        if (bitmap[curThd])
            break;
    } while (1);
    RESTORESTATE;

    EA = 1;
}

/* Preserved submission behavior: restores context without releasing the slot. */
void ThreadExit(void) {
    EA = 0;
    RESTORESTATE;
    EA = 1;
}

void myTimer0Handler(void) {
    SAVESTATE;

    // clang-format off
    __asm
        MOV B, R0
        MOV DPL, R1
        MOV DPH, R2
    __endasm;
    // clang-format on

    /* Round-robin selection skips unused thread slots. */
    do {
        curThd++;
        if (curThd == MAXTHREADS)
            curThd = 0;
        if (bitmap[curThd])
            break;
    } while (1);

    // clang-format off
    __asm
        MOV R0, B
        MOV R1, DPL
        MOV R2, DPH
    __endasm;
    // clang-format on

    RESTORESTATE;

    // clang-format off
    __asm
        RETI
    __endasm;
    // clang-format on
}
