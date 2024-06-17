/****************************************************************************
* Title                 :   ATCMD_A7672_Callbacks
* Filename              :   ATCMD_A7672_Callbacks.h
* Author                :   Sijeo Philip
* Origin Date           :   16/05/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F#
* Notes                 :   These module contains all the callbacks for the
* 							responses for the AT Commands Issued.
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
*  16/05/24   1.0.0   Sijeo Philip   Interface Created.
*
*****************************************************************************/
/** @file ATCMD_A7672_Commands.h
 *  @brief This Module contains all the callbacks for the Responses for the
 *  		AT Commands issued to the Module.
 *
 *  This is Header file for the Callbacks for the AT Command responses.
 */

#ifndef __ATCMD_A7672_CALLBACKS_H__
#define __ATCMD_A7672_CALLBACKS_H__
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


/******************************************************************************
* Typedefs
*******************************************************************************/

/**
 * This is used to Determine the type of Cert/Key Downloaded
 */
typedef enum {
	NO_FILE = 0,
	CERTIFICATE_FILE,
	PUBLICKEY_FILE,
	PRIVATEKEY_FILE,
	ROOTCA_FILE
}file_type_t;
/***
 * This Structure would be place holder for the SMS Sender Mobile Number
 * and Message
 */

typedef struct
{
	uint8_t mobileNo[15]; 		/** << For Storing the Mobile Number of the Sender*/
	uint8_t txtMessage[150]; 	/** << For Storing the SMS Message of the Sender */
}sms_struct_t;


/**
 * This Structure is the place holder for holding the Response code for the
 * current HTTP Transaction and Bytes to be read;
 */
typedef struct
{
	uint8_t httpResponseCode[4];
	uint8_t httpResponseBytes[5];
	uint16_t bytesToRead;
	uint16_t responseCode;
	file_type_t fileType;
}http_response_t;


/******************************************************************************
* Variables
*******************************************************************************/


/******************************************************************************
* Function Prototypes
*******************************************************************************/
atcmd_rsp_t ATCMD_HttpAction_Get_Callback(uint8_t* rsp);
atcmd_rsp_t ATCMD_Creg_Status_Callback( uint8_t* rsp);
atcmd_rsp_t ATCMD_Cpin_Status_Callback(uint8_t* rsp);
atcmd_rsp_t ATCMD_CNTP_Callback (uint8_t *rsp);
atcmd_rsp_t ATCMD_GetDateTime_Callback (uint8_t *rsp);
atcmd_rsp_t ATCMD_HttpRead_Callback( uint8_t* rsp);
atcmd_rsp_t ATCMD_HttpAction_Get_Callback(uint8_t* rsp);
atcmd_rsp_t ATCMD_CertDownload_Callback( uint8_t* rsp);
atcmd_rsp_t ATCMD_Get_IMEI_Callback (uint8_t *rsp);
atcmd_rsp_t ATCMD_Get_Signal_Strength_Callback(uint8_t* rsp);
atcmd_rsp_t ATCMD_Check_Module_Certs_Callback(uint8_t* rsp);
atcmd_rsp_t ATCMD_MQTT_Start_Callback(uint8_t* rsp);
atcmd_rsp_t ATCMD_MQTT_Accquire_Client_Callback(uint8_t* rsp);
atcmd_rsp_t ATCMD_MQTT_Connect_Callback(uint8_t* rsp);
atcmd_rsp_t ATCMD_MQTT_Subtopic_Callback(uint8_t* rsp);
atcmd_rsp_t ATCMD_MQTT_Subscribe_Callback(uint8_t* rsp);
atcmd_rsp_t ATCMD_MQTT_Pubtopic_Callback(uint8_t* rsp);
atcmd_rsp_t ATCMD_MQTT_PubPayload_Callback(uint8_t* rsp);
atcmd_rsp_t ATCMD_MQTT_Publish_Callback(uint8_t* rsp);
atcmd_rsp_t ATCMD_Pdp_Context_Check_Callback(uint8_t* rsp);

int32_t Urc_Sim_Check_Handler(uint8_t *rspStr, uint16_t len);
int32_t Urc_Module_Ready_Handler(uint8_t *rspStr, uint16_t len);
int32_t Urc_Module_Data_Handler(uint8_t* rspStr, uint16_t len);
int32_t Urc_Module_Mqtt_Conn_Handler(uint8_t* rspStr, uint16_t len);
#endif


/*******************************************************END OF FILE **************************************************/
