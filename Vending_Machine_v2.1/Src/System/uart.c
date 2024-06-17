/*******************************************************************************
* Title                 :   uart
* Filename              :   uart.c
* Author                :   Sijeo Philip
* Origin Date           :   26/04/2024
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
*  26/04/24   1.0.0   Sijeo Philip   Initial Release.
*
*******************************************************************************/

/******************************************************************************
* Includes
*******************************************************************************/

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

/******************************************************************************
* Module Static Variable Definitions
*******************************************************************************/

static uint32_t s_byteCount;
static uint8_t s_bufferCount=0;
static uint8_t s_uartBuffer[A7672_UART_BUFFCOUNT_CONF][A7672_UART_BUFFSIZE_CONF];
static uint8_t s_uartBuffer_Transmit[A7672_UART_TX_BUFFSIZE_CONF];
/******************************************************************************
 * Module Global/Extern Variable Definitions
 *****************************************************************************/
uint8_t g_uartRxDataStart=0, g_uartRxDataEnd=0, g_interByteTimer=0, g_interByteDelay = 0;

/******************************************************************************
* Function Prototypes
*******************************************************************************/
static uint32_t Uart_compute_bd(uint32_t periph_clk, uint32_t baudrate);
static void Uart_set_baudrate(uint32_t periph_clk, uint32_t baudrate);
/******************************************************************************
* Function Definitions
*******************************************************************************/

/******************************************************************************
* Function : Uart_Init
*//**
* \b Description:
*
* This function is used to initialize the Uart peripheral of the system.
*
* PRE-CONDITION : The Clock and GPIOs should be initalized.
* POST-CONDITION: The Uart peripheral will be initalized and other UART
* 				  APIs can be called.
*
* @return 		None
*
* \b Example Example:
* @code
* 	 	Uart_Init();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 26/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/

void Uart_Init( void )
{
	__disable_irq();
	Uart_set_baudrate(A7672_UART_CLK, A7672_UART_BR);
	/*
	 * Most of the Setting in CR1 will not take effect is UART is not disabled.
	 * Thus the UART is enabled after setting the CR1 register.
	 */
	A7672_UART->CR1 |= A7672_UART_CR1_CONF;
	A7672_UART->CR3 |= A7672_UART_CR3_CONF;
	/*Enable the UART*/
	A7672_UART->CR1 |= (1U<<0);
	NVIC_EnableIRQ(A7672_UART_IRQ);
	NVIC_EnableIRQ(A7672_UART_TX_DMA_IRQ);
	__enable_irq();

}

/******************************************************************************
* Function : Uart_compute_bd
*//**
* \b Description:
*
* This function is used to compute the value to BRR register to set the UART baudrate
*
* PRE-CONDITION : The Clock and GPIOs should be initalized.
* POST-CONDITION: The Uart peripheral will be baudrate value is calculated.
*
* @in 			uint32_t periph_clk		The clk in Hz for the UART PERIPHERAL
* @in 			uint32_t baudrate		The desired baudrate
*
* @return 		uint32_t 				The value to be set in BRR register.
*
* \b Example Example:
* @code
* 	 	UART->BRR = Uart_compute_bd(8000000, 115200); //system clock is 8MHz and desired baudrate is 11520
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 26/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
static uint32_t Uart_compute_bd(uint32_t periph_clk, uint32_t baudrate)
{
	return ((periph_clk + (baudrate/2U))/baudrate);
}

/******************************************************************************
* Function : Uart_set_baudrate
*//**
* \b Description:
*
* This function is used to initialize the Uart peripheral of the system.
*
* PRE-CONDITION : The Clock and GPIOs should be initalized.
* POST-CONDITION: The Uart peripheral baudrate should be set.
*
* @in 			uint32_t periph_clk		The clk in Hz for the UART PERIPHERAL
* @in 			uint32_t baudrate		The desired baudrate
*
* @return 		None
*
* \b Example Example:
* @code
* 	 	Uart_set_baudrate(8000000, 115200); //system clock is 8MHz and baudrate is  115200
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 26/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
static void Uart_set_baudrate(uint32_t periph_clk, uint32_t baudrate)
{
	A7672_UART->BRR = Uart_compute_bd(periph_clk, baudrate);
}

/******************************************************************************
* Function : Uart_Transmit_Data_Dma
*//**
* \b Description:
*
* This function is used to initialize the Uart peripheral of the system.
*
* PRE-CONDITION : The Clock and GPIOs should be initalized. Uart
* 				  UART initialization should be done.
* POST-CONDITION: The the data will be transmitted and interrupt of Transfer complete is generated..
*
* @in 			uint32_t buff_addr		The address of the buffer to send
* @in 			uint32_t len			The length of the message to be sent
*
* @return 		None
*
* \b Example Example:
* @code
* 		uint8_t msg[16] = "Hello";
* 		uint32_t len = strlen(msg)
* 	 	Uart_set_baudrate((uint32_t)msg, len); //system clock is 8MHz and baudrate is  115200
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 29/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void Uart_Transmit_Data_Dma(uint32_t buff_addr, uint32_t len)
{
	/*Disable the DMA1 */
	 A7672_UART_TX_DMA_STREAM->CCR &= ~(1U<<0);

	/*Wait till the DMA is Disabled*/
	while((A7672_UART_TX_DMA_STREAM->CCR & 0x00000001)){}

	SYS_DMA->IFCR = 0x0F000000;   	/*Clearing the Channel 7 Interrupt Flags */

	/*Set Peripheral Address */
	A7672_UART_TX_DMA_STREAM->CPAR = (uint32_t)(&(A7672_UART->TDR));

	/*Set Memory Address */
	A7672_UART_TX_DMA_STREAM->CMAR = buff_addr;

	/*Set Number of Transfer*/
	A7672_UART_TX_DMA_STREAM->CNDTR = len;

	/*Enable Memory Address Increment, Direction is Set from Memory to Peripheral
	 * Transfer Complete Interrupt is Enabled.*/
	A7672_UART_TX_DMA_STREAM->CCR |= A7672_UART_DMA_TX_CCR_CONF;

	A7672_UART_TX_DMA_STREAM->CCR |= (1U<<0);

}

/******************************************************************************
* Function : Uart_Receive_Data
*//**
* \b Description:
*
* This function is used to initialize the Uart peripheral of the system.
*
* PRE-CONDITION : The Clock and GPIOs should be initalized. Uart
* 				  UART initialization should be done.
* POST-CONDITION: The the data will be received from Uart ..
*
* @in 			uint8_t** 				address of Pointer to Buffer containing the received data
*
* @return 		uint32_t 				Return lenght of data received else zero
*
* \b Example Example:
* @code
* 		uint8_t *pBuffer;
* 		uint32_t len;
* 	 	len = Uart_Receive_Data(&pBuffer);
* @endcode
* @Note: This Function is to be called inside the Thread where the A7672 response is processed
* 		 as it has to polled if the response buffer is having a valid response. The flags
* 		 g_uartRxDataStart, g_uartRxDataEnd indicates the state of UART while reception
* 		 0, 0 -> Reception is Idle
* 		 1, 0 -> Started Receiving the Data
* 		 0, 1 -> Undefined.
* 		 1, 1 -> Ended Receiving of Data. (Write to the Buffer is blocked)
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 04/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* <tr><td> 05/05/2024 </td><td> 0.1.1			 </td><td> SP	   </td><td> First Code Created </td><tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint32_t Uart_Receive_Data(uint8_t **pBuffer)
{
	static uint32_t ret = 0;
	if( (1 == g_uartRxDataStart) && (1 == g_uartRxDataEnd) )
	{
		ret = s_byteCount;
		*pBuffer = s_uartBuffer[s_bufferCount];
		s_byteCount = 0;
		s_bufferCount ^= 1;
		g_uartRxDataStart = 0;
		g_uartRxDataEnd = 0;
		return ret;
	}
	else
	return 0;
}

/******************************************************************************
* Function : Uart_Get_Current_Buffer
*//**
* \b Description:
*
* This function is used to initialize the Uart peripheral of the system.
*
* PRE-CONDITION : None
* POST-CONDITION: The Address of the Uart Buffer currently in use is returned.
*
*
* @return 		uint8_t*				Returns address of the Buffer currently
* 										in use.
*
* \b Example Example:
* @code
* 		uint8_t *pBuffer;
* 		pBuffer = Uart_Get_Current_Buffer();
* @endcode
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
uint8_t* Uart_Get_Current_Buffer(void)
{
	return &s_uartBuffer[s_bufferCount][s_byteCount];
}


/******************************************************************************
* Function : Uart_Get_Tramsit_Buffer
*//**
* \b Description:
*
* This function is used to initialize the Uart peripheral of the system.
*
* PRE-CONDITION : None
* POST-CONDITION: The Address of the Uart Buffer used to Trasnmit is returned.
*
*
* @return 		uint8_t*				Returns address of the Buffer used for
* 										Transmit is returned
*
* \b Example Example:
* @code
* 		uint8_t *pBuffer;
* 		pBuffer = Uart_Get_Transmit_Buffer();
* @endcode
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
uint8_t* Uart_Get_Transmit_Buffer(void)
{
	return &s_uartBuffer_Transmit[0];
}
/*******************************************************************************************************
 * Interrupts Service Routines
 *******************************************************************************************************/

/******************************************************************************
* Function : A7672_UART_HANDLER
*//**
* \b Description:
*
* This is the ISR Handler for the UART used for the A7672_UART.
*
* PRE-CONDITION : The Clock and GPIOs should be initalized. Uart
* 				  UART initialization should be done.
* POST-CONDITION: The the data will be transmitted or received. and interrupt of Transfer complete is generated..
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
* <tr><td> 30/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/

void A7672_UART_HANDLER(void)
{
	/*Clear TC Interrupt Flag*/
	A7672_UART->ICR |= 0x00000040;
	if(( A7672_UART->ISR ) & 0x00000020)
	{
		A7672_UART->ICR |= 0x00000020;
		if(( g_uartRxDataStart == 0 ) || ( g_uartRxDataEnd == 0 ))
			s_uartBuffer[s_bufferCount][s_byteCount] = A7672_UART->RDR;
		if(( s_byteCount == 0 ) && ( g_uartRxDataStart == 0) && ( g_uartRxDataEnd == 0 ))
		{
			/*When First Byte is Received the InterByte Delay is enabled*/
			g_interByteTimer = 1;
			/* The flag indicating the first byte of the response is received is made active*/
			g_uartRxDataStart = 1;
			/*The Flag indicating the last byte of the response is reset */
			g_uartRxDataEnd = 0;
		}
		/*The Inter byte Delay is reloaded. with each byte received */
		g_interByteDelay = A7672_UART_INTERBYTE_DELAY_CONF;
		s_byteCount++;
		if( s_byteCount > A7672_UART_BUFFSIZE_CONF )
		{
			/*If the byte count is above 256 bytes it behaves as circular buffer.
			 * and overwrites from begining of the buffer. */
			s_byteCount = 0;
		}
	}
}

/******************************************************************************
* Function : A7672_UART_TX_DMA_HANDLER
*//**
* \b Description:
*
* This is the ISR Handler for the UART DMA TX Channel used for the A7672_UART.
*
* PRE-CONDITION : The Clock and GPIOs should be initalized. Uart
* 				  UART initialization should be done.
* POST-CONDITION: The the data will be transmitted and interrupt of Transfer complete is generated..
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
* <tr><td> 30/04/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/

void A7672_UART_TX_DMA_HANDLER(void)
{
	/*Checking for Transfer Complete Interrupt
	 * Occurence in Channel 7 DMA1 and Clearing
	 * The Flag
	 */
	if(( SYS_DMA->ISR ) & 0x00002000)
	{

		SYS_DMA->IFCR |= 0x00002000;
	}
}
