/****************************************************************************
* Title                 :  	osKernel_conf
* Filename              :   osKernel_conf.h
* Author                :   Sijeo Philip
* Origin Date           :   30/04/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F3
* Notes                 :   Custom Configurations for the Kernel.
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
/** @file	: uart_conf.h
 *  @brief	: This is a kernel file which helps in implementing Multi Threaded
 *  		  Environment... This file holds configurations for the same..
 *
 */
#ifndef __OSKERNEL_CONF_H__
#define __OSKERNEL_CONF_H__

#include "common_settings.h"


/*This sets the Maximum Number of the Threads possible*/
#ifndef NUM_OF_THREADS
#define NUM_OF_THREADS		2
#endif


/* Stack size of each thread in bytes */
#ifndef THREAD_STACKSIZE
#define THREAD_STACKSIZE			600
#endif


#ifndef THREAD_PERIOD
#define THREAD_PERIOD				100
#endif


#endif
