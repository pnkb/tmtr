#include <avr/io.h>
#include "SPI.h"


void SPI_Init( )
{
	/* Set MOSI and SCK output, all others input */
	SPI_DDR = (1<<MOSI_POS)|(1<<SCK_POS)|(1<<SS_POS);
	
	SPI_PORT |= (1<<SS_POS);
	/* Enable SPI, Master, set clock rate fck/128 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0);
	
	CS_HIGH;
}

unsigned char SPI_Send (unsigned char val)
{
	/* Start transmission */
	SPDR = val;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}

void SPIClose()
{
	SPCR&=(~(1<<SPE));
	SPI_DDR&=(~((1<<MOSI_POS)|(1<<SCK_POS)));
}
