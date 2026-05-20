/**
    @file

    @brief
        Demonstrates the use of the Digital I/O Event Mode to generate
        interrupts.

    @verbatim

    Unlike Match mode where an interrupt only occurs when
    Port 0 contains a certain value, event mode interrupts are triggered
    whenever the value at Port 0 changes.  The same value array used in
    the match mode example is iterated through here, and you will see an
    interrupt triggered each time.    This program should generate ten
    interrupts.

    Port0 must be connected to Port1 bit-per-bit (Port0 bit 0 connected to
    Port1 bit 0 and so on...).

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

		$Id: DioEvent.cpp 152193 2026-02-13 21:38:28Z bkorpacz $
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

#include "DioEvent.h"

/**
 * Variable to allow graceful exit from Ctrl-C.
 */
volatile bool exit_program = false;

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
 * Main program code.
 */
int main(int argument_count, char **arguments) {
    int minor_number;
	int result;
	int i;
    int interrupts = 0;
	uint8_t vals[] = { 0x01, 0xFF, 0xCC, 0xBA, 0xAB,
		0x12, 0xF3, 0x3F, 0x60, 0x3C
	};
    
	/* Initialize critical section */
	InitializeCriticalSection(&critical_section);

	/* Install Ctrl+C handler */
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlCHandler, true);

	printf("\n\tSDM35541 Digital I/O Event Mode Interrupt Example Program\n\n");
    
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
	Fail(result, "Error opening the board.");

	/* Reset the board. */
	printf("Resetting the SDM35541...\n");
	result = SDM35541_Gbc_Board_Reset(board);
	Fail(result, "Failed to reset the board.");

	/* Open the sdm35541 function block. */
	printf("Opening sdm35541 function block %u...\n",
		SDM35541_FUNCTION_BLOCK_ADC1002);
	result = SDM35541_Function_Block_Open(board, SDM35541_FUNCTION_BLOCK_ADC1002, &my_function_block);
	Fail(result, "SDM35541_Function_Block_Open");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Setup Digital Input/Output Configuration
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	
	/* Set DIO IRQ Mode */
	printf("Setting DIO Match Mode...\n");
	result = SDM35541_DIO_IRQ_Mode(board, &my_function_block, SDM35541_DIO_MODE_EVENT);
	Fail(result, "Error setting DIO IRQ mode.");

	/* Set DIO Clock */
	printf("Setting DIO Sample Clock to 8MHz...\n");
	result = SDM35541_DIO_Clock(board, &my_function_block, SDM35541_DIO_CLK_8MHZ);
	Fail(result, "Error setting DIO Clock to 8MHz.");

	/* Configure Port 0 Direction */
	printf("Configuring Port 0 Direction...\n");
	result = SDM35541_DIO_Set_Direction(board, &my_function_block, SDM35541_DIO_PORT0, 0x00);
	Fail(result, "Error setting DIO Port 0 Direction.");

	/* Configure Port 1 Direction */
	printf("Configuring Port 1 Direction...\n");
	result = SDM35541_DIO_Set_Direction(board, &my_function_block, SDM35541_DIO_PORT1, 0xFF);
	Fail(result, "Error setting DIO Port 1 Direction.");

	/*##########################################################################
	   Main Program Code
	   ###################################################################### */

	/* Enable Digital Interrupt Interrupt */
	printf("Enabling Digital Interrupt...\n");
	result = SDM35541_Interrupt_Enable(board, &my_function_block, SDM35541_INT_DIO);
	Fail(result, "Error enabling interrupts.");

	/* Enable DIO IRQ */
	printf("Enabling DIO Interrupts...\n");
	result = SDM35541_DIO_Enable_IRQ(board, &my_function_block, 0xFF);
	Fail(result, "Error enabling DIO IRQ.");

	/* Wait for and test the interrupts as they are received. */
	for (i = 0; i < 10; i++) {
        uint16_t status;
        
        if (exit_program) {
            break;
        }

        /* Set DIO port 1 value. */
		SDM35541_DIO_Set_Port(board, &my_function_block, SDM35541_DIO_PORT1, vals[i]);
		Fail(result,
				     "Error setting value of DIO Port 1.");
        
        /* 
         * Sleep for 1 second. An interrupt should have occurred
         * during this time.
         */
		SDM35541_Micro_Sleep(1000000);

		if (exit_program) {
			break;
		}

        /* Check board-level interrupts */
        result = SDM35541_Get_IT_Mask(board, &my_function_block, &status);
        Fail(result, "Error getting IT Mask");

        /* Check for DIO interrupt */
        if (status & SDM35541_INT_DIO) {
            interrupts++;

            /* Clear DIO IRQ */
            result = SDM35541_DIO_Clear_IRQ(board, &my_function_block);
            Fail(result, "Error clearing DIO Clear IRQ");
        }

        /* Clear IT register */
        result = SDM35541_Clear_IT_Mask(board, &my_function_block, (SDM35541_int_source_t)0xffff);
        Fail(result, "Error clearing IT Register.");
        
        /* Clear interrupt overrun register */
        result = SDM35541_Clear_IT_Overrun(board, &my_function_block);
        Fail(result, "Error clearing IT Overrun Register.");
        
        /* Print number of interrupts */
		printf("Port 1 Output:0x%2x Interrupts %d \r\n", vals[i],
			interrupts);
		fflush(stdout);
	}

	printf("\n");

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

    /* Disable interrupts */
    printf("Disabling DIO Interrupt...\n");
    result = SDM35541_Interrupt_Disable(board, &my_function_block, SDM35541_INT_DIO);
    check_result(result, "Error disabling interrupts.");

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
