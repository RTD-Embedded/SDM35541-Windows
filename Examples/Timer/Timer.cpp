/**
    @file

    @brief
        Demonstrates the use of the User Timer/Counters for keeping time.

    @verbatim

    This example program demonstrates using the User Timer/Counters as a
    simple time keeper.

    UTC 1 is being fed by UTC 0.

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

        $Id: Timer.cpp 152256 2026-02-16 21:11:14Z bkorpacz $
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

#include "Timer.h"

/**
 * Variable to allow graceful exit from Ctrl-C
 */
volatile bool exit_program = false;


/**
 * Main program code.
 */
int main(int argument_count, char **arguments)
{
	struct SDM35541_Board_Descriptor *board;
	struct SDM35541_Function_Block my_function_block;
	uint16_t count;
	int result;
	int minor_number;

	printf("\n\tSDM35541 User Timer/Counter Example Program\n\n");

	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlCHandler, true);

	/* Get board number */
	printf("Enter board number to use: ");
	if (scanf_s("%i", &minor_number) == 0) {
		check_result(-1, "Error getting board number\n");
	}

	check_result(
		minor_number < 0 || minor_number >= SDM35541_MAX_BOARDS,
		"Board number must be between 0-7."
	);

    /*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Device initialization
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Open the sdm35541 device. */
	printf("Opening sdm35541 with minor number %lu...\n",
		minor_number);
	result = SDM35541_Board_Open(minor_number, &board);
	check_result(result, "Error opening the board.");
	
	/* Reset the board. */
	printf("Resetting the SDM35541...\n");
	result = SDM35541_Gbc_Board_Reset(board);
	check_result(result, "Failed to reset the board.");

	/* Open ADC1002 Function Block */
	printf("Board Initialization...\n");
	result = SDM35541_Function_Block_Open(board, SDM35541_FUNCTION_BLOCK_ADC1002, &my_function_block);
	check_result(result, "Error opening the ADC1002 function block.");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Setup User Timer/Counters
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Setup User Timer/Counter 0 */
	printf("User Timer/Counter 0 Setup...\n");
	result = SDM35541_UTC_Setup(board, &my_function_block,
					 SDM35541_UTC_0,
					 SDM35541_CUTC_8_MHZ,
					 SDM35541_GUTC_NOT_GATED,
					 SDM35541_UTC_MODE_SQUARE_WAVE,
					 (uint16_t) (8000000 / TIMER_RATE));
	check_result(result, "Error setting up UTC 0.");

	/* Setup User Timer/Counter 1 */
	printf("User Timer/Counter 1 Setup...\n");
	result = SDM35541_UTC_Setup(board, &my_function_block,
					 SDM35541_UTC_1,
					 SDM35541_CUTC_UTC_0_OUT,
					 SDM35541_GUTC_NOT_GATED,
					 SDM35541_UTC_MODE_EVENT_COUNTER, 0xFFFF);
	check_result(result, "Error setting up UTC 1.");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Show Elapsed Time
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	/* Read the Clock */
	printf("\nPress Control-C to exit.\n");
	printf("\nElapsed time (in seconds)...\n");

	do {
		result = SDM35541_UTC_Get_Count(board, &my_function_block,
						     SDM35541_UTC_1, &count);
		check_result(result,
				     "Error getting UTC 1 count.");

		printf("     %2.2f   \r",
			((float)(0xFFFF - count) / TIMER_RATE));

	} while (!exit_program);
	printf("\n\n");

	/* Set User Timer/Counter 0 gate on to stop the clock */
	printf("Stopping User Timer/Counter 0...\n");
	result =
	    SDM35541_UTC_Set_Gate(board, &my_function_block, SDM35541_UTC_0, SDM35541_GUTC_GATED);
	check_result(result, "Error stopping UTC 0.");
	
	/* Set User Timer/Counter 1 gate on to stop the clock */
	printf("Stopping User Timer/Counter 1...\n");
	result =
	    SDM35541_UTC_Set_Gate(board, &my_function_block, SDM35541_UTC_1, SDM35541_GUTC_GATED);
	check_result(result, "Error stopping UTC 1.");

	/* Reset the board. */
	printf("Board Reset...\n");
	result = SDM35541_Gbc_Board_Reset(board);
	check_result(result, "Error resetting the board.");
	
	/* Close the sdm35541 device. */
	printf("Closing the sdm35541...\n");
	result = SDM35541_Board_Close(board);
	check_result(result, "Error resetting the board.");

	system("pause");

	return 0;
}


/**
 * Handler for CTRL - C events.
 * Cleanup and exit.
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
