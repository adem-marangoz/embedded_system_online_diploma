/**
 * @file EEPROM_25xx256.h
 * @author adem alnajjar (adem.alnajjar95@gmail.com)
 * @brief 25LC256 EEPROM Driver
 * @version 0.1
 * @date 2022-10-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef EEPROM_25XX256_H_
#define EEPROM_25XX256_H_

// ----------------------------- Include Files ---------------------------------
#include "Stm32f10x_SPI.h"
#include "Stm32f10xx_Common.h"
#include "Stm32f10xx_GPIO.h"
// =============================================================================


// ----------------------------- Generaic Objects ------------------------------

#define Read_CMD                0x03    // Read data from memory array beginning at selected address 
#define Write_CMD               0x02    // Write data to memory array beginning at selected address
#define Dis_Write_CMD           0x04    // Reset the write enable latch (disable write operations)
#define En_Write_CMD            0x06    // Set the write enable latch (enable write operations)
#define Read_St_CMD             0x05    // Read Status register
#define Write_St_CMD            0x01    // Write Status register
#define Address_Max             0xFFFF  // Maximum EEPROM address

// =============================================================================
typedef struct 
{
    St_SPI_Typedef *SPI_Instance;
    St_GPIO_Typedef *GPIOx;
    uint16_t NSS_Pin;
}St_EEPROM_25xx256_Typedef;


// ----------------------------- Global Functions ------------------------------
void Init_EEPROM_25x(St_EEPROM_25xx256_Typedef *EEPROM_inst);
void Enable_Write_EEPROM_25xx(St_EEPROM_25xx256_Typedef *EEPROM_inst);
uint8_t Write_Byte_EEPROM_25xx(St_EEPROM_25xx256_Typedef *EEPROM_inst, uint16_t Add, uint8_t data);
uint8_t Write_Bytes_EEPROM_25xx(St_EEPROM_25xx256_Typedef *EEPROM_inst, uint16_t Add, uint8_t len, uint8_t *data);
uint8_t Read_Byte_EEPROM_25xx(St_EEPROM_25xx256_Typedef *EEPROM_inst, uint16_t Add);

// =============================================================================

#endif