#include	"main.h"

#include	<avr/interrupt.h>
#include	<inttypes.h>
#include	<util/delay.h>
#include	<string.h>
#include	<stdio.h>

#include 	"uart.h"
#include	"owi.h"

//-------------------------------------------------------------------
int main(void) {
	DDRB = 0x3F;
	PORTB = 0x00;

	DDRD = 0xFE;
	PORTD = 0xFD;

	DDRC = 0x00;
	PORTC = 0x3F;

	uart_init(BAUD(9600));
	owi_init();
	sei();
	while (1) {
		uint8_t buff[50];
		owi_reset();
		while (owi_getState() != OWI_PRESENCE && owi_getState() != OWI_IDLE)
			;
		if(owi_getState() == OWI_IDLE){
			_delay_ms(100);
//			sprintf((char*)buff,"%u\n\r", tp);
//			uart_puts(buff);
			continue;
		}

		owi_write_byte(0xcc);
		while (owi_getState() != OWI_WRITE_BYTE_OK)
			;
		owi_write_byte(0x44);
		while (owi_getState() != OWI_WRITE_BYTE_OK)
			;
		_delay_ms(1000);
		owi_reset();
		while (owi_getState() != OWI_PRESENCE && owi_getState() != OWI_IDLE)
			;
		if(owi_getState() == OWI_IDLE){
			_delay_ms(100);
			continue;
		}
		owi_write_byte(0xcc);
		while (owi_getState() != OWI_WRITE_BYTE_OK)
			;
		owi_write_byte(0xbe);
		while (owi_getState() != OWI_WRITE_BYTE_OK)
			;
		uint8_t ddd[9];
		for (uint8_t i = 0; i < 9; i++) {
			owi_read_byte();
			while (owi_getState() != OWI_READ_BYTE_OK)
				;
			ddd[i] = owi_get_byte();
			sprintf((char*) buff, "%02X", ddd[i]);
			uart_puts(buff);
		}
		sprintf((char*) buff, "(%02X)\n\r", owi_calc_crc(ddd, 8));
		uart_puts(buff);
		int16_t ttt = *(int16_t*) ddd;
		char minus = ' ';
		uint16_t n1, n2;
		if (ttt < 0) {
			minus = '-';
			ttt = -ttt - 1;
		}
		n1 = (uint16_t) ttt / 2;
		n2 = (ttt & 1) ? 5 : 0;
		sprintf((char*) buff, "(%c%u.%u)\n\r", minus, n1, n2);
		uart_puts(buff);
	}
}
