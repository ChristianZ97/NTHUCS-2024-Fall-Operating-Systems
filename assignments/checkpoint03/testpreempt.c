/*
 * file: testpreempt.c
 */

#include <8051.h>
#include "preemptive.h"

__data __at (0x32) char mutex;
__data __at (0x33) char full;
__data __at (0x34) char empty;

__data __at (0x35) char buffer[3];

__data __at (0x38) char head;
__data __at (0x39) char tail;

void Producer (void) {

    __data __at (0x40) char c = 'A';
    while (1) {
        
        if (c > 'Z') c = 'A';
        SemaphoreWait(empty);
        SemaphoreWait(mutex);

            buffer[head] = c;
            head = (head + 1) % 3;

        SemaphoreSignal(mutex);
        SemaphoreSignal(full);
        c++;
    }
}

void Consumer (void) {

    TMOD |= 0x20; TH1 = (unsigned char)(-6 & 0xFF); SCON = 0x50; TR1 = 1;

    while (1) {

        SemaphoreWait(full);
        SemaphoreWait(mutex);

            SBUF = buffer[tail];
            buffer[tail] = ' ';
            tail = (tail + 1) % 3;

        SemaphoreSignal(mutex);
        SemaphoreSignal(empty);

        while (!TI);
        TI = 0;
    }
}

void main (void) {

    SemaphoreCreate(mutex, 1);
    SemaphoreCreate(full, 0);
    SemaphoreCreate(empty, 3);

    head = 0;
    tail = 0;

    ThreadCreate(Producer);
    Consumer();
}


void _sdcc_gsinit_startup (void) {

    __asm
        LJMP _Bootstrap
    __endasm;
}

void _mcs51_genRAMCLEAR (void) {}
void _mcs51_genXINIT (void) {}
void _mcs51_genXRAMCLEAR (void) {}

void timer0_ISR (void) __interrupt(1) {

    __asm
        LJMP _myTimer0Handler
    __endasm;
}
