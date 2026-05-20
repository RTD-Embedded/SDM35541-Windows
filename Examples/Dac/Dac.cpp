/**
    @file

    @brief
        Demonstrates the use of Digital to Analog conversion.

    @verbatim

    This example program produces a Saw-Toothed wave on a specified DAC channel
	using software     trigger mode. The program will continue to run until
	Ctrl+C is pressed.

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

        $Id: Dac.cpp 152256 2026-02-16 21:11:14Z bkorpacz $
*/

#include <fcntl.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sdm35541_gbc_library.h"
#include "sdm35541_util_library.h"
#include "sdm35541_registers.h"
#include "sdm35541_adc1002_library.h"
#include "SDM35541.h"

#include "Dac.h"


/**
 * Whether to exit the program.
 */
volatile bool exit_program = false;


/**
 * Main program code.
 */
int main(int argc, char** argv) {
	struct SDM35541_Board_Descriptor* board;
	struct SDM35541_Function_Block func_block;
	SDM35541_dac_channel_t dac_channel;
	int minor_number;
	int dac_number;
	int result, i;
	
    /* Install Ctrl+C handler */
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlCHandler, true);

	printf("\n\tSDM35541 Digital to Analog Example Program\n\n");

	/* Get board number */
	printf("Enter board number to use: ");
	if (scanf_s("%i", &minor_number) == 0) {
		check_result(-1, "Error getting board number\n");
	}

	check_result(minor_number < 0 || minor_number >= SDM35541_MAX_BOARDS, "Board number must be between 0-7.");

	/* Get DAC channel number */
	printf("Enter DAC number (1-2): ");
	if (scanf_s("%i", &dac_number) == 0) {
		check_result(-1, "Error getting DAC number\n");
	}

	check_result(dac_number < 1 || dac_number > 2, "DAC channel must be 1 or 2.");

	if (dac_number == 1) {
		dac_channel = SDM35541_DAC1;
	}
	else {
		dac_channel = SDM35541_DAC2;
	}
	
	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Device initialization
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Open the sdm35541 device. */
	printf("Opening sdm35541 with minor number %u...\n",
		minor_number);
	result = SDM35541_Board_Open(minor_number, &board);
	check_result(result, "Error opening the board.");
	
	/* Reset the board. */
	printf("Resetting the SDM35541...\n");
	result = SDM35541_Gbc_Board_Reset(board);
	check_result(result, "Failed to reset the board.");

	/* Initialize the board. */
	printf("Board Initialization...\n");
	result = SDM35541_Function_Block_Open(board, SDM35541_FUNCTION_BLOCK_ADC1002, &func_block);
	check_result(result, "Error opening the ADC1002 function block.");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Main program code.
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Setup DAC */
	printf("Setup DAC%i...\n", dac_number);
	result =
		SDM35541_DAC_Setup(board, &func_block, dac_channel, SDM35541_DAC_RANGE_BIPOLAR_5,
			SDM35541_DAC_UPDATE_SOFTWARE, SDM35541_DAC_MODE_NOT_CYCLE);
	check_result(result, "Error setting up DAC.");

	/* Clear DAC FIFO */
	SDM35541_DAC_Clear(board, &func_block, dac_channel);

	/* Set User Output Signal 0 to DAC1 Conversion */
	printf("Setting User Output Signal 0...\n");
	result = SDM35541_UIO_Select(board, &func_block, SDM35541_UIO0, SDM35541_UIO_DAC1);
	check_result(result, "Error setting user output signal 0.");

	printf("Outputting Sawtooth on DAC Channel %i\n", dac_number);
	printf("Press Ctrl+C to exit.\n");

	/* Programmatically write a sawtooth wave to the DAC FIFO. */
	i = -5000;
	do {
		uint16_t data = (uint16_t)((i / 1000.0) * 4095.0);

		result = SDM35541_DAC_FIFO_Write(board, &func_block, dac_channel, data << 3);
		check_result(result, "Error writing to DAC FIFO.");

		result = SDM35541_DAC_Soft_Update(board, &func_block, dac_channel);
		check_result(result, "Error performing DAC software update.");

		i += 10;

		if (i > 5000) {
			i = -5000;
		}
	} while (!exit_program);

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Program clean up
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Reset the sdm35541 device. */
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
