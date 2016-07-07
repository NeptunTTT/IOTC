/*
    IOTC - Copyright (C) 2016
    Neptun TTT Kft.            
*/

#ifndef MEAS_H_INCLUDED
#define MEAS_H_INCLUDED

#include "ch.h"
#include "hal.h"

enum measChannels
{
  MEAS_AIN1,
  MEAS_AIN2,
  MEAS_AIN3,
  MEAS_AIN4,
  MEAS_AIN5,
  MEAS_NUM_CH
};

void measInit(void);
void measCalc(void);

int16_t measGetValue(enum measChannels ch);
int16_t measInterpolateVoltage(adcsample_t rawvalue);

#endif