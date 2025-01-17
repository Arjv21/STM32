/*******************************************************************************
* Title                 :   ATCMD_A7672_Commands
* Filename              :   ATCMD_A7672_Commands.c
* Author                :   Sijeo Philip
* Origin Date           :   09/05/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F3x
* Notes                 :   This module is used to issue a set of AT commands to the
* 							Module
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
*    Date      Version   Author         Description
*  11/05/2024   1.0.0    Sijeo Philip   Initial Release.
*
*******************************************************************************/
/** @file ATCMD_Core.c
 *  @brief This is the source file for Issuing AT Commands to the Module.
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "ATCMD_Queue_Mgmt.h"
#include "ATCMD_A7672_Commands.h"
#include "ATCMD_A7672_Callbacks.h"
#include "uart.h"
#include "gpio.h"
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/
static http_creds_t s_httpCredentials;

static uint8_t s_tempStr[6];
static uint8_t* s_tempPtr;
extern uint16_t g_mqtt_result;
/******************************************************************************
* Module Global/Extern Variable Definitions
*******************************************************************************/
extern atcmd_cmd_queue_t g_cmdQueue;
extern atcmd_urc_queue_struct_t g_urcEntityQueue;
extern http_response_t g_httpResponse;
extern uint8_t g_pbDone;
extern uint8_t g_simPresent;
extern gsm_gprs_status_t g_moduleStatus;
extern ssl_certs_t g_certificates;
mqtt_creds_t s_mqttCredentials;
uint32_t g_pingTimeout = 0;
char ntpHostName[20];
char dateTime[22];
modem_error_status_t g_modemErrorStatus;
/******************************************************************************
* Module Static Variable Definitions
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/

/******************************************************************************
* Function : ATCMD_A7672_Urc_Queue_Init()
*//**
* \b Description:
*
* This function is used to Initializing the URC Queue.
*
* PRE-CONDITION: The System Clock and Peripherals must be initalized and the Module
* 				Should be Powered On
* POST-CONDITION: The URC Queue is Intialized.
*
* @return		0 in case the Module not ready and 1 otherwise
*
* \b Example Example:
* @code
* 	uint8_t ret;
*	ret = ATCMD_A7672_Urc_Queue_Init();
* @endcode
*
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
void ATCMD_A7672_Urc_Queue_Init( void )
{
	atcmd_urc_queue_struct_t urcQueueStruct = {
				.urcQueue = { {"*ATREADY: 1", NULL },
						{"+CPIN:", Urc_Sim_Check_Handler },
						{"*ISIMAID:", NULL},
						{"SMS DONE", NULL },
						{"PB DONE", Urc_Module_Ready_Handler },
						{"SMS FULL", NULL},
						{"*COPN:", NULL},
						{"+CGEV:", NULL},
						{"+CMQTTRXSTART:", Urc_Module_Data_Handler},
						{"+CMQTTRXTOPIC:", Urc_Module_Data_Handler},
						{"+CMQTTRXPAYLOAD:", Urc_Module_Data_Handler},
						{"+CMQTTCONNLOST:", Urc_Module_Mqtt_Conn_Handler},
						{"+CMQTTNONET", Urc_Module_Mqtt_Conn_Handler},
				},
				.urcQueueCount =13,
		};
	ATCMD_Add_Urc_Queue(&urcQueueStruct);
}

/******************************************************************************
* Function : ATCMD_A7672_Module_Ready()
*//**
* \b Description:
*
* This function is used to Issue AT Commands to the Module for Checking if the Module
* is OK for further processing and Initializing the URC Queue
*
* PRE-CONDITION: The System Clock and Peripherals must be initalized and the Module
* 				Should be Powered On
* POST-CONDITION: The Module is check for readiness for further AT Command processing
*
* @return		0 in case the Module not ready and 1 otherwise
*
* \b Example Example:
* @code
* 	uint8_t ret;
*	ret = ATCMD_A7672_Module_Ready();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 13/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_Module_Ready( void )
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;
	atcmd_at_command_t atCommand[2] = {
			{"ATE0"ATCMD_CMD_END, NULL, 2, 120, ATCMD_CMD1_OK},
			{"AT"ATCMD_CMD_END, NULL, 0, 3, ATCMD_CMD2_OK},
	};

	ATCMD_AT_Command_Queue_Init();
	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 2);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD2_OK == g_cmdQueue.currentCmdStatus )
		{
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;

	return retVal;
}

/******************************************************************************
* Function : ATCMD_A7672_Get_NTP_HostName
*//**
* \b Description: Gets NTP Server IP Address from where modem date & time can be synchronized
* PRE-CONDITION	: The System clock and peripherals must be initialized, The Threads should
* 					be Initialized and Scheduler should be lauched, Modem should be connected
* 					to GPRS Network*
* POST-CONDITION: NTP IP Address will be updated in ntpHostName as a string
* @return		: 0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_Get_NTP_HostName()
* @endcode*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 13/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_Get_NTP_HostName( void )
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;

	atcmd_at_command_t atCommand[] = {

			{"AT+CTZU=1"ATCMD_CMD_END, NULL, 0, 120, ATCMD_CMD1_OK},
			{"AT+CNTP?"ATCMD_CMD_END, ATCMD_CNTP_Callback, 0, 120, ATCMD_CMD2_OK},
	};

	ATCMD_AT_Command_Queue_Init();

	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 2);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD2_OK == g_cmdQueue.currentCmdStatus )
		{
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;

	return retVal;
}

//uint8_t ATCMD_A7672_Get_NTP_HostName( void )
//{
//	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
//	uint8_t volatile retVal = 0;
//
//	atcmd_at_command_t atCommand[] = {
//
//			{"AT+CTZU=1"ATCMD_CMD_END, NULL, 0, 120, ATCMD_CMD1_OK},
//			{"AT+CNTP?"ATCMD_CMD_END, ATCMD_CNTP_Callback, 0, 120, ATCMD_CMD2_OK},
//	};
//
//	ATCMD_AT_Command_Queue_Init();
//
//	g_cmdQueue.cmdQueueWrite = 1;
//	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 2);
//	g_cmdQueue.cmdQueueWrite = 0;
//	if( ATCMD_QUEUE_OPER_OK == status )
//	{
//		while( 0 != g_cmdQueue.commandCount ){}
//		if( ATCMD_CMD2_OK == g_cmdQueue.currentCmdStatus )
//		{
//			retVal = 1;
//		}
//		else
//			retVal = 0;
//	}
//	else
//		retVal = 0;
//
//	return retVal;
//}

uint8_t ATCMD_A7672_DisableAutoTimezoneUpdate( void ){
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;

	atcmd_at_command_t atCommand[1] = {
			{"AT+CTZU=0"ATCMD_CMD_END, NULL, 0, 120, ATCMD_CMD1_OK},
	};
	ATCMD_AT_Command_Queue_Init();
	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 1);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status ){
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD1_OK == g_cmdQueue.currentCmdStatus ){
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;
	return retVal;
}

/******************************************************************************
* Function : ATCMD_A7672_UpdateModemDateTime
*//**
* \b Description: Issues AT Commands to update Modem clock from the NTP Server
* PRE-CONDITION	: The System clock and peripherals must be initialized, The Threads should
* 					be Initialized and Scheduler should be launched
* PRE-CONDITION	: ATCMD_A7672_Get_NTP_HostName() should be called atleast once before calling this function
* POST-CONDITION: MOdem date  & Time will be synchronized with the NTP Server
* @return		: 0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
* 	ATCMD_A7672_Get_NTP_HostName();
*	ATCMD_A7672_UpdateModemDateTime();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 13/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_Update_Modem_DateTime( void ){
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;
//	char i = 0;
	atcmd_at_command_t atCommand[1] = {
			{"AT+CNTP=\"216.239.35.8\",32", NULL, 0, 3, ATCMD_CMD1_OK},
	};

//	strcat((char*)atCommand[0].atCmd, ntpHostName);
	strcat((char*)atCommand[0].atCmd, ATCMD_CMD_END);
	ATCMD_AT_Command_Queue_Init();
	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 1);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status ){
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD1_OK == g_cmdQueue.currentCmdStatus ){
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;
	return retVal;
}

/******************************************************************************
* Function : ATCMD_A7672_Get_DateTime
*//**
* \b Description: Gets Date & Time from the modem and saves in dateTime as string.
* PRE-CONDITION	: The System clock and peripherals must be initialized, The Threads should
*					be Initialized and Scheduler should be launched. ATCMD_A7672_Get_NTP_HostName &
*					ATCMD_A7672_UpdateModemDateTime should be called atleast once before calling
*					this function
* POST-CONDITION: Modem date & time queried and responcse is stored in dateTime as a string.
* @return		: 0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
* 	ATCMD_A7672_Get_NTP_HostName();
*	ATCMD_A7672_UpdateModemDateTime();
*	ATCMD_A7672_Get_DateTime();
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 13/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_Get_DateTime( void ){
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;

	atcmd_at_command_t atCommand[1] = {
			{"AT+CCLK?"ATCMD_CMD_END, ATCMD_GetDateTime_Callback, 0, 120, ATCMD_CMD1_OK},
	};
	ATCMD_AT_Command_Queue_Init();
	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 1);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status ){
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD1_OK == g_cmdQueue.currentCmdStatus ){
			if(!strncmp(dateTime,"70/01/01,00",11))
				retVal = 0;
			else
				retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;
	return retVal;
}
uint8_t ATCMD_A7672_Reset_DateTime( void ){
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;

	atcmd_at_command_t atCommand[1] = {
			{"AT+CCLK=\"70/01/01,00:00:24+00\""ATCMD_CMD_END, NULL, 0, 120, ATCMD_CMD1_OK},
	};
	ATCMD_AT_Command_Queue_Init();
	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 1);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD1_OK == g_cmdQueue.currentCmdStatus )
		{
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;

	return retVal;
}
/******************************************************************************
* Function : ATCMD_A7672_Module_Check
*//**
* \b Description:
*
* This Function issues the AT Commands to check the SimCard, network and SMS Status
* for further application,
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be lauched
*
* POST-CONDITION: The AT Commands to check the SIM Card, Network and SMS Capability is
* Issued to the Modem
*
* @return		0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_Module_Check()
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 13/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_Module_Check( void )
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;

	atcmd_at_command_t atCommand[4] = {
			{"AT+CPIN?"ATCMD_CMD_END, ATCMD_Cpin_Status_Callback, 0, 3, ATCMD_CMD1_OK},
			{"AT+CREG?"ATCMD_CMD_END, ATCMD_Creg_Status_Callback, 0, 3, ATCMD_CMD2_OK},
			{"AT+CMGF=1"ATCMD_CMD_END, NULL, 0, 3, ATCMD_CMD3_OK},
			{"AT+CMGD=1,4"ATCMD_CMD_END, NULL, 0, 3, ATCMD_CMD4_OK},
	};

	ATCMD_AT_Command_Queue_Init();

	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 4);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD4_OK == g_cmdQueue.currentCmdStatus )
		{
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;

	return retVal;
}
/******************************************************************************
* Function : ATCMD_A7672_HTTP_Set_DevName
*//**
* \b Description:
*
* This Function is used to Set the Device Name used for HTTP Transactions.
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: The Device Name used for downloading certificate and Keys over HTTP
* is set..
*
* @param[in]		char*		Name of the Device
*
* @return		0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_HTTP_Set_DevName(const char* devName);
* @endcode
*
*@Note: This Function should be called before the Scheduler/Kernel is Launched.
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 15/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_HTTP_Set_DevName( const char* devName)
{
	if( devName == NULL )
	{
		return 0;
	}
	memset(s_httpCredentials.devName, '\0', sizeof(s_httpCredentials.devName));
	strcat((char*)s_httpCredentials.devName, devName);
//	strcat((char*)s_httpCredentials.devName, "\"");
	return 1;
}


/******************************************************************************
* Function : ATCMD_A7672_HTTP_Get_DevName
*//**
* \b Description:
*
* This Function is used to Get the Device Name Stored in the device..
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: The Device Name is returned
*
*
* @return		NULL or Address of the Device Name Stored for the HTTPS Transactions.;
*
* \b Example Example:
* @code
*	uint8_t *devName = ATCMD_A7672_HTTP_Get_DevName();
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 15/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t * ATCMD_A7672_HTTP_Get_DevName( void )
{
	return &s_httpCredentials.devName[0];
}
/******************************************************************************
* Function : ATCMD_A7672_HTTP_Set_CertURL
*//**
* \b Description:
*
* This Function is used to Set the URL used for downloading Certificate over HTTP.
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: The URL for downloading Certificates is Stored as credentials
* is set..
*
* @param[in]		const char*		URL used for Downloading Certificate
*
* @return		0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_HTTP_Set_CertURL(const char* certURL);
* @endcode
*
*@Note: This Function should be called before the Scheduler/Kernel is Launched.
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 15/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_HTTP_Set_CertURL( const char* certURL )
{
	if( NULL == certURL )
	{
		return 0;
	}
	memset(s_httpCredentials.certURL, '\0', sizeof(s_httpCredentials.certURL));
	strcpy((char*)s_httpCredentials.certURL, "\"");
	strcat((char*)s_httpCredentials.certURL, certURL);
	strcat((char*)s_httpCredentials.certURL, "\"");
	return 1;
}

/******************************************************************************
* Function : ATCMD_A7672_HTTP_Get_CertURL
*//**
* \b Description:
*
* This Function is used to Get the value of the URL stored to download the Certificate.
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: The Address of the URL used to download the Certificate is returned
*
*
* @return		NULL in case of failure and address of the Certificate URL in case it succeeded;
*
* \b Example Example:
* @code
*	uint8_t* certURL = ATCMD_A7672_HTTP_Get_CertURL();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 15/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t * ATCMD_A7672_HTTP_Get_CertURL( void )
{
	return &s_httpCredentials.certURL[0];
}

/******************************************************************************
* Function : ATCMD_A7672_HTTP_Set_PubKeyURL
*//**
* \b Description:
*
* This Function is used to Set the URL to download the Public Key
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: This URL is used to download the Public Key over HTTPS..
*
* @param[in]		const char*		URL to download the Public Key.
*
* @return		0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_HTTP_Set_PubKeyURL(const char* pubKeyURL);
* @endcode
*
*@Note: This Function should be called before the Scheduler/Kernel is Launched.
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 15/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_HTTP_Set_PubKeyURL( const char* pubKeyURL)
{
	if( NULL == pubKeyURL )
	{
		return 0;
	}
	memset(s_httpCredentials.pubKeyURL, '\0', sizeof(s_httpCredentials.pubKeyURL));
	strlcpy((char*)s_httpCredentials.pubKeyURL, "\"", 1);
	strncat((char*)s_httpCredentials.pubKeyURL, pubKeyURL, strlen(pubKeyURL));
	strlcat((char*)s_httpCredentials.pubKeyURL, "\"", 1);
	return 1;
}

/******************************************************************************
* Function : ATCMD_A7672_HTTP_Get_PubKeyURL
*//**
* \b Description:
*
* This Function is used to Get the value of the URL stored to download the Public Key.
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: The Address of the URL used to download the Public Key is returned
*
*
* @return		NULL in case of failure and address of the Public Key URL in case it succeeded;
*
* \b Example Example:
* @code
*	uint8_t* PubKeyURL = ATCMD_A7672_HTTP_Get_PubKeyURL();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 15/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/

uint8_t *ATCMD_A7672_HTTP_Get_PubKeyURL( void )
{
	return &s_httpCredentials.pubKeyURL[0];
}

/******************************************************************************
* Function : ATCMD_A7672_HTTP_Set_PrivKeyURL
*//**
* \b Description:
*
* This Function is used to Set the URL to download the Private Key
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: This URL is used to download the Private Key over HTTPS..
*
* @param[in]		const char*		URL to download the Private Key.
*
* @return		0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_HTTP_Set_PrivKeyURL(const char* privKeyURL);
* @endcode
*
*@Note: This Function should be called before the Scheduler/Kernel is Launched.
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 15/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_HTTP_Set_PrivKeyURL( const char* privKeyURL )
{
	if( NULL == privKeyURL )
	{
		return 0;
	}
	memset(s_httpCredentials.privKeyURL, '\0', sizeof(s_httpCredentials.privKeyURL));
	strcat((char*)s_httpCredentials.privKeyURL, "\"");
	strcat((char*)s_httpCredentials.privKeyURL, privKeyURL);
	strcat((char*)s_httpCredentials.privKeyURL, "\"");
	return 1;
}

/******************************************************************************
* Function : ATCMD_A7672_HTTP_Get_PrivKeyURL
*//**
* \b Description:
*
* This Function is used to Get the value of the URL stored to download the Private Key.
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: The Address of the URL used to download the Private Key is returned
*
*
* @return		NULL in case of failure and address of the Private Key URL in case it succeeded;
*
* \b Example Example:
* @code
*	uint8_t* privKeyURL = ATCMD_A7672_HTTP_Get_PrivKeyURL();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 15/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/

uint8_t *ATCMD_A7672_HTTP_Get_PrivKeyURL( void )
{
	return &s_httpCredentials.privKeyURL[0];
}
/******************************************************************************
* Function : ATCMD_A7672_HTTP_Set_DevProvURL
*//**
* \b Description:
*
* This Function is used to Set the URL for the Provisioning the device using the
* Device Name
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: This URL is used to Provision the Device over HTTPS..
*
* @param[in]		const char*		URL to Provision the Device..
*
* @return		0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_HTTP_Set_DevProvURL(const char* devProvURL);
* @endcode
*
*@Note: This Function should be called before the Scheduler/Kernel is Launched.
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 15/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_HTTP_Set_DevProvURL( const char* devProvURL )
{
	if( NULL == devProvURL )
	{
		return 0;
	}
	memset( s_httpCredentials.devProvURL, '\0', sizeof(s_httpCredentials.devProvURL));
	strcat((char*)s_httpCredentials.devProvURL, "\"");
	strcat((char*)s_httpCredentials.devProvURL, devProvURL);
	strcat((char*)s_httpCredentials.devProvURL, "\"");
	return 1;
}

/******************************************************************************
* Function : ATCMD_A7672_HTTP_Get_DevProvURL
*//**
* \b Description:
*
* This Function is used to Get the value of the URL stored to Provision the Device
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: The Address of the URL used to Provision the Device is returned
*
*
* @return		NULL in case of failure and address of the Device Provisioning URL in case it succeeded;
*
* \b Example Example:
* @code
*	uint8_t* certURL = ATCMD_A7672_HTTP_Get_DevProvURL();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 15/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t *ATCMD_A7672_HTTP_Get_DevProvURL( void )
{
	return &s_httpCredentials.devProvURL[0];
}


/******************************************************************************
* Function : ATCMD_A7672_HTTP_Set_RootCAURL
*//**
* \b Description:
*
* This Function is used to Set the URL for Downloading the RootCA Certificate
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: This URL is used to download the RootCA Certificate..
*
* @param[in]		const char*		URL to Download the RootCA Certificate..
*
* @return		0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_HTTP_Set_RootCAURL(const char* devProvURL);
* @endcode
*
*@Note: This Function should be called before the Scheduler/Kernel is Launched.
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 05/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_HTTP_Set_RootCAURL( const char* rootCAURL )
{
	if( NULL == rootCAURL )
	{
		return 0;
	}
	memset( s_httpCredentials.rootCaURL, '\0', sizeof(s_httpCredentials.rootCaURL));
	strcat((char*)s_httpCredentials.rootCaURL, "\"");
	strcat((char*)s_httpCredentials.rootCaURL, rootCAURL);
	strcat((char*)s_httpCredentials.rootCaURL, "\"");
	return 1;
}

/******************************************************************************
* Function : ATCMD_A7672_HTTP_Get_RootCAURL
*//**
* \b Description:
*
* This Function is used to Get the value of the URL stored to Download the RootCA
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: The Address of the URL used to Download the RootCA is stored.
*
*
* @return		NULL in case of failure and address of URL of RootCA;
*
* \b Example Example:
* @code
*	uint8_t* certURL = ATCMD_A7672_HTTP_Get_RootCAURL();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 05/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t *ATCMD_A7672_HTTP_Get_RootCAURL( void )
{
	return &s_httpCredentials.rootCaURL[0];
}


/******************************************************************************
* Function : ATCMD_A7672_MQTT_Set_URL
*//**
* \b Description:
*
* This Function is used to Set the URL for MQTT Server/Broker
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: This URL is used Connect to the MQTT Server.
*
* @param[in]		const char*		URL of the MQTT Server..
*
* @return		0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_MQTT_Set_URL(const char* devProvURL);
* @endcode
*
*@Note: This Function should be called before the Scheduler/Kernel is Launched.
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 07/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_MQTT_Set_URL(const char* mqttUrl)
{
	if (NULL == mqttUrl)
		return 0;
	memset(s_mqttCredentials.mqttURL,'\0', sizeof(s_mqttCredentials.mqttURL));
	strncpy((char*)s_mqttCredentials.mqttURL, mqttUrl, strlen(mqttUrl));
	return 1;
}

/******************************************************************************
* Function : ATCMD_A7672_MQTT_Get_URL
*//**
* \b Description:
*
* This Function is used to Get the value of the URL of the MQTT SERVER/BROKER
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: The Address of the URL used to Connect to the MQTT SERVER.
*
*
* @return		NULL in case of failure and address of URL of RootCA;
*
* \b Example Example:
* @code
*	uint8_t* URL = ATCMD_A7672_MQTT_Get_URL();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 05/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t* ATCMD_A7672_MQTT_Get_URL(void)
{
	return &s_mqttCredentials.mqttURL[0];
}


/******************************************************************************
* Function : ATCMD_A7672_MQTT_Set_Port
*//**
* \b Description:
*
* This Function is used to Set the Port for MQTT Server/Broker
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: This URL is used Connect to the MQTT Server.
*
* @param[in]		const char*		Port of the MQTT Server..
*
* @return		0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_MQTT_Set_Port(const char* mqttPort);
* @endcode
*
*@Note: This Function should be called before the Scheduler/Kernel is Launched.
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 07/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_MQTT_Set_Port(const char* mqttPort)
{
	if (NULL == mqttPort)
		return 0;
	memset(s_mqttCredentials.mqttPort,'\0', sizeof(s_mqttCredentials.mqttPort));
	strncpy((char*)s_mqttCredentials.mqttPort, mqttPort, strlen(mqttPort));
	return 1;
}

/******************************************************************************
* Function : ATCMD_A7672_MQTT_Get_Port
*//**
* \b Description:
*
* This Function is used to Get the value of the Port of the MQTT SERVER/BROKER
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: The Address of the Port used to Connect to the MQTT SERVER.
*
*
* @return		NULL in case of failure and address Port Number used for MQTT Server
*
* \b Example Example:
* @code
*	uint8_t* URL = ATCMD_A7672_MQTT_Get_Port();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 05/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t* ATCMD_A7672_MQTT_Get_Port(void)
{
	return &s_mqttCredentials.mqttPort[0];
}

/******************************************************************************
* Function : ATCMD_A7672_MQTT_Set_SubTopic
*//**
* \b Description:
*
* This Function is used to Set the Subscribe Topic for MQTT Transactions
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: This Subscribe tOpic is stored and used for Subscribing to MQTT Server.
*
* @param[in]		const char*		MQTT Subscribe Topic
*
* @return		0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_MQTT_Set_SubTopic(const char* mqttSubTopic);
* @endcode
*
*@Note: This Function should be called before the Scheduler/Kernel is Launched.
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 07/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_MQTT_Set_SubTopic(const char* mqttSubTopic)
{
	if (NULL == mqttSubTopic)
		return 0;
	memset(s_mqttCredentials.mqttSubTopic,'\0', sizeof(s_mqttCredentials.mqttSubTopic));
	strncpy((char*)s_mqttCredentials.mqttSubTopic, mqttSubTopic, strlen(mqttSubTopic));
	return 1;
}

/******************************************************************************
* Function : ATCMD_A7672_MQTT_Get_SubTopic
*//**
* \b Description:
*
* This Function is used to Get the Stored Subscribe Topic
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: The Subscribe Topic for the MQTT SERVER is retrieved.
*
*
* @return		NULL in case of failure and address of the Subscribe Topic Stored.
*
* \b Example Example:
* @code
*	uint8_t* URL = ATCMD_A7672_MQTT_Get_SubTopic();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 05/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t* ATCMD_A7672_MQTT_Get_SubTopic( void )
{
	return &s_mqttCredentials.mqttSubTopic[0];
}

/******************************************************************************
* Function : ATCMD_A7672_MQTT_Set_PubTopic
*//**
* \b Description:
*
* This Function is used to Store the Publish Topic for the MQTT Transactions
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: This URL is used Connect to the MQTT Server.
*
* @param[in]		const char*		Publish Topic for the MQTT Server Transactions.
*
* @return		0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_MQTT_Set_PubTopic(const char* mqttPubTopic);
* @endcode
*
*@Note: This Function should be called before the Scheduler/Kernel is Launched.
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 07/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_MQTT_Set_PubTopic(const char* mqttPubTopic)
{
	if (NULL == mqttPubTopic)
		return 0;
	memset(s_mqttCredentials.mqttPubTopic,'\0', sizeof(s_mqttCredentials.mqttPubTopic));
	strncpy((char*)s_mqttCredentials.mqttPubTopic, mqttPubTopic, strlen(mqttPubTopic));
	return 1;
}

/******************************************************************************
* Function : ATCMD_A7672_MQTT_Get_PubTopic
*//**
* \b Description:
*
* This Function is used to Get the Stored Publish Topic
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: The Publish Topic for the MQTT SERVER is retrieved.
*
*
* @return		NULL in case of failure and address of the Publish Topic Stored.
*
* \b Example Example:
* @code
*	uint8_t* URL = ATCMD_A7672_MQTT_Get_PubTopic();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 05/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t* ATCMD_A7672_MQTT_Get_PubTopic( void )
{
	return &s_mqttCredentials.mqttPubTopic[0];
}

/******************************************************************************
* Function : ATCMD_A7672_MQTT_Get_Connection_Status
*//**
* \b Description:
*
* This Function is used to Get Connection Status to the MQTT Servers
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: The Connection STatus to the MQTT SERVER is retrieved.
*
*
* @return		Returns the Connection STatus of the MQTT Server.
*
* \b Example Example:
* @code
*	mqtt_conn_status_t status = ATCMD_A7672_MQTT_Get_Connection_Status();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 05/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
mqtt_conn_status_t ATCMD_A7672_MQTT_Get_Connection_Status( void )
{
	return s_mqttCredentials.connectionStatus;
}

/******************************************************************************
 * Function : ATCMD_A7672_MQTT_Set_Connection_Status
 *//**
 * \b Description:
 *
 * This Function is used to Set Connection Status to the MQTT Servers
 *
 * PRE-CONDITION: The System clock and peripherals must be initialized,
 *
 * POST-CONDITION: The Connection STatus to the MQTT SERVER is set.
 *
 * @Param[in]	: mqtt_conn_status_t (connection status to be set)
 * @return		None.
 *
 * \b Example Example:
 * @code
 *	mqtt_conn_status_t status = ATCMD_A7672_MQTT_Set_Connection_Status();
 * @endcode
 *
 *
 * <br><b> - HISTORY OF CHANGES - </b>
 *
 * <table align="left" style="width:800px">
 * <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
 * <tr><td> 17/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
 * </table><br><br>
 * <hr>
 *
 *******************************************************************************/
 void ATCMD_A7672_MQTT_Set_Connection_Status( mqtt_conn_status_t conn_status)
 {
 	s_mqttCredentials.connectionStatus = conn_status;
 }

/******************************************************************************
* Function : ATCMD_A7672_Register_Message_Callback
*//**
* \b Description:
*
* This Function is used to Get Callback in case of message from th server.
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: The message from server is retrieved.
*
*
* @return		None.
*
* \b Example Example:
* @code
*	mqtt_conn_status_t status = ATCMD_A7672_MQTT_Get_Connection_Status();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 05/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void ATCMD_A7672_Register_Message_Callback(msg_handler_t msg_handler)
{
	s_mqttCredentials.msg_handler = msg_handler;
}

/******************************************************************************
* Function : ATCMD_A7672_Resgister_Status_Send_Callback
*//**
* \b Description:
*
* This Function is used to Get Callback for ping timeout.
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: The callback is called on timeout
*
*@param[in]		status_timeeout_handler_t is address of function of signature void func (void);
*@param[in]		uint32_t timeout in mins
*
* @return		None.
*
* \b Example Example:
* @code
*	ATCMD_A7672_Resgister_Status_Send_Callback(func, 15);
* @endcode
*
*@Note: No loops or blocking codes should be used in this callback...
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 09/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void ATCMD_A7672_Resgister_Status_Send_Callback(status_timeout_handler_t timeout_handler, uint32_t timeout_mins)
{
	s_mqttCredentials.timeout_handler = timeout_handler;
	s_mqttCredentials.status_timeout = timeout_mins*60;
	g_pingTimeout = s_mqttCredentials.status_timeout;
}

/******************************************************************************
* Function : ATCMD_A7672_Register_Error_Callback
*//**
* \b Description:
*
* This Function is used to Get Callback when the APIs fail continuously.
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: The callback is called on continuous error count above the set threshold value
*
*@param[in]		modem_error_handler_t is address of function of signature void func (uint8_t*);
*@param[in]		uint8_t No of errors allowed before getting the callback.
*
* @return		None.
*
* \b Example Example:
* @code
*	ATCMD_A7672_Resgister_Error_Callback(func, 5);
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 17/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void ATCMD_A7672_Register_Error_Callback(modem_error_handler_t error_handler, uint8_t errorCount)
{
	g_modemErrorStatus.errorCountThreshold = errorCount;
	g_modemErrorStatus.modem_error_handler = error_handler;
}
/******************************************************************************
* Function : ATCMD_A7672_Is_Sim_Present
*//**
* \b Description:
*
* This Function is used to Get the status of Sim Card
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
* PRE-CONDITION: The Scheduler and the OS should be Launched.
*
* POST-CONDITION: The Status of the SIM Card is returned
*
*
* @return		0 in case of the SIM is absent 1 if Present
*
* \b Example Example:
* @code
*	uint8_t ret = ATCMD_A7672_Is_Sim_Present();
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
uint8_t ATCMD_A7672_Is_Sim_Present (void)
{
	return g_simPresent;
}

/******************************************************************************
* Function : ATCMD_A7672_Is_Phonebook_Done
*//**
* \b Description:
*
* This Function is used to Get the status of Module PhoneBook readiness
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
* PRE-CONDITION: The Scheduler and the OS should be Launched.
*
* POST-CONDITION: The Status of the PB Done
*
*
* @return		0 in case of the PB done is not ready else 1
*
* \b Example Example:
* @code
*	uint8_t ret = ATCMD_A7672_Is_Phonebook_Done();
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
uint8_t ATCMD_A7672_Is_Phonebook_Done(void)
{
	return g_pbDone;
}

/******************************************************************************
* Function : ATCMD_A7672_Module_On
*//**
* \b Description:
*
* This Function is used to Switch on the Module over Power Key Signal
*
* PRE-CONDITION: The System clock and peripherals must be initialized,
*
* POST-CONDITION: The Module is Turned ON
*
*
* @return		0 in case of the Module is Off else if On it will return 1
*
* \b Example Example:
* @code
*	uint8_t ret = ATCMD_A7672_Module_On();
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

uint8_t ATCMD_A7672_Module_On(void)
{
	Gpio_Set(A7672_POWER_GPIO, A7672_POWER_PIN);
	if(Gpio_Read(A7672_POWER_STATUS_GPIO, A7672_POWER_STATUS_PIN)==1)
	{
		Gpio_Clear(A7672_POWER_GPIO, A7672_POWER_PIN);
		return 1;
	}
	else
		return 0;
}

/******************************************************************************
* Function : ATCMD_A7672_Provision_Device
*//**
* \b Description:
*
* This Function is used to provision device  using HTTPS from Server
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: The AT Commands to Provision the Device over HTTPS is executed.
*
*
* @return		0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_Provision_Device()
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 16/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_Provision_Device( void )
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;

	atcmd_at_command_t atCommand[7] = {
			{"AT+HTTPINIT"ATCMD_CMD_END, NULL, 0, 120, ATCMD_CMD1_OK},
//			{"AT+HTTPPARA=\"SSLCFG\",0"ATCMD_CMD_END, NULL, 0, 120, ATCMD_CMD2_OK},
			{"AT+HTTPPARA=\"URL\",", NULL, 0, 120, ATCMD_CMD2_OK},
			{"AT+HTTPPARA=\"USERDATA\",", NULL, 0, 120, ATCMD_CMD3_OK},
			{"AT+HTTPPARA=\"USERDATA\",", NULL, 0, 120, ATCMD_CMD4_OK},
			{"AT+HTTPPARA=\"USERDATA\",", NULL, 0, 120, ATCMD_CMD5_OK},
			{"AT+HTTPACTION=1"ATCMD_CMD_END, ATCMD_HttpAction_Get_Callback,0, 120, ATCMD_CMD6_OK},
	};
	strcat((char*)atCommand[1].atCmd, (char*)s_httpCredentials.devProvURL);
	strcat((char*)atCommand[1].atCmd, ATCMD_CMD_END);
	strcat((char*)atCommand[2].atCmd, "\"ThingName: ");
	strcat((char*)atCommand[2].atCmd, (char*)s_httpCredentials.devName);
	strcat((char*)atCommand[2].atCmd, ATCMD_CMD_END);
	strcat((char*)atCommand[3].atCmd, "\"ThingTypeName: IoT-Devices\"\r");
	strcat((char*)atCommand[4].atCmd, "\"ThingGroupName: Home-Devices\"\r");

	ATCMD_AT_Command_Queue_Init();

	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 6);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD6_OK == g_cmdQueue.currentCmdStatus )
		{
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;

	return retVal;
}

/******************************************************************************
* Function : ATCMD_A7672_HTTP_Get_Response_Code
*//**
* \b Description:
*
* This Function is returns the current Response Code of the HTTP Transaction
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: Response code of recent unterminated HTTP Transaction is returned
* 				  in unsigned integer 16 bit format..
*
*
* @return		0 in case of failure else response code in uint16_t;
*
* \b Example Example:
* @code
*	uint16_t responseCode = ATCMD_A7672_HTTP_Get_Respose_Code()
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 16/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint16_t ATCMD_A7672_HTTP_Get_Response_Code( void )
{
	return g_httpResponse.responseCode;
}

/******************************************************************************
* Function : ATCMD_A7672_HTTP_Get_Bytes_Rcvd
*//**
* \b Description:
*
* This Function is returns the current Bytes Received of the HTTP Transaction
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: Byted Received of recent unterminated HTTP Transaction is returned
* 				  in unsigned integer 16 bit format..
*
*
* @return		0 in case of failure else Bytes Read in uint16_t;
*
* \b Example Example:
* @code
*	uint16_t bytesRecvd = ATCMD_A7672_HTTP_Get_Bytes_Recvd()
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 16/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint16_t ATCMD_A7672_HTTP_Get_Bytes_Rcvd ( void )
{
	return g_httpResponse.bytesToRead;
}

/******************************************************************************
* Function : ATCMD_A7672_HTTP_Terminate
*//**
* \b Description:
*
* This Function is used to terminate the unterminated HTTP Session;
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: The AT Commands to Terminate HTTP session is issured to the module..
*
*
* @return		0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_HTTP_Terminate()
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 17/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_HTTP_Terminate( void )
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;

	atcmd_at_command_t atCommand[1] = {
			{"AT+HTTPTERM"ATCMD_CMD_END, NULL, 0, 120, ATCMD_CMD1_OK},
		};

	ATCMD_AT_Command_Queue_Init();

	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 1);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD1_OK == g_cmdQueue.currentCmdStatus )
		{
			memset(g_httpResponse.httpResponseBytes, '\0', sizeof(g_httpResponse.httpResponseBytes));
			memset(g_httpResponse.httpResponseCode, '\0', sizeof(g_httpResponse.httpResponseCode));
			g_httpResponse.bytesToRead = 0;
			g_httpResponse.responseCode = 0;
			g_httpResponse.fileType = NO_FILE;
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;
	return retVal;
}


/******************************************************************************
* Function : ATCMD_A7672_HTTP_Read
*//**
* \b Description:
*
* This Function is used to Read data from the unterminated HTTP Session;
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: The AT Commands Read data over unterminated HTTP session..
*
*
* @return		NULL in case of failure and address of the buffer in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_HTTP_Read()
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 17/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t *ATCMD_A7672_HTTP_Read(void)
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;

	atcmd_at_command_t atCommand[1] = {
			{"AT+HTTPREAD=0,", ATCMD_HttpRead_Callback, 0, 120, ATCMD_CMD1_OK},
		};
	if ((g_httpResponse.bytesToRead == 0) || (g_httpResponse.responseCode != 200))
	{
		return NULL;
	}
	strcat((char*)atCommand[0].atCmd, (char*)&g_httpResponse.httpResponseBytes[0]);
	strcat((char*)atCommand[0].atCmd, ATCMD_CMD_END);
	ATCMD_AT_Command_Queue_Init();
	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 1);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD1_OK == g_cmdQueue.currentCmdStatus )
		{

		 return Uart_Get_Transmit_Buffer();
		}
		else
			return NULL;
	}
	else
		return NULL;

}

/******************************************************************************
* Function : ATCMD_A7672_Download_Certificate
*//**
* \b Description:
*
* This Function is used to Download Certificate  using HTTPS from Server
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: The AT Commands to Download Certificate over HTTPS is executed.
*
*
* @return		0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_Download_Certificate()
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 17/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_Download_Certificate( void )
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;

	atcmd_at_command_t atCommand[8] = {
			{"AT+HTTPINIT"ATCMD_CMD_END, NULL, 0, 120, ATCMD_CMD1_OK},
			{"AT+HTTPPARA=\"SSLCFG\",0"ATCMD_CMD_END, NULL, 0, 120, ATCMD_CMD2_OK},
			{"AT+HTTPPARA=\"URL\",", NULL, 0, 120, ATCMD_CMD3_OK},
			{"AT+HTTPPARA=\"USERDATA\",", NULL, 0, 120, ATCMD_CMD4_OK},
			{"AT+HTTPPARA=\"USERDATA\",", NULL, 0, 120, ATCMD_CMD5_OK},
			{"AT+HTTPPARA=\"USERDATA\",", NULL, 0, 120, ATCMD_CMD6_OK},
			{"AT+HTTPACTION=1"ATCMD_CMD_END, ATCMD_HttpAction_Get_Callback,0, 120, ATCMD_CMD7_OK},
	};
	strcat((char*)atCommand[2].atCmd, (char*)s_httpCredentials.certURL);
	strcat((char*)atCommand[2].atCmd, ATCMD_CMD_END);
	strcat((char*)atCommand[3].atCmd, "\"Bucket_name: rcs-iot-device-provisioning\"\r");
	strcat((char*)atCommand[4].atCmd, "\"Key_name: ");
	strcat((char*)atCommand[4].atCmd, (char*)s_httpCredentials.devName);
	strcat((char*)atCommand[4].atCmd, ATCMD_CMD_END);
	strcat((char*)atCommand[5].atCmd, "\"Certificate_name: certificate.pem\"\r");

	ATCMD_AT_Command_Queue_Init();

	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 7);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD7_OK == g_cmdQueue.currentCmdStatus )
		{
			g_httpResponse.fileType = CERTIFICATE_FILE;
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;

	return retVal;
}

/******************************************************************************
* Function : ATCMD_A7672_Download_Private_Key
*//**
* \b Description:
*
* This Function is used to Download Private Key using HTTPS from Server
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: The AT Commands to Download Private Key over HTTPS is executed.
*
*
* @return		0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_Download_Private_Key()
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 17/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_Download_Private_Key( void )
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;

	atcmd_at_command_t atCommand[8] = {
			{"AT+HTTPINIT"ATCMD_CMD_END, NULL, 0, 120, ATCMD_CMD1_OK},
			{"AT+HTTPPARA=\"SSLCFG\",0"ATCMD_CMD_END, NULL, 0, 120, ATCMD_CMD2_OK},
			{"AT+HTTPPARA=\"URL\",", NULL, 0, 120, ATCMD_CMD3_OK},
			{"AT+HTTPPARA=\"USERDATA\",", NULL, 0, 120, ATCMD_CMD4_OK},
			{"AT+HTTPPARA=\"USERDATA\",", NULL, 0, 120, ATCMD_CMD5_OK},
			{"AT+HTTPPARA=\"USERDATA\",", NULL, 0, 120, ATCMD_CMD6_OK},
			{"AT+HTTPACTION=1"ATCMD_CMD_END, ATCMD_HttpAction_Get_Callback,0, 120, ATCMD_CMD7_OK},
	};
	strcat((char*)atCommand[2].atCmd, (char*)s_httpCredentials.privKeyURL);
	strcat((char*)atCommand[2].atCmd, ATCMD_CMD_END);
	strcat((char*)atCommand[3].atCmd, "\"Bucket_name: rcs-iot-device-provisioning\"\r");
	strcat((char*)atCommand[4].atCmd, "\"Key_name: ");
	strcat((char*)atCommand[4].atCmd, (char*)s_httpCredentials.devName);
	strcat((char*)atCommand[4].atCmd, ATCMD_CMD_END);
	strcat((char*)atCommand[5].atCmd, "\"privatekey_name: private.pem.key\"\r");

	ATCMD_AT_Command_Queue_Init();

	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 7);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD7_OK == g_cmdQueue.currentCmdStatus )
		{
			g_httpResponse.fileType = PRIVATEKEY_FILE;
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;

	return retVal;
}

/******************************************************************************
* Function : ATCMD_A7672_Download_Public_Key
*//**
* \b Description:
*
* This Function is used to Download Public Key using HTTPS from Server
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: The AT Commands to Download Public Key over HTTPS is executed.
*
*
* @return		0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_Download_Public_Key()
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 17/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_Download_Public_Key( void )
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;

	atcmd_at_command_t atCommand[8] = {
			{"AT+HTTPINIT"ATCMD_CMD_END, NULL, 0, 120, ATCMD_CMD1_OK},
			{"AT+HTTPPARA=\"SSLCFG\",0"ATCMD_CMD_END, NULL, 0, 120, ATCMD_CMD2_OK},
			{"AT+HTTPPARA=\"URL\",", NULL, 0, 120, ATCMD_CMD3_OK},
			{"AT+HTTPPARA=\"USERDATA\",", NULL, 0, 120, ATCMD_CMD4_OK},
			{"AT+HTTPPARA=\"USERDATA\",", NULL, 0, 120, ATCMD_CMD5_OK},
			{"AT+HTTPPARA=\"USERDATA\",", NULL, 0, 120, ATCMD_CMD6_OK},
			{"AT+HTTPACTION=1"ATCMD_CMD_END, ATCMD_HttpAction_Get_Callback,0, 120, ATCMD_CMD7_OK},
	};
	strcat((char*)atCommand[2].atCmd, (char*)s_httpCredentials.pubKeyURL);
	strcat((char*)atCommand[2].atCmd, ATCMD_CMD_END);
	strcat((char*)atCommand[3].atCmd, "\"Bucket_name: rcs-iot-device-provisioning\"\r");
	strcat((char*)atCommand[4].atCmd, "\"Key_name: ");
	strcat((char*)atCommand[4].atCmd, (char*)s_httpCredentials.devName);
	strcat((char*)atCommand[4].atCmd, ATCMD_CMD_END);
	strcat((char*)atCommand[5].atCmd, "\"publickey_name: public.key.pem\"\r");

	ATCMD_AT_Command_Queue_Init();

	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 7);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD7_OK == g_cmdQueue.currentCmdStatus )
		{
			g_httpResponse.fileType = PUBLICKEY_FILE;
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;

	return retVal;
}

/******************************************************************************
* Function : ATCMD_A7672_Download_Root_Ca
*//**
* \b Description:
*
* This Function is used to Download Root CA Certificate using HTTPS from Server
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: The AT Commands to Download Root CA Certificate over HTTPS is executed.
*
*
* @return		0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_Download_Root_Ca()
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 05/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_Download_Root_Ca( void )
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;

	atcmd_at_command_t atCommand[8] = {
			{"AT+HTTPINIT"ATCMD_CMD_END, NULL, 0, 120, ATCMD_CMD1_OK},
			{"AT+HTTPPARA=\"SSLCFG\",0"ATCMD_CMD_END, NULL, 0, 120, ATCMD_CMD2_OK},
			{"AT+HTTPPARA=\"URL\",", NULL, 0, 120, ATCMD_CMD3_OK},
			{"AT+HTTPPARA=\"USERDATA\",", NULL, 0, 120, ATCMD_CMD4_OK},
			{"AT+HTTPPARA=\"USERDATA\",", NULL, 0, 120, ATCMD_CMD5_OK},
			{"AT+HTTPPARA=\"USERDATA\",", NULL, 0, 120, ATCMD_CMD6_OK},
			{"AT+HTTPACTION=1"ATCMD_CMD_END, ATCMD_HttpAction_Get_Callback,0, 120, ATCMD_CMD7_OK},
	};
	strcat((char*)atCommand[2].atCmd, (char*)s_httpCredentials.pubKeyURL);
	strcat((char*)atCommand[2].atCmd, ATCMD_CMD_END);
	strcat((char*)atCommand[3].atCmd, "\"Bucket_name: rcs-iot-device-provisioning\"\r");
	strcat((char*)atCommand[4].atCmd, "\"Key_name: ");
	strcat((char*)atCommand[4].atCmd, (char*)s_httpCredentials.devName);
	strcat((char*)atCommand[4].atCmd, ATCMD_CMD_END);
	strcat((char*)atCommand[5].atCmd, "\"publickey_name: public.key.pem\"\r");


	ATCMD_AT_Command_Queue_Init();

	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 4);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD4_OK == g_cmdQueue.currentCmdStatus )
		{
			g_httpResponse.fileType = ROOTCA_FILE;
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;

	return retVal;
}

/******************************************************************************
* Function : ATCMD_A7672_CertKey_Module_Download
*//**
* \b Description:
*
* This Function is used to Download Certificate/KEY to the Module
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: The AT Commands to Download Certificate to Module .
*
*
* @return		0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_CertKey_Module_Download()
* @endcode
*
* @Note: This API needs to be used before terminating the HTTP Connections.
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 17/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_CertKey_Module_Download( void )
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;

	atcmd_at_command_t atCommand[1] = {
			{"AT+CCERTDOWN=", ATCMD_CertDownload_Callback, 0, 120, ATCMD_CMD1_OK},
			};
	if ((g_httpResponse.bytesToRead == 0) || (g_httpResponse.responseCode != 200) || (g_httpResponse.fileType == NO_FILE))
	{
		return 0;
	}
	switch(g_httpResponse.fileType)
	{
	case CERTIFICATE_FILE:
		strcat((char*)atCommand[0].atCmd, "\"certificate.pem\",");
		strcat((char*)atCommand[0].atCmd, (char*)g_httpResponse.httpResponseBytes);
		break;
	case PRIVATEKEY_FILE:
		strcat((char*)atCommand[0].atCmd, "\"privatekey.pem\",");
		strcat((char*)atCommand[0].atCmd, (char*)g_httpResponse.httpResponseBytes);
		break;
	case PUBLICKEY_FILE:
		strcat((char*)atCommand[0].atCmd, "\"publickey.pem\",");
		strcat((char*)atCommand[0].atCmd, (char*)g_httpResponse.httpResponseBytes);
		break;
	case ROOTCA_FILE:
		strcat((char*)atCommand[0].atCmd, "\"RootCA.pem\"");
		strcat((char*)atCommand[0].atCmd, (char*)g_httpResponse.httpResponseBytes);
		break;
	default:
		break;
	}
	strcat((char*)atCommand[0].atCmd, ATCMD_CMD_END);
	ATCMD_AT_Command_Queue_Init();
	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 1);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD1_OK == g_cmdQueue.currentCmdStatus )
		{
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;

	return retVal;
}

/******************************************************************************
* Function : ATCMD_A7672_Get_IMEI_Number
*//**
* \b Description:
*
* This Function is used to the IMEI Number of the Module
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: The AT Commands to Download IMEI number of Module is issued. .
*
*
* @return	NULL in case of Failure else returns struct storing IMEI, Model and other paramters..
*
* \b Example Example:
* @code
*	ATCMD_A7672_Get_IMEI_Number()
* @endcode
*
*@Note: On Success the Value gets stored in the http_creds struct and have to be retrieved by
*@Note: Calling the helper function ATCMD_A7672_HTTP_Get_DevName();
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 20/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
gsm_gprs_status_t* ATCMD_A7672_Get_IMEI_Number( void )
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
		uint8_t volatile retVal = 0;

		atcmd_at_command_t atCommand[1] = {
				{"ATI"ATCMD_CMD_END, ATCMD_Get_IMEI_Callback, 0, 120, ATCMD_CMD1_OK},
			};

		ATCMD_AT_Command_Queue_Init();

		g_cmdQueue.cmdQueueWrite = 1;
		status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 1);
		g_cmdQueue.cmdQueueWrite = 0;
		if( ATCMD_QUEUE_OPER_OK == status )
		{
			while( 0 != g_cmdQueue.commandCount ){}
			if( ATCMD_CMD1_OK == g_cmdQueue.currentCmdStatus )
			{
				retVal = 1;
			}
			else
				retVal = 0;
		}
		else
			retVal = 0;
		if( retVal ){
			return  &g_moduleStatus;

		}
		else
			return NULL;
}

/******************************************************************************
* Function : ATCMD_A7672_Get_Signal_Strength
*//**
* \b Description:
*
* This Function is used to Get the Signal Strength of  the Module
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: The AT Commands to update the Signal Strength Value. .
*
*
* @return		0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_Get_Signal_Strength()
* @end code
*
*@Note: This updates the value in the Global Variable and needs to be fetched for
*			for further use using Helper API
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
uint16_t ATCMD_A7672_Get_Signal_Strength(void )
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;

	atcmd_at_command_t atCommand[1] = {
			{"AT+CSQ"ATCMD_CMD_END, ATCMD_Get_Signal_Strength_Callback, 0, 9, ATCMD_CMD1_OK},
		};
	ATCMD_AT_Command_Queue_Init();

	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 1);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD1_OK == g_cmdQueue.currentCmdStatus )
		{
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;

	if(retVal == 1)
	{
		return g_moduleStatus.sigStrength;
	}

	return 0;
}

/******************************************************************************
* Function : ATCMD_A7672_Module_Off
* *//**
* \b Description:
*
* This Function is used to Switch off the Module
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: The AT Commands to Switch off the Module is executed. .
*
*
* @return		0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_Module_Off()
* @end code
*
*@Note: This updates the value in the Global Variable and needs to be fetched for
*			for further use using Helper API
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 04/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_Module_Off(void)
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;

	atcmd_at_command_t atCommand[1] = {
			{"AT+CPOF"ATCMD_CMD_END, NULL, 0, 9, ATCMD_CMD1_OK},
		};
	ATCMD_AT_Command_Queue_Init();
	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 1);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD1_OK == g_cmdQueue.currentCmdStatus )
		{
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;
	return retVal;
}


/******************************************************************************
* Function : ATCMD_A7672_Module_Rest
* *//**
* \b Description:
*
* This Function is used to Switch Reset the Module
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: The AT Commands to Reset the Module is executed. .
*
*
* @return		0 in case of failure and 1 in case it succeeded;
*
* \b Example Example:
* @code
*	ATCMD_A7672_Module_Off()
* @end code
*
*@Note: This updates the value in the Global Variable and needs to be fetched for
*			for further use using Helper API
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 04/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_Module_Reset(void)
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;

	atcmd_at_command_t atCommand[1] = {
			{"AT+CRESET"ATCMD_CMD_END, NULL, 0, 9, ATCMD_CMD1_OK},
		};
	ATCMD_AT_Command_Queue_Init();
	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 1);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD1_OK == g_cmdQueue.currentCmdStatus )
		{
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;
	return retVal;
}


/******************************************************************************
* Function : ATCMD_A7672_Check_Module_Certificates
* *//**
* \b Description:
*
* This Function is used to Check the List of Certificates Downloaded to the Module
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: The AT Command AT+CCERTLIST is executed..
*
*
* @return		NULL in case of Failure or Returns struct ssl_certs_t address in case of Success.
* 				ssl_certs_t contains the list of certificates and Number of certificates.
*
* \b Example Example:
* @code
*	ssl_cert_t* certs = ATCMD_A7672_Check_Module_Certificates()
* @end code
*
*@Note: The Count Variable in the structure can be used to check the number of certificates
*@Note: Downloaded to the module
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 05/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
ssl_certs_t* ATCMD_A7672_Check_Module_Certificates( void )
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;

	atcmd_at_command_t atCommand[1] = {
			{"AT+CCERTLIST"ATCMD_CMD_END, ATCMD_Check_Module_Certs_Callback, 0, 120, ATCMD_CMD1_OK},
		};
	ATCMD_AT_Command_Queue_Init();
	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 1);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD1_OK == g_cmdQueue.currentCmdStatus )
		{
			return &g_certificates;
		}
		else
			return NULL;
	}
	else
		return NULL;

}


/******************************************************************************
* Function : ATCMD_A7672_Delete_Certificate
* *//**
* \b Description:
*
* This Function is used to Delete the Certificates in the Module.
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: The AT Command AT+CCDELE is executed..
*
* @param[in] 	uint8_t* Name of the Certificate to be deleted.
* @return		1 in case of Success or 0 otherwise
*
* \b Example Example:
* @code
*	uint8_t ret = ATCMD_A7672_Delete_Certificate("certificate.pem")
* @end code
*
*@Note: This API should be used with the values stored in the ssl_certs_t structure as the
*@Note: variable stores the name of the files along with quotes else it will fail i.e.
*@Note: This API should be used only after calling ATCMD_A7672_Check_Module_Certifactes()
*@Note: Successfully.
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 05/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_Delete_Certificate (uint8_t* certName)
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;

	atcmd_at_command_t atCommand[1] = {
			{"AT+CCERTDELE=", NULL, 0, 120, ATCMD_CMD1_OK},
		};
	strlcat((char*)&atCommand[0], (char*)certName, strlen((char*)certName));
	strlcat((char*)&atCommand[0], ATCMD_CMD_END, 2);

	ATCMD_AT_Command_Queue_Init();
	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 1);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD1_OK == g_cmdQueue.currentCmdStatus )
		{
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;
	return retVal;
}

/******************************************************************************
* Function : ATCMD_A7672_Is_Module_Certificate
* *//**
* \b Description:
*
* This Function is used to Check the Certificate name is in the Downloaded Certificate
* List.
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: The Will check the certificates from the list.
*
* @param[in] 	uint8_t* certName Name of the Certificate to be checked..
* @return		1 in case of Success or 0 otherwise
*
* \b Example Example:
* @code
*	uint8_t ret = ATCMD_A7672_Is_Module_Certificate("certificate.pem")
* @end code
*
*@Note: This API should be used with the values stored in the ssl_certs_t structure as the
*@Note: variable stores the name of the files along with quotes else it will fail i.e.
*@Note: This API should be used only after calling ATCMD_A7672_Check_Module_Certifactes()
*@Note: Successfully.
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 05/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_Is_Module_Certificate(const char* certName)
{
	volatile int i = 0;

	for (i = 0; i < g_certificates.certCount; i ++)
	{
		if(!strncmp((char*)&g_certificates.certs[i][0], certName, strlen(certName)))
		{
			return 1;
		}
	}
	return 0;
}

/******************************************************************************
* Function : ATCMD_A7672_Sslconf_Init
* *//**
* \b Description:
*
* This Function is used to configure the SSL for further MQTTS based communication
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: This will configure the SSL certificates and Keys for SSL configuration.
*
* @return		1 in case of Success or 0 otherwise
*
* \b Example Example:
* @code
*	uint8_t ret = ATCMD_A7672_Sslconf_Init();
* @end code
*
*@Note: This API should be called only after downloading the certificates and keys to the
*@Note: Module
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 05/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_Sslconf_Init(void)
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;

	atcmd_at_command_t atCommand[5] = {
			{"AT+CSSLCFG=\"sslversion\",0,4"ATCMD_CMD_END, NULL, 0, 120, ATCMD_CMD1_OK},
			{"AT+CSSLCFG=\"authmode\",0,2"ATCMD_CMD_END, NULL, 0, 120, ATCMD_CMD2_OK},
			{"AT+CSSLCFG=\"clientcert\",0,\"certificate.pem\""ATCMD_CMD_END, NULL, 0, 120, ATCMD_CMD3_OK},
			{"AT+CSSLCFG=\"cacert\",0,\"RootCA.pem\""ATCMD_CMD_END, NULL, 0, 120, ATCMD_CMD4_OK},
			{"AT+CSSLCFG=\"clientkey\",0,\"privatekey.pem\""ATCMD_CMD_END, NULL, 0, 120, ATCMD_CMD5_OK},
		};

	ATCMD_AT_Command_Queue_Init();
	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 5);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD5_OK == g_cmdQueue.currentCmdStatus )
		{
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;
	return retVal;
}


/******************************************************************************
* Function : ATCMD_A7672_MQTTS_Connect
* *//**
* \b Description:
*
* This Function is used to connect to the MQTT endpoint provided.
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: This will Connect to the endpoint provided and configured.
*
* @return		0 in case of Success or else it returns the error code for further actions
*
* \b Example Example:
* @code
*	uint8_t ret = ATCMD_A7672_MQTTS_Connect();
* @end code
*
*@Note: This API should be called only after downloading the certificates and keys to the
*@Note: Module
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description										</td></tr>
* <tr><td> 07/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created 									</td></tr>
* <tr><td> 15/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Changed the return type to /n uint16_t error codes </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint16_t ATCMD_A7672_MQTTS_Connect(void)
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;

	atcmd_at_command_t atCommand[4] = {
			{"AT+CMQTTSTART"ATCMD_CMD_END, ATCMD_MQTT_Start_Callback, 0, 120, ATCMD_CMD1_OK},
			{"AT+CMQTTACCQ=0,", ATCMD_MQTT_Accquire_Client_Callback, 0, 120, ATCMD_CMD2_OK},
			{"AT+CMQTTSSLCFG=0,0"ATCMD_CMD_END, NULL, 0, 120, ATCMD_CMD3_OK},
			{"AT+CMQTTCONNECT=0,", ATCMD_MQTT_Connect_Callback, 0, 120, ATCMD_CMD4_OK},
		};
	strcat((char*)atCommand[1].atCmd, "\"");
	strcat((char*)atCommand[1].atCmd, (char*)s_httpCredentials.devName);
	strcat((char*)atCommand[1].atCmd, "\",1\r\n");
	strcat((char*)atCommand[3].atCmd, "\"");
	strcat((char*)atCommand[3].atCmd, (char*)s_mqttCredentials.mqttURL);
	strcat((char*)atCommand[3].atCmd, ":");
	strcat((char*)atCommand[3].atCmd, (char*)s_mqttCredentials.mqttPort);
	strcat((char*)atCommand[3].atCmd, "\"");
	strcat((char*)atCommand[3].atCmd, ",60,1\r\n"); /*<< Keep Alive is set to 60 and Clean Session Flag is set to 1*/

	ATCMD_AT_Command_Queue_Init();
	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 4);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD4_OK == g_cmdQueue.currentCmdStatus )
		{
			retVal = 1;

		}
		else
			retVal = 0;
	}
	else
		retVal = 0;
	if(retVal == 1)
	{
		s_mqttCredentials.connectionStatus = MQTT_CONNECTED;
	}
	else{
		s_mqttCredentials.connectionStatus = MQTT_DISCONNECTED;
	}
	return g_mqtt_result;
}
/******************************************************************************
* Function : ATCMD_A7672_MQTTS_Disonnect
* *//**
* \b Description: This Function is used to disconnect from the MQTT endpoint provided.
* PRE-CONDITION	: The System clock and peripherals must be initialized, The Threads should
* 					be Initialized and Scheduler should be launched
* POST-CONDITION: This will disconnect from the endpoint provided and configured.
* @return		: 1 in case of Success or 0 otherwise
*
* \b Example Example:
* @code
*	uint8_t ret = ATCMD_A7672_MQTTS_Disonnect();
* @end code
*
*@Note: This API should be called only after downloading the certificates and keys to the
*@Note: Module
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 07/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_MQTTS_Disonnect(void)
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;

	atcmd_at_command_t atCommand[3] = {
			{"AT+CMQTTDISC=0,120"ATCMD_CMD_END, NULL, 0, 12, ATCMD_CMD1_OK},
			{"AT+CMQTTREL=0"ATCMD_CMD_END, NULL, 0, 12, ATCMD_CMD2_OK},
			{"AT+CMQTTSTOP"ATCMD_CMD_END, NULL, 0, 12, ATCMD_CMD3_OK},
		};

	ATCMD_AT_Command_Queue_Init();
	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 3);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD3_OK == g_cmdQueue.currentCmdStatus )
		{
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;
	return retVal;
}
/******************************************************************************
* Function : ATCMD_A7672_MQTTS_Subscribe
* *//**
* \b Description:
*
* This Function is used to Subscribe the the Topic Provided
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: This will Subscribe the the Topic Provided.
*
* @return		1 in case of Success or 0 otherwise
*
* \b Example Example:
* @code
*	uint8_t ret = ATCMD_A7672_MQTTS_Subscribe();
* @end code
*
*@Note: This API should be called only after establishing the MQTTS_Connect API
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 07/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_MQTTS_Subscribe(void)
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;
	uint16_t len = 0;

	atcmd_at_command_t atCommand[2] = {
			{"AT+CMQTTSUBTOPIC=0,", ATCMD_MQTT_Subtopic_Callback, 0, 12, ATCMD_CMD1_OK},
			{"AT+CMQTTSUB=0"ATCMD_CMD_END, ATCMD_MQTT_Subscribe_Callback, 0, 120, ATCMD_CMD2_OK},
		};
	len = (uint16_t)strlen((char*)s_mqttCredentials.mqttSubTopic);
	Convert_Integer_To_String(s_tempStr, len);
	strcat((char*)atCommand[0].atCmd, (char*)s_tempStr);
	strcat((char*)atCommand[0].atCmd, ",1\r\n");


	ATCMD_AT_Command_Queue_Init();
	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 2);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD2_OK == g_cmdQueue.currentCmdStatus )
		{
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;
	return retVal;
}

/******************************************************************************
* Function : ATCMD_A7672_MQTTS_Publish
* *//**
* \b Description:
*
* This Function is used to Publish the Data to the Publish Topic.
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: This will Publish to the Topic Provided..
*
* @return		1 in case of Success or 0 otherwise
*
* \b Example Example:
* @code
*	uint8_t ret = ATCMD_A7672_MQTTS_Publish(payloadArray);
* @end code
*
*@Note: This API should be called only after establishing the MQTTS_Connect API
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 07/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_MQTTS_Publish(char* publishPayload)
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;
	uint16_t len = 0;

	atcmd_at_command_t atCommand[3] = {
			{"AT+CMQTTTOPIC=0,", ATCMD_MQTT_Pubtopic_Callback, 3, 120, ATCMD_CMD1_OK},
			{"AT+CMQTTPAYLOAD=0,", ATCMD_MQTT_PubPayload_Callback, 3, 120, ATCMD_CMD2_OK},
			{"AT+CMQTTPUB=0,1,60"ATCMD_CMD_END, ATCMD_MQTT_Publish_Callback, 3, 120, ATCMD_CMD3_OK},
		};
	len = (uint16_t)strlen((char*)s_mqttCredentials.mqttPubTopic);
	memset(s_tempStr, '\0', sizeof(s_tempStr));
	Convert_Integer_To_String(s_tempStr, len);
	strcat((char*)atCommand[0].atCmd, (char*)s_tempStr);
	strcat((char*)atCommand[0].atCmd, "\r\n");
	s_tempPtr = Uart_Get_Transmit_Buffer();
	len = (uint16_t)strlen(publishPayload);
	memset(s_tempStr, '\0', sizeof(s_tempStr));
	Convert_Integer_To_String(s_tempStr, len);
	if((char*)s_tempPtr != publishPayload)
	{
		memset((char*)s_tempPtr, '\0', sizeof(s_tempPtr));
		strncpy((char*)s_tempPtr, publishPayload, len);
		strcat((char*)s_tempStr, "\r\n");
		strcat((char*)atCommand[1].atCmd, (char*)s_tempStr);
	}
	else
	{
		strcat((char*)s_tempStr, "\r\n");
		strcat((char*)atCommand[1].atCmd, (char*)s_tempStr);
	}

	ATCMD_AT_Command_Queue_Init();
	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 3);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD3_OK == g_cmdQueue.currentCmdStatus )
		{
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;
	return retVal;
}



uint8_t* getSignalStrengthPtr(void){
	return &g_moduleStatus.sigStrength;
}


/******************************************************************************
* Function : ATCMD_A7672_Check_Pdp_Context
* *//**
* \b Description:
*
* This Function is used to Check the PDP context of the connection
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: This will Publish to the Topic Provided..
*
* @return		State of the PDP context 0, 1
*
* \b Example Example:
* @code
*	uint8_t ret = ATCMD_A7672_Check_Pdp_Context();
* @end code
*
*@Note: This API should be called only after establishing the MQTTS_Connect API
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 15/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_Check_Pdp_Context( void )
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;

	atcmd_at_command_t atCommand[1] = {
		{"AT+CGATT?"ATCMD_CMD_END, ATCMD_Pdp_Context_Check_Callback, 0, 11, ATCMD_CMD1_OK},
		};

	ATCMD_AT_Command_Queue_Init();
	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 1);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD1_OK == g_cmdQueue.currentCmdStatus )
		{
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;
	return retVal;
}


/******************************************************************************
* Function : ATCMD_A7672_Set_Pdp_Context
* *//**
* \b Description:
*
* This Function is used to Set the PDP context of the connection
*
* PRE-CONDITION: The System clock and peripherals must be initialized, The Threads should
* be Initialized and Scheduler should be launched
*
* POST-CONDITION: This will Publish to the Topic Provided..
*
* @return		State of the PDP context 0, 1
*
* \b Example Example:
* @code
*	uint8_t ret = ATCMD_A7672_Set_Pdp_Context();
* @end code
*
*@Note: This API should be called only after establishing the MQTTS_Connect API
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 17/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t ATCMD_A7672_Set_Pdp_Context( void )
{
	atcmd_queue_status_t volatile status = ATCMD_QUEUE_ERROR;
	uint8_t volatile retVal = 0;

	atcmd_at_command_t atCommand[1] = {
		{"AT+CGACT=1,1"ATCMD_CMD_END, NULL, 0, 11, ATCMD_CMD1_OK},
		};

	ATCMD_AT_Command_Queue_Init();
	g_cmdQueue.cmdQueueWrite = 1;
	status = ATCMD_Add_AT_Cmd_Queue(&atCommand[0], 1);
	g_cmdQueue.cmdQueueWrite = 0;
	if( ATCMD_QUEUE_OPER_OK == status )
	{
		while( 0 != g_cmdQueue.commandCount ){}
		if( ATCMD_CMD1_OK == g_cmdQueue.currentCmdStatus )
		{
			retVal = 1;
		}
		else
			retVal = 0;
	}
	else
		retVal = 0;
	return retVal;
}









/***************************************************************END OF FILE ***************************************/
