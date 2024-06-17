/****************************************************************************
* Title                 :   UART_CONF
* Filename              :   uart_conf.h
* Author                :   Sijeo Philip
* Origin Date           :   26/04/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F3
* Notes                 :   USART Hardware Configuration File
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
/** @file	: uart_conf.h
 *  @brief	: This is Interface file to configure the UART peripheral of the
 *  		  System. Need to refer the Reference Manual to set the appropriate
 *  		  Values.
 *
 */

#ifndef __UART_CONF_H__
#define __UART_CONF_H__


#include "common_settings.h"

#ifndef A7672_UART
#define A7672_UART		USART1
#endif

#ifndef A7672_UART_IRQ
#define A7672_UART_IRQ	USART1_IRQn
#endif

#ifndef A7672_UART_TX_DMA_STREAM
#define A7672_UART_TX_DMA_STREAM 	DMA1_Channel4
#endif

#ifndef A7672_UART_TX_DMA_IRQ
#define A7672_UART_TX_DMA_IRQ		DMA1_Channel4_IRQn
#endif


#ifndef A7672_UART_TX_DMA_HANDLER
#define A7672_UART_TX_DMA_HANDLER	DMA1_CH4_IRQHandler
#endif

#ifndef A7672_UART_HANDLER
#define A7672_UART_HANDLER			USART1_EXTI25_IRQHandler
#endif


#ifndef A7672_UART_BR
#define A7672_UART_BR	115200
#endif

#ifndef A7672_UART_BUFFSIZE_CONF
#define A7672_UART_BUFFSIZE_CONF	2048
#endif

#ifndef A7672_UART_TX_BUFFSIZE_CONF
#define A7672_UART_TX_BUFFSIZE_CONF  2048
#endif

#ifndef A7672_UART_BUFFCOUNT_CONF
#define A7672_UART_BUFFCOUNT_CONF	2
#endif

#ifndef A7672_UART_INTERBYTE_DELAY_CONF
#define A7672_UART_INTERBYTE_DELAY_CONF		10 	/*THIS IS THE NUMBER OF QUANTAS CONFIGURED FOR SCHEDULER */
#endif



/*
 * Word length : 1 Startbit, 8 databits, n stop bits
 * End of Block Interrupt is Disabled
 * Receiver Timeout Interrupt is Disabled
 * DEAT - Driver Enable Assertion time is 0
 * DEDT - Driver Enable Deassertion Time is 0
 * OverSampling Mode is set at 16
 * Character Match interrupt is disabled.
 * Mute Mode is disabled i.e. Receiver is in active mode Permanently
 * Uart wakeup method is idle line
 * Parity control is disabled
 * Parity Error Interrupt is disabled
 * Transmit Empty interrupt is disabled.
 * Transmit Complete interrupt is enabled
 * Interrupt on ORE and RXNE is enabled.
 * IDLE Interrupt is disabled.
 * Transmitter is enabled.
 * Receiver is enabled.
 */

#ifndef A7672_UART_CR1_CONF
#define A7672_UART_CR1_CONF		0x0000006C
#endif

/*
 * Wakeup from Stop Mode Interrupt is Disabled
 * The default value of Wakeup from Stop is active on address Match (This is not used)
 * ReTransmission of Smart Card Auto Retry is disabled
 * Driver Enable Polarity (DE Function) is set to Active High
 * Driver or DE Function is Disabled
 * DMA is not disabled in case of reception error. The corresponding error flag is set but RXNE is kept 0
 * 		preventing from overrun. As a consequence, the DMA request is not asserted. So the erroneous
 * 		data is transferred (no DMA request), but next correct received data will be transferred
 * 		(use for SmartCard mode).
 * 	Overrun Error Flag is set when received data is not read before receiving new data.
 * 	Three Sample bit method is selected.
 * 	CTS interrupt is disabled.
 * 	CTS Hardware flow control is disabled.
 * 	RTS Hardware flow control is disabled.
 * 	DMA Mode is enabled for transmission
 * 	DMA Mode is disabled for Reception.
 * 	SmartCard Mode is disabled.
 * 	NACK Transmission in case of parity error is disabled.
 * 	Half duplex Mode is not selected.
 * 	IrDA is in normal mode.
 * 	IrDA is disabled.
 * 	Error Interrupt in case of framing error, overrun error or noise flag is disabled.
 */
#ifndef A7672_UART_CR3_CONF
#define A7672_UART_CR3_CONF			0x00000080
#endif

/*
 * The DMA should be disabled
 * Memory to Memory Mode is disabled
 * Priority level is set at low
 * Data size of each DMA Transfer to the identified Memory is set to 8 bits
 * Data size of each DMA Transfer to the identified Peripheral is set to 8 bits
 * The increment mode for each DMA transfer to the identified memory is enabled
 * The Increment mode for each DMA transfer to the identified peripheral is disabled.
 * Circular Mode is disabled
 * DIR is set to 1 to read from Memory
 * Transfer Error Interrupt is disabled
 * Half Transfer interrupt disabled
 * Transfer Complete interrupt is enabled.
 *
 *Note : The Value of CCR can be set only if the DMA is disabled, else will not take effect
 */
#ifndef A7672_UART_DMA_TX_CCR_CONF
#define A7672_UART_DMA_TX_CCR_CONF		0x00000092
#endif



#endif
