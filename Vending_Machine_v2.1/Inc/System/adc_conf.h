/****************************************************************************
* Title                 :   ADC_CONF
* Filename              :   adc_conf.h
* Author                :   Meghana
* Origin Date           :   12/06/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F3
* Notes                 :   ADC Hardware Configuration File
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
*  12/06/24   1.0.0   Meghana   Interface Created.
*
*****************************************************************************/
/** @file	: adc_conf.h
 *  @brief	: This is Interface file to configure the ADC peripheral of the
 *  		  System. Need to refer the Reference Manual to set the appropriate
 *  		  Values.
 *
 */

#ifndef ADC_CONF_H_
#define ADC_CONF_H_


#include "common_settings.h"

#ifndef BATTERY_LEVEL_ADC
#define BATTERY_LEVEL_ADC				ADC1
#endif

#ifndef BATTERY_LEVEL_ADC_CFGR_CONF
#define BATTERY_LEVEL_ADC_CFGR_CONF		0X00000000
#endif

#ifndef BATTERY_LEVEL_ADC_SMPR1_CONF
#define BATTERY_LEVEL_ADC_SMPR1_CONF	0X000001C0
#endif

#ifndef BATTERY_LEVEL_ADC_SQR1_CONF
#define BATTERY_LEVEL_ADC_SQR1_CONF		0X00000080
#endif


#endif /* SYSTEM_ADC_CONF_H_ */
