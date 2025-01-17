/****************************************************************************
* Title                 :  	app
* Filename              :   app.h
* Author                :   Sijeo Philip
* Origin Date           :   30/04/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F3
* Notes                 :   APIs to be exposed to be used in Main application.
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
*  30/04/24   1.0.0   Sijeo Philip   Interface Created.
*
*****************************************************************************/
/** @file	: uart_conf.h
 *  @brief	: This is Interface file to Interface between the Driver/Middleware
 *  		  and the user application.
 *  @Note 	: This file can be used as Interface to Main Entry Point and Test the
 *  		  Drivers and Middleware Independently if needed.
 */

#ifndef __APP_H__
#define __APP_H__

/*All Function which are exposed to the Main application will be Prefixed with
 * Vending_Machine_ and will be Camel Cased.*/
#include "osKernel.h"
#include "ATCMD_Core.h"
#include "ATCMD_A7672_Commands.h"
#include "led.h"
//#include "Sensor.h"
//#include "Motor.h"

typedef enum{
	CERT_NOT_OK,
	CERT_OK
}certState_t;

typedef enum{
	NW_REGN_FAILED,
	NW_REGN_SUCCESS
}nwRegnStatus_t;

typedef enum{
	DISPENSE_FAILED,
	DISPENSE_SUCCESS
}dispStatus_t;

typedef enum{
	CONNECT_NOT_CONNECTED,
	CONNECT_FAILED,
	CONNECT_SUCCESS,
	CONNECT_DISCONNECTED
}ConnectStatus_t;

typedef enum{
	SYS_MODE_TEST,
	SYS_MODE_NORMAL_OP,
	SYS_MODE_CONFIG,
	SYS_MODE_FOTA,
	SYS_MODE_FAULT
}sysMode_t;

typedef enum{
	FAULT_NW_FAIL,
	FAULT_BAT_DOWN,
	FAULT_HTTP_CONNECT_FAI,
	FAULT_MQTT_CONNECT_FAIL,
}fault_t;

typedef enum{
	GSM_INFO_NOT_UPDATED,
	GSM_INFO_UPDATED
}gsmInfoStatus_t;


typedef enum{
	MODEM_OFF=0,
	MODEM_ON,
	MODEM_READY,
	MODEM_NOT_RDY,
	MODEM_IDLE
}modemStatus_t;

typedef enum{
	ANNOUNCE_NOT_SENT		= 0,
	ANNOUNCE_SENT			= 1
}announceStatus_t;

typedef enum{
	REQ_TYPE_NONE			= 0,
	REQ_TYPE_DIPSENSE		= 1,
	REQ_TYPE_CHANGE_CHANNEL	= 2,
	REQ_TYPE_REBOOT			= 3,
	REQ_TYPE_MOTOR_TEST		= 4,
	REQ_TYPE_OTA_START		= 5
}requestType_t;

typedef enum{
	MQTT_STATE_INIT = 0,
	MQTT_STATE_ANNOUNCED,
}mqttStatus_t;







extern uint32_t measurementMatrix[7];

#define MAX_MODEM_RESTART_COUNT 3
#define MAX_DISP_RETRY_COUNT	2

#define SENSOR_PULSE_COUNT		0
#define MOTOR_RUN_PULSE_COUNT	1
#define TOTAL_RETRIES			2
#define TOTAL_TRIGGER			3
#define TOTAL_RETRY_TRIGGER		4
#define TOTAL_FIRST_TRIGGER		5
#define RECEIVED_REQUEST_TIME	6



void Vending_Machine_System_Init(void);
void Vending_Machine_ModemHandler(void);
void Vending_Machine_GetModemInfo(void);
void Vending_Machine_State_Ctrl(void);
void Vending_Machine_SystemTest(void);
void Vending_Machine_SystemConfig(void);
void Vending_Machine_NormalOp(void);
void Vending_Machine_Fota(void);
void Vending_Machine_SystemFault(void);
void Vending_Machine_PrepareAnnouncePacket(void);
void Vending_Machine_PrepareAckPacket(void);
void Vending_Machine_PrepareDispenseStatusPacket(void);
void Vending_Machine_PrepareStatusPacket(void);
void Vending_Machine_PrepareChannelChangeAckPacket(void);
void Vending_Machine_ConnectToMqttServer(void);
void Vending_Machine_StartDispense(void);
void vending_Machine_ErrorHandlerCallback(uint8_t *value);
void isTimerexpired(void);

#endif /* APP_H_ */
