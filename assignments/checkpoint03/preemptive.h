/*
 * file: preemptive.h
 */

#ifndef __PREEMPTIVE_H__
#define __PREEMPTIVE_H__

#define MAXTHREADS 4

typedef char ThreadID;
typedef void (*FunctionPtr)(void);

ThreadID ThreadCreate(FunctionPtr);
void ThreadYield(void);
void ThreadExit(void);

/* Map C symbols and unique wait labels to SDCC assembler names. */
#define CNAME(s) _##s
#define LABEL(label) label##$

#define SemaphoreCreate(s, n)                                                          \
    do                                                                                 \
        s = n;                                                                         \
    while (0)

// clang-format off
#define SemaphoreSignal(s)                                      \
    __asm                                                       \
        INC CNAME(s)                                            \
    __endasm;
// clang-format on

/* Spin until the signed semaphore is positive, then decrement it. */
#define SemaphoreWait(s) SemaphoreWaitBody(s, __COUNTER__)

// clang-format off
#define SemaphoreWaitBody(s, label)                             \
    {                                                           \
        __asm                                                   \
            LABEL(label):                                       \
                MOV A, CNAME(s)                                 \
                JZ LABEL(label)                                 \
                JB ACC.7, LABEL(label)                          \
                DEC CNAME(s)                                    \
        __endasm;                                               \
    }
// clang-format on

#endif
