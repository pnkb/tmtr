#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <string.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "SPI.h"
#include "avrlibtypes.h"
#include "MAX6675.h"
#include "lcd.h"

void atmega_init(void); //Initialize ports
void Temp_LCD_Send_float (float , uint8_t , uint8_t);
uint16_t get_temperature (void);

uint8_t blink;
u08 count;
uint16_t Digital_Temp;
float Temperature;

/***************************** PORT INITIALIZATION MACROS*********************************************/
#define INIT_DDRA		0x0F   // Lower 4 lines are output 0b0000 1111. For LCD data.
#define INIT_DDRB		0xE7   // Higer 3 lines are command lines of LCD
#define INIT_DDRG		0x12   // Heart Beat LED
#define INIT_DDRE		0xE8   // 

#define INIT_PORTA		0xFF   // Internal Pull ups for Data lines
#define INIT_PORTB		0xE0   // Initernal pull ups for control lines
#define INIT_PORTG		0x12
#define INIT_PORTE		0xFF
/*****************************************************************************************************/

#define HB_LED_ON		PORTG |= (1 << PORTG4)
#define HB_LED_OFF		PORTG &= ~(1 << PORTG4)

#define LCD_POWER_ON	PORTG |= (1 << PORTG1)
#define LCD_POWER_OFF	PORTG &= ~(1 << PORTG1)

#define SS_LOW			PORTB &= ~(1<<PORTB0)
#define SS_HIGH			PORTB |= (1 << PORTB0)

#define SCK_LOW			PORTB &= ~(1<<PORTB1)
#define SCK_HIGH		PORTB |= (1 << PORTB1)

#define DATAIN_HIGH     PINB & (1 << PORTB3)

#define DIGITAL_READ(x) ((PINB & (1<<<x))?1:0)


int main()
{
	u32 dummy = 0;
	
	
	atmega_init(); //Initialize ports
	//SPI_Init (); // Initialize SPI
	lcd_init();
	SS_HIGH;
	SCK_LOW;
	_delay_ms(100);
	lcd_string ("  Thermometer  ", 1);

	
	while(1)
	{
		Digital_Temp = get_temperature();

		if(Digital_Temp < 0xFFFF) // Check for the TC error
		{			
			if(count <= 3)
			{
				count++;
				dummy = dummy + Digital_Temp;
			}
			else
			{
				Digital_Temp = (u16) (dummy / 4);
				Temperature = Digital_Temp * 0.25;
				//lcd_string ("               ", 2);
				lcd_string ("Temp = ", 2);
				lcdwritefloat(Temperature, 2);
				lcdwritestring ("`C "); 
				//lcdwriteint(d, 5);
				count = 0;
				dummy = 0;
			}
		}
		else if(Digital_Temp == 0xFFFF) // // TC open condition
		{
			lcd_string("Not Connected ", 2);
			count  = 0;
			dummy = 0;
		}
	
		if(blink)
		{
			HB_LED_OFF;
		}
		else
		{
			HB_LED_ON;
		}

		blink ^= 1;
		_delay_ms(200);
		Digital_Temp = 0;
					
	}
	return 0;
}


void atmega_init (void)
{
	MCUCSR|=0x80;
	MCUCSR|=0x80;
	
	DDRA = INIT_DDRA;
	PORTA = INIT_PORTA;

	DDRB = INIT_DDRB;
	PORTB = INIT_PORTB;

	DDRG = INIT_DDRG; 
	PORTG = INIT_PORTG;
}


uint16_t get_temperature (void)
{
	uint16_t d=0;
	int i;
	unsigned char error;
	SS_LOW; //cs low
		_delay_us(1);
		SS_HIGH; // cs high
		_delay_us(10);
	
		SS_LOW; //cs_low

		SCK_HIGH;
		_delay_us(1);
		SCK_LOW;
		
		for (i = 11; i >= 0; i--)
		{
			SCK_HIGH;			
			//d = d + (DIGITAL_READ(PORTB3) << i);
			if(DATAIN_HIGH)
			{
				d = d + (1 << i);
			}
			else
			{
				d = d + (0 << i);
			}
			SCK_LOW;
			
		}
		
		SCK_HIGH;
		if(DATAIN_HIGH)
		{
			error = 1;
			d = 0xFFFF;
		}
		else
		{
			error = 0;
		}
		SCK_LOW;
		
		for(i = 1; i >= 0; i--)
		{
			SCK_HIGH;
			_delay_us(1);
			SCK_LOW;
		}
		SS_HIGH; //chip select high

		return d;
}



