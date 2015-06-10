//
//  test_dlist.h
//  CSE2 embedded os
//
//  Created by Robin Pipirs on 2015-02-03.
//  Copyright (c) 2015 Robin Pipirs. All rights reserved.
//

#ifndef __CSE2_embedded_os__test_dlist__
#define __CSE2_embedded_os__test_dlist__

#include <stdio.h>


//test procedure
void list_test(void);


//test moments
void initiate_lists(void);
void test_create_list(void);
void test_create_listobj(void);
void test_readyList(void);
void test_waitingList(void);
void test_timerList(void);


#endif /* defined(__CSE2_embedded_os__test_dlist__) */
