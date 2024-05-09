/*
 * strings.c
 *
 *  Created on: May 8, 2024
 *      Author: CEPL
 */

#include "strings.h"

void memorySet(char *destArr, char data, unsigned int len){
	for(len--; len>=0;len--)
		*destArr++ = data;
}

