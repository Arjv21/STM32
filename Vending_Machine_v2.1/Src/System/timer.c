/*******************************************************************************
* Title                 :   timer
* Filename              :   timer.c
* Author                :   Sijeo Philip
* Origin Date           :   09/05/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F
* Notes                 :   This functions are used to initialize and control the TIMER
* 							peripheral of the system
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
*  09/05/24   1.0.0   Sijeo Philip   Initial Release.
*
*******************************************************************************/

/******************************************************************************
* Includes
*******************************************************************************/

#include "timer.h"
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

/******************************************************************************
* Module Static Variable Definitions
*******************************************************************************/

/******************************************************************************
 * Module Global/Extern Variable Definitions
 *****************************************************************************/
/*These Variables are First Declared in ATCMD_Core.c*/
extern uint8_t g_delayTimerStart, g_timeOutFlag;
extern uint32_t g_responseTimeOut;
extern mqtt_creds_t s_mqttCredentials;
extern uint32_t g_pingTimeout;
/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/

/******************************************************************************
* Function : Timer_Init
*//**
* \b Description:
*
* This function is used to initialize the Timer peripheral of the system.
*
* PRE-CONDITION : The Clock should be initalized.
* POST-CONDITION: The Timer peripheral will be initialized and other TIMER
* 				  APIs can be called.
*
* @return 		None
*
* \b Example Example:
* @code
* 	 	Timer_Init();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 09/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void Timer_Init( void )
{
	__disable_irq();
	ATCMD_RSP_TIMER->PSC = ATCMD_RSP_TIMER_PSC_CONF;
	ATCMD_RSP_TIMER->ARR = ATCMD_RSP_TIMER_ARR_CONF(1);  	/* This is set at 1Hz */
	ATCMD_RSP_TIMER->CR1 |= ATCMD_RSP_TIMER_CR1_CONF;		/* Timer is Enabled */
	ATCMD_RSP_TIMER->DIER |= ATCMD_RSP_TIMER_DIER_CONF;		/* Update Interrupt of Timer is Enabled*/
	NVIC_SetPriority(ATCMD_RSP_TIMER_IRQ, 6);
	NVIC_EnableIRQ(ATCMD_RSP_TIMER_IRQ);					/* Enable the Timer Interrupt */
	__enable_irq();
}


/********************************************************************************
 * Interrupts
 ********************************************************************************/

/******************************************************************************
* Function : ATCMD_RSP_TIMER_HANDLER
*//**
* \b Description:
*
* This is the ISR Handler for the TIMER used for the System.
*
* PRE-CONDITION : The Clock should be initialized. Timer initialization should be done.
* POST-CONDITION: The Timer will be enable and Interrupt will be generated.
*
* @in 			None
*
* @return 		None
*
* \b Example Example:
* @code
*
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 09/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void ATCMD_RSP_TIMER_HANDLER(void)
{
	ATCMD_RSP_TIMER->SR = 0;

	if((g_delayTimerStart == 1)&&(g_timeOutFlag == 0))
	{
		g_responseTimeOut--;
		if(g_responseTimeOut <= 0) {
			g_timeOutFlag = 1;
			g_delayTimerStart = 0;
		}
	}
	if(s_mqttCredentials.status_timeout != 0)
	{
		if(g_pingTimeout == 0)
		{
			s_mqttCredentials.timeout_handler();
			g_pingTimeout = s_mqttCredentials.status_timeout;
		}
		else
		{
			g_pingTimeout = g_pingTimeout - 1;
		}
	}

}
