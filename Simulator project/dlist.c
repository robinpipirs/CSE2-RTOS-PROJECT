//
//  dlist.c
//  CSE2 embedded os
//
//  Created by Robin Pipirs on 2015-02-02.
//  Copyright (c) 2015 Robin Pipirs. All rights reserved.
//

#include "dlist.h"

list *waitingList;
list *readyList;
list *timerList;

//create a new list
list * create_list()
{
    list *tempList = (list* ) calloc(1, sizeof(list));
    
    //check if the allocation worked
    if (tempList == NULL) {
        return NULL;
    }
    
    //allocation of dummy nodes
    
    //set head and tail
    tempList->pHead = (listobj*) calloc(1, sizeof(listobj));
    
    //if allocation fails. clean up
    if (tempList->pHead == NULL) {
        free(tempList);
        return NULL;
    }
    
    tempList->pTail = (listobj*) calloc(1, sizeof(listobj));
    
    //if allocation fails. clean up
    if (tempList->pTail == NULL) {
        free(tempList->pHead);
        free(tempList);
        return NULL;
    }
    
    //set dummy links
    tempList->pHead->pPrevious = tempList->pHead;
    tempList->pHead->pNext = tempList->pTail;
    tempList->pTail->pNext = tempList->pTail;
    tempList->pTail->pPrevious = tempList->pHead;
    
        return tempList;
}


//create new list Object
listobj * create_listobj(TCB *pTask)
{
    listobj* tempObject = (listobj*) calloc(1, sizeof(listobj));
    
    //if allocation of memory failed return NULL
    if(tempObject == NULL)
    {
        return FAIL;
    }
    
    //allocation succeeded
    else
    {
        
        tempObject->pTask = pTask;
        
        return tempObject;
    }
}


//insert inside empty list
void insert_emptyList(list *list, listobj *newNode)
{
    
    list->pHead->pNext = newNode;
    list->pTail->pPrevious =newNode;
    newNode->pNext = list->pTail;
    newNode->pPrevious = list->pHead;
}


//insert an task in the timer list in the right spot according to the nTCnt
void insert_timerList(listobj *newObj,  int nTCnt)
{
    
    //check if allocation suceeeds else abort
    if (newObj == FAIL)
    {
        return;
    }
    
    //set nTCnt
    newObj->nTCnt = nTCnt;
    
    //set traverse node
    listobj *pObj = timerList->pHead->pNext;
    
    //check if the list is empty if so use insert Empty function
    if(pObj->pNext == pObj)
    {
        insert_emptyList(timerList, newObj);
        return;
    }
    
    else{
        
        //traverse the list && = boolean values, & calculates both and do and after
        //TODO theres some error here
        
        while( (pObj->pNext != pObj) && (pObj->nTCnt < nTCnt) )
        {
            pObj = pObj->pNext;
        }
        
        //found the spot time to insert
        pObj->pPrevious->pNext = newObj;
        newObj->pPrevious = pObj->pPrevious;
        newObj->pNext = pObj;
        pObj->pPrevious = newObj;
        
        return;
    }
}

//inserts the tcb in the readylist sorted by DEADLINE
void insert_readyList(listobj *newObj)
{
    
    //check if allocation suceeeds else abort
    if (newObj == FAIL)
    {
        return;
    }
    
    //set traverse node
    listobj *pObj = readyList->pHead->pNext;
    
    //check if the list is empty if so use insert Empty function
    if(pObj->pNext == pObj)
    {
        insert_emptyList(readyList, newObj);
        return;
    }
    
    else{
        
        //traverse the list && = boolean values, & calculates both and do and after
        //TODO theres some error here
        while( (pObj->pNext != pObj) && (pObj->pTask->DeadLine < newObj->pTask->DeadLine) )
        {
            pObj = pObj->pNext;
        }
        
        //found the spot time to insert
        pObj->pPrevious->pNext = newObj;
        newObj->pPrevious = pObj->pPrevious;
        newObj->pNext = pObj;
        pObj->pPrevious = newObj;
        
        return;
    }
}

//insert waitinglist sorted by DEADLINE
void insert_waitingList(listobj *newObj)
{
    
    //check if allocation suceeeds else abort
    if (newObj == FAIL)
    {
        return;
    }
    
    //set traverse node
    listobj *pObj = waitingList->pHead->pNext;
    
    //check if the list is empty if so use insert Empty function
    if(pObj->pNext == pObj)
    {
        insert_emptyList(waitingList, newObj);
        return;
    }
    
    else{
        
        //traverse the list && = boolean values, & calculates both and do and after
        //TODO theres some error here
        while( (pObj->pNext != pObj) && (pObj->pTask->DeadLine < newObj->pTask->DeadLine) )
        {
            pObj = pObj->pNext;
        }
        
        //found the spot time to insert
        pObj->pPrevious->pNext = newObj;
        newObj->pPrevious = pObj->pPrevious;
        newObj->pNext = pObj;
        pObj->pPrevious = newObj;
        
        return;
    }
}

//print all backwards
void list_printAll_Bw(list *list)
{
    
    //check if list exists
    if (list == NULL) {
        return;
    }
    else{
        
        listobj *tempNode = list->pTail->pPrevious;
        
        //traverse list
        while(tempNode->pPrevious != tempNode){
            printf("nId =nTCnt is: %d and Deadline is: %d \n",tempNode->nTCnt, tempNode->pTask->DeadLine);
            tempNode = tempNode->pPrevious;
        }
    }
}





//print all
void list_printAll(list *list)
{
    
    //check if list exists
    if (list == NULL) {
        return;
    }
    else{
        
        listobj *tempNode = list->pHead->pNext;
        
        //traverse list
        while(tempNode->pNext != tempNode){
            printf("nId =nTCnt is: %d and Deadline is: %d \n",tempNode->nTCnt, tempNode->pTask->DeadLine);
            tempNode = tempNode->pNext;
        }
    }
}


//TODO function that destroys the list
//TODO FREE TBC
void list_destroy(list *list)
{
    
    //check if list exists
    if (list == NULL) {
        return;
    }
    
    else
    {
        
        listobj *tempNode = list->pTail;
        listobj *tempNode2 = list->pTail;
        
        //traverse and free
        while(tempNode2->pPrevious != tempNode2)
        {
            tempNode2 = tempNode2->pPrevious;
            
            if(tempNode->pTask != NULL)
            {
                free(tempNode->pTask);
                
            }
            
            if(tempNode->pMessage != NULL)
            {
                free(tempNode->pMessage);
            }
            
            free(tempNode);
            
            tempNode = tempNode2;
            
        }
        
        tempNode = list->pHead;
        
        free(tempNode);
        free(list);
        
        return;
    }
    
}


listobj *extract_timerList()
{
    return extract_first(timerList);
}


listobj *extract_readyList()
{
    return extract_first(readyList);
}



//extracts the first object of an specific list
listobj *extract_first(list *pList)
{
    
    //check if list exists
    if (pList == NULL) {
        return NULL;
    }
    //list exists
    else
    {
        
        
        listobj *tempElement = pList->pHead;
        listobj *element = tempElement->pNext;
        tempElement->pNext = element->pNext;
        element->pNext->pPrevious = tempElement;
        
        return element;
    }
}
//TODO Delete this method, must be better to fix this in create task
//function that finds an task and returns the list obj containing it
listobj *extract_task(list *pList, TCB *pTask)
{
    
    
    //check if list exists
    if (pList == NULL) {
        return NULL;
    }
    
    else
    {
        //go to first real node
        listobj *tempNode = waitingList->pHead->pNext;
        
        //traverse the list && = boolean values, & calculates both and do and after
        
        while(tempNode->pNext != tempNode)
        {
            //found the object
            if(tempNode->pTask == pTask){
                
                //found the item time to extract
                listobj *element = tempNode;
                
                //rechaining the linked list
                tempNode->pPrevious->pNext = tempNode->pNext;
                tempNode->pNext->pPrevious = tempNode->pPrevious;
                
                
                //extracting
                return element;
                
            }
            else{
                
                tempNode = tempNode->pNext;
                
            }
            
            
        }
        
        //didnt find
        return NULL;
        
    }
}


//function that finds an object and returns it
listobj *extract_waitingList(listobj *pBlock)
{
    
    
    //check if list exists
    if (waitingList == NULL) {
        return NULL;
    }
    
    else
    {
        //go to first real node
        listobj *tempNode = waitingList->pHead->pNext;
        
        //traverse the list && = boolean values, & calculates both and do and after
        
        while(tempNode->pNext != tempNode)
        {
            //found the object
            if(tempNode == pBlock){
                
                //found the item time to extract
                listobj *element = tempNode;
                
                //rechaining the linked list
                tempNode->pPrevious->pNext = tempNode->pNext;
                tempNode->pNext->pPrevious = tempNode->pPrevious;
                
                //extracting
                return element;
                
            }
            else{
                
                tempNode = tempNode->pNext;
                
            }
            
            
        }
        
        //didnt find
        return NULL;
        
    }
}

//check if the list is empty "THIS IS AN TEST FUNCTION"
int isEmptyList(list *plist)
{
    //check if the list is empty. also if its built with dummy nodes
    if (isEqualPointer(plist->pHead->pNext, plist->pTail) && isEqualPointer(plist->pTail->pPrevious, plist->pHead) && isEqualPointer(plist->pHead->pPrevious, plist->pHead) && isEqualPointer(plist->pTail->pNext, plist->pTail))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}