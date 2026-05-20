/**
	@file

	@brief
		Demonstrates the use of Analog to Digital sampling via external trigger.

	@verbatim

	This example program uses the external trigger to toggle the Pacer Clock.
	While the External Trigger is high the Pacer Clock will run and while it is
	low the Pacer Clock will stop.  The status of the Pacer Clock will be
	printed to the screen as External Trigger Edge interrupts are received by
	the user-space ISR.

	Digital I/O Port 1 is used as an input to the external trigger.  The value
	on Port 1 is toggled with the strike of a key on the keyboard.  This
	effectively enables/disables acquisition.

	Note: This program uses DMA

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

		$Id: AdcExternalTrigger.cpp 152297 2026-02-18 14:52:52Z bkorpacz $
*/

#include <conio.h>
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
#include "sdm35541_dma_library.h"
#include "SDM35541.h"

#include "AdcExternalTrigger.h"


/**
 * Interrupt callback
 */
static void MyInterruptCallback(SDM35541_Interrupt_Callback_Info int_info) {
	EnterCriticalSection(&critical_section);
	has_interrupt_occurred = true;
	interrupt_callback_result = int_info.result;
	LeaveCriticalSection(&critical_section);
}


/**
 * DMA Interrupt callback
 */
static void MyDmaCallback(SDM35541_DMA_Callback_Info int_info) {
	EnterCriticalSection(&critical_section);
	has_dma_interrupt_occurred = true;
	dma_callback_result = int_info.result;
	filled_buffers[int_info.buffer_num] = true;
	LeaveCriticalSection(&critical_section);
}

/**
 * Current buffer number that will be filled by DMA.
 */
unsigned int current_buffer_number = 0;

/**
 * Offset in the local buffer
 */
unsigned int buffer_offset = 0;

/**
 * Number of DMA interrupts processed.
 */
int num_ints = 0;


/**
 * Main program code.
 */
int main(int argument_count, char** arguments) {
	SDM35541_cgt_entry_t cgt = { 0 };
	bool is_pacer_enabled = false;
	int channel_number;
	int minor_number;
	int buf_num;
	int result;
	float actualRate;
	uint16_t data;

	/* Initialize critical section */
	InitializeCriticalSection(&critical_section);

	/* Install Ctrl+C handler */
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlCHandler, true);

	/* Get board number */
	printf("Enter board number to use: ");
	if (scanf_s("%i", &minor_number) == 0) {
		Fail(-1, "Error getting board number\n");
	}
	Fail(
		minor_number < 0 || minor_number >= SDM35541_MAX_BOARDS,
		"Board number must be between 0-7."
	);

	/* Get ADC channel number */
	printf("Enter ADC Channel number (1-16): ");
	if (scanf_s("%i", &channel_number) == 0) {
		check_result(-1, "Error getting ADC channel number\n");
	}
	
	check_result(
		channel_number < 1 || channel_number > 16,
		"Channel number must be between 1-16."
	);
	
    /* Channel numbers in the library use 0-based indexing */
    channel_number--;

	getchar();

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
	Fail(result, "Error resetting the board.");

	/* Open function block */
	printf("Opening sdm35541 function block...\n");
	result = SDM35541_Function_Block_Open(board, SDM35541_FUNCTION_BLOCK_ADC1002, &my_function_block);
	Fail(result, "Error opening ADC1002 function block");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Main program code.
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	/* Configure Port 1 Direction */
	printf("Configuring Port 1 Direction...\n");
	result = SDM35541_DIO_Set_Direction(board, &my_function_block, SDM35541_DIO_PORT1, 0xFF);
	Fail(result, "Error setting DIO Port 1 direction.");

	/* Setting Port 1 Output */
	printf("Setting Port 1 Output...\n");
	SDM35541_DIO_Set_Port(board, &my_function_block, SDM35541_DIO_PORT1, 0x00);
	Fail(result, "Error setting DIO Port 1's value.");

	/* Clear ADC FIFO */
	printf("Clearing ADC FIFO...\n");
	result = SDM35541_ADC_Clear(board, &my_function_block);
	Fail(result, "Error clearing ADC FIFO.");

	/* Get FIFO Status */
	result = SDM35541_FIFO_Get_Status(board, &my_function_block, &data);
	Fail(result, "Error getting FIFO status.");
	printf("FIFO Status: 0x%4x\n", data);

	/* Enable Channel Gain Table */
	printf("Enabling Channel Gain Latch...\n");
	result = SDM35541_CGT_Enable(board, &my_function_block, 0x00);
	Fail(result, "Error enabling Channel Gain Table.");

	/* Create CGT Entry */
	cgt.channel = channel_number;

	/* Write the entry to the CGT Latch register (used for single channel) */
	printf("Writing Channel Gain Latch...\n");
	result = SDM35541_CGT_Latch(board, &my_function_block, cgt);
	Fail(result, "Error latching Channel Gain Table.");

	/* Setup pacer clock */
	printf("Setting up Pacer Clock...\n");
	result = SDM35541_PCLK_Setup(board, &my_function_block,
		SDM35541_PCLK_INTERNAL,
		SDM35541_PCLK_FREQ_8_MHZ,
		SDM35541_PCLK_REPEAT,
		SDM35541_PCLK_START_ETRIG_GATE,
		SDM35541_PCLK_STOP_SOFTWARE,
		ADC_RATE, &actualRate);
	Fail(result, "Error setting up pacer clock.");

	/* Set ADC Conversion Signal Select */
	result =
		SDM35541_ADC_Conv_Signal(board, &my_function_block, SDM35541_ADC_CONV_SIGNAL_PCLK);
	Fail(result, "Error setting ADC conversion select signal.");

	/* Setting External Trigger Polarity */
	printf("Setting External Trigger Polarity...\n");
	result =
		SDM35541_ETRIG_Polarity_Select(board, &my_function_block, SDM35541_EXT_POLARITY_POS);
	Fail(result,
		"Error setting external trigger edge.");

	/* Enabling external trigger interrupts */
	printf("Enabling External Trigger Rising / Falling Interrupts...\n");
	result =
		SDM35541_Interrupt_Enable(board, &my_function_block,
			(SDM35541_int_source_t)(SDM35541_INT_ETRIG_RISING |
				SDM35541_INT_ETRIG_FALLING));
	Fail(result, "Error enabling external trigger interrupts.");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Initialize and Start DMA
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */

	/* Tell DMA engine what is the target of DMA */
	result = SDM35541_DMA_Set_Source(board,
		&my_function_block,
		0, SDM35541_DMA_FIFO_ADC,
		SDM35541_DMA_DEMAND_FIFO_ADC);
	Fail(result, "Error setting DMA source.");

	/* Set DMA direction and to consider whether buffers have been used or not */
	result = SDM35541_Dma_Setup(
		board,
		&my_function_block,
		SDM35541_DMA_CHANNEL_ADC,
		SDM35541_DMA_SETUP_DIRECTION_READ,
		NOT_IGNORE_USED
	);
	Fail(result, "Error setting up DMA.");

	/* Set-up DMA interrupts */
	result = SDM35541_Dma_Configure_Interrupts(
		board,
		&my_function_block,
		SDM35541_DMA_CHANNEL_ADC,
		INTERRUPT_ENABLE,
		ERROR_INTR_ENABLE
	);
	Fail(result, "Error enabling DMA interrupts.");

	/* Set-up buffers and install DMA callbacks */
	printf("Setting up DMA buffers and installing callbacks...\n");
	for (buf_num = 0; buf_num < my_function_block.num_dma_buffers; buf_num++) {
		uint8_t ctrl = SDM35541_DMA_BUFFER_CTRL_VALID | SDM35541_DMA_BUFFER_CTRL_INTR;

		/* Last buffer needs to loop */
		if (buf_num == my_function_block.num_dma_buffers - 1) {
			ctrl |= SDM35541_DMA_BUFFER_CTRL_LOOP;
		}

		/* Setup DMA transfer */
		result = SDM35541_Dma_Buffer_Setup(board, &my_function_block, SDM35541_DMA_CHANNEL_ADC,
			buf_num, ctrl);
		Fail(result, "Error setting up DMA buffer.");

		/* Install user callback */
		result = SDM35541_DMA_Install_Callback(
			board,
			my_function_block.fb_num,
			SDM35541_DMA_CHANNEL_ADC,
			buf_num,
			MyDmaCallback,
			NULL
		);
		Fail(result, "Error installing callback.");

		/* Request DMA Transfer */
		result = SDM35541_DMA_Request_Transfer(
			board,
			my_function_block.fb_num,
			SDM35541_DMA_CHANNEL_ADC, // ADC DMA channel
			buf_num, // DMA buffer number
			SDM35541_DMA_OP_BOARD_TO_BUFFER, // transfer data from board to use
			&local_buffer[buffer_offset], // pointer to local buffer
			DMA_BUFFER_SIZE_BYTES, // size of this buffer in bytes
			false, // no demand
			buf_num, // request id
			0, // timeout = 0
			false, // no wait
			NULL);
		Fail(result, "Error calling SDM35541_DMA_Request_Transfer()");

		buffer_offset += DMA_BUFFER_SIZE;
	}

	/* Install Interrupt Callback */
	result = SDM35541_Interrupt_Install_Callback(board,
		&my_function_block, MyInterruptCallback);

	/* Start DMA */
	result = SDM35541_Dma_Start(board, &my_function_block, SDM35541_DMA_CHANNEL_ADC);
	Fail(result, "Error starting ADC DMA.");

	/* Start the pacer clock */
	printf("Starting Pacer Clock...\n");
	result = SDM35541_PCLK_Start(board, &my_function_block);
	Fail(result, "Error starting pacer clock.");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Receiving Data
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	printf("Waiting for External Trigger...\n");

	/*
	 * Loop here and wait for the User ISR to let us know we have received an
	 * interrupt.  If we have, copy the correct half out of the DMA buffer into
	 * our buffer so we can analyze it.
	 */
	while (!exit_program && num_ints < NUM_INTS) {
		bool has_int, has_dma_int;

		/* Check if ENTER key pressed */
		if (_kbhit()) {
			getchar();
			if (is_pacer_enabled) {
				printf("Disabling Pacer Clock!\n");
				SDM35541_DIO_Set_Port(board, &my_function_block, SDM35541_DIO_PORT1,
					0x00);
				Fail(result,
					"Error setting DIO Port 1's value.");
				is_pacer_enabled = false;
			}
			else {
				printf("Enabling Pacer Clock!\n");
				SDM35541_DIO_Set_Port(board, &my_function_block, SDM35541_DIO_PORT1,
					0xFF);
				Fail(result,
					"Error setting DIO Port 1's value.");
				is_pacer_enabled = true;
			}
		}

		SDM35541_Micro_Sleep(100);

		/* Check if an interrupt occurred */
		EnterCriticalSection(&critical_section);
		has_int = has_interrupt_occurred;
		has_dma_int = has_dma_interrupt_occurred;
		LeaveCriticalSection(&critical_section);

		/* Handle board-level interrupt */
		if (has_int) {
			handle_interrupt();
		}
		
		/* Handle DMA interrupt */
		if (has_dma_int) {
			handle_dma_interrupt();
		}

	}

	/* Check FIFO Status */
	result = SDM35541_FIFO_Get_Status(board, &my_function_block, &data);
	Fail(result, "Error getting FIFO status.");
	printf("FIFO Status: 0x%4x\n", data);

	/* Stop the pacer clock */
	printf("Stopping Pacer Clock...\n");
	result = SDM35541_PCLK_Stop(board, &my_function_block);
	Fail(result, "Error stopping pacer clock.");

	/* Print out the samples we obtained. */
	for (unsigned int i = 0; i < NUM_DATA; i++) {
		if (exit_program) {
			break;
		}

		printf("% 2.2f ",
			((float)(SDM35541_ADC_ANALOG_DATA(local_buffer[i]) / 4096.) * 10));
	}

	printf("\n");

	/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	   Program clean up
	   %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */
	   
	/* Disable DMA Channel 0 Done Interrupt */
	printf("Disabling DMA Channel 0 Interrupt...\n");
	result = SDM35541_Dma_Configure_Interrupts(board, &my_function_block,
		SDM35541_DMA_CHANNEL_ADC, INTERRUPT_DISABLE, ERROR_INTR_DISABLE);
	Fail(result, "Error disabling DMA interrupt.");

	/* Disable external trigger interrupts */
	result =
		SDM35541_Interrupt_Disable(board, &my_function_block,
			(SDM35541_int_source_t)(SDM35541_INT_ETRIG_RISING |
				SDM35541_INT_ETRIG_FALLING));
	Fail(result, "Error disabling external trigger interrupts.");

	/* Disable DMA */
	printf("Disabling DMA...\n");
	result = SDM35541_Dma_Stop(board, &my_function_block, SDM35541_DMA_CHANNEL_ADC);
	Fail(result, "Error stopping DMA.");

	Cleanup();

	system("pause");
	return 0;
}


/** 
 * Handle board-level interrupts.
 */
void handle_interrupt() {
	uint16_t status;
	int result;

	EnterCriticalSection(&critical_section);

	/* Check if error occurred in callback */
	Fail(interrupt_callback_result, "Error in Interrupt Callback.");

	has_interrupt_occurred = false;
	
	LeaveCriticalSection(&critical_section);

	/* Check for rising/falling edge interrupt */
	result = SDM35541_Get_IT_Mask(board, &my_function_block, &status);
	Fail(result, "Error getting interrupt mask.");

	if (status & SDM35541_INT_ETRIG_RISING) {
		printf("External Trigger High -- Pacer Clock On!\n");
	}

	if (status & SDM35541_INT_ETRIG_FALLING) {
		printf("External Trigger Low -- Pacer Clock OFF!\n");
	}

	/* Clear IT register */
	result = SDM35541_Clear_IT_Mask(board, &my_function_block, (SDM35541_int_source_t)0xffff);
	Fail(result, "Error clearing IT register.");

	/* Clear IT Overrun register */
	result = SDM35541_Clear_IT_Overrun(board, &my_function_block);
	Fail(result, "Error clearing IT Overrun register.");
}


/**
 * Handle DMA interrupts.
 */
void handle_dma_interrupt() {
	int result;

	EnterCriticalSection(&critical_section);

	/* Check if error occurred in callback */
	Fail(dma_callback_result, "Error in DMA Callback.");

	while (filled_buffers[current_buffer_number]) {
		num_ints++;
		printf("DMA Interrupt!\n");

		/* Allow buffer to be reused */
		filled_buffers[current_buffer_number] = false;

		/* Request New Transfer if there is space in the local buffer. */
		if (buffer_offset < NUM_DATA) {
			result = SDM35541_DMA_Request_Transfer(
				board,
				my_function_block.fb_num,
				SDM35541_DMA_CHANNEL_ADC, // ADC DMA channel
				current_buffer_number, // DMA buffer number
				SDM35541_DMA_OP_BOARD_TO_BUFFER, // transfer data from board to use
				&local_buffer[buffer_offset], // pointer to local buffer
				DMA_BUFFER_SIZE_BYTES, // size of this buffer in bytes
				false, // no demand
				current_buffer_number, // request id
				0, // timeout = 0
				false, // no wait
				NULL);
			Fail(result, "Error calling SDM35541_DMA_Request_Transfer()");
		}

		/* Increment buffer number and offset */
		current_buffer_number++;
		current_buffer_number %= my_function_block.num_dma_buffers;
		buffer_offset += DMA_BUFFER_SIZE;
	}

	/* DMA interrupt has been processed */
	has_dma_interrupt_occurred = false;

	LeaveCriticalSection(&critical_section);
}


/**
 * Handler for CTRL - C events. Cleanup and exit.
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

	/* Remove Interrupt callback */
	result = SDM35541_Interrupt_Remove_Callback(board, &my_function_block);
	check_result(result, "Error removing interrupt callback.");

	/* Remove DMA Callbacks */
	printf("Remove ISR callback...\n");
	for (int i = 0; i < my_function_block.num_dma_buffers; i++) {
		result = SDM35541_DMA_Remove_Callback(
			board,
			my_function_block.fb_num,
			SDM35541_DMA_CHANNEL_ADC,
			i);
		check_result(result, "Error removing DMA callback.");
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
 * prints the error message given and calls the Cleanup function.
 */
void Fail(int result, const char* message) {
	if (result) {
		printf("Error (%d):%s\n", result, message);
		Cleanup();
		check_result(-1, "Failed function call.");
		system("pause");
	}
}
