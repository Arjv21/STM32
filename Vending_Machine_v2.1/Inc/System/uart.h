/****************************************************************************
* Title                 :   UART
* Filename              :   uart.h
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
/** @file	: HAL_USART.h
 *  @brief	: This is Interface to the API layer to Initialize, Read and
 *			  and Write using USART Peripheral
 *
 */

#ifndef __UART_H__
#define __UART_H__

#include "uart_conf.h"

void Uart_Init(void);

void Uart_Transmit_Data_Dma(uint32_t buff_addr, uint32_t len);

uint32_t Uart_Receive_Data(uint8_t **pBuffer);

uint8_t* Uart_Get_Current_Buffer(void);

uint8_t* Uart_Get_Transmit_Buffer(void);


#endif
