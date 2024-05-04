/*
 * int_flash.h
 *
 *  Created on: 20-Apr-2024
 *      Author: CEPL
 */

#ifndef INC_INT_FLASH_H_
#define INC_INT_FLASH_H_

#include "main.h"

#define USER_APP_BASE_ADDRESS	0X8005000


//uint32_t Flash_Write_Data (uint32_t StartPageAddress, uint32_t *Data, uint16_t numberofwords);
uint32_t intFlashWrite(uint32_t flashWriteAddress, uint32_t *pData, uint16_t numberOfWordsTowrite);
uint32_t intFlashWrite1(uint32_t *pData, uint16_t numberOfWordsTowrite);

#endif /* INC_INT_FLASH_H_ */
