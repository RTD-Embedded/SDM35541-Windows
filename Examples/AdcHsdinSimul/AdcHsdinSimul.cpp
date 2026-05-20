/**
    @file

    @brief
        Demonstrates simultaneous ADC and High Speed Digital Input
        (HSDIN) Sampling with DMA.

    @verbatim

    This program simultaneously samples Analog and High Speed Digital data
    acquisition via DMA.

    About 0x80000 samples are gathered on each source at various speeds and
    buffer sizes.  This is done to show the versatility of the driver's DMA
    engine.

    The samples are printed to the screen at the end of the program.

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

        $Id: AdcHsdinSimul.cpp 152193 2026-02-13 21:38:28Z bkorpacz $
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

#include "AdcHsdinSimul.h"

/**
 * Whether a DAC interrupt has occurred.
 */
volatile bool hsdin_interrupt_occurred = false;

/**
 * Whether an ADC interrupt has occurred.
 */
volatile bool adc_interrupt_occurred = false;

/**
 * Number of ADC interrupts handled
 */
unsigned int adc_num_ints = 0;

/**
 * Number of HSDIN interrupts handled
 */
unsigned int hsdin_num_ints = 0;

/**
 * ADC DMA Data
 */
uint16_t adc_dma[ADC_NUM_DATA];

/**
 * HSDIN DMA Data
 */
uint16_t hsdin_dma[HSDIN_NUM_DATA];

/**
 * Whether ADC buffers are filled with DMA data or not.
 */
bool adc_filled_buffers[SDM35541_NUM_DMA_BUFFERS];

/**
 * Whether ADC buffers are filled with DMA data or not.
 */
bool hsdin_filled_buffers[SDM35541_NUM_DMA_BUFFERS];

/**
 * SDM35541 Board descriptor
 */
struct SDM35541_Board_Descriptor* board;

/**
 * ADC1002 Function Block
 */
struct SDM35541_Function_Block my_function_block;

/**
 * Management of shared objects.
 */
CRITICAL_SECTION critical_section;

/**
 * Status of the ADC callback function
 */
volatile int adc_callback_result;

/**
 * Status of the DAC callback function
 */
volatile int hsdin_callback_result;

/**
 * Variable to allow graceful exit from Ctrl-C
 */
static volatile bool exit_program;


/**
 * ADC DMA Callback
 */
void MyAdcCallback(SDM35541_DMA_Callback_Info int_info) {
    EnterCriticalSection(&critical_section);
    adc_filled_buffers[int_info.buffer_num] = true;
    adc_callback_result= int_info.result;
    adc_interrupt_occurred = true;
    LeaveCriticalSection(&critical_section);
}


/**
 * DAC DMA Callback
 */
void MyHsdinCallback(SDM35541_DMA_Callback_Info int_info) {
    EnterCriticalSection(&critical_section);
    hsdin_filled_buffers[int_info.buffer_num] = true;
    hsdin_callback_result = int_info.result;
    hsdin_interrupt_occurred = true;
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
    unsigned int fifo_size;
    int minor_number;
    int result;

    /* Install Ctrl+C handler */
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlCHandler, true);

    InitializeCriticalSection(&critical_section);

    printf("\n\tSDM35541 ADC and HSDIN Simultaneous Sampling Example Program\n\n");

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
    printf("Opening SDM35541 with minor number %lu...\n",
        minor_number);
    result = SDM35541_Board_Open(minor_number, &board);
    Fail(result, "Error opening board.");

    /* Reset the board. */
    printf("Board Reset...\n");
    result = SDM35541_Gbc_Board_Reset(board);
    Fail(result, "Error resetting board.");

    /* Open ADC1002 function block */
    printf("Opening sdm35541 function block...\n");
    result = SDM35541_Function_Block_Open(board, SDM35541_FUNCTION_BLOCK_ADC1002, &my_function_block);
    Fail(result, "Failed to open ADC1002 function block");

    /*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
       Initialize and Start DMA
       %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

    /* Clear ADC FIFO */
    result = SDM35541_ADC_Clear(board, &my_function_block);
    Fail(result, "Error clearing ADC FIFO.");

    /* Setup User Timer/Counter 0 */
    printf("User Timer / Counter 0 Setup...\n");
    result = SDM35541_UTC_Setup(board, &my_function_block,
        SDM35541_UTC_0,
        SDM35541_CUTC_8_MHZ,
        SDM35541_GUTC_GATED,
        SDM35541_UTC_MODE_RATE_GENERATOR,
        (uint16_t)(8000000 / HSDIN_RATE));
    Fail(result, "Error setting up UTC 0.");

    /* Get FIFO Size */
    result = SDM35541_FIFO_Size(board, &fifo_size);
    Fail(result, "Error getting FIFO size.");

    /* Setup User Timer / Counter 1 */
    printf("User Timer / Counter 1 Setup...\n");
    result = SDM35541_UTC_Setup(board, &my_function_block,
        SDM35541_UTC_1,
        SDM35541_CUTC_HSDIN_SIGNAL,
        SDM35541_GUTC_NOT_GATED,
        SDM35541_UTC_MODE_RATE_GENERATOR,
        (fifo_size / 2));
    Fail(result, "Error setting up UTC 1.");

    /* Setup HSDIN */
    HsdinInit();

    /* Setup ADC */
    AdcInit();

    /* Start ADC DMA */
    printf("Starting ADC DMA...\n");
    result = SDM35541_Dma_Start(board, &my_function_block, SDM35541_DMA_CHANNEL_ADC);
    Fail(result, "Could not start ADC DMA.");

    /* Start HSDIN DMA */
    printf("Starting HSDIN DMA Channel...\n");
    result = SDM35541_Dma_Start(board, &my_function_block, SDM35541_DMA_CHANNEL_HSDIN);
    Fail(result, "Error starting HSDIN DMA.");

    /* Start the pacer clock */
    printf("Starting Pacer Clock...\n");
    result = SDM35541_PCLK_Start(board, &my_function_block);
    Fail(result, "Error starting pacer clock.");

    /* Start UTC 0 */
    printf("Starting UTC 0...\n");
    result =
        SDM35541_UTC_Set_Gate(board, &my_function_block, SDM35541_UTC_0, SDM35541_GUTC_NOT_GATED);
    Fail(result, "Error starting UTC 0.");

    printf("Obtaining Samples...\n");

    while (!exit_program &&
        (adc_num_ints < ADC_NUM_INTS || hsdin_num_ints < HSDIN_NUM_INTS))
    {
        bool adc_int;
        bool hsdin_int;

        /* Check if an interrupt occurred */
        EnterCriticalSection(&critical_section);
        adc_int = adc_interrupt_occurred;
        hsdin_int = hsdin_interrupt_occurred;
        LeaveCriticalSection(&critical_section);

        if (adc_int) {
            AdcHandleInterrupt();
        }

        if (hsdin_int) {
            HsdinHandleInterrupt();
        }

        /* Clear interrupts if they occurred */
        if (hsdin_int || adc_int) {
            EnterCriticalSection(&critical_section);
            hsdin_interrupt_occurred = false;
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

    /* Print out the samples we obtained. */
    for (unsigned int i = 0; i < (ADC_NUM_DATA / sizeof(uint16_t)); i++) {
        int16_t adc_data, hsdin_data;
        if (exit_program) {
            break;
        }

        adc_data = SDM35541_ADC_ANALOG_DATA(adc_dma[i]);
        hsdin_data = (uint8_t)hsdin_dma[i];

        printf("%2.2f | 0x%02x \n",
            (adc_data / 4096.) * 10,
            hsdin_data);
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

    /* Delete critical section */
    DeleteCriticalSection(&critical_section);
    
    /* Disable DMA */
    printf("Disabling ADC DMA...\n");
    result = SDM35541_Dma_Stop(board, &my_function_block, SDM35541_DMA_CHANNEL_ADC);
    Fail(result, "Error stopping ADC DMA.");

    /* Disable DMA */
    printf("Disabling HSDIN DMA...\n");
    result = SDM35541_Dma_Stop(board, &my_function_block, SDM35541_DMA_CHANNEL_HSDIN);
    Fail(result, "Error stopping HSDIN DMA.");

    /* Remove ADC Callbacks */
    printf("Removing ADC ISR callbacks...\n");
    for (int i = 0; i < my_function_block.num_dma_buffers; i++) {
        result = SDM35541_DMA_Remove_Callback(board, my_function_block.fb_num, SDM35541_DMA_CHANNEL_ADC, i);
        check_result(result, "Error removing ADC callback.");
    }

    /* Remove HSDIN Callbacks */
    printf("Removing HSDIN ISR callbacks...\n");
    for (int i = 0; i < my_function_block.num_dma_buffers; i++) {
        result = SDM35541_DMA_Remove_Callback(board, my_function_block.fb_num, SDM35541_DMA_CHANNEL_HSDIN, i);
        check_result(result, "Error removing HSDIN callback.");
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
 * Checks for a non-zero return code,
 * prints the error message given and calls the Cleanup function
 */
void Fail(int result, const char* message) {
    if (result) {
        printf("Error (%d):%s\n", result, message);
        Cleanup();
        check_result(-1, "Failed function call.");
        system("pause");
    }
}
