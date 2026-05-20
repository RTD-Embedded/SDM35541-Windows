/**
	@file

	@brief
		Demonstrates the use of Digital to Analog sampling via DMA.

	@verbatim

	This example program is similar to adc_dma except data is sampled at a
	slower rate and instead of logging to a buffer, the data is dumped to a
	file.
	This program will continually gather A/D samples on a specified channel
	until you ask it to quit.

	This program will run until the user presses Ctrl+C to quit.

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

		$Id: AdcDma.cpp 152256 2026-02-16 21:11:14Z bkorpacz $
*/

#include <fcntl.h>
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

#include "AdcDma.h"


/**
*******************************************************************************
@brief
	The DMA callback that will execute when a DMA interrupt occurs.

@param
	info

	A structure containing information about the interrupt.

 @retval
	None.
 *******************************************************************************
*/
static void MyCallback(SDM35541_DMA_Callback_Info callback_info)
{
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
 * Main program code.
 */

int main(int argument_count, char** arguments)
{
	SDM35541_cgt_entry_t cgt = { 0 };
	unsigned int current_buffer = 0;
	float actualRate;
	int minor_number, channel_number;
	int result;
	int i;
	
	/* Initialize critical section */
	InitializeCriticalSection(&critical_section);

	/* Install Ctrl+C handler */
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

	/* Get ADC channel number */
	printf("Enter ADC Channel number (1-16): ");
	if (scanf_s("%i", &channel_number) == 0) {
		check_result(-1, "Error getting ADC channel number\n");
	}

	check_result(channel_number < 1 || channel_number > 16, "Channel number must be between 1-16.");
	
    /* Channel numbers in the library use 0-based indexing */
    channel_number--;

	/* Open file where data will be dumped */
	result = fopen_s(&file_handle, DAT_FILE, "w");
	Fail(result != 0 || file_handle == NULL, "File Open Failure");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Device initialization
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Open the sdm35541 device. */
	printf("Opening sdm35541 with minor number %lu...\n",
		minor_number);
	result = SDM35541_Board_Open(minor_number, &board);
	Fail(result, "Error opening board.");

	/* Reset the board. */
	printf("Resetting the SDM35541...\n");
	result = SDM35541_Gbc_Board_Reset(board);
	Fail(result, "Failed to reset the board.");

	/* Open the sdm35541 function block. */
	printf("Opening sdm35541 function block %u...\n",
		SDM35541_FUNCTION_BLOCK_ADC1002);
	result = SDM35541_Function_Block_Open(board, SDM35541_FUNCTION_BLOCK_ADC1002, &my_function_block);
	Fail(result, "Error opening ADC1002 function block.");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Main program code.
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Clear ADC FIFO */
	printf("Clearing ADC FIFO...\n");
	result = SDM35541_ADC_Clear(board, &my_function_block);
	Fail(result, "Error clearing ADC FIFO.");

	/* Enable Channel Gain Table */
	printf("Enabling Channel Gain Latch...\n");
	result = SDM35541_CGT_Enable(board, &my_function_block, 0x00);
	Fail(result, "Error enabling Channel Gain Table.");

	/* Create CGT Entry */
	cgt.channel = channel_number;

	/* Write the entry to the CGT Latch register (used for single channel) */
	printf("Writing Channel Gain Table entry...\n");
	result = SDM35541_CGT_Latch(board, &my_function_block, cgt);
	Fail(result, "Error latching Channel Gain Table.");

	/* Setup pacer clock */
	printf("Setting up Pacer Clock...\n");
	result = SDM35541_PCLK_Setup(board, &my_function_block,
		SDM35541_PCLK_INTERNAL,
		SDM35541_PCLK_FREQ_8_MHZ,
		SDM35541_PCLK_NO_REPEAT,
		SDM35541_PCLK_START_SOFTWARE,
		SDM35541_PCLK_STOP_SOFTWARE,
		ADC_RATE, &actualRate);
	Fail(result, "Error setting up pacer clock.");

	/* Set ADC Conversion Signal Select */
	result =
		SDM35541_ADC_Conv_Signal(board, &my_function_block, SDM35541_ADC_CONV_SIGNAL_PCLK);
	Fail(result, "Error setting ADC Conversion signal.");

	/* Set User Output Signal 0 to A/D Conversion */
	printf("Setting User Output Signal 0...\n");
	result = SDM35541_UIO_Select(board, &my_function_block, SDM35541_UIO0, SDM35541_UIO_ADC);
	Fail(result, "Error setting user IO signal 0 to ADC conversion.");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Initialize and Start DMA
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Tell DMA engine what is the target of DMA */
	result = SDM35541_DMA_Set_Source(board,
		&my_function_block,
		0, SDM35541_DMA_FIFO_ADC,
		SDM35541_DMA_DEMAND_FIFO_ADC);
	Fail(result, "Error setting ADC DMA source.");

	/* Set DMA direction and to consider whether buffers have been used or not */
	SDM35541_Dma_Setup(board, &my_function_block, SDM35541_DMA_CHANNEL_ADC,
		SDM35541_DMA_SETUP_DIRECTION_READ, NOT_IGNORE_USED);
	Fail(result, "Error setting up ADC DMA.");

	/* Set-up DMA interrupts */
	result = SDM35541_Dma_Configure_Interrupts(board, &my_function_block, SDM35541_DMA_CHANNEL_ADC,
		INTERRUPT_ENABLE, ERROR_INTR_ENABLE);
	Fail(result, "Error enabling DMA interrupts.");

	/* Set-up buffers */
	for (i = 0; i < my_function_block.num_dma_buffers - 1; i++) {
		result = SDM35541_Dma_Buffer_Setup(board, &my_function_block, SDM35541_DMA_CHANNEL_ADC,
			i, SDM35541_DMA_BUFFER_CTRL_VALID | SDM35541_DMA_BUFFER_CTRL_INTR);
		Fail(result, "Error setting up ADC DMA buffer.");
	}

	/* Set-up last buffer and set it to loop back to the first buffer */
	result = SDM35541_Dma_Buffer_Setup(board, &my_function_block, SDM35541_DMA_CHANNEL_ADC,
		i, SDM35541_DMA_BUFFER_CTRL_VALID
		| SDM35541_DMA_BUFFER_CTRL_INTR | SDM35541_DMA_BUFFER_CTRL_LOOP);
	Fail(result, "Error setting up last ADC DMA buffer.");

	/* Initialize each buffer and install callbacks */
	printf("Installing User-Space Callbacks...\n");
	for (int buf_num = 0; buf_num < my_function_block.num_dma_buffers; buf_num++) {
		/* Install callback for each buffer */
		result = SDM35541_DMA_Install_Callback(
			board,
			my_function_block.fb_num,
			SDM35541_DMA_CHANNEL_ADC,
			buf_num,
			MyCallback,
			NULL);
		Fail(result, "Error installing ISR.");

		result = SDM35541_DMA_Request_Transfer(
			board,
			my_function_block.fb_num,
			SDM35541_DMA_CHANNEL_ADC, // ADC DMA channel
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
	result = SDM35541_Dma_Start(board, &my_function_block, SDM35541_DMA_CHANNEL_ADC);
	Fail(result, "Error starting ADC DMA.");

	/* Start the pacer clock */
	printf("Starting Pacer Clock...\n");
	result = SDM35541_PCLK_Start(board, &my_function_block);
	Fail(result, "SDM35541_PCLK_Start");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Receiving Data
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	printf("Obtaining Samples...\n");

	/*
	 * Loop here and wait for the User ISR to let us know we have received an
	 * interrupt.  If we have, copy the correct half out of the DMA buffer into
	 * our buffer so we can analyze it.
	 */
	while(!exit_program) {
		/* If no callback has occurred, wait. */
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
			
			/* Write DMA buffer to file */
			for (unsigned int i = 0; i < DMA_BUFFER_SIZE; i++) {
				float data;
				data = (float)((int16_t)local_buffer[current_buffer][i] >> 3);
				fprintf_s(file_handle, "%2.2f\n", data / 4096.0 * 10.0);
			}

			/* Queue another transfer */
			result = SDM35541_DMA_Request_Transfer(
				board,
				my_function_block.fb_num,
				SDM35541_DMA_CHANNEL_ADC, // ADC DMA channel
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

	/* Stop the pacer clock */
	printf("Stopping Pacer Clock...\n");
	result = SDM35541_PCLK_Stop(board, &my_function_block);
	Fail(result, "Error stopping pacer clock.");

	printf("Wrote data to " DAT_FILE "\n");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Program clean up
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Disable DMA Channel 0 Interrupt */
	printf("Disabling DMA Channel 0 Interrupt...\n");
	result = SDM35541_Dma_Configure_Interrupts(board, &my_function_block,
		SDM35541_DMA_CHANNEL_ADC, INTERRUPT_DISABLE, ERROR_INTR_DISABLE);
	Fail(result, "Failed to disable DMA interrupt.");

	/* Clean up memory */
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

	/* Close file if it was opened */
	if (file_handle != NULL) {
		fclose(file_handle);
	}

	DeleteCriticalSection(&critical_section);

	/* Remove DMA Callback */
	printf("Remove DMA Callback...\n");
	for (int i = 0; i < my_function_block.num_dma_buffers; i++) {
		result = SDM35541_DMA_Remove_Callback(board, my_function_block.fb_num, SDM35541_DMA_CHANNEL_ADC, i);
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
