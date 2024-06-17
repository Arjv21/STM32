/****************************************************************************
* Title                 :   SYS_PERI_CLOCK
* Filename              :   sy_peri_clock.h
* Author                :   Sijeo Philip
* Origin Date           :   26/04/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F3
* Notes                 :   System and Peripheral clock interface file
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
/** @file	: sys_peri_clock.h
 *  @brief	: This is Interface to the API layer to Initialize, System and
 *  		  Peripheral clocks
 *
 */

#ifndef __SYS_PERI_CLOCK_H__
#define __SYS_PERI_CLOCK_H__

#include "common_settings.h"

#ifndef GPIOACLKEN
#define GPIOACLKEN		(1U<<17)
#endif

#ifndef GPIOBCLKEN
#define GPIOBCLKEN		(1U<<18)
#endif

#ifndef GPIOCCLKEN
#define GPIOCCLKEN		(1U<<19)
#endif

#ifndef	GPIODCLKEN
#define GPIODCLKEN		(1U<<20)
#endif

#ifndef FLASHSPICLKEN
#define FLASHSPICLKEN	(1U<<15)
#endif

#ifndef A7672UARTCLKEN
#define A7672UARTCLKEN	(1U<<14)
#endif

#ifndef DMA1CLKEN
#define DMA1CLKEN   	(1U<<0)
#endif

#ifndef DMA2CLKEN
#define DMA2CLKEN		(1U<<1)
#endif

#ifndef TIM2CLKEN
#define TIM2CLKEN		(1U<<0)
#endif

#ifndef ADC1CLKEN
#define ADC1CLKEN		(1U<<28)
#endif

void System_Clock_Init(void);

#endif
