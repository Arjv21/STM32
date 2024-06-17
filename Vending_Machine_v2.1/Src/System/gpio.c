/*******************************************************************************
* Title                 :   gpio
* Filename              :   gpio.c
* Author                :   Sijeo Philip
* Origin Date           :   26/04/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F
* Notes                 :   This functions are used to initialize and control the GPIO
* 							peripheral of the system
*
* THIS SOFTWARE IS PROVIDED BY UNISEM ELECTRONICS "AS IS" AND ANY EXPRESSED
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL UNISEM ELECTRONICS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
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
*    Date    Version   Author         Description
*  26/04/24   1.0.0   Sijeo Philip   Initial Release.
*
*******************************************************************************/

/******************************************************************************
* Includes
*******************************************************************************/
#include "gpio.h"
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/

/******************************************************************************
* Function : Gpio_Init
*//**
* \b Description:
*
* This function is used to initialize the GPIO peripheral
*
* PRE-CONDITION: Clocks should be enabled and intialized before calling this function
*
* @return 		None
*
* \b Example Example:
* @code
* 	 	Gpio_Init();
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 26/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/

void Gpio_Init(void)
{
	/*Configuring the A7672 Communication UART for the AT Commands*/
	A7672_UART_TX_GPIO->MODER |= A7672_UART_TX_MODER_CONF;
	A7672_UART_TX_GPIO->AFR[0] |= A7672_UART_TX_AFL_CONF;
	A7672_UART_TX_GPIO->AFR[1] |= A7672_UART_TX_AFH_CONF;
	A7672_UART_RX_GPIO->MODER |= A7672_UART_RX_MODER_CONF;
	A7672_UART_RX_GPIO->AFR[0] |= A7672_UART_RX_AFL_CONF;
	A7672_UART_RX_GPIO->AFR[1] |= A7672_UART_RX_AFH_CONF;

	/*Configuring GPIO to Switch On/Off the A7672 Module*/
	A7672_POWER_GPIO->MODER |= A7672_POWER_MODER_CONF;
	A7672_POWER_GPIO->PUPDR |= A7672_POWER_PUPDR_CONF;

	/*Configuring the GPIO to Sense the Power Status of A7672 Module*/
	A7672_POWER_STATUS_GPIO->MODER |= A7672_POWER_STATUS_MODER_CONF;
	A7672_POWER_STATUS_GPIO->PUPDR |= A7672_POWER_STATUS_PUPDR_CONF;

	/*Configuring the GPIO to control the Onboard LEDs*/
	LED_GPIO->MODER |= LED1_MODER_CONF | LED2_MODER_CONF;
	LED_GPIO->PUPDR |= LED1_PUPDR_CONF | LED2_PUPDR_CONF;

	/*Configuring GPIO to sense the IR Sensors*/
	SENSOR_GPIO->MODER |= SENSOR_EN_MODER_CONF;
	SENSOR_GPIO->PUPDR |= SENSOR_PUPDR_CONF;

	/*Configuring GPIO to Switch On/Off the Motor */
	MOTOR_GPIO->MODER |= MOTOR_MODER_CONF;
	MOTOR_GPIO->PUPDR |= MOTOR_PUPDR_CONF;


	/*Configuring GPIO to Switch ON/OFF The RGB LEDs expected to be connected externally*/
	RGB_RED_LED_GPIO->MODER |= RGB_RED_LED_MODER_CONF;
	RGB_RED_LED_GPIO->PUPDR |= RGB_RED_LED_PUPDR_CONF;

	RGB_GREEN_LED_GPIO->MODER |= RGB_GREEN_LED_MODER_CONF;
	RGB_GREEN_LED_GPIO->PUPDR |= RGB_GREEN_LED_PUPDR_CONF;

	RGB_BLUE_LED_GPIO->MODER |= RGB_BLUE_LED_MODER_CONF;
	RGB_BLUE_LED_GPIO->PUPDR |= RGB_BLUE_LED_PUPDR_CONF;

	/*Configuring SPI for the External Flash Component */
	FLASH_SPI3_GPIO->MODER |= FLASH_SPI3_MODER_CONF;
	FLASH_SPI3_GPIO->AFR[0] |= FLASH_SPI3_AFL_CONF;
	FLASH_SPI3_GPIO->AFR[1] |= FLASH_SPI3_AFH_CONF;

	FLASH_SPI3_CS_GPIO->MODER |= FLASH_SPI3_CS_MODER_CONF;
	FLASH_SPI3_CS_GPIO->PUPDR |= FLASH_CS_PUPDR_CONF;

	FLASH_RESET_GPIO->MODER |= FLASH_RST_MODER_CONF;
	FLASH_RESET_GPIO->PUPDR |= FLASH_RST_PUPDR_CONF;

	FLASH_WP_GPIO->MODER |= FLASH_WP_MODER_CONF;
	FLASH_WP_GPIO->PUPDR |= FLASH_WP_PUPDR_CONF;

	BATTERY_LEVEL_ADC_GPIO->MODER |= BATTERY_LEVEL_ADC_MODER_CONF;
}
/******************************************************************************
* Function : Gpio_Set
*//**
* \b Description:
*
* This function is used to set the value of the GPIO output to high
*
* PRE-CONDITION: Clocks should be enabled and initialized before calling this function
* PRE-CONDITION: The GPIO Port and Pin should be set to default GPIO output mode for this
* function to work else will not have any effect on the output
* @return 		None
*
* \b Example Example:
* @code
* 	 	Gpio_Set(GPIOB, 13);
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td>31/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void Gpio_Set(GPIO_TypeDef* GPIO, uint8_t pin)
{
	if( pin > 15 )
		return;
	GPIO->BSRR |= (1U<<pin);
}

/******************************************************************************
* Function : Gpio_Clear
*//**
* \b Description:
*
* This function is used to clear the value of the GPIO output to low
*
* PRE-CONDITION: Clocks should be enabled and initialized before calling this function
* PRE-CONDITION: The GPIO Port and Pin should be set to default GPIO output mode for this
* function to work else will not have any effect on the output
* @return 		None
*
* \b Example Example:
* @code
* 	 	Gpio_Clear(GPIOB, 13);
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td>31/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void Gpio_Clear(GPIO_TypeDef* GPIO, uint8_t pin)
{
	if( pin > 15)
		return;
	GPIO->BSRR |= (0x00010000 << pin);
}

/******************************************************************************
* Function : Gpio_Toggle
*//**
* \b Description:
*
* This function is used to toggle the output of GPIO
*
* PRE-CONDITION: Clocks should be enabled and initialized before calling this function
* PRE-CONDITION: The GPIO Port and Pin should be set to default GPIO output mode for this
* function to work else will not have any effect on the output
* @return 		None
*
* \b Example Example:
* @code
* 	 	Gpio_Toggle(GPIOB, 13);
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td>31/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void Gpio_Toggle(GPIO_TypeDef* GPIO, uint8_t pin)
{
	if( pin > 15 )
		return;
	if((GPIO->ODR & (1U<<pin)) != 0)
	{
		Gpio_Clear(GPIO, pin);
	}
	else
	{
		Gpio_Set(GPIO, pin);
	}
}


/******************************************************************************
* Function : Gpio_Read
*//**
* \b Description:
*
* This function is used to read the Port Pin of GPIO
*
* PRE-CONDITION: Clocks should be enabled and initialized before calling this function
* PRE-CONDITION: The GPIO Port and Pin should be set to default GPIO input mode for this
* function to work else will not have reliable return
* @return 		1 if the pin is set else 0 if reset, 2 if error.
*
* \b Example Example:
* @code
* 	 	Gpio_Read(GPIOB, 13);
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td>31/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t Gpio_Read(GPIO_TypeDef* GPIO, uint8_t pin)
{
	if( pin > 15 )
		return 2;

	if((GPIO->IDR & (1U<<pin)) != 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
