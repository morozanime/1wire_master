/*
 * owi.c
 *
 *  Created on: 11 мар. 2021 г.
 *      Author: User
 */

#include	<avr/interrupt.h>
#include	"main.h"
#include	"owi.h"

#define	DELAY_US(x)	((F_CPU/1000000*(x))/8)

volatile static uint8_t state;
volatile static uint16_t tp;
volatile static uint8_t byte;

uint8_t owi_getState(void) {
	return state;
}

void owi_init(void) {
	state = OWI_IDLE;
	DDRD &= ~0x04;
	PORTD &= ~0x04;
	TCCR1A = 0;
	TCCR1B = 1 << CS11;
	MCUCR |= (1 << ISC01) | (1 << ISC00);
}

void owi_reset(void) {
	tp = 0;
	DDRD |= 0x04;
	OCR1A = TCNT1 + DELAY_US(500);
	state = OWI_RESET0;
	TIFR = 1 << OCF1A;
	TIMSK |= (1 << OCIE1A);
}

void owi_write_byte(uint8_t b) {
	byte = b;
	state = OWI_WRITE_BIT0;
	uint16_t t0 = TCNT1;
	DDRD |= 0x04;
	if (byte & 1)
		OCR1A = t0 + DELAY_US(5);
	else
		OCR1A = t0 + DELAY_US(90);
	OCR1B = t0 + DELAY_US(100);
	TIFR = (1 << OCF1A) | (1 << OCF1B);
	TIMSK |= (1 << OCIE1A) | (1 << OCIE1B);
}

void owi_read_byte(void) {
	state = OWI_READ_BIT0;
	uint16_t t0 = TCNT1;
	DDRD |= 0x04;
	OCR1A = t0 + DELAY_US(5);
	OCR1B = t0 + DELAY_US(100);
	TIFR = (1 << OCF1A) | (1 << OCF1B);
	TIMSK |= (1 << OCIE1A) | (1 << OCIE1B);
}

uint8_t owi_get_byte(void) {
	return byte;
}

ISR(TIMER1_COMPA_vect) {
	TIMSK &= ~(1 << OCIE1A);
	if (state == OWI_RESET0) {
		state = OWI_RESET_WF1;
		MCUCR |= (1 << ISC01) | (1 << ISC00);
		GIFR = 1 << INTF0;
		GICR |= 1 << INT0;
		DDRD &= ~0x04;
	} else if (state == OWI_WF_PRESENCE0) {
		state = OWI_IDLE;
	} else if (state == OWI_WF_PRESENCE1) {
		if (tp >= DELAY_US(60) && tp <= DELAY_US(240))
			state = OWI_PRESENCE;
		else
			state = OWI_IDLE;
	} else if (state >= OWI_WRITE_BIT0 && state < OWI_WRITE_BIT0 + 8) {
		DDRD &= ~0x04;
	} else if (state >= OWI_READ_BIT0 && state < OWI_READ_BIT0 + 8) {
		DDRD &= ~0x04;
		PORTB |= 0x01;
		state += 10;
		OCR1A = TCNT1 + DELAY_US(3);
		TIMSK |= (1 << OCIE1A);
	} else if (state >= (OWI_READ_BIT0 + 10)
			&& state < (OWI_READ_BIT0 + 10 + 8)) {
		PORTB &= ~0x01;
		byte >>= 1;
		if (PIND & 0x04)
			byte |= 0x80;
		state -= 10;
	}
}

ISR(TIMER1_COMPB_vect) {
	TIMSK &= ~(1 << OCIE1B);
	if (state >= OWI_WRITE_BIT0 && state < OWI_WRITE_BIT0 + 7) {
		state++;
		uint16_t t0 = TCNT1;
		DDRD |= 0x04;
		byte >>= 1;
		if (byte & 1)
			OCR1A = t0 + DELAY_US(5);
		else
			OCR1A = t0 + DELAY_US(90);
		OCR1B = t0 + DELAY_US(100);
		TIFR = (1 << OCF1A) | (1 << OCF1B);
		TIMSK |= (1 << OCIE1A) | (1 << OCIE1B);
	} else if (state == OWI_WRITE_BIT0 + 7) {
		state = OWI_WRITE_BYTE_OK;
	} else if (state >= OWI_READ_BIT0 && state < OWI_READ_BIT0 + 7) {
		state++;
		DDRD |= 0x04;
		OCR1A = TCNT1 + DELAY_US(3);
		OCR1B = TCNT1 + DELAY_US(100);
		TIFR = (1 << OCF1A) | (1 << OCF1B);
		TIMSK |= (1 << OCIE1A) | (1 << OCIE1B);
	} else if (state == OWI_READ_BIT0 + 7) {
		state = OWI_READ_BYTE_OK;
	}
}

ISR(INT0_vect) {
	GICR &= ~(1 << INT0);
	switch (state) {
	case OWI_RESET_WF1:
		state = OWI_WF_PRESENCE0;
		OCR1A = TCNT1 + DELAY_US(500);
		MCUCR |= (1 << ISC01);
		MCUCR &= ~(1 << ISC00);
		GIFR = (1 << INTF0);
		TIFR = (1 << OCF1A);
		GICR |= (1 << INT0);
		TIMSK |= (1 << OCIE1A);
		break;
	case OWI_WF_PRESENCE0:
		state = OWI_WF_PRESENCE1;
		tp = TCNT1;
		MCUCR |= (1 << ISC01) | (1 << ISC00);
		GIFR = 1 << INTF0;
		GICR |= 1 << INT0;
		break;
	case OWI_WF_PRESENCE1:
		tp = TCNT1 - tp;
		break;
	}
}
