/**
    @file

    @brief
        Example program which demonstrates use of the library to
        open a function block for use.

    @verbatim

        This example program uses the board library to query all
        function blocks on the board.  When a function block is opened
        that has a valid function type, then the number of DMA channels
        and buffers is printed to the screen.  In this way, the example
        program shows an inventory of the function blocks on a given
        board.

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

    $Id: ListFunctionBlocks.cpp 152277 2026-02-17 16:08:38Z bkorpacz $
*/

#include <stdio.h>
#include <errno.h>

#include "SDM35541_gbc_library.h"
#include "SDM35541_board_access.h"
#include "SDM35541_util_library.h"

void Fail(int result, char* message);
void CleanUp();

/**
 * Pointer to the board descriptor used for this example
 */
struct SDM35541_Board_Descriptor *board = NULL;

/**
 * holds the function block information when each
 * function block is opened.
 */
struct SDM35541_Function_Block my_func_block;

int main(int argc, char* argv[])
{
    SDM35541_EnumInfo  device_info;
    SDM35541_EnumInfo  enum_list[8];
    UINT8 enum_max = 8;
	unsigned int board_num = 0;
	int fb_num = 0;
	char errMsg[300];
    int result;

    // Board number
    printf("Enter board number to use:");
    if(scanf_s("%d",&board_num) == 0)
    {
        printf("Error getting board number\n");
        system("pause");
        return(-1);
    }
    
    if(board_num >= enum_max)
    {
        printf("Error: board number cannot exceed %u, exiting.\n", enum_max);
        system("pause");
        return(-1);
    }
    
    // Enumerate the SDM35541 boards available
    result = SDM35541_General_Enum_Boards(enum_list, enum_max);
    Fail(result,"No SDM35541 boards found, exiting");

    // grab the desired SDM35541 in the list
    device_info = enum_list[board_num];

    // make sure the SDM35541 is ready for use
    Fail(device_info.avail != SDM35541_INTRFC_AVAIL_READY, "SDM35541 is not ready for use");

    printf("Opening board.....");
	result = SDM35541_Board_Open(board_num, &board);
	Fail(result, "Could not open board");

	printf("success.\nResetting board.....");
	result = SDM35541_Gbc_Board_Reset(board);
    Fail(result, "Error resetting board");
	printf("success.\n");
	
	printf
	    ("\nListing Function Blocks\n====================================================\n");

	fb_num = 0;
	while ((fb_num < SDM35541_MAX_FB) && (errno != ERANGE)) {
		result = SDM35541_Function_Block_Open(board, fb_num, &my_func_block);

		/*
		 * Because the GBC size is allowed to be less than what it
		 * would take to define all possible function blocks, we
		 * need to capture that error case and not report it.
		 * It just means we're done finding function blocks.
		 */
		if (errno != ERANGE) {

			sprintf_s(errMsg, "Could not open function block %d.", fb_num);
			check_result(result, errMsg);

			switch (my_func_block.type) {

			case SDM35541_FUNC_BLOCK_ADC1002:
				printf
				    ("  FB%d: ADC1002:%d, with %d DMA Channels (%d buffers each)\n",
				     my_func_block.fb_num,
				     my_func_block.sub_type,
				     my_func_block.num_dma_channels,
				     my_func_block.num_dma_buffers);
				break;
			case SDM35541_FUNC_BLOCK_FLASH_PROGRAMMER:
				printf
				    ("  FB%d: Flash Programmer Module, with %d DMA Channels (%d buffers each)\n",
				     my_func_block.fb_num,
				     my_func_block.num_dma_channels,
				     my_func_block.num_dma_buffers);
				break;
			case SDM35541_FUNC_BLOCK_INVALID:
				// Do nothing
				break;
			default:
				printf("  FB%d: **Unknown module type (0x%x)\n",
				       my_func_block.fb_num,
				       my_func_block.type);
				break;
			}

			fb_num++;
		}

	}

	printf("\nClosing Board\n");
	result = SDM35541_Board_Close(board);
	check_result(result, "Error closing board.");
	
	printf("Example program successfully completed.\n");
    system("pause");
	return 0;
}

/**
 * Function to handle freeing various resources used by the program.
 */
void CleanUp()
{   
    int result = SDM35541_ERROR_NO_ERROR;
    
    if(board != NULL)
    {        
        // close the board
		printf("Closing Board\n");
		result = SDM35541_Board_Close(board);
		check_result(result, "Error closing board.");
	}
}


/**
 * Checks for a non-zero return code, prints the error message given and calls the CleanUp function
 */ 
void Fail(int result, char* message)
{
    if(result){
        printf("Error (%d):%s\n", result, message);
        CleanUp();
		system("pause");
        exit(-1);
    }
}

