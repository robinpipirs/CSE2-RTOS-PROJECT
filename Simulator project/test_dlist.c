//
//  test_dlist.c
//  CSE2 embedded os
//
//  Created by Robin Pipirs on 2015-02-03.
//  Copyright (c) 2015 Robin Pipirs. All rights reserved.
//

#include "test_dlist.h"
#include "dlist.h"
#include "test_functions.h"
#include "assert.h"




//the whole test procedure
void list_test(void)
{
    initiate_lists();
    test_create_list();
    test_create_listobj();
    test_readyList();
    test_waitingList();
    test_timerList();
}


/* Help functions*/

void initiate_lists(void)
{
    readyList = create_list();
    waitingList = create_list();
    timerList = create_list();
    
}

//test create_list & dummy nodes
void test_create_list(void)
{
    //check so their not empty
    assert(isNotEqualPointer(readyList, NULL));
    assert(isNotEqualPointer(waitingList, NULL));
    assert(isNotEqualPointer(timerList, NULL));
    
    //check that their copys of the same list
    assert(isNotEqualPointer(readyList, waitingList));
    assert(isNotEqualPointer(waitingList,timerList));
    assert(isNotEqualPointer(timerList,readyList));
    
    //check for dummy nodes in readylist, waitinglist and timerlist
    assert(isEmptyList(readyList));
    assert(isEmptyList(waitingList));
    assert(isEmptyList(timerList));
    
}

//test create_listobj
void test_create_listobj(void){
    
    TCB *pTask = (TCB *) calloc(1, sizeof(TCB));
    
    //check if the task is created
    assert(isNotEqualPointer(pTask, NULL) && "Failed Creation of TCB in test_create_listobj");
    
    listobj *pObj = create_listobj(pTask);
    
    //check create_listobj function
    assert(isNotEqualPointer(pObj, NULL) && "Failed creating listobj in test_create_listobj");
    
    //check so that the task inside the obj is  the pTask
    assert(isEqualPointer(pObj->pTask, pTask) && "The pointer to the task isnt the same as the task inserted - test_Create_listobj");
    
    
    //free to avoid memleaks
    free(pObj->pTask);
    free(pObj);
    
}

//test insert readylist function
void test_readyList(void)
{
    
    //insert 100 TCB with deadline 100->110 into the readylist
    for (int i=0; i<100; i++)
    {
        
        TCB *pTask = (TCB *) calloc(1, sizeof(TCB));
        pTask->DeadLine = 100 + i;
        
        //check if the task is created
        assert(isNotEqualPointer(pTask, NULL) && "Failed Creation of TCB in test_readylist");
        
        listobj *pObj = create_listobj(pTask);
        
        //check create_listobj function
        assert(isNotEqualPointer(pObj, NULL) && "Failed creating listobj in test_readylist");
        
        //check so that the task inside the obj is  the pTask
        assert(isEqualPointer(pObj->pTask, pTask) && "The pointer to the task isnt the same as the task inserted - test_readylist");
        
        insert_readyList(pObj);
        
    }
    
    //test extract
    for (int i = 0; i <100; i++) {
        
        listobj *pObj = extract_readyList();
        
        //test so that the Deadline is correct according to the insert
        assert(isEqualInt(pObj->pTask->DeadLine, (100 + i)));
        
        //free memory
        free(pObj->pTask);
        free(pObj);
        
    }
    
}


//test insert waitinglist function
void test_waitingList(void)
{
    //insert 100 TCB with deadline 100->200 into the waitinglist
    for (int i=0; i<100; i++)
    {
        
        TCB *pTask = (TCB *) calloc(1, sizeof(TCB));
        pTask->DeadLine = 100 + i;
        
        //check if the task is created
        assert(isNotEqualPointer(pTask, NULL) && "Failed Creation of TCB in test_create_listobj");
        
        listobj *pObj = create_listobj(pTask);
        
        //check create_listobj function
        assert(isNotEqualPointer(pObj, NULL) && "Failed creating listobj in test_create_listobj");
        
        //check so that the task inside the obj is  the pTask
        assert(isEqualPointer(pObj->pTask, pTask) && "The pointer to the task isnt the same as the task inserted - test_Create_listobj");
        
        insert_waitingList(pObj);
        
    }
    
    //traverse list
    listobj *pTraverse = waitingList->pHead->pNext;
    
    //extract the items from the waitinglist
    while(pTraverse->pNext != pTraverse){
        
        listobj *pObj = extract_waitingList(pTraverse);

        //test so the extract is correct
        assert(isEqualPointer(pTraverse, pObj));
        
        //free memory
        free(pObj->pTask);
        free(pObj);
        
        pTraverse = pTraverse->pNext;
        
        
    }
    
}

void test_timerList(void)
{
    //insert 100 TCB with deadline 100->200 into the timerlist with nTCnt ranging from 0->100
    for (int i=0; i<100; i++)
    {
        
        TCB *pTask = (TCB *) calloc(1, sizeof(TCB));
        pTask->DeadLine = 100 + i;
        
        //check if the task is created
        assert(isNotEqualPointer(pTask, NULL) && "Failed Creation of TCB in test_create_listobj");
        
        listobj *pObj = create_listobj(pTask);
        
        //check create_listobj function
        assert(isNotEqualPointer(pObj, NULL) && "Failed creating listobj in test_create_listobj");
        
        //check so that the task inside the obj is  the pTask
        assert(isEqualPointer(pObj->pTask, pTask) && "The pointer to the task isnt the same as the task inserted - test_Create_listobj");
        
        insert_timerList(pObj,i);
        
    }
    
    
    //traverse list
    listobj *pTraverse = timerList->pHead->pNext;
    
    //nTCnt
    uint i =0;
    
    //extract the items from the waitinglist
    while(pTraverse->pNext != pTraverse){
        
        listobj *pObj = extract_timerList();
        
        //test so the extract is correct
        assert(isEqualPointer(pTraverse, pObj));
        
        //test so that the nTCnt is correct
        assert(isEqualInt(pObj->nTCnt, i));
        
        
        pTraverse = pTraverse->pNext;
        
        
        //free memory
        free(pObj->pTask);
        free(pObj);
        
        
        
        i++;
    }
}








