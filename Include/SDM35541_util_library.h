/**
    @file

    @brief
        Definitions for the SDM35541 Utilities library, various helper
        functions.

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

    $Id: SDM35541_util_library.h 151713 2026-01-26 21:15:36Z bkorpacz $
*/

#ifndef _SDM35541_UTIL__H_
#define _SDM35541_UTIL__H_

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

 /**
  * @defgroup SDM35541_Util_Library_Functions SDM35541 Utility Library Functions
  * @{
  */

/**
 * @brief
 *      List of possible waveforms that can be generated for DAC purposes.
 */
enum SDM35541_Waveforms {

/**
 * A simple sine wave.
 */
	SDM35541_SINE_WAVE,

/**
 * A square wave starting at max value
 */
	SDM35541_SQUARE_WAVE,

/**
 * A sawtooth wave going for min to max
 */
	SDM35541_SAWTOOTH_WAVE
};

/**
*******************************************************************************
@brief
   Return a 32-bit maskable register value from the data and mask.

@param
   data

   Data portion (upper 16-bits) of the maskable.

@param
   mask

   Mask portion (lower 16-bits) of the maskable

@retval
   maskable

   Maskable register value.

*/
	uint32_t SDM35541_Get_Maskable(uint16_t data, uint16_t mask);

/**
*******************************************************************************
@brief
   Sleep for a specified number of microseconds.

@param
   microsecs

   Length of sleep (microseconds)

@retval
   None

*/
SDM35541LIB_API
	void SDM35541_Micro_Sleep(unsigned long microsecs);

/**
*******************************************************************************
@brief
   Calculate the time difference between the two timeval structs, in
   microseconds.

@param
   last

   The last (most recent) timeval to compare.

@param
   first

   The first (least recent) timeval to compare.

@retval
   difference

   The difference between the two timevals, in microseconds.

*/
	long SDM35541_Get_Time_Diff(struct timeval last, struct timeval first);

/**
*******************************************************************************
@brief
   Generate data with a specific wave pattern.  This is useful for producing
   recognizeable waves for DAC output.

@param
   waveform

   Enumerated value indicating what waveform to produce.

@param
   data

   Pointer to pre-allocated memory to hold resulting data values.

@param
   data_count

   Number of data samples to produce

@param
   max

   The maximum value in this generated data.

@param
   minimum

   The minimum value in this generated data.

@param
   offset

   Offset from 0 that will be the median value of this wave.

@param
   mask

   Bitmask that is applied to every calculated value.  This allows for handling of
   generated data that is less than 32 bits.  To use all 32-bits, the mask would
   be 0xFFFFFFFF.

@note
   No matter the data count, the returned data will only contain 1 period of the
   waveform.  A higher data count will result in a "finer" set of data.

@retval
   0

   Success

@retval
   Non-Zero

   Failure

*/
SDM35541LIB_API 
int SDM35541_Generate_Signal_Data(enum SDM35541_Waveforms waveform,
					 int32_t * data,
					 uint32_t data_count,
					 int32_t max,
					 int32_t minimum,
					 int32_t offset, uint32_t mask);

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
void check_result(int return_val, const char *message);

/**
 * @} SDM35541_Util_Library_Functions
 */

#ifdef __cplusplus
}
#endif
#endif
