#pragma once
/**
    @file

    @brief
        Defines for the ADC DMA Example Program

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

    $Id: AdcDma.h 152256 2026-02-16 21:11:14Z bkorpacz $
*/

/**
 * Filename to dump the data
 */
#define DAT_FILE "./test.dat"

 /**
  * ADC sampling rate in Hz
  */
#define ADC_RATE 40000

/**
 * Number of indices in the local_buffer array.
 */
#define DMA_BUFFER_SIZE 20000

/**
 * Number of bytes the local_buffer array occupies
 */
#define DMA_BUFFER_SIZE_BYTES (DMA_BUFFER_SIZE * sizeof(uint16_t))

/**
 * User-space buffer that stores DMA samples.
 */
uint16_t local_buffer[SDM35541_NUM_DMA_BUFFERS][DMA_BUFFER_SIZE];

/**
 * SDM35541 Board descriptor
 */
struct SDM35541_Board_Descriptor* board;

/**
 * ADC1002 Function Block
 */
struct SDM35541_Function_Block my_function_block;

/**
 * File to store ADC data
 */
FILE* file_handle = NULL;

/**
 * Whether an interrupt has occurred.
 */
volatile bool has_interrupt_occurred = false;

/**
 * Result of the callback function
 */
int callback_result;

/**
 * Whether a DMA buffer is filled with data or not
 */
bool filled_buffers[SDM35541_NUM_DMA_BUFFERS] = { false };

/*
 * Variable used to count how many interrupts occurred.
 */
static volatile int interrupts;

/*
 * Variable to allow graceful exit from Ctrl-C.
 */
volatile bool exit_program;

/**
 * Management of shared objects.
 */
CRITICAL_SECTION critical_section;

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