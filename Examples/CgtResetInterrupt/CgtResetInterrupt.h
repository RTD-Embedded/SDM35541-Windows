/**
    @file

    @brief
        Header for the Channel Gain Table Reset Interrupt Example.

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

        $Id: CgtResetInterrupt.h 152088 2026-02-10 20:33:28Z bkorpacz $
*/

#pragma once


/**
 * The number of channels to sample
 */
#define NUM_CHANNELS 16

/**
 * Sample rate
 */
#define ADC_RATE 200

/**
 * SDM35541 Board descriptor
 */
struct SDM35541_Board_Descriptor *board;

/**
 * ADC1002 Function Block
 */
struct SDM35541_Function_Block my_function_block;

/**
 * Critical Section variable
 */
CRITICAL_SECTION critical_section;

/**
 * Variable to allow graceful exit from Ctrl-C.
 */
volatile bool exit_program = false;

/**
 * Whether an interrupt has occurred.
 */
volatile bool has_interrupt_occurred = false;

/**
 * Status of the interrupt callback function.
 */
volatile bool callback_result;

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
