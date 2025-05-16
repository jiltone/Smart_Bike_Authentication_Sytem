/*
 * final_ev.c
 *
 * Created: 6/21/2024 7:28:38 PM
 * Author : DELL
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "fingerprint.h"

// Define pin constants
#define ENROLL_PIN PD0
#define DEL_PIN PD1
#define UP_PIN PD2
#define DOWN_PIN PD3
#define OPEN_LIGHT_PIN PB0
#define POWER_LIGHT_PIN PB1
#define BUZZER_PIN PB2

// Global variables
uint8_t id;

// Function prototypes
void setup();
void loop();
void checkKeys();
void enroll();
void delet();
uint8_t getFingerprintEnroll();
int getFingerprintIDez();
uint8_t deleteFingerprint(uint8_t id);

int main(void) {
	setup();
	while (1) {
		loop();
	}
}

void setup() {
	// Initialize pins
	DDRD &= ~((1 << ENROLL_PIN) | (1 << DEL_PIN) | (1 << UP_PIN) | (1 << DOWN_PIN));
	PORTD |= ((1 << ENROLL_PIN) | (1 << DEL_PIN) | (1 << UP_PIN) | (1 << DOWN_PIN)); // Enable pull-up resistors

	DDRB |= ((1 << OPEN_LIGHT_PIN) | (1 << POWER_LIGHT_PIN) | (1 << BUZZER_PIN));
	PORTB |= (1 << BUZZER_PIN); // Turn off buzzer

	// Initialize LCD
	lcd_init(LCD_DISP_ON);
	lcd_clrscr();
	lcd_puts("Security System");
	lcd_gotoxy(0, 1);
	lcd_puts("for MotorBike");
	_delay_ms(2000);

	lcd_clrscr();
	lcd_puts("Group 35");
	_delay_ms(2000);

	// Initialize UART for fingerprint sensor
	fingerprint_init();

	lcd_clrscr();
	lcd_puts("Finding Module");
	lcd_gotoxy(0, 1);
	_delay_ms(1000);

	// Check if fingerprint sensor is found
	if (fingerprint_verifyPassword()) {
		lcd_clrscr();
		lcd_puts("Found Module");
		_delay_ms(1000);
		} else {
		lcd_clrscr();
		lcd_puts("module not Found");
		lcd_gotoxy(0, 1);
		lcd_puts("Check Connections");
		while (1);
	}
}

void loop() {
	lcd_gotoxy(0, 0);
	lcd_puts("Press UP/Down ");
	lcd_gotoxy(0, 1);
	lcd_puts("to start System");

	PORTB |= (1 << POWER_LIGHT_PIN);

	if (!(PIND & (1 << UP_PIN)) || !(PIND & (1 << DOWN_PIN))) {
		for (int i = 0; i < 5; i++) {
			lcd_clrscr();
			lcd_puts("Place Finger");
			_delay_ms(2000);

			int result = getFingerprintIDez();
			if (result >= 0) {
				PORTB |= (1 << OPEN_LIGHT_PIN);
				PORTB &= ~(1 << BUZZER_PIN); // Turn off buzzer
				lcd_clrscr();
				lcd_puts("Allowed");
				lcd_gotoxy(0, 1);
				lcd_puts("Gate Opened");
				_delay_ms(5000);
				return;
			}
		}
	}

	checkKeys();
	_delay_ms(1000);
}

void checkKeys() {
	if (!(PIND & (1 << ENROLL_PIN))) {
		lcd_clrscr();
		lcd_puts("Please Wait");
		_delay_ms(1000);
		while (!(PIND & (1 << ENROLL_PIN)));
		enroll();
		} else if (!(PIND & (1 << DEL_PIN))) {
		lcd_clrscr();
		lcd_puts("Please Wait");
		_delay_ms(1000);
		delet();
	}
}

void enroll() {
	int count = 0;
	lcd_clrscr();
	lcd_puts("Enroll Finger");
	lcd_gotoxy(0, 1);
	lcd_puts("Location:");

	while (1) {
		lcd_gotoxy(9, 1);
		lcd_putc('0' + count);

		if (!(PIND & (1 << UP_PIN))) {
			count++;
			if (count > 25) count = 0;
			_delay_ms(500);
			} else if (!(PIND & (1 << DOWN_PIN))) {
			count--;
			if (count < 0) count = 25;
			_delay_ms(500);
			} else if (!(PIND & (1 << DEL_PIN))) {
			id = count;
			getFingerprintEnroll();
			return;
			} else if (!(PIND & (1 << ENROLL_PIN))) {
			return;
		}
	}
}

void delet() {
	int count = 0;
	lcd_clrscr();
	lcd_puts("Delete Finger");
	lcd_gotoxy(0, 1);
	lcd_puts("Location:");

	while (1) {
		lcd_gotoxy(9, 1);
		lcd_putc('0' + count);

		if (!(PIND & (1 << UP_PIN))) {
			count++;
			if (count > 25) count = 0;
			_delay_ms(500);
			} else if (!(PIND & (1 << DOWN_PIN))) {
			count--;
			if (count < 0) count = 25;
			_delay_ms(500);
			} else if (!(PIND & (1 << DEL_PIN))) {
			id = count;
			deleteFingerprint(id);
			return;
			} else if (!(PIND & (1 << ENROLL_PIN))) {
			return;
		}
	}
}

uint8_t getFingerprintEnroll() {
	int p = -1;
	lcd_clrscr();
	lcd_puts("finger ID:");
	lcd_putc('0' + id);
	lcd_gotoxy(0, 1);
	lcd_puts("Place Finger");
	_delay_ms(2000);

	while (p != FINGERPRINT_OK) {
		p = fingerprint_getImage();
		switch (p) {
			case FINGERPRINT_OK:
			lcd_clrscr();
			lcd_puts("Image taken");
			break;
			case FINGERPRINT_NOFINGER:
			lcd_clrscr();
			lcd_puts("No Finger");
			break;
			case FINGERPRINT_PACKETRECIEVEERR:
			lcd_clrscr();
			lcd_puts("Comm Error");
			break;
			case FINGERPRINT_IMAGEFAIL:
			lcd_clrscr();
			lcd_puts("Imaging Error");
			break;
			default:
			lcd_clrscr();
			lcd_puts("Unknown Error");
			break;
		}
	}

	p = fingerprint_image2Tz(1);
	switch (p) {
		case FINGERPRINT_OK:
		lcd_clrscr();
		lcd_puts("Image converted");
		break;
		case FINGERPRINT_IMAGEMESS:
		lcd_clrscr();
		lcd_puts("Image too messy");
		return p;
		case FINGERPRINT_PACKETRECIEVEERR:
		lcd_clrscr();
		lcd_puts("Comm Error");
		return p;
		case FINGERPRINT_FEATUREFAIL:
		lcd_clrscr();
		lcd_puts("Feature Not Found");
		return p;
		case FINGERPRINT_INVALIDIMAGE:
		lcd_clrscr();
		lcd_puts("Feature Not Found");
		return p;
		default:
		lcd_clrscr();
		lcd_puts("Unknown Error");
		return p;
	}

	lcd_clrscr();
	lcd_puts("Remove Finger");
	_delay_ms(2000);
	p = 0;
	while (p != FINGERPRINT_NOFINGER) {
		p = fingerprint_getImage();
	}

	p = -1;
	lcd_clrscr();
	lcd_puts("Place Finger");
	lcd_gotoxy(0, 1);
	lcd_puts("Again");

	while (p != FINGERPRINT_OK) {
		p = fingerprint_getImage();
		switch (p) {
			case FINGERPRINT_OK:
			break;
			case FINGERPRINT_NOFINGER:
			break;
			case FINGERPRINT_PACKETRECIEVEERR:
			break;
			case FINGERPRINT_IMAGEFAIL:
			break;
			default:
			return p;
		}
	}

	p = fingerprint_image2Tz(2);
	switch (p) {
		case FINGERPRINT_OK:
		break;
		case FINGERPRINT_IMAGEMESS:
		return p;
		case FINGERPRINT_PACKETRECIEVEERR:
		return p;
		case FINGERPRINT_FEATUREFAIL:
		return p;
		case FINGERPRINT_INVALIDIMAGE:
		return p;
		default:
		return p;
	}

	p = fingerprint_createModel();
	if (p == FINGERPRINT_OK) {
		lcd_clrscr();
		lcd_puts("Prints matched!");
		} else {
		switch (p) {
			case FINGERPRINT_PACKETRECIEVEERR:
			return p;
			case FINGERPRINT_ENROLLMISMATCH:
			return p;
			default:
			return p;
		}
	}

	p = fingerprint_storeModel(id);
	if (p == FINGERPRINT_OK) {
		lcd_clrscr();
		lcd_puts("Stored!");
		_delay_ms(2000);
		} else {
		switch (p) {
			case FINGERPRINT_PACKETRECIEVEERR:
			return p;
			case FINGERPRINT_BADLOCATION:
			return p;
			case FINGERPRINT_FLASHERR:
			return p;
			default:
			return p;
		}
	}
	return p;
}

int getFingerprintIDez() {
	uint8_t p = fingerprint_getImage();
	if (p != FINGERPRINT_OK) return -1;

	p = fingerprint_image2Tz(1);
	if (p != FINGERPRINT_OK) return -1;

	p = fingerprint_fingerFastSearch();
	if (p != FINGERPRINT_OK) {
		lcd_clrscr();
		lcd_puts("Finger Not Found");
		lcd_gotoxy(0, 1);
		lcd_puts("Try Later");
		_delay_ms(2000);
		return -1;
	}

	return fingerprint_fingerID();
}

uint8_t deleteFingerprint(uint8_t id) {
	uint8_t p = fingerprint_deleteModel(id);
	if (p == FINGERPRINT_OK) {
		lcd_clrscr();
		lcd_puts("Finger Deleted");
		lcd_gotoxy(0, 1);
		lcd_puts("Successfully");
		_delay_ms(1000);
		} else {
		lcd_clrscr();
		lcd_puts("Something Wrong");
		lcd_gotoxy(0, 1);
		lcd_puts("Try Again Later");
		_delay_ms(2000);
		return p;
	}
	return p;
}


