/****************************************************************************
* Title                 :   ATCMD_Core
* Filename              :   ATCMD_Core.h
* Author                :   Sijeo Philip
* Origin Date           :   09/05/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F#
* Notes                 :   These module processes the AT Command and URC queue
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
*  09/05/24   1.0.0   Sijeo Philip   Interface Created.
*
*****************************************************************************/
/** @file ATCMD_Queue_Mgmt.h
 *  @brief This module is for management and processing of AT Command Queue
 *
 *  This is the header file for the definition of function that help to
 *	ADD, DELETE, MODIFY the AT Command QUEUE and process the responses.
 */

#ifndef __ATCMD_CORE_H__
#define __ATCMD_CORE_H__


/******************************************************************************
* Includes
*******************************************************************************/
int8_t ATCMD_Core_Init( void );

#endif
