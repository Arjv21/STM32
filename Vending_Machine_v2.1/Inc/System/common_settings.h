/****************************************************************************
* Title                 :   common_settings
* Filename              :   common_settings.h
* Author                :   Sijeo Philip
* Origin Date           :   26/04/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F3
* Notes                 :   Common Settings define all the MACROs to the whole system
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
/** @file	: common_settings.h
 *  @brief	: This File holds all the common settings to the System
 *
 */

#ifndef __COMMON_SETTINGS_H__
#define __COMMON_SETTINGS_H__

#include "stm32f302xc.h"
#include <string.h>

#ifndef SYS_FREQ
#define SYS_FREQ 		8000000
#endif


#ifndef SYS_FREQ_MHZ
#define SYS_FREQ_MHZ	8
#endif

#ifndef A7672_UART_CLK
#define A7672_UART_CLK		SYS_FREQ
#endif


#ifndef SYS_DMA
#define SYS_DMA		DMA1
#endif

#ifndef FLASH_SPI_DMA
#define FLASH_SPI_DMA	DMA2
#endif

#ifndef NULL
#define NULL 	(void*)0
#endif


/*****************************************************************
 * Functions
 ****************************************************************/
void Delay_Ms(uint32_t ms);
void Delay_Quantas(uint32_t qt);
uint16_t Convert_String_To_Integer(uint8_t *const number);
uint32_t Convert_String_To_Integer32(uint8_t *const number);
void Convert_Integer_To_String(uint8_t * const str, uint16_t number);
uint8_t * Str_Cpy_Marker(uint8_t * src, uint8_t * dest, uint8_t startMarker, uint8_t endMarker);
void strConcat(char* src, char* dest);
void convert_to_epoch(char* date_time, uint32_t* result);
int convert_Ascii2Int(const char* str, int len);


#endif
