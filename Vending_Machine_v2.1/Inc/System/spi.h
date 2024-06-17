/****************************************************************************
* Title                 :   spi
* Filename              :   spi.h
* Author                :   Sijeo Philip
* Origin Date           :   26/04/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F3
* Notes                 :   SPI Hardware Interface file
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
*  09/06/24   1.0.0   Sijeo Philip   Interface Created.
*
*****************************************************************************/
/** @file	: HAL_USART.h
 *  @brief	: This is Interface to the API layer to Initialize, Read and
 *			  and Write using SPI Peripheral
 *
 */

#ifndef __SPI_H__
#define __SPI_H__

#include "spi_conf.h"
#include "gpio.h"

#define ENABLE_CS 	Gpio_Clear(FLASH_SPI3_CS_GPIO, 2)
#define DISABLE_CS 	Gpio_Set(FLASH_SPI3_CS_GPIO,2)

#define ENABLE_FLASH_RST	Gpio_Clear(FLASH_RESET_GPIO, 15)
#define DISABLE_FLASH_RST	Gpio_Set(FLASH_RESET_GPIO, 15)

#define ENABLE_FLASH_WP		Gpio_Clear(FLASH_WP_GPIO, 3)
#define DISABLE_FLASH_WP	Gpio_Set(FLASH_WP_GPIO, 3)

#define ENABLE_FLASH_SPI			(1U<<6)


void Spi_Init(void);

void Spi_Transmit_Data_Dma(uint32_t buff_addr, uint32_t len);

void Spi_Receive_Data_Dma(uint32_t recvd_msg, uint32_t len);

#endif
