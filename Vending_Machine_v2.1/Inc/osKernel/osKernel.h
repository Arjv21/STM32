/****************************************************************************
* Title                 :  	osKernel
* Filename              :   osKernel.h
* Author                :   Sijeo Philip
* Origin Date           :   30/04/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F3
* Notes                 :   APIs for OS contructs and entities.
*
* THIS SOFTWARE IS PROVIDED BY Unisem Electronic"AS IS" AND ANY EXPRESSED
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
*  30/04/24   1.0.0   Sijeo Philip   Interface Created.
*
*****************************************************************************/
/** @file	: osKernel.h
 *  @brief	: This is Interface file for Kernel Scheduler and Related entities.
 *
 */
#ifndef __OSKERNEL_H__
#define __OSKERNEL_H__

#include "osKernel_conf.h"


typedef void(*task_t)(void);

/*Adds Threads to the Scheduler Queue */
int8_t osKernelAddThread( task_t thread, uint32_t wt);
/*By Calling this function the Scheduler starts running with the Quanta given*/
int8_t osKernelLaunch( uint32_t quanta );
/*This function needs to be called in Thread Context, this
 * Function yields the function and the Context is Switched when
 * Called from the function.
 */
void osThreadYield( void );

#endif
