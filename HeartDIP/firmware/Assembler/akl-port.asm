; AntiKippenLights-Mini
;=======================

; ATTiny2313 @ 1 MHz (internal R/C)

; ported from AntiKippenLights and removed the random number generator
;
;    original code by Stefan Schuermans
;    -> https://wiki.blinkenarea.org/bin/view/Blinkenarea/AntiKippenLights
;       (KippenLights special edition with PIC16C54, version 0.2, 2005-01-01)
;
;    ported to AVR by Arne Rossius
;    -> https://wiki.blinkenarea.org/bin/view/Blinkenarea/AntiKippenLights-Mini
;
; differences: removed random number generator, added on/off key

; This code is very chaotic because it's an 1:1 port from the PIC code and I
; didn't change many names or comments, so it's best if you just ignore them.

; Version 0.7, 2005-04-29
; Version 0.8, 2005-06-11
;  * added on/off key function
; Version 0.9, 2009-11-13
;  * added disable on/off-key function (bridge pins 9 (PD5) and 10 (GND))
; Version 0.9.1, 2014-12-06
;  * improved on/off switch debouncing
;  * improved default to off after reset

.include "../include/tn2313def.inc"

.def	temp	= R16
.def	temp2	= R17
.def	A_REG	= R18
.def	B_REG	= R19
.def	C_REG	= R20
.def	D_REG	= R21
.def	COUNT	= R22
.def	EFFECT	= R23
.def	VALUE	= R24
.def	ON	= R25
.def	w1	= R26
.def	w2	= R27

.equ	PIXELS	= 0x60

;-------------------

.org $0000			;reset vector
	rjmp	reset
.org PCIaddr			;pin change interrupt vector
	rjmp	on_off

;-------------------

reset:
	ldi	temp, LOW(RAMEND)
	out	SPL, temp

;	ldi	temp, 0xFF
;	out	DDRB, temp
;	ldi	temp, 0x00
;	out	PORTB, temp
	ldi	temp, 0x3F
	out	DDRB, temp
	ldi	temp, 0xC0
	out	PORTB, temp

	ldi	temp, 0x07
	out	DDRD, temp
	ldi	temp, 0xFF
	out	PORTD, temp
	
	ldi	temp, 0x80	;disable analog comparator
	out	ACSR, temp
	
	ldi	temp, 1<<SM0	;sleep mode = power down
	out	MCUCR, temp
	ldi	temp, 1<<PCIE	;pin change interrupt enabled
	out	GIMSK, temp
	ldi	temp, 0x80	;pin change interrupt only on PB7
	out	PCMSK, temp
	
	ldi	ON, 1	;default to on
	sbis	PIND, 5	;don't enable push-button interrupt if pins 9 and 10 bridged
	rjmp	stay_on
	ldi	ON, 0	;default to off
	sei
	ldi	temp, 1<<SE | 1<<SM0	;enable sleep and select power down mode
	out	MCUCR, temp
	sleep
	ldi	temp, 1<<SM0	;disable sleep
	out	MCUCR, temp
stay_on:

	;make PB6-7 inputs with pullup and thus trigger a pin change IRQ
	;(device will switch to power down mode)
;	ldi	temp, 0x3F
;	out	DDRB, temp
;	ldi	temp, 0xC0
;	out	PORTB, temp
	
	rjmp	MAIN

;-------------------------------------------------------------------------------
; switch on/off (sleep mode/normal mode) [pin change interrupt]
;-------------------------------------------------------------------------------

on_off:
	push	temp
	in	temp, SREG
	push	temp
	
	;check if pin is low
	sbic	PINB, 7
	rjmp	on_off_return
	
	;switch all LEDs off
	ldi	temp, 0xC0
	out	PORTB, temp
	ldi	temp, 0x7F
	out	PORTD, temp

	;check if pin stays low for some time
	ldi	w1, 0x10
	clr	w2
on_off_delay:
	sbic	PINB, 7
	rjmp	on_off_return
	dec	w2
	brne	on_off_delay
	dec	w1
	brne	on_off_delay
	
	;wait until the key is released
wait_key_release:
	ldi	w1, 0x40
	clr	w2
wait_key_release_delay:
	sbis	PINB, 7
	rjmp	wait_key_release
	dec	w2
	brne	wait_key_release_delay
	dec	w1
	brne	wait_key_release_delay
	
	cpi	ON, 0
	breq	switch_on
	
	ldi	ON, 0		;switch off
	ldi	temp, 1<<PCIF	;clear pin-change interrupt (set when the key was released)
	out	EIFR, temp
	sei			;re-enable interrupts so the device can be woken again
	ldi	temp, 1<<SE | 1<<SM0	;enable sleep and select power down mode
	out	MCUCR, temp
	sleep
	ldi	temp, 1<<SM0	;disable sleep
	out	MCUCR, temp
	rjmp	on_off_return
	
switch_on:			;switch on
	ldi	ON, 1
	
on_off_return:			;return to main program
	ldi	temp, 0x20	;clear pin-change interrupt (set when the key was released)
	out	EIFR, temp
	pop	temp
	out	SREG, temp
	pop	temp
	reti

;-------------------------------------------------------------------------------
; set pixels
; INPUT:	W		brightness values to set pixels to
; MODIFIES:	PIXELS
;-------------------------------------------------------------------------------

SETPIX:
	ldi	ZL,	PIXELS
	st	Z+,	temp
	st	Z+,	temp
	st	Z+,	temp
	st	Z+,	temp
	st	Z+,	temp
	st	Z+,	temp
	st	Z+,	temp
	st	Z+,	temp
	st	Z+,	temp
	st	Z+,	temp
	st	Z+,	temp
	st	Z+,	temp
	st	Z+,	temp
	st	Z+,	temp
	st	Z+,	temp
	st	Z+,	temp
	st	Z+,	temp
	st	Z+,	temp
	
	ret

;-------------------------------------------------------------------------------
; output a frame
; INPUT:	PIXELS		brightness values of pixels
; MODIFIES:	PORTA, PORTB, A_REG, VALUE
; 
;-------------------------------------------------------------------------------

OUTPUT:
	ldi	VALUE,	63
OUTPUT_FRAME_LOOP:
	ldi	ZL,	PIXELS

; get bits for 1st 6 LEDs (31 cycles)
	clr	A_REG
	
	ld	temp,	Z+
	cp	temp,	VALUE
	brlo	PC+2
	ori	A_REG,	0x01

	ld	temp,	Z+
	cp	temp,	VALUE
	brlo	PC+2
	ori	A_REG,	0x02

	ld	temp,	Z+
	cp	temp,	VALUE
	brlo	PC+2
	ori	A_REG,	0x04

	ld	temp,	Z+
	cp	temp,	VALUE
	brlo	PC+2
	ori	A_REG,	0x08

	ld	temp,	Z+
	cp	temp,	VALUE
	brlo	PC+2
	ori	A_REG,	0x10

	ld	temp,	Z+
	cp	temp,	VALUE
	brlo	PC+2
	ori	A_REG,	0x20

	ori	A_REG,	0xC0
	out	PORTB,	A_REG
	cbi	PORTD,	0

; get bits for 2nd 6 LEDs (31 cycles)
	clr	A_REG
	
	ld	temp,	Z+
	cp	temp,	VALUE
	brlo	PC+2
	ori	A_REG,	0x01

	ld	temp,	Z+
	cp	temp,	VALUE
	brlo	PC+2
	ori	A_REG,	0x02

	ld	temp,	Z+
	cp	temp,	VALUE
	brlo	PC+2
	ori	A_REG,	0x04

	ld	temp,	Z+
	cp	temp,	VALUE
	brlo	PC+2
	ori	A_REG,	0x08

	ld	temp,	Z+
	cp	temp,	VALUE
	brlo	PC+2
	ori	A_REG,	0x10

	ld	temp,	Z+
	cp	temp,	VALUE
	brlo	PC+2
	ori	A_REG,	0x20

	ori	A_REG,	0xC0
	sbi	PORTD,	0
	out	PORTB,	A_REG
	cbi	PORTD,	1

; get bits for 3rd 6 LEDs (31 cycles)
	clr	A_REG
	
	ld	temp,	Z+
	cp	temp,	VALUE
	brlo	PC+2
	ori	A_REG,	0x01

	ld	temp,	Z+
	cp	temp,	VALUE
	brlo	PC+2
	ori	A_REG,	0x02

	ld	temp,	Z+
	cp	temp,	VALUE
	brlo	PC+2
	ori	A_REG,	0x04

	ld	temp,	Z+
	cp	temp,	VALUE
	brlo	PC+2
	ori	A_REG,	0x08

	ld	temp,	Z+
	cp	temp,	VALUE
	brlo	PC+2
	ori	A_REG,	0x10

	ld	temp,	Z+
	cp	temp,	VALUE
	brlo	PC+2
	ori	A_REG,	0x20

	ori	A_REG,	0xC0
	sbi	PORTD,	1
	out	PORTB,	A_REG
	cbi	PORTD,	2

; give 3rd 6 LEDs time to light (31 cycles)
	ldi	temp,	10
OUTPUT_STEP_WAIT:
	dec	temp
	brne	OUTPUT_STEP_WAIT
	nop
	
; turn off LEDs
	sbi	PORTD,	2

; do an output step for every brightness value from 63 to 1
	dec	VALUE
	breq	PC+2
	rjmp	OUTPUT_FRAME_LOOP

	ret

;-------------------------------------------------------------------------------
; main code
;-------------------------------------------------------------------------------

MAIN:
	clr	COUNT
	ldi	ZH,	0

MAIN_LOOP:
; randomize effect number every 16 effects
	;	movlw		0x0F
	;	andwf		COUNT,W
	;	btfss		STATUS,Z
	;	goto		MAIN_NO_RANDOM
	;	movlw		0
	;	btfsc		PORTA,3
	;	movlw		3
	;	addwf		EFFECT
MAIN_NO_RANDOM:
	inc	COUNT

; show next effect
	inc	EFFECT

	sbrc	EFFECT,	2
	rjmp	MAIN_4
	sbrc	EFFECT,	1
	rjmp	MAIN_2
	sbrc	EFFECT,	0
	rjmp	FLICKER
	rjmp	RUNC
MAIN_2:
	sbrc	EFFECT,	0
	rjmp	BLINK
	rjmp	WORMCC
MAIN_4:
	sbrc	EFFECT,	1
	rjmp	MAIN_6
	sbrc	EFFECT,	0
	rjmp	WOBBLE
	rjmp	RUNCC
MAIN_6:
	sbrc	EFFECT,	0
	rjmp	GLOW
	rjmp	WORMC

;-------------------------------------------------------------------------------
; blinking heart
; MODIFIES:	PORTA, PORTB, A_REG, B_REG, C_REG, VALUE, PIXELS
;-------------------------------------------------------------------------------

BLINK:

; blink 3 times
	ldi	C_REG,	3
BLINK_LOOP:

; turn all pixels on
	ldi	temp,	63
	rcall	SETPIX

; output 50 frames
	ldi	B_REG,	50
BLINK_ON:
	rcall	OUTPUT
	dec	B_REG
	brne	BLINK_ON

; turn all pixels off
	ldi	temp,	0
	rcall	SETPIX

; output 50 frames
	ldi	B_REG,	50
BLINK_OFF:
	rcall	OUTPUT
	dec	B_REG
	brne	BLINK_OFF

; blink 3 times
	dec	C_REG
	brne	BLINK_LOOP

; continue with main loop
	rjmp	MAIN_LOOP

;-------------------------------------------------------------------------------
; glowing heart
; MODIFIES:	PORTA, PORTB, A_REG, B_REG, C_REG, VALUE, PIXELS
;-------------------------------------------------------------------------------
GLOW:

; glow 3 times
	ldi	C_REG,	3
GLOW_LOOP:

; increase glowing
	ldi	B_REG,	0
GLOW_INC:

; set all pixels
	mov	temp,	B_REG
	rcall	SETPIX

; output 2 frames
	rcall	OUTPUT
	rcall	OUTPUT

; increase glowing
	inc	B_REG
	cpi	B_REG,	63
	brne	GLOW_INC

; decrease glowing
	ldi	B_REG,	63
GLOW_DEC:

; set all pixels
	mov	temp,	B_REG
	rcall	SETPIX

; output 2 frames
	rcall	OUTPUT
	rcall	OUTPUT

; decrease glowing
	dec	B_REG
	brne	GLOW_DEC

; glow 3 times
	dec	C_REG
	brne	GLOW_LOOP

; continue with main loop
	rjmp	MAIN_LOOP

;-------------------------------------------------------------------------------
; flickering heart
; MODIFIES:	PORTA, PORTB, A_REG, B_REG, C_REG, VALUE, PIXELS
;-------------------------------------------------------------------------------

FLICKER:

; flicker 10 times
	ldi	C_REG,	10
FLICKER_LOOP:

; turn odd pixels on
	ldi	ZL,	PIXELS
	ldi	temp,	63
	ldi	temp2,	0
	st	Z+,	temp2
	st	Z+,	temp
	st	Z+,	temp2
	st	Z+,	temp
	st	Z+,	temp2
	st	Z+,	temp
	st	Z+,	temp2
	st	Z+,	temp
	st	Z+,	temp2
	st	Z+,	temp
	st	Z+,	temp2
	st	Z+,	temp
	st	Z+,	temp2
	st	Z+,	temp
	st	Z+,	temp2
	st	Z+,	temp
	st	Z+,	temp2
	st	Z+,	temp

; output 15 frames
	ldi	B_REG,	15
FLICKER_ODD:
	rcall	OUTPUT
	dec	B_REG
	brne	FLICKER_ODD

; turn even pixels on
	ldi	ZL,	PIXELS
	ldi	temp,	63
	st	Z+,	temp
	st	Z+,	temp2
	st	Z+,	temp
	st	Z+,	temp2
	st	Z+,	temp
	st	Z+,	temp2
	st	Z+,	temp
	st	Z+,	temp2
	st	Z+,	temp
	st	Z+,	temp2
	st	Z+,	temp
	st	Z+,	temp2
	st	Z+,	temp
	st	Z+,	temp2
	st	Z+,	temp
	st	Z+,	temp2
	st	Z+,	temp
	st	Z+,	temp2

; output 15 frames
	ldi	B_REG,	15
FLICKER_EVEN:
	rcall	OUTPUT
	dec	B_REG
	brne	FLICKER_EVEN

; flicker 10 times
	dec	C_REG
	brne	FLICKER_LOOP

; continue with main loop
	rjmp	MAIN_LOOP

;-------------------------------------------------------------------------------
; wobbling heart
; MODIFIES:	PORTA, PORTB, A_REG, B_REG, C_REG, VALUE, PIXELS
;-------------------------------------------------------------------------------

WOBBLE:

; wobble 5 times
	ldi	D_REG,	5
WOBBLE_LOOP:

; forward
	clr	B_REG
	ldi	C_REG,	63
WOBBLE_FW:

; set all pixels
	ldi	ZL,	PIXELS
	st	Z+,	B_REG
	st	Z+,	C_REG
	st	Z+,	B_REG
	st	Z+,	C_REG
	st	Z+,	B_REG
	st	Z+,	C_REG
	st	Z+,	B_REG
	st	Z+,	C_REG
	st	Z+,	B_REG
	st	Z+,	C_REG
	st	Z+,	B_REG
	st	Z+,	C_REG
	st	Z+,	B_REG
	st	Z+,	C_REG
	st	Z+,	B_REG
	st	Z+,	C_REG
	st	Z+,	B_REG
	st	Z+,	C_REG

; output 2 frames
	rcall	OUTPUT
	rcall	OUTPUT

; forward
	inc	B_REG
	dec	C_REG
	brne	WOBBLE_FW

; backward
	ldi	B_REG,	63
	clr	C_REG
WOBBLE_BW:

; set all pixels
	ldi	ZL,	PIXELS
	st	Z+,	B_REG
	st	Z+,	C_REG
	st	Z+,	B_REG
	st	Z+,	C_REG
	st	Z+,	B_REG
	st	Z+,	C_REG
	st	Z+,	B_REG
	st	Z+,	C_REG
	st	Z+,	B_REG
	st	Z+,	C_REG
	st	Z+,	B_REG
	st	Z+,	C_REG
	st	Z+,	B_REG
	st	Z+,	C_REG
	st	Z+,	B_REG
	st	Z+,	C_REG
	st	Z+,	B_REG
	st	Z+,	C_REG

; output 2 frames
	rcall	OUTPUT
	rcall	OUTPUT

; backward
	inc	C_REG
	dec	B_REG
	brne	WOBBLE_BW

; wobble 5 times
	dec	D_REG
	brne	WOBBLE_LOOP

; continue with main loop
	rjmp	MAIN_LOOP

;-------------------------------------------------------------------------------
; running light - clockwise
; MODIFIES:	PORTA, PORTB, A_REG, B_REG, C_REG, VALUE, PIXELS
;-------------------------------------------------------------------------------
RUNC:

; run 5 rounds
	ldi	D_REG,	5
RUNC_LOOP:

; turn on two pixels opposite to each other clockwise
	clr	C_REG
RUNC_STEP:

; turn all pixels off
	ldi	temp,	0
	rcall	SETPIX

; turn on C_REG-th and C_REG+9-th pixel
	ldi	ZL,	PIXELS
	add	ZL,	C_REG
	ldi	temp,	63
	st	Z,	temp
	subi	ZL,	-9
	st	Z,	temp

; output 20 frames
	ldi	B_REG,	20
RUNC_OUT:
	rcall	OUTPUT
	dec	B_REG
	brne	RUNC_OUT

; turn on two pixels opposite to each other clockwise
	inc	C_REG
	cpi	C_REG,	9		; skip if C_REG is 9
	brne	RUNC_STEP

; run 5 rounds
	dec	D_REG
	brne	RUNC_LOOP

; continue with main loop
	rjmp	MAIN_LOOP

;-------------------------------------------------------------------------------
; running light - couter-clockwise
; MODIFIES:	PORTA, PORTB, A_REG, B_REG, C_REG, VALUE, PIXELS
;-------------------------------------------------------------------------------
RUNCC:

; run 5 rounds
	ldi	D_REG,	5
RUNCC_LOOP:

; turn on two pixels opposite to each other counter-clockwise
	ldi	C_REG,	8
RUNCC_STEP:

; turn all pixels off
	ldi	temp,	0
	rcall	SETPIX

; turn on C_REG-th and C_REG+9-th pixel
	ldi	ZL,	PIXELS
	add	ZL,	C_REG
	ldi	temp,	63
	st	Z,	temp
	subi	ZL,	-9
	st	Z,	temp

; output 20 frames
	ldi	B_REG,	20
RUNCC_OUT:
	rcall	OUTPUT
	dec	B_REG
	brne	RUNCC_OUT

; turn on two pixels opposite to each other counter-clockwise
	dec	C_REG
	cpi	C_REG,	0xFF		; skip if C_REG is -1
	brne	RUNCC_STEP

; run 5 rounds
	dec	D_REG
	brne	RUNCC_LOOP

; continue with main loop
	rjmp	MAIN_LOOP

;-------------------------------------------------------------------------------
; running worm - clockwise
; MODIFIES:	PORTA, PORTB, A_REG, B_REG, C_REG, VALUE, PIXELS
;-------------------------------------------------------------------------------
WORMC:

; turn all pixels off
	ldi	temp,	0
	rcall	SETPIX

; run 3 rounds
	ldi	D_REG,	3
WORMC_LOOP:

; let worm run one round clockwise
	clr	C_REG
WORMC_STEP:

; dim down every pixel
	ldi	ZL,	PIXELS
	
	ldi	temp2,	18
WORMC_DIM:
	ld	temp,	Z
	lsr	temp
	st	Z+,	temp
	dec	temp2
	brne	WORMC_DIM
	
; turn on C_REG-th pixel
	ldi	ZL,	PIXELS
        add	ZL,	C_REG
	ldi	temp,	63
	st	Z,	temp

; output 20 frames
	ldi	B_REG,	20
WORMC_OUT:
	rcall	OUTPUT
	dec	B_REG
	brne	WORMC_OUT

; let worm run one round clockwise
	inc	C_REG
	cpi	C_REG,	18		; skip if C_REG is 18
	brne	WORMC_STEP

; run 3 rounds
	dec	D_REG
	brne	WORMC_LOOP

; continue with main loop
	rjmp	MAIN_LOOP

;-------------------------------------------------------------------------------
; running worm - counter-clockwise
; MODIFIES:	PORTA, PORTB, A_REG, B_REG, C_REG, VALUE, PIXELS
;-------------------------------------------------------------------------------
WORMCC:

; turn all pixels off
	ldi	temp,	0
	rcall	SETPIX

; run 3 rounds
	ldi	D_REG,	3
WORMCC_LOOP:

; let worm run one round counter-clockwise
	ldi	C_REG,	17
WORMCC_STEP:

; dim down every pixel
	ldi	ZL,	PIXELS
	
	ldi	temp2,	18
WORMCC_DIM:
	ld	temp,	Z
	lsr	temp
	st	Z+,	temp
	dec	temp2
	brne	WORMCC_DIM
	
; turn on C_REG-th pixel
	ldi	ZL,	PIXELS
        add	ZL,	C_REG
	ldi	temp,	63
	st	Z,	temp

; output 20 frames
	ldi	B_REG,	20
WORMCC_OUT:
	rcall	OUTPUT
	dec	B_REG
	brne	WORMCC_OUT

; let worm run one round counter-clockwise
	dec	C_REG
	cpi	C_REG,	0xFF		; skip if C_REG is -1
	brne	WORMCC_STEP

; run 3 rounds
	dec	D_REG
	brne	WORMCC_LOOP

; continue with main loop
	rjmp	MAIN_LOOP



;===============================================================================
; end of file
;===============================================================================
