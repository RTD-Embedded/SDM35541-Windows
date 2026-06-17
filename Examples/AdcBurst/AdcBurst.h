
/**
    @file

    @brief
        Header file for the ADC Burst example program.

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

		$Id: AdcBurst.h 152331 2026-02-19 15:47:35Z bkorpacz $
*/
#pragma once

/**
 * The number of channels to sample
 */
#define NUM_CHANNELS 16

/**
 * Burst Clock Rate
 */
#define BURST_RATE (1000000 / 2)

/**
 * Pacer Clock Rate
 */
#define PACER_RATE 100000
