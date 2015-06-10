//
//  dlist.h
//  CSE2 embedded os
//
//  Created by Robin Pipirs on 2015-02-02.
//  Copyright (c) 2015 Robin Pipirs. All rights reserved.
//

#ifndef __CSE2_embedded_os__dlist__
#define __CSE2_embedded_os__dlist__

#include <stdio.h>
#include "stdlib.h"
#include "kernel.h"

// Generic list item
struct l_obj {
    TCB            *pTask;
    uint           nTCnt;
    msg            *pMessage;
    struct l_obj   *pPrevious;
    struct l_obj   *pNext;
};
typedef struct l_obj listobj;


// Generic list
typedef struct{
    listobj        *pHead;
    listobj        *pTail;
}list;

// function prototype


//waiting list
extern list *waitingList;
extern list *readyList;
extern list *timerList;


//list administration
list * create_list();
void list_destroy(list *list);
void list_dummyNodes(list *list);

//delete these functions
void list_printAll(list *list);
void list_printAll_Bw(list *list);

//insert functions
void insert_readyList(listobj *newObj);
void insert_timerList(listobj *newObj, int nTCnt);
void insert_waitingList(listobj *newObj);
void insert_emptyList(list *list, listobj *newNode);

//extract and alloc functions
listobj *extract_first(list *pList);
listobj *extract_timerList();
listobj *extract_readyList();
listobj *extract_waitingList(listobj *pBlock);

listobj * create_listobj(TCB *pTask);

/* HELP FUNCTION */
int isEmptyList(list *plist);

#endif /* defined(__CSE2_embedded_os__dlist__) */
