/*
 * file: test3threads.c
 */

#include <8051.h>
#include "preemptive.h"

__data __at (0x30) signed char mutex;
__data __at (0x32) signed char full;
__data __at (0x34) signed char empty;

__data __at (0x36) char buffer[3];
__data __at (0x39) char *head;
__data __at (0x3B) char *tail;

void Producer1 (void) {

    __data __at (0x3D) char c = 'A';

    while (1) {
        
        if (c > 'Z') c = 'A';
        SemaphoreWait(empty);
        SemaphoreWait(mutex);

        if (head > buffer + 2) head = buffer;
        *head = c;
        head++;

        SemaphoreSignal(mutex);
        SemaphoreSignal(full);

        ThreadYield();
        c++;
    }
}

void Producer2 (void) {

    __data __at (0x3F) char d = '0';

    while (1) {
        
        if (d > '9') d = '0';
        SemaphoreWait(empty);
        SemaphoreWait(mutex);

        if (head > buffer + 2) head = buffer;
        *head = d;
        head++;

        SemaphoreSignal(mutex);
        SemaphoreSignal(full);

        ThreadYield();
        d++;
    }
}

void Consumer (void) {

    TMOD |= 0x20; TH1 = (unsigned char)(-6 & 0xFF); SCON = 0X50; TR1 = 1;

    while (1) {

        if (tail > buffer + 2) tail = buffer;
        SemaphoreWait(full);
        SemaphoreWait(mutex);

        SBUF = *tail;

        SemaphoreSignal(mutex);
        SemaphoreSignal(empty);
        tail++;
        while(!TI);
        TI = 0;
    }
}

void main (void) {

    SemaphoreCreate(mutex, 1);
    SemaphoreCreate(full, 0);
    SemaphoreCreate(empty, 3);

    head = buffer;
    tail = buffer;

    ThreadCreate(Producer1);
    ThreadCreate(Producer2);
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
