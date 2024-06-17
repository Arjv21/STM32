/****************************************************************************
* Title                 :   led
* Filename              :   led.h
* Author                :   Sijeo Philip
* Origin Date           :   31/05/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F#
* Notes                 :   This file is used to drive LEDs in the System
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
*****************************************************************************/
/*************** INTERFACE CHANGE LIST **************************************
*
*    Date    Version   Author         Description
*  31/05/24   1.0.0   Sijeo Philip   Interface Created.
*
*****************************************************************************/
/** @file led.h
 *  @brief This Module contains all the driver interface for LEDs
 *
 *  This is Header file for the interface for LED driver APIs.
 */

#ifndef __LED_H__
#define __LED_H__
/******************************************************************************
* Includes
*******************************************************************************/
#include "common_settings.h"
/******************************************************************************
* Preprocessor Constants
*******************************************************************************/
#ifndef LED1_PIN
#define LED1_PIN			12
#endif

#ifndef LED2_PIN
#define LED2_PIN			13
#endif

#ifndef RGB_RED_LED_PIN
#define RGB_RED_LED_PIN		14
#endif

#ifndef RGB_GREEN_LED_PIN
#define RGB_GREEN_LED_PIN	15
#endif

#ifndef RGB_BLUE_LED_PIN
#define RGB_BLUE_LED_PIN	6
#endif

/******************************************************************************
* Configuration Constants
*******************************************************************************/

/******************************************************************************
* Macros
*******************************************************************************/


/******************************************************************************
* Typedefs
*******************************************************************************/

/******************************************************************************
* Variables
*******************************************************************************/


/******************************************************************************
* Function Prototypes
*******************************************************************************/
void LED_Set_On(uint8_t led);
void LED_Set_Off(uint8_t led);
void LED_Toggle(uint8_t led);

#endif
/*******************************************************END OF FILE **************************************************/
