
/**
    @file

    @brief
        Header for the ADC and DAC Example program.

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

        $Id: AdcDacSimul.h 152193 2026-02-13 21:38:28Z bkorpacz $
*/
#pragma once

#include "SDM35541.h"

/**
 * Size of each DAC Buffer
 */
#define DAC_BUFFER_SIZE 0x8000

 /**
  * Size of each DAC DMA Buffer in bytes
  */
#define DAC_BUFFER_SIZE_BYTES (DAC_BUFFER_SIZE * sizeof(uint16_t))

/**
 * Size of the ADC FIFO to emulate in the driver
 */
#define ADC_BUFFER_SIZE 0x8000

/**
 * Size of each DAC DMA Buffer in bytes
 */
#define ADC_BUFFER_SIZE_BYTES (ADC_BUFFER_SIZE * sizeof(uint16_t))

/**
 * DAC Sample rate
 */
#define DAC_RATE 40000

/**
 * ADC Sample rate
 */
#define ADC_RATE 25000

/**
 * Filename to dump the data
 */
#define DAT_FILE "./test.dat"

/**
 * File storing ADC data
 */
extern FILE* file_handle;

/**
 * Whether a DAC interrupt has occurred.
 */
extern volatile bool dac_interrupt_occurred;

/**
 * Whether an ADC interrupt has occurred.
 */
extern volatile bool adc_interrupt_occurred;

/**
 * Status of the ADC callback function
 */
extern int adc_callback_result;

/**
 * Status of the DAC callback function
 */
extern int dac_callback_result;

/**
 * Management of shared objects.
 */
extern CRITICAL_SECTION critical_section;

/**
 * DAC DMA Data
 */
extern uint16_t dac_dma[DAC_BUFFER_SIZE];

/**
 * Current DAC DMA buffer.
 */
extern int adc_current_buffer;

/**
 * ADC DMA Data
 */
extern uint16_t adc_dma[SDM35541_NUM_DMA_BUFFERS][ADC_BUFFER_SIZE];

/**
 * Whether ADC buffers are filled with DMA data or not.
 */
extern bool adc_filled_buffers[SDM35541_NUM_DMA_BUFFERS];

/**
 * SDM35541 Board descriptor
 */
extern struct SDM35541_Board_Descriptor* board;

/**
 * ADC1002 Function Block
 */
extern struct SDM35541_Function_Block my_function_block;

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

/**
 * Initialize ADC
 */
void AdcInit();

/**
 * Function to handle ADC Interrupt.
 */
void AdcHandleInterrupt();

/**
 * Initialize DAC
 */
void DacInit();

/**
 * ADC Callback function
 */
void MyAdcCallback(SDM35541_DMA_Callback_Info int_info);