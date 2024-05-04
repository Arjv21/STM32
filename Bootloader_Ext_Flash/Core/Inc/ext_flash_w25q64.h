/*
 * ext_flash_w25q64.h
 *
 *  Created on: Apr 19, 2024
 *      Author: CEPL
 */

#include "main.h"

#ifndef INC_EXT_FLASH_W25Q64_H_
#define INC_EXT_FLASH_W25Q64_H_


extern SPI_HandleTypeDef hspi3;


#define WRITE_DISABLE			0x04
#define MANUFACTURER_DEVICE_ID	0x90
#define READ_UNIQUE_ID			0x4B
#define READ_DATA				0x03
#define PAGE_PROGRAM			0x02
#define SECTOR_ERASE_4KB		0x20
#define BLOCK_ERASE_32KB		0x52
#define BLOCK_ERASE_64KB		0xD8
#define CHIP_ERASE				0x60
#define READ_STATUS_REGISTER1	0x05
#define READ_STATUS_REGISTER2	0x35
#define READ_STATUS_REGISTER3	0x15
#define GLOBAL_BLOCK_LOCK		0x7E
#define GLOBAL_BLOCK_UNLOCK		0x98
#define POWER_DOWN				0xB9
#define ENABLE_RESET			0x66
#define RESET_DEVICE			0x99
#define GLOBAL_UNLOCK			0X98

#define FLASH_PAGE0_BASE_ADDRESS	0X000000
#define FLASH_PAGE1_BASE_ADDRESS	0x010000

#define FLASH_FLAG_SAVE_ADDRESS		0X020000



void extFlashInit(void);
void clearBuffer(uint8_t *pBuff, uint16_t buffLen);
void softDelay(uint32_t count);
void SPI_FLASH_getDeviceId(uint8_t *pDeviceIdRxData);
uint8_t extFlashGetStatusReg1();
uint8_t extFlashGetStatusReg2();
uint8_t extFlashGetStatusReg3();
void extFlashSendCommand(uint8_t flashCmd);
void extFlashUnlockAll(void);
void extFlashEnableWrite(void);
void extFlashPageRead(uint32_t address, uint8_t* pData, uint8_t dataLength);
void extFlashPageWrite(uint32_t address, uint8_t* pData, uint16_t dataLength);
void extFlashErase(uint8_t cmd, uint32_t eraseAddress);

#endif /* INC_EXT_FLASH_W25Q64_H_ */
