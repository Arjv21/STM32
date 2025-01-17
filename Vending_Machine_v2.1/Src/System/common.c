/*******************************************************************************
* Title                 :   common
* Filename              :   common.c
* Author                :   Sijeo Philip
* Origin Date           :   10/05/2024
* Version               :   1.0.0
* Compiler              :   gcc
* Target                :   STM32F3x
* Notes                 :   This module contains all common string processing
* 							and Delay Function for use in application.
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
*  09/05/2024   1.0.0    Sijeo Philip   Initial Release.
*
*******************************************************************************/
/** @file common.c
 *  @brief This is Source file for all common string processing and Delay
 *  Functions
 */
/******************************************************************************
* Includes
*******************************************************************************/
#include "common_settings.h"
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/
#define EPOCH_START_YEAR	1970
/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/

/******************************************************************************
* Module Global/Extern Variable Definitions
*******************************************************************************/
uint32_t g_timeDelay = 0;
/******************************************************************************
* Module Static Variable Definitions
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/

/******************************************************************************
* Function : Delay_Ms
*//**
* \b Description:
*
* This is blocking code for Delay in Milliseconds
*
* PRE-CONDITION : None.
* POST-CONDITION: The Code blocks for the milliseconds passed to the Function.
*
* @in 			uint32_t  		Milliseconds of Delay to be generated.
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
* <tr><td> 10/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void Delay_Ms( uint32_t ms)
{
	/*This Loop is Based on the assumption that each loop iterations takes about
	 * 12 cycles, The actual number of cycles will depend on the MCU, need to
	 * Fine tune
	 */
	uint32_t iterations = (SYS_FREQ_MHZ*1000/12) * ms;
	while(iterations > 0){
		iterations--;
		__asm__("NOP");
	}
}


/******************************************************************************
* Function : Delay_Quantas
*//**
* \b Description:
*
* This is blocking code for Delay in Milliseconds
*
* PRE-CONDITION : None.
* POST-CONDITION: The Code blocks for the quantas passed to the Function.
*
* @in 			uint32_t  		Quantas of Delay to be generated.
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
* <tr><td> 10/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void Delay_Quantas( uint32_t qt)
{
	g_timeDelay = qt;
	while(g_timeDelay != 0){}
}

/******************************************************************************
* Function : Convert_String_To_Integer()
*//**
* \b Description:
*
* This function is used to convert string to integer value
*
* PRE-CONDITION: None
*
* POST-CONDITION: The string been passed to function is returned as unsigned integer
*
* @return 		Unsigned Integer

* @param[in]	address of the String to be converted to number
*
* \b Example Example:
* @code
*  uint8_t strNumber[2] = "22"
*  uint8_t number = 0;
* 	number = Convert_String_To_Integer(strNumber);
* @endcode
*
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 15/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint16_t Convert_String_To_Integer(uint8_t *const number)
{
	static uint32_t dec = 0, i;
	static uint8_t len = 0;
	dec = 0;
	len = 0;
	len = strlen((char*)number);
	for( i = 0; i < len; i++ )
	dec = dec * 10 + (number[i] - '0');

	return dec;
}
/******************************************************************************
* Function : Convert_String_To_Integer32()
*//**
* \b Description: This function is used to convert string to integer value
* PRE-CONDITION: None
* POST-CONDITION: The string been passed to function is returned as unsigned integer
* @return 		:32 bit unsigned integer
* @param[in]	address of the String to be converted to number
*
* \b Example Example:
* @code
*  uint8_t strNumber[2] = "22"
*  uint32_t number = 0;
* 	number = Convert_String_To_Integer32(strNumber);
* @endcode
*
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 15/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint32_t Convert_String_To_Integer32(uint8_t *const number)
{
	static uint32_t dec = 0, i;
	static uint8_t len = 0;
	dec = 0;
	len = 0;
	len = strlen((char*)number);
	for( i = 0; i < len; i++ )
	dec = dec * 10 + (number[i] - '0');

	return dec;
}
/******************************************************************************
* Function : Convert_Integer_To_String()
*//**
* \b Description:
*
* This function is used to convert string integer to string value
*
* PRE-CONDITION: None
*
* POST-CONDITION: The Number which is passed to the Function is converted to
*				  string and stored in the address passed
*
* @param[in]	number to converted to String
* @param[out]	converted string is stored in the address passed to the function
*
* \b Example Example:
* @code
*  uint8_t strNumber[2]
*  uint16_t number = 22;
* 	Convert_String_To_Integer(strNumber, number);
* @endcode
*
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 15/05/2024 </td><td> 0.1.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
void Convert_Integer_To_String(uint8_t * const str, uint16_t number)
{
	uint16_t n, len = 0, i, rem;
	n = number;
	if(0 == n)
		len = 1;
	while (0 != n)
	{
		len++;
		n /= 10;
	}

	for ( i = 0; i < len; i++ )
	{
		rem = number % 10;
		number = number / 10;
		str[len - (i + 1)] = rem + '0';
	}
	str[len] = '\0';
}


/******************************************************************************
* Function : Str_Cpy_Marker()
*//**
* \b Description:
*
* This function is extract the string in between the startMarker and end Marker
* from source string and store the the same in the destination array
*
* PRE-CONDITION: None
*
* POST-CONDITION: The string between the start Marker and End Marker in the
*				  source string is copied to the destination string.
*
* @param[in]	Address of the Source String
* @param[out]	Address of the Destination String
* @param[in]	The starting delimiter of the string
* @param[in]	The Ending delimiter of the String

* @return		Address of the next location to End delimiter
* \b Example Example:
* @code
*  uint8_t SrcString[50] = "sijeo@hotmail.com";
*	uint8_t destString[20];
*  uint8_t startMarker = '@';
* uint8_t EndMarker = '.';
* 	Str_Cpy_Marker(SrcString, destString, startMarker, EndMarker);
*  /// The string hotmail will be stored in the dstString
* @endcode
*
*
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 15/05/2024 </td><td> 0.5.0            </td><td> SP      </td><td> Interface Created </td></tr>
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint8_t * Str_Cpy_Marker(uint8_t * src, uint8_t * dest, uint8_t startMarker, uint8_t endMarker)
{
	uint8_t *s, *d;
	uint16_t bytes=0;
	while(*src != startMarker){
		src++;
		if(*src == '\0')
			return 0;
	}
	s = src;
	src++;
	s++;
	while(*src != endMarker){
		src++;
		if(*src == '\0')
			return 0;
	}
	d = src;
	while(s!=d){
		*dest = *s;
		dest++;
		s++;
		bytes++;
	}
	*dest = '\0';
	return d;
}


void strConcat(char* src, char* dest){
	for(;*src != '\0';src++);
	for(;*dest != '\0';dest++,src++){
		*src = *dest;
	}
	*src = '\0';
}


void convert_to_epoch(char* date_time, uint32_t* result)
{
    char isLeapYear=0, month, date, hours, minutes, seconds;
    int year = 0;
    int no_of_yrs = 0;
    int leapdays = 0;
    int yr = 0, m = 0;
    int currentdays = 0;
    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int current_sec=0;
    uint32_t totaldays;
    int32_t correction = 0;
    uint8_t sign;
	int8_t tz;

    year = (*date_time++ - '0') * 10;
    year += (*date_time++ - '0');
    year += 2000;
    date_time++;  //skip /
    month = (*date_time++ - '0') * 10;
    month += (*date_time++ - '0');
    date_time++;  //skip /
    date = (*date_time++ - '0') * 10;
    date += (*date_time++ - '0');
    date_time++;  //skip ,
    hours = (*date_time++ - '0') * 10;
    hours += (*date_time++ - '0');
    date_time++;  //skip :
    minutes = (*date_time++ - '0') * 10;
    minutes += (*date_time++ - '0');
    date_time++;  //skip :
    seconds = (*date_time++ - '0') * 10;
    seconds += (*date_time++ - '0');
    sign = date_time++;
    tz = (*date_time++ - '0') * 10;
    tz += (*date_time++ - '0');
    date_time++;
    if(sign == '-'){
    	tz= tz * -1;
    }
    correction = ((5.5-(float)(tz/4.0)) * 3600);
    no_of_yrs = year - EPOCH_START_YEAR;
    totaldays = no_of_yrs * 365;

    for (yr = EPOCH_START_YEAR; yr < year; yr++){
        isLeapYear = (yr % 4 == 0 && yr % 100 != 0) || (yr % 400 == 0);
        if (isLeapYear == 1 )
        {
            leapdays++;
        }
    }
    isLeapYear =0;
    isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    for (m = 1; m < month; ++m){
        currentdays += days_in_month[m-1];
        if ((m == 2) && (isLeapYear == 1)){
            leapdays = leapdays + 1;
        }
    }
    totaldays += leapdays;
    currentdays += (date - 1);
    current_sec = (hours * 3600) + (minutes * 60) + seconds;
    *result = ((totaldays + currentdays) * 86400) + current_sec;
    *result += correction;
}
