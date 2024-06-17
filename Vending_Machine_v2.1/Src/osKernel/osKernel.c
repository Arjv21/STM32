/*******************************************************************************
* Title                 :   osKernel
* Filename              :   osKernel.c
* Author                :   Sijeo Philip
* Origin Date           :   30/04/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F
* Notes                 :   This functions are used to initialize and control the UART
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
*  30/04/24   1.0.0   Sijeo Philip   Initial Release.
*
*******************************************************************************/

/******************************************************************************
* Includes
*******************************************************************************/
#include "osKernel.h"

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/
#define SYSPRI3				(*((volatile uint32_t*)0xE000ED20))
#define INTCTRL				(*((volatile uint32_t*)0xE000ED04))
/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/
static uint32_t s_MILLIS_PRESCALER;

struct tcb{
	int32_t *stackPt;
	struct tcb *nextPt;
	uint32_t weight;
};

static int32_t s_TCB_STACK[NUM_OF_THREADS][THREAD_STACKSIZE];
static uint8_t s_threadCount = 0;
static uint32_t s_switchWeight=0;
/******************************************************************************
 * Module Global/Extern Definitions
 ******************************************************************************/
/*These Variables are exported from uart.c*/
extern uint8_t g_interByteTimer, g_interByteDelay, g_uartRxDataStart, g_uartRxDataEnd;
/*This Variable is exported and used in common.c in Delay_Quantas*/
extern uint32_t g_timeDelay;
/******************************************************************************
* Module Typedefs
*******************************************************************************/

typedef struct tcb tcbType_t;
static tcbType_t s_tcbs[NUM_OF_THREADS];
static tcbType_t *s_currentPt;
static tcbType_t *s_tailThreadPt = &s_tcbs[0];
/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/
/******************************************************************************
* Function : osSchedulerPeriodicRR
*//**
* \b Description:
*
* This Function is used to switch the thread context on Round Robin Scheduling Scheme
*
* PRE-CONDITION: None
* POST-CONDITION: None.
*
* @return 		None
*
* \b Example Example:
* @code
* 	 	Gpio_Init();
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 30/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* <tr><td> 06/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Weighted RoundRobin </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
static void osSchedulerPeriodicRR(void)
{
	s_currentPt = s_currentPt->nextPt;
	s_switchWeight = s_currentPt->weight;
}

/******************************************************************************
* Function : osSchedulerLaunch
*//**
* \b Description:
*
* The Function is called to lanch the scheduler and run the threads in round robin scheme
*
* PRE-CONDITION: Clocks  and other peripheralshould be enabled and intialized
* 				  before calling this function
*
* @return 		None
*
* \b Example Example:
* @code
* 	 	osSchedulerLaunch();
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
static void osSchedulerLaunch(void)
{
	/*Load address of currentPt into R0*/
	__asm("LDR R0, =s_currentPt");
	/*Load R2 from address equals to R0 i.e. R2=currentPt*/
	__asm("LDR R2, [R0]");
	/*Load Cortex M SP from address equals R2, i.e. SP = currentPt->stackPt*/
	__asm("LDR SP, [R2]");
	/*Restore R4, r5, r6, r7, r8, r9, r10, r11*/
	__asm("POP {R4-R11}");
	/*Restore r0, r1, r2, r3*/
	__asm("POP {R0-R3}");
	/*Restore R12*/
	__asm("POP {R12}");
	/*Skip LR by adding 4 to SP*/
	__asm("ADD SP, SP, #4");
	/*Skip LR*/
	__asm("POP {LR}");
	/*Skip PSR by adding 4 to SP*/
	__asm("ADD SP, SP, #4");
	/*Enable Global Interrupt*/
	__asm("CPSIE   I");
	/*Return from exception*/
	__asm("BX    LR");
}

/******************************************************************************
* Function : osKernelStackInit
*//**
* \b Description:
*
* The Function is called to initialize the Stack for the each thread.
*
* PRE-CONDITION: None
* POST-CONDITION:	Stack for the Thread is initialized.
*
* @in 			int 	The thread for which the stack has to be intialized.
*
* @return 		None
*
* \b Example Example:
* @code
* 	 	osKernelStackInit(ThreadCount);
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
static void osKernelStackInit(int i)
{
	s_tcbs[i].stackPt = &s_TCB_STACK[i][THREAD_STACKSIZE - 16]; /*Stack Pointer.*/
	s_TCB_STACK[i][THREAD_STACKSIZE -1] = 0x01000000; 		/*PSR*/

	s_TCB_STACK[i][THREAD_STACKSIZE - 3] = 0x14141414; /*R14*/
	s_TCB_STACK[i][THREAD_STACKSIZE - 4] = 0x12121212; /*R12*/
	s_TCB_STACK[i][THREAD_STACKSIZE - 5] = 0x03030303; /*R3*/
	s_TCB_STACK[i][THREAD_STACKSIZE - 6] = 0x02020202; /*R2*/
	s_TCB_STACK[i][THREAD_STACKSIZE - 7] = 0x01010101; /*R1*/
	s_TCB_STACK[i][THREAD_STACKSIZE - 8] = 0x00000000; /*R0*/

	/*Have to Manually Saved*/
	s_TCB_STACK[i][THREAD_STACKSIZE - 9] = 0x11111111; /*R11*/
	s_TCB_STACK[i][THREAD_STACKSIZE - 10] = 0x10101010; /*R10*/
	s_TCB_STACK[i][THREAD_STACKSIZE - 11] = 0x09090909; /*R9*/
	s_TCB_STACK[i][THREAD_STACKSIZE - 12] = 0x08080808; /*R8*/
	s_TCB_STACK[i][THREAD_STACKSIZE - 13] = 0x07070707; /*R7*/
	s_TCB_STACK[i][THREAD_STACKSIZE - 14] = 0x06060606; /*R6*/
	s_TCB_STACK[i][THREAD_STACKSIZE - 15] = 0x05050505; /*R5*/
	s_TCB_STACK[i][THREAD_STACKSIZE - 16] = 0x04040404; /*R4*/
}

/******************************************************************************
* Function : osKernelAddThread
*//**
* \b Description:
*
* The Function is called to add a new thread to the Queue.
*
* PRE-CONDITION: None
* POST-CONDITION:	New Thread is added to the Scheduling Queue.
*
* @in 			task_t		Pointer to void func(void) type fucntion
* @in 			uint32_t 	Weight to the Thread, the thread is scheduled for wt*quanta before
* 							next switch, if Zero it will run for one quanta
*
* @return 		 int8_t 	returns the Thread Id if successful else -1
*
* \b Example Example:
* @code
* void task1(void);
* 	 	uint8_t thread_id = osKernelAddThread(task1);
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 30/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* <tr><td> 06/05/2024 </td><td 0.1.0			 </td><td> SP	   </td><td> Added Variable wt </td><tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
int8_t osKernelAddThread( task_t thread, uint32_t wt)
{
	__disable_irq();
	if(( thread == NULL ) || ( s_threadCount >= NUM_OF_THREADS) || (wt <= 0))
		return -1;

	s_threadCount++;
	if(s_threadCount == 1)
	{
		s_tcbs[0].nextPt = NULL;
		s_tcbs[0].weight = wt;
		osKernelStackInit(0);
		s_TCB_STACK[0][THREAD_STACKSIZE - 2] = (int32_t)thread;
	}
	else
	{
		s_tailThreadPt->nextPt = &s_tcbs[s_threadCount-1];
		s_tailThreadPt = &s_tcbs[s_threadCount-1];
		s_tcbs[s_threadCount-1].nextPt = &s_tcbs[0];
		s_tcbs[s_threadCount-1].weight = wt;
		osKernelStackInit(s_threadCount-1);
		s_TCB_STACK[s_threadCount-1][THREAD_STACKSIZE - 2] = (int32_t)thread;
	}

	__enable_irq();
	return s_threadCount;
}

/******************************************************************************
* Function : osKernelLaunch
*//**
* \b Description:
*
* The Function is called to start the Round Robin Scheduler and The registered task..
*
* PRE-CONDITION: The Clocka and System Peripherals should be Initialized and Enabled.
* POST-CONDITION:	The Scheduler is Launched and the Threads Starts running.
*
* @in 			uint32_t		quanta for each context switching is scheduled.
*
* @return 		None
*
* \b Example Example:
* @code
*
*
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 02/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* <tr><td> 06/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Made Changes for Weight RR </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
int8_t osKernelLaunch( uint32_t quanta )
{
	if(( quanta <= 0) || ( quanta > 1000 ))
		return -1;
	__disable_irq();
	s_currentPt = &s_tcbs[0];
	s_switchWeight = s_currentPt->weight;

	s_MILLIS_PRESCALER = (SYS_FREQ/1000); /*This Prescaler Value gives 1 ms Delay*/
	SysTick->CTRL = 0;
	SysTick->VAL = 0;
	SysTick->LOAD = (quanta * s_MILLIS_PRESCALER) - 1;

	SYSPRI3 = (SYSPRI3 & 0x00FFFFFF) | 0xE0000000; /* Priority Level at 7*/
	SysTick->CTRL = 0x00000007; 	/*Enable the SysTick Timer*/
	NVIC_SetPriority(PendSV_IRQn, 0xFF);

	osSchedulerLaunch();
	/*The Control does not go beyond this if the scheduler is successfully Launched*/
	return 0;

}

/******************************************************************************
* Function : osThreadYield
*//**
* \b Description:
*
* The Function is called to switched context of the thread before the Scheduled Quanta
*
* PRE-CONDITION: The Scheduler should be running and threads should be running
* POST-CONDITION:	This function to be called inside the Thread context which will Yeild the
* 					Thread and switch to the next thread in the queue before the Quanta
*
* @return 		None
*
* \b Example Example:
* @code
* void task1(void);
*
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 02/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/

void osThreadYield( void )
{
	SysTick->VAL = 0;
	INTCTRL = 0x04000000; /*Trigger SysTick*/
}

/**********************************************************************************
 * Interrupt Handlers
 *********************************************************************************/
/******************************************************************************
* Function : SysTick_Handler
*//**
* \b Description:
*
* This function ISR Handler for SysTick Timer
*
* PRE-CONDITION: Clocks should be enabled and Initialed and SysTick Time Intterupt
* 				 Should be enabled
*
* @return 		None
*
* \b Example Example:
* @code
*
* @endcode
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 30/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* <tr><td> 06/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Made changes for weight RR </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void SysTick_Handler(void)
{
	if( s_threadCount > 1)
	{
		if( s_switchWeight == 0 ){
			/*Trigger PENDSV */
			INTCTRL = 0x10000000;
		}
		else
		{
			s_switchWeight = s_switchWeight - 1;
		}

	}
	/*These Variables are Used in uart.c*/
	if( 1 ==  g_interByteTimer )
	{
		g_interByteDelay--;
	}
	if(( 0 == g_interByteDelay ) && ( 1 == g_uartRxDataStart ))
	{
		g_uartRxDataEnd = 1;
		g_interByteTimer = 0;
	}
	/*These Variables are exported from common.c*/
	if(g_timeDelay > 0){
		g_timeDelay--;
	}
}

/******************************************************************************
* Function : PendSV_Handler
*//**
* \b Description:
*
* This Low prioriity software exception created for thread context switching
*
* PRE-CONDITION: Clocks should be enabled and intialized PendSV exception should
* 				 be triggered.
*
* @return 		None
*
* \b Example Example:
* @code
*
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
__attribute__((naked))void PendSV_Handler(void)
{
	/*Suspend Current Thread*/
	/*Disable Global Interrupt*/
	__asm("CPSID	I");

	/*Save r4, r5, r6, r7, r8, r9, r10, r11*/
	__asm("PUSH		{R4-R11}");

	/*Load Address of the currentPt into r0*/
	__asm("LDR R0, =s_currentPt");

	/*Load R1 from address equal to R0, i.e. r1 = currentPt*/
	__asm("LDR R1, [R0]");

	/*Store CortexM SP at address equal to R1, i.e. save SP into TCB*/
	__asm("STR SP, [R1]");

	/*Push R0 and LR to the Stack*/
	__asm("PUSH   {R0, LR}");

	/*Switch the Task*/
	osSchedulerPeriodicRR();

	__asm("POP  {R0, LR}");

	/*R1 = currentPt*/
	__asm("LDR R1, [R0]");

	/*Load Cortex-M SP from address equals to R1, i.e. SP = currentPt->stackPt*/
	__asm("LDR SP, [R1]");

	/*Restore r4, r5, r6, r7, r8, r9, r10, r11 */
	__asm("POP {R4-R11}");

	/*Enable Global Interrupt*/
	__asm("CPSIE     I");

	/*Return from Exception and restore R0, r1, r2, r3, , r12, pc, lr, psr*/
	__asm("BX  LR");
}
