/**
    @file

    @brief
        Demonstrates simultaneous Analog and Digital sampling.

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

        $Id: Adc.cpp 152297 2026-02-18 14:52:52Z bkorpacz $
*/

#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sdm35541_gbc_library.h"
#include "sdm35541_examples.h"
#include "sdm35541_util_library.h"
#include "sdm35541_registers.h"
#include "sdm35541_adc1002_library.h"
#include "sdm35541_os.h"
#include "sdm35541_dma_library.h"

#include "AdcDacSimul.h"


void AdcInit() {
    SDM35541_cgt_entry_t cgt = { 0 };
    float temp;
    int result;
    int i;

    /*
     * Tell DMA engine what is the target of DMA
     */
    printf("Setting ADC Channel Source...\n");
    result = SDM35541_DMA_Set_Source(board,
        &my_function_block,
        1, SDM35541_DMA_FIFO_ADC,
        SDM35541_DMA_DEMAND_FIFO_ADC);
    Fail(result, "Error setting ADC DMA source.");

    /*
     * Setup ADC DMA
     */
    printf("Setting up ADC DMA...\n");
    SDM35541_Dma_Setup(board, &my_function_block, SDM35541_DMA_CHANNEL_ADC,
        SDM35541_DMA_SETUP_DIRECTION_READ, NOT_IGNORE_USED);
    Fail(result, "Failed to setup ADC DMA.");

    /*
     * Set-up DMA interrupts
     */
    printf("Configuring ADC DMA Interrupts...\n");
    result = SDM35541_Dma_Configure_Interrupts(board, &my_function_block, SDM35541_DMA_CHANNEL_ADC,
        INTERRUPT_ENABLE, ERROR_INTR_ENABLE);
    Fail(result, "Error enabling ADC DMA interrupt.");

    /*
     * Set-up ADC buffers
     */
    for (i = 0; i < my_function_block.num_dma_buffers - 1; i++) {
        result = SDM35541_Dma_Buffer_Setup(board, &my_function_block, SDM35541_DMA_CHANNEL_ADC,
            i, SDM35541_DMA_BUFFER_CTRL_VALID | SDM35541_DMA_BUFFER_CTRL_INTR);
        Fail(result, "Failed to setup ADC buffer.");
    }

    /*
     * Set-up last buffer and set it to loop back to the first buffer
     */
    result = SDM35541_Dma_Buffer_Setup(board, &my_function_block, SDM35541_DMA_CHANNEL_ADC,
        i, SDM35541_DMA_BUFFER_CTRL_VALID
        | SDM35541_DMA_BUFFER_CTRL_INTR | SDM35541_DMA_BUFFER_CTRL_LOOP);
    Fail(result, "Failed to setup last ADC buffer.");

    /*
     * Install callbacks
     */
    for (i = 0; i < my_function_block.num_dma_buffers; i++) {
        result = SDM35541_DMA_Install_Callback(
            board,                      /* Pointer to board descriptor */
            my_function_block.fb_num,   /* Function block number */
            SDM35541_DMA_CHANNEL_ADC,   /* DMA Channel */
            i,                          /* Buffer num */
            MyAdcCallback,              /* Callback function */
            NULL
        );
        Fail(result, "Error installing callback.");

        result = SDM35541_DMA_Request_Transfer(
            board,
            my_function_block.fb_num,
            SDM35541_DMA_CHANNEL_ADC, /* dma channel */
            i, /* buffer number */
            SDM35541_DMA_OP_BOARD_TO_BUFFER, /* operation */
            adc_dma[i], /* buffer */
            ADC_BUFFER_SIZE_BYTES, /* size */
            false, /* demand */
            i, /* request id */
            0, /* timeout */
            false, /* wait */
            NULL /* dma result */
        );
        Fail(result, "Error starting DMA request transfer");
    }

    /*
     * Start ADC DMA
     */
    printf("Starting ADC DMA...\n");
    result = SDM35541_Dma_Start(board, &my_function_block, SDM35541_DMA_CHANNEL_ADC);
    Fail(result, "Could not start ADC DMA.");

    /* Enable Channel Gain Table */
    printf("Enabling Channel Gain Latch...\n");
    result = SDM35541_CGT_Enable(board, &my_function_block, 0x00);
    Fail(result, "Error enabling Channel Gain Table.");

    /* Write the entry to the CGT Latch register (used for single channel) */
    printf("Writing Channel Gain Table entry...\n");
    result = SDM35541_CGT_Latch(board, &my_function_block, cgt);
    Fail(result, "Error latching Channel Gain Table entry.");

    /* Setup pacer clock */
    printf("Setting up Pacer Clock...\n");
    result = SDM35541_PCLK_Setup(board, &my_function_block,
        SDM35541_PCLK_INTERNAL,
        SDM35541_PCLK_FREQ_8_MHZ,
        SDM35541_PCLK_NO_REPEAT,
        SDM35541_PCLK_START_SOFTWARE,
        SDM35541_PCLK_STOP_SOFTWARE,
        ADC_RATE, &temp);
    Fail(result, "Error setting up pacer clock.");

    /* Set ADC Conversion Signal Select */
    result =
        SDM35541_ADC_Conv_Signal(board, &my_function_block, SDM35541_ADC_CONV_SIGNAL_PCLK);
    Fail(result, "Error setting ADC Conversion Signal.");
}


void AdcHandleInterrupt() {
    int result;

    /* Check for ADC callback error */
    EnterCriticalSection(&critical_section);
    Fail(adc_callback_result, "Error in ADC Callback function");

    /*
     * When one interrupt occurred, we must check to see all buffers that have filled up
     */
    while (adc_filled_buffers[adc_current_buffer]) {
        printf("ADC Interrupt!\n");
        adc_filled_buffers[adc_current_buffer] = false;

        /* Write DMA buffer to file */
        for (unsigned int i = 0; i < ADC_BUFFER_SIZE; i++) {
            float data;
            data = (float)SDM35541_ADC_ANALOG_DATA(adc_dma[adc_current_buffer][i]);
            fprintf_s(file_handle, "%2.2f\n", data / 4096.0 * 10.0);
        }

        /* Queue another transfer */
        result = SDM35541_DMA_Request_Transfer(
            board,
            my_function_block.fb_num,
            SDM35541_DMA_CHANNEL_ADC, // ADC DMA channel
            adc_current_buffer, // DMA buffer number
            SDM35541_DMA_OP_BOARD_TO_BUFFER, // transfer data from board to use
            adc_dma[adc_current_buffer], // pointer to local buffer
            ADC_BUFFER_SIZE_BYTES, // size of this buffer in bytes
            false, // no demand
            adc_current_buffer, // request id
            0, // timeout = 0
            false, // no wait
            NULL);
        Fail(result, "Error calling SDM35541_DMA_Request_Transfer()");

        /* Move to next buffer */
        adc_current_buffer++;
        adc_current_buffer %= SDM35541_NUM_DMA_BUFFERS;
    }

    /* We've handled this callback. */
    adc_interrupt_occurred = false;

    LeaveCriticalSection(&critical_section);
}
