/*
 * file: preemptive.h
 */

#ifndef __PREEMPTIVE_H__
#define __PREEMPTIVE_H__

#define MAXTHREADS 4

typedef char ThreadID;
typedef void (*FunctionPtr)(void);

ThreadID ThreadCreate (FunctionPtr);
void ThreadYield (void);
void ThreadExit (void);

#define CNAME(s) _ ## s
#define LABEL(label) label ## $

#define SemaphoreCreate(s, n) do s = n; while (0)

#define SemaphoreSignal(s)	\
    __asm					\
        INC CNAME(s)		\
    __endasm;

#define SemaphoreWait(s) SemaphoreWaitBody(s, __COUNTER__)

#define SemaphoreWaitBody(s, label)		\
    {									\
	    __asm 							\
		    LABEL(label):				\
		        MOV A, CNAME(s)			\
		        JZ LABEL(label)			\
		        JB ACC.7, LABEL(label)	\
		        DEC CNAME(s)			\
	    __endasm;						\
	}
	
#endif