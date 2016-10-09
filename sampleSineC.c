/**
 Square Tone Generator
 */

/* @section  I N C L U D E S */
#include "at89c5131.h"
#include "stdio.h"
#include "math.h"

unsigned char sendMSB [] = {0x77, 0x7a, 0x7b, 0x7d, 0x7e, 0x7f, 0x7f,
														0x7f, 0x7e,0x7d, 0x7b, 0x7a, 0x77,
														0x75,0x74,0x72,0x71,0x70,0x70,
														0x70,0x71,0x72,0x74,0x75};
unsigned char sendLSB [] = {0xff, 0x11,  0xff, 0xa7, 0xed, 0xb9, 0xff,
														0xb9, 0xed, 0xa7, 0xff, 0x11, 0xff,
														0xed,0x00,0x58,0x12,0x46,0x00,
														0x46,0x12,0x58,0x00,0xed};	


	
void SPI_Init();
void sdelay(int delay);
unsigned char datasend8bit;
unsigned char serial_data;														
sbit CS_BAR = P1^4;									// Chip Select for the DAC
sbit ldac = P3^0;
bit transmit_completed= 0;					// To check if spi data transmit is complete
int iter = 12;

unsigned int serial;														

														
void it_SPI(void) interrupt 9 /* interrupt address is 0x004B */
{
	switch	( SPSTA )         /* read and clear spi status register */
	{
		case 0x80:	
			serial_data=SPDAT;   /* read receive data */
      transmit_completed=1;/* set software flag */
 		break;

		case 0x10:
         /* put here for mode fault tasking */	
		break;
	
		case 0x40:
         /* put here for overrun tasking */	
		break;
	}
}

/**

 * FUNCTION_INPUTS:  P1.5(MISO) serial input  
 * FUNCTION_OUTPUTS: P1.7(MOSI) serial output
 *                   P1.4(SSbar)
                     P1.6(SCK)
 */ 
void SPI_Init()
{
	CS_BAR = 1;	                  	// DISABLE ADC SLAVE SELECT-CS 
	SPCON |= 0x20;               	 	// P1.1(SSBAR) is available as standard I/O pin 
	SPCON |= 0x01;                	// Fclk Periph/4 AND Fclk Periph=12MHz ,HENCE SCK IE. BAUD RATE=3000KHz 
	SPCON |= 0x10;               	 	// Master mode 
	SPCON &= ~0x08;               	// CPOL=0; transmit mode example|| SCK is 0 at idle state
	SPCON |= 0x04;                	// CPHA=1; transmit mode example 
	IEN1 |= 0x04;                	 	// enable spi interrupt 
	EA=1;                         	// enable interrupts 
	SPCON |= 0x40;                	// run spi;ENABLE SPI INTERFACE SPEN= 1 
}

void main(){
	SPI_Init();
	iter = 0;
while (1){
	if(iter == 24) { iter = 0;}
	
	
	CS_BAR = 0;                 // enable DAC as slave		 
	ldac = 1;
	SPDAT= sendMSB[iter];		
  //SPDAT = 0x7b;	// Write start bit to start ADC 
	while(!transmit_completed);
  transmit_completed = 0;
	
	SPDAT = sendLSB[iter];
	//SPDAT = 0xf0;
	while(!transmit_completed);
  transmit_completed = 0;
	
	CS_BAR = 1;
	ldac = 0;
	iter++;
}
}
	
	
	