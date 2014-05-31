/**
  ******************************************************************************
  * @file    ps2.c
  * @author  Eugene
  * @date    Dec 4, 2012
  * @brief   This file provides ps2 configuration
  *
  * Revision 1
  * @author  Lee Chee Cheong
  * @date    May 31, 2014
  * @brief   Port the source code to PIC18F25K20
  ******************************************************************************
 */
/*
       | Bit0 Bit1 Bit2 Bit3 Bit4 Bit5 Bit6 Bit7
--------------------------------------------------
COMBO1 | SLCT JOYL JOYR STRT  UP  RGHT DOWN LEFT
COMBO2 |  L2   R2   L1   R1   /_\  O    X    []
RJOY_X |   Right Joy: 0x00 = Left  0xFF = Right
RJOY_Y |   Right Joy: 0x00 = Up    0xFF = Down
LJOY_X |   Left Joy:  0x00 = Left  0xFF = Right
LJOY_Y |   Left Joy:  0x00 = Up    0xFF = Down

* This file provides ps2 configuration
* Have to initialize by writing psx_init();
* Get response by getting the return from get_psx();
*/

/* Includes ------------------------------------------------------------------*/
#include "ps2.h"

static void psx_LowLevelInit(void);
static UINT8	 psx_write (UINT8	wrData);
static UINT8	 psx_write_lastbyte (UINT8 wrData);

static UINT8 clockDelay = 50;

PSX_DATA psxData;

/**
  * @brief  setup ps2 interface
  * @param  void
  * @retval void
  */

void psx_init (void)
{
	psx_LowLevelInit();
	CLR_PS2_ATT;
	psx_write(0x01);        //enter config mode
	psx_write(0x43);
	psx_write(0x00);
	psx_write(0x01);
	psx_write_lastbyte(0x00);
	SET_PS2_ATT;
	DelayMs(6);
	CLR_PS2_ATT;
	psx_write(0x01);         //turn on analog and lock analog button
	psx_write(0x44);
	psx_write(0x00);
	psx_write(0x01);
	psx_write(0x03);
	psx_write(0x00);
	psx_write(0x00);
	psx_write(0x00);
	psx_write_lastbyte(0x00);
	SET_PS2_ATT;
	DelayMs(6);
	CLR_PS2_ATT;
	psx_write(0x01);         //exit config mode
	psx_write(0x43);
	psx_write(0x00);
	psx_write(0x00);
	psx_write(0x5A);
	psx_write(0x5A);
	psx_write(0x5A);
	psx_write(0x5A);
	psx_write_lastbyte(0x5A);
	SET_PS2_ATT;
	DelayMs(6);
}

/**
  * @brief  setup ps2 bit configurations
  * @param  void
  * @retval void
  */

static void psx_LowLevelInit(void)
{
	INTCON2bits.RBPU = 0;   // PORTB pull up are enabled
	WPUB |= 0x3E;           // enable port pull up for rb 1,2,3,4,5.
	TRISBbits.TRISB1 = 0;	// PS2 Clock, as output
	TRISBbits.TRISB2 = 0;	// PS2 Command, as output
	TRISBbits.TRISB3 = 1;	// PS2 Data, as input
	TRISBbits.TRISB4 = 0;	// PS2 ATT, as output
	TRISBbits.TRISB5 = 1;	// PS2 ACK, as input

	CLR_PS2_COMM;
	CLR_PS2_ATT;
	CLR_PS2_CLOCK;
}

/**
  * @brief  get respond from ps2
  * @param  void
  * @retval return 1 if ps2 has respond; return 0 if ps2 has no respond
  */

UINT8 get_psx (void)
{
	UINT8 rdData;
	CLR_PS2_ATT;
	psx_write(0x01);
	rdData = psx_write(0x42);

	if (rdData == 0x73)
	{
		//read byte 0x5A
		rdData = psx_write(0x00);
		// read data byte 1 (COMBO1) from PSX
		psxData.COMBO1 = psx_write(0x00);
		// read data byte 2 (COMBO2) from PSX
		psxData.COMBO2 = psx_write(0x00);
		// read data byte 3 (RJOY_X) from PSX
		psxData.RJOY_X = psx_write(0x00);
		// read data byte 4 (RJOY_Y) from PSX
		psxData.RJOY_Y = psx_write(0x00);
		// read data byte 5 (LJOY_X) from PSX
		psxData.LJOY_X = psx_write(0x00);
		// read data byte 6 (LJOY_Y) from PSX
		psxData.LJOY_Y = psx_write_lastbyte(0x00);
		SET_PS2_COMM;
		SET_PS2_ATT;
		psxData.Response = 1;
		return 1;
	}
	SET_PS2_COMM;
	SET_PS2_ATT;
	return 0;
}

/**
  * @brief  write command to ps2 and get button value from ps2
  * @param  void
  * @retval return button value
  */

static UINT8 psx_write (UINT8 wrData)
{
	UINT8 rdData, i, dataIn;
	UINT8 clockCnt;
	for (i = 0; i < 8; i ++)
	{
		if(wrData & 1)
			SET_PS2_COMM;
		else
			CLR_PS2_COMM;

		wrData >>= 1;
		for (clockCnt = 0; clockCnt < clockDelay; clockCnt++)
			CLR_PS2_CLOCK;
		dataIn = READ_PS2_DATA;
		rdData = (rdData >> 1) | ((UINT8) dataIn << 7);
		for (clockCnt = 0; clockCnt < clockDelay; clockCnt++)
			SET_PS2_CLOCK;
	}
	//while (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_6) == 1);
	while (READ_PS2_ACK == 0);
	return rdData;
}

/**
  * @brief  write last byte command to ps2 and get button value from ps2
  * @param  void
  * @retval return button value
  */

static UINT8 psx_write_lastbyte (UINT8 wrData)
{
	UINT8 rdData, i, dataIn;
	UINT8 clockCnt;
	for (i = 0; i < 8; i ++)
	{
		if(wrData & 1)
			SET_PS2_COMM;
		else
			CLR_PS2_COMM;
		wrData >>= 1;
		for (clockCnt = 0; clockCnt < clockDelay; clockCnt++)
			CLR_PS2_CLOCK;
		dataIn = READ_PS2_DATA;
		rdData = (rdData >> 1) | ((UINT8) dataIn << 7);
		for (clockCnt = 0; clockCnt < clockDelay; clockCnt++)
			SET_PS2_CLOCK;
	}
	return rdData;
}

void psx_status (void)
{
	UINT8	 Response;
	Response = get_psx();
	if(Response == 1)
	{
		if(!(psxData.COMBO1 & 0x08))          // Start
		{
			//printf ("ps2 start\r\n");
		}
		if(!(psxData.COMBO1 & 0x01))         // Select
		{
			//printf ("ps2 select\r\n");
		}
		if(!(psxData.COMBO2 & 0x08))           //R1
		{
			//printf ("ps2 R1\r\n");
		}
		if(!(psxData.COMBO2 & 0x04))           //L1
		{
			//printf ("ps2 L1\r\n");
		}
		if(!(psxData.COMBO2 & 0x01))           //L2
		{
			//printf ("ps2 L2\r\n");
		}
		if(!(psxData.COMBO2 & 0x02))            //R2
		{
			//printf ("ps2 R2\r\n");
		}
		if(!(psxData.COMBO2 & 0x80))            //[] button
		{
			//printf ("ps2 [] button\r\n");
		}
		else if(!(psxData.COMBO2 & 0x20)) // O button
		{
			//printf ("ps2 O button\r\n");
		}
		if(!(psxData.COMBO2 & 0x40)) // X button
		{
			//printf ("ps2 X button\r\n");
		}
		else if(!(psxData.COMBO2 & 0x10)) // /_\ button
		{
			//printf ("ps2 tri button\r\n");
		}
		if(!(psxData.COMBO1 & 0x20))        //right
		{
			//printf ("ps2 right\r\n");
		}
		else if(!(psxData.COMBO1 & 0x80))        //Left
		{
			//printf ("ps2 left\r\n");
		}
		if(!(psxData.COMBO1 & 0x40))          //Down
		{
			//printf ("ps2 down\r\n");
		}
		else if(!(psxData.COMBO1 & 0x10)) //up
		{
			//printf ("ps2 up\r\n");
		}
		//************************** joystick **********************************************
		if(psxData.LJOY_Y < 0x10 && psxData.RJOY_X == 0x80 )  //forward
		{
			//printf ("ps2 joystick forward\r\n");
		}
		else if(psxData.LJOY_Y > 0xF0 && psxData.RJOY_X == 0x80 ) // backward
		{
			//printf ("ps2 joystic backward\r\n");
		}
		else if(psxData.LJOY_Y < 0x10 && psxData.RJOY_X < 0x10 ) // forward left
		{
			//printf ("ps2 joystic forward left\r\n");
		}
		else if(psxData.LJOY_Y < 0x10 && psxData.RJOY_X > 0xF0 ) // forward right
		{
			//printf ("ps2 joystic forward right\r\n");
		}
		else if(psxData.LJOY_Y > 0xF0 && psxData.RJOY_X < 0x10 ) // backward left
		{
			//printf ("ps2 joystic backward left\r\n");
		}
		else if(psxData.LJOY_Y > 0xF0 && psxData.RJOY_X > 0xF0 ) // backward right
		{
			//printf ("ps2 joystic backward right\r\n");
		}
		else if(psxData.LJOY_Y == 0x80 && psxData.RJOY_X < 0x10 ) // static left
		{
			//printf ("ps2 joystic static left\r\n");
		}
		else if(psxData.LJOY_Y == 0x80 && psxData.RJOY_X > 0xF0 ) // static right
		{
			//printf ("ps2 joystic static right\r\n");
		}
	}
	else
	{
		//printf ("ps2 no respone\r\n");
		DelayMs(200);
	}
}




