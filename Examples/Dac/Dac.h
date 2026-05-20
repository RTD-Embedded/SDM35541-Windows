/**
    @file

    @brief
        Header for the DAC Example program.

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

        $Id: Dac.h 152256 2026-02-16 21:11:14Z bkorpacz $
*/
#pragma once

/**
 * Sample rate
 */
#define DAC_RATE 10000

/**
 * Whether to exit the program.
 */
extern volatile bool exit_program;

/**
 * Handler for CTRL - C events.Cleanup and exit.
 */
bool CtrlCHandler(DWORD fdwCtrlType);
