/*
 * lcd.h
 *
 * Created: 6/21/2024 7:38:05 PM
 *  Author: DELL
 */ 

#ifndef LCD_H
#define LCD_H

#include <avr/io.h>

// LCD commands
#define LCD_CLEAR_DISPLAY 0x01
#define LCD_RETURN_HOME 0x02
#define LCD_ENTRY_MODE_SET 0x06
#define LCD_DISPLAY_ON 0x0C
#define LCD_DISPLAY_OFF 0x08
#define LCD_CURSOR_ON 0x0E
#define LCD_CURSOR_OFF 0x0C
#define LCD_BLINK_ON 0x0D
#define LCD_BLINK_OFF 0x0C
#define LCD_FUNCTION_SET 0x28
#define LCD_SET_DDRAM_ADDR 0x80

// LCD function prototypes
void lcd_init(uint8_t dispAttr);
void lcd_command(uint8_t cmd);
void lcd_data(uint8_t data);
void lcd_clrscr();
void lcd_home();
void lcd_putc(char c);
void lcd_puts(const char *s);
void lcd_gotoxy(uint8_t x, uint8_t y);

#endif // LCD_H
