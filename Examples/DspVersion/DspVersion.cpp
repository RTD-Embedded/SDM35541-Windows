/**
    @file

    @brief
        Demonstrates the use of the SDM35541's DSP functions

    @verbatim

    Queries the onboard DSP bootloader version

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

		$Id: DspVersion.cpp 152297 2026-02-18 14:52:52Z bkorpacz $
*/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>

#include "sdm35541_gbc_library.h"
#include "sdm35541_examples.h"
#include "sdm35541_util_library.h"
#include "sdm35541_registers.h"
#include "sdm35541_adc1002_library.h"
#include "SDM35541.h"

/**
 * Main program code.
 */
int main(int argument_count, char **arguments)
{
	struct SDM35541_Board_Descriptor *board;
    struct SDM35541_Function_Block my_func_block;
	uint32_t version;
	int minor_number;
	int result;

	printf("\n\tSDM35541 DSP Version Example Program\n\n");
	
	/* Get board number */
	printf("Enter board number to use: ");
	if (scanf_s("%i", &minor_number) == 0) {
		check_result(-1, "Error getting board number\n");
	}
	check_result(
		minor_number < 0 || minor_number >= SDM35541_MAX_BOARDS,
		"Board number must be between 0-7.");

    /*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Device initialization
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Open the SDM35541 device. */
	printf("Opening SDM35541 with minor number %u...\n",
		minor_number);
	result = SDM35541_Board_Open(minor_number, &board);
	check_result(result, "Error opening the board.");

	/* Reset the board. */
	printf("Resetting the SDM35541...\n");
	result = SDM35541_Gbc_Board_Reset(board);
	check_result(result, "Failed to reset the board.");

    /* Open the SDM35541 function block */
    printf("Opening SDM35541 function block 0...\n");
    result = SDM35541_Function_Block_Open(board,
            SDM35541_FUNCTION_BLOCK_ADC1002, &my_func_block);
    check_result(result, "Error opening the ADC1002 function block.");

	/* Wait for DSP Reset */
	SDM35541_Micro_Sleep(10000);

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Main program code.
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Get the Bootloader version */
	printf("Getting Bootloader version...\n");
	result = SDM35541_DSP_Get_Version(board, &my_func_block, &version);
	check_result(result, "Error getting DSP Version.");

	printf("\nDSP Version: %d.%d\n\n", version >> 16, version & 0xffff);
	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Program clean up
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	/* Reset the sdm35541 device. */
	printf("Resetting the SDM35541...\n");
	result = SDM35541_Gbc_Board_Reset(board);
	check_result(result, "Error resetting the board.");

	/* Close the sdm35541 device. */
	printf("Closing the SDM35541...\n");
	result = SDM35541_Board_Close(board);
	check_result(result, "Error closing the board.");
	
    system("pause");
    
	return 0;
}
