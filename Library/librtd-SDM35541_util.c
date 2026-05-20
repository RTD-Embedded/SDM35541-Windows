/**
	@file

	@brief
		SDM35541 user library source code

	@verbatim
	--------------------------------------------------------------------------
	This file and its contents are copyright (C) RTD Embedded Technologies,
	Inc.  All Rights Reserved.

	This software is licensed as described in the RTD End-User Software License
	Agreement.  For a copy of this agreement, refer to the file LICENSE.TXT
	(which should be included with this software) or contact RTD Embedded
	Technologies, Inc.
	--------------------------------------------------------------------------
	@endverbatim

	$Id: librtd-SDM35541_util.c 152236 2026-02-16 19:22:03Z bkorpacz $
*/

#include "stdint.h"
#include "windows.h"

#define _USE_MATH_DEFINES /* gets the M_PI and other useful things defined */
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "SDM35541_gbc_library.h"
#include "SDM35541_util_library.h"

SDM35541LIB_API
void SDM35541_Micro_Sleep(unsigned long microsecs) {
    
    LARGE_INTEGER start_count;
    LARGE_INTEGER counts_per_sec;
    LARGE_INTEGER current_count;
    double ticks_per_microsec;
    double ticks_passed;
    
    
    if(QueryPerformanceFrequency(&counts_per_sec) == 0)
    {
        return;
    }

    ticks_per_microsec = (double)(counts_per_sec.QuadPart) / 1000000.0;
    
    if(QueryPerformanceCounter(&start_count) == 0)
    {
        return;
    }
    
    while(1){
        // query the counter until at least m_SleepTimeMS has passed 
        if(QueryPerformanceCounter(&current_count) == 0)
        {
            break;
        }
        
        ticks_passed = (double)(current_count.QuadPart - start_count.QuadPart);
        if(ticks_passed >= (ticks_per_microsec * microsecs))
        {
            break;
        }
    }
    return;
}


uint32_t SDM35541_Get_Maskable(uint16_t data, uint16_t mask)
{

	return (data << 16) | mask;
}


SDM35541LIB_API
int SDM35541_Generate_Signal_Data(enum SDM35541_Waveforms waveform,
				 int32_t * data,
				 uint32_t data_count,
				 int32_t max,
				 int32_t minimum, int32_t offset, uint32_t mask)
{
	float value = 0.0f;
	float increment = 0.0f;
	unsigned int index = 0;
	int32_t center;

	if (data == NULL || (data_count < 1)) {
		errno = EINVAL;
		return -1;
	}

	if (mask == 0) {
		mask = 0xFFFFFFFF;
	}

	switch (waveform) {
	case SDM35541_SINE_WAVE:

        center = minimum + ((max - minimum) / 2);

		increment = (2.0f * (float)M_PI) / ((float)data_count);

		for (index = 0; index < data_count; index++) {

			data[index] = (int32_t) (sin(value) * ((float) (max - center))) + center;
			data[index] += offset;
			data[index] = data[index] & mask;
			value += increment;
		}

		break;
	case SDM35541_SQUARE_WAVE:

		for (index = 0; index < (data_count / 2); index++) {
			data[index] = max + offset;
			data[index] = data[index] & mask;

		}

		for (index = (data_count / 2); index < data_count; index++) {
			data[index] = minimum + offset;
			data[index] = data[index] & mask;
		}
		break;

	case SDM35541_SAWTOOTH_WAVE:
		increment = ((float)(max - minimum)) / (float)data_count;
		value = (float)minimum;

		for (index = 0; index < data_count; index++) {
			data[index] = ((int32_t) value) + offset;
			data[index] = data[index] & mask;
			value += increment;
		}
		break;
	default:
		errno = EINVAL;
		return -2;
		break;

	}

	return 0;
}

/**
*******************************************************************************
@brief
    Check the result of an operation, usually a library call.  If the result is
    non-zero, then it is an error and output the passed message.

@param
    return_val

    Value to be evaluated.  Non-zero values will be considered an error.

@param
    message

    Pointer to string that will be output if an error condition exists.

@retval
    None

 *******************************************************************************
*/
SDM35541LIB_API
void check_result(int return_val, const char *message){
    if (return_val != 0) 
    {
		printf("\n\nERROR(%d): %s", return_val, message);
        exit(EXIT_FAILURE);
	}
}