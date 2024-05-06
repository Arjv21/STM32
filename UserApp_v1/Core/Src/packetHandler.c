/*
 * packetHandler.c
 *
 *  Created on: Apr 27, 2024
 *      Author: CEPL
 */

#include "main.h"
#include "usart.h"

#include "packetHandler.h"
#include "ext_flash_w25q64.h"


uint8_t		readBack[256]		= {0};
uint8_t 	isFlashErased 		= 0;
uint8_t 	isFirmwareVerified	= 0;
uint32_t 	firmwareSize		= 0;

uint8_t		msg[]				= "Erase Done...\n";

void processPacketTest(uint8_t *pFrame, uint16_t pktLen){
	/*check if Frame Start and Frame End is Properly received*/
	if(*pFrame == 0x00)
		savePacketToExtFlash((pFrame+1),pktLen-1);// assuing FRAME= FRAME_TYPE+DATA_BYTES(1+N DATA_BYTES)
	else if(*pFrame == 0x01){
		extFlashEraseFotaSector();
		HAL_UART_Transmit(&huart2, msg, 14, 10);
	}
	else if(*pFrame == 0x02){
		extFlashWriteHeader(pFrame+1);
		HAL_Delay(10);
		NVIC_SystemReset();
	}
	else if(*pFrame == 0x03)
		resetFotaFlag();
}

void savePacketToExtFlash(uint8_t *pData, uint16_t dataLen){
	uint8_t responseString[2] = {0x05,0x00};

	static uint32_t extFlashAddress = FLASH_PAGE0_BASE_ADDRESS;
//	uint8_t compareFlag = 0;
	extFlashPageWrite(extFlashAddress, pData, dataLen);
//	clearBuffer(readBack,256);
//	extFlashPageRead(extFlashAddress, readBack, dataLen);
//	compareFlag = compareBuffer(readBack, pData, dataLen);
//	if(compareFlag == 1){
//		responseString[1] = 0x55;
//		extFlashAddress += dataLen;
//	}
//	else {
//		responseString[1] = 0xAA;
//	}
	extFlashAddress += dataLen;
	HAL_UART_Transmit(&huart2, responseString, 2, 5);
}
uint8_t compareBuffer(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t dataLen){
	for(uint16_t i=0; i<dataLen; i++){
		if(*pBuffer1 == *pBuffer2){
			pBuffer1++;
			pBuffer2++;
		}
		else{
			return 0;
		}
	}
	return 1;
}

