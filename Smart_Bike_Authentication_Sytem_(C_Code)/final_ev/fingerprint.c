/*
 * fingerprint.c
 *
 * Created: 6/21/2024 7:46:11 PM
 *  Author: DELL
 */ 

#include "fingerprint.h"
#include <util/delay.h>
#include <avr/interrupt.h>

// UART setup for fingerprint sensor communication
void uart_init(uint32_t baudrate) {
	uint16_t ubrr_value = (F_CPU / (16UL * baudrate)) - 1;
	UBRR0H = (ubrr_value >> 8);
	UBRR0L = ubrr_value;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0); // Enable RX and TX
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit data
}

void uart_send_byte(uint8_t data) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

uint8_t uart_receive_byte(void) {
	while (!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

void uart_send_packet(const uint8_t *packet, uint16_t len) {
	for (uint16_t i = 0; i < len; i++) {
		uart_send_byte(packet[i]);
	}
}

void uart_receive_packet(uint8_t *packet, uint16_t len) {
	for (uint16_t i = 0; i < len; i++) {
		packet[i] = uart_receive_byte();
	}
}

// Global variable to store the ID of the found fingerprint
uint16_t fingerID;

// Helper function to send a command to the fingerprint sensor and receive a response
uint8_t fingerprint_send_command(const uint8_t *cmd, uint16_t cmd_len, uint8_t *response, uint16_t response_len) {
	uart_send_packet(cmd, cmd_len);
	uart_receive_packet(response, response_len);
	return response[9]; // Return the confirmation code
}

// Function to initialize the fingerprint sensor
void fingerprint_init(void) {
	uart_init(FINGERPRINT_BAUDRATE);
}

// Function to get an image from the fingerprint sensor
uint8_t fingerprint_getImage(void) {
	const uint8_t cmd[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x03, 0x01, 0x00, 0x05};
	uint8_t response[12];
	return fingerprint_send_command(cmd, sizeof(cmd), response, sizeof(response));
}

// Function to convert an image to a template
uint8_t fingerprint_image2Tz(uint8_t slot) {
	const uint8_t cmd[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x04, 0x02, slot, 0x00, 0x08};
	uint8_t response[12];
	return fingerprint_send_command(cmd, sizeof(cmd), response, sizeof(response));
}

// Function to create a model from two templates
uint8_t fingerprint_createModel(void) {
	const uint8_t cmd[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x03, 0x05, 0x00, 0x09};
	uint8_t response[12];
	return fingerprint_send_command(cmd, sizeof(cmd), response, sizeof(response));
}

// Function to store a model in flash memory
uint8_t fingerprint_storeModel(uint16_t id) {
	uint8_t cmd[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x06, 0x06, 0x01, id >> 8, id & 0xFF, 0x00, 0x00, 0x00};
	cmd[14] = 0x0D + id;
	uint8_t response[12];
	return fingerprint_send_command(cmd, sizeof(cmd), response, sizeof(response));
}

// Function to delete a model from flash memory
uint8_t fingerprint_deleteModel(uint16_t id) {
	uint8_t cmd[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x07, 0x0C, id >> 8, id & 0xFF, 0x00, 0x01, 0x00, 0x00, 0x00};
	cmd[15] = 0x15 + id;
	uint8_t response[12];
	return fingerprint_send_command(cmd, sizeof(cmd), response, sizeof(response));
}

// Function to perform a fingerprint search
uint8_t fingerprint_fingerFastSearch(void) {
	const uint8_t cmd[] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x08, 0x0B, 0x01, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x0F};
	uint8_t response[16];
	uint8_t result = fingerprint_send_command(cmd, sizeof(cmd), response, sizeof(response));
	if (result == FINGERPRINT_OK) {
		fingerID = (response[10] << 8) | response[11];
	}
	return result;
}
