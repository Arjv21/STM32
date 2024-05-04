/*
 * int_flash.c
 *
 *  Created on: 20-Apr-2024
 *      Author: CEPL
 */

#include "int_flash.h"


uint32_t intFlashWrite1(uint32_t *pData, uint16_t numberOfWordsTowrite){
	static FLASH_EraseInitTypeDef EraseInitStruct;
	static uint8_t erasePageCount = 20;
	uint32_t PAGEError;
	uint16_t sofar=0;
	uint32_t flashBaseAddress = (USER_APP_BASE_ADDRESS + (erasePageCount*0x800));
	  /* Unlock the Flash to enable the flash control register access *************/
	   HAL_FLASH_Unlock();

	   /* Erase the user Flash area*/
	   /* Erase and write 2K of data at once
	    * the interface expects pData to an array of maximum 2K of data
	    * since page size = 2K
	    * We will erase and program an entire page in a go!!
	    */
	   /* Fill EraseInit structure*/
	   EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	   EraseInitStruct.PageAddress = flashBaseAddress;
	   EraseInitStruct.NbPages     = 1;
	   if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK){
		 /*Error occurred while page erase.*/
		  return HAL_FLASH_GetError ();
	   }
	   /* Program the user Flash area word by word*/

	   while (sofar<numberOfWordsTowrite){
		   if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashBaseAddress, pData[sofar]) == HAL_OK){
			   flashBaseAddress += 4;  // use StartPageAddress += 2 for half word and 8 for double word
			   sofar++;
		   }
		   else{
			   /* Error occurred while writing data in Flash memory*/
			   return HAL_FLASH_GetError ();
		   }
	   }
	   /* Lock the Flash to disable the flash control register access
	    * (recommended to protect the FLASH memory against possible unwanted operation)
	    */
	   HAL_FLASH_Lock();
	   erasePageCount++;

	   return 0;
}



//uint32_t Flash_Write_Data (uint32_t StartPageAddress, uint32_t *Data, uint16_t numberofwords)
//{
//
//	static FLASH_EraseInitTypeDef EraseInitStruct;
//	uint32_t PAGEError;
//	int sofar=0;
//
//	  /* Unlock the Flash to enable the flash control register access *************/
//	   HAL_FLASH_Unlock();
//
//	   /* Erase the user Flash area*/
//
////	  uint32_t StartPage = GetPage(StartPageAddress);
////	  uint32_t EndPageAdress = StartPageAddress + numberofwords*4;
////	  uint32_t EndPage = GetPage(EndPageAdress);
//
//	   /* Fill EraseInit structure*/
//	   EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
//	   EraseInitStruct.PageAddress = StartPageAddress;
//	   EraseInitStruct.NbPages     = (numberofwords/FLASH_PAGE_SIZE) +1;
//
//	   if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
//	   {
//	     /*Error occurred while page erase.*/
//		  return HAL_FLASH_GetError ();
//	   }
//
//	   /* Program the user Flash area word by word*/
//
//	   while (sofar<numberofwords)
//	   {
//	     if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, StartPageAddress, Data[sofar]) == HAL_OK)
//	     {
//	    	 StartPageAddress += 4;  // use StartPageAddress += 2 for half word and 8 for double word
//	    	 sofar++;
//	     }
//	     else
//	     {
//	       /* Error occurred while writing data in Flash memory*/
//	    	 return HAL_FLASH_GetError ();
//	     }
//	   }
//
//	   /* Lock the Flash to disable the flash control register access (recommended
//	      to protect the FLASH memory against possible unwanted operation) *********/
//	   HAL_FLASH_Lock();
//
//	   return 0;
//}







//uint32_t intFlashWrite(uint32_t flashWriteAddress, uint32_t *pData, uint16_t numberOfWordsTowrite){
//	static FLASH_EraseInitTypeDef EraseInitStruct;
//	static uint8_t writeCountInCurrentPage;
//	uint32_t PAGEError;
//	uint16_t sofar=0;
//
//	  /* Unlock the Flash to enable the flash control register access *************/
//	   HAL_FLASH_Unlock();
//
//	   /* Erase the user Flash area*/
//	   /* Erase the Page once per 16 writes
//	    * since page size = 2K and each write is performed for 256 bytes, (2048/256)=16
//	    */
//	   if(writeCountInCurrentPage == 0){
//	//	  uint32_t StartPage = GetPage(StartPageAddress);
//	//	  uint32_t EndPageAdress = StartPageAddress + numberofwords*4;
//	//	  uint32_t EndPage = GetPage(EndPageAdress);
//
//		   /* Fill EraseInit structure*/
//	   EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
//	   EraseInitStruct.PageAddress = flashWriteAddress;
//	   EraseInitStruct.NbPages     = (numberOfWordsTowrite/FLASH_PAGE_SIZE) +1;
//
//		   if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
//		   {
//			 /*Error occurred while page erase.*/
//			  return HAL_FLASH_GetError ();
//		   }
//	   }
//	   /* Program the user Flash area word by word*/
//
//	   while (sofar<numberOfWordsTowrite)
//	   {
//	     if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashWriteAddress, pData[sofar]) == HAL_OK)
//	     {
//	    	 flashWriteAddress += 4;  // use StartPageAddress += 2 for half word and 8 for double word
//	    	 sofar++;
//	     }
//	     else
//	     {
//	       /* Error occurred while writing data in Flash memory*/
//	    	 return HAL_FLASH_GetError ();
//	     }
//	   }
//	   /*
//	    * Reset write counter once entire page is written, so that next write attempt
//	    * begins with next page erase.
//	    */
//	   writeCountInCurrentPage++;
//	   if(writeCountInCurrentPage == 16){
//		   writeCountInCurrentPage = 0;
//	   }
//
//	   /* Lock the Flash to disable the flash control register access (recommended
//	      to protect the FLASH memory against possible unwanted operation) *********/
//	   HAL_FLASH_Lock();
//	   return 0;
//}












