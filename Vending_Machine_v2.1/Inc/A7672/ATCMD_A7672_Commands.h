/****************************************************************************
* Title                 :   ATCMD_A7672_Commands
* Filename              :   ATCMD_A7672_Commands.h
* Author                :   Sijeo Philip
* Origin Date           :   13/05/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F#
* Notes                 :   These module isses the AT Commands to be used
* 							for this application
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
*  13/05/24   1.0.0   Sijeo Philip   Interface Created.
*
*****************************************************************************/
/** @file ATCMD_A7672_Commands.h
 *  @brief This Module issed the AT Commands to the Module for this
 *  		Application.
 *
 *  This is Header files for APIs to issue AT Commands needed for this application
 */

#ifndef __ATCMD_A7672_COMMANDS_H__
#define __ATCMD_A7672_COMMANDS_H__
/******************************************************************************
* Includes
*******************************************************************************/

#include "common_settings.h"
/******************************************************************************
* Preprocessor Constants
*******************************************************************************/

/******************************************************************************
* Configuration Constants
*******************************************************************************/

/******************************************************************************
* Macros
*******************************************************************************/
#ifndef A7672_POWER_PIN
#define A7672_POWER_PIN		11
#endif

#ifndef A7672_RESET_PIN
#define A7672_RESET_PIN		12
#endif

#ifndef A7672_POWER_STATUS_PIN
#define A7672_POWER_STATUS_PIN	7
#endif

/******************************************************************************
* Typedefs
*******************************************************************************/
typedef struct
{
	uint8_t apn[25];		/** << For APN to be Set */
	uint8_t imei[20]; 		/** << For IMEI of Module */
	uint8_t sigStrength;	/** << For Storing the Signal Strenght */
	uint8_t Model[25];	/** << For Storing the Module Model */
	uint8_t smsMode;		/** << For Storing the Current sms Mode */
	uint8_t smsCount;		/** << For Storing the Current SMS Count */
}gsm_gprs_status_t;


/**
 * This Structure is used to store the HTTP Credentials
 */
typedef struct
{
	uint8_t devName[17];		/**<< Device Name Used while device provisioning and Cert/Key Download */
	uint8_t certURL[100];		/**<< URL used to download the certificates */
	uint8_t privKeyURL[100];	/**<< URL used to download the private Key */
	uint8_t pubKeyURL[100];		/**<< URL used to download the Public Key */
	uint8_t devProvURL[100];	/**<< URL used to conduct device Provisioning */
	uint8_t rootCaURL[100];		/**<< URL used to download the RootCA certificate */
}http_creds_t;


/**
 * This Structure is used to store the Certificates stored in the Module.
 */
typedef struct
{
	uint8_t certs[4][25];
	uint8_t certCount;
}ssl_certs_t;

/*
 * This Enum is used for MQTT Connection Status
 */
typedef enum
{
	MQTT_NOT_CONNECTED = 0,
	MQTT_FAILED,
	MQTT_CONNECTED,
	MQTT_DISCONNECTED,
}mqtt_conn_status_t;

/**
 * This Structure is used to Store the Credentials of MQTT Transaction.
 */
typedef void(*msg_handler_t)(uint8_t*);
typedef void(*status_timeout_handler_t)(void);
typedef void(*modem_error_handler_t)(uint8_t* errorCount);

typedef struct
{
	uint8_t mqttURL[100];
	uint8_t mqttPort[10];
	uint8_t mqttSubTopic[100];
	uint8_t mqttPubTopic[100];
	uint32_t status_timeout;
	mqtt_conn_status_t connectionStatus;
	msg_handler_t msg_handler;
	status_timeout_handler_t timeout_handler;
}mqtt_creds_t;


typedef struct
{
	uint8_t errorCountThreshold;
	modem_error_handler_t modem_error_handler;
}modem_error_status_t;
/******************************************************************************
* Variables
*******************************************************************************/
extern char dateTime[22];

/******************************************************************************
* Function Prototypes
*******************************************************************************/
/********************AT COMMANDS TO MODULE *************************************/
uint8_t ATCMD_A7672_Module_Ready( void );
uint8_t ATCMD_A7672_Get_NTP_HostName( void );
uint8_t ATCMD_A7672_DisableAutoTimezoneUpdate( void );
uint8_t ATCMD_A7672_Update_Modem_DateTime( void );
uint8_t ATCMD_A7672_Get_DateTime( void );
uint8_t ATCMD_A7672_Module_Check( void );
uint8_t ATCMD_A7672_Provision_Device( void );
uint8_t ATCMD_A7672_HTTP_Terminate( void );
uint8_t *ATCMD_A7672_HTTP_Read(void);
uint8_t ATCMD_A7672_Download_Certificate( void );
uint8_t ATCMD_A7672_Download_Private_Key( void );
uint8_t ATCMD_A7672_Download_Public_Key( void );
uint8_t ATCMD_A7672_Download_Root_Ca( void );
uint8_t ATCMD_A7672_CertKey_Module_Download( void );
gsm_gprs_status_t* ATCMD_A7672_Get_IMEI_Number( void );
uint16_t ATCMD_A7672_Get_Signal_Strength(void );
ssl_certs_t* ATCMD_A7672_Check_Module_Certificates( void );
uint8_t ATCMD_A7672_Module_On(void);
uint8_t ATCMD_A7672_Module_Off(void);
uint8_t ATCMD_A7672_Module_Reset(void);
uint8_t ATCMD_A7672_Delete_Certificate (uint8_t* certName);
uint8_t ATCMD_A7672_Sslconf_Init(void);
uint16_t ATCMD_A7672_MQTTS_Connect(void);
uint8_t ATCMD_A7672_MQTTS_Disonnect(void);
uint8_t ATCMD_A7672_MQTTS_Publish(char* publishPayload);
uint8_t ATCMD_A7672_MQTTS_Subscribe(void);
uint8_t ATCMD_A7672_Get_DateTime(void);
uint8_t ATCMD_A7672_Check_Pdp_Context( void );
uint8_t ATCMD_A7672_Set_Pdp_Context( void );
/**************************END OF AT COMMAND APIs*******************************/

/**************************HELPER FUNCTIONS ************************************/
uint8_t ATCMD_A7672_HTTP_Set_DevName( const char* devName);
uint8_t * ATCMD_A7672_HTTP_Get_DevName( void );
uint8_t ATCMD_A7672_HTTP_Set_CertURL( const char* certURL );
uint8_t * ATCMD_A7672_HTTP_Get_CertURL( void );
uint8_t ATCMD_A7672_HTTP_Set_PubKeyURL( const char* pubKeyURL);
uint8_t *ATCMD_A7672_HTTP_Get_PubKeyURL( void );
uint8_t ATCMD_A7672_HTTP_Set_PrivKeyURL( const char* privKeyURL );
uint8_t *ATCMD_A7672_HTTP_Get_PrivKeyURL( void );
uint8_t ATCMD_A7672_HTTP_Set_DevProvURL( const char* devProvURL );
uint8_t *ATCMD_A7672_HTTP_Get_DevProvURL( void );
uint16_t ATCMD_A7672_HTTP_Get_Response_Code( void );
uint16_t ATCMD_A7672_HTTP_Get_Bytes_Rcvd ( void );
uint8_t ATCMD_A7672_Is_Sim_Present (void);
uint8_t ATCMD_A7672_Is_Phonebook_Done(void);
uint8_t ATCMD_A7672_HTTP_Set_RootCAURL( const char* rootCAURL );
uint8_t *ATCMD_A7672_HTTP_Get_RootCAURL( void );
uint8_t ATCMD_A7672_Is_Module_Certificate(const char* certName);
uint8_t ATCMD_A7672_MQTT_Set_URL(const char* mqttUrl);
uint8_t* ATCMD_A7672_MQTT_Get_URL(void);
uint8_t ATCMD_A7672_MQTT_Set_Port(const char* mqttPort);
uint8_t* ATCMD_A7672_MQTT_Get_Port(void);
uint8_t ATCMD_A7672_MQTT_Set_SubTopic(const char* mqttSubTopic);
uint8_t* ATCMD_A7672_MQTT_Get_SubTopic( void );
uint8_t ATCMD_A7672_MQTT_Set_PubTopic(const char* mqttPubTopic);
uint8_t* ATCMD_A7672_MQTT_Get_PubTopic( void );
mqtt_conn_status_t ATCMD_A7672_MQTT_Get_Connection_Status( void );
void ATCMD_A7672_MQTT_Set_Connection_Status( mqtt_conn_status_t conn_status);


void ATCMD_A7672_Register_Message_Callback(msg_handler_t msg_handler);
void ATCMD_A7672_Resgister_Status_Send_Callback(status_timeout_handler_t timeout_handler, uint32_t timeout_mins);
void ATCMD_A7672_Register_Error_Callback(modem_error_handler_t error_handler, uint8_t errorCount);
void ATCMD_A7672_Urc_Queue_Init( void );



uint8_t* getSignalStrengthPtr(void);

/***************************END OF HELPER FUNCTION*******************************/

#endif


/*******************************************************END OF FILE **************************************************/
