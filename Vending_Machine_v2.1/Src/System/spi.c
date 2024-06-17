/*******************************************************************************
* Title                 :   spi
* Filename              :   spi.c
* Author                :   Sijeo Philip
* Origin Date           :   09/06/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F
* Notes                 :   This functions are used to initialize and control the SPI
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
*  09/06/24   1.0.0   Sijeo Philip   Initial Release.
*
*******************************************************************************/

/******************************************************************************
* Includes
*******************************************************************************/

#include "spi.h"

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

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/

/******************************************************************************
* Function : Spi_Init
*//**
* \b Description:
*
* This function is used to initialize the spi peripheral of the system.
*
* PRE-CONDITION : The Clock and GPIOs should be initalized.
* POST-CONDITION: The spi peripheral will be initalized and other spi
* 				  APIs can be called.
*
* @return 		None
*
* \b Example Example:
* @code
* 	 	Spi_Init();
* @endcode
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 09/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/

void Spi_Init(void)
{
	FLASH_SPI->CR1 |= FLASH_SPI_CR1_CONF;
	FLASH_SPI->CR2 |= FLASH_SPI_CR2_CONF;
	FLASH_SPI->CR1 |= ENABLE_FLASH_SPI;

	/*****INITIALIZE DMA2_STREAM2 FOR TX OF SPI****************/
	FLASH_SPI3_TX_DMA_STREAM->CCR = 0;		/* << Disable DMA Stream */
	while((FLASH_SPI3_TX_DMA_STREAM->CCR & 0x00000001)){}  /* << Wait till DMA is disabled */

	/**************Configure the DMA Parameters for Transmit Stream *****************/
	/*Enable Memory Addr Increment */
	FLASH_SPI3_TX_DMA_STREAM->CCR |= FLASH_SPI3_TX_DMA_CCR_CONF;

	FLASH_SPI3_RX_DMA_STREAM->CCR = 0;

	while((FLASH_SPI3_RX_DMA_STREAM->CCR & 0x00000001)){}

	FLASH_SPI3_RX_DMA_STREAM->CCR |= FLASH_SPI3_RX_DMA_CCR_CONF;
	/*Enable DMA channel Interrupt in NVIC*/
	NVIC_EnableIRQ(FLASH_SPI3_TX_DMA_IRQ);
	NVIC_EnableIRQ(FLASH_SPI3_RX_DMA_IRQ);

}

/******************************************************************************
* Function : Spi_Transmit_Data_Dma
*//**
* \b Description:
*
* This function is used to Send SPI Data using DMA transfer
*
* PRE-CONDITION : The Clock and GPIOs should be initalized.
* POST-CONDITION: The spi peripheral will be initalized and other spi
* 				  APIs can be called.
*
* @param[in]	uint32_t Address of the Buffer to be transferred
* @param[in]	uint32_t Length of Data to be transferred.
*
* @return 		None
*
* \b Example Example:
* @code
* 	 	Spi_Transmit_Data_Dma();
* @endcode
*
*@Note: the CS should be enabled before calling this API
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 09/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void Spi_Transmit_Data_Dma(uint32_t buff_addr, uint32_t len)
{
	FLASH_SPI_DMA->IFCR = 0x000000A0; /**<< Clear the Interrupts**/

	/*Set the Peripheral Address*/
	FLASH_SPI3_TX_DMA_STREAM->CPAR = (uint32_t)(&(FLASH_SPI->DR));

	/*Set Memory Address*/
	FLASH_SPI3_TX_DMA_STREAM->CMAR = buff_addr;

	/*Set Transfer Lenght*/
	FLASH_SPI3_TX_DMA_STREAM->CNDTR = len;

	/*Enable the DMA Stream*/
	FLASH_SPI3_TX_DMA_STREAM->CCR |= 0x00000001;
}

/******************************************************************************
* Function : Spi_Receive_Data_Dma
*//**
* \b Description:
*
* This function is used to Send SPI Data using DMA transfer
*
* PRE-CONDITION : The Clock and GPIOs should be initalized.
* POST-CONDITION: The spi peripheral will be initalized and other spi
* 				  APIs can be called.
*
* @param[in]	uint32_t Address of the Buffer to be received to
* @param[in]	uint32_t Length of Data to be received.
*
* @return 		None
*
* \b Example Example:
* @code
* 	 	Spi_Receive_Data_Dma();
* @endcode
*
*@Note: the CS should be enabled before calling this API
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 09/06/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void Spi_Receive_Data_Dma(uint32_t recvd_msg, uint32_t len)
{
	FLASH_SPI_DMA->IFCR = 0x0000000A; /**<< Clear the Interrupts*/

	/*Set the Peripheral Address*/
	FLASH_SPI3_RX_DMA_STREAM->CPAR = (uint32_t)(&(FLASH_SPI->DR));

	/*Set Memory Address*/
	FLASH_SPI3_RX_DMA_STREAM->CMAR = recvd_msg;

	/*Set Transfer Lenght*/
	FLASH_SPI3_RX_DMA_STREAM->CNDTR = len;

	/*Enable the DMA Stream*/
	FLASH_SPI3_RX_DMA_STREAM->CCR |= 0x00000001;
}


/****************************************************************************************************
 * INTERRUPTS
 ****************************************************************************************************/

void FLASH_SPI3_TX_DMA_HANDLER(void)
{
	if((FLASH_SPI_DMA->ISR) & 0x00000080)
	{
		//do something...
		FLASH_SPI3_TX_DMA_STREAM->CCR = 0;		/* << Disable DMA Stream */
		/*Clear the flag*/
		FLASH_SPI_DMA->IFCR |= 0x00000080;

	}
	else if((FLASH_SPI_DMA->ISR) & 0x00000020)
	{
        //do something...

		/*Clear the flag*/
		FLASH_SPI_DMA->IFCR |= 0x00000020;

	}

}

void FLASH_SPI3_RX_DMA_HANDLER(void)
{
	if((FLASH_SPI_DMA->ISR) & 0x00000008)
	{
		//do something...
		FLASH_SPI3_RX_DMA_STREAM->CCR = 0;		/* << Disable DMA Stream */
		/*Clear the flag*/
		FLASH_SPI_DMA->IFCR |= 0x00000008;

	}
	else if((FLASH_SPI_DMA->ISR) & 0x00000002)
	{
	      //do something...

		/*Clear the flag*/
		FLASH_SPI_DMA->IFCR |= 0x00000002;
	}

}

