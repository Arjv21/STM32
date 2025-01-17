/*******************************************************************************
* Title                 :   ATCMD_Core
* Filename              :   ATCMD_Core.c
* Author                :   Sijeo Philip
* Origin Date           :   09/05/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F3x
* Notes                 :   This module is used to manage and Process the AT Command Queue
* 							Responses sent to the MODEM from the Queue
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
*  09/05/2024   1.0.0    Sijeo Philip   Initial Release.
*
*******************************************************************************/
/** @file ATCMD_Core.c
 *  @brief This is the source file for managing the AT Command Queue which is
 *			loaded for processing
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "ATCMD_Queue_Mgmt.h"
#include "osKernel.h"
#include "ATCMD_Core.h"
#include "uart.h"
#include "ATCMD_A7672_Commands.h"
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/
static atcommand_exec_modem_status_t s_modemStatus = ATCMD_MODEM_EXECUTE_AT;
/******************************************************************************
* Module Global/Extern Variable Definitions
*******************************************************************************/
extern atcmd_cmd_queue_t g_cmdQueue;
extern atcmd_urc_queue_struct_t g_urcEntityQueue;
extern modem_error_status_t g_modemErrorStatus;
extern uint8_t g_errorCount;
uint8_t g_timeOutFlag=0;
uint8_t g_delayTimerStart =0;
uint32_t g_responseTimeOut = 0;
uint32_t g_recvdBytes = 0;
uint8_t *g_rcvPtr = NULL;

/******************************************************************************
* Module Static Variable Definitions
*******************************************************************************/
static uint8_t s_atExecute=0;
static uint8_t s_modemRspWait=0;
static uint8_t s_processingResponse=0;
static uint8_t s_noResponseCount=0;
static uint8_t s_responseRecvd=0;
static uint8_t g_resetModuleFlag=0;
/******************************************************************************
* Function Prototypes
*******************************************************************************/
void ATCMD_Processor_Thread(void);
/******************************************************************************
* Function Definitions
*******************************************************************************/
/******************************************************************************
* Function : ATCMD_Core_Init
*//**
* \b Description:
*
* This function is used to Add the Thread and Initialize any Variables needed for
* running the AT Command Response Processing Thread
*
* PRE-CONDITION: None
*
* POST-CONDITION: The Thread is added to the Queue and Essential Variables are Initialized
*
* @return		-1 in case it fails else a positive integer or 0;
*
* \b Example Example:
* @code
*	ATCMD_Core_Init();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 09/12/2019 </td><td> 0.5.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/

int8_t ATCMD_Core_Init( void )
{
	if(osKernelAddThread( ATCMD_Processor_Thread, 3) < 1)
		return -1;
return 0;
}


/******************************************************************************
* Function : Rsp_Time_Out_Timer_Start
*//**
* \b Description:
*
* This Function Starts the Response Timeout Timer
*
* PRE-CONDITION: All the Peripherals should be enabled and the Scheduler should be launched
*
* POST-CONDITION: The Response Timer Starts Running (This is inside the Hardware Timer Interrupt)
*
* @return		None;
*
* \b Example Example:
* @code
*	Rsp_Time_Out_Timer_Start();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 09/12/2019 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
static void Rsp_Time_Out_Timer_Start( void )
{
	g_delayTimerStart = 0;
	g_responseTimeOut = g_cmdQueue.atCmdQueue[g_cmdQueue.currentCmd].rspDelay;
	g_timeOutFlag = 0;
	g_delayTimerStart = 1;
}


/******************************************************************************
* Function : Rsp_Time_Out_Timer_Stop
*//**
* \b Description:
*
* This Function Stops the Response Timeout Timer
*
* PRE-CONDITION: All the Peripherals should be enabled and the Scheduler should be launched
*
* POST-CONDITION: The Response Timer Stops Running (This is inside the Hardware Timer Interrupt)
*
* @return		None;
*
* \b Example Example:
* @code
*	Rsp_Time_Out_Timer_Stop();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 09/12/2019 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
static void Rsp_Time_Out_Timer_Stop( void )
{
	g_delayTimerStart = 0;
	g_responseTimeOut = 0;
	g_timeOutFlag = 0;
}


/******************************************************************************
* Function : Send_AT_Command
*//**
* \b Description:
*
* This Function is used to send the AT Command over the UART connected to the Modem
*
* PRE-CONDITION: All the Peripherals should be enabled and the Scheduler should be launched
*
* POST-CONDITION: The AT Command is send to the Modem
*
* @return		None;
*
* \b Example Example:
* @code
*	ATCMD_Core_Init();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 09/12/2019 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
static void Send_AT_Command( atcmd_at_command_t *cmd )
{
	g_cmdQueue.repeatCount = cmd->repCount;
	s_atExecute = 1;
	Uart_Transmit_Data_Dma((uint32_t)cmd->atCmd, strlen((const char*)cmd->atCmd));
	g_timeOutFlag =1;
	Rsp_Time_Out_Timer_Start();
	s_modemRspWait = 1;
}

/******************************************************************************
* Function :Global_Urc_Handler
* *//**
* \b Description:
*
* This Function is used to check the Received Message is Usolicited Response Code Received
* and Queued
*
* PRE-CONDITION: All the Peripherals should be enabled and the Scheduler should be launched
*
* POST-CONDITION: The Function is called to check if it is a Valid URC
*
* @return		None;
*
* \b Example Example:
* @code
*	ret = Global_Urc_Handler(buff, len);
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 10/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
static uint8_t Global_Urc_Handler( uint8_t *rspStr, uint16_t len)
{
	uint8_t retVal = 0;
	uint8_t i = 0;
	uint8_t *p = rspStr;
	while(p)
	{
		while((ATCMD_CMD_CR == *p) || (ATCMD_CMD_LF == *p))
		{
			p++;
		}
		for(i = 0; i < g_urcEntityQueue.urcQueueCount; i++ )
		{
			if(!strncmp((char*)g_urcEntityQueue.urcQueue[i].urcStr, (char*)p,
					strlen((char*)g_urcEntityQueue.urcQueue[i].urcStr)))
			{
				if( g_urcEntityQueue.urcQueue[i].urcCallback )
				{
					if ( 0 > (g_urcEntityQueue.urcQueue[i].urcCallback(p, len)))
						return 0;
					else
						retVal = 1;
				}
				else
					retVal = 0;
				memset(rspStr,'\0', A7672_UART_BUFFSIZE_CONF);
				break;
			}
			else
				retVal = 0;
		}
		p = (uint8_t*)memchr(p, 0x0a, 0);
	}
	return retVal;
}
/******************************************************************************
* Function : ATCMD_Processor_Thread
*//**
* \b Description:
*
* This is a thread that is queued for RoundRobin Scheduluing with the Scheduler
*
* PRE-CONDITION: All the Peripherals should be enabled and the Scheduler should be launched
*
* POST-CONDITION: The Thread will run with Weighted round robin scheduling
*
* @return		None;
*
* \b Example Example:
* @code
*	ATCMD_Core_Init();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 09/12/2019 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void ATCMD_Processor_Thread( void )
{
	static uint8_t ret = 0;
	while(1)
	{
		if(( g_recvdBytes = Uart_Receive_Data(&g_rcvPtr)) > 0)
		{
			s_processingResponse = 1;
			ret = Global_Urc_Handler(g_rcvPtr, g_recvdBytes);
			if(( 0 == ret ) && ( 1 == s_atExecute ))
			{
				Rsp_Time_Out_Timer_Stop();
				if( g_cmdQueue.atCmdQueue[g_cmdQueue.currentCmd].rspFunc )
				{
					g_cmdQueue.currentCmdRet = g_cmdQueue.atCmdQueue[g_cmdQueue.currentCmd].rspFunc(g_rcvPtr);
					s_modemStatus = ATCMD_MODEM_RSP_WAIT;
					switch(g_cmdQueue.currentCmdRet)
					{
					case ATCMD_RSP_ERROR:
						if((g_cmdQueue.atCmdQueue[g_cmdQueue.currentCmd].repCount) > 0)
						{
							g_cmdQueue.atCmdQueue[g_cmdQueue.currentCmd].repCount--;
							g_cmdQueue.currentCmdStatus = ATCMD_NOK_OPER;
						}
						else
							ATCMD_AT_Command_Queue_Init();

						break;
					case ATCMD_RSP_CONTINUE:
					case ATCMD_RSP_FINISH:
						if( g_cmdQueue.commandCount > 0)
						{
							g_cmdQueue.commandCount--;
							g_cmdQueue.currentCmdStatus = g_cmdQueue.atCmdQueue[g_cmdQueue.currentCmd].successStatus;
							g_cmdQueue.currentCmd = (g_cmdQueue.currentCmd + 1)%ATCMD_CMD_QUEUE;
						}
						break;
					case ATCMD_RSP_WAIT:
						s_modemStatus = ATCMD_MODEM_IDLE;
						Rsp_Time_Out_Timer_Start();
						break;
					case ATCMD_RSP_FUN_OVER:
						g_cmdQueue.commandCount = 0;
						g_cmdQueue.currentCmdStatus = g_cmdQueue.atCmdQueue[g_cmdQueue.currentCmd].successStatus;
						Rsp_Time_Out_Timer_Stop();
						break;

					default:
						break;
						}
					}
				if(g_cmdQueue.currentCmdRet != ATCMD_RSP_WAIT)
					s_atExecute = 0;
				s_modemRspWait = 0;
				s_responseRecvd = 1;
				}
			}
		 s_processingResponse =0;
		 switch(s_modemStatus)
		 {
		 case ATCMD_MODEM_EXECUTE_AT:
			 if((g_cmdQueue.commandCount > 0) && (g_cmdQueue.cmdQueueWrite == 0) && (s_atExecute == 0 )&& (g_cmdQueue.currentCmd <= g_cmdQueue.lastCmd))
			 {
				 g_rcvPtr = Uart_Get_Current_Buffer();
				 memset(g_rcvPtr, '\0', A7672_UART_BUFFSIZE_CONF);
				 Send_AT_Command(&g_cmdQueue.atCmdQueue[g_cmdQueue.currentCmd]);
				 s_modemStatus = ATCMD_MODEM_RSP_WAIT;
			 }
			 break;
		 case ATCMD_MODEM_RSP_WAIT:
			 if((1 == g_timeOutFlag) && (1 == s_modemRspWait))
			 {
				 if(g_cmdQueue.repeatCount > 0)
				 {
					 g_cmdQueue.atCmdQueue[g_cmdQueue.currentCmd].repCount--;
					 s_atExecute = 0;
				 }
				 if((g_cmdQueue.atCmdQueue[g_cmdQueue.currentCmd].repCount == 0) && (g_cmdQueue.commandCount > 0))
				 {
					 g_cmdQueue.currentCmd = (g_cmdQueue.currentCmd + 1) % ATCMD_CMD_QUEUE;
					 g_cmdQueue.commandCount--;
					 s_atExecute = 0;
				 }
				 s_noResponseCount++;
				 if(s_noResponseCount > 2)
				 {
					 s_modemStatus = ATCMD_MODEM_NO_RSP;
				 }
				 else
					 s_modemStatus = ATCMD_MODEM_EXECUTE_AT;
				 s_responseRecvd = 0;
				 s_modemRspWait = 0;
				 Rsp_Time_Out_Timer_Stop();
			 }
			 if( 1 == s_responseRecvd )
			 {
				 s_modemStatus = ATCMD_MODEM_EXECUTE_AT;
				 s_responseRecvd = 0;
				 s_noResponseCount = 0;
			 }
			 break;
		 case ATCMD_MODEM_NO_RSP:
			 g_resetModuleFlag = 1;
			 g_cmdQueue.currentCmdStatus = ATCMD_NOK_OPER;
			 ATCMD_AT_Command_Queue_Init();
			 s_modemStatus = ATCMD_MODEM_EXECUTE_AT;
			 break;
		 case ATCMD_MODEM_IDLE:
			 if( g_timeOutFlag == 1 )
			 {

				 Rsp_Time_Out_Timer_Stop();
				 s_modemStatus = ATCMD_MODEM_NO_RSP;
			 }
			 break;
		 default:
			 break;
		}
		 if((g_errorCount > g_modemErrorStatus.errorCountThreshold)&&(g_modemErrorStatus.errorCountThreshold != 0))
		 {
			 g_modemErrorStatus.modem_error_handler(&g_errorCount);
			 g_errorCount = 0;
		 }
	}
}
