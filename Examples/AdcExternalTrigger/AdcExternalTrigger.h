/**
    @file

    @brief
        Header for the ADC External Trigger Interrupt Example program.

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

        $Id: AdcExternalTrigger.h 152146 2026-02-12 16:38:32Z bkorpacz $
*/
#pragma once

/**
 * Size of each DMA buffer
 */
#define DMA_BUFFER_SIZE			0x1000

/**
 * Size of each DMA buffer in bytes
 */
#define DMA_BUFFER_SIZE_BYTES	(DMA_BUFFER_SIZE * sizeof(uint16_t))

/**
 * Number of user ISR interrupts until we have the amount of data we want.
 */
#define NUM_INTS				10

/**
 * Amount of data we want from the board
 */
#define NUM_DATA				(DMA_BUFFER_SIZE * NUM_INTS)

/**
 * Sampling rate
 */
#define ADC_RATE 				5000

/**
 * Variable used to terminate the program early
 */
static volatile int exit_program = 0;

/**
 * SDM35541 Board descriptor
 */
struct SDM35541_Board_Descriptor* board;

/**
 * ADC1002 Function Block
 */
struct SDM35541_Function_Block my_function_block;

/**
 * Whether callback function has been called.
 */
volatile bool has_interrupt_occurred = false;

/**
 * Whether the DMA callback function has been called.
 */
volatile bool has_dma_interrupt_occurred = false;

/**
 * Status of the DMA callback function.
 */
volatile bool dma_callback_result = 0;

/**
 * Status of the interrupt callback function.
 */
volatile bool interrupt_callback_result = 0;

/**
 * Whether ADC buffers are filled with DMA data or not.
 */
volatile bool filled_buffers[SDM35541_NUM_DMA_BUFFERS] = { false };

/**
 * Critical section variable
 */
CRITICAL_SECTION critical_section;

/**
 * Local buffer to store data
 */
uint16_t local_buffer[NUM_DATA];

/**
 * Handle board-level interrupts.
 */
void handle_interrupt();

/**
 * Handle DMA interrupts.
 */
void handle_dma_interrupt();

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