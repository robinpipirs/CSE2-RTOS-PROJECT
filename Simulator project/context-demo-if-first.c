// main.c
#include "kernel.h"

TCB taskA;
TCB taskB;
TCB * Running;

void task1(void);
void task2(void);

TCB mytasks[10];

void TimerInt (void)
{
}

int main(void)
{

	Running = &taskB;
	Running->PC = task2;
	Running->SP = &(Running->StackSeg[STACK_SIZE-1]);

	Running = &taskA;
	Running->PC = task1;
	Running->SP = &(Running->StackSeg[STACK_SIZE-1]);

	LoadContext();
}

void task1(void)
{
	volatile int first = TRUE;
	SaveContext();
        if (first)
        {
	first = FALSE;
        Running = &taskB;
	LoadContext();
        }
        else {
          while(1);
        }
}

void task2(void)
{
	volatile int first = TRUE;
	SaveContext();
        if (first)
        {
	first = FALSE;
        Running = &taskA;
	LoadContext();
        }
        else {
          while(1);
        }
}
