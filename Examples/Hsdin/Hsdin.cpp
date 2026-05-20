/**
    @file

    @brief
        Demonstrates the use of high speed digital data acquisition.

    @verbatim

    This example program simply gathers high speed digital data and displays
    it to the screen when the FIFO is filled.

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

        $Id: Hsdin.cpp 152193 2026-02-13 21:38:28Z bkorpacz $
*/

#include <limits.h>
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
#include "SDM35541.h"

#include "Hsdin.h"

/**
********************************************************************************
@brief
    Main program code.
********************************************************************************
*/
int main(int argument_count, char **arguments)
{
	struct SDM35541_Board_Descriptor *board;
	struct SDM35541_Function_Block my_function_block;
	uint16_t data;
	int minor_number = 0;
	int result;
	int i;

	printf("\n\tSDM35541 High Speed Digital Example Program\n\n");

	/* Get board number */
	printf("Enter board number to use: ");
	if (scanf_s("%i", &minor_number) == 0) {
		check_result(-1, "Error getting board number\n");
	}

	check_result(
		minor_number < 0 || minor_number >= SDM35541_MAX_BOARDS,
		"Board number must be between 0-7."
	);

	/* Open the sdm35541 device. */
	printf("Opening sdm35541 with minor number %lu...\n",
		minor_number);
	result = SDM35541_Board_Open(minor_number, &board);
	check_result(result, "Error opening the board.");

	/* Reset the board. */
	printf("Resetting the SDM35541...\n");
	result = SDM35541_Gbc_Board_Reset(board);
	check_result(result, "Failed to reset the board.");

    /* Open ADC1002 function block */
    printf("Opening sdm35541 function block...\n");
    result = SDM35541_Function_Block_Open(board, SDM35541_FUNCTION_BLOCK_ADC1002, &my_function_block);
    check_result(result, "Failed to open ADC1002 function block.");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Main program code.
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	/* Clear HSDIN FIFO */
	printf("Clearing HSDIN FIFO...\n");
	result = SDM35541_HSDIN_Clear(board, &my_function_block);
	check_result(result, "Error clearing the HSDIN FIFO.");

	/* Check FIFO Status */
	result = SDM35541_FIFO_Get_Status(board, &my_function_block, &data);
	check_result(result, "Error getting FIFO status.");
	printf("FIFO Status: 0x%4x\n", data);

	/* Setup User Timer/Counter 0 */
	printf("User Timer/Counter 0 Setup...\n");
	result = SDM35541_UTC_Setup(board, &my_function_block,
					 SDM35541_UTC_0,
					 SDM35541_CUTC_8_MHZ,
					 SDM35541_GUTC_NOT_GATED,
					 SDM35541_UTC_MODE_RATE_GENERATOR,
					 (uint16_t) (8000000 / TIMER_RATE));
	check_result(result, "SDM35541_UTC_Setup");

	/* Set High Speed Digital Sample Signal to trigger on User Timer/Counter 0 Out */
	printf("Setting High Speed Digital Sample Signal...\n");
	result =
	    SDM35541_HSDIN_Sample_Signal(board,
					 &my_function_block,
					 SDM35541_HSDIN_SIGNAL_UTC0);
	check_result(result,
			     "Error setting HSDIN sample trigger mode.");
				 
	/* Wait for HD FIFO Full */
	printf("Filling FIFO...\n");

	do {
		/* Check FIFO Status */
		result = SDM35541_FIFO_Get_Status(board, &my_function_block, &data);
		check_result(result,
				     "Error getting FIFO status.");
	} while (data & SDM35541_FIFO_HSDIN_NOT_FULL);

	/* Set User Timer/Counter 0 gate on to stop the clock */
	printf("Stopping User Timer/Counter 0...\n");
	result =
	    SDM35541_UTC_Set_Gate(board,
					&my_function_block,
					SDM35541_UTC_0,
					SDM35541_GUTC_GATED);
	check_result(result, "Error enabling UTC 0.");

	/* Read out samples */
	printf("Reading Samples...\n\n");
	i = 1;
	do {
		/* Read HD FIFO */
		result = SDM35541_HSDIN_FIFO_Read(board, &my_function_block, &data);
		check_result(result,
				     "Error reading HSDIN FIFO.");

		printf("0x%2x ", data & 0xff);
		if ((i % 16) == 0) {
			printf("\n");
		}

		/* Increment sample counter. */
		i++;

		/* Read the FIFO Status */
		result = SDM35541_FIFO_Get_Status(board, &my_function_block, &data);
		check_result(result,
				     "Error getting FIFO status.");
	} while (data & SDM35541_FIFO_HSDIN_NOT_EMPTY);

	/* Pring how many samples were received */
	printf("\nReceived %d samples...\n", i - 1);
	
	/* Reset the board. */
	printf("Board Reset...\n");
	result = SDM35541_Gbc_Board_Reset(board);
	check_result(result, "Error resetting the board.");

	/* Close the sdm35541 device. */
	printf("Closing the sdm35541...\n");
	result = SDM35541_Board_Close(board);
	check_result(result, "Error closing the board.");

	system("pause");

	return 0;
}
