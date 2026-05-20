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

        $Id: Dac.cpp 152236 2026-02-16 19:22:03Z bkorpacz $
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
#include "AdcDacSimul.h"


void DacInit() {
    float temp;
    int result;
    int i;

    /* Tell DMA engine what is the target of DMA */
    printf("Setting DAC Channel Source...\n");
    result = SDM35541_DMA_Set_Source(board, &my_function_block, 0,
        SDM35541_DMA_FIFO_DAC2,
        SDM35541_DMA_DEMAND_FIFO_DAC2);
    Fail(result, "Error setting DAC2 DMA source.");

    /* Set DMA direction and to consider whether buffers have been used or not */
    printf("Setting up DAC DMA...\n");
    SDM35541_Dma_Setup(board, &my_function_block, SDM35541_DMA_CHANNEL_DAC2,
        SDM35541_DMA_SETUP_DIRECTION_WRITE, IGNORE_USED);
    Fail(result, "Failed to setup DAC2 DMA.");

    /* Set-up DAC buffer */
    printf("Setting up DAC DMA Buffer...\n");
    result = SDM35541_Dma_Buffer_Setup(
        board,
        &my_function_block,
        SDM35541_DMA_CHANNEL_DAC2,
        0,
        SDM35541_DMA_BUFFER_CTRL_VALID
        | SDM35541_DMA_BUFFER_CTRL_INTR
        | SDM35541_DMA_BUFFER_CTRL_LOOP);
    Fail(result, "Failed to setup DAC buffer 0.");

    /* Create DAC2 waveform sinusoidal */
    for (i = 0; i < DAC_BUFFER_SIZE; i++) {
        temp = 2047 * (float)sin((float)i * (2 * M_PI / 1024.0));
        dac_dma[i] = ((int16_t)temp) << 3;
    }

    /* Request DAC DMA Transfer */
    result = SDM35541_DMA_Request_Transfer(
        board,
        my_function_block.fb_num,
        SDM35541_DMA_CHANNEL_DAC2, /* dma channel */
        0, /* buffer number */
        SDM35541_DMA_OP_BUFFER_TO_BOARD, /* operation */
        dac_dma, /* buffer */
        DAC_BUFFER_SIZE_BYTES, /* size */
        false, /* demand */
        0, /* request id */
        0, /* timeout */
        false, /* wait */
        NULL /* dma result */
    );
    Fail(result, "Error starting DMA request transfer.");
        
    /* Start DAC DMA */
    printf("Starting DAC DMA...\n");
    result = SDM35541_Dma_Start(board, &my_function_block, SDM35541_DMA_CHANNEL_DAC2);
    Fail(result, "Error starting DAC2 DMA.");

    /* Setup DAC 2 */
    printf("Setup DAC2...\n");
    result =
        SDM35541_DAC_Setup(board, &my_function_block, SDM35541_DAC2, SDM35541_DAC_RANGE_BIPOLAR_5,
            SDM35541_DAC_UPDATE_CLOCK, SDM35541_DAC_MODE_CYCLE);
    Fail(result, "Error setting up DAC2.");
}