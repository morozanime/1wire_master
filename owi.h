/*
 * owi.h
 *
 *  Created on: 11 мар. 2021 г.
 *      Author: User
 */

#ifndef OWI_H_
#define OWI_H_

#define	OWI_IDLE			0
#define	OWI_RESET0			1
#define	OWI_RESET_WF1		2
#define	OWI_WF_PRESENCE0	3
#define	OWI_WF_PRESENCE1	4
#define	OWI_PRESENCE		5
#define	OWI_WRITE_BYTE_OK	6
#define	OWI_READ_BYTE_OK	7
#define	OWI_WRITE_BIT0		10
#define	OWI_READ_BIT0		20

uint8_t owi_getState(void);
void owi_init(void);
void owi_reset(void);
void owi_write_byte(uint8_t byte);
void owi_read_byte(void);
uint8_t owi_get_byte(void);

#endif /* OWI_H_ */
