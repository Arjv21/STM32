/****************************************************************************
* Title                 :   ATCMD_Queue_Mgmt
* Filename              :   ATCMD_Queue_Mgmt.h
* Author                :   Sijeo Philip
* Origin Date           :   07/05/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F#
* Notes                 :   These module manages the AT Command and URC queue
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
*  07/05/24   1.0.0   Sijeo Philip   Interface Created.
*
*****************************************************************************/
/** @file ATCMD_Queue_Mgmt.h
 *  @brief This module is for management of AT Command Queue i.e.
 *			Add, Delete and Initializing the Queue
 *
 *  This is the header file for the definition of function that help to
 *	ADD, DELETE, MODIFY the AT Command QUEUEs
 */

#ifndef __ATCMD_QUEUE_MGMT_H__
#define __ATCMD_QUEUE_MGMT_H__
/******************************************************************************
* Includes
*******************************************************************************/

#include "common_settings.h"
/******************************************************************************
* Preprocessor Constants
*******************************************************************************/
#define ATCMD_CMD_QUEUE		8		/* Maximum Number of AT Commands that can be queued */
#define ATCMD_URC_QUEUE		15		/* Maximum Number of URC Queued */
#define ATCMD_CMD_BUFF_SIZE	150		/* Maximum Size of the Command Queued */
/******************************************************************************
* Configuration Constants
*******************************************************************************/


/******************************************************************************
* Macros
*******************************************************************************/
#ifndef ATCMD_CMD_CR
#define ATCMD_CMD_CR					'\x0d'
#endif

#ifndef ATCMD_CMD_LF
#define ATCMD_CMD_LF					'\x0a'
#endif

#ifndef ATCMD_CMD_END
#define ATCMD_CMD_END					"\x0d\x0a"
#endif


#ifndef ATCMD_CMD_CTRL_Z
#define ATCMD_CMD_CTRL_Z				'\x1a'
#endif


/******************************************************************************
* Typedefs
*******************************************************************************/
/**
* These are the return value of response callback to be
* used by the AT command response processor
* base on the response received on each AT Command issued.
*/

typedef enum
{
	ATCMD_RSP_ERROR = -1,
	ATCMD_RSP_CONTINUE = 0,
	ATCMD_RSP_FUN_OVER = 1,
	ATCMD_RSP_FINISH = 2,
	ATCMD_RSP_WAIT,
}atcmd_rsp_t;

/**
 * This Indicates the Status of the Queue while under operation
 */
typedef enum{
	ATCMD_QUEUE_FULL,
	ATCMD_QUEUE_EMPTY,
	ATCMD_QUEUE_ERROR,
	ATCMD_QUEUE_OPER_OK
}atcmd_queue_status_t;

/**
 * Callback Function to be processed on receipt of AT Command
 * Response
 */
typedef atcmd_rsp_t (*atcmd_rsp_callback)(uint8_t* rspStr);

/**
 * Callback Function to be processed on Reciept of URC from the
 * Modem
 */
typedef int32_t(*atcmd_urc_callback)(uint8_t *rspStr, uint16_t len);

/**
 * States of the Command being issued by the Command Engine
 *
 */
typedef enum {
	ATCMD_NOK_OPER = 0,
	ATCMD_CMD1_OK,
	ATCMD_CMD2_OK,
	ATCMD_CMD3_OK,
	ATCMD_CMD4_OK,
	ATCMD_CMD5_OK,
	ATCMD_CMD6_OK,
	ATCMD_CMD7_OK,
	ATCMD_CMD8_OK
}atcmd_rsp_status_t;


typedef enum{ATCMD_MODEM_IDLE=0,
			 ATCMD_MODEM_RSP_WAIT,
			 ATCMD_MODEM_EXECUTE_AT,
			 ATCMD_MODEM_NO_RSP
}atcommand_exec_modem_status_t;

/**
 * This Structure is a place holder for AT Command,
 * Callback for the response received
 * The Number of Times it needs to be issued on failure
 * The timeout period of the response
 * The Status of the response received.
 */
typedef struct {
	uint8_t atCmd[ATCMD_CMD_BUFF_SIZE];     /**<< Command String */
	atcmd_rsp_callback rspFunc;				/**<< Callback on Response received.*/
	uint8_t repCount;						/**<< No of Times the Command to be repeated on Failure */
	uint32_t rspDelay;						/**<< Timeout of the Response */
	atcmd_rsp_status_t successStatus; 		/**<< Success/Failure of the Response */
}atcmd_at_command_t;


/**
 * This is the AT Command Queue for Processing a set
 * of Queue Commands.
 */
typedef struct {
	atcmd_at_command_t atCmdQueue[ATCMD_CMD_QUEUE]; /** <<AT Command to be Processed */
	uint8_t firstCmd; 								/** << First Command in the Queue Index */
	uint8_t lastCmd;								/** << Index of the Last Command in the Queue */
	uint8_t currentCmd;								/** << Index of the Current Command being Processed */
	uint8_t repeatCount;							/** << No of repeats left in the current Command */
	uint8_t commandCount;							/** << No of Commands to be executed */
	uint8_t cmdQueueWrite;							/** << Should be ste when the Queue is been Written */
	atcmd_rsp_t currentCmdRet;						/** << Value of the Current AT Command processing return */
	atcmd_rsp_status_t currentCmdStatus;			/** << Current Command Success/Failure */
}atcmd_cmd_queue_t;

/**
 * Place Holder for expected URCs and Corresponding Callbacks
 */
typedef struct
{
	uint8_t urcStr[30]; /** << Expected URC String */
	atcmd_urc_callback urcCallback; /**<< Corresponding Callback */
}atcmd_urc_queue_t;


/**
 * This is the Queue where URCs and the Corresponding Callbacks are
 * Stored for Processing
 */
typedef struct
{
	atcmd_urc_queue_t urcQueue[ATCMD_URC_QUEUE]; /**<<Expected URC Queue */
	uint16_t urcQueueCount;						 /**<< No. of URCs stored in the Queue */
}atcmd_urc_queue_struct_t;


/******************************************************************************
* Variables
*******************************************************************************/


/******************************************************************************
* Function Prototypes
*******************************************************************************/

atcmd_queue_status_t ATCMD_Add_AT_Cmd(atcmd_at_command_t *atCommand);

void ATCMD_AT_Command_Queue_Init( void );

atcmd_queue_status_t ATCMD_Del_AT_Cmd_Head(void);

atcmd_queue_status_t ATCMD_Add_AT_Cmd_Queue(atcmd_at_command_t* atCommand, uint16_t commandCount );

atcmd_queue_status_t ATCMD_Del_Head( void );

void ATCMD_Urc_Queue_Init(atcmd_urc_queue_struct_t* urcQueue);

atcmd_queue_status_t ATCMD_Add_Urc_Queue( atcmd_urc_queue_struct_t* urcQueueStruct);




#endif


/*******************************************************END OF FILE **************************************************/
