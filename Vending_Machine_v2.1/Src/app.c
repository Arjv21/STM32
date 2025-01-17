/*******************************************************************************
* Title                 :   app
* Filename              :   app.c
* Author                :   Sijeo Philip
* Origin Date           :   26/04/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F
* Notes                 :   This functions are used as interface between between the
* 							Driver/Middleware to the Main application.
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
*  30/04/24   1.0.0   Sijeo Philip   Initial Release.
*
*******************************************************************************/

/******************************************************************************
* Includes
*******************************************************************************/
#include "app.h"
#include "sys_peri_clock.h"
#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "JSONWriter.h"
#include "JSONParser.h"
#include "spi.h"
#include "Sensor.h"
#include "Motor.h"
#include "adc.h"
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/
const char deviceProvisioningURL[] 	= "https://g1hpzj4ej4.execute-api.ap-southeast-1.a!mazonaws.com/prod/device-provision";
const char RootCAURL[]				= "https://jvze9tdee4.execute-api.ap-southeast-1.amazonaws.com/prod/get-rootcacert";
const char clientCertURL[]			= "https://xi0m3d2uk8.execute-api.ap-southeast-1.amazonaws.com/prod/get-certificate";
const char privateKeyURL[]			= "https://lhzv2tcaej.execute-api.ap-southeast-1.amazonaws.com/prod/get-privatekey";
const char mqttEpURL[]				= "tcp://a6oiqul5vocoo-ats.iot.ap-southeast-1.amazonaws.com";
//char mqttTrackingSubTopic[30]		= "TEST/UR/URCS/";			//"TEST/UR/UTCS/864180053965211";
//char mqttChannelChangeSubTopic[35]	= "TEST/UR/URCS/DEF/";
//char mqttChannelChangePubTopic[35]	= "TEST/UR/UTCS/DEF/";
//char mqttOnlinePubTopic[30]			= "TEST/UR/UTDS/";
//char mqttStatusPubTopic[30]			= "TEST/UR/UTMS/";
//char mqttResponsePubTopic[30]		= "TEST/UR/UTCS/";

char mqttTrackingSubTopic[30]		= "/UR/URCS/";			//"TEST/UR/UTCS/864180053965211";
char mqttChannelChangeSubTopic[35]	= "/UR/URCS/DEF/";
char mqttChannelChangePubTopic[35]	= "/UR/UTCS/DEF/";
char mqttOnlinePubTopic[30]			= "/UR/UTDS/";
char mqttStatusPubTopic[30]			= "/UR/UTMS/";
char mqttResponsePubTopic[30]		= "/UR/UTCS/";
char activeTopic[35];

const char firmwareVersion[]		= "HappyTopia_Fw_V1.0";
const char hardwareVersion[]		= "HappyTopia_Pcb_V1.0";

char *jsonBuffer;
struct jWriteControl	jPacket;
struct jsonParser 		jreadPacket;
struct jsonReadToken 	*jPacketToken;


extern uint8_t g_pbDone;
/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/

struct health_t{
	 certState_t 		cert_status;
	 nwRegnStatus_t 	nwRegnStatus;
	 gsmInfoStatus_t 	gsmInfoStatus;
	 modemStatus_t		modemStatus;
	 ConnectStatus_t	httpConnectStatus;
	 ConnectStatus_t	mqttConnectStatus;
	 fault_t			faultReason;
	 uint8_t			modemRestartCount;
	 float 				bat_voltage;
	 uint8_t			bat_voltString[5];
	 dispStatus_t		dispenseStatus;
	 announceStatus_t	announceState;
}g_sysHealth;


struct dispenseRequest_t{
	uint32_t 		serialNumber;
	uint32_t 		requestTime;
	uint8_t			val;
	dispStatus_t	dispenseStatus;
}g_dispenseRequest, g_lastDispenseRequest;


struct mqttPacketPara_t{
	uint32_t		statusMessageIndex;
	char*			dateTimePtr;
	uint32_t		epochTime;
	char	 		modelName[22];
	char*			signalStrength;
	char			statusTimerExpired;
}mqttPacketPara;

uint32_t		measurementMatrix[7];

ssl_certs_t*	InstalledCerts;
sysMode_t 		g_sysState 			= SYS_MODE_TEST;
modemStatus_t 	Modem_State 		= MODEM_OFF;
requestType_t	g_requestType		= REQ_TYPE_NONE;
unsigned char	g_packetReceived	= 0;
unsigned char	g_testDispenseCount	= 0;
char	g_channelToSwitch[10]		= "TEST";
char	g_requestedFWVersion[25]	= "\0";
char 	counter						= 0;
char 	checkNwStatus				= 0;

/******************************************************************************
* Function Prototypes
*******************************************************************************/
uint8_t ATCMD_A7672_ModuleHardPowerOff(void);
void parseMessageFromServer(uint8_t *arg);
void ATCMD_A7672_ModuleHardRestart(void);
/******************************************************************************
* Function Definitions
*******************************************************************************/

void removeQuotes(char *str){
	for(;*str != '"'; str++);
	*str = '\0';
}
/******************************************************************************
* Function :  Vending_Machine_ModemHandler
* \b Description: This function is used to initialize the Modem.
* PRE-CONDITION : None
* POST-CONDITION: The System Clock and Periperals needed for this application will
* 				  be intialized and ready to use.
* @return 		None
*
* \b Example Example:
* @code
* 	 	Vending_Machine_ModemHandler();
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 30/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*******************************************************************************/
void Vending_Machine_ModemHandler(void){
	switch(Modem_State)
	{
	case MODEM_OFF:
		if(Gpio_Read(A7672_POWER_STATUS_GPIO, A7672_POWER_STATUS_PIN)==0){
			if(ATCMD_A7672_Module_On() == 1)
			{
				Modem_State = MODEM_ON;
			}
		}
		else{
			ATCMD_A7672_ModuleHardRestart();
		}
		if(ATCMD_A7672_Module_On() == 1)
		{
			Modem_State = MODEM_ON;
		}
		break;
	case MODEM_ON:
		if(g_pbDone == 1)
		{
			LED_Set_On(LED1_PIN);
			g_pbDone = 0;
			Modem_State = MODEM_READY;
			g_sysHealth.httpConnectStatus 	= CONNECT_NOT_CONNECTED;
			g_sysHealth.mqttConnectStatus 	= CONNECT_NOT_CONNECTED;
			ATCMD_A7672_MQTT_Set_Connection_Status(MQTT_NOT_CONNECTED);
			g_sysHealth.nwRegnStatus 		= NW_REGN_FAILED;
		}
		break;
	case MODEM_READY:
		if(ATCMD_A7672_Module_Ready())
		{
			Modem_State = MODEM_ON;						// keep on checking for "B DONE" urc
			g_sysHealth.modemStatus = MODEM_READY;
			g_sysState = SYS_MODE_TEST;
		}
	break;
	case MODEM_IDLE:
		break;
	default:
		break;
	}
}

void Vending_Machine_State_Ctrl(void){
	switch(g_sysState){
	case SYS_MODE_TEST:
		Vending_Machine_SystemTest();
		break;
	case SYS_MODE_CONFIG:
		Vending_Machine_SystemConfig();
		break;
	case SYS_MODE_NORMAL_OP:
		Vending_Machine_NormalOp();
		break;
	case SYS_MODE_FOTA:
		Vending_Machine_Fota();
		break;
	case SYS_MODE_FAULT:
		Vending_Machine_SystemFault();
		break;
	default:
		break;
	}
}


void Vending_Machine_SystemTest(void){
	if(g_sysHealth.modemStatus == MODEM_READY){
		if(g_sysHealth.gsmInfoStatus == GSM_INFO_NOT_UPDATED){
			Vending_Machine_GetModemInfo();							// update modem firmware version, IMEI & CCID and set gsmInfoStatus to GSM_INFO_UPDATED
			g_sysHealth.gsmInfoStatus = GSM_INFO_UPDATED;
		}
		if(g_sysHealth.nwRegnStatus == NW_REGN_FAILED){
			if(ATCMD_A7672_Module_Check() == 1){						// Check CGREG status for 0,1... if success, send CGATT and set nwRegnStatus to NW_REGN_SUCCESS on OK response
				ATCMD_A7672_Get_NTP_HostName();
				ATCMD_A7672_Update_Modem_DateTime();
				while((ATCMD_A7672_Get_DateTime() == 0));
				g_sysHealth.nwRegnStatus = NW_REGN_SUCCESS;
			}
		}
		else if(g_sysHealth.nwRegnStatus == NW_REGN_SUCCESS){
			if(g_sysHealth.cert_status == CERT_NOT_OK){
				g_sysState = SYS_MODE_CONFIG;						// get any installed certificates list and delete them and download new certificates
			}
			else{
				if(g_sysHealth.mqttConnectStatus == CONNECT_NOT_CONNECTED){
					Vending_Machine_ConnectToMqttServer();
					if (g_sysHealth.mqttConnectStatus == CONNECT_SUCCESS){
						if(g_sysHealth.announceState == ANNOUNCE_NOT_SENT)
						Vending_Machine_PrepareAnnouncePacket();
						if(ATCMD_A7672_MQTTS_Publish(jsonBuffer) == 1){
							g_sysHealth.announceState = ANNOUNCE_SENT;
							g_sysState = SYS_MODE_NORMAL_OP;
						}
						g_sysState = SYS_MODE_NORMAL_OP;
					}
					else{
						g_sysState = SYS_MODE_CONFIG;
					}

//					Vending_Machine_PublishPacket();
//					Vending_Machine_SubscribeActiveTopic();
					g_sysState = SYS_MODE_NORMAL_OP;
				}
				else if(g_sysHealth.mqttConnectStatus == CONNECT_FAILED){
					g_sysState = SYS_MODE_CONFIG;
				}
				else if(g_sysHealth.mqttConnectStatus == CONNECT_DISCONNECTED){
					ATCMD_A7672_MQTTS_Disonnect();
					ATCMD_A7672_MQTT_Set_Connection_Status(MQTT_NOT_CONNECTED);
					if(ATCMD_A7672_Check_Pdp_Context() == 0){
						while(ATCMD_A7672_Set_Pdp_Context());
					}
					Vending_Machine_ConnectToMqttServer();
				}
				else{
					g_sysState = SYS_MODE_NORMAL_OP;
				}
			}
		}
	}
	else if (g_sysHealth.modemStatus == MODEM_NOT_RDY){
		ATCMD_A7672_ModuleHardRestart();
		g_sysHealth.modemRestartCount++;
		if(g_sysHealth.modemRestartCount > MAX_MODEM_RESTART_COUNT){
			g_sysState = SYS_MODE_FAULT;
			g_sysHealth.faultReason = FAULT_NW_FAIL;
		}
	}
	if(checkNwStatus){
		checkNwStatus = 0;
		if(ATCMD_A7672_Check_Pdp_Context() == 0){
			while(ATCMD_A7672_Set_Pdp_Context());
		}
	}

//	if(g_sysHealth.bat_voltage <= 10){
//		g_sysState = SYS_MODE_FAULT;
//		g_sysHealth.faultReason = FAULT_BAT_DOWN;
//	}
}

void Vending_Machine_SystemConfig(void){
//	Vending_Machine_DeleteInstalledCertificates();
	uint8_t certCount=0;

//	if(g_sysHealth.mqttConnectStatus == CONNECT_FAILED){
//		vending_Machine_DeleteAllCerts();
//	}
	InstalledCerts = ATCMD_A7672_Check_Module_Certificates();
	if(InstalledCerts->certCount < 3){
		if(ATCMD_A7672_Download_Root_Ca()){
			ATCMD_A7672_HTTP_Read();
			ATCMD_A7672_CertKey_Module_Download();
			ATCMD_A7672_HTTP_Terminate();
		}
		if(ATCMD_A7672_Provision_Device()){
			ATCMD_A7672_HTTP_Terminate();
		}
		if(ATCMD_A7672_Download_Certificate()){
			ATCMD_A7672_HTTP_Read();
			ATCMD_A7672_CertKey_Module_Download();
			ATCMD_A7672_HTTP_Terminate();
		}
		if(ATCMD_A7672_Download_Private_Key()){
			ATCMD_A7672_HTTP_Read();
			ATCMD_A7672_CertKey_Module_Download();
			ATCMD_A7672_HTTP_Terminate();
		}
		g_sysState = SYS_MODE_TEST;
	}
	else{
		if(ATCMD_A7672_Is_Module_Certificate("RootCA.pem"))
			certCount++;
		if(ATCMD_A7672_Is_Module_Certificate("certificate.pem"))
			certCount++;
		if(ATCMD_A7672_Is_Module_Certificate("privatekey.pem"))
			certCount++;
		if(certCount == InstalledCerts->certCount){
			g_sysHealth.cert_status = CERT_OK;
			g_sysState = SYS_MODE_TEST;
		}
	}
}

void Vending_Machine_NormalOp(void){
	char dispRetryCount = 0;
//	while(1)
	{
		g_sysHealth.mqttConnectStatus = (ConnectStatus_t)ATCMD_A7672_MQTT_Get_Connection_Status();
		if(g_sysHealth.mqttConnectStatus == CONNECT_DISCONNECTED){
			g_sysState = SYS_MODE_TEST;
		}
		if(g_packetReceived){
			g_packetReceived = 0;
			switch(g_requestType){
			case REQ_TYPE_DIPSENSE:
				if(g_dispenseRequest.serialNumber != g_lastDispenseRequest.serialNumber){
					dispRetryCount = MAX_DISP_RETRY_COUNT;
					  dispRetryCount--;
					Vending_Machine_PrepareAckPacket();
					memset(activeTopic, '\0', sizeof(activeTopic));
					strcpy(activeTopic, g_channelToSwitch);
					strConcat(activeTopic, mqttResponsePubTopic);
					ATCMD_A7672_MQTT_Set_PubTopic(activeTopic);
					ATCMD_A7672_MQTTS_Publish(jsonBuffer);
					Vending_Machine_StartDispense();
					if(g_dispenseRequest.dispenseStatus == DISPENSE_SUCCESS){
						measurementMatrix[TOTAL_FIRST_TRIGGER]++;
					}
					else{
						do{
							measurementMatrix[TOTAL_RETRIES]++;
							Vending_Machine_StartDispense();
							if(g_dispenseRequest.dispenseStatus == DISPENSE_SUCCESS){
								measurementMatrix[TOTAL_RETRY_TRIGGER]++;
								dispRetryCount = 0;
								break;
							}
						}while(dispRetryCount);
					}
					Vending_Machine_PrepareDispenseStatusPacket();
					ATCMD_A7672_MQTTS_Publish(jsonBuffer);
					g_lastDispenseRequest.serialNumber = g_dispenseRequest.serialNumber;
					g_lastDispenseRequest.serialNumber = 0;
					g_lastDispenseRequest.requestTime = g_dispenseRequest.requestTime;
					g_lastDispenseRequest.requestTime = 0;
					g_lastDispenseRequest.dispenseStatus = g_dispenseRequest.dispenseStatus;
					g_lastDispenseRequest.dispenseStatus = 0;
				}
				break;
			case REQ_TYPE_CHANGE_CHANNEL:

//				Vending_Machine_SaveActiveChannel(g_channelToSwitch);
				Vending_Machine_PrepareChannelChangeAckPacket();
				memset(activeTopic, '\0', sizeof(activeTopic));
				strcpy(activeTopic, g_channelToSwitch);
				strConcat(activeTopic, mqttChannelChangePubTopic);
				ATCMD_A7672_MQTT_Set_PubTopic(activeTopic);
				ATCMD_A7672_MQTTS_Publish(jsonBuffer);
				ATCMD_A7672_MQTTS_Disonnect();
				g_sysHealth.mqttConnectStatus = CONNECT_NOT_CONNECTED;
				g_sysState = SYS_MODE_TEST;
//				Vending_Machine_ChannelChangeAckPacket();
//				Vending_Machine_Reboot();
				break;
			case REQ_TYPE_OTA_START:

				break;
			case REQ_TYPE_MOTOR_TEST:
				Vending_Machine_PrepareAckPacket();
				memset(activeTopic, '\0', sizeof(activeTopic));
				strcpy(activeTopic, g_channelToSwitch);
				strConcat(activeTopic, mqttResponsePubTopic);
				ATCMD_A7672_MQTT_Set_PubTopic(activeTopic);
				ATCMD_A7672_MQTTS_Publish(jsonBuffer);
				for(; g_testDispenseCount; g_testDispenseCount--){
					Vending_Machine_StartDispense();
					Vending_Machine_PrepareDispenseStatusPacket();
					ATCMD_A7672_MQTTS_Publish(jsonBuffer);
				}
				break;
			case REQ_TYPE_REBOOT:

				break;
			case REQ_TYPE_NONE:
				break;
			default:
				break;
			}
			g_requestType = REQ_TYPE_NONE;
		}
		if(mqttPacketPara.statusTimerExpired){
			mqttPacketPara.statusTimerExpired = 0;
			memset(activeTopic, '\0', sizeof(activeTopic));
			strcpy(activeTopic, g_channelToSwitch);
			strConcat(activeTopic, mqttStatusPubTopic);
			ATCMD_A7672_MQTT_Set_PubTopic(activeTopic);
			Vending_Machine_PrepareStatusPacket();
			ATCMD_A7672_MQTTS_Publish(jsonBuffer);
		}
	}
}


void Vending_Machine_Fota(void){
	Vending_Machine_StartDispense();
	Delay_Ms(3000);
}
void Vending_Machine_SystemFault(void){
	;
}



void System_Variables_Init(void){
	memset(&g_sysHealth,0x00,sizeof(g_sysHealth));
	memset(&g_dispenseRequest,0,sizeof(g_dispenseRequest));
	memset(&g_lastDispenseRequest,0,sizeof(g_lastDispenseRequest));
	ATCMD_A7672_Register_Message_Callback(parseMessageFromServer);
	ATCMD_A7672_Resgister_Status_Send_Callback(isTimerexpired,5);
	ATCMD_A7672_Register_Error_Callback(vending_Machine_ErrorHandlerCallback,2);
	ATCMD_A7672_HTTP_Set_DevProvURL(deviceProvisioningURL);
	ATCMD_A7672_HTTP_Set_RootCAURL(RootCAURL);
	ATCMD_A7672_HTTP_Set_CertURL(clientCertURL);
	ATCMD_A7672_HTTP_Set_PrivKeyURL(privateKeyURL);
	ATCMD_A7672_MQTT_Set_URL(mqttEpURL);
	ATCMD_A7672_MQTT_Set_Port("8883");
	mqttPacketPara.statusMessageIndex = 0;
	mqttPacketPara.dateTimePtr = dateTime;
	mqttPacketPara.signalStrength = (char*)getSignalStrengthPtr();
	strcpy(mqttPacketPara.modelName,"UNI_");
}

/******************************************************************************
* Function :  Vending_Machine_PrepareAnnouncePacket
*//**
* \b Description: This function is used to initialize the Modem.
* PRE-CONDITION : None
* POST-CONDITION: The System Clock and Periperals needed for this application will
* 				  be intialized and ready to use.
* @Parameters	: None
* @return 		: None
*
* \b Example Example:
* @code
* 	 	Vending_Machine_PrepareAnnouncePacket();
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 30/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void Vending_Machine_PrepareAnnouncePacket(void){
	char 	*deviceName		= (char*)ATCMD_A7672_HTTP_Get_DevName();
	g_sysHealth.bat_voltage = ADC1_Read_battery_value();
	ATCMD_A7672_Get_DateTime();
	ATCMD_A7672_Get_Signal_Strength();
	convert_to_epoch(mqttPacketPara.dateTimePtr,&mqttPacketPara.epochTime);
	jsonBuffer = (char*)Uart_Get_Transmit_Buffer();
	removeQuotes(deviceName);
	memset(jsonBuffer, 0,500);
	jwOpen(&jPacket, (char*)jsonBuffer, 500, JW_OBJECT, JW_COMPACT);
	jwObj_int(&jPacket, "PT", 0);
	jwObj_int(&jPacket, "MI", 0);
	jwObj_string(&jPacket, "UD", (char*)deviceName);
	jwObj_string(&jPacket, "UI", (char*)deviceName);
	jwObj_string(&jPacket, "UL", "Unisem");
	jwObj_int(&jPacket, "DT", mqttPacketPara.epochTime);
	jwObj_double(&jPacket, "BV", g_sysHealth.bat_voltage);
	jwObj_string(&jPacket, "LRER", "MANUAL");;
		jwObj_object(&jPacket, "MM");
			jwObj_object(&jPacket, "UDI");
				jwObj_string(&jPacket, "UMN", "UNISEM ELECTRONICS");
				jwObj_string(&jPacket, "HV", (char*)firmwareVersion);
				jwObj_string(&jPacket, "FV", (char*)hardwareVersion);
			jwEnd(&jPacket);
			jwObj_object(&jPacket, "ND");
				jwObj_string(&jPacket, "GFW", "A7672M6_V1.11.1");
				jwObj_string(&jPacket, "IMEI", (char*)deviceName);
				jwObj_string(&jPacket, "CCID", "89914509006607302469");
				jwObj_int(&jPacket, "NSS", ((*mqttPacketPara.signalStrength)*100)/32);
			jwEnd(&jPacket);
		jwEnd(&jPacket);
    jwClose(&jPacket);

}

/******************************************************************************
* Function :  Vending_Machine_PrepareAckPacket
* \b Description: This function is used to initialize the Modem.
* PRE-CONDITION : None
* POST-CONDITION: The System Clock and Periperals needed for this application will
* 				  be intialized and ready to use.
* @Parameters	: None
* @return 		: None
*
* \b Example Example:
* @code
* 	 	Vending_Machine_PrepareAnnouncePacket();
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 30/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void Vending_Machine_PrepareAckPacket(void){
	jsonBuffer = (char*)Uart_Get_Transmit_Buffer();
	ATCMD_A7672_Get_Signal_Strength();
	ATCMD_A7672_Get_DateTime();
	convert_to_epoch(mqttPacketPara.dateTimePtr,&mqttPacketPara.epochTime);
	memset(jsonBuffer,0,500);
	jwOpen(&jPacket, (char*)jsonBuffer, 500, JW_OBJECT, JW_COMPACT);
	jwObj_int(&jPacket, "PT", 3);
	jwObj_int(&jPacket, "PSN", g_dispenseRequest.serialNumber);
	jwObj_string(&jPacket, "MSG", "ACK");
	jwObj_int(&jPacket, "DT", mqttPacketPara.epochTime);
	jwObj_int(&jPacket, "NSS", ((*mqttPacketPara.signalStrength)*100)/32);
	jwObj_string(&jPacket, "MN", mqttPacketPara.modelName);
    jwClose(&jPacket);

}

/******************************************************************************
* Function :  Vending_Machine_PrepareDispenseStatusPacket
* \b Description: This function is used to initialize the Modem.
* PRE-CONDITION : None
* POST-CONDITION: The System Clock and Periperals needed for this application will
* 				  be intialized and ready to use.
* @Parameters	: None
* @return 		: None
*
* \b Example Example:
* @code
* 	 	Vending_Machine_PrepareAnnouncePacket();
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 30/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void Vending_Machine_PrepareDispenseStatusPacket(void){

	ATCMD_A7672_Get_DateTime();
	convert_to_epoch(mqttPacketPara.dateTimePtr,&mqttPacketPara.epochTime);
	jsonBuffer = (char *)Uart_Get_Transmit_Buffer();
	memset(jsonBuffer,0,500);
	jwOpen(&jPacket, (char*)jsonBuffer, 500, JW_OBJECT, JW_COMPACT);
	jwObj_int(&jPacket, "PT", 4);
	jwObj_string(&jPacket, "MN", mqttPacketPara.modelName);
	jwObj_int(&jPacket, "PSN", g_dispenseRequest.serialNumber);
	jwObj_int(&jPacket, "DT", mqttPacketPara.epochTime);
	if(g_sysHealth.dispenseStatus == DISPENSE_SUCCESS){
		jwObj_string(&jPacket, "MSG", "SUCCESS");
	}
	else if(g_sysHealth.dispenseStatus == DISPENSE_FAILED){
		jwObj_string(&jPacket, "MSG", "DISP_FAIL");
	}
		jwObj_array(&jPacket, "MM");
			jwArr_int(&jPacket, measurementMatrix[SENSOR_PULSE_COUNT]);
			jwArr_int(&jPacket, measurementMatrix[MOTOR_RUN_PULSE_COUNT]);
			jwArr_int(&jPacket, measurementMatrix[TOTAL_TRIGGER]);
			jwArr_int(&jPacket, measurementMatrix[TOTAL_RETRIES]);
			jwArr_int(&jPacket, measurementMatrix[TOTAL_RETRY_TRIGGER]);
			jwArr_int(&jPacket, measurementMatrix[TOTAL_FIRST_TRIGGER]);
			jwArr_int(&jPacket, g_lastDispenseRequest.requestTime);
			jwArr_double(&jPacket, g_sysHealth.bat_voltage);
		jwEnd(&jPacket);
    jwClose(&jPacket);
}

/******************************************************************************
* Function :  Vending_Machine_PrepareStatusPacket
* \b Description: Prepare device Status Packet which is to be send in every 15 minutes.
* PRE-CONDITION : None
* POST-CONDITION: The System Clock and Periperals needed for this application will
* 				  be intialized and ready to use.
* @Parameters	: None
* @return 		: None
*
* \b Example Example:
* @code
* 	 	Vending_Machine_PrepareAnnouncePacket();
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 30/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void Vending_Machine_PrepareStatusPacket(void){

	ATCMD_A7672_Get_DateTime();
	ATCMD_A7672_Get_Signal_Strength();
	g_sysHealth.bat_voltage = ADC1_Read_battery_value();
	convert_to_epoch(mqttPacketPara.dateTimePtr,&mqttPacketPara.epochTime);
	jsonBuffer = (char*)Uart_Get_Transmit_Buffer();
	memset(jsonBuffer,'\0',500);
	jwOpen(&jPacket, (char*)jsonBuffer, 500, JW_OBJECT, JW_COMPACT);
	jwObj_int(&jPacket, "PT", 1);
	jwObj_int(&jPacket, "MI", mqttPacketPara.statusMessageIndex);
	jwObj_string(&jPacket, "UD", mqttPacketPara.modelName+4);
	jwObj_string(&jPacket, "UI", mqttPacketPara.modelName+4);
	jwObj_int(&jPacket, "DT", mqttPacketPara.epochTime);
	jwObj_int(&jPacket, "SD", 0);
	jwObj_int(&jPacket, "NSS", ((*mqttPacketPara.signalStrength)*100)/32);
	jwObj_object(&jPacket, "MP");
		jwObj_int(&jPacket, "MT", 1);
		jwObj_int(&jPacket, "MD", 0);
		jwObj_array(&jPacket, "MM");
			jwArr_int(&jPacket, measurementMatrix[SENSOR_PULSE_COUNT]);
			jwArr_int(&jPacket, measurementMatrix[MOTOR_RUN_PULSE_COUNT]);
			jwArr_int(&jPacket, measurementMatrix[TOTAL_TRIGGER]);
			jwArr_int(&jPacket, measurementMatrix[TOTAL_RETRIES]);
			jwArr_int(&jPacket, measurementMatrix[TOTAL_RETRY_TRIGGER]);
			jwArr_int(&jPacket, measurementMatrix[TOTAL_FIRST_TRIGGER]);
			jwArr_int(&jPacket, g_lastDispenseRequest.requestTime);
			jwArr_double(&jPacket, g_sysHealth.bat_voltage);
		jwEnd(&jPacket);
		jwObj_int(&jPacket, "DL", 3);
		jwObj_string(&jPacket, "MN", mqttPacketPara.modelName);
		jwEnd(&jPacket);
    jwClose(&jPacket);
    mqttPacketPara.statusMessageIndex++;
}

/******************************************************************************
* Function :  Vending_Machine_PrepareChannelChangeResponsePacket
* \b Description: Prepare device Response packet for Channel Change Request.
* PRE-CONDITION : None
* POST-CONDITION: The System Clock and Periperals needed for this application will
* 				  be intialized and ready to use.
* @Parameters	: None
* @return 		: None
*
* \b Example Example:
* @code
* 	 	Vending_Machine_PrepareAnnouncePacket();
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 30/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void Vending_Machine_PrepareChannelChangeAckPacket(void){

	ATCMD_A7672_Get_DateTime();
	convert_to_epoch(mqttPacketPara.dateTimePtr,&mqttPacketPara.epochTime);

	jsonBuffer = (char*)Uart_Get_Transmit_Buffer();
	memset(jsonBuffer,'\0',500);

	jwOpen(&jPacket, (char*)jsonBuffer, 500, JW_OBJECT, JW_COMPACT);
	jwObj_int(&jPacket, "PT", 5);
	jwObj_int(&jPacket, "MI", 1);
	jwObj_string(&jPacket, "MN", mqttPacketPara.modelName);
	jwObj_int(&jPacket, "DT", mqttPacketPara.epochTime);
	jwObj_string(&jPacket, "run", mqttPacketPara.modelName+4);
	jwObj_string(&jPacket, "rap", "WiRun_UserConfig");
	jwObj_object(&jPacket, "CP");
		jwObj_int(&jPacket, "mst", 255);
		jwObj_string(&jPacket, "topic", g_channelToSwitch);
		jwEnd(&jPacket);
    jwClose(&jPacket);
}
/******************************************************************************
* Function 		:  parseMessageFromServer
* \b Description: Callback function triggered when "+CMQTTRXPAYLOAD:" comes at UART
* PRE-CONDITION : GSM must be initialized
* POST-CONDITION: The System Clock and Peripherals needed for this application will
* 				  be initialized and ready to use.
* @Parameters	: payload from the "+CMQTTRXPAYLOAD:" URC
* @return 		: None
*
* \b Example Example:
* @code
* 	 	parseMessageFromServer();
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 30/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/

void parseMessageFromServer(uint8_t *arg){
	struct jsonReadToken *tempToken;
	unsigned char err = 0;
	uint8_t packetType = 0;
	memset(jsonBuffer,0,500);
	memset(&jreadPacket,0,sizeof(jreadPacket));
	err = JSONStructInit(&jreadPacket, (char *)arg);
	if(err != JDATA_NOK){
		while(jreadPacket.parserStatus!=JSON_END)
			err = JSONParseData(&jreadPacket);
	}
	jPacketToken = SearchToken(&jreadPacket, "\"PT\"");
	if(jPacketToken != NULL){
		packetType = Convert_String_To_Integer((uint8_t *)&jPacketToken->jsonValue);
		switch(packetType){
		case 2:
		case 132:
			tempToken = SearchToken(&jreadPacket, "\"REQT\"");
			g_dispenseRequest.requestTime = Convert_String_To_Integer32((uint8_t *)&tempToken->jsonValue);
			tempToken = SearchToken(&jreadPacket, "\"VAL\"");
			g_dispenseRequest.val = Convert_String_To_Integer((uint8_t *)&tempToken->jsonValue);
			tempToken = SearchToken(&jreadPacket, "\"PSN\"");
			g_dispenseRequest.serialNumber = Convert_String_To_Integer((uint8_t *)&tempToken->jsonValue);
			g_requestType = REQ_TYPE_DIPSENSE;
			break;
		case 5:		// ChannelChange Request
			tempToken = SearchToken(&jreadPacket, "\"CP\"");
			memset(&jreadPacket,'\0',sizeof(jreadPacket));
			err = JSONStructInit(&jreadPacket, tempToken->jsonValue);
			if(err != JDATA_NOK){
			while(jreadPacket.parserStatus!=JSON_END)
				err = JSONParseData(&jreadPacket);
			}
			tempToken = SearchToken(&jreadPacket, "\"topic\"");
			memset(g_channelToSwitch, '\0', 10);
			strcpy(g_channelToSwitch, tempToken->jsonValue);
			g_requestType = REQ_TYPE_CHANGE_CHANNEL;
		case 6:
		case 128:
			tempToken = SearchToken(&jreadPacket, "\"reboot\"");
			if(Convert_String_To_Integer32((uint8_t *)&tempToken->jsonValue) == 1)
				g_requestType = REQ_TYPE_REBOOT;
			break;
		case 7:
			tempToken = SearchToken(&jreadPacket, "\"Dispense_Count\"");
			g_testDispenseCount = Convert_String_To_Integer32((uint8_t *)&tempToken->jsonValue);
			g_requestType = REQ_TYPE_MOTOR_TEST;
			break;
		case 8:
			tempToken = SearchToken(&jreadPacket, "\"FV\"");
			memset(g_requestedFWVersion, '\0', sizeof(g_requestedFWVersion));
			strcpy(g_requestedFWVersion, tempToken->jsonValue);
			g_requestType = REQ_TYPE_OTA_START;
			break;
		default:
			break;
		}
		g_packetReceived = 1;
	}
}


void isTimerexpired(void){
	counter++;
	if(counter == 3){
		counter = 0;
		mqttPacketPara.statusTimerExpired = 1;
	}
	else{
		checkNwStatus = 1;
		g_sysState = SYS_MODE_TEST;
	}

}



/******************************************************************************
* Function :  Vending_Machine_GetModemInfo
*//**
* \b Description: Get Modem info.
* PRE-CONDITION : None
* POST-CONDITION: The System Clock and Periperals needed for this application will
* 				  be intialized and ready to use.
* @Parameters	: None
* @return 		: None
*
* \b Example Example:
* @code
* 	 	Vending_Machine_ModemHandler();
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 30/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void Vending_Machine_GetModemInfo(void){
	ATCMD_A7672_Get_IMEI_Number();
	ATCMD_A7672_Get_Signal_Strength();
	strcat((char*)mqttPacketPara.modelName,(char*)ATCMD_A7672_HTTP_Get_DevName());
	removeQuotes(mqttPacketPara.modelName);
	strConcat(mqttOnlinePubTopic,mqttPacketPara.modelName+4);
	strConcat(mqttStatusPubTopic,mqttPacketPara.modelName+4);
	strConcat(mqttTrackingSubTopic,mqttPacketPara.modelName+4);
	strConcat(mqttChannelChangeSubTopic,mqttPacketPara.modelName+4);
	strConcat(mqttChannelChangePubTopic,mqttPacketPara.modelName+4);
	strConcat(mqttResponsePubTopic,mqttPacketPara.modelName+4);
}


/******************************************************************************
* Function 		:  Vending_Machine_System_Init
*//**
* \b Description: This function is used to initialize the Complete System in the Main application.
* PRE-CONDITION : None
* POST-CONDITION: The System Clock and Periperals needed for this application will
* 				  be intialized and ready to use.
* @Parameters	: None
* @return 		: None
*
* \b Example Example:
* @code
* 	 	Vending_Machine_System_Init();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 30/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void Vending_Machine_System_Init(void)
{
	/*1. System Clock and Peripheral Clock have to be intialized
	 * and Running
	 */
	System_Clock_Init();
	/*2. GPIOs are Initialized*/
	Gpio_Init();
	/*3. UAART Initialized*/
	Uart_Init();
	/*4. Timer Initalized */
	Timer_Init();
	/*5.SPI Intialized*/
	Spi_Init();
	/*6.ADC Intialized*/
	ADC1_Init();
	/*Enable the Floating Point Unit*/
	SCB->CPACR |= (0xF << 20);
	/*7.Initialize other system variables*/
	System_Variables_Init();

}



void Vending_Machine_ConnectToMqttServer(void){
	uint16_t tempConnResult;
	ATCMD_A7672_Sslconf_Init();
	tempConnResult = ATCMD_A7672_MQTTS_Connect();
	if (tempConnResult == 0){
		g_sysHealth.mqttConnectStatus = CONNECT_SUCCESS;
	}
	else if(tempConnResult == 34){
		g_sysHealth.faultReason = FAULT_MQTT_CONNECT_FAIL;
		g_sysHealth.mqttConnectStatus = CONNECT_FAILED;
	}
	memset(activeTopic, '\0', sizeof(activeTopic));
	strcpy(activeTopic, g_channelToSwitch);
	strConcat(activeTopic, mqttOnlinePubTopic);
	ATCMD_A7672_MQTT_Set_PubTopic(activeTopic);
	memset(activeTopic, '\0', sizeof(activeTopic));
	strcpy(activeTopic, g_channelToSwitch);
	strConcat(activeTopic, mqttTrackingSubTopic);
	ATCMD_A7672_MQTT_Set_SubTopic(activeTopic);
	ATCMD_A7672_MQTTS_Subscribe();
	memset(activeTopic, '\0', sizeof(activeTopic));
	strcpy(activeTopic, g_channelToSwitch);
	strConcat(activeTopic, mqttChannelChangeSubTopic);
	ATCMD_A7672_MQTT_Set_SubTopic(activeTopic);
	ATCMD_A7672_MQTTS_Subscribe();
}

/******************************************************************************
* Function 		: Vending_Machine_StartDispense
*//**
* \b Description: handle dispense operation. will enable sensor and triggers dispense operation.
* 					waits till dispense is success or timeout occurs. result will be stored in
* 					g_sysHealth.dispenseStatus and g_dispenseRequest.dispenseStatus .
* PRE-CONDITION	: The System clock and Gpios must be initialized,
* POST-CONDITION: Dispense operation is done
* @Parameters	: None
* @return		: None
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
void Vending_Machine_StartDispense(void){
	uint8_t result = 0;
	if(Vending_Machine_EnableSensor() == 1){
		VENDING_MACHINE_MOTOR_ON();
		result = Vending_Machine_WaitForDispense();
		if(result == 1){
			g_sysHealth.dispenseStatus = DISPENSE_SUCCESS;
			measurementMatrix[TOTAL_TRIGGER]++;
			g_dispenseRequest.dispenseStatus = DISPENSE_SUCCESS;
		}
		else{
			g_sysHealth.dispenseStatus = DISPENSE_FAILED;
			g_dispenseRequest.dispenseStatus = DISPENSE_FAILED;
		}
		VENDING_MACHINE_MOTRO_OFF();
		Vending_Machine_DisableSensor();
	}
}




/******************************************************************************
* Function 		: ATCMD_A7672_ModulHardReset
*//**
* \b Description: This Function is used to Switch on the Module over Power Key Signal
* PRE-CONDITION	: The System clock and peripherals must be initialized,
* POST-CONDITION: The Module is Turned ON
* @Parameters	: None
* @return		: 0 in case of the Module is Off else if On it will return 1
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

void ATCMD_A7672_ModuleHardRestart(void)
{
	Gpio_Set(A7672_POWER_GPIO, A7672_POWER_PIN);
	Delay_Ms(3000);
	Gpio_Clear(A7672_POWER_GPIO, A7672_POWER_PIN);
	while(Gpio_Read(A7672_POWER_STATUS_GPIO, A7672_POWER_STATUS_PIN)==1);
	Gpio_Clear(A7672_POWER_GPIO, A7672_POWER_PIN);
	Delay_Ms(5);
	Gpio_Set(A7672_POWER_GPIO, A7672_POWER_PIN);
	while(Gpio_Read(A7672_POWER_STATUS_GPIO, A7672_POWER_STATUS_PIN)==0);
	Gpio_Clear(A7672_POWER_GPIO, A7672_POWER_PIN);
}



/******************************************************************************
* Function 		: ATCMD_A7672_ModuleHardPowerOff
*
* \b Description: This Function is used to Switch off the Module over Power Key Signal
* PRE-CONDITION	: The System clock and peripherals must be initialized,
* POST-CONDITION: The Module is Turned Off
* @Parameters	: None
* @return		: 1 in case of the Module is Off else if On it will return 0
*
* \b Example Example:
* @code
*	uint8_t ret = ATCMD_A7672_ModuleHardPowerOff();
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

uint8_t ATCMD_A7672_ModuleHardPowerOff(void){
	uint8_t retVal = 0;
	Gpio_Set(A7672_POWER_GPIO, A7672_POWER_PIN);
	if(Gpio_Read(A7672_POWER_STATUS_GPIO, A7672_POWER_STATUS_PIN)==0){
		Gpio_Clear(A7672_RESET_GPIO, A7672_RESET_PIN);
		retVal = 1;
	}
	return retVal;
}



void vending_Machine_ErrorHandlerCallback(uint8_t *value){
	ATCMD_A7672_ModuleHardRestart();
	g_sysHealth.httpConnectStatus = CONNECT_NOT_CONNECTED;
	g_sysHealth.mqttConnectStatus = CONNECT_NOT_CONNECTED;
	ATCMD_A7672_MQTT_Set_Connection_Status(MQTT_NOT_CONNECTED);
	g_sysHealth.nwRegnStatus = NW_REGN_FAILED;
}
