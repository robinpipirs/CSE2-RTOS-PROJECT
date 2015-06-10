//
//  test_functions.c
//  CSE2 embedded os
//
//  Created by Robin Pipirs on 2015-02-03.
//  Copyright (c) 2015 Robin Pipirs. All rights reserved.
//

#include "test_functions.h"

//compares 2 pointers
int isEqualPointer( void * comp1, void * comp2)
{
    if (comp1 == comp2) {
        return 1;
    }
    else
        return 0;

}

//compares 2 pointers
int isNotEqualPointer( void * comp1, void * comp2)
{
    if (comp1 != comp2) {
        return 1;
    }
    else
        return 0;
    
}

//compare 2 ints
int isEqualInt(int comp1, int comp2)
{
    if (comp1 == comp2) {
        return 1;
    }
    else
        return 0;
}

//compare 2 ints
int isNotEqualInt(int comp1, int comp2)
{
    if (comp1 != comp2) {
        return 1;
    }
    else
        return 0;
}

