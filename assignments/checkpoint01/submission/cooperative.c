#include <8051.h>

#include "cooperative.h"

/*
 * [TODO]
 * declare the static globals here using
 *        __data __at (address) type name; syntax
 * manually allocate the addresses of these variables, for
 * - saved stack pointers (MAXTHREADS)
 * - current thread ID
 * - a bitmap for which thread ID is a valid thread;
 *   maybe also a count, but strictly speaking not necessary
 * - plus any temporaries that you need.
 */
__data __at (0x20) ThreadID curThd;
__data __at (0x22) unsigned char savedSP[MAXTHREADS];
__data __at (0x26) _Bool bitmap[4];
__data __at (0x2A) unsigned char thdCnt;

/*
 * [TODO]
 * define a macro for saving the context of the current thread by
 * 1) push ACC, B register, Data pointer registers (DPL, DPH), PSW
 * 2) save SP into the saved Stack Pointers array
 *   as indexed by the current thread ID.
 * Note that 1) should be written in assembly,
 *     while 2) can be written in either assembly or C
 */
#define SAVESTATE               \
    {                           \
        __asm                   \
        /*your code here*/      \
            push ACC            \
            push B              \
            push DPL            \
            push DPH            \
            push PSW            \
        __endasm;               \
        savedSP[curThd] = SP;   \
    }

/*
 * [TODO]
 * define a macro for restoring the context of the current thread by
 * essentially doing the reverse of SAVESTATE:
 * 1) assign SP to the saved SP from the saved stack pointer array
 * 2) pop the registers PSW, data pointer registers, B reg, and ACC
 * Again, popping must be done in assembly but restoring SP can be
 * done in either C or assembly.
 */
#define RESTORESTATE            \
    {                           \
        SP = savedSP[curThd];   \
        __asm                   \
        /*your code here*/      \
            pop PSW             \
            pop DPH             \
            pop DPL             \
            pop B               \
            pop ACC             \
        __endasm;               \
    }

/*
 * we declare main() as an extern so we can reference its symbol
 * when creating a thread for it.
 */
extern void main(void);

/*
 * Bootstrap is jumped to by the startup code to make the thread for
 * main, and restore its context so the thread can run.
 */
void Bootstrap(void)
{
    /*
     * [TODO]
     * initialize data structures for threads (e.g., mask)
     *
     * optional: move the stack pointer to some known location
     * only during bootstrapping. by default, SP is 0x07.
     */
    for (unsigned int i = 0; i < MAXTHREADS; i++) bitmap[i] = 0;
    thdCnt = 0;
     /* [TODO]
     *     create a thread for main; be sure current thread is
     *     set to this thread ID, and restore its context,
     *     so that it starts running main().
     */
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
ThreadID ThreadCreate(FunctionPtr fp)
{
    /*
     * [TODO]
     * check to see we have not reached the max #threads.
     * if so, return -1, which is not a valid thread ID.
     */
    if (thdCnt >= MAXTHREADS) return -1;
    /*
     * [TODO]
     *     otherwise, find a thread ID that is not in use,
     *     and grab it. (can check the bit mask for threads),
     *
     * [TODO] below
     * a. update the bit mask
         (and increment thread count, if you use a thread count,
          but it is optional)
       b. calculate the starting stack location for new thread
       c. save the current SP in a temporary
          set SP to the starting location for the new thread
       d. push the return address fp (2-byte parameter to
          ThreadCreate) onto stack so it can be the return
          address to resume the thread. Note that in SDCC
          convention, 2-byte ptr is passed in DPTR.  but
          push instruction can only push it as two separate
          registers, DPL and DPH.
       e. we want to initialize the registers to 0, so we
          assign a register to 0 and push it four times
          for ACC, B, DPL, DPH.  Note: push #0 will not work
          because push takes only direct address as its operand,
          but it does not take an immediate (literal) operand.
       f. finally, we need to push PSW (processor status word)
          register, which consist of bits
           CY AC F0 RS1 RS0 OV UD P
          all bits can be initialized to zero, except <RS1:RS0>
          which selects the register bank.
          Thread 0 uses bank 0, Thread 1 uses bank 1, etc.
          Setting the bits to 00B, 01B, 10B, 11B will select
          the register bank so no need to push/pop registers
          R0-R7.  So, set PSW to
          00000000B for thread 0, 00001000B for thread 1,
          00010000B for thread 2, 00011000B for thread 3.
       g. write the current stack pointer to the saved stack
          pointer array for this newly created thread ID
       h. set SP to the saved SP in step c.
       i. finally, return the newly created thread ID.
     */
    // a.
    __data __at (0x2C) ThreadID id = 0;
    for (unsigned int i = 0; i < MAXTHREADS; i++) if (!bitmap[i]) {id = i; bitmap[i] = 1; break;}
    thdCnt++;

    // b.
    savedSP[id] = 0x3F + 16 * id;

    // c.
    __data __at (0x2D) unsigned int tempSP = SP;
    SP = savedSP[id];

    __asm
        // d.
        push DPL
        push DPH
        // e.
        mov r7, #0x00
        push ar7
        push ar7
        push ar7
        push ar7
    __endasm;

    // f.
    switch (id) {
        case 0:
            __asm
                mov r7, #0x00
                push ar7
            __endasm;
            break;

        case 1:
            __asm
                mov r7, #0x08
                push ar7
            __endasm;
            break;

        case 2:
            __asm
                mov r7, #0x10
                push ar7
            __endasm;
            break;

        case 3:
            __asm
                mov r7, #0x18
                push ar7
            __endasm;
            break;

        default:
            break;
    }

    // g.
    savedSP[id] = SP;

    // h.
    SP = tempSP;

    // i.
    return id;
}

/*
 * this is called by a running thread to yield control to another
 * thread.  ThreadYield() saves the context of the current
 * running thread, picks another thread (and set the current thread
 * ID to it), if any, and then restores its state.
 */
void ThreadYield(void)
{
    SAVESTATE;
    do
    {
        /*
         * [TODO]
         * do round-robin policy for now.
         * find the next thread that can run and
         * set the current thread ID to it,
         * so that it can be restored (by the last line of
         * this function).
         * there should be at least one thread, so this loop
         * will always terminate.
         */
        curThd++;
        if (curThd == MAXTHREADS) curThd = 0;
        if (bitmap[curThd]) break;
    } while (1);
    RESTORESTATE;
}

/*
 * ThreadExit() is called by the thread's own code to terminate
 * itself.  It will never return; instead, it switches context
 * to another thread.
 */
void ThreadExit(void)
{
    /*
     * clear the bit for the current thread from the
     * bit mask, decrement thread count (if any),
     * and set current thread to another valid ID.
     * Q: What happens if there are no more valid threads?
     */

    RESTORESTATE;
}
