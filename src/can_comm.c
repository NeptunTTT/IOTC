/*
    IOTC - Copyright (C) 2016
    Neptun TTT Kft.            
*/

#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "shell.h"
#include "chprintf.h"

#include "can_comm.h"
#include "meas.h"

#define BAUDRATE          6

#define CAN_IOTC_EID          0x60
#define CAN_IOTC_MESSAGES_1   0x01
#define CAN_IOTC_MESSAGES_2   0x02
#define CAN_IOTC_MESSAGES_3   0x03
#define CAN_IOTC_MESSAGES_4   0x04
#define CAN_IOTC_MESSAGES_5   0x05

enum canMessages
{
  CAN_MESSAGES_1,
  CAN_MESSAGES_2,
  CAN_MESSAGES_3,
  CAN_MESSAGES_4,
  CAN_MESSAGES_5,
  CAN_NUM_MESS
}canmessages;

static CANTxFrame txmsg;
static CANRxFrame rxmsg;

static uint16_t brp_1 = 6;

/*--------------------------------------------------*/
/* CAN_MCR_ABOM   ->  Automatic Bus-Off Management  */
/* CAN_MCR_AWUM   ->  Automatic Wakeup Mode         */
/* CAN_MCR_TXFP   ->  Transmit FIFO Priority        */
/* CAN_MSR_TXM    ->  Transmit Mode                 */
/* CAN_MSR_RXM    ->  Receive Mode                  */
/* CAN_BTR_LBKM   ->  Loop Back Mode (Debug)        */
/* CAN_BTR_SJW()  ->  Resynchronization Jump Width  */
/* CAN_BTR_TS2()  ->  Time Segment 2                */
/* CAN_BTR_TS1()  ->  Time Segment 1                */
/* CAN_BTR_BRP()  ->  Baud Rate Prescaler           */
/* CAN_IER_SLAKI  ->  Sleep acknowledge interrupt   */
/* _____________________________________________________________________*/
/*                  				CAN_BTR_BRP Baud 500kHz			                */
/*                                                                      */
/*		 		   F1XX		      |		F2XX        |      F4XX        		          */
/*  fPCLK    36 MHz		    |		30 MHz      |      42 MHz			              */
/*  tPCLK		 0,027777777	|		0,033333333 |      0,023809524		          */
/*  BRP[X]   5			      |		4           |      6        		            */
/*  tq =		 0,166666662	|		0,166666667 |      0,166666667              */
/*----------------------------------------------------------------------*/

static const CANConfig cancfg = {
  CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
  CAN_BTR_LBKM | CAN_BTR_SJW(0) | CAN_BTR_TS2(1) |
  CAN_BTR_TS1(8) | CAN_BTR_BRP(6)
};

static int32_t message_status;

/*
 * Receiver thread.
 */
static THD_WORKING_AREA(can_rx_wa, 256);
static THD_FUNCTION(can_rx, p) {
  event_listener_t el;

  (void)p;
  chRegSetThreadName("receiver");
  chEvtRegister(&CAND1.rxfull_event, &el, 0);
  while(!chThdShouldTerminateX()) {
    if (chEvtWaitAnyTimeout(ALL_EVENTS, MS2ST(100)) == 0)
      continue;
    while (canReceive(&CAND1, CAN_ANY_MAILBOX, &rxmsg, TIME_IMMEDIATE) == MSG_OK) {
      
      if (rxmsg.EID == 0x6005){

        canStop(&CAND1);
        CANConfig cancfg = {
          CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
          CAN_BTR_LBKM | CAN_BTR_SJW(0) | CAN_BTR_TS2(1) |
          CAN_BTR_TS1(8) | CAN_BTR_BRP(brp_1)
        };
        canStart(&CAND1, &cancfg);
        
        palClearPad(GPIOA, GPIOA_PO_L);
        palTogglePad(GPIOA, GPIOA_PO_H);
      }

    }
  }
  chEvtUnregister(&CAND1.rxfull_event, &el);
}

/*
 * Transmitter thread.
 */
static THD_WORKING_AREA(can_tx_wa, 256);
static THD_FUNCTION(can_tx, p) {

  (void)p;
  chRegSetThreadName("transmitter");

  while (!chThdShouldTerminateX()) {

    for(message_status = 0; message_status < CAN_NUM_MESS; message_status ++){
      switch(message_status){
        case CAN_MESSAGES_1:
          /* Message 1 */
          /* ID = 0x6001
          * 16bit - Analog input 1
          */
          txmsg.DLC = 4;
          txmsg.EID = 0;
          txmsg.EID = CAN_IOTC_MESSAGES_1;
          txmsg.EID += CAN_IOTC_EID << 8;

          txmsg.data16[0] = measGetValue(MEAS_AIN1);
          
          canTransmit(&CAND1, CAN_ANY_MAILBOX ,&txmsg, MS2ST(100));
          break;
        case CAN_MESSAGES_2:
          /* Message 2 */
          /* ID = 0x6002
          * 16bit - Analog input 2
          */
          txmsg.DLC = 4;
          txmsg.EID = 0;
          txmsg.EID = CAN_IOTC_MESSAGES_2;
          txmsg.EID += CAN_IOTC_EID << 8;

          txmsg.data16[0] = measGetValue(MEAS_AIN2);
          
          canTransmit(&CAND1, CAN_ANY_MAILBOX ,&txmsg, MS2ST(100));
          break;
        case CAN_MESSAGES_3:
          /* Message 3 */
          /* ID = 0x6003
          * 16bit - Analog input 3
          */
          txmsg.DLC = 4;
          txmsg.EID = 0;
          txmsg.EID = CAN_IOTC_MESSAGES_3;
          txmsg.EID += CAN_IOTC_EID << 8;

          txmsg.data16[0] = measGetValue(MEAS_AIN3);
          
          canTransmit(&CAND1, CAN_ANY_MAILBOX ,&txmsg, MS2ST(100));
          break;
        case CAN_MESSAGES_4:
          /* Message 4 */
          /* ID = 0x6004
          * 16bit - Analog input 4
          */
          txmsg.DLC = 4;
          txmsg.EID = 0;
          txmsg.EID = CAN_IOTC_MESSAGES_4;
          txmsg.EID += CAN_IOTC_EID << 8;

          txmsg.data16[0] = measGetValue(MEAS_AIN4);
          
          canTransmit(&CAND1, CAN_ANY_MAILBOX ,&txmsg, MS2ST(100));
          break;
        case CAN_MESSAGES_5:
          /* Message 5 */
          /* ID = 0x6005
          * 16bit - Analog input 5
          */
          txmsg.DLC = 4;
          txmsg.EID = 0;
          txmsg.EID = CAN_IOTC_MESSAGES_5;
          txmsg.EID += CAN_IOTC_EID << 8;

          txmsg.data16[0] = measGetValue(MEAS_AIN5);
          
          canTransmit(&CAND1, CAN_ANY_MAILBOX ,&txmsg, MS2ST(100));
          break;
        default:      
            break;
        
      }
      chThdSleepMilliseconds(100);
      }
    chThdSleepMilliseconds(300);
  }
  return 0;
}

void can_commInit(void){
  brp_1 = 6;
  canStart(&CAND1, &cancfg);
  chThdCreateStatic(can_rx_wa, sizeof(can_rx_wa), NORMALPRIO + 7, can_rx, NULL);
  chThdCreateStatic(can_tx_wa, sizeof(can_tx_wa), NORMALPRIO + 7, can_tx, NULL);

  txmsg.IDE = CAN_IDE_EXT;
  txmsg.RTR = CAN_RTR_DATA;
  txmsg.DLC = 8;

  rxmsg.IDE = CAN_IDE_STD;
  rxmsg.RTR = CAN_RTR_DATA;
  rxmsg.DLC = 8;
}