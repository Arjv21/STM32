/*******************************************************************************
* Title                 :   Sensor
* Filename              :   Sensor.c
* Author                :   ArunRaj
* Origin Date           :   08/06/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F3x
* Notes                 :   Conrtol and process Sensors and Sensor drive
*
* THIS SOFTWARE IS PROVIDED BY UNISEM ELECTRONICS "AS IS" AND ANY EXPRESSED
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL UNISEM ELECTRONICS  OR ITS CONTRIBUTORS BE LIABLE FOR ANY
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
*	Date			Version		Author		Description
*	08/06/2024		1.0.0		ArunRaj		Initial Release.
*
*******************************************************************************/
/** @file	: Sensor.c
 *  @brief	: Conrtol and process Sensors and Sensor drive.
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "Sensor.h"
#include "app.h"
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
* Module Global/Extern Variable Definitions
*******************************************************************************/

/******************************************************************************
* Module Static Variable Definitions
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/



/******************************************************************************
* Function 		: Vending_Machine_EnableSensor
*//**
* \b Description: Enables Sensor module and initializes dispense sensor.
* PRE-CONDITION	: The System clock and Gpio must be initialized,
* POST-CONDITION: Dispense Sensor module is initialized
* @Parameters	: None
* @return		: 0 in case of the Module is Off else if On it will return 1
*
* \b Example Example:
* @code
*	uint8_t ret = Vending_Machine_EnableSensor();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 30/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/

uint8_t Vending_Machine_EnableSensor(void){
	Gpio_Set(SENSOR_GPIO, SENSOR_EN_PIN);
	Delay_Ms(10);
	if(Gpio_Read(SENSOR_GPIO, SENSOR_DISPENSE_SENSE_PIN) == 1)
		return 1;
	else
		return 0;
}

/******************************************************************************
* Function 		: Vending_Machine_DisableSensor
*//**
* \b Description: Disables Sensor module.
* PRE-CONDITION	: The System clock and Gpio must be initialized,
* POST-CONDITION: Dispense Sensor module is disables
* @Parameters	: None
* @return		: none
*
* \b Example Example:
* @code
*	Vending_Machine_DisableSensor();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 30/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void Vending_Machine_DisableSensor(void){
	Gpio_Clear(SENSOR_GPIO, SENSOR_EN_PIN);
}

/******************************************************************************
* Function 		: Vending_Machine_WiatForDispense
*//**
* \b Description: waits till the dispense operation is success or timeout occurs..
* PRE-CONDITION	: The System clock and Gpio must be initialized,
* POST-CONDITION: Dispense is success or failed.
* @Parameters	: None
* @return		: 1 if dispense success.
* 				: 0 if dispense failed.
*
* \b Example Example:
* @code
*	uint8_t ret = Vending_Machine_WiatForDispense();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 30/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t Vending_Machine_WaitForDispense(void){
	static uint32_t	timeOut;
	uint8_t		retVal	= 0;
	timeOut = 12000;
	while(timeOut){
		Delay_Ms(1);
		if(Gpio_Read(SENSOR_GPIO, SENSOR_DISPENSE_SENSE_PIN) == 0){
			measurementMatrix[SENSOR_PULSE_COUNT]++;
			retVal = 1;
			timeOut = 0;
			break;
		}
		timeOut--;
	}
	return retVal;
}
