/**
    @file

    @brief
        Demonstrates the use of High Speed Digital acquisition via DMA.

    @verbatim

    This example program uses UTC1 as a demand mode source for HSDIN DMA
    operations.  UTC1 will act as a sample counter and each time FIFO Half is
    counted it will trigger a demand mode DMA.

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

        $Id: HsdinDma.cpp 152256 2026-02-16 21:11:14Z bkorpacz $
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

#include "HsdinDma.h"

/**
 * Board descriptor
 */
struct SDM35541_Board_Descriptor *board;

/**
 * Board function block
 */
struct SDM35541_Function_Block my_function_block;

/*
 * Variable to allow graceful exit from Ctrl-C.
 */
volatile bool exit_program = false;

/**
 * Whether an interrupt has occurred.
 */
volatile bool has_interrupt_occurred = false;

/**
 * Critical section variable
 */
CRITICAL_SECTION critical_section;

/**
 * Result of the callback function
 */
int callback_result;

/**
 * Stores DMA data
 */
uint16_t local_buffer[SDM35541_NUM_DMA_BUFFERS][DMA_BUFFER_SIZE];

/**
 * Whether a DMA buffer is filled with data or not
 */
bool filled_buffers[SDM35541_NUM_DMA_BUFFERS] = { false };


/**
 * DMA callback function
 */
void MyCallback(SDM35541_DMA_Callback_Info callback_info) {
    EnterCriticalSection(&critical_section);

	/* Save the result of the DMA operation */
	callback_result = callback_info.result;

	/* Let main thread know that an interrupt occurred */
    has_interrupt_occurred = true;

	/* Save which buffer just became filled */
	filled_buffers[callback_info.buffer_num] = true;

    LeaveCriticalSection(&critical_section);
}


/**
********************************************************************************
@brief
    Main program code.
********************************************************************************
*/
int main(int argument_count, char **arguments)
{
	unsigned int fifo_size, samples_read, current_buffer;
	unsigned int minor_number;
	int result, i;
	uint16_t all_dma_data[NUM_DATA];

	samples_read = 0;
	current_buffer = 0;

	/* Initialize critical section */
	InitializeCriticalSection(&critical_section);

	/* Install Ctrl+C handler */
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlCHandler, true);

	printf("\n\tSDM35541 High Speed Digital with DMA Example Program\n\n");
    
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
	/* Open the sdm35541 device. */
	printf("Opening sdm35541 with minor number %lu...\n",
		minor_number);
	result = SDM35541_Board_Open(minor_number, &board);
	Fail(result, "Error opening the board.");

	/* Reset the board. */
	printf("Resetting the SDM35541...\n");
	result = SDM35541_Gbc_Board_Reset(board);
	Fail(result, "Failed to reset the board.");

	/* Initialize the board. */
	printf("Board Initialization...\n");
	result = SDM35541_Function_Block_Open(board, SDM35541_FUNCTION_BLOCK_ADC1002, &my_function_block);
	Fail(result, "Error opening the ADC1002 function block.");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Setup User Timer/Counters
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Set high speed digital to sample with UTC0 trigger */
	printf("Setting High Speed Digital Sampling Signal...\n");
	result = SDM35541_HSDIN_Sample_Signal(board,
                           &my_function_block,
						   SDM35541_HSDIN_SIGNAL_UTC0);
	Fail(result,
			     "Error setting HSDIN trigger mode.");
	/* Setup User Timer/Counter 0 */
	printf("User Timer / Counter 0 Setup...\n");
	result = SDM35541_UTC_Setup(board,
                     &my_function_block,
					 SDM35541_UTC_0,
					 SDM35541_CUTC_8_MHZ,
					 SDM35541_GUTC_GATED,
					 SDM35541_UTC_MODE_RATE_GENERATOR,
					 (uint16_t) (8000000 / RATE));
	Fail(result, "Error setting up UTC0.");

	/* Get FIFO Size */
	result = SDM35541_FIFO_Size(board, &fifo_size);
	Fail(result, "Error getting FIFO size.");
	
	/* Setup User Timer/Counter 1 */
	printf("User Timer / Counter 1 Setup...\n");
	result = SDM35541_UTC_Setup(board,
                     &my_function_block,
					 SDM35541_UTC_1,
					 SDM35541_CUTC_HSDIN_SIGNAL,
					 SDM35541_GUTC_NOT_GATED,
					 SDM35541_UTC_MODE_RATE_GENERATOR,
					 8000000 / RATE);
	Fail(result, "Error setting up UTC1.");

	/* Clear high speed digital FIFO */
	printf("Clearing high speed digital FIFO...\n");
	result = SDM35541_HSDIN_Clear(board, &my_function_block);
	Fail(result, "Error clearing the HSDIN FIFO.");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Initialize and Start DMA
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Initialize DMA on channel 0 */
	printf("Initializing DMA Channel 0...\n");
    result = SDM35541_DMA_Set_Source(board, &my_function_block, 
                    0,
					SDM35541_DMA_FIFO_HSDIN,
					SDM35541_DMA_DEMAND_UTC1);
	Fail(result, "Error setting HSDIN DMA Source.");

	/* DMA Setup */
    result = SDM35541_Dma_Setup(board, &my_function_block,
                    SDM35541_DMA_CHANNEL_HSDIN,
                    SDM35541_DMA_SETUP_DIRECTION_READ,
                    IGNORE_USED);
	Fail(result, "Error setting up HSDIN DMA.");

	/* Enable DMA Channel 0 Done Interrupt. */
	printf("Enabling HSDIN DMA Interrupt...\n");
	result = SDM35541_Dma_Configure_Interrupts(board, &my_function_block,
					SDM35541_DMA_CHANNEL_HSDIN, 
					INTERRUPT_ENABLE,
					ERROR_INTR_ENABLE);
	Fail(result, "Error enabling DMA interrupt.");
	
	/* Set up DMA buffers */
    printf("Creating DMA Buffer\n");
	for (i = 0; i < my_function_block.num_dma_buffers; i++) {
        uint8_t ctrl = SDM35541_DMA_BUFFER_CTRL_INTR
			| SDM35541_DMA_BUFFER_CTRL_VALID;

		/* Loop on the last buffer. */
        if (i == my_function_block.num_dma_buffers - 1)  {
            ctrl |= SDM35541_DMA_BUFFER_CTRL_LOOP;
        }

		result = SDM35541_Dma_Buffer_Setup(board, &my_function_block,
			SDM35541_DMA_CHANNEL_HSDIN,
			i,
			ctrl);
		Fail(result, "Error setting up DMA buffer");
	}

    /* Initialize each buffer and install callback */
	for (int buf_num = 0; buf_num < my_function_block.num_dma_buffers; buf_num++) {
		/* Install callback for each buffer */
		result = SDM35541_DMA_Install_Callback(
			board,
			my_function_block.fb_num,
			SDM35541_DMA_CHANNEL_HSDIN,
			buf_num,
			MyCallback,
			NULL);
		Fail(result, "Error installing ISR.");

		result = SDM35541_DMA_Request_Transfer(
			board,
			my_function_block.fb_num,
			SDM35541_DMA_CHANNEL_HSDIN, // HSDIN DMA channel
			buf_num, // DMA buffer number
			SDM35541_DMA_OP_BOARD_TO_BUFFER, // transfer data from board to use
			local_buffer[buf_num], // pointer to local buffer
			DMA_BUFFER_SIZE_BYTES, // size of this buffer in bytes
			false, // no demand
			buf_num, // request id
			0, // timeout = 0
			false, // no wait
			NULL);
		Fail(result, "Error calling SDM35541_DMA_Request_Transfer()");
	}

	/* Start DMA */
	printf("Starting HSDIN DMA...\n");
	result = SDM35541_Dma_Start(board, &my_function_block, SDM35541_DMA_CHANNEL_HSDIN);
	Fail(result, "Error starting HSDIN DMA.");

	/* Start UTC 0 */
	printf("Starting UTC 0...\n");
	result =
	    SDM35541_UTC_Set_Gate(board, &my_function_block, SDM35541_UTC_0, SDM35541_GUTC_NOT_GATED);
	Fail(result, "Error starting UTC 0.");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Receiving Data
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	printf("Obtaining Samples...\n");

	while (samples_read < NUM_DATA && !exit_program) {
        /* Check if interrupt occurred */
        if (!has_interrupt_occurred) {
            SDM35541_Micro_Sleep(100);
            continue;
        }

		EnterCriticalSection(&critical_section);

		/* Check if callback error had occurred */
		Fail(callback_result, "Error in callback thread");

		/*
		 * When one interrupt occurred, we must check to see all buffers that have filled up
		 */
		while (filled_buffers[current_buffer]) {
			printf("DMA Interrupt!\n");
			filled_buffers[current_buffer] = false;
			
			/* Copy data from local buffer */
			memcpy(all_dma_data + samples_read, local_buffer[current_buffer], DMA_BUFFER_SIZE_BYTES);

			samples_read += DMA_BUFFER_SIZE;

			/* Queue another transfer */
			result = SDM35541_DMA_Request_Transfer(
				board,
				my_function_block.fb_num,
				SDM35541_DMA_CHANNEL_HSDIN, // HSDIN DMA channel
				current_buffer, // DMA buffer number
				SDM35541_DMA_OP_BOARD_TO_BUFFER, // transfer data from board to use
				local_buffer[current_buffer], // pointer to local buffer
				DMA_BUFFER_SIZE_BYTES, // size of this buffer in bytes
				false, // no demand
				current_buffer, // request id
				0, // timeout = 0
				false, // no wait
				NULL);
			Fail(result, "Error calling SDM35541_DMA_Request_Transfer()");

			/* Move to next buffer */
			current_buffer++;
			current_buffer %= SDM35541_NUM_DMA_BUFFERS;
		}
		
		/* We've handled this callback. */
		has_interrupt_occurred = false;

		LeaveCriticalSection(&critical_section);

		/* Clear IT register */
		result = SDM35541_Clear_IT_Mask(board, &my_function_block, (SDM35541_int_source_t)0xffff);
		Fail(result, "Error clearing IT register.");

		/* Clear IT Overrun register */
		result = SDM35541_Clear_IT_Overrun(board, &my_function_block);
		Fail(result, "Error clearing IT Overrun register.");
	}

	/* Set User Timer/Counter 0 gate on to stop the clock */
	printf("Stopping User Timer / Counter 0...\n");
	result =
	    SDM35541_UTC_Set_Gate(board, &my_function_block, SDM35541_UTC_0, SDM35541_GUTC_GATED);
	Fail(result, "Error stopping UTC 0.");

	/* Set User Timer/Counter 1 gate on to stop the clock */
	printf("Stopping User Timer / Counter 1...\n");
	result =
	    SDM35541_UTC_Set_Gate(board, &my_function_block, SDM35541_UTC_1, SDM35541_GUTC_GATED);
	Fail(result, "Error stopping UTC 1.");

	/* Print out the samples we obtained. */
    for (i = 0; i < NUM_DATA; i++) {
        if (exit_program) {
            break;
        }
            
        printf("%d %x\n", i, (uint8_t)all_dma_data[i]);
	}

	printf("\n");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Program clean up
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Disable DMA Channel 0 Done Interrupt */
	printf("Disabling DMA Channel 0 Interrupt...\n");
	result = SDM35541_Dma_Configure_Interrupts(board,
				&my_function_block,
				SDM35541_DMA_CHANNEL_HSDIN,
				INTERRUPT_DISABLE,
				ERROR_INTR_DISABLE);
	Fail(result, "Error disabling DMA interrupts.");
	
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
 * Checks for a non-zero return code,
 * prints the error message given and calls the Cleanup function
 */
void Fail(int result, const char* message) {
	if (result) {
		printf("Error (%d):%s\n", result, message);
		Cleanup();
		check_result(-1, "Failed function call.");
		system("pause");
	}
}
