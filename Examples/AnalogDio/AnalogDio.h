/**
    @file

    @brief
        Header for the Analog DIO Example program.

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

        $Id: AdcHsdinSimul.h 152088 2026-02-10 20:33:28Z bkorpacz $
*/
#pragma once

/**
 * SDM35541 Board descriptor
 */
extern struct SDM35541_Board_Descriptor* board;

/**
 * ADC1002 Function Block
 */
extern struct SDM35541_Function_Block my_function_block;

/**
 * Analog DIO interrupts
 */
extern uint8_t algdio_ints;

/**
 * Whether callback function has been called.
 */
extern bool has_interrupt_occurred;
