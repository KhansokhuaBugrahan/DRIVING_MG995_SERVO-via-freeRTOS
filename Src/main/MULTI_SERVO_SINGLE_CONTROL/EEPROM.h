/*
 * EEPROM.h     model AT24C32
 *
 *  Created on: Feb 4, 2026
 *      Author: khans
 *
 *      32 Kbit
 *      0x0FFF address length
 */
#include "i2c.h"

#define 		EEPROM_DEVICE_ADDRESS     (0xA0) // default
#define         EEPROM_BASE_ADDRESS       (0x0000)

#define         SERVO_ADDRESS_1           (0x000)
#define         SERVO_ADDRESS_2           (0x001)

volatile extern uint8_t servoPos[2];
typedef struct {

	volatile uint8_t servo1;
	volatile uint8_t servo2;

}Servo_Pos_Save_t;



uint8_t eepromReadfromAddress(uint16_t address);
void eepromWriteIntoAddress(uint16_t address, uint8_t data);
void saveServoPos(void);
