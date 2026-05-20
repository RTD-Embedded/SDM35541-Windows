/**
    @file

    @brief
        Demonstrates the use of Analog to Digital Conversion.

    @verbatim

    This example program demonstrates simple Analog sampling on a single
    channel.  When sampling on a signal channel the channel gain table latch
    must be set as shown in this example program.  Samples are gathered until
    the FIFO is filled, then they are printed to the screen.

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

        $Id: Adc.cpp 152236 2026-02-16 19:22:03Z bkorpacz $
*/

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sdm35541_gbc_library.h"
#include "sdm35541_util_library.h"
#include "sdm35541_registers.h"
#include "sdm35541_adc1002_library.h"
#include "SDM35541.h"

#include "Adc.h"


int main(int argc, char** argv) {
    struct SDM35541_Board_Descriptor* board;
    struct SDM35541_Function_Block my_func_block;
    SDM35541_cgt_entry_t cgt = { 0 };
    int help_option_given = 0;
    int minor_number = 0;
    int channel_number = 0;
    float actualRate;
    uint16_t data;
    int result, i;

    /* Get board number */
    printf("Enter board number to use: ");
    if (scanf_s("%i", &minor_number) == 0) {
        check_result(-1, "Error getting board number\n");
    }

    check_result(minor_number < 0 || minor_number >= SDM35541_MAX_BOARDS, "Board number must be between 0-7.");
    
    /* Get ADC channel number */
    printf("Enter ADC Channel number (1-16): ");
    if (scanf_s("%i", &channel_number) == 0) {
        check_result(-1, "Error getting ADC channel number\n");
    }

    check_result(channel_number < 1 || channel_number > 16, "Channel number must be between 1-16.");

    /* Channel numbers in the library use 0-based indexing */
    channel_number--;

    /* Open the SDM35541 device. */
    printf("Opening SDM35541...");
    result = SDM35541_Board_Open(minor_number, &board);
    check_result(result, "Unable to open SDM35541.");
    
	/* Reset the board. */
	printf("Resetting the SDM35541...\n");
	result = SDM35541_Gbc_Board_Reset(board);
	check_result(result, "Failed to reset the board.");

    /* Open the SDM35541 function block */
    printf("Opening SDM35541 function block 0\n");
    result = SDM35541_Function_Block_Open(board,
        SDM35541_FUNCTION_BLOCK_ADC1002, &my_func_block);
    check_result(result, "Error opening the ADC1002 function block.");

    /* Clear ADC FIFO */
    printf("Clear ADC FIFO...\n");
    result = SDM35541_ADC_Clear(board, &my_func_block);
    check_result(result, "Error clearing ADC FIFO.");

    /* Check FIFO Status */
    result = SDM35541_FIFO_Get_Status(board, &my_func_block, &data);
    check_result(result, "Error getting FIFO status.");
    printf("FIFO Status: 0x%4x\n", data);

    /* Enable Channel Gain Table */
    printf("Enabling Channel Gain Latch...\n");
    result = SDM35541_CGT_Enable(board, &my_func_block, 0x00);
    check_result(result, "Error enabling Channel Gain Table.");

    /* Create CGT Entry*/
    cgt.channel = channel_number;

    /* Write the entry to the CGT Latch register (used for single channel) */
    printf("Writing Channel Gain Table entry...\n");
    result = SDM35541_CGT_Latch(board, &my_func_block, cgt);
    check_result(result, "Error latching entry into Channel Gain Table.");

    /* Setup pacer clock */
    printf("Setting up Pacer Clock...\n");
    result = SDM35541_PCLK_Setup(board, &my_func_block,
        SDM35541_PCLK_INTERNAL,
        SDM35541_PCLK_FREQ_8_MHZ,
        SDM35541_PCLK_NO_REPEAT,
        SDM35541_PCLK_START_SOFTWARE,
        SDM35541_PCLK_STOP_SOFTWARE,
        ADC_RATE, &actualRate);
    check_result(result, "Error setting up pacer clock.");

    /* Set ADC Conversion Signal Select */
    result =
        SDM35541_ADC_Conv_Signal(board, &my_func_block, SDM35541_ADC_CONV_SIGNAL_PCLK);
    check_result(result, "Error setting ADC conversion signal.");

    /* Set User Output Signal 0 to A/D Conversion */
    printf("Setting User Output Signal 0...\n");
    result = SDM35541_UIO_Select(board, &my_func_block, SDM35541_UIO0, SDM35541_UIO_ADC);
    check_result(result, "Error setting user output signal 0.");

    /* Start the pacer clock */
    printf("Starting Pacer Clock...\n");
    result = SDM35541_PCLK_Start(board, &my_func_block);
    check_result(result, "Error starting pacer clock.");

    printf("Filling FIFO...\n");

    do {
        /* Check FIFO status */
        result = SDM35541_FIFO_Get_Status(board, &my_func_block, &data);
        check_result(result,
            "Error getting FIFO result.");
    } while (data & SDM35541_FIFO_ADC_NOT_FULL);

    /* Stop the pacer clock */
    printf("Stopping Pacer Clock...\n");
    result = SDM35541_PCLK_Stop(board, &my_func_block);
    check_result(result, "Error stopping pacer clock.");

    /* Read out samples */
    printf("Reading Samples...\n");

    i = 0;
    do {
        /* Read AD FIFO */
        result = SDM35541_ADC_FIFO_Read(board, &my_func_block, &data);
        check_result(result,
            "Error reading ADC FIFO");

        printf("\t%2.2f\n",
            ((SDM35541_ADC_ANALOG_DATA(data) / 4096.) * 10));

        /* Increment sample counter. */
        i++;

        /* Read the FIFO result */
        result = SDM35541_FIFO_Get_Status(board, &my_func_block, &data);
        check_result(result,
            "Error getting FIFO result.");

    } while (data & SDM35541_FIFO_ADC_NOT_EMPTY);

    /* Print how many samples were received */
    printf("Received %d samples...\n", i);

    /* Reset the sdm35541 device. */
    printf("Resetting the sdm35541...\n");
    result = SDM35541_Gbc_Board_Reset(board);
    check_result(result, "Error resetting the board.");

    /* Close the sdm35541 device. */
    printf("Closing the SDM35541...\n");
    result = SDM35541_Board_Close(board);
    check_result(result, "Error closing the board.");

    system("pause");

    return 0;
}