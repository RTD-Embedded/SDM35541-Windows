
/**
    @file

    @brief
        Header for the ADC and HSDIN Example program.

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

#include "SDM35541.h"

/**
 * Size of HSDIN DMA Buffers
 */
#define DMA_HSDIN_BUFFER_SIZE 0x10000

 /**
  * Number of bytes in each HSDIN DMA buffer
  */
#define DMA_HSDIN_BUFFER_SIZE_BYTES (DMA_HSDIN_BUFFER_SIZE * sizeof(uint16_t))

/**
 * Size of ADC DMA Buffers
 */
#define DMA_ADC_BUFFER_SIZE 0x8000

/**
 * Number of bytes in each ADC DMA buffer
 */
#define DMA_ADC_BUFFER_SIZE_BYTES (DMA_ADC_BUFFER_SIZE * sizeof(uint16_t))

/**
 * Amount of data we want from the board
 */
#define ADC_NUM_DATA 0x80000

/**
 * Amount of data we want from the board
 */
#define HSDIN_NUM_DATA 0x80000

/**
 * Number of user ISR interrupts until we have the amount of data we want.
 */
#define ADC_NUM_INTS (ADC_NUM_DATA/(DMA_HSDIN_BUFFER_SIZE))

/**
 * Number of user ISR interrupts until we have the amount of data we want.
 */
#define HSDIN_NUM_INTS (HSDIN_NUM_DATA/(DMA_HSDIN_BUFFER_SIZE))

/**
 * A/D Sampling rate
 */
#define ADC_RATE 50000

/**
 * HD Sampling rate
 */
#define HSDIN_RATE 8000

/**
 * Whether a DAC interrupt has occurred.
 */
extern volatile bool hsdin_interrupt_occurred;

/**
 * Whether an ADC interrupt has occurred.
 */
extern volatile bool adc_interrupt_occurred;

/**
 * Status of the ADC callback function
 */
extern volatile int adc_callback_result;

/**
 * Status of the DAC callback function
 */
extern volatile int hsdin_callback_result;

/**
 * Management of shared objects.
 */
extern CRITICAL_SECTION critical_section;

/**
 * ADC DMA Data
 */
extern uint16_t adc_dma[ADC_NUM_DATA];

/**
 * HSDIN DMA Data
 */
extern uint16_t hsdin_dma[HSDIN_NUM_DATA];

/**
 * Number of ADC interrupts handled
 */
extern unsigned int adc_num_ints;

/**
 * Number of HSDIN interrupts handled
 */
extern unsigned int hsdin_num_ints;

/**
 * Whether ADC buffers are filled with DMA data or not.
 */
extern bool adc_filled_buffers[SDM35541_NUM_DMA_BUFFERS];

/**
 * Whether ADC buffers are filled with DMA data or not.
 */
extern bool hsdin_filled_buffers[SDM35541_NUM_DMA_BUFFERS];

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
void HsdinInit();

/**
 * Function to handle DAC Interrupt.
 */
void HsdinHandleInterrupt();

/**
 * DAC Callback function
 */
void MyHsdinCallback(SDM35541_DMA_Callback_Info int_info);

/**
 * ADC Callback function
 */
void MyAdcCallback(SDM35541_DMA_Callback_Info int_info);
