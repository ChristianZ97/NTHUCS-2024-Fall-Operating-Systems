/*
 * file: testpreempt.c
 */
#include <8051.h>
#include "preemptive.h"

/* Shared state uses the original fixed internal-RAM addresses. */
__data __at(0x32) char buffer;
__data __at(0x33) _Bool bufferStatus; // 0 for empty, 1 for full

/* Produce the next character and publish it through the shared buffer. */
void Producer(void) {
    __data __at(0x34) char c = 'A';
    while (1) {
        if (bufferStatus) {
            ThreadYield();
            continue;
        }
        if (c > 'Z')
            c = 'A';

        EA = 0;
        buffer = c;
        bufferStatus = 1;
        EA = 1;

        c++;
    }
}

/* Configure UART transmission and consume buffered characters. */
void Consumer(void) {
    EA = 0;
    TMOD |= 0x20;
    TH1 = (unsigned char)(-6 & 0xFF);
    SCON = 0X50;
    TR1 = 1;
    EA = 1;

    while (1) {
        if (!bufferStatus) {
            ThreadYield();
            continue;
        }

        EA = 0;
        SBUF = buffer;
        bufferStatus = 0;
        EA = 1;

        while (!TI)
            ;
        EA = 0;
        TI = 0;
        EA = 1;
    }
}

void main(void) {
    EA = 0;
    bufferStatus = 0;
    EA = 1;

    ThreadCreate(Producer);
    Consumer();
}

/* Enter the thread bootstrap instead of the default SDCC startup path. */
void _sdcc_gsinit_startup(void) {
    // clang-format off
    __asm
        LJMP _Bootstrap
    __endasm;
    // clang-format on
}

void _mcs51_genRAMCLEAR(void) {}
void _mcs51_genXINIT(void) {}
void _mcs51_genXRAMCLEAR(void) {}

/* Dispatch Timer 0 directly to the assembly-aware context switch. */
void timer0_ISR(void) __interrupt(1) {
    // clang-format off
    __asm
        LJMP _myTimer0Handler
    __endasm;
    // clang-format on
}
