/*******************************************************************************
* Title                 :   ATCMD_A7672_Callbacks
* Filename              :   ATCMD_A7672_Callbacks.c
* Author                :   Sijeo Philip
* Origin Date           :   16/05/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F3x
* Notes                 :   This module is Callbacks for the Responses of AT Commands
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
*  16/05/2024   1.0.0    Sijeo Philip   Initial Release.
*
*******************************************************************************/
/** @file ATCMD_Core.c
 *  @brief This is the source file for AT Command Response Callbacks.
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "ATCMD_Queue_Mgmt.h"
#include "ATCMD_A7672_Callbacks.h"
#include "ATCMD_A7672_Commands.h"
#include "uart.h"
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/
http_response_t g_httpResponse;
gsm_gprs_status_t g_moduleStatus;
ssl_certs_t g_certificates;
uint8_t g_errorCount=0;
/******************************************************************************
* Module Global/Extern Variable Definitions
*******************************************************************************/
uint8_t g_pbDone = 0;
uint8_t g_simPresent = 0;
uint16_t g_mqtt_result;
extern mqtt_creds_t s_mqttCredentials;
extern char ntpHostName[20];
extern char dateTime[22];
/******************************************************************************
* Module Static Variable Definitions
*******************************************************************************/
static uint8_t* s_tmpStr;
static uint8_t* s_txBuf;
static uint8_t s_tempBuff[50];
static uint16_t s_bytesToRead = 0;
static uint16_t s_byteCount = 0;
static uint8_t s_pubBuff[500];

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/

/******************************************************************************
* Function : ATCMD_Default_Callback()
*//**
* \b Description:
*
* This function is used as response callback which has responses as "OK" or "ERROR"
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
* PRE-CONDITION: The AT command is being issued and response awaited
*
* POST-CONDITION: The callback will be called on AT Commands response receipt
*
* @param[in] the address to the response received from the MODEM
* @return 	enum on the the received response.
*
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
atcmd_rsp_t ATCMD_Default_Callback(uint8_t *rsp)
{
	atcmd_rsp_t ret = ATCMD_RSP_CONTINUE;
	char* rspStrTable[2] = {"OK", "ERROR"};
	uint8_t i = 0;
	int8_t rspType = -1;
	uint8_t* p = rsp;
	while(p)
	{
		/*Ignore /r/n*/
		while( ( ATCMD_CMD_CR == *p) || ( ATCMD_CMD_LF == *p ))
		{
			p++;
		}
		for(i = 0; i < 2; i++)
		{
			if(!strncmp((char*)rspStrTable[i], (char*)p, strlen((char*)rspStrTable[i]) ))
			{
				rspType = (int8_t)i;
				break;
			}
		}
		p = (uint8_t*)strchr((char*)p, 0x0a);
	}
	switch(rspType)
	{
	case 0:
		ret = ATCMD_RSP_FINISH;
		g_errorCount = 0;
		break;
	case 1:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	default:
		g_errorCount++;
		ret = ATCMD_RSP_ERROR;
		break;

	}
	return ret;
}

/******************************************************************************
* Function : ATCMD_Cpin_Status_Callback()
*//**
* \b Description:
*
* This function is used as response callback to AT+CPIN?
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
* PRE-CONDITION: The AT command is being issued and response awaited
*
* POST-CONDITION: The callback will be called on AT Commands response receipt
*
* @param[in] the address to the response received from the MODEM
* @return 	enum on the the received response.
*
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
atcmd_rsp_t ATCMD_Cpin_Status_Callback(uint8_t* rsp)
{
	atcmd_rsp_t volatile ret = ATCMD_RSP_CONTINUE;
	char* volatile rspStrTable[3] = {"+CPIN: READY", "+CPIN: NOT READY", "ERROR"};
	uint8_t volatile i = 0;
	int32_t volatile rspType = -1;
	uint8_t* volatile p = rsp;
	while(p)
	{
		/*Ignore /r/n */
		while((ATCMD_CMD_CR == *p) || (ATCMD_CMD_LF == *p))
		{
			p++;
		}
		for ( i =0; i < 3; i++ )
		{
			if(!strncmp((char*)rspStrTable[i], (char*)p, strlen((char*)rspStrTable[i])))
			{
				rspType =i;
				break;
			}
		}
		p=(uint8_t*)strchr((char*)p, 0x0a);
	}
	switch(rspType)
	{
	case 0:
		ret = ATCMD_RSP_FINISH;
		g_errorCount=0;
		break;
	case 1:
		ret = ATCMD_RSP_ERROR;
		g_errorCount=0;
		break;
	case 2:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	default:
		g_errorCount++;
		ret = ATCMD_RSP_ERROR;
		break;
	}
	return ret;

}

/******************************************************************************
* Function : ATCMD_Creg_Status_Callback()
*//**
* \b Description:
*
* This function is used as response callback to AT+CREG?
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
* PRE-CONDITION: The AT command is being issued and response awaited
*
* POST-CONDITION: The callback will be called on AT Commands response receipt
*
* @param[in] the address to the response received from the MODEM
* @return 	enum on the the received response.
*
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

atcmd_rsp_t ATCMD_Creg_Status_Callback( uint8_t* rsp)
{
	atcmd_rsp_t volatile ret = ATCMD_RSP_CONTINUE;
	char* volatile rspStrTable[3] = {"+CREG: 0,1", "+CREG: 0,5", "ERROR"};
	uint8_t volatile i = 0;
	uint32_t volatile rspType = -1;
	uint8_t* volatile p = rsp;

	while(p)
	{
		/**Ignore /r/n**/
		while( (ATCMD_CMD_CR == *p) || (ATCMD_CMD_LF == *p) )
		{
			p++;
		}
		for( i = 0; i < 3; i++ )
		{
			if(!strncmp((char*)rspStrTable[i], (char*)p, strlen((char*)rspStrTable[i])))
			{
				rspType = i;
				break;
			}
		}
		p = (uint8_t*)strchr((char*)p, 0x0a);
	}
	switch(rspType)
	{
	case 0:
	case 1:
		ret = ATCMD_RSP_FINISH;
		g_errorCount=0;
		break;
	case 2:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	default:
		g_errorCount++;
		ret = ATCMD_RSP_ERROR;
		break;
	}
	return ret;

}
/******************************************************************************
* Function : ATCMD_CNTP_Callback()
*//**
* \b Description: This function is used as response callback to ATI*
* PRE-CONDITION	: UART should be configured and enabled
* PRE-CONDITION	: The Scheduler should be enabled.
* PRE-CONDITION	: The AT command is being issued and response awaited*
* POST-CONDITION: The callback will be called on AT Commands response receipt
*
* @param[in] the address to the response received from the MODEM
* @return 	enum on the the received response.
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
atcmd_rsp_t ATCMD_CNTP_Callback (uint8_t *rsp){
	atcmd_rsp_t volatile ret = ATCMD_RSP_ERROR;
    char* volatile rspStrTable[3] = {"+CNTP: ", "OK","ERROR"};
	uint8_t volatile i  = 0;
	int32_t volatile rspType = -1;
	uint8_t* volatile p = rsp;
	while(p){
		while ( ATCMD_CMD_CR == *p || ATCMD_CMD_LF == *p ){
			p++;
		}
		for ( i = 0; i < 3; i ++){
			if ( !strncmp(rspStrTable[i], ( char * )p, strlen(rspStrTable[i])))	{
				rspType = i;
				break;
			}
		}
		if(rspType == 0){
			s_tmpStr = p;
			memset(s_tempBuff, '\0', sizeof(s_tempBuff));
			memset(ntpHostName, '\0', 20);
			s_tmpStr = Str_Cpy_Marker(s_tmpStr, &s_tempBuff[0], ' ', ATCMD_CMD_CR);
			strncpy(ntpHostName, (char*)s_tempBuff, strlen((char*)s_tempBuff));
			break;
		}
		p = (uint8_t*)strchr((char*)p, 0x0a);
	}

	switch(rspType)	{
	case 0:
	case 1:
		ret = ATCMD_RSP_FINISH;
		g_errorCount = 0;
		break;
	case 2:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	default:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	}
	return ret;
}
/******************************************************************************
* Function : ATCMD_GetDateTime_Callback()
*//**
* \b Description: Callback to process AT+CCLK. Extracts DateTime string and saves into dateTime Array.
* PRE-CONDITION	: UART should be configured and enabled
* PRE-CONDITION	: The Scheduler should be enabled.
* PRE-CONDITION	: The AT+CCLK command is being issued and response awaited
* POST-CONDITION: The callback will be called on AT Commands response receipt
*
* @param[in] the address to the response received from the MODEM
* @return 	enum on the the received response.
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
atcmd_rsp_t ATCMD_GetDateTime_Callback (uint8_t *rsp){
	atcmd_rsp_t volatile ret = ATCMD_RSP_ERROR;
    char* volatile rspStrTable[3] = {"+CCLK: ", "OK", "ERROR"};
	uint8_t volatile i  = 0;
	int32_t volatile rspType = -1;
	uint8_t* volatile p = rsp;
	while(p){
		while ( ATCMD_CMD_CR == *p || ATCMD_CMD_LF == *p ){
			p++;
		}
		for ( i = 0; i < 3; i ++){
			if ( !strncmp(rspStrTable[i], ( char * )p, strlen(rspStrTable[i]))){
				rspType = i;
				break;
			}
		}
		if(rspType == 0){
			s_tmpStr = p;
			memset(s_tempBuff, '\0', sizeof(s_tempBuff));
			memset(dateTime, '\0', 22);
			s_tmpStr = Str_Cpy_Marker(s_tmpStr, &s_tempBuff[0], '"', '"');
			strncpy(dateTime, (char*)s_tempBuff, strlen((char*)s_tempBuff));
			break;
		}
		p = (uint8_t*)strchr((char*)p, 0x0a);
	}
	switch(rspType){
	case 0:
	case 1:
		ret = ATCMD_RSP_FINISH;
		g_errorCount = 0;
		break;
	case 2:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	default:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	}
	return ret;
}

/******************************************************************************
* Function : ATCMD_HttpAction_Callback()
*//**
* \b Description:
*
* This function is used as response callback to AT+HTTPACTION=1
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
* PRE-CONDITION: The AT command is being issued and response awaited
*
* POST-CONDITION: The callback will be called on AT Commands response receipt
*
* @param[in] the address to the response received from the MODEM
* @return 	enum on the the received response.
*
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


atcmd_rsp_t ATCMD_HttpAction_Get_Callback(uint8_t* rsp)
{
	atcmd_rsp_t volatile ret = ATCMD_RSP_CONTINUE;
	char* volatile rspStrTable[3] = {"OK", "+HTTPACTION: ", "ERROR"};
	uint8_t volatile i = 0;
	uint32_t volatile rspType = -1;
	uint8_t* volatile p = rsp;

	while(p)
	{
		/**Ignore /r/n**/
		while( (ATCMD_CMD_CR == *p) || (ATCMD_CMD_LF == *p) )
		{
			p++;
		}
		for( i = 0; i < 3; i++ )
		{
			if(!strncmp((char*)rspStrTable[i], (char*)p, strlen((char*)rspStrTable[i])))
			{
				rspType = i;
				if( rspType == 1 )
				{
					s_tmpStr = p;
				}
				break;
			}
		}
		p = (uint8_t*)strchr((char*)p, 0x0a);
	}
	switch(rspType)
	{
	case 0:
		ret = ATCMD_RSP_WAIT;
		break;
	case 1:
		ret = ATCMD_RSP_FINISH;
		s_tmpStr = Str_Cpy_Marker(s_tmpStr, &g_httpResponse.httpResponseCode[0], ',', ',');
		s_tmpStr = Str_Cpy_Marker(s_tmpStr, &g_httpResponse.httpResponseBytes[0], ',', ATCMD_CMD_CR);
		g_httpResponse.bytesToRead = Convert_String_To_Integer(&g_httpResponse.httpResponseBytes[0]);
		g_httpResponse.responseCode = Convert_String_To_Integer(&g_httpResponse.httpResponseCode[0]);
		g_errorCount = 0;
		break;
	case 2:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	default:
		g_errorCount++;
		ret = ATCMD_RSP_ERROR;
		break;
	}
return ret;
}


/******************************************************************************
* Function : ATCMD_HttpAction_Callback()
*//**
* \b Description:
*
* This function is used as response callback to AT+HTTPREAD=0,<data bytes>
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
* PRE-CONDITION: The AT command is being issued and response awaited
*
* POST-CONDITION: The callback will be called on AT Commands response receipt
*
* @param[in] the address to the response received from the MODEM
* @return 	enum on the the received response.
*
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
atcmd_rsp_t ATCMD_HttpRead_Callback( uint8_t* rsp)
{
	atcmd_rsp_t volatile ret = ATCMD_RSP_CONTINUE;
	char* volatile rspStrTable[3] = {"OK", "+HTTPREAD: ", "ERROR"};
	uint8_t volatile i = 0;
	uint32_t volatile rspType = -1;
	uint8_t* volatile p = rsp;
	while(p)
	{
		/**Ignore /r/n**/
		while( (ATCMD_CMD_CR == *p) || (ATCMD_CMD_LF == *p) )
		{
			p++;
		}
		for( i = 0; i < 3; i++ )
		{
			if(!strncmp((char*)rspStrTable[i], (char*)p, strlen((char*)rspStrTable[i])))
			{
				rspType = i;
				if( rspType == 1 )
				{
					s_tmpStr = p;
				}
				break;
			}
		}
		if((rspType == 1) || (rspType == -1))
		{
			break;
		}
		p = (uint8_t*)strchr((char*)p, 0x0a);
	}
	switch(rspType)
	{
	case 0:
		ret = ATCMD_RSP_FINISH;
		break;
	case 1:
		ret = ATCMD_RSP_FINISH;
		s_tmpStr = (uint8_t*)strchr((char*)s_tmpStr, ATCMD_CMD_LF);
		s_tmpStr++;
		s_txBuf = Uart_Get_Transmit_Buffer();
		memset(s_txBuf, '\0', A7672_UART_TX_BUFFSIZE_CONF);
		s_bytesToRead = g_httpResponse.bytesToRead;

		while(strncmp((char*)s_tmpStr, "+HTTPREAD: 0", 12) != 0)
		{
			if(!strncmp((char*)s_tmpStr, "+HTTPREAD: ", 10))
			{
				s_tmpStr = (uint8_t*)strchr((char*)s_tmpStr, ATCMD_CMD_LF);
				s_tmpStr++;
			}
			if( s_bytesToRead >= 1024 )
			{
				s_byteCount = 1024;
			}
			else
			{
				s_byteCount = s_bytesToRead;
			}
			s_bytesToRead = s_bytesToRead - s_byteCount;
			while(s_byteCount != 0)
			{
				*s_txBuf = *s_tmpStr;
				s_txBuf++;
				s_tmpStr++;
				s_byteCount--;
			}

			while((*s_tmpStr == ATCMD_CMD_LF) || (*s_tmpStr == ATCMD_CMD_CR))
			{
				s_tmpStr++;
			}
		}
		g_errorCount = 0;
		break;
	case 2:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	default:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	}
	return ret;
}

/******************************************************************************
* Function : ATCMD_CertDownload_Callback()
*//**
* \b Description:
*
* This function is used as response callback to AT+CCERTDOWN=<file name>,<len>
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
* PRE-CONDITION: The AT command is being issued and response awaited
*
* POST-CONDITION: The callback will be called on AT Commands response receipt
*
* @param[in] the address to the response received from the MODEM
* @return 	enum on the the received response.
*
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

atcmd_rsp_t ATCMD_CertDownload_Callback( uint8_t* rsp)
{
	atcmd_rsp_t volatile ret = ATCMD_RSP_ERROR;
	char* volatile rspStrTable[3] = {">", "OK", "ERROR"};
	uint8_t volatile i  = 0;
	int32_t volatile rspType = -1;
	uint8_t* volatile p = rsp;

	while ( ATCMD_CMD_CR == *p || ATCMD_CMD_LF == *p )
	{
		p++;
	}
	for ( i = 0; i < 3; i ++)
	{
		if ( !strncmp(rspStrTable[i], ( char * )p, strlen(rspStrTable[i])))
		{
			rspType = i;
			break;
		}

	}
	switch(rspType)
	{
	case 0:
		ret = ATCMD_RSP_WAIT;
		p = Uart_Get_Transmit_Buffer();
		Uart_Transmit_Data_Dma((uint32_t)p, g_httpResponse.bytesToRead);
		break;
	case 1:
		ret = ATCMD_RSP_FINISH;
		g_errorCount = 0;
		break;
	case 2:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	default:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	}
	return ret;
}



/******************************************************************************
* Function : ATCMD_Get_IMEI_Callback()
*//**
* \b Description:
*
* This function is used as response callback to ATI
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
* PRE-CONDITION: The AT command is being issued and response awaited
*
* POST-CONDITION: The callback will be called on AT Commands response receipt
*
* @param[in] the address to the response received from the MODEM
* @return 	enum on the the received response.
*
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
atcmd_rsp_t ATCMD_Get_IMEI_Callback (uint8_t *rsp)
{
	atcmd_rsp_t volatile ret = ATCMD_RSP_ERROR;
    char* volatile rspStrTable[5] = {"Revision: ", "IMEI: ", "+GCAP: ", "OK", "ERROR"};
	uint8_t volatile i  = 0;
	int32_t volatile rspType = -1;
	uint8_t* volatile p = rsp;
	while(p)
	{
		while ( ATCMD_CMD_CR == *p || ATCMD_CMD_LF == *p )
		{
			p++;
		}
		for ( i = 0; i < 5; i ++)
		{
			if ( !strncmp(rspStrTable[i], ( char * )p, strlen(rspStrTable[i])))
			{
				rspType = i;
				break;
			}
		}
		if(rspType == 1)
		{
			s_tmpStr = p;
			memset(s_tempBuff, '\0', sizeof(s_tempBuff));
			s_tmpStr = Str_Cpy_Marker(s_tmpStr, &s_tempBuff[0], ' ', ATCMD_CMD_CR);
			ATCMD_A7672_HTTP_Set_DevName((char*) s_tempBuff);
			memset(g_moduleStatus.imei, '\0', sizeof(g_moduleStatus.imei));
			strncpy((char*)g_moduleStatus.imei, (char*)s_tempBuff, strlen((char*)s_tempBuff));
		}
		if(rspType == 0)
		{
			s_tmpStr = p;
			memset(s_tempBuff, '\0', sizeof(s_tempBuff));
			memset(g_moduleStatus.Model, '\0', sizeof(g_moduleStatus.Model));
			s_tmpStr = Str_Cpy_Marker(s_tmpStr, &s_tempBuff[0], ' ', ATCMD_CMD_CR);
			strncpy((char*)g_moduleStatus.Model, (char*)s_tempBuff, strlen((char*)s_tempBuff));
		}
		p = (uint8_t*)strchr((char*)p, 0x0a);
	}

	switch(rspType)
	{
	case 0:
		ret = ATCMD_RSP_FINISH;
		g_errorCount = 0;
		break;
	case 1:
	case 2:
	case 3:
		ret = ATCMD_RSP_FINISH;
		g_errorCount=0;
		break;
	case 4:
	default:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	}
	return ret;
}


/******************************************************************************
* Function : ATCMD_Get_Signal_Strength_Callback()
*//**
* \b Description:
*
* This function is used as response callback to AT+CSQ
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
* PRE-CONDITION: The AT command is being issued and response awaited
*
* POST-CONDITION: The callback will be called on AT Commands response receipt
*
* @param[in] the address to the response received from the MODEM
* @return 	enum on the the received response.
*
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

atcmd_rsp_t ATCMD_Get_Signal_Strength_Callback(uint8_t* rsp)
{
	atcmd_rsp_t volatile ret = ATCMD_RSP_ERROR;
	char* volatile rspStrTable[3] = {"+CSQ: ", "ERROR"};
	uint8_t volatile i  = 0;
	int32_t volatile rspType = -1;
	uint8_t* volatile p = rsp;
	while ( ATCMD_CMD_CR == *p || ATCMD_CMD_LF == *p )
	{
		p++;
	}
	for ( i = 0; i < 2; i ++)
	{
		if ( !strncmp(rspStrTable[i], ( char * )p, strlen(rspStrTable[i])))
		{
			rspType = i;

			break;
		}
	}

	switch(rspType)
	{
	case 0:
		ret = ATCMD_RSP_FINISH;
		memset(s_tempBuff, '\0', sizeof(s_tempBuff));
		s_tmpStr = Str_Cpy_Marker(p, &s_tempBuff[0], ' ', ',');
		g_moduleStatus.sigStrength = Convert_String_To_Integer(s_tempBuff);
		g_errorCount = 0;
		break;
	case 1:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
 		break;
	default:
   		ret = ATCMD_RSP_ERROR;
   		g_errorCount++;
		break;
	}
	return ret;
}

/******************************************************************************
* Function : ATCMD_MQTT_Start_Callback()
*//**
* \b Description:
*
* This function is used as response callback to AT+CMQTTSTART
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
* PRE-CONDITION: The AT command is being issued and response awaited
*
* POST-CONDITION: The callback will be called on AT Commands response receipt
*
* @param[in] the address to the response received from the MODEM
* @return 	enum on the the received response.
*
*@Note: This should be called only after downloading the Certificates and Keys
*@Note: and calling ATCMD_A7672_Ssl_Conf_Init API Successfully
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
atcmd_rsp_t ATCMD_MQTT_Start_Callback(uint8_t* rsp)
{
	atcmd_rsp_t volatile ret = ATCMD_RSP_ERROR;
	char* volatile rspStrTable[3] = {"OK","+CMQTTSTART: ", "ERROR"};
	uint8_t volatile i  = 0;
	int32_t volatile rspType = -1;
	uint8_t* volatile p = rsp;

	while(p)
	{
		while ( ATCMD_CMD_CR == *p || ATCMD_CMD_LF == *p )
		{
			p++;
		}
		for ( i = 0; i < 3; i ++)
		{
			if ( !strncmp(rspStrTable[i], ( char * )p, strlen(rspStrTable[i])))
			{
				rspType = i;
				break;
			}

		}
		if(rspType == 1)
		{
			memset(s_tempBuff, '\0', sizeof(s_tempBuff));
			Str_Cpy_Marker(p, &s_tempBuff[0], ' ', ATCMD_CMD_CR);
			g_mqtt_result = Convert_String_To_Integer(s_tempBuff);
			break;
		}
		p = (uint8_t*)strchr((char*)p, 0x0a);
	}
	switch(rspType)
	{
	case 0:
		ret = ATCMD_RSP_FINISH;
		g_errorCount = 0;
		break;
	case 1:
		if(g_mqtt_result != 0)
		{
			ret = ATCMD_RSP_ERROR;
		}
		else
		{
			ret = ATCMD_RSP_FINISH;
			g_errorCount=0;
		}

		break;
	case 2:
	default:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	}
	return ret;
}

/******************************************************************************
* Function : ATCMD_MQTT_Accquire_Client_Callback()
*//**
* \b Description:
*
* This function is used as response callback to AT+CMQTTACCQ
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
* PRE-CONDITION: The AT command is being issued and response awaited
*
* POST-CONDITION: The callback will be called on AT Commands response receipt
*
* @param[in] the address to the response received from the MODEM
* @return 	enum on the the received response.
*
*@Note: This should be called only after downloading the Certificates and Keys
*@Note: and calling ATCMD_A7672_Ssl_Conf_Init API Successfully
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
atcmd_rsp_t ATCMD_MQTT_Accquire_Client_Callback(uint8_t* rsp)
{
	atcmd_rsp_t volatile ret = ATCMD_RSP_ERROR;
	char* volatile rspStrTable[3] = {"OK","+CMQTTACCQ: ", "ERROR"};
	uint8_t volatile i  = 0;
	int32_t volatile rspType = -1;
	uint8_t* volatile p = rsp;
	while(p)
	{
		while ( ATCMD_CMD_CR == *p || ATCMD_CMD_LF == *p )
		{
			p++;
		}
		for ( i = 0; i < 3; i ++)
		{
			if ( !strncmp(rspStrTable[i], ( char * )p, strlen(rspStrTable[i])))
			{
				rspType = i;
				break;
			}

		}
		p = (uint8_t*)strchr((char*)p, 0x0a);
	}
	switch(rspType)
	{
	case 0:
		ret = ATCMD_RSP_FINISH;
		g_errorCount = 0;
		break;
	case 1:
		ret = ATCMD_RSP_ERROR;
		break;
	case 2:
	default:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	}
	return ret;
}


/******************************************************************************
* Function : ATCMD_MQTT_Connect_Callback()
*//**
* \b Description:
*
* This function is used as response callback to AT+CMQTTCONNECT
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
* PRE-CONDITION: The AT command is being issued and response awaited
*
* POST-CONDITION: The callback will be called on AT Commands response receipt
*
* @param[in] the address to the response received from the MODEM
* @return 	enum on the the received response.
*
*@Note: This should be called only after downloading the Certificates and Keys
*@Note: and calling ATCMD_A7672_Ssl_Conf_Init API Successfully
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
atcmd_rsp_t ATCMD_MQTT_Connect_Callback(uint8_t* rsp)
{
	atcmd_rsp_t volatile ret = ATCMD_RSP_ERROR;
	char* volatile rspStrTable[3] = {"OK","+CMQTTCONNECT: 0,", "ERROR"};
	uint8_t volatile i  = 0;
	int32_t volatile rspType = -1;
	uint8_t* volatile p = rsp;
	while(p)
	{
		while ( ATCMD_CMD_CR == *p || ATCMD_CMD_LF == *p )
		{
			p++;
		}
		for ( i = 0; i < 3; i ++)
		{
			if ( !strncmp(rspStrTable[i], ( char * )p, strlen(rspStrTable[i])))
			{
				rspType = i;
				break;
			}
		}
		if(rspType == 1)
		{
			memset(s_tempBuff, '\0', sizeof(s_tempBuff));
			Str_Cpy_Marker(p, &s_tempBuff[0], ',', ATCMD_CMD_CR);
			g_mqtt_result = Convert_String_To_Integer(s_tempBuff);
			break;
		}
		p = (uint8_t*)strchr((char*)p, 0x0a);
	}
	switch(rspType)
	{
	case 0:
		ret = ATCMD_RSP_WAIT;
		break;
	case 1:
		if( g_mqtt_result == 0)
		{
			ret = ATCMD_RSP_FINISH;
			g_errorCount = 0;
		}
		else
			ret = ATCMD_RSP_ERROR;
		break;
	case 2:
	default:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	}
	return ret;
}

/******************************************************************************
* Function : ATCMD_MQTT_Subtopic_Callback()
*//**
* \b Description:
*
* This function is used as response callback to AT+CMQTTSUBTOPIC
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
* PRE-CONDITION: The AT command is being issued and response awaited
*
* POST-CONDITION: The callback will be called on AT Commands response receipt
*
* @param[in] the address to the response received from the MODEM
* @return 	enum on the the received response.
*
*@Note: This should be called only after downloading the Certificates and Keys
*@Note: and calling ATCMD_A7672_Ssl_Conf_Init API Successfully
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
atcmd_rsp_t ATCMD_MQTT_Subtopic_Callback(uint8_t* rsp)
{
	atcmd_rsp_t volatile ret = ATCMD_RSP_ERROR;
	char* volatile rspStrTable[4] = {">","OK","+CMQTTSUBTOPIC: ", "ERROR"};
	uint8_t volatile i  = 0;
	int32_t volatile rspType = -1;
	uint8_t* volatile p = rsp;
	while(p)
	{
		while ( ATCMD_CMD_CR == *p || ATCMD_CMD_LF == *p )
		{
			p++;
		}
		for ( i = 0; i < 4; i ++)
		{
			if ( !strncmp(rspStrTable[i], ( char * )p, strlen(rspStrTable[i])))
			{
				rspType = i;
				break;
			}
		}
		p = (uint8_t*)strchr((char*)p, 0x0a);
	}
	switch(rspType)
	{
	case 0:

		ret = ATCMD_RSP_WAIT;
		s_txBuf = ATCMD_A7672_MQTT_Get_SubTopic();
		strcat((char*)s_txBuf, ATCMD_CMD_END);
		Uart_Transmit_Data_Dma((uint32_t)s_txBuf, strlen((char*)s_txBuf));
		break;
	case 1:
		ret = ATCMD_RSP_FINISH;
		g_errorCount = 0;
		break;
	case 2:
		ret = ATCMD_RSP_ERROR;
		break;

	case 3:
	default:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	}
	return ret;
}

/******************************************************************************
* Function : ATCMD_MQTT_Subscribe_Callback()
*//**
* \b Description:
*
* This function is used as response callback to AT+CMQTTSUB
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
* PRE-CONDITION: The AT command is being issued and response awaited
*
* POST-CONDITION: The callback will be called on AT Commands response receipt
*
* @param[in] the address to the response received from the MODEM
* @return 	enum on the the received response.
*
*@Note: This should be called only after downloading the Certificates and Keys
*@Note: and calling ATCMD_A7672_Ssl_Conf_Init API Successfully
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
atcmd_rsp_t ATCMD_MQTT_Subscribe_Callback(uint8_t* rsp)
{
	atcmd_rsp_t volatile ret = ATCMD_RSP_ERROR;
	char* volatile rspStrTable[3] = {"OK","+CMQTTSUB: ", "ERROR"};
	uint8_t volatile i  = 0;
	int32_t volatile rspType = -1;
	uint8_t* volatile p = rsp;
	static uint16_t mqtt_result;
	while(p)
	{
		while ( ATCMD_CMD_CR == *p || ATCMD_CMD_LF == *p )
		{
			p++;
		}
		for ( i = 0; i < 3; i ++)
		{
			if ( !strncmp(rspStrTable[i], ( char * )p, strlen(rspStrTable[i])))
			{
				rspType = i;
				break;
			}
		}
		if(rspType == 1)
		{
			memset(s_tempBuff, '\0', sizeof(s_tempBuff));
			Str_Cpy_Marker(p, &s_tempBuff[0], ',', ATCMD_CMD_CR);
			mqtt_result = Convert_String_To_Integer(s_tempBuff);
			break;
		}
		p = (uint8_t*)strchr((char*)p, 0x0a);
	}
	switch(rspType)
	{
	case 0:
		ret = ATCMD_RSP_WAIT;
		break;
	case 1:
		if(mqtt_result != 0)
		{
			ret = ATCMD_RSP_ERROR;
		}
		else
		{
			ret = ATCMD_RSP_FINISH;
			g_errorCount = 0;
		}

		break;
	case 2:
	default:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	}
	return ret;
}
/******************************************************************************
* Function : ATCMD_Check_Module_Certs_Callback()
*//**
* \b Description:
*
* This function is used as response callback to AT+CCERTLIST
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
* PRE-CONDITION: The AT command is being issued and response awaited
*
* POST-CONDITION: The callback will be called on AT Commands response receipt
*
* @param[in] the address to the response received from the MODEM
* @return 	enum on the the received response.
*
**
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 07/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
atcmd_rsp_t ATCMD_Check_Module_Certs_Callback(uint8_t* rsp)
{
	atcmd_rsp_t volatile ret = ATCMD_RSP_ERROR;
	char* volatile rspStrTable[3] = {"+CCERTLIST: ","OK", "ERROR"};
	uint8_t volatile i  = 0;
	int32_t volatile rspType = -1;
	uint8_t* volatile p = rsp;
	static uint8_t count;
	count = 0;
	while(p)
	{
		while ( ATCMD_CMD_CR == *p || ATCMD_CMD_LF == *p )
		{
			p++;
		}
		for ( i = 0; i < 3; i ++)
		{
			if ( !strncmp(rspStrTable[i], ( char * )p, strlen(rspStrTable[i])))
			{
				rspType = i;
				break;
			}

		}
		if(rspType == 0)
		{
			memset(s_tempBuff, '\0', sizeof(s_tempBuff));
			Str_Cpy_Marker(p, &s_tempBuff[0], '\"', '\"');
			strncpy((char*)&g_certificates.certs[count][0], (char*)s_tempBuff, strlen((char*)s_tempBuff));
			count++;
		}
		p = (uint8_t*)strchr((char*)p, 0x0a);
	}
	switch(rspType)
	{
	case 0:
		ret = ATCMD_RSP_FINISH;
		g_certificates.certCount = count;
		g_errorCount = 0;
		break;
	case 1:
		ret = ATCMD_RSP_FINISH;
		g_certificates.certCount = count;
		g_errorCount = 0;
		break;
	case 2:
	default:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	}
	return ret;
}

/******************************************************************************
* Function : ATCMD_MQTT_Pubtopic_Callback()
*//**
* \b Description:
*
* This function is used as response callback to AT+CMQTTTOPIC
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
* PRE-CONDITION: The AT command is being issued and response awaited
*
* POST-CONDITION: The callback will be called on AT Commands response receipt
*
* @param[in] the address to the response received from the MODEM
* @return 	enum on the the received response.
*
*@Note: This should be called only after downloading the Certificates and Keys
*@Note: and calling ATCMD_A7672_Ssl_Conf_Init API Successfully
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
atcmd_rsp_t ATCMD_MQTT_Pubtopic_Callback(uint8_t* rsp)
{
	atcmd_rsp_t volatile ret = ATCMD_RSP_ERROR;
	char* volatile rspStrTable[4] = {">","OK","+CMQTTTOPIC: ", "ERROR"};
	uint8_t volatile i  = 0;
	int32_t volatile rspType = -1;
	uint8_t* volatile p = rsp;
	while(p)
	{
		while ( ATCMD_CMD_CR == *p || ATCMD_CMD_LF == *p )
		{
			p++;
		}
		for ( i = 0; i < 4; i ++)
		{
			if ( !strncmp(rspStrTable[i], ( char * )p, strlen(rspStrTable[i])))
			{
				rspType = i;
				break;
			}
		}
		p = (uint8_t*)strchr((char*)p, 0x0a);
	}
	switch(rspType)
	{
	case 0:
		ret = ATCMD_RSP_WAIT;
		s_txBuf = ATCMD_A7672_MQTT_Get_PubTopic();
		memset(s_tempBuff, '\0', sizeof(s_tempBuff));
		strcpy((char*)s_tempBuff,(char*)s_txBuf);
		strcat((char*)s_tempBuff, "\r");
		Uart_Transmit_Data_Dma((uint32_t)s_tempBuff, strlen((char*)s_tempBuff));
		break;
	case 1:
		ret = ATCMD_RSP_FINISH;
		g_errorCount=0;
		break;
	case 2:
		ret = ATCMD_RSP_ERROR;
		break;
	case 3:
	default:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	}
	return ret;
}


/******************************************************************************
* Function : ATCMD_MQTT_PubPayload_Callback()
*//**
* \b Description:
*
* This function is used as response callback to AT+CMQTTPAYLOAD
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
* PRE-CONDITION: The AT command is being issued and response awaited
*
* POST-CONDITION: The callback will be called on AT Commands response receipt
*
* @param[in] the address to the response received from the MODEM
* @return 	enum on the the received response.
*
*@Note: This should be called only after downloading the Certificates and Keys
*@Note: and calling ATCMD_A7672_Ssl_Conf_Init API Successfully
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
atcmd_rsp_t ATCMD_MQTT_PubPayload_Callback(uint8_t* rsp)
{
	atcmd_rsp_t volatile ret = ATCMD_RSP_ERROR;
	char* volatile rspStrTable[4] = {">","OK","+CMQTTPAYLOAD: ", "ERROR"};
	uint8_t volatile i  = 0;
	int32_t volatile rspType = -1;
	uint8_t* volatile p = rsp;
	while(p)
	{
		while ( ATCMD_CMD_CR == *p || ATCMD_CMD_LF == *p )
		{
			p++;
		}
		for ( i = 0; i < 4; i ++)
		{
			if ( !strncmp(rspStrTable[i], ( char * )p, strlen(rspStrTable[i])))
			{
				rspType = i;
				break;
			}
		}
		p = (uint8_t*)strchr((char*)p, 0x0a);
	}
	switch(rspType)
	{
	case 0:
		ret = ATCMD_RSP_WAIT;
		s_txBuf = Uart_Get_Transmit_Buffer();
		strcat((char*)s_txBuf, ATCMD_CMD_END);
		Uart_Transmit_Data_Dma((uint32_t)s_txBuf, strlen((char*)s_txBuf));
		break;
	case 1:
		ret = ATCMD_RSP_FINISH;
		g_errorCount = 0;
		break;
	case 2:
		ret = ATCMD_RSP_ERROR;
		break;
	case 3:
	default:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	}
	return ret;
}

/******************************************************************************
* Function : ATCMD_MQTT_Publish_Callback()
*//**
* \b Description:
*
* This function is used as response callback to AT+CMQTTPUB
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
* PRE-CONDITION: The AT command is being issued and response awaited
*
* POST-CONDITION: The callback will be called on AT Commands response receipt
*
* @param[in] the address to the response received from the MODEM
* @return 	enum on the the received response.
*
*@Note: This should be called only after downloading the Certificates and Keys
*@Note: and calling ATCMD_A7672_Ssl_Conf_Init API Successfully
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
atcmd_rsp_t ATCMD_MQTT_Publish_Callback(uint8_t* rsp)
{
	atcmd_rsp_t volatile ret = ATCMD_RSP_ERROR;
	char* volatile rspStrTable[3] = {"OK","+CMQTTPUB: ", "ERROR"};
	uint8_t volatile i  = 0;
	int32_t volatile rspType = -1;
	uint8_t* volatile p = rsp;

	while(p)
	{
		while ( ATCMD_CMD_CR == *p || ATCMD_CMD_LF == *p )
		{
			p++;
		}
		for ( i = 0; i < 3; i ++)
		{
			if ( !strncmp(rspStrTable[i], ( char * )p, strlen(rspStrTable[i])))
			{
				rspType = i;
				break;
			}

		}
		if(rspType == 1)
		{
			memset(s_tempBuff, '\0', sizeof(s_tempBuff));
			Str_Cpy_Marker(p, &s_tempBuff[0],',', ATCMD_CMD_CR);
			g_mqtt_result = Convert_String_To_Integer(s_tempBuff);
			break;
		}
		p = (uint8_t*)strchr((char*)p, 0x0a);
	}
	switch(rspType)
	{
	case 0:
		ret = ATCMD_RSP_WAIT;
		break;
	case 1:
		if(g_mqtt_result != 0)
		{
			ret = ATCMD_RSP_ERROR;
		}
		else
		{
			ret = ATCMD_RSP_FINISH;
			g_errorCount = 0;
		}

		break;

	case 2:
	default:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
		break;
	}
	return ret;
}


/******************************************************************************
* Function : ATCMD_Pdp_Context_Check_Callback()
*//**
* \b Description:
*
* This function is used as response callback to AT+CGATT?
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
* PRE-CONDITION: The AT command is being issued and response awaited
*
* POST-CONDITION: The callback will be called on AT Commands response receipt
*
* @param[in] the address to the response received from the MODEM
* @return 	enum on the the received response.
*
*@Note: This should be called only after downloading the Certificates and Keys
*@Note: and calling ATCMD_A7672_Ssl_Conf_Init API Successfully
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
atcmd_rsp_t ATCMD_Pdp_Context_Check_Callback(uint8_t* rsp)
{
	atcmd_rsp_t volatile ret = ATCMD_RSP_ERROR;
	char* volatile rspStrTable[3] = {"+CGATT:","OK", "ERROR"};
	uint8_t volatile i  = 0;
	int32_t volatile rspType = -1;
	uint8_t* volatile p = rsp;
	static uint8_t s_gprsConnectStatus;
	while(p)
	{
		while ( ATCMD_CMD_CR == *p || ATCMD_CMD_LF == *p )
		{
			p++;
		}
		for ( i = 0; i < 3; i ++)
		{
			if ( !strncmp(rspStrTable[i], ( char * )p, strlen(rspStrTable[i])))
			{
				rspType = i;
				break;
			}
		}
		if(rspType == 0)
		{
			memset(s_tempBuff, '\0', sizeof(s_tempBuff));
			Str_Cpy_Marker(p, &s_tempBuff[0],' ', ATCMD_CMD_CR);
			s_gprsConnectStatus = Convert_String_To_Integer(s_tempBuff);
			break;
		}
		p = (uint8_t*)strchr((char*)p, 0x0a);
	}
	switch(rspType)
	{
	case 0:
		if(s_gprsConnectStatus == 0)
		{
			ret = ATCMD_RSP_ERROR;
			g_errorCount++;
		}
		else
		{
			ret = ATCMD_RSP_FINISH;
			g_errorCount=0;
		}
		break;

	case 1:

		ret = ATCMD_RSP_WAIT;
		break;
	case 2:
	default:
		ret = ATCMD_RSP_ERROR;
		g_errorCount++;
	break;
	}
	return ret;
}
/******************************************************************************************************
 * URC HANDLERS
 ******************************************************************************************************/
/******************************************************************************
* Function : Urc_Sim_Check_Handler()
*//**
* \b Description:
*
* This function is used as response to URC +CPIN: READY or +CPIN: SIM REMOVED
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
*
* POST-CONDITION: The callback will be called on URC receipt
*
* @param[in] the address to the response received from the MODEM
* @param[in] len of the URC received from the Modem
* @return  0 in case of READY or -1 in case or ERROR
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 28/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
int32_t Urc_Sim_Check_Handler(uint8_t *rspStr, uint16_t len)
{
	if(!strncmp((char*)rspStr, "+CPIN: SIM REMOVED",  18))
	{
		g_simPresent = 0;
		return -1;
	}
	if(!strncmp((char*)rspStr, "+CPIN: READY", 12))
	{
		g_simPresent = 1;
		return 0;
	}
	return -1;
}

/******************************************************************************
* Function : Urc_Module_Ready_Handler
*//**
* \b Description:
*
* This function is used as response URC PB DONE received from the Modem
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
*
* POST-CONDITION: The callback will be called on URC PB DONE is received.
*
* @param[in] the address to the URC received from the MODEM
* @param[in] the len of the URC received.
*
* @return 	O if the string received is PB DONE else -1
*
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 28/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
int32_t Urc_Module_Ready_Handler(uint8_t *rspStr, uint16_t len)
{
	if(!strncmp((char*)rspStr, "PB DONE", 7))
	{
		g_pbDone = 1;
		return 0;
	}
	else
	{
		g_pbDone = 0;
	}
		return -1;
}

/******************************************************************************
* Function : Urc_Sim_Check_Handler()
*//**
* \b Description:
*
* This function is used as response to URC +CMQTTRXSTART:,  +CMQTTRXTOPIC:, +CMQTTRXPAYLOAD, +CMQTTRXEND:
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
*
* POST-CONDITION: The callback will be called on URC receipt
*
* @param[in] the address to the response received from the MODEM
* @param[in] len of the URC received from the Modem
* @return  0 in case of READY or -1 in case or ERROR
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 28/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/

int32_t Urc_Module_Data_Handler(uint8_t* rspStr, uint16_t len)
{
	char* volatile rspStrTable[4] = {"+CMQTTRXSTART:","+CMQTTRXTOPIC:","+CMQTTRXPAYLOAD:", "+CMQTTRXEND:"};
		uint8_t volatile i  = 0;
		int32_t volatile rspType = -1;
		uint8_t* volatile p = rspStr;
		while(p)
		{
			while ( ATCMD_CMD_CR == *p || ATCMD_CMD_LF == *p )
			{
				p++;
			}
			for ( i = 0; i < 4; i ++)
			{
				if ( !strncmp(rspStrTable[i], ( char * )p, strlen(rspStrTable[i])))
				{
					rspType = i;
					break;
				}
			}
			if(rspType == 2)
			{
				Str_Cpy_Marker(p, s_pubBuff, ATCMD_CMD_LF, ATCMD_CMD_CR);
				s_mqttCredentials.msg_handler(s_pubBuff);
				break;
			}
			p = (uint8_t*)strchr((char*)p, 0x0a);
		}
		return 0;
}

/******************************************************************************
* Function : Urc_Module_Mqtt_Conn_Handler()
*//**
* \b Description:
*
* This function is used as response to URC +CMQTTCONNLOST:, +CMQTTNONET
*
* PRE-CONDITION: UART should be configured and enabled
* PRE-CONDITION: The Scheduler should be enabled.
*
* POST-CONDITION: The callback will be called on URC receipt
*
* @param[in] the address to the response received from the MODEM
* @param[in] len of the URC received from the Modem
* @return  0 in case of READY or -1 in case or ERROR
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
int32_t Urc_Module_Mqtt_Conn_Handler(uint8_t* rspStr, uint16_t len)
{
char* volatile rspStrTable[2] = {"+CMQTTCONNLOST", "+CMQTTNONET"};
uint8_t volatile i = 0;
uint8_t* volatile p = rspStr;
int32_t volatile rspType = -1;
while ( ATCMD_CMD_CR == *p || ATCMD_CMD_LF == *p )
{
	p++;
}
	for ( i = 0; i < 2; i ++)
	{
		if ( !strncmp(rspStrTable[i], ( char * )p, strlen(rspStrTable[i])))
		{
			rspType = i;
			break;
		}
	}
	if(rspType == 1)
	{
		s_mqttCredentials.connectionStatus = MQTT_DISCONNECTED;
	}
	else if( rspType == 2)
	{
		s_mqttCredentials.connectionStatus = MQTT_NOT_CONNECTED;
	}
return 0;
}
