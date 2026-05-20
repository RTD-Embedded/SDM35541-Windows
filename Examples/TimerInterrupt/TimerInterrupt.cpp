/**
    @file

    @brief
        Demonstrates the use of the User Timer/Counter Out Interrupts.

    @verbatim

    UTC0 is set to 1kHz and UTC1 is set to 1Hz.  An interrupt will occur every
    UTC1 out and UTC1 Inverted Out.  This should end up causing an interrupt.
    Every second from each source but the interrupts will be shifted half a
    second apart. The program counts to ten interrupts, on each channel.

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

		$Id: TimerInterrupt.cpp 152193 2026-02-13 21:38:28Z bkorpacz $
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

#include "TimerInterrupt.h"

/**
 * Whether an interrupt has occurred.
 */
volatile bool has_interrupt_occurred = false;

/**
 * Whether the Ctrl+C handler.
 */
volatile bool exit_program = false;

/**
 * Critical section variable
 */
CRITICAL_SECTION critical_section;

/**
 * SDM35541 Board descriptor
 */
struct SDM35541_Board_Descriptor *board;

/**
 * ADC1002 Function Block
 */
struct SDM35541_Function_Block my_function_block;

/**
 * Interrupt callback information.
 */
volatile int interrupt_result = 0;

/**
 * Global used to keep track of the number of interrupts received.
 */
uint8_t utc1_int = 0;

/**
 * Global used to keep track of the number of interrupts received.
 */
uint8_t utc1_int_inverted = 0;


/**
 * Interrupt callback function
 */
void MyCallback(SDM35541_Interrupt_Callback_Info int_info) {
    EnterCriticalSection(&critical_section);
    has_interrupt_occurred = true;
	interrupt_result = int_info.result;
    LeaveCriticalSection(&critical_section);
}


/**
 * Main program code.
 */
int main(int argument_count, char **arguments)
{
	int result, minor_number;

	/* Initialize critical section */
	InitializeCriticalSection(&critical_section);

	/* Install Ctrl+C handler */
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlCHandler, true);

	printf("\n\tSDM35541 User Timer/Counter Interrupt Example Program\n\n");

	/* Get board number */
	printf("Enter board number to use: ");
	if (scanf_s("%i", &minor_number) == 0) {
		Fail(-1, "Error getting board number\n");
	}

	Fail(
		minor_number < 0 || minor_number >= SDM35541_MAX_BOARDS,
		"Board number must be between 0-7."
	);

    /*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Device initialization
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/*
	 * Open the sdm35541 device.
	 */
	printf("Opening sdm35541 with minor number %lu...\n",
		minor_number);
	result = SDM35541_Board_Open(minor_number, &board);
	Fail(result, "SDM35541_Board_Open");


	/* Reset the board. */
	printf("Resetting the sdm35541...\n");
	result = SDM35541_Gbc_Board_Reset(board);
	check_result(result, "Error resetting the board.");

    /*
     * Open ADC1002 function block
     */
    printf("Opening sdm35541 function block...\n");
    result = SDM35541_Function_Block_Open(board, SDM35541_FUNCTION_BLOCK_ADC1002, &my_function_block);
    Fail(result, "Failed to open ADC1002 function block.");
	
	/*
	 * Setup User Timer/Counter 0
	 */
	printf("User Timer/Counter 0 Setup...\n");
	result = SDM35541_UTC_Setup(board, &my_function_block,
					 SDM35541_UTC_0,
					 SDM35541_CUTC_8_MHZ,
					 SDM35541_GUTC_NOT_GATED,
					 SDM35541_UTC_MODE_RATE_GENERATOR,
					 (uint16_t) UTC0);
	Fail(result, "Error setting up UTC 0.");

	/*
	 * Setup User Timer/Counter 1
	 */
	printf("User Timer/Counter 1 Setup...\n");
	result = SDM35541_UTC_Setup(board, &my_function_block,
					 SDM35541_UTC_1,
					 SDM35541_CUTC_UTC_0_OUT,
					 SDM35541_GUTC_NOT_GATED,
					 SDM35541_UTC_MODE_SQUARE_WAVE,
					 (uint16_t) UTC1);
	Fail(result, "Error setting up UTC 1.");

	/*
	 * Enable UTC1 Out and ~UTC1 Out Interrupt
	 */
	printf("Enabling UTC1 and ~UTC1 Interrupts...\n");
	result = SDM35541_Interrupt_Enable(board, &my_function_block,
		(SDM35541_int_source_t)(SDM35541_INT_UTC1 |
			SDM35541_INT_UTC1_INV));
	Fail(result, "Error enabling UTC interrupts.");

	/*
	 * Installing User-Space ISR
	 */
	printf("Installing User-Space ISR...\n");
	result = SDM35541_Interrupt_Install_Callback(
        board,
        &my_function_block,
        MyCallback);
	Fail(result, "Error installing ISR.");

	/*##########################################################################
	   Show Elapsed Time
	   ###################################################################### */

	printf("\nWaiting to receive interrupts...\n\n");

	int temp1 = utc1_int;
	int temp2 = utc1_int_inverted;

	while (utc1_int < 10 && !exit_program) {
		int int_result;
        uint16_t status;
		bool has_int;

		EnterCriticalSection(&critical_section);
		has_int = has_interrupt_occurred;
		int_result = interrupt_result;
		LeaveCriticalSection(&critical_section);

        if (!has_int) {
            SDM35541_Micro_Sleep(100);
            continue;
        }

		/* Ensure no error occurred in callback */
		Fail(interrupt_result, "Error in the interrupt callback.");

        /* Get interrupt register */
        result = SDM35541_Get_IT_Mask(board, &my_function_block, &status);
        Fail(result, "Error getting interrupt register");

        /* See if external trigger has occurred. */
        if (status & SDM35541_INT_UTC1) {
            utc1_int++;
        }

        if (status & SDM35541_INT_UTC1_INV) {
            utc1_int_inverted++;
        }

        /*
         * Clear IT register
         */
        result = SDM35541_Clear_IT_Mask(board, &my_function_block, (SDM35541_int_source_t)0xffff);
        Fail(result, "Failed to clear IT register.");

        /*
         * Clear IT overrun register
         */
        result = SDM35541_Clear_IT_Overrun(board, &my_function_block);
        Fail(result, "Failed to clear IT overrun register.");

        printf("UTC1: %d | ~UTC1: %d \r", utc1_int,
            utc1_int_inverted);
        fflush(stdout);

		EnterCriticalSection(&critical_section);
		has_interrupt_occurred = false;
		LeaveCriticalSection(&critical_section);

	}

	printf("\n\n");

	/*
	 * Set User Timer/Counter 0 gate to logic 1 to stop the clock
	 */
	printf("Stopping User Timer / Counter 0...\n");
	result =
	    SDM35541_UTC_Set_Gate(board, &my_function_block, SDM35541_UTC_0, SDM35541_GUTC_GATED);
	Fail(result, "Error stopping UTC 0.");

	/*
	 * Set User Timer/Counter 1 gate to logic 1 to stop the clock
	 */
	printf("Stopping User Timer / Counter 1...\n");
	result =
	    SDM35541_UTC_Set_Gate(board, &my_function_block, SDM35541_UTC_1, SDM35541_GUTC_GATED);
	Fail(result, "Error stopping UCT 1.");

	/*
	 * Remove User-Space ISR
	 */
	printf("Removing User-Space ISR...\n");
	result = SDM35541_Interrupt_Remove_Callback(board, &my_function_block);
	Fail(result, "Error removing ISR.");

	Cleanup();

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


/**
 * Close board, clean up interrupts, and free memory
 */
void Cleanup() {
	int result;

	DeleteCriticalSection(&critical_section);

	/* Remove User-Space ISR */
	printf("Remove ISR callback...\n");
	for (int i = 0; i < my_function_block.num_dma_buffers; i++) {
		result = SDM35541_DMA_Remove_Callback(board, my_function_block.fb_num, SDM35541_DMA_CHANNEL_HSDIN, i);
		check_result(result, "Error removing callback.");
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
