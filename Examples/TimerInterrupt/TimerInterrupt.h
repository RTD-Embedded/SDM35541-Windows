/**
    @file

    @brief
        Header for User Timer/Counter Out Interrupts example program.
        
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

		$Id: TimerInterrupt.h 152193 2026-02-13 21:38:28Z bkorpacz $
*/

#pragma once

/**
 * Rate of user timer/counter 0
 */
#define UTC0 8000
/**
 * Rate of user timer/counter 1
 */
#define UTC1 2000

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