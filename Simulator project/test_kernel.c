//
//  test_kernel.c
//  CSE2 embedded os
//
//  Created by Robin Pipirs on 2015-02-03.
//  Copyright (c) 2015 Robin Pipirs. All rights reserved.
//

//Tests for task administration
#include <limits.h>
#include "kernel.h"
#include "dlist.h"
#include "assert.h"
#include "test_functions.h"


/* Forward declaration*/

//used by test recieve and send wait
#define TEST_PATTERN_1 0xAA
#define TEST_PATTERN_2 0x55

//used by recieve and send no wait
#define TEST_PATTERN_3 0x64
#define TEST_PATTERN_4 0x39
#define TEST_PATTERN_5 0x40
#define TEST_PATTERN_6 0x54

//used by combined test
#define TEST_PATTERN_7 0x50
#define TEST_PATTERN_8 0x22

//used by recieve send wait test
mailbox *mb;
//used by recieve send no wait test
mailbox *mb2;
//used by combined test
mailbox *mb3;

int nTest1=0, nTest2=0, nTest3=0, nTest4=0, nTest5=0, nTest6=0;

//test recieve & send wait
void task1(void);
void task2(void);

//test recieve & send no wait
void task3(void);
void task4(void);

int status = FAIL;



void task_test_nowait_a(void);
void task_test_nowait_b(void);

void task_test_wait_a(void);
void task_test_wait_b(void);

void task_test_bothcombined_a(void);
void task_test_bothcombined_b(void);

void test_rtos(void);



//this task combined with nowait_b tests the inegrity of the mailbox structure aswell as the send recieve nowait
void task_test_nowait_a(void)
{
    
    volatile int nData_t3 = 0;
    volatile int nData_t4 = 0;
    volatile int nData_t5 = 0;
    volatile int nData_t6 = 0;
    
    
    //check that mailbox creation suceeds
    mb2=create_mailbox(3,sizeof(int));
    assert(isNotEqualPointer(mb2, NULL));
    
    //test if recieve no wait returns fail
    int x = receive_no_wait(mb2,(void *)&nData_t3 );
    assert(isEqualInt(x, FAIL));
    //check so that no of msg is 0
    assert(isEqualInt(mb2->nMessages, 0));
    //check no blocked msg
    assert(isEqualInt(mb2->nBlockedMsg, 0));
    
    nData_t3 = TEST_PATTERN_3;
    //send no wait
    x = send_no_wait(mb2, (void*) &nData_t3);
    assert(isEqualInt(x, OK));
    //check so that no of msg is 1
    assert(isEqualInt(mb2->nMessages, 1));
    //check no blocked msg
    assert(isEqualInt(mb2->nBlockedMsg, 0));
    //check message in mb
    //assert(isEqualInt((int)mb2->pHead->pNext->pData, nData_t3));
    
    nData_t4 = TEST_PATTERN_4;
    //send no wait
    x = send_no_wait(mb2, (void*) &nData_t4);
    assert(isEqualInt(x, OK));
    //check so that no of msg is 2
    assert(isEqualInt(mb2->nMessages, 2));
    //check no blocked msg
    assert(isEqualInt(mb2->nBlockedMsg, 0));
    //check message in mb
    //assert(isEqualInt((int)mb2->pHead->pNext->pData, nData_t3));
    
    nData_t5 = TEST_PATTERN_5;
    //send no wait
    x = send_no_wait(mb2, (void*) &nData_t5);
    assert(isEqualInt(x, OK));
    //check so that no of msg is 3
    assert(isEqualInt(mb2->nMessages, 3));
    //check no blocked msg
    assert(isEqualInt(mb2->nBlockedMsg, 0));
    //check message in mb
    //assert(isEqualInt((int )mb2->pHead->pNext->pData, nData_t3));
    
    //test delete of mailbox should fail since its not empty
    x = remove_mailbox(mb2);
    assert(isEqualInt(x, NOT_EMPTY));
    
    nData_t6 = TEST_PATTERN_6;
    //send no wait this should not fail since the mb is full, it should delete the oldest msg and add this msg
    x= send_no_wait(mb2, (void*) &nData_t6);
    assert(isEqualInt(x, OK));
    //check so that no of msg is 3
    assert(isEqualInt(mb2->nMessages, 3));
    //check no blocked msg
    assert(isEqualInt(mb2->nBlockedMsg, 0));
    //check message in mb
    //assert(isEqualInt((int)mb2->pHead->pNext->pData, nData_t3));
    
    /**
     * test recieve function
     */
    
    //check so that recieve function works and return ok aswell as the correct value
    x =receive_no_wait(mb2, (void *) &nData_t3);
    assert(isEqualInt(x, OK));
    //check so that no of msg is 2
    assert(isEqualInt(mb2->nMessages, 2));
    //check no blocked msg
    assert(isEqualInt(mb2->nBlockedMsg, 0));
    //check message in mb
    assert(isEqualInt(nData_t3, TEST_PATTERN_3));
    
    //test delete of mailbox should fail since its not empty
    x = remove_mailbox(mb2);
    assert(isEqualInt(x, NOT_EMPTY));
    
    //check so that recieve function works and return ok aswell as the correct value
    x= receive_no_wait(mb2, (void *) &nData_t3);
    assert(isEqualInt(x, OK));
    //check so that no of msg is 2
    assert(isEqualInt(mb2->nMessages, 1));
    //check no blocked msg
    assert(isEqualInt(mb2->nBlockedMsg, 0));
    //check message in mb
    assert(isEqualInt(nData_t3, TEST_PATTERN_4));
    
    //check so that recieve function works and return ok aswell as the correct value
    x= receive_no_wait(mb2, (void *) &nData_t3);
    assert(isEqualInt(x, OK));
    //check so that no of msg is 2
    assert(isEqualInt(mb2->nMessages, 0));
    //check no blocked msg
    assert(isEqualInt(mb2->nBlockedMsg, 0));
    //check message in mb
    assert(isEqualInt(nData_t3, TEST_PATTERN_6));
    
    /**
     *attempt to send an message and recieve it from a different task!
     */
    
    //check so that recieve function works and return ok aswell as the correct value
    x = send_no_wait(mb2, (void *) &nData_t4);
    assert(isEqualInt(x, OK));
    //check so that no of msg is 2
    assert(isEqualInt(mb2->nMessages, 1));
    //check no blocked msg
    assert(isEqualInt(mb2->nBlockedMsg, 0));
    
    //switching to task b
    wait2(10);
    
    //test delete of mailbox should suceed since its empty
    x = remove_mailbox(mb2);
    assert(isEqualInt(x, OK));
    
    //this confirms sending works aswell as the mailbox structure. okey. lets start next test
    terminate();
}

//this task recieves the msg sent by task task_test_nowait_a
void task_test_nowait_b(void)
{
    
    volatile int nData_t7 =0;
    int x = receive_no_wait(mb2,(void *) &nData_t7);
    assert(isEqualInt(x, OK));
    assert(isEqualInt(nData_t7, TEST_PATTERN_4));
    
    terminate();
}

//this task combined with task_test_wait_b tests the integrity of mailbox structure combined with send recieve wait functions
void task_test_wait_a(void)
{
    wait2(15);
    volatile int nData_t1 = TEST_PATTERN_1;
    
    //check that mailbox creation suceeds
    
      mb=create_mailbox(1,sizeof(int));
     assert(isNotEqualPointer(mb, NULL));
    
    wait2(10);
    /* task2 börjar köra */
    
    
    if( no_messages(mb) != -1 ) terminate();/* ERROR */
    
    int x = send_wait(mb,(void *) &nData_t1);
    if(x == DEADLINE_REACHED)
        terminate(); /* ERROR */
    
    wait2(10); /* task2 börjar köra */ /* start test 2 */
    nData_t1 = TEST_PATTERN_2;
    
    if(send_wait(mb,(void *)&nData_t1) == DEADLINE_REACHED)
        terminate(); /* ERROR */
    wait2(10); /* task2 börjar köra */
    
    
    /* start test 3 */
    if(send_wait(mb,(void *)&nData_t1)==DEADLINE_REACHED) {
        
        
        if( no_messages(mb) != 0 )
            terminate(); /* ERROR */
        nTest3 = 1;
        
        
        if (nTest1*nTest2*nTest3) {
            /* Blinka lilla lysdiod */
            /* Test ok! */
        }
        terminate(); /* PASS, no receiver */ }
    
    
    else
    {
        terminate(); /* ERROR */ }
}

//this should be combined with task_test_wait_a
void task_test_wait_b(void)
{
    
    wait2(16);
    volatile int nData_t2 = 0;
    
    if(receive_wait(mb,(void *)&nData_t2) ==
       DEADLINE_REACHED) /* t1 kör nu */
        terminate(); /* ERROR */
    
    if( no_messages(mb) != 0 )
        terminate(); /* ERROR */
    
    if (nData_t2 == TEST_PATTERN_1)
        nTest1 = 1;
    wait2(20); /* t1 kör nu */
    
    
    /* start test 2 */
    if( no_messages(mb) != 1 )
        terminate(); /* ERROR */
    
    if(receive_wait(mb,(void *) &nData_t2) == DEADLINE_REACHED) /* t1 kör nu */
        terminate(); /* ERROR */
    if( no_messages(mb) != 0 )
        terminate(); /* ERROR */
    
    
    if (nData_t2 == TEST_PATTERN_2) 
      nTest2 = 1;
    
    
    /* Start test 3 */
    terminate();
    
}

void task_test_bothcombined_a(void)
{
    //sleep untill 2500 ticks has passed
    wait2(2500);
    
    volatile int nData_tt1 = TEST_PATTERN_7;
    
    /* 1*/
    //test delete of mailbox should suceed since its empty
    //then switch to task b to create mailbox
    int x =remove_mailbox(mb);
    assert(isEqualInt(x, OK));

    wait2(10);
    
    /*2*/
    //send mail and switch to task_test_bothcombined_task_b
    x =send_wait(mb3, (void *) &nData_tt1);
    assert(isEqualInt(x, OK));
    
    wait2(10);
    
    /*3*/
    //test with an recieve wait message in the mailbox and send no wait
    nData_tt1 = TEST_PATTERN_8;
    
    x = send_no_wait(mb3, (void *) &nData_tt1);
    assert(isNotEqualInt(x, DEADLINE_REACHED));
    
    //check the mail
    wait2(10);
    
    terminate();
}

void task_test_bothcombined_b(void)
{
    //sleep untill 2500 ticks has passed
    wait2(2500);
    
    volatile int nData_tt2 = 0;
    
    /*1*/
    //create mailbox and switch to task a to send mail
    mb3=create_mailbox(4,sizeof(int));
    assert(isNotEqualPointer(mb3, NULL));

    wait2(10);
    
    /*2*/
    //recieve the message and check so its correct
    int x = receive_no_wait(mb3, (void *) &nData_tt2);
    assert(isEqualInt(x, OK));
    
    assert(isEqualInt(nData_tt2, TEST_PATTERN_7 ));
    
    /*3*/
    //put an recieve wait message in the mailbox
    
    nData_tt2 = 0;
    
    x = receive_wait(mb3, (void *) &nData_tt2);
    
    assert(isEqualInt(x, OK));
    
    //automaticaly switch to task.
    
    //check the content
    assert(isEqualInt(nData_tt2, TEST_PATTERN_8 ));
    
    terminate();
    

}


void test_rtos(void)
{
    
    
    
    if(init_kernel() != OK)
    {
        /* Memory allocation problems */
        while(1);
    }
    
    if(create_task( task_test_nowait_a, 1000 ) != OK )
    {
        /* Memory allocation problems */
        while(1);
    }
    if (create_task( task_test_nowait_b, 1500 ) != OK )
    {
        /* Memory allocation problems */
        while(1);
    }
    
    if(create_task( task_test_wait_a, 2000 ) != OK )
    {
        /* Memory allocation problems */
        while(1);
    }
    if (create_task( task_test_wait_b, 4000 ) != OK )
    {
        /* Memory allocation problems */
        while(1);
    }
    if (create_task( task_test_bothcombined_a, 6000 ) != OK )
    {
        /* Memory allocation problems */
        while(1);
        
    }
    if (create_task( task_test_bothcombined_b, 8000 ) != OK )
    {
        /* Memory allocation problems */
        while(1);
    }
    
    run(); /* First in readylist is task1 */
    
}
