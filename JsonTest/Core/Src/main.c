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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "JSONWriter.h"
#include "JSONParser.h"
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
struct jWriteControl testJSON;
struct jsonParser testReadJSON;
struct jsonReadToken *jsonPacket;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void jWriteTest(void);
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
//  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
//  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  jWriteTest();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void jWriteTest(void){
    char buffer[1024];
    unsigned int buflen = 1024;
    unsigned char err;
    //i;
//    char buffer1[1024]= "[\"array1\", 0.12345, 345, true, null, {\"key1\":\"value1\", \"int\":10}]";

//    printf("A JSON object example :\n\n");

    jwOpen(&testJSON, buffer, buflen, JW_OBJECT, JW_COMPACT);

    jwObj_string(&testJSON, "key", "value");
    jwObj_int(&testJSON, "int", 10);
    jwObj_double(&testJSON, "double", 1.265487);
    jwObj_null(&testJSON, "null Thing");
    jwObj_bool(&testJSON, "boolean", 1);
    jwObj_array(&testJSON, "EmptyArray");
    jwEnd(&testJSON);
    jwObj_array(&testJSON, "anArray");
    jwArr_string(&testJSON, "array one");
    jwArr_int(&testJSON, -1);
    jwArr_double(&testJSON, 12.568970);
    jwArr_null(&testJSON);
    jwArr_bool(&testJSON, 0);
    jwArr_object(&testJSON);
        jwObj_string(&testJSON, "objArr1", "value1");
        jwObj_string(&testJSON, "objArr2", "value2");
        jwEnd(&testJSON);
    jwArr_array(&testJSON);
        jwArr_int(&testJSON, 1);
        jwArr_int(&testJSON, 2);
        jwArr_int(&testJSON, 3);
    jwEnd(&testJSON);
    jwEnd(&testJSON);
    jwObj_object(&testJSON, "Empty Object");
    jwEnd(&testJSON);
    jwObj_object(&testJSON, "anObject");
    jwObj_string(&testJSON, "msg", "Object in Object");
    jwObj_string(&testJSON, "msg2", "Object in object 2nd");
    jwEnd(&testJSON);
    jwObj_string(&testJSON, "ObjectEntry", "This is the last one");
    jwClose(&testJSON);

//   printf(buffer);
//   printf(buffer1);

    err= JSONStructInit(&testReadJSON, buffer);
//    printf("\nThe number of tokens parsed %d \n", testReadJSON.numTokens);
//    printf("Buffer initialized for reading \n ");
//    printf("The value of error is %d \n", err);
    if(err != JDATA_NOK)
    {
        while(testReadJSON.parserStatus!=JSON_END)
        err = JSONParseData(&testReadJSON);
//        printf("The Error code is %d\n", err);
//        printf("The Number of Tokens parsed are %d\n", testReadJSON.numTokens);
//        printf("The value of key parsed is %s\n", testReadJSON.jData[2].keyValue);
//        printf("The value of data parsed is %s\n", testReadJSON.jData[5].jsonValue);
//        printf("The Value of Stack Position is %d", testReadJSON.parserStatus);
       jsonPacket = SearchToken(&testReadJSON, "\"anObject\"");
       if(jsonPacket!=NULL)
       {
//           printf("Packet Found!!!!\n");
//           printf("The Value of the entered key is %s\n", jsonPacket->jsonValue);
       }

    }

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
