/**
    @file

    @brief
        Demonstrates simultaneous Analog and Digital sampling.

    @verbatim

    Program samples out the DAC and in the ADC simultaneously via DMA.
    Different driver FIFO sizes are emulated to show the diversity of the
    DMA engine.  Also, the DAC and ADC are sampled at different rates to show
    that each interrupt source can be handled at various times.  The data
    captured during this example program is saved to a file named
    'test.txt'.

    @endverbatim

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

        $Id: AdcDacSimul.cpp 152256 2026-02-16 21:11:14Z bkorpacz $
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
#include "SDM35541.h"

#include "AdcDacSimul.h"

/* Begin variables declared in AdcDacSimul.h */
volatile bool adc_interrupt_occurred = false;

FILE* file_handle;

uint16_t dac_dma[DAC_BUFFER_SIZE];

uint16_t adc_dma[SDM35541_NUM_DMA_BUFFERS][ADC_BUFFER_SIZE];

bool adc_filled_buffers[SDM35541_NUM_DMA_BUFFERS];

struct SDM35541_Board_Descriptor* board;

struct SDM35541_Function_Block my_function_block;

CRITICAL_SECTION critical_section;

int adc_callback_result;

int adc_current_buffer = 0;

/* End variables declared in AdcDacSimul.h */

/**
 * Variable to allow graceful exit from Ctrl-C
 */
static volatile bool exit_program = false;


void MyAdcCallback(SDM35541_DMA_Callback_Info int_info) {
    EnterCriticalSection(&critical_section);
    adc_filled_buffers[int_info.buffer_num] = true;
    adc_callback_result= int_info.result;
    adc_interrupt_occurred = true;
    LeaveCriticalSection(&critical_section);
}


/**
********************************************************************************
@brief
    Main program code.
********************************************************************************
*/
int main(int argument_count, char** arguments)
{
    int minor_number;
    int result;
    float temp;

    result = fopen_s(&file_handle, DAT_FILE, "w");

    if (result < 0 || file_handle == NULL) {
        check_result(-1, "File Open Failure: " DAT_FILE);
    }

    /* Install Ctrl+C handler */
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlCHandler, true);

    InitializeCriticalSection(&critical_section);

    printf("\n\tSDM35541 ADC and DAC Simultaneous Sampling Example Program\n\n");

    /* Get board number */
    printf("Enter board number to use: ");
    if (scanf_s("%i", &minor_number) == 0) {
        Fail(-1, "Error getting board number\n");
    }
    Fail(
        minor_number < 0 || minor_number >= SDM35541_MAX_BOARDS,
        "Board number must be between 0-7.");

    /*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
       Device initialization
       %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    /* Open the sdm35541 device. */
    printf("Opening SDM35541 with minor number %u...\n",
        minor_number);
    result = SDM35541_Board_Open(minor_number, &board);
    Fail(result, "Error opening board.");
    
	/* Reset the board. */
	printf("Resetting the SDM35541...\n");
	result = SDM35541_Gbc_Board_Reset(board);
	Fail(result, "Failed to reset the board.");

    /* Open ADC1002 function block */
    printf("Opening SDM35541 function block...\n");
    result = SDM35541_Function_Block_Open(board, SDM35541_FUNCTION_BLOCK_ADC1002, &my_function_block);
    Fail(result, "Failed to open ADC1002 function block");

    /*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
       Initialize and Start DMA
       %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    /* Clear ADC FIFO */
    result = SDM35541_ADC_Clear(board, &my_function_block);
    Fail(result, "Error clearing ADC FIFO.");

    /* Setup DAC */
    DacInit();

    /* Setup ADC */
    AdcInit();

    /* Set DAC Rate */
    printf("Setting DAC Rate...\n");
    result =
        SDM35541_DAC_Set_Rate(board, &my_function_block, SDM35541_DAC_FREQ_8_MHZ, DAC_RATE, &temp);
    Fail(result, "Error setting DAC2 rate");
    printf("DAC Rate set to % 6.2f \n", temp);

    /* Start the pacer clock */
    printf("Starting Pacer Clock...\n");
    result = SDM35541_PCLK_Start(board, &my_function_block);
    Fail(result, "Error starting pacer clock.");

    while (!exit_program) {
        bool adc_int;

        /* Check if a DAC or ADC interrupt occurred */
        EnterCriticalSection(&critical_section);
        adc_int = adc_interrupt_occurred;
        LeaveCriticalSection(&critical_section);

        if (adc_int) {
            AdcHandleInterrupt();
        }

        /* Clear interrupts if they occurred */
        if (adc_int) {
            EnterCriticalSection(&critical_section);
            adc_interrupt_occurred = false;
            LeaveCriticalSection(&critical_section);

            /* Clear IT register */
            result = SDM35541_Clear_IT_Mask(board, &my_function_block, (SDM35541_int_source_t)0xffff);
            Fail(result, "Failed to clear IT register.");

            /* Clear IT overrun register */
            result = SDM35541_Clear_IT_Overrun(board, &my_function_block);
            Fail(result, "Failed to clear IT overrun register.");
        }

        SDM35541_Micro_Sleep(100);
    }

    /* Clean up and close board */
    Cleanup();
    
    system("pause");

    return 0;
}


/**
 * Handler for CTRL - C events.Cleanup and exit.
 */
bool CtrlCHandler(DWORD fdwCtrlType) {
    switch (fdwCtrlType) {
    case CTRL_C_EVENT:
        printf("CTRL-C pressed, exiting test\n");
        exit_program = true;
        return true;
    default:
        return false;
    }
}


/**
 * Close board, clean up interrupts, and free memory
 */
void Cleanup() {
    int result;

    /* Close file if it was opened */
    if (file_handle != NULL) {
        fclose(file_handle);
    }

    DeleteCriticalSection(&critical_section);

    /* Disable ADC DMA */
    printf("Disabling ADC DMA...\n");
    result = SDM35541_Dma_Stop(board, &my_function_block, SDM35541_DMA_CHANNEL_ADC);
    Fail(result, "Error stopping ADC DMA.");

    /* Disable DAC DMA */
    printf("Disabling DAC DMA...\n");
    result = SDM35541_Dma_Stop(board, &my_function_block, SDM35541_DMA_CHANNEL_DAC2);
    Fail(result, "Error stopping DAC2 DMA.");

    /* Remove ADC Callbacks */
    printf("Remove ISR callback...\n");
    for (int i = 0; i < my_function_block.num_dma_buffers; i++) {
        result = SDM35541_DMA_Remove_Callback(board, my_function_block.fb_num, SDM35541_DMA_CHANNEL_ADC, i);
        check_result(result, "Error removing ADC callback.");
    }

    /* Reset the board. */
    printf("Board Reset...\n");
    result = SDM35541_Gbc_Board_Reset(board);
    check_result(result, "Error resetting the board.");

    /* Close the sdm35541 device. */
    printf("Closing the sdm35541...\n");
    result = SDM35541_Board_Close(board);
    check_result(result, "Error closing board.");
}


/**
 * Checks for a non-zero return code, prints the error message given and calls the CleanUp function
 */
void Fail(int result, const char* message) {
    if (result) {
        printf("Error (%d):%s\n", result, message);
        Cleanup();
        check_result(-1, "Failed function call.");
        system("pause");
    }
}
