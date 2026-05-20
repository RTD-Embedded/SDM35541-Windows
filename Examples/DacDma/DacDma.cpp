/**
	@file

	@brief
		Demonstrates the use of Digital to Analog conversion with DMA.

	@verbatim

	This program displays a Sine Wave on either DAC 1 or DAC 2. This sign wave
	is repeated via	DMA and continued to be displayed until the user presses
	CTRL+C to end the program.

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

		$Id: DacDma.cpp 152236 2026-02-16 19:22:03Z bkorpacz $
*/


#include <limits.h>
#include <math.h>
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

#include "DacDma.h"

/**
 * Variable to allow graceful exit from Ctrl-C
 */
volatile uint8_t exit_program;

/**
 * SDM35541 Board Descriptor
 */
struct SDM35541_Board_Descriptor* board;

/**
 * SDM35541 function block
 */
struct SDM35541_Function_Block func_block;

/** 
 * Data used for DMA
 */
uint16_t dma_data[BUFFER_SIZE];

/**
 * The DAC channel
 */
SDM35541_dac_channel_t dac_channel;

/**
 * DAC DMA FIFO source
 */
SDM35541_dma_source_t dma_source;

/**
 * DMA request type
 */
SDM35541_dma_request_t dma_request;

/**
 * DMA channel for DAC1 or DAC2
 */
SDM35541_dma_channel_t dac_dma_channel;


/**
 * Main program code.
 */
int main(int argument_count, char** arguments)
{
	unsigned int minor_number;
	int dac_number;
	int result, i;
	float temp;

	printf("\n\tSDM35541 Digital to Analog with DMA Example Program\n\n");

	/* Install CTRL-C handler */
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlCHandler, TRUE);

	/* Get board number */
	printf("Enter board number to use: ");
	if (scanf_s("%i", &minor_number) == 0) {
		Fail(-1, "Error getting board number\n");
	}
	Fail(minor_number < 0 || minor_number >= SDM35541_MAX_BOARDS, "Board number must be between 0-7.");

	/* Get DAC channel number */
	printf("Enter DAC number (1-2): ");
	if (scanf_s("%i", &dac_number) == 0) {
		Fail(-1, "Error getting DAC number\n");
	}
	Fail(dac_number < 1 || dac_number > 2, "DAC channel must be 1 or 2.");

	/* Assign DAC and DMA variables based on the channel requested. */
	if (dac_number == 1) {
		dac_dma_channel = SDM35541_DMA_CHANNEL_DAC1;
		dma_request = SDM35541_DMA_DEMAND_FIFO_DAC1;
		dma_source = SDM35541_DMA_FIFO_DAC1;
		dac_channel = SDM35541_DAC1;
	}
	else if (dac_number == 2) {
		dac_dma_channel = SDM35541_DMA_CHANNEL_DAC2;
		dma_request = SDM35541_DMA_DEMAND_FIFO_DAC2;
		dma_source = SDM35541_DMA_FIFO_DAC2;
		dac_channel = SDM35541_DAC2;
	}

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Device initialization
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Open the sdm35541 device. */
	printf("Opening sdm35541 with minor number %u...\n",
		minor_number);
	result = SDM35541_Board_Open(minor_number, &board);
	Fail(result, "Error opening the board.");

	/* Reset the board. */
	printf("Resetting the SDM35541...\n");
	result = SDM35541_Gbc_Board_Reset(board);
	Fail(result, "Failed to reset the board.");

	/* Open the sdm35541 function block. */
	printf("Function block open...\n");
	result = SDM35541_Function_Block_Open(board, SDM35541_FUNCTION_BLOCK_ADC1002, &func_block);
	Fail(result, "Error opening ADC1002 function block.");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Initialize DAC and buffer
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Create DAC waveform sinusoidal. */
	for (i = 0; i < BUFFER_SIZE; i++) {
		int data;
		data = (int)(4095. * i / 1024) - 2047;

		dma_data[i] = ((int16_t)data) << 3;
	}

	/* Setup DAC1 */
	printf("Setup DAC %u...\n", dac_number);
	result = SDM35541_DAC_Setup(board, &func_block,
		dac_channel,
		SDM35541_DAC_RANGE_BIPOLAR_5,
		SDM35541_DAC_UPDATE_CLOCK,
		SDM35541_DAC_MODE_NOT_CYCLE);
	Fail(result, "Error setting up DAC.");

	/* Set DAC Rate */
	printf("Setting DAC Rate...\n");
	result =
		SDM35541_DAC_Set_Rate(board, &func_block, SDM35541_DAC_FREQ_8_MHZ, DAC_RATE, &temp);
	Fail(result, "Error setting DAC rate.");
	printf("DAC Rate set to % 6.2f \n", temp);

	/* Clear board-level interrupt levels */
	SDM35541_Clear_IT_Mask(board, &func_block, (SDM35541_int_source_t) 0xffff);
	SDM35541_Clear_IT_Overrun(board, &func_block);

	 /*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		Initialize and Start DMA
		%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Tell DMA engine what is the target of DMA */
	printf("Setting DAC %i DMA Source...\n", dac_number);
	result = SDM35541_DMA_Set_Source(board,
		&func_block,
		0, dma_source, dma_request);
	Fail(result, "Error setting DAC DMA source.");

	/* Setup DMA */
	printf("Setting up DAC %i...\n", dac_number);
	result = SDM35541_Dma_Setup(
		board,
		&func_block,
		dac_dma_channel,
		SDM35541_DMA_SETUP_DIRECTION_WRITE,
		IGNORE_USED
	);
	Fail(result, "Error setting up DMA.");

	/* Initialize DMA Buffer */
	printf("Initializing DMA Buffer\n");
	for (i = 0; i < func_block.num_dma_buffers; i++) {
		uint8_t flags = SDM35541_DMA_BUFFER_CTRL_VALID;
		
		/* Final buffer loops */
		if (i == func_block.num_dma_buffers - 1) {
			flags |= SDM35541_DMA_BUFFER_CTRL_LOOP;
		}

		result = SDM35541_Dma_Buffer_Setup(board, &func_block,
			dac_dma_channel,
			i,
			flags);
		Fail(result, "Error setting up DMA buffer.");
	}

	/* Write Data to DMA Buffer */
	printf("Requesting DMA Transfer...\n");
	for (i = 0; i < func_block.num_dma_buffers; i++) {
		printf("Requesting transfer for buffer %i\n", i);
		result = SDM35541_DMA_Request_Transfer(board,
			func_block.fb_num,
			dac_dma_channel, /* DAC1/2 DMA channel */
			i,  /* DMA buffer 0 */
			SDM35541_DMA_OP_BUFFER_TO_BOARD,
			dma_data,
			BUFFER_SIZE_BYTES,
			false,
			i, /* request id */
			0, /* timeout = 0 */
			false, /* no wait */
			NULL);
		Fail(result, "Error requesting DMA Transfer.");
	}

	/* Start DMA */
	printf("Starting DMA Channel %i...\n", dac_dma_channel);
	result = SDM35541_Dma_Start(board, &func_block, dac_dma_channel);
	Fail(result, "Error starting DAC DMA.");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Main program code.
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Start the DAC */
	printf("Starting DAC%i...\n", dac_number);
	result = SDM35541_DAC_Start(board, &func_block);
	Fail(result, "Error starting DAC.");

	printf("Displaying waveform on DAC %i. Press Ctrl+C to exit...\n", dac_number);

	/* Wait for Ctrl+C. */
	while (!exit_program) {
		SDM35541_Micro_Sleep(500);
	}

	/* Clean up and close board */
	Cleanup();

	system("pause");

	return 0;
}


/**
 * Handler for CTRL-C events.
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

	/* Remove User-Space ISR */
	printf("Remove ISR callback...\n");
	for (int i = 0; i < func_block.num_dma_buffers; i++) {
		result = SDM35541_DMA_Remove_Callback(board, func_block.fb_num, dac_dma_channel, i);
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
