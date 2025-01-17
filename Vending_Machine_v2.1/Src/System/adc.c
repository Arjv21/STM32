/*******************************************************************************
* Title                 :   Adc.h
* Filename              :   Adc.h
* Author                :   Meghana
* Origin Date           :   12/06/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F3x
* Notes                 :   Conrtol and process Sensors and Sensor drive
*
* THIS SOFTWARE IS PROVIDED BY UNISEM ELECTRONICS "AS IS" AND ANY EXPRESSED
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL UNISEM ELECTRONICS  OR ITS CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************/
/*************** SOURCE REVISION LOG *****************************************
*
*	Date			Version		Author		Description
*	12/06/2024		1.0.0		Meghana		Initial Release.
*
*******************************************************************************/
/** @file	: Adc.c
 *  @brief	: Configure internal ADC. Functions for reading battery voltage.
 */

/******************************************************************************
* Includes
*******************************************************************************/
#include "adc_conf.h"
#include "gpio_conf.h"
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/
//#define DISABLE_ADC(ADC)		{ADC->CR |= 0x00000002;
//								while(ADC->CR & 0x00000002);
//}

#define ENABLE_ADC(ADC)   	{ADC->CR |= 0x00000001;			\
						while (!(ADC->ISR & 0x00000001));	\
						}												// Wait until ADC is ready



#define CALIB_ADC(ADC)		{ADC->CR |= 0X80000000;  		\
  	  	  	  	  	  	while(ADC->CR & 0x80000000);		\
						}												// Start calibration

#define ADC_START_CONVERSION 	0x00000004
#define ADC_EOC					0x00000004

/******************************************************************************
* Module Typedefs
*******************************************************************************/

/******************************************************************************
* Module Global/Extern Variable Definitions
*******************************************************************************/

/******************************************************************************
* Module Static Variable Definitions
*******************************************************************************/
uint32_t adc_value = 0;
float adc_read_battery_val1=0;
float adc_read_battery_val2=0;
/******************************************************************************
* Function Definitions
*******************************************************************************/


/******************************************************************************
* Function 		: ADC1_Init
*//**
* \b Description: Initialize adc.
* PRE-CONDITION	: The System clock and Gpio must be initialized,
* POST-CONDITION: adc is initialized
* @Parameters	: None
* @return		: none
*
* \b Example Example:
* @code
*	ADC1_Init();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 30/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void ADC1_Init(void)
{

  BATTERY_LEVEL_ADC_GPIO->MODER |= BATTERY_LEVEL_ADC_MODER_CONF;        // Analog input PA1
  ADC1_2_COMMON->CCR 			|= 0x00010000;
//  DISABLE_ADC(BATTERY_LEVEL_ADC);
//  if (ADC1->CR & ADC_CR_ADEN)               // Ensure the ADC is disabled before calibration

  BATTERY_LEVEL_ADC->CR &= ~ADC_CR_ADVREGEN;
  BATTERY_LEVEL_ADC->CR |= ADC_CR_ADVREGEN_0; // Enable the voltage regulator

  Delay_Ms(10);

  BATTERY_LEVEL_ADC->CR &= ~0x00000001;             // Disable ADC


  BATTERY_LEVEL_ADC->CR |= ADC_CR_ADCAL;                 // Start calibration
  while(BATTERY_LEVEL_ADC->CR & ADC_CR_ADCAL){}       // Wait until calibration is complete
//  CALIB_ADC(BATTERY_LEVEL_ADC);

  ENABLE_ADC(BATTERY_LEVEL_ADC);

  BATTERY_LEVEL_ADC->CFGR 		|= BATTERY_LEVEL_ADC_CFGR_CONF;
  BATTERY_LEVEL_ADC->SMPR1 		|= BATTERY_LEVEL_ADC_SMPR1_CONF;				// ADC1 CAHNNEL 7
  BATTERY_LEVEL_ADC->SQR1 		|= BATTERY_LEVEL_ADC_SQR1_CONF;				// Set channel 7 as the first conversion
 }


/******************************************************************************
* Function 		: ADC1_Init
*//**
* \b Description: Initialize adc.
* PRE-CONDITION	: The System clock and Gpio must be initialized,
* POST-CONDITION: adc is initialized
* @Parameters	: None
* @return		: none
*
* \b Example Example:
* @code
*	ADC1_Init();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 30/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
float ADC1_Read_battery_value(void)
{
	BATTERY_LEVEL_ADC->CR |= ADC_START_CONVERSION;			// Start ADC conversion
	while (!(BATTERY_LEVEL_ADC->ISR & ADC_EOC));			// Wait until conversion is complete
    adc_value =  ADC1->DR;                       			// Read the ADC value
    adc_read_battery_val2 = adc_value / 1236.0f; 			// Perform float division      1212 and 202 for 200K and 1M reg
    adc_read_battery_val1 = adc_value / 213.0f;  			// Perform float division      and these values for 220K and 1M reg
    //return adc_read_battery_val2;                			//return value
    return adc_read_battery_val1;
}
