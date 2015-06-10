//
//  kernel.c
//  CSE2 embedded os
//
//  Created by Robin Pipirs on 2015-01-28.
//  Copyright (c) 2015 Robin Pipirs. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "kernel.h"
#include "dlist.h"
#include <limits.h>

/** $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
 $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
 $ List functions and object functions $
 $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
 $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */


//variables
TCB * Running;

uint tick_counter;
uint state;



/* DUMMY FUNCTION  DELETE When hardware is connected*/

uint set_isr(uint set)
{
  return set;
}

void timer0_start(void)
{

}

/*TASK ADMINISTRATION*/

void Idle_Task(void)
{
    SaveContext();
    TimerInt();
    LoadContext();
}


//initiate kernel
int init_kernel(void)
{

    
    //if kernel is in startup mode return fail
    if(readyList!= NULL && waitingList != NULL && timerList != NULL )
    {
        return FAIL;
    }
    
    //Set tickcounter to zero
    set_ticks(0);
    
    //Create Nescessary data structures
    
    //initiate lists
    readyList = create_list();
    
    waitingList = create_list();

    timerList = create_list();
    
    //Create an idle task
    create_task(Idle_Task, UINT_MAX);
    
    //Set the kernel in startup mode
    state = INIT;
    
    //return ok
    return OK;
}

exception create_task( void (* body)(), uint d )
{
    volatile int first = TRUE;
    
    //turn isr off
    uint x = set_isr(OFF);
 
    //ALLOCATE MEMORY FOR TCB
    TCB *taskTCB = (TCB *) calloc(1, sizeof(TCB));
    
    //turn isr on
    set_isr(x);
    
    if (taskTCB == NULL)
    {
        return FAIL;
    }
    
    //SET DEADLINE IN TCB
    taskTCB->DeadLine =d;
    
    //SET THE TCBS PC TO POINT TO THE TASK BODY
    taskTCB->PC = body;
    
    //SET TCB's SP to point to the stack segment
    taskTCB->SP = &(taskTCB->StackSeg[STACK_SIZE-1]);
    
    //IF Startup mode then
    if(state == INIT)
    {
        
        //insert new task in readylist
        insert_readyList(create_listobj(taskTCB));
        
        //return status
        return OK;
    }
    else
    {

        //save context
        SaveContext();
                
        if (first) {
          
            first = FALSE;
            
            //turn interupts off
            set_isr(OFF);
            
            insert_readyList(create_listobj(taskTCB));
            
            //check if the task is higher priority then the running
            if (taskTCB->DeadLine < Running->DeadLine) {
                Running = taskTCB;
            }
            
            //load and enable interupts
            LoadContext();
        }
    
    }
    return OK;
}


//terminates the running task
void terminate( void )
{
    //disable interupts
    set_isr(OFF);
    
    
    //removes and frees the running task from readylist
    listobj *terminateObj = extract_readyList();
    free(terminateObj->pTask);
    free(terminateObj);
    
    //set the next task to be running task
    Running = readyList->pHead->pNext->pTask;
    
    //load context
   LoadContext();
}

//this function starts the kernel
void run( void )
{
    //initialize interupt timer
    timer0_start();
    
    //Set the kernel in running mode
    state = RUNNING;
    
    Running = readyList ->pHead->pNext->pTask;
    
    //Enable interupts
    set_isr(ON);
    
    //Load Context
    LoadContext();

}


/*COMUNICATION*/

mailbox * create_mailbox( uint nMessages, uint nDataSize )
{
    //turn isr off
    uint x = set_isr(OFF);
    
    //allocate memory for the mailbox
    mailbox *mb = (mailbox *) calloc(1, sizeof(mailbox));
    
    if (mb == NULL) {
        
        //turn isr on
        set_isr(x);
        
        return NULL;
    }
    //initialize mailbox structure
    mb->nDataSize = nDataSize;
    mb->nMaxMessages = nMessages;
    //create nodes
    mb->pHead = (msg *) calloc(1, sizeof(msg));
    
    //creation of nodes failed
    if (mb->pHead == NULL) {
        free(mb);
        return NULL;
    }
    
    mb->pTail = (msg *) calloc(1, sizeof(msg));
    
    //creation of tail node failed clean
    if (mb->pTail == NULL)
    {
        free(mb->pHead);
        free(mb);
        return NULL;
    }
             
    //set links
    mb->pHead->pNext = mb->pTail;
    mb->pHead->pPrevious = mb->pHead;
    mb->pTail->pPrevious = mb->pHead;
    mb->pTail->pNext = mb->pTail;
    
    //turn isr on
    set_isr(x);
    
    //return mailbox
    return mb;
}

//returns the no of messages
int             no_messages( mailbox* mBox )
{
    return mBox->nMessages;
}

//removes the mailbox
exception remove_mailbox(mailbox* mBox)
{
    //check if mailbox is empty
    if(mBox->nMessages == 0)
    {
        //turn isr off
        uint x = set_isr(OFF);
        
        //free nodes and mailbox structure
        free(mBox->pTail);
        free(mBox->pHead);
        free(mBox);
        
        //turn isr on
        set_isr(x);
        
        return OK;
        
        
    }
    else
        return NOT_EMPTY;
}


//send wait function
//TODO check so that the mbox isnt full or blocked etc
exception  send_wait( mailbox* mBox, void* pData )
{
    //first time
    volatile int first = TRUE;
    
    //turn isr off
    set_isr(OFF);
    
    SaveContext();
    
    //if first time executing
    if (first) {
        first = FALSE;
        
        //if theres an recieving blocked task
        if (mBox->nBlockedMsg < 0 && mBox->nMessages <0) {
            
            //send data
            memcpy(mBox->pHead->pNext->pData, pData, mBox->nDataSize);
            
            //*mBox->pHead->pNext->pData = *(char*)pData;
            
            //remove recieving tasks msg
            
            //move recieving task into readylist
            listobj *pBlocked = extract_waitingList(mBox->pHead->pNext->pBlock);
            insert_readyList(pBlocked);
            
            //re-route links remove first item in mb
            mBox->pHead->pNext->pNext->pPrevious = mBox->pHead;
            mBox->pHead->pNext = mBox->pHead->pNext->pNext;
            
            free(pBlocked->pMessage);
         
            //update running pointer
            Running = readyList->pHead->pNext->pTask;
            
            //decr blocked msg this means remove an blocked task
            mBox->nBlockedMsg++;
            
            //incr no of msg on the mbox
            mBox->nMessages++;
            
            
        }
        
        //ELSE put mail in mailbox
        //What if mailbox is full? it will return fail
        else
        {
            //check so that the mailbox isnt full
            if (mBox->nMessages >= mBox->nMaxMessages) {
                return FAIL;
            }
            
            //allocate a msg structure
            msg *tMsg = (msg *) calloc(1,sizeof(msg));
            
            //check memory
            if(tMsg == NULL){
            
                //HANDLE THIS ERROR SOMEHOW
                return FAIL;
            }
            
            //link msg
            readyList->pHead->pNext->pMessage = tMsg;
            //Set data pointer
            tMsg->pData = pData;
            //set blocked link
            tMsg->pBlock = readyList->pHead->pNext;
            
            
            //add msg to the mailbox should be added last (FIFO)
            mBox->pTail->pPrevious->pNext = tMsg;
            tMsg->pPrevious = mBox->pTail->pPrevious;
            tMsg->pNext = mBox->pTail;
            mBox->pTail->pPrevious = tMsg;
            
            //move sending task from readylist to waitinglist
            insert_waitingList(extract_readyList());
            //update running pointer
            Running = readyList->pHead->pNext->pTask;
            
            //incr blocked msg and no of msg
            mBox->nBlockedMsg++;
            
            //decr no of msg on the mbox
            //this means theres an sending task
            mBox->nMessages++;
            
            
            
        }
        //load context
        LoadContext();
        
    }
    
    
    
    // Not first
    else
    {
    //TODO dont quite understand this
        
        
    //If theres an blocked task which deadline is reached
        if(deadline() <= ticks())
        {
            //disable interupts
            int x = set_isr(OFF);
            
            //re-link the msg
            mBox->pHead->pNext = mBox->pHead->pNext->pNext;
            mBox->pHead->pNext->pNext->pPrevious = mBox->pHead;

            free(readyList->pHead->pNext->pMessage);
            
            mBox->nBlockedMsg --;
            mBox->nMessages --;
            
            //enable interupts
            set_isr(x);
            //return deadline reached
            return DEADLINE_REACHED;
            
        }
        else
        {
            return OK;
        }
    }
    return OK;
}

exception   receive_wait( mailbox* mBox, void* pData )
{
    //first time
    volatile int first = TRUE;
    //Disable interrupt
    set_isr(OFF);
    //Save context
    SaveContext();
    
    //IF first execution THEN
    if (first) {
        
        //Set: ìnot first execution any more
        first = FALSE;
        
        //IF send Message is waiting THEN
        if (mBox->nMessages >0 ) {
            
            //Copy senderís data to receiving taskís data area
            memcpy(pData, mBox->pHead->pNext->pData, mBox->nDataSize);
            //*(char*)pData = *mBox->pHead->pNext->pData;
            
            //Remove sending taskís Message struct from the Mailbox
            msg *pMsg = mBox->pHead->pNext;
            
            mBox->pHead->pNext = mBox->pHead->pNext->pNext;
            mBox->pHead->pNext->pNext->pPrevious = mBox->pHead;
            
            //IF Message was of wait type THEN Move sending task to Ready list
            if (mBox->nBlockedMsg >0) {
                
                insert_readyList(extract_waitingList(pMsg->pBlock));
                
                //update running pointer
                Running = readyList->pHead->pNext->pTask;
                
                //free senders data area
                free(pMsg);
                mBox->nBlockedMsg--;
                mBox->nMessages--;
            }
            else{
                //Free senders data area
                free(pMsg);
                mBox->nMessages--;
            }
        
        }
        
        //put an recieve message to the mailbox
        //nMessages --
        //nBlocked ++
        else{
     
            //allocate a msg structure
            msg *tMsg = (msg *) calloc(1, sizeof(msg));
            
            //check memory
            if(tMsg == NULL){
                
                //HANDLE THIS ERROR SOMEHOW
                return FAIL;
            }
            
            //Set data pointer
            tMsg->pData = pData;
            
            //link the msg
            readyList->pHead->pNext->pMessage = tMsg;
            
            //set blocked link
            tMsg->pBlock = readyList->pHead->pNext;
            
            
            
            //add msg to the mailbox
            mBox->pTail->pPrevious->pNext = tMsg;
            tMsg->pPrevious = mBox->pTail->pPrevious;
            tMsg->pNext = mBox->pTail;
            mBox->pTail->pPrevious = tMsg;
            
            //Move receiving task from Readylist to Waitinglist
            insert_waitingList(extract_readyList());
            //update running pointer
            Running = readyList->pHead->pNext->pTask;
            
            mBox->nBlockedMsg--;
            mBox->nMessages--;
            
        }
        //Load context
        LoadContext();
    }
    else
    {
     
        //IF deadline is reached THEN
        if (ticks()>= deadline()) {
            //Disable interrupt
            int x = set_isr(OFF);
            //Remove receive Message
            msg *tMsg = mBox->pHead->pNext;
            
            mBox->pHead->pNext = mBox->pHead->pNext->pNext;
            mBox->pHead->pNext->pNext->pPrevious = mBox->pHead;
            
            free(tMsg);
            
            mBox->nBlockedMsg ++;
            mBox->nMessages ++;
            
            //Enable interrupt
            set_isr(x);
            
            //Return DEADLINE_REACHED
            return DEADLINE_REACHED;
        }
        
        else
        {
            return OK;
        }
    }
    return OK;
    
}

int	send_no_wait( mailbox* mBox, void* pData )
{
    //first time
    volatile int first = TRUE;
    //status
    volatile int status = FAIL;
    //Disable interrupt
    set_isr(OFF);
    //Save context
    SaveContext();
    //IF first execution THEN
    if (first) {
        //Set: înot first execution anymoreî
        first = FALSE;
        
        //IF  task is waiting for message
        if (mBox->nMessages<0) {
            status = OK;
            
            //Copy data to receiving tasksí data area.
            memcpy(mBox->pHead->pNext->pData, pData, mBox->nDataSize);
            
            
            //Move receiving task to Readylist
            listobj *pBlocked = extract_waitingList(mBox->pHead->pNext->pBlock);
            insert_readyList(pBlocked);
            
            //Remove receiving taskís Message struct from the Mailbox
            mBox->pHead->pNext->pNext->pPrevious = mBox->pHead;
            mBox->pHead->pNext = mBox->pHead->pNext->pNext;
     
            
            free(pBlocked->pMessage);
            
            //update running pointer
            Running = readyList->pHead->pNext->pTask;
            
            //decr blocked msg this means remove an blocked task
            mBox->nBlockedMsg++;
            mBox->nMessages++;
            
            //Load context
            LoadContext();
            
        }
        
        //none is waiting for message, put mail in mailbox
        else
        {
            status = OK;
            
            //allocate a msg structure
            msg *tMsg = (msg *) calloc(1,sizeof(msg));
            
            //check memory
            if(tMsg == NULL){
                
                //HANDLE THIS ERROR SOMEHOW
                return FAIL;
            }
            
            //Set data pointer
            tMsg->pData = pData;
            
            //if mailbox is full!
            if (mBox->nMessages >= mBox->nMaxMessages) {
                //Remove the oldest Message struct
                msg *pMsg = mBox->pTail->pPrevious;
                //reroute links
                mBox->pTail->pPrevious->pPrevious->pNext = mBox->pTail;
                mBox->pTail->pPrevious = mBox->pTail->pPrevious->pPrevious;
                
                free(pMsg);
                
                mBox->nMessages--;
            }
            
            //add msg to the mailbox
            mBox->pTail->pPrevious->pNext = tMsg;
            tMsg->pPrevious = mBox->pTail->pPrevious;
            tMsg->pNext = mBox->pTail;
            mBox->pTail->pPrevious = tMsg;

            //incr messages in mailbox
            mBox->nMessages++;
        
        }
        
    }
    return status;
}


#define bortkommenterat
#ifdef bortkommenterat

int receive_no_wait( mailbox* mBox, void* pData )
{
    //first time
    volatile int first = TRUE;
    //status
    volatile int status = FAIL;
    //Disable interrupt
    set_isr(OFF);
    //Save context
    SaveContext();
    
    //IF first execution THEN
    if (first){
        //Set: ìnot first execution any moreî
        first= FALSE;

        //IF send Message is waiting THEN
        if (mBox->nMessages >0){
            status = OK;
            //Copy senderís data to receiving taskís data area
            memcpy(pData, mBox->pHead->pNext->pData, mBox->nDataSize);
            //Remove sending taskís Message struct from the Mailbox
            msg *pMsg = mBox->pHead->pNext;
            //reroute links
            mBox->pHead->pNext = mBox->pHead->pNext->pNext;
            mBox->pHead->pNext->pNext->pPrevious = mBox->pHead;
            
            //IF Message was of wait type THEN Move sending task to Ready list
            if (mBox->nBlockedMsg >0) {
                
                insert_readyList(extract_waitingList(pMsg->pBlock));
                
                //update running pointer
                Running = readyList->pHead->pNext->pTask;
                
                mBox->nBlockedMsg--;
            }
            //free senders data area
            free(pMsg);
            mBox->nMessages--;
        }
        LoadContext();
    }
    return status;
    
}
#endif

/**   TIMING  */

//block the calling task untill the no of ticks has expired
exception	wait2( uint nTicks )
{
    volatile int first = TRUE;
    volatile int status = DEADLINE_REACHED;
    
    //turn isr off
    set_isr(OFF);
    
    //Save context
    SaveContext();
    
    //if first execution then
    if (first) {
        //set not first
        first = FALSE;
        
        //extract current task from readylist
        //place running task in waitlist
        insert_timerList(extract_readyList(), (nTicks+ticks()));
        
        //update running pointer?
        Running = readyList->pHead->pNext->pTask;
        
        //load context
        LoadContext();
    }
    //else
    else
    {
        
        //if deadline is reached
        if (deadline() == ticks()){
            status = DEADLINE_REACHED;
        }
        //status is OK
        else
            status = OK;
    }
    return status;
    
}


//set the tick_counter to the given value
void set_ticks( uint no_of_ticks )
{
    tick_counter = no_of_ticks;
}

//returns the value of the tick_counter
uint ticks( void )
{
    return tick_counter;
}

//returns the deadline of the current task
uint deadline( void )
{
    return Running->DeadLine;
}

//set deadline to the given number
void set_deadline( uint nNew )
{
    Running->DeadLine = nNew;
}

//Not avail for the user to call
void TimerInt(void)
{
    //incrm ticks
    tick_counter++;
    
    //check the timerlist for tasks that are ready for execution,
    //move these into readylist
    while ((timerList->pHead->pNext != timerList->pTail) && (timerList->pHead->pNext->nTCnt <= ticks())) {
        listobj *pObj = extract_timerList();
        insert_readyList(pObj);
        
    }
    
    //check the waitinglist for tasks that have expired deadlines
    while ((waitingList->pHead->pNext != waitingList->pTail) && (waitingList->pHead->pNext->pTask->DeadLine <= ticks())) {
        
        //move to readylist if the task has expired
        listobj *pObj = extract_waitingList(waitingList->pHead->pNext);
        insert_readyList(pObj);
        //clean the mailbox entrys
        
        
        //check if the moved task is of higher priority
        
    }
    
    //check if the moved task is of higher priority if so context switch
    if ((readyList->pHead->pNext->pTask != Running) && (readyList->pHead->pNext->pTask->DeadLine < Running->DeadLine)) {
        Running = readyList->pHead->pNext->pTask;
    }
    
}