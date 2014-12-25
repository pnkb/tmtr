#ifndef lcd_H
#define lcd_H


#define line1 0x80
#define line2 0xC0

#define EN 5
#define RW 6
#define RS 7

#define CURSOR_SHIFT_LEFT	0x10		//50us delay
#define CURSOR_SHIFT_RIGHT	0x14		//50us delay

#define MOVE_RIGHT			0
#define MOVE_LEFT			1

#define LCD_IO_MODE      	1

/* instruction register bit positions, see HD44780U data sheet */
#define LCD_CLR               0      /* DB0: clear display                  */
#define LCD_HOME              1      /* DB1: return to home position        */
#define LCD_ENTRY_MODE        2      /* DB2: set entry mode                 */
#define LCD_ENTRY_INC         1      /*   DB1: 1=increment, 0=decrement     */
#define LCD_ENTRY_SHIFT       0      /*   DB2: 1=display shift on           */
#define LCD_ON                3      /* DB3: turn lcd/cursor on             */
#define LCD_ON_DISPLAY        2      /*   DB2: turn display on              */
#define LCD_ON_CURSOR         1      /*   DB1: turn cursor on               */
#define LCD_ON_BLINK          0      /*     DB0: blinking cursor ?          */
#define LCD_MOVE              4      /* DB4: move cursor/display            */
#define LCD_MOVE_DISP         3      /*   DB3: move display (0-> cursor) ?  */
#define LCD_MOVE_RIGHT        2      /*   DB2: move right (0-> left) ?      */
#define LCD_FUNCTION          5      /* DB5: function set                   */
#define LCD_FUNCTION_8BIT     4      /*   DB4: set 8BIT mode (0->4BIT mode) */
#define LCD_FUNCTION_2LINES   3      /*   DB3: two lines (0->one line)      */
#define LCD_FUNCTION_10DOTS   2      /*   DB2: 5x10 font (0->5x7 font)      */
#define LCD_CGRAM             6      /* DB6: set CG RAM address             */
#define LCD_DDRAM             7      /* DB7: set DD RAM address             */
#define LCD_BUSY              7      /* DB7: LCD is busy                    */

/* move cursor/shift display */
#define LCD_MOVE_CURSOR_LEFT     0x10   /* move cursor left  (decrement)          */
#define LCD_MOVE_CURSOR_RIGHT    0x14   /* move cursor right (increment)          */
#define LCD_MOVE_DISP_LEFT       0x18   /* shift display left                     */
#define LCD_MOVE_DISP_RIGHT      0x1C   /* shift display right                    */

#define lcdwritestringxy(x,y,msg) {\
 lcd_gotoxy(x,y);\
 lcdwritestring(msg);\
}

#define lcdwriteintxy(x,y,val,fl) {\
 lcd_gotoxy(x,y);\
 lcdwriteint(val,fl);\
}

#define lcd_clr() lcd_cmd(0b00000001)

void lcd_config(void);
void lcd_init(void);
void lcd_en_tgl (void);
void lcd_cmd(unsigned char letter);
void lcd_char(unsigned char letter);
void lcd_nibble(unsigned char nib,unsigned char rs);
void lcd_clr_line(unsigned char line);
void lcd_string(char *, unsigned char line);
void lcd_string_position (char *senpoint,unsigned char line,unsigned char position,unsigned char l_r);
extern void lcd_gotoxy(unsigned char x, unsigned char y);
void lcdwritestring (const char *);
void lcdwriteint (unsigned int, unsigned int field_length);
void lcdwritefloat(float val, unsigned char decimal_length);

#endif
