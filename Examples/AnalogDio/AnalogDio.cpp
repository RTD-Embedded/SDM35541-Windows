/**
	@file

	@brief
		Demonstrates the use of Analog DIO Connector on the SDM35541

	@verbatim

	This example shows use of the Analog Connector DIO.  This consists of pins 1
	and 2 on CN9 of the SDM35541.  In this particular example, pin 1 is set as
	an output and pin 2 is set as an input.  Pin 2 rising edge interrupt is
	enabled.  Pulses are sent out pin 1 until pin 2 receives 2 interrupts.  Pin
	1 should be connect to pin 2.

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

		$Id: sdm35541_analog_dio.c 150884 2025-11-19 21:16:23Z bkorpacz $
*/

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sdm35541_gbc_library.h"
#include "sdm35541_util_library.h"
#include "sdm35541_registers.h"
#include "sdm35541_adc1002_library.h"
#include "sdm35541_os.h"
#include "SDM35541.h"

#include "AnalogDio.h"


/**
 * Global used to keep track of the number of interrupts received
 */
static volatile int interrupts;

/**
 * SDM35541 Board descriptor
 */
struct SDM35541_Board_Descriptor* board;

/**
 * ADC1002 Function Block
 */
struct SDM35541_Function_Block my_function_block;

/**
 * Analog DIO interrupts
 */
uint8_t algdio_ints;

/**
 * Whether callback function has been called.
 */
bool has_interrupt_occurred = false;


/**
 @brief Interrupt callback
 **/
static void MyCallback(SDM35541_Interrupt_Callback_Info int_info) {
	SDM35541_int_source_t status;
	int result;

	/*
	 * Get ALGDIO interrupts
	 * Reading this clears the ALGDIO interrupt
	 */
	result = SDM35541_ALGDIO_Get_IRQ_Status(board, &my_function_block, &algdio_ints);
	if (result) {
		return;
	}

	/*
	 * Shift algdio ints to conform to sdm35541_int_source_t
	 */
	status = (SDM35541_int_source_t) ((algdio_ints & 0x3c) << 24);

	if (status & SDM35541_INT_ALGDIO_POS_PIN2) {
		interrupts++;
	}

}


/**
 * Main program code.
 */
int main(int argc, char** argv)
{
	int minor_number;
	int result;
	int i;

	printf("\n\tSDM35541 Analog Connector DIO Example Program\n\n");

	/* Get board number */
	printf("Enter board number to use: ");
	if (scanf_s("%i", &minor_number) == 0) {
		check_result(-1, "Error getting board number\n");
	}

	check_result(minor_number < 0 || minor_number >= SDM35541_MAX_BOARDS, "Board number must be between 0-7.");

	/* Open the sdm35541 device. */
	printf("Opening sdm35541 with minor number %u...\n",
		minor_number);
	result = SDM35541_Board_Open(minor_number, &board);
	check_result(result, "Error opening board.");
	
	/* Reset the board. */
	printf("Resetting the SDM35541...\n");
	result = SDM35541_Gbc_Board_Reset(board);
	check_result(result, "Failed to reset the board.");

	/* Open ADC1002 function block */
	printf("Opening sdm35541 function block...\n");
	result = SDM35541_Function_Block_Open(board, SDM35541_FUNCTION_BLOCK_ADC1002, &my_function_block);
	check_result(result, "Failed to open ADC1002 function block.");

	/* Enabling Pin 2 Positive Edge Interrupt */
	printf("Enabling Pin 2 Positive Edge Interrupt...\n");
	result =
		SDM35541_Interrupt_Enable(board, &my_function_block, SDM35541_INT_ALGDIO_POS_PIN2);
	check_result(result, "Error enabling ALGDIO interrupt.");

	/* Install user-space callback */
	printf("Installing interrupt callback...\n");
	result = SDM35541_Interrupt_Install_Callback(board, &my_function_block, MyCallback);
	check_result(result, "Error installing interrupt callback.");

	/* Setting Analog DIO Direction */
	result = SDM35541_ALGDIO_Set_Direction(board, &my_function_block, SDM35541_ALGDIO_OUTPUT,	//pin 1
		SDM35541_ALGDIO_INPUT);	//pin 2
	check_result(result,
		"Error setting ALGDIO Direction.");

	/* Setting Analog DIO Mask */
	result = SDM35541_ALGDIO_Set_Mask(board, &my_function_block,
		SDM35541_ALGDIO_UNMASKED,
		SDM35541_ALGDIO_MASKED);
	check_result(result, "Error setting ALGDIO mask.");

	printf("Writing Values to Pin 1...\n");
	i = 0;

	while (i < 10) {
		printf("\tPin1 Logic High!\n");
		result = SDM35541_ALGDIO_Set_Data(board, &my_function_block, 0xFF,	//pin 1
			0x00);	//pin 2
		check_result(result,
			"Error setting ALGDIO Pin 1 high.");

		/* Sleep for 0.5sec */
		SDM35541_Micro_Sleep(500000);
		printf("\tPin1 Logic Low!\n");
		result = SDM35541_ALGDIO_Set_Data(board, &my_function_block, 0x00,	//pin 1
			0x00);	//pin 2
		check_result(result,
			"Error setting ALGDIO Pin 1 low.");

		/* Sleep for 0.5sec */
		SDM35541_Micro_Sleep(500000);
		i++;
	}

	/*
	 * Print number of received interrupts
	 */
	printf("Received %u positive edge interrupts\n", interrupts);

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Program clean up
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	/* Remove User-Space  */
	printf("Removing User-Space ISR...\n");
	result = SDM35541_Interrupt_Remove_Callback(board, &my_function_block);
	check_result(result, "Error removing ISR.");

	/* Reset the board. */
	printf("Board Reset...\n");
	result = SDM35541_Gbc_Board_Reset(board);
	check_result(result, "Error resetting board.");

	/*
	 * Close the sdm35541 device.
	 */
	printf("Closing the sdm35541...\n");
	result = SDM35541_Board_Close(board);
	check_result(result, "Error closing the board.");

	system("pause");

	return 0;
}
