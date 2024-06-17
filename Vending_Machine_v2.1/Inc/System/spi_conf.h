/****************************************************************************
* Title                 :   SPI_CONF
* Filename              :   spi_conf.h
* Author                :   Sijeo Philip
* Origin Date           :   09/06/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F3
* Notes                 :   SPI Hardware Configuration File
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
/** @file	: uart_conf.h
 *  @brief	: This is Interface file to configure the SPI peripheral of the
 *  		  System. Need to refer the Reference Manual to set the appropriate
 *  		  Values.
 *
 */

#ifndef __SPI_CONF_H__
#define __SPI_CONF_H__


#include "common_settings.h"

#ifndef FLASH_SPI
#define FLASH_SPI		SPI3
#endif

#ifndef A7672_UART_IRQ
#define A7672_UART_IRQ	SPI3_IRQn
#endif

#ifndef FLASH_SPI3_TX_DMA_STREAM
#define FLASH_SPI3_TX_DMA_STREAM 	DMA2_Channel2
#endif

#ifndef FLASH_SPI3_TX_DMA_IRQ
#define FLASH_SPI3_TX_DMA_IRQ		DMA2_Channel2_IRQn
#endif


#ifndef FLASH_SPI3_TX_DMA_HANDLER
#define FLASH_SPI3_TX_DMA_HANDLER	DMA2_CH2_IRQHandler
#endif

#ifndef FLASH_SPI_HANDLER
#define FLASH_SPI_HANDLER			SPI3_IRQHandler
#endif


#ifndef FLASH_SPI3_RX_DMA_STREAM
#define FLASH_SPI3_RX_DMA_STREAM 	DMA2_Channel1
#endif

#ifndef FLAS_SPI3_RX_DMA_IRQ
#define FLASH_SPI3_RX_DMA_IRQ		DMA2_Channel1_IRQn
#endif


#ifndef FLASH_SPI3_RX_DMA_HANDLER
#define FLASH_SPI3_RX_DMA_HANDLER	DMA2_CH1_IRQHandler
#endif


/*
 * SSI and SSM is enabled.
 * SPI is enabled in Master Mode.
 * Baud rate is set to PCLK/4
 * CPHA and CPOL is set Clk to 1 when idle, Second clock edge is first data capture.
 *
 */

#ifndef FLASH_SPI_CR1_CONF
#define FLASH_SPI_CR1_CONF		0x0000030F
#endif

/*
 *.DMARX and DMATX is Enabled.
 */
#ifndef FLASH_SPI_CR2_CONF
#define FLASH_SPI_CR2_CONF		0x00000003
#endif


/**
 * TCIE AND TEIE IS ENABLED.
 * DIRECTION IS SET TO READ FROM MEMORY
 * MINC MEMORY INCREMENT IS ENABLED.
 * TRANSFER SIZE IS SET TO 8BITS
 */
#ifndef FLASH_SPI3_TX_DMA_CCR_CONF
#define FLASH_SPI3_TX_DMA_CCR_CONF		0x0000109A
#endif

/**
 * TCIE AND TEIE IS ENABLED.
 * DIRECTION IS SET TO READ FROM PERIPHERAL
 * MINC MEMORY INCREMENT IS ENABLED.
 * TRANSFER SIZE IS SET TO 8BITS
 */
#ifndef FLASH_SPI3_RX_DMA_CCR_CONF
#define FLASH_SPI3_RX_DMA_CCR_CONF		0x0000108A
#endif

#endif
