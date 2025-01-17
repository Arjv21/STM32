/*
 * ext_flash_w25q64.c
 *
 *  Created on: Apr 19, 2024
 *      Author: CEPL
 */

#include "ext_flash_w25q64.h"

uint8_t flashInstruction[260] = {0};

void clearBuffer(uint8_t *pBuff, uint16_t buffLen){
	for(uint16_t i = 0;i < buffLen; i++)
		pBuff[i] = 0x00;
}
void softDelay(uint32_t count){
	for(;count>0;count--);
}
void flashDataCopy(uint8_t *pDest, uint8_t *pSrc, uint16_t len){
	for(uint16_t i=0; i<len; i++)
		pDest[i] = pSrc[i];
}
void extFlashHardReset(void){
	  HAL_GPIO_WritePin(F_WP_GPIO_Port, F_WP_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(F_RST_GPIO_Port, F_RST_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(F_nCS_GPIO_Port, F_nCS_Pin, GPIO_PIN_SET);
	  softDelay(100);
	  HAL_GPIO_WritePin(F_RST_GPIO_Port, F_RST_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(F_nCS_GPIO_Port, F_nCS_Pin, GPIO_PIN_RESET);
	  softDelay(100);
	  HAL_GPIO_WritePin(F_RST_GPIO_Port, F_RST_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(F_nCS_GPIO_Port, F_nCS_Pin, GPIO_PIN_SET);
	  softDelay(200);
}
void extFlashGetDeviceId(uint8_t *pDeviceIdRxData){
	uint8_t manufacturerId[] = {0x90,0x00,0x00,0x00};
	HAL_SPI_Init(&hspi3);				//
	HAL_GPIO_WritePin(F_nCS_GPIO_Port, F_nCS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi3, (uint8_t *)&manufacturerId, 4, 1);
	HAL_SPI_Receive(&hspi3, pDeviceIdRxData, 2, 100);
	HAL_GPIO_WritePin(F_nCS_GPIO_Port, F_nCS_Pin, GPIO_PIN_SET);
	HAL_SPI_Init(&hspi3);
	softDelay(500);
}
uint8_t extFlashGetStatusReg1(){
	flashInstruction[0] = 0x05;
	uint8_t status = 0;
	HAL_SPI_Init(&hspi3);
	HAL_GPIO_WritePin(F_nCS_GPIO_Port, F_nCS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi3, (uint8_t *)flashInstruction, 1, 100);
	HAL_SPI_Receive(&hspi3, &status, 1, 1);
	HAL_GPIO_WritePin(F_nCS_GPIO_Port, F_nCS_Pin, GPIO_PIN_SET);
	HAL_SPI_DeInit(&hspi3);
	return status;
}
uint8_t extFlashGetStatusReg2(){
	flashInstruction[0] = 0x35;
	uint8_t status = 0;
	HAL_SPI_Init(&hspi3);
	HAL_GPIO_WritePin(F_nCS_GPIO_Port, F_nCS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi3, (uint8_t *)flashInstruction, 1, 100);
	HAL_SPI_Receive(&hspi3, &status, 1, 1);
	HAL_GPIO_WritePin(F_nCS_GPIO_Port, F_nCS_Pin, GPIO_PIN_SET);
	HAL_SPI_DeInit(&hspi3);
	return status;
}
uint8_t extFlashGetStatusReg3(){
	flashInstruction[0] = 0x15;
	uint8_t status = 0;
	HAL_SPI_Init(&hspi3);
	HAL_GPIO_WritePin(F_nCS_GPIO_Port, F_nCS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi3, (uint8_t *)flashInstruction, 1, 100);
	HAL_SPI_Receive(&hspi3, &status, 1, 1);
	HAL_GPIO_WritePin(F_nCS_GPIO_Port, F_nCS_Pin, GPIO_PIN_SET);
	HAL_SPI_DeInit(&hspi3);
	return status;
}
void extFlashSendCommand(uint8_t flashCmd){
	HAL_SPI_Init(&hspi3);
	HAL_GPIO_WritePin(F_nCS_GPIO_Port, F_nCS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi3, (uint8_t *)&flashCmd, 1, 1);
	HAL_GPIO_WritePin(F_nCS_GPIO_Port, F_nCS_Pin, GPIO_PIN_SET);
	HAL_SPI_DeInit(&hspi3);
}
void extFlashPageRead(uint32_t address, uint8_t* pData, uint8_t dataLength){
	flashInstruction[0] = READ_DATA;
	for(uint8_t i = 3; i>0; i--){
		flashInstruction[i] = (uint8_t)address & 0xFF;
		address = address>>8;
	}
	HAL_SPI_Init(&hspi3);
	HAL_GPIO_WritePin(F_nCS_GPIO_Port, F_nCS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi3, (uint8_t *)&flashInstruction, 4, 1);
	HAL_SPI_Receive(&hspi3, pData, dataLength, 10);
	HAL_GPIO_WritePin(F_nCS_GPIO_Port, F_nCS_Pin, GPIO_PIN_SET);
	HAL_SPI_DeInit(&hspi3);
	HAL_Delay(1);
}
void extFlashPageWrite(uint32_t address, uint8_t* pData, uint16_t dataLength){
	uint8_t status;
	extFlashSendCommand(WRITE_ENABLE);
	clearBuffer(flashInstruction, 260);
	flashInstruction[0] = PAGE_PROGRAM;
	for(uint8_t i = 3; i>0; i--){
		flashInstruction[i] = (uint8_t)address & 0xFF;
		address = address>>8;
	}
	flashDataCopy(flashInstruction+4, pData, dataLength);
	HAL_SPI_Init(&hspi3);
	HAL_GPIO_WritePin(F_nCS_GPIO_Port, F_nCS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi3, flashInstruction, dataLength+4, 10);
	HAL_GPIO_WritePin(F_nCS_GPIO_Port, F_nCS_Pin, GPIO_PIN_SET);
	HAL_SPI_DeInit(&hspi3);
	do{
		HAL_Delay(1);
		status = extFlashGetStatusReg1();
	}while((status & 0x03) != 0x00);
}
void extFlashErase(uint8_t cmd, uint32_t eraseAddress){
	extFlashSendCommand(WRITE_ENABLE);
	clearBuffer(flashInstruction, 260);
	flashInstruction[0]	= cmd;
	for(uint8_t i = 3; i>0; i--){
		flashInstruction[i] = (uint8_t)eraseAddress & 0xFF;
		eraseAddress = eraseAddress>>8;
	}
	HAL_SPI_Init(&hspi3);
	HAL_GPIO_WritePin(F_nCS_GPIO_Port, F_nCS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi3, (uint8_t *)&flashInstruction, 4, 1);
	HAL_GPIO_WritePin(F_nCS_GPIO_Port, F_nCS_Pin, GPIO_PIN_SET);
	HAL_SPI_DeInit(&hspi3);

	uint8_t status;
	do{
		HAL_Delay(1);
		status = extFlashGetStatusReg1();
	}while((status & 0x03) != 0x00);
}
void extFlashEraseFotaSector(void){
	extFlashSendCommand(WRITE_ENABLE);
	extFlashSendCommand(GLOBAL_UNLOCK);
	extFlashErase(BLOCK_ERASE_64KB, FLASH_PAGE0_BASE_ADDRESS);
	extFlashErase(BLOCK_ERASE_64KB, FLASH_PAGE1_BASE_ADDRESS);
}

void extFlashWriteHeader(uint8_t *pData){
	extFlashErase(SECTOR_ERASE_4KB, FLASH_FLAG_SAVE_ADDRESS);

	uint8_t header[3] = {0};
	header[0] = 0xBB;
	header[1] = *pData;
	header[2] = *(pData+1);
	extFlashPageWrite(FLASH_FLAG_SAVE_ADDRESS, header, 3);

}


void resetFotaFlag(void){
	extFlashErase(SECTOR_ERASE_4KB, FLASH_FLAG_SAVE_ADDRESS);
	uint8_t header[3] = {0};
	header[0] = 0xAA;
	extFlashPageWrite(FLASH_FLAG_SAVE_ADDRESS, header, 3);
}
