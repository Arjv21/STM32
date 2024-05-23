/*
 * packetHandler.h
 *
 *  Created on: Apr 27, 2024
 *      Author: CEPL
 */

#ifndef INC_PACKETHANDLER_H_
#define INC_PACKETHANDLER_H_




#define FOTA_INIT		0x11
#define FOTA_END		0x22
#define FOTA_UPDATE		0x33
#define FOTA_ACK		0x00
#define FOTA_NACK		0xFF

#define FOTA_TYPE		1
#define FOTA_LEN		2
#define FOTA_PAYLOAD	3

#define TYPE_DATA		0X00
#define TYPE_CMD 		0X01
#define TYPE_HEADER		0X02
#define TYPE_RESP		0X03


//void getFirmwareSize(unit8_t *pFrame);
//void processPacket(unit8_t *pFrame, uint16_t pktLen);
void processPacketTest(uint8_t *pFrame, uint16_t pktLen);
void savePacketToExtFlash(uint8_t *pData, uint16_t dataLen);
uint8_t compareBuffer(uint8_t *pBuffer1, uint8_t *pBuffer2, uint16_t dataLen);

#endif /* INC_PACKETHANDLER_H_ */
