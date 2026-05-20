/**
	@file

	@brief
		Demonstrates auto-calibration of SDM35541

	@verbatim

		This program utilizes the on-board DSP to auto calibrate the A/D and D/A
		converters.

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

		$Id: Calibrate.cpp 152236 2026-02-16 19:22:03Z bkorpacz $
*/

#include <limits.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sdm35541_os.h"
#include "sdm35541_gbc_library.h"
#include "sdm35541_util_library.h"
#include "sdm35541_registers.h"
#include "sdm35541_adc1002_library.h"

#include "SDM35541.h"


/**
 * Main program code.
 */
int main(int argument_count, char** arguments)
{
	struct SDM35541_Board_Descriptor* board;
	struct SDM35541_Function_Block my_func_block;
	int result;
	uint8_t data;

	unsigned int minor_number = 0;
	unsigned int dac1_range = 0;
	unsigned int dac2_range = 0;
	int dac1_val = 0;
	int dac2_val = 0;

	printf("\n\tSDM35541 AutoCal Example Program\n\n");

	/* Get board number */
	printf("Enter board number to use: ");
	if (scanf_s("%i", &minor_number) == 0) {
		check_result(-1, "Error getting board number\n");
	}

	check_result(
		minor_number < 0 || minor_number >= SDM35541_MAX_BOARDS,
		"Board number must be between 0-7.");

	/* Get DAC1Range */
	printf("\nDAC1Range:    0 = 0 - 5V\n"
		   "              1 = 0 - 10V\n"
		   "              2 = +/- 5V\n"
		   "              3 = +/- 10V\n");
	printf("Enter range at which to calibrate DAC1: ");

	if (scanf_s("%i", &dac1_range) == 0) {
		check_result(-1, "Error getting dac1_range");
	}

	check_result(
		dac1_range < 0 || dac1_range > 3,
		"Error: Invalid value for '--dac1range'");

	/* Get DAC1VAL */
	printf("\nDAC1VAL:      -2048 to 2047 bipolar, 0 to 4096 unipolar.\n");
	printf("Enter value to set DAC1 after calibration: ");

	if (scanf_s("%i", &dac1_val) == 0) {
		check_result(-1, "Error getting dac1_val");
	}

	/* dac1val input validation */
	if (dac1_range == 0 || dac1_range == 1) {
		check_result(
			dac1_val > 4096 || dac1_val < 0,
			"Error: Invalid value for 'dac1val' unipolar.");
	} else {
		check_result(
			dac1_val > 2047 || dac1_val < -2048,
			"Error: Invalid value for 'dac1val' bipolar.");
	}
	
	/* Get DAC2Range */
	printf("\nDAC2Range:    0 = 0 - 5V\n"
		   "              1 = 0 - 10V\n"
		   "              2 = +/- 5V\n"
		   "              3 = +/- 10V\n");
	printf("Enter range at which to calibrate DAC2: \n");

	if (scanf_s("%i", &dac2_range) == 0) {
		check_result(-1, "Error getting dac2_range");
	}

	/* dac2range input validation */
	check_result(
		dac2_range < 0 || dac2_range > 3,
		"Error: Invalid value for '--dac2range'");

	/* Get DAC2VAL */
	printf("\nDAC2VAL:      -2048 to 2047 bipolar, 0 to 4096 unipolar.\n");
	printf("Enter value to set DAC2 after calibration: \n");

	if (scanf_s("%i", &dac2_val) == 0) {
		check_result(-1, "Error getting dac2_val");
	}

	/* dac2val input validation */
	if (dac2_range == 0 || dac2_range == 1) {
		check_result(
			dac2_val > 4096 || dac2_val < 0,
			"Error: Invalid value for 'dac2val' unipolar.");
	} else {
		check_result(
			dac2_val > 2047 || dac2_val < -2048,
			"Error: Invalid value for 'dac2val' bipolar.");
	}

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Device initialization
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/*  Open the sdm35541 device. */
	printf("Opening SDM35541 with minor number %lu ...\n",
		minor_number);
	result = SDM35541_Board_Open(minor_number, &board);
	check_result(result, "Error opening board.");

	/* Reset the board. */
	printf("Resetting the SDM35541...\n");
	result = SDM35541_Gbc_Board_Reset(board);
	check_result(result, "Failed to reset the board.");

	/* Open the SDM35541 function block */
	printf("Opening SDM35541 function block 0\n");
	result = SDM35541_Function_Block_Open(board,
		SDM35541_FUNCTION_BLOCK_ADC1002, &my_func_block);
	check_result(result, "Error opening ADC1002 function block.");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Main program code.
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Calibrate the board */
	printf("Calibrating the board ... \n");
	result = SDM35541_Calibrate(board, &my_func_block,
		dac1_val,
		dac2_val, (SDM35541_dac_range_t) dac1_range, (SDM35541_dac_range_t) dac2_range);
	check_result(result, "Error running calibration.");

	/* Wait for the command to finish */
	printf("Waiting for Calibration Algorithm to complete\n");
	do {
		result = SDM35541_DSP_CMD_Complete(board, &my_func_block, &data);
		check_result(result,
			"Error waiting for DSP command to complete.");
	} while (data & 0xFF);

	/* Checking result of DSP command */
	printf("Reading DSP Command Status\n");
	result = SDM35541_DSP_CMD_Status(board, &my_func_block, SDM35541_DSP_CAL_AUTO);
	check_result(result, "Error checking DSP command status.");

	/*
	 * If SDM35541_DSP_CMD_Status return a 0 then the command we requested was
	 * successful.
	 */
	printf("Calibration complete!\n");

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
