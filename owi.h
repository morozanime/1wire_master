/*
 * owi.h
 *
 *  Created on: 11 мар. 2021 г.
 *      Author: User
 */

#ifndef OWI_H_
#define OWI_H_

uint8_t owi_getState(void);
void owi_init(void);
void owi_reset(void);
void owi_write_byte(uint8_t byte);
void owi_read_byte(void);
uint8_t owi_get_byte(void);
uint8_t owi_calc_crc(uint8_t *mas, uint8_t Len);

enum OWI_STATE {
	OWI_READ_BIT0,
	OWI_READ_BIT1,
	OWI_READ_BIT2,
	OWI_READ_BIT3,
	OWI_READ_BIT4,
	OWI_READ_BIT5,
	OWI_READ_BIT6,
	OWI_READ_BIT7,

	OWI_READ_BIT__,
	OWI_READ_BIT___,

	OWI_READ_BITA0,
	OWI_READ_BITA1,
	OWI_READ_BITA2,
	OWI_READ_BITA3,
	OWI_READ_BITA4,
	OWI_READ_BITA5,
	OWI_READ_BITA6,
	OWI_READ_BITA7,

	OWI_READ_BIT__1,
	OWI_READ_BIT___1,

	OWI_WRITE_BIT0,
	OWI_WRITE_BIT1,
	OWI_WRITE_BIT2,
	OWI_WRITE_BIT3,
	OWI_WRITE_BIT4,
	OWI_WRITE_BIT5,
	OWI_WRITE_BIT6,
	OWI_WRITE_BIT7,

	OWI_READ_BYTE_OK,
	OWI_WRITE_BYTE_OK,

	OWI_IDLE,
	OWI_RESET0,
	OWI_RESET_WF1,
	OWI_WF_PRESENCE0,
	OWI_WF_PRESENCE1,
	OWI_PRESENCE,
};

#endif /* OWI_H_ */
