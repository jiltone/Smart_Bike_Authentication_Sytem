/*
 * lcd.c
 *
 * Created: 6/21/2024 7:38:38 PM
 *  Author: DELL
 */ 

#include "lcd.h"
#include <util/delay.h>

// Define the LCD port and pin connections
#define LCD_PORT PORTB
#define LCD_DDR DDRB
#define LCD_RS PB0
#define LCD_RW PB1
#define LCD_E PB2

#define LCD_D4 PB4
#define LCD_D5 PB5
#define LCD_D6 PB6
#define LCD_D7 PB7

static void lcd_enable_pulse(void) {
	LCD_PORT |= (1 << LCD_E);
	_delay_us(1);
	LCD_PORT &= ~(1 << LCD_E);
	_delay_us(100);
}

static void lcd_send_nibble(uint8_t nibble) {
	if (nibble & (1 << 0)) LCD_PORT |= (1 << LCD_D4); else LCD_PORT &= ~(1 << LCD_D4);
	if (nibble & (1 << 1)) LCD_PORT |= (1 << LCD_D5); else LCD_PORT &= ~(1 << LCD_D5);
	if (nibble & (1 << 2)) LCD_PORT |= (1 << LCD_D6); else LCD_PORT &= ~(1 << LCD_D6);
	if (nibble & (1 << 3)) LCD_PORT |= (1 << LCD_D7); else LCD_PORT &= ~(1 << LCD_D7);

	lcd_enable_pulse();
}

static void lcd_send_byte(uint8_t byte, uint8_t is_data) {
	if (is_data) {
		LCD_PORT |= (1 << LCD_RS);
		} else {
		LCD_PORT &= ~(1 << LCD_RS);
	}

	lcd_send_nibble(byte >> 4);
	lcd_send_nibble(byte & 0x0F);
}

void lcd_init(uint8_t dispAttr) {
	LCD_DDR |= (1 << LCD_RS) | (1 << LCD_RW) | (1 << LCD_E) | (1 << LCD_D4) | (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7);
	LCD_PORT &= ~((1 << LCD_RS) | (1 << LCD_RW) | (1 << LCD_E));

	_delay_ms(40); // Wait for LCD to power up

	lcd_send_nibble(0x03);
	_delay_ms(5);
	lcd_send_nibble(0x03);
	_delay_us(200);
	lcd_send_nibble(0x03);
	_delay_us(200);
	lcd_send_nibble(0x02); // Set to 4-bit mode
	_delay_us(200);

	lcd_command(LCD_FUNCTION_SET);
	lcd_command(LCD_DISPLAY_OFF);
	lcd_command(LCD_CLEAR_DISPLAY);
	_delay_ms(2);
	lcd_command(LCD_ENTRY_MODE_SET);
	lcd_command(dispAttr);
}

void lcd_command(uint8_t cmd) {
	lcd_send_byte(cmd, 0);
}

void lcd_data(uint8_t data) {
	lcd_send_byte(data, 1);
}

void lcd_clrscr() {
	lcd_command(LCD_CLEAR_DISPLAY);
	_delay_ms(2);
}

void lcd_home() {
	lcd_command(LCD_RETURN_HOME);
	_delay_ms(2);
}

void lcd_putc(char c) {
	lcd_data(c);
}

void lcd_puts(const char *s) {
	while (*s) {
		lcd_putc(*s++);
	}
}

void lcd_gotoxy(uint8_t x, uint8_t y) {
	uint8_t address = x + (y ? 0x40 : 0x00);
	lcd_command(LCD_SET_DDRAM_ADDR | address);
}
