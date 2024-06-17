/****************************************************************************
* Title                 :   TIMER_CONF
* Filename              :   timer_conf.h
* Author                :   Sijeo Philip
* Origin Date           :   09/05/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F3
* Notes                 :   TIMER Hardware Configuration Interface file
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
*  09/05/24   1.0.0   Sijeo Philip   Interface Created.
*
*****************************************************************************/
/** @file	: timer_conf.h
 *  @brief	: This is Interface to define the TIMER of the
 *  System
 *
 */
#ifndef __TIMER_CONF_H__
#define __TIMER_CONF_H__

#include "common_settings.h"

#ifndef ATCMD_RSP_TIMER
#define ATCMD_RSP_TIMER		TIM2
#endif

/**
 * This is selected ss the SYS_FREQ is 8MHz the Pre-scaler is Divided by 8
 * Please refer pg 542 of Refernce Manual for this Settintg i.e. the Value - 1
 */
#ifndef ATCMD_RSP_TIMER_PSC_CONF
#define ATCMD_RSP_TIMER_PSC_CONF	7
#endif

/*
 * This Value is set as 1000000 for 1sec Timer, This can be reduced for
 * millisecond timer This can reduced to 1 microsecond timer, Please
 * refer pg 542 of Reference Manual for this Settintg
 */
#ifndef ATCMD_RSP_TIMER_ARR_CONF
#define ATCMD_RSP_TIMER_ARR_CONF(FREQ)	(1000000/FREQ -1)
#endif

/**
 * This is Enabling the Timer In Control Register 1
 */
#ifndef ATCMD_RSP_TIMER_CR1_CONF
#define ATCMD_RSP_TIMER_CR1_CONF	0x0001
#endif


/**
 * This is Enabling the Timer Update Interrupt
 */
#ifndef ATCMD_RSP_TIMER_DIER_CONF
#define ATCMD_RSP_TIMER_DIER_CONF	0x00000001
#endif



#ifndef ATCMD_RSP_TIMER_IRQ
#define ATCMD_RSP_TIMER_IRQ		TIM2_IRQn
#endif



#ifndef ATCMD_RSP_TIMER_HANDLER
#define ATCMD_RSP_TIMER_HANDLER	TIM2_IRQHandler
#endif




#endif
