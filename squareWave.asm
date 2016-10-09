;Square wave generator


;P2.0-2.3 coloumn and P2.4 - 2.7 rows
ORG 00h
LJMP main

org 50h
main:
		mov p1, 00h;
		mov 4fh, #20
		 MOV P2,#0FFH ;make P2 an input port
	;K1: MOV P1,#0 ;ground all rows at once
	K1: 
	    MOV P2, #0Fh;ground all rows at once
	     MOV A,P2 ;read all col ;(ensure keys open)
		ANL A,#00001111B ;masked unused bits
		CJNE A,#00001111B,K2 ;till all keys release
		SJMP K1
		
	K2: ACALL DELAY ;call 20 msec delay ;(delay 10ms)
		MOV P2, #0FH
		 MOV A,P2 ;see if any key is pressed; debounce
		 ANL A,#00001111B ;mask unused bits
		 CJNE A,#00001111B,OVER;key pressed, find row
		 SJMP K2 ;check till key pressed
		 
	OVER: ACALL DELAY ;wait 20 msec debounce time ;(delay 10ms)
			MOV P2, #0FH
			MOV A,P2 ;check key closure
			ANL A,#00001111B ;mask unused bits
			CJNE A,#00001111B,OVER1;key pressed, find row
			SJMP K2 ;if none, keep polling

	OVER1:  
				;MOV P1, #11111110B ;ground row 0
				
				MOV P2, #11101111B;
				MOV A,P2 ;read all columns
				ANL A,#11110000B ;mask unused bits
				CLR C
				SUBB A, #11100000B
				;CJNE A,#11100000B,ROW_0 ;key row 0, find col.
				JZ ROW_0; 
				
				MOV P2, #11011111B;
				MOV A,P2 ;read all columns
				ANL A,#11110000B ;mask unused bits
				CLR C
				SUBB A, #11010000B
				;CJNE A,#11100000B,ROW_0 ;key row 0, find col.
				JZ ROW_1;
				
				MOV P2, #10111111B;
				MOV A,P2 ;read all columns
				ANL A,#11110000B ;mask unused bits
				CLR C
				SUBB A, #10110000B
				;CJNE A,#11100000B,ROW_0 ;key row 0, find col.
				JZ ROW_2;
				
				MOV P2, #01111111B;
				MOV A,P2 ;read all columns
				ANL A,#11110000B ;mask unused bits
				CLR C
				SUBB A, #01110000B
				;CJNE A,#11100000B,ROW_0 ;key row 0, find col.
				JZ ROW_3;
				
				LJMP K2 ;if none, false input, ;repeat


	ROW_0: MOV DPTR,#KCODE0 ;set DPTR=start of row 0
			SJMP FIND ;find col. Key belongs to
			
	ROW_1: MOV DPTR,#KCODE1 ;set DPTR=start of row
			SJMP FIND ;find col. Key belongs to
			
	ROW_2: MOV DPTR,#KCODE2 ;set DPTR=start of row 2
			SJMP FIND ;find col. Key belongs to
			
	ROW_3: MOV DPTR,#KCODE3 ;set DPTR=start of row 3
	
	FIND: 
			MOV P2, #0fh
			mov a, P2
			find2:		RRC A ;see if any CY bit low
						JNC MATCH ;if zero, get ASCII code
						INC DPTR ;point to next col. addr
			
						SJMP find2 ;keep searching
			
	MATCH: 
			CLR A ;set A=0 (match is found)
			MOVC A,@A+DPTR ;get ASCII from table
			SWAP A;
			MOV P1,A ;display pressed key
			
			LJMP K1
;ASCII LOOK-UP TABLE FOR EACH ROW


DELAY:
	USING 0
	PUSH PSW	;to make sure valuse of registers are stored somewhere 
	PUSH AR1
	PUSH AR2
	PUSH AR3
	PUSH B
	MOV A, 4FH	; D
	MOV B, #10	
	MUL AB		;No. of times 50ms blocks to be executed to generate D/2 sec delay
	MOV R3, A
	
					;upper loop running 10*D times
	MOV R2,#40				
	BACK1:					;inner loop of 50ms delay
		MOV R1,#0FFH
		BACK :				;inner to inner loop
			DJNZ R1, BACK
		DJNZ R2, BACK1

	
	
	POP B 		;restoring values of the registers
	POP AR3
	POP AR2
	POP AR1
	POP PSW 
	RET



ORG 300h
KCODE0: DB 12,1,2,3 ;ROW 0
KCODE1: DB 4,5,6,7 ;ROW 1
KCODE2: DB 8,9,10,11 ;ROW 2
KCODE3: DB 12,13,14,15 ;ROW 3
end
	
/*ORG 300H
	KCODE0: DB 0DF, 73h, H40,8100,7680,7290 ;ROW 0
	KCODE1: DB 6912,6480,6075,5760 ;ROW 1
	KCODE2: DB 5400,5120, 4860,4608 ;ROW 2
	KCODE3: DB 2,1,4,0 ;ROW 3
END
*/