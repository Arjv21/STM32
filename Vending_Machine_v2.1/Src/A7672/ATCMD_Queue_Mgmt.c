/*******************************************************************************
* Title                 :   ATCMD_Queue_Mgmt
* Filename              :   ATCMD_Queue_Mgmt.c
* Author                :   Sijeo Philip
* Origin Date           :   07/05/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F3x
* Notes                 :   This module is used to manage the AT Command Queue
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
*  07/05/2024   1.0.0    Sijeo Philip   Initial Release.
*
*******************************************************************************/
/** @file ATCMD_Queue_Mgmt.c
 *  @brief This is the source file for managing the AT Command Queue which is
 *			loaded for processing
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "ATCMD_Queue_Mgmt.h"

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/
atcmd_cmd_queue_t g_cmdQueue;
atcmd_urc_queue_struct_t g_urcEntityQueue;

/******************************************************************************
* Function Prototypes
*******************************************************************************/
extern atcmd_rsp_t ATCMD_Default_Callback(uint8_t * rsp);
/******************************************************************************
* Function Definitions
*******************************************************************************/
/******************************************************************************
* Function : ATCMD_Queue_Full_Check()
*//**
* \b Description:
*
* This function is used to check if the Queue is loaded to its capacity
*
* PRE-CONDITION: At Command Queue should be declared global
*
* POST-CONDITION: It checks if the Queue is full
* @return 		  true if full or false if otherwise
*
*
* \b Example Example:
* @code
* 	atcmd_cmd_queue_t cmdQueue;
*   uint8_t queueStatus;
*	 	queueStatus = ATCMD_Queue_Full_Check();
* @endcode
*
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 07/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
static uint8_t ATCMD_Queue_Full_Check( void )
{
	if ( ATCMD_CMD_QUEUE <= g_cmdQueue.commandCount )
	{
		return 1;
	}
	else {
		return 0;
	}
}

/******************************************************************************
* Function : ATCMD_Command_Remove()
*//**
* \b Description:
*
* This function is used to  clear the AT Command at the location passed to it
*
* PRE-CONDITION: At Command Queue should be declared global
*
* POST-CONDITION: It clears the location of the Queue passed to it
* @return 		  None
*
*
* \b Example Example:
* @code
* 	atcmd_cmd_queue_t cmdQueue;
*
*  atcmd_command_remove(&cmdQueue.atCmdQueue[0]);
* @endcode
*
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 07/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
static void ATCMD_Command_Remove(atcmd_at_command_t *atCommand)
{
	memset(atCommand->atCmd, '\0', sizeof(atCommand->atCmd));
	atCommand->repCount = 0;
	atCommand->rspDelay = 0;
	atCommand->rspFunc = NULL;
	atCommand->successStatus = ATCMD_NOK_OPER;

}

/******************************************************************************
* Function : ATCMD_Add_AT_Cmd()
*//**
* \b Description:
*
* This function is add a single AT Command to the Queue
*
* PRE-CONDITION: Command Structure should be loaded
*
* POST-CONDITION: The loaded structure is passed on to the function to be written
*				  in AT Command Queue
* @return 		  enum Status of the queue operation done
*
* @param[in]	 address of the Structure loaded with AT Command value
*
* \b Example Example:
* @code
* 	atcmd_at_command_t atCommand = {"AT/n/r", 2, 3000, CMD1_OK };
*	atcmd_queue_status_t queueStatus;
* 	queueStatus = ATCMD_Add_AT_Cmd( &atCommand );
* @endcode
*
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 07/05/2024 </td><td> 0.5.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
atcmd_queue_status_t ATCMD_Add_AT_Cmd( atcmd_at_command_t *atCommand)
{
	if(ATCMD_Queue_Full_Check())
		return ATCMD_QUEUE_FULL;

	if( 0 < g_cmdQueue.commandCount )
	{
		g_cmdQueue.lastCmd = (g_cmdQueue.lastCmd + 1) % ATCMD_CMD_QUEUE;
	}

	memset(g_cmdQueue.atCmdQueue[g_cmdQueue.lastCmd].atCmd, '\0', sizeof(g_cmdQueue.atCmdQueue[g_cmdQueue.lastCmd].atCmd));
	strcpy((char*)g_cmdQueue.atCmdQueue[g_cmdQueue.lastCmd].atCmd, (char*)atCommand->atCmd);
	g_cmdQueue.atCmdQueue[g_cmdQueue.lastCmd].repCount = atCommand->repCount;
	if( NULL != atCommand->rspFunc )
	{
		g_cmdQueue.atCmdQueue[g_cmdQueue.lastCmd].rspFunc = atCommand->rspFunc;
	}
	else
	{
		g_cmdQueue.atCmdQueue[g_cmdQueue.lastCmd].rspFunc = ATCMD_Default_Callback;
	}
	g_cmdQueue.atCmdQueue[g_cmdQueue.lastCmd].rspDelay = atCommand->rspDelay;
	g_cmdQueue.atCmdQueue[g_cmdQueue.lastCmd].successStatus = atCommand->successStatus;
	g_cmdQueue.commandCount++;
	return ATCMD_QUEUE_OPER_OK;
}

/******************************************************************************
* Function : ATCMD_AT_Command_Queue_Init()
*//**
* \b Description:
*
* This function is used to initialize the AT Command Queue
*
* PRE-CONDITION: AT Command Queue is Declared
*
* POST-CONDITION: AT Command Queue is cleared and initialized.
* @return 		  None
*
*
* \b Example Example:
* @code
* 	atcmd_cmd_queue_t cmdQueue;
* 	ATCMD_AT_Command_Queue_Init();
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 07/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void ATCMD_AT_Command_Queue_Init( void )
{
	static uint8_t i =0;
	for(i = 0; i < ATCMD_CMD_QUEUE; i++)
	{
		memset(g_cmdQueue.atCmdQueue[i].atCmd, '\0', sizeof(g_cmdQueue.atCmdQueue[i].atCmd));
		g_cmdQueue.atCmdQueue[i].repCount = 0;
		g_cmdQueue.atCmdQueue[i].rspDelay = 0;
		g_cmdQueue.atCmdQueue[i].rspFunc = NULL;
		g_cmdQueue.atCmdQueue[i].successStatus = 0;
	}

	g_cmdQueue.firstCmd = 0;
	g_cmdQueue.lastCmd = 0;
	g_cmdQueue.currentCmd = 0;
	g_cmdQueue.repeatCount = 0;
	g_cmdQueue.commandCount = 0;
	g_cmdQueue.cmdQueueWrite = 0;
	g_cmdQueue.currentCmdRet = ATCMD_RSP_ERROR;
	g_cmdQueue.currentCmdStatus = ATCMD_NOK_OPER;
}

/******************************************************************************
* Function : ATCMD_Del_AT_Cmd_Head()
*//**
* \b Description:
*
* This function is used to delete the command from head of the Command Queue
* to the current command.
*
* PRE-CONDITION: AT command Queue should be declared
*
* POST-CONDITION: The Command at the head of the Queue to the current command will be deleted
* @return 		  enum Status of the queue operation done
*
*
* \b Example Example:
* @code
* 	atcmd_queue_status_t queueStatus;
* 	queueStatus = ATCMD_Del_AT_Cmd_Head();
* @endcode
*
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 08/05/2024 </td><td> 0.5.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
atcmd_queue_status_t ATCMD_Del_AT_Cmd_Head( void )
{
	static uint8_t i = 0;
	if( g_cmdQueue.firstCmd == g_cmdQueue.currentCmd )
	{
		return ATCMD_QUEUE_ERROR;
	}

	if( 0 == g_cmdQueue.commandCount )
	{
		return ATCMD_QUEUE_EMPTY;
	}

	if( g_cmdQueue.currentCmd < g_cmdQueue.firstCmd )
	{
		for( i = g_cmdQueue.firstCmd; i < ATCMD_CMD_QUEUE; i++ )
		{
			ATCMD_Command_Remove(&g_cmdQueue.atCmdQueue[i]);
			g_cmdQueue.commandCount--;
		}
		for( i = 0; i < g_cmdQueue.currentCmd; i++ )
		{
			ATCMD_Command_Remove(&g_cmdQueue.atCmdQueue[i]);
			g_cmdQueue.commandCount--;
		}
	}
	else
	{
		for(i = g_cmdQueue.firstCmd; i < g_cmdQueue.currentCmd; i++)
		{
			ATCMD_Command_Remove( &g_cmdQueue.atCmdQueue[i]);
			g_cmdQueue.commandCount--;
		}
	}
	g_cmdQueue.firstCmd = g_cmdQueue.currentCmd;
	return ATCMD_QUEUE_OPER_OK;
}

/******************************************************************************
* Function : ATCMD_Add_AT_Cmd_Queue()
*//**
* \b Description:
*
* This function is used to add multiple commands to the Queue
*
* PRE-CONDITION: AT command Queue should be declared and initialized
*
* POST-CONDITION: The set of command will be loaded to the Command Queue
*				  for processing
* @in 			atcmd_at_commant_t 		Address Command to be inserted in the Queue
* @in			uint16_t 				No of Commands to be inserted in the Queue
* @return 		  enum Status of the queue operation done
*
*
* \b Example Example:
* @code
*	atcmd_at_command_t commandQueue[10];
* 	atcmd_queue_status_t queueStatus;
* 	queueStatus = ATCMD_Add_AT_Cmd_Queue(commandQueue, 5);
* @endcode
*
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 08/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
atcmd_queue_status_t ATCMD_Add_AT_Cmd_Queue( atcmd_at_command_t *atCommand, uint16_t commandCount)
{
	static uint16_t i;
	i = commandCount;
	if( ATCMD_CMD_QUEUE < (g_cmdQueue.commandCount + commandCount))
	{
		return ATCMD_QUEUE_ERROR;
	}
	ATCMD_Del_AT_Cmd_Head();
	while(i)
	{
		ATCMD_Add_AT_Cmd(&atCommand[commandCount - i]);
		i--;
	}
	return ATCMD_QUEUE_OPER_OK;
}

/******************************************************************************
* Function : ATCMD_Del_Head()
*//**
* \b Description:
*
* This function is used to delete the Command from the head of the queue
*
* PRE-CONDITION: AT command Queue should be declared and initialized
*
* POST-CONDITION: The Command at the Head of the Queue will be deleted
* @return 		  enum Status of the queue operation done
*
*
* \b Example Example:
* @code
*	atcmd_at_command_t commandQueue[10];
* 	atcmd_queue_status_t queueStatus;
* 	queueStatus = ATCMD_Del_Head();
* @endcode
*
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 08/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
atcmd_queue_status_t ATCMD_Del_Head( void )
{
	if( 0 == g_cmdQueue.commandCount )
	{
		return ATCMD_QUEUE_EMPTY;
	}
	ATCMD_Command_Remove( &g_cmdQueue.atCmdQueue[g_cmdQueue.firstCmd]);
	if( g_cmdQueue.firstCmd == g_cmdQueue.currentCmd )
	{
		g_cmdQueue.currentCmd = (g_cmdQueue.currentCmd + 1) % ATCMD_CMD_QUEUE;
	}
	g_cmdQueue.firstCmd = ( g_cmdQueue.currentCmd + 1 ) % ATCMD_CMD_QUEUE;
	g_cmdQueue.commandCount--;
	return ATCMD_QUEUE_OPER_OK;
}

/******************************************************************************
* Function : ATCMD_Urc_Queue_Init()
*//**
* \b Description:
*
* This function is used to initialize the URC queue
*
* PRE-CONDITION: URC command queue must be declared and initialized
*
* POST-CONDITION: The URCs will be queued for further processing
*
* @in 			atcmd_urc_queue_struct_t 		pointer to the structure storing URCs
* @return 		  None
*
*
* \b Example Example:
* @code
*	atcmd_at_command_t urcQueue[10];
* 	atcmd_queue_status_t queueStatus;
* 	queueStatus = ATCMD_Urc_Queue_Init();
* @endcode
*
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 08/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void ATCMD_Urc_Queue_Init( atcmd_urc_queue_struct_t *urcQueue)
{
	uint8_t i = 0;
	urcQueue->urcQueueCount = 0;

	for(i = 0; i < ATCMD_URC_QUEUE; i++)
	{
		memset(urcQueue->urcQueue[i].urcStr, '\0', sizeof(uint8_t)*30);
		urcQueue->urcQueue[i].urcCallback = NULL;
	}
}

/******************************************************************************
* Function : ATCMD_Add_Urc()
*//**
* \b Description:
*
* This function is used to add the URCs to the URC Queue
*
* PRE-CONDITION: URC command queue must be declared and initialized
*
* POST-CONDITION: The URCs will be queued for further processing

* @param[in]	  Address to the URC Struct to be added to the URC Queue
* @param[in]	  position at which it should be added
* @return 		  enum Status of the queue operation done
*
*
* \b Example Example:
* @code
*	atcmd_urc_queue_struct_t urcQueue[10];
* 	atcmd_queue_status_t queueStatus;
* 	queueStatus = ATCMD_Add_Urc(&urcQueue, ATCMD_URC_QUEUE);
* @endcode
*
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 08/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
static atcmd_queue_status_t ATCMD_Add_Urc( atcmd_urc_queue_t *UrcQueue, uint8_t position )
{
	if( position > ATCMD_URC_QUEUE )
	{
		return ATCMD_QUEUE_ERROR;
	}
	strcpy((char*)g_urcEntityQueue.urcQueue[position].urcStr, (char*)UrcQueue->urcStr);
	g_urcEntityQueue.urcQueue[position].urcCallback = UrcQueue->urcCallback;
	return ATCMD_QUEUE_OPER_OK;
}

/******************************************************************************
* Function : ATCMD_Add_Urc_Queue()
*//**
* \b Description:
*
* This function is used to add the URCs to the URC Queue
*
* PRE-CONDITION: URC command queue must be declared and initialized
*
* POST-CONDITION: The URCs will be queued for further processing

* @param[in]	  Address to the URC Struct to be added to the URC Queue
* @param[in]	  No of URCs to be added
* @return 		  enum Status of the queue operation done
*
*
* \b Example Example:
* @code
*	atcmd_urc_queue_struct_t urcQueue[10];
* 	atcmd_queue_status_t queueStatus;
* 	queueStatus = ATCMD_Add_Urc_Queue(&urcQueue, ATCMD_URC_QUEUE);
* @endcode
*
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 08/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
atcmd_queue_status_t ATCMD_Add_Urc_Queue( atcmd_urc_queue_struct_t *urcQueueStruct)
{
	uint8_t i = 0;
	ATCMD_Urc_Queue_Init(&g_urcEntityQueue);
	if( g_urcEntityQueue.urcQueueCount + urcQueueStruct->urcQueueCount > ATCMD_URC_QUEUE)
	{
		return ATCMD_QUEUE_ERROR;
	}
	for( i = 0; i < urcQueueStruct->urcQueueCount; i++ )
	{
		ATCMD_Add_Urc(&urcQueueStruct->urcQueue[g_urcEntityQueue.urcQueueCount + i], g_urcEntityQueue.urcQueueCount + i);
	}
	g_urcEntityQueue.urcQueueCount = g_urcEntityQueue.urcQueueCount + urcQueueStruct->urcQueueCount;
	return ATCMD_QUEUE_OPER_OK;
}
