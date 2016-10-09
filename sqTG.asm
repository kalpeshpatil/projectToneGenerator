ORG 00h
LJMP MAIN


	
ORG 000BH
	
	LCALL OCTAVE
	RETI
	
OCTAVE:  
	      MOV TH0, 21H
	      MOV TL0, 20H
			DJNZ R0, FIN
			CPL P0.1
			MOV R0, 30H
    FIN: RET

ORG 50H

MAIN:
		 MOV P0,#0FFH;
		 MOV P2,#0FFH ;make P2 an input port
		 MOV 30H,#1 ; DEFAULT 480 HZ OCTAVE
		 MOV TMOD, #01H
		 MOV TH0,#0F7H ; DEFAULT IS FOR 480HZ OCTAVE
		 MOV TL0,#0DDH
		 SETB IE.1
		 SETB IE.7
		 SETB TR0
	K1: 
	    MOV P2, #0Fh;ground all rows at once
	     MOV A,P2 ;read all col ;(ensure keys open)
		ANL A,#00001111B ;masked unused bits
		CJNE A,#00001111B,K2;till all keys release
		LJMP K1

     K2: ACALL DELAY ;wait 10 msec debounce time ;(delay 10ms)
			MOV P2, #0FH
			MOV A,P2 ;check key closure
			ANL A,#00001111B ;mask unused bits
			CJNE A,#00001111B,OVER;key pressed, find row
			SJMP K2 ;if none, keep polling
		 
	OVER: ACALL DELAY ;wait 20 msec debounce time ;(delay 10ms)
			MOV P2, #0FH
			MOV A,P2 ;check key closure
			ANL A,#00001111B ;mask unused bits
			CJNE A,#00001111B,OVER1;key pressed, find row
			SJMP K2 ;if none, keep polling
			
			

	OVER1:  
				;MOV P1, #11111110B ;ground row 0
				MOV P2, #0F0h
				MOV A, P2
				CLR C
				
				JNB P2.4, ROW_0
				JNB P2.5, ROW_1
				JNB P2.6 , ROW_2
				JNB P2.7, ROW_3
		
				SJMP K1
				


	ROW_0: MOV DPTR,#KCODE0 ;set DPTR=start of row 0
			SJMP FIND ;find col. Key belongs to
			
	ROW_1: MOV DPTR,#KCODE1 ;set DPTR=start of row
			SJMP FIND ;find col. Key belongs to
			
	ROW_2: MOV DPTR,#KCODE2 ;set DPTR=start of row 2
			SJMP FIND ;find col. Key belongs to
			
	ROW_3: MOV DPTR,#KCODE3 ;set DPTR=start of row 3
	FIND3:CLR C
			MOV P2, #0fh
			mov a, P2
			find4:		RRC A ;see if any CY bit low
						JNC MATCH1 ;if zero, get ASCII code
						INC DPTR ;point to next col. add
						SJMP find4 ;keep searching
	
	MATCH1: 
	CLR A ;set A=0 (match is found)
			MOVC A,@A+DPTR ;get ASCII from table
			MOV 30H,A; FACTOR FOR OCTAVE
			MOV R0,30H
			LJMP K1
			
	FIND: 
			CLR C
			MOV P2, #0fh
			mov a, P2
			find2:		RRC A ;see if any CY bit low
						JNC MATCH ;if zero, get ASCII code
						INC DPTR ;point to next col. addr
			            INC DPTR ; TWO BYTES FOR TIMER
						SJMP find2 ;keep searching
			
	MATCH: 
			CLR A ;set A=0 (match is found)
			MOVC A,@A+DPTR ;get ASCII from table
			MOV 21H,A; TH0
			INC DPTR 
			CLR A
			MOVC A, @A+DPTR
			MOV 20H,A ; TL0
			LJMP K1


DELAY:
	USING 0
	PUSH PSW	;to make sure valuse of registers are stored somewhere 
	PUSH AR1
	PUSH AR2
	
				;upper loop running 10*D times
	MOV R2,#40
	 BACK1:					;inner loop of 50ms delay
		 MOV R1,#0FFH
		  BACK:				;inner to inner loop
			DJNZ R1, BACK
		DJNZ R2, BACK1
	
	POP AR2
	POP AR1
	POP PSW 
	RET


;ASCII LOOK-UP TABLE FOR EACH ROW
ORG 300h
KCODE0: DB  0F7H,0DDH ,0F8H,5FH,0F8H,0C4H,0F9H,22H ;ROW 0
KCODE1: DB  0F9H,7DH,0F9h,0E5H,0FAH,46H,0FAH,93H ;ROW 1
KCODE2: DB  0FAH,0E9H,0FBH,2DH ,0FBH,6CH,0FBH,0A8H;ROW 2
KCODE3: DB   4,2,1,1 ;ROW 3
end
	