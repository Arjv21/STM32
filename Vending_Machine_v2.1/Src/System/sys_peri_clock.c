/*******************************************************************************
* Title                 :   sys_peri_clock
* Filename              :   sys_peri_clock.c
* Author                :   Sijeo Philip
* Origin Date           :   26/04/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F
* Notes                 :   This functions are used to initialize and configure the system
* 							and peripheral clock
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

#include "sys_peri_clock.h"

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
* Function : System Clock Init
*//**
* \b Description:
*
* This function is used to initialize the System Clock and the peripherals used
*
* POST-CONDITION: No Function calls or Peripheral Initialization functions to be called
* Before calling this function
*
* @return 		None
*
* \b Example Example:
* @code
* 	 	System_Clock_Init();
* @endcode
*
* @Note: The Value of the SYS_FREQ should be changed in "common_settings.h" as the the
* configurations done in this function... i.e. currently it is set to 8MHz i.e. default
* clock is RC Oscillator for this device i.e. STM32F334
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

void System_Clock_Init(void)
{
	/*Check the Datasheet of the Device for the Block Diagram
	 * and check for the clock attached to the below periherals
	 */
	RCC->AHBENR |= GPIOACLKEN | GPIOBCLKEN | GPIOCCLKEN | GPIODCLKEN | ADC1CLKEN;
	RCC->APB1ENR |= TIM2CLKEN | FLASHSPICLKEN;
	RCC->APB2ENR |=  A7672UARTCLKEN;
	RCC->AHBENR |= DMA1CLKEN | DMA2CLKEN;
}
