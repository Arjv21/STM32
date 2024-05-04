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
#include "ext_flash_w25q64.h"
#include "int_flash.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
SPI_HandleTypeDef hspi3;

/* USER CODE BEGIN PV */

//uint32_t 	readData[64] = {0};
//uint8_t 	readWord[4];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI3_Init(void);
/* USER CODE BEGIN PFP */
static void goto_application( void );
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//void Init_SPI(void){
//	RCC->AHBENR = (0x0F << 20);			// ENABLE IOPORTS A,B,C,D
//	RCC->APB1ENR |= (1 << 15);			// ENABLE SPI3 MODULE CLOCK
//}
//
//void setPinMode(void){
//	// RESET PIN MODES
//	GPIOA->MODER &=  ~(3u << (2*15));	// CLEAR PA15
//	GPIOB->MODER &=  ~(3u << (2*3));	// CLEAR PB3
//	GPIOC->MODER &= ~((3u << (2*10))	// CLEAR PC10
//					 |(3u << (2*11))	// CLEAR PC11
//					 |(3u << (2*12)));	// CLEAR PC12
//	GPIOD->MODER &=  ~(3u << (2*2));	// CLEAR PD2
//
//	// CONFIGURE PIN MODES
//	GPIOA->MODER |=  (1u << (2*15));	// SET PA15 AS GPIO (F_RST)
//	GPIOB->MODER |=  (1u << (2*3));		// SET PB3 AS GPIO 	(F_WP)
//	GPIOC->MODER |= ((2u << (2*10))		// SET PC10 TO AF 	(CLK)
//					|(2u << (2*11))		// SET PC11 TO AF 	(MOSI)
//					|(2u << (2*12)));	// SET PC12 TO AF 	(MISO)
//	GPIOD->MODER |=  (1u << (2*2));		// SET PD2 AS GPIO 	(F_nCS)
//}
unsigned char deviceIdRxData[2];

void changeWordEndian(uint8_t* readWordPtr);
void changeWordEndian(uint8_t* readWordPtr){
	uint8_t tempVar = 0;
	tempVar = readWordPtr[3];
	readWordPtr[3] = readWordPtr[0];
	readWordPtr[0] = tempVar;
	tempVar = readWordPtr[2];
	readWordPtr[2] = readWordPtr[1];
	readWordPtr[1] = tempVar;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint32_t	flashReadData[512] 		= {0};
	uint32_t	extFlashReadAddress 	= FLASH_PAGE0_BASE_ADDRESS;
//	uint32_t	intFlashWriteAddress 	= 0x50000;
//	uint32_t	extFlashWriteAddress 	= 0x0;
//	uint16_t	totalFirmwareBytes		= 0;
	uint16_t	totalWordsToRead 		= 0;
	uint16_t	wordsToRead 			= 0;
	uint16_t	wordsRead				= 0;
	uint8_t		device_Id[2]			= {0};
	uint8_t		savedFlagData[3]		= {0};


//	unsigned char statusByte=0;
//	unsigned char readByte = 0;
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
  /* USER CODE BEGIN 2 */
  extFlashHardReset();
  extFlashSendCommand(ENABLE_RESET);
  extFlashSendCommand(RESET_DEVICE);
  extFlashSendCommand(GLOBAL_UNLOCK);
  extFlashGetDeviceId(device_Id);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  extFlashPageRead(FLASH_FLAG_SAVE_ADDRESS,savedFlagData, 3);
  if(savedFlagData[0] == 0xBB){
	  totalWordsToRead = 0;
	  totalWordsToRead = savedFlagData[1];
	  totalWordsToRead = (totalWordsToRead << 8) | savedFlagData[2];
//	  totalWordsToRead = totalFirmwareBytes/4;
	  do{
		  if(totalWordsToRead >= 512)
			  wordsToRead = 512;
		  else
			  wordsToRead = totalWordsToRead;
		  for(wordsRead = 0; wordsRead < wordsToRead; wordsRead++){
	  		  extFlashPageRead(extFlashReadAddress,(uint8_t*)&flashReadData[wordsRead], 4);
	  		  extFlashReadAddress += 4;
//	  		changeWordEndian((uint8_t*)&flashReadData[wordsRead]);
	  	  }
		  totalWordsToRead -= wordsToRead;
		  intFlashWrite1(flashReadData, wordsRead);
	  }while(totalWordsToRead != 0);
	  resetFotaFlag();
  }
  while (1)
  {
//	  for(unsigned char i = 0; i<=20; i++ ){
//		  HAL_Delay(100);
//	  }
	  	  goto_application();
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 7;
  hspi3.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi3.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(F_RST_GPIO_Port, F_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(F_nCS_GPIO_Port, F_nCS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(F_WP_GPIO_Port, F_WP_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : F_RST_Pin */
  GPIO_InitStruct.Pin = F_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(F_RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : F_nCS_Pin */
  GPIO_InitStruct.Pin = F_nCS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(F_nCS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : F_WP_Pin */
  GPIO_InitStruct.Pin = F_WP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(F_WP_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
static void goto_application( void )
{
//  printf("Going to Jump to Application...\n");
  void (*app_reset_handler)(void) = (void*)(*((volatile uint32_t*)(0x08005000 + 4U)));
  app_reset_handler();
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
