//
//  main.c
//  CSE2 embedded os
//
//  Created by Robin Pipirs on 2015-01-21.
//  Copyright (c) 2015 Robin Pipirs. All rights reserved.
//


#define TEST_PATTERN_1 0xAA
#define TEST_PATTERN_2 0x55

#include <stdio.h>
#include <stdlib.h>
#include "kernel.h"
#include "dlist.h"
#include <math.h>

//for application purpose

//used by recieve send wait test
mailbox *mbi;
mailbox *mbf;

//tasks
void car_movement(void);
void cruisecontrol(void);

//variables
volatile double currentspeed = 10;
volatile double targetspeed = 100;
volatile double friction_drag_etc = -0.98;
volatile int target_reached =0;
int main(void) {
    
    if(init_kernel() != OK)
    {
        /* Memory allocation problems */
        while(1);
    }
    
    if(create_task( car_movement, 3000 ) != OK )
    {
        /* Memory allocation problems */
        while(1);
    }
    if (create_task(cruisecontrol, 3500 ) != OK )
    {
        /* Memory allocation problems */
        while(1);
    }
    if ((mbi=create_mailbox(1,sizeof(int))) == NULL) {
        /* Memory allocation problems */
        while (1);
    }
    if ((mbf=create_mailbox(1,sizeof(double))) == NULL) {
        /* Memory allocation problems */
        while (1);
    }
    
    run();
    
    
    return 0;
}

void car_movement(void)  {
    
    volatile int throttle_cm;
    volatile int target = 1;
    //if we havnt reached our target speed
    while (target) {
      if((abs((currentspeed/targetspeed)-1)<0.5) && (currentspeed <102) && (currentspeed > 99))  
      {target = 0;}
      //send the current speed
        send_wait(mbf, (void *)&currentspeed);
        
        //communicating with the cruise controll
        //recieve instructions for the trottle
        receive_no_wait(mbi, (void *)&throttle_cm);
        
        
        //if throttle is positive incr the speed
        if (throttle_cm > 0) {
            currentspeed = currentspeed + throttle_cm + friction_drag_etc;
        }
        //if throttle decr the speed
        else
            currentspeed = currentspeed + throttle_cm + friction_drag_etc;
    }
    target_reached =1;
    terminate();
    
}

void cruisecontrol(void)  {
    
    volatile int throttle_cc = 0;
    volatile double thespeed = 0;
    
    //if we havnt reached our target speed
    while (target_reached !=1) {
        
        receive_wait(mbf, (void*) &thespeed);
        
        //if current speed is lower then the target speed increase the acceleration by pushing the throttle more
        if (thespeed < targetspeed) {
            
            //throttle 9 max acceleration
            if (throttle_cc < 10) {
                throttle_cc++;
                send_no_wait(mbi,(void *) &throttle_cc);
            }
        }
        else if(throttle_cc >= 0)
        {
            throttle_cc--;
            send_no_wait(mbi,(void *)&throttle_cc);
        }
        
    }
    
    terminate();
}

