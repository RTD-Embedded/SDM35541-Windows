/**
    @file

    @brief
        Header for the High Speed Digital Input DMA Example program.

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

        $Id: HsdinDma.h 152193 2026-02-13 21:38:28Z bkorpacz $
*/
#pragma once

/**
 * Amount of data we want from the board
 */
#define NUM_DATA 0x15000

/**
 * Number of entries in each DMA buffer
 */
#define DMA_BUFFER_SIZE	0x1000

/**
 * Number of bytes in each DMA buffer
 */
#define DMA_BUFFER_SIZE_BYTES	(DMA_BUFFER_SIZE * sizeof(uint16_t))

/**
 * Rate at which samples are collected
 */
#define RATE    50000		//50kHz

/**
 * Handler for CTRL - C events.Cleanup and exit.
 */
bool CtrlCHandler(DWORD fdwCtrlType);

/**
 * Cleanup memory.
 */
void Cleanup();

/**
 * Checks for a non-zero return code, prints the error message given and calls the CleanUp function
 */
void Fail(int result, const char* message);