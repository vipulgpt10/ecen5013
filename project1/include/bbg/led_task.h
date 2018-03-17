/**
 * @file led_task.h
 * @brief An abtraction for USR LED switching functions
 *
 * This header file provides the prototypes of the function for
 * switching USR LED0 on Beaglebone Green.
 *
 * @author Vipul Gupta
 * @date 17 March 2018
 *
 */

#ifndef __LIGHT_TASK_H__
#define __LIGHT_TASK_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Blue USR LED0 but green name! */
#define LED0_PATH "/sys/class/leds/beaglebone:green:usr0/brightness"

#define LED0_INITPATH "/sys/class/leds/beaglebone:green:usr0/trigger"


/**
 * @brief Turns on USR LED0
 *
 * It turns OFF the USR LED0 on Beaglebone Green.
 *
 */
void LED_ON(void);

/**
 * @brief Turns off USR LED0
 *
 * It turns OFF the USR LED0 on Beaglebone Green.
 *
 */
void LED_OFF(void);

/**
 * @brief Turns off USR LED0
 *
 * It turns OFF the USR LED0 on Beaglebone Green.
 *
 */
void LED_INIT(void);

#endif	/* __LIGHT_TASK_H__ */