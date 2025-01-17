/****************************************************************************
* Title                 :   GPIO_CONF
* Filename              :   gpio_conf.h
* Author                :   Sijeo Philip
* Origin Date           :   26/04/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F3
* Notes                 :   USART Hardware Interface file
*
* THIS SOFTWARE IS PROVIDED BY Unisem Electronic"AS IS" AND ANY EXPRESSED
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
*****************************************************************************/

/*************** INTERFACE CHANGE LIST **************************************
*
*    Date    Version   Author         Description
*  26/04/24   1.0.0   Sijeo Philip   Interface Created.
*
*****************************************************************************/
/** @file	: gpio_conf.h
 *  @brief	: This is Interface to define the GPIOs for each peripherals of the
 *  System
 *
 */

#ifndef __GPIO_CONF_H__
#define __GPIO_CONF_H__

#include "common_settings.h"

#ifndef A7672_UART_TX_GPIO
#define A7672_UART_TX_GPIO		GPIOA
#endif

#ifndef A7672_UART_RX_GPIO
#define A7672_UART_RX_GPIO		GPIOA
#endif

#ifndef A7672_POWER_GPIO
#define A7672_POWER_GPIO		GPIOA
#endif

#ifndef A7672_RESET_GPIO
#define A7672_RESET_GPIO		GPIOA
#endif

#ifndef A7672_POWER_STATUS_GPIO
#define A7672_POWER_STATUS_GPIO	GPIOC
#endif

#ifndef LED_GPIO
#define LED_GPIO				GPIOB
#endif

#ifndef RGB_RED_LED_GPIO
#define RGB_RED_LED_GPIO		GPIOB
#endif

#ifndef RGB_GREEN_LED_GPIO
#define RGB_GREEN_LED_GPIO		GPIOB
#endif

#ifndef RGB_BLUE_LED_GPIO
#define RGB_BLUE_LED_GPIO		GPIOC
#endif

#ifndef SENSOR_GPIO
#define SENSOR_GPIO				GPIOB
#endif

#ifndef MOTOR_GPIO
#define MOTOR_GPIO				GPIOA
#endif

#ifndef FLASH_SPI3_GPIO
#define FLASH_SPI3_GPIO			GPIOC
#endif

#ifndef FLASH_SPI3_CS_GPIO
#define FLASH_SPI3_CS_GPIO		GPIOD
#endif

#ifndef FLASH_RESET_GPIO
#define FLASH_RESET_GPIO		GPIOA
#endif

#ifndef FLASH_WP_GPIO
#define FLASH_WP_GPIO			GPIOB
#endif

#ifndef BATTERY_LEVEL_ADC_GPIO
#define BATTERY_LEVEL_ADC_GPIO	GPIOA
#endif



/*refer pg.167 of Reference Manual to Set PA9 to Alternate Function
 * Please take a note of the Reset Value of the register while setting the value*/
#ifndef A7672_UART_TX_MODER_CONF
#define A7672_UART_TX_MODER_CONF	0x00080000
#endif


#ifndef A7672_UART_TX_AFL_CONF
#define A7672_UART_TX_AFL_CONF		0x00000000
#endif

/*refer pg.172 of Reference Manual to Set Alternate Function of PA9 to UART
* Table 14 of Device Datasheet for Alternate Function Number */
#ifndef A7672_UART_TX_AFH_CONF
#define A7672_UART_TX_AFH_CONF		0x00000070
#endif


/*refer pg.167 of Reference Manual to Set PA10 to Alternate Function
 * Please take a note of the Reset Value of the register while setting the value*/
#ifndef A7672_UART_RX_MODER_CONF
#define A7672_UART_RX_MODER_CONF	0x00200000
#endif



#ifndef A7672_UART_RX_AFL_CONF
#define A7672_UART_RX_AFL_CONF		0x00000000
#endif


/*refer pg.148 of Reference Manual to Set Alternate Function of PA10 to UART
 * Table 14 of Device Datasheet for Alternate Function Number
 */
#ifndef A7672_UART_RX_AFH_CONF
#define A7672_UART_RX_AFH_CONF		0x00000700
#endif


/*Refer Page 167 of Device Reference Manual for GPIOA Port 11 Configured as
 * General Purpose Output Mode.
 */
#ifndef A7672_POWER_MODER_CONF
#define A7672_POWER_MODER_CONF		0x00400000
#endif


/*refer Page 167 of device Reference Manual the GPIOB port 12 is configured
 * in General Purpose Output Mode.
 */
#ifndef LED1_MODER_CONF
#define LED1_MODER_CONF				0x01000000
#endif


/*refer Page 167 of device Reference Manual the GPIOB port 13 is configured
 * in General Purpose Output Mode.
 */
#ifndef LED2_MODER_CONF
#define LED2_MODER_CONF				0x04000000
#endif


/*Refer Page 167 of Device Reference Manual for GPIOB Port 14 is Configured as
 * in Genernal Purpose Output Mode.
 */
#ifndef RGB_RED_LED_MODER_CONF
#define RGB_RED_LED_MODER_CONF		0x10000000
#endif


/*refer pg.167 of Reference Manual to Set PC10,PC11 and PC12 to Alternate Function
 * Please take a note of the Reset Value of the register while setting the value*/
#ifndef FLASH_SPI3_MODER_CONF
#define FLASH_SPI3_MODER_CONF		0x02A00000
#endif


#ifndef FLASH_SPI3_AFL_CONF
#define FLASH_SPI3_AFL_CONF			0x00000000
#endif

/*refer pg.172 of Reference Manual to Set Alternate Function of PC10, PC11,PC12 to SPI
* Table 14 of Device Datasheet for Alternate Function Number */
#ifndef FLASH_SPI3_AFH_CONF
#define FLASH_SPI3_AFH_CONF			0x00066600
#endif


/*refer pg.167 of Reference Manual to Set PD2 to General OUTPUT function
 * Please take a note of the Reset Value of the register while setting the value*/
#ifndef FLASH_SPI3_CS_MODER_CONF
#define FLASH_SPI3_CS_MODER_CONF	0x00000010
#endif


/*refer pg.167 of Reference Manual to Set PA15 to General OUTPUT Pin
 * Please take a note of the Reset Value of the register while setting the value*/
#ifndef FLASH_RST_MODER_CONF
#define FLASH_RST_MODER_CONF		0x40000000
#endif


/*refer pg.167 of Reference Manual to Set PB3 to General OUTPUT pin
 * Please take a note of the Reset Value of the register while setting the value*/
#ifndef FLASH_WP_MODER_CONF
#define FLASH_WP_MODER_CONF			0x00000040
#endif


/*Refer Page 167 of Device Reference Manual for GPIOB Port 15 Configured as
 * General Purpose Output Mode.
 */
#ifndef RGB_GREEN_LED_MODER_CONF
#define RGB_GREEN_LED_MODER_CONF	0x40000000
#endif


/*Refer Page 167 of Device Reference Manual for GPIOC Port 6 Configured as
 * General Purpose Output Mode.
 */
#ifndef RGB_BLUE_LED_MODER_CONF
#define RGB_BLUE_LED_MODER_CONF		0x00001000
#endif


/*refer Page 167 of device Reference Manual the GPIOB port 7 is configured
 * in General Purpose Output Mode. GPIOB Pin 4,5,6 configured as Input Mode.
 */
#ifndef SENSOR_EN_MODER_CONF
#define SENSOR_EN_MODER_CONF		0x00004000
#endif


/*refer Page 167 of device Reference Manual the GPIOB port 12 is configured
 * in General Purpose Output Mode.
 */
#ifndef MOTOR_MODER_CONF
#define MOTOR_MODER_CONF			0x00010000
#endif



/*Refer Page 167 of the Device Reference Manual the GPIOB port 12 is configured
 * as pull down mode, As LEDs are Active High configuration.
 */
#ifndef LED1_PUPDR_CONF
#define LED1_PUPDR_CONF				0x02000000
#endif

/*Refer Page 169 of the Device Reference Manual the GPIOB port 13 is configured
 * as pull down mode, As LED connected is in Active High Mode.
 */
#ifndef LED2_PUPDR_CONF
#define LED2_PUPDR_CONF				0x08000000
#endif


/*Refer Page 167 of the Device Reference Manual the GPIOB port 14 is configured
 * as pull down mode, As LEDs are Active High configuration.
 */
#ifndef RGB_RED_LED_PUPDR_CONF
#define RGB_RED_LED_PUPDR_CONF		0x20000000
#endif

/*Refer Page 169 of the Device Reference Manual the GPIOB port 15 is configured
 * as pull down mode, As LED connected is in Active High Mode.
 */
#ifndef RGB_GREEN_LED_PUPDR_CONF
#define RGB_GREEN_LED_PUPDR_CONF	0x80000000
#endif


/*Refer Page 169 of the Device Reference Manual the GPIOC port 6 is configured
 * as pull down mode, As LED connected is in Active High Mode.
 */
#ifndef RGB_BLUE_LED_PUPDR_CONF
#define RGB_BLUE_LED_PUPDR_CONF		0x00002000
#endif


/*Refer Page 169 of the Device Reference Manual the GPIOA port 11 is configured
 * as pull down mode, As LED connected is in Active High Mode.
 */
#ifndef A7672_POWER_PUPDR_CONF
#define A7672_POWER_PUPDR_CONF		0x00800000
#endif


/*Refer Page 169 of the Device Reference Manual the GPIOB Pins 4,5,6 are configured
 * as pull up mode.
 */
#ifndef SENSOR_PUPDR_CONF
#define SENSOR_PUPDR_CONF			0x00009500
#endif


/*Refer Page 169 of the Device Reference Manual the GPIOA Pin 8 is configured
 * as pull up mode.
 */
#ifndef MOTOR_PUPDR_CONF
#define MOTOR_PUPDR_CONF			0x00010000
#endif


/*Refer Page 167 of Device Reference Manual for GPIOC Port 7 Configured as
 * General Purpose Output Mode.
 */
#ifndef A7672_POWER_STATUS_MODER_CONF
#define A7672_POWER_STATUS_MODER_CONF		0x00000000
#endif

#ifndef A7672_POWER_STATUS_PUPDR_CONF
#define A7672_POWER_STATUS_PUPDR_CONF		0x00008000
#endif



/*Refer Page 169 of the Device Reference Manual the GPIOD port 2 is configured
 * as pull UP mode, As the CS of the Flash is Active Low.
 */
#ifndef FLASH_CS_PUPDR_CONF
#define FLASH_CS_PUPDR_CONF					0x00000010
#endif

/*Refer Page 169 of the Device Reference Manual the GPIOA port 15 is configured
 * as pull UP mode, As the RESET of the Flash is Active Low.
 */
#ifndef FLASH_RST_PUPDR_CONF
#define FLASH_RST_PUPDR_CONF				0x40000000
#endif

/*Refer Page 169 of the Device Reference Manual the GPIOB port 3 is configured
 * as pull UP mode, As the Write Protect of the Flash is Active Low.
 */
#ifndef FLASH_WP_PUPDR_CONF
#define FLASH_WP_PUPDR_CONF					0x00000040
#endif

/*Refer Page 169 of the Device Reference Manual the GPIOB port 3 is configured
 * as pull UP mode, As the Write Protect of the Flash is Active Low.
 */
#ifndef BATTERY_LEVEL_ADC_MODER_CONF
#define BATTERY_LEVEL_ADC_MODER_CONF		0x0000000C
#endif



#endif
