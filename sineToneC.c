/**
 Square Tone Generator
 */

/* @section  I N C L U D E S */
#include "at89c5131.h"
#include "stdio.h"


void timer0isr();
void timerInit();
void delay_ms(int);


unsigned char sendMSB [] = {0x77, 0x7a, 0x7b, 0x7d, 0x7e, 0x7f, 0x7f,
														0x7f, 0x7e,0x7d, 0x7b, 0x7a, 0x77,
														0x75,0x74,0x72,0x71,0x70,0x70,
														0x70,0x71,0x72,0x74,0x75};
unsigned char sendLSB [] = {0xff, 0x11,  0xff, 0xa7, 0xed, 0xb9, 0xff,
														0xb9, 0xed, 0xa7, 0xff, 0x11, 0xff,
														0xed,0x00,0x58,0x12,0x46,0x00,
														0x46,0x12,0x58,0x00,0xed};	

unsigned char tempsine1 = 0x77;
unsigned char tempsine2 = 0xff;				
														
int oct_sel =1;
int oct_freq = 1; 

unsigned char port2;

bit sendVal = 1;

unsigned char tmh = 0xf7;
unsigned char tml = 0xdd ;
unsigned char acc;

unsigned char i, j;
bit f1 = 0;

unsigned char keypadArray [] = {
	0xFf,0x51,0xFf,0x5c,0xFf,0x65,0xFf,0x6c,
	0xFf,0x74,0xFf,0x7c,0xFf,0x85,0xff,0x8b,
  0xFf,0x92,0xFf,0x98,0xFf,0x9d,0xFf,0xA2};

unsigned char oct_sel_array[] = {4,2,1,1};

//SPI functions
void SPI_Init();
unsigned char datasend8bit;
unsigned char serial_data;														
sbit CS_BAR = P1^4;									// Chip Select for the DAC
sbit ldac = P3^0;
bit transmit_completed= 0;					// To check if spi data transmit is complete
int iter = 0;

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
//

void timerInit(){
	TH0 = 0xff	;										//For 25ms operation
	TL0 = 0x51;
	TMOD = (TMOD & 0xF0) | 0x01;   
	EA = 1;				// Set T/C0 Mode 
	ET0 = 1;			// Enable Timer 0 Interrupts 
	TR0 = 1;   
}

void timer0isr() interrupt 1 {
	TH0 = tmh;
	TL0 = tml;
	if(oct_sel == 1){
		if(iter == 24) { iter = 0;}
	 tempsine1 = sendMSB[iter];	
	 tempsine2 = sendLSB[iter];	
	 iter++;	
	 oct_sel = oct_freq;
	 sendVal = 1;
	}
	else{
		oct_sel -- ;
	}
}

void delay_ms(int delay)
{
	int d=0;
	while(delay>0)
	{
		for(d=0;d<382;d++);
		delay--;
	}
}

void main(){
	
	 P0 = 0x00;
	 P2 = 0xFF;
	 timerInit();
	 SPI_Init();
	 
	while(1){
		
		if(sendVal ==1){
		CS_BAR = 0;                 // enable DAC as slave		 
		ldac = 1;
		SPDAT= tempsine1;
		while(!transmit_completed);
		transmit_completed = 0;
		SPDAT = tempsine2;
		while(!transmit_completed);
		transmit_completed = 0;
		CS_BAR = 1;
		ldac = 0;
		sendVal = 0;
		}
		P2 = 0x0F;//ground all rows at once
	  acc = P2 ;//read all col ;(ensure keys open)
		acc &= 0x0f;// ;masked unused bits
		if(acc == 0x0f) continue;
		f1 = 1;
		
			delay_ms(10);
			P2 = 0x0F;//ground all rows at once
			acc = P2 ;//read all col ;(ensure keys open)
			acc &= 0x0f;// ;masked unused bits
			if(acc == 0x0f) continue;
			
			delay_ms(10);
			P2 = 0x0F;//ground all rows at once
			acc = P2 ;//read all col ;(ensure keys open)
			acc &= 0x0f;// ;masked unused bits
		  if(acc == 0x0f) continue;
			
			
			P2=0xF0;
			
			if(P2 == 0xE0) {i=0;}
			else if(P2 == 0xD0 ) {i=1;}
			else if(P2 == 0xB0) {i=2;}
			else if (P2 ==0x70 ) {i=3;}
			
			P2 = 0x0f;
			
			if(P2 == 0x0e) {j=0;}
			else if(P2 == 0x0d) {j=1;}
			else if(P2 == 0x0b) {j=2;}
			else if(P2 == 0x07) {j=3;}
			
			if(i<3){
			tmh = keypadArray[8*i+2*j];
			tml = keypadArray[8*i+2*j+1];
			}
			else
			{oct_freq = oct_sel_array[j];}
			
			f1 = 0;
			
		
	}
			
}
