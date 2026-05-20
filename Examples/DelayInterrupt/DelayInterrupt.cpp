/**
    @file

    @brief
        This example program demonstrates the use of the Delay Counter Interrupt

    @verbatim

    Samples are gathered via the Pacer Clock which is triggered by Delay
    Software Start.  It will wait to collect samples until 10 samples have been
    read then it will collect 10 samples.  This process will be repeated via use
    of the delay and about counters.  An interrupt will occur each time the
    delay counter hits 0 delay counter.

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

		$Id: DelayInterrupt.cpp 152193 2026-02-13 21:38:28Z bkorpacz $
*/

#include <errno.h>
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

#include "DelayInterrupt.h"

/**
 * Whether an interrupt has occurred.
 */
volatile bool has_interrupt_occurred = false;

/**
 * Variable to allow graceful exit from Ctrl-C.
 */
volatile bool exit_program = false;

/**
 * Status of the interrupt callback function.
 */
volatile bool callback_result;

/**
 * SDM35541 Board descriptor
 */
struct SDM35541_Board_Descriptor* board;

/**
 * ADC1002 Function Block
 */
struct SDM35541_Function_Block my_function_block;

/**
 * Critical Section variable
 */
CRITICAL_SECTION critical_section;


/**
 @brief User-Space ISR
 **/
void MyCallback(SDM35541_Interrupt_Callback_Info int_info) {
	EnterCriticalSection(&critical_section);
	callback_result = int_info.result;
	has_interrupt_occurred = true;
	LeaveCriticalSection(&critical_section);
}


/**
 * Main program code.
 */
int main(int argument_count, char **arguments) {
	SDM35541_cgt_entry_t cgt = { 0 };
	float actualRate;
    int interrupts = 0;
	int minor_number;
	int result;
    
	/* Initialize critical section */
	InitializeCriticalSection(&critical_section);

	/* Install Ctrl+C handler */
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlCHandler, true);

	printf("\n\tSDM35541 Delay Counter Interrupt Example Program\n\n");

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
	printf("Opening sdm35541 with minor number %lu...\n",
		minor_number);
	result = SDM35541_Board_Open(minor_number, &board);
	Fail(result, "SDM35541_Board_Open");

    /* Open ADC1002 function block */
    printf("Opening sdm35541 function block...\n");
    result = SDM35541_Function_Block_Open(board, SDM35541_FUNCTION_BLOCK_ADC1002, &my_function_block);
    Fail(result, "Failed to open ADC1002 function block.");

	/* Installing User-Space ISR */
	printf("Installing User-Space ISR...\n");
	result = SDM35541_Interrupt_Install_Callback(
		board,
		&my_function_block,
		MyCallback);

	/* Clear ADC FIFO */
	printf("Clearing ADC FIFO...\n");
	result = SDM35541_ADC_Clear(board, &my_function_block);
	Fail(result, "Error clearing ADC FIFO.");

	/* Enable Channel Gain Table */
	printf("Enabling Channel Gain Latch...\n");
	result = SDM35541_CGT_Enable(board, &my_function_block, 0x00);
	Fail(result, "Error enabling Channel Gain Table.");

	/* Write the entry to the CGT Latch register (used for single channel) */
	printf("Writing Channel Gain Table entry...\n");
	result = SDM35541_CGT_Latch(board, &my_function_block, cgt);
	Fail(result, "Error latching entry into the Channel Gain Table.");

	/* Setup pacer clock */
	printf("Setting up Pacer Clock...\n");
	result = SDM35541_PCLK_Setup(board, &my_function_block,
					  SDM35541_PCLK_INTERNAL,
					  SDM35541_PCLK_FREQ_8_MHZ,
					  SDM35541_PCLK_REPEAT,
					  SDM35541_PCLK_START_DSOFTWARE,
					  SDM35541_PCLK_STOP_ACNT,
					  ADC_RATE, &actualRate);
	Fail(result, "Error setting up the pacer clock.");

	/* Set ADC Conversion Signal Select */
	printf("Setting A/D Conversion Sample Signal.. \n");
	result =
	    SDM35541_ADC_Conv_Signal(board, &my_function_block, SDM35541_ADC_CONV_SIGNAL_PCLK);
	Fail(result, "Error setting up pacer clock.");

	/* Set User Output Signal 0 to A/D Conversion */
	printf("Setting User Output Signal 0...\n");
	result = SDM35541_UIO_Select(board, &my_function_block, SDM35541_UIO0, SDM35541_UIO_ADC);
	Fail(result, "Error setting user output signal.");

	/* Enabling About Counter */
	printf("Enabling About Counter...\n");
	result = SDM35541_ADC_About_Enable(board, &my_function_block, 0xFF);
	Fail(result, "Error enabling about counter.");

	/* Load About Counter */
	printf("Loading About Counter...\n");
	result = SDM35541_ACNT_Set_Count(board, &my_function_block, 10 - 1);
	Fail(result, "Error setting about counter.");

	/* Load Delay Counter */
	printf("Loading Delay Counter...\n");
	result = SDM35541_DCNT_Set_Count(board, &my_function_block, 10 - 1);
	Fail(result, "Error setting delay counter.");

	/* Enable Delay Counter Interrupt */
	printf("Enabling Delay Counter Interrupt...\n");
	result = SDM35541_Interrupt_Enable(board, &my_function_block, SDM35541_INT_DELAY);
	Fail(result, "Error enabling delay interrupt.");

	/* Start the pacer clock */
	printf("Starting Pacer Clock...\n");
	result = SDM35541_PCLK_Start(board, &my_function_block);
	Fail(result, "Error starting the pacer clock.");
	printf("Waiting for Interrupts...\n");

    while (interrupts < 10) {
		uint16_t status;
		bool has_int;

		EnterCriticalSection(&critical_section);
		has_int = has_interrupt_occurred;
		LeaveCriticalSection(&critical_section);
		
        result = SDM35541_PCLK_Start(board, &my_function_block);
        Fail(result,
            "Error starting pacer clock.");

		if (!has_int) {
            SDM35541_Micro_Sleep(100);
			continue;
		}

		EnterCriticalSection(&critical_section);
		/* Allow another interrupt to occur. */
		has_interrupt_occurred = false;

		/* Check if callback error had occurred */
		Fail(callback_result, "Error in callback function.");
		LeaveCriticalSection(&critical_section);

		/* Get board interrupts. */
		result = SDM35541_Get_IT_Mask(board, &my_function_block, &status);
		Fail(result, "Error getting IT register.");
        
        /* See if external trigger has occurred. */
        if (status & SDM35541_INT_DELAY) {
            interrupts++;
        }

		/* Clear IT register */
		result = SDM35541_Clear_IT_Mask(board, &my_function_block, (SDM35541_int_source_t) 0xffff);
		Fail(result, "Error clearing IT register.");

		/* Clearing IT Overrun */
		result = SDM35541_Clear_IT_Overrun(board, &my_function_block);
		Fail(result, "Error clearing IT Overrun register.");
   
		printf("Interrupts: %d\r", interrupts);
	}

	printf("\n");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Program clean up
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	
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

	DeleteCriticalSection(&critical_section);

	/* Remove User-Space ISR */
	printf("Removing callback...\n");
	result = SDM35541_Interrupt_Remove_Callback(board, &my_function_block);
	check_result(result, "Error removing callback.");

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
