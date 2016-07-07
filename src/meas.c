/*
    IOTC - Copyright (C) 2016
    Neptun TTT Kft.            
*/

#include "meas.h"
#include "chprintf.h"

#define ADC_GRP1_BUF_DEPTH      8

enum measStates
{
  MEAS_START,
  MEAS_RUNING
} measstate;

static int32_t measValue[MEAS_NUM_CH +2];
static adcsample_t samples[MEAS_NUM_CH * ADC_GRP1_BUF_DEPTH];

static void adcerrorcallback(ADCDriver *adcp, adcerror_t err);

/*
 * ADC conversion group.
 * Mode:        Linear buffer, 8 samples of 5 channel, SW triggered.
 * Channels:    IN1, IN2, IN3, IN4, IN5, IN6.
 * SMPR1 - CH 10...17,               |  SMPR2 - CH 0...9  |
 * SQR1 - 13...16 + sequence length, |  SQR2 - 7...12,    | SQR3 - 1...6
 */
static const ADCConversionGroup adcgrpcfg = {
  FALSE,
  MEAS_NUM_CH,
  NULL,
  adcerrorcallback,
  0, 0,                                                                     /* CR1, CR2 */
  0,                                                                        /* SMPR1 |                  |                   */
  ADC_SMPR2_SMP_AN2(ADC_SAMPLE_13P5) | ADC_SMPR2_SMP_AN3(ADC_SAMPLE_13P5) | /* SMPR2 | AN2-PA2-AIN_1    | AN3-PA3-AIN_2     */
  ADC_SMPR2_SMP_AN6(ADC_SAMPLE_13P5) | ADC_SMPR2_SMP_AN7(ADC_SAMPLE_13P5) | /* SMPR2 | AN6-PA6-AIN_3    | AN7-PA7-AIN_4     */
  ADC_SMPR2_SMP_AN8(ADC_SAMPLE_13P5),                                       /* SMPR2 | AN8-PB0-AIN_5    |                   */
  ADC_SQR1_NUM_CH(MEAS_NUM_CH),                                             /* SQR1  -----------Number of sensors---------- */
  0,                                                                        /* SQR2  |                  |                   */
  ADC_SQR3_SQ5_N(ADC_CHANNEL_IN8) |                                         /* SQR3  | 5. IN8-AIN_5      |                  */
  ADC_SQR3_SQ4_N(ADC_CHANNEL_IN7) | ADC_SQR3_SQ3_N(ADC_CHANNEL_IN6) |       /* SQR3  | 4. IN7-AIN_4      | 3. IN6-AIN_3     */
  ADC_SQR3_SQ2_N(ADC_CHANNEL_IN3) | ADC_SQR3_SQ1_N(ADC_CHANNEL_IN2)         /* SQR3  | 2. IN3-AIN_2      | 1. IN2-AIN_1     */
};
 
void measInit(void){
	/*
  * Activates the ADC1 driver.
  */
  adcStart(&ADCD1, NULL);
  measstate = MEAS_START;
}

void measCalc(void){
	int ch;
  int avg, i;
  /*
  * Starts the ADC conversion.
  */
  //Prociba menő fesz * 5,545 = tápfesz
  switch(measstate){

    case MEAS_START:
      measstate = MEAS_RUNING;
      break;
    case MEAS_RUNING:
      for(ch = 0; ch < MEAS_NUM_CH; ch++) {
        avg = 0;
        for(i = 0; i < ADC_GRP1_BUF_DEPTH; i ++) {
          avg += samples[ch + MEAS_NUM_CH * i];
        }
        avg /= ADC_GRP1_BUF_DEPTH;
        switch(ch){
          case MEAS_AIN1:
            break;
          case MEAS_AIN2:
            break;
          case MEAS_AIN3:
            break;
          case MEAS_AIN4:
            /*if (avg > 1900){
              palClearPad(GPIOA, GPIOA_PO_L);
              palSetPad(GPIOA, GPIOA_PO_H);
            }
            else{
              palClearPad(GPIOA, GPIOA_PO_L);
              palClearPad(GPIOA, GPIOA_PO_H);
            }*/
            break;
          case MEAS_AIN5:
            break;
          default:
            break;
        }
        chSysLock();
        measValue[ch] = (int16_t)avg;
        chSysUnlock();
      }
      break;
    default:
      break;
  }
  adcConvert(&ADCD1, &adcgrpcfg, samples, ADC_GRP1_BUF_DEPTH);
}

int16_t measGetValue(enum measChannels ch){
	  return measValue[ch];
}

/*
 * Calculate the minivolt from the ADC value
 */
/*int16_t measInterpolateVoltage(adcsample_t rawvalue){
  int16_t value = 0;
  adcsample_t left, right;
  int i;

  if(rawvalue >= measVOLTcalib[0]){
    value = MEAS_VOLTCAL_START;
  }
  else if(rawvalue < measVOLTcalib[MEAS_VOLTCAL_NUM - 1]){
    value = MEAS_VOLTCAL_START + (MEAS_VOLTCAL_NUM - 1) * MEAS_VOLTCAL_STEP;
  }
  else {
    i = 0;
    left = measVOLTcalib[i];
    right = measVOLTcalib[i + 1];
    while(rawvalue < right && i < MEAS_VOLTCAL_NUM - 1){
      i++;
      left = right;
      right = measVOLTcalib[i + 1];
    }
    value = MEAS_VOLTCAL_START +
            i * MEAS_VOLTCAL_STEP +
            (MEAS_VOLTCAL_STEP * (left - rawvalue)) / (left - right);
  }
  return value;
}*/

/*
 * ADC error callback. TODO: utilize
 */
static void adcerrorcallback(ADCDriver *adcp, adcerror_t err) {

  (void)adcp;
  (void)err;
}