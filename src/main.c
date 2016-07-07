/*
    IOTC - Copyright (C) 2016
    Neptun TTT Kft.            
*/

#include <stdio.h>
#include <string.h>

#include "ch.h"
#include "hal.h"

#include "meas.h"
#include "can_comm.h"

static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (true) {
    //palClearPad(GPIOA, GPIOA_PO_L);
    //palTogglePad(GPIOA, GPIOA_PO_H);
    chThdSleepMilliseconds(250);
    //palClearPad(GPIOA, GPIOA_PO_L);
    //palSetPad(GPIOA, GPIOA_PO_H);
    //chThdSleepMilliseconds(300);
  }
  return 0; /* Never executed.*/
}
/*
 * 20ms Task
 */
static THD_WORKING_AREA(watask20ms, 256);
static THD_FUNCTION(task20ms, arg) {
  systime_t time; 

  (void)arg;
  chRegSetThreadName("task20ms");
  time = chVTGetSystemTime();  
  while (true) {
    time += MS2ST(1);
    
    measCalc();

    //palClearPad(GPIOA, GPIOA_PO_L);
    //palSetPad(GPIOA, GPIOA_PO_H);
    //chThdSleepMilliseconds(300);
    chThdSleepUntil(time);
  }
  return 0; /* Never executed.*/
}

int main(void) {

  halInit();
  chSysInit();
  /*
   * Initialize the Analog measurement module.
   */
  measInit();

  /*
   * CAN bus initialization.
   */
  can_commInit();

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  /*
   * Creates the 1ms Task.
   */
  chThdCreateStatic(watask20ms, sizeof(watask20ms), NORMALPRIO, task20ms, NULL);


  while (true) {
    chThdSleepMilliseconds(1000);
  }
}