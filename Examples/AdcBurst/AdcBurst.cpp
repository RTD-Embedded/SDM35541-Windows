/**
    @file

    @brief
        Demonstrates the use of Analog to Digital Burst sampling.

    @verbatim

    This example program using the Pacer Clock and Burst Clock to perform
    Burst sampling.  Burst sampling is near simultaneous sampling of a given
    number of channels as configured per the channel gain table.  In this
    example we sample from all 16 channels on each Pacer Clock conversion
    signal.  Samples are acquired until the FIFO is filled then they are
    printed to the screen.

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

		$Id: AdcBurst.cpp 152297 2026-02-18 14:52:52Z bkorpacz $
*/

#include <limits.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sdm35541_gbc_library.h"
#include "sdm35541_registers.h"
#include "sdm35541_util_library.h"
#include "sdm35541_adc1002_library.h"
#include "sdm35541_dma_library.h"
#include "sdm35541_examples.h"
#include "SDM35541.h"
#include "AdcBurst.h"


/**
 * Main program code.
 */
int main(int argument_count, char **arguments){
	struct SDM35541_Board_Descriptor *board;
	struct SDM35541_Function_Block my_function_block;
	int result, i;
	float actualRate;
	uint16_t data;
	int minor_number;

	printf("\n\tSDM35541 A/D Multi-Channel Burst Example Program\n\n");

    /* Get board number */
    printf("Enter board number to use: ");
    if (scanf_s("%i", &minor_number) == 0) {
        check_result(-1, "Error getting board number\n");
    }

    check_result(
        minor_number < 0 || minor_number >= SDM35541_MAX_BOARDS,
        "Board number must be between 0-7."
    );

    /*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Device initialization
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Open the sdm35541 device. */
	printf("Opening SDM35541 with minor number %u...\n",
		minor_number);
	result = SDM35541_Board_Open(minor_number, &board);
	check_result(result, "Error opening the board.");
	
	/* Reset the board. */
	printf("Resetting the SDM35541...\n");
	 result = SDM35541_Gbc_Board_Reset(board);
	 check_result(result, "Failed to reset the board.");

	/* Initialize the board. */
	printf("Board Initialization...\n");
	result = SDM35541_Function_Block_Open(board, SDM35541_FUNCTION_BLOCK_ADC1002, &my_function_block);
	check_result(result, "Error opening the ADC1002 function block.");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Main program code.
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	   
	/* Clear ADC FIFO */
	printf("Clearing ADC FIFO...\n");
	result = SDM35541_ADC_Clear(board, &my_function_block);
	check_result(result, "Error clearing the ADC FIFO.");

	/* Clear CGT FIFO */
	result = SDM35541_CGT_Clear(board, &my_function_block);
	check_result(result, "Error clearing Channel Gain Table FIFO.");

	/* Check FIFO Status */
	result = SDM35541_FIFO_Get_Status(board, &my_function_block, &data);
	check_result(result, "Error getting FIFO status.");
	printf("FIFO Status: 0x%4x\n", data);

	/* Enable Channel Gain Table */
	printf("Enabling Channel Gain Table...\n");
	result = SDM35541_CGT_Enable(board, &my_function_block, 0xFF);
	check_result(result, "Error enabling the Channel Gain Table.");

	/* Create CGT Entry */
	printf("Writing Channel Gain Table...\n");
	for (i = 0; i < NUM_CHANNELS; i++) {
		SDM35541_cgt_entry_t cgt = { 0 };
		cgt.channel = i;

		/* Write the entry to the CGT */
		result = SDM35541_CGT_Write(board, &my_function_block, cgt);
		check_result(result, "Error writing Channel Gain Table entry to FIFO.");
	}

	printf("Setting up Burst Clock...\n");
	result = SDM35541_BCLK_Setup(board, &my_function_block,
					  SDM35541_BCLK_START_PACER,
					  SDM35541_BCLK_FREQ_8_MHZ,
					  BURST_RATE, &actualRate);
	check_result(result, "Error setting up burst clock.");

	/* Setup pacer clock */
	printf("Setting up Pacer Clock...\n");
	result = SDM35541_PCLK_Setup(board, &my_function_block,
					  SDM35541_PCLK_INTERNAL,
					  SDM35541_PCLK_FREQ_8_MHZ,
					  SDM35541_PCLK_NO_REPEAT,
					  SDM35541_PCLK_START_SOFTWARE,
					  SDM35541_PCLK_STOP_SOFTWARE,
					  PACER_RATE, &actualRate);
	check_result(result, "Error setting up pacer clock.");

	/* Set ADC Conversion Signal Select */
	result =
	    SDM35541_ADC_Conv_Signal(board, &my_function_block, SDM35541_ADC_CONV_SIGNAL_BCLK);
	check_result(result, "Error setting up ADC Conversion Signal");
	result = SDM35541_FIFO_Get_Status(board, &my_function_block, &data);
	printf("FIFO STATUS: %02x\n", data);

	/* Start the pacer clock */
	printf("Starting Pacer Clock...\n");
	result = SDM35541_PCLK_Start(board, &my_function_block);
	check_result(result, "Error starting the pacer clock.");

	printf("Filling FIFO...\n");

	do {
		/* Check FIFO Status */
		result = SDM35541_FIFO_Get_Status(board, &my_function_block, &data);
		check_result(result,
				     "Error getting FIFO status.");
	} while (data & SDM35541_FIFO_ADC_NOT_FULL);

	/* Stop the pacer clock */
	printf("Stopping Pacer Clock...\n");
	result = SDM35541_PCLK_Stop(board, &my_function_block);
	check_result(result, "Error stopping the pacer clock.");

	/* Read out samples */
	printf("Reading Samples...\n");
	i = 0;
	do {
		/* Read AD FIFO */
		result = SDM35541_ADC_FIFO_Read(board, &my_function_block, &data);
		check_result(result,
				     "Error reading the ADC FIFO.");
		printf("CH%2u: %2.2f\n", (i % NUM_CHANNELS) + 1,
			((SDM35541_ADC_ANALOG_DATA(data) / 4096.) * 10));

		/* Increment sample counter. */
		i++;

		/* Read the FIFO Status */
		result = SDM35541_FIFO_Get_Status(board, &my_function_block, &data);
		check_result(result,
				     "Error reading the ADC FIFO.");
	} while (data & SDM35541_FIFO_ADC_NOT_EMPTY);

	/* Pring how many samples were received */
	printf("Received %d samples...\n", i);
	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Program clean up
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	/* Reset the board. */
	printf("Resetting the sdm35541...\n");
	result = SDM35541_Gbc_Board_Reset(board);
	check_result(result, "Error resetting the board.");

	/* Close the sdm35541 device. */
	printf("Closing the sdm35541...\n");
	result = SDM35541_Board_Close(board);
	check_result(result, "Error closing the board.");

	system("pause");
	
	return 0;
}
