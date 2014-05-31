/*
  ******************************************************************************
  * @file    ps2.h
  * @author  Eugene
  * @date    Dec 4, 2012
  * @brief   This file provides ps2 header file
  ******************************************************************************
 */

#ifndef PS2_H_
#define PS2_H_

/* Includes ------------------------------------------------------------------*/
#include "ConfigApp.h"
#include "WirelessProtocols/MCHP_API.h"
#include "WirelessProtocols/Console.h"
#include "DemoOutput.h"
#include "HardwareProfile.h"
#include "TimeDelay.h" 

// PSX command/data control
#define SET_PS2_COMM 		LATBbits.LATB2 = 1 //orange output
#define CLR_PS2_COMM 		LATBbits.LATB2 = 0
#define SET_PS2_ATT 		LATBbits.LATB4 = 1 //yellow output
#define CLR_PS2_ATT 		LATBbits.LATB4 = 0
#define SET_PS2_CLOCK 		LATBbits.LATB1 = 1 //blue output
#define CLR_PS2_CLOCK 		LATBbits.LATB1 = 0
#define READ_PS2_DATA		PORTBbits.RB3	
#define READ_PS2_ACK		PORTBbits.RB5

//GPIOE, GPIO_Pin_2 = data(brown)
//GPIOE, GPIO_Pin_6 = ack (green)

typedef struct {
	UINT8	 COMBO1;
	UINT8	 COMBO2;
	UINT8	 RJOY_X;
	UINT8	 RJOY_Y;
	UINT8	 LJOY_X;
	UINT8	 LJOY_Y;
	UINT8	 Response;
} PSX_DATA;

extern PSX_DATA psxData;

/** Functions prototype
  * @{
  */

void psx_init (void);
void psx_status (void);
UINT8	get_psx (void);


#endif /* PS2_H_ */
