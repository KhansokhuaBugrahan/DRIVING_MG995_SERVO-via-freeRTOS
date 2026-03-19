/*
 * EEPROM.c
 *
 *  Created on: Feb 4, 2026
 *      Author: khans
 *       Model: AT24C32        0xFFF address length
 *       					   2^12 = 4096 x 8 = 32768 bit
 *       					   128 pages
 *       					   each page 32 address 32 byte = 256 bit
 *
 */

#include "EEPROM.h"
#include "timer.h"

static void eepromWaitReady(void);
static void eepromWaitReady(void)
{
    while(1)
    {
        i2cStart();

        I2C1->I2C_DR = EEPROM_DEVICE_ADDRESS;

        while(!(I2C1->I2C_SR1 & ((1u << 1) | (1u << 10))));

        if(I2C1->I2C_SR1 & (1u << 1))
        {
            // ACK received → ready
            (void)I2C1->I2C_SR1;
            (void)I2C1->I2C_SR2;

            i2cStop();
            return;
        }

        // NACK → busy
        I2C1->I2C_SR1 &= ~(1u << 10);  // clear AF
        i2cStop();
    }
}
uint8_t eepromReadfromAddress(uint16_t address)
{
	  uint8_t upperAddressByte =  address >> 8;
	  uint8_t lowerAddressByte =  address & 0xFF;
	  uint8_t addressInfo [2] = {upperAddressByte, lowerAddressByte};

	  i2cStart();
	  i2cAddress(EEPROM_DEVICE_ADDRESS);
	  i2cWriteMulti(addressInfo, 2);

	  i2cStart(); // repeated start
	  i2cAddress(EEPROM_DEVICE_ADDRESS | 0x01);


	  I2C1->I2C_CR1 &= ~(1u << 10); // clear ACK bit, tell next byte is last one
	  i2cStop();


	  while(!(I2C1->I2C_SR1 & ( 1u << 6))); // receiver buffer full?
	  uint8_t data = I2C1->I2C_DR;

	  return data;

}
void eepromWriteIntoAddress(uint16_t address, uint8_t data)
{
	  uint8_t upperAddressByte =  address >> 8;
	  uint8_t lowerAddressByte =  address & 0xFF;
	  uint8_t addressDataInfo [3] = {upperAddressByte, lowerAddressByte, data};

	  i2cStart();
	  i2cAddress(EEPROM_DEVICE_ADDRESS);
	  i2cWriteMulti(addressDataInfo, 3);
	  i2cStop();

	  eepromWaitReady();
}
void saveServoPos(void)
{
	  servoPos[0] = (TIMER3->TIMx_CCR1 - 500) / 8;
	  servoPos[1] = (TIMER3->TIMx_CCR3 - 500) / 8;

	  uint8_t addressDataInfo [4] = {0x00, 0x00, servoPos[0], servoPos[1]};

	  i2cStart();
	  i2cAddress(EEPROM_DEVICE_ADDRESS);
	  i2cWriteMulti(addressDataInfo, 4);

	  i2cStop();

	  eepromWaitReady();
}
