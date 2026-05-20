/**
    @file

    @brief
        Example program which demonstrates use of the SDM35541's
        on board temperature sensor.

    @verbatim

        This example program uses the board library to query the on board
        temperature sensor over the I2C bus.

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

    $Id: Temperature.cpp 152193 2026-02-13 21:38:28Z bkorpacz $
*/
#include <stdio.h>

#include "SDM35541_adc1002_library.h"
#include "SDM35541_board_access.h"
#include "SDM35541_util_library.h"
#include "sdm35541_registers.h"
#include "SDM35541.h"


/**
 * Main program code.
 */
int main(int argc, char** argv) {
    struct SDM35541_Board_Descriptor* board;
    struct SDM35541_Function_Block my_func_block;
    int help_option_given = 0;
    int minor_number = 0;
    int result;
    uint8_t temp;

    printf("Enter board number to use:");

    if (scanf_s("%i", &minor_number) == 0) {
        check_result(-1, "Error getting board number");
    }

    /* Ensure value given is valid */
    check_result(minor_number < 0 || minor_number >= SDM35541_MAX_BOARDS, "Board number must be between 0-7");

    /* Open the SDM35541 device. */
    printf("Opening SDM35541...");
    result = SDM35541_Board_Open(minor_number, &board);
    check_result(result, "Unable to open SDM35541.");

    /* Open the SDM35541 function block */
    printf("Opening SDM35541 function block 0\n");
    result = SDM35541_Function_Block_Open(board,
        SDM35541_FUNCTION_BLOCK_ADC1002, &my_func_block);
    check_result(result, "Error opening the ADC1002 function block.");

    /* Get the Temp */
    printf("Get the Temperature from the board...\n");
    result  = SDM35541_Get_Temp(board, &my_func_block, &temp);
    check_result(result, "Error getting board temperature.");

    printf("\nTemp: %d C\n\n", (int8_t)temp);

    /* Reset the sdm35541 device. */
    printf("Resetting the sdm35541...\n");
    result = SDM35541_Gbc_Board_Reset(board);
    check_result(result, "Error resetting the board.");

    /* Close the sdm35541 device. */
    printf("Closing the SDM35541...\n");
    result = SDM35541_Board_Close(board);
    check_result(result, "Error closing the board.");

    system("pause");

    return 0;
}