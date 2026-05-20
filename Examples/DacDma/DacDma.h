/**
    @file

    @brief
        Header for the DAC DMA Example program.

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

        $Id: DacDma.h 152193 2026-02-13 21:38:28Z bkorpacz $
*/
#pragma once

/**
 * Size of the FIFO to emulate in the driver.
 */
#define BUFFER_SIZE 		0x2000

/**
 * Size of each DMA buffer in bytes.
 */
#define BUFFER_SIZE_BYTES	(BUFFER_SIZE * sizeof(uint16_t))

 /**
  * Sample rate
  */
#define DAC_RATE			10000

/**
 * Prototype for the CTRL-C handler function.
 */
bool CtrlCHandler(DWORD fdwCtrlType);

/**
 * Close board, clean up interrupts, and free memory
 */
void Cleanup();

/**
 * Checks for a non-zero return code, prints the error message given and calls the CleanUp function
 */
void Fail(int result, const char* message);