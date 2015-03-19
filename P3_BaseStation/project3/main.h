/* 
 * User Space Defintions and Constants. 
 * 
 * 
 */ 

#include "test.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


/* r_main is the main entry point to the users code. It should be responsible for the setup
 * of the user space and initialize other tasks, then return to start system execution */
int r_main();