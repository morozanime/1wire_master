
#include	"main.h"
#include 	<avr/io.h>

.text
.global	TIMER1_COMPA_vect
.global TIMER1_COMPA_tbl
.global state

TIMER1_COMPA_vect:
	push	r30
	in		r30, _SFR_IO_ADDR(SREG)
	push	r30
	push	r31

	in		r30, _SFR_IO_ADDR(TIMSK)
	andi	r30, ~(1 << OCIE1A)
	out		_SFR_IO_ADDR(TIMSK), r30

	push	r24
	ldi		r30,lo8(TIMER1_COMPA_tbl)
	ldi		r31,hi8(TIMER1_COMPA_tbl)
	clc
	ror		r31
	ror		r30
	lds		r24,state
	add		r30,r24
	adc		r31,r1
	icall
	pop		r24

	pop		r31
	pop		r30
	out		_SFR_IO_ADDR(SREG),r30
	pop		r30
	reti

TIMER1_COMPA_tbl:
	rjmp	OWI_IDLE_TIMER1_COMPA_vect			;0
	rjmp	OWI_RESET0_TIMER1_COMPA_vect		;1
	rjmp	OWI_RESET_WF1_TIMER1_COMPA_vect		;2
	rjmp	OWI_WF_PRESENCE0_TIMER1_COMPA_vect	;3
	rjmp	OWI_WF_PRESENCE1_TIMER1_COMPA_vect	;4
	rjmp	OWI_PRESENCE_TIMER1_COMPA_vect		;5
	rjmp	OWI_READ_BYTE_OK_TIMER1_COMPA_vect	;6
	rjmp	OWI_WRITE_BIT0_TIMER1_COMPA_vect	;7
	rjmp	OWI_WRITE_BIT0_TIMER1_COMPA_vect	;8
	rjmp	OWI_WRITE_BIT0_TIMER1_COMPA_vect	;9
	rjmp	OWI_WRITE_BIT0_TIMER1_COMPA_vect	;10
	rjmp	OWI_WRITE_BIT0_TIMER1_COMPA_vect	;11
	rjmp	OWI_WRITE_BIT0_TIMER1_COMPA_vect	;12
	rjmp	OWI_WRITE_BIT0_TIMER1_COMPA_vect	;13
	rjmp	OWI_WRITE_BIT0_TIMER1_COMPA_vect	;14
	rjmp	OWI_WRITE_BYTE_OK_TIMER1_COMPA_vect	;15
	rjmp	OWI_READ_BIT0_TIMER1_COMPA_vect		;16
	rjmp	OWI_READ_BIT0_TIMER1_COMPA_vect		;17
	rjmp	OWI_READ_BIT0_TIMER1_COMPA_vect		;18
	rjmp	OWI_READ_BIT0_TIMER1_COMPA_vect		;19
	rjmp	OWI_READ_BIT0_TIMER1_COMPA_vect		;20
	rjmp	OWI_READ_BIT0_TIMER1_COMPA_vect		;21
	rjmp	OWI_READ_BIT0_TIMER1_COMPA_vect		;22
	rjmp	OWI_READ_BIT0_TIMER1_COMPA_vect		;23
	rjmp	OWI_READ_BITA0_TIMER1_COMPA_vect	;24
	rjmp	OWI_READ_BITA0_TIMER1_COMPA_vect	;25
	rjmp	OWI_READ_BITA0_TIMER1_COMPA_vect	;26
	rjmp	OWI_READ_BITA0_TIMER1_COMPA_vect	;27
	rjmp	OWI_READ_BITA0_TIMER1_COMPA_vect	;28
	rjmp	OWI_READ_BITA0_TIMER1_COMPA_vect	;29
	rjmp	OWI_READ_BITA0_TIMER1_COMPA_vect	;30
	rjmp	OWI_READ_BITA0_TIMER1_COMPA_vect	;31

OWI_IDLE_TIMER1_COMPA_vect:
OWI_PRESENCE_TIMER1_COMPA_vect:
OWI_WRITE_BYTE_OK_TIMER1_COMPA_vect:
OWI_PRESENCE_TIMER1_COMPA_vect:
OWI_RESET_WF1_TIMER1_COMPA_vect:
OWI_READ_BYTE_OK_TIMER1_COMPA_vect:
	ret

OWI_RESET0_TIMER1_COMPA_vect:
	ldi		r30,OWI_RESET_WF1
	sts 	state,r30

	in		r30,_SFR_IO_ADDR(MCUCR)
	ori 	r30,(1 << ISC00)
	out		_SFR_IO_ADDR(MCUCR),r30

	ldi		r30,(1 << INTF0)
	out		_SFR_IO_ADDR(GIFR),r30

	in		r30,_SFR_IO_ADDR(GICR)
	ori		r30,(1 << INT0)
	out 	_SFR_IO_ADDR(GICR),r30

	cbi		_SFR_IO_ADDR(DDRD),2
	ret

OWI_WF_PRESENCE0_TIMER1_COMPA_vect:
	ldi		r30,OWI_IDLE
	sts 	state,r30
	ret

OWI_WF_PRESENCE1_TIMER1_COMPA_vect:
	lds 	r30,(tp)
	lds 	r31,(tp+1)
	sbiw	r30, 60
	cpi		r30, 181
	cpc		r31, r1
	brcc	OWI_WF_PRESENCE1_TIMER1_COMPA_vect_0

	ldi		r30,OWI_PRESENCE
	sts		state,r30
	ret
OWI_WF_PRESENCE1_TIMER1_COMPA_vect_0:
	ldi		r30,OWI_IDLE
	sts 	state,r30
	ret

OWI_WRITE_BIT0_TIMER1_COMPA_vect:
	cbi		_SFR_IO_ADDR(DDRD),2
	ret

OWI_READ_BIT0_TIMER1_COMPA_vect:
	sbi		_SFR_IO_ADDR(PORTB),0
	cbi		_SFR_IO_ADDR(DDRD),2

	lds		r30,state
	subi	r30,-8
	sts		state,r30

	in		r30,_SFR_IO_ADDR(TCNT1L)
	in		r31,_SFR_IO_ADDR(TCNT1H)
	adiw	r30,DELAY_US(10)
	out		_SFR_IO_ADDR(OCR1AL),r30
	out		_SFR_IO_ADDR(OCR1AH),r31

	ldi		r30,(1 << OCF1A)
	out		_SFR_IO_ADDR(TIFR),r30

	in		r30,_SFR_IO_ADDR(TIMSK)
	ori		r30,(1 << OCIE1A)
	out		_SFR_IO_ADDR(TIMSK),r30

	cbi		_SFR_IO_ADDR(PORTB),0
	ret

OWI_READ_BITA0_TIMER1_COMPA_vect:
	sbi		_SFR_IO_ADDR(PORTB),1

	lds		r30,byte
	lsr		r30
	sbic	_SFR_IO_ADDR(PIND),2
	ori		r30,0x80
	sts		byte,r30

	lds		r30,state
	subi	r30,8
	sts		state,r30

	cbi		_SFR_IO_ADDR(PORTB),1
	ret

