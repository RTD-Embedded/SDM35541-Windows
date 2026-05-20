/**
    @file

    @brief
        Demonstrates the use of the External Trigger rising/falling interrupts.

    @verbatim

    This program uses UTC1 out to set off interrupts.  User Timer/Counter 1
    Out Pin must be routed to External Trigger Pin.  This should cause an
    interrupt to be received every second. Ensure pin 35 is connected to pin 21
    on CN6.

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

		$Id: ExternalTriggerInterrupt.cpp 152277 2026-02-17 16:08:38Z bkorpacz $
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

#include "ExternalTriggerInterrupt.h"

/*
 * Variable to allow graceful exit from Ctrl-C.
 */
volatile bool exit_program = false;

/**
 * Whether an interrupt has occurred.
 */
volatile bool has_interrupt_occurred = false;

/**
 * Status of the interrupt callback function.
 */
volatile int interrupt_result = 0;

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
 * Interrupt callback function.
 */
void MyCallback(SDM35541_Interrupt_Callback_Info callback_info) {
    EnterCriticalSection(&critical_section);
    has_interrupt_occurred = true;
    interrupt_result = callback_info.result;
    LeaveCriticalSection(&critical_section);
}


/**
 * 	Main program code.
 */
int main(int argument_count, char **arguments)
{
    SDM35541_int_source_t edge_val;
    int interrupts = 0;
	int minor_number;
	int edge;
	int result;

	/* Initialize critical section */
	InitializeCriticalSection(&critical_section);

	/* Install Ctrl+C handler */
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlCHandler, true);

	printf("\n\tSDM35541 External Trigger Interrupt Example Program\n\n");

	/* Get board number */
	printf("Enter board number to use: ");
	if (scanf_s("%i", &minor_number) == 0) {
		Fail(-1, "Error getting board number\n");
	}

	Fail(
		minor_number < 0 || minor_number >= SDM35541_MAX_BOARDS,
		"Board number must be between 0-7.");

    /* Get the edge that will trigger the interrupt */
    printf("Edge which will set off the interrupt.\n");
	printf("EDGE:         0 - Rising | 1 - Falling: ");

    if (scanf_s("%i", &edge) == 0) {
        Fail(-1, "Error getting trigger edge.\n");
    }

    Fail(edge < 0 || edge > 1,
        "Edge must be 0 for rising edge or 1 for falling edge trigger.");

	/* Open the sdm35541 device. */
	printf("Opening sdm35541 with minor number %lu...\n",
		minor_number);
	result = SDM35541_Board_Open(minor_number, &board);
	Fail(result, "SDM35541_Board_Open");

    /* Open ADC1002 function block */
    printf("Opening sdm35541 function block...\n");
    result = SDM35541_Function_Block_Open(board, SDM35541_FUNCTION_BLOCK_ADC1002, &my_function_block);
    Fail(result, "Failed to open ADC1002 function block.");

	/* Setup User Timer/Counter 0 to 800Hz */
	printf("User Timer/Counter 0 Setup: 800Hz...\n");
	result = SDM35541_UTC_Setup(board, &my_function_block,
					 SDM35541_UTC_0,
					 SDM35541_CUTC_8_MHZ,
					 SDM35541_GUTC_NOT_GATED,
					 SDM35541_UTC_MODE_RATE_GENERATOR,
					 (uint16_t) 8000);
	Fail(result, "Error setting up UTC 0.");

	/* Setup User Timer/Counter 1 to 1Hz */
	printf("User Timer/Counter 1 Setup: 1Hz...\n");
	result = SDM35541_UTC_Setup(board, &my_function_block,
					 SDM35541_UTC_1,
					 SDM35541_CUTC_UTC_0_OUT,
					 SDM35541_GUTC_NOT_GATED,
					 SDM35541_UTC_MODE_RATE_GENERATOR,
					 (uint16_t) 1000);
	Fail(result, "Error setting up UTC 1.");

	/* Enable interrupt based on user choice. */
	if (edge == RISING_EDGE) {
		/* Enable External Trigger Rising Interrupt */
		printf("Enabling External Trigger Rising Interrupt...\n");
		result =
		    SDM35541_Interrupt_Enable(board, &my_function_block, SDM35541_INT_ETRIG_RISING);
		Fail(result,
				     "Error enabling external interrupt.");

        edge_val = SDM35541_INT_ETRIG_RISING;
	} else if (edge == FALLING_EDGE) {
		/* Enable External Trigger Falling Interrupt */
		printf("Enabling External Trigger Falling Interrupt...\n");
		result =
		    SDM35541_Interrupt_Enable(board, &my_function_block, SDM35541_INT_ETRIG_FALLING);
		Fail(result,
				     "Error enabling external interrupt.");
                     
		edge_val = SDM35541_INT_ETRIG_FALLING;
	}
	
	/* Installing User-Space ISR */
	printf("Installing User-Space ISR...\n");
    result = SDM35541_Interrupt_Install_Callback(
        board,
        &my_function_block,
        MyCallback);
	Fail(result, "Error installing ISR.");

	/* Wait for and test the interrupts as they are received. */
	printf("Waiting for %s edge interrupts \n", edge == RISING_EDGE ? "RISING" : "FALLING");

	while (interrupts < 10 && !exit_program) {
        int int_result;     /* result of interrupt callback */
        bool has_int;       /* whether an interrupt occurred */
        uint16_t status;    /* value of interrupt status register */

        EnterCriticalSection(&critical_section);
        has_int = has_interrupt_occurred;
        int_result = interrupt_result;
        LeaveCriticalSection(&critical_section);
        
        /* Check to see if an interrupt has occurred */
        if (!has_int) {
            SDM35541_Micro_Sleep(100);
            continue;
        }

        /* Check if callback had an error */
        Fail(int_result, "Error occurred in the interrupt callback.");
        
        /* Reset flag so interrupt can be detected again */
        EnterCriticalSection(&critical_section);
        has_interrupt_occurred = false;
        LeaveCriticalSection(&critical_section);

        /* Get interrupt status */
        result = SDM35541_Get_IT_Mask(board, &my_function_block, &status);
        Fail(result, "Error getting interrupt register");

        /* See if external trigger interrupt has occurred. */
        if (status & edge_val) {
            interrupts++;
        }

        /* Clear IT register */
        result = SDM35541_Clear_IT_Mask(board, &my_function_block, (SDM35541_int_source_t)0xffff);
        Fail(result, "Failed to clear IT register.");

        /* Clear IT overrun register */
        result = SDM35541_Clear_IT_Overrun(board, &my_function_block);
        Fail(result, "Failed to clear IT overrun register.");

        /* Print number of interrupts handled */
		printf("%d Received! \r", interrupts);
		fflush(stdout);
	}

	/*
	 * Set User Timer/Counter 0 gate to logic 1 to stop the clock
	 */
	printf("\nStopping User Timer / Counter 0...\n");
	result =
	    SDM35541_UTC_Set_Gate(board, &my_function_block, SDM35541_UTC_0, SDM35541_GUTC_GATED);
	Fail(result, "Error disabling UTC 0.");

	/*
	 * Set User Timer/Counter 1 gate to logic 1 to stop the clock
	 */
	printf("Stopping User Timer / Counter 1...\n");
	result =
	    SDM35541_UTC_Set_Gate(board, &my_function_block, SDM35541_UTC_1, SDM35541_GUTC_GATED);
	Fail(result, "Error disabling UTC 1.");

	/* Clean up and close board */
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
