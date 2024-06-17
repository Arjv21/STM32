/*******************************************************************************
* Title                 :   Sensor
* Filename              :   Sensor.h
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

#ifndef SENSOR_H_
#define SENSOR_H_


/******************************************************************************
* Includes
*******************************************************************************/
#include "common_settings.h"
#include "gpio.h"
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/
#ifndef SENSOR_EN_PIN
#define SENSOR_EN_PIN					7
#endif

#ifndef SENSOR_DISPENSE_SENSE_PIN
#define SENSOR_DISPENSE_SENSE_PIN		4
#endif

#ifndef SENSOR_POSITION_SENSE_PIN
#define SENSOR_POSITION_SENSE_PIN		5
#endif

#ifndef SENSOR_DOOR_OPEN_SENSE_PIN
#define SENSOR_DOOR_OPEN_SENSE_PIN		6
#endif
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
* Function Prototypes
*******************************************************************************/
uint8_t Vending_Machine_EnableSensor(void);
void Vending_Machine_DisableSensor(void);
uint8_t Vending_Machine_WaitForDispense(void);

#endif /* SENSOR_H_ */
