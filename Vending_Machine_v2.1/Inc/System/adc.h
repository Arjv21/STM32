/*******************************************************************************
* Title                 :   Adc.h
* Filename              :   Adc.h
* Author                :   Meghana
* Origin Date           :   12/06/2024
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
*	12/06/2024		1.0.0		Meghana		Initial Release.
*
*******************************************************************************/
/** @file	: Adc.c
 *  @brief	: Conrtol and process Sensors and Sensor drive.
 */

#ifndef ADC_H_
#define ADC_H_

/******************************************************************************
* Includes
*******************************************************************************/
#include "adc_conf.h"
#include "gpio.h"
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/
#ifndef _EN_PIN
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
void ADC1_Init(void);
float ADC1_Read_battery_value(void);

#endif /* MISC_ADC_H_ */
