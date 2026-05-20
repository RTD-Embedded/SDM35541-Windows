/**
    @file

    @brief
        Initialization and interrupt handling for the HSDIN.

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

        $Id: Hsdin.cpp 152297 2026-02-18 14:52:52Z bkorpacz $
*/

#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define _USE_MATH_DEFINES
#include <cmath>

#include "sdm35541_gbc_library.h"
#include "sdm35541_examples.h"
#include "sdm35541_util_library.h"
#include "sdm35541_registers.h"
#include "sdm35541_adc1002_library.h"
#include "sdm35541_os.h"
#include "sdm35541_dma_library.h"
#include "AdcHsdinSimul.h"


/**
 * Current HSDIN DMA buffer.
 */
static int hsdin_current_buffer = 0;

/**
 * Location in the local HSDIN DMA buffer
 * to place the next DMA data.
 */
unsigned int hsdin_buffer_offset = 0;

void HsdinInit() {
    int result;
    int i;

    /* Set high speed digital sampling signal */
    printf("Setting High Speed Digital Sampling Signal...\n");
    result = SDM35541_HSDIN_Sample_Signal(board, &my_function_block,
        SDM35541_HSDIN_SIGNAL_UTC0);
    Fail(result,
        "Error setting HSDIN sample trigger mode.");

    /* Clear high speed digital FIFO */
    printf("Clearing high speed digital FIFO...\n");
    result = SDM35541_HSDIN_Clear(board, &my_function_block);
    Fail(result, "Error clearing HSDIN FIFO.");

    /* Tell DMA engine what is the target of DMA. */
    printf("Setting HSDIN Channel Source...\n");
    result = SDM35541_DMA_Set_Source(board, &my_function_block, 0,
        SDM35541_DMA_FIFO_HSDIN,
        SDM35541_DMA_DEMAND_UTC1);
    Fail(result, "Error setting HSDIN DMA source.");
    
    /* Set-up HSDIN DMA interrupts */
    printf("Configuring HSDIN DMA interrupts...\n");
    result = SDM35541_Dma_Configure_Interrupts(board,
        &my_function_block,
        SDM35541_DMA_CHANNEL_HSDIN,
        INTERRUPT_ENABLE,
        ERROR_INTR_ENABLE);
    Fail(result, "Error enabling DMA interrupts.");

    /* Setup HSDIN Buffers */
    printf("Setting up HSDIN buffers...\n");
    for (i = 0; i < my_function_block.num_dma_buffers; i++) {
        uint8_t ctrl = SDM35541_DMA_BUFFER_CTRL_VALID | SDM35541_DMA_BUFFER_CTRL_INTR;

        /* Last buffer should loop back to the first. */
        if (i == my_function_block.num_dma_buffers - 1) {
            ctrl |= SDM35541_DMA_BUFFER_CTRL_LOOP;
        }

        result = SDM35541_Dma_Buffer_Setup(board, &my_function_block, SDM35541_DMA_CHANNEL_HSDIN,
            i, ctrl);
        Fail(result, "Failed to setup HSDIN buffer.");
    }

    /* Install callbacks */
    for (i = 0; i < my_function_block.num_dma_buffers; i++) {
        result = SDM35541_DMA_Install_Callback(
            board,                          /* Pointer to board descriptor */
            my_function_block.fb_num,       /* Function block number */
            SDM35541_DMA_CHANNEL_HSDIN,     /* DMA Channel */
            i,                              /* Buffer num */
            MyHsdinCallback,                /* Callback function */
            NULL                            /* DMA result */
        );
        Fail(result, "Error installing callback.");

        result = SDM35541_DMA_Request_Transfer(
            board,                          /* Pointer to board descriptor */
            my_function_block.fb_num,       /* Function block number */
            SDM35541_DMA_CHANNEL_HSDIN,     /* DMA channel */
            i,                              /* Buffer number */
            SDM35541_DMA_OP_BOARD_TO_BUFFER,/* Operation */
            &hsdin_dma[hsdin_buffer_offset],/* Buffer */
            DMA_HSDIN_BUFFER_SIZE_BYTES,    /* Size */
            false,                          /* Demand */
            i,                              /* Request id */
            0,                              /* Timeout */
            false,                          /* wait */
            NULL                            /* DMA result */
        );
        Fail(result, "Error starting DMA request transfer");

        hsdin_buffer_offset += DMA_HSDIN_BUFFER_SIZE;
    }

    /* Set DMA direction and to consider whether buffers have been used or not */
    SDM35541_Dma_Setup(board,
        &my_function_block,
        SDM35541_DMA_CHANNEL_HSDIN,
        SDM35541_DMA_SETUP_DIRECTION_READ,
        NOT_IGNORE_USED);
    Fail(result, "Error setting up HSDIN DMA.");
}


void HsdinHandleInterrupt() {
    int result;

    /* Check for HSDIN callback error */
    EnterCriticalSection(&critical_section);
    Fail(hsdin_callback_result, "Error in HSDIN Callback function");

    /* When one interrupt occurred, we must check to see all buffers that have filled up */
    while (hsdin_filled_buffers[hsdin_current_buffer]) {
        printf("HSDIN Interrupt!\n");

        /* Allow buffer to be reused */
        hsdin_filled_buffers[hsdin_current_buffer] = false;

        /* Queue another transfer if there is space in the local buffer. */
        if (hsdin_buffer_offset < HSDIN_NUM_DATA) {
            result = SDM35541_DMA_Request_Transfer(
                board,
                my_function_block.fb_num,
                SDM35541_DMA_CHANNEL_HSDIN, // HSDIN DMA channel
                hsdin_current_buffer, // DMA buffer number
                SDM35541_DMA_OP_BOARD_TO_BUFFER, // transfer data from board to use
                &hsdin_dma[hsdin_buffer_offset], // pointer to local buffer
                DMA_HSDIN_BUFFER_SIZE_BYTES, // size of this buffer in bytes
                false, // no demand
                hsdin_current_buffer, // request id
                0, // timeout = 0
                false, // no wait
                NULL);
            Fail(result, "Error calling SDM35541_DMA_Request_Transfer()");
        }

        /* Increment number of received interrupts */
        hsdin_num_ints++;

        /* Move to next buffer */
        hsdin_current_buffer++;
        hsdin_current_buffer %= SDM35541_NUM_DMA_BUFFERS;
        hsdin_buffer_offset += DMA_HSDIN_BUFFER_SIZE;

        /* Stop HSDIN DMA when all of the data has been received. */
        if (hsdin_num_ints >= HSDIN_NUM_INTS) {
            printf("Stopping HSDIN DMA...\n");
            result = SDM35541_Dma_Stop(board, &my_function_block, SDM35541_DMA_CHANNEL_HSDIN);
            Fail(result, "Error stopping HSDIN");
            break;
        }
    }

    /* We've handled this callback. */
    hsdin_interrupt_occurred = false;

    LeaveCriticalSection(&critical_section);
}
