/*
 * file: testcoop.c
 */
#include <8051.h>
#include "cooperative.h"

/* Shared state uses the original fixed internal-RAM addresses. */
__data __at(0x30) unsigned char buffer;
__data __at(0x32) unsigned char bufferStatus; // 0 for empty, 1 for full

/* Produce the next character and publish it through the shared buffer. */
void Producer(void) {
    __data __at(0x34) unsigned char c = 'A';
    while (1) {
        if (bufferStatus)
            ThreadYield();
        if (c > 'Z')
            c = 'A';
        buffer = c++;
        bufferStatus = 1;
    }
}

/* Configure UART transmission and consume buffered characters. */
void Consumer(void) {
    TMOD = 0x20;
    TH1 = (unsigned char)(-6 & 0xFF);
    SCON = 0X50;
    TR1 = 1;

    while (1) {
        while (!bufferStatus)
            ThreadYield();

        SBUF = buffer;
        while (!TI)
            ThreadYield();
        TI = 0;
        bufferStatus = 0;
    }
}

void main(void) {
    bufferStatus = 0;

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
