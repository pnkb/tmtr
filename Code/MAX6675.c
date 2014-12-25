#include "MAX6675.h"
#include "SPI.h"

u16 ReadMAX6675 (void)
{
	u08 lbyte;
	u08 hbyte;
	u08 dummy = 0xFF;
	u16 data;

	CS_LOW;
	lbyte = SPI_Send (dummy);
	hbyte = SPI_Send (dummy);
	CS_HIGH;
	if(CheckForThermocoupleOpen(lbyte))
		return 0; // Thermocouple is open
	else
	{
		data = hbyte << 8;
		data |= lbyte;
		data &= 0x7FF8;
		data = (data >> 3);
		return data;
	}
	return 0; // ontrol wont reach here. To avoid warning
}

u08 CheckForThermocoupleOpen (u08 lower)
{
	if(lower & (1 << 2))
		return 1; // Open COndition
	else
		return 0; // Healthy
}

float CalculateTemp (u16 data)
{
	float temp = 0;

	temp = data * 0.25;
	return temp;
}
