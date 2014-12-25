#include <avr/io.h>
#include "avrlibtypes.h"

#ifndef SPI_H
#define SPI_H

#define SPI_PORT 	PORTB
#define SPI_DDR 	DDRB

//PINS
#define MISO_POS 	PORTB3
#define MOSI_POS	PORTB2
#define SCK_POS 	PORTB1
#define SS_POS		PORTB0

#define CS_LOW		PORTB &= ~(1 << PORTB0)
#define CS_HIGH		PORTB |= (1 << PORTB0)

void SPI_Init (void);
u08 SPI_Send (u08 val);
void SPIClose (void);

#endif
