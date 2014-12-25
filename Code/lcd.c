#ifndef lcd_C
#define lcd_C

#include <avr\io.h>
#include <stdio.h>
#include <util\delay.h>
#include "lcd.h"

void lcd_config(void)
{
	DDRB |=0xE0; //1110 0000
	DDRA |=0x0F; //0000 1111

	PORTB &=~(1<<RW);
	return;
}

void lcd_init(void)
{
	lcd_config();
	_delay_ms(50);				// wait after power up

	lcd_nibble(0x03,0);			// send 03h 3 times to initialize

	lcd_en_tgl();
	_delay_ms(10);

	lcd_en_tgl();
	_delay_us(250);
	
	lcd_en_tgl();
	_delay_ms(10);

	lcd_nibble(0x02,0);			// enable 4 bit mode
	_delay_us(250);

	lcd_cmd(0x28);					//set 4-bit mode and 2 lines @ 5x7
	_delay_us(250);
	
	lcd_cmd(0x10);					//cursor move & shift left
	_delay_us(250);

	lcd_cmd(0x06);					//entry mode = increment
	_delay_us(250);
	
	lcd_cmd(0x0C);					//display on - cursor blink on
	_delay_us(250);

	lcd_cmd(0x01);					//clear display ram
	_delay_ms(10);

	return;
}

void lcd_en_tgl (void)
{
	PORTB |=(1<<EN);
	PORTB &=~(1<<EN);					//Enable pin
	return;
}

void lcd_cmd(unsigned char letter)
{
	// Command Function
	//The RS is set to 0 to signify this is a command
	unsigned char temp;             // temp Variable

	temp=letter;                    //move letter to temp
	temp=temp>>4;                   //shift temp to right by 4
	lcd_nibble(temp,0);             //send out higher nibble

	temp=letter;                    //move letter to temp
	temp=temp&0x0F;                 
	lcd_nibble(temp,0);             //send out lower nibble
	_delay_us(50);
	return;
}

void lcd_char(unsigned char letter)
{
	//TData  Function
	//The RS is set to 1 to signify this is a command
	unsigned char temp;             // temp Variable

	temp=letter;                    //move letter to temp
	temp=temp>>4;                   //shift temp to right by 4
	lcd_nibble(temp,1);             //send out higher nibble

	temp=letter;                    //move letter to temp
	temp=temp&0x0F;                 
	lcd_nibble(temp,1);             //send out lower nibble

	_delay_us(50);
	return;
}

void lcd_nibble(unsigned char nib,unsigned char reg_sel)
{
	//unsigned char i,x;

	if(reg_sel)
		PORTB |= (1<<RS);               // Set RS Pin (defined in header file)
	else
		PORTB &= ~(1<<RS); 
	//nib=(nib<<4);
	PORTA=nib; //0000 1111
	_delay_us(20);
	lcd_en_tgl();                   // Toggle E pin (defined in header file)
	return;
}

void lcd_clr_line(unsigned char line)
{
	unsigned char x=0;
// clear line cxC0;                         // 2 = Line 2
	lcd_cmd(line);                         // Send command to jump to beggining of line (1/2)
    _delay_us(200);

    for(x=0x00;x<0x14;x++)
	{                   					    // Loop through all 20 chars of line (even tho 16 are viewable)
        lcd_char(0x20);                     // Send Blank Character
        _delay_us(200);
    }
    lcd_cmd(line);                         // Go back to beggining of line
    _delay_us(200);
	return;
}

void lcd_string(char *senpoint, unsigned char line)
{
    if(line==1)
        lcd_cmd(line1);
    if(line==2)
        lcd_cmd(line2);
    while(*senpoint != '\0')            // While we havent seen a \0 (esc) go on
	{
		lcd_char(*senpoint);            // Send 1st char to our char function
		senpoint++;                     // Send next
	}

	return;
}

void lcd_string_position (char *senpoint, unsigned char line, unsigned char position, unsigned char l_r)	//position starts from 0
{
	lcd_cmd (line);
  	while (position != 0)			//first position or home position is 0 and last position is 15
  	{
    	if(l_r)
      	lcd_cmd(CURSOR_SHIFT_LEFT);	//move to left 
    	else
      	lcd_cmd(CURSOR_SHIFT_RIGHT);	//move to right 
    	position--;
  	}
  	while (*senpoint != '\0' )
  	{
    	lcd_char(*senpoint);		// send the character to be displayed
    	senpoint++;
  	}
}

/*************************************************************************
Set cursor to specified position
Input:    x  horizontal position  (0: left most position)
          y  vertical position    (0: first line)
Returns:  none
*************************************************************************/
void lcd_gotoxy(uint8_t x,uint8_t y)
{
 if(x<40)
 {
  if(y) x|=0b01000000;
  x|=0b10000000;
  lcd_cmd(x);
  }
}
/* lcd_gotoxy */

void lcdwritestring(const char *msg)
{
	/*****************************************************************
	
	This function Writes a given string to lcd at the current cursor
	location.

	Arguments:
	msg: a null terminated string to print


	*****************************************************************/
 while(*msg!='\0')
 {
	lcd_char(*msg);
	msg++;
 }
}

void lcdwriteint(unsigned int val,unsigned int field_length)
{
	/***************************************************************
	This function writes a integer type value to LCD module

	Arguments:
	1)int val	: Value to print

	2)unsigned int field_length :total length of field in which the value is printed
	must be between 1-5 if it is -1 the field length is no of digits in the val

	****************************************************************/

	char str[6]={0,0,0,0,0,0};
	int i=5,j=0;
	unsigned int local_copy = val;
	while(val)
	{
		str[i]=val%10;
		val=val/10;
		i--;
	}
	if(field_length==-1)
		while(str[j]==0) j++;
	else
	{
		if(local_copy <= 9)
		{
			j = 5;
		}
		else if(local_copy <= 99)
		{
			j = 4;
		}
		else if(local_copy <= 999)
		{
			j = 3;
		}
		else if(local_copy <= 9999)
		{
			j = 2;
		}
		else if(local_copy <= 99999)
		{
			j = 1;
		}
		else if(local_copy <= 999999)
			j = 0;
		//j=6-field_length;
	}
	
	if(val<0) 
		lcd_char('-');
	for(i=j;i<6;i++)
	{
		lcd_char(48+str[i]);
	}
}

void lcdwritefloat(float val, unsigned char decimal_length) // Decimal Length between 1 to 6
{
	/***************************************************************
	This function writes a integer type value to LCD module

	Arguments:
	1)int val	: Value to print

	2)unsigned int field_length :total length of field in which the value is printed
	must be between 1-5 if it is -1 the field length is no of digits in the val

	****************************************************************/

	char str[6]={0,0,0,0,0,0};
	char dec_str[6] = {0, 0, 0, 0, 0, 0};
	int i=5,j=0, m = 5, n = 0;
	unsigned int Process_val_int, Process_val_dec, local_copy_dec = 0, int_component = (unsigned int) val;
	float dec_component = (val - int_component);

	if(decimal_length == 1) 
	{
		local_copy_dec = (unsigned int) (dec_component * 10);
		n = 5;
	}
	else if(decimal_length == 2)
	{
		local_copy_dec = (unsigned int) (dec_component * 100);
		n = 4;
	}
	else if(decimal_length == 3)
	{
		local_copy_dec = (unsigned int) (dec_component * 1000);
		n = 3;
	}
	else if(decimal_length == 4)
	{
		local_copy_dec = (unsigned int) (dec_component * 10000);
		n = 2;
	}
	else if(decimal_length == 5)
	{
		local_copy_dec = (unsigned int) (dec_component * 100000);
		n = 1;
	}

	else if(decimal_length == 6)
	{
		local_copy_dec = (unsigned int) (dec_component * 100000);
		n = 0;
	}

	Process_val_int = int_component;

	while(Process_val_int)
	{
		str[i]=Process_val_int%10;
		Process_val_int=Process_val_int/10;
		i--;
	}
	
	Process_val_dec = local_copy_dec;

	while(Process_val_dec)
	{
		dec_str[m] = Process_val_dec%10;
		Process_val_dec = Process_val_dec/10;
		m--;
	}

	
	if(int_component <= 9)
	{
		j = 5;
	}
	else if(int_component <= 99)
	{
		j = 4;
	}
	else if(int_component <= 999)
	{
		j = 3;
	}
	else if(int_component <= 9999)
	{
		j = 2;
	}
	else if(int_component <= 99999)
	{
		j = 1;
	}
	else if(int_component <= 999999)
	{
		j = 0;
	}
	
	/*if(local_copy_dec <= 9)
	{
		n = 5;
	}
	else if(local_copy_dec <= 99)
	{
		n = 4;
	}
	else if(local_copy_dec <= 999)
	{
		n = 3;
	}
	else if(local_copy_dec <= 9999)
	{
		n = 2;
	}
	else if(local_copy_dec <= 99999)
	{
		n = 1;
	}
	else if(local_copy_dec <= 999999)
	{
		n = 0;
	}
	//j=6-field_length;
	*/
	
	if(Process_val_int<0) 
		lcd_char('-');
	for(i=j;i<6;i++)
	{
		lcd_char(48+str[i]);
	}

	lcd_char('.');

	if(Process_val_dec<0) 
		lcd_char('-');
	for(m=n;m<6;m++)
	{
		lcd_char(48+dec_str[m]);
	}
}
	
#endif
