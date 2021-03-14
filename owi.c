/*
 * owi.c
 *
 *  Created on: 11 мар. 2021 г.
 *      Author: User
 */

#include	<avr/interrupt.h>
#include	"main.h"
#include	"owi.h"

static uint8_t state;
static uint16_t tp;
static uint8_t byte;

/* Подсчет CRC8 массива mas длиной Len */
uint8_t owi_calc_crc(uint8_t *mas, uint8_t Len) {
	uint8_t i, dat, crc, fb, st_byt;
	st_byt = 0;
	crc = 0;
	do {
		dat = mas[st_byt];
		for (i = 0; i < 8; i++) {  // счетчик битов в байте
			fb = crc ^ dat;
			fb &= 1;
			crc >>= 1;
			dat >>= 1;
			if (fb == 1)
				crc ^= 0x8c; // полином
		}
		st_byt++;
	} while (st_byt < Len); // счетчик байтов в массиве
	return crc;
}

uint8_t owi_getState(void) {
	return state;
}

void owi_init(void) {
	state = OWI_IDLE;
	DDRD &= ~0x04;
	PORTD &= ~0x04;
	TCCR1A = 0;
	TCCR1B = 1 << CS11;
	MCUCR |= (1 << ISC01);
}

void owi_reset(void) {
	DDRD |= 0x04;
	OCR1A = TCNT1 + DELAY_US(500);
	state = OWI_RESET0;
	TIFR = 1 << OCF1A;
	TIMSK |= (1 << OCIE1A);
}

void owi_write_byte(uint8_t b) {
	byte = b;
	state = OWI_WRITE_BIT0;
	DDRD |= 0x04;
	if (byte & 1)
		OCR1A = TCNT1 + DELAY_US(3);
	else
		OCR1A = TCNT1 + DELAY_US(90);
	byte >>= 1;
	OCR1B = TCNT1 + DELAY_US(100);
	TIFR = (1 << OCF1A) | (1 << OCF1B);
	TIMSK |= (1 << OCIE1A) | (1 << OCIE1B);
}

void owi_read_byte(void) {
	state = OWI_READ_BIT0;
	DDRD |= 0x04;
	OCR1A = TCNT1 + DELAY_US(10);
	OCR1B = TCNT1 + DELAY_US(100);
	TIFR = (1 << OCF1A) | (1 << OCF1B);
	TIMSK |= (1 << OCIE1A) | (1 << OCIE1B);
}

uint8_t owi_get_byte(void) {
	return byte;
}

ISR(TIMER1_COMPA_vect) {
	if (state <= OWI_READ_BIT7) {
		DDRD &= ~0x04;
		OCR1A = TCNT1 + DELAY_US(3);
		TIFR = (1 << OCF1A);
		state += OWI_READ_BITA0 - OWI_READ_BIT0;
	} else if (state <= OWI_READ_BITA7) {
		byte >>= 1;
		if (PIND & 0x04)
			byte |= 0x80;
		TIMSK &= ~(1 << OCIE1A);
		state -= OWI_READ_BITA0 - OWI_READ_BIT0;
	} else if (state <= OWI_WRITE_BIT7) {
		DDRD &= ~0x04;
		TIMSK &= ~(1 << OCIE1A);
	} else if (state == OWI_RESET0) {
		state = OWI_RESET_WF1;
		MCUCR |= (1 << ISC00);
		GIFR = 1 << INTF0;
		GICR |= 1 << INT0;
		DDRD &= ~0x04;
		TIMSK &= ~(1 << OCIE1A);
	} else if (state == OWI_WF_PRESENCE0) {
		state = OWI_IDLE;
		TIMSK &= ~(1 << OCIE1A);
	} else if (state == OWI_WF_PRESENCE1) {
		if ((tp >= DELAY_US(60)) && (tp <= DELAY_US(240)))
			state = OWI_PRESENCE;
		else
			state = OWI_IDLE;
		TIMSK &= ~(1 << OCIE1A);
	}
}

 ISR(TIMER1_COMPB_vect) {
	if (state < OWI_READ_BIT7) {
		DDRD |= 0x04;
		OCR1A = TCNT1 + DELAY_US(3);
		OCR1B = TCNT1 + DELAY_US(100);
		TIFR = (1 << OCF1A) | (1 << OCF1B);
		TIMSK |= (1 << OCIE1A) | (1 << OCIE1B);
		state++;
	} else if (state < OWI_WRITE_BIT7) {
		DDRD |= 0x04;
		if (byte & 1)
			OCR1A = TCNT1 + DELAY_US(3);
		else
			OCR1A = TCNT1 + DELAY_US(90);
		OCR1B = TCNT1 + DELAY_US(100);
		TIFR = (1 << OCF1A) | (1 << OCF1B);
		TIMSK |= (1 << OCIE1A) | (1 << OCIE1B);
		state++;
		byte >>= 1;
	} else if (state == OWI_READ_BIT7) {
		state = OWI_READ_BYTE_OK;
		TIMSK &= ~(1 << OCIE1B);
	} else if (state == OWI_WRITE_BIT7) {
		state = OWI_WRITE_BYTE_OK;
		TIMSK &= ~(1 << OCIE1B);
	}
}

ISR(INT0_vect) {
	if (state == OWI_RESET_WF1) {
		state = OWI_WF_PRESENCE0;
		OCR1A = TCNT1 + DELAY_US(500);
		MCUCR &= ~(1 << ISC00);
		GIFR = (1 << INTF0);
		TIFR = (1 << OCF1A);
		TIMSK |= (1 << OCIE1A);
	} else if (state == OWI_WF_PRESENCE0) {
		state = OWI_WF_PRESENCE1;
		tp = TCNT1;
		MCUCR |= (1 << ISC00);
		GIFR = 1 << INTF0;
	} else if (state == OWI_WF_PRESENCE1) {
		tp = TCNT1 - tp;
		GICR &= ~(1 << INT0);
	}
}
