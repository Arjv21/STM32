/*******************************************************************************
* Title                 :   led
* Filename              :   led.c
* Author                :   Sijeo Philip
* Origin Date           :   31/05/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F
* Notes                 :  This function is used to drive the LEDs in the System.
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
#include "led.h"
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
* Function : LED_Set_On
*//**
* \b Description:
*
* This function is used set the LED to on state.
*
* PRE-CONDITION: Clocks should be enabled and intialized before calling this function
*
* @return 		None
*
* \b Example Example:
* @code
* 	 	LED_Set_On(LED1_PIN);
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 31/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void LED_Set_On(uint8_t led)
{
	Gpio_Set(LED_GPIO, led);
}
/******************************************************************************
* Function : LED_Set_Off
*//**
* \b Description:
*
* This function is used set the LED to off state.
*
* PRE-CONDITION: Clocks should be enabled and intialized before calling this function
*
* @return 		None
*
* \b Example Example:
* @code
* 	 	LED_Set_Off(LED1_PIN);
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 31/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void LED_Set_Off(uint8_t led)
{
	Gpio_Clear(LED_GPIO, led);
}

/******************************************************************************
* Function : LED_Toggle
*//**
* \b Description:
*
* This function is used to toggle the LED.
*
* PRE-CONDITION: Clocks should be enabled and intialized before calling this function
*
* @return 		None
*
* \b Example Example:
* @code
* 	 	LED_Toggle(LED1_PIN);
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 31/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void LED_Toggle(uint8_t led)
{
	Gpio_Toggle(LED_GPIO, led);
}

