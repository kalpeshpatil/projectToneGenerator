/**
 Square Tone Generator
 */

/* @section  I N C L U D E S */
#include "at89c5131.h"
#include "stdio.h"


void timer0isr();
void timerInit();
void delay_ms(int);

int oct_sel =1;
int oct_freq = 1; 

unsigned char port2;
sbit temp = 0x81;


unsigned char tmh = 0xf7;
unsigned char tml = 0xdd;
unsigned char acc;

int i, j;
bit f1, f2, f3 =0;

unsigned char keypadArray [] = {
	0xF7,0xDD,0xF8,0x5F,0xF8,0xC4,0xF9,0x22,
	0xF9,0x7D,0xF9,0xE5,0xFA,0x46,0xFA,0x93,
  0xFA,0xE9,0xFB,0x2D,0xFB,0x6C,0xFB,0xA8};

unsigned char oct_sel_array[] = {4,2,1,1};

void timerInit(){
	TH0 = 0xf7	;										//For 25ms operation
	TL0 = 0xdd;
	TMOD = 0x01;  
	EA = 1;				// Set T/C0 Mode 
	ET0 = 1;			// Enable Timer 0 Interrupts 
	TR0 = 1;   
}

void timer0isr() interrupt 1 {
	TH0 = tmh;
	TL0 = tml;
	if(oct_sel == 1){
		temp = ~(temp);
		
		
		oct_sel = oct_freq;
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

void main(){P1 = 0x00;
	 P0 = 0x00;
	 P2 = 0xFF;
	 timerInit();
	 
	while(1){
		
		P2 = 0x0F;//ground all rows at once
	  acc = P2 ;//read all col ;(ensure keys open)
		acc &= 0x0f;// ;masked unused bits
		if(acc == 0x0f) continue;
		f1 = 1;
		while(f1 == 1){
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
			else if (P2 ==0x70 ) {i=3;P1 = 0x04;}
			
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
			
}
