/*
 * packetHandler.c
 *
 *  Created on: Apr 27, 2024
 *      Author: CEPL
 */

#include "main.h"


#include "packetHandler.h"
#include "ext_flash_w25q64.h"


uint8_t isFlashErased 		= 0;
uint8_t isFirmwareVerified	= 0;
uint32_t firmwareSize		= 0;


void processPacket(unit8_t *pFrame, uint16_t pktLen){
	/*check if Frame Start and Frame End is Properly received*/
	if((*pFrame == 0xAA) && (*(pFrame+pktLen-1) == 0xBB)){
//		if(checkCrc()==CRC_OK);
		switch(pFrame[FOTA_TYPE]){
		case TYPE_DATA:

			break;
		case TYPE_CMD:/* command frame received. check the payload for the command*/
			if(pFrame[FOTA_PAYLOAD] == FOTA_INIT){
				extFlashEraseFotaSector();
				flashErased = 1;
			}
			else if(pFrame[FOTA_PAYLOAD] == FOTA_END){
				verifyFirmware();
				extFlashWriteHeader();
			}
			break;
		case TYPE_HEADER: /* Header Frame Received. Extract Firmware Length*/
				getFirmwareSize(pFrame);
			break;
		default:
			break;
		}
	}
}

void verifyFirmware(void){

}

void getFirmwareSize(unit8_t *pFrame){
	firmwareSize = 0;
	for(uint8_t i = 0; i<4; i++){
		firmwareSize = ((firmwareSize << 8)|(pFrame[FOTA_PAYLOAD + i]));
	}
}



