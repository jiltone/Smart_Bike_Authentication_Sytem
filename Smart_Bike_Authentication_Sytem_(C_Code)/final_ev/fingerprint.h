/*
 * fingerprint.h
 *
 * Created: 6/21/2024 7:45:37 PM
 *  Author: DELL
 */ 

#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#include <avr/io.h>

// Define the status codes
#define FINGERPRINT_OK 0x00
#define FINGERPRINT_PACKETRECIEVEERR 0x01
#define FINGERPRINT_NOFINGER 0x02
#define FINGERPRINT_IMAGEFAIL 0x03
#define FINGERPRINT_IMAGEMESS 0x06
#define FINGERPRINT_FEATUREFAIL 0x07
#define FINGERPRINT_INVALIDIMAGE 0x15
#define FINGERPRINT_ENROLLMISMATCH 0x0A
#define FINGERPRINT_BADLOCATION 0x0B
#define FINGERPRINT_FLASHERR 0x18

// Define the UART settings for the fingerprint sensor
#define FINGERPRINT_BAUDRATE 57600
#define FINGERPRINT_UART_PORT PORTD
#define FINGERPRINT_UART_DDR DDRD
#define FINGERPRINT_UART_RX PD0
#define FINGERPRINT_UART_TX PD1

// Fingerprint functions
void fingerprint_init(void);
uint8_t fingerprint_getImage(void);
uint8_t fingerprint_image2Tz(uint8_t slot);
uint8_t fingerprint_createModel(void);
uint8_t fingerprint_storeModel(uint16_t id);
uint8_t fingerprint_deleteModel(uint16_t id);
uint8_t fingerprint_fingerFastSearch(void);

extern uint16_t fingerID;  // This variable will hold the ID of the found fingerprint

#endif // FINGERPRINT_H
