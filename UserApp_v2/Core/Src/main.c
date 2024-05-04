/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ext_flash_w25q64.h"
#include "int_flash.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
unsigned char userData[] ={
		 0x48,0x45,0x4C,0xAA,0x5F,0x57,0x4F,0x52,0x4C,0x44,0xFF,0x84,0x0A
		,0x48,0x45,0x4C,0x4C,0x5F,0x57,0x4F,0x52,0x4C,0x44,0x00,0x83,0x0A
		,0x48,0x45,0x4C,0x4C,0x5F,0x57,0x4F,0x52,0x4C,0x44,0x00,0x83,0x0A
		,0x48,0x45,0x4C,0x4C,0x5F,0x57,0x4F,0x52,0x4C,0x44,0x01,0x84,0x0A
		,0x48,0x45,0x4C,0x4C,0x5F,0x57,0x4F,0x52,0x4C,0x44,0x00,0x83,0x0A
		,0x48,0x45,0x4C,0x4C,0x5F,0x57,0x4F,0x52,0x4C,0x44,0x00,0x82,0x0A
		,0x48,0x45,0x4C,0x4C,0x5F,0x57,0x4F,0x52,0x4C,0x44,0x01,0x83,0x0A
		,0x48,0x45,0x4C,0x4C,0x5F,0x57,0x4F,0x52,0x4C,0x44,0xFF,0x82,0x0A
		,0x48,0x45,0x4C,0x4C,0x5F,0x57,0x4F,0x52,0x4C,0x44,0x00,0x83,0x0A
		,0x48,0x45,0x4C,0x4C,0x5F,0x57,0x4F,0x52,0x4C,0x44,0x00,0x82,0x0A
		,0x48,0x45,0x4C,0x4C,0x5F,0x57,0x4F,0x52,0x4C,0x44,0x01,0x81,0x0A
		,0x48,0x45,0x4C,0x4C,0x5F,0x57,0x4F,0x52,0x4C,0x44,0x01,0x82,0x0A
		,0x48,0x45,0x4C,0x4C,0x5F,0x57,0x4F,0x52,0x4C,0x44,0x01,0x83,0x0A
		,0x48,0x45,0x4C,0x4C,0x5F,0x57,0x4F,0x52,0x4C,0x44,0x01,0x82,0x0A
		,0x48,0x45,0x4C,0x4C,0x5F,0x57,0x4F,0x52,0x4C,0x44,0x00,0x83,0x0A
		,0x48,0x45,0x4C,0x4C,0x5F,0x57,0x4F,0x52,0x4C,0x44,0x01,0x83,0x0A
		,0x48,0x45,0x4C,0x4C,0x5F,0x57,0x4F,0x52,0x4C,0x44,0xFF,0x84,0x0A
		,0x48,0x45,0x4C,0x4C,0x5F,0x57,0x4F,0x52,0x4C,0x44,0x00,0x83,0x0A
		,0x48,0x45,0x4C,0x4C,0x5F,0x57,0x4F,0x52,0x4C,0x44,0x00,0x83,0x0A
		,0x48,0x45,0x4C,0x4C,0x5F,0x57,0x4F,0x52,0x4C,
};
uint32_t 	flashWriteAddress	= 0;
uint32_t 	extFlashReadAddress	= 0;
uint32_t 	flashReadData[512]	= {0};
uint32_t	errorCode			= 0;
uint16_t 	receivedPacketSize 	= 0;
uint8_t  	DataFlagReadBack 	= 0;
uint8_t 	dummyFlagData 		= 0;
uint8_t 	uartRxData[270]		= {0};
uint8_t 	packetReceived 		= 0;
uint8_t		device_Id[2]		= {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI3_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UARTEx_ReceiveToIdle_IT(&huart2, uartRxData, 512);
  extFlashHardReset();
  HAL_Delay(1);
  extFlashSendCommand(ENABLE_RESET);
  extFlashSendCommand(RESET_DEVICE);
  extFlashSendCommand(GLOBAL_UNLOCK);
  extFlashGetDeviceId(device_Id);
//  extFlashErase(BLOCK_ERASE_64KB, FLASH_PAGE0_BASE_ADDRESS);
//  extFlashErase(BLOCK_ERASE_64KB, FLASH_PAGE1_BASE_ADDRESS);
//  extFlashErase(SECTOR_ERASE_4KB, FLASH_FLAG_SAVE_ADDRESS);
//  dummyFlagData = 0xAA;
//  extFlashPageWrite(FLASH_FLAG_SAVE_ADDRESS, &dummyFlagData, 1);
  dummyFlagData = 0;
  extFlashPageRead(FLASH_FLAG_SAVE_ADDRESS,&dummyFlagData, 1);
  if(dummyFlagData == 0xAA){
	  extFlashErase(BLOCK_ERASE_64KB, FLASH_PAGE0_BASE_ADDRESS);
	  extFlashErase(BLOCK_ERASE_64KB, FLASH_PAGE1_BASE_ADDRESS);
	  flashWriteAddress = FLASH_PAGE0_BASE_ADDRESS;
	  for(uint8_t i = 0; i < 32; i++){
		  /* write the same data 32 time
		   * 32*256=8K in the external flash
		   */
		  extFlashPageWrite(flashWriteAddress, userData, sizeof(userData));
		  flashWriteAddress += sizeof(userData);
	  }
  	  extFlashErase(SECTOR_ERASE_4KB, FLASH_FLAG_SAVE_ADDRESS);
  	  dummyFlagData = 0xBB;
  	  extFlashPageWrite(FLASH_FLAG_SAVE_ADDRESS, &dummyFlagData, 1);
  }
  else if(dummyFlagData == 0xBB){
	  for(uint8_t pageCount =0; pageCount < 4; pageCount++){
		  for(uint16_t i=0; i < 512; i++){
			  extFlashPageRead(extFlashReadAddress,(uint8_t*)&flashReadData[i], 4);
			  extFlashReadAddress += 4;
		  }
		  errorCode = intFlashWrite1(flashReadData, (sizeof(flashReadData)/4));
	  }
  	  extFlashErase(SECTOR_ERASE_4KB, FLASH_FLAG_SAVE_ADDRESS);
  	  dummyFlagData = 0xAA;
  	  extFlashPageWrite(FLASH_FLAG_SAVE_ADDRESS, &dummyFlagData, 1);
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(packetReceived){
		  packetReceived = 0;
		  processPacket(uartRxData, receivedPacketSize);
	  }
	  HAL_GPIO_WritePin(Led_Green_GPIO_Port, Led_Green_Pin, GPIO_PIN_SET);
	  HAL_Delay(100);
	  HAL_GPIO_WritePin(Led_Green_GPIO_Port, Led_Green_Pin, GPIO_PIN_RESET);
	  HAL_Delay(1000);
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
	packetReceived = 1;
	receivedPacketSize = Size;
	HAL_UARTEx_ReceiveToIdle_IT(&huart2, uartRxData, 512);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
